# Makefile for Lab 08 - Shared Memory
# Adjust QNX_TARGET / CPU as needed for your SDP environment.

CC := qcc
CFLAGS := -Vgcc_ntoaarch64le -g -Wall
SRC := src/main.c
BIN := bin/lab08-shared-memory

all: $(BIN)

$(BIN): $(SRC)
	mkdir -p bin
	$(CC) $(CFLAGS) -o $(BIN) $(SRC)

clean:
	rm -rf bin

.PHONY: all clean
