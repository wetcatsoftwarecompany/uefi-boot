/* Kernel memory layout.
 * The kernel is linked to run at 1 MiB physical. UEFI identity-maps
 * memory, so the bootloader can load each segment at its p_paddr and
 * jump to the entry point without setting up paging first. */
OUTPUT_FORMAT(elf64-x86-64)
ENTRY(kernel_main)

SECTIONS
{
  . = 0x100000;

  .text   : ALIGN(4K) { *(.text .text.*) }
  .rodata : ALIGN(4K) { *(.rodata .rodata.*) }
  .data   : ALIGN(4K) { *(.data .data.*) }
  .bss    : ALIGN(4K) { *(COMMON) *(.bss .bss.*) }

  /DISCARD/ : { *(.comment) *(.note .note.*) *(.eh_frame*) }
}
