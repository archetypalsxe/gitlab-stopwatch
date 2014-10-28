SRCDIR = src/

CC = gcc

ALL:
	$(CC) `pkg-config --cflags gtk+-3.0` $(SRCDIR)*.c -o RUN `pkg-config --libs gtk+-3.0`

clean:
	rm -f *.o

