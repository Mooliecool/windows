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
// File: posdata.h
//
// ===========================================================================

#ifndef _POSDATA_H_
#define _POSDATA_H_

#define MAX_POS_LINE_LEN    0x00fffffe  // 24 bits of size data per line, minus one to distinquish from POSDATA(-1)
#define MAX_POS_LINE        0x0ffffffe  // 28   "    "    line data,      minus one  "      "        "      "

#define LINEMASK            0x0fffffff
#define CHARMASK            0x00ffffff

////////////////////////////////////////////////////////////////////////////////
// POSDATA
//
// This class holds line and character index data in a 64-bit structure, using
// only 52 bits (leaving a byte-and-a-half for user-defined storage).

struct POSDATA
{
    // The alignment optimizes accesses to iChar and iLine such that no
    // shifting is necessary.
    unsigned long   iChar:24;       // 16,777,214 characters per line max
    unsigned long   iUserByte:8;    // Unused "user" data (used for the token value by the token stream, etc)
    unsigned long   iLine:28;       // 268,435,454 lines per file max
    unsigned long   iUserBits:4;    // Unused "user" bits

    POSDATA ();
    POSDATA (long i, long c);

    bool Adjust (const POSDATA &posOld, const POSDATA &posNew);
    long Compare (const POSDATA &p) const;

    bool    operator < (const POSDATA &p) const;
    bool    operator > (const POSDATA &p) const;
    bool    operator <= (const POSDATA &p) const;
    bool    operator >= (const POSDATA &p) const;
    bool    operator == (const POSDATA &p) const;
    bool    operator != (const POSDATA &p) const;

    void    SetUninitialized ();
    bool    IsUninitialized () const;
    bool    IsZero () const;

    friend POSDATA operator - (POSDATA p, unsigned long i);
    friend POSDATA operator + (POSDATA p, unsigned long i);
    friend long operator - (POSDATA p1, POSDATA p2);
};

inline POSDATA operator - (POSDATA p, unsigned long i)
{
    ASSERT(!p.IsUninitialized());
    POSDATA pr(p);
    ASSERT (pr.iChar >= i); 
    pr.iChar -= i; 
    return pr;
}

inline POSDATA operator + (POSDATA p, unsigned long i)
{
    ASSERT(!p.IsUninitialized());
    POSDATA pr(p);
    pr.iChar += i;
    return pr;
}

inline long operator - (POSDATA p1, POSDATA p2)
{
    ASSERT(!(p1.IsUninitialized() || p2.IsUninitialized()));
    ASSERT (p1.iLine == p2.iLine);
    ASSERT (p1.iChar >= p2.iChar); 
    return (long)(p1.iChar - p2.iChar);
}

#endif  // _POSDATA_H_
