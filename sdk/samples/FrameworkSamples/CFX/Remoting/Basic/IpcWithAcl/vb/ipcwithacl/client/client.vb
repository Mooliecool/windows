 '---------------------------------------------------------------------
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
'---------------------------------------------------------------------
Imports System
Imports System.Runtime.Remoting
Imports Microsoft.Samples.SharedInterface


'/ <summary>
'/ This is a sample IPC client.
'/ </summary>

Public Class ClientApp

    '/ <summary>
    '/ The main entry point for the application.
    '/ </summary>
    Public Shared Sub Main()
        Try
            Dim remoteObject As ISharedInterface = CType(Activator.GetObject(GetType(ISharedInterface), "ipc://test/server.rem"), ISharedInterface)
            Console.WriteLine("Server responds: " + remoteObject.HelloWorld("Hi Server"))
        Catch e As RemotingException
            Console.WriteLine("An exception has occured: " + e.Message)
            Console.WriteLine("Perhaps you do not have access to the IPC channel.")
            Console.WriteLine("Stack trace:" + vbLf + e.StackTrace)
        End Try

    End Sub 'Main 
End Class 'ClientApp