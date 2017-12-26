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

// commacro.h - Handy COM boilerplate

//-----------------------------------------------------------------
//
// This was all stolen or inspired by existing macros.
//
// Note: Many projects have their own variations on these macros.
// If this is the case, and you want to use this file, make sure to
// #include this AFTER your project's headers to get your projects's
// definitions for the names defined here.
//
// The test macros generally expect functions with the following form:
/*
HRESULT Foo ()
{
    HRESULT hr;
    // ... 
Error:
    //Cleanup
    RELEASE(piface);
    return hr;
}
*/
//
//-----------------------------------------------------------------
#ifdef _MSC_VER
#pragma once
#endif

#ifndef __COMMACRO_H__
#define __COMMACRO_H__


// AddRef/Release with guard for NULL
#ifndef ADDREF
#define ADDREF(PUNK) { if (PUNK) (PUNK)->AddRef(); }
#endif

#ifndef RELEASE
#define RELEASE(PUNK)                \
{                                    \
    if (PUNK)                        \
    {                                \
        LPUNKNOWN punkXXX = (PUNK);  \
        (PUNK) = NULL;               \
        punkXXX->Release();          \
    }                                \
}
#endif

// In multiple inheritance you might need to disambiguate which IUnknown to use.
#ifndef RELEASETYPE
#define RELEASETYPE(PUNK,TYPE)              \
{                                           \
    if (PUNK)                               \
    {                                       \
        LPUNKNOWN punkXXX = (TYPE *)(PUNK); \
        (PUNK) = NULL;                      \
        punkXXX->Release();                 \
    }                                       \
}
#endif

// NON_COOKIE : never a valid/'live' cookie.
// I would have named it "INVALID_COOKIE" but that is 
// already used elsewhere in VS with varying definitions.
#define NON_COOKIE  ((DWORD)-2)

// unadvise and release a connection point
#ifndef UNADVISE_RELEASE
#define UNADVISE_RELEASE(pcp, cookie) \
{                                     \
    if (pcp)                          \
    {                                 \
        pcp->Unadvise(cookie);        \
        pcp->Release();               \
        pcp = NULL;                   \
    }                                 \
}
#endif

// write optional return value
#ifndef SETRETVAL
#define SETRETVAL(pv, v) { if (pv) (*(pv)) = (v); }
#endif

// sense of test is the invalid condition
#ifndef INVALIDARG
#define INVALIDARG(expr)        { if (expr) return E_INVALIDARG; }
#define INVALIDARG_GO(expr)     { if (expr) { hr = E_INVALIDARG; goto Error; } }
#endif

// sense of test is the valid condition
#ifndef VALIDARG
#define VALIDARG(expr)          { if (!(expr)) return E_INVALIDARG; }
#define VALIDARG_GO(expr)       { if (!(expr)) { hr = E_INVALIDARG; goto Error; } }
#endif

// sense of test is the unexpected condition
#ifndef UNEXPECTEDARG
#define UNEXPECTEDARG(expr)     { if (expr) return E_UNEXPECTED; }
#define UNEXPECTEDARG_GO(expr)  { if (expr) { hr = E_UNEXPECTED; goto Error; } }
#endif

// sense of test is the expected condition
#ifndef EXPECTEDARG
#define EXPECTEDARG(expr)       { if (!(expr)) return E_UNEXPECTED; }
#define EXPECTEDARG_GO(expr)    { if (!(expr)) { hr = E_UNEXPECTED; goto Error; } }
#endif

#ifndef EXPECTEDPTR
#define EXPECTEDPTR(ptr)        { if (NULL == ptr) return E_POINTER; }
#define EXPECTEDPTR_GO(ptr)     { if (NULL == ptr) { hr = E_POINTER; goto Error; } }
#endif

#ifndef IFSUCCEEDED
#define IFSUCCEEDED(expr)       if (SUCCEEDED(hr = (expr)))
#endif

#ifndef IFFAILED
#define IFFAILED(expr)          if (FAILED(hr = (expr)))
#endif


#ifndef IfFailGoto
#define IfFailGoto(expr, label)  { hr = (expr); if(FAILED(hr)) goto label; }
#endif

#ifndef IfFailRet
#define IfFailRet(expr)          { hr = (expr); if(FAILED(hr)) return hr; }
#endif

#ifndef IfFailGo
#define IfFailGo(expr)           { hr = (expr); if(FAILED(hr)) goto Error; }
#endif

// allocated memory check
#ifndef IfNullMemGoto
#define IfNullMemGoto(ptr, label)   { if (!ptr) { hr = E_OUTOFMEMORY; goto label; } }
#endif

#ifndef IfNullMemGo
#define IfNullMemGo(ptr)            { if (!ptr) { hr = E_OUTOFMEMORY; goto Error; } }
#endif

#ifndef IfNullMemRet
#define IfNullMemRet(ptr)           { if (!ptr) return E_OUTOFMEMORY; }
#endif

// simple expression checking
#ifndef IfFailExprGo
#define IfFailExprGo(expr, hrret)      { if (!expr) { hr = hrret; goto Error; } }
#endif

#ifndef ____COMMACRO_UNKNOWN____
#define ____COMMACRO_UNKNOWN____

// standard declaration of IUnknown
#define DECLARE_STD_UNKNOWN \
  STDMETHOD (QueryInterface)(REFIID riid, void __RPC_FAR * __RPC_FAR * ppvObj); \
  STDMETHOD_(ULONG, AddRef )(); \
  STDMETHOD_(ULONG, Release)();

// Sometimes an object has only IUnknown and can use this complete QI implementation.
//
#define STD_UNKNOWN_QI_IMPL(__Class) \
STDMETHODIMP __Class::QueryInterface(REFIID riid, void __RPC_FAR * __RPC_FAR * ppvObj) \
{ \
  if (NULL == ppvObj) return E_POINTER; \
  *ppvObj = NULL;                       \
  if (riid == IID_IUnknown) *ppvObj = (IUnknown *)this; \
  else return E_NOINTERFACE; \
  AddRef(); \
  return NOERROR; \
}

#define STD_QI_IMPL_BEGIN(__Class) \
STDMETHODIMP __Class::QueryInterface(REFIID riid, void __RPC_FAR *__RPC_FAR * ppvObj) \
{ \
  EXPECTEDPTR(ppvObj); \
  *ppvObj = NULL;

// __UnkClass is used to disambiguate IUnknown. 
// Use IUnknown when no disambiguation is necessary.
#define STD_QI_MAP_BEGIN(__UnkClass) \
  if (riid == IID_IUnknown) *ppvObj = (IUnknown*)(__UnkClass*)this;

#define STD_QI_MAP_ENTRY(__Interface) \
  else if (riid == IID_##__Interface) *ppvObj = (__Interface*)this;

#define STD_QI_MAP_ENTRY2(__Interface, __QClass) \
  else if (riid == IID_##__Interface) *ppvObj = (__Interface*)(__QClass*)this;

#define STD_QI_MAP_END \
  else return E_NOINTERFACE;

#define STD_QI_IMPL_END \
  AddRef(); \
  return NOERROR; \
}

#define STD_ADDREF_IMPL(__Class, __m_cRef) \
STDMETHODIMP_(ULONG) __Class::AddRef () \
{ \
  return ++__m_cRef; \
}

#define STD_RELEASE_IMPL(__Class, __m_cRef) \
STDMETHODIMP_(ULONG) __Class::Release () \
{ \
  if (--__m_cRef)       \
    return (__m_cRef);  \
  delete this;          \
  return 0;             \
}

#define STD_ADDREF_RELEASE_IMPL(__Class, __m_cRef) \
  STD_ADDREF_IMPL(__Class, __m_cRef) \
  STD_RELEASE_IMPL(__Class, __m_cRef)

#endif // ____COMMACRO_UNKNOWN____


#endif // __COMMACRO_H__
