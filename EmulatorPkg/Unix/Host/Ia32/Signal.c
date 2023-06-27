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
  EFI_SYSTEM_CONTEXT_IA32   Ia32Context;
  ucontext_t                *context;

  ZeroMem ((VOID *)&Ia32Context, sizeof (Ia32Context));
  SignalInfo->SystemContext.SystemContextIa32 = &Ia32Context;
  context = (ucontext_t *) ucontext;

  Ia32Context.Cr2 = context->uc_mcontext.cr2;
  Ia32Context.Eflags = context->uc_mcontext.gregs[REG_EFL];
  Ia32Context.Eip = context->uc_mcontext.gregs[REG_EIP];
  Ia32Context.Gs = context->uc_mcontext.gregs[REG_GS];
  Ia32Context.Fs = context->uc_mcontext.gregs[REG_FS];
  Ia32Context.Es = context->uc_mcontext.gregs[REG_ES];
  Ia32Context.Ds = context->uc_mcontext.gregs[REG_DS];
  Ia32Context.Cs = context->uc_mcontext.gregs[REG_CS];
  Ia32Context.Ss = context->uc_mcontext.gregs[REG_SS];
  Ia32Context.Edi = context->uc_mcontext.gregs[REG_EDI];
  Ia32Context.Esi = context->uc_mcontext.gregs[REG_ESI];
  Ia32Context.Ebp = context->uc_mcontext.gregs[REG_EBP];
  Ia32Context.Esp = context->uc_mcontext.gregs[REG_ESP];
  Ia32Context.Ebx = context->uc_mcontext.gregs[REG_EBX];
  Ia32Context.Edx = context->uc_mcontext.gregs[REG_EDX];
  Ia32Context.Ecx = context->uc_mcontext.gregs[REG_ECX];
  Ia32Context.Eax = context->uc_mcontext.gregs[REG_EAX];

  ReverseGasketUint64Uint64 (SignalHandler, (VOID *)SignalNumber, (VOID *)SignalInfo);

  context->uc_mcontext.cr2 = Ia32Context.Cr2;
  context->uc_mcontext.gregs[REG_EFL] = Ia32Context.Eflags;
  context->uc_mcontext.gregs[REG_EIP] = Ia32Context.Eip;
  context->uc_mcontext.gregs[REG_GS] = Ia32Context.Gs;
  context->uc_mcontext.gregs[REG_FS] = Ia32Context.Fs;
  context->uc_mcontext.gregs[REG_ES] = Ia32Context.Es;
  context->uc_mcontext.gregs[REG_DS] = Ia32Context.Ds;
  context->uc_mcontext.gregs[REG_CS] = Ia32Context.Cs;
  context->uc_mcontext.gregs[REG_SS] = Ia32Context.Ss;
  context->uc_mcontext.gregs[REG_EDI] = Ia32Context.Edi;
  context->uc_mcontext.gregs[REG_ESI] = Ia32Context.Esi;
  context->uc_mcontext.gregs[REG_EBP] = Ia32Context.Ebp;
  context->uc_mcontext.gregs[REG_ESP] = Ia32Context.Esp;
  context->uc_mcontext.gregs[REG_EBX] = Ia32Context.Ebx;
  context->uc_mcontext.gregs[REG_EDX] = Ia32Context.Edx;
  context->uc_mcontext.gregs[REG_ECX] = Ia32Context.Ecx;
  context->uc_mcontext.gregs[REG_EAX] = Ia32Context.Eax;
}
