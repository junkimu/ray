@SETLOCAL
@CALL "C:\Program Files (x86)\Microsoft Visual Studio\2017\Community\VC\Auxiliary\Build\vcvars64.bat"

@CD /D %~dp0

DEL ray.exe
cl main.cpp stb_image_write.cpp /W4 /EHsc /link /out:ray.exe
IF NOT EXIST ray.exe (
  PAUSE
  EXIT /B 1
)
ray.exe > output.ppm
