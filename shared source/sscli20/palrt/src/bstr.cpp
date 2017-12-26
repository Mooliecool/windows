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
// File: bstr.cpp
// 
// ===========================================================================


/*++

Abstract:

    PALRT BSTR support

Revision History:

--*/

#include "rotor_palrt.h"

#define CCH_BSTRMAX 0x7FFFFFFF  // 4 + (0x7ffffffb + 1 ) * 2 ==> 0xFFFFFFFC
#define CB_BSTRMAX 0xFFFFFFFa   // 4 + (0xfffffff6 + 2) ==> 0xFFFFFFFC

#define WIN32_ALLOC_ALIGN (16 - 1)      

inline unsigned long CbSysStringSize(unsigned long cchSize, BOOL isByteLen)
{
    // +2 for the null terminator
    // + DWORD_PTR to store the byte length of the string
    if (isByteLen) {
        return ((cchSize) + 2 + sizeof(DWORD_PTR)+ WIN32_ALLOC_ALIGN) & ~WIN32_ALLOC_ALIGN; // +2 is for cbNull
    }
    else {
        return (((cchSize) * 2) + 2 + sizeof(DWORD_PTR)+ WIN32_ALLOC_ALIGN) & ~WIN32_ALLOC_ALIGN;
    }
}

/***
*BSTR SysAllocStringLen(char*, unsigned int)
*Purpose:
*  Allocation a bstr of the given length and initialize with
*  the pasted in string
*
*Entry:
*  [optional]
*
*Exit:
*  return value = BSTR, NULL if the allocation failed.
*
***********************************************************************/
STDAPI_(BSTR) SysAllocStringLen(const OLECHAR *psz, UINT len)
{
    
    BSTR bstr;
    DWORD cbTotal;

    _ASSERTE(len <= CCH_BSTRMAX);         // assert if way too big a request
    if (len > CCH_BSTRMAX)
        return NULL;            // fail gracefully in retail w/o overflowing

    cbTotal = CbSysStringSize(len, FALSE);

    bstr = (OLECHAR *)HeapAlloc(GetProcessHeap(), 0, cbTotal);

    if(bstr != NULL){

      *(DWORD FAR*)bstr = (DWORD)len * sizeof(OLECHAR);

      bstr = (BSTR) ((char*) bstr + sizeof(DWORD));

      if(psz != NULL){
            memcpy(bstr, psz, len * sizeof(OLECHAR));
      }

      bstr[len] = '\0'; // always 0 terminate
    }

    return bstr;
}

/***
*BSTR SysAllocString(char*)
*Purpose:
*  Allocation a bstr using the passed in string
*
*Entry:
*  String to create a bstr for
*
*Exit:
*  return value = BSTR, NULL if allocation failed
*
***********************************************************************/
STDAPI_(BSTR) SysAllocString(const OLECHAR* psz)
{
    if(psz == NULL)
      return NULL;

    return SysAllocStringLen(psz, (DWORD)wcslen(psz));
}

STDAPI_(BSTR)
SysAllocStringByteLen(const char FAR* psz, unsigned int len)
{
    BSTR bstr;
    DWORD cbTotal;

    _ASSERTE(len <= CB_BSTRMAX);    // assert if way too big a request
    if (len > CB_BSTRMAX)
        return NULL;                // fail gracefully in retail w/o overflowing

    cbTotal = CbSysStringSize(len, TRUE);

    bstr = (OLECHAR *)HeapAlloc(GetProcessHeap(), 0, cbTotal);

    if (bstr != NULL) {
      *(DWORD FAR*)bstr = (DWORD)len;

      bstr = (WCHAR*) ((char*) bstr + sizeof(DWORD_PTR));

      if (psz != NULL) {
            memcpy(bstr, psz, len);
      }

      // NULL-terminate with both a narrow and wide zero.
      *((char *)bstr + len) = '\0';
      *(WCHAR *)((char *)bstr + ((len + 1) & ~1)) = 0;
    }

    return bstr;
}

/***
*void SysFreeString(BSTR)
*Purpose:
*  Free the given BSTR.
*
*Entry:
*  bstr = the BSTR to free
*
*Exit:
*  None
*
***********************************************************************/
STDAPI_(void) SysFreeString(BSTR bstr)
{
    if(bstr == NULL)
      return;
    HeapFree(GetProcessHeap(), 0, (BYTE *)bstr-sizeof(DWORD_PTR));    
}

/***
*unsigned int SysStringLen(BSTR)
*Purpose:
*  return the length in characters of the given BSTR.
*
*Entry:
*  bstr = the BSTR to return the length of
*
*Exit:
*  return value = unsigned int, length in characters.
*
***********************************************************************/
STDAPI_(unsigned int)
SysStringLen(BSTR bstr)
{
    if(bstr == NULL)
      return 0;
    return (unsigned int)((((DWORD FAR*)bstr)[-1]) / sizeof(OLECHAR));
}

/***
*unsigned int SysStringByteLen(BSTR)
*Purpose:
*  return the size in bytes of the given BSTR.
*
*Entry:
*  bstr = the BSTR to return the size of
*
*Exit:
*  return value = unsigned int, size in bytes.
*
***********************************************************************/
STDAPI_(unsigned int)
SysStringByteLen(BSTR bstr)
{
    if(bstr == NULL)
      return 0;
    return (unsigned int)(((DWORD FAR*)bstr)[-1]);
}

extern "C" HRESULT
ErrStringCopy(BSTR bstrSource, BSTR FAR *pbstrOut)
{
    if (bstrSource == NULL) {
	*pbstrOut = NULL;
	return NOERROR;
    }
    if ((*pbstrOut = SysAllocStringLen(bstrSource,
                                       SysStringLen(bstrSource))) == NULL)
	return E_OUTOFMEMORY;

    return NOERROR;
}

/***
*PRIVATE HRESULT ErrSysAllocString(char*, BSTR*)
*Purpose:
*  This is an implementation of SysAllocString that check for the
*  NULL return value and return the corresponding error - E_OUTOFMEMORY.
*
*  This is simply a convenience, and this routine is only used
*  internally by the oledisp component.
*
*Entry:
*  psz = the source string
*
*Exit:
*  return value = HRESULT
*    S_OK
*    E_OUTOFMEMORY
*
*  *pbstrOut = the newly allocated BSTR
*
***********************************************************************/
extern "C" HRESULT
ErrSysAllocString(const OLECHAR FAR* psz, BSTR FAR* pbstrOut)
{
    if(psz == NULL){
      *pbstrOut = NULL;
      return NOERROR;
    }

    if((*pbstrOut = SysAllocString(psz)) == NULL)
      return E_OUTOFMEMORY;

    return NOERROR;
}
