call  "C:\Program Files\Microsoft Visual Studio\2022\Community\VC\Auxiliary\Build\vcvars32.bat"
pushd .
cd    /d %~dp0
cd    dagger-relic
meson compile -C builddir
if    errorlevel 1 pause
move  builddir\dagger.exe dagger.exe
popd
pause
