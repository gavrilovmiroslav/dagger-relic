cd ..\..

@if not defined _echo echo off
for /f "usebackq delims=" %%i in (`Tools\VSWhere\vswhere.exe -prerelease -latest -property installationPath`) do (
  if exist "%%i\Common7\Tools\vsdevcmd.bat" (
    set VSCMD_DEBUG=1
    "%%i\Common7\Tools\vsdevcmd.bat"
    tools\Premake\premake-5.0.0-beta2-windows\premake5.exe vs2022

    msbuild Dagger.sln
    copy /Y Libs\SDL2\Binaries\Debug-windows-x86_64\SDL2.dll bin\Debug\

    exit
  )
)

rem Instance or command prompt not found
exit /b 2