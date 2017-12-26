'********************************** Module Header **************************************'
' Module Name:      MainModule.vb
' Project:          VBEnumStringConverter
' Copyright (c)     Microsoft Corporation. 
'
'
'
' This source is subject to the Microsoft Public License.
' See http://www.microsoft.com/opensource/licenses.mspx#Ms-PL.
' All other rights reserved.
'
' THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY KIND, 
' EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED 
' WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR PURPOSE. 
'\**************************************************************************************'


Imports System.ComponentModel


Module MainModule

    Sub Main()
        Console.WriteLine("Using EnumConverter to")
        If True Then
            Dim converter As New EnumConverter(GetType(ProgrammingLanguage))

            ' Convert string to enum.
            Dim langStr As String = "VB, CS, Cpp, XAML"
            Console.WriteLine("Convert the string ""{0}"" to enum...", langStr)
            Dim lang As ProgrammingLanguage =
                DirectCast(converter.ConvertFromString(langStr), ProgrammingLanguage)
            Console.WriteLine("Done!")

            ' Convert enum to string.
            Console.WriteLine("Convert the resulting enum to string...")
            langStr = converter.ConvertToString(lang)
            Console.WriteLine("Done! ""{0}""", langStr)
        End If

        Console.WriteLine(vbLf & "Using EnumDescriptionConverter to")
        If True Then
            Dim converter As New EnumDescriptionConverter(GetType(ProgrammingLanguage))

            ' Convert string to enum.
            Dim langStr As String = "Visual Basic, Visual C#, Visual C++, XAML"
            Console.WriteLine("Convert the string ""{0}"" to enum...", langStr)
            Dim lang As ProgrammingLanguage =
                DirectCast(converter.ConvertFromString(langStr), ProgrammingLanguage)
            Console.WriteLine("Done!")

            ' Convert enum to string.
            Console.WriteLine("Convert the resulting enum to string...")
            langStr = converter.ConvertToString(lang)
            Console.WriteLine("Done! ""{0}""", langStr)
        End If

        Console.ReadLine()
    End Sub

End Module

