#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <string.h>

#define MAX_SIZE 10

int main(void){
	int fd;
	char buf[MAX_SIZE];

	fd=open("/dev/my_virt_dev",O_RDWR);
	if(fd<0){
		printf("Fail: %s\n",strerror(errno));
		return 0;
	}

	write(fd,"hello",5);
	read(fd,buf,5);
	printf("%s\n",buf);

	close(fd);
	return 0;
}
