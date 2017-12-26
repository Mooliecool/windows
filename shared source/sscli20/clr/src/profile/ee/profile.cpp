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
//*****************************************************************************
//*****************************************************************************

#include "stdafx.h"
#include "eeprofinterfaces.h"
#include "eetoprofinterfaceimpl.h"
#include "corprof.h"

ProfToEEInterface     *g_pProfToEEInterface = NULL;
ICorProfilerCallback2 *g_pCallback          = NULL;
CorProfInfo           *g_pInfo              = NULL;
ICorProfilerCallback  *g_pEverettCallback_DELETE_ME = NULL;
BOOL                   g_fSupportWhidbeyOnly_DELETE_ME = FALSE;

extern "C" {
/*
 * GetEEProfInterface is used to get the interface with the profiler code.
 */
void __cdecl GetEEToProfInterface(EEToProfInterface **ppEEProf)
{
    InitializeLogging();

    LOG((LF_CORPROF, LL_INFO10, "**PROF: EE has requested interface to "
         "profiling code.\n"));
    
    // Check if we're given a bogus pointer
    if (ppEEProf == NULL)
    {
        LOG((LF_CORPROF, LL_INFO10, "**PROF: EE provided invalid pointer.  "
             "%s::%d.\n", __FILE__, __LINE__));
        return;
    }

    // Initial value
    *ppEEProf = NULL;

    // Create a new Impl object and cast it to the virtual class type
    EEToProfInterface *pEEProf =
        (EEToProfInterface *) new EEToProfInterfaceImpl();
    
    _ASSERTE(pEEProf != NULL);

    // If we succeeded, send it back
    if (pEEProf != NULL)
        if (SUCCEEDED(pEEProf->Init()))
            *ppEEProf = pEEProf;
        else
            delete pEEProf;

    return;
}

/*
 * SetProfEEInterface is used to provide the profiler code with an interface
 * to the profiler.
 */
void __cdecl SetProfToEEInterface(ProfToEEInterface *pProfEE)
{
    InitializeLogging();

    LOG((LF_CORPROF, LL_INFO10, "**PROF: Profiling code being provided with EE interface.\n"));

    // Save the pointer
    g_pProfToEEInterface = pProfEE;

    return;
}

}


typedef HRESULT __stdcall DLLGETCLASSOBJECT(REFCLSID rclsid,
                                            REFIID   riid,
                                            void   **ppv);

HRESULT PALAPI ProfilerCreateInstance( __in LPWSTR wszProfileDLL,
                                      REFCLSID   rclsid,
                                      REFIID     riid,
                                      void     **ppv )
{
    HRESULT             hr = NOERROR;
    HINSTANCE           dll;
    DLLGETCLASSOBJECT   *dllGetClassObject;
    IClassFactory       *classFactory;

    dll = LoadLibraryW(wszProfileDLL);
    if (dll == NULL)
        goto Win32Error;

    dllGetClassObject = (DLLGETCLASSOBJECT*)GetProcAddress(dll, "DllGetClassObject");
    if (dllGetClassObject == NULL) {
        // The CLR shim exports a DllGetClassObject which in turn decides which DLL to load and
        // call DllGetClassObjectInternal on.  Without the shim, the PALRT must do the same
        // here.
        dllGetClassObject = (DLLGETCLASSOBJECT*)GetProcAddress(dll, "DllGetClassObjectInternal");
        if (dllGetClassObject == NULL) {
            goto Win32Error;
        }
    }

    hr = (*dllGetClassObject)(rclsid, IID_IClassFactory, (void**)&classFactory);
    if (FAILED(hr))
        return hr;

    hr = classFactory->CreateInstance(NULL, riid, ppv);
    classFactory->Release();
    return hr;

Win32Error:
    DWORD dwError = GetLastError();
    return HRESULT_FROM_WIN32(dwError);

}
/*
 * This will attempt to CoCreate all registered profilers
 */
HRESULT CoCreateProfiler( __inout_z LPWSTR wszCLSID, __in LPWSTR wszProfileDLL, ICorProfilerCallback **ppCallback, BOOL fSupportEverett )
{
    LOG((LF_CORPROF, LL_INFO10, "**PROF: Entered CoCreateProfiler.\n"));

    HRESULT hr;

    // Translate the string into a CLSID
    CLSID clsid;
	if (*wszCLSID == L'{')
    {
        hr = IIDFromString(wszCLSID, &clsid);
    }
	else
	{
        hr = E_INVALIDARG;
	}

#if defined(LOGGING)
    if (hr == E_INVALIDARG || hr == CO_E_CLASSSTRING)
    {
        LOG((LF_CORPROF, LL_INFO10, "**PROF: Invalid CLSID or ProgID. %s::%d\n",
             __FILE__, __LINE__));
    }
#endif


    if (FAILED(hr))
        return (hr);

    hr = ProfilerCreateInstance(
        wszProfileDLL, 
        clsid, 
        fSupportEverett ? IID_ICorProfilerCallback : IID_ICorProfilerCallback2, 
        (LPVOID *)ppCallback);

    // Return the result of the CoCreateInstance operation
    return (hr);
}

