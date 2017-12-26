================================================================================
       Windows APPLICATION: VBCheckProcessType Overview                        
===============================================================================

/////////////////////////////////////////////////////////////////////////////
Summary:
The sample demonstrates how to identify the process type in run time, including 
whether this process is a 64bit process, managed process, .NET 4.0 process, WPF 
process or console process.

NOTE:
1. This application must run on Windows Vista or later versions because the 
   EnumProcessModulesEx function is only available on these Windows versions.

2. On 64bit environment, the platform of this application must be Any CPU  or
   x64, because the EnumProcessModulesEx Function is intended primarily for 
   64-bit applications. 
  
   See: http://msdn.microsoft.com/en-us/library/ms682633(VS.85).aspx

////////////////////////////////////////////////////////////////////////////////
Demo:

Step1. Run VBCheckProcessType.exe.

Step2. Click the Refresh button, you will see all the processes in the data grid
       view.

/////////////////////////////////////////////////////////////////////////////
Code Logic:

1. Create the NativeMethods class to wrap the necessary Windows APIs in kernel32.dll
   and psapi.dll, including

   GetConsoleMode
   GetStdHandle
   AttachConsole
   FreeConsole
   IsWow64Process
   EnumProcessModulesEx
   GetModuleFileNameEx


2. Design the  RunningProcess class to wrap a System.Diagnostics.Process instance and
   check the process type.

   To determine whether a process is a 64bit process on x64 OS, we can use the Windows 
   API IsWow64Process Function
  
  
   To determine whether a process is a managed process, we can check whether the 
   .Net Runtime Execution engine MSCOREE.dll is loaded.
   
   To determine whether a process is a managed process, we can check whether the 
   CLR.dll is loaded. Before .Net 4.0, the workstation CLR runtime is called 
   MSCORWKS.DLL. In .Net 4.0, this DLL is replaced by CLR.dll. 
   
   To determine whether a process is a WPF process, we can check whether the 
   PresentationCore.dll is loaded.
   
   To determine whether a process is a console process, we can check whether
   the target process has a console window.

3. Design the MainForm to display all the running processes type.

/////////////////////////////////////////////////////////////////////////////
References:

GetConsoleMode Function
http://msdn.microsoft.com/en-us/library/ms683167(VS.85).aspx

GetStdHandle Function
http://msdn.microsoft.com/en-us/library/ms683231(VS.85).aspx

AttachConsole Function
http://msdn.microsoft.com/en-us/library/ms681952(VS.85).aspx

FreeConsole Function
http://msdn.microsoft.com/en-us/library/ms683150(VS.85).aspx

Determine Whether a Program Is a Console or GUI Application
http://www.devx.com/tips/Tip/33584

EnumProcessModulesEx Function
http://msdn.microsoft.com/en-us/library/ms682633(VS.85).aspx

GetModuleFileNameEx Function
http://msdn.microsoft.com/en-us/library/ms683198(VS.85).aspx

IsWow64Process Function
http://msdn.microsoft.com/en-us/library/ms684139(VS.85).aspx
/////////////////////////////////////////////////////////////////////////////
