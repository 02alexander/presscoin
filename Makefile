
CC = gcc
CFLAGS = -O -Wall -ftrapv
TARGET = scheduler

all:
	$(CC) $(CFLAGS) scheduler.c -o $(TARGET)