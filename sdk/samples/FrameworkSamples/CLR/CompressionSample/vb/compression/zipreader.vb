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
Imports System.IO.Compression


'/ <summary>
'/ This is a Reader that reads the files baseStream to get an 
'/ entry in the zip archive one after another.The zip entry contains 
'/ information about the file name, size, compressed size, CRC, etc.
'/ It includes support for GZIP and DEFLATE methods of compression.
'/ <summary>

Public Class ZipReader
    Private zipStream As Stream 'The zip stream which is instantiated
    'and closed after every operation
    Private zipName As String 'Name given to the archive
    Private baseStream As Stream 'The base stream from which the header 
    'and compressed data are read
    Private numberOfFiles As Int16
    Private thisMethod As Byte
    
    Private md5 As System.Security.Cryptography.MD5CryptoServiceProvider
    
    'Required for checking CRC
    '/ <summary>
    '/ Creates a new Zip input stream, reading a zip archive.
    '/ </summary>
    Public Sub New(ByVal fileStream As Stream, ByVal name As String) 
        zipName = name
        baseStream = fileStream
        numberOfFiles = - 1
        thisMethod = 255
        md5 = New System.Security.Cryptography.MD5CryptoServiceProvider()
    
    End Sub 'New
    
    
    '/ <summary>
    '/ Reads the super header
    '/ Super header structure:
    '/		number of files - 2 byte
    '/		method of compression - 1 byte
    '/ </summary>
    '/ <exception cref="ArgumentOutOfRangeException">
    '/ Thrown if the super header is tampered
    '/ </exception>
    '/ 
    Private Sub ReadSuperHeader() 
        numberOfFiles = ReadLeInt16()
        thisMethod = ReadLeByte()
        If method <> ZipConstants.DEFLATE AndAlso method <> ZipConstants.GZIP Then
            Throw New ArgumentOutOfRangeException()
        End If
    
    End Sub 'ReadSuperHeader
     
    Private Function ReadBuf(ByVal outBuf() As Byte, ByVal length As Integer) As Integer 
        Return baseStream.Read(outBuf, 0, length)
    
    End Function 'ReadBuf
    
    
    '/ <summary>
    '/ Read a byte from baseStream.
    '/ </summary>
    Private Function ReadLeByte() As Byte 
        Return System.Convert.ToByte(baseStream.ReadByte() And &HFF)
    
    End Function 'ReadLeByte
    
    
    '/ <summary>
    '/ Read an unsigned short baseStream little endian byte order.
    '/ </summary>
    Private Function ReadLeInt16() As Int16
        Dim i As Int16 = CType(ReadLeByte(), Int16)
        Dim j As Int16 = CType(ReadLeByte(), Int16)
        Return (i Or (j << 8))

    End Function 'ReadLeInt16
    
    
    '/ <summary>
    '/ Read an int baseStream little endian byte order.
    '/ </summary>
    Private Function ReadLeInt32() As Int32
        Dim ui As Int32
        Dim uj As Int32
        ui = CType(ReadLeInt16(), Int32)
        uj = CType(ReadLeInt16(), Int32)
        If (ui < 0) Then
            ui = System.Math.Pow(2, 16) + ui
        End If
        Return (ui Or (uj << 16))
    End Function 'ReadLeInt32
    
    
    Private Function ConvertToString(ByVal data() As Byte) As String 
        Return System.Text.Encoding.ASCII.GetString(data, 0, data.Length)
    
    End Function 'ConvertToString
    
    
    '/ <summary>
    '/ Open the next entry from the zip archive, and return its 
    '/ description. The method expects the pointer to be intact.
    '/ </summary>
    Private Function GetNextEntry() As ZipEntry 
        Dim currentEntry As ZipEntry = Nothing
        Try
            Dim size As Int32 = ReadLeInt32()
            If size = - 1 Then
                Return New ZipEntry(String.Empty)
            End If 
            Dim csize As Int32 = ReadLeInt32()
            Dim crc(15) As Byte
            ReadBuf(crc, crc.Length)
            
            Dim dostime As Int32 = ReadLeInt32()
            Dim nameLength As Int16 = ReadLeInt16()
            
            Dim buffer(nameLength - 1) As Byte
            ReadBuf(buffer, nameLength)
            Dim name As String = ConvertToString(buffer)
            
            currentEntry = New ZipEntry(name)
            currentEntry.Size = size
            currentEntry.CompressedSize = csize
            currentEntry.SetCrc(crc)
            currentEntry.DosTime = dostime
        Catch ex As ArgumentException
            ZipConstants.ShowError(ZipConstants.ArgumentError)
        Catch ex As ObjectDisposedException
            ZipConstants.ShowError(ZipConstants.CloseError)
        End Try
        Return currentEntry
    
    End Function 'GetNextEntry
    
    
    '/ <summary>
    '/ Writes the uncompressed data into the filename in the 
    '/ entry. It instantiates a memory stream which will serve 
    '/ as a temp store and decompresses it using Gzip Stream or
    '/ Deflate stream
    '/ </summary>
    Private Sub WriteUncompressedFile(ByVal entry As ZipEntry, ByVal completePath As String) 
        Dim ms As New MemoryStream()
        Try
            Dim b(entry.CompressedSize - 1) As Byte
            baseStream.Read(b, 0, CType(entry.CompressedSize, Integer))
            If CheckCRC(entry.GetCrc(), b) Then
                ms.Write(b, 0, b.Length)
            End If
            ms.Seek(0, SeekOrigin.Begin)
            If Method = ZipConstants.DEFLATE Then
                zipStream = New DeflateStream(ms, CompressionMode.Decompress, False)
            ElseIf Method = ZipConstants.GZIP Then
                zipStream = New GZipStream(ms, CompressionMode.Decompress, False)
            End If

            Dim index As Integer = entry.Name.LastIndexOf(ZipConstants.BackSlash)
            Dim name As String = completePath + entry.Name.Substring(index + 1)
            Dim rewrite As New FileStream(name, FileMode.Create)
            b = New Byte(entry.Size - 1) {}
            zipStream.Read(b, 0, CType(entry.Size, Integer))

            rewrite.Write(b, 0, CType(entry.Size, Integer))
            rewrite.Close()
        Catch ex As IOException
            ZipConstants.ShowError(ZipConstants.IOError)
        Catch ex As ArgumentException
            ZipConstants.ShowError(ZipConstants.ArgumentError)
        Finally
            zipStream.Close()
            ms.Close()
        End Try
    
    End Sub 'WriteUncompressedFile
    
    
    '/ <summary>
    '/ Extracts all the entries in the list of entries
    '/	</summary>
    '/ <param name="zipentries">
    '/	List of all the zip entries. Can be empty.
    '/ </param>
    Public Sub ExtractAll(ByVal zipEntries As List(Of ZipEntry), _
                    ByVal startPath As String)
        Try
            Dim dir As New DirectoryInfo(startPath + zipName)
            If Not dir.Exists Then
                dir.Create()
            End If
            Dim jump As Integer = 3
            baseStream.Seek(jump, SeekOrigin.Begin)

            Dim entry As ZipEntry
            For Each entry In zipEntries
                Dim index1 As Integer = entry.Name.IndexOf(ZipConstants.BackSlash)
                Dim index2 As Integer = entry.Name.LastIndexOf(ZipConstants.BackSlash)
                Dim relPath As String = entry.Name.Substring(index1 + 1, index2 - index1)
                If index1 = 0 Then
                    relPath = String.Empty
                End If
                If relPath.Length <> 0 Then
                    dir.CreateSubdirectory(relPath)
                End If
                jump = ZipConstants.FixedHeaderSize + entry.NameLength
                baseStream.Seek(jump, SeekOrigin.Current)
                WriteUncompressedFile(entry, startPath + zipName + ZipConstants.BackSlash + relPath)
            Next entry
            CompressionForm.statusMessage = String.Format(System.Threading.Thread.CurrentThread.CurrentUICulture, ZipConstants.ExtractMessage, startPath + zipName + ZipConstants.BackSlash)
        Catch ex As IOException
            ZipConstants.ShowError(ZipConstants.IOError)
        Catch ex As OutOfMemoryException
            ZipConstants.ShowError(ZipConstants.MemoryError)
        End Try
    End Sub


    '/ <summary>
    '/ Extracts the specified entry
    '/	</summary>
    '/ <param name="entry">
    '/	The entry that is to be extracted.Cannot be null
    '/ </param>
    '/ <param name="jump">
    '/	The offset from the SeekOrigin.Begin at which the 
    '/ comrpessed data is located
    '/ </param>

    Public Sub Extract(ByVal entry As ZipEntry, ByVal jump As Long, ByVal startPath As String)
        Try
            Dim dir As New DirectoryInfo(startPath + zipName)
            If Not dir.Exists Then
                dir.Create()
            End If
            Dim index1 As Integer = entry.Name.IndexOf(ZipConstants.BackSlash)
            Dim index2 As Integer = entry.Name.LastIndexOf(ZipConstants.BackSlash)
            Dim relPath As String = entry.Name.Substring(index1 + 1, index2 - index1)
            If index1 = 0 Then
                relPath = String.Empty
            End If
            If relPath.Length <> 0 Then
                dir.CreateSubdirectory(relPath)
            End If
            baseStream.Seek(jump, SeekOrigin.Begin)
            jump = ZipConstants.FixedHeaderSize + entry.NameLength
            baseStream.Seek(jump, SeekOrigin.Current)

            WriteUncompressedFile(entry, startPath + zipName + ZipConstants.BackSlash + relPath)
            CompressionForm.statusMessage = String.Format(System.Threading.Thread.CurrentThread.CurrentUICulture, ZipConstants.ExtractMessage, startPath + zipName + ZipConstants.BackSlash)
        Catch ex As IOException
            ZipConstants.ShowError(ZipConstants.IOError)
        Catch ex As OutOfMemoryException
            ZipConstants.ShowError(ZipConstants.MemoryError)
        End Try
    End Sub

    '/ <summary>
    '/ Gets all the entries in the file 
    '/ </summary>
    '/ <returns>
    '/	List of all the zip entries
    '/ </returns> 

    Public Function GetAllEntries() As List(Of ZipEntry)
        Dim headers As List(Of ZipEntry)
        headers = Nothing
        Try
            If thisMethod = 255 OrElse numberOfFiles = -1 Then
                baseStream.Seek(0, SeekOrigin.Begin)
                ReadSuperHeader()
            End If
            headers = New List(Of ZipEntry)(numberOfFiles)
            baseStream.Seek(3, SeekOrigin.Begin)
            Dim i As Integer

            While i < numberOfFiles
                Dim entry As ZipEntry = GetNextEntry()
                headers.Add(entry)
                baseStream.Seek(entry.CompressedSize, SeekOrigin.Current)
                i += 1
            End While
        Catch ex As IOException
            ZipConstants.ShowError(ZipConstants.IOError)
        End Try

        Return headers
    End Function


    '/ <summary>
    '/ Gets the method of compression of the archive.
    '/ </summary>
    '/ <returns>
    '/ the ZipConstants.Deflate or ZipConstants.Gzip
    '/ </returns>
    Public ReadOnly Property Method() As Byte
        Get
            Return thisMethod
        End Get
    End Property

    'Check the CRC of the byte array and return true if check successful
    'false otherwise
    Private Function CheckCRC(ByVal crc As Byte(), ByVal data As Byte()) As Boolean
        Dim newCrc As Byte() = md5.ComputeHash(data)
        Dim i As Integer

        While i < crc.Length
            If crc(i) <> newCrc(i) Then
                Return False
            End If
            i += 1
        End While
        Return True

    End Function 'CheckCRC
End Class 'ZipReader