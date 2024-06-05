CR_C:=arm-linux-gnueabihf-
obj-m=led_pratice.o

KDIR:=/home/lgh/linux-1.20230405/
PWD:=$(shell pwd)

default:
	 make ARCH=arm CROSS_COMPILE=$(CR_C) -C $(KDIR) M=$(PWD) modules
clean:
	make ARCH=arm CROSS_COMPILE=$(CR_C) -C $(KDIR) M=$(PWD) clean
