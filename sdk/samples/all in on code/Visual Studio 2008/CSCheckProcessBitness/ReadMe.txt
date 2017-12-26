=============================================================================
       CONSOLE APPLICATION : CSCheckProcessBitness Project Overview
=============================================================================

/////////////////////////////////////////////////////////////////////////////
Summary:

The code sample demonstrates how to determine whether the given process is
a 64-bit process or not.


/////////////////////////////////////////////////////////////////////////////
Demo:

The following steps walk through a demonstration of the sample.

Step1. After you successfully build the sample project in Visual Studio 2008 
targeting "Any CPU" platform, you will get an application: 
CSCheckProcessBitness.exe. 

Step2. Run the application in a command prompt (cmd.exe) on a 64-bit 
operating system (e.g. Windows 7 x64). The application prints the following 
content in the command prompt:

  Current process is 64-bit
  
It indictates that the current process is a 64-bit process.

Step3. If you specify the process ID of a 32-bit process as the argument of 
CSCheckProcessBitness.exe (e.g. CSCheckProcessBitness 987), the application 
will print the following content in the command prompt:
  
  Process XXX is not 64-bit

It indictates that the given process is not a 64-bit process.


/////////////////////////////////////////////////////////////////////////////
Implementation:

If you are determining whether the currently running process is a 64-bit 
process, you can check the size of a pointer.

    static bool Is64BitProcess()
    {
        return (IntPtr.Size == 8);
    }

If you are detecting whether an arbitrary application running on the system 
is a 64-bit process, you need to determine the OS bitness and if it is 64-bit, 
call IsWow64Process() with the target process handle.

    static bool Is64BitProcess(IntPtr hProcess)
    {
        bool flag = false;

        if (Is64BitOS())
        {
            // On 64-bit OS, if a process is not running under Wow64 mode, 
            // the process must be a 64-bit process.
            flag = !(NativeMethods.IsWow64Process(hProcess, out flag) && flag);
        }

        return flag;
    }


/////////////////////////////////////////////////////////////////////////////
References:

How to detect programmatically whether you are running on 64-bit Windows
http://blogs.msdn.com/oldnewthing/archive/2005/02/01/364563.aspx


/////////////////////////////////////////////////////////////////////////////