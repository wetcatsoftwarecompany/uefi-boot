.RECIPEPREFIX = >

# UEFI application (PE/COFF, Microsoft ABI).
CC      := clang
TARGET  := x86_64-unknown-windows
CFLAGS  := -target $(TARGET) -std=c11 -ffreestanding -fshort-wchar -mno-red-zone \
           -fno-stack-protector -mno-stack-arg-probe -Wall -Wextra -g \
           -Iinclude -I../include
LDFLAGS := -target $(TARGET) -nostdlib -fuse-ld=lld-link \
           -Wl,-entry:efi_main -Wl,-subsystem:efi_application

SRC := $(wildcard src/*.c)
OBJ := $(patsubst src/%.c,build/%.o,$(SRC))
EFI := build/BOOTX64.EFI

all: $(EFI)

build/%.o: src/%.c $(wildcard include/*.h) $(wildcard ../include/*.h) | build
> $(CC) $(CFLAGS) -c $< -o $@

$(EFI): $(OBJ)
> $(CC) $(LDFLAGS) $^ -o $@

build:
> mkdir -p $@

clean:
> rm -rf build

.PHONY: all clean
