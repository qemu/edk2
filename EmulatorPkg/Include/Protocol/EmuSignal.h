/** @file
  Invoke syscalls for signal handling.

  Copyright (c) 2023, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#ifndef __EMU_SIGNAL__
#define __EMU_SIGNAL__

#include <Protocol/DebugSupport.h>

///
/// Signal Numbers
///
#define EMU_SIGILL      4     ///< Illegal Instruction
#define EMU_SIGBUS      7     ///< Incorrect memory access
#define EMU_SIGSEGV     11    ///< Invalid memory reference
#define EMU_SIGTERM     15    ///< Terminate process.

///
/// Signal Codes
///
#define EMU_ILL_ILLOPC  1     ///< Illegal Opcode
#define EMU_BUS_ADRALN  1     ///< Invalid address alignment
#define EMU_SEGV_ACCERR 2     ///< Invalid permissions for mapped object

///
/// Signal Information
///
typedef struct {
  UINTN                 SignalCode;         ///< A value indicating why this signal was sent.
  VOID                  *SignalAddress;     ///< Memory address that caused the fault.
  EFI_SYSTEM_CONTEXT    SystemContext;      ///< Machine Context at the time the signal was sent.
} EMU_SIGNAL_HANDLER_INFO;

typedef struct _EMU_SIGNAL_THUNK_PROTOCOL EMU_SIGNAL_THUNK_PROTOCOL;

/**
  Callback that is invoked when the OS sends a signal.

  @param[in]        SignalCode        A value indicating why this signal was sent.
  @param[in]        SignalAddress     Memory address that caused the fault.

  @retval None

**/
typedef
VOID
(EFIAPI *EMU_SIGNAL_HANDLER)(
  IN      UINTN                       SignalNumber,
  IN OUT  EMU_SIGNAL_HANDLER_INFO     *SignalInfo
  );

/**
  Registers a new signal handler for the given signal number.

  @param[in]        SignalNumber      The signal number to register a callback for.
  @param[in]        SignalHandler     The signal handler function.

  @retval EFI_SUCCESS                 The signal handler was registered successfully.
  @retval EFI_INVALID_PARAMETER       SignalHandler is NULL or SignalNumber is is not a invalid signal number.
  @retval EFI_DEVICE_ERROR            sigaction syscall returned an error.

**/
typedef
EFI_STATUS
(EFIAPI *EMU_SIGNAL_REGISTER_SIGNAL_HANDLER)(
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
typedef
EFI_STATUS
(EFIAPI *EMU_SIGNAL_UNREGISTER_SIGNAL_HANDLER)(
  IN      UINTN                       SignalNumber
  );


struct _EMU_SIGNAL_THUNK_PROTOCOL {
  EMU_SIGNAL_REGISTER_SIGNAL_HANDLER                          RegisterSignalHandler;
  EMU_SIGNAL_UNREGISTER_SIGNAL_HANDLER                        UnregisterSignalHandler;
};

#endif
