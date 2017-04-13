#include <stdio.h>
#include <unistd.h>

int main (int argc, char *argv[]) {
	int status;
	status = access ("test", F_OK);
	if (status < 0) {
		printf ("test is not exited. \n");
		return -1;
	}
	printf ("test is exited. \n");
	return 0;
}
