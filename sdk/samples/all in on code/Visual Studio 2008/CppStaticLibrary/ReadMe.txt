========================================================================
    STATIC LIBRARY : CppStaticLibrary Project Overview
========================================================================

/////////////////////////////////////////////////////////////////////////////
Use:

This example demonstrates exporting functions and classes for use in C or C++ 
language executables in the form of a static library.

A static library or statically-linked library is a set of routines, external 
functions and variables which are resolved in a caller at compile-time and 
copied into a target application by a compiler, linker, or binder, producing 
an object file and a stand-alone executable. This executable and the process 
of compiling it are both known as a static build of the program.

There are several advantages to statically linking libraries with an 
executable instead of dynamically linking them. The most significant is that 
the application can be certain that all its libraries are present and that 
they are the correct version. This avoids dependency problems. In some cases, 
static linking can result in a performance improvement. Static linking can 
also allow the application to be contained in a single executable file, 
simplifying distribution and installation. On the other hand, statically 
linking libraries with the executable increases its size. This is because the
library code is stored within the executable rather than in separate files.

The sample LIB exports these functionalities:

// Functions
int /*__cdecl*/ GetStringLength1(PCWSTR pszString);
int __stdcall GetStringLength2(PCWSTR pszString);

// Class
CppSimpleClass


/////////////////////////////////////////////////////////////////////////////
Sample Relation:

CppStaticallyLinkLib -> CppStaticLibrary
CppStaticallyLinkLib references CppStaticLibrary, and uses the functionality 
from the static library.

CppCLIWrapLib -> CppStaticLibrary
CppCLIWrapLib wraps the native class in CppStaticLibrary so it can be 
consumed by code authored in C#, or other .NET language.


/////////////////////////////////////////////////////////////////////////////
Implementation:

A. Creating the project

Step1. Create a Visual C++ / Win32 / Win32 Project named CppStaticLibrary in 
Visual Studio 2008.

Step2. In the page "Application Settings" of Win32 Application Wizard, select
Application type as Static library, and check the Precompiled header checkbox. 
Click Finish.

B. Adding a class and some functions to the static library

Step1. Create the files CppSimpleClass.h/cpp.

Step2. Declare the class CppSimpleClass in CppSimpleClass.h, and implement 
the class in CppSimpleClass.cpp.

Step3. Create the files CppLibFuncs.h/cpp.

Step4. Declare the functions GetStringLength1 and GetStringLength2 in 
CppLibFuncs.h, and implement the functions in CppLibFuncs.cpp.


/////////////////////////////////////////////////////////////////////////////
References:

Creating and Using a Static Library (C++)
http://msdn.microsoft.com/en-us/library/ms235627.aspx

Static library
http://en.wikipedia.org/wiki/Static_library


/////////////////////////////////////////////////////////////////////////////
