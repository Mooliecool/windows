'-----------------------------------------------------------------------
'  This file is part of the Microsoft .NET SDK Code Samples.
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
Imports System
Imports System.Runtime.Remoting
Imports Microsoft.Samples.SharedInterface


Module Module1

    Sub Main()

        RemotingConfiguration.Configure("Client.exe.config")

        Dim remoteObject As ISharedInterface = CType(Activator.GetObject(GetType(ISharedInterface), "tcp://localhost:8080/server.rem"), ISharedInterface)
        Console.WriteLine(("Server responds: " + remoteObject.HelloWorld("Hi Server")))

    End Sub

End Module
