#include <stdio.h>
#include <stdlib.h>

int main (int argc, char *argv[]) {
	printf ("mount u pan \n");
	system ("mkdir /mnt/usb");
	system ("mount /dev/sda /mnt/usb");
	system ("ls /mnt/usb");
	system ("umount /mnt/usb");
	system ("rmdir /mnt/usb");
	return 0;
}
