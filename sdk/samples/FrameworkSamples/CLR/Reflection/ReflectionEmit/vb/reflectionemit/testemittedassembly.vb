'-----------------------------------------------------------------------
'  This file is part of the Microsoft .NET Framework SDK Code Samples.
' 
'  Copyright (C) Microsoft Corporation.  All rights reserved.
' 
'This source code is intended only as a supplement to Microsoft
'Development Tools and/or on-line documentation.  See these other
'materials for detailed information regarding Microsoft code samples.
' 
'THIS CODE AND INFORMATION ARE PROVIDED AS IS WITHOUT WARRANTY OF ANY
'KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
'IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
'PARTICULAR PURPOSE.
'
'=====================================================================
'  File:      TestEmittedAssembly.vb
'
'  Summary:   Demonstrates how to use reflection emit.
'
'=====================================================================

Option Strict On
Option Explicit On


Imports System


Public Class App
    Public Shared Sub Main()
      Try
         UseHelloWorld()
      Catch e As System.TypeLoadException
         Console.WriteLine("Unable to load HelloWorld type " & _
            "from EmittedAssembly.dll!\nExecute EmitAssembly " & _
            "with option 2 from the command line to build the assembly.")
      End Try
    End Sub 'Main

    Private Shared Sub UseHelloWorld()
       Dim h As New HelloWorld("HelloWorld!")
       Console.WriteLine(h.GetGreeting())
    End Sub 'UseHelloWorld
End Class 'App