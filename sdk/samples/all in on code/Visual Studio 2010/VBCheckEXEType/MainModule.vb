'*************************** Module Header ******************************\
' Module Name:  MainModule.vb
' Project:	    VBCheckEXEType
' Copyright (c) Microsoft Corporation.
' 
' This source file is used to handle the input command.
' 
' 
' This source is subject to the Microsoft Public License.
' See http://www.microsoft.com/opensource/licenses.mspx#Ms-PL.
' All other rights reserved.
' 
' THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY KIND, 
' EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED 
' WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR PURPOSE.
'**************************************************************************

Imports System.IO

Module MainModule

    Sub Main()
        Do
            Console.WriteLine("Please type the exe file path:")
            Console.WriteLine("<Empty to exit>")
            Dim path As String = Console.ReadLine()

            If String.IsNullOrEmpty(path) Then
                Exit Do
            End If

            If Not File.Exists(path) Then
                Console.WriteLine("The path does not exist!")
                Continue Do
            End If
            Try
                Dim exeFile As New ExecutableFile(path)

                Dim isConsole = exeFile.IsConsoleApplication
                Dim isDotNet = exeFile.IsDotNetAssembly


                Console.WriteLine(String.Format("ConsoleApplication: {0}" _
                                                & ControlChars.CrLf _
                                                & ".NetApplication: {1}",
                                                isConsole,
                                                isDotNet))

                If isDotNet Then
                    Console.WriteLine("Compiled .NET Runtime: " & exeFile.GetCompiledRuntimeVersion())
                    Console.WriteLine("Full Name: " & exeFile.GetFullDisplayName())
                    Dim attributes = exeFile.GetAttributes()
                    For Each attribute In attributes
                        Console.WriteLine(String.Format("{0}: {1}", attribute.Key, attribute.Value))
                    Next attribute
                Else
                    Dim is32Bit = exeFile.Is32bitImage
                    Console.WriteLine("32bit application: " & is32Bit)
                End If
            Catch ex As Exception
                Console.WriteLine(ex.Message)
            End Try
            Console.WriteLine()
        Loop
    End Sub

End Module
