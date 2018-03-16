#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <signal.h>
#include <sys/time.h>

static int count = 0;

void test_func()
{
//	printf("count is %d\n", count++);
	++count;
}

void init_sigaction()
{
	struct sigaction act;

	act.sa_handler = test_func; //设置处理信号的函数
	act.sa_flags  = 0;

	sigemptyset(&act.sa_mask);
	sigaction(SIGPROF, &act, NULL);//时间到发送SIGROF信号
}

void init_time()
{
	struct itimerval val;

	val.it_value.tv_sec = 0; //1秒后启用定时器
	val.it_value.tv_usec = 10;

	val.it_interval = val.it_value; //定时器间隔为1s

	setitimer(ITIMER_PROF, &val, NULL);
}

int main(int argc, char **argv)
{

	init_sigaction();
	init_time();

	while (count < 10000) {
		usleep (20);
	}

	return 0;
}
