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
// File: umisc.h
// 
// ===========================================================================


// Abstract:
//
//	A collection of utility macros.
//

#ifndef UMISC_H
#define UMISC_H

#define COM_METHOD  HRESULT STDMETHODCALLTYPE

inline HRESULT HrFromWin32(DWORD dwWin32Error)
{
    return HRESULT_FROM_WIN32(dwWin32Error);
}

// Some helper #def's to safely Release, close & delete Objects under
// failure conditions
	
#define RELEASE(x) 			    \
	do						    \
	{						    \
		if (x)				    \
		{					    \
            IUnknown *punk = x; \
            x = NULL;           \
            punk->Release();    \
		}					    \
	} while (0)				
	

#include "debugmacros.h"	
//
// Good for verifying params withing range.
//

#define IfFalseGo(expr, HR)				\
	do {								\
		if (!(expr)) {					\
			hr = (HR);					\
			goto ErrExit;					\
		}								\
	} while (0)


//
// Check a boolean, condition jump to label 'Error'.
//

#define IfFalseGoto(_cond)\
	do {								\
		if (!(_cond)) {					\
			goto ErrExit;					\
		}								\
	} while (0)


#if _DEBUG
// ----------------------------------------------------------------------------
// Validation macros

#define IsValidReadPtr(ptr, type) !IsBadReadPtr((ptr), sizeof(type))

#define IsValidWritePtr(ptr, type) !IsBadWritePtr((ptr), sizeof(type))

#define IsValidReadBufferPtr(ptr, type, len) !IsBadReadPtr((ptr), sizeof(type) * (len))

#define IsValidWriteBufferPtr(ptr, type, len) !IsBadWritePtr((ptr), sizeof(type) * (len))

#define IsValidInterfacePtr(ptr, type) IsValidReadPtr(ptr, type)

#define IsValidCodePtr(ptr) !IsBadCodePtr((FARPROC)(ptr))

#define IsValidStringPtr(ptr) !IsBadStringPtr((ptr), 0xffffffff)

#define IsValidIID(iid) IsValidReadPtr(&(iid), IID)

#define IsValidCLSID(clsid) IsValidReadPtr(&(clsid), CLSID)
#else

#define IsValidReadPtr(ptr, type)
#define IsValidWritePtr(ptr, type)
#define IsValidReadBufferPtr(ptr, type, len)
#define IsValidWriteBufferPtr(ptr, type, len)
#define IsValidInterfacePtr(ptr, type)
#define IsValidCodePtr(ptr)
#define IsValidStringPtr(ptr)
#define IsValidIID(iid)
#define IsValidCLSID(clsid)

#endif

#endif
