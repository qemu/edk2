/** @file
  Instance of Cache Maintenance Library using emulator thunks to OS syscalls

  Copyright (c) 2023, Intel Corporation. All rights reserved.<BR>

  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#include <Uefi/UefiBaseType.h>
#include <Pi/PiHob.h>
#include <Library/BaseLib.h>
#include <Library/DebugLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/HobLib.h>

#include <Protocol/EmuThunk.h>
#include <Protocol/EmuIoThunk.h>
#include <Protocol/EmuCache.h>

EFI_STATUS
EFIAPI
DxeGetCacheThunkProtocol (
  OUT      EMU_CACHE_THUNK_PROTOCOL   **EmuCache
  )
{
  //
  // Note: Because the HOB list is not available during early initialization of
  //       DxeMain we need to delay enumerating the HOB list and not use a
  //       LibraryClass constructor method.
  //
  EFI_HOB_GUID_TYPE       *GuidHob;
  EMU_THUNK_PROTOCOL      *Thunk;
  EMU_IO_THUNK_PROTOCOL   *EmuIoThunk;
  EFI_STATUS              Status;

  //
  // Get Unix Thunk Protocol so we can retrieve I/O Thunk Protocols
  //
  GuidHob = GetFirstGuidHob (&gEmuThunkProtocolGuid);
  if (GuidHob == NULL) {
    return EFI_NOT_FOUND;
  }

  Thunk = (EMU_THUNK_PROTOCOL *)(*(UINTN *)(GET_GUID_HOB_DATA (GuidHob)));
  if (Thunk == NULL) {
    return EFI_NOT_FOUND;
  }

  //
  // Locate the I/O Thunk Protocol representing the Cache Thunk Protocol
  //
  for (Status = EFI_SUCCESS, EmuIoThunk = NULL; !EFI_ERROR (Status); ) {
    Status = Thunk->GetNextProtocol (FALSE, &EmuIoThunk);
    if (EFI_ERROR (Status)) {
      break;
    }

    if (EmuIoThunk->Instance == 0) {
      if (CompareGuid (EmuIoThunk->Protocol, &gEmuCacheThunkProtocolGuid)) {
        //
        // Open the I/O Thunk Protocol to retrieve the Cache Thunk Protocol
        //
        Status = EmuIoThunk->Open (EmuIoThunk);
        if (EFI_ERROR (Status)) {
            return Status;
        }
        *EmuCache = (EMU_CACHE_THUNK_PROTOCOL *) EmuIoThunk->Interface;
        return EFI_SUCCESS;
      }
    }
  }

  return EFI_NOT_FOUND;
}

/**
  Invalidates the entire instruction cache in cache coherency domain of the
  calling CPU.

**/
VOID
EFIAPI
InvalidateInstructionCache (
  VOID
  )
{
  EMU_CACHE_THUNK_PROTOCOL   *EmuCache;
  EFI_STATUS                 Status;

  Status = DxeGetCacheThunkProtocol (&EmuCache);
  if (!EFI_ERROR (Status) && EmuCache != NULL) {
    EmuCache->InvalidateInstructionCache ();
  }
}

/**
  Invalidates a range of instruction cache lines in the cache coherency domain
  of the calling CPU.

  Invalidates the instruction cache lines specified by Address and Length. If
  Address is not aligned on a cache line boundary, then entire instruction
  cache line containing Address is invalidated. If Address + Length is not
  aligned on a cache line boundary, then the entire instruction cache line
  containing Address + Length -1 is invalidated. This function may choose to
  invalidate the entire instruction cache if that is more efficient than
  invalidating the specified range. If Length is 0, then no instruction cache
  lines are invalidated. Address is returned.

  If Length is greater than (MAX_ADDRESS - Address + 1), then ASSERT().

  @param  Address The base address of the instruction cache lines to
                  invalidate. If the CPU is in a physical addressing mode, then
                  Address is a physical address. If the CPU is in a virtual
                  addressing mode, then Address is a virtual address.

  @param  Length  The number of bytes to invalidate from the instruction cache.

  @return Address.

**/
VOID *
EFIAPI
InvalidateInstructionCacheRange (
  IN      VOID   *Address,
  IN      UINTN  Length
  )
{
  EMU_CACHE_THUNK_PROTOCOL   *EmuCache;
  EFI_STATUS                 Status;

  Status = DxeGetCacheThunkProtocol (&EmuCache);
  if (!EFI_ERROR (Status) && EmuCache != NULL) {
    return EmuCache->InvalidateInstructionCacheRange (Address, Length);
  }
  return NULL;
}

/**
  Writes Back and Invalidates the entire data cache in cache coherency domain
  of the calling CPU.

  Writes Back and Invalidates the entire data cache in cache coherency domain
  of the calling CPU. This function guarantees that all dirty cache lines are
  written back to system memory, and also invalidates all the data cache lines
  in the cache coherency domain of the calling CPU.

**/
VOID
EFIAPI
WriteBackInvalidateDataCache (
  VOID
  )
{
  EMU_CACHE_THUNK_PROTOCOL   *EmuCache;
  EFI_STATUS                 Status;

  Status = DxeGetCacheThunkProtocol (&EmuCache);
  if (!EFI_ERROR (Status) && EmuCache != NULL) {
    EmuCache->WriteBackInvalidateDataCache ();
  }
}

/**
  Writes Back and Invalidates a range of data cache lines in the cache
  coherency domain of the calling CPU.

  Writes Back and Invalidate the data cache lines specified by Address and
  Length. If Address is not aligned on a cache line boundary, then entire data
  cache line containing Address is written back and invalidated. If Address +
  Length is not aligned on a cache line boundary, then the entire data cache
  line containing Address + Length -1 is written back and invalidated. This
  function may choose to write back and invalidate the entire data cache if
  that is more efficient than writing back and invalidating the specified
  range. If Length is 0, then no data cache lines are written back and
  invalidated. Address is returned.

  If Length is greater than (MAX_ADDRESS - Address + 1), then ASSERT().

  @param  Address The base address of the data cache lines to write back and
                  invalidate. If the CPU is in a physical addressing mode, then
                  Address is a physical address. If the CPU is in a virtual
                  addressing mode, then Address is a virtual address.
  @param  Length  The number of bytes to write back and invalidate from the
                  data cache.

  @return Address of cache invalidation.

**/
VOID *
EFIAPI
WriteBackInvalidateDataCacheRange (
  IN      VOID   *Address,
  IN      UINTN  Length
  )
{
  EMU_CACHE_THUNK_PROTOCOL   *EmuCache;
  EFI_STATUS                 Status;

  Status = DxeGetCacheThunkProtocol (&EmuCache);
  if (!EFI_ERROR (Status) && EmuCache != NULL) {
    return EmuCache->WriteBackInvalidateDataCacheRange (Address, Length);
  }
  return NULL;
}

/**
  Writes Back the entire data cache in cache coherency domain of the calling
  CPU.

  Writes Back the entire data cache in cache coherency domain of the calling
  CPU. This function guarantees that all dirty cache lines are written back to
  system memory. This function may also invalidate all the data cache lines in
  the cache coherency domain of the calling CPU.

**/
VOID
EFIAPI
WriteBackDataCache (
  VOID
  )
{
  EMU_CACHE_THUNK_PROTOCOL   *EmuCache;
  EFI_STATUS                 Status;

  Status = DxeGetCacheThunkProtocol (&EmuCache);
  if (!EFI_ERROR (Status) && EmuCache != NULL) {
    EmuCache->WriteBackDataCache ();
  }
}

/**
  Writes Back a range of data cache lines in the cache coherency domain of the
  calling CPU.

  Writes Back the data cache lines specified by Address and Length. If Address
  is not aligned on a cache line boundary, then entire data cache line
  containing Address is written back. If Address + Length is not aligned on a
  cache line boundary, then the entire data cache line containing Address +
  Length -1 is written back. This function may choose to write back the entire
  data cache if that is more efficient than writing back the specified range.
  If Length is 0, then no data cache lines are written back. This function may
  also invalidate all the data cache lines in the specified range of the cache
  coherency domain of the calling CPU. Address is returned.

  If Length is greater than (MAX_ADDRESS - Address + 1), then ASSERT().

  @param  Address The base address of the data cache lines to write back. If
                  the CPU is in a physical addressing mode, then Address is a
                  physical address. If the CPU is in a virtual addressing
                  mode, then Address is a virtual address.
  @param  Length  The number of bytes to write back from the data cache.

  @return Address of cache written in main memory.

**/
VOID *
EFIAPI
WriteBackDataCacheRange (
  IN      VOID   *Address,
  IN      UINTN  Length
  )
{
  EMU_CACHE_THUNK_PROTOCOL   *EmuCache;
  EFI_STATUS                 Status;

  Status = DxeGetCacheThunkProtocol (&EmuCache);
  if (!EFI_ERROR (Status) && EmuCache != NULL) {
    return EmuCache->WriteBackDataCacheRange (Address, Length);
  }
  return NULL;
}

/**
  Invalidates the entire data cache in cache coherency domain of the calling
  CPU.

  Invalidates the entire data cache in cache coherency domain of the calling
  CPU. This function must be used with care because dirty cache lines are not
  written back to system memory. It is typically used for cache diagnostics. If
  the CPU does not support invalidation of the entire data cache, then a write
  back and invalidate operation should be performed on the entire data cache.

**/
VOID
EFIAPI
InvalidateDataCache (
  VOID
  )
{
  EMU_CACHE_THUNK_PROTOCOL   *EmuCache;
  EFI_STATUS                 Status;

  Status = DxeGetCacheThunkProtocol (&EmuCache);
  if (!EFI_ERROR (Status) && EmuCache != NULL) {
    EmuCache->InvalidateDataCache ();
  }
}

/**
  Invalidates a range of data cache lines in the cache coherency domain of the
  calling CPU.

  Invalidates the data cache lines specified by Address and Length. If Address
  is not aligned on a cache line boundary, then entire data cache line
  containing Address is invalidated. If Address + Length is not aligned on a
  cache line boundary, then the entire data cache line containing Address +
  Length -1 is invalidated. This function must never invalidate any cache lines
  outside the specified range. If Length is 0, the no data cache lines are
  invalidated. Address is returned. This function must be used with care
  because dirty cache lines are not written back to system memory. It is
  typically used for cache diagnostics. If the CPU does not support
  invalidation of a data cache range, then a write back and invalidate
  operation should be performed on the data cache range.

  If Length is greater than (MAX_ADDRESS - Address + 1), then ASSERT().

  @param  Address The base address of the data cache lines to invalidate. If
                  the CPU is in a physical addressing mode, then Address is a
                  physical address. If the CPU is in a virtual addressing mode,
                  then Address is a virtual address.
  @param  Length  The number of bytes to invalidate from the data cache.

  @return Address.

**/
VOID *
EFIAPI
InvalidateDataCacheRange (
  IN      VOID   *Address,
  IN      UINTN  Length
  )
{
  EMU_CACHE_THUNK_PROTOCOL   *EmuCache;
  EFI_STATUS                 Status;

  Status = DxeGetCacheThunkProtocol (&EmuCache);
  if (!EFI_ERROR (Status) && EmuCache != NULL) {
    return EmuCache->InvalidateDataCacheRange (Address, Length);
  }
  return NULL;
}
