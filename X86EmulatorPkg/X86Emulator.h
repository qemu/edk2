//
// Copyright (c) 2017, Linaro, Ltd. <ard.biesheuvel@linaro.org>
// Copyright (c) 2023, Intel Corporation. All rights reserved.<BR>
//
// This library is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public
// License as published by the Free Software Foundation; either
// version 2 of the License, or (at your option) any later version.
//

#include <Library/BaseLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/DebugLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/PeCoffLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/UefiDriverEntryPoint.h>
#include <Library/EmuSignalLib.h>

#include <Protocol/Cpu.h>
#include <Protocol/CpuIo2.h>
#include <Protocol/DebugSupport.h>
#include <Protocol/PeCoffImageEmulator.h>

typedef struct {
  LIST_ENTRY            Link;
  EFI_PHYSICAL_ADDRESS  ImageBase;
  UINT64                ImageSize;
} X86_IMAGE_RECORD;

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
  );

UINT64
X86EmulatorVmEntry (
  IN  UINTN               Pc,
  IN  UINTN               *Args,
  IN  X86_IMAGE_RECORD    *Record,
  IN  UINTN               Lr
  );

X86_IMAGE_RECORD*
EFIAPI
FindImageRecord (
  IN  EFI_PHYSICAL_ADDRESS    Address
  );

#define CODE_GEN_BUFFER_PAGES   (8 * 1024)

extern UINT8 *static_code_gen_buffer;

void dump_x86_state(void);
