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
Imports System.Reflection
Imports Microsoft.Samples.SharedInterface


Public Class HelloWorld
    Inherits MarshalByRefObject
    Implements IHelloWorld

    Public Function Echo(ByVal input As String) As String Implements IHelloWorld.Echo
        Dim originalColor As ConsoleColor = Console.BackgroundColor
        Console.BackgroundColor = ConsoleColor.Blue
        Dim currentAppDomain As String = AppDomain.CurrentDomain.FriendlyName
        Console.WriteLine("AppDomain: {0}", currentAppDomain)
        Console.WriteLine("Echo Input: ", input)
        Console.WriteLine()
        Console.WriteLine("Non-GAC assemblies loaded in {0} appDomain:", AppDomain.CurrentDomain.FriendlyName)
        Dim assembly As Assembly

        For Each assembly In AppDomain.CurrentDomain.GetAssemblies()
            If Not assembly.GlobalAssemblyCache Then
                Console.WriteLine(vbTab + assembly.GetName().Name)
            End If
        Next assembly

        Console.BackgroundColor = originalColor

        Return input + " from AppDomain " + currentAppDomain

    End Function



End Class
