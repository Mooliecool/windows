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
// File: memlook.h
//
// Handles member lookup - lookup within a type and its base types.
// ===========================================================================

#ifndef __memlook_h__
#define __memlook_h__

namespace MemLookFlags { enum _Enum {
    None = 0,

    Ctor = EXF_CTOR,
    NewObj = EXF_NEWOBJCALL,
    Operator = EXF_OPERATOR,
    Indexer = EXF_INDEXER,
    UserCallable = EXF_USERCALLABLE,
    BaseCall = EXF_BASECALL,

    // All EXF flags are < 0x01000000
    TypeVarsAllowed = 0x40000000,

    All = Ctor | NewObj | Operator | Indexer | UserCallable | BaseCall | TypeVarsAllowed,
}; };
DECLARE_FLAGS_TYPE(MemLookFlags);

/******************************************************************************
    MemberLookup class handles looking for a member within a type and its
    base types. This only handles AGGTYPESYMs and TYVARSYMs.

    Lookup must be called before any other methods.
******************************************************************************/
class MemberLookup {
private:
    // The inputs to Lookup.
    COMPILER * m_compiler;
    TYPESYM * m_typeSrc;
    EXPR * m_obj;
    TYPESYM * m_typeQual;
    PARENTSYM * m_symWhere;
    NAME * m_name;
    int m_arity;
    int m_flags;

    // For maintaining the type array. We throw the first 8 or so here.
    AGGTYPESYM * m_rgtypeStart[8];
    int m_ctypeMax;

    // Results of the lookup.
    AGGTYPESYM ** m_prgtype;    // All the types containing relevant symbols.
    int m_ctype;                // Number of types.
    int m_csym;                 // Number of syms found.
    SymWithType m_swtFirst;     // The first symbol found.
    bool m_fMulti;              // Whether symFirst is of a kind for which we collect multiples (methods and indexers).

    // These are for error reporting.
    SymWithType m_swtAmbig;     // An ambiguous symbol.
    SymWithType m_swtInaccess;  // An inaccessible symbol.
    SymWithType m_swtBad;       // If we're looking for a constructor or indexer, this matched on name, but isn't the right thing.
    SymWithType m_swtBogus;     // A bogus member - such as an indexed property.
    SymWithType m_swtBadArity;
    SymWithType m_swtAmbigWarn; // An ambiguous symbol, but only warn.

    void RecordType(AGGTYPESYM * type, SYM * sym);
    bool SearchSingleType(AGGTYPESYM * type, bool * pfHideByName);
    bool LookupInClass(AGGTYPESYM * type, AGGTYPESYM ** ptypeEnd);
    bool LookupInInterfaces(AGGTYPESYM * typeStart, TypeArray * types);

    COMPILER * compiler() { return m_compiler; }
    void ReportBogus(BASENODE * tree, SymWithType swt);

#ifdef DEBUG
    bool m_fValid;
public:
    MemberLookup() { m_fValid = false; }
#endif

public:
    // Returns false iff there was no symbol found or an ambiguity.
    bool Lookup(COMPILER * compiler, TYPESYM * typeSrc, EXPR * obj, PARENTSYM * symWhere, NAME * name, int arity, MemLookFlagsEnum flags);

    /**************************************************************************
        Results of the lookup.
    **************************************************************************/

    // Whether there were errors.
    bool FError() { ASSERT(m_fValid); return !m_swtFirst || m_swtAmbig; }
    // Whether we can give an error better than "member not found".
    bool HasIntelligentErrorInfo() { ASSERT(m_fValid); return m_swtAmbig || m_swtInaccess || m_swtBad || m_swtBogus || m_swtBadArity; }

    // The number of symbols found.
    int SymCount() { ASSERT(m_fValid); return m_csym; }
    // The first symbol found.
    SYM * SymFirst() { ASSERT(m_fValid); return m_swtFirst.Sym(); }
    SymWithType & SwtFirst() { ASSERT(m_fValid); return m_swtFirst; }
    AGGTYPESYM * SymFirstAsAts(TypeArray * typeArgs);

    // Whether the kind of symbol found is method, aggregate, or indexer.
    bool FMultiKind() { ASSERT(m_fValid); return m_fMulti; }

    // Number of types in which symbols were found.
    int TypeCount() { ASSERT(m_fValid); return m_ctype; }
    // Retrieve the i'th type.
    AGGTYPESYM * Type(int i) { ASSERT(m_fValid); ASSERT(0 <= i && i < m_ctype); return m_prgtype[i]; }
    // Put all the types in a type array.
    TypeArray * GetAllTypes();

    /**************************************************************************
        Operations after the lookup.
    **************************************************************************/
    void ReportErrors(BASENODE * tree);
    void ReportWarnings(BASENODE * tree);
    void FillGroup(EXPRMEMGRP * grp);
};

#endif // __memlook_h__
