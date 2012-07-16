call "%VS100COMNTOOLS%\..\..\VC\vcvarsall.bat" x86
devenv Source/VSProjects/VS10/Crow.sln /rebuild "Dev - Release|Win32"

pause