@echo off

set opts=-FC -GR- -EHa- -nologo -Zi
set code=%cd%
pushd ..\build
cl %opts% %code%\x64 -Fesparrow.exe
popd
