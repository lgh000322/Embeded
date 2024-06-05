#include <stdio.h>
#include <stdint.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <string.h>
#include <sys/ioctl.h>
#include <asm/ioctl.h>
#include <linux/spi/spidev.h>
#include <sys/mman.h> // 추가된 헤더 파일
#include <stdlib.h>

#define DATA_IN 4
#define GPFSEL0 0x00
#define GPIO_BASE 0x3F200000
#define GPCLRO 0x28

static const uint8_t spiBPW = 8;
static const uint16_t spiDelay = 0;
static uint32_t spiSpeed = 1000000;
static int spiChannel = 2;
static int spiFd;

int spi_setup(int channel, int speed, int mode);
int spi_data_rw(int fd, int channel, unsigned char* data, int len);
int adc_mcp3008_read(int channel);

int main(void) {
    int fd;
    int fd2;

    fd = spi_setup(spiChannel, spiSpeed, 0);
    if (fd < 0) {
        printf("fd오류\n");
        return 1; // 실패 시 프로그램 종료
    }

    fd2 = open("/dev/mem", O_RDWR | O_SYNC); // 파일 열기 옵션 수정: 파일 열기 옵션에 < 추가
    if (fd2 < 0) {
        printf("fd2오류\n");
        return 1;
    }

    char *gpio_mmap = (char*) mmap(0, 4096, PROT_READ | PROT_WRITE, MAP_SHARED, fd2, GPIO_BASE);
    if (gpio_mmap == MAP_FAILED) {
        printf("gpio_mmap 오류\n");
        exit(-1);
    }

    volatile unsigned int *gpio = (volatile unsigned int*) gpio_mmap;
    gpio[GPFSEL0 / 4] |= (1 << ((DATA_IN % 10) * 3));
    gpio[GPCLRO / 4] = (1 << (DATA_IN % 32));

    while (1) {
        int x_value = adc_mcp3008_read(0); // X좌표
        int y_value = adc_mcp3008_read(1); // Y좌표

        if (y_value > 600) {
            gpio[GPCLRO / 4] = (1 << (DATA_IN % 32)); // GPCLRO 수정
        }
        printf("X: %d, Y: %d\n", x_value, y_value);
        sleep(1);
    }

    close(spiFd);
    return 0; 
}

int spi_setup(int channel, int speed, int mode) {
    int fd;
    char spiDev[32];
    mode &= 3;

    snprintf(spiDev, 31, "/dev/spidev0.%d", channel);

    fd = open(spiDev, O_RDWR);
    if (fd < 0) {
        printf("Fail: %s\n", strerror(errno));
        return -1;
    }
    spiFd = fd;

    int err = 0;
    err |= ioctl(fd, SPI_IOC_WR_MODE, &mode);
    err |= ioctl(fd, SPI_IOC_WR_BITS_PER_WORD, &spiBPW);
    err |= ioctl(fd, SPI_IOC_WR_MAX_SPEED_HZ, &speed);

    if (err < 0) {
        printf("Error in setup\n");
        return -1;
    }

    return fd;
}

int spi_data_rw(int fd, int channel, unsigned char* data, int len) {
    struct spi_ioc_transfer tr;
    memset(&tr, 0, sizeof(tr));

    tr.tx_buf = (unsigned long)data;
    tr.rx_buf = (unsigned long)data;
    tr.len = len;
    tr.delay_usecs = spiDelay;
    tr.speed_hz = spiSpeed;
    tr.bits_per_word = spiBPW;

    int err = ioctl(fd, SPI_IOC_MESSAGE(1), &tr);

    if (err < 0) {
        printf("Error in r/w\n");
        return err;
    }

    return 0;
}

int adc_mcp3008_read(int channel) {
    unsigned char buf[3];
    buf[0] = 0x01;
    buf[1] = (0x80 | (channel << 4)); // 채널 설정 수정
    buf[2] = 0x00;
    spi_data_rw(spiFd, spiChannel, buf, 3);

    return ((buf[1] & 0x03) << 8) | buf[2];
}