#include "boot_info.h"

/* Kernel entry point. The bootloader jumps here after ExitBootServices,
 * so no UEFI services are available: everything we know about the machine
 * comes from *info. */
void kernel_main(BootInfo *info)
{
  if (info && info->magic == BOOT_INFO_MAGIC && info->framebuffer.base) {
    /* fill the screen with one colour so we can see the kernel is running */
    volatile uint32_t *fb = (volatile uint32_t *)info->framebuffer.base;
    for (uint32_t y = 0; y < info->framebuffer.height; y++)
      for (uint32_t x = 0; x < info->framebuffer.width; x++)
        fb[y * info->framebuffer.pixels_per_line + x] = 0x0090EE90;  /* light green */
  }

  for (;;)
    __asm__ volatile ("cli; hlt");
}
