@echo off
setlocal

echo ========================================
echo Solution Generator
echo ========================================
echo.

REM Check for Git
where git >nul 2>&1
if errorlevel 1 (
    echo [ERROR] Git is not installed!
    pause
    exit /b 1
)
echo [OK] Git

REM Check for vcpkg
if not exist "%VCPKG_ROOT%\vcpkg.exe" (
    echo [ERROR] vcpkg not found at %%VCPKG_ROOT%%!
    echo Please install vcpkg and set VCPKG_ROOT.
    pause
    exit /b 1
)
echo [OK] vcpkg at: %VCPKG_ROOT%

REM Check for vcpkg.json
if not exist "vcpkg.json" (
    echo [ERROR] vcpkg.json not found!
    pause
    exit /b 1
)
echo [OK] vcpkg.json found

REM Check if dependencies are missing
"%VCPKG_ROOT%\vcpkg.exe" install --dry-run --triplet=x64-windows >nul 2>&1
if errorlevel 1 (
    echo [INFO] Some dependencies are missing. Installing...
    "%VCPKG_ROOT%\vcpkg.exe" install --triplet=x64-windows
    if errorlevel 1 (
        echo [ERROR] Failed to install dependencies!
        pause
        exit /b 1
    )
    echo [OK] Dependencies installed
) else (
    echo [OK] All dependencies already installed, skipping...
)

REM Check for CMake
where cmake >nul 2>&1
if errorlevel 1 (
    echo [ERROR] CMake not found!
    pause
    exit /b 1
)
echo [OK] CMake

REM Create build directory if needed
if not exist "generated-vs" (
    mkdir generated-vs
)

REM Change to build directory
cd generated-vs

REM Generate Visual Studio solution
cmake .. -DCMAKE_TOOLCHAIN_FILE="%VCPKG_ROOT%/scripts/buildsystems/vcpkg.cmake" -G "Visual Studio 17 2022" -A x64
if errorlevel 1 (
    echo [ERROR] CMake configuration failed!
    pause
    exit /b 1
)
echo [SUCCESS] Solution generated successfully!

cd ..
endlocal
pause 