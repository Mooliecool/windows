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
// File: error.h
//
// ===========================================================================

#ifndef __error_h__
#define __error_h__

// This encapsulates a variable set of arguments that can be processed by
// FormatMessage using the FORMAT_MESSAGE_ARGUMENT_ARRAY option. Each
// argument occupies sizeof(INT_PTR) bytes. Slots can contain either pointers
// or scalars. Some slots MAY NOT BE pointers!
struct VarArgList {
    PCWSTR * args;

    VarArgList(PCWSTR * args) { this->args = args; }
    PCWSTR * Args() { return args; }
};

/******************************************************************************
    New Error API.
    ErrArg - the argument value and kind of value. This has constructors that
        provide implicit conversions from typical types to ErrArg.
    ErrArgKind - the kind of argument value.
    ErrArgTyped - a pair consisting of a SYM and a containing AGGTYPESYM.
******************************************************************************/
namespace ErrArgKind { enum _Enum {
    Int,
    Ids,
    SymKind,
    AggKind,
    Sym,
    Name,
    Str,
    PredefName,
    LocNode,
    NameNode,
    TypeNode,
    Ptr,
    SymWithType,
    MethWithInst,

    Lim
}; };
DECLARE_ENUM_TYPE(ErrArgKind)

namespace ErrArgFlags { enum _Enum {
    None    = 0x0000,
    Ref     = 0x0001, // The arg's location should be included in the error message
    NoStr   = 0x0002, // The arg should NOT be included in the error message, just the location
    RefOnly = Ref | NoStr,
    Unique  = 0x0004, // The string should be distinct from other args marked with Unique
}; };
DECLARE_FLAGS_TYPE(ErrArgFlags)

struct ErrArg {
    ErrArgKindEnum eak : 16;
    ErrArgFlagsEnum eaf : 16;

    union {
        int n;
        int ids;
        SYMKIND sk;
        AggKindEnum ak;
        SYM * sym;
        NAME * name;
        PCWSTR psz;
        PREDEFNAME pdn;
        BASENODE * node;
        NAMENODE * nameNode;
        TYPEBASENODE * typeNode;
        const SymWithType * pswt;
        const MethPropWithInst * pmpwi;
        void * ptr;
    };

    ErrArg() { }
    ErrArg(int n) { this->eak = ErrArgKind::Int; this->eaf = ErrArgFlags::None; this->n = n; }
    ErrArg(SYMKIND sk) { ASSERT(sk != SK_NSAIDSYM); this->eaf = ErrArgFlags::None; this->eak = ErrArgKind::SymKind; this->sk = sk; } // NSAIDSYMs are treated differently based on the SYM not the SK
    ErrArg(NAME * name) { this->eak = ErrArgKind::Name; this->eaf = ErrArgFlags::None; this->name = name; }
    ErrArg(PCWSTR psz) { this->eak = ErrArgKind::Str; this->eaf = ErrArgFlags::None; this->psz = psz; }
    ErrArg(const CComBSTR & bstr) { this->eak = ErrArgKind::Str; this->eaf = ErrArgFlags::None; this->psz = bstr.m_str; }
    ErrArg(PREDEFNAME pdn) { this->eak = ErrArgKind::PredefName; this->eaf = ErrArgFlags::None; this->pdn = pdn; }

    ErrArg(SYM * sym) { this->eak = ErrArgKind::Sym; this->eaf = ErrArgFlags::None; this->sym = sym; }
    ErrArg(NAMENODE * nameNode) { this->eak = ErrArgKind::NameNode; this->eaf = ErrArgFlags::None; this->nameNode = nameNode; }
    ErrArg(TYPEBASENODE * typeNode) { this->eak = ErrArgKind::TypeNode; this->eaf = ErrArgFlags::None; this->typeNode = typeNode; }
    ErrArg(const SymWithType & swt) { this->eak = ErrArgKind::SymWithType; this->eaf = ErrArgFlags::None; this->pswt = &swt; }
    ErrArg(const MethPropWithInst & mpwi) { this->eak = ErrArgKind::MethWithInst; this->eaf = ErrArgFlags::None; this->pmpwi = &mpwi; }

    ErrArg(SYM * sym, ErrArgFlagsEnum eaf) { this->eak = ErrArgKind::Sym; this->eaf = eaf; this->sym = sym; }
    ErrArg(NAMENODE * nameNode, ErrArgFlagsEnum eaf) { this->eak = ErrArgKind::NameNode; this->eaf = eaf; this->nameNode = nameNode; }
    ErrArg(TYPEBASENODE * typeNode, ErrArgFlagsEnum eaf) { this->eak = ErrArgKind::TypeNode; this->eaf = eaf; this->typeNode = typeNode; }
    ErrArg(const SymWithType & swt, ErrArgFlagsEnum eaf) { this->eak = ErrArgKind::SymWithType; this->eaf = eaf; this->pswt = &swt; }
    ErrArg(const MethPropWithInst & mpwi, ErrArgFlagsEnum eaf) { this->eak = ErrArgKind::MethWithInst; this->eaf = eaf; this->pmpwi = &mpwi; }

    // Returns true if the pointer produced is really a string.
    bool ConvertBasic(PCWSTR * ppsz)
    {
        ASSERT(!this->eaf);
        switch (this->eak) {
        case ErrArgKind::Int:
            *ppsz = (PCWSTR)(INT_PTR)this->n;
            return false;
        case ErrArgKind::Str:
            *ppsz = this->psz;
            return true;
        case ErrArgKind::Ptr:
            *ppsz = (PCWSTR)(INT_PTR)this->ptr;
            return false;
        case ErrArgKind::Name:
            *ppsz = this->name->text;
            return true;
        default:
            VSFAIL("Unhandled ErrArg kind in ErrArg::ConvertBasic");
            *ppsz = L"";
            return true;
        }
    }
};

struct ErrArgRef : ErrArg {
    ErrArgRef() { }
    ErrArgRef(int n) : ErrArg(n) { }
    ErrArgRef(SYMKIND sk) : ErrArg(sk) { }
    ErrArgRef(NAME * name) : ErrArg(name) { }
    ErrArgRef(PCWSTR psz) : ErrArg(psz) { }
    ErrArgRef(PREDEFNAME pdn) : ErrArg(pdn) { }
    ErrArgRef(SYM * sym) : ErrArg(sym) { this->eaf = ErrArgFlags::Ref; }
    ErrArgRef(NAMENODE * nodeName) : ErrArg(nodeName) { this->eaf = ErrArgFlags::Ref; }
    ErrArgRef(TYPEBASENODE * nodeType) : ErrArg(nodeType) { this->eaf = ErrArgFlags::Ref; }
    ErrArgRef(const SymWithType & swt) : ErrArg(swt) { this->eaf = ErrArgFlags::Ref; }
    ErrArgRef(const MethPropWithInst & mpwi) : ErrArg(mpwi) { this->eaf = ErrArgFlags::Ref; }

    ErrArgRef(SYM * sym, ErrArgFlagsEnum eaf) : ErrArg(sym) { this->eaf = eaf | ErrArgFlags::Ref; }
    ErrArgRef(NAMENODE * nodeName, ErrArgFlagsEnum eaf) : ErrArg(nodeName) { this->eaf = eaf | ErrArgFlags::Ref; }
    ErrArgRef(TYPEBASENODE * nodeType, ErrArgFlagsEnum eaf) : ErrArg(nodeType) { this->eaf = eaf | ErrArgFlags::Ref; }
    ErrArgRef(const SymWithType & swt, ErrArgFlagsEnum eaf) : ErrArg(swt) { this->eaf = eaf | ErrArgFlags::Ref; }
    ErrArgRef(const MethPropWithInst & mpwi, ErrArgFlagsEnum eaf) : ErrArg(mpwi) { this->eaf = eaf | ErrArgFlags::Ref; }
};

struct ErrArgRefOnly : ErrArgRef {
    ErrArgRefOnly(SYM * sym) : ErrArgRef(sym) { this->eaf = ErrArgFlags::RefOnly; }
    ErrArgRefOnly(BASENODE * node) {
        this->eak = ErrArgKind::LocNode;
        this->eaf = ErrArgFlags::RefOnly;
        this->node = node;
    }
};

// This is used with COMPILER::ErrorRef to indicate no reference.
struct ErrArgNoRef : ErrArgRef {
    ErrArgNoRef(SYM * sym) { this->eak = ErrArgKind::Sym; this->eaf = ErrArgFlags::None; this->sym = sym; }
    ErrArgNoRef(const SymWithType & swt) { this->eak = ErrArgKind::SymWithType; this->eaf = ErrArgFlags::None; this->pswt = &swt; }
    ErrArgNoRef(const MethPropWithInst & mpwi) { this->eak = ErrArgKind::MethWithInst; this->eaf = ErrArgFlags::None; this->pmpwi = &mpwi; }
};

struct ErrArgIds : ErrArgRef {
    ErrArgIds(int ids) {
        this->eak = ErrArgKind::Ids;
        this->eaf = ErrArgFlags::None;
        this->ids = ids;
    }
};

struct ErrArgPtr : ErrArgRef {
    ErrArgPtr(void * ptr) {
        this->eak = ErrArgKind::Ptr;
        this->eaf = ErrArgFlags::None;
        this->ptr = ptr;
    }
};

struct ErrArgLocNode : ErrArgRef {
    ErrArgLocNode(BASENODE * node) {
        this->eak = ErrArgKind::LocNode;
        this->eaf = ErrArgFlags::RefOnly;
        this->node = node;
    }
};

struct ErrArgNameNode : ErrArgRef {
    ErrArgNameNode(BASENODE * node, ErrArgFlagsEnum eaf = ErrArgFlags::None) {
        this->eak = ErrArgKind::NameNode;
        this->eaf = eaf;
        this->node = node;
    }
};

struct ErrArgTypeNode : ErrArgRef {
    ErrArgTypeNode(BASENODE * node, ErrArgFlagsEnum eaf = ErrArgFlags::None) {
        this->eak = ErrArgKind::TypeNode;
        this->eaf = eaf;
        this->node = node;
    }
};

struct ErrArgSymKind : ErrArgRef {
    // Can't be inlined because of SYM not being defined yet
    ErrArgSymKind(SYM * sym);
};

struct ErrArgAggKind : ErrArgRef {
    // Can't be inlined because of AGGSYM not being defined yet
    ErrArgAggKind(AGGSYM * agg);
    ErrArgAggKind(AGGTYPESYM * ats);
};

////////////////////////////////////////////////////////////////////////////////
// CError
//
// This object is the implementation of ISCError for all compiler errors,
// including lexer, parser, and compiler errors.

class CError :
    public CComObjectRootMT,
    public ICSError
{
private:
    enum { bufferSize = 4096 };

    struct LOCATION
    {
        BSTR    bstrFile;
        POSDATA posStart;
        POSDATA posEnd;
    };

    ERRORKIND       m_iKind;
    CComBSTR        m_sbstrText;
    short           m_iID;
    BOOL            m_fWasWarning;
    short           m_iLocations;
    LOCATION        *m_pLocations;
    LOCATION        *m_pMappedLocations;

public:
    BEGIN_COM_MAP(CError)
        COM_INTERFACE_ENTRY(ICSError)
    END_COM_MAP()

    CError ();
    ~CError();

    static BSTR ComputeString (HINSTANCE hModuleMessages, long iResourceID, VarArgList args);
    HRESULT     Initialize (HINSTANCE hModuleMessages, long iErrorIndex, VarArgList args);
    HRESULT     Initialize (HINSTANCE hModuleMessages, ERRORKIND iKind, BOOL fWasWarning, short iErrorID, long iResourceID, VarArgList args);
    HRESULT     AddLocation (PCWSTR pszFile, const POSDATA *pposStart, const POSDATA *pposEnd, PCWSTR pszMapFile, const POSDATA *pMapStart, const POSDATA *pMapEnd);
    BOOL        UpdateLocations (const POSDATA &posOldEnd, const POSDATA &posNewEnd);
    ERRORKIND   Kind() { return m_iKind; }
    BOOL        WasWarning() { return m_fWasWarning; }
    HRESULT     WarnAsError(HINSTANCE hModuleMessages);
    short       ID() { return m_iID; }
    short       LocationCount() { return m_iLocations; }
    PCWSTR      GetText () { return m_sbstrText; }


    // ICSError
    STDMETHOD(GetErrorInfo)(long *piErrorID, ERRORKIND *pKind, PCWSTR *ppszText);
    STDMETHOD(GetLocationCount)(long *piCount);
    STDMETHOD(GetLocationAt)(long iIndex, PCWSTR *ppszFileName, POSDATA *pposStart, POSDATA *pposEnd);
    STDMETHOD(GetUnmappedLocationAt)(long iIndex, PCWSTR *ppszFileName, POSDATA *pposStart, POSDATA *pposEnd);

};

////////////////////////////////////////////////////////////////////////////////
// CErrorContainer
//
// This is a standard implementation of ICSErrorContainer.  It can either
// accumulate errors in order of addition, or maintain order by line number.
// It also has a range replacement mode, which allows new incoming errors to
// replace any existing ones (used by incremental tokenization).

class CErrorContainer :
    public CComObjectRootMT,
    public ICSErrorContainer
{
private:
    // Don't ever hold more than this many errors.
    enum { cerrMax = 200 };

    ERRORCATEGORY   m_iCategory;
    DWORD_PTR       m_dwID;
    CError          **m_ppErrors;
    long            m_iErrors;
    long            m_iWarnings;
    long            m_iWarnAsErrors;
    long            m_iCount;
    CErrorContainer *m_pReplacements;

    void ClearSomeErrors(long ierrMin, long ierrLim);

public:
    BEGIN_COM_MAP(CErrorContainer)
        COM_INTERFACE_ENTRY(ICSErrorContainer)
    END_COM_MAP()

    CErrorContainer();
    ~CErrorContainer();

    static  HRESULT     CreateInstance (ERRORCATEGORY iCategory, DWORD_PTR dwID, CErrorContainer **ppContainer);

    HRESULT     Initialize (ERRORCATEGORY iCategory, DWORD_PTR dwID);
    HRESULT     Clone (CErrorContainer **ppContainer);
    HRESULT     AddError (CError *pError);
    HRESULT     BeginReplacement ();
    HRESULT     EndReplacement (const POSDATA &posStart, const POSDATA &posOldEnd, const POSDATA &posNewEnd);
    long        Count () { return m_iCount; }
    long        RefCount () { return (long)m_dwRef; }
    void        ReleaseAllErrors ();

    // ICSErrorContainer
    STDMETHOD(GetContainerInfo)(ERRORCATEGORY *pCategory, DWORD_PTR *pdwID);
    STDMETHOD(GetErrorCount)(long *piWarnings, long *piErrors, long *piFatals, long *piTotal);
    STDMETHOD(GetWarnAsErrorCount)(long *piWarnAsErrors);
    STDMETHOD(GetErrorAt)(long iIndex, ICSError **ppError);
};



// Global Utility functions.
extern bool IsValidWarningNumber(int id);
extern short ErrorNumberFromID(long iErrorIndex);
extern bool __cdecl LoadAndFormatMessage(HINSTANCE hModuleMessages, int resid, __out_ecount(cchMax) PWSTR buffer, int cchMax, ...);
extern bool __cdecl LoadAndFormatMessage(int resid, __out_ecount(cchMax) PWSTR buffer, int cchMax, ...);
#ifdef DEBUG
extern bool IsWarningID(long iErrorIndex);
extern void CheckErrorMessageInfo(HINSTANCE hModuleMessages, MEMHEAP * heap, bool dumpAll);
#endif //DEBUG

inline bool HasBadChars(PCWSTR psz) {
    for ( ; *psz; psz++) {
        if (*psz == L'\xFFFF')
            return true;
    }
    return false;
}

inline void ClobberBadChars(PWCH pchMin, PWCH pchLim) {
    for ( ; pchMin < pchLim; pchMin++) {
        if (*pchMin == L'\xFFFF')
            *pchMin = L'.';
    }
}

#define ConvertAndCleanArgToStack(parg, ppsz) \
    if ((parg)->ConvertBasic(ppsz) && *ppsz && HasBadChars(*ppsz)) { \
        size_t _cch = wcslen(*ppsz) + 1; \
        PWCH _psz = STACK_ALLOC(wchar_t, _cch); \
        wcscpy_s(_psz, _cch, *ppsz); \
        ClobberBadChars(_psz, _psz + _cch); \
        *ppsz = _psz; \
    } else (void)0

#endif // __error_h__
