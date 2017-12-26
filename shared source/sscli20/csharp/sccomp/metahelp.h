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
// File: metahelp.h
//
// Helper routines for importing/emitting CLR metadata.
// ===========================================================================

#ifndef __metahelp_h__
#define __metahelp_h__

class MetaDataHelper
{
protected:
    WCHAR m_chNested;

    virtual void EscapeSpecialChars(StringBldr & str, int ichMin) { }
    virtual void GetTypeVars(SYM * sym, StringBldr & str, SYM * symInner);

    void AddTypeModifiers(PTYPESYM type, StringBldr & str);

public:
    MetaDataHelper(WCHAR chNested = NESTED_CLASS_SEP) {
        m_chNested = chNested;
    }

    // Instance methods.
    void GetMetaDataName(BAGSYM * sym, StringBldr & str);
    void GetFullName(SYM * sym, StringBldr & str, SYM * symInner = NULL);
    void GetExplicitImplName(SYM * sym, StringBldr & str);
    void GetExplicitImplTypeName(TYPESYM * type, StringBldr & str);

    // Static methods.
    static DWORD GetTypeAccessFlags(SYM * sym);  // Accepts TYPEDEFSYMs or AGGSYMs
    static DWORD GetAggregateFlags(AGGSYM * sym);

    static bool GetMetaDataName(BAGSYM * sym, __out_ecount(cch) PWSTR buffer, int cch) {
        StringBldrFixed str(buffer, cch);
        MetaDataHelper help;
        help.GetMetaDataName(sym, str);
        return !str.Error();
    }
    static bool GetFullName(SYM * sym, __out_ecount(cch) PWSTR buffer, int cch) {
        StringBldrFixed str(buffer, cch);
        MetaDataHelper help;
        help.GetFullName(sym, str);
        return !str.Error();
    }
    static bool GetExplicitImplName(SYM * sym, __out_ecount(cch) PWSTR buffer, int cch) {
        StringBldrFixed str(buffer, cch);
        MetaDataHelper help;
        help.GetExplicitImplName(sym, str);
        return !str.Error();
    }
};

class MetaDataHelperXml : public MetaDataHelper
{
protected:
    COMPILER * m_compiler;

    virtual void EscapeSpecialChars(StringBldr & str, int ichMin);
    virtual void GetTypeVars(SYM * sym, StringBldr & str, SYM * symInner);

public:
    MetaDataHelperXml(COMPILER * compiler, WCHAR chNested = L'.') : MetaDataHelper(chNested)
    {
        m_compiler = compiler;
    }

    void GetTypeName(TYPESYM * type, StringBldr & str);
};

class TypeNameSerializer
{
protected:
    COMPILER *                  m_compiler;
    CComPtr<ITypeNameBuilder>   m_qbldr;
    
    bool CheckHR(TYPESYM * type, HRESULT hr);
    bool GetAssemblyQualifiedTypeNameCore(TYPESYM * type, bool fOpenType);
    bool GetAggName(AGGSYM * agg);
    bool AddTypeModifiers(TYPESYM * type);

public:
    TypeNameSerializer(COMPILER * compiler);
    ~TypeNameSerializer();
    BSTR GetAssemblyQualifiedTypeName(TYPESYM * type, bool fOpenType);

};


class CAggSymNameEncImportIter
{
private:
    // Immutable state
    COMPILER *  m_compiler;
    AGGSYM *    m_agg;
    NAME *      m_name;

    // Enumerator state
    SYM *       m_curSym;
    bool        m_fSimpleName;

private:
    bool IsExplImpl(SYM *sym);
    bool IsEplImplWithName(SYM *sym);
    SYM *NextExplImplWithName(SYM *pSym);

public:
    CAggSymNameEncImportIter(COMPILER *compiler, AGGSYM *agg, NAME *name);

    bool MoveNext();
    SYM *Current();
};


#endif // __metahelp_h__
