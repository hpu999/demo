#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <sys/types.h>

int main()
{
    pid_t status;


    status = system("ls");

    if (-1 == status) {
        printf("system error!");
    }
    else  {
        printf("exit status value = [0x%x]\n", status);

        if (WIFEXITED(status)) {
            if (0 == WEXITSTATUS(status)) {
                printf("run shell script successfully.\n");
            }
            else {
                printf("run shell script fail, script exit code: %d\n", WEXITSTATUS(status));
            }
        }
        else {
            printf("exit status = [%d]\n", WEXITSTATUS(status));
        }
    }

    return 0;
}
