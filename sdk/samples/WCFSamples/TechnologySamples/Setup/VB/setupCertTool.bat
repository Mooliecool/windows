@ECHO OFF
REM Build the tool
ECHO Building FindPrivateKey
msbuild /v:m FindPrivateKey\FindPrivateKey.csproj
ECHO Copying FindPrivateKey to %ProgramFiles%\ServiceModelSampleTools
mkdir "%ProgramFiles%\ServiceModelSampleTools"
copy FindPrivateKey\bin\FindPrivateKey.exe "%ProgramFiles%\ServiceModelSampleTools"
ECHO Finished.  Run CleanupCertTool.bat to remove this tool.
