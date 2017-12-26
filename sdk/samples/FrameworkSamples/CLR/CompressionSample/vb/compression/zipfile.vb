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
Imports System.Collections.Generic
Imports System.IO


'/ <summary>
'/ This class represents a Zip archive.  You can ask for the contained
'/ entries, or get an uncompressed file for a file entry.  
'/ </summary>

Public Class ZipFile
    Private zipEntries As List(Of ZipEntry)
    Private thisReader As ZipReader
    Private thisWriter As ZipWriter
    
    Private baseStream As Stream ' Stream to which the writer writes 
    ' both header and data, the reader
    ' reads this
    Private zipName As String
    
    
    '/ <summary>
    '/ Created a new Zip file with the given name.
    '/ </summary>
    '/ <param name="method"> Gzip or deflate</param>
    '/ <param name="name"> Zip name</param>
    Public Sub New(ByVal name As String, ByVal method As Byte, ByVal mode As FileMode) 
        Try
            zipName = name
            
            baseStream = New FileStream(zipName, mode)
            thisWriter = New ZipWriter(baseStream)
            thisWriter.Method = method
            
            'New File
            thisWriter.WriteSuperHeader(0, method)
            
            Dim index1 As Integer = zipName.IndexOf(ZipConstants.Dot)
            Dim index2 As Integer = zipName.LastIndexOf(ZipConstants.BackSlash)
            thisReader = New ZipReader(baseStream, zipName.Substring(index2, index1 - index2))
            
            zipEntries = thisReader.GetAllEntries()
            CompressionForm.statusMessage = String.Format(System.Threading.Thread.CurrentThread.CurrentUICulture, ZipConstants.NewMessage, name)
        Catch ex As IOException
            ZipConstants.ShowError(ZipConstants.IOError)
        End Try

    End Sub 'New


    '/ <summary>
    '/ Opens a Zip file with the given name.
    '/ </summary>
    '/ <param name="name"> Zip name</param>
    Public Sub New(ByVal name As String)
        Try
            zipName = name
            baseStream = New FileStream(zipName, FileMode.Open)
            thisWriter = New ZipWriter(baseStream)

            Dim index1 As Integer = zipName.IndexOf(ZipConstants.Dot)
            Dim index2 As Integer = zipName.LastIndexOf(ZipConstants.BackSlash)
            thisReader = New ZipReader(baseStream, zipName.Substring(index2, index1 - index2))

            zipEntries = thisReader.GetAllEntries()
            thisWriter.Method = thisReader.Method
            If CompressionForm.statusMessage = Nothing Or CompressionForm.statusMessage <> String.Empty Then
                CompressionForm.statusMessage = String.Format(System.Threading.Thread.CurrentThread.CurrentUICulture, ZipConstants.OpenMessage, name)
            End If
        Catch ex As IOException
            ZipConstants.ShowError("Error opening the file")
        Catch ex As ArgumentOutOfRangeException
            ZipConstants.ShowError(ZipConstants.CorruptedError)
        End Try

    End Sub 'New



    '/ <summary>
    '/ Gets offset to which the jump should be made by summing up 
    '/ all the individual lengths.
    '/ </summary>
    '/ <returns>
    '/ the offset from SeekOrigin.Begin
    '/ </returns>
    Private Function GetOffset(ByVal index As Integer) As Long
        If index > zipEntries.Count Then
            Return -1
        End If
        Dim jump As Integer = ZipConstants.SuperHeaderSize
        Dim i As Integer

        i = 0
        While i < index - 1
            Dim entry As ZipEntry = zipEntries(i)
            jump += ZipConstants.FixedHeaderSize + entry.NameLength + entry.CompressedSize
            i += 1
        End While
        Return jump

    End Function 'GetOffset


    Public Sub Add(ByVal fileName As String)
        Dim ci As System.Globalization.CultureInfo = System.Threading.Thread.CurrentThread.CurrentUICulture
        If fileName.ToLower(ci).Equals(zipName.ToLower(ci)) Then
            ZipConstants.ShowError("Cannot add the current xip file")
            CompressionForm.statusMessage = String.Empty
            Return
        End If
        Dim entry As New ZipEntry(fileName)
        thisWriter.Add(entry)

        If CompressionForm.statusMessage.Length <> 0 Then
            zipEntries.Add(entry)
            thisWriter.CloseHeaders(CType(zipEntries.Count, Int16))
        End If

    End Sub 'Add


    Public Sub Extract(ByVal index As Integer, ByVal path As String)

        If index < 0 OrElse index >= zipEntries.Count Then
            ZipConstants.ShowError("Argument out of range" + "exception")
            Return
        End If
        thisReader.Extract(zipEntries(index), GetOffset((index + 1)), path)

    End Sub 'Extract


    Public Sub ExtractAll(ByVal path As String)
        thisReader.ExtractAll(zipEntries, path)

    End Sub 'ExtractAll


    '/ <summary>
    '/ Closes the ZipFile.  This also closes all input streams given by
    '/ this class.  After this is called, no further method should be
    '/ called.
    '/ </summary>
    Public Sub Close()
        If Not (baseStream Is Nothing) Then
            baseStream.Close()
        End If

    End Sub 'Close 
    '/ <summary>
    '/ Gets the entries of compressed files.
    '/ </summary>
    '/ <returns>
    '/ Collection of ZipEntries
    '/ </returns>
    Public ReadOnly Property Entries() As List(Of ZipEntry)
        Get
            Return zipEntries
        End Get
    End Property


    Public Function CompressionMethod() As Byte
        Return thisWriter.Method

    End Function 'CompressionMethod


    Public Function CheckFileExists(ByVal fileName As String) As Integer
        Dim ci As System.Globalization.CultureInfo = System.Threading.Thread.CurrentThread.CurrentUICulture
        Dim i As Integer = -1
        Dim eachEntry As ZipEntry
        For Each eachEntry In zipEntries
            i += 1
            If eachEntry.Name.ToLower(ci).Equals(fileName.ToLower(ci)) Then
                Return i
            End If
        Next eachEntry
        Return -1

    End Function 'CheckFileExists


    '/ <summary>
    '/ Remove an entry from the archive
    '/ </summary>
    '/ <param name="index">
    '/ The index of the entry that is to be removed
    '/ </param>
    Private Sub DeleteEntryFromFile(ByVal index As Integer)
        Dim jump As Long = ZipConstants.SuperHeaderSize
        Dim i As Integer

        While i < index
            jump += ZipConstants.FixedHeaderSize + zipEntries(i).NameLength + zipEntries(i).CompressedSize
            i += 1
        End While
        Dim entry As ZipEntry = zipEntries(index)
        Dim fileJump As Long = ZipConstants.FixedHeaderSize + entry.NameLength + entry.CompressedSize
        baseStream.Seek(jump + fileJump, SeekOrigin.Begin)
        Dim length As Long = baseStream.Length - fileJump - jump
        Dim b(length) As Byte
        baseStream.Read(b, 0, CType(length, Integer))
        baseStream.Seek(jump, SeekOrigin.Begin)
        baseStream.Write(b, 0, CType(length, Integer))
        baseStream.SetLength(baseStream.Length - fileJump)
        CompressionForm.statusMessage = "Removed successfully"

    End Sub 'DeleteEntryFromFile


    '/ <summary>
    '/ Remove an entry from the archive
    '/ </summary>
    '/ <param name="index">
    '/ The index of the entry that is to be removed
    '/ </param>
    Public Sub Remove(ByVal index As Integer)
        Dim jump As Long = ZipConstants.SuperHeaderSize
        Dim i As Integer

        While i < index
            jump += ZipConstants.FixedHeaderSize + zipEntries(i).NameLength + zipEntries(i).CompressedSize
            i += 1
        End While
        thisWriter.Remove(jump, zipEntries(index))
        zipEntries.RemoveAt(index)
        If CompressionForm.statusMessage.Length <> 0 Then
            thisWriter.CloseHeaders(CType(zipEntries.Count, Int16))
        End If

    End Sub 'Remove
End Class 'ZipFile 
