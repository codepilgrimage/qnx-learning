# Makefile for Lab 04 - Signals
# Adjust QNX_TARGET / CPU as needed for your SDP environment.

CC := qcc
CFLAGS := -Vgcc_ntoaarch64le -g -Wall
SRC := src/main.c
BIN := bin/lab04-signals

all: $(BIN)

$(BIN): $(SRC)
	mkdir -p bin
	$(CC) $(CFLAGS) -o $(BIN) $(SRC)

clean:
	rm -rf bin

.PHONY: all clean
