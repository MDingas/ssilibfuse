CC=cc
FUSE_LIBS=`pkg-config fuse --libs`
GTK_LIBS=`pkg-config --libs gtk+-3.0`
FUSE_CFLAGS:=`pkg-config fuse --cflags`
GTK_CFLAGS+=`pkg-config --cflags gtk+-3.0`

all: mounter

mounter: filesystem.o validation.o email.o utils.o
	$(CC) filesystem.o validation.o email.o utils.o -o mounter $(FUSE_LIBS) $(GTK_LIBS)

filesystem.o: filesystem.c params.h
	$(CC) $(FUSE_CFLAGS) $(GTK_CFLAGS) filesystem.c -c

validation.o: validation.c
	$(CC) $(GTK_CFLAGS) validation.c -c

email.o: email.c
	$(CC) email.c -c

utils.o: utils.c
	$(CC) utils.c -c

build: all
	rm -f *.o

clean:
	rm -f *.o mounter
