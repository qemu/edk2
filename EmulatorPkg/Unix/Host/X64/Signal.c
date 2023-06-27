/** @file
  X64 specific code for signal handling.

  Copyright (c) 2023, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#define _GNU_SOURCE
#include "Host.h"

/**
  Invokes the given signal handler.

  @param[in]        SignalHandler     The signal handler function to be invoked.
  @param[in]        SignalNumber      The signal number that caused the invocation of the handler.
  @param[in]        SignalInfo        Signal information.
  @param[in, out]   ucontext          Machine context data provided by OS.

  @retval None

**/
VOID
SecInvokeSignalHandler (
  IN      EMU_SIGNAL_HANDLER        SignalHandler,
  IN      UINTN                     SignalNumber,
  IN      EMU_SIGNAL_HANDLER_INFO   *SignalInfo,
  IN OUT  void                      *ucontext
  )
{
  EFI_SYSTEM_CONTEXT_X64    X64Context;
  ucontext_t                *context;

  ZeroMem ((VOID *)&X64Context, sizeof (X64Context));
  SignalInfo->SystemContext.SystemContextX64 = &X64Context;
  context = (ucontext_t *) ucontext;

  X64Context.Cr2 = context->uc_mcontext.gregs[REG_CR2];
  X64Context.Rflags = context->uc_mcontext.gregs[REG_EFL];
  X64Context.Rip = context->uc_mcontext.gregs[REG_RIP];
  X64Context.Cs = (context->uc_mcontext.gregs[REG_CSGSFS] & 0xFFFF);
  X64Context.Fs = ((context->uc_mcontext.gregs[REG_CSGSFS] >> 32) & 0xFFFF);
  X64Context.Gs = ((context->uc_mcontext.gregs[REG_CSGSFS] >> 16) & 0xFFFF);
  X64Context.Rdi = context->uc_mcontext.gregs[REG_RDI];
  X64Context.Rsi = context->uc_mcontext.gregs[REG_RSI];
  X64Context.Rbp = context->uc_mcontext.gregs[REG_RBP];
  X64Context.Rsp = context->uc_mcontext.gregs[REG_RSP];
  X64Context.Rbx = context->uc_mcontext.gregs[REG_RBX];
  X64Context.Rdx = context->uc_mcontext.gregs[REG_RDX];
  X64Context.Rcx = context->uc_mcontext.gregs[REG_RCX];
  X64Context.Rax = context->uc_mcontext.gregs[REG_RAX];
  X64Context.R8 = context->uc_mcontext.gregs[REG_R8];
  X64Context.R9 = context->uc_mcontext.gregs[REG_R9];
  X64Context.R10 = context->uc_mcontext.gregs[REG_R10];
  X64Context.R11 = context->uc_mcontext.gregs[REG_R11];
  X64Context.R12 = context->uc_mcontext.gregs[REG_R12];
  X64Context.R13 = context->uc_mcontext.gregs[REG_R13];
  X64Context.R14 = context->uc_mcontext.gregs[REG_R14];
  X64Context.R15 = context->uc_mcontext.gregs[REG_R15];

  ReverseGasketUint64Uint64 (SignalHandler, (VOID *)SignalNumber, (VOID *)SignalInfo);

  context->uc_mcontext.gregs[REG_CR2] = X64Context.Cr2;
  context->uc_mcontext.gregs[REG_EFL] = X64Context.Rflags;
  context->uc_mcontext.gregs[REG_RIP] = X64Context.Rip;
  context->uc_mcontext.gregs[REG_CSGSFS]  = (X64Context.Cs & 0xFFFF)          |
                                            (16 << (X64Context.Gs & 0xFFFF))  |
                                            (32 << (X64Context.Fs & 0xFFFF));
  context->uc_mcontext.gregs[REG_RDI] = X64Context.Rdi;
  context->uc_mcontext.gregs[REG_RSI] = X64Context.Rsi;
  context->uc_mcontext.gregs[REG_RBP] = X64Context.Rbp;
  context->uc_mcontext.gregs[REG_RSP] = X64Context.Rsp;
  context->uc_mcontext.gregs[REG_RBX] = X64Context.Rbx;
  context->uc_mcontext.gregs[REG_RDX] = X64Context.Rdx;
  context->uc_mcontext.gregs[REG_RCX] = X64Context.Rcx;
  context->uc_mcontext.gregs[REG_RAX] = X64Context.Rax;
  context->uc_mcontext.gregs[REG_R8] = X64Context.R8;
  context->uc_mcontext.gregs[REG_R9] = X64Context.R9;
  context->uc_mcontext.gregs[REG_R10] = X64Context.R10;
  context->uc_mcontext.gregs[REG_R11] = X64Context.R11;
  context->uc_mcontext.gregs[REG_R12] = X64Context.R12;
  context->uc_mcontext.gregs[REG_R13] = X64Context.R13;
  context->uc_mcontext.gregs[REG_R14] = X64Context.R14;
  context->uc_mcontext.gregs[REG_R15] = X64Context.R15;
}
