/** @file
Header file for CalcuateCrc32 routine

Copyright (c) 2004 - 2014, Intel Corporation. All rights reserved.<BR>
This program and the accompanying materials                          
are licensed and made available under the terms and conditions of the BSD License         
which accompanies this distribution.  The full text of the license may be found at        
http://opensource.org/licenses/bsd-license.php                                            
                                                                                          
THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,                     
WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.             

**/

#ifndef _PCD_VALUE_COMMON_H
#define _PCD_VALUE_COMMON_H

#include <Common/UefiBaseTypes.h>

#define __FIELD_SIZE(TYPE, Field) (sizeof((TYPE *)0)->Field)
#define __ARRAY_ELEMENT_SIZE(TYPE, Field) (sizeof((TYPE *)0)->Field[0])
#define __OFFSET_OF(TYPE, Field) ((UINT32) &(((TYPE *)0)->Field))
#define __FLEXIBLE_SIZE(Size, TYPE, Field, MaxIndex)   if (__FIELD_SIZE(TYPE, Field) == 0) Size = (__OFFSET_OF(TYPE, Field) + __ARRAY_ELEMENT_SIZE(TYPE, Field) * (MaxIndex))

VOID
PcdEntryPoint (
  VOID
  );

int
PcdValueMain (
  int   argc,
  char  *argv[]
  );

VOID
__PcdSet (
  CHAR8   *SkuName             OPTIONAL,
  CHAR8   *DefaultValueName    OPTIONAL,
  CHAR8   *TokenSpaceGuidName,
  CHAR8   *TokenName,
  UINT64  Value
  );

VOID
__PcdSet (
  CHAR8   *SkuName             OPTIONAL,
  CHAR8   *DefaultValueName    OPTIONAL,
  CHAR8   *TokenSpaceGuidName,
  CHAR8   *TokenName,
  UINT64  Value
  );

VOID *
__PcdGetPtr (
  CHAR8   *SkuName             OPTIONAL,
  CHAR8   *DefaultValueName    OPTIONAL,
  CHAR8   *TokenSpaceGuidName,
  CHAR8   *TokenName,
  UINT32  *Size
  );

VOID
__PcdSetPtr (
  CHAR8   *SkuName             OPTIONAL,
  CHAR8   *DefaultValueName    OPTIONAL,
  CHAR8   *TokenSpaceGuidName,
  CHAR8   *TokenName,
  UINT32  Size,
  UINT8   *Value
  );

#define PcdGet(A, B, C, D)  __PcdGet(#A, #B, #C, #D)
#define PcdSet(A, B, C, D, Value)  __PcdSet(#A, #B, #C, #D, Value)
#define PcdGetPtr(A, B, C, D, Size)  __PcdGetPtr(#A, #B, #C, #D, Size)
#define PcdSetPtr(A, B, C, D, Size, Value)  __PcdSetPtr(#A, #B, #C, #D, Size, Value)

#endif
