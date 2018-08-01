; PunkBuster RtCW 1.0 Installer
; Created by Martin Crawford
; Revision 3
;	- Adding pbsvc.exe to start PnkBstrA and B
; r.2 - using pbsetup.exe

;--------------------------------
!include LogicLib.nsh

; The name of the installer
Name "RtCW 1.4 Master Server Fix"
!define APP_NAME "Return to Castle Wolfenstein 1.4 Master List Fix"

; The file to write
OutFile "RtCW PunkBuster Fix.exe"

; The default installation directory
; Martin - The trailing \ prevents NSIS from appending Return to... on whatever the user picks (who thought that would be a good idea...)
;		http://www.nullsoft.com/free/nsis/makensis.htm
InstallDir "$PROGRAMFILES\Return to Castle Wolfenstein\"

;DirText "This will install Return to Castle Wolfenstein PunkBuster on your computer.\n  Choose your RtCW folder"

XPStyle on

; Request application privileges for Windows Vista
RequestExecutionLevel admin

!define StrRep "!insertmacro StrRep"
!macro StrRep output string old new
    Push "${string}"
    Push "${old}"
    Push "${new}"
    !ifdef __UNINSTALL__
        Call un.StrRep
    !else
        Call StrRep
    !endif
    Pop ${output}
!macroend
 
!macro Func_StrRep un
    Function ${un}StrRep
        Exch $R2 ;new
        Exch 1
        Exch $R1 ;old
        Exch 2
        Exch $R0 ;string
        Push $R3
        Push $R4
        Push $R5
        Push $R6
        Push $R7
        Push $R8
        Push $R9
 
        StrCpy $R3 0
        StrLen $R4 $R1
        StrLen $R6 $R0
        StrLen $R9 $R2
        loop:
            StrCpy $R5 $R0 $R4 $R3
            StrCmp $R5 $R1 found
            StrCmp $R3 $R6 done
            IntOp $R3 $R3 + 1 ;move offset by 1 to check the next character
            Goto loop
        found:
            StrCpy $R5 $R0 $R3
            IntOp $R8 $R3 + $R4
            StrCpy $R7 $R0 "" $R8
            StrCpy $R0 $R5$R2$R7
            StrLen $R6 $R0
            IntOp $R3 $R3 + $R9 ;move offset by length of the replacement string
            Goto loop
        done:
 
        Pop $R9
        Pop $R8
        Pop $R7
        Pop $R6
        Pop $R5
        Pop $R4
        Pop $R3
        Push $R0
        Push $R1
        Pop $R0
        Pop $R1
        Pop $R0
        Pop $R2
        Exch $R1
    FunctionEnd
!macroend
!insertmacro Func_StrRep ""
!insertmacro Func_StrRep "un."
	  

;MessageBox MB_OK "Please make sure the install path is correct!"

;--------------------------------

; Pages

Page directory
Page instfiles

;--------------------------------

; The stuff to install
Section "" ;No components page, name is not important

; ****************************
; Steam RtCW
; ****************************
ReadRegStr $0 HKLM "SOFTWARE\Microsoft\Windows\CurrentVersion\Uninstall\Steam App 9010" "InstallLocation"
${If} $0 == ""
	DetailPrint "Steam RtCW NOT found"
${Else}
	DetailPrint "Patching Steam RtCW(1.4) at $0"
	SetOutPath $0
	; Clean up old Files
	Delete "$0\Quake3.exe"
	Delete "$0\WolfMP.exe"
	
	; Install Files
	File /r 1.4\*.*
	File /r pb
	
	; Start PnkBstrA
	; Martin - 6/29/10
	File pbsvc.exe
	ExecWait '"$0\pbsvc.exe" /i --no-prompts --i-accept-the-pb-eula'
	
	; Cleanup installed File
	Delete "$0\pbsvc.exe"
	Delete "$0\pbsetup.exe"
	Delete "$0\pbuser.htm"
	Delete "$0\pbgame.htm"
	CreateShortCut "$DESKTOP\Return to Castle Wolfenstein Multiplayer.lnk" "$0\WolfMP.exe"
${EndIf}
; ****************************
; Black CD RtCW
; ****************************
ReadRegStr $0 HKLM "SOFTWARE\Microsoft\Windows\CurrentVersion\Uninstall\Return to Castle Wolfenstein" "DisplayIcon"
${If} $0 == ""
	DetailPrint "Black RtCW NOT found, will install to the prompted directory!"
	DetailPrint "Patching RtCW 1.4 at $INSTDIR"
	SetOutPath $INSTDIR
	; Clean up old Files
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
	
	; Install Files
	File /r 1.4\*.*
	File /r pb
	
	; Start PnkBstrA
	; Martin - 6/29/10
	File pbsvc.exe
	ExecWait '"$INSTDIR\pbsvc.exe" /i --no-prompts --i-accept-the-pb-eula'
	
	; Cleanup installed File
	Delete "$INSTDIR\pbsvc.exe"
	Delete "$INSTDIR\pbsetup.exe"
	Delete "$INSTDIR\pbuser.htm"
	Delete "$INSTDIR\pbgame.htm"
	CreateShortCut "$DESKTOP\Return to Castle Wolfenstein S4NDMoD.lnk" "$INSTDIR\WolfMP.exe" "+set fs_game s4ndmod"
${Else}
	ReadRegStr $1 HKLM "SOFTWARE\Activision\Return to Castle Wolfenstein" "Version"
	${Select} $R1
		${Case} "1.4"
			DetailPrint "Patching RtCW 1.4 at $0"
			${StrRep} $0 $0 'WolfMP.exe,-0' ''
			SetOutPath $0
			; Clean up old Files
			Delete "$0\Quake3.exe"
			Delete "$0\WolfMP.exe"
			
			; Install Files
			File /r 1.4\*.*
			File /r pb
			
			; Start PnkBstrA
			; Martin - 6/29/10
			File pbsvc.exe
			ExecWait '"$0\pbsvc.exe" /i --no-prompts --i-accept-the-pb-eula'
			
			; Cleanup installed File
			Delete "$0\pbsvc.exe"
			Delete "$0\pbsetup.exe"
			Delete "$0\pbuser.htm"
			Delete "$0\pbgame.htm"
			CreateShortCut "$DESKTOP\Return to Castle Wolfenstein Multiplayer.lnk" "$0\WolfMP.exe"
		${Case} "1.41"
			DetailPrint "Patching RtCW 1.41 at $0"
			${StrRep} $0 $0 'WolfMP.exe,-0' ''
			SetOutPath $0
			; Clean up old Files
			Delete "$0\Quake3.exe"
			Delete "$0\WolfMP.exe"
			
			; Install Files
			File /r 1.4\*.*
			File /r pb
			
			; Start PnkBstrA
			; Martin - 6/29/10
			File pbsvc.exe
			ExecWait '"$0\pbsvc.exe" /i --no-prompts --i-accept-the-pb-eula'
			
			; Cleanup installed File
			Delete "$0\pbsvc.exe"
			Delete "$0\pbsetup.exe"
			Delete "$0\pbuser.htm"
			Delete "$0\pbgame.htm"
			CreateShortCut "$DESKTOP\Return to Castle Wolfenstein Multiplayer.lnk" "$0\WolfMP.exe"
	${EndSelect}
${EndIf}
SectionEnd ; end the section