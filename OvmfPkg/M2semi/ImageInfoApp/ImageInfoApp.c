#include <Uefi.h>
#include <Library/UefiLib.h>
#include <PiDxe.h>
#include <Library/BaseMemoryLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/DxeServicesTableLib.h>
#include <Library/DebugLib.h>
#include <Library/PrintLib.h>
#include <Library/DevicePathLib.h>
#include <Library/HobLib.h>
#include <Library/PcdLib.h>
#include <Library/UefiApplicationEntryPoint.h>
#include <Protocol/Shell.h>
#include <Protocol/ShellParameters.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/ShellLib.h>
#include <Protocol/LoadedImage.h>
#include <Guid/FileInfo.h>
#include <Protocol/DevicePathToText.h>
#include <Protocol/PciRootBridgeIo.h>
#include <Protocol/DevicePathToText.h>

extern UINTN               Argc;
extern CHAR16              **Argv;
extern EFI_SHELL_PROTOCOL  *mShellProtocol;

extern EFI_STATUS
GetArg (
  VOID
  );

extern EFI_SHELL_PROTOCOL *
GetShellProtocol (
  VOID
  );

STATIC VOID
ToolInfo (
  VOID
  )
{
  ShellPrintEx (-1, -1, L"%E+-----------------------------------------------------------------------------+\n");
  ShellPrintEx (-1, -1, L"%E|%V          Descriptions:         ImageInfoApp Utility                         %E|\n");
  ShellPrintEx (-1, -1, L"%E|%V          Version:  V1.0        Build Date:   2023-03-06                     %E|\n");
  ShellPrintEx (-1, -1, L"%E|%V          Copyright (c) 2023,   M2SEMI.   All rights Reserved                %E|\n");
  ShellPrintEx (-1, -1, L"%E|%V          Usage:    [-h]                                                     %E|\n");
  ShellPrintEx (-1, -1, L"%E+-----------------------------------------------------------------------------+\n");
}

VOID
PrintUsage (
  VOID
  )
{
  ShellPrintEx (-1, -1, L"%B-Info        Show Image Info\n");
  ShellPrintEx (-1, -1, L"%B-Chk [Addr]  Check Module Addr\n");
}

CHAR16  *mMemoryTypeShortName[] = {
  L"Reserved",
  L"LoaderCode",
  L"LoaderData",
  L"BS_Code",
  L"BS_Data",
  L"RT_Code",
  L"RT_Data",
  L"Available",
  L"Unusable",
  L"ACPI_Recl",
  L"ACPI_NVS",
  L"MMIO",
  L"MMIO_Port",
  L"PalCode",
  L"Persistent",
};

CHAR16  mUnknownStr[11];

STATIC CHAR16 *
ShortNameOfMemoryType (
  IN UINT32  Type
  )
{
  if (Type < sizeof (mMemoryTypeShortName) / sizeof (mMemoryTypeShortName[0])) {
    return mMemoryTypeShortName[Type];
  } else {
    UnicodeSPrint (mUnknownStr, sizeof (mUnknownStr), L"%08x", Type);
    return mUnknownStr;
  }
}

STATIC EFI_STATUS
ShowImageInfo (
  UINTN                      Index,
  EFI_LOADED_IMAGE_PROTOCOL  *mImageInfo
  )
{
  Print (L"%-4d============================================================\n", Index + 1);
  Print (
         L"  Start - End    : %p - %p(0X%X)\n",
         mImageInfo->ImageBase,
         (CHAR8 *)mImageInfo->ImageBase + mImageInfo->ImageSize,
         mImageInfo->ImageSize
         );
  Print (
         L"  ImageType       : Code : 0X%x(%s)   Data: 0X%x(%s)\n",
         mImageInfo->ImageCodeType,
         ShortNameOfMemoryType ((UINT32)mImageInfo->ImageCodeType),
         mImageInfo->ImageDataType,
         ShortNameOfMemoryType ((UINT32)mImageInfo->ImageDataType)
         );
  Print (L"  FilePath Type   : Type: 0x%x SubType: 0X%x\n", mImageInfo->FilePath->Type, mImageInfo->FilePath->SubType);
  if ((mImageInfo->FilePath->Type == 4) && (mImageInfo->FilePath->SubType == 6)) {
    Print (L"  Image Guid      : %g\n", mImageInfo->FilePath + 1);
  }

  Print (L"  DeviceHandle    : %p\n", mImageInfo->DeviceHandle);
  Print (L"  ParentHandle    : %p\n", mImageInfo->ParentHandle);
  // Print(L"  Load - LoadEnd  : %p - %p\n",mImageInfo->LoadOptions,
  //                                      (CHAR8 *)mImageInfo->LoadOptions + mImageInfo->LoadOptionsSize,
  //                                      mImageInfo->LoadOptionsSize);
  Print (L"\n");
  return EFI_SUCCESS;
}

STATIC EFI_STATUS
GetImageInfo (
  )
{
  EFI_STATUS  Status;
  UINTN       HandleCount;
  EFI_HANDLE  *HandleBuffer;
  UINTN       Index = 0;

  EFI_LOADED_IMAGE_PROTOCOL  *mImageInfo;

  Status = gBS->LocateHandleBuffer (
                                    ByProtocol,
                                    &gEfiLoadedImageProtocolGuid,
                                    NULL,
                                    &HandleCount,
                                    &HandleBuffer
                                    );

  if (EFI_ERROR (Status)) {
    Print (L"Can't Locate EFI_LOADED_IMAGE_PROTOCOL: %r\n", Status);
    return Status;
  }

  Print (L"Count of Image Protocol: %d\n", HandleCount);

  for (Index = 0; Index < HandleCount; Index++) {
    Status = gBS->HandleProtocol (
                                  HandleBuffer[Index],
                                  &gEfiLoadedImageProtocolGuid,
                                  (VOID **)&mImageInfo
                                  );
    if (EFI_ERROR (Status)) {
      continue;
    }

    Status = ShowImageInfo (Index, mImageInfo);
  }

  FreePool (HandleBuffer);
  return EFI_SUCCESS;
}

EFI_STATUS
EFIAPI
ImageInfoAppEntryPoint (
  IN EFI_HANDLE        ImageHandle,
  IN EFI_SYSTEM_TABLE  *SystemTable
  )
{
  GetShellProtocol ();

  GetArg ();

  if (Argc == 1) {
    ToolInfo ();
    return EFI_SUCCESS;
  } else if ((Argc == 2) && ((StrCmp (Argv[1], L"-h") == 0))) {
    PrintUsage ();
  } else if ((Argc == 2) && ((StrCmp (Argv[1], L"-Info") == 0))) {
    GetImageInfo ();
  } else if ((Argc == 3) && ((StrCmp (Argv[1], L"-Chk") == 0))) {
    EFI_STATUS  Status;
    UINTN       HandleCount;
    EFI_HANDLE  *HandleBuffer;
    UINTN       Index = 0;

    EFI_LOADED_IMAGE_PROTOCOL  *mImageInfo;

    Status = gBS->LocateHandleBuffer (
                                      ByProtocol,
                                      &gEfiLoadedImageProtocolGuid,
                                      NULL,
                                      &HandleCount,
                                      &HandleBuffer
                                      );

    if (EFI_ERROR (Status)) {
      Print (L"Can't Locate EFI_LOADED_IMAGE_PROTOCOL: %r\n", Status);
      return Status;
    }

    UINTN  ChkAddress = StrHexToUintn (Argv[2]);

    for (Index = 0; Index < HandleCount; Index++) {
      Status = gBS->HandleProtocol (
                                    HandleBuffer[Index],
                                    &gEfiLoadedImageProtocolGuid,
                                    (VOID **)&mImageInfo
                                    );
      if (EFI_ERROR (Status)) {
        continue;
      }

      if ((ChkAddress >= (UINTN)mImageInfo->ImageBase) && ((ChkAddress <= (UINTN)(UINTN)mImageInfo->ImageBase + mImageInfo->ImageSize))) {
        Print (L"Index = %d\n", Index);
        Print (L"ImageBase = %p\n", mImageInfo->ImageBase);
      }
    }

    Print (L"Not Found Match Image Address\n");
    FreePool (HandleBuffer);
    return EFI_SUCCESS;
  } else {
    Print (L"Invalid command line option(s)\n");
  }

  return EFI_SUCCESS;
}
