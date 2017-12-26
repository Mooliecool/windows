========================================================================
    CONSOLE APPLICATION : CppStaticallyLinkLib Project Overview
========================================================================

/////////////////////////////////////////////////////////////////////////////
Use:

This sample demonstrates statically linking CppStaticLibrary.lib and using 
its functionalities.

There are several advantages to statically linking libraries with an 
executable instead of dynamically linking them. The most significant is that 
the application can be certain that all its libraries are present and that 
they are the correct version. This avoids dependency problems. In some cases, 
static linking can result in a performance improvement. Static linking can 
also allow the application to be contained in a single executable file, 
simplifying distribution and installation. On the other hand, statically 
linking libraries with the executable increases its size. This is because the 
library code is stored within the executable rather than in separate files. 


/////////////////////////////////////////////////////////////////////////////
Project Relation:

CppStaticallyLinkLib -> CppStaticLibrary
CppStaticallyLinkLib references CppStaticLibrary, and uses the functionality 
from the static library.


/////////////////////////////////////////////////////////////////////////////
Implementation:

A. Referencing the Static Library

Option1. Link the LIB file of CppStaticLibrary by entering the LIB file name  
in Project Properties / Linker / Input / Additional Dependencies. We can 
configure the search path of the LIB file in Project Properties / Linker / 
General / Additional Library Directories.

Option2. Select References... from the Project's shortcut menu. From the 
Property Pages dialog box, expand the Common Properties node and select 
References. Then select the Add New Reference... button. The Add Reference 
dialog box is displayed. This dialog box lists all the libraries that you can 
reference. The Project tab lists all the projects in the current solution and 
any libraries they contain. On the Projects tab, select CppStaticLibrary. 
Then select OK.

B. Including the header file

To reference the header files of the static library, you can modify the 
include directories path. To do this, in the Property Pages dialog box, 
expand the Configuration Properties node, expand the C/C++ node, and then 
select General. Next to Additional Include Directories, type the path of the 
location of the CppSimpleClass.h and CppLibFuncs.h header files.

In the source code file, include header with the statement:

	#include "CppSimpleClass.h"
	#include "CppLibFuncs.h"

C. Using the symbols

For example:

	_TCHAR* result;
	HelloWorld(&result);


/////////////////////////////////////////////////////////////////////////////
References:

Creating and Using a Static Library (C++)
http://msdn.microsoft.com/en-us/library/ms235627.aspx

Static library
http://en.wikipedia.org/wiki/Static_library


/////////////////////////////////////////////////////////////////////////////
