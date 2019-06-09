#
# InstDrv Example, (c) 2003 Jan Kiszka (Jan Kiszka@web.de)
#

Name "InstDrv.dll test"

OutFile "InstDrv-Test.exe"

ShowInstDetails show

ComponentText "InstDrv Plugin Usage Example"

Page components
Page instfiles

Section "Install a Driver" InstDriver
    InstDrv::InitDriverSetup /NOUNLOAD "{4d36e978-e325-11ce-bfc1-08002be10318}" "IrCOMM2k"
    Pop $0
    DetailPrint "InitDriverSetup: $0"

    InstDrv::DeleteOemInfFiles /NOUNLOAD
    Pop $0
    DetailPrint "DeleteOemInfFiles: $0"
    StrCmp $0 "00000000" PrintInfNames ContInst1

  PrintInfNames:
    Pop $0
    DetailPrint "Deleted $0"
    Pop $0
    DetailPrint "Deleted $0"

  ContInst1:
    InstDrv::CreateDevice /NOUNLOAD
    Pop $0
    DetailPrint "CreateDevice: $0"

    SetOutPath $TEMP
    File "ircomm2k.inf"
    File "ircomm2k.sys"

    InstDrv::InstallDriver /NOUNLOAD "$TEMP\ircomm2k.inf"
    Pop $0
    DetailPrint "InstallDriver: $0"
    StrCmp $0 "00000000" PrintReboot ContInst2

  PrintReboot:
    Pop $0
    DetailPrint "Reboot: $0"

  ContInst2:
    InstDrv::CountDevices
    Pop $0
    DetailPrint "CountDevices: $0"
SectionEnd

Section "Uninstall the driver again" UninstDriver
    InstDrv::InitDriverSetup /NOUNLOAD "{4d36e978-e325-11ce-bfc1-08002be10318}" "IrCOMM2k"
    Pop $0
    DetailPrint "InitDriverSetup: $0"

    InstDrv::DeleteOemInfFiles /NOUNLOAD
    Pop $0
    DetailPrint "DeleteOemInfFiles: $0"
    StrCmp $0 "00000000" PrintInfNames ContUninst1

  PrintInfNames:
    Pop $0
    DetailPrint "Deleted $0"
    Pop $0
    DetailPrint "Deleted $0"

  ContUninst1:
    InstDrv::RemoveAllDevices
    Pop $0
    DetailPrint "RemoveAllDevices: $0"
    StrCmp $0 "00000000" PrintReboot ContUninst2

  PrintReboot:
    Pop $0
    DetailPrint "Reboot: $0"

  ContUninst2:
    Delete "$SYSDIR\system32\ircomm2k.sys"
SectionEnd