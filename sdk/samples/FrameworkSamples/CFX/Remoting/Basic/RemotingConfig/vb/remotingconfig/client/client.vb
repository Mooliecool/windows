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
Imports Microsoft.Samples.SharedImplementation


'/ <summary>
'/ This is a sample IPC client.
'/ </summary>

Public Class ClientApp

    '/ <summary>
    '/ The main entry point for the application.
    '/ </summary>
    Public Shared Sub Main()
        RemotingConfiguration.Configure("Client.exe.config", true)

        Dim proxy As HelloWorld = New HelloWorld()
        Console.WriteLine("Server responds: {0}", proxy.Echo("Hello"))
    End Sub 'Main 
End Class 'ClientApp