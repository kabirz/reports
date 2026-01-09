CC = gcc
CFLAGS = -Wall -Wextra -pthread -std=c99 -D_POSIX_C_SOURCE=200809L
LDFLAGS = -pthread -lrt

SRCS = $(wildcard *.c)
TARGETS = $(SRCS:.c=)

.PHONY: all clean

all: $(TARGETS)

%: %.c
	$(CC) $(CFLAGS) -o $@ $< $(LDFLAGS)

clean:
	rm -f $(TARGETS)