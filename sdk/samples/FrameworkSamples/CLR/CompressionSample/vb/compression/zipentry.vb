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


'/ <summary>
'/ This class represents a member of a zip archive.  ZipReader and
'/ ZipWriter will give instances of this class as information
'/ about the members in an archive.  
'/ </summary>

Public Class ZipEntry
    'Private fields
    Private thisSize As Int32
    Private thisCompressedSize As Int32
    Private thisDosTime As Int32 'Time represented as an Int
    Private thisNameLength As Int16 'Length of the variable sized name
    Private thisCrc() As Byte 'Array of 16 CRC bytes
    Private thisName As String
    
    
    '/ <summary>
    '/ Creates a zip entry with the given name.
    '/ </summary>
    '/ <param name="name">
    '/ the name. May include directory components separated by '/'.
    '/ </param>
    Public Sub New(ByVal newName As String)
        If newName Is Nothing Then
            'Wrong entry
            Throw New System.ArgumentNullException()
        End If
        Me.ModifiedDateTime = System.DateTime.Now
        Me.Name = newName
        Me.thisSize = 0
        Me.thisCompressedSize = 0
        Me.thisCrc = New Byte(15) {}

    End Sub 'New
    
    
    
    Public Property DosTime() As Int32
        Get
            Return thisDosTime
        End Get
        Set(ByVal value As Int32)
            Me.thisDosTime = value
        End Set
    End Property
    
    
    Public Property NameLength() As Int16
        Get
            Return thisNameLength
        End Get
        Set(ByVal value As Int16)
            'Check if the value is greater than 16 bytes
            If CType(value, Int16) > &HFFFF Then
                Throw New ArgumentOutOfRangeException()
            End If
            Me.thisNameLength = value
        End Set
    End Property
    
    '/ <summary>
    '/ Gets/Sets the time of last modification of the entry.
    '/ </summary>
    
    Public Property ModifiedDateTime() As DateTime
        Get
            Dim sec As Integer = 2 * (thisDosTime And &H1F)
            Dim min As Integer = (thisDosTime >> 5) And &H3F
            Dim hrs As Integer = (thisDosTime >> 11) And &H1F
            Dim day As Integer = (thisDosTime >> 16) And &H1F
            Dim mon As Integer = (thisDosTime >> 21) And &HF
            Dim year As Integer = ((thisDosTime >> 25) And &H7F) + 1980 ' since 1900 
            Return New System.DateTime(year, mon, day, hrs, min, sec)
        End Get
        Set(ByVal value As DateTime)
            DosTime = ((CType(value.Year, Int32) - 1980 And &H7F) << 25) _
                Or (CType(value.Month, Int32) << 21) Or _
                (CType(value.Day, Int32) << 16) Or (CType(value.Hour, Int32) << 11) _
                Or (CType(value.Minute, Int32) << 5) Or (CType(value.Second, Int32) >> 1)
        End Set
    End Property
    
    '/ <summary>
    '/ Returns the entry name.  The path components in the entry are
    '/ always separated by slashes ('/').
    '/ </summary>
    
    Public Property Name() As String 
        Get
            Return thisName
        End Get
        Set
            'Check if the value is greater than 16 bytes or null
            If value Is Nothing OrElse value.Length > &HFFFFL Then
                Throw New ArgumentOutOfRangeException()
            End If 
            If value.Length <> 0 Then
                thisName = value
                thisNameLength = CType(Value.Length, Int16)
            End If
        End Set
    End Property
    
    '/ <summary>
    '/ Gets/Sets the size of the uncompressed data.
    '/ </summary>
    '/ <exception cref="System.ArgumentException">
    '/ if size is not in 0..0xffffffffL
    '/ </exception>
    '/ <returns>
    '/ the size 
    '/ </returns>
    
    Public Property Size() As Int32
        Get
            Return thisSize
        End Get
        Set(ByVal value As Int32)
            'Check if the value is greater than 32 bytes
            If CType(value, Int32) > Int32.MaxValue Then
                Throw New ArgumentOutOfRangeException()
            End If
            thisSize = CType(value, Int32)
        End Set
    End Property
    '/ <summary>
    '/ Gets/Sets the size of the compressed data.
    '/ </summary>
    '/ <exception cref="System.ArgumentOutOfRangeException">
    '/ if csize is not in 0..0xffffffffUL
    '/ </exception>
    '/ <returns>
    '/ the compressed size.
    '/ </returns>
    
    Public Property CompressedSize() As Int32
        Get
            Return thisCompressedSize
        End Get
        Set(ByVal value As Int32)
            'Check if the value is greater than 32 bytes
            If CType(value, Int32) > Int32.MaxValue Then
                Throw New ArgumentOutOfRangeException()
            End If
            Me.thisCompressedSize = CType(value, Int32)
        End Set
    End Property
    
    '/ <summary>
    '/ Gets/Sets the crc of the compressed data.
    '/ </summary>
    '/ <exception cref="System.ArgumentOutOfRangeException">
    '/ if crc is not in 16 byte array
    '/ </exception>
    '/ <returns>
    '/ the crc.
    '/ </returns>
    Public Function GetCrc() As Byte() 
        Return thisCrc
    
    End Function 'GetCrc
    
    Public Sub SetCrc(ByVal value() As Byte) 
        'Check if the Length of value array is greater than 16
        If value.Length <> thisCrc.Length Then
            Throw New ArgumentOutOfRangeException()
        End If
        thisCrc = value
    
    End Sub 'SetCrc
End Class 'ZipEntry 