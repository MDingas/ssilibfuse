CC=cc
FUSE_LIBS=`pkg-config fuse --libs`
GTK_LIBS=`pkg-config --libs gtk+-3.0`
FUSE_CFLAGS:=`pkg-config fuse --cflags`
GTK_CFLAGS+=`pkg-config --cflags gtk+-3.0`

all: mounter
	
mounter: main.o validation.o
	$(CC) main.o validation.o -o mounter $(FUSE_LIBS) $(GTK_LIBS)  

main.o: main.c params.h
	$(CC) $(FUSE_CFLAGS) $(GTK_CFLAGS) main.c -c 

validation.o: validation.c
	$(CC) $(GTK_CFLAGS) validation.c -c 

build: all
	rm -f *.o

clean: 
	rm -f *.o mounter
