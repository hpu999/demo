#include <stdio.h>
#include <stdlib.h>

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

int main (int argc, char *argv[])
{
    unsigned int num;
	num = convert (argv[1]);
    printf("The string 'str' is %s and the number 'num' is %d[0x%08x]. \n",
                   argv[1], num, num);
	return 0;
}
