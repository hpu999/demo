#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <signal.h>

int fd1, fd2;
int size, size1, size2;
char buff[1];
char buff2[1];
int cnt = 0;

void my_func (int sign_no) {
	printf ("signal ...\n");
	exit (0);
}

int main (int argc, char* argv[]) {
	if ((NULL == argv[1]) || (NULL == argv[2])) {
		printf ("Usage:%s <file1> <file2> \n", argv[0]);
	}

	fd1 = open (argv[1], O_RDONLY);
	if (fd1 < 0) {
		perror ("Open file1:");
		return -1;
	}

	fd2 = open (argv[2], O_RDONLY);
	if (fd2 < 0) {
		perror ("Open file2 ");
		goto err1;
	}
	
	size1 = lseek (fd1, 0, SEEK_END);
	lseek (fd1, 0, SEEK_SET);

	size2 = lseek (fd2, 0, SEEK_END);
	lseek (fd2, 0, SEEK_SET);
	
	if (size1 - size2) {
		printf ("%s \n", (size1 - size2) > 0 ? "第一个文件比第二个文件大" : "第二个文件比第一个文件大");
	}

	size = size1 > size2 ? size2 : size1;

	int tmp = size;	
	int res1, res2;

	signal (SIGINT, my_func);
//	signal (SIGQUIT, my_func);

	while (tmp) {
		memset (buff, 0, sizeof (buff));
		memset (buff2, 0, sizeof (buff2));

		res1 = read (fd1, buff, sizeof (buff));
		if (res1 < 0) {
			perror ("read file1 ");
			goto err;
		}
		res2 = read (fd2, buff2, sizeof (buff2));
		if (res2 < 0) {
			perror ("read file2 ");
			goto err;
		}
		if (res1 - res2) {
			lseek (fd1, 0 - res1, SEEK_CUR);
			lseek (fd2, 0 - res2, SEEK_CUR);
			continue;
		}
		tmp -= res1;
		if (memcmp (buff, buff2, res1)) {
			++cnt;
			printf ("offset = %d \n", size - tmp);
//			goto err;
		} 
	}
	printf ("cnt = %d \n", cnt);
	
	close (fd1);
	close (fd2);

	return 0;

err:
	close (fd2);
err1:
	close (fd1);
	return -1;
}
