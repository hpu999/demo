#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

int main (int argc, char *argv[]) {
	int fd;

	printf ("mount u pan \n");
	system ("mkdir /mnt/usb");
	system ("mount /dev/sda /mnt/usb");
	system ("touch /mnt/usb/test");

	fd = open ("/mnt/usb/test", O_RDWR);
	if (fd < 0) {
		perror ("open /mnt/usb/test");
		system ("umount /mnt/usb");
		system ("rmdir /mnt/usb");
		return -1;
	}
	printf ("I can open the file in the usb. \n");
	close (fd);

	system ("rm /mnt/usb/test");
	system ("umount /mnt/usb");
	system ("rmdir /mnt/usb");
	return 0;
}
