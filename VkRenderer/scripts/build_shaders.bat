cd %~dp0..\shaders
echo %cd%
for %%i in (*.vert *.frag) do ( glslc.exe %%i -o %%i.spv )
REM for %%i in (..\shaders\*.vert ..\shaders\*.frag) do ( glslc.exe %%i -o ..\shaders\%%i.spv )
REM pause
REM for /F %%i %cd% in (*.vert *.tesc *.tese *.geom *.frag *.comp) do 
REM (
	REM echo %%i
REM )
::%VK_SDK_PATH%\Bin\glslc.exe -o %%f.spv