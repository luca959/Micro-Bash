OBJS	= Micro.o
SOURCE	= Micro.c
HEADER	= stdio.h unistd.h stdlib.h sys/wait.h string.h stdbool.h errno.h
OUT	= micro_bash
CC	= gcc
FLAGS 	= -Wall -pedantic -Werror

all: micro_bash

micro_bash: $(OBJS)
	$(CC) -o $@ $^ $(FLAGS)

%.o: %.c $(HEADER)
	$(CC) -c -o $@ $< $(FLAGS)
	
run: $(OUT)
	./$(OUT)
