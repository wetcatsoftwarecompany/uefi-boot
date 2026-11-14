/* Minimal UEFI type definitions (subset of the UEFI specification). */
#ifndef EFI_H
#define EFI_H

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

#define EFI_LOADED_IMAGE_PROTOCOL_GUID \
  { 0x5B1B31A1, 0x9562, 0x11d2, { 0x8E, 0x3F, 0x00, 0xA0, 0xC9, 0x69, 0x72, 0x3B } }
#define EFI_SIMPLE_FILE_SYSTEM_PROTOCOL_GUID \
  { 0x0964e5b22, 0x6459, 0x11d2, { 0x8e, 0x39, 0x00, 0xa0, 0xc9, 0x69, 0x72, 0x3b } }

/* Simple File System Protocol */
typedef struct EFI_SIMPLE_FILE_SYSTEM_PROTOCOL EFI_SIMPLE_FILE_SYSTEM_PROTOCOL;
typedef struct EFI_FILE_PROTOCOL               EFI_FILE_PROTOCOL;

typedef EFI_STATUS (EFIAPI *EFI_SIMPLE_FILE_SYSTEM_PROTOCOL_OPEN_VOLUME) (
   IN EFI_SIMPLE_FILE_SYSTEM_PROTOCOL     *This,
   OUT EFI_FILE_PROTOCOL                  **Root
   );

/* File Protocol */
#define EFI_FILE_MODE_READ    0x0000000000000001ULL
typedef EFI_STATUS (EFIAPI *EFI_FILE_OPEN) (
   IN EFI_FILE_PROTOCOL                   *This,
   OUT EFI_FILE_PROTOCOL                  **NewHandle,
   IN CHAR16                              *FileName,
   IN UINT64                              OpenMode,
   IN UINT64                              Attributes
   );

typedef EFI_STATUS (EFIAPI *EFI_FILE_CLOSE) (
   IN EFI_FILE_PROTOCOL                   *This
   );

typedef EFI_STATUS (EFIAPI *EFI_FILE_READ) (
   IN EFI_FILE_PROTOCOL                   *This,
   IN OUT UINTN                           *BufferSize,
   OUT VOID                               *Buffer
   );

typedef EFI_STATUS (EFIAPI *EFI_FILE_GET_INFO) (
   IN EFI_FILE_PROTOCOL                   *This,
   IN EFI_GUID                            *InformationType,
   IN OUT UINTN                           *BufferSize,
   OUT VOID                               *Buffer
   );

/* File Info */
typedef short               INT16;

typedef struct {
  UINT16  Year;         /* 1900 - 9999 */
  UINT8   Month;        /* 1 - 12 */
  UINT8   Day;          /* 1 - 31 */
  UINT8   Hour;         /* 0 - 23 */
  UINT8   Minute;       /* 0 - 59 */
  UINT8   Second;       /* 0 - 59 */
  UINT8   Pad1;
  UINT32  Nanosecond;   /* 0 - 999,999,999 */
  INT16   TimeZone;     /* -1440 to 1440 or 2047 */
  UINT8   Daylight;
  UINT8   Pad2;
} EFI_TIME;

typedef struct {
  UINT64    Size;               /* size of this struct including FileName */
  UINT64    FileSize;           /* file size in bytes */
  UINT64    PhysicalSize;       /* bytes used on disk */
  EFI_TIME  CreateTime;
  EFI_TIME  LastAccessTime;
  EFI_TIME  ModificationTime;
  UINT64    Attribute;
  CHAR16    FileName[];         /* NUL-terminated, variable length */
} EFI_FILE_INFO;

#define EFI_FILE_INFO_ID \
  { 0x09576e92, 0x6d3f, 0x11d2, { 0x8e, 0x39, 0x00, 0xa0, 0xc9, 0x69, 0x72, 0x3b } }

#define EFI_BUFFER_TOO_SMALL  0x8000000000000005ULL

struct EFI_FILE_PROTOCOL {
  UINT64           Revision;
  EFI_FILE_OPEN    Open;
  EFI_FILE_CLOSE   Close;
  void            *Delete;             /* placeholders until you need them */
  EFI_FILE_READ    Read;
  void            *Write;
  void            *GetPosition;
  void            *SetPosition;
  EFI_FILE_GET_INFO GetInfo;
  void            *SetInfo;
  void            *Flush;
};

  struct EFI_SIMPLE_FILE_SYSTEM_PROTOCOL {
    UINT64                                        Revision;
    EFI_SIMPLE_FILE_SYSTEM_PROTOCOL_OPEN_VOLUME   OpenVolume;
};

/* Status codes (high bit set = error) */
#define EFI_SUCCESS            0
#define EFI_LOAD_ERROR         0x8000000000000001ULL
#define EFI_INVALID_PARAMETER  0x8000000000000002ULL

/* Configuration Table (system table entries such as ACPI) */
struct EFI_CONFIGURATION_TABLE {
  EFI_GUID  VendorGuid;
  VOID      *VendorTable;
};

#define EFI_ACPI_20_TABLE_GUID \
  { 0x8868e871, 0xe4f1, 0x11d3, { 0xbc, 0x22, 0x00, 0x80, 0xc7, 0x3c, 0x88, 0x81 } }

/* Graphics Output Protocol */
#define EFI_GRAPHICS_OUTPUT_PROTOCOL_GUID \
  { 0x9042a9de, 0x23dc, 0x4a38, { 0x96, 0xfb, 0x7a, 0xde, 0xd0, 0x80, 0x51, 0x6a } }

typedef enum {
  PixelRedGreenBlueReserved8BitPerColor,
  PixelBlueGreenRedReserved8BitPerColor,
  PixelBitMask,
  PixelBltOnly,                        /* no direct framebuffer access */
  PixelFormatMax
} EFI_GRAPHICS_PIXEL_FORMAT;

typedef struct {
  UINT32  RedMask;
  UINT32  GreenMask;
  UINT32  BlueMask;
  UINT32  ReservedMask;
} EFI_PIXEL_BITMASK;

typedef struct {
  UINT32                     Version;
  UINT32                     HorizontalResolution;
  UINT32                     VerticalResolution;
  EFI_GRAPHICS_PIXEL_FORMAT  PixelFormat;
  EFI_PIXEL_BITMASK          PixelInformation;
  UINT32                     PixelsPerScanLine;
} EFI_GRAPHICS_OUTPUT_MODE_INFORMATION;

typedef struct {
  UINT32                                MaxMode;
  UINT32                                Mode;
  EFI_GRAPHICS_OUTPUT_MODE_INFORMATION  *Info;
  UINTN                                 SizeOfInfo;
  EFI_PHYSICAL_ADDRESS                  FrameBufferBase;
  UINTN                                 FrameBufferSize;
} EFI_GRAPHICS_OUTPUT_PROTOCOL_MODE;

typedef struct {
  void                               *QueryMode;   /* placeholders until you need them */
  void                               *SetMode;
  void                               *Blt;
  EFI_GRAPHICS_OUTPUT_PROTOCOL_MODE  *Mode;
} EFI_GRAPHICS_OUTPUT_PROTOCOL;

#endif /* EFI_H */
