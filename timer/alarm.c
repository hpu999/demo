#include <stdio.h>
#include <signal.h>

void timer(int sig)
{
	if(SIGALRM == sig)
	{
		printf("timer\n");
		alarm(1);    //we contimue set the timer
	}

	return ;
}

int main()
{
	signal(SIGALRM, timer); //relate the signal and function

	alarm(1);    //trigger the timer

	getchar();

	return 0;
}
