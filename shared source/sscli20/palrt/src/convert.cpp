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
// File: convert.cpp
// 
// ===========================================================================

/***
*
*Purpose:
*  This module contains the low level VARTYPE coersion API.
*
*Revision History:
*
*Implementation Notes:
*
*****************************************************************************/

#include "rotor_palrt.h"

#include "oautil.h"
#include "oleauto.h"
#include "crtstuff.h"

#include "convert.h"

#include <limits.h>
#include <math.h>

// Per-app APP_DATA instance
ITLS g_itlsAppData = ITLS_EMPTY;

// Thai characters begin/end margin.
#define xchAbvKorKai    0x0e00
#define xchKorKai       0x0e01
#define xchFongMun      0x0e4f

// Special for fix NLS bug
#define xchAnsiPeriod   0x002e

const OLECHAR szChinaNumZero[] = L"\x25cb\x96f6";
const OLECHAR szChinaNumOne[] = L"\x4e00\x58f9";
const OLECHAR szChinaNumTwo[] = L"\x4e8c\x8d30";
const OLECHAR szChinaNumThree[] = L"\x4e09\x53c1";
const OLECHAR szChinaNumFour[] = L"\x56db\x8086";
const OLECHAR szChinaNumFive[] = L"\x4e94\x4f0d";
const OLECHAR szChinaNumSix[] = L"\x516d\x9646";
const OLECHAR szChinaNumSeven[] = L"\x4e03\x67d2";
const OLECHAR szChinaNumEight[] = L"\x516b\634c";
const OLECHAR szChinaNumNine[] = L"\x4e5d\x7396";
const OLECHAR szChinaNumTen[] = L"\x5341\x62fe";
const OLECHAR szChinaNumHundred[] = L"\x767e";
const OLECHAR szChinaNumThousand[] = L"\x5343";
const OLECHAR szChinaNumTenThousand[] = L"\x4e07";
const OLECHAR szChinaNumHundredMillion[] = L"\x4ebf";
const OLECHAR szChinaNumTrillion[] = L"\x5146";

const int g_NumChinaNum = 16;
const OLECHAR * const g_szChinaNum[g_NumChinaNum] =
{
  szChinaNumZero,
  szChinaNumOne,
  szChinaNumTwo,
  szChinaNumThree,
  szChinaNumFour,
  szChinaNumFive,
  szChinaNumSix,
  szChinaNumSeven,
  szChinaNumEight,
  szChinaNumNine,
  szChinaNumTen,
  szChinaNumHundred,
  szChinaNumThousand,
  szChinaNumTenThousand,
  szChinaNumHundredMillion,
  szChinaNumTrillion
};

const unsigned __int64 ulPower10[] = {1,
                    UI64(10),
                    UI64(100),
                    UI64(1000),
                    UI64(10000),
                    UI64(100000),
                    UI64(1000000),
                    UI64(10000000),
                    UI64(100000000),
                    UI64(1000000000),
                    UI64(10000000000),
                    UI64(100000000000),
                    UI64(1000000000000),
                    UI64(10000000000000),
                    UI64(100000000000000),
                    UI64(1000000000000000),
                    UI64(10000000000000000),
                    UI64(100000000000000000),
                    UI64(1000000000000000000),
                    UI64(10000000000000000000)};

const DBLSTRUCT  dsR4Max = DEFDS(0xEFFFFFFF, 0xFFFFF, DBLBIAS + 128, 0);

extern "C" {

INTERNAL_(HRESULT)
DispAlloc(size_t cb, void FAR* FAR* ppv)
{
    void FAR* pv;
    HRESULT hresult = NOERROR;
    
    if ((pv = HeapAlloc(GetProcessHeap(), 0, cb )) == NULL)
      hresult = E_OUTOFMEMORY;
    
    *ppv = pv;

    return hresult;
}

INTERNAL_(void)
DispFree(void FAR* pv)
{

    if(pv == NULL)
      return;
    HeapFree(GetProcessHeap(), 0, pv);    
}

};

const NUMINFO niEnglish = {
        MAKELCID(MAKELANGID(LANG_ENGLISH, SUBLANG_ENGLISH_US), SORT_DEFAULT), // lcid 0x0409
        0, // dwFlags
        1,          // cchCurrency
        TRUE,       // fFastCur
        { '$', '\0' }, // rgbCurrency
        '.',        // chDecimal
        ',',        // chThousand1
        ',',        // chThousand2
        TRUE,       // fLeadingZero
        '.',        // chCurrencyDecimal
        ',',        // chCurrencyThousand
};

class CNumInfo
{
public:
    CNumInfo();
    ~CNumInfo();
    NUMINFO *GetPNumInfo() { return &m_numinfo; }

    ULONG AddRef();
    ULONG Release();

    static HRESULT Create(LCID lcid, DWORD dwFlags, CNumInfo**ppDateInfo);
    static HRESULT Create(LCID lcid, DWORD dwFlags, NUMINFO **ppDateInfo);

    CNumInfo *GetNext()
    {
        return m_pNext;
    }

private:
    NUMINFO m_numinfo;
    CNumInfo* m_pNext;
    CNumInfo* m_pPrev;
    LONG m_cRefs;

    HRESULT Init(LCID lcid, unsigned long dwFlags);
    static CNumInfo *m_pNumInfoCache;
    friend void InitNumInfo(BOOL fInit);

};

        
/***
*HRESULT GetNumInfo(LCID lcid, unsigned long dwFlags, NUMINFO ** ppnuminfo)
*Purpose:
*  Fill in the given NUMINFO structure.
*
*Entry:
*  None
*
*Exit:
*  return value = HRESULT
*
*  *ppnuminfo = * to the filled NUMINFO struct
*
***********************************************************************/
HRESULT
GetNumInfo(LCID lcid, unsigned long dwFlags, NUMINFO ** ppnuminfo)
{
    CNumInfo *pNumInfo = NULL;

    *ppnuminfo = NULL;
    
    // Rotor Support
    _ASSERTE(lcid == LOCALE_NEUTRAL || lcid == 0x0409 || lcid == LOCALE_USER_DEFAULT);

    if (lcid == LOCALE_USER_DEFAULT)
        lcid = GetUserDefaultLCID();


    APP_DATA * pappdata = NULL;
    IfFailRet(GetAppData(&pappdata));

    pNumInfo = (CNumInfo*)pappdata->m_pNumInfo;

    if (pNumInfo)
    {
        NUMINFO *p = pNumInfo->GetPNumInfo();
        if (p->lcid == lcid && p->dwFlags == dwFlags)
        {
            *ppnuminfo = p;
            return S_OK;
        }
    }


    // call out to C++
    IfFailRet(CNumInfo::Create(lcid, dwFlags, &pNumInfo));

    if (pappdata) {
        if (pappdata->m_pNumInfo)
            ((CNumInfo*)pappdata->m_pNumInfo)->Release();
        pappdata->m_pNumInfo = pNumInfo;
    }

    *ppnuminfo = pNumInfo->GetPNumInfo();
    
    return NOERROR;
}

void InitNumInfo(BOOL fInit)
{
    if (fInit)
    {
        InitializeCriticalSection(&NumInfo_csCacheLock);
    }
    else
    {
        CNumInfo *pInfo = CNumInfo::m_pNumInfoCache;
        CNumInfo *pNext;

        if (pInfo) 
        {
            while (TRUE)
            {
                pNext = pInfo->m_pNext;
                pInfo->Release();
                if (pNext == CNumInfo::m_pNumInfoCache)
                        break;
                pInfo = pNext;
            }
        }

        DeleteCriticalSection(&NumInfo_csCacheLock);
    }
}

CNumInfo::CNumInfo() 
{
    memset(&m_numinfo, 0, sizeof(m_numinfo));
    m_pNext = NULL;
    m_pPrev = NULL;
    m_cRefs = 0;
}

CNumInfo::~CNumInfo() 
{ 
    m_pNext = NULL;
    m_pPrev = NULL;
}

CNumInfo *CNumInfo::m_pNumInfoCache = NULL;
CRITICAL_SECTION NumInfo_csCacheLock;

HRESULT CNumInfo::Create(LCID lcid, DWORD dwFlags, NUMINFO **ppNumInfo)
{
    HRESULT hr;
    CNumInfo *pdi = NULL;

    hr = Create(lcid, dwFlags, &pdi);
    if (SUCCEEDED(hr))
        *ppNumInfo = &pdi->m_numinfo;
    return hr;
}

HRESULT CNumInfo::Create(LCID lcid, DWORD dwFlags, CNumInfo**ppNumInfo)
{
    CNumInfo *pInfo = *ppNumInfo;
    HRESULT hr = S_OK;

    // Require caller to handle the default conversions
    _ASSERTE(lcid != LOCALE_USER_DEFAULT);

    if (pInfo)
    {
        NUMINFO *p = &pInfo->m_numinfo;
        if (p->lcid == lcid && p->dwFlags == dwFlags)
        {
            return S_OK;
        }
    }
        
    EnterCriticalSection(&NumInfo_csCacheLock);
    CNumInfo *pInfoHead = m_pNumInfoCache;
    pInfo = pInfoHead;
    if (pInfoHead)
    {
        while (TRUE)
        {
            NUMINFO *p = &pInfo->m_numinfo;

            if ((p->lcid == lcid) && (p->dwFlags == dwFlags))
            {
                // found it
                break;
            }
            pInfo = pInfo->m_pNext;
            if (pInfo == pInfoHead) 
            {
                pInfo = NULL;
                break;
            }

        }
    }

    if (pInfo == NULL)
    {
        // Need to create a new one
        pInfo = new CNumInfo();
        if (NULL == pInfo) 
        {
            hr = E_OUTOFMEMORY;
            goto Done;
        }

        pInfo->AddRef();
        pInfo->Init(lcid, dwFlags);

        if (m_pNumInfoCache)
        {
            pInfo->m_pPrev = m_pNumInfoCache->m_pPrev;
            m_pNumInfoCache->m_pPrev->m_pNext = pInfo;
            m_pNumInfoCache->m_pPrev = pInfo;
            pInfo->m_pNext = m_pNumInfoCache;
        }
        else
        {
            // Make this circular list
            pInfo->m_pNext = pInfo;
            pInfo->m_pPrev = pInfo;
        }
        m_pNumInfoCache = pInfo;
    }

    // AddRef on this class is not necessary, as long as we never 
    // release the class until we get unloaded
    pInfo->AddRef();
    *ppNumInfo = pInfo;

    // Make most recently used first in list
    if (m_pNumInfoCache != pInfo)
        m_pNumInfoCache = pInfo;

Done:
    LeaveCriticalSection(&NumInfo_csCacheLock);

    return hr;
}

HRESULT
CNumInfo::Init(LCID lcid, unsigned long dwFlags)
{

    // Rotor only uses these 2 parameters;

    _ASSERTE(lcid == 0x0409 || lcid == LOCALE_NEUTRAL);

    // OPTIMIZATION: If we're looking for plain English, give back a standard
    // form
    if (lcid == MAKELCID(MAKELANGID(LANG_ENGLISH, SUBLANG_ENGLISH_US), SORT_DEFAULT)) {
        m_numinfo = niEnglish;
        return S_OK;
    }
    

    NUMINFO * pnuminfo;

    pnuminfo = &m_numinfo;

    // Else cache is marked invalid (lcid==-1), or cache contains info
    // for another lcid.  In either case, must build a new NUMINFO

    _ASSERTE(dwFlags == 0 || dwFlags == LOCALE_NOUSEROVERRIDE);
    int len = 0;

    OLECHAR szBuff[4];

    len = GetLocaleInfoW(lcid,
                         LOCALE_SCURRENCY | dwFlags,
                         pnuminfo->rgbCurrency,
                         SIZEOFCH(pnuminfo->rgbCurrency)) - 1;

    // len == # of chars, NOT including NULL terminator

    if (len <= 0) {
      // GetLocaleInfo failed, or no currency symbol specified.
      // Note for compatibility with previous versions of OA, assume the
      // currency symbol is a '$'.
      pnuminfo->rgbCurrency[0] = OASTR('$');
      pnuminfo->cchCurrency = 1;
      pnuminfo->fFastCur = TRUE;
    } else {
      pnuminfo->cchCurrency = len;
      pnuminfo->fFastCur = FALSE;
    }


    // Note that GetLocaleInfo returns the # of chars INCLUDING the NULL.
    if (GetLocaleInfoW(lcid,
                      LOCALE_SDECIMAL | dwFlags,
                      szBuff, SIZEOFCH(szBuff)) <= 1) {
      // GetLocaleInfo failed, or no decimal symbol specified.  Just assume
      // '.' for the decimal symbol, because our code can't deal with having
      // no decimal symbol.
      pnuminfo->chDecimal = OASTR('.');
    } else {
      pnuminfo->chDecimal = szBuff[0];
      _ASSERTE(pnuminfo->chDecimal != OASTR('\0'));
    }

    // Note that GetLocaleInfo returns the # of chars INCLUDING the NULL.
    if (GetLocaleInfoW(lcid,
                      LOCALE_SMONDECIMALSEP | dwFlags,
                      szBuff, SIZEOFCH(szBuff)) <= 1) {
      // GetLocaleInfo failed, or no decimal symbol specified.  Just assume
      // '.' for the decimal symbol, because our code can't deal with having
      // no decimal symbol.
      pnuminfo->chCurrencyDecimal = pnuminfo->chDecimal;
    } else {
      pnuminfo->chCurrencyDecimal = szBuff[0];
      _ASSERTE(pnuminfo->chCurrencyDecimal != OASTR('\0'));
    }

    // Note that GetLocaleInfo returns the # of chars INCLUDING the NULL.
    if (GetLocaleInfoW(lcid,
                      LOCALE_SMONTHOUSANDSEP | dwFlags,
                      szBuff,
                      SIZEOFCH(szBuff)) <= 1) {
      pnuminfo->chCurrencyThousand = OASTR(',');
    } else {
      pnuminfo->chCurrencyThousand = szBuff[0];
    }

    // Note that GetLocaleInfo returns the # of chars INCLUDING the NULL.
    if (GetLocaleInfoW(lcid,
                      LOCALE_STHOUSAND | dwFlags,
                      szBuff,
                      SIZEOFCH(szBuff)) <= 1) {
      // GetLocaleInfo failed, or no thousands separator specified.
      // For compatibilty with previous versions of OA, assume we have
      // no thousands separator (don't assume ',').  Code must be able
      // to handle the case where pnuminfo->chThousand1 == 0.
      pnuminfo->chThousand1 = OASTR('\0');
    } else {
      pnuminfo->chThousand1 = szBuff[0];
    }

    if (pnuminfo->chThousand1 == pnuminfo->chDecimal) {
      pnuminfo->chThousand1 = OASTR('\0');
    }

    // set up alternate thousands separator, to handle the case where the
    // thousands separator is some wierd-ass char (like a non-breaking space)
    // that is NOT a space, but is to be treated like one.
    pnuminfo->chThousand2 = (IsCharType(lcid, pnuminfo->chThousand1, C1_SPACE))
         ? OASTR(' ') : pnuminfo->chThousand1;

    // If the currency separator is the same as the decimal separator, then
    // things will still work ok.  We look for the decimal separator before
    // the currency separator.

    // If the currency separator is the same as the thousands separator, then
    // things will still work OK.  We will strip off all of them.

    WCHAR szBuff2[2];

    szBuff2[0] = '1';           // assume leading zero, in case call fails
    GetLocaleInfoW(lcid, LOCALE_ILZERO | dwFlags, szBuff2, sizeof(szBuff2));
    pnuminfo->fLeadingZero = (szBuff2[0] ==L'0') ? 0 : 1;

    pnuminfo->lcid = lcid;
    pnuminfo->dwFlags = dwFlags;   // cache hit requires both LCID and flags
    
    return NOERROR;
}

ULONG CNumInfo::AddRef() 
{ 
    return InterlockedIncrement(&m_cRefs); 
}

ULONG CNumInfo::Release() 
{ 
    LONG cRefs = InterlockedDecrement(&m_cRefs); 
    if (cRefs == 0)
    {
        delete this;
    }
    return cRefs;
}

EXTERN_C INTERNAL_(int)
IsThai(LCID lcid)
{
  if (lcid == LOCALE_USER_DEFAULT || lcid == LOCALE_NEUTRAL)
    lcid = GetUserDefaultLCID();

  return (PRIMARYLANGID(LANGIDFROMLCID(lcid)) == LANG_THAI);
}

EXTERN_C INTERNAL_(int)
IsTaiwan(LCID lcid)
{
  if (lcid == LOCALE_USER_DEFAULT || lcid == LOCALE_NEUTRAL)
    lcid = GetUserDefaultLCID();

  return (LANGIDFROMLCID(lcid) == MAKELANGID(LANG_CHINESE, SUBLANG_CHINESE_TRADITIONAL));
}

EXTERN_C INTERNAL_(int)
IsChina(LCID lcid)
{
  if (lcid == LOCALE_USER_DEFAULT || lcid == LOCALE_NEUTRAL)
    lcid = GetUserDefaultLCID();

  LANGID primary = PRIMARYLANGID(LANGIDFROMLCID(lcid));
  LANGID sub = SUBLANGID(LANGIDFROMLCID(lcid));
  return ((primary == LANG_CHINESE) && (sub != SUBLANG_CHINESE_TRADITIONAL));
}

/***
* FMakePosCy - make a positive currency value and return sign
* Purpose:
*   Return the positive value of the input currency value and a
*   flag with the sign of the original value.
*
* Entry:
*   pcyValue - pointer to currency input value
*
* Exit:
*   pcyValue - pointer to positive currency value
*   returns: FALSE if positive, TRUE if negative
*
* Exceptions:
*
* Note:
*   A maximum negative value input is returned unchanged, but
*   treated as an unsigned value by the calling routines.
*
***********************************************************************/

int
FMakePosCy(CY FAR* pcy)
{
    int fNegative;

    fNegative = FALSE;
    if(pcy->u.Hi < 0){
      pcy->u.Hi = ~pcy->u.Hi;
      if((pcy->u.Lo = (unsigned long)(-(long)pcy->u.Lo)) == 0)
        pcy->u.Hi++;
      fNegative = TRUE;
    }
    return fNegative;
}


/***
* UnpackCy - separate currency value into four two-byte integers
* Purpose:
*   Unpack the currency value input into the lower half of the
*   specified pointer to an array of unsigned longs.  The array
*   goes from least- to most-significant values.
*
* Entry:
*   pcy - pointer to currency input value
*
* Exit:
*   plValues - pointer to start of unsigned long array
*
* Exceptions:
*
***********************************************************************/

void
UnpackCy(CY FAR* pcy, unsigned long FAR* plValues)
{
    *plValues++ = pcy->u.Lo & 0xffff;
    *plValues++ = pcy->u.Lo >> 16;
    *plValues++ = (unsigned long)pcy->u.Hi & 0xffff;
    *plValues   = (unsigned long)pcy->u.Hi >> 16;
}

STDAPI
VarBstrFromCy(CY cyIn, LCID lcid, ULONG dwFlags, BSTR * pbstrOut)
{
    // Rotor doesn't support any extra flags
    _ASSERTE(dwFlags == 0);
    _ASSERTE(lcid == LOCALE_NEUTRAL);
    
    OLECHAR buffer[40];
    OLECHAR * pchBuffer = buffer;

#define CYSTRMAX    32  
        
    int index;
    int grpValue;
    int indResult;
    int fNegative;
    int fNzQuotient;
    OLECHAR chResult[CYSTRMAX];
    unsigned long input[4];
    NUMINFO * pnuminfo;

    _ASSERTE(dwFlags == 0 || dwFlags == LOCALE_NOUSEROVERRIDE);        

    IfFailRet(GetNumInfo(lcid, dwFlags, &pnuminfo));

    // if value is negative, set flag and negate
    // (max. negative value 0x80...0 works since it inverts to itself)

    fNegative = FMakePosCy(&cyIn);

    // split number into four short values

    UnpackCy(&cyIn, input);

    // string will be built from right to left
    // index to the end of the string (null-to-be)

    indResult = CYSTRMAX - 1;

    // outer loop to divide input array by 10000 repeatedly

    do {
      // flag is set if any quotient is nonzero to stop dividing

      fNzQuotient = FALSE;

      // divide the value in input by 10000, with the remainder
      // in grpValue

      for (index = 3; index > 0; index--) {

        input[index - 1] |= (input[index] % 10000) << 16;
        if ((input[index] /= 10000) != 0)
      fNzQuotient = TRUE;
      }

      grpValue = (int)(input[index] % 10000);
      if ((input[0] /= 10000) != 0)
        fNzQuotient = TRUE;

      // inner loop divides grpValue by 10 repeatedly to get digits

      for (index = 0; index < 4; index++) {
        chResult[--indResult] = (OLECHAR)(grpValue % 10 + OASTR('0'));
        grpValue /= 10;
      }

      // for first grouping, put in decimal point

      if (indResult == CYSTRMAX - 5)
        chResult[--indResult] = pnuminfo->chDecimal;

    }while (fNzQuotient);

    // trim any leading zeroes from the string

    while (chResult[indResult] == OASTR('0'))
      indResult++;

    // remove a leading zero to a decimal point depending on Locale setting
        
    if (pnuminfo->fLeadingZero && chResult[indResult] == pnuminfo->chDecimal)
      chResult[--indResult] = OASTR('0');      

    // trim any trailing zeroes from the string

    index = CYSTRMAX - 2;
    while (chResult[index] == OASTR('0'))
      index--;

    // process trailing decimal point

    if (chResult[index] == pnuminfo->chDecimal) {

      // if just decimal point, put in a zero before it depending on locale

      if (index == indResult)
        chResult[--indResult] = OASTR('0');

      // move before the decimal point

      index--;
    }

    // fix the end of the string

    chResult[++index] = OASTR('\0');

    // if negative, put sign in buffer
    if(fNegative)
      *pchBuffer++ = OASTR('-');

    wcscpy(pchBuffer, &chResult[indResult]);
    
    return ErrSysAllocString(buffer, pbstrOut);

}

/***
*VOID ReleaseAppData()
*
*Purpose:
*   Release per-app data 
*
*Inputs:
*
*Outputs:
*
******************************************************************************/
VOID ReleaseAppData()
{
    APP_DATA *pappdata;

    // Do we have the AppData
    if (g_itlsAppData != ITLS_EMPTY)
    {
        if ( (pappdata = Pappdata()) ) 
        {

            if (pappdata->m_pNumInfo != NULL) {
                ((CNumInfo*)pappdata->m_pNumInfo)->Release();
            }

            // Free the IErrorInfo
            if (pappdata->m_perrinfo) {
                pappdata->m_perrinfo->Release();
            }

            HeapFree(GetProcessHeap(), 0, pappdata);
        }
        TlsSetValue(g_itlsAppData, NULL);
    }
}

/***
*STDAPI InitAppData()
*
*Purpose:
*   Initializes per-app data that is used by oleaut32.dll.
*
*Inputs:
*
*Outputs:
*   TIPERROR
*
******************************************************************************/
STDAPI InitAppData()
{
    APP_DATA *pappdata;
    HRESULT  hr;

    if ((pappdata = (APP_DATA *)TlsGetValue(g_itlsAppData)) == NULL)
    {
      // 1st call for this instance
      pappdata = (APP_DATA *)HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, sizeof(APP_DATA) );
                                           
      if (pappdata == NULL) {
        hr = E_OUTOFMEMORY;
        goto Error;
      }


      // Call the constructor for APP_DATA
      pappdata->Init();

      // Cache the value of the g_itlsAppData
      if (!TlsSetValue(g_itlsAppData, pappdata)) {
        hr = E_OUTOFMEMORY;
        goto Error;
      }
    }

    return NOERROR;

Error:

    TlsSetValue(g_itlsAppData, NULL);

    if (pappdata)
    {
        HeapFree(GetProcessHeap(), 0, pappdata);
    }

    return hr;
}

INTERNAL_(int)
IsCharType(LCID lcid, OLECHAR ch, DWORD dwType)
{
    WORD    wOut[2];
    OLECHAR str[2];
    BOOL    bRet;

    // Due to GETSTRINGTYPE in 874 NLS cannot return us the correct one.
    if (IsThai(lcid)) {

        str[0] = ch;
        str[1] = 0;

        if (ch >= xchAbvKorKai) {
            if (dwType == C1_SPACE)
                return FALSE;
            if (((ch >= xchKorKai) && (ch <= xchFongMun)) ||
                ((ch == xchAnsiPeriod) && (dwType == C1_ALPHA)) )
                return (int)(dwType);
            else if ((dwType == C1_DIGIT) &&
                     (ch >= xchThaiZero && ch <= xchThaiNine))
                return (int)(dwType);
            else if ((dwType == (C1_DIGIT | C1_ALPHA)) &&
                    ((ch >= xchKorKai) && (ch <= xchFongMun) ||
                    (ch == xchAnsiPeriod)))
                return (int)(dwType);
            else
                return FALSE;
        }
////////////////////////////////////////////////////////////////////////////////
//                                                                            //
//  This code seems to say that if the character is a period ('.'), it can be //
//   any character type at all.  The dwType is a mask of character types, and //
//   is never 0.  So, if (ch == xchAnsiPeriod) the function returns true.     //
//                                                                            //
        else if (ch == xchAnsiPeriod)                                         //
        {
            return (int)(dwType);
            
        // THis is derived from the code above
        //    if (dwType == C1_ALPHA)
        //        return (int)(dwType);
        //    else if (dwType == (C1_DIGIT | C1_ALPHA))
        //        return (int)(dwType);
        //    else
        //        return FALSE;

        // If the code intends to treat a period as an alpha char:
        //    if (dwType & C1_ALPHA)
        //        return (int)dwType);
        //    else
        //        return FALSE;
        }
//                                                                            //
////////////////////////////////////////////////////////////////////////////////                
        else
        {
            bRet = GetStringTypeExW(lcid, CT_CTYPE1, str, 1, wOut);

            _ASSERTE(bRet);

            return (int)(wOut[0]&dwType);
        }
    }
    else if (IsChina(lcid) || IsTaiwan(lcid)) {
      int n;
      for (n = 0; n < g_NumChinaNum; n ++)
    {
      for (int m = 0; g_szChinaNum[n][m] != OASTR('\0'); m ++)
        if (ch == g_szChinaNum[n][m])
          return C1_DIGIT & dwType;
    }
      str[0] = ch;
      str[1] = 0;

      bRet = GetStringTypeExW(lcid, CT_CTYPE1, str, 1, wOut);
      _ASSERTE(bRet);
      return (int)(wOut[0]&dwType);
    }
    else
    { //IsThai

      str[0] = ch;
      str[1] = 0;
      bRet = GetStringTypeExW(lcid, CT_CTYPE1, str, 1, wOut);

      _ASSERTE(bRet);

    }

    return (int)(wOut[0]&dwType);
}

STDAPI
VarBstrFromI2(SHORT iVal, LCID lcid, ULONG dwFlags, BSTR * pbstrOut)
{
    OLECHAR buffer[40];
    OLECHAR FAR* pchBuffer;
    
    // integers have no decimals, and thus they are locale-unaware.
    // lcid remains unused
        
    pchBuffer = buffer;
    
    disp_itoa((int)iVal, pchBuffer);
    
    return ErrSysAllocString(buffer, pbstrOut);
}

STDAPI
VarBstrFromUI1(BYTE bVal, LCID lcid, ULONG dwFlags, BSTR * pbstrOut)
{
    return VarBstrFromI2((SHORT)(unsigned short)bVal, lcid, dwFlags, pbstrOut);
}

STDAPI
VarBstrFromI4(LONG lIn, LCID lcid, ULONG dwFlags, BSTR * pbstrOut)
{
    OLECHAR buffer[40];
    OLECHAR FAR* pchBuffer = buffer;

    // longs have no decimals, and thus they are locale-unaware.
    // lcid remains unused

    disp_ltoa(lIn, pchBuffer);
    
    return ErrSysAllocString(buffer, pbstrOut);
}


STDAPI
VarBstrFromI8(LONG64 i64In, LCID lcid, ULONG dwFlags, BSTR FAR* pbstrOut)
{
    OLECHAR buffer[80];
    OLECHAR FAR* pchBuffer = buffer;

    // longs have no decimals, and thus they are locale-unaware.
    // lcid remains unused

    disp_i64toa(i64In, pchBuffer);
    
    return ErrSysAllocString(buffer, pbstrOut);
}


/***
* EditStrFromReal - edit real string to BASIC format
*
* Purpose:
*
*    Convert the given string in place to the BASIC format.RR
*
*   Fractions less than .1 are output in C in exponent format;
*       e.g., BASIC: .00777 --> C: 7.77e-003
*   Exponents in C use 'e' while BASIC uses 'E'.
*   Exponents in C use three-digit exponents always, BASIC uses
*   two if three are not needed.
*       e.g., BASIC: 3.456E+4 --> C: 3.456e+004
*   Trailing decimals used in C, not in BASIC
*       e.g., BASIC: 1234 --> C: 1234.
*   Fractions less than 1.0 are output with a leading zero depending
*       the locale setting LOCALE_ILZERO
*       e.g.,  .1234 or 0.1234
*   Maximum length integers are output in C as exponential, but
*   as integers in BASIC
*       e.g., BASIC: 1234567 --> 1.234567e+006
*
* Entry:
*   pchBuffer - pointer to string to be processed
*   cDigits - number of signficant digits, or maximum decimal
*
* Exit:
*   pchBuffer - converted string of the real value      
*
* Exceptions:
***********************************************************************/

void
EditStrFromReal(OLECHAR FAR* pchBuffer, int cDigits, NUMINFO * pnuminfo)
{
    OLECHAR FAR* pchTemp;
    OLECHAR FAR* pchEnd;
    int length, lenFrac, valExp;

    // first, replace the '.' returned by the C-rutime gcvt function with
    // the locale-specific decimal.

    pchTemp = pchBuffer;
    while(*pchTemp) {
      if (*pchTemp == OASTR('.')) {
        *pchTemp = pnuminfo->chDecimal;
        break;
      }
      pchTemp++;
    }


    // skip over a leading minus sign

    if(*pchBuffer == OASTR('-'))
      pchBuffer++;

    // get length and point to 'e' if exponental value
    length = (int)wcslen(pchBuffer);
    pchTemp = pchBuffer + length - 5;

    // test if exponential value
    if (length > 6 && *pchTemp == OASTR('e')) {

      // test if negative exponent
      if (*(pchTemp + 1) == OASTR('-')) {

    // point to first exponent digit
    pchTemp += 2;

    // calcuate length of fraction
    // "d.mm--mme-nnn" - two before, five after

    lenFrac = length - 7;

    // evaluate exponent value

    valExp = *pchTemp++ - OASTR('0');
    valExp = valExp * 10 + *pchTemp++ - OASTR('0');
    valExp = valExp * 10 + *pchTemp - OASTR('0');

    // determine if number can be a fraction...
    // length is:
    // valExp  - 1 leading zeroes
    // lenFrac + 1 digits (frac plus first digit)
    //
    if (valExp + lenFrac <= cDigits) {
      // point past new end of fraction and
      // to end of fraction in exponent

      pchEnd = pchBuffer + valExp + lenFrac + 1;
      pchTemp = pchBuffer + lenFrac + 1;

      // write null for new fraction

      *pchEnd-- = OASTR('\0');

      // copy exponent fraction to new fraction
      while(lenFrac--)
        *pchEnd-- = *pchTemp--;

      // copy leading digit
      *pchEnd-- = *pchBuffer;

      // set the leading zeroes, if any
      while (pchEnd > pchBuffer)
        *pchEnd-- = OASTR('0');

      // set the decimal point of new fraction
      *pchEnd-- = pnuminfo->chDecimal;
  
          // add a leading zero to a decimal point depending on Locale setting
          // this is OK so long as the input buffer is really greater than 
          // cDigit, which is the case on all call here (buf[40])
          if (pnuminfo->fLeadingZero) {
            memmove(pchBuffer+1, pchBuffer, BYTELEN(pchBuffer));
            *pchBuffer = OASTR('0');
          }
          
    }
    else    // if no conversion, point back to 'e' in value
      pchTemp = pchBuffer + length - 5;
      }
      // test if positive exponent
      else if (*(pchTemp + 1) == OASTR('+')) {

        // point to first exponent digit

        pchTemp += 2;

        // calcuate length of fraction
        // "d.mm--mme-nnn" - two before, five after

        lenFrac = length - 7;

        // evaluate exponent value

        valExp = *pchTemp++ - OASTR('0');
        valExp = valExp * 10 + *pchTemp++ - OASTR('0');
        valExp = valExp * 10 + *pchTemp - OASTR('0');

        // the only conversion done is when the exponent
        // is one less than the number of digits to make
        // an integer of length cDigits

        if (valExp == cDigits - 1) {
        
          // point to first fraction digit

          pchTemp = pchBuffer + 2;

          // copy fraction digits one location to the left

          while (*pchTemp >= OASTR('0') && *pchTemp <= OASTR('9')) {
            *(pchTemp - 1) = *pchTemp;
            pchTemp++;
            valExp--;
          }

          // zero-fill any remaining digits and terminate
          // pchTemp is left on null, so exponent is not
          // processed

          pchTemp--;

          while (valExp--)
        *pchTemp++ = OASTR('0');
          *pchTemp = OASTR('\0');

        }
        else
          // if no conversion, point back to 'e' in value
          pchTemp = pchBuffer + length - 5;
      }

      // if pchTemp points to an 'e', process the exponential

      if (*pchTemp == OASTR('e')) {

        // convert 'e' to upper case
        *pchTemp = OASTR('E');

        // if first exponent digit is a zero, remove it
        if (*(pchTemp + 2) == OASTR('0'))
          memmove(pchTemp+2, pchTemp+3, BYTELEN(pchTemp));

        // if exponent is preceded by a decimal point, remove it
        if (*(pchTemp - 1) == pnuminfo->chDecimal)
          memmove(pchTemp -1, pchTemp, BYTELEN(pchTemp));    
      }
      
    }

    // if not an exponent, do some processing

    else {

      // remove any trailing decimal point

      pchTemp = pchBuffer + length - 1;

      if(*pchTemp == pnuminfo->chDecimal)
        *pchTemp = OASTR('\0');

      // remove a leading zero to a decimal point depending on Locale setting

      if (!pnuminfo->fLeadingZero &&
          (*pchBuffer == OASTR('0'))  && 
          (*(pchBuffer + 1) == pnuminfo->chDecimal))
        memmove(pchBuffer, pchBuffer + 1, BYTELEN(pchBuffer));
    }
}


STDAPI 
VarBstrFromR4(FLOAT fltIn, LCID lcid, ULONG dwFlags, BSTR * pbstrOut)
{
      
    OLECHAR buffer[40];
    NUMINFO * pnuminfo;

    _ASSERTE(dwFlags == 0);
    
    disp_gcvt((double)fltIn, 7, buffer, 40);

    IfFailRet(GetNumInfo(lcid, dwFlags, &pnuminfo));

    // process the string to the BASIC format
    EditStrFromReal(buffer, 7, pnuminfo);

    return ErrSysAllocString(buffer, pbstrOut);
}

STDAPI
VarBstrFromR8(DOUBLE dblIn, LCID lcid, ULONG dwFlags, BSTR * pbstrOut)
{
      
    OLECHAR buffer[40];
    NUMINFO * pnuminfo;

    _ASSERTE(dwFlags == 0);
   
    disp_gcvt(dblIn, 15, buffer, 40);

    IfFailRet(GetNumInfo(lcid, dwFlags, &pnuminfo));

    // process the string to the BASIC format
    EditStrFromReal(buffer, 15, pnuminfo);

    return ErrSysAllocString(buffer, pbstrOut);

}


STDAPI 
VarBstrFromI1(CHAR cIn, LCID lcid, ULONG dwFlags, BSTR *pbstrOut)
{
    return VarBstrFromI2(cIn, lcid, dwFlags, pbstrOut);
}

STDAPI 
VarBstrFromUI2(USHORT uiIn, LCID lcid, ULONG dwFlags, BSTR *pbstrOut)
{
    return VarBstrFromUI4(uiIn, lcid, dwFlags, pbstrOut);
}

STDAPI 
VarBstrFromUI4(ULONG ulIn, LCID lcid, ULONG dwFlags, BSTR *pbstrOut)
{
    OLECHAR         buffer[40];

    _ASSERTE(dwFlags == 0);

    disp_ultoa(ulIn, buffer);
    
    return ErrSysAllocString(buffer, pbstrOut);
}

STDAPI 
VarBstrFromUI8(ULONG64 ui64In, LCID lcid, ULONG dwFlags, BSTR FAR* pbstrOut)
{
    OLECHAR         buffer[80];

    _ASSERTE(dwFlags == 0);

    disp_ui64toa(ui64In, buffer);
    
    return ErrSysAllocString(buffer, pbstrOut);
}

long
ConvI4FromR8(double dblIn)
{
    long   lRes;
    double dblDif;

    lRes = (long)dblIn;
    dblDif = dblIn - (double)lRes;
                    
    if ( fabs(dblDif) != 0.5 || (lRes & 1) )
      lRes += (long)(2*dblDif);

    return lRes;
}

STDAPI 
VarI1FromR8(DOUBLE dblIn, CHAR FAR* pcOut)
{
    if  (dblIn >= (SCHAR_MIN - 0.5) && dblIn < (SCHAR_MAX + 0.5))
    {
        *pcOut = (char)ConvI4FromR8(dblIn);
        return NOERROR;
    }

    return RESULT(DISP_E_OVERFLOW);
}

STDAPI 
VarUI1FromR8(DOUBLE dblIn, BYTE FAR* pbOut)
{
    if(dblIn >= -0.5 && dblIn < 255.5){
      *pbOut = (unsigned char)ConvI4FromR8(dblIn);
      return NOERROR;
    }
    return RESULT(DISP_E_OVERFLOW);
}


STDAPI 
VarI2FromR8(DOUBLE dblIn, SHORT * psOut)
{
    if(dblIn >= -32768.5 && dblIn < 32767.5){
      *psOut = (short)ConvI4FromR8(dblIn);
      return NOERROR;
    }
    return RESULT(DISP_E_OVERFLOW);
}

STDAPI 
VarUI2FromR8(DOUBLE dblIn, USHORT *puiOut)
{
    if  (dblIn >= -0.5 && dblIn < (USHRT_MAX + 0.5))
    {
        *puiOut = (unsigned short)ConvI4FromR8(dblIn);
        return NOERROR;
    }

    return RESULT(DISP_E_OVERFLOW);
}

STDAPI
VarI4FromR8(DOUBLE dblIn, LONG FAR* plOut)
{
    if(dblIn >= -2147483648.5 && dblIn < 2147483647.5){
      *plOut = ConvI4FromR8(dblIn);
      return NOERROR;
    }
    return RESULT(DISP_E_OVERFLOW);
}

unsigned long
ConvUI4FromR8(DOUBLE dblVal)
{
    unsigned long ulResult;
    double        dblInt, dblFrac;

    // split double value into integer and fractional parts

    dblFrac = modf(dblVal, &dblInt);


    // convert the integer part to an unsigned long value
    //WARNING: the caller must ensure that the R8 to UI4 conversion
    //         will not overflow the UI4.  If it does, the Win16
    //         compiler doesn't call FWAIT after the _aFftol call
    //         so the exception won't be raised until the next
    //         FWAIT, which may not be for a very long time.
    //         Ole2Disp isn't supposed to raise exceptions, so
    //         it is a bug if a subsequent FWAIT causes an exception.
    ulResult = (unsigned long)dblInt;

    // round to the nearer integer, if at midpoint,
    // towards the integer with the LSB zero

    if (dblFrac > 0.5 || (dblFrac == 0.5 && (ulResult & 1)))
    {
        ulResult++;
    }
    else
    {
        if  (dblFrac < -0.5 || (dblFrac == -0.5 && (ulResult & 1)))
        {
            ulResult--;
        }
    }

    return ulResult;
}

STDAPI 
VarUI4FromR8(DOUBLE dblIn, ULONG *pulOut)
{
    if  (dblIn >= -0.5 && dblIn < (ULONG_MAX + 0.5))
    {
        *pulOut = ConvUI4FromR8(dblIn);
        return NOERROR;
    }

    return RESULT(DISP_E_OVERFLOW);
}

STDAPI
VarR8FromStr(OLECHAR FAR* strIn, LCID lcid, ULONG dwFlags, DOUBLE FAR* pdblOut)
{
    BYTE      rgbDig[R8_MAXDIG];
    NUMPARSE  numprs;
    VARIANT   var;

    numprs.cDig = R8_MAXDIG;   // initialize to size of array
    numprs.dwInFlags = NUMPRS_STD;

    IfFailRet(VarParseNumFromStr(strIn, lcid, dwFlags, &numprs, rgbDig));
    IfFailRet(VarNumFromParseNum(&numprs, rgbDig, VTBIT_R8, &var));
    _ASSERTE(V_VT(&var) == VT_R8);

    *pdblOut = V_R8(&var);
    return NOERROR;
}

