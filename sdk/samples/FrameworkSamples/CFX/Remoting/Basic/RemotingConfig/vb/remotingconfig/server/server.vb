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
Imports System.Collections
Imports System.Runtime.Remoting
Imports Microsoft.Samples.SharedImplementation


'/ <summary>
'/ Summary description for Class1.
'/ </summary>

Public Class Class1

    '/ <summary>
    '/ The main entry point for the application.
    '/ </summary>
    Shared Sub Main()
        RemotingConfiguration.Configure("Server.exe.config", true)

        Console.WriteLine("Ready...")
        Console.WriteLine("Press enter to exit.")
        Console.ReadLine()

    End Sub 'Main
End Class 'Class1