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
// File: constval.h
//
// ===========================================================================

#ifndef _CONSTVAL_H_
#define _CONSTVAL_H_

#include <pshpack1.h>
struct DecimalConstantBuffer {
    WORD format;
    BYTE scale;
    BYTE sign;
    DWORD hi;
    DWORD mid;
    DWORD low;
    WORD cNamedArgs;
};
#include <poppack.h>

// portable access to nameless unions in DECIMALs
#ifndef DECIMAL_LO32
#define DECIMAL_LO32(dec)       ((dec).Lo32)
#endif
#ifndef DECIMAL_MID32
#define DECIMAL_MID32(dec)      ((dec).Mid32)
#endif
#ifndef DECIMAL_HI32
#define DECIMAL_HI32(dec)       ((dec).Hi32)
#endif
#ifndef DECIMAL_SCALE
#define DECIMAL_SCALE(dec)      ((dec).scale)
#endif
#ifndef DECIMAL_SIGN
#define DECIMAL_SIGN(dec)       ((dec).sign)
#endif
#ifndef DECIMAL_SIGNSCALE
#define DECIMAL_SIGNSCALE(dec)  ((dec).signscale)
#endif


/*
 * A string constant. These are NOT nul terminated, and can contain
 * internal nul characters.  
 */
struct STRCONST {
    int         length;
    WCHAR *     text;
};

/*
 * A constant value. We want this to use only 4 bytes, so larger
 * values are represented by pointers.
 */
struct CONSTVAL {
    union {
        INT_PTR         init;           // This is first so it will be used for initialization.
                                        // It is an INT_PTR to take advantage of the larger sizes of pointers.
        int             iVal;           // 4 byte or less integral values. (must be 0 or 1 for bools).
        unsigned int    uiVal;          // unsigned 4 byte int.

        // long values -- allocated and pointed to.
        double *        doubleVal;      // Use for "float" constants too.
        __int64 *       longVal;
        unsigned __int64 * ulongVal;    // unsigned 8 byte int.
        STRCONST *      strVal;
        DECIMAL *       decVal;
    };
};

struct ConstValInit : CONSTVAL {
    ConstValInit() { init = 0; }
    ConstValInit(int val) { iVal = val; }
    ConstValInit(unsigned val) { uiVal = val; }
    ConstValInit(STRCONST * val) { strVal = val; }
};

union CONSTVALNS  // the same union w/o a string field...
{
    int             iVal;           // 4 byte or less integral values. (must be 0 or 1 for bools).
    unsigned int    uiVal;          // unsigned 4 byte int.

    // long values -- allocated and pointed to.
    double *        doubleVal;      // Use for "float" constants too.
    __int64 *       longVal;
    unsigned __int64 * ulongVal;    // unsigned 8 byte int.
    DECIMAL *       decVal;           
};

#endif  // _CONSTVAL_H_
