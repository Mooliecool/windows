=============================================================================
      CONSOLE APPLICATION : VBCallNativeDllWrapper Project Overview
=============================================================================

/////////////////////////////////////////////////////////////////////////////
Summary:

The code sample demonstrates calling the functions and classes exported by a 
native C++ DLL module from VB.NET code through C++/CLI wrapper classes.

  VBCallNativeDllWrapper (this .NET application)
          -->
      CppCLINativeDllWrapper (the C++/CLI wrapper)
              -->
          CppDynamicLinkLibrary (a native C++ DLL module)


/////////////////////////////////////////////////////////////////////////////
Demo:

The following steps walk through a demonstration of the .NET-native interop 
sample.

Step1. After you successfully build the CppDynamicLinkLibrary, 
CppCLINativeDllWrapper, and VBCallNativeDllWrapper sample projects in Visual 
Studio 2008, you will get the applications: VBCallNativeDllWrapper.exe and 
two DLL files: CppCLINativeDllWrapper.dll and CppDynamicLinkLibrary.dll. 
Their relationship is that VBCallNativeDllWrapper.exe invokes 
CppCLINativeDllWrapper.dll, which further invokes functions and classes 
exported by CppDynamicLinkLibrary.dll.

Step2. Run VBCallNativeDllWrapper in a command prompt. The application should
print the following messages in the console. 

    Module "CppDynamicLinkLibrary" is loaded
    GetStringLength1("HelloWorld") => 10
    GetStringLength2("HelloWorld") => 10
    Max(2, 3) => 3
    Class: CSimpleObject::FloatProperty = 1.20
    Module "CppDynamicLinkLibrary" is loaded

The messages indicate that VBCallNativeDllWrapper successfuly loaded 
CppDynamicLinkLibrary.dll and invoked the functions (GetStringLength1, 
GetStringLength2, Max) and the class (CSimpleObject) exported by the native 
module.

NOTE: You may receive the following error if you run the debug build of the 
sample project on a system without the Visual Studio 2008 installed. 

    Unhandled Exception: System.IO.FileLoadException: Could not load file or 
    assembly 'CppCLINativeDllWrapper, Version=1.0.0.0, Culture=neutral, 
    PublicKeyToken=null' or one of its dependencies. This application has 
    failed to start because the application configuration is incorrect. 
    Reinstalling the application may fix this problem. (Exception from 
    HRESULT: 0x800736B1)
    File name: 'CppCLINativeDllWrapper, Version=1.0.0.0, Culture=neutral, 
    PublicKeyToken=null' ---> System.Runtime.InteropServices.COMException 
    (0x800736B1): This application has failed to start because the application 
    configuration is incorrect. Reinstalling the application may fix this 
    problem. (Exception from HRESULT: 0x800736B1)
       at CSCallNativeDllWrapper.Program.Main(String[] args)

This is caused by the fact the debug build of CppCLINativeDllWrapper and 
CppDynamicLinkLibrary depends on the Debug CRT which is only available in the 
development environments with Visual Studio 2008 installed. You must run the 
release build of the sample project in the non-development environment. 


/////////////////////////////////////////////////////////////////////////////
Sample Relation:
(The relationship between the current sample and the rest samples in 
Microsoft All-In-One Code Framework http://1code.codeplex.com)

VBCallNativeDllWrapper -> CppCLINativeDllWrapper -> CppDynamicLinkLibrary
The VB.NET sample application VBCallNativeDllWrapper calls the functions and 
classes exported by the native C++ DLL module CppDynamicLinkLibrary through 
the C++/CLI wrapper class library CppCLINativeDllWrapper.


/////////////////////////////////////////////////////////////////////////////
Implementation:

Step1. Reference the C++/CLI wrapper class library CppCLINativeDllWrapper in 
the VB.NET sample applicatino. CppCLINativeDllWrapper wraps the functions and 
classes exported by the native C++ DLL CppDynamicLinkLibrary.

Step2. Call the .NET classes CSimpleObjectWrapper and NativeMethods exposed 
by CppCLINativeDllWrapper to indirectly access the functions and classes 
exported by the native C++ DLL CppDynamicLinkLibrary. For example, 

    Dim obj As New CSimpleObjectWrapper
    obj.FloatProperty = 1.2F
    Dim fProp As Single = obj.FloatProperty
    Console.WriteLine("Class: CSimpleObject::FloatProperty = {0:F2}", fProp)


/////////////////////////////////////////////////////////////////////////////
References:

How to: Wrap Native Class for Use by C#
http://msdn.microsoft.com/en-us/library/ms235281.aspx


/////////////////////////////////////////////////////////////////////////////