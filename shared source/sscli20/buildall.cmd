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

if "%_NTTREE%" == "" goto NtTreeNotSet

REM        

REM Note:
REM The BUILDALL_BUILDING, BUILDALL_ERRORLOG and BUILDALL_WARNINGLOG variables are
REM used by other scripts to report errors so setlocal/endlocal may not be used.
REM

if not exist "%USERPROFILE%\rotor" goto NoRotorUserProfile
if "%1"=="clean" rmdir /s /q "%USERPROFILE%\rotor"
if "%1"=="-c" rmdir /s /q "%USERPROFILE%\rotor"
:NoRotorUserProfile

if not exist %_NTTREE% goto NoNtTree
if "%1"=="clean" (
    rmdir /s /q "%_NTTREE%" 
    REM        
)

if "%1"=="-c" (
    rmdir /s /q "%_NTTREE%"
REM        
)
:NoNtTree

if not exist %_NTTREE% mkdir %_NTTREE%
if not exist %_NTTREE%\sdk\pal\lib\%_BUILDTYPE%\%_BUILDARCH% mkdir %_NTTREE%\sdk\pal\lib\%_BUILDTYPE%\%_BUILDARCH%
if not exist %NDPDIR%\clr\lib\%_BUILDTYPE%\%_BUILDARCH% mkdir %NDPDIR%\clr\lib\%_BUILDTYPE%\%_BUILDARCH%

REM        

echo.
echo --- Copy prebuilt files ---
echo.
if not exist %ROTOR_DIR%\prebuilt\idl\obj%BUILD_ALT_DIR% mkdir %ROTOR_DIR%\prebuilt\idl\obj%BUILD_ALT_DIR%
if not exist %ROTOR_DIR%\prebuilt\idl\obj%BUILD_ALT_DIR%\%_BUILDARCH% mkdir %ROTOR_DIR%\prebuilt\idl\obj%BUILD_ALT_DIR%\%_BUILDARCH%
copy %ROTOR_DIR%\prebuilt\idl\*.h %ROTOR_DIR%\prebuilt\idl\obj%BUILD_ALT_DIR%\%_BUILDARCH%\
copy %ROTOR_DIR%\prebuilt\idl\*_i.c %ROTOR_DIR%\prebuilt\idl\obj%BUILD_ALT_DIR%\%_BUILDARCH%\

if not exist %ROTOR_DIR%\prebuilt\yacc\obj%BUILD_ALT_DIR% mkdir %ROTOR_DIR%\prebuilt\yacc\obj%BUILD_ALT_DIR%
if not exist %ROTOR_DIR%\prebuilt\yacc\obj%BUILD_ALT_DIR%\%_BUILDARCH% mkdir %ROTOR_DIR%\prebuilt\yacc\obj%BUILD_ALT_DIR%\%_BUILDARCH%
copy %ROTOR_DIR%\prebuilt\yacc\*.c %ROTOR_DIR%\prebuilt\yacc\obj%BUILD_ALT_DIR%\%_BUILDARCH%\

REM        

if not "%ROTOR_TOOLSET%" == "MSVC" goto UsePrebuiltBootstrap

echo.
echo --- Building the PAL ---
echo.
set BUILDALL_BUILDING=%ROTOR_DIR%\pal\win32
set BUILDALL_ERRORLOG=%BUILDALL_BUILDING%\build%BUILD_ALT_DIR%.log

if "%_BUILDTYPE%"=="prf"  perl %ROTOR_DIR%\prepare.pl 

pushd %ROTOR_DIR%\pal\win32
call make.cmd %*
popd
if ERRORLEVEL 1 goto DisplayError

echo.
echo --- Building the binplace tool ---
echo.
set BUILDALL_BUILDING=%ROTOR_DIR%\tools\binplace
set BUILDALL_ERRORLOG=%BUILDALL_BUILDING%\build%BUILD_ALT_DIR%.log
pushd %ROTOR_DIR%\tools\binplace
call make.cmd %*
popd
if ERRORLEVEL 1 goto DisplayError

echo.
echo --- Building the build tool ---
echo.
set BUILDALL_BUILDING=%ROTOR_DIR%\tools\build
set BUILDALL_ERRORLOG=%BUILDALL_BUILDING%\build%BUILD_ALT_DIR%.log
pushd %ROTOR_DIR%\tools\build
call make.cmd %*
popd
if ERRORLEVEL 1 goto DisplayError

echo.
echo --- Building the cppmunge tool ---
echo.
set BUILDALL_BUILDING=%ROTOR_DIR%\tools\cppmunge
set BUILDALL_ERRORLOG=%BUILDALL_BUILDING%\build%BUILD_ALT_DIR%.log
pushd %ROTOR_DIR%\tools\cppmunge
call make.cmd %*
popd
if ERRORLEVEL 1 goto DisplayError

goto :BootstrapDone

:UsePrebuiltBootstrap

echo.
echo --- Copying prebuilt bootstrap ---
echo.

@if "%_echo%"=="" echo on

copy %ROTOR_DIR%\prebuilt\tools\%_BUILDTYPE%\*.* %_NTTREE%

del %_NTTREE%\sdk\pal\lib\%_BUILDTYPE%\%_BUILDARCH%\rotor_pal.*
gcc -E -nostdinc -w -x assembler-with-cpp -P %C_DEFINES% %ROTOR_DIR%\pal\win32\rotor_pal.src >%_NTTREE%\sdk\pal\lib\%_BUILDTYPE%\%_BUILDARCH%\rotor_pal.def
dlltool -k --input-def %_NTTREE%\sdk\pal\lib\%_BUILDTYPE%\%_BUILDARCH%\rotor_pal.def -l %_NTTREE%\sdk\pal\lib\%_BUILDTYPE%\%_BUILDARCH%\rotor_pal.lib

del %_NTTREE%\sdk\pal\lib\%_BUILDTYPE%\%_BUILDARCH%\rotor_palrt.*
gcc -E -nostdinc -w -x assembler-with-cpp -P %C_DEFINES% %ROTOR_DIR%\palrt\src\rotor_palrt.src >%_NTTREE%\sdk\pal\lib\%_BUILDTYPE%\%_BUILDARCH%\rotor_palrt.def
dlltool -k --input-def %_NTTREE%\sdk\pal\lib\%_BUILDTYPE%\%_BUILDARCH%\rotor_palrt.def -l %_NTTREE%\sdk\pal\lib\%_BUILDTYPE%\%_BUILDARCH%\rotor_palrt.lib

del %_NTTREE%\sdk\pal\lib\%_BUILDTYPE%\%_BUILDARCH%\sscoree.*
gcc -E -nostdinc -w -x assembler-with-cpp -P %C_DEFINES% %ROTOR_DIR%\palrt\sscoree\sscoree.src >%_NTTREE%\sdk\pal\lib\%_BUILDTYPE%\%_BUILDARCH%\sscoree.def
dlltool -k --input-def %_NTTREE%\sdk\pal\lib\%_BUILDTYPE%\%_BUILDARCH%\sscoree.def -l %_NTTREE%\sdk\pal\lib\%_BUILDTYPE%\%_BUILDARCH%\sscoree.lib

del %_NTTREE%\sdk\pal\lib\%_BUILDTYPE%\%_BUILDARCH%\rotor_debug.*
dlltool -k --input-def %ROTOR_DIR%\palrt\debug\rotor_debug.def -l %_NTTREE%\sdk\pal\lib\%_BUILDTYPE%\%_BUILDARCH%\rotor_debug.lib
@if "%_echo%"=="" echo off

goto :BootstrapDone

:BootstrapDone

set BUILDALL_OPTIONS=%*
set BUILDALL_SETUP_SCRIPT=%ROTOR_DIR%\env\bin\setup.pl
set BUILDALL_SETUP_LOG=%_NTTREE%\setup.log

call :RunBuild %ROTOR_DIR%\tools\resourcecompiler "Resource Compiler"
if ERRORLEVEL 1 goto DisplayError
call :RunBuild %ROTOR_DIR%\palrt "PAL RT"
if ERRORLEVEL 1 goto DisplayError

if exist %ROTOR_DIR%\palrt\unilib\sources call :RunBuild %ROTOR_DIR%\palrt\unilib "UniLib"
if ERRORLEVEL 1 goto DisplayError

call :RunBuild %BASEDIR%\csharp "C# compiler"
if ERRORLEVEL 1 goto DisplayError

call :RunBuild %NDPDIR%\clr\src "CLR"
if ERRORLEVEL 1 goto DisplayError

if not "%ROTOR_TOOLSET%" == "MSVC" goto Done

set BUILD_OPTIONS=~misc
call :RunBuild %NDPDIR%\fx\src "FX - pass1 only" -L
if ERRORLEVEL 1 goto DisplayError
REM        
set BUILD_OPTIONS=
call :RunBuild %NDPDIR%\clr\src\managedlibraries "Remoting - pass1 only" -L
if ERRORLEVEL 1 goto DisplayError
call :RunBuild %VSADIR% "JScript - pass1 only" -L
if ERRORLEVEL 1 goto DisplayError

call :RunBuild %NDPDIR%\fx\src\sys "FX - System.dll"
if ERRORLEVEL 1 goto DisplayError
call :RunSetup "System.dll" GAC System.dll
if ERRORLEVEL 1 goto DisplayError

set BUILD_OPTIONS=~sys ~misc
call :RunBuild %NDPDIR%\fx\src "FX"
if ERRORLEVEL 1 goto DisplayError
call :RunSetup "System.Configuration.dll" GAC System.Configuration.dll
if ERRORLEVEL 1 goto DisplayError
REM        


set BUILD_OPTIONS=

REM        

call :RunSetup "System.Configuration.dll" GAC System.Configuration.dll
if ERRORLEVEL 1 goto DisplayError
call :RunSetup "System.Xml.dll" GAC System.Xml.dll
if ERRORLEVEL 1 goto DisplayError
call :RunSetup "System.Data.SqlXml.dll" GAC System.Data.SqlXml.dll
if ERRORLEVEL 1 goto DisplayError

call :RunBuild %NDPDIR%\fx\src\misc "FX - Misc"
if ERRORLEVEL 1 goto DisplayError

call :RunBuild %NDPDIR%\clr\src\managedlibraries "Remoting"
if ERRORLEVEL 1 goto DisplayError
call :RunBuild %VSADIR% "JScript"
if ERRORLEVEL 1 goto DisplayError

call :RunBuild %NDPDIR%\clr\src\toolbox\sos\dactablegen "dactablegen"
if ERRORLEVEL 1 goto DisplayError
call :RunBuild %NDPDIR%\clr\src\dacupdatedll "dacupdatedll"
if ERRORLEVEL 1 goto DisplayError
call :RunBuild %NDPDIR%\clr\src\toolbox\sos\strike "strike dll"
if ERRORLEVEL 1 goto DisplayError

REM        







call :RunSetup "everything"
if ERRORLEVEL 1 goto DisplayError

call :RunBuild %ROTOR_DIR%\samples "samples"
if ERRORLEVEL 1 goto DisplayError

call :RunBuild %ROTOR_DIR%\tools\ildbconv "pdb to ildb Tool"
if ERRORLEVEL 1 goto DisplayError

goto Done

:DisplayError
echo *** Error while building %BUILDALL_BUILDING%
echo     Open %BUILDALL_ERRORLOG% to see the error log.
echo     Open %BUILDALL_WARNINGLOG% to see any warnings.
exit /B 1


:NtTreeNotSet
echo ERROR: The build environment variable isn't set.
echo Please run env.bat [checked^|free^|debug]
exit /B 1

:Done
set BUILDALL_BUILDING=
set BUILDALL_ERRORLOG=
set BUILDALL_WARNINGLOG=
exit /B 0


:RunBuild

set BUILDALL_BUILDING=%1

set BUILDALL_TITLE=%2
set BUILDALL_TITLE=%BUILDALL_TITLE:"=%

echo.
echo --- Building %BUILDALL_TITLE% ---
echo.

set BUILDALL_ERRORLOG=%BUILDALL_BUILDING%\build%BUILD_ALT_DIR%.err
set BUILDALL_WARNINGLOG=%BUILDALL_BUILDING%\build%BUILD_ALT_DIR%.wrn

pushd %BUILDALL_BUILDING%
set BUILDALL_FAILED=0
if "%VTTY%" == "1" set "BUILDALL_OPTIONS=%BUILDALL_OPTIONS% -vtty"
build %3 %4 %5 %6 %BUILDALL_OPTIONS%
if ERRORLEVEL 1 set BUILDALL_FAILED=1
popd

exit /B %BUILDALL_FAILED%


:RunSetup

set BUILDALL_TITLE=%1
set BUILDALL_TITLE=%BUILDALL_TITLE:"=%

echo.
echo --- Setting up %BUILDALL_TITLE% ---
echo.

set BUILDALL_BUILDING=%BUILDALL_SETUP_SCRIPT%
set BUILDALL_ERRORLOG=%BUILDALL_SETUP_LOG%
set BUILDALL_WARNINGLOG=

set BUILDALL_FAILED=0
perl %BUILDALL_BUILDING% %2 %3 %4 %5 >>%BUILDALL_ERRORLOG%
if ERRORLEVEL 1 set BUILDALL_FAILED=1

exit /B %BUILDALL_FAILED%
