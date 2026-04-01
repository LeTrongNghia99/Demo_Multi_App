@echo off
REM Build and run both SenderApp and ReceiverApp

REM Set Qt installation path
set "QT_PATH=E:\Qt\6.11.0\mingw_64"

REM Set CMake prefix path
set "CMAKE_PREFIX_PATH=%QT_PATH%\lib\cmake"

REM Set build directory
set BUILD_DIR=build

REM Create build directory if not exists
if not exist %BUILD_DIR% mkdir %BUILD_DIR%

cd %BUILD_DIR%

if exist CMakeCache.txt del /f /q CMakeCache.txt
if exist CMakeFiles rmdir /s /q CMakeFiles

REM Run CMake configure and build
cmake .. -G "MinGW Makefiles"
if errorlevel 1 goto error
cmake --build .
if errorlevel 1 goto error

if not exist ReceiverApp_2\ReceiverApp_2.exe (
    echo ReceiverApp_2.exe not found!
    goto error
)

REM Deploy Qt DLLs and QML for SenderApp
"%QT_PATH%\bin\windeployqt.exe" SenderApp\SenderApp.exe

REM Deploy Qt DLLs and QML for ReceiverApp
"%QT_PATH%\bin\windeployqt.exe" ReceiverApp\ReceiverApp.exe

REM Deploy Qt DLLs and QML for ReceiverApp_2
"%QT_PATH%\bin\windeployqt.exe" ReceiverApp_2\ReceiverApp_2.exe

REM Run both apps (in background)

REM Run SenderApp in new console window
start "SenderApp" cmd /k SenderApp\SenderApp.exe

REM Run ReceiverApp in new console window
start "ReceiverApp" cmd /k ReceiverApp\ReceiverApp.exe

REM Run ReceiverApp_2 in new console window
start "ReceiverApp_2" cmd /k ReceiverApp_2\ReceiverApp_2.exe

goto end

:error
echo Build failed!
pause

:end
cd ..
