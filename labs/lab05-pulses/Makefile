# Makefile — Lab 05: Pulses (death_pulse)

CC_ARM64  := qcc -Vgcc_ntoaarch64le
CC_X86    := qcc -Vgcc_ntox86_64
CFLAGS    := -g -Wall -Wextra
SRC       := src/death_pulse.c
BIN_ARM64 := bin/death_pulse
BIN_X86   := bin/death_pulse_x86

# Default: build for ARM64 (matches the default SDP 8.0 QEMU image)
all: arm64

arm64: $(BIN_ARM64)

x86: $(BIN_X86)

$(BIN_ARM64): $(SRC)
	mkdir -p bin
	$(CC_ARM64) $(CFLAGS) -o $@ $<

$(BIN_X86): $(SRC)
	mkdir -p bin
	$(CC_X86) $(CFLAGS) -o $@ $<

clean:
	rm -rf bin

.PHONY: all arm64 x86 clean
