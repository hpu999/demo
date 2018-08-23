KERNELDIR ?=  /lib/modules/`uname -r`/build
PWD := $(shell pwd)
CROSS_COMPILE ?=
CC ?= $(CROSS_COMPILE)gcc

obj-m := mmap_drv2.o

all:
	$(MAKE) -C $(KERNELDIR) M=$(PWD) modules

clean:
	rm -rf *.o *~ core .depend .*.cmd *.ko *.mod.c .tmp_versions *.markers *.symvers *.order
