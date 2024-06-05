#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/mman.h>

#define GPIO_BASE 0x3F200000
#define GPFSEL0 0x00
#define GPFSEL2 0x08
#define GPFSET0 0x1C
#define GPCLR0 0x28

#define DATA_IN 4
#define CLK_LAT 22
#define CLK_SH 23

int main() {
    int fd = open("/dev/mem", O_RDWR | O_SYNC);
    if (fd < 0) {
        printf("Error: can't open /dev/mem\n");
        exit(-1);
    }
    char *gpio_mmap = (char *)mmap(0, 4096, PROT_READ | PROT_WRITE, MAP_SHARED, fd, GPIO_BASE);
    if (gpio_mmap == MAP_FAILED) {
        printf("Error in mmap\n");
        exit(-1);
    }
    volatile unsigned int *gpio = (volatile unsigned int *)gpio_mmap;

    gpio[GPFSEL0 / 4] |= (1 << ((DATA_IN % 10) * 3)); 
    gpio[GPFSEL2 / 4] |= (1 << ((CLK_LAT % 10) * 3)); 
    gpio[GPFSEL2 / 4] |= (1 << ((CLK_SH % 10) * 3));  

    int signal[] = {1, 0, 1, 0, 1, 0, 1, 0};
    int i;

    while (1) {
        for (i = 0; i < 8; i++) {
            if (signal[i] == 1) {
                gpio[GPFSET0 / 4] = (1 << (DATA_IN % 32)); 
            } else {
                gpio[GPCLR0 / 4] = (1 << (DATA_IN % 32)); 
            }

            gpio[GPFSET0 / 4] = (1 << (CLK_SH % 32)); 
            usleep(100000); 
            gpio[GPCLR0 / 4] = (1 << (CLK_SH % 32)); 
            usleep(100000); 
        }

        gpio[GPFSET0 / 4] = (1 << (CLK_LAT % 32)); 
        usleep(100000); 
        gpio[GPCLR0 / 4] = (1 << (CLK_LAT % 32)); 
        usleep(100000); 
    }

    munmap(gpio_mmap, 4096);
    return 0;
}
