CC=gcc
CFLAG= -std=c99 -g
LDFLAGS= -lreadline -lhistory -lncurses -lpthread 
TARGET = cube

all: cube

cube: cube.c 
	$(CC) $(CFLAG) cube.c wizard.c $(LDFLAGS) -o cube

.PHONY: clean
clean:
	rm cube

