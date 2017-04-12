/*
 * Copyright 2004-2009 Freescale Semiconductor, Inc. All rights reserved.
 *
 * The code contained herein is licensed under the GNU General Public
 * License. You may obtain a copy of the GNU General Public License
 * Version 2 or later at the following locations:
 *
 * http://www.opensource.org/licenses/gpl-license.html
 * http://www.gnu.org/copyleft/gpl.html
 */

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>
#include <fcntl.h>
#include <poll.h>
#include <sys/ioctl.h>
#include <termios.h>
#include <unistd.h>

struct cmd_data {
	unsigned int m_addr;
	unsigned int m_data;
};

unsigned int convert (char *str) {
    unsigned int i,num = 0;

    if (str[0] == '0' && (str[1] == 'x' || str[1] == 'X')) {
        for(i = 0;str[i] != '\0';i++) {
            if(str[i]>='0' && str[i]<='9')
             num = num*16 + str[i]-'0';
           else if(str[i]<='f' && str[i]>='a')
                num = num*16 + str[i]-'a' + 10; 
            else if(str[i]<='F' && str[i]>='A')
                num = num*16 + str[i]-'A' + 10; 
        }
    }   
    else {
        num = atoi (str);
    }   

    return num;
}

int read_test(struct cmd_data *data) {
	int fd;
	fd = open("/dev/register_test",O_RDWR);
	if(fd < 0){
	    printf("Failed open\n");
		exit(-1);
	}

	read(fd, data, sizeof(struct cmd_data));

	close(fd);

	printf ("read register : reg[0x%08x]   -->   val[0x%08x] \n", data->m_addr, data->m_data);
	return 0;
}

int write_test(struct cmd_data *data) {
	int fd;
	fd = open("/dev/register_test",O_RDWR);
	if(fd < 0){
	    printf("Failed open\n");
		exit(-1);
	}

	write(fd, data, sizeof(struct cmd_data));

	close(fd);

	printf ("set register : reg[0x%08x]   -->   val[0x%08x] \n", data->m_addr, data->m_data);
	return 0;
}

int main(int argc,char **argv) {

	struct cmd_data data;
	if (3 == argc) {
		if (!strcmp (argv[1], "read")) {
			data.m_addr = convert (argv[2]);
			data.m_data = 0;
			read_test (&data);
		}
	}
	if (4 == argc) {
		if (!strcmp (argv[1], "write")) {
			data.m_addr = convert (argv[2]);
			data.m_data = convert (argv[3]);
			write_test (&data);
		}
	}
	
  return 0;
}
