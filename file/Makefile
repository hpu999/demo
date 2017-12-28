obj-m +=k_file.o

all:
	make -C /lib/modules/`uname -r`/build SUBDIRS=$(PWD) modules
clean:
	make -C /lib/modules/`uname -r`/build SUBDIRS=$(PWD) clean
install:
	scp k_file.ko root@192.168.1.4:/tmp	
