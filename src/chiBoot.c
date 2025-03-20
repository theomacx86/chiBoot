#include "Base.h"
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

  while(1);

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
  EFI_STATUS             Status;
  UINTN                  MemoryMapSize = 0, MapKey, DescriptorSize;
  UINT32                 DescriptorVersion;
  EFI_MEMORY_DESCRIPTOR  *MemoryMap = NULL;

  Status = gBS->GetMemoryMap (&MemoryMapSize, MemoryMap, &MapKey, &DescriptorSize, &DescriptorVersion);

  do {
    MemoryMap = (EFI_MEMORY_DESCRIPTOR *)AllocatePool (MemoryMapSize + DescriptorSize*2);
    ASSERT (MemoryMap != NULL);
    Status = gBS->GetMemoryMap (&MemoryMapSize, MemoryMap, &MapKey, &DescriptorSize, &DescriptorVersion);
    if (EFI_ERROR (Status)) {
      FreePool(MemoryMap);
    }
  } while (Status == EFI_BUFFER_TOO_SMALL);

  if(Status != EFI_SUCCESS)
  {
    Print(L"fucked");
  }
  ASSERT_EFI_ERROR (Status);

  EFI_MEMORY_DESCRIPTOR  *MemoryMapEntry, *MemoryMapEnd;

  MemoryMapEntry = MemoryMap;
  MemoryMapEnd   = (EFI_MEMORY_DESCRIPTOR *)((UINT8 *)MemoryMap + MemoryMapSize);
  while ((UINTN)MemoryMapEntry < (UINTN)MemoryMapEnd) {
        if(MemoryMapEntry->Type != EfiConventionalMemory && MemoryMapEntry->Type != EfiUnusableMemory) continue;
      Print (L"%8d %8d %4d\n", MemoryMapEntry->PhysicalStart, MemoryMapEntry->NumberOfPages, MemoryMapEntry->Type);

    MemoryMapEntry = NEXT_MEMORY_DESCRIPTOR (MemoryMapEntry, DescriptorSize);
  }

  return EFI_SUCCESS;
}
