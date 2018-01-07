TARGETNAME     = hid-penmount
OBJ            = hid-penmount.o
MODULE         = $(TARGETNAME).ko

ifneq ($(KERNELRELEASE),)
#=======================================================
# 1. During make kernel
#=======================================================

#=======================================================
# Module
#=======================================================
obj-m         += $(TARGETNAME).o

else

KERNELPATH    = /lib/modules/$(shell uname -r)
BUILDPATH     = $(KERNELPATH)/build
INSTALLPATH   = $(KERNELPATH)/kernel/drivers/hid
#=======================================================
# 2. Make individual module
#=======================================================
all:
	make -C $(BUILDPATH) M=$(PWD) modules
	
install:
	@if [ "`/sbin/lsmod | grep hid_penmount`" != "" ] ; then \
		/sbin/modprobe -r $(TARGETNAME) ; \
	fi
	@cp $(MODULE) $(INSTALLPATH)
	@/sbin/depmod
	@/sbin/modprobe $(TARGETNAME)
clean:
	make -C $(BUILDPATH) M=$(PWD) clean

endif
