# Generic Makefile for one C file

CC = gcc
CFLAGS = -Wall -Wextra -O2

# Pass FILE=filename (without .c)
FILE ?= main

.PHONY: all clean run

all: $(FILE)

$(FILE): $(FILE).c
	$(CC) $(CFLAGS) -o $@ $<

run: $(FILE)
	./$(FILE)
	rm -f $(FILE)