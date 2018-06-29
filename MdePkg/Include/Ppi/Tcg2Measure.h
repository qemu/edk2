/** @file
  This file provides function for TCG2 measure in PEI phase.

  Copyright (c) 2018, Intel Corporation. All rights reserved.<BR>
  This program and the accompanying materials                          
  are licensed and made available under the terms and conditions of the BSD License         
  which accompanies this distribution.  The full text of the license may be found at        
  http://opensource.org/licenses/bsd-license.php                                            

  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,                     
  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.             

**/

#ifndef __TCG2_MEASURE_PPI_H__
#define __TCG2_MEASURE_PPI_H__

///
/// The GUID is used to identify TCG2 Measure PPI.
/// 
typedef struct _EFI_PEI_TCG2_MEASURE_PPI   EFI_PEI_TCG2_MEASURE_PPI;


/**
  The EFI_TCG2_MEASURE_PPI. HashLogExtendEvent function call provides callers with
  an opportunity to extend and optionally log events without requiring
  knowledge of actual TPM commands. 
  The extend operation will occur even if this function cannot create an event
  log entry. The interface is designed to be called before Memory Ready

  @param[in]  This                Points to this instance of the
                                EFI_PEI_FIRMWARE_VOLUME_PPI.
  @param[in]  Flags              Bitmap providing additional information. Defined in Tcg2Protocol.h
  @param[in]  DataToHash         Physical address of the start of the data buffer to be hashed. 
  @param[in]  DataToHashLen      The length in bytes of the buffer referenced by DataToHash.
  @param[in]  Event              Pointer to data buffer containing information about the event.

  @retval EFI_SUCCESS            Operation completed successfully.
  @retval EFI_DEVICE_ERROR       The command was unsuccessful.
  @retval EFI_VOLUME_FULL        The extend operation occurred, but the event could not be written to one or more event logs.
  @retval EFI_INVALID_PARAMETER  One or more of the parameters are incorrect.
  
**/
typedef
EFI_STATUS
(EFIAPI *EFI_PEI_TCG2_HASH_LOG_EXTEND_EVENT)(
  IN  UINT64                           Flags,
  IN  UINT8                            *HashData,
  IN  UINTN                            HashDataLen,
  IN  EFI_TCG2_EVENT                   *Event
);


///
/// This file provides function for TCG2 measure in PEI phase.
///
struct _EFI_PEI_TCG2_MEASURE_PPI {
  EFI_PEI_TCG2_HASH_LOG_EXTEND_EVENT       HashLogExtendEvent;
};

extern EFI_GUID gEdkiiTcg2MeasurePpiGuid;

#endif 
