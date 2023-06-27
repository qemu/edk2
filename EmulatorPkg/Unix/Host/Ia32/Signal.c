/** @file
  IA32 specific code for signal handling.

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
  EFI_SYSTEM_CONTEXT_IA32   IA32Context;
  ucontext_t                *context;

  ZeroMem ((VOID *)&IA32Context, sizeof (IA32Context));
  SignalInfo->SystemContext.SystemContextIA32 = &IA32Context;
  context = (ucontext_t *) ucontext;

  IA32Context.Cr2 = context->uc_mcontext.cr2;
  IA32Context.Eflags = context->uc_mcontext.gregs[REG_EFL];
  IA32Context.Eip = context->uc_mcontext.gregs[REG_EIP];
  IA32Context.Gs = context->uc_mcontext.gregs[REG_GS];
  IA32Context.Fs = context->uc_mcontext.gregs[REG_FS];
  IA32Context.Es = context->uc_mcontext.gregs[REG_ES];
  IA32Context.Ds = context->uc_mcontext.gregs[REG_DS];
  IA32Context.Cs = context->uc_mcontext.gregs[REG_CS];
  IA32Context.Ss = context->uc_mcontext.gregs[REG_SS];
  IA32Context.Edi = context->uc_mcontext.gregs[REG_EDI];
  IA32Context.Esi = context->uc_mcontext.gregs[REG_ESI];
  IA32Context.Ebp = context->uc_mcontext.gregs[REG_EBP];
  IA32Context.Esp = context->uc_mcontext.gregs[REG_ESP];
  IA32Context.Ebx = context->uc_mcontext.gregs[REG_EBX];
  IA32Context.Edx = context->uc_mcontext.gregs[REG_EDX];
  IA32Context.Ecx = context->uc_mcontext.gregs[REG_ECX];
  IA32Context.Eax = context->uc_mcontext.gregs[REG_EAX];

  ReverseGasketUint64Uint64 (SignalHandler, (VOID *)SignalNumber, (VOID *)SignalInfo);

  context->uc_mcontext.cr2 = IA32Context.Cr2;
  context->uc_mcontext.gregs[REG_EFL] = IA32Context.Eflags;
  context->uc_mcontext.gregs[REG_EIP] = IA32Context.Eip;
  context->uc_mcontext.gregs[REG_GS] = IA32Context.Gs;
  context->uc_mcontext.gregs[REG_FS] = IA32Context.Fs;
  context->uc_mcontext.gregs[REG_ES] = IA32Context.Es;
  context->uc_mcontext.gregs[REG_DS] = IA32Context.Ds;
  context->uc_mcontext.gregs[REG_CS] = IA32Context.Cs;
  context->uc_mcontext.gregs[REG_SS] = IA32Context.Ss;
  context->uc_mcontext.gregs[REG_EDI] = IA32Context.Edi;
  context->uc_mcontext.gregs[REG_ESI] = IA32Context.Esi;
  context->uc_mcontext.gregs[REG_EBP] = IA32Context.Ebp;
  context->uc_mcontext.gregs[REG_ESP] = IA32Context.Esp;
  context->uc_mcontext.gregs[REG_EBX] = IA32Context.Ebx;
  context->uc_mcontext.gregs[REG_EDX] = IA32Context.Edx;
  context->uc_mcontext.gregs[REG_ECX] = IA32Context.Ecx;
  context->uc_mcontext.gregs[REG_EAX] = IA32Context.Eax;
}
