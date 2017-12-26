@if "%_echo%"=="" echo off
REM ==++==
REM 
REM 
REM  Copyright (c) 2006 Microsoft Corporation.  All rights reserved.
REM 
REM  The use and distribution terms for this software are contained in the file
REM  named license.txt, which can be found in the root of this distribution.
REM  By using this software in any fashion, you are agreeing to be bound by the
REM  terms of this license.
REM 
REM  You must not remove this notice, or any other, from this software.
REM 
REM 
REM ==--==
if not exist obj%BUILD_ALT_DIR% goto NoObjDir
if "%1"=="clean" rmdir /s /q obj%BUILD_ALT_DIR%
if "%1"=="-c" rmdir /s /q obj%BUILD_ALT_DIR%
:NoObjDir
if not exist obj%BUILD_ALT_DIR% md obj%BUILD_ALT_DIR%
if not exist obj%BUILD_ALT_DIR%\rotor_x86 md obj%BUILD_ALT_DIR%\rotor_x86
type default.mac >obj%BUILD_ALT_DIR%\_objects.mac
nmake ROTOR_X86=1 -f makefile.win >build%BUILD_ALT_DIR%.log 2>&1
if ERRORLEVEL 1 goto :ReportError
echo Build successful.
goto :EOF

:ReportError
if '%BUILDALL_BUILDING%'=='' (
    echo.
    for %%i in (build%BUILD_ALT_DIR%.log) do echo Build failed.  Please see %%~fi for details.
)
exit /B 1

