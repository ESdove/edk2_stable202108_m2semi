@echo off

REM 检查是否传入了参数
IF [%1]==[] (
    echo "Empty parameter"
    goto :end
)

REM 根据参数执行不同的命令
IF [%1]==[build] (
    edksetup.bat rebuild 
    build -t VS2019 -a IA32 -a X64 -p OvmfPkg\OvmfPkgIa32X64.dsc
) ELSE IF [%1]==[run] (
    set QEMU=C:\Dev\QEMU
    set WORKSPACE=%cd%
    set PATH=%QEMU%
    qemu-system-x86_64.exe ^
         -pflash Build\Ovmf3264\DEBUG_VS2019\FV\OVMF.fd ^
         -hda fat:rw:USB ^
         -nic none ^
         -debugcon file:debug.log ^
         -global isa-debugcon.iobase=0x402
) ELSE (
    echo "Invalid command line option(s)"
)

:end