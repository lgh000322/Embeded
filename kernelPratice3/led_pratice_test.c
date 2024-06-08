#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <string.h>

#define MAX_SIZE 8

int main(void) {
    int fd;
    printf("test executing\n");
    char buf[MAX_SIZE] = {1, 0, 1, 0, 1, 0, 1, 0};
    printf("buf initialized\n");

    fd = open("/dev/my_led_dev", O_RDWR);
    if (fd < 0) {
        printf("Fail: %s\n", strerror(errno));
        return 1;
    }

    printf("File opened\n");

    if (write(fd, buf, MAX_SIZE) < 0) {
        printf("Write failed: %s\n", strerror(errno));
        close(fd);
        return 1;
    }

    printf("buf written\n");
    close(fd);

    return 0;
}
