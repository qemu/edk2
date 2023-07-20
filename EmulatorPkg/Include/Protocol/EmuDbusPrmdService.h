/** @file
  Provides the mailbox for invoking the PRM Dispatch API from dbus

  Copyright (c) 2023, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#ifndef __EMU_DBUS_PRMD_SERVICE__
#define __EMU_DBUS_PRMD_SERVICE__

typedef struct {
  UINTN         Function;
  EFI_STATUS    Status;
  UINT8         Data[1];
} EMU_DBUS_PRMD_SERVICE_COMMUNICATE_HEADER;

//
// The payload for this function is EMU_DBUS_PRMD_SERVICE_COMMUNICATE_INSTALL_PACKAGE.
//
#define EMU_DBUS_PRMD_SERVICE_FUNCTION_INSTALL_PACKAGE  1
//
// The payload for this function is .
//
#define EMU_DBUS_PRMD_SERVICE_FUNCTION_ENUMERATE_HANDLERS  2
//
// The payload for this function is EMU_DBUS_PRMD_SERVICE_COMMUNICATE_INVOKE_HANDLER.
//
#define EMU_DBUS_PRMD_SERVICE_FUNCTION_INVOKE_HANDLER  3
//
// The payload for this function is .
//
#define EMU_DBUS_PRMD_SERVICE_FUNCTION_UNINSTALL_PRM_MODULE  4
//
// The payload for this function is .
//
#define EMU_DBUS_PRMD_SERVICE_FUNCTION_GET_HANDLER_DEBUG_INFO  5

typedef struct {
  VOID          *PrmPackage;
  UINT32        PrmPackageLength;
} EMU_DBUS_PRMD_SERVICE_COMMUNICATE_INSTALL_PACKAGE;

typedef struct {
  EFI_GUID      HandlerGuid;
  EFI_STATUS    PrmHandlerReturnStatus;
  VOID          *ParameterBuffer;
  UINT8         ParameterBufferLength;
} EMU_DBUS_PRMD_SERVICE_COMMUNICATE_INVOKE_HANDLER;

typedef struct {
  EFI_GUID      HandlerGuid;
  VOID          *AcpiParameterBuffer;
  UINT32        AcpiParameterBufferLength;
  CHAR8         *HandlerName;
} EMU_DBUS_PRMD_SERVICE_COMMUNICATE_GET_HANDLER_DEBUG_INFO;

typedef struct _EMU_DBUS_PRMD_SERVICE_THUNK_PROTOCOL EMU_DBUS_PRMD_SERVICE_THUNK_PROTOCOL;

/**
  Acquire the lock semaphore for the PRM Dispatch Service Mailbox

  @retval EFI_SUCCESS                 The mailbox was acquired successfully.
  @retval EFI_TIMEOUT                 Acquiring the mailbox failed.

**/
typedef
EFI_STATUS
(EFIAPI *EMU_DBUS_PRMD_SERVICE_ACQUIRE_MAILBOX)(
  VOID
  );

/**
  Release the lock semaphore for the PRM Dispatch Service Mailbox

  @retval EFI_SUCCESS                 The mailbox was released successfully.
  @retval EFI_TIMEOUT                 Releasing the mailbox failed.

**/
typedef
EFI_STATUS
(EFIAPI *EMU_DBUS_PRMD_SERVICE_RELEASE_MAILBOX)(
  VOID
  );

/**
  Wait for a new message to be stored in the PRM Dispatch Service Mailbox

  @retval EFI_SUCCESS                 A new message is in the mailbox
  @retval EFI_TIMEOUT                 Waiting for a new message failed.

**/
typedef
EFI_STATUS
(EFIAPI *EMU_DBUS_PRMD_SERVICE_WAIT_FOR_MESSAGE)(
  VOID
  );

/**
  Signal that the message has been successfully processed and the results
  are waiting in the PRM Dispatch Service Mailbox

  @retval None

**/
typedef
VOID
(EFIAPI *EMU_DBUS_PRMD_SERVICE_MESSAGE_RESPONSE_READY)(
  VOID
  );

struct _EMU_DBUS_PRMD_SERVICE_THUNK_PROTOCOL {
  EMU_DBUS_PRMD_SERVICE_ACQUIRE_MAILBOX                   AcquireMailbox;
  EMU_DBUS_PRMD_SERVICE_RELEASE_MAILBOX                   ReleaseMailbox;
  EMU_DBUS_PRMD_SERVICE_WAIT_FOR_MESSAGE                  WaitForMessage;
  EMU_DBUS_PRMD_SERVICE_MESSAGE_RESPONSE_READY            MessageResponseReady;

  EMU_DBUS_PRMD_SERVICE_COMMUNICATE_HEADER                *Mailbox;
};

#endif
