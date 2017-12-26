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
if "%FEATURE_PAL%"=="1" goto UseMe
if not exist %NTMAKEENV%\binplace.exe goto UseMe
call %NTMAKEENV%\binplace.exe %*
goto End

:UseMe
REM Calculate the directory and full qualifed file name from the input
REM echo %*

REM Look for -R to find destination directory
:Shift
if "%1"=="-R" goto FoundDestination
if "%1"=="" goto InvalidCommand
shift
goto shift

:FoundDestination
set DestinationDir=%2

REM Not look for the last parameter
:Shift2
if "%2"=="" goto FoundLast
shift
goto Shift2

:FoundLast
set QualifiedFileName=%1

REM Calculate the Path for the sourcefile to copy
call :ComputeFilePath  %QualifiedFileName%
set SourcePath=%RESULT%

REM Calculate the Source File (without extension)
call :ComputeFileName  %QualifiedFileName%
set SourceFile=%RESULT%

REM Calculate the File Extension
call :ComputeExtension %QualifiedFileName%
set Extension=%RESULT%

REM Copy the file
echo Copying %QualifiedFileName% to %DestinationDir%
if not exist %DestinationDir% md %DestinationDir%
copy %QualifiedFileName% %DestinationDir%
if exist %QualifiedFileName%.manifest copy %QualifiedFileName%.manifest %DestinationDir%

REM If the <file>.pdb exist, copy it over as well
if not exist %SourcePath%%SourceFile%.pdb goto PDBCopyDone
if /i "%Extension%"==".dll" set SymbolDestination=%DestinationDir%\Symbols\dll
if /i "%Extension%"==".exe" set SymbolDestination=%DestinationDir%\Symbols\Exe

if "%SymbolDestination%"=="" goto PDBCopyDone
if not exist %SymbolDestination% md %SymbolDestination%
echo Copying %SourcePath%%SourceFile%.pdb to %SymbolDestination%
copy %SourcePath%%SourceFile%.pdb %SymbolDestination%
:PDBCopyDone

REM We're done!
Goto End

REM Various useful expansion codes

REM ------------------------------------------
REM Expand a string to a full path
REM ------------------------------------------
:FullPath
set RESULT=%~f1
goto :EOF

REM ------------------------------------------
REM Compute the Path for the given input
REM ------------------------------------------
:ComputeFilePath
set RESULT=%~p1
Call :FullPath %RESULT%
goto :EOF

REM ------------------------------------------
REM Compute the filename for the given input
REM ------------------------------------------
:ComputeFileName
set RESULT=%~n1
goto :EOF

REM ------------------------------------------
REM Compute the Path for the given input
REM ------------------------------------------
:ComputeExtension
set RESULT=%~x1
goto :EOF

REM ------------------------------------------
REM Invalid command
REM ------------------------------------------
:InvalidCommand
echo Invalid command
goto :EOF

:End
endlocal
