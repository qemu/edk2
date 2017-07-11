/** @file
  This sample application shows TestPcd field value.

  Copyright (c) 2017, Intel Corporation. All rights reserved.<BR>
  This program and the accompanying materials                          
  are licensed and made available under the terms and conditions of the BSD License         
  which accompanies this distribution.  The full text of the license may be found at        
  http://opensource.org/licenses/bsd-license.php                                            

  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,                     
  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.             

**/

#include <Uefi.h>
#include <Library/PcdLib.h>
#include <Library/UefiLib.h>
#include <Library/UefiApplicationEntryPoint.h>
#include <Guid/Test.h>
#include <Guid/Test2.h>

/**
  The user Entry Point for Application. The user code starts with this function
  as the real entry point for the application.

  @param[in] ImageHandle    The firmware allocated handle for the EFI image.  
  @param[in] SystemTable    A pointer to the EFI System Table.
  
  @retval EFI_SUCCESS       The entry point is executed successfully.
  @retval other             Some error occurs when executing this entry point.

**/
EFI_STATUS
EFIAPI
UefiMain (
  IN EFI_HANDLE        ImageHandle,
  IN EFI_SYSTEM_TABLE  *SystemTable
  )
{
  TEST    *Test;
  TEST2   *Test2;
  TEST2   *Test3;
  TEST2   *Test4;
  UINTN   PcdSize;

  Test  = (TEST *) PcdGetPtr (TestFix);
  Test2 = (TEST2 *) PcdGetPtr (TestPatchable);
  Test3 = (TEST2 *) PcdGetPtr (TestDynamic);
  Test4 = (TEST2 *) PcdGetPtr (TestDynamicExHii);

  Print (L"TestFix.Array is %s\n", (CHAR16*)Test->Array);
  Print (L"TestPatchable.Guid is %g\n", Test2->Guid);
  Print (L"TestDynamic.FlexibleArray is %s\n", (CHAR16*)Test3->FlexibleArray);
  Print (L"TestDynamicExHii.Guid is %g\n", Test4->Guid);

  Test3->OneLineBoolA = TRUE;
  PcdSize = PcdGetSize (TestDynamic);
  PcdSetPtrS (TestDynamic, &PcdSize, Test3);
  
  Test4->Array2[0] = 0;
  Test4->Array2[1] = 1;
  PcdSize = PcdGetSize (TestDynamicExHii);
  PcdSetPtrS (TestDynamicExHii, &PcdSize, Test4);
  
  return EFI_SUCCESS;
}
