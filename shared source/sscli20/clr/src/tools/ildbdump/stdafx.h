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
// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently, but
// are changed infrequently
//

#ifndef _ILDBDUMP_H_
#define _ILDBDUMP_H_

#include "rotor_pal.h"
#include "rotor_palrt.h"

#define NEW( x ) ( ::new (nothrow) x )
#define DELETE( x ) ( ::delete(x) )
#define DELETEARRAY( x ) (::delete[] (x))

#include "oaidl.h"
#include "oleauto.h"

#include "cor.h"
#include "corimage.h"

#include "pdbdata.h"
#include "symwrite.h"

//---------------------------------------------------------------------------
// Cover for GetLastError that return an HRESULT.  If last error was not set,
// then we are forced to E_FAIL.
//---------------------------------------------------------------------------
HRESULT HrLastError()
{
  long lErr = GetLastError();
  return lErr ? HRESULT_FROM_WIN32(lErr) : E_FAIL;
}

#ifndef IfFailGoto
#define IfFailGoto(EXPR, LABEL) \
do { hr = (EXPR); if(FAILED(hr)) { goto LABEL; } } while (0)
#endif

#ifndef IfFailRet
#define IfFailRet(EXPR) \
do { hr = (EXPR); if(FAILED(hr)) { return (hr); } } while (0)
#endif

#ifndef IfFailGo
#define IfFailGo(EXPR) IfFailGoto(EXPR, Error)
#endif

#ifndef IfNullGo
#define IfNullGo(EXPR) if (EXPR == NULL) { hr = E_OUTOFMEMORY; goto Error; }
#endif

#ifndef IfFalseGo
#define IfFalseGo(EXPR, ERRORCODE) if (!EXPR) { hr = ERRORCODE; goto Error; }
#endif

#endif // _ILDBDUMP_H_

