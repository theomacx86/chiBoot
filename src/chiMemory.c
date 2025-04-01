#include "Uefi/UefiSpec.h"
#include "Include/chiMemory.h"

#include <Uefi.h>
#include <Library/UefiLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/BaseMemoryLib.h>


EFI_STATUS
BuildMemoryMap (
  MemoryMapInfo_S * KMemoryMap
  )
{
  EFI_STATUS Status;
  UINTN MemoryMapSize = 0;
  EFI_MEMORY_DESCRIPTOR * MemoryMap = NULL;
  EFI_MEMORY_DESCRIPTOR * Current = NULL;
  UINTN DescriptorSize = 0;
  UINTN MapKey = 0;
  UINT32 DescriptorVersion = 0;
  UINTN TotalUsableMemory = 0;

  //Get memory map size first
  Status = gBS->GetMemoryMap(&MemoryMapSize, MemoryMap, &MapKey, &DescriptorSize, &DescriptorVersion);
  MemoryMapSize += DescriptorSize * 2;

  gBS->AllocatePool(EfiLoaderData, MemoryMapSize, (VOID**) &MemoryMap);

  //First attempt.
  Status = gBS->GetMemoryMap(&MemoryMapSize, MemoryMap, &MapKey, &DescriptorSize, &DescriptorVersion);

  while(Status == EFI_BUFFER_TOO_SMALL)
  {
    gBS->FreePool(MemoryMap);
    MemoryMapSize += DescriptorSize * 2;
    gBS->AllocatePool(EfiLoaderData, MemoryMapSize, (VOID**) &MemoryMap);
    Status = gBS->GetMemoryMap(&MemoryMapSize, MemoryMap, &MapKey, &DescriptorSize, &DescriptorVersion); //new attempt
  }

  if(Status == EFI_INVALID_PARAMETER)
  {
    Print(L"Idiot");
    gBS->FreePool(MemoryMap);
    return EFI_INVALID_PARAMETER;
  }

  KMemoryMap->Descriptor = MemoryMap;
  KMemoryMap->DescriptorSize = DescriptorSize;
  KMemoryMap-> DescriptorVersion = DescriptorVersion;
  KMemoryMap->MapKey = MapKey;
  KMemoryMap->MemoryMapSize = MemoryMapSize;

  Print(L"We have a memmap lol\n");
  Print(L"Map size %8u Number of descriptors %8u\n", MemoryMapSize, DescriptorSize);

  Current = MemoryMap;

  for(UINTN i = 0; i < MemoryMapSize / DescriptorSize; ++i)
  {
    if(Current->Type == EfiConventionalMemory ||
      Current->Type == EfiLoaderData ||
      Current->Type == EfiLoaderCode ||
      Current->Type == EfiBootServicesCode ||
      Current->Type == EfiBootServicesData ||
      Current->Type == EfiPersistentMemory) 
    {
      Print(L"Usable range %16lx %16lx\n", Current->PhysicalStart, Current->NumberOfPages * 4096);
      TotalUsableMemory += Current->NumberOfPages * 4096;
    }
    //Current +=  DescriptorSize;
    Current = NEXT_MEMORY_DESCRIPTOR(Current, DescriptorSize);
  }

  return EFI_SUCCESS;
}

