csc /t:module Math.cs
csc /addmodule:Math.netmodule /t:module MathClient.cs
al MathClient.netmodule Math.netmodule /main:MathClient.Main /out:MathTest.exe /t:exe
