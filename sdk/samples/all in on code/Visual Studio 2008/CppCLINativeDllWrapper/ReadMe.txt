=============================================================================
      DYNAMIC LINK LIBRARY : CppCLINativeDllWrapper Project Overview
=============================================================================

/////////////////////////////////////////////////////////////////////////////
Summary:

This C++/CLI code sample demonstrates making C++/CLI wrapper classes for a 
native C++ DLL module that allow you to call from any .NET code to the 
classes and functions exported by the module.

  CSCallNativeDllWrapper/VBCallNativeDllWrapper (any .NET clients)
          -->
      CppCLINativeDllWrapper (this C++/CLI wrapper)
              -->
          CppDynamicLinkLibrary (a native C++ DLL module)

In this code sample, the CSimpleObjectWrapper class wraps the native C++ 
class CSimpleObject, and the NativeMethods class wraps the global functions 
exported by CppDynamicLinkLibrary.dll.

The interoperability features supported by Visual C++/CLI offer a particular 
advantage over other .NET languages when it comes to interoperating with 
native modules. Apart from the traditional explicit P/Invoke, C++/CLI allows 
implicit P/Invoke, also known as C++ Interop, or It Just Work (IJW), which 
mixes managed code and native code almost invisibly. The feature provides 
better type safety, easier coding, greater performance, and is more forgiving 
if the native API is modified. You can use the technology to build .NET 
wrappers for native C++ classes and functions if their source code is 
available, and allow any .NET clients to access the native C++ classes and 
functions through the wrappers.


/////////////////////////////////////////////////////////////////////////////
Sample Relation:
(The relationship between the current sample and the rest samples in 
Microsoft All-In-One Code Framework http://1code.codeplex.com)

CppCLINativeDllWrapper -> CppDynamicLinkLibrary
The C++/CLI sample module CppCLINativeDllWrapper wraps the classes and 
functions exported by the native C++ sample module CppDynamicLinkLibrary. The
wrapper classes and functions can be called by any .NET code to indirectly 
interoperate with the native C++ classes and functions.


/////////////////////////////////////////////////////////////////////////////
Implementation:

Step1. Create a Visual C++ / CLR / Class Library project named 
CppCLINativeDllWrapper in Visual Studio 2008. The project wizard generates 
a default empty C++/CLI class:

    namespace CppCLINativeDllWrapper {

	    public ref class Class1
	    {
		    // TODO: Add your methods for this class here.
	    };
    }

Step2. Reference the native C++ DLL CppDynamicLinkLibrary.

  Option1. Link the LIB file of the DLL by entering the LIB file name in 
  Project Properties / Linker / Input / Additional Dependencies. We can 
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

Step3. Including the header file that declares the functions and classes of 
the DLL.

    #include "CppDynamicLinkLibrary.h"

You can configure the search path of the header file in Project Properties / 
C/C++ / General / Additional Include Directories.

Step4. Design the C++/CLI wrapper class CSimpleObjectWrapper for the native 
C++ class CSimpleObject.

    public ref class CSimpleObjectWrapper
    {
    public:
        CSimpleObjectWrapper(void);
        virtual ~CSimpleObjectWrapper(void);

        // Property
        property float FloatProperty
        {
            float get(void);
            void set(float value);
        }

        // Method
        virtual String ^ ToString(void) override;

        // Static method
        static int GetStringLength(String ^ str);

    protected:
        !CSimpleObjectWrapper(void);

    private:
        CSimpleObject *m_impl;
    };

The class wraps an instance of the native C++ class CSimpleObject. The 
instance is pointed by the private member variable m_impl. It is 
initialized in the constructor CSimpleObjectWrapper(void);, and is freed 
in the desctructor (virtual ~CSimpleObjectWrapper(void);) and the finalizer 
(!CSimpleObjectWrapper(void);).

    CSimpleObjectWrapper::CSimpleObjectWrapper(void)
    {
        m_impl = new CSimpleObject();
    }
    
    CSimpleObjectWrapper::~CSimpleObjectWrapper(void)
    {
        if (m_impl)
        {
            delete m_impl;
            m_impl = NULL;
        }
    }

    CSimpleObjectWrapper::!CSimpleObjectWrapper(void)
    {
        if (m_impl)
        {
            delete m_impl;
            m_impl = NULL;
        }
    }

The public member properties and methods of CSimpleObjectWrapper wraps those 
of the native C++ class CSimpleObject. They redirects the calls to 
CSimpleObject through the CSimpleObject instance pointed by m_impl. Type 
marshaling takes place between the managed and native code.

    float CSimpleObjectWrapper::FloatProperty::get(void)
    {
        return m_impl->get_FloatProperty();
    }

    void CSimpleObjectWrapper::FloatProperty::set(float value)
    {
        m_impl->set_FloatProperty(value);
    }

    String ^ CSimpleObjectWrapper::ToString(void)
    {
        wchar_t szStr[100];
        HRESULT hr = m_impl->ToString(szStr, ARRAYSIZE(szStr));
        if (FAILED(hr))
        {
            Marshal::ThrowExceptionForHR(hr);
        }
        // Marshal PWSTR to System::String and return it.
        return gcnew String(szStr);
    }

    int CSimpleObjectWrapper::GetStringLength(System::String ^ str)
    {
        // Marshal System::String to PCWSTR, and call the C++ function.
        marshal_context ^ context = gcnew marshal_context();
        PCWSTR pszString = context->marshal_as<const wchar_t*>(str);
        int length = CSimpleObject::GetStringLength(pszString);
        delete context;
        return length;
    }

Step5. Design the C++/CLI wrapper class NativeMethods for the functions 
exported by the native C++ DLL module.

    /// <summary>
    /// Function delegate of the 'PFN_COMPARE' callback function. 
    /// </summary>
    /// <remarks>
    /// The delegate type has the UnmanagedFunctionPointerAttribute. Using 
    /// this attribute, you can specify the calling convention of the native 
    /// function pointer type. In the native API's header file, the callback 
    /// PFN_COMPARE is defined as __stdcall (CALLBACK), so here we specify 
    /// CallingConvention::StdCall.
    /// </remarks>
    [UnmanagedFunctionPointer(CallingConvention::StdCall)]
    public delegate int CompareCallback(int a, int b);


    /// <summary>
    /// This C++/CLI class wraps the functions exported by the native C++ 
    /// module CppDynamicLinkLibrary.dll. 
    /// </summary>
    public ref class NativeMethods
    {
    public:
        static int GetStringLength1(String ^ str);
        static int GetStringLength2(String ^ str);
        static int Max(int a, int b, CompareCallback ^ cmpFunc);
    };

All methods in NativeMethods are declared as static for the global functions 
exported by CppDynamicLinkLibrary. They redirect calls to the native DLL.

    int NativeMethods::GetStringLength1(String ^ str)
    {
        // Marshal System::String to PCWSTR, and call the C++ function.
        marshal_context ^ context = gcnew marshal_context();
        PCWSTR pszString = context->marshal_as<const wchar_t*>(str);
        int length = ::GetStringLength1(pszString);
        delete context;
        return length;
    }

    int NativeMethods::GetStringLength2(String ^ str)
    {
        // Marshal System::String to PCWSTR, and call the C++ function.
        marshal_context ^ context = gcnew marshal_context();
        PCWSTR pszString = context->marshal_as<const wchar_t*>(str);
        int length = ::GetStringLength2(pszString);
        delete context;
        return length;
    }

    int NativeMethods::Max(int a, int b, CompareCallback ^ cmpFunc)
    {
        // Convert the delegate to a function pointer.
        IntPtr pCmpFunc = Marshal::GetFunctionPointerForDelegate(cmpFunc);
        return ::Max(a, b, static_cast<::PFN_COMPARE>(pCmpFunc.ToPointer()));
    }


/////////////////////////////////////////////////////////////////////////////
References:

Using C++ Interop (Implicit PInvoke)
http://msdn.microsoft.com/en-us/library/2x8kf7zx.aspx

How to: Wrap Native Class for Use by C#
http://msdn.microsoft.com/en-us/library/ms235281.aspx


/////////////////////////////////////////////////////////////////////////////