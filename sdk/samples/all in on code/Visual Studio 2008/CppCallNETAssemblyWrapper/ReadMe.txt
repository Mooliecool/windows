=============================================================================
      CONSOLE APPLICATION : CppCallNETAssemblyWrapper Project Overview
=============================================================================

/////////////////////////////////////////////////////////////////////////////
Summary:

The code sample demonstrates calling the .NET classes defined in a managed 
assembly from a native C++ application through C++/CLI wrapper classes.

  CppCallNETAssemblyWrapper (this native C++ application)
          -->
      CppCLINETAssemblyWrapper (the C++/CLI wrapper)
              -->
          CSClassLibrary (a .NET assembly)


/////////////////////////////////////////////////////////////////////////////
Demo:

The following steps walk through a demonstration of the .NET-native interop 
sample.

Step1. After you successfully build the CSClassLibrary, 
CppCLINETAssemblyWrapper, and CppCallNETAssemblyWrapper sample projects in 
Visual Studio 2008, you will get the application: 
CppCallNETAssemblyWrapper.exe and two DLL files: CppCLINETAssemblyWrapper.dll 
and CSClassLibrary.dll. Their relationship is that CppCallNETAssemblyWrapper 
invokes CppCLINETAssemblyWrapper.dll, which further invokes the public class 
defined in CSClassLibrary.dll.

Step2. Run CppCallNETAssemblyWrapper in a command prompt. The application 
should print the following messages in the console. 

    Module "CSClassLibrary" is not loaded
    Class: CSSimpleObject::FloatProperty = 1.20
    Class: CSSimpleObject::ToString => "1.20"
    Module "CSClassLibrary" is loaded

The messages indicate that the .NET assembly CSClassLibrary is not loaded 
until CppCallNETAssemblyWrapper invokes a symbol contained in the DLL. 
Through the C++/CLI wrapper class CSSimpleObjectWrapper defined in 
CppCLINETAssemblyWrapper.dll, CppCallNETAssemblyWrapper created an instance 
of the CSSimpleObject class defined in CSClassLibrary, and invoked its 
property FloatProperty and the method ToString. 


/////////////////////////////////////////////////////////////////////////////
Sample Relation:
(The relationship between the current sample and the rest samples in 
Microsoft All-In-One Code Framework http://1code.codeplex.com)

CppCallNETAssemblyWrapper -> CppCLINETAssemblyWrapper -> CSClassLibrary
The native C++ sample application CppCallNETAssemblyWrapper calls the .NET 
class defined in the C# class library CSClassLibrary through the C++/CLI 
wrapper CppCLINETAssemblyWrapper.


/////////////////////////////////////////////////////////////////////////////
Implementation:

Step1. Implicitly link to the C++/CLI class library CppCLINETAssemblyWrapper 
that exports the wrapper class CSSimpleObjectWrapper.

  First, you need to link the LIB file of the DLL by entering the LIB file 
  name in Project Properties / Linker / Input / Additional Dependencies. You 
  can configure the search path of the LIB file in Project Properties / 
  Linker / General / Additional Library Directories.
  
  Next, include the header file that declares the wrapper class.
  
    #include "CppCLINETAssemblyWrapper.h"
  
  You can configure the search path of the header file in Project Properties /
  C/C++ / General / Additional Include Directories.

Step2. Call the wrapper class CSSimpleObjectWrapper exported by 
CppCLINETAssemblyWrapper to indirectly access the .NET class CSSimpleObject 
defined in the C# class library CSClassLibrary. For example, 

    CSSimpleObjectWrapper obj;

    obj.set_FloatProperty(1.2F);
    float fProp = obj.get_FloatProperty();
    wprintf(L"Class: CSSimpleObject::FloatProperty = %.2f\n", fProp);


/////////////////////////////////////////////////////////////////////////////
References:

Using C++ Interop (Implicit PInvoke)
http://msdn.microsoft.com/en-us/library/2x8kf7zx.aspx


/////////////////////////////////////////////////////////////////////////////
