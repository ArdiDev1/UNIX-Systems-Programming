CLI=client
SRV=server
OBJS=command.o
HEADERS=myftp.h

TARGETS=$(CLI) $(SRV)
# SRCS=$(patsubst %,%.c,$(TARGETS)) $(patsubst %.o,%.c,$(OBJS))
CC=gcc
CFLAGS=-Wall -g -Wextra -std=gnu99
CFLAGS_SRV=$(CFLAGS) -pthread -c
LDFLAGS_SRV=-pthread

all : $(TARGETS)

$(CLI): $(CLI).o $(OBJS)
	$(CC) -o $@ $^

$(CLI).o: $(CLI).c $(HEADERS)

$(SRV): $(SRV).o $(OBJS)
	$(CC) $(LDFLAGS_SRV) -o $@ $^

$(SRV).o: $(SRV).c $(HEADERS)
	$(CC) $(CFLAGS_SRV) -o $@ $<

clean: 
	@rm -f $(TARGETS) *.o a.out 
