.RECIPEPREFIX = >

CC      := clang
TARGET  := x86_64-unknown-windows
CFLAGS  := -target $(TARGET) -std=c11 -ffreestanding -fshort-wchar -mno-red-zone \
           -fno-stack-protector -mno-stack-arg-probe -Wall -Wextra -g -Iinclude
LDFLAGS := -target $(TARGET) -nostdlib -fuse-ld=lld-link \
           -Wl,-entry:efi_main -Wl,-subsystem:efi_application

SRC := $(wildcard src/*.c)
OBJ := $(patsubst src/%.c,build/%.o,$(SRC))
EFI := esp/EFI/BOOT/BOOTX64.EFI

OVMF_CODE := /usr/share/OVMF/OVMF_CODE_4M.fd
OVMF_VARS := /usr/share/OVMF/OVMF_VARS_4M.fd

all: $(EFI)

build/%.o: src/%.c | build
> $(CC) $(CFLAGS) -c $< -o $@

$(EFI): $(OBJ) | esp/EFI/BOOT
> $(CC) $(LDFLAGS) $^ -o $@

build esp/EFI/BOOT:
> mkdir -p $@

ovmf_vars.fd:
> cp $(OVMF_VARS) $@

run: $(EFI) ovmf_vars.fd
> qemu-system-x86_64 \
>   -drive if=pflash,format=raw,readonly=on,file=$(OVMF_CODE) \
>   -drive if=pflash,format=raw,file=ovmf_vars.fd \
>   -drive format=raw,file=fat:rw:esp \
>   -serial stdio -net none

clean:
> rm -rf build esp ovmf_vars.fd

.PHONY: all run clean
