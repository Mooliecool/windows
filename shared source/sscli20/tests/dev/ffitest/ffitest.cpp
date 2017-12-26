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
// File: ffitest.cpp
// 
// Simple FFI test
// ===========================================================================

#include "windows.h"
#include "stdlib.h"

#include "cor.h"
#include "mscoree.h"
#include "corffi.h"

#include <product_version.h>

#include "palstartup.h"

int __cdecl main(int argc, char ** argv)
{   
    IManagedInstanceWrapper *pWrap;
    IManagedInstanceWrapper *pTypeWrap;
    VARIANT RetVal;
    HRESULT hr;

    if (!PAL_RegisterLibrary(L"rotor_palrt") ||
        !PAL_RegisterLibrary(L"sscoree")) {
        fprintf(stderr, "PAL_RegisterLibraryW failed\n");
        return 1;
    }

    VariantInit(&RetVal);

    hr = ClrCreateManagedInstance(
        L"System.Random,mscorlib,PublicKeyToken=b77a5c561934e089",
        IID_IManagedInstanceWrapper,
        (void**)&pWrap);
    if (FAILED(hr)) {
        fprintf(stderr, "new System.Random() failed with hr=0x%08x\n", hr);
        goto DisplayErrorInfo;
    }   

    hr = pWrap->InvokeByName(
        L"Next", CorFFIInvokeMethod, 0, NULL, &RetVal);

    if (FAILED(hr)) {
        fprintf(stderr, "Random.Next() failed with hr=0x%08x\n", hr);
        goto DisplayErrorInfo;
    }

    if (V_VT(&RetVal) != VT_I4) {
        fprintf(stderr, "Random.Next() returned unexpected type (%d)\n", V_VT(&RetVal));
        return 3;
    }

    printf("Random.Next() returned %d\n", V_I4(&RetVal));

    VariantClear(&RetVal);
    pWrap->Release();

    // try something more complex

    // the current implementation of ClrCreateManagedInstance needs 
    //  a fully qualified name of the assembly if the assembly is in the GAC
    hr = ClrCreateManagedInstance(
        L"System.Xml.Xsl.XslTransform,System.Xml,Version=" VER_ASSEMBLYVERSION_STR_L L",Culture=neutral,PublicKeyToken=b77a5c561934e089",
        IID_IManagedInstanceWrapper,
        (void**)&pWrap);
    if (FAILED(hr)) {
        fprintf(stderr, "new System.Xml.Xsl.XslTransform() failed with hr=0x%08x\n", hr);
        goto DisplayErrorInfo;
    }

    hr = pWrap->InvokeByName(L"GetType", CorFFIInvokeMethod, 0, NULL, &RetVal);
    if (FAILED(hr))
    {
        fprintf(stderr, "XslTransform.GetType() failed with hr=0x%08x\n", hr);
        goto DisplayErrorInfo;
    }

    if (V_VT(&RetVal) != VT_UNKNOWN) {
        fprintf(stderr, "XslTransform.GetType() returned unexpected type (%d)\n", V_VT(&RetVal));
        return 3;
    }

    hr = V_UNKNOWN(&RetVal)->QueryInterface(IID_IManagedInstanceWrapper,
        (void**)&pTypeWrap);
    if (FAILED(hr))
    {
        fprintf(stderr, "QI for IManagedInstanceWrapper failed with hr=0x%08x\n", hr);
        return 4;
    }

    VariantClear(&RetVal);

    hr = pTypeWrap->InvokeByName(L"FullName", CorFFIGetProperty, 0, NULL, &RetVal);
    if (FAILED(hr))
    {
        fprintf(stderr, "Type.FullName failed with hr=0x%08x\n", hr);
        goto DisplayErrorInfo;
    }

    if (V_VT(&RetVal) != VT_BSTR) {
        fprintf(stderr, "Type.FullName returned unexpected type (%d)\n", V_VT(&RetVal));
        return 5;
    }

    printf("XslTransform.GetType().FullName returned %S\n", V_BSTR(&RetVal));

    VariantClear(&RetVal);
    pTypeWrap->Release();
    pWrap->Release();

    hr = ClrCreateManagedInstance(
        L"UnknownClass,UnknownAssembly,PublicKeyToken=b77a5c561934e089",
        IID_IManagedInstanceWrapper,
        (void**)&pWrap);
    if (SUCCEEDED(hr)) {
        fprintf(stderr, "new UnknownClass() haven't failed gracefully\n", hr);
        return 6;
    }

    fprintf(stderr, "PASSED\n");

    return 0;

DisplayErrorInfo:
    IErrorInfo *pErr = NULL;
    GetErrorInfo(0, &pErr);
    if (pErr != NULL) {
        BSTR bstr = NULL;
        pErr->GetDescription(&bstr);
        if (bstr != NULL) {
            fprintf(stderr, "%S\n", bstr);
            SysFreeString(bstr);
        }
        pErr->Release();
    }
    return 2;
}
