CC=gcc
CFLAGS=-O3 -march=native
OBJ=simple-curl.o

all: simple-curl

%.o: %.c
	$(CC) -c -o $@ $< $(CFLAGS) $(shell curl-config --cflags)

simple-curl: simple-curl.o
	$(CC) $(OBJ) -o simple-curl $(shell curl-config --libs)

clean:
	rm simple-curl $(OBJ)
