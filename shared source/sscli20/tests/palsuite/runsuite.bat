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
if '%CORBASE%'=='' (
  echo Error!  This script must be run from within a Rotor build window
  goto :EOF
)
setlocal
set LIB=%CORENV%\sdk\lib\i386;%TARGETCOMPLUSSDK%\pal\lib
set INCLUDE=%CORENV%\crt\inc\i386;d:\env.i386\sdk\inc
set PAL_DISABLE_MESSAGEBOX=1
set ROTOR_DIR=%CORBASE%\..
set PLATFORM=WIN32
set TH_RESULTS=%ROTOR_DIR%\test_results.log
set TH_CONFIG=%ROTOR_DIR%\tests\palsuite\testconfig.dat
set TH_DIR=%ROTOR_DIR%\tests\palsuite
set TH_RESULTS=%ROTOR_DIR%\pal_test_results.log
set TH_XRUN=%ROTOR_DIR%\tests\harness\xrun\xrun
set PATH=%PATH%;%ROTOR_DIR%\tests\tools

echo Building the test harnesss...
cd /d %ROTOR_DIR%\tests\harness\test_harness
make
if errorlevel 1 (
  echo Error building the test harness driver
  goto :EOF
)

echo Building xrun...
cd /d %ROTOR_DIR%\tests\harness\xrun
make
if errorlevel 1 (
  echo Error building xrun
  goto :EOF
)

echo Running the suites...
cd /d %ROTOR_DIR%\tests\palsuite
%ROTOR_DIR%\tests\harness\test_harness\testharness

