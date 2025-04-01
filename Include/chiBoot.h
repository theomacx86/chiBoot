/**
 * @file chiBoot.h
 * @author Théo Jehl
 * @brief
 * @version 0.1
 * @date 2025-03-06
 *
 * @copyright Copyright Théo Jehl (c) 2025
 *
 */

#ifndef CHIBOOT_H
#define CHIBOOT_H

#include <Uefi.h>
#include "Include/chiMemory.h"
#include "ProcessorBind.h"
#include "Uefi/UefiBaseType.h"

typedef struct {
  MemoryMapInfo_S    MemoryMap;
} Kernel_Params;

EFI_STATUS
LoadKernelElf (
  IN CHAR16  *FileName,
  OUT UINTN  *KernelSize,
  OUT VOID   **Buffer,
  OUT UINTN  *PageCount
  );

#endif
