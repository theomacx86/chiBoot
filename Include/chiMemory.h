/**
 * @file chiMemory.h
 * @author Théo Jehl
 * @brief Memory management header
 * @version 0.1
 * @date 2025-04-01
 *
 * @copyright Copyright Théo Jehl (c) 2025
 *
 */

#ifndef CHIMEMORY_H
#define CHIMEMORY_H

#include "ProcessorBind.h"
#include <Uefi.h>

#define PAGE_SIZE  4096

// Page mask used for virtual addresses and PML offset
#define PHYS_PAGE_ADDRESS_MARK  0x0000FFFFFFFFF000

/*
  Page table
*/
typedef struct{
  UINTN Pages[512];
} PageTable_S;

/*
  Memory map info passed to the kernel.
*/
typedef struct {
  UINTN                    MemoryMapSize;
  EFI_MEMORY_DESCRIPTOR    *Descriptor;
  UINTN                    MapKey;
  UINTN                    DescriptorSize;
  UINTN                    DescriptorVersion;
} MemoryMapInfo_S;

/**
 * @brief Retrieve the UEFI memory map.
 *
 * @param MemoryMap
 * @return EFI_STATUS
 */
EFI_STATUS
BuildMemoryMap (
  MemoryMapInfo_S  *KMemoryMap
  );

/**
 * @brief Allocate pages.
 * 
 * @param PageCount Number of pages
 * @return VOID* NULL if failure, pointer to bufffer otherwise
 */
VOID *
MmapAllocatePages(
  UINTN PageCount,
  MemoryMapInfo_S * MemoryMap
);
#endif
