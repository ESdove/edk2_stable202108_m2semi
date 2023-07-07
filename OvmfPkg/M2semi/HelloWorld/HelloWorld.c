#include <Uefi.h>
#include <Library/UefiLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/UefiApplicationEntryPoint.h>
#include <Library/PrintLib.h>
#include <Library/DebugLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/DevicePathLib.h>

EFI_STATUS
EFIAPI
UefiMain (
  EFI_HANDLE        ImageHandle,
  EFI_SYSTEM_TABLE  *SystemTable
  )
{
  EFI_SIMPLE_TEXT_OUTPUT_PROTOCOL  *ConOut;
  EFI_STATUS                       Status;
  UINTN                            HandleCount;
  EFI_HANDLE                       *HandleBuffer;
  UINTN                            Index;

  Status = SystemTable->BootServices->LocateProtocol (&gEfiSimpleTextOutProtocolGuid, NULL, (void **)&ConOut);
  if (EFI_ERROR (Status)) {
    return Status;
  }

  DEBUG ((DEBUG_INFO, "ConOut = %x\n", ConOut));
  ConOut->OutputString (ConOut, L"Hello, World!\r\n");
  gST->ConOut->OutputString (gST->ConOut, L"Hello World\n\r");
  DEBUG ((DEBUG_INFO, "ConOut = %x\n", gST->ConOut));
  Status = gBS->LocateHandleBuffer (ByProtocol, &gEfiSimpleTextOutProtocolGuid, NULL, &HandleCount, &HandleBuffer);

  DEBUG ((DEBUG_INFO, "Handle Count = %d\n", HandleCount));
  for (Index = 0; Index < HandleCount; Index++) {
    Status = gBS->HandleProtocol (
                                  HandleBuffer[Index],
                                  &gEfiSimpleTextOutProtocolGuid,
                                  (VOID **)&ConOut
                                  );
    DEBUG ((DEBUG_INFO, "%2d Count=%x - %s\n", Index, ConOut,ConvertDevicePathToText (DevicePathFromHandle (HandleBuffer[Index]), TRUE, TRUE)));
    ConOut->OutputString(ConOut,L"TestConOut\n");
  }

  DEBUG ((DEBUG_INFO, "Hello M2semi\n"));
  return EFI_SUCCESS;
}
