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
Imports System.Windows.Forms


'/ <summary>
'/ File Dialog that has the features for this application.
'/ </summary>

Public Class CustomFileDialog
    'Private openFileDialog control
    Private fileDialog As System.Windows.Forms.OpenFileDialog
    
    
    '/ <summary>
    '/ Creates a new Custom File Dialog object
    '/ </summary>
    Public Sub New() 
        fileDialog = New OpenFileDialog()
        Reset()
    
    End Sub 'New
    
    
    Private Sub Reset() 
        fileDialog.CheckFileExists = False
        fileDialog.CheckPathExists = False
        fileDialog.ValidateNames = False
        fileDialog.InitialDirectory = ZipConstants.Dot
    
    End Sub 'Reset
    
    '/ <summary>
    '/ Opens the dialog in add mode
    '/ User can select multiple files
    '/ </summary>
    '/ <returns>
    '/	All the filenames selected 
    '/</returns>
    Public Function AddMode() As String() 
        fileDialog.FileName = String.Empty
        fileDialog.Multiselect = True
        fileDialog.Title = ZipConstants.AddFiles
        fileDialog.Filter = ZipConstants.AllExtensions
        Dim dr As DialogResult = fileDialog.ShowDialog()
        If dr = DialogResult.OK Then
            Return fileDialog.FileNames
        End If
        Return Nothing
    
    End Function 'AddMode
    
    
    '/ <summary>
    '/ Opens the dialog in new mode
    '/ User cannot select multiple files
    '/ </summary>
    '/ <returns>
    '/	The filename selected 
    '/</returns>
    Public Function NewMode() As String 
        fileDialog.FileName = String.Empty
        fileDialog.Multiselect = False
        fileDialog.Title = ZipConstants.NewArchive
        fileDialog.Filter = ZipConstants.Extension
        'BUGBUG: The filedialog does not add extensions
        Dim dr As DialogResult = fileDialog.ShowDialog()
        If dr = DialogResult.OK Then
            If Not fileDialog.FileName.Contains(".xip") Then
                fileDialog.FileName += ".xip"
            End If
            Return fileDialog.FileName
        End If
        Return Nothing
    
    End Function 'NewMode
    
    
    '/ <summary>
    '/ Opens the dialog in open mode
    '/ User cannot select multiple files
    '/ </summary>
    '/ <returns>
    '/	The filename selected 
    '/</returns>
    Public Function OpenMode() As String 
        fileDialog.FileName = String.Empty
        
        fileDialog.Multiselect = False
        fileDialog.Title = ZipConstants.OpenArchive
        fileDialog.Filter = ZipConstants.Extension
        Dim dr As DialogResult = fileDialog.ShowDialog()
        If dr = DialogResult.OK Then
            If Not fileDialog.FileName.Contains(ZipConstants.Dot) Then
                fileDialog.FileName += ".xip"
            End If
            Return fileDialog.FileName
        End If
        Return Nothing
    
    End Function 'OpenMode
End Class 'CustomFileDialog 
