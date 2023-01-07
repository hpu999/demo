#include <stdio.h>
#include <stdlib.h>

//error相关头文件
#include <string.h>
#include <errno.h>

//access所需头文件
 #include <unistd.h>

//open()相关头文件
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <stdint.h>

#define LEDS_MULTI_CTRL_FILE    "/dev/gpiochip5"
/* Maximum number of requested handles */
#define GPIOHANDLES_MAX 64

/* Linerequest flags */
#define GPIOHANDLE_REQUEST_INPUT    (1UL << 0)
#define GPIOHANDLE_REQUEST_OUTPUT    (1UL << 1)
#define GPIOHANDLE_REQUEST_ACTIVE_LOW    (1UL << 2)
#define GPIOHANDLE_REQUEST_OPEN_DRAIN    (1UL << 3)
#define GPIOHANDLE_REQUEST_OPEN_SOURCE    (1UL << 4)

int leds_multi_init(void);

struct gpiochip_info {
    char name[32];
    char label[32];
    unsigned int lines;
};

struct gpioline_info {
    unsigned int line_offset;
    unsigned int flags;
    char name[32];
    char consumer[32];
};

struct gpiohandle_request {
    unsigned int lineoffsets[GPIOHANDLES_MAX];
    unsigned int flags;
    unsigned char default_values[GPIOHANDLES_MAX];
    char consumer_label[32];
    unsigned int lines;
    int fd;
};

/**
 * struct gpiohandle_data - Information of values on a GPIO handle
 * @values: when getting the state of lines this contains the current
 * state of a line, when setting the state of lines these should contain
 * the desired target state
 */
struct gpiohandle_data {
    unsigned char values[GPIOHANDLES_MAX];
};

#define GPIO_GET_CHIPINFO_IOCTL  2151986177    //_IOR(0xB4, 0x01, struct gpiochip_info)
#define GPIO_GET_LINEINFO_IOCTL  3225990146    //_IOWR(0xB4, 0x02, struct gpioline_info)
#define GPIO_GET_LINEHANDLE_IOCTL  3245126659  //_IOWR(0xB4, 0x03, struct gpiohandle_request)

#define GPIOHANDLE_GET_LINE_VALUES_IOCTL  3225465864  //_IOWR(0xB4, 0x08, struct gpiohandle_data)
#define GPIOHANDLE_SET_LINE_VALUES_IOCTL  3225465865  //_IOWR(0xB4, 0x09, struct gpiohandle_data)

struct gpiochip_info gpioinfo;
struct gpioline_info gpioline;
struct gpiohandle_request gpiohandle;
static int multi_ctrl_fd = -1;
struct gpiohandle_data gpiohandledata;

int gpio_leds_init(void);
int gpio_leds_state_get(void);
int gpio_leds_state_set(unsigned char value);

int main(int argc, char** argv)
{
    //gpio_leds_init();
    /*
    printf("cxw GPIO_GET_CHIPINFO_IOCTL =%ld\n", GPIO_GET_CHIPINFO_IOCTL);
    printf("cxw GPIO_GET_LINEINFO_IOCTL =%ld\n", GPIO_GET_LINEINFO_IOCTL);
    printf("cxw GPIO_GET_LINEHANDLE_IOCTL =%ld\n", GPIO_GET_LINEHANDLE_IOCTL);
    printf("cxw GPIOHANDLE_GET_LINE_VALUES_IOCTL =%ld\n", GPIOHANDLE_GET_LINE_VALUES_IOCTL);
    printf("cxw GPIOHANDLE_SET_LINE_VALUES_IOCTL =%ld\n", GPIOHANDLE_SET_LINE_VALUES_IOCTL);
    */

    gpio_leds_init();
    printf("gpio_leds_state_get gpiohandledata.values[0] = %d\n", gpio_leds_state_get());
    gpio_leds_state_set(1);
    sleep(5);
    gpio_leds_state_set(0);

    return 0;
}

int gpio_leds_init(void)
{
    int ret = -1;

    multi_ctrl_fd = open(LEDS_MULTI_CTRL_FILE, O_RDONLY);
    if (multi_ctrl_fd < 0) {
        fprintf(stderr,"%s,can't open file %s\n",__func__, LEDS_MULTI_CTRL_FILE);
        return -1;
    }

    if ((ret = ioctl(multi_ctrl_fd, GPIO_GET_CHIPINFO_IOCTL, &gpioinfo)) < 0){
        printf("LEDS_MULTI_CTRL_IOCTL_MULTI_SET failed\n");
        return -errno;
    }
    printf("cxw gpioinfo.name = %s, gpioinfo.label = %s, gpioinfo.lines = %d\n", gpioinfo.name, gpioinfo.label, gpioinfo.lines);

    gpioline.line_offset = 11;
    if ((ret = ioctl(multi_ctrl_fd, GPIO_GET_LINEINFO_IOCTL, &gpioline)) < 0){
        printf("LEDS_MULTI_CTRL_IOCTL_MULTI_SET failed\n");
        return -errno;
    }
    printf("cxw gpioline.line_offset = %d, gpioline.flags = %d, gpioline.name = %s, gpioline.consumer = %s\n", gpioline.line_offset, gpioline.flags, gpioline.name, gpioline.consumer);

    gpiohandle.lineoffsets[0] = 11;
    gpiohandle.lines = 1;
    gpiohandle.flags = GPIOHANDLE_REQUEST_OUTPUT;
    gpiohandle.default_values[0] = 0;
    if ((ret = ioctl(multi_ctrl_fd, GPIO_GET_LINEHANDLE_IOCTL, &gpiohandle)) < 0){
        printf("LEDS_MULTI_CTRL_IOCTL_MULTI_SET failed\n");
        return -errno;
    }

    return 0;
}

int gpio_leds_state_get(void)
{
    int ret = -1;

    if ((ret = ioctl(gpiohandle.fd, GPIOHANDLE_GET_LINE_VALUES_IOCTL, &gpiohandledata)) < 0){
        printf("LEDS_MULTI_CTRL_IOCTL_MULTI_SET failed\n");
        return -errno;
    }
    printf("gpio_leds_state_get gpiohandledata.values[0] = %d\n", gpiohandledata.values[0]);
    return gpiohandledata.values[0];
}

int gpio_leds_state_set(unsigned char value)
{
    int ret = -1;

    gpiohandledata.values[0] = value;
    if ((ret = ioctl(gpiohandle.fd, GPIOHANDLE_SET_LINE_VALUES_IOCTL, &gpiohandledata)) < 0){
        printf("LEDS_MULTI_CTRL_IOCTL_MULTI_SET failed\n");
        return -errno;
    }

    return 0;
}
