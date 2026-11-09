CC=gcc
CFLAGS=-I.

all : bin/client bin/server

bin/client.o : src/client.c src/mulmod.h
	$(CC) -o bin/client.o src/client.c $(CFLAGS) -c

bin/server.o : src/client.c src/mulmod.h
	$(CC) -o bin/server.o src/server.c $(CFLAGS) -c

bin/static/mulmod.o : src/mulmod.c src/mulmod.h
	$(CC) -o bin/static/mulmod.o src/mulmod.c $(CFLAGS) -c

bin/static/libmulmod.a : bin/client.o bin/server.o bin/static/mulmod.o src/mulmod.h
	ar rcs bin/static/libmulmod.a bin/static/mulmod.o

bin/client : bin/static/libmulmod.a
	$(CC) bin/client.o  -Lbin/static -lmulmod -lm -o bin/client

bin/server : bin/static/libmulmod.a
	$(CC) bin/server.o  -Lbin/static -lmulmod -lm -o bin/server

clean :
	rm bin/client bin/server bin/client.o bin/server.o bin/static/mulmod.o bin/static/libmulmod.a