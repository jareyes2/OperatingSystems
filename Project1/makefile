CC=gcc
CFLAGS=-std=c99
all: runsc runcs

runsc: system_call.o
	$(CC) -o runsc system_call.o
runcs: context_switch.o
	$(CC) -o runcs context_switch.o

.PHONY: clean
clean: 
	rm system_call.o
	rm runsc
	rm context_switch.o
	rm runcs
