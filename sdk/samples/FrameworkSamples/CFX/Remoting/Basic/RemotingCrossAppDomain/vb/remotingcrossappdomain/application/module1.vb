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
Imports System.Reflection
Imports Microsoft.Samples.SharedInterface

Module Module1

    Sub Main()
        ' Uncomment the following line to set My.User to the currently logged on Windows user.
        ' My.User.InitializeWithWindowsUser

        'Create new appDomain
        Dim domain As AppDomain = AppDomain.CreateDomain("NewAppDomain")

        'Create remote object in new appDomain via shared interface
        'to avoid loading the implementation library into this appDomain
        Dim proxy As IHelloWorld = _
                domain.CreateInstanceAndUnwrap( _
                    "ImplementationLibrary", _
                    "Microsoft.Samples.ImplementationLibrary.HelloWorld")

        'Output results of the call
        Console.WriteLine(vbCrLf + "Return:" + vbCrLf + vbTab + proxy.Echo("Hello"))
        Console.WriteLine()

        Console.WriteLine("Non-GAC assembliesloaded in {0} appDomain:", AppDomain.CurrentDomain.FriendlyName)

        Dim assembly As Assembly

        For Each assembly In AppDomain.CurrentDomain.GetAssemblies()
            If Not assembly.GlobalAssemblyCache Then
                Console.WriteLine(vbTab + assembly.GetName().Name)
            End If
        Next assembly

        Console.WriteLine(vbCrLf + "ImplementationLibrary should not be loaded.")

        Console.ReadLine()
    End Sub

End Module
