/** @file
  Library that provides access to POSIX signals.

  Copyright (c) 2023, Intel Corporation. All rights reserved.<BR>

  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#include <Uefi/UefiBaseType.h>
#include <Pi/PiHob.h>
#include <Library/BaseLib.h>
#include <Library/DebugLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/HobLib.h>

#include <Library/EmuThunkLib.h>
#include <Library/EmuSignalLib.h>

#include <Protocol/EmuIoThunk.h>

EMU_SIGNAL_THUNK_PROTOCOL   *gEmuSignal = NULL;

/**
  The constructor function caches the pointer of EMU signal protocol.

  @param  ImageHandle   The firmware allocated handle for the EFI image.
  @param  SystemTable   A pointer to the EFI System Table.

  @retval EFI_SUCCESS   The constructor always returns EFI_SUCCESS.

**/
EFI_STATUS
EFIAPI
DxeEmuSignalLibConstructor (
  IN EFI_HANDLE        ImageHandle,
  IN EFI_SYSTEM_TABLE  *SystemTable
  )
{
  EMU_IO_THUNK_PROTOCOL   *EmuIoThunk;
  EFI_STATUS              Status;

  //
  // Locate the I/O Thunk Protocol representing the Cache Thunk Protocol
  //
  for (Status = EFI_SUCCESS, EmuIoThunk = NULL; !EFI_ERROR (Status); ) {
    Status = gEmuThunk->GetNextProtocol (FALSE, &EmuIoThunk);
    if (EFI_ERROR (Status)) {
      break;
    }

    if (EmuIoThunk->Instance == 0) {
      if (CompareGuid (EmuIoThunk->Protocol, &gEmuSignalThunkProtocolGuid)) {
        //
        // Open the I/O Thunk Protocol to retrieve the Cache Thunk Protocol
        //
        Status = EmuIoThunk->Open (EmuIoThunk);
        if (EFI_ERROR (Status)) {
            return Status;
        }
        gEmuSignal = (EMU_SIGNAL_THUNK_PROTOCOL *) EmuIoThunk->Interface;
        return EFI_SUCCESS;
      }
    }
  }

  return EFI_NOT_FOUND;
}

/**
  Registers a new signal handler for the given signal number.

  @param[in]        SignalNumber      The signal number to register a callback for.
  @param[in]        SignalHandler     The signal handler function.

  @retval EFI_SUCCESS                 The signal handler was registered successfully.
  @retval EFI_INVALID_PARAMETER       SignalHandler is NULL or SignalNumber is is not a invalid signal number.
  @retval EFI_DEVICE_ERROR            sigaction syscall returned an error.
  @retval EFI_NOT_FOUND               EMU_SIGNAL_THUNK_PROTOCOL does not exist.

**/
EFI_STATUS
EFIAPI
EmuRegisterSignalHandler (
  IN      UINTN                       SignalNumber,
  IN      EMU_SIGNAL_HANDLER          SignalHandler
  )
{
  ASSERT (gEmuSignal != NULL);
  return gEmuSignal->RegisterSignalHandler (SignalNumber, SignalHandler);
}

/**
  Unregisters a previously registered signal handler.

  @param[in]        SignalNumber      The signal number to unregister the callback for.

  @retval EFI_SUCCESS                 The signal handler was unregistered successfully.
  @retval EFI_INVALID_PARAMETER       SignalNumber does not have a handler registered or is not a invalid signal number.
  @retval EFI_DEVICE_ERROR            signal syscall returned an error.
  @retval EFI_NOT_FOUND               EMU_SIGNAL_THUNK_PROTOCOL does not exist.

**/
EFI_STATUS
EFIAPI
EmuUnregisterSignalHandler (
  IN      UINTN                       SignalNumber
  )
{
  ASSERT (gEmuSignal != NULL);
  return gEmuSignal->UnregisterSignalHandler (SignalNumber);
}

/**
  Get IsSigTermSignaled value.

  @retval        Value             current IsSigTermSignaled value.

**/
UINT32
EFIAPI
EmuGetIsSigTermSignaled (
  VOID
  )
{
  UINT32           value;

  ASSERT (gEmuSignal != NULL);
  value = gEmuSignal->GetIsSigTermSignaled();

  return value;
}

/**
  Set IsSigTermSignaled value.

  @param[in]        Value             desired IsSigTermSignaled value.

**/
VOID
EFIAPI
EmuSetIsSigTermSignaled (
  IN      UINT32                       value
  )
{
  ASSERT (gEmuSignal != NULL);
  gEmuSignal->SetIsSigTermSignaled(value);

  return;
}