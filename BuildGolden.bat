call "%VS100COMNTOOLS%..\..\VC\vcvarsall.bat" x86
devenv Source/VSProjects/VS10/Crow.sln /clean "Golden - Release|Win32"
devenv Source/VSProjects/VS10/Crow.sln /build "Golden - Release|Win32" /project Crow
pause