#include "efi.h"
#include "elf.h"
#include "boot_info.h"

static EFI_GUID gEfiLoadedImageProtocolGuid      = EFI_LOADED_IMAGE_PROTOCOL_GUID;
static EFI_GUID gEfiSimpleFileSystemProtocolGuid = EFI_SIMPLE_FILE_SYSTEM_PROTOCOL_GUID;
static EFI_GUID gEfiFileInfoGuid                 = EFI_FILE_INFO_ID;
static EFI_GUID gEfiGraphicsOutputProtocolGuid   = EFI_GRAPHICS_OUTPUT_PROTOCOL_GUID;
static EFI_GUID gEfiAcpi20TableGuid              = EFI_ACPI_20_TABLE_GUID;

/* The kernel is an ELF built for the System V ABI, while this bootloader
 * uses the Microsoft ABI by default, so the call must say which one. */
typedef void (__attribute__((sysv_abi)) *KERNEL_ENTRY)(BootInfo *info);

#define PAGE_SIZE 4096ULL

static EFI_SYSTEM_TABLE  *gST;
static EFI_HANDLE         gImageHandle;
static EFI_BOOT_SERVICES *gBS;

/* ---------------------------------------------------------------- output */

static void print(CHAR16 *s)
{
  gST->ConOut->OutputString(gST->ConOut, s);
}

static void print_hex(UINT64 value)
{
  CHAR16 buf[19];                      /* "0x" + 16 digits + NUL */
  buf[0] = L'0';
  buf[1] = L'x';
  for (int i = 0; i < 16; i++) {
    UINT8 nibble = (value >> ((15 - i) * 4)) & 0xF;
    buf[2 + i] = nibble < 10 ? L'0' + nibble : L'A' + (nibble - 10);
  }
  buf[18] = 0;
  print(buf);
}

/* Print ASCII bytes: convert to CHAR16 in small chunks, "\n" -> "\r\n". */
static void print_ascii(UINT8 *buffer, UINTN size)
{
  CHAR16 out[65];                      /* room for "\r\n" expansion + NUL */
  UINTN  n = 0;
  for (UINTN i = 0; i < size; i++) {
    if (buffer[i] == '\n' && (i == 0 || buffer[i - 1] != '\r'))
      out[n++] = L'\r';
    out[n++] = (CHAR16)buffer[i];
    if (n >= 62) {                     /* flush before the chunk fills up */
      out[n] = 0;
      print(out);
      n = 0;
    }
  }
  out[n] = 0;
  print(out);
}

/* Print the file name parts of a device path.
 * A device path is a list of variable-length nodes; each starts with
 * Type, SubType and a 2-byte Length. The list ends with Type 0x7F.
 * File path nodes (Type 4 = media, SubType 4 = file path) hold a
 * NUL-terminated CHAR16 string right after the 4-byte header. */
static void print_file_path(EFI_DEVICE_PATH_PROTOCOL *node)
{
  while (node->Type != 0x7F) {
    UINT16 length = node->Length[0] | (node->Length[1] << 8);

    if (node->Type == 4 && node->SubType == 4)
      print((CHAR16 *)((UINT8 *)node + 4));

    if (length < 4)
      break;    /* malformed node, avoid looping forever */
    node = (EFI_DEVICE_PATH_PROTOCOL *)((UINT8 *)node + length);
  }
}

/* ----------------------------------------------------------------- files */

/* Read a whole file into a pool buffer. On success the caller owns
 * *out_buffer (free it with FreePool; it is 0 for an empty file). */
static EFI_STATUS read_file(EFI_FILE_PROTOCOL *root, CHAR16 *path,
                            UINT8 **out_buffer, UINTN *out_size)
{
  EFI_FILE_PROTOCOL *file = 0;
  EFI_STATUS status = root->Open(root, &file, path, EFI_FILE_MODE_READ, 0);
  if (status != EFI_SUCCESS)
    return status;

  /* get the file size: first call with size 0 to learn how big EFI_FILE_INFO is */
  UINTN info_size = 0;
  EFI_FILE_INFO *info = 0;
  status = file->GetInfo(file, &gEfiFileInfoGuid, &info_size, 0);
  if (status == EFI_BUFFER_TOO_SMALL) {
    status = gBS->AllocatePool(EfiLoaderData, info_size, (VOID **)&info);
    if (status == EFI_SUCCESS)
      status = file->GetInfo(file, &gEfiFileInfoGuid, &info_size, info);
  }
  if (status != EFI_SUCCESS) {
    if (info)
      gBS->FreePool(info);
    file->Close(file);
    return status;
  }
  UINTN size = info->FileSize;         /* in: buffer size, out: bytes read */
  gBS->FreePool(info);

  /* allocate exactly enough for the whole file */
  UINT8 *buffer = 0;
  if (size > 0) {
    status = gBS->AllocatePool(EfiLoaderData, size, (VOID **)&buffer);
    if (status == EFI_SUCCESS) {
      status = file->Read(file, &size, buffer);
      if (status != EFI_SUCCESS) {
        gBS->FreePool(buffer);
        buffer = 0;
      }
    }
  }
  file->Close(file);
  if (status != EFI_SUCCESS)
    return status;

  *out_buffer = buffer;
  *out_size   = size;
  return EFI_SUCCESS;
}

/* ---------------------------------------------------------------- kernel */

/* Copy every PT_LOAD segment of an ELF64 file to its physical address
 * (p_paddr) and zero the part not stored in the file (.bss).
 * The memory is reserved with AllocatePages(AllocateAddress) so the
 * firmware will not hand it out to anyone else. */
static EFI_STATUS load_kernel(UINT8 *data, UINTN size, UINT64 *entry)
{
  Elf64_Ehdr *eh = (Elf64_Ehdr *)data;

  if (size < sizeof(Elf64_Ehdr) ||
      eh->e_ident[0] != 0x7F || eh->e_ident[1] != 'E' ||
      eh->e_ident[2] != 'L'  || eh->e_ident[3] != 'F' ||
      eh->e_ident[EI_CLASS] != ELFCLASS64 ||
      eh->e_ident[EI_DATA]  != ELFDATA2LSB ||
      eh->e_type != ET_EXEC || eh->e_machine != EM_X86_64 ||
      eh->e_phentsize != sizeof(Elf64_Phdr) ||
      eh->e_phoff > size ||
      (UINT64)eh->e_phnum * sizeof(Elf64_Phdr) > size - eh->e_phoff) {
    print(L"kernel.elf is not a valid x86-64 ELF executable\r\n");
    return EFI_LOAD_ERROR;
  }

  Elf64_Phdr *ph = (Elf64_Phdr *)(data + eh->e_phoff);

  /* find the physical range covered by all loadable segments */
  UINT64 lo = ~0ULL, hi = 0;
  for (UINT16 i = 0; i < eh->e_phnum; i++) {
    if (ph[i].p_type != PT_LOAD || ph[i].p_memsz == 0)
      continue;
    if (ph[i].p_filesz > ph[i].p_memsz ||
        ph[i].p_offset > size || ph[i].p_filesz > size - ph[i].p_offset) {
      print(L"kernel.elf has a malformed segment\r\n");
      return EFI_LOAD_ERROR;
    }
    if (ph[i].p_paddr < lo)
      lo = ph[i].p_paddr;
    if (ph[i].p_paddr + ph[i].p_memsz > hi)
      hi = ph[i].p_paddr + ph[i].p_memsz;
  }
  if (hi == 0) {
    print(L"kernel.elf has no loadable segments\r\n");
    return EFI_LOAD_ERROR;
  }
  lo &= ~(PAGE_SIZE - 1);                       /* round down to a page */
  hi = (hi + PAGE_SIZE - 1) & ~(PAGE_SIZE - 1); /* round up to a page */

  if (eh->e_entry < lo || eh->e_entry >= hi) {
    print(L"kernel.elf entry point is outside its segments\r\n");
    return EFI_LOAD_ERROR;
  }

  EFI_PHYSICAL_ADDRESS base = lo;
  EFI_STATUS status = gBS->AllocatePages(AllocateAddress, EfiLoaderCode,
                                         (hi - lo) / PAGE_SIZE, &base);
  if (status != EFI_SUCCESS) {
    print(L"Could not reserve memory for the kernel at ");
    print_hex(lo);
    print(L"\r\n");
    return status;
  }

  gBS->SetMem((VOID *)lo, hi - lo, 0);          /* zeroes .bss as well */
  for (UINT16 i = 0; i < eh->e_phnum; i++) {
    if (ph[i].p_type == PT_LOAD && ph[i].p_filesz > 0)
      gBS->CopyMem((VOID *)ph[i].p_paddr, data + ph[i].p_offset, ph[i].p_filesz);
  }

  print(L"Kernel loaded at ");
  print_hex(lo);
  print(L" - ");
  print_hex(hi);
  print(L", entry ");
  print_hex(eh->e_entry);
  print(L"\r\n");

  *entry = eh->e_entry;
  return EFI_SUCCESS;
}

/* ------------------------------------------------------------- boot info */

static void get_framebuffer(BootFramebuffer *fb)
{
  EFI_GRAPHICS_OUTPUT_PROTOCOL *gop = 0;
  EFI_STATUS status = gBS->LocateProtocol(&gEfiGraphicsOutputProtocolGuid, 0, (VOID **)&gop);
  if (status != EFI_SUCCESS || !gop) {
    print(L"No graphics output, kernel gets no framebuffer\r\n");
    return;
  }

  EFI_GRAPHICS_OUTPUT_MODE_INFORMATION *mode_info = gop->Mode->Info;
  if (mode_info->PixelFormat == PixelBltOnly) {
    print(L"Graphics output has no framebuffer (BltOnly)\r\n");
    return;
  }

  fb->base            = gop->Mode->FrameBufferBase;
  fb->size            = gop->Mode->FrameBufferSize;
  fb->width           = mode_info->HorizontalResolution;
  fb->height          = mode_info->VerticalResolution;
  fb->pixels_per_line = mode_info->PixelsPerScanLine;
  fb->pixel_format    = mode_info->PixelFormat;
}

static BOOLEAN guid_equal(EFI_GUID *a, EFI_GUID *b)
{
  if (a->Data1 != b->Data1 || a->Data2 != b->Data2 || a->Data3 != b->Data3)
    return 0;
  for (int i = 0; i < 8; i++)
    if (a->Data4[i] != b->Data4[i])
      return 0;
  return 1;
}

/* The ACPI 2.0+ RSDP is published in the system configuration table. */
static UINT64 find_rsdp(void)
{
  for (UINTN i = 0; i < gST->NumberOfTableEntries; i++) {
    if (guid_equal(&gST->ConfigurationTable[i].VendorGuid, &gEfiAcpi20TableGuid))
      return (UINT64)gST->ConfigurationTable[i].VendorTable;
  }
  return 0;
}

/* Get the final memory map and leave boot services.
 * ExitBootServices only succeeds with the map key of the *current* map;
 * if anything changed the map in between, it fails and we must fetch the
 * map again. After a failed attempt no allocations or console output are
 * allowed, so the buffer is allocated up front with some spare room. */
static EFI_STATUS exit_boot_services(BootMemoryMap *mm)
{
  UINTN  map_size = 0, map_key = 0, desc_size = 0;
  UINT32 desc_version = 0;
  EFI_MEMORY_DESCRIPTOR *map = 0;

  EFI_STATUS status = gBS->GetMemoryMap(&map_size, 0, &map_key, &desc_size, &desc_version);
  if (status != EFI_BUFFER_TOO_SMALL)
    return status;

  /* the allocation itself can add entries to the map, so leave room */
  UINTN buffer_size = map_size + 8 * desc_size;
  status = gBS->AllocatePool(EfiLoaderData, buffer_size, (VOID **)&map);
  if (status != EFI_SUCCESS)
    return status;

  for (int attempt = 0; attempt < 2; attempt++) {
    map_size = buffer_size;
    status = gBS->GetMemoryMap(&map_size, map, &map_key, &desc_size, &desc_version);
    if (status != EFI_SUCCESS)
      return status;

    status = gBS->ExitBootServices(gImageHandle, map_key);
    if (status == EFI_SUCCESS) {
      mm->map                = (UINT64)map;
      mm->map_size           = map_size;
      mm->descriptor_size    = desc_size;
      mm->descriptor_version = desc_version;
      return EFI_SUCCESS;
    }
  }
  return status;
}

/* ------------------------------------------------------------------ main */

EFI_STATUS EFIAPI efi_main(IN EFI_HANDLE ImageHandle, IN EFI_SYSTEM_TABLE *SystemTable)
{
  gImageHandle = ImageHandle;
  gST          = SystemTable;
  if (gST->Hdr.Signature != 0x5453595320494249ULL)
    return 1;   // not a valid system table
  gBS = gST->BootServices;

  gST->ConOut->Reset(gST->ConOut, 0);
  print(L"Loading Molos 1.0.0\r\n");

  //get the loaded image
  EFI_LOADED_IMAGE_PROTOCOL *loaded_image = 0;
  EFI_STATUS status = gBS->HandleProtocol(gImageHandle, &gEfiLoadedImageProtocolGuid, (VOID**)&loaded_image);
  if (status != EFI_SUCCESS) {
    print(L"Failed to get loaded image\r\n");
    return status;
  }

  print(L"File path: ");
  if (loaded_image->FilePath)
    print_file_path(loaded_image->FilePath);
  else
    print(L"(none)");
  print(L"\r\n");

  //open the volume we were loaded from
  EFI_SIMPLE_FILE_SYSTEM_PROTOCOL *fs = 0;
  status = gBS->HandleProtocol(loaded_image->DeviceHandle, &gEfiSimpleFileSystemProtocolGuid, (VOID **)&fs);
  if (status != EFI_SUCCESS || !fs) {
    print(L"Failed to get file system protocol\r\n");
    return status ? status : 1;
  }

  EFI_FILE_PROTOCOL *root = 0;
  status = fs->OpenVolume(fs, &root);
  if (status != EFI_SUCCESS) {
    print(L"Failed to open volume\r\n");
    return status;
  }

  //print hello.txt
  UINT8 *buffer = 0;
  UINTN  size   = 0;
  status = read_file(root, L"\\hello.txt", &buffer, &size);
  if (status == EFI_SUCCESS) {
    print(L"Contents of \\hello.txt:\r\n");
    print_ascii(buffer, size);
    print(L"\r\n");
    if (buffer)
      gBS->FreePool(buffer);
  } else {
    print(L"Failed to read \\hello.txt\r\n");
  }

  //read and load the kernel
  UINT8 *kernel_file = 0;
  UINTN  kernel_size = 0;
  status = read_file(root, L"\\kernel.elf", &kernel_file, &kernel_size);
  root->Close(root);
  if (status != EFI_SUCCESS || !kernel_file) {
    print(L"Failed to read \\kernel.elf\r\n");
    return status ? status : EFI_LOAD_ERROR;
  }

  UINT64 kernel_entry = 0;
  status = load_kernel(kernel_file, kernel_size, &kernel_entry);
  gBS->FreePool(kernel_file);          /* segments are copied, file no longer needed */
  if (status != EFI_SUCCESS)
    return status;

  //collect what the kernel needs while boot services still exist
  BootInfo *info = 0;
  status = gBS->AllocatePool(EfiLoaderData, sizeof(BootInfo), (VOID **)&info);
  if (status != EFI_SUCCESS) {
    print(L"Failed to allocate boot info\r\n");
    return status;
  }
  gBS->SetMem(info, sizeof(BootInfo), 0);
  info->magic = BOOT_INFO_MAGIC;
  get_framebuffer(&info->framebuffer);
  info->rsdp = find_rsdp();

  print(L"Framebuffer ");
  print_hex(info->framebuffer.base);
  print(L", RSDP ");
  print_hex(info->rsdp);
  print(L"\r\nExiting boot services and starting kernel...\r\n");

  //from here on no UEFI boot services, no console output
  status = exit_boot_services(&info->memory_map);
  if (status != EFI_SUCCESS) {
    for (;;)
      __asm__ volatile ("cli; hlt");   /* can't report it any more */
  }

  KERNEL_ENTRY kernel_main = (KERNEL_ENTRY)kernel_entry;
  kernel_main(info);

  for (;;)                             /* the kernel should never return */
    __asm__ volatile ("cli; hlt");
  return 0;
}
