========================================================================
    CONSOLE APPLICATION : CppSynchronousIO Project Overview
========================================================================

/////////////////////////////////////////////////////////////////////////////
Use:

CppSynchronousIO demonstrates the synchronous file I/O operations. In 
synchronous file I/O, a thread starts an I/O operation and immediately 
enters a wait state until the I/O request has completed. 


/////////////////////////////////////////////////////////////////////////////
Project Relation:

CppSynchronousIO - CppAsynchronousIO
CppSynchronousIO demonstrates synchronous I/O and CppAsynchronousIO shows 
asynchronous I/O operations.


/////////////////////////////////////////////////////////////////////////////
Code Logic:

1. Create/open a file for read and write without the FILE_FLAG_OVERLAPPED 
flag. If the FILE_FLAG_OVERLAPPED flag is specified, the system will think 
that you want to perform asynchronous I/O with the device. (CreateFile)

2. Synchronously write bytes to the file by calling WriteFile. The OVERLAPPED 
structure is optional for the API. Its return value indicates whether the 
synchronous write operation succeeds or not.

3. Synchronously read bytes from the file by calling ReadFile. The OVERLAPPED 
structure is optional for the API. Its return value indicates whether the 
synchronous read operation succeeds or not.

4. Pay attention to the current file pointer while reading and writing the 
file using the same file handle. Calling CreateFile causes the system to 
create a file kernel object that manages operations on the file. Inside this 
kernel object is a file pointer.  This file pointer indicates the 64-bit 
offset within the file where the next synchronous read or write should be 
performed. Initially, this file pointer is set to 0, so if you call ReadFile 
immediately after a call to CreateFile, you will start reading the file from 
offset 0. If you read 10 bytes of the file into memory, the system updates 
the pointer associated with the file handle so that the next call to ReadFile 
starts reading at the eleventh byte in the file at offset 10. To set the 
position of the current file pointer, use the SetFilePointerEx API.

5. Close the file. (CloseHandle)


/////////////////////////////////////////////////////////////////////////////
References:

MSDN: Synchronous and Asynchronous I/O
http://msdn.microsoft.com/en-us/library/aa365683.aspx 

MSDN: Opening a File for Reading or Writing
http://msdn.microsoft.com/en-us/library/bb540534.aspx

Windows via C/C++, Fifth Edition. Performing Synchronous Device I/O
http://msdn.microsoft.com/en-us/library/cc500402.aspx


/////////////////////////////////////////////////////////////////////////////
