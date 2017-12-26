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
// File: name.h
//
// ===========================================================================

#ifndef _NAME_H_
#define _NAME_H_

#ifdef _MSC_VER
#pragma warning(disable:4200)
#endif

/*
 * A NAME structure stores a single name in the name table.
 * It is allocated out of a no-release allocator.
 */

class CStringBuilder;

#ifdef _MSC_VER
#pragma warning (push)
#pragma warning (disable : 4510) // default constructor could not be generated
#pragma warning (disable : 4610) // struct 'NAME' can never be instantiated - user defined constructor required
#endif

struct NAME {
    unsigned hash;         // hash value
    NAME * nextInBucket;   // next NAME in this hash bucket
    const wchar_t text[];  // text of the name

    // Make the life of the C# language service easier
};

#ifdef _MSC_VER
#pragma warning (pop)
#endif

typedef NAME * PNAME;       // pointer to name.

#endif  // _NAME_H_
