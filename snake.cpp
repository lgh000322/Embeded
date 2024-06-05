#include <iostream>
#include <thread>
#include <mutex>
#include <queue>
#include <unistd.h>
#include <condition_variable>
#include <stdio.h>
#include <stdint.h>
#include <fcntl.h>
#include <errno.h>
#include <string.h>
#include <sys/ioctl.h>
#include <asm/ioctl.h>
#include <linux/spi/spidev.h>
#include <sys/mman.h>
#include <stdlib.h>

#define DATA_IN 4
#define GPFSEL0 0x00
#define GPIO_BASE 0x3F200000
#define GPCLRO 0x28

std::mutex m;
std::queue<int> q;
std::condition_variable cv;

using namespace std;

static const uint8_t spiBPW = 8;
static const uint16_t spiDelay = 0;
static uint32_t spiSpeed = 1000000;
static int spiChannel = 2;
static int spiFd;

const int width = 80;
const int height = 20;

int x, y;
int fruitCordX, fruitCordY;
int playerScore;
int snakeTailX[100], snakeTailY[100];
int snakeTailLen;
enum snakesDirection { STOP = 0, LEFT, RIGHT, UP, DOWN };
snakesDirection sDir;
bool isGameOver;

void GameInit()
{
    isGameOver = false;
    sDir = STOP;
    x = width / 2;
    y = height / 2;
    fruitCordX = rand() % width;
    fruitCordY = rand() % height;
    playerScore = 0;
}

void GameRender(string playerName)
{
    std::system("clear");

    for (int i = 0; i < width + 2; i++)
        cout << "-";
    cout << endl;

    for (int i = 0; i < height; i++) {
        for (int j = 0; j <= width; j++) {
            if (j == 0 || j == width)
                cout << "|";
            if (i == y && j == x)
                cout << "O";
            else if (i == fruitCordY && j == fruitCordX)
                cout << "#";
            else {
                bool prTail = false;
                for (int k = 0; k < snakeTailLen; k++) {
                    if (snakeTailX[k] == j && snakeTailY[k] == i) {
                        cout << "o";
                        prTail = true;
                    }
                }
                if (!prTail)
                    cout << " ";
            }
        }
        cout << endl;
    }

    for (int i = 0; i < width + 2; i++)
        cout << "-";
    cout << endl;

    cout << playerName << "'s Score: " << playerScore << endl;
    cout << "Up: 8 / Down: 2 / Left: 4 / Right: 6" << endl;
}

void UpdateGame()
{
    int headX = x;
    int headY = y;
    int prevX = snakeTailX[0];
    int prevY = snakeTailY[0];
    int prev2X, prev2Y;

    switch (sDir) {
        case LEFT:
            x--;
            if (x < 0) { x = 0; return; }
            break;
        case RIGHT:
            x++;
            if (x >= width) { x = width - 1; return; }
            break;
        case UP:
            y--;
            if (y < 0) { y = 0; return; }
            break;
        case DOWN:
            y++;
            if (y >= height) { y = height - 1; return; }
            break;
    }

    snakeTailX[0] = headX;
    snakeTailY[0] = headY;

    for (int i = 1; i < snakeTailLen; i++) {
        prev2X = snakeTailX[i];
        prev2Y = snakeTailY[i];
        snakeTailX[i] = prevX;
        snakeTailY[i] = prevY;
        prevX = prev2X;
        prevY = prev2Y;
    }

    for (int i = 0; i < snakeTailLen; i++) {
        if (snakeTailX[i] == x && snakeTailY[i] == y)
		//테스트를 위해 게임 종료조건을 false로 설정함.
            isGameOver = false;
    }

    if (x == fruitCordX && y == fruitCordY) {
        playerScore += 10;
        fruitCordX = rand() % width;
        fruitCordY = rand() % height;
        snakeTailLen++;
    }
}

void UserInput()
{
    int kbhit;
    std::cin >> kbhit;
    switch (kbhit) {
        case 4:
            sDir = LEFT;
            break;
        case 6:
            sDir = RIGHT;
            break;
        case 8:
            sDir = UP;
            break;
        case 2:
            sDir = DOWN;
            break;
        case 0:
            isGameOver = true;
            break;
    }
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
    buf[1] = (0x80 | (channel << 4));
    buf[2] = 0x00;
    spi_data_rw(spiFd, spiChannel, buf, 3);

    return ((buf[1] & 0x03) << 8) | buf[2];
}

void initFd() {
    int fd;
    fd = spi_setup(spiChannel, spiSpeed, 0);
    if (fd < 0) {
        std::cout << "fd오류" << std::endl;
        exit(1);
    }
}

void snakeAct() {
    while (!isGameOver) {
        std::unique_lock<std::mutex> lk(m);
        cv.wait(lk, [&] { return !q.empty() || isGameOver; });

        if (isGameOver) {
            lk.unlock();
            return;
        }

        sDir = static_cast<snakesDirection>(q.front());
        q.pop();
        
		if(sDir!=STOP)
		{
			GameRender("Player");
            UpdateGame();
		}
        
        lk.unlock();
    }
}

void joyStickAct() {
    while (!isGameOver) {
		m.lock();
        unsigned int x_value = adc_mcp3008_read(0);
        unsigned int y_value = adc_mcp3008_read(1);

        snakesDirection dir = sDir;

        if ((x_value > 400 && x_value < 600) && (y_value < 300)) {
            dir = UP;
        } else if ((x_value > 400 && x_value < 600) && (y_value > 700)) {
            dir = DOWN;
        } else if ((x_value < 400) && (y_value > 300 && y_value < 700)) {
            dir = LEFT;
        } else if ((x_value > 600) && (y_value > 300 && y_value < 700)) {
            dir = RIGHT;
        }else{
			dir=STOP;
		}

        q.push(dir);
        m.unlock();
        cv.notify_one();
        usleep(150000); // Delay to avoid rapid input
    }
}

int main()
{
    string playerName;
    cout << "Enter your name: ";
    cin >> playerName;

    GameInit();
    initFd();
	GameRender("Player");
	UpdateGame();

    std::thread t1(snakeAct);
    std::thread t2(joyStickAct);

    t1.join();
    t2.join();

    return 0;
}
