'***************************** Module Header ******************************\
' Module Name: LinqToFileSystem.vb
' Project: VBLinqExtension
' Copyright (c) Microsoft Corporation.
'
' It is a simple LINQ to File System library to return the FileInfor of all 
' the files under one folder, and to get the largest files among these files.
' 
' This source is subject to the Microsoft Public License.
' See http://www.microsoft.com/opensource/licenses.mspx#Ms-PL.
' All other rights reserved.
'
' THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY KIND, 
' EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED 
' WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR PURPOSE.
'\**************************************************************************

#Region "Imports directives"
Imports System
Imports System.Collections.Generic
Imports System.Linq
Imports System.Text
Imports System.IO
Imports System.Runtime.CompilerServices

#End Region

Public Module LinqToFileSystem

    ''' <summary>
    ''' Get the FileInfo of all the files under one folder
    ''' </summary>
    ''' <param name="path">The path of the certain folder</param>
    ''' <returns>IEnumerable collection of FileInfo</returns>
    ''' 
    Public Function GetFiles(ByVal path As String) As IEnumerable(Of FileInfo)
        ' Check if the folder path exists
        If Directory.Exists(path) Then
            ' Get all file names under the folder
            Dim fileNames = Directory.GetFiles(path, "*.*", SearchOption.AllDirectories)

            Dim list As New List(Of FileInfo)

            ' Create new FileInfos to based on the file names
            For Each name In fileNames
                list.Add(New FileInfo(name))
            Next

            Return list
        Else
            Throw (New DirectoryNotFoundException())
        End If
    End Function


    ''' <summary>
    ''' Extension method of IEnumerable(FileInfo) to get the largest 
    ''' files inside.
    ''' </summary>
    ''' <returns>IEnumerable collection of largest files' FileInfo</returns>
    ''' 
    <Extension()> _
    Public Function LargestFiles(ByVal files As IEnumerable(Of FileInfo)) As IEnumerable(Of FileInfo)
        Dim fileLists As New List(Of FileInfo)()

        Dim maxLength As Long = 0

        ' Check each file to get the largest files
        For Each file In files
            If file.Length > maxLength Then
                fileLists.Clear()
                fileLists.Add(file)
                maxLength = file.Length
            ElseIf file.Length = maxLength Then
                fileLists.Add(file)
            End If
        Next

        Return fileLists
    End Function
End Module
