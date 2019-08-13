@echo off
title Setup LXMax VisualStudio build
set sourcedir=%cd%

if not exist "build" (
	mkdir "build"
)
echo -----------------------------------------
echo Installing conan dependencies...
echo -----------------------------------------
cd "build"
conan install "%sourcedir%" -g cmake_multi -s build_type=Debug -s compiler="Visual Studio" -s compiler.runtime=MTd --build=missing
conan install "%sourcedir%" -g cmake_multi -s build_type=Release -s compiler="Visual Studio" -s compiler.runtime=MT --build=missing

echo.
echo.
echo -----------------------------------------
echo Running CMake...
echo -----------------------------------------
cmake "%sourcedir%" -G "Visual Studio 16"

echo.
echo.
echo -----------------------------------------
echo Build ready!
echo -----------------------------------------
pause

rem Return to the original directory
cd "%sourcedir%"