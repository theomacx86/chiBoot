#include "Base.h"
#include "Library/DevicePathLib.h"
#include "Protocol/DevicePath.h"
#include "Protocol/LoadedImage.h"
#include "Uefi/UefiBaseType.h"
#include <Library/UefiLib.h>
#include <Library/UefiApplicationEntryPoint.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Protocol/SimpleFileSystem.h>

EFI_LOADED_IMAGE image;
GUID ImageHandleGuid = EFI_LOADED_IMAGE_PROTOCOL_GUID;

EFI_STATUS
EFIAPI
UefiMain (
  IN EFI_HANDLE        ImageHandle,
  IN EFI_SYSTEM_TABLE  *SystemTable
  )
{
    EFI_STATUS Status;
    EFI_LOADED_IMAGE_PROTOCOL * LoadedImage;
    EFI_SIMPLE_FILE_SYSTEM_PROTOCOL * FileSystem;
    EFI_FILE_PROTOCOL * FileProtocol;

    Status = gBS->HandleProtocol(ImageHandle, &gEfiLoadedImageProtocolGuid, (VOID**) &LoadedImage);
    if(EFI_ERROR(Status))
    {
        return Status;
    }

    //Attempt to open the filesystem 
    Status = gBS->HandleProtocol(LoadedImage->DeviceHandle, &gEfiSimpleFileSystemProtocolGuid, (VOID**) &FileSystem);
    if(EFI_ERROR(Status))
    {
        Print(L"Failed to get filesystem protocol");
        return Status;
    }

    Status = FileSystem->OpenVolume(FileSystem, &FileProtocol);
    if(EFI_ERROR(Status))
    {
        Print(L"Failed to open filesystem");
        return Status;
    }

    return EFI_SUCCESS;
}
