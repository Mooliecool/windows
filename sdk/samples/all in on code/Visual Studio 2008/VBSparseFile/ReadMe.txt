========================================================================
    CONSOLE APPLICATION : VBSparseFile Project Overview
========================================================================

/////////////////////////////////////////////////////////////////////////////
Use:

VBSparseFile demonstrates the common operations on sparse files. A sparse 
file is a type of computer file that attempts to use file system space more 
efficiently when blocks allocated to the file are mostly empty. This is 
achieved by writing brief information (metadata) representing the empty 
blocks to disk instead of the actual "empty" space which makes up the block, 
using less disk space. You can find in this example the creation of sparse 
file, the detection of sparse attribute, the retrieval of sparse file size, 
and the query of sparse file layout.


/////////////////////////////////////////////////////////////////////////////
Code Logic:

A. Determine if the volume support sparse streams.

The Win32 API function GetVolumeInformation returns a set of file system 
flags that you can analyze to determine if the drive supports sparse streams. 

	' Get volume information
	Dim fileSystemFlags As FileSystemFeature
	Dim volumeSerialNumber As UInt32
	Dim maxComponentLength As UInt32
	NativeMethod.GetVolumeInformation(rootPathName, Nothing, _
		NativeMethod.MAX_PATH, volumeSerialNumber, maxComponentLength, _
		fileSystemFlags, Nothing, NativeMethod.MAX_PATH)

	Return ((fileSystemFlags And FileSystemFeature.SupportsSparseFiles) _
		= FileSystemFeature.SupportsSparseFiles)

B. Create a sparse file.

After creating a normal file, you must use the DeviceIoControl function with 
the FSCTL_SET_SPARSE control code to mark the file as sparse. Then you write 
blocks of data to the file and specify the region of data as sparse using 
the DeviceIoControl function with the FSCTL_SET_ZERO_DATA control code. The 
function also requires the starting and the ending address (not the size) of 
the sparse zero block. Note, however, that this operation does not perform 
actual file I/O, and unlike the WriteFile function, it does not move the 
current file I/O pointer or sets the end-of-file pointer. That is, if you 
want to place a sparse zero block in the end of the file, you must move the 
file pointer accordingly using the SetFilePointer function and call the 
SetEndOfFile function, otherwise DeviceIoControl will have no effect. 

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
		EIoControlCode.FsctlSetSparse, IntPtr.Zero, 0, IntPtr.Zero, 0, _
		bytesReturned, lpOverlapped)

C. Determine if a file is sparse.

In order to check if a file is sparse, use FileInfo.FileAttributes to check 
for FileAttributes.SparseFile.

	' Get file information
	Dim fileInfo As New FileInfo(fileName)

	' Check for sparse file
	Return ((fileInfo.Attributes And FileAttributes.SparseFile) = _
		FileAttributes.SparseFile)

D. Get file size.

Use the GetCompressedFileSize function to obtain the actual size allocated on 
disk for a sparse file. This total does not include the size of the regions 
which were deallocated because they were filled with zeroes. Use the 
FileInfo.Length property to determine the total size of a file, including the 
size of the sparse regions that were deallocated.

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

E. Query the sparse file layout.

You can specify the range to query in DeviceIoControl with the 
FSCTL_QUERY_ALLOCATED_RANGES control code, and provide a sufficient buffer 
for output info. The output contains the allocated (not sparse) regions in 
the sparse file.


/////////////////////////////////////////////////////////////////////////////
References:

MSDN: Sparse Files
http://msdn.microsoft.com/en-us/library/aa365564.aspx

NTFS Sparse Files with C#
http://blogs.msdn.com/codedebate/archive/2007/12/18/6797175.aspx

Technet: Fsutil sparse
http://technet.microsoft.com/en-us/library/cc788025.aspx

Sparse files
http://bartdesmet.net/blogs/bart/archive/2004/05/16/274.aspx


/////////////////////////////////////////////////////////////////////////////