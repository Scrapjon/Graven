@echo off
setlocal enabledelayedexpansion

:: Config
set OUT=Graven.exe

:: Comp
set SOURCES=main.cpp

for /R src %%f in (*.cpp) do (
	set "SOURCES=!SOURCES! "%%f""
)

echo Compiling...
g++ -m32 -std=c++98 %SOURCES% -static-libstdc++ -Isrc -Ivendor/SDL/include -Lvendor/SDL/lib -lmingw32 -lopengl32 -lSDLmain -lSDL -o %OUT%

endlocal