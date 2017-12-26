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
// File: oautil.h - OLE Automation component-wide utility functions.
// 
// ===========================================================================
/***
*
*Purpose:
*  OA-wide utility function headers.
*
*Implementation Notes:
*
*****************************************************************************/

#ifndef OAUTIL_H_INCLUDED
#define OAUTIL_H_INCLUDED

#ifndef EXTERN_C
# ifdef __cplusplus
#  define EXTERN_C extern "C"
# else
#  define EXTERN_C extern
# endif
#endif

#ifdef _DEBUG
# define ASSERTDATA static char g_szFileName[] = __FILE__;
#else
# define ASSERT(X)
#ifdef NETDISP
# define ASSERTSZ(X, SZ)
#endif
# define ASSERTDATA
#endif

# define INTERNALCALLTYPE    __stdcall
# define INTERNALCALL_(TYPE) TYPE INTERNALCALLTYPE

#define INTERNAL_(TYPE)  INTERNALCALL_(TYPE)


#define HRESULT_FAILED(X) ((X) != NOERROR && FAILED((SCODE)(X)))

#define HRESULT_SUCCESS(X) ((X) == NOERROR || !FAILED((SCODE)(X)))

#define IfFailGo(expression, label)	\
    { hresult = (expression);		\
      if(HRESULT_FAILED(hresult))	\
	goto label;         		\
    }

#define IfFailRet(expression)		\
    { HRESULT hresult = (expression);	\
      if(HRESULT_FAILED(hresult))	\
	return hresult;			\
    }

#define RESULT(X)   ((HRESULT)(X))

#   // all assume we are operating on unicode characters
#define SIZEOFCH(x) (sizeof(x)/2)
#define OASTR(str) L##str
#define BYTELEN(x) (wcslen(x)*2+2)

// private SysAllocString helper that return an HRESULT

extern "C" HRESULT
ErrSysAllocString(const OLECHAR FAR* psz, BSTR FAR* pbstrOut);

extern "C" HRESULT
ErrStringCopy(BSTR bstrSource, BSTR FAR *pbstrOut);

// convert.cpp
//

EXTERN_C INTERNAL_(int) IsTaiwan(LCID lcid);
EXTERN_C INTERNAL_(int) IsChina(LCID lcid);
EXTERN_C INTERNAL_(int) IsThai(LCID lcid);

#define xchThaiZero 0x0e50
#define xchThaiNine 0x0e59


// private NLS wrapper functions (for WIN32)

EXTERN_C INTERNAL_(int)
IsCharType(LCID lcid, OLECHAR ch, DWORD dwType);

extern "C"
INTERNAL_(HRESULT) DispAlloc(size_t cb, void FAR* FAR* ppv);

extern "C"
INTERNAL_(void) DispFree(void FAR* pv);

#endif  // ! OAUTIL_H_INCLUDED

