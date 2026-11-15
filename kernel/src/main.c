#include "boot_info.h"
#include "serial.h"

static const char *memory_type_name(uint32_t type)
{
  switch (type) {
  case BOOT_MEM_RESERVED:              return "Reserved";
  case BOOT_MEM_LOADER_CODE:           return "LoaderCode";
  case BOOT_MEM_LOADER_DATA:           return "LoaderData";
  case BOOT_MEM_BOOT_SERVICES_CODE:    return "BootServicesCode";
  case BOOT_MEM_BOOT_SERVICES_DATA:    return "BootServicesData";
  case BOOT_MEM_RUNTIME_SERVICES_CODE: return "RuntimeServicesCode";
  case BOOT_MEM_RUNTIME_SERVICES_DATA: return "RuntimeServicesData";
  case BOOT_MEM_CONVENTIONAL:          return "Conventional";
  case BOOT_MEM_UNUSABLE:              return "Unusable";
  case BOOT_MEM_ACPI_RECLAIM:          return "ACPIReclaim";
  case BOOT_MEM_ACPI_NVS:              return "ACPINVS";
  case BOOT_MEM_MMIO:                  return "MMIO";
  case BOOT_MEM_MMIO_PORT_SPACE:       return "MMIOPortSpace";
  case BOOT_MEM_PAL_CODE:              return "PalCode";
  case BOOT_MEM_PERSISTENT:            return "Persistent";
  case BOOT_MEM_UNACCEPTED:            return "Unaccepted";
  default:                             return "Unknown";
  }
}

/* Dump the memory map handed over by the bootloader to the serial port. */
static void print_memory_map(BootMemoryMap *mm)
{
  uint64_t count = mm->descriptor_size ? mm->map_size / mm->descriptor_size : 0;
  uint64_t free_pages = 0;

  serial_puts("Memory map: ");
  serial_dec(count);
  serial_puts(" entries, descriptor size ");
  serial_dec(mm->descriptor_size);
  serial_puts("\n");
  serial_puts("  #   Type                 Start               End (excl.)         Pages       Attribute\n");

  for (uint64_t i = 0; i < count; i++) {
    /* step by descriptor_size, NOT sizeof(BootMemoryDescriptor) */
    BootMemoryDescriptor *d =
      (BootMemoryDescriptor *)(mm->map + i * mm->descriptor_size);
    uint64_t end = d->physical_start + d->number_of_pages * 4096;

    serial_puts("  ");
    if (i < 10)  serial_putc(' ');
    if (i < 100) serial_putc(' ');
    serial_dec(i);
    serial_putc(' ');
    serial_pad(memory_type_name(d->type), 21);
    serial_hex(d->physical_start);
    serial_putc(' ');
    serial_hex(end);
    serial_putc(' ');
    serial_dec(d->number_of_pages);
    serial_puts("\t");
    serial_hex(d->attribute);
    serial_puts("\n");

    /* free for the kernel to use now that boot services are gone */
    if (d->type == BOOT_MEM_CONVENTIONAL ||
        d->type == BOOT_MEM_BOOT_SERVICES_CODE ||
        d->type == BOOT_MEM_BOOT_SERVICES_DATA)
      free_pages += d->number_of_pages;
  }

  serial_puts("Usable memory: ");
  serial_dec(free_pages * 4096 / (1024 * 1024));
  serial_puts(" MiB (Conventional + BootServices)\n");
}

/* Kernel entry point. The bootloader jumps here after ExitBootServices,
 * so no UEFI services are available: everything we know about the machine
 * comes from *info. */
void kernel_main(BootInfo *info)
{
  serial_init();
  serial_puts("\nMolos kernel started\n");

  if (!info || info->magic != BOOT_INFO_MAGIC) {
    serial_puts("Invalid boot info, halting\n");
    for (;;)
      __asm__ volatile ("cli; hlt");
  }

  print_memory_map(&info->memory_map);

  if (info->framebuffer.base) {
    /* fill the screen with one colour so we can see the kernel is running */
    volatile uint32_t *fb = (volatile uint32_t *)info->framebuffer.base;
    for (uint32_t y = 0; y < info->framebuffer.height; y++)
      for (uint32_t x = 0; x < info->framebuffer.width; x++)
        fb[y * info->framebuffer.pixels_per_line + x] = 0x0090EE90;  /* light green */
  }

  serial_puts("Halting\n");
  for (;;)
    __asm__ volatile ("cli; hlt");
}
