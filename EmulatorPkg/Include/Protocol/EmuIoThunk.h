/** @file
  Emulator Thunk to abstract OS services from pure EFI code

  Copyright (c) 2010 - 2011, Apple Inc. All rights reserved.<BR>
  Copyright (c) 2023, Intel Corporation. All rights reserved.<BR>

  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#ifndef __EMU_IO_THUNK__
#define __EMU_IO_THUNK__

typedef struct _EMU_IO_THUNK_PROTOCOL EMU_IO_THUNK_PROTOCOL;

typedef
EFI_STATUS
(EFIAPI *EMU_IO_THUNK_PROTOCOL_CLOSE_OPEN)(
  IN  EMU_IO_THUNK_PROTOCOL   *This
  );

typedef
EFI_STATUS
(EFIAPI *EMU_IO_THUNK_PROTOCOL_CLOSE_CLOSE)(
  IN  EMU_IO_THUNK_PROTOCOL   *This
  );

struct _EMU_IO_THUNK_PROTOCOL {
  EFI_GUID                             *Protocol;
  VOID                                 *Interface; /// Only be valid after Open() is called
  CHAR16                               *ConfigString;
  UINT16                               Instance;
  EMU_IO_THUNK_PROTOCOL_CLOSE_OPEN     Open;
  EMU_IO_THUNK_PROTOCOL_CLOSE_CLOSE    Close;
  VOID                                 *Private;   /// Used by implementation
};

extern EFI_GUID  gEmuIoThunkProtocolGuid;

#endif
