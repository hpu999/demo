#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/file.h>
#include <errno.h>

static int g_mutex = -1;

void SleepUs(int waitus)
{
    usleep(waitus);
}

int XKF_UnlockDev(void)
{
    int ret;
    int i;

    if(g_mutex == -1)
        return 0;
    ret = flock(g_mutex,LOCK_UN);
    if(ret != 0)
    {
        SleepUs(10000);
        flock(g_mutex,LOCK_UN);
    }
    close(g_mutex);
    g_mutex = -1;

    return 0;
}

int XKF_LockDev(void)
{
    int ret;
    int i;
    int mutex = -1;

    mutex = open("/dev/zero",O_RDONLY);
    if(-1 == mutex)
    {
        return -2;
    }
    for (i=0; i<60*1000; i++)
    {
        ret = flock(mutex, LOCK_EX | LOCK_NB);
        if(0 == ret)
        {
            while(g_mutex != -1)
            {
                SleepUs(10);
            }
            g_mutex = mutex;
            return 0;
        }
        SleepUs(1000);
    }
    close(mutex);

    //超时,释放锁
    XKF_UnlockDev();

    return -3;
}

void *test (void *arg)
{
    int ret=-1;
    printf("%s[%d] \n", __func__, __LINE__);
   ret= XKF_LockDev();
    printf("%s[%d] ret=[%d]\n", __func__, __LINE__,ret);
    sleep(5);
   ret= XKF_UnlockDev();
    printf("%s[%d] ret=[%d]\n", __func__, __LINE__,ret);
}

int main (int argc, char argv[])
{
    pthread_t tid[64];

    pthread_create (&tid[0], NULL, test, NULL);
    pthread_create (&tid[1], NULL, test, NULL);
    pthread_create (&tid[2], NULL, test, NULL);
    pthread_create (&tid[3], NULL, test, NULL);
    pthread_create (&tid[4], NULL, test, NULL);
    pthread_create (&tid[5], NULL, test, NULL);
    pthread_create (&tid[6], NULL, test, NULL);
    pthread_create (&tid[7], NULL, test, NULL);
    pthread_create (&tid[8], NULL, test, NULL);
    pthread_create (&tid[9], NULL, test, NULL);

    pthread_join (tid[0], NULL);
    pthread_join (tid[1], NULL);
    pthread_join (tid[2], NULL);
    pthread_join (tid[3], NULL);
    pthread_join (tid[4], NULL);
    pthread_join (tid[5], NULL);
    pthread_join (tid[6], NULL);
    pthread_join (tid[7], NULL);
    pthread_join (tid[8], NULL);
    pthread_join (tid[9], NULL);

    return 0;
}
