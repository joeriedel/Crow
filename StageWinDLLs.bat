mkdir Bin
cd Bin
mkdir win-msvc8-debug
mkdir win-msvc8-developer
mkdir win-msvc8-golden
cd ..


xcopy Radiance\Extern\Win\Cg\2.2\bin\cg.dll Bin\win-msvc8-debug /D
xcopy Radiance\Extern\Win\Cg\2.2\bin\cgGL.dll Bin\win-msvc8-debug /D
xcopy Radiance\Extern\Win\Cg\2.2\bin\cg.dll Bin\win-msvc8-debug /D
xcopy Radiance\Extern\Win\Cg\2.2\bin\cg.dll Bin\win-msvc8-developer /D
xcopy Radiance\Extern\Win\Cg\2.2\bin\cgGL.dll Bin\win-msvc8-developer /D
xcopy Radiance\Extern\Win\Cg\2.2\bin\cg.dll Bin\win-msvc8-developer /D

xcopy Radiance\Extern\Win\Qt\4.5.2\vs8-dll\lib\QtCored4.dll Bin\win-msvc8-debug /D
xcopy Radiance\Extern\Win\Qt\4.5.2\vs8-dll\lib\QtGuid4.dll Bin\win-msvc8-debug /D
xcopy Radiance\Extern\Win\Qt\4.5.2\vs8-dll\lib\QtOpenGLd4.dll Bin\win-msvc8-debug /D
xcopy Radiance\Extern\Win\Qt\4.5.2\vs8-dll\lib\QtCore4.dll Bin\win-msvc8-developer /D
xcopy Radiance\Extern\Win\Qt\4.5.2\vs8-dll\lib\QtGui4.dll Bin\win-msvc8-developer /D
xcopy Radiance\Extern\Win\Qt\4.5.2\vs8-dll\lib\QtOpenGL4.dll Bin\win-msvc8-developer /D

xcopy Radiance\Extern\Win\SDL\1.2\SDL.dll Bin\win-msvc8-debug /D
xcopy Radiance\Extern\Win\SDL\1.2\SDL.dll Bin\win-msvc8-developer /D
xcopy Radiance\Extern\Win\SDL\1.2\SDL.dll Bin\win-msvc8-golden /D

xcopy Radiance\Extern\Win\VLD\1.9h\VS8\bin\dbghelp.dll Bin\win-msvc8-debug /D
xcopy Radiance\Extern\Win\VLD\1.9h\VS8\bin\Microsoft.DTfW.DHL.manifest Bin\win-msvc8-debug /D
xcopy Radiance\Extern\Win\VLD\1.9h\VS8\bin\vld.dll Bin\win-msvc8-debug /D
