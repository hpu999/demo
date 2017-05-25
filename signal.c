#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>

void my_func1 (int sign_no) {
	printf ("signal1 ...\n");
	exit (0);
}

void my_func2 (int sign_no) {
	printf ("signal2 ...\n");
	exit (0);
}

int main (int argc, char* argv[]) {
	// 注册触发中断函数的信号
	signal (SIGINT , my_func1);
	signal (SIGQUIT, my_func2);

	while (1) {
		;
	}
	return 0;
}
