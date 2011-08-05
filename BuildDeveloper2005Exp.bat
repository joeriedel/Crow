call "%VS80COMNTOOLS%vsvars32.bat"
rem devenv Source/VS8Projects/Crow.sln /rebuild "Release|Win32" /project QGames
rem call scons --nocom --nounittests --developer --2005Exp -j 4 --clean
call scons --nocom --nounittests --developer --2005Exp -j 4
call StageWinDLLs.bat
pause