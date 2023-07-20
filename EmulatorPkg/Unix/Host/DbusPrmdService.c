/** @file
  Provides the PRM Dispatch API as a dbus callable service

  Copyright (c) 2023, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

//#include <stdbool.h>
//#include <string.h>
//#include <stdio.h>
//#include <stdlib.h>
//#include <unistd.h>
#include "Host.h"
#include <pthread.h>
#include <systemd/sd-bus.h>

#include <Protocol/EmuDbusPrmdService.h>
//#include <dbus/dbus.h>
//#include <dbus/dbus-glib-lowlevel.h> /* for glib main loop */

/**
  Get IsSigTermSignaled value.

  @retval        Value             current IsSigTermSignaled value.

**/
UINT32
EFIAPI
GetIsSigTermSignaled (
  VOID
  );

#define PRMD_DBUS_OBJECT_NAME       "/com/intel/PrmDispatch"
#define PRMD_DBUS_INTERFACE_NAME    "com.intel.PrmDispatch"
#define PRMD_DBUS_SERVICE_NAME      "com.intel.PrmDispatch" // Note: This is called a D-Bus "bus name"

#define PRMD_DBUS_MAILBOX_SIZE      SIZE_4KB

const char *PrmDispatchApiVersion = "0.1";

EMU_DBUS_PRMD_SERVICE_COMMUNICATE_HEADER   *gPrmDispatchMailbox = NULL;

STATIC pthread_mutex_t mPrmDispatchMailbox = PTHREAD_MUTEX_INITIALIZER;

STATIC int mMessageIsReadyFlag = 0;
STATIC pthread_mutex_t mMessageIsReadyMutex = PTHREAD_MUTEX_INITIALIZER;
STATIC pthread_cond_t mMessageIsReadyCond = PTHREAD_COND_INITIALIZER;

STATIC int mResponseIsReadyFlag = 0;
STATIC pthread_mutex_t mResponseIsReadyMutex = PTHREAD_MUTEX_INITIALIZER;
STATIC pthread_cond_t mResponseIsReadyCond = PTHREAD_COND_INITIALIZER;

STATIC sd_bus              *mBus = NULL;

UINT32
ConvertEfiStatus (
  EFI_STATUS    Status
  )
{
  UINT32    Status32 = 0;

  if ((Status & MAX_BIT) != 0) {
    Status32 |= 0x80000000;   // Error Bit
  }
  if ((Status & (MAX_BIT >> 1)) != 0) {
    Status32 |= 0x40000000;   // OEM Namespace Bit
  }
  if ((Status & (MAX_BIT >> 2)) != 0) {
    Status32 |= 0x20000000;   // PI Spec Namespace Bit
  }
  Status32 |= (UINT32) (((UINTN) Status) & 0x1FFFFFFF);   // Mantissa
  return Status32;
}

EFI_STATUS
ConvertStatus (
  int Status
  )
{
  if (Status == EINVAL) {
    return EFI_INVALID_PARAMETER;
  }
  if (Status == ETIMEDOUT) {
    return EFI_TIMEOUT;
  }
  if (Status == EBUSY) {        // The mutex could not be acquired because it was already locked.
    return EFI_ALREADY_STARTED;
  }
  if (Status == EOPNOTSUPP) {   // The byte order in the message is different than native byte order.
    return EFI_INVALID_PARAMETER;
  }
  if (Status == EPERM) {        // The current thread does not own the mutex.
                                // The message is not sealed.
    return EFI_INVALID_PARAMETER;
  }
  if (Status == EBADMSG) {      // The message cannot be parsed.
    return EFI_INVALID_PARAMETER;
  }
  if (Status == ENXIO) {        // The message does not contain the specified type at current position.
    return EFI_INVALID_PARAMETER;
  }
  if (Status == EAGAIN) {       // The mutex could not be acquired because the maximum number of recursive locks for mutex has been exceeded.
    return EFI_ABORTED;
  }
  if (Status == EDEADLK) {      // The current thread already owns the mutex.
    return EFI_ABORTED;
  }
  if (Status == EAGAIN) {       // The system lacked the necessary resources to create another thread
    return EFI_OUT_OF_RESOURCES;
  }
  return EFI_UNSUPPORTED;
}

/**
  Wait for DXE to process the message in the PRM Dispatch Service Mailbox

  @retval EFI_SUCCESS                 A new message is in the mailbox
  @retval EFI_TIMEOUT                 Waiting for a new message failed.

**/
EFI_STATUS
DbusWaitForDxeResponse (
  VOID
  )
{
  int Status;

  Status = pthread_mutex_lock (&mResponseIsReadyMutex);
  if (Status != 0) {
    return ConvertStatus (Status);
  }
  while (!mResponseIsReadyFlag) {
    Status = pthread_cond_wait (&mResponseIsReadyCond, &mResponseIsReadyMutex);
    if (Status != 0) {
      pthread_mutex_unlock (&mResponseIsReadyMutex);
      return ConvertStatus (Status);
    }
  }
  mResponseIsReadyFlag = 0;
  Status = pthread_mutex_unlock (&mResponseIsReadyMutex);
  if (Status != 0) {
    return ConvertStatus (Status);
  }
  return EFI_SUCCESS;
}

/**
  Signal DXE that a new message is waiting in the PRM Dispatch Service Mailbox

  @retval None

**/
VOID
DbusSignalDxeMessageIsReady (
  VOID
  )
{
  int Status;

  Status = pthread_mutex_lock (&mMessageIsReadyMutex);
  if (Status != 0) {
    return;
  }
  mMessageIsReadyFlag = 1;
  Status = pthread_cond_signal (&mMessageIsReadyCond);
  if (Status != 0) {
    pthread_mutex_unlock (&mMessageIsReadyMutex);
    return;
  }
  Status = pthread_mutex_unlock (&mMessageIsReadyMutex);
  if (Status != 0) {
    return;
  }
}

static int
DbusInstallPrmPackage (
  sd_bus_message  *Message,
  void            *UserData,
  sd_bus_error    *ReturnStatus
  )
{
  EMU_DBUS_PRMD_SERVICE_COMMUNICATE_INSTALL_PACKAGE   *InstallPackageData;
  EFI_STATUS                                          EfiStatus;
  EFI_STATUS                                          ReturnedStatus;
  int                                                 Status;
  char                                                *PrmPackage;
  size_t                                              PrmPackageLength;

  //
  // Get the PRM package from the D-BUS message
  //
  Status = sd_bus_message_read_array (Message, 'y', (const void **)&PrmPackage, &PrmPackageLength);
  if (Status == 0) {
    fprintf (stderr, "Failed to read PRM package, array is empty\n");
    return sd_bus_reply_method_return (Message, "u", ConvertEfiStatus (EFI_INVALID_PARAMETER));
  }
  if (Status < 0) {
    fprintf (stderr, "Failed to read PRM package: %s\n", strerror (-Status));
    return sd_bus_reply_method_return (Message, "u", ConvertEfiStatus (ConvertStatus (-Status)));
  }

  //
  // Populate the mailbox message
  //
  EfiStatus = SecDbusAcquireMailbox ();
  if (EFI_ERROR (EfiStatus)) {
    fprintf (stderr, "SecDbusAcquireMailbox() Failed: %x\n", (int) EfiStatus);
    return sd_bus_reply_method_return (Message, "u", ConvertEfiStatus (EfiStatus));
  }
  ZeroMem ((VOID *) gPrmDispatchMailbox, PRMD_DBUS_MAILBOX_SIZE);
  gPrmDispatchMailbox->Function = EMU_DBUS_PRMD_SERVICE_FUNCTION_INSTALL_PACKAGE;
  InstallPackageData = (EMU_DBUS_PRMD_SERVICE_COMMUNICATE_INSTALL_PACKAGE *) &(gPrmDispatchMailbox->Data[0]);
  InstallPackageData->PrmPackage = (VOID *) PrmPackage;
  InstallPackageData->PrmPackageLength = (UINT32) PrmPackageLength;
  EfiStatus = SecDbusReleaseMailbox ();
  if (EFI_ERROR (EfiStatus)) {
    fprintf (stderr, "SecDbusReleaseMailbox() Failed: %x\n", (int) EfiStatus);
    return sd_bus_reply_method_return (Message, "u", ConvertEfiStatus (EfiStatus));
  }

  //
  // Inform the DaemonBds DXE driver that a new message is waiting
  //
  DbusSignalDxeMessageIsReady ();

  //
  // Wait for DaemonBds to process the message
  //
  EfiStatus = DbusWaitForDxeResponse ();
  if (EFI_ERROR (EfiStatus)) {
    fprintf (stderr, "DbusWaitForDxeResponse() Failed: %x\n", (int) EfiStatus);
    return sd_bus_reply_method_return (Message, "u", ConvertEfiStatus (EfiStatus));
  }

  //
  // Read the returned status code from DaemonBds
  //
  EfiStatus = SecDbusAcquireMailbox ();
  if (EFI_ERROR (EfiStatus)) {
    fprintf (stderr, "SecDbusAcquireMailbox() Failed: %x\n", (int) EfiStatus);
    return sd_bus_reply_method_return (Message, "u", ConvertEfiStatus (EfiStatus));
  }
  ReturnedStatus = gPrmDispatchMailbox->Status;
  EfiStatus = SecDbusReleaseMailbox ();
  if (EFI_ERROR (EfiStatus)) {
    fprintf (stderr, "SecDbusReleaseMailbox() Failed: %x\n", (int) EfiStatus);
    return sd_bus_reply_method_return (Message, "u", ConvertEfiStatus (EfiStatus));
  }

  //
  // Send the status code back to the D-BUS client
  //
  return sd_bus_reply_method_return (Message, "u", ConvertEfiStatus (ReturnedStatus));
}

static int
DbusEnumerateHandlers (
  sd_bus_message  *Message,
  void            *UserData,
  sd_bus_error    *ReturnStatus
  )
{
  //@todo
  return -1;
}

static int
DbusInvokeHandler (
  sd_bus_message  *Message,
  void            *UserData,
  sd_bus_error    *ReturnStatus
  )
{
  EFI_GUID                                            HandlerGuid;
  EMU_DBUS_PRMD_SERVICE_COMMUNICATE_INVOKE_HANDLER    *InvokeHandlerData;
  EFI_STATUS                                          EfiStatus;
  EFI_STATUS                                          ReturnedStatus;
  EFI_STATUS                                          PrmHandlerStatus;
  int                                                 Status;
  char                                                *HandlerGuidStr;
  char                                                *ParameterBufferSrc;
  size_t                                              ParameterBufferLength;
  char                                                *ParameterBufferDest = NULL;
  sd_bus_message                                      *Reply = NULL;
  char                                                EmptyArray[] = "\0";

  //
  // Get the HandlerGuid and ParameterBuffer from the D-BUS message
  //
  Status = sd_bus_message_read (Message, "s", &HandlerGuidStr);
  if (Status < 0) {
    fprintf (stderr, "Failed to read PRM package: %s\n", strerror (-Status));
    EfiStatus = ConvertStatus (-Status);
    goto Error;
  }
  EfiStatus = AsciiStrToGuid (HandlerGuidStr, &HandlerGuid);
  if (EFI_ERROR (EfiStatus)) {
    fprintf (stderr, "AsciiStrToGuid() Failed: %x\n", (int) EfiStatus);
    goto Error;
  }
  Status = sd_bus_message_read_array (Message, 'y', (const void **)&ParameterBufferSrc, &ParameterBufferLength);
  if (Status == 0) {
    fprintf (stderr, "Failed to read PRM package, array is empty\n");
    EfiStatus = EFI_INVALID_PARAMETER;
    goto Error;
  }
  if (Status < 0) {
    fprintf (stderr, "Failed to read PRM package: %s\n", strerror (-Status));
    EfiStatus = ConvertStatus (-Status);
    goto Error;
  }
  ParameterBufferDest = malloc (ParameterBufferLength);
  if (ParameterBufferDest == NULL) {
    fprintf (stderr, "DbusInvokeHandler() Failed: Out of Memory\n");
    EfiStatus = EFI_OUT_OF_RESOURCES;
    goto Error;
  }
  CopyMem (ParameterBufferDest, ParameterBufferSrc, (UINTN) ParameterBufferLength);

  //
  // Populate the mailbox message
  //
  EfiStatus = SecDbusAcquireMailbox ();
  if (EFI_ERROR (EfiStatus)) {
    fprintf (stderr, "SecDbusAcquireMailbox() Failed: %x\n", (int) EfiStatus);
    goto Error;
  }
  ZeroMem ((VOID *) gPrmDispatchMailbox, PRMD_DBUS_MAILBOX_SIZE);
  gPrmDispatchMailbox->Function = EMU_DBUS_PRMD_SERVICE_FUNCTION_INVOKE_HANDLER;
  InvokeHandlerData = (EMU_DBUS_PRMD_SERVICE_COMMUNICATE_INVOKE_HANDLER *) &(gPrmDispatchMailbox->Data[0]);
  CopyGuid (&InvokeHandlerData->HandlerGuid, &HandlerGuid);
  InvokeHandlerData->ParameterBuffer = ParameterBufferDest;
  InvokeHandlerData->ParameterBufferLength = ParameterBufferLength;
  EfiStatus = SecDbusReleaseMailbox ();
  if (EFI_ERROR (EfiStatus)) {
    fprintf (stderr, "SecDbusReleaseMailbox() Failed: %x\n", (int) EfiStatus);
    goto Error;
  }

  //
  // Inform the DaemonBds DXE driver that a new message is waiting
  //
  DbusSignalDxeMessageIsReady ();

  //
  // Wait for DaemonBds to process the message
  //
  EfiStatus = DbusWaitForDxeResponse ();
  if (EFI_ERROR (EfiStatus)) {
    fprintf (stderr, "DbusWaitForDxeResponse() Failed: %x\n", (int) EfiStatus);
    goto Error;
  }

  //
  // Create a message for the reply
  //
  Status = sd_bus_message_new_method_return (Message, &Reply);
  if (Status < 0) {
    fprintf (stderr, "sd_bus_message_new_method_return() Failed: %s\n", strerror (-Status));
    EfiStatus = ConvertStatus (-Status);
    goto Error;
  }

  //
  // Populate the reply with the returned data from DaemonBds
  //
  EfiStatus = SecDbusAcquireMailbox ();
  if (EFI_ERROR (EfiStatus)) {
    fprintf (stderr, "SecDbusAcquireMailbox() Failed: %x\n", (int) EfiStatus);
    sd_bus_message_unref (Reply);
    Reply = NULL;
    goto Error;
  }
  ReturnedStatus = gPrmDispatchMailbox->Status;
  PrmHandlerStatus = InvokeHandlerData->PrmHandlerReturnStatus;
  Status = sd_bus_message_append_array (Reply, 'y', ParameterBufferDest, ParameterBufferLength);
  if (Status < 0) {
    fprintf (stderr, "sd_bus_message_append_array() Failed: %s\n", strerror (-Status));
    EfiStatus = ConvertStatus (-Status);
    sd_bus_message_unref (Reply);
    Reply = NULL;
    SecDbusReleaseMailbox ();
    goto Error;
  }
  EfiStatus = SecDbusReleaseMailbox ();
  if (EFI_ERROR (EfiStatus)) {
    fprintf (stderr, "SecDbusReleaseMailbox() Failed: %x\n", (int) EfiStatus);
    sd_bus_message_unref (Reply);
    Reply = NULL;
    goto Error;
  }

  free (ParameterBufferDest);
  ParameterBufferDest = NULL;
  Status = sd_bus_message_append (Reply, "uu", ConvertEfiStatus (PrmHandlerStatus), ConvertEfiStatus (ReturnedStatus));
  if (Status < 0) {
    fprintf (stderr, "sd_bus_message_append() Failed: %s\n", strerror (-Status));
    EfiStatus = ConvertStatus (-Status);
    sd_bus_message_unref (Reply);
    Reply = NULL;
  }

  //
  // Send the reply to the D-BUS client
  //
  Status = sd_bus_send (mBus, Reply, NULL);
  sd_bus_message_unref (Reply);

  return Status;

Error:
  if (ParameterBufferDest != NULL) {
    free (ParameterBufferDest);
  }
  Status = sd_bus_message_new_method_return (Message, &Reply);
  if (Status < 0) {
    return Status;
  }
  Status = sd_bus_message_append_array (Reply, 'y', EmptyArray, 0);
  if (Status < 0) {
    sd_bus_message_unref (Reply);
    return Status;
  }
  Status = sd_bus_message_append (Reply, "uu", 0, ConvertEfiStatus (EfiStatus));
  if (Status < 0) {
    sd_bus_message_unref (Reply);
    return Status;
  }
  Status = sd_bus_send (mBus, Reply, NULL);
  sd_bus_message_unref (Reply);

  return Status;
}

static int
DbusUninstallPrmModule (
  sd_bus_message  *Message,
  void            *UserData,
  sd_bus_error    *ReturnStatus
  )
{
  //@todo
  return EFI_UNSUPPORTED;
}

  // SD_BUS_METHOD_WITH_ARGS (
  //   "GetHandlerDebugInfo",
  //   SD_BUS_ARGS ("s", HandlerGuid),
  //   SD_BUS_RESULT ("ay", AcpiParameterBuffer, "s", HandlerName, "u", EfiStatus),
  //   DbusGetHandlerDebugInfo,
  //   SD_BUS_VTABLE_UNPRIVILEGED
  //   ),

static int
DbusGetHandlerDebugInfo (
  sd_bus_message  *Message,
  void            *UserData,
  sd_bus_error    *ReturnStatus
  )
{
  EFI_GUID                                                    HandlerGuid;
  EMU_DBUS_PRMD_SERVICE_COMMUNICATE_GET_HANDLER_DEBUG_INFO    *GetHandlerDebugInfoData;
  EFI_STATUS                                                  EfiStatus;
  EFI_STATUS                                                  ReturnedStatus;
  int                                                         Status;
  char                                                        *HandlerGuidStr;
  sd_bus_message                                              *Reply = NULL;
  char                                                        EmptyArray[] = "\0";

  //
  // Get the HandlerGuid and ParameterBuffer from the D-BUS message
  //
  Status = sd_bus_message_read (Message, "s", &HandlerGuidStr);
  if (Status < 0) {
    fprintf (stderr, "Failed to read PRM package: %s\n", strerror (-Status));
    EfiStatus = ConvertStatus (-Status);
    goto Error;
  }
  EfiStatus = AsciiStrToGuid (HandlerGuidStr, &HandlerGuid);
  if (EFI_ERROR (EfiStatus)) {
    fprintf (stderr, "AsciiStrToGuid() Failed: %x\n", (int) EfiStatus);
    goto Error;
  }

  //
  // Populate the mailbox message
  //
  EfiStatus = SecDbusAcquireMailbox ();
  if (EFI_ERROR (EfiStatus)) {
    fprintf (stderr, "SecDbusAcquireMailbox() Failed: %x\n", (int) EfiStatus);
    goto Error;
  }
  ZeroMem ((VOID *) gPrmDispatchMailbox, PRMD_DBUS_MAILBOX_SIZE);
  gPrmDispatchMailbox->Function = EMU_DBUS_PRMD_SERVICE_FUNCTION_GET_HANDLER_DEBUG_INFO;
  GetHandlerDebugInfoData = (EMU_DBUS_PRMD_SERVICE_COMMUNICATE_GET_HANDLER_DEBUG_INFO *) &(gPrmDispatchMailbox->Data[0]);
  CopyGuid (&GetHandlerDebugInfoData->HandlerGuid, &HandlerGuid);
  EfiStatus = SecDbusReleaseMailbox ();
  if (EFI_ERROR (EfiStatus)) {
    fprintf (stderr, "SecDbusReleaseMailbox() Failed: %x\n", (int) EfiStatus);
    goto Error;
  }

  //
  // Inform the DaemonBds DXE driver that a new message is waiting
  //
  DbusSignalDxeMessageIsReady ();

  //
  // Wait for DaemonBds to process the message
  //
  EfiStatus = DbusWaitForDxeResponse ();
  if (EFI_ERROR (EfiStatus)) {
    fprintf (stderr, "DbusWaitForDxeResponse() Failed: %x\n", (int) EfiStatus);
    goto Error;
  }

  //
  // Create a message for the reply
  //
  Status = sd_bus_message_new_method_return (Message, &Reply);
  if (Status < 0) {
    fprintf (stderr, "sd_bus_message_new_method_return() Failed: %s\n", strerror (-Status));
    EfiStatus = ConvertStatus (-Status);
    goto Error;
  }

  //
  // Populate the reply with the returned data from DaemonBds
  //
  EfiStatus = SecDbusAcquireMailbox ();
  if (EFI_ERROR (EfiStatus)) {
    fprintf (stderr, "SecDbusAcquireMailbox() Failed: %x\n", (int) EfiStatus);
    sd_bus_message_unref (Reply);
    Reply = NULL;
    goto Error;
  }
  ReturnedStatus = gPrmDispatchMailbox->Status;
  //@todo There is an obscure race condition here. The AcpiParameterBuffer and
  //      HandlerName are contained in PrmDispatcher's internal data structures.
  //      Therefore, if someone implements a EFI_TIMER event handler that
  //      installs/uninstall PRM modules these structures could be modified by
  //      the main thread while we are reading them. This is very unlikely but
  //      DaemonBds should be modified to handle this corner case.
  Status  = sd_bus_message_append_array (
              Reply,
              'y',
              GetHandlerDebugInfoData->AcpiParameterBuffer,
              GetHandlerDebugInfoData->AcpiParameterBufferLength
              );
  if (Status < 0) {
    fprintf (stderr, "sd_bus_message_append_array() Failed: %s\n", strerror (-Status));
    EfiStatus = ConvertStatus (-Status);
    sd_bus_message_unref (Reply);
    Reply = NULL;
    SecDbusReleaseMailbox ();
    goto Error;
  }
  Status = sd_bus_message_append (Reply, "s", GetHandlerDebugInfoData->HandlerName);
  if (Status < 0) {
    fprintf (stderr, "sd_bus_message_append() Failed: %s\n", strerror (-Status));
    EfiStatus = ConvertStatus (-Status);
    sd_bus_message_unref (Reply);
    Reply = NULL;
    SecDbusReleaseMailbox ();
    goto Error;
  }
  EfiStatus = SecDbusReleaseMailbox ();
  if (EFI_ERROR (EfiStatus)) {
    fprintf (stderr, "SecDbusReleaseMailbox() Failed: %x\n", (int) EfiStatus);
    sd_bus_message_unref (Reply);
    Reply = NULL;
    goto Error;
  }

  Status = sd_bus_message_append (Reply, "u", ConvertEfiStatus (ReturnedStatus));
  if (Status < 0) {
    fprintf (stderr, "sd_bus_message_append() Failed: %s\n", strerror (-Status));
    EfiStatus = ConvertStatus (-Status);
    sd_bus_message_unref (Reply);
    Reply = NULL;
  }

  //
  // Send the reply to the D-BUS client
  //
  Status = sd_bus_send (mBus, Reply, NULL);
  sd_bus_message_unref (Reply);

  return Status;

Error:
  Status = sd_bus_message_new_method_return (Message, &Reply);
  if (Status < 0) {
    return Status;
  }
  Status = sd_bus_message_append_array (Reply, 'y', EmptyArray, 0);
  if (Status < 0) {
    sd_bus_message_unref (Reply);
    return Status;
  }
  Status = sd_bus_message_append (Reply, "su", EmptyArray, ConvertEfiStatus (EfiStatus));
  if (Status < 0) {
    sd_bus_message_unref (Reply);
    return Status;
  }
  Status = sd_bus_send (mBus, Reply, NULL);
  sd_bus_message_unref (Reply);

  return Status;
}

//
// D-Bus Interface Definition
//
static const sd_bus_vtable PrmDispatchVtable[] = {
  SD_BUS_VTABLE_START (0),
  SD_BUS_METHOD_WITH_ARGS (
    "InstallPrmPackage",
    SD_BUS_ARGS ("ay", PrmPackage),
    SD_BUS_RESULT ("u", EfiStatus),
    DbusInstallPrmPackage,
    SD_BUS_VTABLE_UNPRIVILEGED
    ),
  SD_BUS_METHOD_WITH_ARGS (
    "EnumerateHandlers",
    SD_BUS_NO_ARGS,
    SD_BUS_RESULT ("u", EfiStatus, "a{ss}", PrmHandlerInfo),
    DbusEnumerateHandlers,
    SD_BUS_VTABLE_UNPRIVILEGED
    ),
  SD_BUS_METHOD_WITH_ARGS (
    "InvokeHandler",
    SD_BUS_ARGS ("s", HandlerGuid, "ay", ParameterBuffer),
    SD_BUS_RESULT ("ay", ModifiedParameterBuffer, "u", PrmHandlerReturnStatus, "u", EfiStatus),
    DbusInvokeHandler,
    SD_BUS_VTABLE_UNPRIVILEGED
    ),
  SD_BUS_METHOD_WITH_ARGS (
    "UninstallPrmModule",
    SD_BUS_ARGS ("s", ModuleGuid),
    SD_BUS_RESULT ("u", EfiStatus),
    DbusUninstallPrmModule,
    SD_BUS_VTABLE_UNPRIVILEGED
    ),
  SD_BUS_METHOD_WITH_ARGS (
    "GetHandlerDebugInfo",
    SD_BUS_ARGS ("s", HandlerGuid),
    SD_BUS_RESULT ("ay", AcpiParameterBuffer, "s", HandlerName, "u", EfiStatus),
    DbusGetHandlerDebugInfo,
    SD_BUS_VTABLE_UNPRIVILEGED
    ),
  SD_BUS_VTABLE_END
};

void*
DBusWorkerThread (
  void *Arg
  )
{
//  struct sigaction    act;
  int                 Status;
  sd_bus_slot         *Slot = NULL;


  // // @todo DELETE
  // // Register a signal handler for SIGTERM so that if the daemon is requested
  // // to exit this thread will shut itself down cleanly.
  // //
  // sigemptyset (&act.sa_mask);
  // act.sa_flags = SA_SIGINFO;
  // act.sa_sigaction = SignalHandler;
  // sigaction (SIGTERM, &act, NULL);

  //
  // Open a connection to the system level D-BUS
  //
  Status = sd_bus_open_system (&mBus);
  if (Status < 0) {
    fprintf (stderr, "Failed to connect to system D-BUS: %s\n", strerror (-Status));
    goto Done;
  }

  //
  // Register the PrmDispatch service on to the message bus
  //
  Status  = sd_bus_add_object_vtable (
              mBus,
              &Slot,
              PRMD_DBUS_OBJECT_NAME,
              PRMD_DBUS_INTERFACE_NAME,
              PrmDispatchVtable,
              NULL
              );
  if (Status < 0) {
    fprintf (stderr, "Failed to add D-Bus object: %s\n", strerror (-Status));
    goto Done;
  }

  Status = sd_bus_request_name (mBus, PRMD_DBUS_SERVICE_NAME, 0);
  if (Status < 0) {
    fprintf (stderr, "Failed to acquire D-Bus service name: %s\n", strerror (-Status));
    goto Done;
  }

  //
  // Message Processing Loop
  //
  while (1) {
    Status = sd_bus_process (mBus, NULL);
    if (Status < 0) {
      fprintf (stderr, "Failed to process bus: %s\n", strerror (-Status));
      goto Done;
    }
    if (Status > 0) {
      //
      // If a dbus request was processed, immediately try to process another
      // request because it is possible that multiple requests are currently
      // sitting in the queue
      //
      continue;
    }

check_sigterm:
    //
    // If we have received SIGTERM, exit the loop and allow the thread to quit
    //
    if (GetIsSigTermSignaled ()) {
      printf ("Exiting D-BUS loop.\n");
      goto Done;
    }

    //
    // Wait for the next message to come in
    //
    Status = sd_bus_wait (mBus, 1000000 /* 1 sec */);
    if (Status < 0) {
      fprintf (stderr, "Failed to wait for bus traffic: %s\n", strerror (-Status));
      goto Done;
    }
    if (Status == 0) {
      //
      // sd_bus_wait() timed out, check for SIGTERM and then start waiting again
      //
      goto check_sigterm;
    }
  }

Done:
  sd_bus_slot_unref (Slot);
  sd_bus_unref (mBus);
  free (gPrmDispatchMailbox);
  gPrmDispatchMailbox = NULL;
  return NULL;
}

/**
  Acquire the lock semaphore for the PRM Dispatch Service Mailbox

  @retval EFI_SUCCESS                 The mailbox was acquired successfully.
  @retval EFI_TIMEOUT                 Acquiring the mailbox failed.

**/
EFI_STATUS
SecDbusAcquireMailbox (
  VOID
  )
{
  int Status;

  Status = pthread_mutex_lock (&mPrmDispatchMailbox);
  if (Status != 0) {
    return ConvertStatus (Status);
  }
  return EFI_SUCCESS;
}

/**
  Release the lock semaphore for the PRM Dispatch Service Mailbox

  @retval EFI_SUCCESS                 The mailbox was released successfully.
  @retval EFI_TIMEOUT                 Releasing the mailbox failed.

**/
EFI_STATUS
SecDbusReleaseMailbox (
  VOID
  )
{
  int Status;

  Status = pthread_mutex_unlock (&mPrmDispatchMailbox);
  if (Status != 0) {
    return ConvertStatus (Status);
  }
  return EFI_SUCCESS;
}

/**
  Wait for a new message to be stored in the PRM Dispatch Service Mailbox

  @retval EFI_SUCCESS                 A new message is in the mailbox
  @retval EFI_TIMEOUT                 Waiting for a new message failed.

**/
EFI_STATUS
SecDbusWaitForMessage (
  VOID
  )
{
  int Status;

  Status = pthread_mutex_lock (&mMessageIsReadyMutex);
  if (Status != 0) {
    return ConvertStatus (Status);
  }
  while (!mMessageIsReadyFlag) {
    Status = pthread_cond_wait (&mMessageIsReadyCond, &mMessageIsReadyMutex);
    if (Status != 0) {
      pthread_mutex_unlock (&mMessageIsReadyMutex);
      return ConvertStatus (Status);
    }
  }
  mMessageIsReadyFlag = 0;
  Status = pthread_mutex_unlock (&mMessageIsReadyMutex);
  if (Status != 0) {
    return ConvertStatus (Status);
  }
  return EFI_SUCCESS;
}

/**
  Signal that the message has been successfully processed and the results
  are waiting in the PRM Dispatch Service Mailbox

  @retval None

**/
VOID
SecDbusMessageResponseReady (
  VOID
  )
{
  int Status;

  Status = pthread_mutex_lock (&mResponseIsReadyMutex);
  if (Status != 0) {
    return;
  }
  mResponseIsReadyFlag = 1;
  Status = pthread_cond_signal (&mResponseIsReadyCond);
  if (Status != 0) {
    pthread_mutex_unlock (&mResponseIsReadyMutex);
    return;
  }
  Status = pthread_mutex_unlock (&mResponseIsReadyMutex);
  if (Status != 0) {
    return;
  }
}

EMU_DBUS_PRMD_SERVICE_THUNK_PROTOCOL  gEmuDbusPrmdServiceThunk = {
  GasketEmuDbusAcquireMailbox,
  GasketEmuDbusReleaseMailbox,
  GasketEmuDbusWaitForMessage,
  GasketEmuDbusMessageResponseReady,
  NULL  // Mailbox is initialized in SecDbusInitializeListener()
};

/**
  Initialize the background worker thread for D-BUS communication

  @retval EFI_SUCCESS                 The D-BUS listener was activated successfully.

**/
EFI_STATUS
SecDbusInitializeListener (
  VOID
  )
{
  pthread_t Thread;
  int Status;

  gPrmDispatchMailbox = (EMU_DBUS_PRMD_SERVICE_COMMUNICATE_HEADER *) malloc (PRMD_DBUS_MAILBOX_SIZE);
  if (gPrmDispatchMailbox == NULL) {
    return EFI_OUT_OF_RESOURCES;
  }
  gEmuDbusPrmdServiceThunk.Mailbox = gPrmDispatchMailbox;

  Status = pthread_create (&Thread, NULL, DBusWorkerThread, NULL);
  if (Status != 0) {
    return ConvertStatus (Status);
  }
  return EFI_SUCCESS;
}

EFI_STATUS
SecDbusPrmdServiceThunkOpen (
  IN  EMU_IO_THUNK_PROTOCOL  *This
  )
{
  if (This->Instance != 0) {
    // Only single instance is supported
    return EFI_NOT_FOUND;
  }

  This->Interface = &gEmuDbusPrmdServiceThunk;

  return EFI_SUCCESS;
}

EFI_STATUS
SecDbusPrmdServiceThunkClose (
  IN  EMU_IO_THUNK_PROTOCOL  *This
  )
{
  return EFI_SUCCESS;
}

EMU_IO_THUNK_PROTOCOL  gDbusPrmdServiceThunkIo = {
  &gEmuDbusPrmdServiceProtocolGuid,
  NULL,
  NULL,
  0,
  GasketEmuDbusThunkOpen,
  GasketEmuDbusThunkClose,
  NULL
};

// GMainLoop *mainloop;

// /*
//  * This is the XML string describing the interfaces, methods and
//  * signals implemented by our 'Server' object. It's used by the
//  * 'Introspect' method of 'org.freedesktop.DBus.Introspectable'
//  * interface.
//  */
// const char *server_introspection_xml =
//     DBUS_INTROSPECT_1_0_XML_DOCTYPE_DECL_NODE
//     "<node>\n"

//     "  <interface name='org.freedesktop.DBus.Introspectable'>\n"
//     "    <method name='Introspect'>\n"
//     "      <arg name='data' type='s' direction='out' />\n"
//     "    </method>\n"
//     "  </interface>\n"

//     "  <interface name='org.freedesktop.DBus.Properties'>\n"
//     "    <method name='Get'>\n"
//     "      <arg name='interface' type='s' direction='in' />\n"
//     "      <arg name='property'  type='s' direction='in' />\n"
//     "      <arg name='value'     type='s' direction='out' />\n"
//     "    </method>\n"
//     "    <method name='GetAll'>\n"
//     "      <arg name='interface'  type='s'     direction='in'/>\n"
//     "      <arg name='properties' type='a{sv}' direction='out'/>\n"
//     "    </method>\n"
//     "  </interface>\n"

//     "  <interface name='com.Intel.PrmDispatch'>\n"
//     "    <property name='Version' type='s' access='read' />\n"
//     "    <method name='InstallPrmPackage'>\n"
//     "       <arg type='ay' direction='in' name='PrmPackage' />\n"
//     "       <arg type='u' direction='out' name='EfiStatus' />\n"
//     "    </method>\n"
//     "    <method name='EnumerateHandlers'>\n"
//     "       <arg type='u' direction='out' name='EfiStatus'>\n"
//     "       <arg type='a{ss} direction='out' name='PrmHandlerInfo' />\n"
//     "         <entry key='PlatformGuid' value='s'/>\n"
//     "         <entry key='ModuleGuid' value='s'/>\n"
//     "         <entry key='HandlerGuid' value='s'/>\n"
//     "       </arg>\n"
//     "    </method>\n"
//     "    <method name='InvokeHandler'>\n"
//     "       <arg type='s' direction='in' name='HandlerGuid'/>\n"
//     "       <arg type='ay' direction='in' name='ParameterBuffer' optional='true'/>\n"
//     "       <arg type='u' direction='out' name='PrmHandlerReturnStatus'/>\n"
//     "       <arg type='u' direction='out' name='EfiStatus'  "
//     "    </method>\n"
//     "    <method name='UninstallPrmModule'>\n"
//     "       <arg type='s' direction='in' name='ModuleGuid'/>\n"
//     "       <arg type='u' direction='out' name='EfiStatus'/>\n"
//     "     </method>\n"
//     "     <method name='GetHandlerDebugInfo'>\n"
//     "       <arg type='s' direction='in' name='HandlerGuid'/>\n"
//     "       <arg type='ay' direction='out' name='AcpiParameterBuffer'/>\n"
//     "       <arg type='s' direction='out' name='HandlerName' />\n"
//     "       <arg type='u' durection='out' name='EfiStatus'/>\n"
//     "     </method>\n"
//     "    <method name='Quit'>\n"
// 	  "    </method>\n"
//     "  </interface>\n"

//     "</node>\n";

// /*
//  * This implements 'Get' method of DBUS_INTERFACE_PROPERTIES so a
//  * client can inspect the properties/attributes of 'TestInterface'.
//  */
// DBusHandlerResult server_get_properties_handler(const char *property, DBusConnection *conn, DBusMessage *reply)
// {
//   if (!strcmp(property, "Version"))
//   {
//     dbus_message_append_args(reply,
//                              DBUS_TYPE_STRING, &version,
//                              DBUS_TYPE_INVALID);
//   }
//   else
//     /* Unknown property */
//     return DBUS_HANDLER_RESULT_NOT_YET_HANDLED;

//   if (!dbus_connection_send(conn, reply, NULL))
//     return DBUS_HANDLER_RESULT_NEED_MEMORY;
//   return DBUS_HANDLER_RESULT_HANDLED;
// }

// /*
//  * This implements 'GetAll' method of DBUS_INTERFACE_PROPERTIES. This
//  * one seems required by g_dbus_proxy_get_cached_property().
//  */
// DBusHandlerResult server_get_all_properties_handler(DBusConnection *conn, DBusMessage *reply)
// {
//   DBusHandlerResult result;
//   DBusMessageIter array, dict, iter, variant;
//   const char *property = "Version";

//   /*
//    * All dbus functions used below might fail due to out of
//    * memory error. If one of them fails, we assume that all
//    * following functions will fail too, including
//    * dbus_connection_send().
//    */
//   result = DBUS_HANDLER_RESULT_NEED_MEMORY;

//   dbus_message_iter_init_append(reply, &iter);
//   dbus_message_iter_open_container(&iter, DBUS_TYPE_ARRAY, "{sv}", &array);

//   /* Append all properties name/value pairs */
//   property = "Version";
//   dbus_message_iter_open_container(&array, DBUS_TYPE_DICT_ENTRY, NULL, &dict);
//   dbus_message_iter_append_basic(&dict, DBUS_TYPE_STRING, &property);
//   dbus_message_iter_open_container(&dict, DBUS_TYPE_VARIANT, "s", &variant);
//   dbus_message_iter_append_basic(&variant, DBUS_TYPE_STRING, &version);
//   dbus_message_iter_close_container(&dict, &variant);
//   dbus_message_iter_close_container(&array, &dict);
//   dbus_message_iter_close_container(&iter, &array);

//   if (dbus_connection_send(conn, reply, NULL))
//     result = DBUS_HANDLER_RESULT_HANDLED;
//   return result;
// }

// /*
//  * This function implements the 'TestInterface' interface for the
//  * 'Server' DBus object.
//  *
//  * It also implements 'Introspect' method of
//  * 'org.freedesktop.DBus.Introspectable' interface which returns the
//  * XML string describing the interfaces, methods, and signals
//  * implemented by 'Server' object. This also can be used by tools such
//  * as d-feet(1) and can be queried by:
//  *
//  * $ gdbus introspect --session --dest org.example.TestServer --object-path /org/example/TestObject
//  */
// DBusHandlerResult server_message_handler(DBusConnection *conn, DBusMessage *message, void *data)
// {
//   DBusHandlerResult result;
//   DBusMessage *reply = NULL;
//   DBusError err;
//   bool quit = false;

//   fprintf(stderr, "Got D-Bus request: %s.%s on %s\n",
//           dbus_message_get_interface(message),
//           dbus_message_get_member(message),
//           dbus_message_get_path(message));

//   /*
//    * Does not allocate any memory; the error only needs to be
//    * freed if it is set at some point.
//    */
//   dbus_error_init(&err);

//   if (dbus_message_is_method_call(message, DBUS_INTERFACE_INTROSPECTABLE, "Introspect"))
//   {

//     if (!(reply = dbus_message_new_method_return(message)))
//       goto fail;

//     dbus_message_append_args(reply,
//                              DBUS_TYPE_STRING, &server_introspection_xml,
//                              DBUS_TYPE_INVALID);
//   }
//   else if (dbus_message_is_method_call(message, DBUS_INTERFACE_PROPERTIES, "Get"))
//   {
//     const char *interface, *property;

//     if (!dbus_message_get_args(message, &err,
//                                DBUS_TYPE_STRING, &interface,
//                                DBUS_TYPE_STRING, &property,
//                                DBUS_TYPE_INVALID))
//       goto fail;

//     if (!(reply = dbus_message_new_method_return(message)))
//       goto fail;

//     result = server_get_properties_handler(property, conn, reply);
//     dbus_message_unref(reply);
//     return result;
//   }
//   else if (dbus_message_is_method_call(message, DBUS_INTERFACE_PROPERTIES, "GetAll"))
//   {

//     if (!(reply = dbus_message_new_method_return(message)))
//       goto fail;

//     result = server_get_all_properties_handler(conn, reply);
//     dbus_message_unref(reply);
//     return result;
//   }
//   else if (dbus_message_is_method_call(message, "com.Intel.PrmDispatch", "Quit"))
//   {
//     /*
//      * Quit() has no return values but a METHOD_RETURN
//      * reply is required, so the caller will know the
//      * method was successfully processed.
//      */
//     reply = dbus_message_new_method_return(message);
//     quit = true;
//   }
//   else
//     return DBUS_HANDLER_RESULT_NOT_YET_HANDLED;

// fail:
//   if (dbus_error_is_set(&err))
//   {
//     if (reply)
//       dbus_message_unref(reply);
//     reply = dbus_message_new_error(message, err.name, err.message);
//     dbus_error_free(&err);
//   }

//   /*
//    * In any cases we should have allocated a reply otherwise it
//    * means that we failed to allocate one.
//    */
//   if (!reply)
//     return DBUS_HANDLER_RESULT_NEED_MEMORY;

//   /* Send the reply which might be an error one too. */
//   result = DBUS_HANDLER_RESULT_HANDLED;
//   if (!dbus_connection_send(conn, reply, NULL))
//     result = DBUS_HANDLER_RESULT_NEED_MEMORY;
//   dbus_message_unref(reply);

//   if (quit)
//   {
//     fprintf(stderr, "Server exiting...\n");
// 		g_main_loop_quit(mainloop);
// 	}
// 	return result;
// }


// const DBusObjectPathVTable server_vtable = {
// 	.message_function = server_message_handler
// };


// int run_server(void)
// {
// 	DBusConnection *conn;
// 	DBusError err;
// 	int rv;

//   dbus_error_init(&err);

// 	/* connect to the daemon bus */
// 	conn = dbus_bus_get(DBUS_BUS_SESSION, &err);
// 	if (!conn) {
// 		fprintf(stderr, "Failed to get a session DBus connection: %s\n", err.message);
// 		goto fail;
// 	}

// 	rv = dbus_bus_request_name(conn, "com.intel.PrmDispatch", DBUS_NAME_FLAG_REPLACE_EXISTING , &err);
// 	if (rv != DBUS_REQUEST_NAME_REPLY_PRIMARY_OWNER) {
// 		fprintf(stderr, "Failed to request name on bus: %s\n", err.message);
// 		goto fail;
// 	}

// 	if (!dbus_connection_register_object_path(conn, "/com/intel/PrmDispatch", &server_vtable, NULL)) {
// 		fprintf(stderr, "Failed to register a object path for 'PrmDispatch'\n");
// 		goto fail;
// 	}

// 	/*
// 	 * For the sake of simplicity we're using glib event loop to
// 	 * handle DBus messages. This is the only place where glib is
// 	 * used.
// 	 */
// 	printf("Starting demo dbus server v%s\n", version);
// 	mainloop = g_main_loop_new(NULL, false);
// 	/* Set up the DBus connection to work in a GLib event loop */
// 	dbus_connection_setup_with_g_main(conn, NULL);
// 	/* Start the glib event loop */
// 	g_main_loop_run(mainloop);

// 	return EXIT_SUCCESS;
// fail:
// 	dbus_error_free(&err);
// 	return EXIT_FAILURE;
// }

// int main(void){
//   run_server();
// }
