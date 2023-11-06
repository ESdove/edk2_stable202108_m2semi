#include <Uefi.h>
#include <PiDxe.h>
#include <Library/BaseLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/UefiRuntimeServicesTableLib.h>
#include <Library/DxeServicesTableLib.h>
#include <Library/UefiLib.h>
#include <Library/DebugLib.h>
#include <Library/PrintLib.h>
#include <Library/HobLib.h>
#include <Library/PcdLib.h>
#include <Library/UefiApplicationEntryPoint.h>
#include <Protocol/Shell.h>
#include <Protocol/ShellParameters.h>
#include <Guid/HobList.h>
#include <Library/MemoryAllocationLib.h>
#include <Guid/MemoryAllocationHob.h>
#include <Library/ShellLib.h>
#include <Protocol/HiiDatabase.h>

#define DEFAULT_DAY   01
#define DEFAULT_MONTH 01
#define DEFAULT_YEAR  2021
#define DEFAULT_HOUR   06
#define DEFAULT_MINUTE 06
#define DEFAULT_SECOND 06

extern UINTN Argc;
extern CHAR16 **Argv;
extern EFI_SHELL_PROTOCOL *mShellProtocol;
extern EFI_SHELL_PROTOCOL *GetShellProtocol(VOID);
extern EFI_STATUS GetArg(VOID);


VOID ToolInfo(VOID)
{
  ShellPrintEx(-1, -1, L"%E+-----------------------------------------------------------------------------+\n");
  ShellPrintEx(-1, -1, L"%E|%V          Descriptions:         Time Utility                                 %E|\n");
  ShellPrintEx(-1, -1, L"%E|%V          Version:  V1.0        Build Date:   %a - %a         %E|\n",__DATE__,__TIME__);
  ShellPrintEx(-1, -1, L"%E|%V          Copyright (c) 2023,   M2SEMI.   All rights Reserved                %E|\n");
  ShellPrintEx(-1, -1, L"%E|%V          Usage:    [-h]                                                     %E|\n");
  ShellPrintEx(-1, -1, L"%E+-----------------------------------------------------------------------------+\n");
}

VOID PrintUsage(VOID)
{
  ShellPrintEx(-1, -1, L"%B-GetTime        Get Current Time\n"); 
  ShellPrintEx(-1, -1, L"%B-Default        Set Default Time 2021/01/01 - 06/06/06\n"); 
  ShellPrintEx(-1, -1, L"%B-SetTime        Set Time eg: -SetTime 2023 11 06 09 30 00\n"); 
}



EFI_STATUS
EFIAPI
Time(
    IN EFI_HANDLE ImageHandle,
    IN EFI_SYSTEM_TABLE *SystemTable)
{
  EFI_STATUS   Status;
  EFI_TIME     Time;

  GetShellProtocol();

  GetArg();


  if (Argc == 1) {
    ToolInfo();
    return EFI_SUCCESS;
  } else if (Argc == 2 && ((StrCmp(Argv[1], L"-h") == 0))){
    PrintUsage();
  } else if(Argc == 2 && ((StrCmp(Argv[1], L"-GetTime") == 0))){

    Status = gRT->GetTime(&Time, NULL);
    if (!EFI_ERROR(Status)){
    Print(L"Time = %04d/%02d/%02d - %02d/%02d/%02d\n",Time.Year,Time.Month,Time.Day,Time.Hour,Time.Minute,Time.Second);                       
    }
  } else if(Argc == 2 && ((StrCmp(Argv[1], L"-Default") == 0))){
    Time.Hour = DEFAULT_HOUR;
    Time.Minute = DEFAULT_MINUTE;
    Time.Second = DEFAULT_SECOND;
    Time.Month = DEFAULT_MONTH;
    Time.Day = DEFAULT_DAY;
    Time.Year = DEFAULT_YEAR;
    Status = gRT->SetTime(&Time);
    if(!EFI_ERROR (Status)){
      Print(L"Reset Default Time 2021/01/01 - 06/06/06\n");
    }
   
  } else if(Argc == 8 && ((StrCmp(Argv[1], L"-SetTime") == 0))){
     UINT16 Year;
     UINT8 Hour,Minute,Second,Month,Day;

     Year = (UINT16)StrDecimalToUintn(Argv[2]);
     Month = (UINT8)StrDecimalToUintn(Argv[3]);
     Day = (UINT8)StrDecimalToUintn(Argv[4]);
     Hour = (UINT8)StrDecimalToUintn(Argv[5]);
     Minute = (UINT8)StrDecimalToUintn(Argv[6]);
     Second = (UINT8)StrDecimalToUintn(Argv[7]);

    Status = gRT->GetTime(&Time, NULL);
    if (!EFI_ERROR(Status)){
      Time.Year = Year;
      Time.Month = Month;
      Time.Day = Day;
      Time.Hour = Hour;
      Time.Minute = Minute;
      Time.Second = Second;
      Status = gRT->SetTime(&Time);
    
      if(!EFI_ERROR (Status)){
       Print(L"SetTime Successfully\n");
      }
    }
   
  } else{
    Print(L"Invalid command line option(s)\n");
  }

  return EFI_SUCCESS;
}
