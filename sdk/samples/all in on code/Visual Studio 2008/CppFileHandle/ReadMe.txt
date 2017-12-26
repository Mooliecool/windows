========================================================================
    CONSOLE APPLICATION : CppFileHandle Project Overview
========================================================================

/////////////////////////////////////////////////////////////////////////////
Use:

CppFileHandle demonstrates two typical scenarios of using file handles:

1) Enumerate file handles of a process
2) Get file name from a file handle


/////////////////////////////////////////////////////////////////////////////
Code Logic:

A. Enumerate file handles of a process

It requires using some undocumented APIs to enumerate file handles of a 
process. Because these APIs and structs are internal to the operating system 
and subject to change from one release of Windows to another, to maintain the 
compatibility of your application, it is better not to use them.

1. Prepare for NtQuerySystemInformation and NtQueryInformationFile.
The functions have no associated import library. You must use the 
LoadLibrary and GetProcAddress functions to dynamically link to ntdll.dll.

2. Get system handle information. (NtQuerySystemInformation, 
SystemHandleInformation, SYSTEM_HANDLE_INFORMATION)
NtQuerySystemInformation does not return the correct required buffer size if 
the buffer passed is too small. Instead you must call the function while 
increasing the buffer size until the function no longer returns 
STATUS_INFO_LENGTH_MISMATCH.

3. Enumerate file handles of the process. (SYSTEM_HANDLE_INFORMATION, 
HANDLE_TYPE_FILE, DuplicateHandle, NtQueryInformationFile)
Because handle is meaningful only to its hosting process, we need to 
duplicate other process's handle to the current process (DuplicateHandle) so 
as to further query the file information of the file object. 
NtQueryInformationFile is used to retrieve file name information.

B. Get file name from file handle

1. Create a file mapping object (CreateFileMapping, MapViewOfFile).

2. Call the GetMappedFileName function to obtain the file name. File name 
returned by GetMappedFileName contains device file name like:

	\Device\HarddiskVolume2\Users\JLG\AppData\Local\Temp\HLe6098.tmp

To translate path with device name to drive letters, we need to enumerate 
logical drive letters (GetLogicalDriveStrings), and get the device names 
corresponding to the driver letters, then compare the device names with the 
name returned by GetMappedFileName. If the match is found, replace the device 
name in the file path with drive letter.


/////////////////////////////////////////////////////////////////////////////
References:

MSDN: Obtaining a File Name From a File Handle
http://msdn.microsoft.com/en-us/library/aa366789.aspx


/////////////////////////////////////////////////////////////////////////////
