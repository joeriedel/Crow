call "%VS80COMNTOOLS%vsvars32.bat"
devenv Source/VS8Projects/Crow.sln /rebuild "Release|Win32" /project QGames
call scons --nocom --nounittests --developer -j 4 --clean
call scons --nocom --nounittests --developer -j 4
call StageWinDLLs.bat
pause