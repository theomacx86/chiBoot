/**
 * @file chiBoot.h
 * @author your name (you@domain.com)
 * @brief 
 * @version 0.1
 * @date 2025-03-06
 * 
 * @copyright Copyright (c) 2025
 * 
 */

#include <Uefi.h>
#include "E820.h"
#include "ProcessorBind.h"
#include "Uefi/UefiBaseType.h"


typedef struct LoaderData {
    UINT64 E820Count;
    E820_S * E820Entries;
    UINT64 KernelAddress;
    EFI_PHYSICAL_ADDRESS FramebufferAddress;

} LoaderData_S ;

EFI_STATUS
BuildMemoryMap(LoaderData_S * LoaderData);

EFI_STATUS
RegisterFramebuffer(LoaderData_S * LoaderData);

EFI_STATUS
PrintMemoryMap(
  LoaderData_S *LoaderData
);
