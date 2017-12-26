@ECHO OFF

cd ..

REM Set OS specific variables
IF "x%OS%x" == "xWindows_NTx" Goto Launch_WinNT

:Launch_Win9x
start command.com /k setenv.bat %1 %2 %3
exit

:Launch_WinNT
start cmd.exe /k setenv.bat %1 %2 %3
exit
