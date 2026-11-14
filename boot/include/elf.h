/* Minimal ELF64 definitions needed to load the kernel. */
#ifndef ELF_H
#define ELF_H

#include <stdint.h>

#define EI_CLASS      4
#define EI_DATA       5
#define ELFCLASS64    2
#define ELFDATA2LSB   1     /* little endian */
#define ET_EXEC       2     /* executable file */
#define EM_X86_64     62
#define PT_LOAD       1     /* loadable segment */

typedef struct {
  uint8_t   e_ident[16];    /* 0x7F 'E' 'L' 'F', class, data, ... */
  uint16_t  e_type;
  uint16_t  e_machine;
  uint32_t  e_version;
  uint64_t  e_entry;        /* entry point address */
  uint64_t  e_phoff;        /* program header table offset */
  uint64_t  e_shoff;
  uint32_t  e_flags;
  uint16_t  e_ehsize;
  uint16_t  e_phentsize;    /* size of one program header */
  uint16_t  e_phnum;        /* number of program headers */
  uint16_t  e_shentsize;
  uint16_t  e_shnum;
  uint16_t  e_shstrndx;
} Elf64_Ehdr;

typedef struct {
  uint32_t  p_type;
  uint32_t  p_flags;
  uint64_t  p_offset;       /* where the segment starts in the file */
  uint64_t  p_vaddr;
  uint64_t  p_paddr;        /* where it must be loaded in memory */
  uint64_t  p_filesz;       /* bytes to copy from the file */
  uint64_t  p_memsz;        /* bytes in memory (rest is zeroed, e.g. .bss) */
  uint64_t  p_align;
} Elf64_Phdr;

#endif /* ELF_H */
