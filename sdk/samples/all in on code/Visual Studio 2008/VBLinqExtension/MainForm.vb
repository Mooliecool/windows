'****************************** Module Header ******************************\
' Module Name:    MainForm.cs
' Project:        VBLinqExtension
' Copyright (c) Microsoft Corporation.
'
' The VBSLinqExtension sample demonstrates the Microsoft Language-Integrated 
' Query (LINQ) technology to access CSV files, File System information, .NET
' assemblies(Reflection), and string text.  
' 
' This source is subject to the Microsoft Public License.
' See http://www.microsoft.com/opensource/licenses.mspx#Ms-PL.
' All other rights reserved.
'
' THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY KIND, 
' EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED 
' WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR PURPOSE.
'***************************************************************************/

#Region "Imports directives"
Imports System
Imports System.Collections.Generic
Imports System.ComponentModel
Imports System.Data
Imports System.Drawing
Imports System.Linq
Imports System.Text
Imports System.Windows.Forms
#End Region

Public Class MainForm

#Region "LINQ Extension Introduction"

    ' LINQ to CSV Introduction
    Private LinqToCSVIntro As String = _
    "A simple LINQ to CSV library to load simple CSV files into string arrays line by line." & _
    vbCr & vbLf & vbCr & vbLf & _
    "The CSV file stores All-In-One Code Framework project information and its format:" & _
    vbCr & vbLf & "(Project Name, Program Language, Description, Owner)"

    ' LINQ to File System Introduction
    Private LinqToFileSystemIntro As String = _
    "A simple LINQ to File System library to show files grouped by extension names under one folder, " & _
    "and to display the largest file name and the size."

    ' LINQ to Reflection Introduction
    Private LinqToReflectionIntro As String = _
    "A simple LINQ to Reflection library to display the methods of the exported class in an assembly grouped by return value type."

    ' LINQ to String Introduction
    Private LinqToStringIntro As String = _
    "A simple LINQ to String library to show the digis characters in a string, to count occurrences of a word in a string, " & _
    "and to query for sentences that contain a specified set of words."

#End Region


#Region "Load LINQ Extension Forms Button Event Handlers"

    ' LINQ to CSV Button
    Private Sub btnLinqToCSV_Click(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles btnLinqToCSV.Click
        Dim form As New LinqToCSVForm
        form.ShowDialog()
    End Sub

    ' LINQ to File System Button
    Private Sub btnLinqToFileSystem_Click(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles btnLinqToFileSystem.Click
        Dim form As New LinqToFileSystemForm
        form.ShowDialog()
    End Sub

    ' LINQ to Reflection Button
    Private Sub btnLinqToReflection_Click(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles btnLinqToReflection.Click
        Dim form As New LinqToReflectionForm
        form.ShowDialog()
    End Sub

    ' LINQ to String Button
    Private Sub btnLinqToString_Click(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles btnLinqToString.Click
        Dim form As New LinqToStringForm
        form.ShowDialog()
    End Sub

    ' LINQ to String Button Mouse Enter
    Private Sub btnLinqToCSV_MouseEnter(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles btnLinqToCSV.MouseEnter
        ' Display LINQ to CSV Introduction
        lblIntroduction.Text = "LINQ to CSV"
        txtBoxIntro.Text = LinqToCSVIntro
    End Sub

    ' LINQ to File System Button Mouse Enter
    Private Sub btnLinqToFileSystem_MouseEnter(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles btnLinqToFileSystem.MouseEnter
        ' Display LINQ to File System Introduction
        lblIntroduction.Text = "LINQ to File System"
        txtBoxIntro.Text = LinqToFileSystemIntro
    End Sub

    ' LINQ to Reflection Button Mouse Enter
    Private Sub btnLinqToReflection_MouseEnter(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles btnLinqToReflection.MouseEnter
        ' Display LINQ to Reflection Introduction
        lblIntroduction.Text = "LINQ to Reflection"
        txtBoxIntro.Text = LinqToReflectionIntro
    End Sub

    ' LINQ to String Button Mouse Enter
    Private Sub btnLinqToString_MouseEnter(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles btnLinqToString.MouseEnter
        ' Display LINQ to String Introduction
        lblIntroduction.Text = "LINQ to String"
        txtBoxIntro.Text = LinqToStringIntro
    End Sub

    ' Button Mouse Leave
    Private Sub btnLinqToCSV_MouseLeave(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles btnLinqToCSV.MouseLeave
        ' Clear the LINQ Extension Introduction
        lblIntroduction.Text = ""
        txtBoxIntro.Text = ""
    End Sub

    ' Button Mouse Leave
    Private Sub btnLinqToFileSystem_MouseLeave(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles btnLinqToFileSystem.MouseLeave
        ' Clear the LINQ Extension Introduction
        lblIntroduction.Text = ""
        txtBoxIntro.Text = ""
    End Sub

    ' Button Mouse Leave
    Private Sub btnLinqToReflection_MouseLeave(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles btnLinqToReflection.MouseLeave
        ' Clear the LINQ Extension Introduction
        lblIntroduction.Text = ""
        txtBoxIntro.Text = ""
    End Sub

    ' Button Mouse Leave
    Private Sub btnLinqToString_MouseLeave(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles btnLinqToString.MouseLeave
        ' Clear the LINQ Extension Introduction
        lblIntroduction.Text = ""
        txtBoxIntro.Text = ""
    End Sub

#End Region

End Class