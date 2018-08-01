; Master server Demo Patch

;--------------------------------
!include LogicLib.nsh

; The name of the installer
Name "Master Server Patch for RtCW Demo"
!define APP_NAME "Return to Castle Wolfenstein"

; The file to write
OutFile "RtCW_Demo_patched_master.exe"

; The default installation directory
; Martin - The trailing \ prevents NSIS from appending Return to... on whatever the user picks (who thought that would be a good idea...)
;		http://www.nullsoft.com/free/nsis/makensis.htm
InstallDir "$PROGRAMFILES\Return to Castle Wolfenstein Multiplayer DEMO\"

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
  Delete "$INSTDIR\WolfMPDemo.exe"
  
  ; Install Files
  File WolfMPDemo.exe
  
  MessageBox MB_OK "You are now using the new master server list!"
  
SectionEnd ; end the section
