#include <stdio.h>
#include <linux/rtc.h>
#include <sys/ioctl.h>
#include <sys/time.h>
#include <sys/types.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <stdlib.h>

int main(int argc, char* argv[])
{
	unsigned long i = 0;
	unsigned long data = 0;
	int retval = 0;
	int fd = open ("/dev/rtc", O_RDONLY);

	if(fd < 0)
	{
		perror("open");
		exit(errno);
	}

	/*Set the freq as 4Hz*/
	if(ioctl(fd, RTC_IRQP_SET, 1) < 0)
	{
		perror("ioctl(RTC_IRQP_SET)");
		close(fd);
		exit(errno);
	}
	/* Enable periodic interrupts */
	if(ioctl(fd, RTC_PIE_ON, 0) < 0)
	{
		perror("ioctl(RTC_PIE_ON)");
		close(fd);
		exit(errno);
	}

	for(i = 0; i < 100; i++)
	{
		if(read(fd, &data, sizeof(unsigned long)) < 0)
		{
			perror("read");
			close(fd);
			exit(errno);

		}
		printf("timer\n");
	}
	/* Disable periodic interrupts */
	ioctl(fd, RTC_PIE_OFF, 0);
	close(fd);

	return 0;
}
