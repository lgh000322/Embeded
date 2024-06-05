#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <string.h>

#define MAX_SIZE 10

int main(void)
{
    int fd;
    char buf[MAX_SIZE];

    fd=open("/dev/my_led_dev",O_RDWR);
    if(fd<0)
    {
        printf("Fail: %s\n",strerror(errno));
        return 0;
    }

    buf[0]=1;
    write(fd,buf,1);
    sleep(1);

    close(fd);
    return 0;


}
