/** @file
  SIGSEGV handler for x86 Emulator invocation

  Copyright (c) 2023, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#include "X86Emulator.h"

/**
  Callback that is invoked when the OS sends the SIGSEGV signal,
  which may indicate that we are attempting to execute x86 code.

  @param[in]        SignalCode        A value indicating why this signal was sent.
  @param[in]        SignalAddress     Memory address that caused the fault.

  @retval None

**/
VOID
EFIAPI
X86EmulatorSignalHandler (
  IN      UINTN                       SignalNumber,
  IN OUT  EMU_SIGNAL_HANDLER_INFO     *SignalInfo
  )
{
  UINT32                    Args[8];
  X86_IMAGE_RECORD          *Record;
  EFI_SYSTEM_CONTEXT_ARM    *ArmContext;

  if (SignalNumber == EMU_SIGSEGV && SignalInfo->SignalCode == EMU_SEGV_ACCERR) {
    Record = FindImageRecord ((EFI_PHYSICAL_ADDRESS) (UINTN) SignalInfo->SignalAddress);
    if (Record != NULL) {
      ArmContext = SignalInfo->SystemContext.SystemContextArm;
      Args[0] = ArmContext->R0;
      Args[1] = ArmContext->R1;
      Args[2] = ArmContext->R3;
      Args[3] = ArmContext->R4;
      Args[4] = *((UINT32 *)ArmContext->SP);
      Args[5] = *((UINT32 *)(ArmContext->SP + 4));
      Args[6] = *((UINT32 *)(ArmContext->SP + 8));
      Args[7] = *((UINT32 *)(ArmContext->SP + 12));
      ArmContext->R0 = (UINT32)X86EmulatorVmEntry (ArmContext->PC, &Args[0], Record, ArmContext->LR);
      return;
    }
  }
}
