/** @file

Copyright (c) 2007 - 2017, Intel Corporation. All rights reserved.<BR>
This program and the accompanying materials
are licensed and made available under the terms and conditions of the BSD License
which accompanies this distribution.  The full text of the license may be found at
http://opensource.org/licenses/bsd-license.php

THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

Module Name:

  NVDataStruc.h

Abstract:

  NVData structure used by the sample driver

Revision History:


**/

#ifndef _NVDATASTRUC_H_
#define _NVDATASTRUC_H_

#include <Guid/HiiPlatformSetupFormset.h>
#include <Guid/HiiFormMapMethodGuid.h>
#include <Guid/DriverSampleHii.h>
#include <Guid/ZeroGuid.h>

#define CONFIGURATION_VARSTORE_ID    0x1234
#define BITS_VARSTORE_ID    0x2345

#pragma pack(1)

typedef struct {
  UINT16   Field16;
  UINT8    MyBits4 : 1;  ///< Bits 0
  UINT8    MyBits3 : 3;  ///< Bits 3:1
  UINT8    MyBits5 : 3;  ///< Bits 6:4
  UINT16   MyBits6 : 4;  ///< Bits 3:0
  UINT8    Field8;
} MY_BITS_DATA;

typedef union {
  UINT16   MyBits7 : 10; ///< Bits 2:0
  UINT16   MyBits8 : 3; ///< Bits 5:3
  UINT16   MyBits9 : 1; ///< Bits 6
} MY_UNION_DATA1;

typedef union {
  UINT16   Field16;
  UINT8    Field8;
} MY_UNION_DATA2;

typedef union {
  MY_UNION_DATA1  Data1;
  MY_UNION_DATA2  Data2;
  MY_BITS_DATA    Data3;
} MY_EFI_UNION_DATA;

typedef struct {
  UINT16  MyStringData[40];
  UINT16  SomethingHiddenForHtml;
  UINT8   HowOldAreYouInYearsManual;
  UINT16  HowTallAreYouManual;
  UINT8   HowOldAreYouInYears;
  UINT16  HowTallAreYou;
  UINT8   MyFavoriteNumber;
  UINT8   TestLateCheck;
  UINT8   TestLateCheck2;
  UINT8   QuestionAboutTreeHugging;
  UINT8   ChooseToActivateNuclearWeaponry;
  UINT8   SuppressGrayOutSomething;
  UINT8   OrderedList[8];
  UINT16  BootOrder[8];
  UINT8   BootOrderLarge;
  UINT8   DynamicRefresh;
  UINT8   DynamicOneof;
  UINT8   DynamicOrderedList[5];
  UINT8   Reserved;
  EFI_HII_REF RefData;
  UINT8   NameValueVar0;
  UINT16  NameValueVar1;
  UINT16  NameValueVar2[20];
  UINT8   SerialPortNo;
  UINT8   SerialPortStatus;
  UINT16  SerialPortIo;
  UINT8   SerialPortIrq;
  UINT8   GetDefaultValueFromCallBack;
  UINT8   GetDefaultValueFromAccess;
  EFI_HII_TIME  Time;
  UINT8   RefreshGuidCount;
  UINT8   Match2;
  UINT8   GetDefaultValueFromCallBackForOrderedList[3];
  UINT8   Bits1 : 5;
  UINT8   Bits2 : 6;
  UINT8   Bits3 : 1;
  UINT16  Bits4 : 6;
  UINT16  Bits5 : 12;
  MY_BITS_DATA  MyBitData;
  MY_EFI_UNION_DATA MyUnionData;
} DRIVER_SAMPLE_CONFIGURATION;

//
// 2nd NV data structure definition
//
typedef struct {
  UINT8         Field8;
  UINT16        Field16;
  UINT8         OrderedList[3];
  UINT16        SubmittedCallback;
} MY_EFI_VARSTORE_DATA;

//
// 3rd NV data structure definition
//
typedef struct {
  UINT16   Field16;
  MY_BITS_DATA  BitsData;
  UINT8    Field8;
  UINT16   MyBits7 : 3; ///< Bits 2:0
  UINT16   MyBits8 : 3; ///< Bits 5:3
  UINT16   MyBits9 : 1; ///< Bits 6
} MY_EFI_BITS_VARSTORE_DATA;

//
// Labels definition
//
#define LABEL_UPDATE1               0x1234
#define LABEL_UPDATE2               0x2234
#define LABEL_UPDATE3               0x3234
#define LABEL_END                   0x2223

#pragma pack()

#endif
