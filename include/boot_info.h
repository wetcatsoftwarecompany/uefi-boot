/* Contract between the bootloader and the kernel.
 *
 * The bootloader fills in a BootInfo before calling ExitBootServices and
 * passes a pointer to it as the kernel entry point's only argument:
 *
 *     void kernel_main(BootInfo *info);      // System V ABI
 *
 * Only fixed-size types are used here, so the kernel never depends on
 * the EFI headers. Both sides must be rebuilt when this file changes. */
#ifndef BOOT_INFO_H
#define BOOT_INFO_H

#include <stdint.h>

#define BOOT_INFO_MAGIC  0x4D4F4C4F53424F4FULL   /* "MOLOSBOO" */

typedef struct {
  uint64_t base;            /* physical address of the framebuffer */
  uint64_t size;            /* size in bytes */
  uint32_t width;           /* visible pixels per line */
  uint32_t height;          /* number of lines */
  uint32_t pixels_per_line; /* stride; may be larger than width */
  uint32_t pixel_format;    /* EFI_GRAPHICS_PIXEL_FORMAT from GOP */
} BootFramebuffer;

/* One entry of the memory map, same layout as EFI_MEMORY_DESCRIPTOR.
 * Entries are BootMemoryMap.descriptor_size bytes apart, which may be
 * larger than sizeof(BootMemoryDescriptor): always step by that size. */
typedef struct {
  uint32_t type;            /* EFI_MEMORY_TYPE, see BOOT_MEM_* below */
  uint32_t pad;
  uint64_t physical_start;
  uint64_t virtual_start;
  uint64_t number_of_pages; /* 4 KiB pages */
  uint64_t attribute;
} BootMemoryDescriptor;

enum {
  BOOT_MEM_RESERVED,
  BOOT_MEM_LOADER_CODE,
  BOOT_MEM_LOADER_DATA,
  BOOT_MEM_BOOT_SERVICES_CODE,
  BOOT_MEM_BOOT_SERVICES_DATA,
  BOOT_MEM_RUNTIME_SERVICES_CODE,
  BOOT_MEM_RUNTIME_SERVICES_DATA,
  BOOT_MEM_CONVENTIONAL,
  BOOT_MEM_UNUSABLE,
  BOOT_MEM_ACPI_RECLAIM,
  BOOT_MEM_ACPI_NVS,
  BOOT_MEM_MMIO,
  BOOT_MEM_MMIO_PORT_SPACE,
  BOOT_MEM_PAL_CODE,
  BOOT_MEM_PERSISTENT,
  BOOT_MEM_UNACCEPTED,
};

typedef struct {
  uint64_t map;             /* physical address of the EFI memory map */
  uint64_t map_size;        /* total size in bytes */
  uint64_t descriptor_size; /* size of one descriptor (not sizeof!) */
  uint32_t descriptor_version;
  uint32_t reserved;
} BootMemoryMap;

typedef struct {
  uint64_t        magic;    /* BOOT_INFO_MAGIC */
  BootFramebuffer framebuffer;
  BootMemoryMap   memory_map;
  uint64_t        rsdp;     /* physical address of the ACPI RSDP, 0 if none */
} BootInfo;

#endif /* BOOT_INFO_H */
