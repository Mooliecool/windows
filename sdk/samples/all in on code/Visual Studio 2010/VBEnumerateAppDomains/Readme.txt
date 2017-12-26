=============================================================================
       Windows APPLICATION: VBEnumerateAppDomains Overview                        
=============================================================================

/////////////////////////////////////////////////////////////////////////////
Summary:

The sample demonstrates how to enumerate the managed processes and AppDomains 
using hosting API and debugging API. All these APIs are unmanaged, but 
MdbgCore.dll and mscoree wrapped them.

To identify whether a process is a managed process, you can check whether it 
loads CLRs using hosting API. If you want to enumerate the AppDomains in the 
process, attach a debugger to the process using debugging API.

Notice that 
1. You cannot debug your own process, so if you want to enumerate the 
   AppDomains in the current process, you can use ICorRuntimeHost.
2. If you want to enumerate x86 managed processes in a 64bit OS, you have to 
   set the platform of this application to x86.
3. Some processes cannot be attached because 
   3.1 The processes have already been attached, like *.exe.vshost.
   3.2 The processes are not in Synchronized state. Some steps of the Attach 
       operation require that the processes should be in Synchronized state. 
       See http://msdn.microsoft.com/en-us/library/ms404528.aspx
   
/////////////////////////////////////////////////////////////////////////////
Demo:
Step1. Build the sample project in Visual Studio 2010.

Step2. Run VBEnumerateAppDomains.exe. This application will show following 
help text. 

    Please choose a command:
    1: Show AppDomains in current process.
    2: List all managed processes.
    3: Show help text.
    4: Exit this application.
    To show the AppDomains in a specified process, please type ""PID"" and
    the ID of the process directly, like PID1234.

Step3. Type 1 and press Enter, you can see all AppDomains in current process.

Step4. Type 2 and press Enter, you can see all managed processes that are 
running.

Step5. Type PIDxxxx(xxxx is a process ID that you get in Step4, like PID1234), 
you will see all AppDomains in the spcified process.

You can use following code to create a AppDomain in an application which is 
to test. 

    var newDomain = AppDomain.CreateDomain("Hello World!");


/////////////////////////////////////////////////////////////////////////////
Code Logic:

First, copy mdbgcore.dll to the _External_Dependencies folder and add the 
assembly to the project references.

This assembly is a part of Windows SDK. If you installed VS2010, the assembly 
is under 

C:\Program Files\Microsoft SDKs\Windows\v7.0A\Bin\ (32 bit OS) or
C:\Program Files (x86)\Microsoft SDKs\Windows\v7.0A\Bin\x64(64 bit OS). 

You can also download the latest Windows SDK in following link. 
http://www.microsoft.com/downloads/en/details.aspx?FamilyID=35AEDA01-421D-4BA5-B44B-543DC8C33A20
 
Second, add a COM reference: Common Language Runtime Execution Engine 2.4 
Library.

Third, this application supports 2 ways to start. One as demo, the other is 
to start with a argument, like 

VBEnumerateAppDomains.exe CurrentProcess
VBEnumerateAppDomains.exe ListAllManagedProcesses
VBEnumerateAppDomains.exe PID1234.

Forth, use mscoree.ICorRuntimeHost to show AppDomains in current process.

Fifth, use CLRMetaHost to enumerate the loaded runtimes of a process. If a 
process loads one or more Common Language Runtimes, it could be considered as 
a managed process.

At last, use MDbgEngine to attach a CorDebugger to the specified process, and 
then enumerate the AppDomains.


/////////////////////////////////////////////////////////////////////////////
References:

MSDN: ICorRuntimeHost Interface
http://msdn.microsoft.com/en-us/library/ms164320.aspx

MSDN: ICLRMetaHost Interface
http://msdn.microsoft.com/en-us/library/dd233134.aspx

MSDN: Debugging (Unmanaged API Reference)
http://msdn.microsoft.com/en-us/library/ms404520.aspx


/////////////////////////////////////////////////////////////////////////////
