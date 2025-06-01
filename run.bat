@echo off
setlocal enabledelayedexpansion

echo ================================================
echo 🚗 RideEasy - Ride Sharing Platform
echo ================================================

:: Change to script directory
cd /d "%~dp0"
echo 📁 Working directory: %CD%

:: Check for required files
if not exist "main.cpp" (
    echo ❌ Missing main.cpp
    goto :error
)
if not exist "User.h" (
    echo ❌ Missing User.h
    goto :error
)
if not exist "RideManager.h" (
    echo ❌ Missing RideManager.h
    goto :error
)

:: Clean previous builds
echo 🧹 Cleaning previous builds...
if exist "rideeasy.exe" del "rideeasy.exe"
if exist "a.exe" del "a.exe"

:: Check for g++ compiler
where g++ >nul 2>&1
if %errorlevel% neq 0 (
    echo ❌ g++ compiler not found!
    echo Please install MinGW-w64 from: https://www.mingw-w64.org/
    goto :error
)

echo ✅ Found g++ compiler

:: Compile
echo 🔨 Compiling C++ project...
g++ -std=c++17 -Wall -Wextra -O2 -I. main.cpp -o rideeasy.exe -static-libgcc -static-libstdc++

if %errorlevel% neq 0 (
    echo ❌ Compilation failed!
    goto :error
)

echo ✅ Compilation successful!

:: Run
echo 🚀 Running rideeasy.exe...
echo --------------------------------------------------
rideeasy.exe
echo --------------------------------------------------
echo ✅ Program executed successfully!
goto :end

:error
echo ❌ Build failed!
pause
exit /b 1

:end
pause
