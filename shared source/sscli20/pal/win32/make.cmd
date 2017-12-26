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
setlocal
if not exist obj%BUILD_ALT_DIR% goto NoObjExist
if "%1"=="clean" rmdir /s /q obj%BUILD_ALT_DIR%
if "%1"=="-c" rmdir /s /q obj%BUILD_ALT_DIR%
:NoObjExist

if not "%MSVCDIR%" == "" set NMAKE=nmake
if     "%MSVCDIR%" == "" set NMAKE=%LKGVC_DIR%\tools\x86\vc\bin\nmake.exe

if not exist obj%BUILD_ALT_DIR% md obj%BUILD_ALT_DIR%
if not exist obj%BUILD_ALT_DIR%\rotor_x86 md obj%BUILD_ALT_DIR%\rotor_x86
if not exist %_NTTREE%\sdk\pal\lib\%_BUILDTYPE% md %_NTTREE%\sdk\pal\lib\%_BUILDTYPE%
if not exist %_NTTREE%\sdk\pal\lib\%_BUILDTYPE%\rotor_x86 md %_NTTREE%\sdk\pal\lib\%_BUILDTYPE%\rotor_x86
type default.mac >obj%BUILD_ALT_DIR%\_objects.mac
if ERRORLEVEL 1 goto ReportError
%NMAKE% ROTOR_X86=1 PASS0ONLY=1 >build%BUILD_ALT_DIR%.log 2>&1

if ERRORLEVEL 1 goto ReportError
%NMAKE% ROTOR_X86=1 MAKEDLL=1 >>build%BUILD_ALT_DIR%.log 2>&1
if ERRORLEVEL 1 goto ReportError

set MSVCRT_DLL_NAME=msvcr%ROTOR_TOOLSET_VERSION%d.dll
if "%_BUILDTYPE%"=="fre" set MSVCRT_DLL_NAME=msvcr%ROTOR_TOOLSET_VERSION%.dll
if "%_BUILDTYPE%"=="prf" set MSVCRT_DLL_NAME=msvcr%ROTOR_TOOLSET_VERSION%.dll

if not "%MSVCDIR%" == "" set MSVCRT_DLL_FULL_NAME=%windir%\system32\%MSVCRT_DLL_NAME%

if not "%ROTOR_TOOLSET_VERSION%" == "80" (
    xcopy /d /y %MSVCRT_DLL_FULL_NAME% %_NTTREE% >>build%BUILD_ALT_DIR%.log 2>&1
    if ERRORLEVEL 1 goto ReportError
    xcopy /d /y %MSVCRT_DLL_FULL_NAME% %_NTTREE%\sdk\bin\ >>build%BUILD_ALT_DIR%.log 2>&1
    if ERRORLEVEL 1 goto ReportError
)

:BuildSuccess
echo Build successful.
endlocal
goto :EOF

:ReportError
if '%BUILDALL_BUILDING%'=='' (
    echo.
    for %%i in (build%BUILD_ALT_DIR%.log) do echo Build failed.  Please see %%~fi for details.
)
exit /B 1


