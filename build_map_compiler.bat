@echo off
setlocal enabledelayedexpansion

:: Config
set OUT=gmap.exe

:: Comp
set SOURCES=

for /R Tools/MapCompiler %%f in (*.cpp) do (
	set "SOURCES=!SOURCES! "%%f""
)

echo [MAP_COMPILER]: COMPILING...
g++ -O3 -DDEBUG -g -m32 -std=c++98 %SOURCES% -static-libstdc++ -Isrc -o %OUT%
echo [MAP_COMPILER]: COMPILATION COMPLETE!

endlocal