call "%VS80COMNTOOLS%vsvars32.bat"
devenv Source/VS8Projects/Crow.sln /build "Release|Win32" /project QGames
call scons --nocom --nounittests --developer -j 4
call StageWinDLLs.bat
pause