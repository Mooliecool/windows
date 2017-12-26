=============================================================================
        DYNAMIC LINK LIBRARY : CppDynamicLinkLibrary Project Overview
=============================================================================

/////////////////////////////////////////////////////////////////////////////
Summary:

A dynamic-link library (DLL) is a module that contain functions and data that 
can be used by another module (application or DLL). This Win32 DLL code 
sample demonstrates exporting data, functions and classes for use in 
executables.

The sample DLL exports these data, functions and classes:

    // Global Data
    int g_nVal1
    int g_nVal2

    // Ordinary Functions
    int __cdecl GetStringLength1(PCWSTR pszString);
    int __stdcall GetStringLength2(PCWSTR pszString);

    // Callback Function
    int __stdcall Max(int a, int b, PFN_COMPARE cmpFunc)

    // Class
    class CSimpleObject
    {
    public:
        CSimpleObject(void);  // Constructor
        virtual ~CSimpleObject(void);  // Destructor
          
        // Property
        float get_FloatProperty(void);
        void set_FloatProperty(float newVal);

        // Method
        HRESULT ToString(PWSTR pszBuffer, DWORD dwSize);

        // Static method
        static int GetStringLength(PCWSTR pszString);

    private:
        float m_fField;
    };

Two methods are used to export the symbols from the sample DLL:

1. Export symbols from a DLL using .DEF files

A module-definition (.DEF) file is a text file containing one or more module 
statements that describe various attributes of a DLL. Create a .DEF file and 
use the .def file when building the DLL. Using this approach, we can export 
functions from the DLL by ordinal rather than by name. 

2. Export symbols from a DLL using __declspec(dllexport) 

__declspec(dllexport) adds the export directive to the object file so we do 
not need to use a .def file. This convenience is most apparent when trying to
export decorated C++ function names. 


/////////////////////////////////////////////////////////////////////////////
Sample Relation:
(The relationship between the current sample and the rest samples in 
Microsoft All-In-One Code Framework http://1code.codeplex.com)

CppImplicitlyLinkDll -> CppDynamicLinkLibrary
CppImplicitlyLinkDll implicitly links (staticly loads) the DLL and uses its 
symbols.

CppDelayloadDll -> CppDynamicLinkLibrary
CppDelayloadDll delay-loads the DLL and uses its symbols.

CppLoadLibrary -> CppDynamicLinkLibrary
CppLoadLibrary dynamically loads the DLL and uses its symbols.

CSLoadLibrary -> CppDynamicLinkLibrary
CSLoadLibrary, a .NET executable, dynamically loads the native DLL and uses
its symbols through the APIs: LoadLibrary, GetProcAddress and FreeLibrary.

CSPInvokeDll -> CppDynamicLinkLibrary
CSPInvokeDll, a .NET executable, dynamically loads the native DLL and uses 
its symbols through P/Invoke.


/////////////////////////////////////////////////////////////////////////////
Implementation:

A. Creating the project

Step1. Create a Visual C++ / Win32 / Win32 Project named 
"CppDynamicLinkLibrary" in Visual Studio 2008.

Step2. In the page "Application Settings" of Win32 Application Wizard, select
Application type as DLL, and check the Export symbols checkbox. Click Finish.

B. Exporting symbols from a DLL using .DEF files
http://msdn.microsoft.com/en-us/library/d91k01sh.aspx
A module-definition (.def) file is a text file containing one or more module 
statements that describe various attributes of a DLL. It provides the linker 
with information about exports, attributes, and other information about the 
program to be linked.

Step1. Declare the data and functions to be exported in the header file, and 
define them in the .cpp file.

    int g_nVal1;
    int /*__cdecl*/ GetStringLength1(PCWSTR pszString)
    int __stdcall GetStringLength1(PCWSTR pszString)
    int __stdcall Max(int a, int b, PFN_COMPARE cmpFunc)

Step2. Add a .DEF file named CppDynamicLinkLibrary.def to the project. The 
first statement in the file must be the LIBRARY statement. This statement 
identifies the .def file as belonging to a DLL. The LIBRARY statement is 
followed by the name of the DLL. The linker places this name in the DLL's 
import library. Next, the EXPORTS statement lists the names and, optionally, 
the ordinal values of the data and functions exported by the DLL. 

    LIBRARY   CppDynamicLinkLibrary
    EXPORTS
       GetStringLength1     @1
       Max					@2
       g_nVal1				DATA

Step3. In order that the DLL project invoke the .def file during the linker 
phase, right-click the project and open its Properties dialog. In the Linker / 
Input page, set the value of Module Definition File (/DEF:) to be 
"CppDynamicLinkLibrary.def".

C. Exporting symbols from a DLL using __declspec(dllexport)

Step1. Create the following #ifdef block in the header file to make exporting 
& importing from a DLL simpler (This should be automatically added if you 
check the "Export symbols" checkbox when you create the project). All files 
within this DLL are compiled with the CPPDYNAMICLINKLIBRARY_EXPORTS symbol 
defined on the command line (see the C/C++ / Preprocessor page in the project 
Properties dialog). This symbol should not be defined on any project that 
uses this DLL. This way any other project whose source files include this 
file see SYMBOL_DECLSPEC functions as being imported from a DLL, whereas this 
DLL sees symbols defined with this macro as being exported.

	#ifdef CPPDYNAMICLINKLIBRARY_EXPORTS
	#define SYMBOL_DECLSPEC __declspec(dllexport)
	#else
	#define SYMBOL_DECLSPEC __declspec(dllimport)
	#endif

Step2. Declare the data, functions, and classes to be exported in the header 
file. Add SYMBOL_DECLSPEC in the signatures. For those data and functions 
that may be accessed from the C language modules or dynamically linked by any 
executables, add EXTERN_C (i.e. extern "C") at the beginning to specify C 
linkage. This removes the C++ type-safe naming (aka. name decoration). 
Initialize the data and implement the functions and classes in the .cpp file. 

    EXTERN_C SYMBOL_DECLSPEC int g_nVal2;
    EXTERN_C SYMBOL_DECLSPEC int /*__cdecl*/ GetStringLength2(PCWSTR pszString);
    EXTERN_C SYMBOL_DECLSPEC int __stdcall GetStringLength2(PCWSTR pszString);
    class SYMBOL_DECLSPEC CSimpleObject
    {
        ...
    };


/////////////////////////////////////////////////////////////////////////////
References:

MSDN: Exporting from a DLL
http://msdn.microsoft.com/en-us/library/z4zxe9k8.aspx

MSDN: Exporting from a DLL Using DEF Files
http://msdn.microsoft.com/en-us/library/d91k01sh.aspx

MSDN: Exporting from a DLL Using __declspec(dllexport)
http://msdn.microsoft.com/en-us/library/a90k134d.aspx

MSDN: Creating and Using a Dynamic Link Library (C++)
http://msdn.microsoft.com/en-us/library/ms235636.aspx

HOWTO: How To Export Data from a DLL or an Application
http://support.microsoft.com/kb/90530

Dynamic-link library
http://en.wikipedia.org/wiki/Dynamic_link_library


/////////////////////////////////////////////////////////////////////////////