/* base types */
typedef unsigned long long  UINT64;
typedef unsigned int        UINT32;
typedef unsigned short      CHAR16;
typedef unsigned char       BOOLEAN;
typedef UINT64              UINTN;
typedef UINTN               EFI_STATUS;
typedef void               *EFI_HANDLE;
typedef UINTN               EFI_TPL;


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

//typedefs for boot services

typedef EFI_TPL (EFIAPI *EFI_RAISE_TPL)(IN EFI_TPL NewTpl);
typedef void (EFIAPI *EFI_RESTORE_TPL) (IN EFI_TPL OldTpl);
//******************************************************
//EFI_ALLOCATE_TYPE
//******************************************************
// These types are discussed in the "Description" section below.
typedef enum {
   AllocateAnyPages,
   AllocateMaxAddress,
   AllocateAddress,
   MaxAllocateType
} EFI_ALLOCATE_TYPE;

//******************************************************
//EFI_MEMORY_TYPE
//******************************************************
// These type values are discussed in Memory Type Usage before ExitBootServices()  and  Memory Type Usage after ExitBootServices().
typedef enum {
   EfiReservedMemoryType,
   EfiLoaderCode,
   EfiLoaderData,
   EfiBootServicesCode,
   EfiBootServicesData,
   EfiRuntimeServicesCode,
   EfiRuntimeServicesData,
   EfiConventionalMemory,
   EfiUnusableMemory,
   EfiACPIReclaimMemory,
   EfiACPIMemoryNVS,
   EfiMemoryMappedIO,
   EfiMemoryMappedIOPortSpace,
   EfiPalCode,
   EfiPersistentMemory,
   EfiUnacceptedMemoryType,
   EfiMaxMemoryType
} EFI_MEMORY_TYPE;

//******************************************************
//EFI_PHYSICAL_ADDRESS
//******************************************************
typedef UINT64 EFI_PHYSICAL_ADDRESS;

typedef EFI_STATUS (EFIAPI *EFI_ALLOCATE_PAGES) (
   IN EFI_ALLOCATE_TYPE                   Type,
   IN EFI_MEMORY_TYPE                     MemoryType,
   IN UINTN                               Pages,
   IN OUT EFI_PHYSICAL_ADDRESS            *Memory
   );

//******************************************************
// Supporting types
//******************************************************
typedef void                VOID;
typedef unsigned char       UINT8;
typedef unsigned short      UINT16;
typedef UINT64              EFI_VIRTUAL_ADDRESS;
typedef void               *EFI_EVENT;

typedef struct {
  UINT32  Data1;
  UINT16  Data2;
  UINT16  Data3;
  UINT8   Data4[8];
} EFI_GUID;

typedef struct {
  UINT32                Type;
  EFI_PHYSICAL_ADDRESS  PhysicalStart;
  EFI_VIRTUAL_ADDRESS   VirtualStart;
  UINT64                NumberOfPages;
  UINT64                Attribute;
} EFI_MEMORY_DESCRIPTOR;

typedef struct {
  UINT8  Type;
  UINT8  SubType;
  UINT8  Length[2];
} EFI_DEVICE_PATH_PROTOCOL;

typedef enum {
  TimerCancel,
  TimerPeriodic,
  TimerRelative
} EFI_TIMER_DELAY;

typedef enum {
  EFI_NATIVE_INTERFACE
} EFI_INTERFACE_TYPE;

typedef enum {
  AllHandles,
  ByRegisterNotify,
  ByProtocol
} EFI_LOCATE_SEARCH_TYPE;

typedef struct {
  EFI_HANDLE  AgentHandle;
  EFI_HANDLE  ControllerHandle;
  UINT32      Attributes;
  UINT32      OpenCount;
} EFI_OPEN_PROTOCOL_INFORMATION_ENTRY;

typedef VOID (EFIAPI *EFI_EVENT_NOTIFY)(
   IN EFI_EVENT                           Event,
   IN VOID                                *Context
   );

//******************************************************
// Memory Services
//******************************************************
typedef EFI_STATUS (EFIAPI *EFI_FREE_PAGES) (
   IN EFI_PHYSICAL_ADDRESS                Memory,
   IN UINTN                               Pages
   );

typedef EFI_STATUS (EFIAPI *EFI_GET_MEMORY_MAP) (
   IN OUT UINTN                           *MemoryMapSize,
   OUT EFI_MEMORY_DESCRIPTOR              *MemoryMap,
   OUT UINTN                              *MapKey,
   OUT UINTN                              *DescriptorSize,
   OUT UINT32                             *DescriptorVersion
   );

typedef EFI_STATUS (EFIAPI *EFI_ALLOCATE_POOL) (
   IN EFI_MEMORY_TYPE                     PoolType,
   IN UINTN                               Size,
   OUT VOID                               **Buffer
   );

typedef EFI_STATUS (EFIAPI *EFI_FREE_POOL) (
   IN VOID                                *Buffer
   );

//******************************************************
// Event & Timer Services
//******************************************************
typedef EFI_STATUS (EFIAPI *EFI_CREATE_EVENT) (
   IN UINT32                              Type,
   IN EFI_TPL                             NotifyTpl,
   IN EFI_EVENT_NOTIFY                    NotifyFunction,   /* OPTIONAL */
   IN VOID                                *NotifyContext,   /* OPTIONAL */
   OUT EFI_EVENT                          *Event
   );

typedef EFI_STATUS (EFIAPI *EFI_SET_TIMER) (
   IN EFI_EVENT                           Event,
   IN EFI_TIMER_DELAY                     Type,
   IN UINT64                              TriggerTime
   );

typedef EFI_STATUS (EFIAPI *EFI_WAIT_FOR_EVENT) (
   IN UINTN                               NumberOfEvents,
   IN EFI_EVENT                           *Event,
   OUT UINTN                              *Index
   );

typedef EFI_STATUS (EFIAPI *EFI_SIGNAL_EVENT) (
   IN EFI_EVENT                           Event
   );

typedef EFI_STATUS (EFIAPI *EFI_CLOSE_EVENT) (
   IN EFI_EVENT                           Event
   );

typedef EFI_STATUS (EFIAPI *EFI_CHECK_EVENT) (
   IN EFI_EVENT                           Event
   );

typedef EFI_STATUS (EFIAPI *EFI_CREATE_EVENT_EX) (
   IN UINT32                              Type,
   IN EFI_TPL                             NotifyTpl,
   IN EFI_EVENT_NOTIFY                    NotifyFunction,   /* OPTIONAL */
   IN const VOID                          *NotifyContext,   /* OPTIONAL */
   IN const EFI_GUID                      *EventGroup,      /* OPTIONAL */
   OUT EFI_EVENT                          *Event
   );

//******************************************************
// Protocol Handler Services
//******************************************************
typedef EFI_STATUS (EFIAPI *EFI_INSTALL_PROTOCOL_INTERFACE) (
   IN OUT EFI_HANDLE                      *Handle,
   IN EFI_GUID                            *Protocol,
   IN EFI_INTERFACE_TYPE                  InterfaceType,
   IN VOID                                *Interface
   );

typedef EFI_STATUS (EFIAPI *EFI_REINSTALL_PROTOCOL_INTERFACE) (
   IN EFI_HANDLE                          Handle,
   IN EFI_GUID                            *Protocol,
   IN VOID                                *OldInterface,
   IN VOID                                *NewInterface
   );

typedef EFI_STATUS (EFIAPI *EFI_UNINSTALL_PROTOCOL_INTERFACE) (
   IN EFI_HANDLE                          Handle,
   IN EFI_GUID                            *Protocol,
   IN VOID                                *Interface
   );

typedef EFI_STATUS (EFIAPI *EFI_HANDLE_PROTOCOL) (
   IN EFI_HANDLE                          Handle,
   IN EFI_GUID                            *Protocol,
   OUT VOID                               **Interface
   );

typedef EFI_STATUS (EFIAPI *EFI_REGISTER_PROTOCOL_NOTIFY) (
   IN EFI_GUID                            *Protocol,
   IN EFI_EVENT                           Event,
   OUT VOID                               **Registration
   );

typedef EFI_STATUS (EFIAPI *EFI_LOCATE_HANDLE) (
   IN EFI_LOCATE_SEARCH_TYPE              SearchType,
   IN EFI_GUID                            *Protocol,        /* OPTIONAL */
   IN VOID                                *SearchKey,       /* OPTIONAL */
   IN OUT UINTN                           *BufferSize,
   OUT EFI_HANDLE                         *Buffer
   );

typedef EFI_STATUS (EFIAPI *EFI_LOCATE_DEVICE_PATH) (
   IN EFI_GUID                            *Protocol,
   IN OUT EFI_DEVICE_PATH_PROTOCOL        **DevicePath,
   OUT EFI_HANDLE                         *Device
   );

typedef EFI_STATUS (EFIAPI *EFI_INSTALL_CONFIGURATION_TABLE) (
   IN EFI_GUID                            *Guid,
   IN VOID                                *Table
   );

//******************************************************
// Image Services
//******************************************************
typedef EFI_STATUS (EFIAPI *EFI_IMAGE_LOAD) (
   IN BOOLEAN                             BootPolicy,
   IN EFI_HANDLE                          ParentImageHandle,
   IN EFI_DEVICE_PATH_PROTOCOL            *DevicePath,      /* OPTIONAL */
   IN VOID                                *SourceBuffer,    /* OPTIONAL */
   IN UINTN                               SourceSize,
   OUT EFI_HANDLE                         *ImageHandle
   );

typedef EFI_STATUS (EFIAPI *EFI_IMAGE_START) (
   IN EFI_HANDLE                          ImageHandle,
   OUT UINTN                              *ExitDataSize,
   OUT CHAR16                             **ExitData        /* OPTIONAL */
   );

typedef EFI_STATUS (EFIAPI *EFI_EXIT) (
   IN EFI_HANDLE                          ImageHandle,
   IN EFI_STATUS                          ExitStatus,
   IN UINTN                               ExitDataSize,
   IN CHAR16                              *ExitData         /* OPTIONAL */
   );

typedef EFI_STATUS (EFIAPI *EFI_IMAGE_UNLOAD) (
   IN EFI_HANDLE                          ImageHandle
   );

typedef EFI_STATUS (EFIAPI *EFI_EXIT_BOOT_SERVICES) (
   IN EFI_HANDLE                          ImageHandle,
   IN UINTN                               MapKey
   );

//******************************************************
// Miscellaneous Services
//******************************************************
typedef EFI_STATUS (EFIAPI *EFI_GET_NEXT_MONOTONIC_COUNT) (
   OUT UINT64                             *Count
   );

typedef EFI_STATUS (EFIAPI *EFI_STALL) (
   IN UINTN                               Microseconds
   );

typedef EFI_STATUS (EFIAPI *EFI_SET_WATCHDOG_TIMER) (
   IN UINTN                               Timeout,
   IN UINT64                              WatchdogCode,
   IN UINTN                               DataSize,
   IN CHAR16                              *WatchdogData     /* OPTIONAL */
   );

typedef VOID (EFIAPI *EFI_COPY_MEM) (
   IN VOID                                *Destination,
   IN VOID                                *Source,
   IN UINTN                               Length
   );

typedef VOID (EFIAPI *EFI_SET_MEM) (
   IN VOID                                *Buffer,
   IN UINTN                               Size,
   IN UINT8                               Value
   );

typedef EFI_STATUS (EFIAPI *EFI_CALCULATE_CRC32) (
   IN VOID                                *Data,
   IN UINTN                               DataSize,
   OUT UINT32                             *Crc32
   );

//******************************************************
// Driver Support Services
//******************************************************
typedef EFI_STATUS (EFIAPI *EFI_CONNECT_CONTROLLER) (
   IN EFI_HANDLE                          ControllerHandle,
   IN EFI_HANDLE                          *DriverImageHandle,   /* OPTIONAL */
   IN EFI_DEVICE_PATH_PROTOCOL            *RemainingDevicePath, /* OPTIONAL */
   IN BOOLEAN                             Recursive
   );

typedef EFI_STATUS (EFIAPI *EFI_DISCONNECT_CONTROLLER) (
   IN EFI_HANDLE                          ControllerHandle,
   IN EFI_HANDLE                          DriverImageHandle,    /* OPTIONAL */
   IN EFI_HANDLE                          ChildHandle           /* OPTIONAL */
   );

//******************************************************
// Open and Close Protocol Services
//******************************************************
typedef EFI_STATUS (EFIAPI *EFI_OPEN_PROTOCOL) (
   IN EFI_HANDLE                          Handle,
   IN EFI_GUID                            *Protocol,
   OUT VOID                               **Interface,      /* OPTIONAL */
   IN EFI_HANDLE                          AgentHandle,
   IN EFI_HANDLE                          ControllerHandle,
   IN UINT32                              Attributes
   );

typedef EFI_STATUS (EFIAPI *EFI_CLOSE_PROTOCOL) (
   IN EFI_HANDLE                          Handle,
   IN EFI_GUID                            *Protocol,
   IN EFI_HANDLE                          AgentHandle,
   IN EFI_HANDLE                          ControllerHandle
   );

typedef EFI_STATUS (EFIAPI *EFI_OPEN_PROTOCOL_INFORMATION) (
   IN EFI_HANDLE                          Handle,
   IN EFI_GUID                            *Protocol,
   OUT EFI_OPEN_PROTOCOL_INFORMATION_ENTRY **EntryBuffer,
   OUT UINTN                              *EntryCount
   );

//******************************************************
// Library Services
//******************************************************
typedef EFI_STATUS (EFIAPI *EFI_PROTOCOLS_PER_HANDLE) (
   IN EFI_HANDLE                          Handle,
   OUT EFI_GUID                           ***ProtocolBuffer,
   OUT UINTN                              *ProtocolBufferCount
   );

typedef EFI_STATUS (EFIAPI *EFI_LOCATE_HANDLE_BUFFER) (
   IN EFI_LOCATE_SEARCH_TYPE              SearchType,
   IN EFI_GUID                            *Protocol,        /* OPTIONAL */
   IN VOID                                *SearchKey,       /* OPTIONAL */
   OUT UINTN                              *NoHandles,
   OUT EFI_HANDLE                         **Buffer
   );

typedef EFI_STATUS (EFIAPI *EFI_LOCATE_PROTOCOL) (
   IN EFI_GUID                            *Protocol,
   IN VOID                                *Registration,    /* OPTIONAL */
   OUT VOID                               **Interface
   );

typedef EFI_STATUS (EFIAPI *EFI_INSTALL_MULTIPLE_PROTOCOL_INTERFACES) (
   IN OUT EFI_HANDLE                      *Handle,
   ...
   );

typedef EFI_STATUS (EFIAPI *EFI_UNINSTALL_MULTIPLE_PROTOCOL_INTERFACES) (
   IN EFI_HANDLE                          Handle,
   ...
   );

struct EFI_BOOT_SERVICES {
  EFI_TABLE_HEADER     Hdr;

  //
  // Task Priority Services
  //
  EFI_RAISE_TPL        RaiseTPL;       // EFI 1.0+
  EFI_RESTORE_TPL      RestoreTPL;     // EFI 1.0+

    //
    // Memory Services
    //
    EFI_ALLOCATE_PAGES   AllocatePages;  // EFI 1.0+
    EFI_FREE_PAGES       FreePages;      // EFI 1.0+
    EFI_GET_MEMORY_MAP   GetMemoryMap;   // EFI 1.0+
    EFI_ALLOCATE_POOL    AllocatePool;   // EFI 1.0+
    EFI_FREE_POOL        FreePool;       // EFI 1.0+

    //
    // Event & Timer Services
    //
    EFI_CREATE_EVENT     CreateEvent;    // EFI 1.0+
    EFI_SET_TIMER        SetTimer;       // EFI 1.0+
    EFI_WAIT_FOR_EVENT   WaitForEvent;   // EFI 1.0+
    EFI_SIGNAL_EVENT     SignalEvent;    // EFI 1.0+
    EFI_CLOSE_EVENT      CloseEvent;     // EFI 1.0+
    EFI_CHECK_EVENT      CheckEvent;     // EFI 1.0+

    //
    // Protocol Handler Services
    //
    EFI_INSTALL_PROTOCOL_INTERFACE     InstallProtocolInterface;            // EFI 1.0+
    EFI_REINSTALL_PROTOCOL_INTERFACE   ReinstallProtocolInterface;          // EFI 1.0+
    EFI_UNINSTALL_PROTOCOL_INTERFACE   UninstallProtocolInterface;          // EFI 1.0+
    EFI_HANDLE_PROTOCOL                HandleProtocol;                      // EFI 1.0+
 VOID*   Reserved;    // EFI 1.0+
    EFI_REGISTER_PROTOCOL_NOTIFY       RegisterProtocolNotify;              // EFI  1.0+
    EFI_LOCATE_HANDLE                  LocateHandle;                        // EFI 1.0+
    EFI_LOCATE_DEVICE_PATH             LocateDevicePath;                    // EFI 1.0+
 EFI_INSTALL_CONFIGURATION_TABLE       InstallConfigurationTable;           // EFI 1.0+

    //
    // Image Services
    //
    EFI_IMAGE_LOAD                 LoadImage;        // EFI 1.0+
    EFI_IMAGE_START                StartImage;       // EFI 1.0+
    EFI_EXIT                       Exit;             // EFI 1.0+
    EFI_IMAGE_UNLOAD               UnloadImage;      // EFI 1.0+
    EFI_EXIT_BOOT_SERVICES         ExitBootServices; // EFI 1.0+

    //
    // Miscellaneous Services
    //
    EFI_GET_NEXT_MONOTONIC_COUNT   GetNextMonotonicCount; // EFI 1.0+
    EFI_STALL                      Stall;                 // EFI 1.0+
    EFI_SET_WATCHDOG_TIMER         SetWatchdogTimer;      // EFI 1.0+

    //
    // DriverSupport Services
    //
    EFI_CONNECT_CONTROLLER         ConnectController;     // EFI 1.1
    EFI_DISCONNECT_CONTROLLER      DisconnectController;  // EFI 1.1+

    //
    // Open and Close Protocol Services
    //
    EFI_OPEN_PROTOCOL              OpenProtocol;           // EFI 1.1+
    EFI_CLOSE_PROTOCOL             CloseProtocol;          // EFI 1.1+
 EFI_OPEN_PROTOCOL_INFORMATION     OpenProtocolInformation;// EFI 1.1+

    //
    // Library Services
    //
    EFI_PROTOCOLS_PER_HANDLE       ProtocolsPerHandle;     // EFI 1.1+
    EFI_LOCATE_HANDLE_BUFFER       LocateHandleBuffer;     // EFI 1.1+
    EFI_LOCATE_PROTOCOL            LocateProtocol;         // EFI 1.1+
  EFI_INSTALL_MULTIPLE_PROTOCOL_INTERFACES    InstallMultipleProtocolInterfaces;    // EFI 1.1+
  EFI_UNINSTALL_MULTIPLE_PROTOCOL_INTERFACES UninstallMultipleProtocolInterfaces;   // EFI 1.1+*

    //
    // 32-bit CRC Services
    //
    EFI_CALCULATE_CRC32    CalculateCrc32;     // EFI 1.1+

    //
    // Miscellaneous Services
    //
    EFI_COPY_MEM           CopyMem;        // EFI 1.1+
    EFI_SET_MEM            SetMem;         // EFI 1.1+
    EFI_CREATE_EVENT_EX    CreateEventEx;  // UEFI 2.0+
  };

_Static_assert(sizeof(EFI_SIMPLE_TEXT_OUTPUT_PROTOCOL) == 80, "TEXT_OUTPUT layout wrong");

/* Loaded Image Protocol */
typedef struct {
  UINT32                    Revision;
  EFI_HANDLE                ParentHandle;
  EFI_SYSTEM_TABLE          *SystemTable;
  EFI_HANDLE                DeviceHandle;
  EFI_DEVICE_PATH_PROTOCOL  *FilePath;
  VOID                      *Reserved;
  UINT32                    LoadOptionsSize;
  VOID                      *LoadOptions;
  VOID                      *ImageBase;
  UINT64                    ImageSize;
  EFI_MEMORY_TYPE           ImageCodeType;
  EFI_MEMORY_TYPE           ImageDataType;
  EFI_IMAGE_UNLOAD          Unload;
} EFI_LOADED_IMAGE_PROTOCOL;

static EFI_GUID gEfiLoadedImageProtocolGuid =
  { 0x5B1B31A1, 0x9562, 0x11d2, { 0x8E, 0x3F, 0x00, 0xA0, 0xC9, 0x69, 0x72, 0x3B } };

static EFI_SYSTEM_TABLE  *gST;
static EFI_HANDLE         gImageHandle;
static EFI_BOOT_SERVICES *gBS;

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
      gST->ConOut->OutputString(gST->ConOut, (CHAR16 *)((UINT8 *)node + 4));

    if (length < 4)
      break;    /* malformed node, avoid looping forever */
    node = (EFI_DEVICE_PATH_PROTOCOL *)((UINT8 *)node + length);
  }
}

EFI_STATUS EFIAPI efi_main(IN EFI_HANDLE ImageHandle, IN EFI_SYSTEM_TABLE *SystemTable)
{
  gImageHandle = ImageHandle;
  gST          = SystemTable;
  if (gST->Hdr.Signature != 0x5453595320494249ULL)
    return 1;   // not a valid system table
  gBS = gST->BootServices;

  gST->ConOut->Reset(gST->ConOut, 0);
  gST->ConOut->OutputString(gST->ConOut, L"Loading Molos 1.0.0\r\n");

  //get the loaded image
  EFI_LOADED_IMAGE_PROTOCOL *loaded_image = 0;
  EFI_STATUS status = gBS->HandleProtocol(gImageHandle, &gEfiLoadedImageProtocolGuid, (VOID**)&loaded_image);
  if (status != 0) {
    gST->ConOut->OutputString(gST->ConOut, L"Failed to get loaded image\r\n");
    return status;
  }

  gST->ConOut->OutputString(gST->ConOut, L"File path: ");
  if (loaded_image->FilePath)
    print_file_path(loaded_image->FilePath);
  else
    gST->ConOut->OutputString(gST->ConOut, L"(none)");
  gST->ConOut->OutputString(gST->ConOut, L"\r\n");

  for (;;) { }
  return 0;
}