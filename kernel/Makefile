.RECIPEPREFIX = >

# Freestanding ELF kernel (System V ABI), loaded by the UEFI bootloader.
CC      := clang
LD      := ld.lld
TARGET  := x86_64-unknown-none-elf
CFLAGS  := -target $(TARGET) -std=c11 -ffreestanding -fno-stack-protector \
           -fno-pic -mno-red-zone -mgeneral-regs-only -Wall -Wextra -g \
           -Iinclude -I../include
LDFLAGS := -nostdlib -static -T linker.ld

SRC := $(wildcard src/*.c)
OBJ := $(patsubst src/%.c,build/%.o,$(SRC))
ELF := build/kernel.elf

all: $(ELF)

build/%.o: src/%.c $(wildcard src/*.h) $(wildcard ../include/*.h) | build
> $(CC) $(CFLAGS) -c $< -o $@

$(ELF): $(OBJ) linker.ld
> $(LD) $(LDFLAGS) $(OBJ) -o $@

build:
> mkdir -p $@

clean:
> rm -rf build

.PHONY: all clean
