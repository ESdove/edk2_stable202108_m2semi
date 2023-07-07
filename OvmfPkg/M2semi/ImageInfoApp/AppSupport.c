#include <Uefi.h>
#include <Protocol/Shell.h>
#include <Protocol/ShellParameters.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/PrintLib.h>
#include <Library/UefiLib.h>
#include <Library/DebugLib.h>
#include <Library/ShellLib.h>

// Constant \ Macro\ Type \ Variable \ PPI \ GUID \ and Function Definition(s)
UINTN               Argc;
CHAR16              **Argv;
EFI_SHELL_PROTOCOL  *mShellProtocol = NULL;

/**
  This function parse application ARG.
  @return Status
**/
EFI_STATUS
GetArg (
  VOID
  )
{
  EFI_STATUS                     Status;
  EFI_SHELL_PARAMETERS_PROTOCOL  *ShellParameters;

  // UINTN                         i = 0;

  Status = gBS->HandleProtocol (
                                gImageHandle,
                                &gEfiShellParametersProtocolGuid,
                                (VOID **)&ShellParameters
                                );

  if (EFI_ERROR (Status)) {
    return Status;
  }

  Argc = ShellParameters->Argc;
  Argv = ShellParameters->Argv;

  // DEBUG((DEBUG_INIT,"Argc = %d\n",Argc));
  // for (i = 0; i < Argc; i++)
  // {
  //   DEBUG((DEBUG_INIT,"  Argv[%d] = %s\n",i,Argv[i]));
  // }

  return EFI_SUCCESS;
}

/**
  Get shell protocol.
  @return Pointer to shell protocol.
**/
EFI_SHELL_PROTOCOL *
GetShellProtocol (
  VOID
  )
{
  EFI_STATUS  Status;

  if (mShellProtocol == NULL) {
    Status = gBS->LocateProtocol (
                                  &gEfiShellProtocolGuid,
                                  NULL,
                                  (VOID **)&mShellProtocol
                                  );
    if (EFI_ERROR (Status)) {
      mShellProtocol = NULL;
    }
  }

  return mShellProtocol;
}

// Hex Processing

STATIC CONST CHAR8  Hex[] = {
  '0',
  '1',
  '2',
  '3',
  '4',
  '5',
  '6',
  '7',
  '8',
  '9',
  'A',
  'B',
  'C',
  'D',
  'E',
  'F'
};

VOID
EFIAPI
OemDumpHex (
  IN UINTN  DataSize,
  IN VOID   *UserData
  )
{
  UINT8  *Data;

  CHAR8  Val[50];

  CHAR8  Str[20];

  UINT8  TempByte;
  UINTN  Size;
  UINTN  Index;
  UINTN  RelOffset = 0;
  UINTN  *Offset   = UserData;

  Data = UserData;
  while (DataSize != 0) {
    Size = 16;
    if (Size > DataSize) {
      Size = DataSize;
    }

    for (Index = 0; Index < Size; Index += 1) {
      TempByte           = Data[Index];
      Val[Index * 3 + 0] = Hex[TempByte >> 4];
      Val[Index * 3 + 1] = Hex[TempByte & 0xF];
      Val[Index * 3 + 2] = (CHAR8)((Index == 7) ? '-' : ' ');
      Str[Index]         = (CHAR8)((TempByte < ' ' || TempByte > '~') ? '.' : TempByte);
    }

    Val[Index * 3] = 0;
    Str[Index]     = 0;
    ShellPrintEx (-1, -1, L"%E%*a%08X(%08X): %-48a *%a*\r\n", 2, "", Offset, RelOffset, Val, Str);

    Data      += Size;
    Offset     = (UINTN *)((UINT8 *)Offset + Size);
    RelOffset += Size;
    DataSize  -= Size;
  }
}
