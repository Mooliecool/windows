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
// File: symkinds.h
//
// Contains a list of the various concrete symbol kinds. Abstract symbol
// kinds are NOT defined here.
// ===========================================================================

#if !defined(SYMBOLDEF)
#error Must define SYMBOLDEF macro before including symkinds.h
#endif
#if !defined(SYMBOLDEF_EXTRA)
#define SYMBOLDEF_EXTRA(a,b,c) SYMBOLDEF(a,b,c)
#endif

// Note: If we ever get more than 64 of these we'll need to expand the mask type.
//        Name           Global   Local

// Namespace
SYMBOLDEF(NSSYM,            1,      0)
SYMBOLDEF(NSDECLSYM,        1,      0)
SYMBOLDEF(NSAIDSYM,         1,      0)

// Aggregate
SYMBOLDEF(AGGSYM,           1,      0)
SYMBOLDEF(AGGDECLSYM,       1,      0)
SYMBOLDEF(AGGTYPESYM,       1,      0)
SYMBOLDEF(FWDAGGSYM,        1,      0)

// "Members" of aggs.
SYMBOLDEF(TYVARSYM,         1,      0)
SYMBOLDEF(MEMBVARSYM,       1,      0)
SYMBOLDEF(LOCVARSYM,        0,      1)
SYMBOLDEF(METHSYM,          1,      0)
SYMBOLDEF(FAKEMETHSYM,      1,      0) // this has to be immediately after METHSYM
SYMBOLDEF(PROPSYM,          1,      0)
SYMBOLDEF(EVENTSYM,         1,      0)

// Primitive types.
SYMBOLDEF(VOIDSYM,          1,      0)
SYMBOLDEF(NULLSYM,          1,      0)
SYMBOLDEF(UNITSYM,          1,      1)
SYMBOLDEF(ANONMETHSYM,      1,      0)
SYMBOLDEF(METHGRPSYM,       1,      0)
SYMBOLDEF(ERRORSYM,         1,      0)

// Derived types - Parent is the base type.
SYMBOLDEF(ARRAYSYM,         1,      0)
SYMBOLDEF(PTRSYM,           1,      0)
SYMBOLDEF(PINNEDSYM,        1,      0)
SYMBOLDEF(PARAMMODSYM,      1,      0)
SYMBOLDEF(MODOPTSYM,        1,      0)
SYMBOLDEF(MODOPTTYPESYM,    1,      0)
SYMBOLDEF(NUBSYM,           1,      0) // Nullable type as a "derived" type - the parent is the base type.

// Files
SYMBOLDEF(INFILESYM,        1,      0)
SYMBOLDEF(MODULESYM,        1,      0)
SYMBOLDEF(RESFILESYM,       1,      0)
SYMBOLDEF(OUTFILESYM,       1,      0)
SYMBOLDEF(XMLFILESYM,       1,      0)
SYMBOLDEF(SYNTHINFILESYM,   1,      0)

// Aliases
SYMBOLDEF(ALIASSYM,         1,      0)
SYMBOLDEF(EXTERNALIASSYM,   1,      0)

// Other
SYMBOLDEF(SCOPESYM,         1,      1)
SYMBOLDEF(CACHESYM,         0,      1)
SYMBOLDEF(LABELSYM,         0,      1)
SYMBOLDEF(MISCSYM,          1,      0)
SYMBOLDEF(GLOBALATTRSYM,    1,      0)
SYMBOLDEF(ANONSCOPESYM,     0,      1)

SYMBOLDEF_EXTRA(UNRESAGGSYM, 1, 0)
SYMBOLDEF_EXTRA(IFACEIMPLMETHSYM, 1, 0)
SYMBOLDEF_EXTRA(INDEXERSYM, 1, 0)

#undef SYMBOLDEF
#undef SYMBOLDEF_EXTRA
