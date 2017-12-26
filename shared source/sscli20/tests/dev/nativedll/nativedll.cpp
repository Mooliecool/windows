// ==++==
// 
//   
//    Copyright (c) 2006 Microsoft Corporation.  All rights reserved.
//   
//    The use and distribution terms for this software are contained in the file
//    named license.txt, which can be found in the root of this distribution.
//    By using this software in any fashion, you are agreeing to be bound by the
//    terms of this license.
//   
//    You must not remove this notice, or any other, from this software.
//   
// 
// ==--==
// ===========================================================================
// File: nativedll.cpp
// 
// native dll for FFI testing, can be reused for other tests too
// ===========================================================================

#include "windows.h"
#include "stdlib.h"

#include "cor.h"
#include "corffi.h"

extern "C" BOOL WINAPI DllMain( HINSTANCE hInst, DWORD dwReason, LPVOID pvReserved )
{
    switch (dwReason)
    {
    case DLL_PROCESS_ATTACH:
        if (PAL_Initialize(NULL, NULL) != 0)
            return FALSE;
        DisableThreadLibraryCalls(hInst);
        break;

    case DLL_PROCESS_DETACH:
        PAL_Terminate();
        break;

    default:
        break;
    }
    return TRUE;
}

void fail(char *msg)
{
    fprintf(stderr, "Failed: %s\n", msg);
    exit(1);
}

// simple delegate roundtrip
typedef int (__stdcall *MySimpleDelegate)(int x);

MySimpleDelegate g_CachedSimpleDelegate;

extern "C" void __stdcall CacheSimpleDelegate(MySimpleDelegate pfn)
{
    g_CachedSimpleDelegate = pfn;
}

extern "C" int __stdcall CallSimpleDelegate(MySimpleDelegate pfn, int x)
{
    if (pfn == NULL)
        pfn = g_CachedSimpleDelegate;

    return pfn(x) + 1;
}

// delegate roundtrip
typedef double (__stdcall *MyDelegate)(IUnknown* punk); 

MyDelegate g_CachedDelegate;

extern "C" void __stdcall CacheDelegate(MyDelegate pfn)
{
    g_CachedDelegate = pfn;
}

extern "C" double __stdcall CallDelegate(MyDelegate pfn, IUnknown* punk)
{
    if (pfn == NULL)
        pfn = g_CachedDelegate;

    return pfn(punk) + 1;
}

// ffi roundtrip
extern "C" double __stdcall CallFFI(IUnknown* pcallback, IUnknown* punk)
{
    IManagedInstanceWrapper* pmiw;
    VARIANT arg;
    VARIANT retval;
    HRESULT hr;

    V_VT(&arg) = VT_UNKNOWN;
    V_UNKNOWN(&arg) = punk;

    pcallback->QueryInterface(IID_IManagedInstanceWrapper, (void**)&pmiw);
    hr = pmiw->InvokeByName(
        L"MyCallback", CorFFIInvokeMethod, 1, &arg, &retval);
    pmiw->Release();

    if (FAILED(hr))
        fail("MyCallback failed");

    if (V_VT(&retval) != VT_R8)
        fail("Expected R8");
       
    return V_R8(&retval) + 1;
}

// array marshalling
extern "C" int __stdcall Sum(int count, int* a)
{
    int sum = 0;
    for (int i = 0; i < count; i++)
        sum += a[i];
    return sum;
}

// array marshalling ex
extern "C" int __stdcall ObjectSum(int count, IUnknown** a)
{
    int sum = 0;
    for (int i = 0; i < count; i++) {
        VARIANT var;
        HRESULT hr;
        IManagedInstanceWrapper* pmiw;

        a[i]->QueryInterface(IID_IManagedInstanceWrapper, (void**)&pmiw);
        hr = pmiw->InvokeByName(
            L"Value", CorFFIGetProperty, 0, NULL, &var);
        pmiw->Release();

        if (FAILED(hr))
            fail("get_Value failed");

        if (V_VT(&var) != VT_I4) {
            fail("Expected VT_I4");            
        }

        sum += V_I4(&var);
    }
    return sum;
}

// errorinfo roundtrip
extern "C" HRESULT __stdcall ReallyBadError(IUnknown *punk)
{
    HRESULT hr;
    IErrorInfo* err;
    ICreateErrorInfo  *cerr;
    BSTR bstr;
    WCHAR buf[10000];
    IManagedInstanceWrapper* pmiw;

    punk->QueryInterface(IID_IManagedInstanceWrapper, (void**)&pmiw);
    hr = pmiw->InvokeByName(
        L"BadError", CorFFIInvokeMethod, 0, NULL, NULL);
    pmiw->Release();

    if (GetErrorInfo(0, &err) != S_OK)
        fail("No error info!");

    err->GetDescription(&bstr);
    // printf("Description: %S\n", bstr);
    err->Release();

    _snwprintf(buf, 10000, L"qwerty %s", bstr);
    SysFreeString(bstr);

    if (CreateErrorInfo(&cerr) != S_OK)
        fail("Failed to create error info!");

    cerr->SetDescription(buf);

    cerr->QueryInterface(IID_IErrorInfo, (void**)&err);
    cerr->Release();

    SetErrorInfo(0, err);
    err->Release();

    return E_FAIL;
}

// exception roundtrip
extern "C" void __stdcall Exception12345678(MyDelegate pfn, IUnknown* punk)
{
    BOOL fException = FALSE;
    PAL_TRY {
        pfn(punk);
    }
    PAL_EXCEPT(EXCEPTION_EXECUTE_HANDLER) {
        fException = TRUE;
    }
    PAL_ENDTRY

    if (!fException) {
        fail("No exception thrown from managed code!");
    }

    RaiseException(0x12345678, EXCEPTION_NONCONTINUABLE_EXCEPTION, 0, NULL);
}

extern "C" MySimpleDelegate DelegateMarshal(MySimpleDelegate d)
{
    return d;
}
