#include <stdio.h>
#include <stdlib.h>
#include <string.h>

enum SpiMode {
    CPHA        = 0x01,     
    CPOL        = 0x02, 

    MODE0       = 0x00,
    MODE1       = MODE0|CPHA,
    MODE2       = CPOL|MODE0,
    MODE3       = CPOL|CPHA
};

int main (int argc, char *argv[]) {
	printf ("CPHA = %d \n", CPHA);
	printf ("CPOL = %d \n", CPOL);
	printf ("MODE0 = %d \n", MODE0);
	printf ("MODE1 = %d \n", MODE1);
	printf ("MODE2 = %d \n", MODE2);
	printf ("MODE3 = %d \n", MODE3);

	enum SpiMode mode = MODE2;
	printf ("mode = %d \n", mode);

	return 0;
}
