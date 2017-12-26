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

REM This script finds the MSVC and perl binaries, sets those related environment variables
REM and then calls env.core.pl to generate a bat script to execute to set up a build
REM environment.

if not exist .\env.core.pl (
    echo This script must be run from the root of the SSCLI directory.
    goto DisplayError
)

if not "%_NTROOT%" == "" goto CannotRunMoreThanOnce

REM Figure out the path to the current directory
call :ComputeBase %~f0
set ROTOR_DIR=%RESULT%

set _COMPILER_FLAG=
if "%1"=="mingw" goto MinGW
if "%1"=="lkgvc" goto LastKnownGood

REM Check to to see if MSVCDIR is set
IF "%MSVCDIR%"=="" for /f "delims=," %%i in ("%VS80COMNTOOLS%") do call :TRYSETMSVC "%%~i"
IF "%MSVCDIR%"=="" goto MSVCDirNotSet

REM Make sure MSVCDIR is using the short names
Call :ShortName "%MSVCDIR%"
set MSVCDIR=%RESULT%

set ROTOR_TOOLSET=MSVC

REM Detect toolset version
( for /f %%v in ('cl -nologo -EP %ROTOR_DIR%\env\bin\mscver.h') do set ROTOR_TOOLSET_VERSION=%%v ) 2>NUL:
set /A ROTOR_TOOLSET_VERSION=%ROTOR_TOOLSET_VERSION%/10-60

goto ToolsetDone

:LastKnownGood

REM Path to the checked in tools
call :SetLKGVCDir
set PATH=%LKGVC_DIR%\tools\x86;%LKGVC_DIR%\tools\perl\bin;%PATH%

set _COMPILER_FLAG=--compiler=lkgvc
shift

goto ToolsetDone

:MinGW

REM Path to the checked in tools
call :SetLKGVCDir
set PATH=%LKGVC_DIR%\tools\x86;%LKGVC_DIR%\tools\perl\bin;%PATH%

set _COMPILER_FLAG=--compiler=mingw
shift

goto ToolsetDone

:ToolsetDone

REM Try and determine if Perl is installed.
perl -v >NUL: 2>NUL:
if ERRORLEVEL 1 goto NoPerlInstalled

REM Try and setup the PERLLIB environment variable
for %%i in (perl.exe) do set PERLLIB=%%~dp$PATH:i..\lib
if "%PERLLIB%"=="" goto PerlLibDirNotSet
if "%PERLLIB%"=="..\lib" goto PerlLibDirNotSet

REM Expand the PERLLIB environment variable
Call :FullPath %PERLLIB%
set PERLLIB=%RESULT%
if "%PERLLIB%"=="" goto PerlLibDirNotSet

REM --------------------------------------------
REM Allow perl scripts to be run as commands (for vssGet vssCheckOut ...)
REM --------------------------------------------
(ASSOC .PL=Perl.Script) > NUL:
(FTYPE Perl.Script=perl.exe "%%1" %%*) > NUL:

REM Call env.core.pl to setup the cor build environment
set _OUTFILE=%TEMP%\corenv_%RANDOM%.bat
perl .\env.core.pl %_COMPILER_FLAG% --output=%_OUTFILE% %1 %2 %3 %4 %5 %6 %7 %8 %9

if ERRORLEVEL 1 goto DisplayError

call %_OUTFILE%
del %_OUTFILE%
set _OUTFILE=
set RESULT=

goto Done

REM ------------------------------------------
REM Expand a string to a full path
REM ------------------------------------------
:FullPath
set RESULT=%~f1
goto :EOF

REM ------------------------------------------
REM Compute the current directory
REM given a path to this batch script.
REM ------------------------------------------
:ComputeBase
set RESULT=%~dp1
REM Remove the trailing \
set RESULT=%RESULT:~0,-1%
Call :FullPath %RESULT%
goto :EOF

REM ------------------------------------------
REM Check if the file vsvars32.bat exists at
REM the specified path and if it does, call it.
REM The supplied path may or may not be wrapped
REM in "", so we unconditionally strip them then
REM add them under our own control.
REM ------------------------------------------
:TRYSETMSVC
IF "%~1" == "" goto :EOF
IF EXIST "%~1\vsvars32.bat" call "%~1\vsvars32.bat"
IF NOT "%MSVCDIR%"=="" goto :EOF
IF "%VSINSTALLDIR%" == "" goto :EOF
set MSVCDIR=%VSINSTALLDIR%\VC
goto :EOF

:SetLKGVCDir
REM If user has specified LKGVC_DIR, then use it. Otherwise assume LKGVC_DIR is %ROTOR_DIR%\..
REM Note, LKGVC_DIR and BASEDIR are conceptually different:
REM     BASEDIR is the root dir for ROTOR source code. (In Windows, BASEDIR=$(_NTDRIVE)$(_NTROOT) )
REM     LKGVC_DIR is the root dir for LKGVC tool set
REM , although sometimes they might contain the same value.
if "%LKGVC_DIR%"=="" set LKGVC_DIR=%ROTOR_DIR%\..
goto :EOF

REM ------------------------------------------
REM Expand a string to a full path using a Short Name
REM ------------------------------------------
:ShortName
set RESULT=%~sf1
goto :EOF

:MSVCDirNotSet
echo.
echo *** Error *** 
echo MSVCDir not defined. VC++ environment variables must be set before executing.
echo.
exit /B 1

:NoPerlInstalled
echo.
echo *** Error *** 
echo Perl does not appear to be installed. Perl.exe failed to execute.
echo Please ensure Perl is installed and can be found in the PATH.
exit /B 1

:PerlLibDirNotSet
echo.
echo *** Error *** 
echo PERLLIB not defined. Make sure you have Perl correctly installed and configured.
echo.
exit /B 1

:DisplayError
if NOT "%_OUTFILE%"=="" del %_OUTFILE%
set _OUTFILE=
set RESULT=

echo.
echo *** Error while running env.bat.
exit /B 1

:CannotRunMoreThanOnce
echo.
echo *** Error *** 
echo You cannot run env.bat more than once
echo.
exit /B 1

:Done
exit /B 0
