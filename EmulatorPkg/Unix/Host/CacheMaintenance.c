/** @file
  Invoke syscalls for cache maintenance on platforms that require kernel
  mode to manage the instruction cache. ARM and RISC-V for example.

  Copyright (c) 2023, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#include "Host.h"

extern UINTN              gSystemMemoryCount;
extern EMU_SYSTEM_MEMORY  *gSystemMemory;

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
EmuInvalidateInstructionCacheRange (
  IN      VOID   *Address,
  IN      UINTN  Length
  )
{
  __builtin___clear_cache ((char*)Address, ((char*)Address) + Length);

  return Address;
}

/**
  Invalidates the entire instruction cache in cache coherency domain of the
  calling CPU.

**/
VOID
EFIAPI
EmuInvalidateInstructionCache (
  VOID
  )
{
  UINTN   Index;

  if (gSystemMemory != NULL) {
    for (Index = 0; Index < gSystemMemoryCount; Index++) {
      if (gSystemMemory[Index].Memory != 0) {
        EmuInvalidateInstructionCacheRange ((VOID *)(UINTN)gSystemMemory[Index].Memory, gSystemMemory[Index].Size);
      }
    }
  }
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
EmuWriteBackInvalidateDataCache (
  VOID
  )
{
  EmuInvalidateInstructionCache ();
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
EmuWriteBackInvalidateDataCacheRange (
  IN      VOID   *Address,
  IN      UINTN  Length
  )
{
  return EmuInvalidateInstructionCacheRange (Address, Length);
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
EmuWriteBackDataCache (
  VOID
  )
{
  EmuWriteBackInvalidateDataCache ();
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
EmuWriteBackDataCacheRange (
  IN      VOID   *Address,
  IN      UINTN  Length
  )
{
  return EmuWriteBackInvalidateDataCacheRange (Address, Length);
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
EmuInvalidateDataCache (
  VOID
  )
{
  EmuWriteBackInvalidateDataCache ();
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
EmuInvalidateDataCacheRange (
  IN      VOID   *Address,
  IN      UINTN  Length
  )
{
  return EmuWriteBackInvalidateDataCacheRange (Address, Length);
}

EMU_CACHE_THUNK_PROTOCOL  gEmuCacheThunk = {
  EmuInvalidateInstructionCache,
  EmuInvalidateInstructionCacheRange,
  EmuWriteBackInvalidateDataCache,
  EmuWriteBackInvalidateDataCacheRange,
  EmuWriteBackDataCache,
  EmuWriteBackDataCacheRange,
  EmuInvalidateDataCache,
  EmuInvalidateDataCacheRange
};

EFI_STATUS
EFIAPI
CacheOpen (
  IN  EMU_IO_THUNK_PROTOCOL  *This
  )
{
  if (This->Instance != 0) {
    // Only single instance is supported
    return EFI_NOT_FOUND;
  }

  This->Interface = &gEmuCacheThunk;

  return EFI_SUCCESS;
}

EFI_STATUS
EFIAPI
CacheClose (
  IN  EMU_IO_THUNK_PROTOCOL  *This
  )
{
  return EFI_SUCCESS;
}

EMU_IO_THUNK_PROTOCOL  gCacheThunkIo = {
  &gEmuCacheThunkProtocolGuid,
  NULL,
  NULL,
  0,
  CacheOpen,
  CacheClose,
  NULL
};
