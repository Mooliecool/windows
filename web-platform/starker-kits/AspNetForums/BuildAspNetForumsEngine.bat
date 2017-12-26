@echo off
echo ------------------------------------------------------------------------
echo Building AspNetForums engine
echo ------------------------------------------------------------------------
csc /t:library /out:AspNetForums\bin\AspNetForums.dll /recurse:Engine\*.cs
move *.dll AspNetForums\bin\
