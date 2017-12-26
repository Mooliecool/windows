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
Imports System.IO
Imports System.IO.Compression


'/ <summary>
'/ This is a Writer that writes the files into a zip
'/ archive one after another.  The zip entries contains 
'/ information about the file name, size, compressed size,
'/ CRC, etc.
'/ It includes support for GZIP and DEFLATE compression methods.
'/ </summary>

Public Class ZipWriter
    Private zipStream As Stream 'The zip stream which is instantiated
    'and closed after every operation
    Private baseStream As Stream 'The base stream from which the header 
    'and compressed data are read
    Private offset As Long
    Private thisMethod As Byte
    
    Private md5 As System.Security.Cryptography.MD5CryptoServiceProvider
    
    'Required for creating CRC
    '/ <summary>
    '/ Creates a new Zip writer, used to write a zip archive.
    '/ </summary>
    '/ <param name="fileStream">
    '/ the output stream to which the zip archive is written.
    '/ </param>
    Public Sub New(ByVal fileStream As Stream) 
        baseStream = fileStream
        md5 = New System.Security.Cryptography.MD5CryptoServiceProvider()
    
    End Sub 'New
    
    
    '/ <summary>
    '/ Super Header format
    '/ </summary>
    '/ <param name="number">Number of files in the archive</param>
    '/ <param name="mode">Mode of zipping can be either GZip or Deflate</param>
    Overloads Public Sub WriteSuperHeader(ByVal number As Int16, ByVal mode As Byte) 
        baseStream.Seek(0, SeekOrigin.Begin)
        WriteLeInt16(number)
        baseStream.WriteByte(mode)
    
    End Sub 'WriteSuperHeader
    
    
    '/ <summary>
    '/ Writes the superheader
    '/ Resets the pointer back to the original position
    '/ </summary>
    '/ <param name="number">Number of files in the archive</param>
    Overloads Private Sub WriteSuperHeader(ByVal number As Int16) 
        Try
            Dim pos As Long = baseStream.Position
            baseStream.Seek(0, SeekOrigin.Begin)
            WriteLeInt16(number)
            baseStream.Seek(pos, SeekOrigin.Begin)
        Catch ex As IOException
            ZipConstants.ShowError(ZipConstants.IOError)
        Catch ex As ArgumentException
            ZipConstants.ShowError(ZipConstants.SeekError)
        End Try
    
    End Sub 'WriteSuperHeader
    
    
    '/ <summary>
    '/ Writes a stream of bytes into the stream.
    '/ </summary>
    Private Sub WriteBytes(ByVal value() As Byte) 
        Dim b As Byte
        For Each b In  value
            baseStream.WriteByte(b)
        Next b
    
    End Sub 'WriteBytes
     '/ <summary>
    '/ Write an int16 in little endian byte order.
    '/ </summary>
    Private Sub WriteLeInt16(ByVal value As Int16)
        If (value < 0) Then
            Dim s As UInt16 = System.Math.Pow(2, 16) + value
            baseStream.WriteByte(System.Convert.ToByte(s And &HFF))
            baseStream.WriteByte(System.Convert.ToByte(s >> 8))
        Else
            baseStream.WriteByte(System.Convert.ToByte(value And &HFF))
            baseStream.WriteByte(System.Convert.ToByte(value >> 8))

        End If

    End Sub 'WriteLeInt16

    
    
    '/ <summary>
    '/ Write an int32 in little endian byte order.
    '/ </summary>
    Private Sub WriteLeInt32(ByVal value As Int32)
        Dim i As UInt16
        Dim j As Int16
        i = value And &HFFFF
        If (i > (System.Math.Pow(2, 15) - 1)) Then
            j = -(System.Math.Pow(2, 16) - i)
        Else
            j = CType(i, Int16)
        End If
        WriteLeInt16(j)
        WriteLeInt16(System.Convert.ToInt16(value >> 16))

    End Sub 'WriteLeInt32
    
    
    '/ <summary>
    '/ Puts the next header in a predefined order
    '/ </summary>
    '/ <param name="entry">
    '/ the ZipEntry which contains all the information
    '/ </param>
    Private Sub PutNextHeader(ByVal entry As ZipEntry) 
        Try
            WriteLeInt32(entry.Size)
            'REcord the offset to write proper CRC and compressed size
            offset = baseStream.Position
            WriteLeInt32(entry.CompressedSize)
            WriteBytes(entry.GetCrc())
            WriteLeInt32(entry.DosTime)
            WriteLeInt16(entry.NameLength)
            Dim names As Byte() = ConvertToArray(entry.Name)
            baseStream.Write(names, 0, names.Length)
        Catch e As IOException
            ZipConstants.ShowError(ZipConstants.IOError)
        Catch ex As ArgumentException
            ZipConstants.ShowError(ZipConstants.SeekError)
        End Try
    
    End Sub 'PutNextHeader
    
    
    '/ <summary>
    '/ Writes the compressed data into the basestream 
    '/ It instantiates a memory stream which will serve 
    '/ as a temp store and then compresses it using Gzip Stream or
    '/ Deflate stream and writes it to the base stream
    '/ </summary>
    Private Sub WriteCompressedFile(ByVal fStream As FileStream, ByVal entry As ZipEntry) 
        Dim ms As New MemoryStream()
        Try
            If thisMethod = ZipConstants.DEFLATE Then
                zipStream = New DeflateStream(ms, CompressionMode.Compress, True)
            ElseIf thisMethod = ZipConstants.GZIP Then
                zipStream = New GZipStream(ms, CompressionMode.Compress, True)
            End If
            
            Dim buffer(fStream.Length - 1) As Byte
            fStream.Read(buffer, 0, buffer.Length)
            zipStream.Write(buffer, 0, buffer.Length)
            zipStream.Close()
            
            Dim b(ms.Length - 1) As Byte
            ms.Seek(0, SeekOrigin.Begin)
            ms.Read(b, 0, b.Length)
            baseStream.Write(b, 0, b.Length)
            'Go back and write the length and the CRC
            WriteCompressedSizeCRC(CType(ms.Length, Integer), ComputeMD5(b), entry)
        
        Catch ex As IOException
            ZipConstants.ShowError(ZipConstants.IOError)
        Catch ex As ArgumentException
            ZipConstants.ShowError(ZipConstants.SeekError)
        Finally
            ms.Close()
        End Try
    
    End Sub 'WriteCompressedFile
     
    
    Private Sub WriteCompressedSizeCRC(ByVal value As Int32, ByVal crc() As Byte, ByVal entry As ZipEntry) 
        Try
            entry.CompressedSize = value
            entry.SetCrc(crc)
            baseStream.Seek(offset, SeekOrigin.Begin)
            WriteLeInt32(entry.CompressedSize)
            WriteBytes(crc)
            'Remove the recorded offset
            offset = - 1
            baseStream.Seek(0, SeekOrigin.End)
        Catch ex As IOException
            ZipConstants.ShowError(ZipConstants.IOError)
        Catch ex As ArgumentException
            ZipConstants.ShowError(ZipConstants.ArgumentError)
        End Try
    
    End Sub 'WriteCompressedSizeCRC
    
    
    Private Function ConvertToArray(ByVal name As String) As Byte() 
        Return System.Text.Encoding.ASCII.GetBytes(name)
    
    End Function 'ConvertToArray
    
    
    '/ <summary>
    '/ Completes the header.This will update the superheader.
    '/ </summary>
    '/ <param name="numberOfFiles">
    '/ The total number of files in the archive
    '/ </param>
    Public Sub CloseHeaders(ByVal numberOfFiles As Int16) 
        WriteSuperHeader(numberOfFiles)
    
    End Sub 'CloseHeaders
    
    
    '/ <summary>
    '/ Add a new entry to the zip
    '/ </summary>
    '/ <param name="entry">
    '/ The details about the header of the entry
    '/ </param>
    Public Sub Add(ByVal entry As ZipEntry) 
        Dim fs As FileStream = Nothing
        Try
            fs = File.OpenRead(entry.Name)
            entry.Size = CType(fs.Length, Int32)
            entry.ModifiedDateTime = File.GetLastWriteTime(entry.Name)
            PutNextHeader(entry)
            WriteCompressedFile(fs, entry)
            CompressionForm.statusMessage = ZipConstants.AddMessage
        Catch ex As ArgumentOutOfRangeException
            ZipConstants.ShowError(ZipConstants.ArgumentError)
        Catch ex As ArgumentException

            ZipConstants.ShowError(ZipConstants.FileError)
        Catch ex As FileNotFoundException
            ZipConstants.ShowError(ZipConstants.FileNotFoundError)
        Catch ex As IOException
            ZipConstants.ShowError(ZipConstants.IOError)
        Catch ex As OutOfMemoryException
            ZipConstants.ShowError(ZipConstants.MemoryError)
        Catch ex As UnauthorizedAccessException
            ZipConstants.ShowError(ZipConstants.FileNotFoundError)
        End Try

    End Sub 'Add

    '/ <summary>
    '/ Sets default compression method. 
    '/ </summary>
    '/ <param name = "method">
    '/ the Compression method which can be Gzip or deflate.
    '/ </param>

    Public Property Method() As Byte
        Get
            Return thisMethod
        End Get
        Set(ByVal value As Byte)
            thisMethod = Value
        End Set
    End Property


    Public Function ComputeMD5(ByVal input() As Byte) As Byte()
        Return md5.ComputeHash(input)

    End Function 'ComputeMD5


    '/ <summary>
    '/ Remove an entry from the archive
    '/ </summary>
    '/ <param name="jump">
    '/ The offset of the file to be removed
    '/ </param>
    Public Sub Remove(ByVal jump As Long, ByVal entry As ZipEntry)
        Try
            Dim fileJump As Long = ZipConstants.FixedHeaderSize + entry.NameLength + entry.CompressedSize
            baseStream.Seek(jump + fileJump, SeekOrigin.Begin)
            Dim length As Long = baseStream.Length - fileJump - jump
            Dim b(length - 1) As Byte
            baseStream.Read(b, 0, CType(length, Integer))
            baseStream.Seek(jump, SeekOrigin.Begin)
            baseStream.Write(b, 0, CType(length, Integer))
            baseStream.SetLength(baseStream.Length - fileJump)
            CompressionForm.statusMessage = "Removed successfully"
        Catch ex As ArgumentException
            ZipConstants.ShowError(ZipConstants.FileError)
        Catch ex As IOException
            ZipConstants.ShowError(ZipConstants.IOError)
        End Try

    End Sub 'Remove
End Class 'ZipWriter