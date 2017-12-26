=============================================================================
       CONSOLE APPLICATION : CppImplicitlyLinkDll Project Overview
=============================================================================

/////////////////////////////////////////////////////////////////////////////
Summary:

Normally, when we link to a DLL via a LIB file, the DLL is loaded when the  
application starts up. This kind of loading is kwown as implicit linking,  
because the system takes care of loading the DLL for us - all we need to do 
is link with the LIB file.

After the configuration of linking, we can import symbols of a DLL into the 
application using the keyword __declspec(dllimport) no matter whether the 
symbols were exported with __declspec(dllexport) or with a .def file.

This sample demonstrates implicitly linking CppDynamicLinkLibrary.dll and 
importing and using its symbols.


/////////////////////////////////////////////////////////////////////////////
Sample Relation:

CppImplicitlyLinkDll -> CppDynamicLinkLibrary
CppImplicitlyLinkDll implicitly links (staticly loads) the 
CppDynamicLinkLibrary.dll and uses its symbols.


/////////////////////////////////////////////////////////////////////////////
Implementation:

Step1. Reference the Dynamic Link Library in your C++ project.

  Option1. Link the LIB file of the DLL by entering the LIB file name in 
  Project Properties / Linker / Input / Additional Dependencies. You can 
  configure the search path of the LIB file in Project Properties / Linker / 
  General / Additional Library Directories.

  Option2. Select References from the Project's shortcut menu. On the 
  Property Pages dialog box, expand the Common Properties node, select 
  References, and then select the Add New Reference... button. The Add 
  Reference dialog box is displayed. This dialog lists all the libraries that 
  you can reference. The Projects tab lists all the projects in the current 
  solution and any libraries they contain. If the CppDynamicLinkLibrary 
  project is in the current solution, select CppDynamicLinkLibrary and click 
  OK in the Projects tab.

Step2. Include the header file that declares the functions and classes of the 
DLL.

    #include "CppDynamicLinkLibrary.h"

You can configure the search path of the header file in Project Properties / 
C/C++ / General / Additional Include Directories.

Step3. Use the imported symbols.

For example:

    PWSTR pszString = L"HelloWorld";
    int nLength;
    nLength = GetStringLength1(pszString);


/////////////////////////////////////////////////////////////////////////////
References:

MSDN: Importing into an Application
http://msdn.microsoft.com/en-us/library/kh1zw7z7.aspx

MSDN: Creating and Using a Dynamic Link Library (C++)
http://msdn.microsoft.com/en-us/library/ms235636.aspx


/////////////////////////////////////////////////////////////////////////////