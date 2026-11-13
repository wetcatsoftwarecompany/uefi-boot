/* base types */
typedef unsigned long long  UINT64;
typedef unsigned int        UINT32;
typedef unsigned short      CHAR16;
typedef unsigned char       BOOLEAN;
typedef UINT64              UINTN;
typedef UINTN               EFI_STATUS;
typedef void               *EFI_HANDLE;

#define IN
#define OUT
#define EFIAPI __attribute__((ms_abi))

/* forward declarations */
typedef struct EFI_SIMPLE_TEXT_INPUT_PROTOCOL  EFI_SIMPLE_TEXT_INPUT_PROTOCOL;
typedef struct EFI_SIMPLE_TEXT_OUTPUT_PROTOCOL EFI_SIMPLE_TEXT_OUTPUT_PROTOCOL;
typedef struct EFI_RUNTIME_SERVICES            EFI_RUNTIME_SERVICES;
typedef struct EFI_BOOT_SERVICES               EFI_BOOT_SERVICES;
typedef struct EFI_CONFIGURATION_TABLE         EFI_CONFIGURATION_TABLE;

typedef struct {
  UINT64      Signature;
  UINT32      Revision;
  UINT32      HeaderSize;
  UINT32      CRC32;
  UINT32      Reserved;
} EFI_TABLE_HEADER;

typedef struct {
  EFI_TABLE_HEADER                 Hdr;
  CHAR16                           *FirmwareVendor;
  UINT32                           FirmwareRevision;
  EFI_HANDLE                       ConsoleInHandle;
  EFI_SIMPLE_TEXT_INPUT_PROTOCOL   *ConIn;
  EFI_HANDLE                       ConsoleOutHandle;
  EFI_SIMPLE_TEXT_OUTPUT_PROTOCOL  *ConOut;
  EFI_HANDLE                       StandardErrorHandle;
  EFI_SIMPLE_TEXT_OUTPUT_PROTOCOL  *StdErr;
  EFI_RUNTIME_SERVICES             *RuntimeServices;
  EFI_BOOT_SERVICES                *BootServices;
  UINTN                            NumberOfTableEntries;
  EFI_CONFIGURATION_TABLE          *ConfigurationTable;
} EFI_SYSTEM_TABLE;

_Static_assert(sizeof(EFI_SYSTEM_TABLE) == 120, "EFI_SYSTEM_TABLE layout wrong");

/* Simple Text Output Protocol */
typedef EFI_STATUS (EFIAPI *EFI_TEXT_RESET)(
  IN EFI_SIMPLE_TEXT_OUTPUT_PROTOCOL *This,
  IN BOOLEAN                         ExtendedVerification);

typedef EFI_STATUS (EFIAPI *EFI_TEXT_STRING)(
  IN EFI_SIMPLE_TEXT_OUTPUT_PROTOCOL *This,
  IN CHAR16                          *String);

struct EFI_SIMPLE_TEXT_OUTPUT_PROTOCOL {
  EFI_TEXT_RESET   Reset;
  EFI_TEXT_STRING  OutputString;
  void            *TestString;         /* placeholders until you need them */
  void            *QueryMode;
  void            *SetMode;
  void            *SetAttribute;
  void            *ClearScreen;
  void            *SetCursorPosition;
  void            *EnableCursor;
  void            *Mode;
};

_Static_assert(sizeof(EFI_SIMPLE_TEXT_OUTPUT_PROTOCOL) == 80, "TEXT_OUTPUT layout wrong");

static EFI_SYSTEM_TABLE  *gST;
static EFI_HANDLE         gImageHandle;
static EFI_BOOT_SERVICES *gBS;

EFI_STATUS EFIAPI efi_main(IN EFI_HANDLE ImageHandle, IN EFI_SYSTEM_TABLE *SystemTable)
{
  gImageHandle = ImageHandle;
  gST          = SystemTable;
  if (gST->Hdr.Signature != 0x5453595320494249ULL)
    return 1;   // not a valid system table
  gBS = gST->BootServices;

  gST->ConOut->Reset(gST->ConOut, 0);
  gST->ConOut->OutputString(gST->ConOut, L"Loading Molos 1.0.0!\r\n");

  for (;;) { }
  return 0;
}