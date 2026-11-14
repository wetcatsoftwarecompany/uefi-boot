.RECIPEPREFIX = >

# Top level: builds the bootloader and kernel, then assembles the ESP.
#   make        build everything into esp/
#   make run    build and boot in QEMU
#   make clean  remove all build output

BOOT_EFI   := boot/build/BOOTX64.EFI
KERNEL_ELF := kernel/build/kernel.elf

OVMF_CODE := /usr/share/OVMF/OVMF_CODE_4M.fd
OVMF_VARS := /usr/share/OVMF/OVMF_VARS_4M.fd

all: esp

boot:
> $(MAKE) -C boot

kernel:
> $(MAKE) -C kernel

# esp/ is generated: files/ holds extra files copied to the ESP root.
esp: boot kernel
> mkdir -p esp/EFI/BOOT
> cp $(BOOT_EFI) esp/EFI/BOOT/BOOTX64.EFI
> cp $(KERNEL_ELF) esp/kernel.elf
> cp -r files/. esp/

ovmf_vars.fd:
> cp $(OVMF_VARS) $@

run: esp ovmf_vars.fd
> qemu-system-x86_64 \
>   -drive if=pflash,format=raw,readonly=on,file=$(OVMF_CODE) \
>   -drive if=pflash,format=raw,file=ovmf_vars.fd \
>   -drive format=raw,file=fat:rw:esp \
>   -serial stdio -net none

clean:
> $(MAKE) -C boot clean
> $(MAKE) -C kernel clean
> rm -rf build esp ovmf_vars.fd

.PHONY: all boot kernel esp run clean
