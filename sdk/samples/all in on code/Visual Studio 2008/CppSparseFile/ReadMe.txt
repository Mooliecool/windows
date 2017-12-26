========================================================================
    CONSOLE APPLICATION : CppSparseFile Project Overview
========================================================================

/////////////////////////////////////////////////////////////////////////////
Use:

CppSparseFile demonstrates the common operations on sparse files. A sparse 
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

	DWORD dwVolFlags;
	GetVolumeInformation(lpRootPathName, NULL, MAX_PATH, NULL, NULL, 
		&dwVolFlags, NULL, MAX_PATH);

	return (dwVolFlags & FILE_SUPPORTS_SPARSE_FILES) ? TRUE : FALSE;

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

	// Create a normal file
	HANDLE hSparseFile = CreateFile(lpFileName, GENERIC_WRITE, 0, NULL, 
		CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);

	if (hSparseFile == INVALID_HANDLE_VALUE)
		return hSparseFile;

	// Use the DeviceIoControl function with the FSCTL_SET_SPARSE control 
	// code to mark the file as sparse. If you don't mark the file as sparse, 
	// the FSCTL_SET_ZERO_DATA control code will actually write zero bytes to 
	// the file instead of marking the region as sparse zero area.
	DWORD dwTemp;
	DeviceIoControl(hSparseFile, FSCTL_SET_SPARSE, NULL, 0, NULL, 0, &dwTemp, 
		NULL);

C. Determine if a file is sparse.

In order to check if a file is sparse, use GetFileAttributes to check for 
the FILE_ATTRIBUTE_SPARSE_FILE attribute.

	// Open the file for read
	HANDLE hFile = CreateFile(lpFileName, GENERIC_READ, 0, NULL, 
		OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	if (hFile == INVALID_HANDLE_VALUE)
		return FALSE;

	// Get file information
	BY_HANDLE_FILE_INFORMATION bhfi;
	GetFileInformationByHandle(hFile, &bhfi);
	CloseHandle(hFile);

	return (bhfi.dwFileAttributes & FILE_ATTRIBUTE_SPARSE_FILE) 
		? TRUE : FALSE;

D. Get file size.

Use the GetCompressedFileSize function to obtain the actual size allocated on 
disk for a sparse file. This total does not include the size of the regions 
which were deallocated because they were filled with zeroes. Use the 
GetFileSizeEx function to determine the total size of a file, including the 
size of the sparse regions that were deallocated.

	// Retrieves the size of the specified file, in bytes. The size includes 
	// both allocated ranges and sparse ranges.
	HANDLE hFile = CreateFile(lpFileName, GENERIC_READ, 0, NULL, 
		OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	if (hFile == INVALID_HANDLE_VALUE)
		return FALSE;	
	LARGE_INTEGER liSparseFileSize;
	GetFileSizeEx(hFile, &liSparseFileSize);
	
	// Retrieves the file's actual size on disk, in bytes. The size does not 
	// include the sparse ranges.
	LARGE_INTEGER liSparseFileCompressedSize;
	liSparseFileCompressedSize.LowPart = GetCompressedFileSize(lpFileName, 
		(LPDWORD)&liSparseFileCompressedSize.HighPart);

	// Print the result
	wprintf(L"\nFile total size: %I64uKB\nActual size on disk: %I64uKB\n", 
		liSparseFileSize.QuadPart / 1024, 
		liSparseFileCompressedSize.QuadPart / 1024);

	CloseHandle(hFile);

E. Query the sparse file layout.

You can specify the range to query in DeviceIoControl with the 
FSCTL_QUERY_ALLOCATED_RANGES control code, and provide a sufficient buffer 
for output info. The output contains the allocated (not sparse) regions in 
the sparse file.


/////////////////////////////////////////////////////////////////////////////
References:

MSDN: Sparse Files
http://msdn.microsoft.com/en-us/library/aa365564.aspx

NTFS Sparse Files For Programmers
http://www.flexhex.com/docs/articles/sparse-files.phtml

Technet: Fsutil sparse
http://technet.microsoft.com/en-us/library/cc788025.aspx


/////////////////////////////////////////////////////////////////////////////
