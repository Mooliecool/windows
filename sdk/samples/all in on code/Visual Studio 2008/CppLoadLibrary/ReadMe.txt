=============================================================================
          CONSOLE APPLICATION : CppLoadLibrary Project Overview
=============================================================================

/////////////////////////////////////////////////////////////////////////////
Summary:

This is an example of dynamically loading a DLL using the APIs LoadLibrary, 
GetProcAddress and FreeLibrary. In contrast with implicit linking (static 
loading), dynamic loading does not require the LIB file, and the application 
loads the module just before we call a function in the DLL. The API functions 
LoadLibrary and GetProcAddress are used to load the DLL and then retrieve the
address of a function in the export table. Because we explicitly invoke these 
APIs, this kind of loading is also referred to as explicit linking. 


/////////////////////////////////////////////////////////////////////////////
Sample Relation:

CppLoadLibrary -> CppDynamicLinkLibrary
CppLoadLibrary dynamically loads CppDynamicLinkLibrary.dll and calls the 
functions exported by the DLL.

CppLoadLibrary - CSLoadLibrary
CSLoadLibrary in C# mimics the behavior of CppLoadLibrary to dynamically load
a native DLL, get the address of a function in the export table, and call it.


/////////////////////////////////////////////////////////////////////////////
Implementation:

1. Type-define the functions exported from the DLL. For example:

	typedef int (* LPFNGETSTRINGLENGTH1)    (PWSTR);

2. Dynamically load the DLL by calling LoadLibrary.

    hModule = LoadLibrary(pszModuleName);

3. Call GetProcAddress to get the address of the function in the export table 
of the DLL.

    LPFNGETSTRINGLENGTH1 lpfnGetStringLength1 = (LPFNGETSTRINGLENGTH1) 
        GetProcAddress(hModule, "GetStringLength1");

4. Call the function.

    PWSTR pszString = L"HelloWorld";
    int nLength;
    nLength = lpfnGetStringLength1(pszString);

5. Unload the library by calling FreeLibrary.

    FreeLibrary(hModule);


/////////////////////////////////////////////////////////////////////////////
References:

MSDN: Using Run-Time Dynamic Linking
http://msdn.microsoft.com/en-us/library/ms686944.aspx


/////////////////////////////////////////////////////////////////////////////