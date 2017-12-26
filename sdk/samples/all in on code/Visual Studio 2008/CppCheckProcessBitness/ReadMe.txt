=============================================================================
       CONSOLE APPLICATION : CppCheckProcessBitness Project Overview
=============================================================================

/////////////////////////////////////////////////////////////////////////////
Summary:

The code sample demonstrates how to determine whether the given process is
a 64-bit process or not.


/////////////////////////////////////////////////////////////////////////////
Demo:

The following steps walk through a demonstration of the sample.

Step1. After you successfully build the sample project in Visual Studio 2008
targeting the *x64* platform, you will get an application: 
CppCheckProcessBitness.exe. 

Step2. Run the application in a command prompt (cmd.exe) on a 64bit 
operating system (e.g. Windows 7 x64). The application prints the following 
content in the command prompt:

  Current process is 64-bit

It indictates that the current process is a 64-bit process.

Step3. If you specify the process ID of a 32bit process as the argument of 
CppCheckProcessBitness.exe (e.g. CppCheckProcessBitness 987), the application 
will print the following content in the command prompt:

  Process XXX is not 64-bit

It indictates that the given process is not a 64-bit process.


/////////////////////////////////////////////////////////////////////////////
Implementation:

If you are determining whether the currently running process is a 64-bit 
process, you can use the VC++ preprocessor symbols which get set during the 
build to simply return true/false.

    BOOL Is64BitProcess(void)
    {
    #if defined(_WIN64)
        return TRUE;   // 64-bit program
    #else
        return FALSE;
    #endif
    }

If you are detecting whether an arbitrary application running on the system 
is a 64-bit process, you need to determine the OS bitness and if it is 64-bit, 
call IsWow64Process() with the target process handle.

    BOOL Is64BitProcess(HANDLE hProcess)
    {
	    BOOL f64bitProc = FALSE;

        if (Is64BitOS())
	    {
		    // On 64bit OS, if a process is not running under Wow64 mode, the 
            // process must be a 64bit process.
            f64bitProc = !(SafeIsWow64Process(hProcess, &f64bitProc) && f64bitProc);
	    }

	    return f64bitProc;
    }


/////////////////////////////////////////////////////////////////////////////
References:

How to detect programmatically whether you are running on 64-bit Windows
http://blogs.msdn.com/oldnewthing/archive/2005/02/01/364563.aspx


/////////////////////////////////////////////////////////////////////////////