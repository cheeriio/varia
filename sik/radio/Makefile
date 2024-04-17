CFLAGS = -pedantic -Wall -Wextra -std=gnu17 -pthread -O2
CC = gcc

all: sikradio-sender sikradio-receiver

sikradio-sender: radio-sender.c radio-util.h common.h err.h
	$(CC) $(CFLAGS) radio-sender.c -o sikradio-sender

sikradio-receiver: radio-receiver.c radio-util.h common.h err.h
	$(CC) $(CFLAGS) radio-receiver.c -o sikradio-receiver

clean:
	rm -f *.o
	rm sikradio-sender
	rm sikradio-receiver
