@echo off
setlocal enabledelayedexpansion

::::::::::::::::::
:: Bake Shaders ::
::::::::::::::::::
set SHADER_OUT="./ShaderBaker.exe"

set SHADER_SOURCES=Tools/ShaderBaker/ShaderBaker.cpp

set SHADER_DIR="./ShaderSources"
set BAKED_OUT="./Baked/ShaderSources.h"

:: Look into cutting down this command bc we don't need all this... - Oliver
g++ -O3 -m32 -std=c++98 %SHADER_SOURCES% -static-libstdc++ -Isrc -o %SHADER_OUT% && %SHADER_OUT% %SHADER_DIR% %BAKED_OUT%