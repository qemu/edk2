/** @file
  ARM specific code for signal handling.

  Copyright (c) 2023, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

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
  EFI_SYSTEM_CONTEXT_ARM    ArmContext;
  ucontext_t                *context;

  ZeroMem ((VOID *)&ArmContext, sizeof (ArmContext));
  SignalInfo->SystemContext.SystemContextArm = &ArmContext;
  context = (ucontext_t *) ucontext;

  ArmContext.R0 = context->uc_mcontext.arm_r0;
  ArmContext.R1 = context->uc_mcontext.arm_r1;
  ArmContext.R2 = context->uc_mcontext.arm_r2;
  ArmContext.R3 = context->uc_mcontext.arm_r3;
  ArmContext.R4 = context->uc_mcontext.arm_r4;
  ArmContext.R5 = context->uc_mcontext.arm_r5;
  ArmContext.R6 = context->uc_mcontext.arm_r6;
  ArmContext.R7 = context->uc_mcontext.arm_r7;
  ArmContext.R8 = context->uc_mcontext.arm_r8;
  ArmContext.R9 = context->uc_mcontext.arm_r9;
  ArmContext.R10 = context->uc_mcontext.arm_r10;
  ArmContext.R11 = context->uc_mcontext.arm_fp;
  ArmContext.R12 = context->uc_mcontext.arm_ip;
  ArmContext.SP = context->uc_mcontext.arm_sp;
  ArmContext.LR = context->uc_mcontext.arm_lr;
  ArmContext.PC = context->uc_mcontext.arm_pc;
  ArmContext.CPSR = context->uc_mcontext.arm_cpsr;

  ReverseGasketUint64Uint64 (SignalHandler, (VOID *)SignalNumber, (VOID *)SignalInfo);

  context->uc_mcontext.arm_r0 = ArmContext.R0;
  context->uc_mcontext.arm_r1 = ArmContext.R1;
  context->uc_mcontext.arm_r2 = ArmContext.R2;
  context->uc_mcontext.arm_r3 = ArmContext.R3;
  context->uc_mcontext.arm_r4 = ArmContext.R4;
  context->uc_mcontext.arm_r5 = ArmContext.R5;
  context->uc_mcontext.arm_r6 = ArmContext.R6;
  context->uc_mcontext.arm_r7 = ArmContext.R7;
  context->uc_mcontext.arm_r8 = ArmContext.R8;
  context->uc_mcontext.arm_r9 = ArmContext.R9;
  context->uc_mcontext.arm_r10 = ArmContext.R10;
  context->uc_mcontext.arm_fp = ArmContext.R11;
  context->uc_mcontext.arm_ip = ArmContext.R12;
  context->uc_mcontext.arm_sp = ArmContext.SP;
  context->uc_mcontext.arm_lr = ArmContext.LR;
  context->uc_mcontext.arm_pc = ArmContext.PC;
  context->uc_mcontext.arm_cpsr = ArmContext.CPSR;
}
