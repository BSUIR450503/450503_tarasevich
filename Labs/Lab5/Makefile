CC = gcc
CFLAGS = -Wall -Wpedantic -std=c11
OS = $(shell uname)

all: lib
 ifeq ($(OS), Linux)
	$(CC) $(CFLAGS) -o lab5 main.c handler.c -lrt -lpthread -ldl
 else
	$(CC) -o lab5 main.c handler.c
 endif

lib:
 ifeq ($(OS), Linux)
	$(CC) $(CFLAGS) -fPIC -c dynamic.c -lpthread -lrt -laio
	$(CC) $(CFLAGS) -shared -o Linux_Shared.so dynamic.o -lrt -lpthread -ldl
	rm dynamic.o		
 else
	$(CC) $(CFLAGS) -c dynamic.c
	$(CC) $(CFLAGS) -shared -o Windows_Shared.dll dynamic.o
	rm dynamic.o
 endif