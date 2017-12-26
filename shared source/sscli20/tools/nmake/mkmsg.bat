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

REM Compute the Path and make sure it's created
call :ComputePath %2
if not exist %RESULT% mkdir %RESULT%

echo // File %2 created by mkmsg.bat from source %3 >%2


for /f "eol=# tokens=1,2*" %%i in (%3) do if "%%i"=="<<FMSG>>" (
   echo.
) else (
    if '%1'=='-rc' (
        echo { %%j, "%%k" } , >> %2
    ) else (
        echo #define %%i %%j >>%2
    )
)
goto :Done

REM ------------------------------------------
REM Compute the path for the output file
REM ------------------------------------------
:ComputePath
set RESULT=%~dp1
goto :EOF

:Done
endlocal
