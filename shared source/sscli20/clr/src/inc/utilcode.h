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
// UtilCode.h
//
// Utility functions implemented in UtilCode.lib.
//
// Please note that the debugger has a parallel copy of this file elsewhere
// (in Debug\Shell\DebuggerUtil.h)                                                  
//
//*****************************************************************************
#ifndef __UtilCode_h__
#define __UtilCode_h__


#include "crtwrap.h"
#include "winwrap.h"
#include "safewrap.h"
#include <stdio.h>
#include <malloc.h>
#include <ole2.h>
#include <oleauto.h>
#include <limits.h>
#include <daccess.h>
#include "clrhost.h"
#include "debugmacros.h"
#include "dbgalloc.h"
#include "corhlpr.h"
#include "safegetfilesize.h"
#include "winnls.h"
#include "clrtypes.h"
#include "check.h"
#include "safemath.h"

#include "contract.h"
#include "entrypoints.h"

#if !defined(_DEBUG_IMPL) && defined(_DEBUG) && !defined(DACCESS_COMPILE)
#define _DEBUG_IMPL 1
#endif

//********** Max Allocation Request support ***********************************
#ifdef _DEBUG_IMPL
#define MAXALLOC
#endif // _DEBUG

typedef LPCSTR  LPCUTF8;
typedef LPSTR   LPUTF8;

#include "nsutilpriv.h"

#include "stdmacros.h"

/*
// This is for WinCE
#ifdef VERIFY
#undef VERIFY
#endif

#ifdef _ASSERTE
#undef _ASSERTE
#endif
*/

//********** Macros. **********************************************************
#ifndef FORCEINLINE
 #if _MSC_VER < 1200
   #define FORCEINLINE inline
 #else
   #define FORCEINLINE __forceinline
 #endif
#endif


#include <stddef.h> // for offsetof

#ifndef NumItems
// Number of elements in a fixed-size array
#define NumItems(s) (sizeof(s) / sizeof(s[0]))
#endif

#ifndef StrLen
// Number of characters in a string literal. Excludes terminating NULL.
#define StrLen(str) (NumItems(str) - 1)
#endif


#define IS_DIGIT(ch) ((ch >= L'0') && (ch <= L'9'))
#define DIGIT_TO_INT(ch) (ch - L'0')
#define INT_TO_DIGIT(i) ((WCHAR)(L'0' + i))

#define IS_HEXDIGIT(ch) (((ch >= L'a') && (ch <= L'f')) || \
                         ((ch >= L'A') && (ch <= L'F')))
#define HEXDIGIT_TO_INT(ch) ((towlower(ch) - L'a') + 10)
#define INT_TO_HEXDIGIT(i) ((WCHAR)(L'a' + (i - 10)))


// Helper will 4 byte align a value, rounding up.
#define ALIGN4BYTE(val) (((val) + 3) & ~0x3)

// These macros can be used to cast a pointer to a derived/base class to the
// opposite object.  You can do this in a template using a normal cast, but
// the compiler will generate 4 extra insructions to keep a null pointer null
// through the adjustment.  The problem is if it is contained it can never be
// null and those 4 instructions are dead code.
#define INNER_TO_OUTER(p, I, O) ((O *) ((char *) p - (int) ((char *) ((I *) ((O *) 8)) - 8)))
#define OUTER_TO_INNER(p, I, O) ((I *) ((char *) p + (int) ((char *) ((I *) ((O *) 8)) - 8)))

#ifdef  _DEBUG
#define DEBUGARG(x)         , x
#else
#define DEBUGARG(x)
#endif

#ifndef sizeofmember
// Returns the size of a class or struct member.
#define sizeofmember(c,m) (sizeof(((c*)0)->m))
#endif

//=--------------------------------------------------------------------------=
// Prefast helpers.
//

#include "safemath.h"

//=--------------------------------------------------------------------------=
// string helpers.

//
// given and ANSI String, copy it into a wide buffer.
// be careful about scoping when using this macro!
//
// how to use the below two macros:
//
//  ...
//  LPSTR pszA;
//  pszA = MyGetAnsiStringRoutine();
//  MAKE_WIDEPTR_FROMANSI(pwsz, pszA);
//  MyUseWideStringRoutine(pwsz);
//  ...
//
// similarily for MAKE_ANSIPTR_FROMWIDE.  note that the first param does not
// have to be declared, and no clean up must be done.
//

// We'll define an upper limit that allows multiplication by 4 (the max
// bytes/char in UTF-8) but still remains positive, and allows some room for pad.
// Under normal circumstances, we should never get anywhere near this limit.
#define MAKE_MAX_LENGTH 0x1fffff00

#ifndef MAKE_TOOLONGACTION
#define MAKE_TOOLONGACTION RaiseException(EXCEPTION_INT_OVERFLOW, EXCEPTION_NONCONTINUABLE, 0, 0)
#endif

#ifndef MAKE_TRANSLATIONFAILED
#define MAKE_TRANSLATIONFAILED RaiseException(ERROR_NO_UNICODE_TRANSLATION, EXCEPTION_NONCONTINUABLE, 0, 0)
#endif

// This version throws on conversion errors (ie, no best fit character
// mapping to characters that look similar, and no use of the default char
// ('?') when printing out unrepresentable characters.  Use this method for
// most development in the EE, especially anything like metadata or class
// names.  See the BESTFIT version if you're printing out info to the console.
#define MAKE_MULTIBYTE_FROMWIDE(ptrname, widestr, codepage) \
    long __l##ptrname = (long)wcslen(widestr);        \
    if (__l##ptrname > MAKE_MAX_LENGTH)         \
        MAKE_TOOLONGACTION;                     \
    __l##ptrname = (long)((__l##ptrname + 1) * 2 * sizeof(char)); \
    CQuickBytes __CQuickBytes##ptrname; \
    __CQuickBytes##ptrname.AllocThrows(__l##ptrname); \
    BOOL __b##ptrname; \
    DWORD __cBytes##ptrname = WszWideCharToMultiByte(codepage, WC_NO_BEST_FIT_CHARS, widestr, -1, (LPSTR)__CQuickBytes##ptrname.Ptr(), __l##ptrname-1, NULL, &__b##ptrname); \
    if (__b##ptrname || (__cBytes##ptrname == 0 && (widestr[0] != L'\0'))) { \
        _ASSERTE(!"Strict Unicode -> MultiByte character conversion failure!"); \
        MAKE_TRANSLATIONFAILED; \
    } \
    LPSTR ptrname = (LPSTR)__CQuickBytes##ptrname.Ptr()

// This version does best fit character mapping and also allows the use
// of the default char ('?') for any Unicode character that isn't
// representable.  This is reasonable for writing to the console, but
// shouldn't be used for most string conversions.
#define MAKE_MULTIBYTE_FROMWIDE_BESTFIT(ptrname, widestr, codepage) \
    long __l##ptrname = (long)wcslen(widestr);        \
    if (__l##ptrname > MAKE_MAX_LENGTH)         \
        MAKE_TOOLONGACTION;                     \
    __l##ptrname = (long)((__l##ptrname + 1) * 2 * sizeof(char)); \
    CQuickBytes __CQuickBytes##ptrname; \
    __CQuickBytes##ptrname.AllocThrows(__l##ptrname); \
    DWORD __cBytes##ptrname = WszWideCharToMultiByte(codepage, 0, widestr, -1, (LPSTR)__CQuickBytes##ptrname.Ptr(), __l##ptrname-1, NULL, NULL); \
    if (__cBytes##ptrname == 0 && __l##ptrname != 0) { \
        _ASSERTE(!"Unicode -> MultiByte (with best fit mapping) character conversion failed"); \
        MAKE_TRANSLATIONFAILED; \
    } \
    LPSTR ptrname = (LPSTR)__CQuickBytes##ptrname.Ptr()

// Use for anything critical other than output to console, where weird
// character mappings are unacceptable.
#define MAKE_ANSIPTR_FROMWIDE(ptrname, widestr) MAKE_MULTIBYTE_FROMWIDE(ptrname, widestr, CP_ACP)

// Use for output to the console.
#define MAKE_ANSIPTR_FROMWIDE_BESTFIT(ptrname, widestr) MAKE_MULTIBYTE_FROMWIDE_BESTFIT(ptrname, widestr, CP_ACP)

#define MAKE_WIDEPTR_FROMANSI(ptrname, ansistr) \
    CQuickBytes __qb##ptrname; \
    long __l##ptrname; \
    __l##ptrname = WszMultiByteToWideChar(CP_ACP, 0, ansistr, -1, 0, 0); \
    if (__l##ptrname > MAKE_MAX_LENGTH) \
        MAKE_TOOLONGACTION; \
    LPWSTR ptrname = (LPWSTR) __qb##ptrname.AllocThrows((__l##ptrname+1)*sizeof(WCHAR));  \
    if (WszMultiByteToWideChar(CP_ACP, MB_ERR_INVALID_CHARS, ansistr, -1, ptrname, __l##ptrname) == 0) { \
        _ASSERTE(!"ANSI -> Unicode translation failed!"); \
        MAKE_TRANSLATIONFAILED; \
    }

#define MAKE_WIDEPTR_FROMANSI_NOTHROW(ptrname, ansistr) \
    CQuickBytes __qb##ptrname; \
    LPWSTR ptrname = 0; \
    long __l##ptrname; \
    __l##ptrname = WszMultiByteToWideChar(CP_ACP, 0, ansistr, -1, 0, 0); \
    if (__l##ptrname <= MAKE_MAX_LENGTH) { \
        ptrname = (LPWSTR) __qb##ptrname.AllocNoThrow((__l##ptrname+1)*sizeof(WCHAR));  \
        if (ptrname) { \
            if (WszMultiByteToWideChar(CP_ACP, MB_ERR_INVALID_CHARS, ansistr, -1, ptrname, __l##ptrname) != 0) { \
                ptrname[__l##ptrname] = 0; \
            } else { \
                _ASSERTE(!"ANSI -> Unicode translation failed!"); \
                ptrname = 0; \
            } \
        } \
    }

#define MAKE_UTF8PTR_FROMWIDE(ptrname, widestr) \
    CQuickBytes __qb##ptrname; \
    long __l##ptrname = (long)wcslen(widestr); \
    if (__l##ptrname > MAKE_MAX_LENGTH) \
        MAKE_TOOLONGACTION; \
    __l##ptrname = (long)((__l##ptrname + 1) * 2 * sizeof(char)); \
    LPUTF8 ptrname = (LPUTF8) __qb##ptrname .AllocThrows(__l##ptrname); \
    INT32 __lresult##ptrname=WszWideCharToMultiByte(CP_UTF8, 0, widestr, -1, ptrname, __l##ptrname-1, NULL, NULL); \
    if ((__lresult##ptrname==0) && (((LPCWSTR)widestr)[0] != L'\0')) { \
        if (::GetLastError()==ERROR_INSUFFICIENT_BUFFER) { \
            INT32 __lsize##ptrname=WszWideCharToMultiByte(CP_UTF8, 0, widestr, -1, NULL, 0, NULL, NULL); \
            ptrname = (LPSTR) __qb##ptrname.AllocThrows(__lsize##ptrname); \
            if (0==WszWideCharToMultiByte(CP_UTF8, 0, widestr, -1, ptrname, __lsize##ptrname, NULL, NULL)) { \
                MAKE_TRANSLATIONFAILED; \
            } \
        } \
        else { \
            MAKE_TRANSLATIONFAILED; \
        } \
    }

#define MAKE_UTF8PTR_FROMWIDE_NOTHROW(ptrname, widestr) \
    CQuickBytes __qb##ptrname; \
    long __l##ptrname = (long)wcslen(widestr); \
    LPUTF8 ptrname = 0; \
    if (__l##ptrname <= MAKE_MAX_LENGTH) { \
        __l##ptrname = (long)((__l##ptrname + 1) * 2 * sizeof(char)); \
        ptrname = (LPUTF8) __qb##ptrname.AllocNoThrow(__l##ptrname); \
    } \
    if (ptrname) { \
        INT32 __lresult##ptrname=WszWideCharToMultiByte(CP_UTF8, 0, widestr, -1, ptrname, __l##ptrname-1, NULL, NULL); \
        if ((__lresult##ptrname==0) && (((LPCWSTR)widestr)[0] != L'\0')) { \
            if (::GetLastError()==ERROR_INSUFFICIENT_BUFFER) { \
                INT32 __lsize##ptrname=WszWideCharToMultiByte(CP_UTF8, 0, widestr, -1, NULL, 0, NULL, NULL); \
                ptrname = (LPSTR) __qb##ptrname .AllocNoThrow(__lsize##ptrname); \
                if (ptrname) { \
                    if (WszWideCharToMultiByte(CP_UTF8, 0, widestr, -1, ptrname, __lsize##ptrname, NULL, NULL) != 0) { \
                        ptrname[__l##ptrname] = 0; \
                    } else { \
                        _ASSERTE(!"Unicode -> UTF-8 translation error!  (Do you have unpaired Unicode surrogate chars in your string?)"); \
                        ptrname = 0; \
                    } \
                } \
            } \
            else { \
                _ASSERTE(!"Unicode -> UTF-8 translation error!  (Do you have unpaired Unicode surrogate chars in your string?)"); \
                ptrname = 0; \
            } \
        } \
    } \

#define MAKE_WIDEPTR_FROMUTF8N(ptrname, utf8str, n8chrs) \
    CQuickBytes __qb##ptrname; \
    long __l##ptrname; \
    __l##ptrname = WszMultiByteToWideChar(CP_UTF8, 0, utf8str, n8chrs, 0, 0); \
    if (__l##ptrname > MAKE_MAX_LENGTH) \
        MAKE_TOOLONGACTION; \
    LPWSTR ptrname = (LPWSTR) __qb##ptrname .AllocThrows((__l##ptrname+1)*sizeof(WCHAR)); \
    if (0==WszMultiByteToWideChar(CP_UTF8, MB_ERR_INVALID_CHARS, utf8str, n8chrs, ptrname, __l##ptrname)) { \
        _ASSERTE(!"UTF-8 -> Unicode translation failed!"); \
        MAKE_TRANSLATIONFAILED; \
    } \
    ptrname[__l##ptrname] = 0;

#define MAKE_WIDEPTR_FROMUTF8(ptrname, utf8str)           MAKE_WIDEPTR_FROMUTF8N(ptrname, utf8str, -1)

#define MAKE_WIDEPTR_FROMUTF8N_NOTHROW(ptrname, utf8str, n8chrs) \
    CQuickBytes __qb##ptrname; \
    long __l##ptrname; \
    LPWSTR ptrname = 0; \
    __l##ptrname = WszMultiByteToWideChar(CP_UTF8, 0, utf8str, n8chrs, 0, 0); \
    if (__l##ptrname <= MAKE_MAX_LENGTH) { \
        ptrname = (LPWSTR) __qb##ptrname.AllocNoThrow((__l##ptrname+1)*sizeof(WCHAR));  \
        if (ptrname) { \
            if (WszMultiByteToWideChar(CP_UTF8, MB_ERR_INVALID_CHARS, utf8str, n8chrs, ptrname, __l##ptrname) != 0) { \
                ptrname[__l##ptrname] = 0; \
            } else { \
                _ASSERTE(!"UTF-8 -> Unicode translation failed!"); \
                ptrname = 0; \
            } \
        } \
    }

#define MAKE_WIDEPTR_FROMUTF8_NOTHROW(ptrname, utf8str)   MAKE_WIDEPTR_FROMUTF8N_NOTHROW(ptrname, utf8str, -1)

// This method takes the number of characters
#define MAKE_MULTIBYTE_FROMWIDEN(ptrname, widestr, _nCharacters, _pCnt, codepage)        \
    CQuickBytes __qb##ptrname; \
    long __l##ptrname; \
    __l##ptrname = WszWideCharToMultiByte(codepage, WC_NO_BEST_FIT_CHARS, widestr, _nCharacters, NULL, 0, NULL, NULL);           \
    if (__l##ptrname > MAKE_MAX_LENGTH) \
        MAKE_TOOLONGACTION; \
    ptrname = (LPUTF8) __qb##ptrname .AllocThrows(__l##ptrname+1); \
    BOOL __b##ptrname; \
    DWORD _pCnt = WszWideCharToMultiByte(codepage, WC_NO_BEST_FIT_CHARS, widestr, _nCharacters, ptrname, __l##ptrname, NULL, &__b##ptrname);  \
    if (__b##ptrname || (_pCnt == 0 && _nCharacters > 0)) { \
        _ASSERTE("Strict Unicode -> MultiByte character translation failed!"); \
        MAKE_TRANSLATIONFAILED; \
    } \
    ptrname[__l##ptrname] = 0;

#define MAKE_MULTIBYTE_FROMWIDEN_BESTFIT(ptrname, widestr, _nCharacters, _pCnt, codepage)        \
    CQuickBytes __qb##ptrname; \
    long __l##ptrname; \
    __l##ptrname = WszWideCharToMultiByte(codepage, 0, widestr, _nCharacters, NULL, 0, NULL, NULL);           \
    if (__l##ptrname > MAKE_MAX_LENGTH) \
        MAKE_TOOLONGACTION; \
    ptrname = (LPUTF8) __qb##ptrname .AllocThrows(__l##ptrname+1); \
    DWORD _pCnt = WszWideCharToMultiByte(codepage, 0, widestr, _nCharacters, ptrname, __l##ptrname, NULL, NULL);  \
    if (_pCnt == 0 && _nCharacters > 0) { \
        _ASSERTE("Unicode -> MultiByte character translation failed!"); \
        MAKE_TRANSLATIONFAILED; \
    } \
    ptrname[__l##ptrname] = 0;

#define MAKE_ANSIPTR_FROMWIDEN(ptrname, widestr, _nCharacters, _pCnt)        \
       MAKE_MULTIBYTE_FROMWIDEN(ptrname, widestr, _nCharacters, _pCnt, CP_ACP)



//*****************************************************************************
// Placement new is used to new and object at an exact location.  The pointer
// is simply returned to the caller without actually using the heap.  The
// advantage here is that you cause the ctor() code for the object to be run.
// This is ideal for heaps of C++ objects that need to get init'd multiple times.
// Example:
//      void        *pMem = GetMemFromSomePlace();
//      Foo *p = new (pMem) Foo;
//      DoSomething(p);
//      p->~Foo();
//*****************************************************************************
#ifndef __PLACEMENT_NEW_INLINE
#define __PLACEMENT_NEW_INLINE
inline void *__cdecl operator new(size_t, void *_P)
{
    LEAF_CONTRACT;

    return (_P);
}
#endif // __PLACEMENT_NEW_INLINE


/********************************************************************************/
/* portability helpers */
#define IN_WIN64(x)
#define IN_WIN32(x)     x


#ifdef MAXALLOC
// these defs allow testers to specify max number of requests for an allocation before
// returning outofmemory
void * AllocMaxNew( size_t n, void **ppvCallstack, BOOL isArray);
class AllocRequestManager {
  public:
    UINT m_newRequestCount;
    UINT m_maxRequestCount;
    AllocRequestManager(LPCTSTR key);
    BOOL CheckRequest(size_t n);
    void UndoRequest();
};
#endif


#ifdef _DEBUG_IMPL
HRESULT _OutOfMemory(LPCSTR szFile, int iLine);
#define OutOfMemory() _OutOfMemory(__FILE__, __LINE__)
#else
inline HRESULT OutOfMemory()
{
    LEAF_CONTRACT;
    return (E_OUTOFMEMORY);
}
#endif

//*****************************************************************************
// Handle accessing localizable resource strings
//*****************************************************************************

// Notes about the culture callbacks:
// - The language we're operating in can change at *runtime*!
// - A process may operate in *multiple* languages.
//     (ex: Each thread may have it's own language)
// - If we don't care what language we're in (or have no way of knowing),
//     then return a 0-length name and UICULTUREID_DONTCARE for the culture ID.
// - GetCultureName() and the GetCultureId() must be in sync (refer to the
//     same language).
// - We have two functions separate functions for better performance.
//     - The name is used to resolve a directory for MsCorRC.dll.
//     - The id is used as a key to map to a dll hinstance.

// Callback to obtain both the culture name and the culture's parent culture name
typedef HRESULT (*FPGETTHREADUICULTURENAMES)(LPWSTR wszCultureName, SIZE_T dwCultureName, LPWSTR wszParentCultureName, SIZE_T dwParentCultureName);

// Callback to return the culture ID.
const LCID UICULTUREID_DONTCARE = (LCID)-1;
typedef int (*FPGETTHREADUICULTUREID)();

HMODULE CLRLoadLibrary(LPCWSTR lpLibFileName);


BOOL CLRFreeLibrary(HMODULE hModule);

#define LoadStringRC __error("From inside the CLR, use UtilLoadStringRC; LoadStringRC is only meant to be exported.")
#define LoadStringRCEx __error("From inside the CLR, use UtilLoadStringRCEx; LoadStringRC is only meant to be exported.")

// Load a string using the resources for the current module.
STDAPI UtilLoadStringRC(UINT iResouceID, __out_ecount (iMax) LPWSTR szBuffer, int iMax, int bQuiet=FALSE);
STDAPI UtilLoadStringRCEx(LCID lcid, UINT iResourceID, __out_ecount (iMax) LPWSTR szBuffer, int iMax, int bQuiet, int *pcwchUsed);

// Specify callbacks so that UtilLoadStringRC can find out which language we're in.
// If no callbacks specified (or both parameters are NULL), we default to the
// resource dll in the root (which is probably english).
void SetResourceCultureCallbacks(
    FPGETTHREADUICULTURENAMES fpGetThreadUICultureNames,
    FPGETTHREADUICULTUREID fpGetThreadUICultureId
);

void GetResourceCultureCallbacks(
        FPGETTHREADUICULTURENAMES* fpGetThreadUICultureNames,
        FPGETTHREADUICULTUREID* fpGetThreadUICultureId
);


//*****************************************************************************
// Must associate each handle to an instance of a resource dll with the int
// that it represents
//*****************************************************************************
typedef HSATELLITE HRESOURCEDLL;

class CCulturedHInstance
{
public:
    CCulturedHInstance() {
        LEAF_CONTRACT;
        m_LangId = 0;
        m_hInst = NULL;
    };

    int             m_LangId;
    HRESOURCEDLL    m_hInst;
};

//*****************************************************************************
// CCompRC manages string Resource access for COM+. This includes loading
// the MsCorRC.dll for resources as well allowing each thread to use a
// a different localized version.
//*****************************************************************************
class CCompRC
{
public:

    CCompRC()
    {
        // This constructor will be fired up on startup. Make sure it doesn't
        // do anything besides zero-out out values.

        m_fpGetThreadUICultureId = NULL;
        m_fpGetThreadUICultureNames = NULL;
        

        m_pHash = NULL;
        m_nHashSize = 0;

    }// CCompRC

    HRESULT Init(__in_opt __in_z WCHAR* pResourceFile);
    void Destroy();

    HRESULT LoadString(UINT iResourceID, __out_ecount (iMax) LPWSTR szBuffer, int iMax, int bQuiet=0, int *pcwchUsed=NULL);
    HRESULT LoadString(LCID langId, UINT iResourceID, __out_ecount (iMax) LPWSTR szBuffer, int iMax, int bQuiet, int *pcwchUsed);

    void SetResourceCultureCallbacks(
        FPGETTHREADUICULTURENAMES fpGetThreadUICultureNames,
        FPGETTHREADUICULTUREID fpGetThreadUICultureId
    );

    void GetResourceCultureCallbacks(
        FPGETTHREADUICULTURENAMES* fpGetThreadUICultureNames,
        FPGETTHREADUICULTUREID* fpGetThreadUICultureId
    );


    // Get the default resource location (mscorrc.dll)
    static CCompRC* GetDefaultResourceDll();
    static void ShutdownDefaultResourceDll();
    static void GetDefaultCallbacks(
                    FPGETTHREADUICULTURENAMES* fpGetThreadUICultureNames,
                    FPGETTHREADUICULTUREID* fpGetThreadUICultureId)
    {
        WRAPPER_CONTRACT;
        m_DefaultResourceDll.GetResourceCultureCallbacks(
                    fpGetThreadUICultureNames,
                    fpGetThreadUICultureId);
    }

    static void SetDefaultCallbacks(
                FPGETTHREADUICULTURENAMES fpGetThreadUICultureNames,
                FPGETTHREADUICULTUREID fpGetThreadUICultureId)
    {
        WRAPPER_CONTRACT;
        // Either both are NULL or neither are NULL
        _ASSERTE((fpGetThreadUICultureNames != NULL) ==
                 (fpGetThreadUICultureId != NULL));

        m_DefaultResourceDll.SetResourceCultureCallbacks(
                fpGetThreadUICultureNames,
                fpGetThreadUICultureId);
    }

private:
    HRESULT GetLibrary(int langId, HRESOURCEDLL* phInst);
    HRESULT LoadLibraryHelper(HRESOURCEDLL *pHInst,
                              __out_ecount(rcPathSize) WCHAR *rcPath, const DWORD rcPathSize);
    HRESULT LoadLibrary(HRESOURCEDLL * pHInst);
    HRESULT LoadResourceFile(HRESOURCEDLL * pHInst, LPCWSTR lpFileName);

    // We do not have global constructors any more
    static LONG     m_dwDefaultInitialized;
    static CCompRC  m_DefaultResourceDll;

    // We must map between a thread's int and a dll instance.
    // Since we only expect 1 language almost all of the time, we'll special case
    // that and then use a variable size map for everything else.
    CCulturedHInstance m_Primary;
    CCulturedHInstance * m_pHash;
    int m_nHashSize;

    CRITSEC_COOKIE m_csMap;

    static WCHAR* m_pDefaultResource;
    WCHAR* m_pResourceFile;

    // Main accessors for hash
    HRESOURCEDLL LookupNode(int langId);
    HRESULT AddMapNode(int langId, HRESOURCEDLL hInst);

    FPGETTHREADUICULTUREID m_fpGetThreadUICultureId;
    FPGETTHREADUICULTURENAMES m_fpGetThreadUICultureNames;
};

int UtilMessageBox(
                  HWND hWnd,        // Handle to Owner Window
                  UINT uText,       // Resource Identifier for Text message
                  UINT uCaption,    // Resource Identifier for Caption
                  UINT uType,       // Style of MessageBox
                  BOOL ShowFileNameInTitle, // Flag to show FileName in Caption
                  ...);             // Additional Arguments

int UtilMessageBoxNonLocalized(
                  HWND hWnd,        // Handle to Owner Window
                  LPCWSTR lpText,    // Resource Identifier for Text message
                  LPCWSTR lpTitle,   // Resource Identifier for Caption
                  UINT uType,       // Style of MessageBox
                  BOOL ShowFileNameInTitle, // Flag to show FileName in Caption
                  ...);             // Additional Arguments

int UtilMessageBoxVA(
                  HWND hWnd,        // Handle to Owner Window
                  UINT uText,       // Resource Identifier for Text message
                  UINT uCaption,    // Resource Identifier for Caption
                  UINT uType,       // Style of MessageBox
                  BOOL ShowFileNameInTitle, // Flag to show FileName in Caption
                  va_list args);    // Additional Arguments

int UtilMessageBoxNonLocalizedVA(
                  HWND hWnd,        // Handle to Owner Window
                  LPCWSTR lpText,    // Text message
                  LPCWSTR lpCaption, // Caption
                  UINT uType,       // Style of MessageBox
                  BOOL ShowFileNameInTitle, // Flag to show FileName in Caption
                  va_list args);    // Additional Arguments

int UtilMessageBoxCatastrophic(
                  UINT uText,       // Text for MessageBox
                  UINT uTitle,      // Title for MessageBox
                  UINT uType,       // Style of MessageBox
                  BOOL ShowFileNameInTitle, // Flag to show FileName in Caption
                  ...);

int UtilMessageBoxCatastrophicNonLocalized(
                  LPCWSTR lpText,    // Text for MessageBox
                  LPCWSTR lpTitle,   // Title for MessageBox
                  UINT uType,       // Style of MessageBox
                  BOOL ShowFileNameInTitle, // Flag to show FileName in Caption
                  ...);

int UtilMessageBoxCatastrophicVA(
                  UINT uText,       // Text for MessageBox
                  UINT uTitle,      // Title for MessageBox
                  UINT uType,       // Style of MessageBox
                  BOOL ShowFileNameInTitle, // Flag to show FileName in Caption
                  va_list args);    // Additional Arguments

int UtilMessageBoxCatastrophicNonLocalizedVA(
                  LPCWSTR lpText,    // Text for MessageBox
                  LPCWSTR lpTitle,   // Title for MessageBox
                  UINT uType,       // Style of MessageBox
                  BOOL ShowFileNameInTitle, // Flag to show FileName in Caption
                  va_list args);    // Additional Arguments

// The HRESULT_FROM_WIN32 macro evaluates its arguments three times.
inline HRESULT HRESULT_FROM_GetLastError()
{
    WRAPPER_CONTRACT;
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

inline HRESULT HRESULT_FROM_GetLastErrorNA()
{
    WRAPPER_CONTRACT;
    DWORD dw = GetLastError();
    // Make sure we return a failure
    if (dw == ERROR_SUCCESS)
        return E_FAIL;
    else
        return HRESULT_FROM_WIN32(dw);
}

inline HRESULT BadError(HRESULT hr)
{
    LEAF_CONTRACT;
    _ASSERTE(!"Serious Error");
    return (hr);
}

#define TESTANDRETURN(test, hrVal)              \
{                                               \
    int ___test = (int)(test);                  \
    _ASSERTE(___test);                          \
    if (! ___test)                              \
        return hrVal;                           \
}

#define TESTANDRETURNPOINTER(pointer)           \
    TESTANDRETURN(pointer!=NULL, E_POINTER)

#define TESTANDRETURNMEMORY(pointer)            \
    TESTANDRETURN(pointer!=NULL, E_OUTOFMEMORY)

#define TESTANDRETURNHR(hr)                     \
    TESTANDRETURN(SUCCEEDED(hr), hr)

#define TESTANDRETURNARG(argtest)               \
    TESTANDRETURN(argtest, E_INVALIDARG)

// The following is designed to be used within a while loop to test a
// condition, set hr in the enclosing scope value if failed, and leave the block
#define TESTANDBREAK(test, hrVal)               \
{                                               \
    int ___test = (int)(test);                  \
    _ASSERTE(___test);                          \
    if (! ___test) {                            \
        hr = hrVal;                             \
        break;                                  \
    }                                           \
}

#define TESTANDBREAKHR(hr)                      \
    TESTANDBREAK(SUCCEEDED(hr), hr)

// Count the bits in a value in order iBits time.
inline int CountBits(int iNum)
{
    LEAF_CONTRACT;
    int iBits;
    for (iBits=0;  iNum;  iBits++)
        iNum = iNum & (iNum - 1);
    return (iBits);
}

#ifdef _DEBUG
#define RVA_OR_SHOULD_BE_ZERO(RVA, dwParentAttrs, dwMemberAttrs, dwImplFlags, pImport, methodDef) \
        _ASSERTE(RVA != 0                                                                   \
        || IsTdInterface(dwParentAttrs)                                                     \
        || (                                                                                \
            (IsReallyMdPinvokeImpl(dwMemberAttrs)|| IsMiInternalCall(dwImplFlags))          \
            && NDirect::HasNAT_LAttribute(pImport, methodDef)==S_OK)                        \
        || IsMiRuntime(dwImplFlags)                                                         \
        || IsMdAbstract(dwMemberAttrs)                                                      \
        )

#endif //_DEBUG


// Turn a bit in a mask into TRUE or FALSE
template<class T, class U> inline VARIANT_BOOL GetBitFlag(T flags, U bit)
{
    LEAF_CONTRACT;
    if ((flags & bit) != 0)
        return VARIANT_TRUE;
    return VARIANT_FALSE;
}

// Set or clear a bit in a mask, depending on a BOOL.
template<class T, class U> inline void PutBitFlag(T &flags, U bit, VARIANT_BOOL bValue)
{
    LEAF_CONTRACT;
    if (bValue)
        flags |= bit;
    else
        flags &= ~(bit);
}


// prototype for a function to print formatted string to stdout.

int _cdecl PrintfStdOut(LPCWSTR szFmt, ...);


// Used to remove trailing zeros from Decimal types.
// NOTE: Assumes hi32 bits are empty (used for conversions from Cy->Dec)
inline HRESULT DecimalCanonicalize(DECIMAL* dec)
{
    WRAPPER_CONTRACT;

    // Clear the VARENUM field
    (*(USHORT*)dec) = 0;

    // Remove trailing zeros:
    DECIMAL temp;
    DECIMAL templast;
    temp = templast = *dec;

    // Ensure the hi 32 bits are empty (should be if we came from a currency)
    if ((DECIMAL_HI32(temp) != 0) || (DECIMAL_SCALE(temp) > 4))
        return DISP_E_OVERFLOW;

    // Return immediately if dec represents a zero.
    if (DECIMAL_LO32(temp) == 0 && DECIMAL_MID32(temp) == 0)
        return S_OK;

    // Compare to the original to see if we've
    // lost non-zero digits (and make sure we don't overflow the scale BYTE)

    while ((DECIMAL_SCALE(temp) <= 4) && (VARCMP_EQ == VarDecCmp(dec, &temp)))
    {

        templast = temp;

        // Remove the last digit and normalize.  Ignore temp.Hi32
        // as Currency values will have a max of 64 bits of data.
        DECIMAL_SCALE(temp)--;
        UINT64 temp64 = (((UINT64) DECIMAL_MID32(temp)) << 32) + DECIMAL_LO32(temp);
        temp64 /= 10;

        DECIMAL_MID32(temp) = (ULONG)(temp64 >> 32);
        DECIMAL_LO32(temp) = (ULONG)temp64;
    }
    *dec = templast;

    return S_OK;
}

//*****************************************************************************
//
// Paths functions. Use these instead of the CRT.
//
//*****************************************************************************
// secure version! Specify the size of the each buffer in count of elements
void    SplitPath(register const WCHAR *path,
                  __inout_z __inout_ecount_opt(driveSizeInWords) WCHAR *drive, int driveSizeInWords,
                  __inout_z __inout_ecount_opt(dirSizeInWords) WCHAR *dir, int dirSizeInWords,
                  __inout_z __inout_ecount_opt(fnameSizeInWords) WCHAR *fname, size_t fnameSizeInWords,
                  __inout_z __inout_ecount_opt(extSizeInWords) WCHAR *ext, size_t extSizeInWords);
void    MakePath(__out_ecount (MAX_PATH) register WCHAR *path, const WCHAR *drive, const WCHAR *dir, const WCHAR *fname, const WCHAR *ext);
WCHAR * FullPath(__out_ecount (maxlen) WCHAR *UserBuf, const WCHAR *path, size_t maxlen);

HRESULT CompletePathA(
    __out_ecount (MAX_PATH) LPSTR  szPath,  //@parm  [out] Full Path name   (Must be MAX_PATH in size)
    LPCSTR              szRelPath,          //@parm  Relative Path name
    LPCSTR              szAbsPath           //@parm  Absolute Path name portion (NULL uses current path)
    );

//*****************************************************************************
//
// SString version of the path functions.
//
//*****************************************************************************
void    SplitPath(SString path, SString &drive, SString &dir, SString &fname, SString &ext);


inline BOOL ContainsUnmappableChars(LPCWSTR widestr, UINT codePage)
{
    return FALSE;
}



//*****************************************************************************
//
// **** REGUTIL - Static helper functions for reading/writing to Windows registry.
//
//*****************************************************************************


class REGUTIL
{
public:

//*****************************************************************************

    enum CORConfigLevel
    {
        COR_CONFIG_ENV          = 0x01,
        COR_CONFIG_USER         = 0x02,
        COR_CONFIG_MACHINE      = 0x04,
        COR_CONFIG_FUSION       = 0x08,

        COR_CONFIG_REGISTRY     = (COR_CONFIG_USER|COR_CONFIG_MACHINE|COR_CONFIG_FUSION),
        COR_CONFIG_ALL          = (COR_CONFIG_ENV|COR_CONFIG_USER|COR_CONFIG_MACHINE),
    };

    static DWORD GetConfigDWORD(
        LPCWSTR        name,
        DWORD          defValue,
        CORConfigLevel level = COR_CONFIG_ALL,
        BOOL           fPrependCOMPLUS = TRUE);

    static DWORD GetConfigFlag(
        LPCWSTR        name,
        DWORD          bitToSet,
        BOOL           defValue = FALSE);

    static LPWSTR GetConfigString(LPCWSTR name,
                                  BOOL fPrependCOMPLUS = TRUE,
                                  CORConfigLevel level = COR_CONFIG_ALL);

    static void   FreeConfigString(__in __in_z LPWSTR name);

    static LPWSTR EnvGetString(LPCWSTR name, BOOL fPrependCOMPLUS);
    static BOOL UseRegistry();

};

#include "ostype.h"

ULONGLONG CLRGetTickCount64();

//
// Use this function to reset the s_nextCodeStartAllocHint
// after unloading an AppDomain
//
void ResetNextCodeStartAllocHint();

//
// Use this function to setup pMinAddr and pMaxAddr  
// when you need to do a ClrVirtualAllocWithinRange and 
// the range just needs to be in the executable code area,
// but is not otherwise constrained.
//
void InitArgsForAllocInExecutableRange(const BYTE *&pMinAddr,
                                       const BYTE *&pMaxAddr);

//
// Returns true if this range is the standard executable range
// that is returned by  InitArgsForAllocInExecutableRange()
// returns false otherwise
//
bool IsStandardExecutableRange(const BYTE *pMinAddr,
                               const BYTE *pMaxAddr);

//
// Allocate free memory that will be used for executable code
// Handles the special requirements that we have on 64-bit platforms
// where we want the executable memory to be located near mscorwks
//
BYTE * ClrVirtualAllocExecutable(SIZE_T dwSize, 
                                 DWORD flAllocationType,
                                 DWORD flProtect);

//
// Allocate free memory within the range [pMinAddr..pMaxAddr] using
// ClrVirtualQuery to find free memory and ClrVirtualAlloc to allocate it.
// If pStart in non-NULL we start there and work up to pMaxAddr and failing 
//  that we start a second scan at pMinAddr and work upwards back to pStart.
// If pStart is NULL we start at pMinAddr and work upwards to pMaxAddr.
// The result is returned in pStart (it is a reference parameter)
// Note that this function replaces FindFreeSpaceWithinRange
//
HRESULT ClrVirtualAllocWithinRange(const BYTE *&pStart,
                                   const BYTE *pMinAddr,
                                   const BYTE *pMaxAddr,
                                   SIZE_T dwSize, 
                                   DWORD flAllocationType,
                                   DWORD flProtect);

//******************************************************************************
// Returns the number of processors that a process has been configured to run on
//******************************************************************************
int GetCurrentProcessCpuCount();

//*****************************************************************************
// This class exists to get an increasing low resolution counter value fast.
//*****************************************************************************
class CTimeCounter
{
    static DWORD m_iTickCount;          // Last tick count value.
    static ULONG m_iTime;               // Current count.

public:
    enum { TICKSPERSEC = 10 };

//*****************************************************************************
// Get the current time for use in the aging algorithm.
//*****************************************************************************
    static ULONG GetCurrentCounter()    // The current time.
    {
        LEAF_CONTRACT;
        return (m_iTime);
    }

//*****************************************************************************
// Set the current time for use in the aging algorithm.
//*****************************************************************************
    static void UpdateTime()
    {
        WRAPPER_CONTRACT;
        DWORD       iTickCount;         // New tick count.

        // Determine the delta since the last update.
        m_iTime += (((iTickCount = GetTickCount()) - m_iTickCount) + 50) / 100;
        m_iTickCount = iTickCount;
    }

//*****************************************************************************
// Calculate refresh age.
//*****************************************************************************
    static USHORT RefreshAge(long iMilliseconds)
    {
        LEAF_CONTRACT;
        // Figure out the age to allow.
        return ((USHORT)(iMilliseconds / (1000 / TICKSPERSEC)));
    }
};


//*****************************************************************************
// Return != 0 if the bit at the specified index in the array is on and 0 if
// it is off.
//*****************************************************************************
inline int GetBit(const BYTE *pcBits,int iBit)
{
    LEAF_CONTRACT;
    return (pcBits[iBit>>3] & (1 << (iBit & 0x7)));
}

//*****************************************************************************
// Set the state of the bit at the specified index based on the value of bOn.
//*****************************************************************************
inline void SetBit(BYTE *pcBits,int iBit,int bOn)
{
    LEAF_CONTRACT;
    if (bOn)
        pcBits[iBit>>3] |= (1 << (iBit & 0x7));
    else
        pcBits[iBit>>3] &= ~(1 << (iBit & 0x7));
}


template<typename T>
class SimpleListNode
{
public:
    SimpleListNode<T>(const T& _t)
    {
        data = _t;
        next = 0;
    }

    T                  data;
    SimpleListNode<T>* next;
};

template<typename T>
class SimpleList
{
public:
    typedef SimpleListNode<T> NodeType;

    SimpleList<T>()
    {
        head = NULL;
    }

    void LinkHead(NodeType* pNode)
    {
        pNode->next = head;
                      head = pNode;
    }

    NodeType* UnlinkHead()
    {
        NodeType* ret = head;

        if (head)
        {
            head = head->next;
        }
        return ret;
    }

    NodeType* Head()
    {
        return head;
    }

protected:

    NodeType* head;
};


template < typename T, typename U >
struct Pair
{
public:
    typedef Pair< T, U > this_type;
    typedef T first_type;
    typedef U second_type;

    Pair()
    {}

    Pair( T const & t, U const & u )
        : m_first( t )
        , m_second( u )
    {}

    Pair( this_type const & obj )
        : m_first( obj.m_first )
        , m_second( obj.m_second )
    {}

    this_type & operator=( this_type const & obj )
    {
        m_first = obj.m_first;
        m_second = obj.m_second;
        return *this;
    }

    T & First()
    {
        return m_first;
    }

    T const & First() const
    {
        return m_first;
    }

    U & Second()
    {
        return m_second;
    }

    U const & Second() const
    {
        return m_second;
    }

private:
    first_type  m_first;
    second_type m_second;
};


template < typename T, typename U >
Pair< T, U > MakePair( T const & t, U const & u )
{
    return Pair< T, U >( t, u );
}


//*****************************************************************************
// This class implements a dynamic array of structures for which the order of
// the elements is unimportant.  This means that any item placed in the list
// may be swapped to any other location in the list at any time.  If the order
// of the items you place in the array is important, then use the CStructArray
// class.
//*****************************************************************************

template <class T,
          int iGrowInc,
          class ALLOCATOR>
class CUnorderedArrayWithAllocator
{
    int         m_iCount;               // # of elements used in the list.
    int         m_iSize;                // # of elements allocated in the list.
public:
#ifndef DACCESS_COMPILE
    T           *m_pTable;              // Pointer to the list of elements.
#else
    TADDR        m_pTable;              // Pointer to the list of elements.
#endif

public:

#ifndef DACCESS_COMPILE

    CUnorderedArrayWithAllocator() :
        m_iCount(0),
        m_iSize(0),
        m_pTable(NULL)
    {
        LEAF_CONTRACT;
    }
    ~CUnorderedArrayWithAllocator()
    {
        LEAF_CONTRACT;
        // Free the chunk of memory.
        if (m_pTable != NULL)
            ALLOCATOR::Free(this, m_pTable);
    }

    void Clear()
    {
        WRAPPER_CONTRACT;
        m_iCount = 0;
        if (m_iSize > iGrowInc)
        {
            T* tmp = ALLOCATOR::AllocNoThrow(this, iGrowInc);
            if (tmp) {
                ALLOCATOR::Free(this, m_pTable);
                m_pTable = tmp;
                m_iSize = iGrowInc;
            }
        }
    }

    void Clear(int iFirst, int iCount)
    {
        WRAPPER_CONTRACT;
        int     iSize;

        if (iFirst + iCount < m_iCount)
            memmove(&m_pTable[iFirst], &m_pTable[iFirst + iCount], sizeof(T) * (m_iCount - (iFirst + iCount)));

        m_iCount -= iCount;

        iSize = ((m_iCount / iGrowInc) * iGrowInc) + ((m_iCount % iGrowInc != 0) ? iGrowInc : 0);
        if (m_iSize > iGrowInc && iSize < m_iSize)
        {
            T *tmp = ALLOCATOR::AllocNoThrow(this, iSize);
            if (tmp) {
                memcpy (tmp, m_pTable, iSize * sizeof(T));
                delete [] m_pTable;
                m_pTable = tmp;
                m_iSize = iSize;
            }
        }
        _ASSERTE(m_iCount <= m_iSize);
    }

    T *Table()
    {
        LEAF_CONTRACT;
        return (m_pTable);
    }

    T *Append()
    {
        CONTRACTL {
            NOTHROW;
        } CONTRACTL_END;

        // The array should grow, if we can't fit one more element into the array.
        if (m_iSize <= m_iCount && GrowNoThrow() == NULL)
            return (NULL);
        return (&m_pTable[m_iCount++]);
    }

    T *AppendThrowing()
    {
        CONTRACTL {
            THROWS;
        } CONTRACTL_END;

        // The array should grow, if we can't fit one more element into the array.
        if (m_iSize <= m_iCount)
            Grow();
        return (&m_pTable[m_iCount++]);
    }

    void Delete(const T &Entry)
    {
        LEAF_CONTRACT;
        --m_iCount;
        for (int i=0; i <= m_iCount; ++i)
            if (m_pTable[i] == Entry)
            {
                m_pTable[i] = m_pTable[m_iCount];
                return;
            }

        // Just in case we didn't find it.
        ++m_iCount;
    }

    void DeleteByIndex(int i)
    {
        LEAF_CONTRACT;
        --m_iCount;
        m_pTable[i] = m_pTable[m_iCount];
    }

    void Swap(int i,int j)
    {
        LEAF_CONTRACT;
        T       tmp;

        if (i == j)
            return;
        tmp = m_pTable[i];
        m_pTable[i] = m_pTable[j];
        m_pTable[j] = tmp;
    }

#else

    TADDR Table()
    {
        LEAF_CONTRACT;
        return (m_pTable);
    }

    void EnumMemoryRegions(void)
    {
        DacEnumMemoryRegion(m_pTable, m_iCount * sizeof(T));
    }

#endif // #ifndef DACCESS_COMPILE

    USHORT Count()
    {
        LEAF_CONTRACT;
        return (m_iCount);
    }

private:
    T *Grow();
    T *GrowNoThrow();
};


#ifndef DACCESS_COMPILE

//*****************************************************************************
// Increase the size of the array.
//*****************************************************************************
template <class T,
          int iGrowInc,
          class ALLOCATOR>
T *CUnorderedArrayWithAllocator<T,iGrowInc,ALLOCATOR>::GrowNoThrow()  // NULL if can't grow.
{
    WRAPPER_CONTRACT;
    T       *pTemp;

    // try to allocate memory for reallocation.
    if ((pTemp = ALLOCATOR::AllocNoThrow(this, m_iSize+iGrowInc)) == NULL)
        return (NULL);
    memcpy (pTemp, m_pTable, m_iSize*sizeof(T));
    ALLOCATOR::Free(this, m_pTable);
    m_pTable = pTemp;
    m_iSize += iGrowInc;
    _ASSERTE(m_iSize > 0);
    return (pTemp);
}

template <class T,
          int iGrowInc,
          class ALLOCATOR>
T *CUnorderedArrayWithAllocator<T,iGrowInc,ALLOCATOR>::Grow()  // exception if can't grow.
{
    WRAPPER_CONTRACT;
    T       *pTemp;

    // try to allocate memory for reallocation.
    pTemp = ALLOCATOR::AllocThrowing(this, m_iSize+iGrowInc);
    memcpy (pTemp, m_pTable, m_iSize*sizeof(T));
    ALLOCATOR::Free(this, m_pTable);
    m_pTable = pTemp;
    m_iSize += iGrowInc;
    _ASSERTE(m_iSize > 0);
    return (pTemp);
}

#endif // #ifndef DACCESS_COMPILE


template <class T>
class CUnorderedArray__Allocator
{
public:

    static T *AllocThrowing (void*, int nElements)
    {
        return new T[nElements];
    }

    static T *AllocNoThrow (void*, int nElements)
    {
        return new (nothrow) T[nElements];
    }

    static void Free (void*, T *pTable)
    {
        delete [] pTable;
    }
};


template <class T,int iGrowInc>
class CUnorderedArray : public CUnorderedArrayWithAllocator<T, iGrowInc, CUnorderedArray__Allocator<T> >
{
public:

    CUnorderedArray ()
    {
        LEAF_CONTRACT;
    }
};


//Used by the debugger.  Included here in hopes somebody else might, too
typedef CUnorderedArray<SIZE_T, 17> SIZE_T_UNORDERED_ARRAY;


//*****************************************************************************
// This class implements a dynamic array of structures for which the insert
// order is important.  Inserts will slide all elements after the location
// down, deletes slide all values over the deleted item.  If the order of the
// items in the array is unimportant to you, then CUnorderedArray may provide
// the same feature set at lower cost.
//*****************************************************************************
class CStructArray
{
    BYTE        *m_pList;               // Pointer to the list of elements.
    int         m_iCount;               // # of elements used in the list.
    int         m_iSize;                // # of elements allocated in the list.
    int         m_iGrowInc;             // Growth increment.
    short       m_iElemSize;            // Size of an array element.
    bool        m_bFree;                // true if data is automatically maintained.

public:
    CStructArray(short iElemSize, short iGrowInc = 1) :
        m_pList(NULL),
        m_iCount(0),
        m_iSize(0),
        m_iGrowInc(iGrowInc),
        m_iElemSize(iElemSize),
        m_bFree(true)
    {
        LEAF_CONTRACT;
    }
    ~CStructArray()
    {
        WRAPPER_CONTRACT;
        Clear();
    }

    void *Insert(int iIndex);
    void *InsertThrowing(int iIndex);
    void *Append();
    void *AppendThrowing();
    int AllocateBlock(int iCount);
    void AllocateBlockThrowing(int iCount);
    void Delete(int iIndex);
    void *Ptr()
    {
        LEAF_CONTRACT;
        return (m_pList);
    }
    void *Get(long iIndex)
    {
        WRAPPER_CONTRACT;
        _ASSERTE(iIndex < m_iCount);
        return ((void *) ((size_t) Ptr() + (iIndex * m_iElemSize)));
    }
    int Size()
    {
        LEAF_CONTRACT;
        return (m_iCount * m_iElemSize);
    }
    int Count()
    {
        LEAF_CONTRACT;
        return (m_iCount);
    }
    void Clear();
    void ClearCount()
    {
        LEAF_CONTRACT;
        m_iCount = 0;
    }

    void InitOnMem(short iElemSize, void *pList, int iCount, int iSize, int iGrowInc=1)
    {
        LEAF_CONTRACT;
        m_iElemSize = iElemSize;
        m_iGrowInc = (short) iGrowInc;
        m_pList = (BYTE*)pList;
        m_iCount = iCount;
        m_iSize = iSize;
        m_bFree = false;
    }

private:
    void Grow(int iCount);
};


//*****************************************************************************
// This template simplifies access to a CStructArray by removing void * and
// adding some operator overloads.
//*****************************************************************************
template <class T>
class CDynArray : public CStructArray
{
public:
    CDynArray(short iGrowInc=16) :
        CStructArray(sizeof(T), iGrowInc)
    {
        LEAF_CONTRACT;
    }

    T *Insert(long iIndex)
    {
        WRAPPER_CONTRACT;
        return ((T *)CStructArray::Insert((int)iIndex));
    }

    T *InsertThrowing(long iIndex)
    {
        WRAPPER_CONTRACT;
        return ((T *)CStructArray::InsertThrowing((int)iIndex));
    }

    T *Append()
    {
        WRAPPER_CONTRACT;
        return ((T *)CStructArray::Append());
    }

    T *AppendThrowing()
    {
        WRAPPER_CONTRACT;
        return ((T *)CStructArray::AppendThrowing());
    }

    T *Ptr()
    {
        WRAPPER_CONTRACT;
        return ((T *)CStructArray::Ptr());
    }

    T *Get(long iIndex)
    {
        WRAPPER_CONTRACT;
        return (Ptr() + iIndex);
    }
    T &operator[](long iIndex)
    {
        WRAPPER_CONTRACT;
        return (*(Ptr() + iIndex));
    }
    int ItemIndex(T *p)
    {
        WRAPPER_CONTRACT;
        return (((long)(LONG_PTR)p - (long)(LONG_PTR)Ptr()) / sizeof(T));
    }
    void Move(int iFrom, int iTo)
    {
        WRAPPER_CONTRACT;
        T       tmp;

        _ASSERTE(iFrom >= 0 && iFrom < Count() &&
                 iTo >= 0 && iTo < Count());

        tmp = *(Ptr() + iFrom);
        if (iTo > iFrom)
            memmove(Ptr() + iFrom, Ptr() + iFrom + 1, (iTo - iFrom) * sizeof(T));
        else
            memmove(Ptr() + iFrom + 1, Ptr() + iFrom, (iTo - iFrom) * sizeof(T));
        *(Ptr() + iTo) = tmp;
    }
};

// Some common arrays.
typedef CDynArray<int> INTARRAY;
typedef CDynArray<short> SHORTARRAY;
typedef CDynArray<long> LONGARRAY;
typedef CDynArray<USHORT> USHORTARRAY;
typedef CDynArray<ULONG> ULONGARRAY;
typedef CDynArray<BYTE> BYTEARRAY;
typedef CDynArray<mdToken> TOKENARRAY;

template <class T> class CStackArray : public CStructArray
{
public:
    CStackArray(int iGrowInc=4) :
        CStructArray(sizeof(T), iGrowInc),
        m_curPos(0)
    {
        LEAF_CONTRACT;
    }

    void Push(T p)
    {
        WRAPPER_CONTRACT;
        // We should only inc m_curPos after we grow the array.
        T *pT = (T *)CStructArray::InsertThrowing(m_curPos);
        m_curPos ++;
        *pT = p;
    }

    T * Pop()
    {
        WRAPPER_CONTRACT;
        T * retPtr;

        _ASSERTE(m_curPos > 0);

        retPtr = (T *)CStructArray::Get(m_curPos-1);
        CStructArray::Delete(m_curPos--);

        return (retPtr);
    }

    int Count()
    {
        LEAF_CONTRACT;
        return(m_curPos);
    }

private:
    int m_curPos;
};


//*****************************************************************************
// This template manages a list of free entries by their 0 based offset.  By
// making it a template, you can use whatever size free chain will match your
// maximum count of items.  -1 is reserved.
//*****************************************************************************
template <class T> class TFreeList
{
public:
    void Init(
        T           *rgList,
        int         iCount)
    {
        LEAF_CONTRACT;
        // Save off values.
        m_rgList = rgList;
        m_iCount = iCount;
        m_iNext = 0;

        // Init free list.
        int i;
        for (i=0;  i<iCount - 1;  i++)
            m_rgList[i] = i + 1;
        m_rgList[i] = (T) -1;
    }

    T GetFreeEntry()                        // Index of free item, or -1.
    {
        LEAF_CONTRACT;
        T           iNext;

        if (m_iNext == (T) -1)
            return (-1);

        iNext = m_iNext;
        m_iNext = m_rgList[m_iNext];
        return (iNext);
    }

    void DelFreeEntry(T iEntry)
    {
        LEAF_CONTRACT;
        _ASSERTE(iEntry < m_iCount);
        m_rgList[iEntry] = m_iNext;
        m_iNext = iEntry;
    }

    // This function can only be used when it is guaranteed that the free
    // array is contigous, for example, right after creation to quickly
    // get a range of items from the heap.
    void ReserveRange(int iCount)
    {
        LEAF_CONTRACT;
        _ASSERTE(iCount < m_iCount);
        _ASSERTE(m_iNext == 0);
        m_iNext = iCount;
    }

private:
    T           *m_rgList;              // List of free info.
    int         m_iCount;               // How many entries to manage.
    T           m_iNext;                // Next item to get.
};


//*****************************************************************************
// This template will manage a pre allocated pool of fixed sized items.
//*****************************************************************************
template <class T, int iMax, class TFree> class TItemHeap
{
public:
    TItemHeap() :
        m_rgList(0),
        m_iCount(0)
    {
        LEAF_CONTRACT;
    }

    ~TItemHeap()
    {
        WRAPPER_CONTRACT;
        Clear();
    }

    TFree ItemIndex(T *p)
    {
        LEAF_CONTRACT;
        _ASSERTE(p >= &m_rgList[0] && p <= &m_rgList[m_iCount - 1]);
        _ASSERTE(((ULONG) p - (ULONG) m_rgList) % sizeof(T) == 0);
        return ((TFree) ((ULONG) p - (ULONG) m_rgList) / sizeof(T));
    }

    // Retrieve an item that lives in the heap itself.  Overflow items
    // cannot be retrieved using this method.
    T *GetAt(int i)
    {
        LEAF_CONTRACT;
        _ASSERTE(i < m_iCount);
        return (&m_rgList[i]);
    }

    T *AddEntry()
    {
        WRAPPER_CONTRACT;

#ifdef _DEBUG    // Instrumentation for OOM fault injection testing
        {
            void *tmp = new (nothrow) char;
            if (!tmp)
            {
                return NULL;
            }
            delete tmp;
        }
#endif


        // Allocate first time.
        if (!InitList())
            return (0);

        // Get an entry from the free list.  If we don't have any left to give
        // out, then simply allocate a single item from the heap.
        TFree       iEntry;
        if ((iEntry = m_Free.GetFreeEntry()) == (TFree) -1)
            return (new (nothrow) T);

        // Run placement new on the heap entry to init it.
        return (new (&m_rgList[iEntry]) T);
    }

    // Free the entry if it belongs to us, if we allocated it from the heap
    // then delete it for real.
    void DelEntry(T *p)
    {
        WRAPPER_CONTRACT;
        if (p >= &m_rgList[0] && p <= &m_rgList[iMax - 1])
        {
            p->~T();
            m_Free.DelFreeEntry(ItemIndex(p));
        }
        else
            delete p;
    }

    // Reserve a range of items from an empty list.
    T *ReserveRange(int iCount)
    {
        WRAPPER_CONTRACT;
        // Don't use on existing list.
        _ASSERTE(m_rgList == 0);
        if (!InitList())
            return (0);

        // Heap must have been created large enough to work.
        _ASSERTE(iCount < m_iCount);

        // Mark the range as used, run new on each item, then return first.
        m_Free.ReserveRange(iCount);
        while (iCount--)
            new (&m_rgList[iCount]) T;
        return (&m_rgList[0]);
    }

    void Clear()
    {
        LEAF_CONTRACT;
        if (m_rgList)
            delete [] (BYTE*)m_rgList;
        m_rgList = 0;
    }

private:
    int InitList()
    {
        WRAPPER_CONTRACT;
        if (m_rgList == 0)
        {
            int         iSize = (iMax * sizeof(T)) + (iMax * sizeof(TFree));
            if ((m_rgList = (T *) new (nothrow) BYTE[iSize]) == 0)
                return (false);
            m_iCount = iMax;
            m_Free.Init((TFree *) &m_rgList[iMax], iMax);
        }
        return (true);
    }

private:
    T           *m_rgList;              // Array of objects to manage.
    int         m_iCount;               // How many items do we have now.
    TFreeList<TFree> m_Free;            // Free list.
};




//*****************************************************************************
//*****************************************************************************
template <class T> class CQuickSort
{
protected:
    T           *m_pBase;                   // Base of array to sort.
private:
    SSIZE_T     m_iCount;                   // How many items in array.
    SSIZE_T     m_iElemSize;                // Size of one element.
public:
    CQuickSort(
        T           *pBase,                 // Address of first element.
        SSIZE_T     iCount) :               // How many there are.
        m_pBase(pBase),
        m_iCount(iCount),
        m_iElemSize(sizeof(T))
    {
        LEAF_CONTRACT;
    }

//*****************************************************************************
// Call to sort the array.
//*****************************************************************************
    inline void Sort()
    {
        WRAPPER_CONTRACT;
        SortRange(0, m_iCount - 1);
    }

protected:
//*****************************************************************************
// Override this function to do the comparison.
//*****************************************************************************
    virtual FORCEINLINE int Compare(        // -1, 0, or 1
        T           *psFirst,               // First item to compare.
        T           *psSecond)              // Second item to compare.
    {
        LEAF_CONTRACT;
        return (memcmp(psFirst, psSecond, sizeof(T)));
//      return (::Compare(*psFirst, *psSecond));
    }

    virtual FORCEINLINE void Swap(
        SSIZE_T     iFirst,
        SSIZE_T     iSecond)
    {
        LEAF_CONTRACT;
        if (iFirst == iSecond) return;
        T sTemp( m_pBase[iFirst] );
        m_pBase[iFirst] = m_pBase[iSecond];
        m_pBase[iSecond] = sTemp;
    }

private:
    inline void SortRange(
        SSIZE_T     iLeft,
        SSIZE_T     iRight)
    {
        WRAPPER_CONTRACT;
        SSIZE_T     iLast;
        SSIZE_T     i;                      // loop variable.

        // if less than two elements you're done.
        if (iLeft >= iRight)
            return;

        // The mid-element is the pivot, move it to the left.
        Swap(iLeft, (iLeft+iRight)/2);
        iLast = iLeft;

        // move everything that is smaller than the pivot to the left.
        for(i = iLeft+1; i <= iRight; i++)
            if (Compare(&m_pBase[i], &m_pBase[iLeft]) < 0)
                Swap(i, ++iLast);

        // Put the pivot to the point where it is in between smaller and larger elements.
        Swap(iLeft, iLast);

        // Sort the each partition.
        SortRange(iLeft, iLast-1);
        SortRange(iLast+1, iRight);
    }


};

//*****************************************************************************
// This template encapsulates a binary search algorithm on the given type
// of data.
//*****************************************************************************
class CBinarySearchILMap;
template <class T> class CBinarySearch
{
    friend class CBinarySearchILMap; // CBinarySearchILMap is to be
        // instantiated once, then used a bunch of different times on
        // a bunch of different arrays.  We need to declare it a friend
        // in order to reset m_pBase and m_iCount

private:
    const T     *m_pBase;                   // Base of array to sort.
    int         m_iCount;                   // How many items in array.

public:
    CBinarySearch(
        const T     *pBase,                 // Address of first element.
        int         iCount) :               // Value to find.
        m_pBase(pBase),
        m_iCount(iCount)
    {
        LEAF_CONTRACT;
    }

//*****************************************************************************
// Searches for the item passed to ctor.
//*****************************************************************************
    const T *Find(                          // Pointer to found item in array.
        const T     *psFind,                // The key to find.
        int         *piInsert = NULL)       // Index to insert at.
    {
        WRAPPER_CONTRACT;
        int         iMid, iFirst, iLast;    // Loop control.
        int         iCmp;                   // Comparison.

        iFirst = 0;
        iLast = m_iCount - 1;
        while (iFirst <= iLast)
        {
            iMid = (iLast + iFirst) / 2;
            iCmp = Compare(psFind, &m_pBase[iMid]);
            if (iCmp == 0)
            {
                if (piInsert != NULL)
                    *piInsert = iMid;
                return (&m_pBase[iMid]);
            }
            else if (iCmp < 0)
                iLast = iMid - 1;
            else
                iFirst = iMid + 1;
        }
        if (piInsert != NULL)
            *piInsert = iFirst;
        return (NULL);
    }

//*****************************************************************************
// Override this function to do the comparison if a comparison operator is
// not valid for your data type (such as a struct).
//*****************************************************************************
    virtual int Compare(                    // -1, 0, or 1
        const T     *psFirst,               // Key you are looking for.
        const T     *psSecond)              // Item to compare to.
    {
        LEAF_CONTRACT;
        return (memcmp(psFirst, psSecond, sizeof(T)));
//      return (::Compare(*psFirst, *psSecond));
    }
};


//*****************************************************************************
// This class manages a bit vector. Allocation is done implicity through the
// template declaration, so no init code is required.  Due to this design,
// one should keep the max items reasonable (eg: be aware of stack size and
// other limitations).  The intrinsic size used to store the bit vector can
// be set when instantiating the vector.  The FindFree method will search
// using sizeof(T) for free slots, so pick a size that works well on your
// platform.
//*****************************************************************************
template <class T, int iMaxItems> class CBitVector
{
    T       m_bits[((iMaxItems/(sizeof(T)*8)) + ((iMaxItems%(sizeof(T)*8)) ? 1 : 0))];
    T       m_Used;

public:
    CBitVector()
    {
        LEAF_CONTRACT;
        memset(&m_bits[0], 0, sizeof(m_bits));
        memset(&m_Used, 0xff, sizeof(m_Used));
    }

//*****************************************************************************
// Get or Set the given bit.
//*****************************************************************************
    int GetBit(int iBit)
    {
        LEAF_CONTRACT;
        return (m_bits[iBit/(sizeof(T)*8)] & (1 << (iBit & ((sizeof(T) * 8) - 1))));
    }

    void SetBit(int iBit, int bOn)
    {
        LEAF_CONTRACT;
        if (bOn)
            m_bits[iBit/(sizeof(T)*8)] |= (1 << (iBit & ((sizeof(T) * 8) - 1)));
        else
            m_bits[iBit/(sizeof(T)*8)] &= ~(1 << (iBit & ((sizeof(T) * 8) - 1)));
    }

//*****************************************************************************
// Find the first free slot and return its index.
//*****************************************************************************
    int FindFree()                          // Index or -1.
    {
        WRAPPER_CONTRACT;
        int         i,j;                    // Loop control.

        // Check a byte at a time.
        for (i=0;  i<sizeof(m_bits);  i++)
        {
            // Look for the first byte with an open slot.
            if (m_bits[i] != m_Used)
            {
                // Walk each bit in the first free byte.
                for (j=i * sizeof(T) * 8;  j<iMaxItems;  j++)
                {
                    // Use first open one.
                    if (GetBit(j) == 0)
                    {
                        SetBit(j, 1);
                        return (j);
                    }
                }
            }
        }

        // No slots open.
        return (-1);
    }
};

//*****************************************************************************
// This class manages a bit vector. Internally, this class uses CQuickBytes, which
// automatically allocates 512 bytes on the stack. So this overhead must be kept in
// mind while using it.
// This class has to be explicitly initialized.
//*****************************************************************************
class CDynBitVector
{
    BYTE    *m_bits;
    BYTE    m_Used;
    int     m_iMaxItem;
    int     m_iBitsSet;
    CQuickBytes m_Bytes;

public:
    CDynBitVector() :
        m_bits(NULL),
        m_iMaxItem(0)
    {
        LEAF_CONTRACT;
    }

    HRESULT Init(int MaxItems)
    {
        WRAPPER_CONTRACT;
        int actualSize = (MaxItems/8) + ((MaxItems%8) ? 1 : 0);

        actualSize = ALIGN4BYTE(actualSize);

        m_Bytes.AllocNoThrow(actualSize);
        if(!m_Bytes)
        {
            return(E_OUTOFMEMORY);
        }

        m_bits = (BYTE *) m_Bytes.Ptr();

        m_iMaxItem = MaxItems;
        m_iBitsSet = 0;

        memset(m_bits, 0, m_Bytes.Size());
        memset(&m_Used, 0xff, sizeof(m_Used));
        return(S_OK);
    }

//*****************************************************************************
// Get, Set the given bit.
//*****************************************************************************
    int GetBit(int iBit)
    {
        LEAF_CONTRACT;
        return (m_bits[iBit/8] & (1 << (iBit & 7)));
    }

    void SetBit(int iBit, int bOn)
    {
        LEAF_CONTRACT;
        if (bOn)
        {
            m_bits[iBit/8] |= (1 << (iBit & 7));
            m_iBitsSet++;
        }
        else
        {
            m_bits[iBit/8] &= ~(1 << (iBit & 7));
            m_iBitsSet--;
        }
    }

//******************************************************************************
// Not all the bits.
//******************************************************************************
    void NotBits()
    {
        WRAPPER_CONTRACT;
        ULONG *pCurrent = (ULONG *)m_bits;
        SIZE_T cbSize = Size()/4;
        int iBitsSet;

        m_iBitsSet = 0;

        for(SIZE_T i=0; i<cbSize; i++, pCurrent++)
        {
            iBitsSet = CountBits(*pCurrent);
            m_iBitsSet += (8 - iBitsSet);
            *pCurrent = ~(*pCurrent);
        }
    }

    BYTE * Ptr()
    {
        LEAF_CONTRACT;
        return(m_bits);
    }

    SIZE_T Size()
    {
        WRAPPER_CONTRACT;
        return(m_Bytes.Size());
    }

    int BitsSet()
    {
        LEAF_CONTRACT;
        return(m_iBitsSet);
    }
};


//*****************************************************************************
// The information that the hash table implementation stores at the beginning
// of every record that can be but in the hash table.
//*****************************************************************************
typedef DPTR(struct HASHENTRY) PTR_HASHENTRY;
struct HASHENTRY
{
    ULONG      iPrev;                  // Previous bucket in the chain.
    ULONG      iNext;                  // Next bucket in the chain.
};

typedef DPTR(struct FREEHASHENTRY) PTR_FREEHASHENTRY;
struct FREEHASHENTRY : HASHENTRY
{
    ULONG      iFree;
};

//*****************************************************************************
// Used by the FindFirst/FindNextEntry functions.  These api's allow you to
// do a sequential scan of all entries.
//*****************************************************************************
struct HASHFIND
{
    ULONG      iBucket;            // The next bucket to look in.
    ULONG      iNext;
};


//
//
//
//
//
class CHashTable
{
    friend class DebuggerRCThread; //RCthread actually needs access to
    //fields of derrived class DebuggerPatchTable

protected:
    TADDR       m_pcEntries;            // Pointer to the array of structs.
    ULONG      m_iEntrySize;           // Size of the structs.

    ULONG      m_iBuckets;
    PTR_ULONG  m_piBuckets;

    INDEBUG(unsigned    m_maxSearch;)   // For evaluating perf characteristics

    HASHENTRY *EntryPtr(ULONG iEntry)
    {
        LEAF_CONTRACT;
        return (PTR_HASHENTRY(m_pcEntries + (iEntry * m_iEntrySize)));
    }

    ULONG     ItemIndex(HASHENTRY *p)
    {
        LEAF_CONTRACT;
        return (ULONG)((PTR_HOST_TO_TADDR(p) - m_pcEntries) / m_iEntrySize);
    }


public:

    CHashTable(
        ULONG      iBuckets) :
        m_pcEntries((TADDR)NULL),
        m_iBuckets(iBuckets)
    {
        LEAF_CONTRACT;

        m_piBuckets = NULL;

        INDEBUG(m_maxSearch = 0;)
    }

    CHashTable() :
        m_pcEntries((TADDR)NULL),
        m_iBuckets(5)
    {
        LEAF_CONTRACT;

        m_piBuckets = NULL;

        INDEBUG(m_maxSearch = 0;)
    }

#ifndef DACCESS_COMPILE

    ~CHashTable()
    {
        LEAF_CONTRACT;
        if (m_piBuckets != NULL)
        {
            delete [] m_piBuckets;
            m_piBuckets = NULL;
        }
    }

//*****************************************************************************
// This is the second part of construction where we do all of the work that
// can fail.  We also take the array of structs here because the calling class
// presumably needs to allocate it in its NewInit.
//*****************************************************************************
    HRESULT NewInit(                    // Return status.
        BYTE        *pcEntries,         // Array of structs we are managing.
        ULONG      iEntrySize);        // Size of the entries.

//*****************************************************************************
// This can be called to change the pointer to the table that the hash table
// is managing.  You might call this if (for example) you realloc the size
// of the table and its pointer is different.
//*****************************************************************************
    void SetTable(
        BYTE        *pcEntries)         // Array of structs we are managing.
    {
        LEAF_CONTRACT;
        m_pcEntries = (TADDR)pcEntries;
    }

//*****************************************************************************
// Clear the hash table as if there were nothing in it.
//*****************************************************************************
    void Clear()
    {
        LEAF_CONTRACT;
        _ASSERTE(m_piBuckets != NULL);
        memset(m_piBuckets, 0xff, m_iBuckets * sizeof(ULONG));
    }

    BYTE *Add(                          // New entry.
        ULONG      iHash,              // Hash value of entry to add.
        ULONG      iIndex);

    void Delete(
        ULONG      iHash,              // Hash value of entry to delete.
        ULONG      iIndex);            // Index of struct in m_pcEntries.

    void Delete(
        ULONG      iHash,              // Hash value of entry to delete.
        HASHENTRY   *psEntry);          // The struct to delete.

//*****************************************************************************
// The item at the specified index has been moved, update the previous and
// next item.
//*****************************************************************************
    void Move(
        ULONG      iHash,              // Hash value for the item.
        ULONG      iNew);              // New location.

#endif // #ifndef DACCESS_COMPILE

//*****************************************************************************
// Return a boolean indicating whether or not this hash table has been inited.
//*****************************************************************************
    int IsInited()
    {
        LEAF_CONTRACT;
        return (m_piBuckets != NULL);
    }

//*****************************************************************************
// Search the hash table for an entry with the specified key value.
//*****************************************************************************
    BYTE *Find(                         // Index of struct in m_pcEntries.
        ULONG      iHash,              // Hash value of the item.
        BYTE        *pcKey);            // The key to match.

//*****************************************************************************
// Search the hash table for the next entry with the specified key value.
//*****************************************************************************
    ULONG FindNext(                    // Index of struct in m_pcEntries.
        BYTE        *pcKey,             // The key to match.
        ULONG      iIndex);            // Index of previous match.

//*****************************************************************************
// Returns the first entry in the first hash bucket and inits the search
// struct.  Use the FindNextEntry function to continue walking the list.  The
// return order is not gauranteed.
//*****************************************************************************
    BYTE *FindFirstEntry(               // First entry found, or 0.
        HASHFIND    *psSrch)            // Search object.
    {
        WRAPPER_CONTRACT;
        if (m_piBuckets == 0)
            return (0);
        psSrch->iBucket = 1;
        psSrch->iNext = m_piBuckets[0];
        return (FindNextEntry(psSrch));
    }

//*****************************************************************************
// Returns the next entry in the list.
//*****************************************************************************
    BYTE *FindNextEntry(                // The next entry, or0 for end of list.
        HASHFIND    *psSrch);           // Search object.

#ifdef DACCESS_COMPILE
    void EnumMemoryRegions(CLRDataEnumMemoryFlags flags,
                           ULONG numEntries);
#endif

protected:
    virtual BOOL Cmp(const BYTE *pc1, const HASHENTRY *pc2) = 0;
};


//*****************************************************************************
// Allocater classes for the CHashTableAndData class.
//
// CVMemData is for VirtualAlloc, for pre-reserving pages, and delaying the commit
// CNewData uses malloc, and throws an exception on OutOfMemory
// CNewDataNoThrow uses malloc but never throwns an exception
//
//*****************************************************************************

class CVMemData
{
public:
    static BYTE *Alloc(int iSize, int iMaxSize)
    {
        WRAPPER_CONTRACT;
        BYTE        *pPtr;

        _ASSERTE((iSize & 4095) == 0);
        _ASSERTE((iMaxSize & 4095) == 0);
        if ((pPtr = (BYTE *) ClrVirtualAlloc(NULL, iMaxSize,
                                             MEM_RESERVE, PAGE_NOACCESS)) == NULL ||
            ClrVirtualAlloc(pPtr, iSize, MEM_COMMIT, PAGE_READWRITE) == NULL)
        {
            if (pPtr)
            {
                ClrVirtualFree(pPtr, 0, MEM_RELEASE);
            }
            return (NULL);
        }
        return (pPtr);
    }
    static void Free(BYTE *pPtr, int iSize)
    {
        WRAPPER_CONTRACT;
        _ASSERTE((iSize & 4095) == 0);
        ClrVirtualFree(pPtr, iSize, MEM_DECOMMIT);
        ClrVirtualFree(pPtr, 0, MEM_RELEASE);
    }
    static BYTE *Grow(BYTE *pPtr, int iCurSize)
    {
        WRAPPER_CONTRACT;
        _ASSERTE((iCurSize & 4095) == 0);
        return ((BYTE *) ClrVirtualAlloc(pPtr + iCurSize, GrowSize(iCurSize), MEM_COMMIT, PAGE_READWRITE));
    }
    static void Clean(BYTE * pData, int iSize)
    {
    }
    static int RoundSize(int iSize)
    {
        LEAF_CONTRACT;
        return ((iSize + 4095) & ~4095);
    }
    static int GrowSize(int iCurSize)
    {
        LEAF_CONTRACT;
        return (4096);
    }
};

class CNewData
{
public:
    static BYTE *Alloc(int iSize, int iMaxSize)
    {
        WRAPPER_CONTRACT;
        return (new BYTE[iSize]);
    }
    static void Free(BYTE *pPtr, int iSize)
    {
        LEAF_CONTRACT;
        delete [] pPtr;
    }
    static BYTE *Grow(BYTE *&pPtr, int iCurSize)
    {
        WRAPPER_CONTRACT;
        BYTE *p = new (nothrow) BYTE[iCurSize + GrowSize(iCurSize)];
        if (p == 0) return (0);
        memcpy (p, pPtr, iCurSize);
        delete [] pPtr;
        pPtr = p;
        return pPtr;
    }
    static void Clean(BYTE * pData, int iSize)
    {
    }
    static int RoundSize(int iSize)
    {
        LEAF_CONTRACT;
        return (iSize);
    }
    static int GrowSize(int iCurSize)
    {
        LEAF_CONTRACT;
        int newSize = (3 * iCurSize) / 2;
        return (newSize < 256) ? 256 : newSize;
    }
};

class CNewDataNoThrow
{
public:
    static BYTE *Alloc(int iSize, int iMaxSize)
    {
        WRAPPER_CONTRACT;
        return (new (nothrow) BYTE[iSize]);
    }
    static void Free(BYTE *pPtr, int iSize)
    {
        LEAF_CONTRACT;
        delete [] pPtr;
    }
    static BYTE *Grow(BYTE *&pPtr, int iCurSize)
    {
        WRAPPER_CONTRACT;
        BYTE *p = new (nothrow) BYTE[iCurSize + GrowSize(iCurSize)];
        if (p == 0) return (0);
        memcpy (p, pPtr, iCurSize);
        delete [] pPtr;
        pPtr = p;
        return pPtr;
    }
    static void Clean(BYTE * pData, int iSize)
    {
    }
    static int RoundSize(int iSize)
    {
        LEAF_CONTRACT;
        return (iSize);
    }
    static int GrowSize(int iCurSize)
    {
        LEAF_CONTRACT;
        int newSize = (3 * iCurSize) / 2;
        return (newSize < 256) ? 256 : newSize;
    }
};


//*****************************************************************************
// This simple code handles a contiguous piece of memory.  Growth is done via
// realloc, so pointers can move.  This class just cleans up the amount of code
// required in every function that uses this type of data structure.
//*****************************************************************************
class CMemChunk
{
public:
    CMemChunk() : m_pbData(0), m_cbSize(0), m_cbNext(0) { }
    ~CMemChunk()
    {
        WRAPPER_CONTRACT;
        Clear();
    }

    BYTE *GetChunk(int cbSize)
    {
        WRAPPER_CONTRACT;

#ifdef _DEBUG    // Instrumentation for OOM fault injection testing
        {
            char *tmp = new (nothrow) char;
            if (!tmp)
            {
                return NULL;
            }
            delete tmp;
        }
#endif

        BYTE *p;
        if (m_cbSize - m_cbNext < cbSize)
        {
            int cbNew = max(cbSize, 512);
            p = new (nothrow) BYTE[m_cbSize + cbNew];
            if (!p) return (0);
            if (m_pbData) {
                memcpy (p, m_pbData, m_cbSize);
                delete [] m_pbData;
            }
            m_pbData = p;
            m_cbSize += cbNew;
        }
        p = m_pbData + m_cbNext;
        m_cbNext += cbSize;
        return (p);
    }

    // Can only delete the last unused chunk.  no free list.
    void DelChunk(BYTE *p, int cbSize)
    {
        LEAF_CONTRACT;
        _ASSERTE(p >= m_pbData && p < m_pbData + m_cbNext);
        if (p + cbSize  == m_pbData + m_cbNext)
            m_cbNext -= cbSize;
    }

    int Size()
    {
        LEAF_CONTRACT;
        return (m_cbSize);
    }

    int Offset()
    {
        LEAF_CONTRACT;
        return (m_cbNext);
    }

    BYTE *Ptr(int cbOffset = 0)
    {
        LEAF_CONTRACT;
        _ASSERTE(m_pbData && m_cbSize);
        _ASSERTE(cbOffset < m_cbSize);
        return (m_pbData + cbOffset);
    }

    void Clear()
    {
        LEAF_CONTRACT;
        if (m_pbData)
            delete [] m_pbData;
        m_pbData = 0;
        m_cbSize = m_cbNext = 0;
    }

private:
    BYTE        *m_pbData;              // Data pointer.
    int         m_cbSize;               // Size of current data.
    int         m_cbNext;               // Next place to write.
};


//*****************************************************************************
// CHashTable expects the data to be in a single array - this is provided by
// CHashTableAndData.
// The array is allocated using the MemMgr type. CVMemData, CNewData and
// CNewDataNoThrow can be used for this.
//*****************************************************************************
template <class MemMgr>
class CHashTableAndData : public CHashTable
{
public:
    ULONG      m_iFree;                // Index into m_pcEntries[] of next available slot
    ULONG      m_iEntries;             // size of m_pcEntries[]

public:

    CHashTableAndData() :
        CHashTable()
    {
        LEAF_CONTRACT;
    }

    CHashTableAndData(
        ULONG      iBuckets) :
        CHashTable(iBuckets)
    {
        LEAF_CONTRACT;
    }

#ifndef DACCESS_COMPILE

    ~CHashTableAndData()
    {
        WRAPPER_CONTRACT;
        if (m_pcEntries != NULL)
            MemMgr::Free((BYTE*)m_pcEntries, MemMgr::RoundSize(m_iEntries * m_iEntrySize));
    }

//*****************************************************************************
// This is the second part of construction where we do all of the work that
// can fail.  We also take the array of structs here because the calling class
// presumably needs to allocate it in its NewInit.
//*****************************************************************************
    HRESULT NewInit(                    // Return status.
        ULONG      iEntries,           // # of entries.
        ULONG      iEntrySize,         // Size of the entries.
        int         iMaxSize);          // Max size of data.

//*****************************************************************************
// Clear the hash table as if there were nothing in it.
//*****************************************************************************
    void Clear()
    {
        WRAPPER_CONTRACT;
        m_iFree = 0;
        InitFreeChain(0, m_iEntries);
        CHashTable::Clear();
    }

//*****************************************************************************
// Grabs a slot for the new entry to be added.
// The caller should fill in the non-HASHENTRY part of the returned slot
//*****************************************************************************
    BYTE *Add(
        ULONG      iHash)              // Hash value of entry to add.
    {
        WRAPPER_CONTRACT;
        FREEHASHENTRY *psEntry;

        // Make the table bigger if necessary.
        if (m_iFree == UINT32_MAX && !Grow())
            return (NULL);

        // Add the first entry from the free list to the hash chain.
        psEntry = (FREEHASHENTRY *) CHashTable::Add(iHash, m_iFree);
        m_iFree = psEntry->iFree;

        // If we're recycling memory, give our memory-allocator a chance to re-init it.

        // Each entry is prefixed with a header - we don't want to trash that.
        SIZE_T cbHeader = sizeof(FREEHASHENTRY);
        MemMgr::Clean((BYTE*) psEntry + cbHeader, (int) (m_iEntrySize - cbHeader));

        return ((BYTE *) psEntry);
    }

    void Delete(
        ULONG      iHash,              // Hash value of entry to delete.
        ULONG      iIndex)             // Index of struct in m_pcEntries.
    {
        WRAPPER_CONTRACT;
        CHashTable::Delete(iHash, iIndex);
        ((FREEHASHENTRY *) EntryPtr(iIndex))->iFree = m_iFree;
        m_iFree = iIndex;
    }

    void Delete(
        ULONG      iHash,              // Hash value of entry to delete.
        HASHENTRY   *psEntry)           // The struct to delete.
    {
        WRAPPER_CONTRACT;
        CHashTable::Delete(iHash, psEntry);
        ((FREEHASHENTRY *) psEntry)->iFree = m_iFree;
        m_iFree = ItemIndex(psEntry);
    }

#endif

    static SIZE_T helper_GetOffsetOfEntries()
    {
        LEAF_CONTRACT;
        return offsetof(CHashTableAndData, m_pcEntries);
    }

    static SIZE_T helper_GetOffsetOfCount()
    {
        LEAF_CONTRACT;
        return offsetof(CHashTableAndData, m_iEntries);
    }

#ifdef DACCESS_COMPILE
    void EnumMemoryRegions(CLRDataEnumMemoryFlags flags)
    {
        CHashTable::EnumMemoryRegions(flags, m_iEntries);
    }
#endif

private:
    void InitFreeChain(ULONG iStart,ULONG iEnd);
    int Grow();
};

#ifndef DACCESS_COMPILE

//*****************************************************************************
// This is the second part of construction where we do all of the work that
// can fail.  We also take the array of structs here because the calling class
// presumably needs to allocate it in its NewInit.
//*****************************************************************************
template<class MemMgr>
HRESULT CHashTableAndData<MemMgr>::NewInit(// Return status.
    ULONG      iEntries,               // # of entries.
    ULONG      iEntrySize,             // Size of the entries.
    int         iMaxSize)               // Max size of data.
{
    WRAPPER_CONTRACT;
    BYTE        *pcEntries;
    HRESULT     hr;


    // note that this function can throw because it depends on the <M>::Alloc

    // Allocate the memory for the entries.
    if ((pcEntries = MemMgr::Alloc(MemMgr::RoundSize(iEntries * iEntrySize),
                                   MemMgr::RoundSize(iMaxSize))) == 0)
        return (E_OUTOFMEMORY);
    m_iEntries = iEntries;

    // Init the base table.
    if (FAILED(hr = CHashTable::NewInit(pcEntries, iEntrySize)))
        MemMgr::Free(pcEntries, MemMgr::RoundSize(iEntries * iEntrySize));
    else
    {
        // Init the free chain.
        m_iFree = 0;
        InitFreeChain(0, iEntries);
    }
    return (hr);
}

//*****************************************************************************
// Initialize a range of records such that they are linked together to be put
// on the free chain.
//*****************************************************************************
template<class MemMgr>
void CHashTableAndData<MemMgr>::InitFreeChain(
    ULONG      iStart,                 // Index to start initializing.
    ULONG      iEnd)                   // Index to stop initializing
{
    LEAF_CONTRACT;
    BYTE* pcPtr;
    _ASSERTE(iEnd > iStart);

    pcPtr = (BYTE*)m_pcEntries + iStart * m_iEntrySize;
    for (++iStart; iStart < iEnd; ++iStart)
    {
        ((FREEHASHENTRY *) pcPtr)->iFree = iStart;
        pcPtr += m_iEntrySize;
    }
    ((FREEHASHENTRY *) pcPtr)->iFree = UINT32_MAX;
}

//*****************************************************************************
// Attempt to increase the amount of space available for the record heap.
//*****************************************************************************
template<class MemMgr>
int CHashTableAndData<MemMgr>::Grow()   // 1 if successful, 0 if not.
{
    WRAPPER_CONTRACT;
    int         iCurSize;               // Current size in bytes.
    int         iEntries;               // New # of entries.

    _ASSERTE(m_pcEntries != NULL);
    _ASSERTE(m_iFree == UINT32_MAX);

    // Compute the current size and new # of entries.
    S_UINT32 iTotEntrySize = S_UINT32(m_iEntries) * S_UINT32(m_iEntrySize);
    if( iTotEntrySize.IsOverflow() )
    {
        _ASSERTE( !"CHashTableAndData overflow!" );
        return (0);
    }    
    iCurSize = MemMgr::RoundSize( iTotEntrySize.Value() );
    iEntries = (iCurSize + MemMgr::GrowSize(iCurSize)) / m_iEntrySize;

    if ( (iEntries < 0) || ((ULONG)iEntries <= m_iEntries) )
    {
        _ASSERTE( !"CHashTableAndData overflow!" );
        return (0);
    }

    // Try to expand the array.
    if (MemMgr::Grow(*(BYTE**)&m_pcEntries, iCurSize) == 0)
        return (0);

    // Init the newly allocated space.
    InitFreeChain(m_iEntries, iEntries);
    m_iFree = m_iEntries;
    m_iEntries = iEntries;
    return (1);
}

#endif // #ifndef DACCESS_COMPILE

//*****************************************************************************
//*****************************************************************************

inline COUNT_T HashCOUNT_T(COUNT_T currentHash, COUNT_T data)
{
    LEAF_CONTRACT;
    return ((currentHash << 5) + currentHash) ^ data;
}

inline COUNT_T HashPtr(COUNT_T currentHash, void * ptr)
{
    LEAF_CONTRACT;
    return HashCOUNT_T(currentHash, COUNT_T(SIZE_T(ptr)));
}


inline ULONG HashBytes(BYTE const *pbData, size_t iSize)
{
    LEAF_CONTRACT;
    ULONG   hash = 5381;

    BYTE const *pbDataEnd = pbData + iSize;

    for (/**/ ; pbData < pbDataEnd; pbData++)
    {
        hash = ((hash << 5) + hash) ^ *pbData;
    }
    return hash;
}

// Helper function for hashing a string char by char.
inline ULONG HashStringA(LPCSTR szStr)
{
    LEAF_CONTRACT;
    ULONG   hash = 5381;
    int     c;

    while ((c = *szStr) != 0)
    {
        hash = ((hash << 5) + hash) ^ c;
        ++szStr;
    }
    return hash;
}

inline ULONG HashString(LPCWSTR szStr)
{
    LEAF_CONTRACT;
    ULONG   hash = 5381;
    int     c;

    while ((c = *szStr) != 0)
    {
        hash = ((hash << 5) + hash) ^ c;
        ++szStr;
    }
    return hash;
}

// Case-insensitive string hash function.
inline ULONG HashiString(LPCWSTR szStr)
{
    LEAF_CONTRACT;
    ULONG   hash = 5381;
    while (*szStr != 0)
    {
        hash = ((hash << 5) + hash) ^ towupper(*szStr);
        szStr++;
    }
    return hash;
}

// Case-insensitive string hash function.
inline ULONG HashiStringN(LPCWSTR szStr, DWORD count)
{
    LEAF_CONTRACT;
    ULONG   hash = 5381;
    while (*szStr != 0 && count--)
    {
        hash = ((hash << 5) + hash) ^ towupper(*szStr);
        szStr++;
    }
    return hash;
}

// Case-insensitive string hash function when all of the
// characters in the string are known to be below 0x80.
// Knowing this is much more efficient than calling
// towupper above.
inline ULONG HashiStringKnownLower80(LPCWSTR szStr) {
    LEAF_CONTRACT;
    ULONG hash = 5381;
    int c;
    int mask = ~0x20;
    while ((c = *szStr)!=0) {
        //If we have a lowercase character, ANDing off 0x20
        //(mask) will make it an uppercase character.
        if (c>='a' && c<='z') {
            c&=mask;
        }
        hash = ((hash << 5) + hash) ^ c;
        ++szStr;
    }
    return hash;
}


// // //
// // //  See $\src\utilcode\Debug.cpp for "Binomial (K, M, N)", which
// // //  computes the binomial distribution, with which to compare your
// // //  hash-table statistics.
// // //



#if defined(_UNICODE) || defined(UNICODE)
#define _tHashString(szStr) HashString(szStr)
#else
#define _tHashString(szStr) HashStringA(szStr)
#endif



//*****************************************************************************
// This helper template is used by the TStringMap to track an item by its
// character name.
//*****************************************************************************
template <class T> class TStringMapItem : HASHENTRY
{
public:
    TStringMapItem() :
        m_szString(0)
    {
        LEAF_CONTRACT;
    }
    ~TStringMapItem()
    {
        LEAF_CONTRACT;
        delete [] m_szString;
    }

    HRESULT SetString(LPCTSTR szValue)
    {
        WRAPPER_CONTRACT;
        int         iLen = (int)(_tcslen(szValue) + 1);
        if ((m_szString = new TCHAR[iLen]) == 0)
            return (OutOfMemory());
        _tcscpy_s(m_szString, iLen, szValue);
        return (S_OK);
    }

public:
    LPTSTR      m_szString;             // Key data.
    T           m_value;                // Value for this key.
};


//*****************************************************************************
// This template provides a map from string to item, determined by the template
// type passed in.
//*****************************************************************************
template <class T, int iBuckets=17, class TAllocator=CNewData, int iMaxSize=4096>
class TStringMap :
    protected CHashTableAndData<TAllocator>
{
    typedef CHashTableAndData<TAllocator> Super;

public:
    typedef TStringMapItem<T> TItemType;
    typedef TStringMapItem<long> TOffsetType;

#ifndef DACCESS_COMPILE

    TStringMap() :
        CHashTableAndData<TAllocator>(iBuckets)
    {
        LEAF_CONTRACT;
    }

//*****************************************************************************
// This is the second part of construction where we do all of the work that
// can fail.  We also take the array of structs here because the calling class
// presumably needs to allocate it in its NewInit.
//*****************************************************************************
    HRESULT NewInit()                   // Return status.
    {
        WRAPPER_CONTRACT;
        return (CHashTableAndData<TAllocator>::NewInit(
                                    CNewData::GrowSize(0)/sizeof(TItemType),
                                    sizeof(TItemType),
                                    iMaxSize));
    }

//*****************************************************************************
// For each item still allocated, invoke its dtor so it frees up anything it
// holds onto.
//*****************************************************************************
    void Clear()
    {
        WRAPPER_CONTRACT;
        HASHFIND    sSrch;
        TItemType   *p = (TItemType *) FindFirstEntry(&sSrch);

        while (p != 0)
        {
            // Call dtor on the item, since m_value is contained the scalar
            // dtor will get called.
            p->~TStringMapItem();
            p = (TItemType *) FindNextEntry(&sSrch);
        }
        CHashTableAndData<TAllocator>::Clear();
    }

#endif // #ifndef DACCESS_COMPILE

//*****************************************************************************
// Retrieve an item by name.
//*****************************************************************************
    T *GetItem(                         // Null or object.
        LPCTSTR     szKey)              // What to do the lookup on.
    {
        WRAPPER_CONTRACT;
        TItemType   sInfo;
        TItemType   *ptr;               // Working pointer.

        // Create a key.
        sInfo.m_szString = (LPTSTR) szKey;

        // Look it up in the hash table.
        ptr = (TItemType *) Find( HashString(szKey), (BYTE *) &sInfo);

        // Don't let dtor free our string.
        sInfo.m_szString = 0;

        // If pointer found, return to caller.  To handle T's that have
        // an operator &(), find raw address without going through &m_value.
        if (ptr)
            return ((T *) ((BYTE *) ptr + offsetof(TOffsetType, m_value)));
        else
            return (0);
    }

//*****************************************************************************
// Initialize an iterator and return the first item.
//*****************************************************************************
    TItemType *FindFirstEntry(
        HASHFIND *psSrch)
    {
        WRAPPER_CONTRACT;
        TItemType   *ptr = (TItemType *) Super::FindFirstEntry(psSrch);

        return (ptr);
    }

//*****************************************************************************
// Return the next item, via an iterator.
//*****************************************************************************
    TItemType *FindNextEntry(
        HASHFIND *psSrch)
    {
        WRAPPER_CONTRACT;
        TItemType   *ptr = (TItemType *) Super::FindNextEntry(psSrch);

        return (ptr);
    }

#ifndef DACCESS_COMPILE

//*****************************************************************************
// Add an item to the list.
//*****************************************************************************
    HRESULT AddItem(                    // S_OK, or S_FALSE.
        LPCTSTR     szKey,              // The key value for the item.
        T           &item)              // Thing to add.
    {
        WRAPPER_CONTRACT;
        TItemType   *ptr;               // Working pointer.

        // Allocate an entry in the hash table.
        if ((ptr = (TItemType *) Add( HashString(szKey))) == 0)
            return (OutOfMemory());

        // Fill the record.
        if (ptr->SetString(szKey) < 0)
        {
            DelItem(ptr);
            return (OutOfMemory());
        }

        // Call the placement new operator on the item so it can init itself.
        // To handle T's that have an operator &(), find raw address without
        // going through &m_value.
        T *p = new ((void *) ((BYTE *) ptr + offsetof(TOffsetType, m_value))) T;
        *p = item;
        return (S_OK);
    }

//*****************************************************************************
// Delete an item.
//*****************************************************************************
    void DelItem(
        LPCTSTR     szKey)                  // What to delete.
    {
        WRAPPER_CONTRACT;
        TItemType   sInfo;
        TItemType   *ptr;               // Working pointer.

        // Create a key.
        sInfo.m_szString = (LPTSTR) szKey;

        // Look it up in the hash table.
        ptr = (TItemType *) Find( HashString(szKey), (BYTE *) &sInfo);

        // Don't let dtor free our string.
        sInfo.m_szString = 0;

        // If found, delete.
        if (ptr)
            DelItem(ptr);
    }

#endif // #ifndef DACCESS_COMPILE

//*****************************************************************************
// Compare the keys for two collections.
//*****************************************************************************
    BOOL Cmp(                               // 0 or != 0.
        const BYTE  *pData,                 // Raw key data on lookup.
        const HASHENTRY *pElement)          // The element to compare data against.
    {
        LEAF_CONTRACT;
        TItemType   *p = (TItemType *) (size_t) pElement;
        return (_tcscmp(((TItemType *) pData)->m_szString, p->m_szString));
    }

private:
    void DelItem(
        TItemType   *pItem)             // Entry to delete.
    {
        WRAPPER_CONTRACT;
        // Need to destruct this item.
        pItem->~TStringMapItem();
        Delete( HashString(pItem->m_szString), (HASHENTRY *)(void *)pItem);
    }
};



//*****************************************************************************
// This class implements a closed hashing table.  Values are hashed to a bucket,
// and if that bucket is full already, then the value is placed in the next
// free bucket starting after the desired target (with wrap around).  If the
// table becomes 75% full, it is grown and rehashed to reduce lookups.  This
// class is best used in a reltively small lookup table where hashing is
// not going to cause many collisions.  By not having the collision chain
// logic, a lot of memory is saved.
//
// The user of the template is required to supply several methods which decide
// how each element can be marked as free, deleted, or used.  It would have
// been possible to write this with more internal logic, but that would require
// either (a) more overhead to add status on top of elements, or (b) hard
// coded types like one for strings, one for ints, etc... This gives you the
// flexibility of adding logic to your type.
//*****************************************************************************
class CClosedHashBase
{
    BYTE *EntryPtr(int iEntry)
    {
        LEAF_CONTRACT;
        return (m_rgData + (iEntry * m_iEntrySize));
    }

    BYTE *EntryPtr(int iEntry, BYTE *rgData)
    {
        LEAF_CONTRACT;
        return (rgData + (iEntry * m_iEntrySize));
    }

public:
    enum ELEMENTSTATUS
    {
        FREE,                               // Item is not in use right now.
        DELETED,                            // Item is deleted.
        USED                                // Item is in use.
    };

    CClosedHashBase(
        int         iBuckets,               // How many buckets should we start with.
        int         iEntrySize,             // Size of an entry.
        bool        bPerfect) :             // true if bucket size will hash with no collisions.
        m_bPerfect(bPerfect),
        m_iBuckets(iBuckets),
        m_iEntrySize(iEntrySize),
        m_iCount(0),
        m_iCollisions(0),
        m_rgData(0)
    {
        LEAF_CONTRACT;
        m_iSize = iBuckets + 7;
    }

    ~CClosedHashBase()
    {
        WRAPPER_CONTRACT;
        Clear();
    }

    virtual void Clear()
    {
        LEAF_CONTRACT;
        delete [] m_rgData;
        m_iCount = 0;
        m_iCollisions = 0;
        m_rgData = 0;
    }

//*****************************************************************************
// Accessors for getting at the underlying data.  Be careful to use Count()
// only when you want the number of buckets actually used.
//*****************************************************************************

    int Count()
    {
        LEAF_CONTRACT;
        return (m_iCount);
    }

    int Collisions()
    {
        LEAF_CONTRACT;
        return (m_iCollisions);
    }

    int Buckets()
    {
        LEAF_CONTRACT;
        return (m_iBuckets);
    }

    void SetBuckets(int iBuckets, bool bPerfect=false)
    {
        LEAF_CONTRACT;
        _ASSERTE(m_rgData == 0);
        m_iBuckets = iBuckets;
        m_iSize = m_iBuckets + 7;
        m_bPerfect = bPerfect;
    }

    BYTE *Data()
    {
        LEAF_CONTRACT;
        return (m_rgData);
    }

//*****************************************************************************
// Add a new item to hash table given the key value.  If this new entry
// exceeds maximum size, then the table will grow and be re-hashed, which
// may cause a memory error.
//*****************************************************************************
    BYTE *Add(                              // New item to fill out on success.
        void        *pData)                 // The value to hash on.
    {
        WRAPPER_CONTRACT;
        // If we haven't allocated any memory, or it is too small, fix it.
        if (!m_rgData || ((m_iCount + 1) > (m_iSize * 3 / 4) && !m_bPerfect))
        {
            if (!ReHash())
                return (0);
        }

        return (DoAdd(pData, m_rgData, m_iBuckets, m_iSize, m_iCollisions, m_iCount));
    }

//*****************************************************************************
// Delete the given value.  This will simply mark the entry as deleted (in
// order to keep the collision chain intact).  There is an optimization that
// consecutive deleted entries leading up to a free entry are themselves freed
// to reduce collisions later on.
//*****************************************************************************
    void Delete(
        void        *pData);                // Key value to delete.


//*****************************************************************************
//  Callback function passed to DeleteLoop.
//*****************************************************************************
    typedef BOOL (* DELETELOOPFUNC)(        // Delete current item?
         BYTE *pEntry,                      // Bucket entry to evaluate
         void *pCustomizer);                // User-defined value

//*****************************************************************************
// Iterates over all active values, passing each one to pDeleteLoopFunc.
// If pDeleteLoopFunc returns TRUE, the entry is deleted. This is safer
// and faster than using FindNext() and Delete().
//*****************************************************************************
    void DeleteLoop(
        DELETELOOPFUNC pDeleteLoopFunc,     // Decides whether to delete item
        void *pCustomizer);                 // Extra value passed to deletefunc.


//*****************************************************************************
// Lookup a key value and return a pointer to the element if found.
//*****************************************************************************
    BYTE *Find(                             // The item if found, 0 if not.
        void        *pData);                // The key to lookup.

//*****************************************************************************
// Look for an item in the table.  If it isn't found, then create a new one and
// return that.
//*****************************************************************************
    BYTE *FindOrAdd(                        // The item if found, 0 if not.
        void        *pData,                 // The key to lookup.
        bool        &bNew);                 // true if created.

//*****************************************************************************
// The following functions are used to traverse each used entry.  This code
// will skip over deleted and free entries freeing the caller up from such
// logic.
//*****************************************************************************
    BYTE *GetFirst()                        // The first entry, 0 if none.
    {
        WRAPPER_CONTRACT;
        int         i;                      // Loop control.

        // If we've never allocated the table there can't be any to get.
        if (m_rgData == 0)
            return (0);

        // Find the first one.
        for (i=0;  i<m_iSize;  i++)
        {
            if (Status(EntryPtr(i)) != FREE && Status(EntryPtr(i)) != DELETED)
                return (EntryPtr(i));
        }
        return (0);
    }

    BYTE *GetNext(BYTE *Prev)               // The next entry, 0 if done.
    {
        WRAPPER_CONTRACT;
        int         i;                      // Loop control.

        for (i = (int)(((size_t) Prev - (size_t) &m_rgData[0]) / m_iEntrySize) + 1; i<m_iSize;  i++)
        {
            if (Status(EntryPtr(i)) != FREE && Status(EntryPtr(i)) != DELETED)
                return (EntryPtr(i));
        }
        return (0);
    }

private:
//*****************************************************************************
// Hash is called with a pointer to an element in the table.  You must override
// this method and provide a hash algorithm for your element type.
//*****************************************************************************
    virtual unsigned long Hash(             // The key value.
        void const  *pData)=0;              // Raw data to hash.

//*****************************************************************************
// Compare is used in the typical memcmp way, 0 is eqaulity, -1/1 indicate
// direction of miscompare.  In this system everything is always equal or not.
//*****************************************************************************
    virtual unsigned long Compare(          // 0, -1, or 1.
        void const  *pData,                 // Raw key data on lookup.
        BYTE        *pElement)=0;           // The element to compare data against.

//*****************************************************************************
// Return true if the element is free to be used.
//*****************************************************************************
    virtual ELEMENTSTATUS Status(           // The status of the entry.
        BYTE        *pElement)=0;           // The element to check.

//*****************************************************************************
// Sets the status of the given element.
//*****************************************************************************
    virtual void SetStatus(
        BYTE        *pElement,              // The element to set status for.
        ELEMENTSTATUS eStatus)=0;           // New status.

//*****************************************************************************
// Returns the internal key value for an element.
//*****************************************************************************
    virtual void *GetKey(                   // The data to hash on.
        BYTE        *pElement)=0;           // The element to return data ptr for.

//*****************************************************************************
// This helper actually does the add for you.
//*****************************************************************************
    BYTE *DoAdd(void *pData, BYTE *rgData, int &iBuckets, int iSize,
                int &iCollisions, int &iCount);

//*****************************************************************************
// This function is called either to init the table in the first place, or
// to rehash the table if we ran out of room.
//*****************************************************************************
    bool ReHash();                          // true if successful.

//*****************************************************************************
// Walk each item in the table and mark it free.
//*****************************************************************************
    void InitFree(BYTE *ptr, int iSize)
    {
        WRAPPER_CONTRACT;
        int         i;
        for (i=0;  i<iSize;  i++, ptr += m_iEntrySize)
            SetStatus(ptr, FREE);
    }

private:
    bool        m_bPerfect;                 // true if the table size guarantees
                                            //  no collisions.
    int         m_iBuckets;                 // How many buckets do we have.
    int         m_iEntrySize;               // Size of an entry.
    int         m_iSize;                    // How many elements can we have.
    int         m_iCount;                   // How many items are used.
    int         m_iCollisions;              // How many have we had.
    BYTE        *m_rgData;                  // Data element list.
};

template <class T> class CClosedHash : public CClosedHashBase
{
public:
    CClosedHash(
        int         iBuckets,               // How many buckets should we start with.
        bool        bPerfect=false) :       // true if bucket size will hash with no collisions.
        CClosedHashBase(iBuckets, sizeof(T), bPerfect)
    {
        WRAPPER_CONTRACT;
    }

    T &operator[](long iIndex)
    {
        WRAPPER_CONTRACT;
        return ((T &) *(Data() + (iIndex * sizeof(T))));
    }


//*****************************************************************************
// Add a new item to hash table given the key value.  If this new entry
// exceeds maximum size, then the table will grow and be re-hashed, which
// may cause a memory error.
//*****************************************************************************
    T *Add(                                 // New item to fill out on success.
        void        *pData)                 // The value to hash on.
    {
        WRAPPER_CONTRACT;
        return ((T *) CClosedHashBase::Add(pData));
    }

//*****************************************************************************
// Lookup a key value and return a pointer to the element if found.
//*****************************************************************************
    T *Find(                                // The item if found, 0 if not.
        void        *pData)                 // The key to lookup.
    {
        WRAPPER_CONTRACT;
        return ((T *) CClosedHashBase::Find(pData));
    }

//*****************************************************************************
// Look for an item in the table.  If it isn't found, then create a new one and
// return that.
//*****************************************************************************
    T *FindOrAdd(                           // The item if found, 0 if not.
        void        *pData,                 // The key to lookup.
        bool        &bNew)                  // true if created.
    {
        WRAPPER_CONTRACT;
        return ((T *) CClosedHashBase::FindOrAdd(pData, bNew));
    }


//*****************************************************************************
// The following functions are used to traverse each used entry.  This code
// will skip over deleted and free entries freeing the caller up from such
// logic.
//*****************************************************************************
    T *GetFirst()                           // The first entry, 0 if none.
    {
        WRAPPER_CONTRACT;
        return ((T *) CClosedHashBase::GetFirst());
    }

    T *GetNext(T *Prev)                     // The next entry, 0 if done.
    {
        WRAPPER_CONTRACT;
        return ((T *) CClosedHashBase::GetNext((BYTE *) Prev));
    }
};


//*****************************************************************************
// Closed hash with typed parameters.  The derived class is the second
//  parameter to the template.  The derived class must implement:
//    unsigned long Hash(const T *pData);
//    unsigned long Compare(const T *p1, T *p2);
//    ELEMENTSTATUS Status(T *pEntry);
//    void SetStatus(T *pEntry, ELEMENTSTATUS s);
//    void* GetKey(T *pEntry);
//*****************************************************************************
template<class T, class H>class CClosedHashEx : public CClosedHash<T>
{
public:
    CClosedHashEx(
        int         iBuckets,               // How many buckets should we start with.
        bool        bPerfect=false) :       // true if bucket size will hash with no collisions.
        CClosedHash<T> (iBuckets, bPerfect)
    {
        WRAPPER_CONTRACT;
    }

    unsigned long Hash(const void *pData)
    {
        WRAPPER_CONTRACT;
        return static_cast<H*>(this)->Hash((const T*)pData);
    }

    unsigned long Compare(const void *p1, BYTE *p2)
    {
        WRAPPER_CONTRACT;
        return static_cast<H*>(this)->Compare((const T*)p1, (T*)p2);
    }

    typename CClosedHash<T>::ELEMENTSTATUS Status(BYTE *p)
    {
        WRAPPER_CONTRACT;
        return static_cast<H*>(this)->Status((T*)p);
    }

    void SetStatus(BYTE *p, typename CClosedHash<T>::ELEMENTSTATUS s)
    {
        WRAPPER_CONTRACT;
        static_cast<H*>(this)->SetStatus((T*)p, s);
    }

    void* GetKey(BYTE *p)
    {
        WRAPPER_CONTRACT;
        return static_cast<H*>(this)->GetKey((T*)p);
    }
};


//*****************************************************************************
// This template is another form of a closed hash table.  It handles collisions
// through a linked chain.  To use it, derive your hashed item from HASHLINK
// and implement the virtual functions required.  1.5 * ibuckets will be
// allocated, with the extra .5 used for collisions.  If you add to the point
// where no free nodes are available, the entire table is grown to make room.
// The advantage to this system is that collisions are always directly known,
// there either is one or there isn't.
//*****************************************************************************
struct HASHLINK
{
    ULONG       iNext;                  // Offset for next entry.
};

template <class T> class CChainedHash
{
public:
    CChainedHash(int iBuckets=32) :
        m_rgData(0),
        m_iBuckets(iBuckets),
        m_iCount(0),
        m_iMaxChain(0),
        m_iFree(0)
    {
        LEAF_CONTRACT;
        m_iSize = iBuckets + (iBuckets / 2);
    }

    ~CChainedHash()
    {
        LEAF_CONTRACT;
        if (m_rgData)
            delete [] m_rgData;
    }

    void SetBuckets(int iBuckets)
    {
        LEAF_CONTRACT;
        _ASSERTE(m_rgData == 0);
        // if iBuckets==0, then we'll allocate a zero size array and AV on dereference.
        _ASSERTE(iBuckets > 0);
        m_iBuckets = iBuckets;
        m_iSize = iBuckets + (iBuckets / 2);
    }

    T *Add(void const *pData)
    {
        WRAPPER_CONTRACT;
        ULONG       iHash;
        int         iBucket;
        T           *pItem;

        // Build the list if required.
        if (m_rgData == 0 || m_iFree == 0xffffffff)
        {
            if (!ReHash())
                return (0);
        }

        // Hash the item and pick a bucket.
        iHash = Hash(pData);
        iBucket = iHash % m_iBuckets;

        // Use the bucket if it is free.
        if (InUse(&m_rgData[iBucket]) == false)
        {
            pItem = &m_rgData[iBucket];
            pItem->iNext = 0xffffffff;
        }
        // Else take one off of the free list for use.
        else
        {
            ULONG       iEntry;

            // Pull an item from the free list.
            iEntry = m_iFree;
            pItem = &m_rgData[m_iFree];
            m_iFree = pItem->iNext;

            // Link the new node in after the bucket.
            pItem->iNext = m_rgData[iBucket].iNext;
            m_rgData[iBucket].iNext = iEntry;
        }
        ++m_iCount;
        return (pItem);
    }

    T *Find(void const *pData, bool bAddIfNew=false)
    {
        WRAPPER_CONTRACT;
        ULONG       iHash;
        int         iBucket;
        T           *pItem;

        // Check states for lookup.
        if (m_rgData == 0)
        {
            // If we won't be adding, then we are through.
            if (bAddIfNew == false)
                return (0);

            // Otherwise, create the table.
            if (!ReHash())
                return (0);
        }

        // Hash the item and pick a bucket.
        iHash = Hash(pData);
        iBucket = iHash % m_iBuckets;

        // If it isn't in use, then there it wasn't found.
        if (!InUse(&m_rgData[iBucket]))
        {
            if (bAddIfNew == false)
                pItem = 0;
            else
            {
                pItem = &m_rgData[iBucket];
                pItem->iNext = 0xffffffff;
                ++m_iCount;
            }
        }
        // Scan the list for the one we want.
        else
        {
            ULONG iChain = 0;
            for (pItem=(T *) &m_rgData[iBucket];  pItem;  pItem=GetNext(pItem))
            {
                if (Cmp(pData, pItem) == 0)
                    break;
                ++iChain;
            }

            if (!pItem && bAddIfNew)
            {
                ULONG       iEntry;

                // Record maximum chain length.
                if (iChain > m_iMaxChain)
                    m_iMaxChain = iChain;

                // Now need more room.
                if (m_iFree == 0xffffffff)
                {
                    if (!ReHash())
                        return (0);
                }

                // Pull an item from the free list.
                iEntry = m_iFree;
                pItem = &m_rgData[m_iFree];
                m_iFree = pItem->iNext;

                // Link the new node in after the bucket.
                pItem->iNext = m_rgData[iBucket].iNext;
                m_rgData[iBucket].iNext = iEntry;
                ++m_iCount;
            }
        }
        return (pItem);
    }

    int Count()
    {
        LEAF_CONTRACT;
        return (m_iCount);
    }

    int Buckets()
    {
        LEAF_CONTRACT;
        return (m_iBuckets);
    }

    ULONG MaxChainLength()
    {
        LEAF_CONTRACT;
        return (m_iMaxChain);
    }

    virtual void Clear()
    {
        LEAF_CONTRACT;
        // Free up the memory.
        if (m_rgData)
        {
            delete [] m_rgData;
            m_rgData = 0;
        }

        m_rgData = 0;
        m_iFree = 0;
        m_iCount = 0;
        m_iMaxChain = 0;
    }

    virtual bool InUse(T *pItem)=0;
    virtual void SetFree(T *pItem)=0;
    virtual ULONG Hash(void const *pData)=0;
    virtual int Cmp(void const *pData, void *pItem)=0;
private:
    inline T *GetNext(T *pItem)
    {
        LEAF_CONTRACT;
        if (pItem->iNext != 0xffffffff)
            return ((T *) &m_rgData[pItem->iNext]);
        return (0);
    }

    bool ReHash()
    {
        WRAPPER_CONTRACT;
        T           *rgTemp;
        int         iNewSize;

        // If this is a first time allocation, then just malloc it.
        if (!m_rgData)
        {
            if ((m_rgData = new (nothrow) T[m_iSize]) == 0)
                return (false);

            int i;
            for (i=0;  i<m_iSize;  i++)
                SetFree(&m_rgData[i]);

            m_iFree = m_iBuckets;
            for (i=m_iBuckets;  i<m_iSize;  i++)
                ((T *) &m_rgData[i])->iNext = i + 1;
            ((T *) &m_rgData[m_iSize - 1])->iNext = 0xffffffff;
            return (true);
        }

        // Otherwise we need more room on the free chain, so allocate some.
        iNewSize = m_iSize + (m_iSize / 2);

        // Allocate/realloc memory.
        if ((rgTemp = new (nothrow) T[iNewSize]) == 0)
            return (false);

        memcpy (rgTemp,m_rgData,m_iSize*sizeof(T));
        delete [] m_rgData;

        // Init new entries, save the new free chain, and reset internals.
        m_iFree = m_iSize;
        for (int i=m_iFree;  i<iNewSize;  i++)
        {
            SetFree(&rgTemp[i]);
            ((T *) &rgTemp[i])->iNext = i + 1;
        }
        ((T *) &rgTemp[iNewSize - 1])->iNext = 0xffffffff;

        m_rgData = rgTemp;
        m_iSize = iNewSize;
        return (true);
    }

private:
    T           *m_rgData;              // Data to store items in.
    int         m_iBuckets;             // How many buckets we want.
    int         m_iSize;                // How many are allocated.
    int         m_iCount;               // How many are we using.
    ULONG       m_iMaxChain;            // Max chain length.
    ULONG       m_iFree;                // Free chain.
};


//*****************************************************************************

class CHistogram
{
public:
    // One bucket per integral number
    // eg. If count==5 : { 0, 1, 2, 3, 4, everything else }
    CHistogram(unsigned count);

    // Fixed size buckets
    // eg. If count==5 and bucketSize==10 : { 0-9, 10-19, 20-29, 30-39, 40-49, everything else }
    CHistogram(unsigned count, unsigned bucketSize);

    // User specified buckets eg. { 0-23, 24-27, 28-50, everything else }
    // Caller owns bucketBoundaries[]. CHistogram will make copy of it
    //
    // CHistogram(unsigned count, const unsigned * const bucketBoundaries);

    ~CHistogram();

    // Increment the count for the bucket containing "val"
    void RecordEntry(unsigned val);

    void Print(bool fShowMinMax = true,         // Show the bucket with the min & max entries
               bool fShowEmptyBuckets = true);  // Should empty buckets be omitted or displayed?

protected:

    void        Init(unsigned count, unsigned bucketSize);

    unsigned    m_dwBuckets;    // count of explicit buckets (ignoring the tail bucket)
    unsigned    m_dwBucketSize; // size of each bucket
    unsigned  * m_pBuckets;     // Count for each bucket

    unsigned    m_iEntries;     // Number of recorded entries
};

//*****************************************************************************
//
//********** String helper functions.
//
//*****************************************************************************

//*****************************************************************************
// Parse a string that is a list of strings separated by the specified
// character.  This eliminates both leading and trailing whitespace.  Two
// important notes: This modifies the supplied buffer and changes the szEnv
// parameter to point to the location to start searching for the next token.
// This also skips empty tokens (e.g. two adjacent separators).  szEnv will be
// set to NULL when no tokens remain.  NULL may also be returned if no tokens
// exist in the string.
//*****************************************************************************
char *StrTok(                           // Returned token.
    __deref_inout_z char *&szEnv,       // Location to start searching.
    char        ch);                    // Separator character.


//*****************************************************************************
// Return the length portion of a BSTR which is a ULONG before the start of
// the null terminated string.
//*****************************************************************************
inline int GetBstrLen(BSTR bstr)
{
    LEAF_CONTRACT;
    return *((ULONG *) bstr - 1);
}


//*****************************************************************************
// Checks if string length exceeds the specified limit
//*****************************************************************************
inline BOOL IsStrLongerThan(__in __in_z char* pstr, unsigned N)
{
    LEAF_CONTRACT;
    unsigned i = 0;
    if(pstr)
    {
        for(i=0; (i < N)&&(pstr[i]); i++);
    }
    return (i >= N);
}


//*****************************************************************************
// Class to parse a list of simple assembly names and then find a match
//*****************************************************************************

class AssemblyNamesList
{
    struct AssemblyName
    {
        LPUTF8          m_assemblyName;
        AssemblyName   *m_next;         // Next name
    };

    AssemblyName       *m_pNames;       // List of names

public:

    bool IsInList(LPCUTF8 assemblyName);

    bool IsEmpty()
    {
        LEAF_CONTRACT;
        return m_pNames == 0;
    }

    AssemblyNamesList(LPWSTR list);
    ~AssemblyNamesList();
};

//*****************************************************************************
// Class to parse a list of method names and then find a match
//*****************************************************************************

class MethodNamesListBase
{
    struct MethodName
    {
        LPUTF8      methodName;     // NULL means wildcard
        LPUTF8      className;      // NULL means wildcard
        int         numArgs;        // number of args for the method, -1 is wildcard
        MethodName *next;           // Next name
    };

    MethodName     *pNames;         // List of names

public:
    void Init()
    {
        LEAF_CONTRACT;
        pNames = 0;
    }

    void Init(__in __in_z LPWSTR list)
    {
        WRAPPER_CONTRACT;
        pNames = 0;
        Insert(list);
    }

    void Destroy();

    void Insert(__in __in_z LPWSTR list);

    bool IsInList(LPCUTF8 methodName, LPCUTF8 className, PCCOR_SIGNATURE sig);
    bool IsEmpty()
    {
        LEAF_CONTRACT;
        return pNames == 0;
    }
};

class MethodNamesList : public MethodNamesListBase
{
public:
    MethodNamesList()
    {
        WRAPPER_CONTRACT;
        Init();
    }

    MethodNamesList(LPWSTR list)
    {
        WRAPPER_CONTRACT;
        Init(list);
    }

    ~MethodNamesList()
    {
        WRAPPER_CONTRACT;
        Destroy();
    }
};

/**************************************************************************/
/* simple wrappers around the REGUTIL and MethodNameList routines that make
   the lookup lazy */

/* to be used as static variable - no constructor/destructor, assumes zero
   initialized memory */

class ConfigDWORD
{
public:
    inline DWORD val(__in __in_z LPWSTR keyName, DWORD defaultVal=0)
    {
        WRAPPER_CONTRACT;
        // make sure that the memory was zero initialized
        _ASSERTE(m_inited == 0 || m_inited == 1);

        if (!m_inited) init(keyName, defaultVal);
        return m_value;
    }

private:
    void init(__in __in_z LPWSTR keyName, DWORD defaultVal=0);

private:
    DWORD  m_value;
    BYTE m_inited;
};

/**************************************************************************/
class ConfigString
{
public:
    inline LPWSTR val(__in __in_z LPWSTR keyName)
    {
        WRAPPER_CONTRACT;
        // make sure that the memory was zero initialized
        _ASSERTE(m_inited == 0 || m_inited == 1);

        if (!m_inited) init(keyName);
        return m_value;
    }

private:
    void init(__in __in_z LPWSTR keyName);

private:
    LPWSTR m_value;
    BYTE m_inited;
};

/**************************************************************************/
class ConfigMethodSet
{
public:
    bool isEmpty()
    {
        WRAPPER_CONTRACT;
        _ASSERTE(m_inited == 1);
        return m_list.IsEmpty();
    }

    bool contains(LPCUTF8 methodName, LPCUTF8 className, PCCOR_SIGNATURE sig);

    inline void ensureInit(__in __in_z LPWSTR keyName)
    {
        WRAPPER_CONTRACT;
        // make sure that the memory was zero initialized
        _ASSERTE(m_inited == 0 || m_inited == 1);

        if (!m_inited) init(keyName);
    }

private:
    void init(__in __in_z LPWSTR keyName);

private:
    MethodNamesListBase m_list;

    BYTE m_inited;
};

//*****************************************************************************
// Smart Pointers for use with COM Objects.
//
// Based on the CSmartInterface class in Dale Rogerson's technical
// article "Calling COM Objects with Smart Interface Pointers" on MSDN.
//*****************************************************************************

template <class I>
class CIfacePtr
{
public:
//*****************************************************************************
// Construction - Note that it does not AddRef the pointer.  The caller must
// hand this class a reference.
//*****************************************************************************
    CIfacePtr(
        I           *pI = NULL)         // Interface ptr to store.
    :   m_pI(pI)
    {
        WRAPPER_CONTRACT;
    }

//*****************************************************************************
// Copy Constructor
//*****************************************************************************
    CIfacePtr(
        const CIfacePtr<I>& rSI)        // Interface ptr to copy.
    :   m_pI(rSI.m_pI)
    {
        WRAPPER_CONTRACT;
        if (m_pI != NULL)
            m_pI->AddRef();
    }

//*****************************************************************************
// Destruction
//*****************************************************************************
    ~CIfacePtr()
    {
        WRAPPER_CONTRACT;
        if (m_pI != NULL)
            m_pI->Release();
    }

//*****************************************************************************
// Assignment Operator for a plain interface pointer.  Note that it does not
// AddRef the pointer.  Making this assignment hands a reference count to this
// class.
//*****************************************************************************
    CIfacePtr<I>& operator=(            // Reference to this class.
        I           *pI)                // Interface pointer.
    {
        LEAF_CONTRACT;
        if (m_pI != NULL)
            m_pI->Release();
        m_pI = pI;
        return (*this);
    }

//*****************************************************************************
// Assignment Operator for a CIfacePtr class.  Note this releases the reference
// on the current ptr and AddRefs the new one.
//*****************************************************************************
    CIfacePtr<I>& operator=(            // Reference to this class.
        const CIfacePtr<I>& rSI)
    {
        LEAF_CONTRACT;
        // Only need to AddRef/Release if difference
        if (m_pI != rSI.m_pI)
        {
            if (m_pI != NULL)
                m_pI->Release();

            if ((m_pI = rSI.m_pI) != NULL)
                m_pI->AddRef();
        }
        return (*this);
    }

//*****************************************************************************
// Conversion to a normal interface pointer.
//*****************************************************************************
    operator I*()                       // The contained interface ptr.
    {
        LEAF_CONTRACT;
        return (m_pI);
    }

//*****************************************************************************
// Deref
//*****************************************************************************
    I* operator->()                     // The contained interface ptr.
    {
        LEAF_CONTRACT;
        _ASSERTE(m_pI != NULL);
        return (m_pI);
    }

//*****************************************************************************
// Address of
//*****************************************************************************
    I** operator&()                     // Address of the contained iface ptr.
    {
        LEAF_CONTRACT;
        return (&m_pI);
    }

//*****************************************************************************
// Equality
//*****************************************************************************
    BOOL operator==(                    // TRUE or FALSE.
        I           *pI) const          // An interface ptr to cmp against.
    {
        LEAF_CONTRACT;
        return (m_pI == pI);
    }

//*****************************************************************************
// In-equality
//*****************************************************************************
    BOOL operator!=(                    // TRUE or FALSE.
        I           *pI) const          // An interface ptr to cmp against.
    {
        LEAF_CONTRACT;
        return (m_pI != pI);
    }

//*****************************************************************************
// Negation
//*****************************************************************************
    BOOL operator!() const              // TRUE if NULL, FALSE otherwise.
    {
        LEAF_CONTRACT;
        return (!m_pI);
    }

protected:
    I           *m_pI;                  // The actual interface Ptr.
};



//*****************************************************************************
// Convert a UTF8 string into a wide string and build a BSTR.
//*****************************************************************************
inline BSTR Utf8StringToBstr(           // The new BSTR.
    LPCSTR      szStr,                  // The string to turn into a BSTR.
    int         iSize=-1)               // Size of string without 0, or -1 for default.
{
    WRAPPER_CONTRACT;
    BSTR        bstrVal;
    int         iReq;                   // Chars required for string.

    // Account for terminating 0.
    if (iSize != -1)
        ++iSize;

    // How big a buffer is needed?
    if ((iReq = WszMultiByteToWideChar(CP_UTF8, 0, szStr, iSize, 0, 0)) == 0)
        return (0);


    // Allocate the BSTR.
    if ((bstrVal = ::SysAllocStringLen(0, iReq)) == 0)
        return (0);


    // Convert into the buffer.
    if (WszMultiByteToWideChar(CP_UTF8, 0, szStr, iSize, bstrVal, iReq) == 0)
    {   // Failed, so clean up.
        _ASSERTE(!"Unexpected MultiByteToWideChar() failure");
        ::SysFreeString(bstrVal);
        return 0;
    }

    return (bstrVal);
}

//*****************************************************************************
// Convert a pointer to a string into a GUID.
//*****************************************************************************
HRESULT LPCSTRToGuid(                   // Return status.
    LPCSTR      szGuid,                 // String to convert.
    GUID        *psGuid);               // Buffer for converted GUID.

//*****************************************************************************
// Convert a GUID into a pointer to a string
//*****************************************************************************
int GuidToLPWSTR(                  // Return status.
    GUID        Guid,                  // [IN] The GUID to convert.
    __out_ecount (cchGuid) LPWSTR szGuid, // [OUT] String into which the GUID is stored
    DWORD       cchGuid);              // [IN] Size in wide chars of szGuid


typedef VPTR(class RangeList) PTR_RangeList;

class RangeList
{
  public:
    VPTR_BASE_CONCRETE_VTABLE_CLASS(RangeList)

#ifndef DACCESS_COMPILE
    RangeList();
    ~RangeList();
#else
    RangeList()
    {
        LEAF_CONTRACT;
    }
#endif

    // Wrappers to make the virtual calls DAC-safe.
    BOOL AddRange(const BYTE *start, const BYTE *end, void *id)
    {
        return this->AddRangeWorker(start, end, id);
    }
    
    void RemoveRanges(void *id, const BYTE *start = NULL, const BYTE *end = NULL)
    {
        return this->RemoveRangesWorker(id, start, end);
    }
    
    BOOL IsInRange(TADDR address, TADDR *pID = NULL)
    {
        return this->IsInRangeWorker(address, pID);
    }
    
    BOOL IsInRange(const BYTE *address, void **pID = NULL)
    {
        STATIC_CONTRACT_SO_TOLERANT;
        // Pointer form should eventually be removed.
        return this->IsInRangeWorker((TADDR) address, (TADDR*)pID);
    }
    
    TADDR FindIdWithinRange(TADDR start, TADDR end)
    {
        return this->FindIdWithinRangeWorker(start, end);
    }
    
    void *FindIdWithinRange(const BYTE *start, const BYTE *end)
    {
        // Pointer form should eventually be removed.
        return (void*) this->FindIdWithinRangeWorker((TADDR)start, (TADDR)end);
    }

#ifndef DACCESS_COMPILE

    // You can overload these two for synchronization (as LockedRangeList does)
    virtual BOOL AddRangeWorker(const BYTE *start, const BYTE *end, void *id);
    // If both "start" and "end" are NULL, then this method deletes all ranges with
    // the given id (i.e. the original behaviour).  Otherwise, it ignores the given
    // id and deletes all ranges falling in the region [start, end).
    virtual void RemoveRangesWorker(void *id, const BYTE *start = NULL, const BYTE *end = NULL);
#else
    virtual BOOL AddRangeWorker(const BYTE *start, const BYTE *end, void *id)
    {
        return TRUE;
    }
    virtual void RemoveRangesWorker(void *id, const BYTE *start = NULL, const BYTE *end = NULL) { }
#endif // !DACCESS_COMPILE

    virtual BOOL IsInRangeWorker(TADDR address, TADDR *pID = NULL);

    // This is used by the EditAndContinueModule to track open SLOTs that
    // are interspersed among classes.
    // Note that this searches from the start all the way to the end, so that we'll
    // pick element that is the farthest away from start as we can get.
    // Of course, this is pretty slow, so be warned.
    virtual TADDR FindIdWithinRangeWorker(TADDR start, TADDR end);
 

#ifdef DACCESS_COMPILE
    void EnumMemoryRegions(enum CLRDataEnumMemoryFlags flags);
#endif

    enum
    {
        RANGE_COUNT = 10
    };

   
  private:
    struct Range
    {
        TADDR start;
        TADDR end;
        TADDR id;
    };

    struct RangeListBlock
    {
        Range                ranges[RANGE_COUNT];
        DPTR(RangeListBlock) next;

#ifdef DACCESS_COMPILE
        void EnumMemoryRegions(enum CLRDataEnumMemoryFlags flags);
#endif

    };

    void InitBlock(RangeListBlock *block);

    RangeListBlock       m_starterBlock;
    DPTR(RangeListBlock) m_firstEmptyBlock;
    TADDR                m_firstEmptyRange;
};



//
// A private function to do the equavilent of a CoCreateInstance in
// cases where we can't make the real call. Use this when, for
// instance, you need to create a symbol reader in the Runtime but
// we're not CoInitialized. Obviously, this is only good for COM
// objects for which CoCreateInstance is just a glorified
// find-and-load-me operation.
//
#define FakeCoCreateInstance PAL_CoCreateInstance

//*****************************************************************************
// Gets the directory based on the location of the module. This routine
// is called at COR setup time. Set is called during EEStartup and by the
// MetaData dispenser.
//*****************************************************************************
HRESULT GetInternalSystemDirectory(__out_ecount_opt (*pdwLength) __out_opt LPWSTR buffer, __inout DWORD* pdwLength);

//*****************************************************************************
// This function validates the given Method/Field/Standalone signature. (util.cpp)
//*****************************************************************************
struct IMDInternalImport;
HRESULT validateTokenSig(
    mdToken             tk,                     // [IN] Token whose signature needs to be validated.
    PCCOR_SIGNATURE     pbSig,                  // [IN] Signature.
    ULONG               cbSig,                  // [IN] Size in bytes of the signature.
    DWORD               dwFlags,                // [IN] Method flags.
    IMDInternalImport*  pImport);               // [IN] Internal MD Import interface ptr

//*****************************************************************************
// This function validates that a sig fits into its designated space.  Note that it does
// note validate the sig--call validateTokenSig for that. (util.cpp)
//*****************************************************************************
HRESULT validateSigCompression(
    mdToken     tk,                             // [IN] Token whose signature needs to be validated.
    PCCOR_SIGNATURE pbSig,                      // [IN] Signature.
    ULONG       cbSig);                         // [IN] Size in bytes of the signature.
//*****************************************************************************
// Determine the version number of the runtime that was used to build the
// specified image. The pMetadata pointer passed in is the pointer to the
// metadata contained in the image.
//*****************************************************************************
HRESULT GetImageRuntimeVersionString(PVOID pMetaData, LPCSTR* pString);
void  AdjustImageRuntimeVersion (SString* pVersion);

//*****************************************************************************
// The registry keys and values that contain the information regarding
// the default registered unmanaged debugger.
//*****************************************************************************
const WCHAR kUnmanagedDebuggerKey[] = L"SOFTWARE\\Microsoft\\Windows NT\\CurrentVersion\\AeDebug";
const WCHAR kUnmanagedDebuggerValue[] = L"Debugger";
const WCHAR kUnmanagedDebuggerAutoValue[] = L"Auto";

//*****************************************************************************
// Retrieve information regarding what registered default unmanaged debugger
// from the registry.
//*****************************************************************************
void GetUnmanagedDebuggerRegistryInfo(SString &debuggerKeyValue, BOOL *pfAuto);

//*****************************************************************************
// Convert a UTF8 string to Unicode, into a CQuickArray<WCHAR>.
//*****************************************************************************
HRESULT Utf2Quick(
    LPCUTF8     pStr,                   // The string to convert.
    CQuickArray<WCHAR> &rStr,           // The QuickArray<WCHAR> to convert it into.
    int         iCurLen);               // Inital characters in the array to leave (default 0).

//*****************************************************************************
//  Extract the movl 64-bit unsigned immediate from an IA64 bundle
//  (Format X2)
//*****************************************************************************
UINT64 GetIA64Imm64(UINT64 * pBundle);
UINT64 GetIA64Imm64(UINT64 qword0, UINT64 qword1);

//*****************************************************************************
//  Deposit the movl 64-bit unsigned immediate into an IA64 bundle
//  (Format X2)
//*****************************************************************************
void PutIA64Imm64(UINT64 * pBundle, UINT64 imm64);

//*****************************************************************************
//  Extract the addl 22-bit signed immediate from an IA64 bundle
//  (Format A5)
//*****************************************************************************
INT32 GetIA64Imm22(UINT64 * pBundle, UINT32 slot);

//*****************************************************************************
//  Deposit the addl 22-bit signed immediate into an IA64 bundle
//  (Format A5)
//*****************************************************************************
void  PutIA64Imm22(UINT64 * pBundle, UINT32 slot, INT32 imm22);

//*****************************************************************************
//  Extract the IP-Relative signed 25-bit immediate from an IA64 bundle
//  (Formats B1, B2 or B3)
//  Note that due to branch target alignment requirements
//       the lowest four bits in the result will always be zero.
//*****************************************************************************
INT32 GetIA64Rel25(UINT64 * pBundle, UINT32 slot);
INT32 GetIA64Rel25(UINT64 qword0, UINT64 qword1, UINT32 slot);

//*****************************************************************************
//  Deposit the IP-Relative signed 25-bit immediate into an IA64 bundle
//  (Formats B1, B2 or B3)
//  Note that due to branch target alignment requirements
//       the lowest four bits are required to be zero.
//*****************************************************************************
void PutIA64Rel25(UINT64 * pBundle, UINT32 slot, INT32 imm25);

//*****************************************************************************
//  Extract the IP-Relative signed 64-bit immediate from an IA64 bundle
//  (Formats X3 or X4)
//*****************************************************************************
INT64 GetIA64Rel64(UINT64 * pBundle);
INT64 GetIA64Rel64(UINT64 qword0, UINT64 qword1);

//*****************************************************************************
//  Deposit the IP-Relative signed 64-bit immediate into a IA64 bundle
//  (Formats X3 or X4)
//*****************************************************************************
void PutIA64Rel64(UINT64 * pBundle, INT64 imm64);

//*****************************************************************************
// Splits a command line into argc/argv lists, using the VC7 parsing rules.
// This functions interface mimics the CommandLineToArgvW api.
// If function fails, returns NULL.
// If function suceeds, call delete [] on return pointer when done.
//*****************************************************************************
LPWSTR *SegmentCommandLine(LPCWSTR lpCmdLine, DWORD *pNumArgs);

#if defined(_X86_) || defined (_PPC_)
// Interlockedxxx64 are only supported on Windows Server 2003 or higher for X86 so
// define our own
// On X86 this uses the X86 cmpxchg8b instruction

#define InterlockedCompareExchange64 InterlockedCompareExchange8B
#define InterlockedIncrement64 InterlockedIncrement8B
#define InterlockedDecrement64 InterlockedDecrement8B

extern LONGLONG InterlockedCompareExchange64(LONGLONG volatile *pDestination,
                                                          LONGLONG exchange,
                                                          LONGLONG comparand);

#elif !defined(InterlockedCompareExchange64)
#error "InterlockedCompareExchange64 not defined - should be intrinsic"
#endif

LONGLONG FORCEINLINE InterlockedIncrement64(LONGLONG volatile *Addend)
{
    LONGLONG Old;

    do {
        Old = *Addend;
    } while (InterlockedCompareExchange64(Addend,
                                          Old + 1,
                                          Old) != Old);

    return Old + 1;
}

LONGLONG FORCEINLINE InterlockedDecrement64(LONGLONG volatile *Addend)
{
    LONGLONG Old;

    do {
        Old = *Addend;
    } while (InterlockedCompareExchange64(Addend,
                                          Old - 1,
                                          Old) != Old);

    return Old - 1;
}

//
// TEB access can be dangerous when using fibers because a fiber may
// run on multiple threads.  If the TEB pointer is retrieved and saved
// and then a fiber is moved to a different thread, when it accesses
// the saved TEB pointer, it will be looking at the TEB state for a
// different fiber.
//
// These accessors serve the purpose of retrieving information from the
// TEB in a manner that ensures that the current fiber will not switch
// threads while the access is occuring.
//
class ClrTeb
{
public:

    // returns pointer that uniquely identifies the fiber
    static void* GetFiberPtrId()
    {
        LEAF_CONTRACT;
        // not fiber for FEATURE_PAL - use the regular thread ID
        return (void *)(size_t)GetCurrentThreadId();
    }

    static void* InvalidFiberPtrId()
    {
        return NULL;
    }

};

// check if current thread is a GC thread (concurrent or server)
inline BOOL IsGCSpecialThread ()
{
    STATIC_CONTRACT_NOTHROW;
    STATIC_CONTRACT_GC_NOTRIGGER;
    STATIC_CONTRACT_MODE_ANY;

    return !!(((size_t)ClrFlsGetValue (TlsIdx_ThreadType)) & ThreadType_GC);
}

// check if current thread is a Gate thread
inline BOOL IsGateSpecialThread ()
{
    STATIC_CONTRACT_NOTHROW;
    STATIC_CONTRACT_GC_NOTRIGGER;
    STATIC_CONTRACT_MODE_ANY;

    return !!(((size_t)ClrFlsGetValue (TlsIdx_ThreadType)) & ThreadType_Gate);
}

// check if current thread is a Timer thread
inline BOOL IsTimerSpecialThread ()
{
    STATIC_CONTRACT_NOTHROW;
    STATIC_CONTRACT_GC_NOTRIGGER;
    STATIC_CONTRACT_MODE_ANY;

    return !!(((size_t)ClrFlsGetValue (TlsIdx_ThreadType)) & ThreadType_Timer);
}

// check if current thread is a debugger helper thread
inline BOOL IsDbgHelperSpecialThread ()
{
    STATIC_CONTRACT_NOTHROW;
    STATIC_CONTRACT_GC_NOTRIGGER;
    STATIC_CONTRACT_MODE_ANY;

    return !!(((size_t)ClrFlsGetValue (TlsIdx_ThreadType)) & ThreadType_DbgHelper);
}

// check if current thread is a thread which is performing shutdown
inline BOOL IsShutdownSpecialThread ()
{
    STATIC_CONTRACT_NOTHROW;
    STATIC_CONTRACT_GC_NOTRIGGER;
    STATIC_CONTRACT_MODE_ANY;

    return !!(((size_t)ClrFlsGetValue (TlsIdx_ThreadType)) & ThreadType_Shutdown);
}

// check if current thread is a thread which is performing shutdown
inline BOOL IsSuspendEEThread ()
{
    STATIC_CONTRACT_NOTHROW;
    STATIC_CONTRACT_GC_NOTRIGGER;
    STATIC_CONTRACT_MODE_ANY;

    return !!(((size_t)ClrFlsGetValue (TlsIdx_ThreadType)) & ThreadType_DynamicSuspendEE);
}

inline BOOL IsGCThread ()
{
    STATIC_CONTRACT_NOTHROW;
    STATIC_CONTRACT_GC_NOTRIGGER;
    STATIC_CONTRACT_MODE_ANY;

    return IsGCSpecialThread () || IsSuspendEEThread ();
}

inline BOOL IsFinalizerThread ()
{
    STATIC_CONTRACT_NOTHROW;
    STATIC_CONTRACT_GC_NOTRIGGER;
    STATIC_CONTRACT_MODE_ANY;

    return !!(((size_t)ClrFlsGetValue (TlsIdx_ThreadType)) & ThreadType_Finalizer);
}

inline BOOL IsADUnloadHelperThread ()
{
    STATIC_CONTRACT_NOTHROW;
    STATIC_CONTRACT_GC_NOTRIGGER;
    STATIC_CONTRACT_MODE_ANY;

    return !!(((size_t)ClrFlsGetValue (TlsIdx_ThreadType)) & ThreadType_ADUnloadHelper);
}

inline BOOL IsShutdownHelperThread ()
{
    STATIC_CONTRACT_NOTHROW;
    STATIC_CONTRACT_GC_NOTRIGGER;
    STATIC_CONTRACT_MODE_ANY;

    return !!(((size_t)ClrFlsGetValue (TlsIdx_ThreadType)) & ThreadType_ShutdownHelper);
}

// set specical type for current thread
inline void ClrFlsSetThreadType (TlsThreadTypeFlag flag)
{
    STATIC_CONTRACT_NOTHROW;
    STATIC_CONTRACT_GC_NOTRIGGER;
    STATIC_CONTRACT_MODE_ANY;

    ClrFlsSetValue (TlsIdx_ThreadType, (LPVOID)(((size_t)ClrFlsGetValue (TlsIdx_ThreadType)) |flag));
}

// clear specical type for current thread
inline void ClrFlsClearThreadType (TlsThreadTypeFlag flag)
{
    STATIC_CONTRACT_NOTHROW;
    STATIC_CONTRACT_GC_NOTRIGGER;
    STATIC_CONTRACT_MODE_ANY;

    ClrFlsSetValue (TlsIdx_ThreadType, (LPVOID)(((size_t)ClrFlsGetValue (TlsIdx_ThreadType)) & ~flag));
}

class ClrFlsThreadTypeSwitch
{
public:
    ClrFlsThreadTypeSwitch (TlsThreadTypeFlag flag)
    {
        STATIC_CONTRACT_NOTHROW;
        STATIC_CONTRACT_GC_NOTRIGGER;
        STATIC_CONTRACT_MODE_ANY;

        originalFlag = (TlsThreadTypeFlag)(size_t)ClrFlsGetValue (TlsIdx_ThreadType);
        ClrFlsSetValue (TlsIdx_ThreadType, (LPVOID)(size_t)(originalFlag | flag));
    }

    ~ClrFlsThreadTypeSwitch ()
    {
        STATIC_CONTRACT_NOTHROW;
        STATIC_CONTRACT_GC_NOTRIGGER;
        STATIC_CONTRACT_MODE_ANY;

        ClrFlsSetValue (TlsIdx_ThreadType, (LPVOID)(size_t)originalFlag);
    }

private:
    TlsThreadTypeFlag originalFlag;
};

//*********************************************************************************

// When we're hosted, operations called by the host (such as Thread::YieldTask)
// may not cause calls back into the host, as the host needs not be reentrant.
// Use the following holder for code in which calls into the host are forbidden.
// (If a call into the host is attempted nevertheless, an assert will fire.)

class ForbidCallsIntoHostOnThisThread
{
private:
    static volatile PVOID s_pvOwningFiber;

    FORCEINLINE static BOOL Enter(BOOL)
    {
        WRAPPER_CONTRACT;
        return VipInterlockedCompareExchange(
            &s_pvOwningFiber, ClrTeb::GetFiberPtrId(), NULL) == NULL;
    }

    FORCEINLINE static void Leave(BOOL)
    {
        LEAF_CONTRACT;
        s_pvOwningFiber = NULL;
    }

public:
    typedef ConditionalStateHolder<BOOL, ForbidCallsIntoHostOnThisThread::Enter, ForbidCallsIntoHostOnThisThread::Leave> Holder;

    FORCEINLINE static BOOL CanThisThreadCallIntoHost()
    {
        WRAPPER_CONTRACT;
        return s_pvOwningFiber != ClrTeb::GetFiberPtrId();
    }
};

typedef ForbidCallsIntoHostOnThisThread::Holder ForbidCallsIntoHostOnThisThreadHolder;

FORCEINLINE BOOL CanThisThreadCallIntoHost()
{
    WRAPPER_CONTRACT;
    return ForbidCallsIntoHostOnThisThread::CanThisThreadCallIntoHost();
}

//*********************************************************************************

#include "contract.inl"

void ClrTrace( wchar_t const * format, ... );


//*********************************************************************************

namespace util
{
    //  compare adapters
    //  
    
    template < typename T >
    struct less
    {
        bool operator()( T const & first, T const & second ) const
        {
            return first < second;
        }
    };

    template < typename T >
    struct greater
    {
        bool operator()( T const & first, T const & second ) const
        {
            return first > second;
        }
    };
    

    //  sort adapters
    //

    template< typename Iter, typename Pred >
    void sort( Iter begin, Iter end, Pred pred );

    template< typename T, typename Pred >
    void sort( T * begin, T * end, Pred pred )
    {
        struct sort_helper : CQuickSort< T >
        {
            sort_helper( T * begin, T * end, Pred pred )
                : CQuickSort< T >( begin, end - begin )
                , m_pred( pred )
            {}
            
            virtual int Compare( T * first, T * second )
            {
                return m_pred( *first, *second ) ? -1
                            : ( m_pred( *second, *first ) ? 1 : 0 );
            }

            Pred m_pred;
        };

        sort_helper sort_obj( begin, end, pred );
        sort_obj.Sort();
    }
    

    template < typename Iter >
    void sort( Iter begin, Iter end );

    template < typename T >
    void sort( T * begin, T * end )
    {
        util::sort( begin, end, util::less< T >() );
    }

    
    // binary search adapters
    //

    template < typename Iter, typename T, typename Pred >
    Iter lower_bound( Iter begin, Iter end, T const & val, Pred pred );

    template < typename T, typename Pred >
    T * lower_bound( T * begin, T * end, T const & val, Pred pred )
    {
        for (; begin != end; )
        {
            T * mid = begin + ( end - begin ) / 2;
            if ( pred( *mid, val ) )
                begin = ++mid;
            else
                end = mid;
        }

        return begin;
    }


    template < typename Iter, typename T >
    Iter lower_bound( Iter begin, Iter end, T const & val );

    template < typename T >
    T * lower_bound( T * begin, T * end, T const & val )
    {
        return util::lower_bound( begin, end, val, util::less< T >() );
    }
}


/* ------------------------------------------------------------------------ *
 * Overloaded operators for the executable heap
 * ------------------------------------------------------------------------ */

struct CExecutable { int x; };
extern const CExecutable executable;

void * __cdecl operator new(size_t n, const CExecutable&);
void * __cdecl operator new[](size_t n, const CExecutable&);
void * __cdecl operator new(size_t n, const CExecutable&, const NoThrow&);
void * __cdecl operator new[](size_t n, const CExecutable&, const NoThrow&);


//
// Executable heap delete to match the executable heap new above.
//
template<class T> void DeleteExecutable(T *p)
{
    if (p != NULL)
    {
        p->T::~T();

        ClrHeapFree(ClrGetProcessExecutableHeap(), 0, p);
    }
}

INDEBUG(BOOL DbgIsExecutable(LPVOID lpMem, SIZE_T length);)

class HighCharHelper {
public:
    static inline BOOL IsHighChar(int c) {
        return (BOOL)HighCharTable[c];
    }

private:
    static const BYTE HighCharTable[];
};


BOOL ThreadWillCreateGuardPage(SIZE_T sizeReservedStack, SIZE_T sizeCommitedStack);

FORCEINLINE void HolderSysFreeString(BSTR str) { CONTRACT_VIOLATION(ThrowsViolation); SysFreeString(str); }

typedef Wrapper<BSTR, DoNothing, HolderSysFreeString> BSTRHolder;

BOOL FileExists(LPCWSTR filename);




#endif // __UtilCode_h__

