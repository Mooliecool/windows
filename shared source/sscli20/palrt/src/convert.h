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
// File: convert.h
// 
// ===========================================================================

/***
*Purpose:
*  Common header (shared by convert.cpp and decimal.cpp) for numeric
*  conversions and other math stuff.
*
*Revision History:
*
*                                              
*
*Implementation Notes:
*
*****************************************************************************/

#ifndef _CONVERT_H_ /* { */
#define _CONVERT_H_


#include "oautil.h"
#include "oaidl.h"

typedef DWORD ITLS;             //index to a thread-local-storage slot.
#define ITLS_EMPTY TLS_OUT_OF_INDEXES

extern ITLS g_itlsAppData;

#define R8_MAXDIG 20

//***********************************************************************
//
// Structures
//

typedef union{
    struct {
#if BIGENDIAN
      ULONG sign:1;
      ULONG exp:11;
      ULONG mantHi:20;
      ULONG mantLo;
#else // BIGENDIAN
      ULONG mantLo;
      ULONG mantHi:20;
      ULONG exp:11;
      ULONG sign:1;
#endif
    } u;
    double dbl;
} DBLSTRUCT;

// Intializer for a DBLSTRUCT
#if BIGENDIAN
#define DEFDS(Lo, Hi, exp, sign) { {sign, exp, Hi, Lo } }
#else
#define DEFDS(Lo, Hi, exp, sign) { {Lo, Hi, exp, sign} }
#endif


typedef struct {
#if BIGENDIAN
    ULONG sign:1;
    ULONG exp:8;
    ULONG mant:23;
#else
    ULONG mant:23;
    ULONG exp:8;
    ULONG sign:1;
#endif
} SNGSTRUCT;



typedef union {
    DWORDLONG int64;
    struct {
#ifdef BIGENDIAN
        ULONG Hi;
        ULONG Lo;
#else
        ULONG Lo;
        ULONG Hi;
#endif
    } u;
} SPLIT64;



//***********************************************************************
//
// Constants
//

#define DBLBIAS 1022

#define SNGBIAS 126

#define DEC_MAXDIG 29
#define DECMAX 28

#define MAXINTPWR10 22

static const ULONG ulTenToTenDiv4 = 2500000000U;
static const ULONG ulTenToNine    = 1000000000U;


// NumInfo data structures
#define CCHCURMAX 10

extern CRITICAL_SECTION NumInfo_csCacheLock;

typedef struct tagNUMINFO {
    LCID    lcid;
    DWORD   dwFlags;            // flags used to build this numinfo
    int     cchCurrency;        // length of currency string
    BOOL    fFastCur;           // fast currency check OK
    OLECHAR rgbCurrency[CCHCURMAX];// currency string, lowercased,
                                   // NOT null terminated
    OLECHAR chDecimal;          // decimal separator
    OLECHAR chThousand1;        // primary thousand separator
    OLECHAR chThousand2;        // alternate thousand separator
    OLECHAR fLeadingZero;       // 1 if leading zeros, 0 otherwise
    OLECHAR chCurrencyDecimal;  // currency decimal separator
    OLECHAR chCurrencyThousand; // currency thousand separator
} NUMINFO;


//***********************************************************************
//
// Functions
//

extern "C" 
HRESULT GetNumInfo(LCID lcid, unsigned long dwFlags, NUMINFO ** ppnuminfo);

// The following are supplied by $(TARG)\oleconva.$(A)

extern "C" {
DWORDLONG UInt64x64To128(SPLIT64 sdlOp1, SPLIT64 sdlOp2, DWORDLONG *pdlHi);
}


//***********************************************************************
//
// Inlines for Decimal
//


#ifndef UInt32x32To64
#define UInt32x32To64(a, b) ((DWORDLONG)((DWORD)(a)) * (DWORDLONG)((DWORD)(b)))
#endif

#define Div64by32(num, den) ((ULONG)((DWORDLONG)(num) / (ULONG)(den)))
#define Mod64by32(num, den) ((ULONG)((DWORDLONG)(num) % (ULONG)(den)))

ULONG Div96By32(ULONG *rgulNum, ULONG ulDen);

inline DWORDLONG DivMod32by32(ULONG num, ULONG den)
{
    SPLIT64  sdl;

    sdl.u.Lo = num / den;
    sdl.u.Hi = num % den;
    return sdl.int64;
}

inline DWORDLONG DivMod64by32(DWORDLONG num, ULONG den)
{
    SPLIT64  sdl;

    sdl.u.Lo = Div64by32(num, den);
    sdl.u.Hi = Mod64by32(num, den);
    return sdl.int64;
}


//***********************************************************************
//
// Data
//
extern const unsigned __int64 ulPower10[];

// Access dblPower10 directly when ix is known to be <= 80.  Otherwise use fnDblPower10
//  which uses Pow() when ix exceedes the table size.
extern double fnDblPower10(int ix);
extern const double dblPower10[];
extern const DBLSTRUCT dsR4Max;
extern const SPLIT64 sdlTenToEighteen;

#if defined(__cplusplus)


// Per-app data structure
struct APP_DATA
{

    const class CNumInfo *m_pNumInfo;
    IErrorInfo *          m_perrinfo;

    void Init()
    {
      memset (this, 0, sizeof(APP_DATA));
    }

public:

};


STDAPI InitAppData();
VOID ReleaseAppData();

// Inlined accessor functions.
/***
*APP_DATA *Pappdata()
*
*Purpose:
*   Returns per-app struct shared by typelib and obrun.
*
*Inputs:
*
*Outputs:
*   APP_DATA *
*
******************************************************************************/

inline APP_DATA *Pappdata()
{
    
    if (g_itlsAppData == ITLS_EMPTY)
    {
        return NULL;
    }

    return((APP_DATA *)TlsGetValue(g_itlsAppData));
}

/***
* HRESULT GetAppData()
*
*Purpose:
*   Returns the appdata, creating it if it doesn't exist.
*
*Inputs:
*
*Outputs:
*   AppData.
*   returns HRESULT.
*
******************************************************************************/

inline HRESULT GetAppData(APP_DATA **ppappdata)
{
    if ((*ppappdata = Pappdata()) == NULL) {
      HRESULT hresult;

      if (FAILED(hresult = InitAppData())) {
        return hresult;
      }

      *ppappdata = Pappdata();

      _ASSERTE(*ppappdata != NULL);
    }

    return NOERROR;
}

#endif  // __cplusplus

#endif /* } _CONVERT_H_ */
