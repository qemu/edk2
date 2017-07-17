This branch is used to enable Structure PCD feature. It bases on edk2 repo UDK2017 branch.

The branch owner:
Gao, Liming <liming.gao@intel.com>
Zhu, Yonghong <yonghong.zhu@intel.com>
Zeng, Star <star.zeng@intel.com>
Feng, Bob C <bob.c.feng@intel.com>
Bi, Dandan <dandan.bi@intel.com>

## Feature Introduction
Structure PCD is to support the same C structure/Enum/Macro definition in PCD and VFR, and specify the value for individual 
fields in a VOID* PCD in DEC/DSC/FDF/CL. It includes the group of the sub features. 

https://bugzilla.tianocore.org/show_bug.cgi?id=541 [BaseTools] Extended PCD Value Syntax to support the flexible value formats

https://bugzilla.tianocore.org/show_bug.cgi?id=542 [BaseTools] Structure PCD value assignment in DEC/DSC
  The first version POC has been added to support FixedAtBuild and Patchable StructurePcd value assignment in dec and dsc file.
  TestPkg DEC/DSC and Application shows StructurePcd declaration, value assignment and consumed in source code.
  TestPkg TestApp can directly be built with build command.
  POC has been updated to support DynamicPcd/DynamicExPcd. 
  TestPkg DSC/FDF are updated to include NT32 emualtion platform. 
  TestPkg TestApp are updated to consume structure PCD with all types.
  POC has been updated to support PCD when more than one SkuId. 
  TestPkg.dsc adds two SKUIDs cases. 

https://bugzilla.tianocore.org/show_bug.cgi?id=543 [BaseTools] Extended SKU Support - Inheritance

https://bugzilla.tianocore.org/show_bug.cgi?id=544 [BaseTools] PCD: Extended SKU support 2 - sub SKU

https://bugzilla.tianocore.org/show_bug.cgi?id=545 [BaseTools] Support Bit fields used as EFI VarStore/Buffer VarStore in VFR
  POC has been pushed. MdeModulePkg/DriverSample is also updated to add the sample code to show the structure
  with bit field and UNION type. 

https://bugzilla.tianocore.org/show_bug.cgi?id=546 [PCD] Pcd Database size optimization for multi-SKU

https://bugzilla.tianocore.org/show_bug.cgi?id=603 [BaseTools] VfrCompiler supports UNION type as VarStore in VFR 

https://bugzilla.tianocore.org/show_bug.cgi?id=611 Collect DynamicHii PCD value as the default setting for EFI Variable and HII VarStore

## Timeline
Target for 2017

### NOTES
Some changes in this branch are in BaseTools. To apply them, user needs to run BaseTools from sources. 
In Linux, BaseTools run from source. In Windows, BaseTools can run from source. The step is like below:
  cd edk2
  set PYTHON_HOME=C:\Python27
  edksetup.bat --nt32
  nmake -f BaseTools\Makefile
  build -p TestPkg\TestPkg.dsc -y Report.txt
  build -p TestPkg\TestPkg.dsc run # trig NT32 platform, and show dynamic PCD.
