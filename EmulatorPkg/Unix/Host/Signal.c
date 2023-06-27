/** @file
  Invoke syscalls for signal handling.

  Copyright (c) 2023, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#include "Host.h"

STATIC EMU_SIGNAL_HANDLER  mSigIllHandler = NULL;
STATIC EMU_SIGNAL_HANDLER  mSigBusHandler = NULL;
STATIC EMU_SIGNAL_HANDLER  mSigSegvHandler = NULL;

/**
  Converts from emulator thunk signal numbers to UNIX signal numbers. Emulator
  thunk signal numbers exist so that the only place platform-specific UNIX
  header files need to be consumed is in the Host application.

  @param[in]        SignalNumber      The emulator thunk signal number.
  @param[out]       signum            The UNIX signal number.
  @param[out]       SignalHandler     If non-NULL returns the currently registered handler for the signal.

  @retval EFI_SUCCESS                 The signal number was converted successfully.
  @retval EFI_INVALID_PARAMETER       Unknown signal number.

**/
STATIC
EFI_STATUS
EmuSignalNumberToUnix (
  IN      UINTN                       SignalNumber,
  OUT     int                         *signum,
  OUT     EMU_SIGNAL_HANDLER          **SignalHandler    OPTIONAL
  )
{
  switch (SignalNumber) {
    case EMU_SIGILL:
      *signum = SIGILL;
      if (SignalHandler != NULL) {
        *SignalHandler = &mSigIllHandler;
      }
      break;
    case EMU_SIGBUS:
      *signum = SIGBUS;
      if (SignalHandler != NULL) {
        *SignalHandler = &mSigBusHandler;
      }
      break;
    case EMU_SIGSEGV:
      *signum = SIGSEGV;
      if (SignalHandler != NULL) {
        *SignalHandler = &mSigSegvHandler;
      }
      break;
    default:
      return EFI_INVALID_PARAMETER;
  }
  return EFI_SUCCESS;
}

/**
  Converts from UNIX signal numbers to emulator thunk signal numbers. Emulator
  thunk signal numbers exist so that the only place platform-specific UNIX
  header files need to be consumed is in the Host application.

  @param[in]        signum            The UNIX signal number.
  @param[in]        si_code           A value indicating why this signal was sent.
  @param[out]       SignalNumber      The emulator thunk signal number.
  @param[out]       SignalCode        A value indicating why this signal was sent.
  @param[out]       SignalHandler     If non-NULL returns the currently registered handler for the signal.

  @retval EFI_SUCCESS                 The signal number was converted successfully.
  @retval EFI_INVALID_PARAMETER       Unknown signal number.

**/
STATIC
EFI_STATUS
UnixSignalNumberToEmu (
  IN      int                         signum,
  IN      int                         si_code,
  OUT     UINTN                       *SignalNumber,
  OUT     UINTN                       *SignalCode,
  OUT     EMU_SIGNAL_HANDLER          **SignalHandler    OPTIONAL
  )
{
  switch (signum) {
    case SIGILL:
      *SignalNumber = EMU_SIGILL;
      switch (si_code) {
        case ILL_ILLOPC:
          *SignalCode = EMU_ILL_ILLOPC;
          break;
        default:
          *SignalCode = 0;
      }
      if (SignalHandler != NULL) {
        *SignalHandler = &mSigIllHandler;
      }
      break;
    case SIGBUS:
      *SignalNumber = EMU_SIGBUS;
      switch (si_code) {
        case BUS_ADRALN:
          *SignalCode = EMU_BUS_ADRALN;
          break;
        default:
          *SignalCode = 0;
      }
      if (SignalHandler != NULL) {
        *SignalHandler = &mSigBusHandler;
      }
      break;
    case SIGSEGV:
      *SignalNumber = EMU_SIGSEGV;
      switch (si_code) {
        case SEGV_ACCERR:
          *SignalCode = EMU_SEGV_ACCERR;
          break;
        default:
          *SignalCode = 0;
      }
      if (SignalHandler != NULL) {
        *SignalHandler = &mSigSegvHandler;
      }
      break;
    default:
      return EFI_INVALID_PARAMETER;
  }
  return EFI_SUCCESS;
}

/**
  Invoked by the Linux kernel to handle a signal.

  @param[in]        sig               The signal number that caused the invocation of the handler.
  @param[in]        info              A structure containing further information about the signal.
  @param[in]        ucontext          Signal context information, not used.

  @retval None

**/
void
sec_signal_handler (
  int                                 sig,
  siginfo_t                           *info,
  void                                *ucontext
  )
{
  EMU_SIGNAL_HANDLER_INFO   SignalInfo;
  UINTN                     SignalNumber;
  EFI_STATUS                Status;
  EMU_SIGNAL_HANDLER        *Handler;

  //
  // This data structure conversion code keeps all usage of Linux data
  // structures and header files confined to the Host application.
  //
  ZeroMem ((VOID *)&SignalInfo, sizeof (SignalInfo));
  Handler = NULL;
  SignalInfo.SignalAddress = info->si_addr;
  Status = UnixSignalNumberToEmu (sig, info->si_code, &SignalNumber, &(SignalInfo.SignalCode), &Handler);
  if (EFI_ERROR (Status)) {
    return;
  }

  //
  // Invoke the EFIAPI handler
  //
  if (*Handler != NULL) {
    SecInvokeSignalHandler (*Handler, SignalNumber, &SignalInfo, ucontext);
  }
}

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
  )
{
  struct sigaction    act;
  EFI_STATUS          Status;
  int                 signum;
  EMU_SIGNAL_HANDLER  *InstalledHandler;

  sigemptyset (&act.sa_mask);

  // Block SIGALRM so the emulated timer interrupts in EmuThunk.c don't preempt the handler
  sigaddset (&act.sa_mask, SIGALRM);
  act.sa_flags = SA_SIGINFO;
  act.sa_sigaction = sec_signal_handler;
  if (SignalHandler == NULL) {
    return EFI_INVALID_PARAMETER;
  }
  Status = EmuSignalNumberToUnix (SignalNumber, &signum, &InstalledHandler);
  if (EFI_ERROR (Status)) {
    return Status;
  }
  if (*InstalledHandler == NULL) {
    if (sigaction (signum, &act, NULL) != 0) {
      return EFI_DEVICE_ERROR;
    }
  }
  *InstalledHandler = SignalHandler;
  return EFI_SUCCESS;
}

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
  )
{
  EFI_STATUS          Status;
  int                 signum;
  EMU_SIGNAL_HANDLER  *InstalledHandler;

  Status = EmuSignalNumberToUnix (SignalNumber, &signum, &InstalledHandler);
  if (EFI_ERROR (Status)) {
    return Status;
  }
  if(*InstalledHandler == NULL) {
    return EFI_INVALID_PARAMETER;
  }
  if (signal (signum, SIG_DFL) == SIG_ERR) {
    return EFI_DEVICE_ERROR;
  }
  *InstalledHandler = NULL;
  return EFI_SUCCESS;
}

EMU_SIGNAL_THUNK_PROTOCOL  gEmuSignalThunk = {
  GasketRegisterSignalHandler,
  GasketUnregisterSignalHandler
};

EFI_STATUS
SignalOpen (
  IN  EMU_IO_THUNK_PROTOCOL  *This
  )
{
  if (This->Instance != 0) {
    // Only single instance is supported
    return EFI_NOT_FOUND;
  }

  This->Interface = &gEmuSignalThunk;

  return EFI_SUCCESS;
}

EFI_STATUS
SignalClose (
  IN  EMU_IO_THUNK_PROTOCOL  *This
  )
{
  return EFI_SUCCESS;
}

EMU_IO_THUNK_PROTOCOL  gSignalThunkIo = {
  &gEmuSignalThunkProtocolGuid,
  NULL,
  NULL,
  0,
  GasketSignalOpen,
  GasketSignalClose,
  NULL
};
