'********************************* Module Header *********************************\
' Module Name:        ProjectFileItem.vb
' Project :           VBVSXSaveProject
' Copyright (c)       Microsoft Corporation
' 
' Get the project files information. the flag of both IsUnderProjectFolder and
' Included are used to set the options about selected copy files item.
' 
' The source is subject to the Microsoft Public License.
' See http://www.microsoft.com/opensource/licenses.mspx#Ms-PL.
' All other rights reserved
' 
' THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY KIND, EITHER 
' EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED WARRANTIES OF 
' MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR PURPOSE.
'***********************************************************************************

Imports System.IO

Namespace Files
    Public Class ProjectFileItem
        ''' <summary>
        ''' The file information.
        ''' </summary>
        Public Property Fileinfo() As FileInfo

        ''' <summary>
        ''' File name.
        ''' </summary>
        Public ReadOnly Property FileName() As String
            Get
                Return Fileinfo.Name
            End Get
        End Property

        ''' <summary>
        ''' Full path of the file.
        ''' </summary>
        Public ReadOnly Property FullName() As String
            Get
                Return Fileinfo.FullName
            End Get
        End Property

        ''' <summary>
        ''' Specify whether the file is in project folder.
        ''' </summary>
        Public Property IsUnderProjectFolder() As Boolean

        ''' <summary>
        ''' Specify whether the file should be copied.
        ''' </summary>
        Public Property NeedCopy() As Boolean
    End Class
End Namespace