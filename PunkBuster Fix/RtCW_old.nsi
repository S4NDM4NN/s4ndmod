; PunkBuster RtCW 1.0 Installer
; Created by Martin Crawford
; Revision 3
;	- Adding pbsvc.exe to start PnkBstrA and B
; r.2 - using pbsetup.exe

;--------------------------------
!include LogicLib.nsh

; The name of the installer
Name "PunkBuster Patch for RtCW 1.0"
!define APP_NAME "Return to Castle Wolfenstein"

; The file to write
OutFile "RtCW_1.0_PB.exe"

; The default installation directory
; Martin - The trailing \ prevents NSIS from appending Return to... on whatever the user picks (who thought that would be a good idea...)
;		http://www.nullsoft.com/free/nsis/makensis.htm
InstallDir "$PROGRAMFILES\Return to Castle Wolfenstein\"

;DirText "This will install Return to Castle Wolfenstein PunkBuster on your computer.\n  Choose your RtCW folder"

XPStyle on

; Request application privileges for Windows Vista
RequestExecutionLevel admin

;MessageBox MB_OK "Please make sure the install path is correct!"

;--------------------------------

; Pages

Page directory
Page instfiles

;--------------------------------

; The stuff to install
Section "" ;No components page, name is not important

  ; Set output path to the installation directory.
  SetOutPath $INSTDIR
  
  ; Clean old files
  Delete "$INSTDIR\main\mp_bin.pk3"
  Delete "$INSTDIR\main\cgame_mp_x86.dll"
  Delete "$INSTDIR\main\ui_mp_x86.dll"
  Delete "$INSTDIR\cgame_mp_x86.dll"
  Delete "$INSTDIR\ui_mp_x86.dll"
  Delete "$INSTDIR\Quake3.exe"
  Delete "$INSTDIR\WolfMP.exe"
  Delete "$INSTDIR\s4ndmod\mp_bin.pk3"
  Delete "$INSTDIR\s4ndmod\cgame_mp_x86.dll"
  Delete "$INSTDIR\s4ndmod\ui_mp_x86.dll"
  
  ; Delete PB Installer files
  Delete "$INSTDIR\pbsetup.exe"
  Delete "$INSTDIR\pbuser.htm"
  Delete "$INSTDIR\pbgame.htm"
  
  ; Remove HTTP Downloader
  DetailPrint "Please wait while HTTP Downloader is uninstalled."
  ExecWait "$INSTDIR\http_download\Uninstall.exe"
  ;Delete "$INSTDIR\http_download"
  ;sleep 3000
  MessageBox MB_OK "Press OK to continue."
  RmDir /r "$INSTDIR\http_download"
 
  ; Install Files
  File Quake3.exe
  File /r pb
  File /r main
  File /r s4ndmod
  
  ; Start PnkBstrA
  ; Martin - 6/29/10
  File pbsvc.exe
  ExecWait '"$INSTDIR\pbsvc.exe" /i --no-prompts --i-accept-the-pb-eula'
  
  ; Cleanup installed File
  Delete "$INSTDIR\pbsvc.exe"
  Delete "$INSTDIR\pbsetup.exe"
  Delete "$INSTDIR\pbuser.htm"
  Delete "$INSTDIR\pbgame.htm"
  
  ; Shortcut please!
  CreateShortCut "$DESKTOP\Return to Castle Wolfenstein.lnk" "$INSTDIR\Quake3.exe" "+set fs_game s4ndmod"
  MessageBox MB_OK "A shortcut has been placed on your desktop."
  
SectionEnd ; end the section
