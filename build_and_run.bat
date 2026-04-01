

@echo off
REM Build and run both SenderApp and ReceiverApp

set "CMAKE_PREFIX_PATH=C:\Qt\6.10.2\mingw_64\lib\cmake"

REM Set build directory
set BUILD_DIR=build

REM Create build directory if not exists
if not exist %BUILD_DIR% mkdir %BUILD_DIR%
cd %BUILD_DIR%

REM Run CMake configure and build
cmake ..
if errorlevel 1 goto error
cmake --build .
if errorlevel 1 goto error


REM Deploy Qt DLLs and QML for SenderApp
"C:\Qt\6.10.2\mingw_64\bin\windeployqt.exe" SenderApp\SenderApp.exe

REM Deploy Qt DLLs and QML for ReceiverApp
"C:\Qt\6.10.2\mingw_64\bin\windeployqt.exe" ReceiverApp\ReceiverApp.exe

REM Deploy Qt DLLs and QML for ReceiverApp_2
"C:\Qt\6.10.2\mingw_64\bin\windeployqt.exe" ReceiverApp_2\ReceiverApp_2.exe

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
