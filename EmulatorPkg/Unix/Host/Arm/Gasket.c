/**@file Gasket.c

  Manages any differences between the UNIX ABI and the EFI/Windows ABI
  For ARM systems the AAPCS
  (https://github.com/ARM-software/abi-aa/blob/main/aapcs32/aapcs32.rst)
  has mostly standardized the ABI across different operating systems. There are
  some differences in argument passing for variadic functions and passing of
  enum values as arguments. Neither of these cases apply for
  the EMU_THUNK_PROTOCOL so we can use a simple wrapper functions.

Copyright (c) 2004 - 2023, Intel Corporation. All rights reserved.<BR>
SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#include "GasketFunctionDefinitions.h"

//
// Gasket functions for the top-level EMU_THUNK_PROTOCOL
//

UINTN
EFIAPI
GasketSecWriteStdErr (
  IN UINT8  *Buffer,
  IN UINTN  NumberOfBytes
  )
{
  return SecWriteStdErr (Buffer, NumberOfBytes);
}

EFI_STATUS
EFIAPI
GasketSecConfigStdIn (
  VOID
  )
{
  return SecConfigStdIn ();
}

UINTN
EFIAPI
GasketSecWriteStdOut (
  IN UINT8  *Buffer,
  IN UINTN  NumberOfBytes
  )
{
  return SecWriteStdOut (Buffer, NumberOfBytes);
}

UINTN
EFIAPI
GasketSecReadStdIn (
  IN UINT8  *Buffer,
  IN UINTN  NumberOfBytes
  )
{
  return SecReadStdIn (Buffer, NumberOfBytes);
}

BOOLEAN
EFIAPI
GasketSecPollStdIn (
  VOID
  )
{
  return SecPollStdIn ();
}

VOID *
EFIAPI
GasketSecMalloc (
  IN  UINTN  Size
  )
{
  return SecMalloc (Size);
}

VOID *
EFIAPI
GasketSecValloc (
  IN  UINTN  Size
  )
{
  return SecValloc (Size);
}

BOOLEAN
EFIAPI
GasketSecFree (
  IN  VOID  *Ptr
  )
{
  return SecFree (Ptr);
}

RETURN_STATUS
EFIAPI
GasketSecPeCoffGetEntryPoint (
  IN     VOID  *Pe32Data,
  IN OUT VOID  **EntryPoint
  )
{
  return SecPeCoffGetEntryPoint (Pe32Data, EntryPoint);
}

VOID
EFIAPI
GasketSecPeCoffRelocateImageExtraAction (
  IN OUT PE_COFF_LOADER_IMAGE_CONTEXT  *ImageContext
  )
{
  SecPeCoffRelocateImageExtraAction (ImageContext);
}

VOID
EFIAPI
GasketSecPeCoffUnloadImageExtraAction (
  IN OUT PE_COFF_LOADER_IMAGE_CONTEXT  *ImageContext
  )
{
  SecPeCoffUnloadImageExtraAction (ImageContext);
}

VOID
EFIAPI
GasketSecEnableInterrupt (
  VOID
  )
{
  SecEnableInterrupt ();
}

VOID
EFIAPI
GasketSecDisableInterrupt (
  VOID
  )
{
  SecDisableInterrupt ();
}

UINT64
EFIAPI
GasketQueryPerformanceFrequency (
  VOID
  )
{
  return QueryPerformanceFrequency ();
}

UINT64
EFIAPI
GasketQueryPerformanceCounter (
  VOID
  )
{
  return QueryPerformanceCounter ();
}

VOID
EFIAPI
GasketSecSleep (
  IN  UINT64  Nanoseconds
  )
{
  SecSleep (Nanoseconds);
}

VOID
EFIAPI
GasketSecCpuSleep (
  VOID
  )
{
  SecCpuSleep ();
}

VOID
EFIAPI
GasketSecExit (
  UINTN  Status
  )
{
  SecExit (Status);
}

VOID
EFIAPI
GasketSecGetTime (
  OUT  EFI_TIME              *Time,
  OUT EFI_TIME_CAPABILITIES  *Capabilities OPTIONAL
  )
{
  SecGetTime (Time, Capabilities);
}

VOID
EFIAPI
GasketSecSetTime (
  IN  EFI_TIME  *Time
  )
{
  SecSetTime (Time);
}

VOID
EFIAPI
GasketSecSetTimer (
  IN  UINT64                  PeriodMs,
  IN  EMU_SET_TIMER_CALLBACK  CallBack
  )
{
  SecSetTimer (PeriodMs, CallBack);
}

EFI_STATUS
EFIAPI
GasketSecSetMemoryAttributes (
  IN  EFI_PHYSICAL_ADDRESS              BaseAddress,
  IN  UINT64                            Length,
  IN  UINT64                            Attributes
  )
{
  return SecSetMemoryAttributes (BaseAddress, Length, Attributes);
}

EFI_STATUS
EFIAPI
GasketSecGetNextProtocol (
  IN  BOOLEAN                EmuBusDriver,
  OUT EMU_IO_THUNK_PROTOCOL  **Instance  OPTIONAL
  )
{
  return SecGetNextProtocol (EmuBusDriver, Instance);
}

//
// Gasket functions for PPIs produced by SEC
//

EFI_STATUS
EFIAPI
GasketSecUnixPeiAutoScan (
  IN  UINTN                 Index,
  OUT EFI_PHYSICAL_ADDRESS  *MemoryBase,
  OUT UINT64                *MemorySize
  )
{
  return SecUnixPeiAutoScan (Index, MemoryBase, MemorySize);
}

EFI_STATUS
EFIAPI
GasketSecUnixFdAddress (
  IN     UINTN                 Index,
  IN OUT EFI_PHYSICAL_ADDRESS  *FdBase,
  IN OUT UINT64                *FdSize,
  IN OUT EFI_PHYSICAL_ADDRESS  *FixUp
  )
{
  return SecUnixFdAddress (Index, FdBase, FdSize, FixUp);
}

VOID *
EFIAPI
GasketSecEmuThunkAddress (
  VOID
  )
{
  return SecEmuThunkAddress ();
}

//
// Reverse (UNIX to EFIAPI) gaskets
//

UINTN
ReverseGasketUint64 (
  UINTN   CallBack,
  UINT64  a
  )
{
  CALL_BACK   Function;

  Function = (CALL_BACK)(VOID *) CallBack;
  Function (a);
  return CallBack;
}

UINTN
ReverseGasketUint64Uint64 (
  VOID  *CallBack,
  VOID  *Context,
  VOID  *Key
  )
{
  CALL_BACK_2ARG  Function;

  Function = (CALL_BACK_2ARG) CallBack;
  Function (Context, Key);
  return (UINTN) CallBack;
}

//
// Gasket functions for EMU_GRAPHICS_WINDOW_PROTOCOL
//

EFI_STATUS
EFIAPI
GasketX11Size (
  EMU_GRAPHICS_WINDOW_PROTOCOL  *GraphicsWindowsIo,
  UINT32                        Width,
  UINT32                        Height
  )
{
  return X11Size (GraphicsWindowsIo, Width, Height);
}

EFI_STATUS
EFIAPI
GasketX11CheckKey (
  EMU_GRAPHICS_WINDOW_PROTOCOL  *GraphicsWindowsIo
  )
{
  return X11CheckKey (GraphicsWindowsIo);
}

EFI_STATUS
EFIAPI
GasketX11GetKey (
  EMU_GRAPHICS_WINDOW_PROTOCOL  *GraphicsWindowsIo,
  EFI_KEY_DATA                  *key
  )
{
  return X11GetKey (GraphicsWindowsIo, key);
}

EFI_STATUS
EFIAPI
GasketX11KeySetState (
  EMU_GRAPHICS_WINDOW_PROTOCOL  *GraphicsWindowsIo,
  EFI_KEY_TOGGLE_STATE          *KeyToggleState
  )
{
  return X11KeySetState (GraphicsWindowsIo, KeyToggleState);
}

EFI_STATUS
EFIAPI
GasketX11RegisterKeyNotify (
  IN EMU_GRAPHICS_WINDOW_PROTOCOL                      *GraphicsWindowsIo,
  IN EMU_GRAPHICS_WINDOW_REGISTER_KEY_NOTIFY_CALLBACK  MakeCallBack,
  IN EMU_GRAPHICS_WINDOW_REGISTER_KEY_NOTIFY_CALLBACK  BreakCallBack,
  IN VOID                                              *Context
  )
{
  return X11RegisterKeyNotify (GraphicsWindowsIo, MakeCallBack, BreakCallBack, Context);
}

EFI_STATUS
EFIAPI
GasketX11Blt (
  IN  EMU_GRAPHICS_WINDOW_PROTOCOL    *GraphicsWindows,
  IN  EFI_UGA_PIXEL                   *BltBuffer OPTIONAL,
  IN  EFI_UGA_BLT_OPERATION           BltOperation,
  IN  EMU_GRAPHICS_WINDOWS__BLT_ARGS  *Args
  )
{
  return X11Blt (GraphicsWindows, BltBuffer, BltOperation, Args);
}

EFI_STATUS
EFIAPI
GasketX11CheckPointer (
  EMU_GRAPHICS_WINDOW_PROTOCOL  *GraphicsWindowsIo
  )
{
  return X11CheckPointer (GraphicsWindowsIo);
}

EFI_STATUS
EFIAPI
GasketX11GetPointerState (
  EMU_GRAPHICS_WINDOW_PROTOCOL  *GraphicsWindowsIo,
  EFI_SIMPLE_POINTER_STATE      *state
  )
{
  return X11GetPointerState (GraphicsWindowsIo, state);
}

EFI_STATUS
EFIAPI
GasketX11GraphicsWindowOpen (
  IN  EMU_IO_THUNK_PROTOCOL  *This
  )
{
  return X11GraphicsWindowOpen (This);
}

EFI_STATUS
EFIAPI
GasketX11GraphicsWindowClose (
  IN  EMU_IO_THUNK_PROTOCOL  *This
  )
{
  return X11GraphicsWindowClose (This);
}

//
// Gasket functions for accessing Pthreads
//

UINTN
EFIAPI
GasketPthreadMutexLock (
  IN VOID  *Mutex
  )
{
  return PthreadMutexLock (Mutex);
}

UINTN
EFIAPI
GasketPthreadMutexUnLock (
  IN VOID  *Mutex
  )
{
  return PthreadMutexUnLock (Mutex);
}

UINTN
EFIAPI
GasketPthreadMutexTryLock (
  IN VOID  *Mutex
  )
{
  return PthreadMutexTryLock (Mutex);
}

VOID *
EFIAPI
GasketPthreadMutexInit (
  IN VOID
  )
{
  return PthreadMutexInit ();
}

UINTN
EFIAPI
GasketPthreadMutexDestroy (
  IN VOID  *Mutex
  )
{
  return PthreadMutexDestroy (Mutex);
}

UINTN
EFIAPI
GasketPthreadCreate (
  IN  VOID                       *Thread,
  IN  VOID                       *Attribute,
  IN  THREAD_THUNK_THREAD_ENTRY  Start,
  IN  VOID                       *Context
  )
{
  return PthreadCreate (Thread, Attribute, Start, Context);
}

VOID
EFIAPI
GasketPthreadExit (
  IN VOID  *ValuePtr
  )
{
  PthreadExit (ValuePtr);
}

UINTN
EFIAPI
GasketPthreadSelf (
  VOID
  )
{
  return PthreadSelf ();
}

EFI_STATUS
EFIAPI
GasketPthreadOpen (
  IN  EMU_IO_THUNK_PROTOCOL  *This
  )
{
  return PthreadOpen (This);
}

EFI_STATUS
EFIAPI
GasketPthreadClose (
  IN  EMU_IO_THUNK_PROTOCOL  *This
  )
{
  return PthreadClose (This);
}

//
// Gasket functions for accessing the host filesystem
//

EFI_STATUS
EFIAPI
GasketPosixOpenVolume (
  IN EFI_SIMPLE_FILE_SYSTEM_PROTOCOL  *This,
  OUT EFI_FILE_PROTOCOL               **Root
  )
{
  return PosixOpenVolume (This, Root);
}

EFI_STATUS
EFIAPI
GasketPosixFileOpen (
  IN EFI_FILE_PROTOCOL   *This,
  OUT EFI_FILE_PROTOCOL  **NewHandle,
  IN CHAR16              *FileName,
  IN UINT64              OpenMode,
  IN UINT64              Attributes
  )
{
  return PosixFileOpen (This, NewHandle, FileName, OpenMode, Attributes);
}

EFI_STATUS
EFIAPI
GasketPosixFileClose (
  IN EFI_FILE_PROTOCOL  *This
  )
{
  return PosixFileClose (This);
}

EFI_STATUS
EFIAPI
GasketPosixFileDelete (
  IN EFI_FILE_PROTOCOL  *This
  )
{
  return PosixFileDelete (This);
}

EFI_STATUS
EFIAPI
GasketPosixFileRead (
  IN EFI_FILE_PROTOCOL  *This,
  IN OUT UINTN          *BufferSize,
  OUT VOID              *Buffer
  )
{
  return PosixFileRead (This, BufferSize, Buffer);
}

EFI_STATUS
EFIAPI
GasketPosixFileWrite (
  IN EFI_FILE_PROTOCOL  *This,
  IN OUT UINTN          *BufferSize,
  IN VOID               *Buffer
  )
{
  return PosixFileWrite (This, BufferSize, Buffer);
}

EFI_STATUS
EFIAPI
GasketPosixFileSetPosition (
  IN EFI_FILE_PROTOCOL  *This,
  IN UINT64             Position
  )
{
  return PosixFileSetPosition (This, Position);
}

EFI_STATUS
EFIAPI
GasketPosixFileGetPosition (
  IN EFI_FILE_PROTOCOL  *This,
  OUT UINT64            *Position
  )
{
  return PosixFileGetPosition (This, Position);
}

EFI_STATUS
EFIAPI
GasketPosixFileGetInfo (
  IN EFI_FILE_PROTOCOL  *This,
  IN EFI_GUID           *InformationType,
  IN OUT UINTN          *BufferSize,
  OUT VOID              *Buffer
  )
{
  return PosixFileGetInfo (This, InformationType, BufferSize, Buffer);
}

EFI_STATUS
EFIAPI
GasketPosixFileSetInfo (
  IN EFI_FILE_PROTOCOL  *This,
  IN EFI_GUID           *InformationType,
  IN UINTN              BufferSize,
  IN VOID               *Buffer
  )
{
  return PosixFileSetInfo (This, InformationType, BufferSize, Buffer);
}

EFI_STATUS
EFIAPI
GasketPosixFileFlush (
  IN EFI_FILE_PROTOCOL  *This
  )
{
  return PosixFileFlush (This);
}

EFI_STATUS
EFIAPI
GasketPosixFileSystemThunkOpen (
  IN  EMU_IO_THUNK_PROTOCOL  *This
  )
{
  return PosixFileSystemThunkOpen (This);
}

EFI_STATUS
EFIAPI
GasketPosixFileSystemThunkClose (//@todo
  IN  EMU_IO_THUNK_PROTOCOL  *This
  )
{
  return PosixFileSystemThunkClose (This);
}

//
// Gasket functions for reading/writing to disk images
//

EFI_STATUS
EFIAPI
GasketEmuBlockIoReset (
  IN EMU_BLOCK_IO_PROTOCOL  *This,
  IN BOOLEAN                ExtendedVerification
  )
{
  return EmuBlockIoReset (This, ExtendedVerification);
}

EFI_STATUS
EFIAPI
GasketEmuBlockIoReadBlocks (
  IN     EMU_BLOCK_IO_PROTOCOL  *This,
  IN     UINT32                 MediaId,
  IN     EFI_LBA                LBA,
  IN OUT EFI_BLOCK_IO2_TOKEN    *Token,
  IN     UINTN                  BufferSize,
  OUT VOID                      *Buffer
  )
{
  return EmuBlockIoReadBlocks (This, MediaId, LBA, Token, BufferSize, Buffer);
}

EFI_STATUS
EFIAPI
GasketEmuBlockIoWriteBlocks (
  IN     EMU_BLOCK_IO_PROTOCOL  *This,
  IN     UINT32                 MediaId,
  IN     EFI_LBA                LBA,
  IN OUT EFI_BLOCK_IO2_TOKEN    *Token,
  IN     UINTN                  BufferSize,
  IN     VOID                   *Buffer
  )
{
  return EmuBlockIoWriteBlocks (This, MediaId, LBA, Token, BufferSize, Buffer);
}

EFI_STATUS
EFIAPI
GasketEmuBlockIoFlushBlocks (
  IN     EMU_BLOCK_IO_PROTOCOL  *This,
  IN OUT EFI_BLOCK_IO2_TOKEN    *Token
  )
{
  return EmuBlockIoFlushBlocks (This, Token);
}

EFI_STATUS
EFIAPI
GasketEmuBlockIoCreateMapping (
  IN     EMU_BLOCK_IO_PROTOCOL  *This,
  IN     EFI_BLOCK_IO_MEDIA     *Media
  )
{
  return EmuBlockIoCreateMapping (This, Media);
}

EFI_STATUS
EFIAPI
GasketBlockIoThunkOpen (
  IN  EMU_IO_THUNK_PROTOCOL  *This
  )
{
  return EmuBlockIoThunkOpen (This);
}

EFI_STATUS
EFIAPI
GasketBlockIoThunkClose (
  IN  EMU_IO_THUNK_PROTOCOL  *This
  )
{
  return EmuBlockIoThunkClose (This);
}

//
// Gasket functions for accessing BSD Sockets (TCP-IP Networks)
//

EFI_STATUS
EFIAPI
GasketSnpCreateMapping (
  IN     EMU_SNP_PROTOCOL         *This,
  IN     EFI_SIMPLE_NETWORK_MODE  *Media
  )
{
  return EmuSnpCreateMapping (This, Media);
}

EFI_STATUS
EFIAPI
GasketSnpStart (
  IN EMU_SNP_PROTOCOL  *This
  )
{
  return EmuSnpStart (This);
}

EFI_STATUS
EFIAPI
GasketSnpStop (
  IN EMU_SNP_PROTOCOL  *This
  )
{
  return EmuSnpStop (This);
}

EFI_STATUS
EFIAPI
GasketSnpInitialize (
  IN EMU_SNP_PROTOCOL  *This,
  IN UINTN             ExtraRxBufferSize  OPTIONAL,
  IN UINTN             ExtraTxBufferSize  OPTIONAL
  )
{
  return EmuSnpInitialize (This, ExtraRxBufferSize, ExtraTxBufferSize);
}

EFI_STATUS
EFIAPI
GasketSnpReset (
  IN EMU_SNP_PROTOCOL  *This,
  IN BOOLEAN           ExtendedVerification
  )
{
  return EmuSnpReset (This, ExtendedVerification);
}

EFI_STATUS
EFIAPI
GasketSnpShutdown (
  IN EMU_SNP_PROTOCOL  *This
  )
{
  return EmuSnpShutdown (This);
}

EFI_STATUS
EFIAPI
GasketSnpReceiveFilters (
  IN EMU_SNP_PROTOCOL  *This,
  IN UINT32            Enable,
  IN UINT32            Disable,
  IN BOOLEAN           ResetMCastFilter,
  IN UINTN             MCastFilterCnt     OPTIONAL,
  IN EFI_MAC_ADDRESS   *MCastFilter OPTIONAL
  )
{
  return EmuSnpReceiveFilters (This, Enable, Disable, ResetMCastFilter, MCastFilterCnt, MCastFilter);
}

EFI_STATUS
EFIAPI
GasketSnpStationAddress (
  IN EMU_SNP_PROTOCOL  *This,
  IN BOOLEAN           Reset,
  IN EFI_MAC_ADDRESS   *New OPTIONAL
  )
{
  return EmuSnpStationAddress (This, Reset, New);
}

EFI_STATUS
EFIAPI
GasketSnpStatistics (
  IN EMU_SNP_PROTOCOL         *This,
  IN BOOLEAN                  Reset,
  IN OUT UINTN                *StatisticsSize   OPTIONAL,
  OUT EFI_NETWORK_STATISTICS  *StatisticsTable  OPTIONAL
  )
{
  return EmuSnpStatistics (This, Reset, StatisticsSize, StatisticsTable);
}

EFI_STATUS
EFIAPI
GasketSnpMCastIpToMac (
  IN EMU_SNP_PROTOCOL  *This,
  IN BOOLEAN           IPv6,
  IN EFI_IP_ADDRESS    *IP,
  OUT EFI_MAC_ADDRESS  *MAC
  )
{
  return EmuSnpMCastIpToMac (This, IPv6, IP, MAC);
}

EFI_STATUS
EFIAPI
GasketSnpNvData (
  IN EMU_SNP_PROTOCOL  *This,
  IN BOOLEAN           ReadWrite,
  IN UINTN             Offset,
  IN UINTN             BufferSize,
  IN OUT VOID          *Buffer
  )
{
  return EmuSnpNvData (This, ReadWrite, Offset, BufferSize, Buffer);
}

EFI_STATUS
EFIAPI
GasketSnpGetStatus (
  IN EMU_SNP_PROTOCOL  *This,
  OUT UINT32           *InterruptStatus OPTIONAL,
  OUT VOID             **TxBuf OPTIONAL
  )
{
  return EmuSnpGetStatus (This, InterruptStatus, TxBuf);
}

EFI_STATUS
EFIAPI
GasketSnpTransmit (
  IN EMU_SNP_PROTOCOL  *This,
  IN UINTN             HeaderSize,
  IN UINTN             BufferSize,
  IN VOID              *Buffer,
  IN EFI_MAC_ADDRESS   *SrcAddr  OPTIONAL,
  IN EFI_MAC_ADDRESS   *DestAddr OPTIONAL,
  IN UINT16            *Protocol OPTIONAL
  )
{
  return EmuSnpTransmit (This, HeaderSize, BufferSize, Buffer, SrcAddr, DestAddr, Protocol);
}

EFI_STATUS
EFIAPI
GasketSnpReceive (
  IN EMU_SNP_PROTOCOL  *This,
  OUT UINTN            *HeaderSize OPTIONAL,
  IN OUT UINTN         *BufferSize,
  OUT VOID             *Buffer,
  OUT EFI_MAC_ADDRESS  *SrcAddr    OPTIONAL,
  OUT EFI_MAC_ADDRESS  *DestAddr   OPTIONAL,
  OUT UINT16           *Protocol   OPTIONAL
  )
{
  return EmuSnpReceive (This, HeaderSize, BufferSize, Buffer, SrcAddr, DestAddr, Protocol);
}

EFI_STATUS
EFIAPI
GasketSnpThunkOpen (
  IN  EMU_IO_THUNK_PROTOCOL  *This
  )
{
  return EmuSnpThunkOpen (This);
}

EFI_STATUS
EFIAPI
GasketSnpThunkClose (
  IN  EMU_IO_THUNK_PROTOCOL  *This
  )
{
  return EmuSnpThunkClose (This);
}

//
// Gasket functions for accessing POSIX signals
//

/**
  Registers a new signal handler for the given signal number.

  @param[in]        SignalNumber      The signal number to register a callback for.
  @param[in]        SignalHandler     The signal handler function.

  @retval EFI_SUCCESS                 The signal handler was registered successfully.
  @retval EFI_INVALID_PARAMETER       SignalHandler is NULL or SignalNumber is is not a invalid signal number.

**/
EFI_STATUS
EFIAPI
GasketRegisterSignalHandler (
  IN      UINTN                       SignalNumber,
  IN      EMU_SIGNAL_HANDLER          SignalHandler
  )
{
  return SecRegisterSignalHandler (SignalNumber, SignalHandler);
}

/**
  Unregisters a previously registered signal handler.

  @param[in]        SignalNumber      The signal number to unregister the callback for.

  @retval EFI_SUCCESS                 The signal handler was unregistered successfully.
  @retval EFI_INVALID_PARAMETER       SignalNumber does not have a handler registered or is not a invalid signal number.

**/
EFI_STATUS
EFIAPI
GasketUnregisterSignalHandler (
  IN      UINTN                       SignalNumber
  )
{
  return SecUnregisterSignalHandler (SignalNumber);
}

EFI_STATUS
EFIAPI
GasketSignalOpen (
  IN  EMU_IO_THUNK_PROTOCOL  *This
  )
{
  return SignalOpen (This);
}

EFI_STATUS
EFIAPI
GasketSignalClose (
  IN  EMU_IO_THUNK_PROTOCOL  *This
  )
{
  return SignalClose (This);
}

UINT32
EFIAPI
GasketGetIsSigTermSignaled (
  VOID
  )
{
  return GetIsSigTermSignaled ();
}

VOID
EFIAPI
GasketSetIsSigTermSignaled (
  IN      UINT32   value
  )
{
  return SetIsSigTermSignaled (value);
}
