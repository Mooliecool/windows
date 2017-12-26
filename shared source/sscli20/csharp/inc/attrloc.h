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
// File: attrloc.h
//
// Defines attribute locations
// ===========================================================================

#if !defined(ATTRLOCDEF)
#error Must define ATTRLOCDEF
#endif

// NOTE: value must be 2 << (index-in-list + 1)

//         enum         value   name            CorAttributeTargets?
ATTRLOCDEF(AL_ASSEMBLY, 0x0001, L"assembly"     )
ATTRLOCDEF(AL_MODULE,   0x0002, L"module"       )
ATTRLOCDEF(AL_TYPE,     0x0004, L"type"         )
ATTRLOCDEF(AL_METHOD,   0x0008, L"method"       )
ATTRLOCDEF(AL_FIELD,    0x0010, L"field"        )
ATTRLOCDEF(AL_PROPERTY, 0x0020, L"property"     )
ATTRLOCDEF(AL_EVENT,    0x0040, L"event"        )
ATTRLOCDEF(AL_PARAM,    0x0080, L"param"        )
ATTRLOCDEF(AL_RETURN,   0x0100, L"return"       )
ATTRLOCDEF(AL_TYPEVAR,  0x0200, L"typevar"      )
ATTRLOCDEF(AL_UNKNOWN,  0x0400, L"$AL_UNKNOWN$" )

// NOTE:
// NOTE: Remember to update this count when changing the above array
// NOTE:
#define ATTRLOC_COUNT (11)

#undef ATTRLOCDEF
