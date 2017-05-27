## @file
# EFI/PI Reference Module Package for All Architectures
#
# (C) Copyright 2014 Hewlett-Packard Development Company, L.P.<BR>
# Copyright (c) 2007 - 2016, Intel Corporation. All rights reserved.<BR>
#
#    This program and the accompanying materials
#    are licensed and made available under the terms and conditions of the BSD License
#    which accompanies this distribution. The full text of the license may be found at
#    http://opensource.org/licenses/bsd-license.php
#
#    THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
#    WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.
#
##

[Defines]
  PLATFORM_NAME                  = TestPkg
  PLATFORM_GUID                  = 35B4419B-4CF6-46FA-9A5D-741D0D29CD61
  PLATFORM_VERSION               = 0.1
  DSC_SPECIFICATION              = 0x00010005
  OUTPUT_DIRECTORY               = Build/TestPkg
  SUPPORTED_ARCHITECTURES        = IA32|IPF|X64|EBC|ARM|AARCH64
  BUILD_TARGETS                  = DEBUG|RELEASE|NOOPT
  SKUID_IDENTIFIER               = DEFAULT

[LibraryClasses]
  BaseLib|MdePkg/Library/BaseLib/BaseLib.inf
  PrintLib|MdePkg/Library/BasePrintLib/BasePrintLib.inf
  DebugPrintErrorLevelLib|MdeModulePkg/Library/DxeDebugPrintErrorLevelLib/DxeDebugPrintErrorLevelLib.inf
  HobLib|MdePkg/Library/DxeHobLib/DxeHobLib.inf
  MemoryAllocationLib|MdePkg/Library/UefiMemoryAllocationLib/UefiMemoryAllocationLib.inf
  DevicePathLib|MdePkg/Library/UefiDevicePathLib/UefiDevicePathLib.inf
  UefiApplicationEntryPoint|MdePkg/Library/UefiApplicationEntryPoint/UefiApplicationEntryPoint.inf
  UefiLib|MdePkg/Library/UefiLib/UefiLib.inf
  PcdLib|MdePkg/Library/DxePcdLib/DxePcdLib.inf
  BaseMemoryLib|MdePkg/Library/BaseMemoryLibOptDxe/BaseMemoryLibOptDxe.inf
  UefiBootServicesTableLib|MdePkg/Library/UefiBootServicesTableLib/UefiBootServicesTableLib.inf
  DebugLib|MdePkg/Library/UefiDebugLibConOut/UefiDebugLibConOut.inf
  UefiRuntimeServicesTableLib|MdePkg/Library/UefiRuntimeServicesTableLib/UefiRuntimeServicesTableLib.inf

###################################################################################################
#
# Components Section - list of the modules and components that will be processed by compilation
#                      tools and the EDK II tools to generate PE32/PE32+/Coff image files.
#
# Note: The EDK II DSC file is not used to specify how compiled binary images get placed
#       into firmware volume images. This section is just a list of modules to compile from
#       source into UEFI-compliant binaries.
#       It is the FDF file that contains information on combining binary files into firmware
#       volume images, whose concept is beyond UEFI and is described in PI specification.
#       Binary modules do not need to be listed in this section, as they should be
#       specified in the FDF file. For example: Shell binary (Shell_Full.efi), FAT binary (Fat.efi),
#       Logo (Logo.bmp), and etc.
#       There may also be modules listed in this section that are not required in the FDF file,
#       When a module listed here is excluded from FDF file, then UEFI-compliant binary will be
#       generated for it, but the binary will not be put into any firmware volume.
#
###################################################################################################

[PcdsFixedAtBuild]
  gEfiStructuredPcdPkgTokenSpaceGuid.Test|{0xFF, 0xFF}
  gEfiStructuredPcdPkgTokenSpaceGuid.Test.A|MacroTest2
  gEfiStructuredPcdPkgTokenSpaceGuid.Test.C|"a"
  gEfiStructuredPcdPkgTokenSpaceGuid.Test.Array2[0]|2
  gEfiStructuredPcdPkgTokenSpaceGuid.Test.FlexibleArray[7]|5
  gEfiStructuredPcdPkgTokenSpaceGuid.Test.FlexibleArray[4]|L"C"
  gEfiStructuredPcdPkgTokenSpaceGuid.Test.FlexibleArray[3]|"b"
  gEfiStructuredPcdPkgTokenSpaceGuid.Test.Array2|{1, 2, 3}
  gEfiStructuredPcdPkgTokenSpaceGuid.Test.Array|L"Hello World"

[PcdsPatchableInModule]
  gEfiStructuredPcdPkgTokenSpaceGuid.Test2|{0xFF, 0xFF}
  gEfiStructuredPcdPkgTokenSpaceGuid.Test2.A|MacroTest2
  gEfiStructuredPcdPkgTokenSpaceGuid.Test2.C|"a"
  gEfiStructuredPcdPkgTokenSpaceGuid.Test2.Array2[0]|2
  gEfiStructuredPcdPkgTokenSpaceGuid.Test2.FlexibleArray[7]|5
  gEfiStructuredPcdPkgTokenSpaceGuid.Test2.FlexibleArray[4]|L"C"
  gEfiStructuredPcdPkgTokenSpaceGuid.Test2.FlexibleArray[3]|"b"
  gEfiStructuredPcdPkgTokenSpaceGuid.Test2.Guid|GUID("6F08F62E-5C19-498E-9157-B59CE6F362F1")
  gEfiStructuredPcdPkgTokenSpaceGuid.Test2.FlexibleArray|"World"
  gEfiStructuredPcdPkgTokenSpaceGuid.Test2.FlexibleArray|L"World"

[Components]
  TestPkg/Application/TestApp/TestApp.inf