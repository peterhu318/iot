CC = gcc
CFLAGS = -Wall -Wextra
LIBS = -lpaho-mqtt3c -lm

iot: iot.c iot.h yl.c ll.c
	$(CC) -o iot iot.c yl.c ll.c  $(LIBS)
clean:
	rm -f iot
