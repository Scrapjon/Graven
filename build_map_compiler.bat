@echo off
setlocal enabledelayedexpansion

:: Config
set OUT=gmap.exe

:: Comp
set SOURCES=

for /R Tools/MapCompiler %%f in (*.cpp) do (
	set "SOURCES=!SOURCES! "%%f""
)

echo Compiling...
g++ -DDEBUG -g -m32 -std=c++98 %SOURCES% -static-libstdc++ -Isrc -lmingw32 -o %OUT%

endlocal