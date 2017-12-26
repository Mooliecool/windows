echo off
cd "V1 Application\TestV1Part1\bin"
TestV1Part1.exe
Pause
cd "..\..\..\V2 Application\TestV2\bin"
TestV2.exe
Pause
cd "..\..\..\V1 Application\TestV1Part2\bin"
TestV1Part2.exe
Pause
cd ..\..\..
echo on
