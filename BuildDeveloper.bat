call "%VS100COMNTOOLS%..\..\VC\vcvarsall.bat" x86
devenv Source/VSProjects/VS10/Crow.sln /clean "Dev - Release|Win32"
devenv Source/VSProjects/VS10/Crow.sln /build "Dev - Release|Win32" /project QGames
devenv Source/VSProjects/VS10/Crow.sln /build "Dev - Release|Win32" /project Crow
pause