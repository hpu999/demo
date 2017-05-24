#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

int fd1, fd2;
int size;
char buff[1];
char buff2[1];
int cnt = 0;

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
		return -1;
	}
	
	size = lseek (fd1, 0, SEEK_END);
	lseek (fd1, 0, SEEK_SET);

	int tmp = size;	
	int res1, res2;

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
	
	close (fd1);
	close (fd2);

	return 0;

err:
	close (fd1);
	close (fd2);
	return -1;
}
