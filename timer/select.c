#include <sys/time.h>
#include <sys/select.h>
#include <time.h>
#include <stdio.h>

/*seconds: the seconds; mseconds: the micro seconds*/
void setTimer(int seconds, int mseconds)
{
	struct timeval temp;

	temp.tv_sec = seconds;
	temp.tv_usec = mseconds;

	select(0, NULL, NULL, NULL, &temp);
	printf("timer\n");

	return ;
}

int main()
{
	int i;

	for(i = 0 ; i < 100; i++)
	  setTimer(1, 0);

	return 0;
}
