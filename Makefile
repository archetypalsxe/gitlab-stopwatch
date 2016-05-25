SRCDIR = src/

CC = gcc

ALL:
	$(CC) -lnotify `pkg-config --cflags gtk+-3.0` $(SRCDIR)test.c -o RUN `pkg-config --libs gtk+-3.0`

clean:
	rm -f *.o

