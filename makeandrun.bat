@SETLOCAL
@IF EXIST "C:\Program Files (x86)\Microsoft Visual Studio\2019\Community\VC\Auxiliary\Build\vcvars64.bat" CALL "C:\Program Files (x86)\Microsoft Visual Studio\2019\Community\VC\Auxiliary\Build\vcvars64.bat"
@IF EXIST "C:\Program Files (x86)\Microsoft Visual Studio\2019\Professional\VC\Auxiliary\Build\vcvars64.bat" CALL "C:\Program Files (x86)\Microsoft Visual Studio\2019\Professional\VC\Auxiliary\Build\vcvars64.bat"

@CD /D %~dp0

DEL ray.exe output.png output.ppm
IF NOT EXIST stb_image_write.obj (cl /nologo /c stb_image_write.cpp)
cl main.cpp stb_image_write.obj /nologo /O2 /W4 /EHsc /link /out:ray.exe
IF NOT EXIST ray.exe (
  PAUSE
  EXIT /B 1
)
ray.exe
