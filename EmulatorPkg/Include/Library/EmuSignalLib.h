/** @file
  Library that provides access to POSIX signals.

  Copyright (c) 2023, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#ifndef __EMU_SIGNAL_LIB__
#define __EMU_SIGNAL_LIB__

#include <Protocol/EmuSignal.h>

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
  );

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
  );

/**
  Get IsSigTermSignaled value.

  @retval        Value             current IsSigTermSignaled value.

**/
UINT32
EFIAPI
EmuGetIsSigTermSignaled (
  VOID
  );

/**
  Set IsSigTermSignaled value.

  @param[in]        Value             desired IsSigTermSignaled value.

**/
VOID
EFIAPI
EmuSetIsSigTermSignaled (
  IN      UINT32                       value
  );
#endif
