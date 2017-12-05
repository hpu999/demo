#include <sys/mman.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include <stdio.h>

#define BUF_SIZE 4096 

int main(int argc,char *argv[])
{
	if (3 > argc) {
		printf ("Usage : %s <src_file> <dest_file> \n", argv[0]);
		return 0;
	}

	int fd_r,fd_w;
	char * p_w,*p_r ;
	char buf[BUF_SIZE];
	int length_r,length_w,len = 0;
	int off= 0;
	assert(argc == 3);
	fd_r = open(argv[1],O_RDWR);
	assert(fd_r);
	fd_w = open(argv[2],O_RDWR|O_CREAT|O_TRUNC,0644);
	assert(fd_w);
	length_r = lseek(fd_r,0,SEEK_END);
	length_w = lseek(fd_w,length_r - 1,SEEK_CUR);
	write(fd_w, "0", 1);
	lseek(fd_w,0,SEEK_SET);
	lseek(fd_r,0,SEEK_SET);
	//assert(p_w);
	while(1)
	{
		p_r = mmap(NULL,BUF_SIZE,PROT_READ|PROT_WRITE,MAP_SHARED,fd_r,off);
		p_w = mmap(NULL,BUF_SIZE,PROT_WRITE,MAP_SHARED,fd_w,off);
		memcpy(p_w,p_r,BUF_SIZE);
		off += BUF_SIZE;
		if((off+BUF_SIZE) >= length_r)
		{
			p_r = mmap(NULL,length_r - off ,PROT_READ|PROT_WRITE,MAP_SHARED,fd_r,off);
			p_w = mmap(NULL,length_r - off,PROT_WRITE,MAP_SHARED,fd_w,off);
			memcpy(p_w,p_r,length_r -off );
			munmap(p_r,length_r - off);
			munmap(p_w,length_r - off);
			break;	
		}
		munmap(p_r, BUF_SIZE);
		munmap(p_w, BUF_SIZE);
	}
	
	
	close(fd_r);
	close(fd_w);
	return 0;
}
