'---------------------------------------------------------------------
'  This file is part of the Microsoft .NET Framework SDK Code Samples.
' 
'  Copyright (C) Microsoft Corporation.  All rights reserved.
' 
'  This source code is intended only as a supplement to Microsoft
'  Development Tools and/or on-line documentation.  See these other
'  materials for detailed information regarding Microsoft code samples.
' 
'  THIS CODE AND INFORMATION ARE PROVIDED AS IS WITHOUT WARRANTY OF ANY
'  KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
'  IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
'  PARTICULAR PURPOSE.
'---------------------------------------------------------------------
Imports System
Imports System.IO
Imports System.CodeDom
Imports System.CodeDom.Compiler
Imports Microsoft.CSharp
Imports Microsoft.VisualBasic
Imports System.Security

' <summary>
' Generate generic code using CodeDOM API
' </summary>

Class GenerateGenericList

    ' <summary>
    ' The main entry point for the application.
    ' </summary>
    <STAThread()> _
    Public Shared Sub Main(ByVal args() As String)

        'Static variables
        Dim provider As CodeDomProvider = Nothing
        'Target location of generated files
        Dim fullpath As String = String.Empty

        'Check if number of arguments are right
        If args.Length < 1 OrElse args.Length > 2 Then
            WriteUsage()
            Return
        End If
        'Check for language
        If args(0).ToUpperInvariant() = "VB" Then
            'Instantiate a Visual Basic Code Provider
            provider = New VBCodeProvider()
        ElseIf args(0).ToUpperInvariant() = "C#" Then
            'Instantiate a C# Code Provider
            provider = New CSharpCodeProvider()
        Else
            WriteUsage()
            Return
        End If

        'Extract path from command line arguments
        If args.Length > 1 Then
            Try
                fullpath = Path.GetFullPath(args(1))
                If Not Directory.Exists(fullpath) Then
                    Console.WriteLine("Path does not exist!")
                    Return
                End If
            Catch Exc As SecurityException
                Console.WriteLine("Write permission to specified path is denied!")
                Return
            Catch Exc As PathTooLongException
                Console.WriteLine("Specified path too long!")
                Return
            Catch Exc As DirectoryNotFoundException
                Console.WriteLine("Specified path does not exist!")
                Return
            End Try
        End If

        If fullpath Is Nothing Or fullpath.Trim().Length = 0 Then
            fullpath = Environment.CurrentDirectory
        End If

        Try
            Dim genList As New GenerateList(provider, fullpath)
            genList.GenerateCode()
        Catch e As Exception
            Console.WriteLine("Error generating files" & vbCrLf & e.ToString())
            Return
        End Try

        Console.WriteLine("Code generated at " + fullpath)

    End Sub 'Main


    'Help message
    Private Shared Sub WriteUsage()
        Console.WriteLine("---Usage---")
        Console.WriteLine("CodeDomSample [lang] [path (optional)]")
        Console.WriteLine("lang - VB/C# - language in which files need to be generated")
        Console.WriteLine("path - path where files generated need to reside (optional parameter)")
        Console.WriteLine()

    End Sub 'WriteUsage
End Class 'GenerateGenericList 
