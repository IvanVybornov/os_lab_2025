CC=gcc
CFLAGS=-I.

all : zombie process_memory parallel_sum

zombie :
	$(CC) -o zombie zombie.c $(CFLAGS)

process_memory :
	$(CC) -o process_memory process_memory.c $(CFLAGS)

parallel_sum : utils.o ../../lab3/src/utils.h
	$(CC) -o parallel_sum utils.o parallel_sum.c $(CFLAGS)

utils.o : ../../lab3/src/utils.h
	$(CC) -o utils.o -c ../../lab3/src/utils.c $(CFLAGS)

clean :
	rm zombie parallel_sum process_memory utils.o