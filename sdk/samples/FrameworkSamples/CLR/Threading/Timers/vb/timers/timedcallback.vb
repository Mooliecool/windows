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
'-----------------------------------------------------------------------
'  File:      TimedCallback.vb
'
'  Summary:   Demonstrates the use of the Timer class to generate a 
'             periodic callback to a method.
'---------------------------------------------------------------------

Option Explicit On 
Option Strict On


Imports System
Imports System.Threading


Class App
    
    Public Shared Sub Main()
        Console.WriteLine("Checking for status updates every 2 seconds.")
        Console.WriteLine("   (Hit Enter to terminate the sample)")
        Dim myTimer As New Timer(New TimerCallback(AddressOf CheckStatus), Nothing, 0, 2000)
        Console.ReadLine()
	myTimer.Dispose()
    End Sub 'Main
    
    
    
    ' The callback method's signature MUST match that of a System.Threading.TimerCallback 
    ' delegate (it takes an Object parameter and returns void)
    Shared Sub CheckStatus(state As Object)
        Console.WriteLine("Checking Status.")
    End Sub 'CheckStatus ' ...
End Class 'App
