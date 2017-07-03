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

  Test  = (TEST *) PcdGetPtr (Test);
  Test2 = (TEST2 *) PcdGetPtr (Test2);
  Test3 = (TEST2 *) PcdGetPtr (Test3);
  Test4 = (TEST2 *) PcdGetPtr (Test4);

  Print (L"Test.Array is %s\n", (CHAR16*)Test->Array);
  Print (L"Test2.Guid is %g\n", Test2->Guid);
  Print (L"Test3.Array is %s\n", (CHAR16*)Test3->Array2);
  Print (L"Test4.Guid is %g\n", Test4->Guid);
  
  return EFI_SUCCESS;
}
