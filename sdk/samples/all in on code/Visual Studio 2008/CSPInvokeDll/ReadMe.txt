=============================================================================
            CONSOLE APPLICATION : CSPInvokeDll Project Overview
=============================================================================

/////////////////////////////////////////////////////////////////////////////
Summary:

Platform Invocation Services (P/Invoke) in .NET allows managed code to call 
unmanaged functions that are implemented and exported in unmanaged DLLs. This 
VC# code sample demonstrates using P/Invoke to call the functions exported by 
the native DLLs: CppDynamicLinkLibrary.dll, user32.dll and msvcrt.dll.


/////////////////////////////////////////////////////////////////////////////
Sample Relation:

CSPInvokeDll -> CppDynamicLinkLibrary
CSPInvokeDll P/Ivnoke the functions exported by the native DLL 
CppDynamicLinkLibrary.


/////////////////////////////////////////////////////////////////////////////
Implementation:

A. P/Invoke functions exposed from a native C++ DLL module.

Step1. Declare the methods as having an implementation from a DLL export.
First, declare the method with the static and extern C# keywords. Next, 
attach the DllImport attribute to the method. The DllImport attribute allows 
us to specify the name of the DLL that contains the method. The general 
practice is to name the C# method the same as the exported method, but we can
also use a different name for the C# method. Specify custom marshaling 
information for the method's parameters and return value, which will override 
the .NET Framework default marshaling. 

For example, 

    [DllImport("CppDynamicLinkLibrary.dll", CharSet = CharSet.Auto,
        CallingConvention = CallingConvention.Cdecl)]
    internal static extern int GetStringLength1(string str);

These tools can help your write the correct P/Invoke declartions.

	Dumpbin: View the export table of a DLL

	PInvoke.NET: PInvoke.net is primarily a wiki, allowing developers to find, 
	edit and add PInvoke* signatures, user-defined types, and any other info 
	related to calling Win32 and other unmanaged APIs from managed code such 
	as C# or VB.NET.
	
	PInvoke Interop Assistant: It is a toolkit that helps developers to 
	efficiently convert from C to managed P/Invoke signatures or verse visa. 

Step2. Call the methods through the PInvoke signatures. For example:

	string str = "HelloWorld";
    int length;
    length = NativeMethod.GetStringLength1(str);

B. P/Invoke C++ classes exposed from a native C++ DLL module.

There is no easy way to call the classes in a native C++ DLL module through 
P/Invoke. Visual C++ Team Blog introduced a solution, but it is complicated: 
http://go.microsoft.com/?linkid=9729423.

The recommended way of calling native C++ class from .NET are:

  1) use a C++/CLI class library to wrap the native C++ module, and your .NET 
     code class the C++/CLI wrapper class to indirectly access the native C++ 
     class.

  2) convert the native C++ module to be a COM server and expose the native 
     C++ class through a COM interface. Then, the .NET code can access the 
     class through .NET-COM interop.

C. Unload the native DLL module.

You can unload the DLL by first calling GetModuleHandle to get the handle of 
the module and then calling FreeLibrary to unload it. 

    [DllImport("kernel32.dll", CharSet = CharSet.Auto, SetLastError = true)]
    static extern IntPtr GetModuleHandle(string moduleName);

    [DllImport("kernel32.dll", CharSet = CharSet.Auto, SetLastError = true)]
    [return: MarshalAs(UnmanagedType.Bool)]
    static extern bool FreeLibrary(IntPtr hModule);

    // Unload the DLL by calling GetModuleHandle and FreeLibrary. 
    if (!FreeLibrary(GetModuleHandle(moduleName)))
    {
        Console.WriteLine("FreeLibrary failed w/err {0}", 
        Marshal.GetLastWin32Error());
    }


/////////////////////////////////////////////////////////////////////////////
References:

MSDN: Platform Invoke Tutorial
http://msdn.microsoft.com/en-us/library/aa288468.aspx

MSDN: Using P/Invoke to Call Unmanaged APIs from Your Managed Classes
http://msdn.microsoft.com/en-us/library/aa719104.aspx

MSDN: Calling Win32 DLLs in C# with P/Invoke
http://msdn.microsoft.com/en-us/magazine/cc164123.aspx

PInvoke.NET
http://www.pinvoke.net/

PInvoke Interop Assistant 
http://www.codeplex.com/clrinterop


/////////////////////////////////////////////////////////////////////////////