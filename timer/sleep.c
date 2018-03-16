#include <stdio.h>
#include <unistdio.h>

int main ()
{
	int i = 0;
	for (i = 0; i < 100; ++i) {
		sleep (1);
//		usleep (10);
		printf ("timer \n");
	}

	return 0;
}
