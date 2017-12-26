=============================================================================
           CONSOLE APPLICATION : CSLoadLibrary Project Overview
=============================================================================

/////////////////////////////////////////////////////////////////////////////
Summary:

VBLoadLibrary in VB.NET mimics the behavior of CppLoadLibrary to 
dynamically load a native DLL (LoadLibrary) get the address of a function 
in the export table (GetProcAddress, Marshal.GetDelegateForFunctionPointer), 
and call it. The technology is called Dynamic P/Invoke. It serves as a 
supplement for the P/Invoke technique and is useful especially when the 
target DLL is not in the search path of P/Invoke. If you use P/Invoke, CLR 
will search the dll in your assembly's directory first, then search the dll 
in directories listed in PATH environment variable. If the dll is not in 
any of those directories, you have to use the so called Dynamic PInvoke 
technique that is demonstrated in this code sample. 


/////////////////////////////////////////////////////////////////////////////
Sample Relation:

VBLoadLibrary -> CppDynamicLinkLibrary
VBLoadLibrary, a .NET executable, dynamically loads the native DLL and uses
its symbols through the APIs: LoadLibrary, GetProcAddress and FreeLibrary.

VBLoadLibrary - CSPInvokeDll
VBLoadLibrary demonstrates the Dynamic PInvoke technique. It's useful 
especially when the target DLL is not in the search path of P/Invoke.

VBLoadLibrary - CSLoadLibrary - CppLoadLibrary
VBLoadLibrary (in VB.NET) and CSLoadLibrary (in C#) mimic the behavior of 
CppLoadLibrary to dynamically load a native DLL, get the address of a 
function in the export table, and call it.


/////////////////////////////////////////////////////////////////////////////
Implementation:

1. P/Invoke the API LoadLibrary to dynamically load a native DLL.

2. P/Invoke the API GetProcAddress to get the function pointer of a specified
function in the DLL's export table.

3. Call Marshal.GetDelegateForFunctionPointer to convert the function pointer
to a Delegate object.

4. Call the Delegate.

5. Call FreeLibrary on the unmanaged dll. 
(Be careful of calling kernel32!FreeLibrary from managed code! This is unsafe 
and can crash if done wrong.)


/////////////////////////////////////////////////////////////////////////////
References:

Type-safe Managed wrappers for kernel32!GetProcAddress
http://blogs.msdn.com/jmstall/archive/2007/01/06/Typesafe-GetProcAddress.aspx

Dynamic PInvoke
http://blogs.msdn.com/junfeng/archive/2004/07/14/181932.aspx


/////////////////////////////////////////////////////////////////////////////