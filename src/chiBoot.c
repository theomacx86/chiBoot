#include "AutoGen.h"
#include "Base.h"
#include "ProcessorBind.h"
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
#include "Include/chiMemory.h"
#include "Include/elf.h"

EFI_LOADED_IMAGE_PROTOCOL  *LoadedImage;


EFI_STATUS
EFIAPI
UefiMain (
  IN EFI_HANDLE        ImageHandle,
  IN EFI_SYSTEM_TABLE  *SystemTable
  )
{
  EFI_STATUS     Status;
  Kernel_Params  KernelParams = { 0 };
  VOID           *KernelBuffer;
  CHAR16         *KernelName     = L"\\kernel.elf";
  UINTN          KernelPageCount = 0;
  UINTN KernelSize = 0;

  Status = gBS->HandleProtocol (ImageHandle, &gEfiLoadedImageProtocolGuid, (VOID **)&LoadedImage);
  if (EFI_ERROR (Status)) {
    Print (L"Failed to open gEfiLoadedImageProtocol, aborting !");
    return Status;
  }

  Status = BuildMemoryMap (&(KernelParams.MemoryMap));
  if (EFI_ERROR (Status)) {
    Print (L"Failed to retrieve memory map, aborting !");
    return Status;
  }

  Status = LoadKernelElf (KernelName, &KernelSize, &KernelBuffer, &KernelPageCount);
  
  UINT32 check = 0x7F454C46;
  if( *((UINT32*)KernelBuffer) == check )
  {
    Print(L"ELF");
  }
  else{
    Print(L"Not elf?");
  }
  
  return EFI_SUCCESS;
}

EFI_STATUS
LoadKernelElf (
  IN CHAR16  *FileName,
  OUT UINTN  *KernelSize,
  OUT VOID   **Buffer,
  OUT UINTN  *PageCount
  )
{
  EFI_STATUS                       Status;
  EFI_SIMPLE_FILE_SYSTEM_PROTOCOL  *Filesystem;
  EFI_FILE_PROTOCOL                *Root;
  EFI_FILE_PROTOCOL                *KernelFile;
  EFI_FILE_INFO                    *FileInfo;
  UINTN                            BufferSize;
  UINTN                     Pages;
  VOID                             *KernelBufer;

  Status = gBS->HandleProtocol (LoadedImage->DeviceHandle, &gEfiSimpleFileSystemProtocolGuid, (VOID **)&Filesystem);

  if (EFI_ERROR (Status)) {
    Print (L"Failed to locate filesystem protocol.\n");
    return Status;
  }

  Status = Filesystem->OpenVolume (Filesystem, &Root);
  if (EFI_ERROR (Status)) {
    Print (L"Failed to open volume.\n");
    return Status;
  }

  Status = Root->Open (Root, &Root, L"\\", EFI_FILE_MODE_READ, EFI_FILE_DIRECTORY);
  if (EFI_ERROR (Status)) {
    Print (L"Failed to open root folder.\n");
    return Status;
  }

  Status = Root->Open (Root, &KernelFile, L"kernel.elf", EFI_FILE_MODE_READ, EFI_FILE_READ_ONLY);
  if (EFI_ERROR (Status)) {
    Print (L"Failed to open kernel file.\n");
    return Status;
  }

  BufferSize = sizeof (EFI_FILE_INFO) + sizeof (CHAR16) * 256;
  FileInfo   = AllocatePool (BufferSize);

  Status = KernelFile->GetInfo (KernelFile, &gEfiFileInfoGuid, &BufferSize, FileInfo);
  if (EFI_ERROR (Status)) {
    FreePool(FileInfo);
    Print (L"Failed to open file info.\n");
    return Status;
  }

  Print(L"Filesize %16lu", (UINTN) FileInfo->FileSize );

  if(FileInfo->FileSize % PAGE_SIZE == 0)
  {
    Pages = FileInfo->FileSize / PAGE_SIZE;
  }
  else {
    Pages = FileInfo->FileSize / PAGE_SIZE + 1;
  }

  Print(L"Allocating %lu pages (%lu bytes)\n", Pages, FileInfo->FileSize);
  Status = gBS->AllocatePages(AllocateAnyPages, EfiLoaderCode, Pages, (EFI_PHYSICAL_ADDRESS*) &KernelBufer);

  BufferSize = Pages * PAGE_SIZE;

  if (EFI_ERROR (Status)) {
    Print (L"Failed to allocate buffer.\n");
    FreePool(FileInfo);
    return Status;
  }

  Status = KernelFile->Read(KernelFile, &BufferSize, KernelBufer);
  if (EFI_ERROR (Status)) {
    Print (L"Failed to read file in the buffer.\n");
    FreePool(FileInfo);
    FreePages(KernelBufer, Pages);
    return Status;
  }

  Print(L"Read %16lu bytes\n", BufferSize);

  if(BufferSize > 0)
  {
    if( *((UINT32*) KernelBufer) == ELF_MAGIC )
    {
      Print(L"File is ELF.\n");
    }
    else{
      FreePages(KernelBufer, Pages);
      FreePool(FileInfo);
    }
  }

  FreePool(FileInfo);

  //Read successful
  *Buffer = KernelBufer;
  *PageCount = Pages;
  *KernelSize = BufferSize;

  return EFI_SUCCESS;
}
