========================================================================
    CONSOLE APPLICATION : VBPInvokeDll Project Overview
========================================================================

/////////////////////////////////////////////////////////////////////////////
Summary:

Platform Invocation Services (P/Invoke) in .NET allows managed code to call  
unmanaged functions that are implemented and exported in unmanaged DLLs. This 
VB.NET code sample demonstrates using P/Invoke to call the functions exported 
by the native DLLs: CppDynamicLinkLibrary.dll, user32.dll and msvcrt.dll.


/////////////////////////////////////////////////////////////////////////////
Sample Relation:

VBPInvokeDll -> CppDynamicLinkLibrary
VBPInvokeDll P/Ivnoke the functions exported by the native DLL 
CppDynamicLinkLibrary.


/////////////////////////////////////////////////////////////////////////////
Implementation:

A. P/Invoke functions exposed from a native C++ DLL module.

Step1. Declare the methods as having an implementation from a DLL export.
First, declare the method with the Shared VB.NET keyword. Next, Attach the 
DllImport attribute to the method. The DllImport attribute allows us to 
specify the name of the DLL that contains the method. The general practice 
is to name the VB.NET method the same as the exported method, but we can also 
use a different name for the VB.NET method. Specify custom marshaling 
information for the method's parameters and return value, which will override 
the .NET Framework default marshaling. 

For example, 

    <DllImport("CppDynamicLinkLibrary.dll", CharSet:=CharSet.Auto, _
        CallingConvention:=CallingConvention.Cdecl)> _
    Public Shared Function GetStringLength1(ByVal str As String) As Integer
    End Function

These tools can help you write the correct P/Invoke declartions.

	Dumpbin: View the export table of a DLL

	PInvoke.NET: PInvoke.net is primarily a wiki, allowing developers to find, 
	edit and add PInvoke* signatures, user-defined types, and any other info 
	related to calling Win32 and other unmanaged APIs from managed code such 
	as C# or VB.NET.
	
	PInvoke Interop Assistant: It is a toolkit that helps developers to 
	efficiently convert from C to managed P/Invoke signatures or verse visa. 

Step2. Calling the methods through the PInvoke signatures. For example:

	Dim str As String = "HelloWorld"
    Dim length As Integer
    length = NativeMethod.GetStringLength1(str)

B. P/Invoke C++ classes exposed from a native C++ DLL module.

There is no easy way to call the classes in a native C++ DLL module through 
P/Invoke. Visual C++ Team Blog introduced a solution, but it is complicated: 
http://go.microsoft.com/?linkid=9729423.

The recommended way of calling native C++ class from .NET are 

  1) use a C++/CLI class library to wrap the native C++ module, and your .NET 
     code class the C++/CLI wrapper class to indirectly access the native C++ 
     class.
   
  2) convert the native C++ module to be a COM server and expose the native 
     C++ class through a COM interface. Then, the .NET code can access the 
     class through .NET-COM interop.

C. Unload the native DLL module.

You can unload the DLL by first calling GetModuleHandle to get the handle of 
the module and then calling FreeLibrary to unload it. 

    <DllImport("kernel32.dll", CharSet:=CharSet.Auto, SetLastError:=True)> _
    Function GetModuleHandle(ByVal moduleName As String) As IntPtr
    End Function

    <DllImport("kernel32.dll", CharSet:=CharSet.Auto, SetLastError:=True)> _
    Function FreeLibrary(ByVal hModule As IntPtr) _
        As <MarshalAs(UnmanagedType.Bool)> Boolean
    End Function
    
    ' Unload the DLL by calling GetModuleHandle and FreeLibrary. 
    If (Not FreeLibrary(GetModuleHandle(moduleName))) Then
        Console.WriteLine("FreeLibrary failed w/err {0}", _
            Marshal.GetLastWin32Error())
    End If


/////////////////////////////////////////////////////////////////////////////
References:

Platform Invoke Tutorial
http://msdn.microsoft.com/en-us/library/aa288468.aspx

Using P/Invoke to Call Unmanaged APIs from Your Managed Classes
http://msdn.microsoft.com/en-us/library/aa719104.aspx

Calling Win32 DLLs in C# with P/Invoke
http://msdn.microsoft.com/en-us/magazine/cc164123.aspx

PInvoke.NET
http://www.pinvoke.net/

PInvoke Interop Assistant 
http://www.codeplex.com/clrinterop


/////////////////////////////////////////////////////////////////////////////
