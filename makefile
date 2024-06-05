hello: a.o b.o
	arm-linux-gnueabi-gcc a.o b.o -o hello
a.o: a.c
	arm-linux-gnueabi-gcc -c a.c
b.o: b.c
	arm-linux-gnueabi-gcc -c b.c
