'***************************** Module Header ******************************\
'Module Name:  ReadOnlyIStreamWrapper.vb
'Project:      VBOneNoteRibbonAddIn
'Copyright (c) Microsoft Corporation.
'
'IStream wrapper class
'
'This source is subject to the Microsoft Public License.
'See http://www.microsoft.com/opensource/licenses.mspx#Ms-PL.
'All other rights reserved.
'
'THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY KIND, 
'EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED 
'WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR PURPOSE.
'\**************************************************************************


#Region "Imports directives"

Imports System.Collections.Generic
Imports System.Linq
Imports System.Text
Imports System.Runtime.InteropServices.ComTypes
Imports System.IO
#End Region


''' <summary>
''' IStream wrapper class
''' </summary>
Friend Class ReadOnlyIStreamWrapper
    Implements IStream
    ' _stream field
    Private _stream As Stream

    ''' <summary>
    ''' CCOMStreamWrapper construct method
    ''' </summary>
    ''' <param name="streamWrap">stream</param>
    Public Sub New(ByVal streamWrap As Stream)
        Me._stream = streamWrap
    End Sub

    ' Summary:
    '     Creates a new stream object with its own seek pointer that references the
    '     same bytes as the original stream.
    '
    ' Parameters:
    '   ppstm:
    '     When this method returns, contains the new stream object. This parameter
    '     is passed uninitialized.
    Public Sub Clone(ByRef ppstm As IStream) Implements IStream.Clone
        ppstm = New ReadOnlyIStreamWrapper(Me._stream)
    End Sub

    '
    ' Summary:
    '     Ensures that any changes made to a stream object that is open in transacted
    '     mode are reflected in the parent storage.
    '
    ' Parameters:
    '   grfCommitFlags:
    '     A value that controls how the changes for the stream object are committed.
    Public Sub Commit(ByVal grfCommitFlags As Integer) Implements IStream.Commit
        Me._stream.Flush()
    End Sub

    '
    ' Summary:
    '     Copies a specified number of bytes from the current seek pointer in the stream
    '     to the current seek pointer in another stream.
    '
    ' Parameters:
    '   pstm:
    '     A reference to the destination stream.
    '
    '   cb:
    '     The number of bytes to copy from the source stream.
    '
    '   pcbRead:
    '     On successful return, contains the actual number of bytes read from the source.
    '
    '   pcbWritten:
    '     On successful return, contains the actual number of bytes written to the
    '     destination.
    Public Sub CopyTo(ByVal pstm As IStream, ByVal cb As Long, ByVal pcbRead As IntPtr, _
                      ByVal pcbWritten As IntPtr) Implements IStream.CopyTo
        Throw New NotSupportedException("ReadOnlyIStreamWrapper does not support CopyTo")
    End Sub

    '
    ' Summary:
    '     Restricts access to a specified range of bytes in the stream.
    '
    ' Parameters:
    '   libOffset:
    '     The byte offset for the beginning of the range.
    '
    '   cb:
    '     The length of the range, in bytes, to restrict.
    '
    '   dwLockType:
    '     The requested restrictions on accessing the range.
    Public Sub LockRegion(ByVal libOffset As Long, ByVal cb As Long, ByVal dwLockType As Integer) _
        Implements IStream.LockRegion
        Throw New NotSupportedException("ReadOnlyIStreamWrapper does not support LockRegion")
    End Sub

    '
    ' Summary:
    '     Reads a specified number of bytes from the stream object into memory starting
    '     at the current seek pointer.
    '
    ' Parameters:
    '   pv:
    '     When this method returns, contains the data read from the stream. This parameter
    '     is passed uninitialized.
    '
    '   cb:
    '     The number of bytes to read from the stream object.
    '
    '   pcbRead:
    '     A pointer to a ULONG variable that receives the actual number of bytes read
    '     from the stream object.
    Public Sub Read(ByVal pv As Byte(), ByVal cb As Integer, ByVal pcbRead As IntPtr) Implements IStream.Read
        System.Runtime.InteropServices.Marshal.WriteInt64(pcbRead, CLng(Me._stream.Read(pv, 0, cb)))
    End Sub

    '
    ' Summary:
    '     Discards all changes that have been made to a transacted stream since the
    '     last System.Runtime.InteropServices.ComTypes.IStream.Commit(System.Int32)
    '     call.
    Public Sub Revert() Implements IStream.Revert
        Throw New NotSupportedException("ReadOnlyIStreamWrapper does not support Revert")
    End Sub

    '
    ' Summary:
    '     Changes the seek pointer to a new location relative to the beginning of the
    '     stream, to the end of the stream, or to the current seek pointer.
    '
    ' Parameters:
    '   dlibMove:
    '     The displacement to add to dwOrigin.
    '
    '   dwOrigin:
    '     The origin of the seek. The origin can be the beginning of the file, the
    '     current seek pointer, or the end of the file.
    '
    '   plibNewPosition:
    '     On successful return, contains the offset of the seek pointer from the beginning
    '     of the stream.
    Public Sub Seek(ByVal dlibMove As Long, ByVal dwOrigin As Integer, ByVal plibNewPosition As IntPtr) _
        Implements IStream.Seek

        Dim num As Long = 0L
        System.Runtime.InteropServices.Marshal.WriteInt64(plibNewPosition, Me._stream.Position)
        Select Case dwOrigin
            Case 0
                num = dlibMove
                Exit Select

            Case 1
                num = Me._stream.Position + dlibMove
                Exit Select

            Case 2
                num = Me._stream.Length + dlibMove
                Exit Select
            Case Else

                Return
        End Select
        If (num >= 0L) AndAlso (num < Me._stream.Length) Then
            Me._stream.Position = num
            System.Runtime.InteropServices.Marshal.WriteInt64(plibNewPosition, Me._stream.Position)
        End If
    End Sub

    '
    ' Summary:
    '     Changes the size of the stream object.
    '
    ' Parameters:
    '   libNewSize:
    '     The new size of the stream as a number of bytes.
    Public Sub SetSize(ByVal libNewSize As Long) Implements IStream.SetSize
        Me._stream.SetLength(libNewSize)
    End Sub

    '
    ' Summary:
    '     Retrieves the System.Runtime.InteropServices.STATSTG structure for this stream.
    '
    ' Parameters:
    '   pstatstg:
    '     When this method returns, contains a STATSTG structure that describes this
    '     stream object. This parameter is passed uninitialized.
    '
    '   grfStatFlag:
    '     Members in the STATSTG structure that this method does not return, thus saving
    '     some memory allocation operations.
    Public Sub Stat(ByRef pstatstg As STATSTG, ByVal grfStatFlag As Integer) Implements IStream.Stat
        pstatstg = New STATSTG()
        pstatstg.cbSize = Me._stream.Length
        If (grfStatFlag And 1) = 0 Then
            pstatstg.pwcsName = Me._stream.ToString()
        End If
    End Sub

    '
    ' Summary:
    '     Removes the access restriction on a range of bytes previously restricted
    '     with the System.Runtime.InteropServices.ComTypes.IStream.LockRegion(System.Int64,System.Int64,
    '     System.Int32)
    '     method.
    '
    ' Parameters:
    '   libOffset:
    '     The byte offset for the beginning of the range.
    '
    '   cb:
    '     The length, in bytes, of the range to restrict.
    '
    '   dwLockType:
    '     The access restrictions previously placed on the range.
    Public Sub UnlockRegion(ByVal libOffset As Long, ByVal cb As Long, ByVal dwLockType As Integer) _
        Implements IStream.UnlockRegion

        Throw New NotSupportedException("ReadOnlyIStreamWrapper does not support UnlockRegion")
    End Sub

    '
    ' Summary:
    '     Writes a specified number of bytes into the stream object starting at the
    '     current seek pointer.
    '
    ' Parameters:
    '   pv:
    '     The buffer to write this stream to.
    '
    '   cb:
    '     The number of bytes to write to the stream.
    '
    '   pcbWritten:
    '     On successful return, contains the actual number of bytes written to the
    '     stream object. If the caller sets this pointer to System.IntPtr.Zero, this
    '     method does not provide the actual number of bytes written.
    Public Sub Write(ByVal pv As Byte(), ByVal cb As Integer, ByVal pcbWritten As IntPtr) _
        Implements IStream.Write

        System.Runtime.InteropServices.Marshal.WriteInt64(pcbWritten, 0L)
        Me._stream.Write(pv, 0, cb)
        System.Runtime.InteropServices.Marshal.WriteInt64(pcbWritten, CLng(cb))
    End Sub
End Class
