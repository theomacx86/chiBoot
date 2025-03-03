[Defines]
PLATFORM_NAME           = chiBootPkg
PLATFORM_GUID           = 4638FEC9-F462-46DA-AED9-0B840FECDAC4
PLATFORM_VERSION        = 1.0
SUPPORTED_ARCHITECTURES = X64
BUILD_TARGETS           = RELEASE|DEBUG|NOOPT
SKUID_IDENTIFIER        = DEFAULT
DSC_SPECIFICATION       = 0x00010006

!include MdePkg/MdeLibs.dsc.inc

[LibraryClasses]

BaseLib|MdePkg/Library/BaseLib/BaseLib.inf
UefiLib|MdePkg/Library/UefiLib/UefiLib.inf
UefiBootServicesTableLib|MdePkg/Library/UefiBootServicesTableLib/UefiBootServicesTableLib.inf
UefiApplicationEntryPoint|MdePkg/Library/UefiApplicationEntryPoint/UefiApplicationEntryPoint.inf
DebugLib|MdePkg/Library/BaseDebugLibNull/BaseDebugLibNull.inf
PcdLib|MdePkg/Library/DxePcdLib/DxePcdLib.inf
BaseMemoryLib|MdePkg/Library/BaseMemoryLib/BaseMemoryLib.inf
PrintLib|MdePkg/Library/BasePrintLib/BasePrintLib.inf
MemoryAllocationLib|MdePkg/Library/UefiMemoryAllocationLib/UefiMemoryAllocationLib.inf
DevicePathLib|MdePkg/Library/UefiDevicePathLibDevicePathProtocol/UefiDevicePathLibDevicePathProtocol.inf
UefiBootServicesTableLib|MdePkg/Library/UefiBootServicesTableLib/UefiBootServicesTableLib.inf
UefiRuntimeServicesTableLib|MdePkg/Library/UefiRuntimeServicesTableLib/UefiRuntimeServicesTableLib.inf

[Components]
chiBoot/chiBoot.inf
