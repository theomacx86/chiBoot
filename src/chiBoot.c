#include "Base.h"
#include "Include/E820.h"
#include "Library/BaseMemoryLib.h"
#include "Library/DebugLib.h"
#include "Library/DevicePathLib.h"
#include "ProcessorBind.h"
#include "Protocol/DevicePath.h"
#include "Protocol/GraphicsOutput.h"
#include "Protocol/LoadedImage.h"
#include "Uefi/UefiBaseType.h"
#include "Uefi/UefiMultiPhase.h"
#include "Uefi/UefiSpec.h"
#include <Library/UefiLib.h>
#include <Library/UefiApplicationEntryPoint.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Protocol/SimpleFileSystem.h>
#include <Library/MemoryAllocationLib.h>
#include <Guid/FileInfo.h>

#include "Include/chiBoot.h"

EFI_LOADED_IMAGE  image;
GUID              ImageHandleGuid = EFI_LOADED_IMAGE_PROTOCOL_GUID;

LoaderData_S  *LoaderData;

EFI_STATUS
EFIAPI
UefiMain (
  IN EFI_HANDLE        ImageHandle,
  IN EFI_SYSTEM_TABLE  *SystemTable
  )
{
  EFI_STATUS                       Status;
  EFI_LOADED_IMAGE_PROTOCOL        *LoadedImage;
  EFI_SIMPLE_FILE_SYSTEM_PROTOCOL  *FileSystem;
  EFI_FILE_PROTOCOL                *FileProtocol;
  EFI_FILE_INFO                    *FileInfo;
  UINTN                             BufferSize;
  
  Status = gBS->HandleProtocol (ImageHandle, &gEfiLoadedImageProtocolGuid, (VOID **)&LoadedImage);
  if (EFI_ERROR (Status)) {
    return Status;
  }

  // Attempt to open the filesystem
  Status = gBS->HandleProtocol (LoadedImage->DeviceHandle, &gEfiSimpleFileSystemProtocolGuid, (VOID **)&FileSystem);
  if (EFI_ERROR (Status)) {
    Print (L"Failed to get filesystem protocol");
    return Status;
  }

  Status = FileSystem->OpenVolume (FileSystem, &FileProtocol);
  if (EFI_ERROR (Status)) {
    Print (L"Failed to open filesystem");
    return Status;
  }
    
  BufferSize = sizeof(EFI_FILE_INFO) + sizeof(CHAR16) * 256;
  FileInfo = AllocateZeroPool(BufferSize);
  if(FileInfo == NULL)
  {
    Print (L"Failed to allocate mem");
    return EFI_OUT_OF_RESOURCES;
  }
  
  Status = FileProtocol->Read(FileProtocol, &BufferSize, (VOID*) FileInfo);
  if (EFI_ERROR (Status)) {
    Print (L"Failed to read idk what");
    return Status;
  }

  // Loop through the directory entries and print directories
  while (TRUE) {
    if (EFI_ERROR(Status)) {
      break;
    }

    // Check if the file is a directory
    if ((FileInfo->Attribute & EFI_FILE_DIRECTORY) != 0) {
      Print(L"Directory: %s\n", FileInfo->FileName);
    }

    // Read next entry
    Status = FileProtocol->Read(FileProtocol, &BufferSize, (VOID*)FileInfo);
    if (EFI_ERROR(Status)) {
      break;
    }
  }

  //while(1);

  Status = gBS->AllocatePool (EfiLoaderData, sizeof (LoaderData_S), (VOID **)&LoadedImage);
  if (EFI_ERROR (Status)) {
    Print (L"Failed to allocate Loader data structure.");
    return Status;
  }

  Status = BuildMemoryMap (LoaderData);
  if (EFI_ERROR (Status)) {
    Print (L"Fucked.");
    return Status;
  }

  return EFI_SUCCESS;
}

EFI_STATUS
RegisterFramebuffer(LoaderData_S * LoaderData)
{
    EFI_STATUS Status;
    EFI_GRAPHICS_OUTPUT_PROTOCOL * Gop;
    //EFI_GRAPHICS_OUTPUT_MODE_INFORMATION * GopInfo;
    Status = gBS->LocateProtocol(&gEfiGraphicsOutputProtocolGuid, NULL, (VOID**) &Gop);

    if(EFI_ERROR(Status))
    {
        Print(L"GOP Is not available.\n");
        LoaderData->FramebufferAddress = (EFI_PHYSICAL_ADDRESS) NULL;
        return EFI_NOT_FOUND;
    }

    return EFI_SUCCESS;
}

EFI_STATUS
BuildMemoryMap (
  LoaderData_S  *LoaderData
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
    return EFI_INVALID_PARAMETER;
  }

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

  Print(L"Total available memory (in bytes) %16lx\n", TotalUsableMemory);

  Status = gBS->AllocatePool(EfiLoaderData, MemoryMapSize / DescriptorSize, (VOID**) &(LoaderData->E820Entries));
  SetMem(LoaderData->E820Entries, (MemoryMapSize/DescriptorSize), 0);

  if(EFI_ERROR(Status))
  {
    Print(L"Failed to allocate E820 buffer");
    gBS->FreePool(MemoryMap);
    return Status;
  }

  LoaderData->E820Count = MemoryMapSize / DescriptorSize;

  for(UINTN i = 0; i < MemoryMapSize / DescriptorSize; ++i)
  {
    if(Current->Type == EfiConventionalMemory ||
      Current->Type == EfiLoaderData ||
      Current->Type == EfiLoaderCode ||
      Current->Type == EfiBootServicesCode ||
      Current->Type == EfiBootServicesData ||
      Current->Type == EfiPersistentMemory) 
    {
      LoaderData->E820Entries[i].Type = E820_RAM;
    }

    if(Current->Type == EfiReservedMemoryType ||
      Current->Type == EfiRuntimeServicesCode ||
      Current->Type == EfiRuntimeServicesData ||
      Current->Type == EfiUnusableMemory ||
      Current->Type == EfiPersistentMemory ||
      Current->Type == EfiPalCode)
    {
      LoaderData->E820Entries[i].Type = E820_UNUSABLE;
    }

    if(Current->Type == EfiACPIReclaimMemory)
    {
      LoaderData->E820Entries[i].Type = E820_ACPI;
    }

    if(Current->Type == EfiACPIMemoryNVS)
    {
      LoaderData->E820Entries[i].Type = E820_NVS;
    }

    if(Current->Type == EfiMemoryMappedIO ||
      Current->Type == EfiMemoryMappedIOPortSpace )
    {
      LoaderData->E820Entries[i].Type = E820_RESERVED;
    }

    if(LoaderData->E820Entries[i].Type == 0)
    {
      LoaderData->E820Entries[i].Type = E820_UNUSABLE;
    }

    LoaderData->E820Entries[i].Address = Current->PhysicalStart;
    LoaderData->E820Entries[i].Size = Current->NumberOfPages * 4096;
  }
  
  //Memory map successfully built.
  gBS->FreePool(MemoryMap);
  
  return EFI_SUCCESS;
}
