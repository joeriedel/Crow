;-- Crow Installer

SetCompressor /SOLID /FINAL lzma

!include MUI2.nsh
!include MUI_EXTRAPAGES.nsh

Name "Crow Beta"
OutFile "Installer.exe"
InstallDir $PROGRAMFILES\Crow
InstallDirRegKey HKCU "Software\Crow" ""

RequestExecutionLevel admin

!define MUI_ABORTWARNING

!insertmacro MUI_PAGE_WELCOME
!insertmacro MUI_PAGE_LICENSE CrowEULA.rtf
!insertmacro MUI_PAGE_DIRECTORY

Var StartMenuFolder
!define MUI_STARTMENUPAGE_REGISTRY_ROOT "HKCU" 
!define MUI_STARTMENUPAGE_REGISTRY_KEY "Software\Crow" 
!define MUI_STARTMENUPAGE_REGISTRY_VALUENAME "Start Menu Folder"

!insertmacro MUI_PAGE_STARTMENU Application $StartMenuFolder

!insertmacro MUI_PAGE_INSTFILES
!insertmacro MUI_PAGE_README CrowReadme.rtf
!insertmacro MUI_PAGE_FINISH

!insertmacro MUI_UNPAGE_WELCOME
!insertmacro MUI_UNPAGE_CONFIRM
!insertmacro MUI_UNPAGE_INSTFILES
!insertmacro MUI_UNPAGE_FINISH

!insertmacro MUI_LANGUAGE "English"
;Set up install lang strings for 1st lang
	${ReadmeLanguage} "${LANG_ENGLISH}" \
          "Read Me" \
          "Please review the following important information." \
          "About $(^name):" \
          "$\n  Click on scrollbar arrows or press Page Down to review the entire text."
 
;Set up uninstall lang strings for 1st lang
${Un.ReadmeLanguage} "${LANG_ENGLISH}" \
          "Read Me" \
          "Please review the following important Uninstall information." \
          "About $(^name) Uninstall:" \
          "$\n  Click on scrollbar arrows or press Page Down to review the entire text."

Section "Dummy Section" SecDummy

  SetOutPath "$INSTDIR"
  
  ;ADD YOUR OWN FILES HERE...
  File Crow.exe
  File autoexec.txt
  File CrowEULA.rtf
  File CrowReadme.rtf
  File OpenAL32.dll
  File QtCore4.dll
  File QtGui4.dll
  File vcredist_x86.exe

  SetOutPath "$INSTDIR\Base"

  File Base\icon.tga
  File Base\launcher1.png
  File Base\launcher2.png
  File Base\pak0.pak

  SetOutPath "$INSTDIR\Concept Art"
  File "Concept Art\*.*"

  SetOutPath "$INSTDIR\Development Renders"
  File "Development Renders\*.*"

  SetOutPath "$INSTDIR\Marketing Images"
  File "Marketing Images\*.*"

  SetOutPath "$INSTDIR\Remastered Soundtrack"
  File "Remastered Soundtrack\*.*"

  ;Store installation folder
  WriteRegStr HKCU "Software\Crow" "" $INSTDIR

  ;Create uninstaller
  WriteUninstaller "$INSTDIR\Uninstall.exe"
  
  !insertmacro MUI_STARTMENU_WRITE_BEGIN Application
  
  ; this is the working directory for shortcuts
  SetOutPath "$INSTDIR"

  ;Create shortcuts
  CreateDirectory "$SMPROGRAMS\$StartMenuFolder"
  CreateShortCut "$SMPROGRAMS\$StartMenuFolder\Crow.lnk" "$INSTDIR\Crow.exe"
  CreateShortCut "$SMPROGRAMS\$StartMenuFolder\CrowEULA.lnk" "$INSTDIR\CrowEULA.rtf"
  CreateShortCut "$SMPROGRAMS\$StartMenuFolder\CrowReadme.lnk" "$INSTDIR\CrowReadme.rtf"
  CreateShortCut "$SMPROGRAMS\$StartMenuFolder\Uninstall.lnk" "$INSTDIR\Uninstall.exe"
  CreateShortCut "$SMPROGRAMS\$StartMenuFolder\Concept Art.lnk" "$INSTDIR\Concept Art"
  CreateShortCut "$SMPROGRAMS\$StartMenuFolder\Development Renders.lnk" "$INSTDIR\Development Renders"
  CreateShortCut "$SMPROGRAMS\$StartMenuFolder\Marketing Images.lnk" "$INSTDIR\Marketing Images"
  CreateShortCut "$SMPROGRAMS\$StartMenuFolder\Remastered Soundtrack.lnk" "$INSTDIR\Remastered Soundtrack"

  !insertmacro MUI_STARTMENU_WRITE_END

  ExecWait '"$INSTDIR\vcredist_x86.exe" /q /norestart'
  Delete vcredist_x86.exe

SectionEnd

LangString DESC_SecDummy ${LANG_ENGLISH} "A test section."
!insertmacro MUI_FUNCTION_DESCRIPTION_BEGIN
	!insertmacro MUI_DESCRIPTION_TEXT ${SecDummy} $(DESC_SecDummy)
!insertmacro MUI_FUNCTION_DESCRIPTION_END

Section "Uninstall"

	Delete "$INSTDIR\Crow.exe"
	Delete "$INSTDIR\CrowEULA.rtf"
	Delete "$INSTDIR\CrowReadme.rtf"
	Delete "$INSTDIR\OpenAL32.dll"
	Delete "$INSTDIR\QtCore4.dll"
	Delete "$INSTDIR\QtGui4.dll"
	Delete "$INSTDIR\Base\icon.tga"
	Delete "$INSTDIR\Base\launcher1.png"
	Delete "$INSTDIR\Base\launcher2.png"
	Delete "$INSTDIR\Base\pak0.pak"
	Delete "$INSTDIR\Uninstall.exe"
	Delete "$INSTDIR\Concept Art\*.*"
    Delete "$INSTDIR\Development Renders\*.*"
    Delete "$INSTDIR\Marketing Images\*.*"
    Delete "$INSTDIR\Remastered Soundtrack\*.*"
	Delete "$INSTDIR\autoexec.txt"
	Delete "$INSTDIR\log.txt"
	Delete "$INSTDIR\settings.prefs"
	Delete "$INSTDIR\globals.data"
	Delete "$INSTDIR\Save*.sav"
	Delete "$INSTDIR\vcredist_x86.exe"
  	RMDir "$INSTDIR\Base"
	RMDir "$INSTDIR\Concept Art"
	RMDir "$INSTDIR\Development Renders"
	RMDir "$INSTDIR\Marketing Images"
	RMDir "$INSTDIR\Remastered Soundtrack"
	RMDir "$INSTDIR"

	!insertmacro MUI_STARTMENU_GETFOLDER Application $StartMenuFolder

	Delete "$SMPROGRAMS\$StartMenuFolder\Crow.lnk"
	Delete "$SMPROGRAMS\$StartMenuFolder\EULA.lnk"
	Delete "$SMPROGRAMS\$StartMenuFolder\README.lnk"
	Delete "$SMPROGRAMS\$StartMenuFolder\Uninstall.lnk"
	RMDir "$SMPROGRAMS\$StartMenuFolder"
	
	DeleteRegKey /ifempty HKCU "Software\Crow"
	
SectionEnd
