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

#include "rotor_pal.h"
#include "rotor_palrt.h"
#include "syms.h"
#include "libs.h"
#include "sscoree_int.h"

#include <stdio.h>

#ifdef _DEBUG
//#define TRACE_LOADS 1
#endif

ROTOR_PAL_CTOR_TEST_BODY (SSCOREE_INT);

/***************************************************************************/
/* Functions internal to this library                                      */
/***************************************************************************/

#ifdef _DEBUG
static void DisplayMessageFromSystem(DWORD LastErr)
{
    DWORD dw;
    WCHAR ErrorBuffer[256];

    dw = FormatMessageW(FORMAT_MESSAGE_FROM_SYSTEM,
                NULL,
                LastErr,
                0,
                ErrorBuffer,
                sizeof(ErrorBuffer)/sizeof(WCHAR),
                NULL);
    if (dw == 0) {
        fwprintf(stderr, L"ERROR:  LastError=%d\n", LastErr);
        return;
    }

    fwprintf(stderr, L"%s", ErrorBuffer);
}
#endif // _DEBUG

ShimmedLib
FindSymbolsLib (
    ShimmedSym SymIndex)
{
    // some trickery to figure out which library this symbol is in
    _ASSERTE (SYM_INDEX_VALID (SymIndex));
        
#define SSCOREE_LIB_START(LIBNAME)                                      \
    if (SymIndex < SHIMLIB_ ## LIBNAME) {                               \
        return LIB_ ## LIBNAME;                                         \
    }                                                                   \
    if (SymIndex == SHIMLIB_ ## LIBNAME) {                              \
        return LIB_ ## MAX_LIB;                                         \
    }
#include "sscoree_shims.h"
    
    return LIB_MAX_LIB;
}

HMODULE
SetupLib (
    ShimmedLib LibIndex)
{
    HMODULE lib_handle;
    WCHAR FullPath[_MAX_PATH];

    if (!PAL_GetPALDirectory (FullPath, _MAX_PATH)) {
        return NULL;
    }
    if (wcslen(FullPath) + wcslen(g_Libs[LibIndex].Name) >= _MAX_PATH) {
        SetLastError(ERROR_FILENAME_EXCED_RANGE);
        return NULL;
    }
    wcsncat(FullPath, g_Libs[LibIndex].Name, _MAX_PATH);

    lib_handle = LoadLibrary (FullPath);
    if (lib_handle == NULL) {
#ifdef _DEBUG
        fprintf (stderr,
                    "SscoreeShimGetProcAddress: LoadLibrary (\"%S\") failed\n",
                    FullPath);
        DisplayMessageFromSystem(GetLastError());
#endif
        return lib_handle;
    }
    g_Libs[LibIndex].Handle = lib_handle;

#ifdef _DEBUG
    // first time we've hit this library. Run some tests.
    SscoreeVerifyLibrary (LibIndex);
#endif

    ROTOR_PAL_CTOR_TEST_RUN(SSCOREE_INT);

    return lib_handle;
}

static
FARPROC
SetupProc (
    ShimmedSym SymIndex,
    LPCSTR     SymName)
{
    HMODULE lib_handle;
    FARPROC proc;

    ShimmedLib LibIndex = FindSymbolsLib (SymIndex);
    _ASSERTE (LIB_INDEX_VALID (LibIndex));

#ifdef TRACE_LOADS
    printf ("SscoreeShimGetProcAddress: Loading library %d (%S)\n",
            LibIndex, g_Libs[LibIndex].Name);
#endif

    lib_handle = g_Libs[LibIndex].Handle;
    if (lib_handle == NULL) {
        lib_handle = SetupLib (LibIndex);
        if (lib_handle == NULL)
            return NULL;
    }
    _ASSERTE (lib_handle);

    proc = g_Syms[SymIndex].Proc;
    if (proc == NULL) {
        proc = GetProcAddress (lib_handle, SymName);
        if (!proc) {
#ifdef _DEBUG
            fprintf (stderr,
                        "SscoreeShimGetProcAddress: GetProcAddress (\"%s\") failed\n",
                        SymName);
#endif
            return proc;
        }
        g_Syms[SymIndex].Proc = proc;
    }

    return proc;
}

FARPROC
SscoreeShimGetProcAddress (
    ShimmedSym SymIndex,
    LPCSTR     SymName)
{
    FARPROC proc;

#ifdef TRACE_LOADS
    printf ("SscoreeShimGetProcAddress: Loading Symbol %d (%s)\n",
            SymIndex, g_Syms[SymIndex].Name);
#endif

    _ASSERTE (SYM_INDEX_VALID (SymIndex));
    _ASSERTE (SymName);
    _ASSERTE (g_Syms[SymIndex].Name);
    _ASSERTE (!strcmp (g_Syms[SymIndex].Name, SymName));

    proc = g_Syms[SymIndex].Proc;

    if (proc == NULL) {
        proc = SetupProc(SymIndex, SymName);
    }

    return proc;
}

HRESULT HRESULT_FROM_GetLastError() // from utilcode.h
{
    DWORD dw = GetLastError();
    // Make sure we return a failure
    if (dw == ERROR_SUCCESS)
    {
        _ASSERTE(!"We were expecting to get an error code, but a success code is being returned. Check this code path for Everett!");
        return E_FAIL;
    }
    else
        return HRESULT_FROM_WIN32(dw);
}
