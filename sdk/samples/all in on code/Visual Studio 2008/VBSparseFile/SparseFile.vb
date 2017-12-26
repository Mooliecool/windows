'****************************** Module Header ******************************'
' Module Name:  SparseFile.vb
' Project:      VBSparseFile
' Copyright (c) Microsoft Corporation.
' 
' SparseFile encapsulates the common operations on sparse files.
' 
' This source is subject to the Microsoft Public License.
' See http://www.microsoft.com/opensource/licenses.mspx#Ms-PL.
' All other rights reserved.
' 
' History:
' * 7/7/2009 11:34 PM Jialiang Ge Created
'***************************************************************************'

#Region "Imports directives"

Imports System.IO
Imports System.Runtime.InteropServices
Imports System.Threading
Imports Microsoft.Win32.SafeHandles

#End Region


Public Class SparseFile

    ''' <summary>
    ''' VolumeSupportsSparseFiles determines if the volume supports sparse 
    ''' streams.
    ''' </summary>
    ''' <param name="rootPathName">Volume root path e.g. C:\</param>
    ''' <returns></returns>
    ''' <remarks></remarks>
    Public Shared Function VolumeSupportsSparseFiles( _
    ByVal rootPathName As String) As Boolean

        ' Get volume information
        Dim fileSystemFlags As FileSystemFeature
        Dim volumeSerialNumber As UInt32
        Dim maxComponentLength As UInt32
        NativeMethod.GetVolumeInformation(rootPathName, Nothing, _
                                          NativeMethod.MAX_PATH, _
                                          volumeSerialNumber, _
                                          maxComponentLength, _
                                          fileSystemFlags, Nothing, _
                                          NativeMethod.MAX_PATH)

        Return ((fileSystemFlags And FileSystemFeature.SupportsSparseFiles) _
                = FileSystemFeature.SupportsSparseFiles)

    End Function


    ''' <summary>
    ''' Determine if a file is sparse.
    ''' </summary>
    ''' <param name="fileName">File name</param>
    ''' <returns></returns>
    ''' <remarks></remarks>
    Public Shared Function IsSparseFile(ByVal fileName As String) As Boolean

        ' Get file information
        Dim fileInfo As New FileInfo(fileName)

        ' Check for sparse file
        Return ((fileInfo.Attributes And FileAttributes.SparseFile) = _
                FileAttributes.SparseFile)

    End Function


    ''' <summary>
    ''' Get sparse file sizes.
    ''' </summary>
    ''' <param name="fileName">File name</param>
    ''' <remarks>
    ''' http://msdn.microsoft.com/en-us/library/aa365276.aspx
    ''' </remarks>
    Public Shared Sub GetSparseFileSize(ByVal fileName As String)

        ' Retrieves the size of the specified file, in bytes. The size 
        ' includes both allocated ranges and sparse ranges.
        Dim fileInfo As New FileInfo(fileName)
        Dim sparseFileSize As Long = fileInfo.Length

        ' Retrieves the file's actual size on disk, in bytes. The size 
        ' does not include the sparse ranges.
        Dim high As UInt32
        Dim low As UInt32 = NativeMethod.GetCompressedFileSize(fileName, high)
        Dim sparseFileCompressedSize As UInt64 = ((high << &H20) + low)

        ' Print the result
        Console.WriteLine(ChrW(10) & "File total size: {0}KB", _
                          (sparseFileSize / &H400))
        Console.WriteLine("Actual size on disk: {0}KB", _
                          (sparseFileCompressedSize / CULng(&H400)))

    End Sub


    ''' <summary>
    ''' Create a sparse file.
    ''' </summary>
    ''' <param name="fileName">The name of the sparse file</param>
    ''' <returns></returns>
    ''' <remarks></remarks>
    Public Shared Function Create(ByVal fileName As String) As FileStream

        ' Create a normal file
        Dim fs As New FileStream(fileName, FileMode.Create, FileAccess.Write)

        ' Use the DeviceIoControl function with the FSCTL_SET_SPARSE control 
        ' code to mark the file as sparse. If you don't mark the file as 
        ' sparse, the FSCTL_SET_ZERO_DATA control code will actually write 
        ' zero bytes to the file instead of marking the region as sparse 
        ' zero area.
        Dim bytesReturned As Integer = 0
        Dim lpOverlapped As New NativeOverlapped
        NativeMethod.DeviceIoControl(fs.SafeFileHandle, _
                                     EIoControlCode.FsctlSetSparse, _
                                     IntPtr.Zero, 0, IntPtr.Zero, 0, _
                                     bytesReturned, lpOverlapped)

        Return fs

    End Function


    ''' <summary>
    ''' Converting a file region to A sparse zero area.
    ''' </summary>
    ''' <param name="hSparseFile">Safe handle of the sparse file</param>
    ''' <param name="start">Start address of the sparse zero area</param>
    ''' <param name="size">
    ''' Size of the sparse zero block. The minimum sparse size is 64KB.
    ''' </param>
    ''' <remarks>
    ''' Note that SetSparseRange does not perform actual file I/O, and unlike 
    ''' the WriteFile function, it does not move the current file I/O pointer 
    ''' or sets the end-of-file pointer. That is, if you want to place a 
    ''' sparse zero block in the end of the file, you must move the file 
    ''' pointer accordingly using the FileStream.Seek function, otherwise 
    ''' DeviceIoControl will have no effect. 
    ''' </remarks>
    Public Shared Sub SetSparseRange(ByVal hSparseFile As SafeFileHandle, _
                                     ByVal start As UInt32, _
                                     ByVal size As UInt32)

        ' Specify the starting and the ending address (not the size) of the 
        ' sparse zero block
        Dim fzdi As FILE_ZERO_DATA_INFORMATION
        fzdi.FileOffset = start
        fzdi.BeyondFinalZero = (start + size)
        Dim hfzdi As GCHandle = GCHandle.Alloc(fzdi, GCHandleType.Pinned)

        ' Mark the range as sparse zero block
        Dim bytesReturned As Integer = 0
        Dim lpOverlapped As New NativeOverlapped
        NativeMethod.DeviceIoControl(hSparseFile, _
                                     EIoControlCode.FsctlSetZeroData, _
                                     hfzdi.AddrOfPinnedObject, _
                                     Marshal.SizeOf(fzdi), IntPtr.Zero, 0, _
                                     bytesReturned, lpOverlapped)

        hfzdi.Free()

    End Sub


    ''' <summary>
    ''' Query the sparse file layout.
    ''' </summary>
    ''' <param name="fileName">File name</param>
    ''' <returns></returns>
    ''' <remarks></remarks>
    Public Shared Function GetSparseRanges(ByVal fileName As String) As Boolean

        ' Open the file for read
        Using fs As FileStream = New FileStream(fileName, FileMode.Open, _
                                                FileAccess.Read)

            ' Set the range to be examined (the whole file)
            Dim queryRange As FILE_ALLOCATED_RANGE_BUFFER
            queryRange.FileOffset = 0
            queryRange.Length = fs.Length
            Dim hQueryRange As GCHandle = GCHandle.Alloc(queryRange, _
                                                         GCHandleType.Pinned)

            ' Allocated areas info
            ' DeviceIoControl will return as many results as fit into this 
            ' buffer and will report error code ERROR_MORE_DATA as long as 
            ' more data is available
            Dim allocRanges As FILE_ALLOCATED_RANGE_BUFFER() = _
            New FILE_ALLOCATED_RANGE_BUFFER(&H400 - 1) {}
            Dim hAllocRanges As GCHandle = GCHandle.Alloc(allocRanges, _
                                                          GCHandleType.Pinned)

            Dim nbytes As Integer = 0
            Dim bFinished As Boolean = False
            Console.WriteLine(ChrW(10) & "Allocated ranges in the file:")
            Do
                Dim lpOverlapped As New NativeOverlapped
                bFinished = NativeMethod.DeviceIoControl( _
                fs.SafeFileHandle, EIoControlCode.FsctlQueryAllocatedRanges, _
                hQueryRange.AddrOfPinnedObject, Marshal.SizeOf(queryRange), _
                hAllocRanges.AddrOfPinnedObject, _
                Marshal.SizeOf(GetType(FILE_ALLOCATED_RANGE_BUFFER)) * &H400, _
                nbytes, (lpOverlapped))

                If Not bFinished Then
                    Dim err As Integer = Marshal.GetLastWin32Error

                    ' ERROR_MORE_DATA is the only error that is normal
                    If (err <> NativeMethod.ERROR_MORE_DATA) Then
                        Console.WriteLine("DeviceIoControl failed w/err 0x{0:X}", _
                                          err)
                        Return False
                    End If

                End If

                ' Calculate the number of records returned
                Dim allocRangeCount As Integer = nbytes / Marshal.SizeOf( _
                GetType(FILE_ALLOCATED_RANGE_BUFFER))

                ' Print each allocated range
                For i As Integer = 0 To allocRangeCount - 1
                    Console.WriteLine("allocated range: {0} {1}", _
                                      allocRanges(i).FileOffset, _
                                      allocRanges(i).Length)
                Next i

                ' Set starting address and size for the next query
                If Not (bFinished OrElse (allocRangeCount <= 0)) Then

                    queryRange.FileOffset = _
                    allocRanges(allocRangeCount - 1).FileOffset + _
                    allocRanges((allocRangeCount - 1)).Length

                    queryRange.Length = fs.Length - queryRange.FileOffset

                End If

            Loop While Not bFinished

            ' Release the pinned GC handles
            hAllocRanges.Free()
            hQueryRange.Free()

        End Using

        Return True

    End Function

End Class
