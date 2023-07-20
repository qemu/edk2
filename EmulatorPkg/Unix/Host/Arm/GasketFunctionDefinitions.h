/**@file GasketFunctionDefitions.h

  This header file contains function definitions for the UNIX side of the ABI.
  The Gaskets call these functions to allow UNIX APIs to be accessed from
  within the PEI and DXE environments.

Copyright (c) 2004 - 2023, Intel Corporation. All rights reserved.<BR>
SPDX-License-Identifier: BSD-2-Clause-Patent

**/

//
// Functions for the top-level EMU_THUNK_PROTOCOL
//

#ifndef _GASKET_FUNCTION_DEFINITIONS_H_
#define _GASKET_FUNCTION_DEFINITIONS_H_

#include <Uefi/UefiBaseType.h>
#include <Protocol/SimpleFileSystem.h>
#include <Protocol/EmuThunk.h>
#include <Protocol/EmuThread.h>
#include <Protocol/EmuBlockIo.h>
#include <Protocol/EmuGraphicsWindow.h>
#include <Protocol/EmuSnp.h>
#include <Protocol/EmuSignal.h>
#include "Gasket.h"

UINTN
SecWriteStdErr (
  IN UINT8  *Buffer,
  IN UINTN  NumberOfBytes
  );

EFI_STATUS
SecConfigStdIn (
  VOID
  );

UINTN
SecWriteStdOut (
  IN UINT8  *Buffer,
  IN UINTN  NumberOfBytes
  );

UINTN
SecReadStdIn (
  IN UINT8  *Buffer,
  IN UINTN  NumberOfBytes
  );

BOOLEAN
SecPollStdIn (
  VOID
  );

VOID *
SecMalloc (
  IN  UINTN  Size
  );

VOID *
SecValloc (
  IN  UINTN  Size
  );

BOOLEAN
SecFree (
  IN  VOID  *Ptr
  );

RETURN_STATUS
EFIAPI
SecPeCoffGetEntryPoint (
  IN     VOID  *Pe32Data,
  IN OUT VOID  **EntryPoint
  );

VOID
EFIAPI
SecPeCoffRelocateImageExtraAction (
  IN OUT PE_COFF_LOADER_IMAGE_CONTEXT  *ImageContext
  );

VOID
SecEnableInterrupt (
  VOID
  );

VOID
SecDisableInterrupt (
  VOID
  );

UINT64
QueryPerformanceFrequency (
  VOID
  );

UINT64
QueryPerformanceCounter (
  VOID
  );

VOID
SecSleep (
  IN  UINT64  Nanoseconds
  );

VOID
SecCpuSleep (
  VOID
  );

VOID
SecExit (
  UINTN  Status
  );

VOID
SecGetTime (
  OUT  EFI_TIME              *Time,
  OUT EFI_TIME_CAPABILITIES  *Capabilities OPTIONAL
  );

VOID
SecSetTime (
  IN  EFI_TIME  *Time
  );

VOID
SecSetTimer (
  IN  UINT64                  PeriodMs,
  IN  EMU_SET_TIMER_CALLBACK  CallBack
  );

/**
  This function modifies the attributes for the memory region specified by BaseAddress and
  Length from their current attributes to the attributes specified by Attributes. Since
  this is a user mode application, memory cachability attributes are ignored. Only memory
  protection attributes are interpreted.

  @param  BaseAddress      The physical address that is the start address of a memory region.
  @param  Length           The size in bytes of the memory region.
  @param  Attributes       The bit mask of attributes to set for the memory region.

  @retval EFI_SUCCESS           The attributes were set for the memory region.
  @retval EFI_ACCESS_DENIED     The attributes for the memory resource range specified by
                                BaseAddress and Length cannot be modified.
  @retval EFI_INVALID_PARAMETER Length is zero.
                                Attributes specified an illegal combination of attributes that
                                cannot be set together.
  @retval EFI_OUT_OF_RESOURCES  There are not enough system resources to modify the attributes of
                                the memory resource range.
  @retval EFI_UNSUPPORTED       The processor does not support one or more bytes of the memory
                                resource range specified by BaseAddress and Length.
                                The bit mask of attributes is not support for the memory resource
                                range specified by BaseAddress and Length.

**/
EFI_STATUS
EFIAPI
SecSetMemoryAttributes (
  IN  EFI_PHYSICAL_ADDRESS              BaseAddress,
  IN  UINT64                            Length,
  IN  UINT64                            Attributes
  );

EFI_STATUS
SecGetNextProtocol (
  IN  BOOLEAN                EmuBusDriver,
  OUT EMU_IO_THUNK_PROTOCOL  **Instance   OPTIONAL
  );

VOID
EFIAPI
SecPeCoffUnloadImageExtraAction (
  IN PE_COFF_LOADER_IMAGE_CONTEXT  *ImageContext
  );

//
// Functions for PPIs produced by SEC
//

EFI_STATUS
SecUnixPeiAutoScan (
  IN  UINTN                 Index,
  OUT EFI_PHYSICAL_ADDRESS  *MemoryBase,
  OUT UINT64                *MemorySize
  );

EFI_STATUS
SecUnixFdAddress (
  IN     UINTN                 Index,
  IN OUT EFI_PHYSICAL_ADDRESS  *FdBase,
  IN OUT UINT64                *FdSize,
  IN OUT EFI_PHYSICAL_ADDRESS  *FixUp
  );

/**
  Since the SEC is the only Unix program in stack it must export
  an interface to do POSIX calls. gEmuThunkProtocol is initialized in EmuThunk.c

  @retval Address of the gEmuThunkProtocol global

**/
VOID *
SecEmuThunkAddress (
  VOID
  );

//
// Reverse (UNIX to EFIAPI) gaskets
//

typedef
VOID
(EFIAPI *CALL_BACK_2ARG) (
  IN VOID  *Context,
  IN VOID  *Key
  );

//
// Functions for accessing libX11
//

EFI_STATUS
X11Size (
  IN  EMU_GRAPHICS_WINDOW_PROTOCOL  *GraphicsIo,
  IN  UINT32                        Width,
  IN  UINT32                        Height
  );

EFI_STATUS
X11CheckKey (
  IN  EMU_GRAPHICS_WINDOW_PROTOCOL  *GraphicsIo
  );

EFI_STATUS
X11GetKey (
  IN  EMU_GRAPHICS_WINDOW_PROTOCOL  *GraphicsIo,
  IN  EFI_KEY_DATA                  *KeyData
  );

EFI_STATUS
X11KeySetState (
  IN EMU_GRAPHICS_WINDOW_PROTOCOL  *GraphicsIo,
  IN EFI_KEY_TOGGLE_STATE          *KeyToggleState
  );

EFI_STATUS
X11RegisterKeyNotify (
  IN EMU_GRAPHICS_WINDOW_PROTOCOL                      *GraphicsIo,
  IN EMU_GRAPHICS_WINDOW_REGISTER_KEY_NOTIFY_CALLBACK  MakeCallBack,
  IN EMU_GRAPHICS_WINDOW_REGISTER_KEY_NOTIFY_CALLBACK  BreakCallBack,
  IN VOID                                              *Context
  );

EFI_STATUS
X11Blt (
  IN EMU_GRAPHICS_WINDOW_PROTOCOL     *GraphicsIo,
  IN  EFI_UGA_PIXEL                   *BltBuffer OPTIONAL,
  IN  EFI_UGA_BLT_OPERATION           BltOperation,
  IN  EMU_GRAPHICS_WINDOWS__BLT_ARGS  *Args
  );

EFI_STATUS
X11CheckPointer (
  IN  EMU_GRAPHICS_WINDOW_PROTOCOL  *GraphicsIo
  );

EFI_STATUS
X11GetPointerState (
  IN  EMU_GRAPHICS_WINDOW_PROTOCOL  *GraphicsIo,
  IN  EFI_SIMPLE_POINTER_STATE      *State
  );

EFI_STATUS
X11GraphicsWindowOpen (
  IN  EMU_IO_THUNK_PROTOCOL  *This
  );

EFI_STATUS
X11GraphicsWindowClose (
  IN  EMU_IO_THUNK_PROTOCOL  *This
  );

//
// Functions for accessing Pthreads
//

UINTN
EFIAPI
PthreadMutexLock (
  IN VOID  *Mutex
  );

UINTN
EFIAPI
PthreadMutexUnLock (
  IN VOID  *Mutex
  );

UINTN
EFIAPI
PthreadMutexTryLock (
  IN VOID  *Mutex
  );

VOID *
PthreadMutexInit (
  IN VOID
  );

UINTN
PthreadMutexDestroy (
  IN VOID  *Mutex
  );

UINTN
PthreadCreate (
  IN  VOID                       *Thread,
  IN  VOID                       *Attribute,
  IN  THREAD_THUNK_THREAD_ENTRY  Start,
  IN  VOID                       *Context
  );

VOID
PthreadExit (
  IN VOID  *ValuePtr
  );

UINTN
PthreadSelf (
  VOID
  );

EFI_STATUS
PthreadOpen (
  IN  EMU_IO_THUNK_PROTOCOL  *This
  );

EFI_STATUS
PthreadClose (
  IN  EMU_IO_THUNK_PROTOCOL  *This
  );

//
// Functions for accessing the host filesystem
//

/**
  Open the root directory on a volume.

  @param  This Protocol instance pointer.
  @param  Root Returns an Open file handle for the root directory

  @retval EFI_SUCCESS          The device was opened.
  @retval EFI_UNSUPPORTED      This volume does not support the file system.
  @retval EFI_NO_MEDIA         The device has no media.
  @retval EFI_DEVICE_ERROR     The device reported an error.
  @retval EFI_VOLUME_CORRUPTED The file system structures are corrupted.
  @retval EFI_ACCESS_DENIED    The service denied access to the file.
  @retval EFI_OUT_OF_RESOURCES The volume was not opened due to lack of resources.

**/
EFI_STATUS
PosixOpenVolume (
  IN EFI_SIMPLE_FILE_SYSTEM_PROTOCOL  *This,
  OUT EFI_FILE_PROTOCOL               **Root
  );

/**
  Opens a new file relative to the source file's location.

  @param  This       The protocol instance pointer.
  @param  NewHandle  Returns File Handle for FileName.
  @param  FileName   Null terminated string. "\", ".", and ".." are supported.
  @param  OpenMode   Open mode for file.
  @param  Attributes Only used for EFI_FILE_MODE_CREATE.

  @retval EFI_SUCCESS          The device was opened.
  @retval EFI_NOT_FOUND        The specified file could not be found on the device.
  @retval EFI_NO_MEDIA         The device has no media.
  @retval EFI_MEDIA_CHANGED    The media has changed.
  @retval EFI_DEVICE_ERROR     The device reported an error.
  @retval EFI_VOLUME_CORRUPTED The file system structures are corrupted.
  @retval EFI_ACCESS_DENIED    The service denied access to the file.
  @retval EFI_OUT_OF_RESOURCES The volume was not opened due to lack of resources.
  @retval EFI_VOLUME_FULL      The volume is full.

**/
EFI_STATUS
PosixFileOpen (
  IN EFI_FILE_PROTOCOL   *This,
  OUT EFI_FILE_PROTOCOL  **NewHandle,
  IN CHAR16              *FileName,
  IN UINT64              OpenMode,
  IN UINT64              Attributes
  );

/**
  Close the file handle

  @param  This          Protocol instance pointer.

  @retval EFI_SUCCESS   The device was opened.

**/
EFI_STATUS
PosixFileClose (
  IN EFI_FILE_PROTOCOL  *This
  );

/**
  Close and delete the file handle.

  @param  This                     Protocol instance pointer.

  @retval EFI_SUCCESS              The device was opened.
  @retval EFI_WARN_DELETE_FAILURE  The handle was closed but the file was not deleted.

**/
EFI_STATUS
PosixFileDelete (
  IN EFI_FILE_PROTOCOL  *This
  );

/**
  Read data from the file.

  @param  This       Protocol instance pointer.
  @param  BufferSize On input size of buffer, on output amount of data in buffer.
  @param  Buffer     The buffer in which data is read.

  @retval EFI_SUCCESS          Data was read.
  @retval EFI_NO_MEDIA         The device has no media.
  @retval EFI_DEVICE_ERROR     The device reported an error.
  @retval EFI_VOLUME_CORRUPTED The file system structures are corrupted.
  @retval EFI_BUFFER_TO_SMALL  BufferSize is too small. BufferSize contains required size.

**/
EFI_STATUS
PosixFileRead (
  IN EFI_FILE_PROTOCOL  *This,
  IN OUT UINTN          *BufferSize,
  OUT VOID              *Buffer
  );

/**
  Write data to a file.

  @param  This       Protocol instance pointer.
  @param  BufferSize On input size of buffer, on output amount of data in buffer.
  @param  Buffer     The buffer in which data to write.

  @retval EFI_SUCCESS          Data was written.
  @retval EFI_UNSUPPORTED      Writes to Open directory are not supported.
  @retval EFI_NO_MEDIA         The device has no media.
  @retval EFI_DEVICE_ERROR     The device reported an error.
  @retval EFI_DEVICE_ERROR     An attempt was made to write to a deleted file.
  @retval EFI_VOLUME_CORRUPTED The file system structures are corrupted.
  @retval EFI_WRITE_PROTECTED  The device is write protected.
  @retval EFI_ACCESS_DENIED    The file was open for read only.
  @retval EFI_VOLUME_FULL      The volume is full.

**/
EFI_STATUS
PosixFileWrite (
  IN EFI_FILE_PROTOCOL  *This,
  IN OUT UINTN          *BufferSize,
  IN VOID               *Buffer
  );

/**
  Set a files current position

  @param  This            Protocol instance pointer.
  @param  Position        Byte position from the start of the file.

  @retval EFI_SUCCESS     Data was written.
  @retval EFI_UNSUPPORTED Seek request for non-zero is not valid on open.

**/
EFI_STATUS
PosixFileSetPosition (
  IN EFI_FILE_PROTOCOL  *This,
  IN UINT64             Position
  );

/**
  Get a file's current position

  @param  This            Protocol instance pointer.
  @param  Position        Byte position from the start of the file.

  @retval EFI_SUCCESS     Data was written.
  @retval EFI_UNSUPPORTED Seek request for non-zero is not valid on open..

**/
EFI_STATUS
PosixFileGetPosition (
  IN EFI_FILE_PROTOCOL  *This,
  OUT UINT64            *Position
  );

/**
  Get information about a file.

  @param  This            Protocol instance pointer.
  @param  InformationType Type of information to return in Buffer.
  @param  BufferSize      On input size of buffer, on output amount of data in buffer.
  @param  Buffer          The buffer to return data.

  @retval EFI_SUCCESS          Data was returned.
  @retval EFI_UNSUPPORTED      InformationType is not supported.
  @retval EFI_NO_MEDIA         The device has no media.
  @retval EFI_DEVICE_ERROR     The device reported an error.
  @retval EFI_VOLUME_CORRUPTED The file system structures are corrupted.
  @retval EFI_WRITE_PROTECTED  The device is write protected.
  @retval EFI_ACCESS_DENIED    The file was open for read only.
  @retval EFI_BUFFER_TOO_SMALL Buffer was too small; required size returned in BufferSize.

**/
EFI_STATUS
PosixFileGetInfo (
  IN EFI_FILE_PROTOCOL  *This,
  IN EFI_GUID           *InformationType,
  IN OUT UINTN          *BufferSize,
  OUT VOID              *Buffer
  );

/**
  Set information about a file

  @param  File            Protocol instance pointer.
  @param  InformationType Type of information in Buffer.
  @param  BufferSize      Size of buffer.
  @param  Buffer          The data to write.

  @retval EFI_SUCCESS          Data was returned.
  @retval EFI_UNSUPPORTED      InformationType is not supported.
  @retval EFI_NO_MEDIA         The device has no media.
  @retval EFI_DEVICE_ERROR     The device reported an error.
  @retval EFI_VOLUME_CORRUPTED The file system structures are corrupted.
  @retval EFI_WRITE_PROTECTED  The device is write protected.
  @retval EFI_ACCESS_DENIED    The file was open for read only.

**/
EFI_STATUS
PosixFileSetInfo (
  IN EFI_FILE_PROTOCOL  *This,
  IN EFI_GUID           *InformationType,
  IN UINTN              BufferSize,
  IN VOID               *Buffer
  );

/**
  Flush data back for the file handle.

  @param  This Protocol instance pointer.

  @retval EFI_SUCCESS          Data was written.
  @retval EFI_UNSUPPORTED      Writes to Open directory are not supported.
  @retval EFI_NO_MEDIA         The device has no media.
  @retval EFI_DEVICE_ERROR     The device reported an error.
  @retval EFI_VOLUME_CORRUPTED The file system structures are corrupted.
  @retval EFI_WRITE_PROTECTED  The device is write protected.
  @retval EFI_ACCESS_DENIED    The file was open for read only.
  @retval EFI_VOLUME_FULL      The volume is full.

**/
EFI_STATUS
PosixFileFlush (
  IN EFI_FILE_PROTOCOL  *This
  );

EFI_STATUS
PosixFileSystemThunkOpen (
  IN  EMU_IO_THUNK_PROTOCOL  *This
  );

EFI_STATUS
PosixFileSystemThunkClose (
  IN  EMU_IO_THUNK_PROTOCOL  *This
  );

//
// Functions for reading/writing to disk images
//

EFI_STATUS
EmuBlockIoReset (
  IN EMU_BLOCK_IO_PROTOCOL  *This,
  IN BOOLEAN                ExtendedVerification
  );

/**
  Read BufferSize bytes from Lba into Buffer.

  This function reads the requested number of blocks from the device. All the
  blocks are read, or an error is returned.
  If EFI_DEVICE_ERROR, EFI_NO_MEDIA,_or EFI_MEDIA_CHANGED is returned and
  non-blocking I/O is being used, the Event associated with this request will
  not be signaled.

  @param[in]       This       Indicates a pointer to the calling context.
  @param[in]       MediaId    Id of the media, changes every time the media is
                              replaced.
  @param[in]       Lba        The starting Logical Block Address to read from.
  @param[in, out]  Token      A pointer to the token associated with the transaction.
  @param[in]       BufferSize Size of Buffer, must be a multiple of device block size.
  @param[out]      Buffer     A pointer to the destination buffer for the data. The
                              caller is responsible for either having implicit or
                              explicit ownership of the buffer.

  @retval EFI_SUCCESS           The read request was queued if Token->Event is
                                not NULL.The data was read correctly from the
                                device if the Token->Event is NULL.
  @retval EFI_DEVICE_ERROR      The device reported an error while performing
                                the read.
  @retval EFI_NO_MEDIA          There is no media in the device.
  @retval EFI_MEDIA_CHANGED     The MediaId is not for the current media.
  @retval EFI_BAD_BUFFER_SIZE   The BufferSize parameter is not a multiple of the
                                intrinsic block size of the device.
  @retval EFI_INVALID_PARAMETER The read request contains LBAs that are not valid,
                                or the buffer is not on proper alignment.
  @retval EFI_OUT_OF_RESOURCES  The request could not be completed due to a lack
                                of resources.
**/
EFI_STATUS
EmuBlockIoReadBlocks (
  IN     EMU_BLOCK_IO_PROTOCOL  *This,
  IN     UINT32                 MediaId,
  IN     EFI_LBA                LBA,
  IN OUT EFI_BLOCK_IO2_TOKEN    *Token,
  IN     UINTN                  BufferSize,
  OUT VOID                      *Buffer
  );

/**
  Write BufferSize bytes from Lba into Buffer.

  This function writes the requested number of blocks to the device. All blocks
  are written, or an error is returned.If EFI_DEVICE_ERROR, EFI_NO_MEDIA,
  EFI_WRITE_PROTECTED or EFI_MEDIA_CHANGED is returned and non-blocking I/O is
  being used, the Event associated with this request will not be signaled.

  @param[in]       This       Indicates a pointer to the calling context.
  @param[in]       MediaId    The media ID that the write request is for.
  @param[in]       Lba        The starting logical block address to be written. The
                              caller is responsible for writing to only legitimate
                              locations.
  @param[in, out]  Token      A pointer to the token associated with the transaction.
  @param[in]       BufferSize Size of Buffer, must be a multiple of device block size.
  @param[in]       Buffer     A pointer to the source buffer for the data.

  @retval EFI_SUCCESS           The write request was queued if Event is not NULL.
                                The data was written correctly to the device if
                                the Event is NULL.
  @retval EFI_WRITE_PROTECTED   The device can not be written to.
  @retval EFI_NO_MEDIA          There is no media in the device.
  @retval EFI_MEDIA_CHANGED     The MediaId does not match the current device.
  @retval EFI_DEVICE_ERROR      The device reported an error while performing the write.
  @retval EFI_BAD_BUFFER_SIZE   The Buffer was not a multiple of the block size of the device.
  @retval EFI_INVALID_PARAMETER The write request contains LBAs that are not valid,
                                or the buffer is not on proper alignment.
  @retval EFI_OUT_OF_RESOURCES  The request could not be completed due to a lack
                                of resources.

**/
EFI_STATUS
EmuBlockIoWriteBlocks (
  IN     EMU_BLOCK_IO_PROTOCOL  *This,
  IN     UINT32                 MediaId,
  IN     EFI_LBA                LBA,
  IN OUT EFI_BLOCK_IO2_TOKEN    *Token,
  IN     UINTN                  BufferSize,
  IN     VOID                   *Buffer
  );

/**
  Flush the Block Device.

  If EFI_DEVICE_ERROR, EFI_NO_MEDIA,_EFI_WRITE_PROTECTED or EFI_MEDIA_CHANGED
  is returned and non-blocking I/O is being used, the Event associated with
  this request will not be signaled.

  @param[in]      This     Indicates a pointer to the calling context.
  @param[in,out]  Token    A pointer to the token associated with the transaction

  @retval EFI_SUCCESS          The flush request was queued if Event is not NULL.
                               All outstanding data was written correctly to the
                               device if the Event is NULL.
  @retval EFI_DEVICE_ERROR     The device reported an error while writing back
                               the data.
  @retval EFI_WRITE_PROTECTED  The device cannot be written to.
  @retval EFI_NO_MEDIA         There is no media in the device.
  @retval EFI_MEDIA_CHANGED    The MediaId is not for the current media.
  @retval EFI_OUT_OF_RESOURCES The request could not be completed due to a lack
                               of resources.

**/
EFI_STATUS
EmuBlockIoFlushBlocks (
  IN     EMU_BLOCK_IO_PROTOCOL  *This,
  IN OUT EFI_BLOCK_IO2_TOKEN    *Token
  );

EFI_STATUS
EmuBlockIoCreateMapping (
  IN     EMU_BLOCK_IO_PROTOCOL  *This,
  IN     EFI_BLOCK_IO_MEDIA     *Media
  );

EFI_STATUS
EmuBlockIoThunkOpen (
  IN  EMU_IO_THUNK_PROTOCOL  *This
  );

EFI_STATUS
EmuBlockIoThunkClose (
  IN  EMU_IO_THUNK_PROTOCOL  *This
  );

//
// Functions for accessing BSD Sockets (TCP-IP Networks)
//

/**
  Register storage for SNP Mode.

  @param  This Protocol instance pointer.
  @param  Mode SimpleNetworkProtocol Mode structure passed into driver.

  @retval EFI_SUCCESS           The network interface was started.
  @retval EFI_INVALID_PARAMETER One or more of the parameters has an unsupported value.

**/
EFI_STATUS
EmuSnpCreateMapping (
  IN     EMU_SNP_PROTOCOL         *This,
  IN     EFI_SIMPLE_NETWORK_MODE  *Mode
  );

/**
  Changes the state of a network interface from "stopped" to "started".

  @param  This Protocol instance pointer.

  @retval EFI_SUCCESS           The network interface was started.
  @retval EFI_ALREADY_STARTED   The network interface is already in the started state.
  @retval EFI_INVALID_PARAMETER One or more of the parameters has an unsupported value.
  @retval EFI_DEVICE_ERROR      The command could not be sent to the network interface.
  @retval EFI_UNSUPPORTED       This function is not supported by the network interface.

**/
EFI_STATUS
EmuSnpStart (
  IN EMU_SNP_PROTOCOL  *This
  );

/**
  Changes the state of a network interface from "started" to "stopped".

  @param  This Protocol instance pointer.

  @retval EFI_SUCCESS           The network interface was stopped.
  @retval EFI_ALREADY_STARTED   The network interface is already in the stopped state.
  @retval EFI_INVALID_PARAMETER One or more of the parameters has an unsupported value.
  @retval EFI_DEVICE_ERROR      The command could not be sent to the network interface.
  @retval EFI_UNSUPPORTED       This function is not supported by the network interface.

**/
EFI_STATUS
EmuSnpStop (
  IN EMU_SNP_PROTOCOL  *This
  );

/**
  Resets a network adapter and allocates the transmit and receive buffers
  required by the network interface; optionally, also requests allocation
  of additional transmit and receive buffers.

  @param  This              The protocol instance pointer.
  @param  ExtraRxBufferSize The size, in bytes, of the extra receive buffer space
                            that the driver should allocate for the network interface.
                            Some network interfaces will not be able to use the extra
                            buffer, and the caller will not know if it is actually
                            being used.
  @param  ExtraTxBufferSize The size, in bytes, of the extra transmit buffer space
                            that the driver should allocate for the network interface.
                            Some network interfaces will not be able to use the extra
                            buffer, and the caller will not know if it is actually
                            being used.

  @retval EFI_SUCCESS           The network interface was initialized.
  @retval EFI_NOT_STARTED       The network interface has not been started.
  @retval EFI_OUT_OF_RESOURCES  There was not enough memory for the transmit and
                                receive buffers.
  @retval EFI_INVALID_PARAMETER One or more of the parameters has an unsupported value.
  @retval EFI_DEVICE_ERROR      The command could not be sent to the network interface.
  @retval EFI_UNSUPPORTED       This function is not supported by the network interface.

**/
EFI_STATUS
EmuSnpInitialize (
  IN EMU_SNP_PROTOCOL  *This,
  IN UINTN             ExtraRxBufferSize  OPTIONAL,
  IN UINTN             ExtraTxBufferSize  OPTIONAL
  );

/**
  Resets a network adapter and re-initializes it with the parameters that were
  provided in the previous call to Initialize().

  @param  This                 The protocol instance pointer.
  @param  ExtendedVerification Indicates that the driver may perform a more
                               exhaustive verification operation of the device
                               during reset.

  @retval EFI_SUCCESS           The network interface was reset.
  @retval EFI_NOT_STARTED       The network interface has not been started.
  @retval EFI_INVALID_PARAMETER One or more of the parameters has an unsupported value.
  @retval EFI_DEVICE_ERROR      The command could not be sent to the network interface.
  @retval EFI_UNSUPPORTED       This function is not supported by the network interface.

**/
EFI_STATUS
EmuSnpReset (
  IN EMU_SNP_PROTOCOL  *This,
  IN BOOLEAN           ExtendedVerification
  );

/**
  Resets a network adapter and leaves it in a state that is safe for
  another driver to initialize.

  @param  This Protocol instance pointer.

  @retval EFI_SUCCESS           The network interface was shutdown.
  @retval EFI_NOT_STARTED       The network interface has not been started.
  @retval EFI_INVALID_PARAMETER One or more of the parameters has an unsupported value.
  @retval EFI_DEVICE_ERROR      The command could not be sent to the network interface.
  @retval EFI_UNSUPPORTED       This function is not supported by the network interface.

**/
EFI_STATUS
EmuSnpShutdown (
  IN EMU_SNP_PROTOCOL  *This
  );

/**
  Manages the multicast receive filters of a network interface.

  @param  This             The protocol instance pointer.
  @param  Enable           A bit mask of receive filters to enable on the network interface.
  @param  Disable          A bit mask of receive filters to disable on the network interface.
  @param  ResetMCastFilter Set to TRUE to reset the contents of the multicast receive
                           filters on the network interface to their default values.
  @param  McastFilterCnt   Number of multicast HW MAC addresses in the new
                           MCastFilter list. This value must be less than or equal to
                           the MCastFilterCnt field of EMU_SNP_MODE. This
                           field is optional if ResetMCastFilter is TRUE.
  @param  MCastFilter      A pointer to a list of new multicast receive filter HW MAC
                           addresses. This list will replace any existing multicast
                           HW MAC address list. This field is optional if
                           ResetMCastFilter is TRUE.

  @retval EFI_SUCCESS           The multicast receive filter list was updated.
  @retval EFI_NOT_STARTED       The network interface has not been started.
  @retval EFI_INVALID_PARAMETER One or more of the parameters has an unsupported value.
  @retval EFI_DEVICE_ERROR      The command could not be sent to the network interface.
  @retval EFI_UNSUPPORTED       This function is not supported by the network interface.

**/
EFI_STATUS
EmuSnpReceiveFilters (
  IN EMU_SNP_PROTOCOL  *This,
  IN UINT32            Enable,
  IN UINT32            Disable,
  IN BOOLEAN           ResetMCastFilter,
  IN UINTN             MCastFilterCnt     OPTIONAL,
  IN EFI_MAC_ADDRESS   *MCastFilter OPTIONAL
  );

/**
  Modifies or resets the current station address, if supported.

  @param  This  The protocol instance pointer.
  @param  Reset Flag used to reset the station address to the network interfaces
                permanent address.
  @param  New   The new station address to be used for the network interface.

  @retval EFI_SUCCESS           The network interfaces station address was updated.
  @retval EFI_NOT_STARTED       The network interface has not been started.
  @retval EFI_INVALID_PARAMETER One or more of the parameters has an unsupported value.
  @retval EFI_DEVICE_ERROR      The command could not be sent to the network interface.
  @retval EFI_UNSUPPORTED       This function is not supported by the network interface.

**/
EFI_STATUS
EmuSnpStationAddress (
  IN EMU_SNP_PROTOCOL  *This,
  IN BOOLEAN           Reset,
  IN EFI_MAC_ADDRESS   *New OPTIONAL
  );

/**
  Resets or collects the statistics on a network interface.

  @param  This            Protocol instance pointer.
  @param  Reset           Set to TRUE to reset the statistics for the network interface.
  @param  StatisticsSize  On input the size, in bytes, of StatisticsTable. On
                          output the size, in bytes, of the resulting table of
                          statistics.
  @param  StatisticsTable A pointer to the EFI_NETWORK_STATISTICS structure that
                          contains the statistics.

  @retval EFI_SUCCESS           The statistics were collected from the network interface.
  @retval EFI_NOT_STARTED       The network interface has not been started.
  @retval EFI_BUFFER_TOO_SMALL  The Statistics buffer was too small. The current buffer
                                size needed to hold the statistics is returned in
                                StatisticsSize.
  @retval EFI_INVALID_PARAMETER One or more of the parameters has an unsupported value.
  @retval EFI_DEVICE_ERROR      The command could not be sent to the network interface.
  @retval EFI_UNSUPPORTED       This function is not supported by the network interface.

**/
EFI_STATUS
EmuSnpStatistics (
  IN EMU_SNP_PROTOCOL         *This,
  IN BOOLEAN                  Reset,
  IN OUT UINTN                *StatisticsSize   OPTIONAL,
  OUT EFI_NETWORK_STATISTICS  *StatisticsTable  OPTIONAL
  );

/**
  Converts a multicast IP address to a multicast HW MAC address.

  @param  This The protocol instance pointer.
  @param  IPv6 Set to TRUE if the multicast IP address is IPv6 [RFC 2460]. Set
               to FALSE if the multicast IP address is IPv4 [RFC 791].
  @param  IP   The multicast IP address that is to be converted to a multicast
               HW MAC address.
  @param  MAC  The multicast HW MAC address that is to be generated from IP.

  @retval EFI_SUCCESS           The multicast IP address was mapped to the multicast
                                HW MAC address.
  @retval EFI_NOT_STARTED       The network interface has not been started.
  @retval EFI_BUFFER_TOO_SMALL  The Statistics buffer was too small. The current buffer
                                size needed to hold the statistics is returned in
                                StatisticsSize.
  @retval EFI_INVALID_PARAMETER One or more of the parameters has an unsupported value.
  @retval EFI_DEVICE_ERROR      The command could not be sent to the network interface.
  @retval EFI_UNSUPPORTED       This function is not supported by the network interface.

**/
EFI_STATUS
EmuSnpMCastIpToMac (
  IN EMU_SNP_PROTOCOL  *This,
  IN BOOLEAN           IPv6,
  IN EFI_IP_ADDRESS    *IP,
  OUT EFI_MAC_ADDRESS  *MAC
  );

/**
  Performs read and write operations on the NVRAM device attached to a
  network interface.

  @param  This       The protocol instance pointer.
  @param  ReadWrite  TRUE for read operations, FALSE for write operations.
  @param  Offset     Byte offset in the NVRAM device at which to start the read or
                     write operation. This must be a multiple of NvRamAccessSize and
                     less than NvRamSize.
  @param  BufferSize The number of bytes to read or write from the NVRAM device.
                     This must also be a multiple of NvramAccessSize.
  @param  Buffer     A pointer to the data buffer.

  @retval EFI_SUCCESS           The NVRAM access was performed.
  @retval EFI_NOT_STARTED       The network interface has not been started.
  @retval EFI_INVALID_PARAMETER One or more of the parameters has an unsupported value.
  @retval EFI_DEVICE_ERROR      The command could not be sent to the network interface.
  @retval EFI_UNSUPPORTED       This function is not supported by the network interface.

**/
EFI_STATUS
EmuSnpNvData (
  IN EMU_SNP_PROTOCOL  *This,
  IN BOOLEAN           ReadWrite,
  IN UINTN             Offset,
  IN UINTN             BufferSize,
  IN OUT VOID          *Buffer
  );

/**
  Reads the current interrupt status and recycled transmit buffer status from
  a network interface.

  @param  This            The protocol instance pointer.
  @param  InterruptStatus A pointer to the bit mask of the currently active interrupts
                          If this is NULL, the interrupt status will not be read from
                          the device. If this is not NULL, the interrupt status will
                          be read from the device. When the  interrupt status is read,
                          it will also be cleared. Clearing the transmit  interrupt
                          does not empty the recycled transmit buffer array.
  @param  TxBuf           Recycled transmit buffer address. The network interface will
                          not transmit if its internal recycled transmit buffer array
                          is full. Reading the transmit buffer does not clear the
                          transmit interrupt. If this is NULL, then the transmit buffer
                          status will not be read. If there are no transmit buffers to
                          recycle and TxBuf is not NULL, * TxBuf will be set to NULL.

  @retval EFI_SUCCESS           The status of the network interface was retrieved.
  @retval EFI_NOT_STARTED       The network interface has not been started.
  @retval EFI_INVALID_PARAMETER One or more of the parameters has an unsupported value.
  @retval EFI_DEVICE_ERROR      The command could not be sent to the network interface.
  @retval EFI_UNSUPPORTED       This function is not supported by the network interface.

**/
EFI_STATUS
EmuSnpGetStatus (
  IN EMU_SNP_PROTOCOL  *This,
  OUT UINT32           *InterruptStatus OPTIONAL,
  OUT VOID             **TxBuf OPTIONAL
  );

/**
  Places a packet in the transmit queue of a network interface.

  @param  This       The protocol instance pointer.
  @param  HeaderSize The size, in bytes, of the media header to be filled in by
                     the Transmit() function. If HeaderSize is non-zero, then it
                     must be equal to This->Mode->MediaHeaderSize and the DestAddr
                     and Protocol parameters must not be NULL.
  @param  BufferSize The size, in bytes, of the entire packet (media header and
                     data) to be transmitted through the network interface.
  @param  Buffer     A pointer to the packet (media header followed by data) to be
                     transmitted. This parameter cannot be NULL. If HeaderSize is zero,
                     then the media header in Buffer must already be filled in by the
                     caller. If HeaderSize is non-zero, then the media header will be
                     filled in by the Transmit() function.
  @param  SrcAddr    The source HW MAC address. If HeaderSize is zero, then this parameter
                     is ignored. If HeaderSize is non-zero and SrcAddr is NULL, then
                     This->Mode->CurrentAddress is used for the source HW MAC address.
  @param  DestAddr   The destination HW MAC address. If HeaderSize is zero, then this
                     parameter is ignored.
  @param  Protocol   The type of header to build. If HeaderSize is zero, then this
                     parameter is ignored. See RFC 1700, section "Ether Types", for
                     examples.

  @retval EFI_SUCCESS           The packet was placed on the transmit queue.
  @retval EFI_NOT_STARTED       The network interface has not been started.
  @retval EFI_NOT_READY         The network interface is too busy to accept this transmit request.
  @retval EFI_BUFFER_TOO_SMALL  The BufferSize parameter is too small.
  @retval EFI_INVALID_PARAMETER One or more of the parameters has an unsupported value.
  @retval EFI_DEVICE_ERROR      The command could not be sent to the network interface.
  @retval EFI_UNSUPPORTED       This function is not supported by the network interface.

**/
EFI_STATUS
EmuSnpTransmit (
  IN EMU_SNP_PROTOCOL  *This,
  IN UINTN             HeaderSize,
  IN UINTN             BufferSize,
  IN VOID              *Buffer,
  IN EFI_MAC_ADDRESS   *SrcAddr  OPTIONAL,
  IN EFI_MAC_ADDRESS   *DestAddr OPTIONAL,
  IN UINT16            *Protocol OPTIONAL
  );

/**
  Receives a packet from a network interface.

  @param  This       The protocol instance pointer.
  @param  HeaderSize The size, in bytes, of the media header received on the network
                     interface. If this parameter is NULL, then the media header size
                     will not be returned.
  @param  BufferSize On entry, the size, in bytes, of Buffer. On exit, the size, in
                     bytes, of the packet that was received on the network interface.
  @param  Buffer     A pointer to the data buffer to receive both the media header and
                     the data.
  @param  SrcAddr    The source HW MAC address. If this parameter is NULL, the
                     HW MAC source address will not be extracted from the media
                     header.
  @param  DestAddr   The destination HW MAC address. If this parameter is NULL,
                     the HW MAC destination address will not be extracted from the
                     media header.
  @param  Protocol   The media header type. If this parameter is NULL, then the
                     protocol will not be extracted from the media header. See
                     RFC 1700 section "Ether Types" for examples.

  @retval  EFI_SUCCESS           The received data was stored in Buffer, and BufferSize has
                                 been updated to the number of bytes received.
  @retval  EFI_NOT_STARTED       The network interface has not been started.
  @retval  EFI_NOT_READY         The network interface is too busy to accept this transmit
                                 request.
  @retval  EFI_BUFFER_TOO_SMALL  The BufferSize parameter is too small.
  @retval  EFI_INVALID_PARAMETER One or more of the parameters has an unsupported value.
  @retval  EFI_DEVICE_ERROR      The command could not be sent to the network interface.
  @retval  EFI_UNSUPPORTED       This function is not supported by the network interface.

**/
EFI_STATUS
EmuSnpReceive (
  IN EMU_SNP_PROTOCOL  *This,
  OUT UINTN            *HeaderSize OPTIONAL,
  IN OUT UINTN         *BufferSize,
  OUT VOID             *Buffer,
  OUT EFI_MAC_ADDRESS  *SrcAddr    OPTIONAL,
  OUT EFI_MAC_ADDRESS  *DestAddr   OPTIONAL,
  OUT UINT16           *Protocol   OPTIONAL
  );

EFI_STATUS
EmuSnpThunkOpen (
  IN  EMU_IO_THUNK_PROTOCOL  *This
  );

EFI_STATUS
EmuSnpThunkClose (
  IN  EMU_IO_THUNK_PROTOCOL  *This
  );

//
// Gasket functions for accessing POSIX signals
//

/**
  Registers a new signal handler for the given signal number.

  @param[in]        SignalNumber      The signal number to register a callback for.
  @param[in]        SignalHandler     The signal handler function.

  @retval EFI_SUCCESS                 The signal handler was registered successfully.
  @retval EFI_INVALID_PARAMETER       SignalHandler is NULL or SignalNumber is is not a invalid signal number.
  @retval EFI_DEVICE_ERROR            sigaction syscall returned an error.

**/
EFI_STATUS
SecRegisterSignalHandler (
  IN      UINTN                       SignalNumber,
  IN      EMU_SIGNAL_HANDLER          SignalHandler
  );

/**
  Unregisters a previously registered signal handler.

  @param[in]        SignalNumber      The signal number to unregister the callback for.

  @retval EFI_SUCCESS                 The signal handler was unregistered successfully.
  @retval EFI_INVALID_PARAMETER       SignalNumber does not have a handler registered or is not a invalid signal number.
  @retval EFI_DEVICE_ERROR            signal syscall returned an error.

**/
EFI_STATUS
SecUnregisterSignalHandler (
  IN      UINTN                       SignalNumber
  );

/**
  Get IsSigTermSignaled value.

  @retval        Value             current IsSigTermSignaled value.

**/
UINT32
EFIAPI
GetIsSigTermSignaled (
  VOID
  );

/**
  Set IsSigTermSignaled value.

  @param[in]        Value             desired IsSigTermSignaled value.

**/
VOID
EFIAPI
SetIsSigTermSignaled (
  IN      UINT32   value
  );

EFI_STATUS
EmuSignalThunkOpen (
  IN  EMU_IO_THUNK_PROTOCOL  *This
  );

EFI_STATUS
EmuSignalThunkClose (
  IN  EMU_IO_THUNK_PROTOCOL  *This
  );

/**
  Acquire the lock semaphore for the PRM Dispatch Service Mailbox

  @retval EFI_SUCCESS                 The mailbox was acquired successfully.
  @retval EFI_TIMEOUT                 Acquiring the mailbox failed.

**/
EFI_STATUS
SecDbusAcquireMailbox (
  VOID
  );

/**
  Release the lock semaphore for the PRM Dispatch Service Mailbox

  @retval EFI_SUCCESS                 The mailbox was released successfully.
  @retval EFI_TIMEOUT                 Releasing the mailbox failed.

**/
EFI_STATUS
SecDbusReleaseMailbox (
  VOID
  );

/**
  Wait for a new message to be stored in the PRM Dispatch Service Mailbox

  @retval EFI_SUCCESS                 A new message is in the mailbox
  @retval EFI_TIMEOUT                 Waiting for a new message failed.

**/
EFI_STATUS
SecDbusWaitForMessage (
  VOID
  );

/**
  Signal that the message has been successfully processed and the results
  are waiting in the PRM Dispatch Service Mailbox

  @retval None

**/
VOID
SecDbusMessageResponseReady (
  VOID
  );

EFI_STATUS
SecDbusPrmdServiceThunkOpen (
  IN  EMU_IO_THUNK_PROTOCOL  *This
  );

EFI_STATUS
SecDbusPrmdServiceThunkClose (
  IN  EMU_IO_THUNK_PROTOCOL  *This
  );

#endif
