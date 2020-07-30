CC=gcc
CFLAGS=-std=c99

memsim: memsim.o lru_policy.o fifo_policy.o
	$(CC) -o memsim memsim.o lru_policy.o fifo_policy.o
memsim.o: memsim.c lru_policy.h fifo_policy.h
	$(CC) -c memsim.c
lru_policy.o: lru_policy.c lru_policy.h
	$(CC) -c lru_policy.c
fifo_policy.o: fifo_policy.c fifo_policy.h
	$(CC) -c fifo_policy.c
