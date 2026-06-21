# Makefile for Lab 07 - Network
# Adjust QNX_TARGET / CPU as needed for your SDP environment.

CC := qcc
CFLAGS := -Vgcc_ntoaarch64le -g -Wall
SRC := src/main.c
BIN := bin/lab07-network

all: $(BIN)

$(BIN): $(SRC)
	mkdir -p bin
	$(CC) $(CFLAGS) -o $(BIN) $(SRC)

clean:
	rm -rf bin

.PHONY: all clean
