!include WordFunc.nsh

; Definitions for Java Detection

!define JAVA_VERSION "6.0"

Function GetFileVersion
	!define GetFileVersion `!insertmacro GetFileVersionCall`
 
	!macro GetFileVersionCall _FILE _RESULT
		Push `${_FILE}`
		Call GetFileVersion
		Pop ${_RESULT}
	!macroend
 
	Exch $0
	Push $1
	Push $2
	Push $3
	Push $4
	Push $5
	Push $6
	ClearErrors
 
	GetDllVersion '$0' $1 $2
	IfErrors error
	IntOp $3 $1 >> 16
	IntOp $3 $3 & 0x0000FFFF
	IntOp $4 $1 & 0x0000FFFF
	IntOp $5 $2 >> 16
	IntOp $5 $5 & 0x0000FFFF
	IntOp $6 $2 & 0x0000FFFF
	StrCpy $0 '$3.$4.$5.$6'
	goto end
 
	error:
	SetErrors
	StrCpy $0 ''
 
	end:
	Pop $6
	Pop $5
	Pop $4
	Pop $3
	Pop $2
	Pop $1
	Exch $0
FunctionEnd

Function openLinkNewWindow
  Push $3
  Exch
  Push $2
  Exch
  Push $1
  Exch
  Push $0
  Exch

  ReadRegStr $1 HKCU "Software\Microsoft\Windows\CurrentVersion\Explorer\FileExts\.html\UserChoice" "Progid"
  IfErrors iexplore

  Goto foundbrowser
iexplore:
  StrCpy $1 "IE.AssocFile.HTM"

foundbrowser:  

  StrCpy $2 "\shell\open\command"

  StrCpy $3 $1$2

  ReadRegStr $0 HKCR $3 ""

# Get browser path
  DetailPrint $0
 
  StrCpy $2 '"'
  StrCpy $1 $0 1
  StrCmp $1 $2 +2 # if path is not enclosed in " look for space as final char
  StrCpy $2 ' '
  StrCpy $3 1
  loop:
    StrCpy $1 $0 1 $3
    DetailPrint $1
    StrCmp $1 $2 found
    StrCmp $1 "" found
    IntOp $3 $3 + 1
    Goto loop
 
  found:
    StrCpy $1 $0 $3
    StrCmp $2 " " +2
      StrCpy $1 '$1"'
 
  Pop $0
  Exec '$1 $0'
  Pop $0
  Pop $1
  Pop $2
  Pop $3
FunctionEnd
 
!macro _OpenURL URL
Push "${URL}"
Call openLinkNewWindow
!macroend
 
!define OpenURL '!insertmacro "_OpenURL"'

Function DoDetectJRE

  DetailPrint "Desired Java version ${JAVA_VERSION}"

  SearchPath $0 javaw.exe
  IfErrors no
  
  DetailPrint "Detected java in $0"

  ${GetFileVersion} "$0" $1
  IfErrors no

  DetailPrint "Java version $1"

  ${VersionCompare} $1 ${JAVA_VERSION} $2
  IntCmp $2 1 yes yes old

yes:
  StrCpy $0 2
  Goto done

old:
  StrCpy $0 1
  Goto done

no:
  StrCpy $0 0
  Goto done  

done:

FunctionEnd

var dialog
var hwnd
var null

var install
var quit
var skip

Function GetJRE
  ${OpenURL} "java.com"
  MessageBox MB_OK "Click OK to continue after completing the Java Install."
FunctionEnd

Function DetectJRE

  Call DoDetectJRE

  IntCmp $0 1 ask_old ask_no yes

ask_no:
  StrCpy $0 "Cannot find Java. Download and install?"
  Goto ask

ask_old:
  StrCpy $0 "Java version appears to be too old. Download and install?"
  Goto ask

ask:
  MessageBox MB_YESNOCANCEL $0 IDYES do_java IDNO skip_java

do_java:
  Call GetJRE


skip_java:
yes:

FunctionEnd
