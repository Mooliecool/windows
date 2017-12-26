========================================================================
    CONSOLE APPLICATION : CSFileMappingClient Project Overview
========================================================================

/////////////////////////////////////////////////////////////////////////////
Summary:

File mapping is a mechanism for one-way or duplex inter-process communication 
among two or more processes in the local machine. To share a file or memory, 
all of the processes must use the name or the handle of the same file mapping 
object.

To share a file, the first process creates or opens a file by using the 
CreateFile function. Next, it creates a file mapping object by using the 
CreateFileMapping function, specifying the file handle and a name for the 
file mapping object. The names of event, semaphore, mutex, waitable timer, 
job, and file mapping objects share the same name space. Therefore, the 
CreateFileMapping and OpenFileMapping functions fail if they specify a name
that is in use by an object of another type.

To share memory that is not associated with a file, a process must use the 
CreateFileMapping function and specify INVALID_HANDLE_VALUE as the hFile 
parameter instead of an existing file handle. The corresponding file mapping 
object accesses memory backed by the system paging file. You must specify 
a size greater than zero when you use an hFile of INVALID_HANDLE_VALUE in a 
call to CreateFileMapping.

Processes that share files or memory must create file views by using the 
MapViewOfFile or MapViewOfFileEx function. They must coordinate their access 
using semaphores, mutexes, events, or some other mutual exclusion technique.

The VC# code sample demonstrates opening a file mapping object named 
"Local\SampleMap" and reading the string written to the file mapping by other 
process. Because the Base Class Library of .NET Framework 2/3/3.5 does not 
have any public classes to operate on file mapping objects, you have to 
P/Invoke the Windows APIs as shown in this code sample.


/////////////////////////////////////////////////////////////////////////////
Demo:

The following steps walk through a demonstration of the file mapping sample.

Step1. After you successfully build the CSFileMappingClient and 
CSFileMappingServer sample projects in Visual Studio 2008, you will get the 
applications: CSFileMappingClient.exe and CSFileMappingServer.exe. 

Step2. Run CSFileMappingServer.exe in a command prompt. The application will 
create a file mapping object of a specified size that is backed by the system 
paging file. Its name is "Local\SampleMap".

  The file mapping (Local\SampleMap) is created

Next, the application maps a view of the file mapping into the address space 
of the process, and writes a string to the view.

  The file view is mapped
  This message is written to the view:
  "Message from the first process."

Step3. Run CSFileMappingClient.exe in another command prompt. 
CSFileMappingClient opens the file mapping object "Local\SampleMap", maps 
the same view of the file mapping into its address space, and read the string 
written by the first process from the view.

  The file mapping (Local\SampleMap) is opened
  The file view is mapped
  Read from the file mapping:
  "Message from the first process."

Step4. Press ENTER in both command prompts to close CSFileMappingServer and 
CSFileMappingClient.


/////////////////////////////////////////////////////////////////////////////
Sample Relation:
(The relationship between the current sample and the rest samples in 
Microsoft All-In-One Code Framework http://1code.codeplex.com)

CSFileMappingClient -> CSFileMappingServer
CSFileMappingServer creates the file mapping named "Local\SampleMap" and 
writes a string to it. CSFileMappingClient reads the string from the file 
mapping.


/////////////////////////////////////////////////////////////////////////////
Code Logic:

1. Try to open the file mapping object "Local\SampleMap" by P/Invoking 
OpenFileMapping.

2. Map a view of the file mapping into the address space of the current 
process by P/Invoking MapViewOfFile.

3. Read a string from the view.

4. Unmap the file view (UnmapViewOfFile) and close the file mapping object 
(CloseHandle).


/////////////////////////////////////////////////////////////////////////////
References:

MSDN: Creating Named Shared Memory
http://msdn.microsoft.com/en-us/library/aa366551.aspx


/////////////////////////////////////////////////////////////////////////////
