/** @file
  Instance of ARM MMU Library using emulator thunks to OS syscalls

  Copyright (c) 2023, Intel Corporation. All rights reserved.<BR>

  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#include <PiPei.h>
#include <Library/BaseLib.h>
#include <Library/DebugLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/PeiServicesLib.h>
#include <Library/ArmMmuLib.h>

#include <Ppi/EmuThunk.h>
#include <Protocol/EmuThunk.h>

EFI_STATUS
EFIAPI
PeiGetEmuThunkProtocol (
  OUT      EMU_THUNK_PROTOCOL   **EmuThunk
  )
{
  EMU_THUNK_PPI           *ThunkPpi;
  EFI_STATUS              Status;

  //
  // Locate Unix Thunk Ppi
  //
  Status = PeiServicesLocatePpi (
             &gEmuThunkPpiGuid,
             0,
             NULL,
             (VOID **)&ThunkPpi
             );
  if (EFI_ERROR (Status)) {
    return Status;
  }
  //
  // Get the Unix Thunk Protocol so we can retrieve I/O Thunk Protocols
  //
  *EmuThunk = (EMU_THUNK_PROTOCOL *)ThunkPpi->Thunk ();
  if (*EmuThunk == NULL) {
    return EFI_NOT_FOUND;
  }

  return EFI_SUCCESS;
}

EFI_STATUS
ArmSetMemoryAttributes (
  IN EFI_PHYSICAL_ADDRESS  BaseAddress,
  IN UINT64                Length,
  IN UINT64                Attributes
  )
{
  EMU_THUNK_PROTOCOL    *EmuThunk;
  EFI_STATUS            Status;

  Status = PeiGetEmuThunkProtocol (&EmuThunk);
  if (EFI_ERROR (Status)) {
    return Status;
  }

  return EmuThunk->SetMemoryAttributes (BaseAddress, Length, Attributes);
}

EFI_STATUS
EFIAPI
ArmSetMemoryRegionNoExec (
  IN  EFI_PHYSICAL_ADDRESS  BaseAddress,
  IN  UINT64                Length
  )
{
  return ArmSetMemoryAttributes (BaseAddress, Length, EFI_MEMORY_XP);
}

EFI_STATUS
EFIAPI
ArmClearMemoryRegionNoExec (
  IN  EFI_PHYSICAL_ADDRESS  BaseAddress,
  IN  UINT64                Length
  )
{
  return ArmSetMemoryAttributes (BaseAddress, Length, 0);
}

EFI_STATUS
EFIAPI
ArmSetMemoryRegionReadOnly (
  IN  EFI_PHYSICAL_ADDRESS  BaseAddress,
  IN  UINT64                Length
  )
{
  return ArmSetMemoryAttributes (BaseAddress, Length, EFI_MEMORY_RO);
}

EFI_STATUS
EFIAPI
ArmClearMemoryRegionReadOnly (
  IN  EFI_PHYSICAL_ADDRESS  BaseAddress,
  IN  UINT64                Length
  )
{
  return ArmSetMemoryAttributes (BaseAddress, Length, 0);
}

EFI_STATUS
EFIAPI
ArmConfigureMmu (
  IN  ARM_MEMORY_REGION_DESCRIPTOR  *MemoryTable,
  OUT VOID                          **TranslationTableBase OPTIONAL,
  OUT UINTN                         *TranslationTableSize  OPTIONAL
  )
{
  return EFI_UNSUPPORTED;
}

VOID
EFIAPI
ArmReplaceLiveTranslationEntry (
  IN  UINT64   *Entry,
  IN  UINT64   Value,
  IN  UINT64   RegionStart,
  IN  BOOLEAN  DisableMmu
  )
{
  return;
}
