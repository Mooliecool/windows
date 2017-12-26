@echo off
IF "%OS%" == "Windows_NT" setlocal

set _BINDIR=x86

:: Re-enable if x86 version does not work on Win9x (seems to)
:: IF "%OS%" == "Windows_NT" IF /i x%PROCESSOR_ARCHITECTURE%==xALPHA set _BINDIR=Alpha
:: IF NOT "%OS%" == "Windows_NT" set _BINDIR=Win95

Echo Removing Platform SDK MSI keys (Pre-Win2K Beta3)...
call "%_BINDIR%\msizap.exe" TW! {AB2365AB-80F2-11D2-989A-00C04F7978A9}
echo.

Echo Removing Platform SDK MSI keys (Win2K Beta3)...
call "%_BINDIR%\msizap.exe" TW! {e4429075-F9CE-11D2-A0EC-009027342177}
echo.

Echo Removing Platform SDK MSI keys (Win2K Beta3 Web-Post)...
call "%_BINDIR%\msizap.exe" TW! {1FCDACA0-0A5F-11D3-97F1-0000F81F5937}
echo.

Echo Removing Platform SDK MSI keys (Win2K RC1)...
call "%_BINDIR%\msizap.exe" TW! {BD139179-168F-4021-B2FC-C7374B12B472}
echo.

Echo Removing Platform SDK MSI keys (Win2K RC2)...
call "%_BINDIR%\msizap.exe" TW! {6299BF98-F26A-43A4-AFD4-4E3B099FE5C5}
echo.

Echo Removing Platform SDK MSI keys (Win2k RC3/Win2K RTM/Jan 2000 PSDK)...
call "%_BINDIR%\msizap.exe" TW! {11B187BC-FE20-4903-9FDF-0882A471DBB9}
echo.

Echo Removing "Platform SDK" MSI keys (April 2000 PSDK)...
call "%_BINDIR%\msizap.exe" TW! {A7FD291B-AAD6-493E-9B6D-7B64AE57667E}
echo.

Echo Removing "Platform SDK" MSI keys (July 2000 PSDK)...
call "%_BINDIR%\msizap.exe" TW! {FB392771-9E4A-4DF8-9262-080BD65C0967}
echo.

Echo Removing "Platform SDK" MSI keys (Whistler Beta 1)...
call "%_BINDIR%\msizap.exe" TW! {0F0F2BEA-FAE1-4DAB-AEDF-2CADC3135BB7}
echo.

Echo Removing "Platform SDK" MSI keys (Whistler Beta 2)...
call "%_BINDIR%\msizap.exe" TW! {D9A3B8CE-033E-4CFA-A990-E142D353D177}
echo.

:: We don't need this, unless our installer was interrupted during these operations.
:: Echo Removing InProgress and Rollback MSI info
:: call "%_BINDIR%\msizap.exe" PS!
:: echo.

:: Cleanup and exit
:end
IF "%OS%" == "Windows_NT" endlocal