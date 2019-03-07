@SETLOCAL
@IF EXIST "C:\Program Files (x86)\Microsoft Visual Studio\2017\Community\VC\Auxiliary\Build\vcvars64.bat" CALL "C:\Program Files (x86)\Microsoft Visual Studio\2017\Community\VC\Auxiliary\Build\vcvars64.bat"
@IF EXIST "C:\Program Files (x86)\Microsoft Visual Studio\2017\Professional\VC\Auxiliary\Build\vcvars64.bat" CALL "C:\Program Files (x86)\Microsoft Visual Studio\2017\Professional\VC\Auxiliary\Build\vcvars64.bat"

@CD /D %~dp0

DEL ray.exe output.png output.ppm
IF NOT EXIST stb_image_write.obj (cl /nologo /c stb_image_write.cpp)
cl main.cpp /nologo /O2 /W4 /EHsc /link stb_image_write.obj /out:ray.exe
IF NOT EXIST ray.exe (
  PAUSE
  EXIT /B 1
)
ray.exe
