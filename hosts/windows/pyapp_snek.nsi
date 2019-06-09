[% extends "pyapp_msvcrt.nsi" %]

[% block ui_pages %]

!addplugindir ../../Instdrv/NSIS/Plugins
!addincludedir ../../Instdrv/NSIS/Includes
!include x64.nsh

[[ super() ]]

[% endblock %]

[% block install_files %]
    [[ super() ]]

    ; Install driver bits
    InstDrv::InitDriverSetup /NOUNLOAD {4D36E96D-E325-11CE-BFC1-08002BE10318} AltusMetrumSerial
    InstDrv::DeleteOemInfFiles /NOUNLOAD
    InstDrv::CreateDevice /NOUNLOAD
    
    ${DisableX64FSRedirection}
    IfFileExists $WINDIR\System32\PnPutil.exe 0 nopnp
	${DisableX64FSRedirection}
	nsExec::ExecToLog '"$WINDIR\System32\PnPutil.exe" -i -a "$INSTDIR\altusmetrum.inf"'
	Goto donedrv
nopnp:
	InstDrv::InstallDriver /NOUNLOAD "$INSTDIR\altusmetrum.inf"
donedrv:

[% endblock %]
