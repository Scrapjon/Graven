@echo off
setlocal enabledelayedexpansion

:: Bake shaders
call ./bake_shaders.bat


::::::::::::::::::
:: Compile Game ::
::::::::::::::::::


:: Config
set OUT=Graven.exe

:: Comp
set SOURCES=main.cpp

for /R src %%f in (*.cpp) do (
	set "SOURCES=!SOURCES! "%%f""
)

echo Compiling...
g++ -O3 -m32 -std=c++98 %SOURCES% -static-libstdc++ -Isrc -Ivendor/SDL/include -Lvendor/SDL/lib -lopengl32 -lSDLmain -lSDL -o %OUT%

endlocal