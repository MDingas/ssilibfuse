CC=cc
LIBS=`pkg-config fuse --libs`
CFLAGS=`pkg-config fuse --cflags`

build: all
	rm -f *.o

all: main.o
	$(CC) main.o -o mounter $(LIBS)  

main.o: main.c params.h
	$(CC) $(CFLAGS) main.c -c 

clean: 
	rm -f *.o mounter