========================================================================
    WINDOWS FORMS APPLICATION : VBCodeDOM Project Overview
========================================================================

/////////////////////////////////////////////////////////////////////////////
Usage:

The VBCodeDOM project demonstrates how to use the .NET CodeDOM mechanism to
enable dynamic souce code generation and compilation at runtime.

The ScriptControl class included in this project accepts a piece of code
written in C#/VB.NET/JScript and dynamically generates method & class wrapper
for the code. Then the dynamically generated code will be compiled into
an in-memory assembly. The compiled code then can be called via reflection.

The MainForm class in this project provides a simple UI for testing the
ScriptControl class.


/////////////////////////////////////////////////////////////////////////////
References:

Dynamic Source Code Generation and Compilation
http://msdn.microsoft.com/en-us/library/650ax5cx.aspx


/////////////////////////////////////////////////////////////////////////////