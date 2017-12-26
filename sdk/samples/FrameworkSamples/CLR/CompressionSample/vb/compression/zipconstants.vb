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
'/ This class contains ZipConstants used for gzip.
'/ </summary>

NotInheritable Class ZipConstants
    
    Private Sub New() 
    
    End Sub 'New
    Public Const DEFLATE As Byte = 4 'Indicating deflate method of compression
    Public Const GZIP As Byte = 6 'Indicating gzip method of compression
    Public Const FixedHeaderSize As Integer = 30 'Fixed part of the header
    Public Const SuperHeaderSize As Integer = 3
    
    Public Const AddFiles As String = "Add files"
    Public Const NewArchive As String = "New archive"
    Public Const OpenArchive As String = "Open archive"
    'Extensions for the files
    Public Const Extension As String = "xip files (*.xip)|*.xip"
    Public Const AllExtensions As String = "All files (*.*)|*.*"
    
    Public Const BackSlash As String = "\"
    Public Const Dot As String = "."
    
    Public Const ExtractMessage As String = "Extracted files to {0} successfully."
    Public Const OpenMessage As String = "Opened archive {0} for editing"
    Public Const NewMessage As String = "Opened new archive {0} for editing"
    Public Const AddMessage As String = "Added files to the archive."
    
    Public Const Title As String = "Compression sample"
    Public Const GzipName As String = "GZip"
    Public Const DeflateName As String = "Deflate"
    Public Const ErrorName As String = "Error"
    
    Public Const SeekError As String = "Attempted to seek before the beginning"
    Public Const IOError As String = "An IO error has occured"
    Public Const CloseError As String = "Stream already closed"
    Public Const MemoryError As String = "Running out of memory"
    Public Const ArgumentError As String = "Argument invalid"
    Public Const FileNotFoundError As String = "File not found"
    Public Const FileError As String = "Filename invalid"
    Public Const CorruptedError As String = "Trying to read a corrupted xip file"
    Public Const FileExistsError As String = "File already exists"
    
    Public Const FileReplace As String = "The file chosen already exists. Do you want to replace it?"
    Public Const Replace As String = "Replace"
    
    Public Shared Sub ShowError(ByVal [error] As String) 
        Dim opt As MessageBoxOptions
        If System.Threading.Thread.CurrentThread.CurrentUICulture.TextInfo.IsRightToLeft = True Then
            opt = MessageBoxOptions.RightAlign Or MessageBoxOptions.RtlReading
        Else
            opt = MessageBoxOptions.DefaultDesktopOnly
        End If
        MessageBox.Show([error], ErrorName, MessageBoxButtons.OK, MessageBoxIcon.Error, MessageBoxDefaultButton.Button1, opt)
        CompressionForm.statusMessage = String.Empty
    
    End Sub 'ShowError
End Class 'ZipConstants