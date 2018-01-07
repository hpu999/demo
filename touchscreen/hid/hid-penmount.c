 /*
 *  HID driver for PenMount touchscreens
 *
 *  Copyright (c) 2014 Christian Gmeiner <christian.gmeiner <at> gmail.com>
 *  Copyright (c) 2016 John Sung <penmount <at> seed.net.tw>
 *
 *  based on hid-penmount copyrighted by
 *    PenMount Touch Solutions <penmount <at> seed.net.tw>
 */

/*
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the Free
 * Software Foundation; either version 2 of the License, or (at your option)
 * any later version.
 */

#include <linux/module.h>
#include <linux/hid.h>
#include <linux/version.h>
#if LINUX_VERSION_CODE >= KERNEL_VERSION(2, 6, 38)
#include <linux/input/mt.h>
#endif

#if LINUX_VERSION_CODE >= KERNEL_VERSION(2, 6, 38)
#define PM_MAX_CONTACT		10
#else
#define PM_MAX_CONTACT		1
#endif
#define PM_DEF_CONTACT_P2_WIN8	10
#define PM_DEF_CONTACT_PCI	5
#define PM_DEF_CONTACT_6000	1
#define PM_HID_REPORT_SIZE	5
#define PM_HID_RAW_SIZE         128
#define PM_HID_UPDATE_SIZE      512
#define PM_HID_REPORT_ID	0x00

#ifndef USB_VENDOR_ID_PENMOUNT
#define USB_VENDOR_ID_PENMOUNT		0x14E1
#endif

#ifndef USB_DEVICE_ID_PENMOUNT_PCI
#define USB_DEVICE_ID_PENMOUNT_PCI	0x3500
#endif

#ifndef USB_DEVICE_ID_PENMOUNT_6000
#define USB_DEVICE_ID_PENMOUNT_6000	0x6000
#endif

#ifndef USB_DEVICE_ID_PENMOUNT_P2_WIN8
#define USB_DEVICE_ID_PENMOUNT_P2_WIN8	0x3508
#endif

#ifndef ABS_MT_POSITION_X
#define ABS_MT_POSITION_X       0x35    /* Center X ellipse position */
#endif

#ifndef ABS_MT_POSITION_Y
#define ABS_MT_POSITION_Y       0x36    /* Center Y ellipse position */
#endif

#ifndef HID_DG_CONFIDENCE
#define HID_DG_CONFIDENCE       0x47
#endif

#ifndef HID_DG_CONTACTID
#define HID_DG_CONTACTID        0x51
#endif

#ifndef HID_DG_CONTACTCOUNT
#define HID_DG_CONTACTCOUNT     0x54
#endif

#ifndef HID_DG_SCANTIME
#define HID_DG_SCANTIME         0x56
#endif

//------------------------------------------------------
// Version Information
//------------------------------------------------------
#ifndef PMDRIVER_MAJORVER
#define PMDRIVER_MAJORVER 1
#define PMDRIVER_MINORVER 15
#define PMDRIVER_BUILDVER 0
#define PMDRIVER_VERSION  ((PMDRIVER_MAJORVER<<8)|(PMDRIVER_MINORVER<<4)|PMDRIVER_BUILDVER)
#endif
//------------------------------------------------------

struct mt_slot {
	unsigned char id;
	unsigned short x, y;
	unsigned char active; /* is the touch valid? */
	unsigned char updated;
	unsigned char valid;
};

struct penmount {
	char version[32];
	unsigned short model;
	struct hid_device *hid;
	struct input_dev *input;
	unsigned char maxcontacts;
	unsigned char report_id;
	unsigned char read_size;
	struct mt_slot slots[PM_MAX_CONTACT];
	struct mt_slot curdata;
	unsigned char ack[PM_HID_REPORT_SIZE];
};

int g_DebugLevel = 0;
module_param_named(debug, g_DebugLevel, int, S_IWUSR | S_IWGRP | S_IRUSR | S_IRGRP | S_IROTH);

static int penmount_hid_setreport(struct penmount *pm, int report_type, unsigned char *cmd, int length)
{
	int ret = 0;
	int i = 0;
	unsigned char * report = NULL;

	report = kmalloc(length, GFP_KERNEL | __GFP_ZERO);
	if (report == NULL) {
		return -ENOMEM;
	}

	report[0] = pm->report_id;
	for (i = 0; i < length; i++)
		report[i+1] = cmd[i];


#if LINUX_VERSION_CODE >= KERNEL_VERSION(3, 15, 0)
	ret = hid_hw_raw_request(pm->hid, pm->report_id, report,
		length+1, report_type, HID_REQ_SET_REPORT);
#elif LINUX_VERSION_CODE >= KERNEL_VERSION(2, 6, 34)
	ret = pm->hid->hid_output_raw_report(pm->hid, report,
		length+1, report_type);
#endif

#if LINUX_VERSION_CODE >= KERNEL_VERSION(2, 6, 38)
	if (ret < 0)
		hid_err(pm->hid, "Failed to set %s report !\n", (report_type == HID_FEATURE_REPORT)?"feature":"output");
#endif

	kfree (report);

	return ret;
}

static int penmount_hid_getreport(struct penmount *pm, int report_type, unsigned char *ack, int length)
{
	int ret = -EINVAL;
	int i = 0;
	unsigned char * report = NULL;

	report = kmalloc(length, GFP_KERNEL | __GFP_ZERO);
	if (report == NULL) {
		return -ENOMEM;
	}

	report[0] = pm->report_id;
#if LINUX_VERSION_CODE >= KERNEL_VERSION(3, 15, 0)
	ret = hid_hw_raw_request(pm->hid, pm->report_id, report,
		length+1, report_type, HID_REQ_GET_REPORT);
#elif LINUX_VERSION_CODE >= KERNEL_VERSION(3, 0, 0)
	ret = pm->hid->hid_get_raw_report(pm->hid, pm->report_id, report,
		length+1, report_type);
#endif
	if (ret < 0) {
#if LINUX_VERSION_CODE >= KERNEL_VERSION(2, 6, 38)
		hid_err(pm->hid, "Failed to get %s report !\n", (report_type == HID_FEATURE_REPORT)?"feature":"input");
#endif
		kfree (report);
		return ret;
	}

	for (i = 0; i < length; i++)
		ack[i] = report[i+1];

	kfree (report);
	return ret;
}

static ssize_t penmount_cmd_store(struct device *dev,
		struct device_attribute *attr, const char *buffer, size_t count)
{
	struct penmount *pm = NULL;
	struct hid_device *hdev = NULL;
	unsigned char cmd[PM_HID_REPORT_SIZE] = { 0, 0, 0, 0, 0 };

	hdev = dev_get_drvdata(dev);
	if (hdev == NULL)
		return -EINVAL;

	pm = hid_get_drvdata(hdev);
	if ((pm == NULL) || (buffer == NULL))
		return -EINVAL;

	sscanf(buffer, "%hhX %hhX %hhX %hhX %hhX", &cmd[0], &cmd[1],
		&cmd[2], &cmd[3], &cmd[4]);

	if (penmount_hid_setreport(pm, HID_FEATURE_REPORT, cmd, PM_HID_REPORT_SIZE) < 0)
		return 0;

	return count;
}

static ssize_t penmount_cmd_show(struct device *dev,
		struct device_attribute *attr, char *buffer)
{
	struct penmount *pm = NULL;
	struct hid_device *hdev = NULL;
	size_t count = 0;
	unsigned char ack[PM_HID_REPORT_SIZE] = { 0, 0, 0, 0, 0 };

	hdev = dev_get_drvdata(dev);
	if (hdev == NULL)
		return -EINVAL;

	pm = hid_get_drvdata(hdev);
	if ((pm == NULL) || (buffer == NULL))
		return -EINVAL;

	if (penmount_hid_getreport(pm, HID_FEATURE_REPORT, ack, PM_HID_REPORT_SIZE) < 0)
		return 0;

	count = sprintf(buffer, "0x%02X 0x%02X 0x%02X 0x%02X 0x%02X\n", ack[0],
			ack[1], ack[2], ack[3], ack[4]);

	return count;
}

static ssize_t penmount_ver_show(struct device *dev,
		struct device_attribute *attr, char *buffer)
{
	struct penmount *pm = NULL;
	struct hid_device *hdev = NULL;
	size_t count = 0;

	hdev = dev_get_drvdata(dev);
	if (hdev == NULL)
		return -EINVAL;

	pm = hid_get_drvdata(hdev);
	if ((pm == NULL) || (buffer == NULL))
		return -EINVAL;

	count = sprintf(buffer, "%s\n", pm->version);

	return count;
}

static ssize_t penmount_enable_store(struct device *dev,
		struct device_attribute *attr, const char *buffer, size_t count) {
	struct penmount *pm = NULL;
	struct hid_device *hdev = NULL;
	unsigned char cmd[PM_HID_REPORT_SIZE] = { 0xF1, 0, 0, 0, 1 };
	unsigned char enable_state = 1;

	hdev = dev_get_drvdata(dev);
	if (hdev == NULL)
		return -EINVAL;

	pm = hid_get_drvdata(hdev);
	if ((pm == NULL) || (buffer == NULL))
		return -EINVAL;

	sscanf(buffer, "%hhX", &enable_state);

	if (enable_state) {
		cmd[0] = 0xF1;
	} else {
		cmd[0] = 0xF0;
	}

	if (penmount_hid_setreport(pm, HID_FEATURE_REPORT, cmd, PM_HID_REPORT_SIZE) < 0)
		return 0;

	return count;
}

static ssize_t penmount_enable_show(struct device *dev,
		struct device_attribute *attr, char *buffer) {
	struct penmount *pm = NULL;
	struct hid_device *hdev = NULL;
	unsigned char enable_state = 1;
	size_t count = 0;
	unsigned char cmd[PM_HID_REPORT_SIZE] = { 0xE0, 0, 0xF0, 0, 0 };
	unsigned char ack[PM_HID_REPORT_SIZE] = { 0, 0, 0, 0, 0 };

	hdev = dev_get_drvdata(dev);
	if (hdev == NULL) {
		return -EINVAL;
	}

	pm = hid_get_drvdata(hdev);
	if ((pm == NULL) || (buffer == NULL))
		return -EINVAL;

	if (pm->model == USB_DEVICE_ID_PENMOUNT_6000)
		return 0;

	if (penmount_hid_setreport(pm, HID_FEATURE_REPORT, cmd, PM_HID_REPORT_SIZE) < 0)
		return 0;

	if (penmount_hid_getreport(pm, HID_FEATURE_REPORT, ack, PM_HID_REPORT_SIZE) < 0)
		return 0;

	if ((ack[4] & 0x03) == 0x03) {
		enable_state = 1;
	} else {
		enable_state = 0;
	}

	count = sprintf(buffer, "%d\n", enable_state);

	return count;
}

static ssize_t penmount_fw_store(struct device *dev,
		struct device_attribute *attr, const char *buffer, size_t count) {
	struct penmount *pm = NULL;
	struct hid_device *hdev = NULL;
	int length = 0;
	char * str = (char *) buffer;
	char * pch = NULL;
	unsigned char fw[PM_HID_UPDATE_SIZE];
	int i = 0;

	hdev = dev_get_drvdata(dev);
	if (hdev == NULL)
		return -EINVAL;

	pm = hid_get_drvdata(hdev);
	if ((pm == NULL) || (buffer == NULL))
		return -EINVAL;

	pch = strsep (&str," ");
	while (pch != NULL) {
		sscanf(pch, "%hhX", fw+length);
		length++;
		if (length >= PM_HID_UPDATE_SIZE) {
#if LINUX_VERSION_CODE >= KERNEL_VERSION(2, 6, 38)
			hid_info(pm->hid, "Ignoring update data > (%d)\n", length);
#endif			
			break;
		}
		pch = strsep (&str, " ");
	}

#if LINUX_VERSION_CODE >= KERNEL_VERSION(2, 6, 38)
	if (g_DebugLevel) {
		hid_info(pm->hid, "Update data (%d) : ", length);
		for (i=0; i<length; i++) {
			printk ("[0x%02X]", fw[i]);
		}
		printk ("\n");
	}
#endif

	penmount_hid_setreport(pm, HID_OUTPUT_REPORT, fw, length);

	return count;
}

static ssize_t penmount_raw_show(struct device *dev,
		struct device_attribute *attr, char *buffer) {
	struct penmount *pm = NULL;
	struct hid_device *hdev = NULL;
	size_t count = 0;
	int ret = 0;
	int i = 0;
	unsigned char * raw = NULL;
	char str[5];

	hdev = dev_get_drvdata(dev);
	if (hdev == NULL) {
		return -EINVAL;
	}

	pm = hid_get_drvdata(hdev);
	if ((pm == NULL) || (buffer == NULL))
		return -EINVAL;

	raw = kmalloc(pm->read_size, GFP_KERNEL | __GFP_ZERO);
	if (raw == NULL) {
		return -ENOMEM;
	}

	ret = penmount_hid_getreport(pm, HID_FEATURE_REPORT, raw, pm->read_size);
	for (i=0; i<pm->read_size; i++) {
		sprintf(str, "0x%02X ", raw[i]);	
		strcat (buffer, str);
	}
	kfree (raw);
	
#if LINUX_VERSION_CODE >= KERNEL_VERSION(2, 6, 38)
	if (g_DebugLevel) {
		hid_info(pm->hid, "Raw data (%d) : %s\n", pm->read_size, buffer);
	}
#endif
	count = strlen (buffer) + 1;
	return count;
}

static DEVICE_ATTR(raw,    0444, penmount_raw_show,    NULL);
static DEVICE_ATTR(fw,     0222, NULL,                 penmount_fw_store);
static DEVICE_ATTR(ver,    0444, penmount_ver_show,    NULL);
static DEVICE_ATTR(cmd,    0666, penmount_cmd_show,    penmount_cmd_store);
static DEVICE_ATTR(enable, 0666, penmount_enable_show, penmount_enable_store);

static struct attribute *penmount_attrs[] = {
	&dev_attr_raw.attr,
	&dev_attr_fw.attr,
	&dev_attr_cmd.attr,
	&dev_attr_ver.attr,
	&dev_attr_enable.attr,
	NULL
};

static const struct attribute_group penmount_attr_group = {
	.attrs = penmount_attrs,
};

static int penmount_get_version(struct penmount *pm)
{
	int ret = 0;
	unsigned short product_version = 0;
	unsigned char major_version = 0;
	unsigned char minor_version = 0;
	unsigned char build_version = 0;
	unsigned char odm_version = 0;
	unsigned char cmd[PM_HID_REPORT_SIZE] = { 0xEE, 0, 0, 0, 0 };
	unsigned char ack[PM_HID_REPORT_SIZE] = { 0, 0, 0, 0, 0 };

	ret = penmount_hid_setreport(pm, HID_FEATURE_REPORT, cmd, PM_HID_REPORT_SIZE);
	if (ret < 0) {
#if LINUX_VERSION_CODE >= KERNEL_VERSION(2, 6, 38)
		hid_warn(pm->hid, "Failed to get firmware version !");
#endif
		return ret;
	}

	ret = penmount_hid_getreport(pm, HID_FEATURE_REPORT, ack, PM_HID_REPORT_SIZE);
	if (ret < 0) {
#if LINUX_VERSION_CODE >= KERNEL_VERSION(2, 6, 38)
		hid_warn(pm->hid, "Failed to get firmware version !");
#endif
		return ret;
	}

	switch (pm->model) {
	case USB_DEVICE_ID_PENMOUNT_P2_WIN8:
	case USB_DEVICE_ID_PENMOUNT_PCI:
		product_version = (ack[2] * 256 + ack[1]) & 0x7FFF;
		major_version = ack[3];
		break;
	case USB_DEVICE_ID_PENMOUNT_6000:
		product_version = ack[1] * 256 + ack[2];
		major_version = ack[4];
		break;
	}

	cmd[0] = 0xED;
	ret = penmount_hid_setreport(pm, HID_FEATURE_REPORT, cmd, PM_HID_REPORT_SIZE);
	if (ret < 0) {
#if LINUX_VERSION_CODE >= KERNEL_VERSION(2, 6, 38)
		hid_warn(pm->hid, "Failed to get firmware version !");
#endif
		return ret;
	}

	ret = penmount_hid_getreport(pm, HID_FEATURE_REPORT, ack, PM_HID_REPORT_SIZE);
	if (ret < 0) {
#if LINUX_VERSION_CODE >= KERNEL_VERSION(2, 6, 38)
		hid_warn(pm->hid, "Failed to get firmware version !");
#endif
		return ret;
	}

	switch (pm->model) {
	case USB_DEVICE_ID_PENMOUNT_P2_WIN8:
	case USB_DEVICE_ID_PENMOUNT_PCI:
		minor_version = ack[1];
		odm_version = ack[2];
		build_version = ack[3];
		break;
	case USB_DEVICE_ID_PENMOUNT_6000:
		minor_version = ack[2];
		build_version = ack[4];
		break;
	}

	if (!odm_version) {
		sprintf(pm->version, "%d.%d.%d.%d", product_version,
			major_version, minor_version, build_version);
	} else {
		sprintf(pm->version, "%d.D%02X.%d.%d.%d", product_version,
			odm_version, major_version, minor_version,
			build_version);
	}
#if LINUX_VERSION_CODE >= KERNEL_VERSION(2, 6, 38)
	hid_info(pm->hid, "Firmware version %s\n", pm->version);
#endif
	return ret;
}


static int penmount_check_burst_read(struct penmount *pm)
{
	int ret = 0;
	unsigned char cmd[PM_HID_REPORT_SIZE] = { 0xFA, 0, 0, 0, 0 };
	unsigned char ack[PM_HID_REPORT_SIZE] = { 0, 0, 0, 0, 0 };
	unsigned short report_size = 0;

	if (pm->model == USB_DEVICE_ID_PENMOUNT_6000)
		return -1;

	ret = penmount_hid_setreport(pm, HID_FEATURE_REPORT, cmd, PM_HID_REPORT_SIZE);
	if (ret < 0) {
#if LINUX_VERSION_CODE >= KERNEL_VERSION(2, 6, 38)
		hid_warn(pm->hid, "Failed to get firmware version !");
#endif
		return ret;
	}

	ret = penmount_hid_getreport(pm, HID_FEATURE_REPORT, ack, PM_HID_REPORT_SIZE);
	if (ret < 0) {
#if LINUX_VERSION_CODE >= KERNEL_VERSION(2, 6, 38)
		hid_warn(pm->hid, "Failed to get firmware version !");
#endif
		return ret;
	}

	if (ack[0] == 0xAA) {
		return 0;
	}

	report_size = (ack[2] * 256 + ack[1]) & 0x7FFF;
	if (report_size) {
		pm->read_size = report_size;
#if LINUX_VERSION_CODE >= KERNEL_VERSION(2, 6, 38)
		hid_info(pm->hid, "Device supports burst read mode : %d bytes\n", pm->read_size);
#endif
	}

	return ret;
}

static void penmount_send_event(struct penmount *pm)
{
#if LINUX_VERSION_CODE >= KERNEL_VERSION(2, 6, 38)
	int i;
	for (i = 0; i < pm->maxcontacts; ++i) {
		input_mt_slot(pm->input, i);
		input_mt_report_slot_state(pm->input, MT_TOOL_FINGER,
				pm->slots[i].active);
		if (pm->slots[i].active) {
			input_event(pm->input, EV_ABS, ABS_MT_POSITION_X,
				pm->slots[i].x);
			input_event(pm->input, EV_ABS, ABS_MT_POSITION_Y,
				pm->slots[i].y);
		}
	}

	input_mt_report_pointer_emulation(pm->input, true);
#else
	input_event(pm->input, EV_KEY, BTN_TOUCH, pm->slots[0].active);
	input_event(pm->input, EV_ABS, ABS_X, pm->slots[0].x);
	input_event(pm->input, EV_ABS, ABS_Y, pm->slots[0].y);
#endif
	input_sync(pm->input);
}

static void penmount_process_event(struct penmount *pm)
{
	unsigned char i = 0;

	if ((pm->model == USB_DEVICE_ID_PENMOUNT_P2_WIN8)
		&& (!pm->curdata.valid)) {
		return;
	}
	if (pm->curdata.id >= pm->maxcontacts)
		return;

	pm->slots[pm->curdata.id].active = pm->curdata.active;
	pm->slots[pm->curdata.id].x = pm->curdata.x;
	pm->slots[pm->curdata.id].y = pm->curdata.y;

	if ((!pm->slots[pm->curdata.id].updated) && (pm->curdata.active)) {
		pm->slots[pm->curdata.id].updated = 1;
		return;
	}

	penmount_send_event(pm);

	for (i = 0; i < PM_MAX_CONTACT; i++)
		pm->slots[pm->curdata.id].updated = 0;
}

static int penmount_event(struct hid_device *hdev, struct hid_field *field,
		struct hid_usage *usage, __s32 value)
{
	struct penmount *pm = (struct penmount *) hid_get_drvdata(hdev);

	if (pm == NULL)
		return 0;

	if (pm->model == USB_DEVICE_ID_PENMOUNT_6000) {
		/* Fallback to the generic hidinput handling */
		return 0;
	}

	if (hdev->claimed & HID_CLAIMED_INPUT) {
		switch (usage->hid) {
		case HID_DG_CONTACTID:
			pm->curdata.id = value;
			break;
		case HID_DG_TIPSWITCH:
			pm->curdata.active = value;
			break;
		case HID_DG_CONFIDENCE:
			pm->curdata.valid = !!value;
			break;
		case HID_GD_X:
			pm->curdata.x = value;
			break;
		case HID_GD_Y:
			pm->curdata.y = value;
			penmount_process_event(pm);
			break;
		default:
		case HID_DG_CONTACTCOUNT:
		case HID_DG_SCANTIME:
			/* Fallback to the generic hidinput handling */
			return 0;
		}
	}

	if ((hdev->claimed & HID_CLAIMED_HIDDEV) && (hdev->hiddev_hid_event))
		hdev->hiddev_hid_event(hdev, field, usage, value);

	return 1;
}

static int penmount_input_mapping(struct hid_device *hdev,
		struct hid_input *hi, struct hid_field *field,
		struct hid_usage *usage, unsigned long **bit, int *max)
{
	struct penmount *pm = (struct penmount *) hid_get_drvdata(hdev);

	switch (usage->hid) {
	case HID_GD_X:
		if (pm->maxcontacts > 1) {
			hid_map_usage(hi, usage, bit, max, EV_ABS,
				ABS_MT_POSITION_X);
			input_set_abs_params(hi->input, ABS_MT_POSITION_X,
				field->logical_minimum, field->logical_maximum,
				0, 0);
		}
		hid_map_usage(hi, usage, bit, max, EV_ABS, ABS_X);
		input_set_abs_params(hi->input, ABS_X, field->logical_minimum,
				field->logical_maximum, 0, 0);
		return 1;
	case HID_GD_Y:
		if (pm->maxcontacts > 1) {
			hid_map_usage(hi, usage, bit, max, EV_ABS,
				ABS_MT_POSITION_Y);
			input_set_abs_params(hi->input, ABS_MT_POSITION_Y,
				field->logical_minimum, field->logical_maximum,
				0, 0);
		}
		hid_map_usage(hi, usage, bit, max, EV_ABS, ABS_Y);
		input_set_abs_params(hi->input, ABS_Y, field->logical_minimum,
				field->logical_maximum, 0, 0);
		return 1;
	case HID_UP_BUTTON | 0x0001:
	case HID_DG_TIPSWITCH:
		hid_map_usage(hi, usage, bit, max, EV_KEY, BTN_TOUCH);
		input_set_capability(hi->input, EV_KEY, BTN_TOUCH);
		return 1;
	case HID_DG_CONTACTID:
#if LINUX_VERSION_CODE >= KERNEL_VERSION(3, 7, 0)
		input_mt_init_slots(hi->input, pm->maxcontacts,
			INPUT_MT_DIRECT);
#elif LINUX_VERSION_CODE >= KERNEL_VERSION(2, 6, 38)
		input_mt_init_slots(hi->input, pm->maxcontacts);
#endif
		return 1;
	default:
	case HID_UP_BUTTON | 0x0002:
		/* Ignore PenMount 6000 button 2, its value is always 0. */
		return -1;
	}

	return 0;
}

static int penmount_input_mapped(struct hid_device *hdev, struct hid_input *hi,
                 struct hid_field *field, struct hid_usage *usage,
                 unsigned long **bit, int *max)
{
       if (usage->type == EV_KEY || usage->type == EV_ABS)
                set_bit(usage->type, hi->input->evbit);

        return -1;
}

static void penmount_feature_mapping(struct hid_device *hdev,
		struct hid_field *field, struct hid_usage *usage)
{
	struct penmount *pm = (struct penmount *) hid_get_drvdata(hdev);

	if (pm == NULL)
		return;

	switch (usage->hid) {
	case HID_DG_CONTACTMAX:
		pm->maxcontacts = field->value[0];
		/* field->value[0] value can be 0, in this case, use the
			maximum value. */
		if (!pm->maxcontacts)
			pm->maxcontacts = field->logical_maximum;
		break;
	}
}

static int penmount_probe(struct hid_device *hdev,
		const struct hid_device_id *id)
{
	struct penmount *pm = NULL;
	struct hid_input *hidinput = NULL;
	int ret = 0;

	pm = kmalloc(sizeof(struct penmount), GFP_KERNEL | __GFP_ZERO);
	if (pm == NULL)
		return -ENOMEM;

	hid_set_drvdata(hdev, pm);
	pm->hid = hdev;
	pm->model = id->product;
	pm->report_id = PM_HID_REPORT_ID;
	switch (id->product) {
	case USB_DEVICE_ID_PENMOUNT_P2_WIN8:
		pm->maxcontacts = PM_DEF_CONTACT_P2_WIN8;
		pm->report_id = 0x01;
		break;
	case USB_DEVICE_ID_PENMOUNT_PCI:
		pm->maxcontacts = PM_DEF_CONTACT_PCI;
		break;
	default:
	case USB_DEVICE_ID_PENMOUNT_6000:
		pm->maxcontacts = PM_DEF_CONTACT_6000;
		break;
	}

#if LINUX_VERSION_CODE < KERNEL_VERSION(2, 6, 38)
	pm->maxcontacts = 1;
	printk ("[PENMOUNT] System supports single touch only !\n");
#endif

	ret = hid_parse(hdev);
	if (ret) {
#if LINUX_VERSION_CODE >= KERNEL_VERSION(2, 6, 38)
		hid_err(hdev, "Failed to parse HID report !(%d)\n", ret);
#endif
		kfree(pm);
		return ret;
	}

	ret = hid_hw_start(hdev, HID_CONNECT_DEFAULT);
	if (ret) {
#if LINUX_VERSION_CODE >= KERNEL_VERSION(2, 6, 38)
		hid_err(hdev, "Failed to start device !(%d)\n", ret);
#endif
		kfree(pm);
		return ret;
	}

	hidinput = list_entry(hdev->inputs.next, struct hid_input, list);
	if (hidinput != NULL) {
		hidinput->input->id.version = PMDRIVER_VERSION;
		pm->input = hidinput->input;
#if LINUX_VERSION_CODE >= KERNEL_VERSION(2, 6, 38)
		// TODO: sysfs_create_group() may trigger BUG in 2.6.32
		set_bit(INPUT_PROP_DIRECT, hidinput->input->propbit);
		if (sysfs_create_group(&hidinput->input->dev.kobj,
				&penmount_attr_group)) {
			hid_warn(hdev, "Failed to create attr group !\n");
		}
#endif
	}

	penmount_get_version(pm);
	penmount_check_burst_read(pm);
#if LINUX_VERSION_CODE >= KERNEL_VERSION(2, 6, 38)
	hid_info(hdev, "Device supports %d touch contacts !\n", pm->maxcontacts);
#endif
	return ret;
}

static void penmount_remove(struct hid_device *hdev)
{
	struct penmount *pm = NULL;

	pm = hid_get_drvdata(hdev);
	if (pm != NULL) {
		sysfs_remove_group(&pm->input->dev.kobj, &penmount_attr_group);
		kfree(pm);
	}
	hid_hw_stop(hdev);
	hid_set_drvdata(hdev, NULL);
}

static const struct hid_device_id penmount_devices[] = {
	{ HID_USB_DEVICE(USB_VENDOR_ID_PENMOUNT, USB_DEVICE_ID_PENMOUNT_6000) },
	{ HID_USB_DEVICE(USB_VENDOR_ID_PENMOUNT, USB_DEVICE_ID_PENMOUNT_PCI) },
	{ HID_USB_DEVICE(USB_VENDOR_ID_PENMOUNT, USB_DEVICE_ID_PENMOUNT_P2_WIN8) },
	{ }
};
MODULE_DEVICE_TABLE(hid, penmount_devices);

static struct hid_driver penmount_driver = {
	.name = "hid-penmount",
	.id_table = penmount_devices,
	.input_mapping = penmount_input_mapping,
	.input_mapped = penmount_input_mapped,
#if LINUX_VERSION_CODE >= KERNEL_VERSION(2, 6, 38)
	.feature_mapping = penmount_feature_mapping,
#endif
	.probe = penmount_probe,
	.remove = penmount_remove,
	.event = penmount_event,
};

#if LINUX_VERSION_CODE >= KERNEL_VERSION(3, 9, 0)
module_hid_driver(penmount_driver);
#else
static int __init penmount_init(void)
{
	printk ("[PENMOUNT] Loading %s V%d.%d.%d\n", penmount_driver.name, PMDRIVER_MAJORVER, PMDRIVER_MINORVER, PMDRIVER_BUILDVER);
	return hid_register_driver(&penmount_driver);
}

static void __exit penmount_exit(void)
{
	hid_unregister_driver(&penmount_driver);
}

module_init(penmount_init);
module_exit(penmount_exit);
#endif

MODULE_AUTHOR("Christian Gmeiner <christian.gmeiner@gmail.com>");
MODULE_AUTHOR("John Sung <penmount.touch@gmail.com>");
MODULE_DESCRIPTION("PenMount HID TouchScreen Driver");
MODULE_LICENSE("GPL");
