/** @file

  Definition for the Platform Runtime Mechanism (PRM) ACPI table (PRMT).

  Copyright (c) 2020, Intel Corporation. All rights reserved.<BR>
  Copyright (c) Microsoft Corporation
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#ifndef PRMT_ACPI_TABLE_H_
#define PRMT_ACPI_TABLE_H_

#include <Base.h>
#include <IndustryStandard/Acpi10.h>
#include <PrmAcpiDescriptionTable.h>

#define PRM_TABLE_SIGNATURE  SIGNATURE_32 ('P', 'R', 'M', 'T')

#define PRM_TABLE_REVISION                       0x0
#define PRM_MODULE_INFORMATION_STRUCT_REVISION   0x00
#define PRM_HANDLER_INFORMATION_STRUCT_REVISION  0x00

//
// Helper macros to build PRM Information structures
//
// Todo: Revisit whether to use; currently both macros are not used
//
#define PRM_MODULE_INFORMATION_STRUCTURE(ModuleGuid, ModuleRevision, HandlerCount, PrmHanderInfoStructureArray)  {                      \
    {                                                                                                                                   \
      PRM_MODULE_INFORMATION_STRUCT_REVISION,                                                                                      /* UINT16    StructureRevision;   */                         \
      (OFFSET_OF (PRM_MODULE_INFORMATION_STRUCT, HandlerInfoStructure) + (HandlerCount * sizeof (PRM_HANDLER_INFORMATION_STRUCT))) /* UINT16    StructureLength;     */ \
      ModuleGuid,                                                                                                                  /* GUID      ModuleGuid;          */                         \
      ModuleRevision,                                                                                                              /* UINT16    ModuleRevision       */                         \
      HandlerCount,                                                                                                                /* UINT16    HandlerCount         */                         \
      OFFSET_OF (PRM_MODULE_INFORMATION_STRUCT, HandlerInfoOffset),                                                                /* UINT32    HandlerInfoOffset    */                         \
      PrmHanderInfoStructureArray                                                                                                  /* PRM_HANDLER_INFORMATION_STRUCT HandlerInfoStructure */    \
    } \
  }

#define PRM_HANDLER_INFORMATION_STRUCTURE(HandlerGuid, PhysicalAddress)  {                                                  \
    {                                                                                                                              \
      PRM_HANDLER_INFORMATION_STRUCT_REVISION,                             /* UINT16                  StructureRevision;      */   \
      sizeof (PRM_HANDLER_INFORMATION_STRUCT),                             /* UINT16                  StructureLength;        */   \
      HandlerGuid,                                                         /* GUID                    HandlerGuid;            */   \
      PhysicalAddress,                                                     /* UINT64                  PhysicalAddress         */   \
    } \
  }

#endif // _PRMT_ACPI_TABLE_H_
