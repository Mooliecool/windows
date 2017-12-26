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
// ---------------------------------------------------------------------------
// typestring.cpp
// ---------------------------------------------------------------------------
//
// This module contains all helper functions required to produce
// string representations of types, with options to control the
// appearance of namespace and assembly information.  Its primary use
// is in reflection (Type.Name, Type.FullName, Type.ToString, etc) but
// over time it could replace the use of TypeHandle.GetName etc for
// diagnostic messages.
//
// ---------------------------------------------------------------------------

#ifndef TYPESTRING_H
#define TYPESTRING_H

#include "common.h"
#include "class.h"
#include "typehandle.h"
#include "sstring.h"
#include "typekey.h"
#include "typeparse.h"

class TypeLibExporter;
class TypeString;

class TypeNameBuilder : public ITypeNameBuilder
{
    friend class TypeNameBuilderWrapper;

public:
    virtual HRESULT __stdcall QueryInterface(REFIID riid, void **ppUnk);
    virtual ULONG __stdcall AddRef() { LEAF_CONTRACT; return InterlockedIncrement((LONG*)&m_count); }
    virtual ULONG __stdcall Release();

public:
    static FCDECL1(void, _ReleaseTypeNameBuilder, TypeNameBuilder* pTnb);
    static FCDECL0(TypeNameBuilder*, _CreateTypeNameBuilder);
    static FCDECL1(void, _OpenGenericArguments, TypeNameBuilder *pTnb);
    static FCDECL1(void, _CloseGenericArguments, TypeNameBuilder *pTnb);
    static FCDECL1(void, _OpenGenericArgument, TypeNameBuilder *pTnb);
    static FCDECL1(void, _CloseGenericArgument, TypeNameBuilder *pTnb);
    static FCDECL2(void, _AddName, TypeNameBuilder *pTnb, StringObject* UNSAFEName);
    static FCDECL1(void, _AddPointer, TypeNameBuilder *pTnb);
    static FCDECL1(void, _AddByRef, TypeNameBuilder *pTnb);
    static FCDECL1(void, _AddSzArray, TypeNameBuilder *pTnb);
    static FCDECL2(void, _AddArray, TypeNameBuilder *pTnb, DWORD dwRank);
    static FCDECL2(void, _AddAssemblySpec, TypeNameBuilder *pTnb, StringObject* UNSAFEAssemblySpec);
    static FCDECL1(StringObject*, _ToString, TypeNameBuilder *pTnb);
    static FCDECL1(void, _Clear, TypeNameBuilder *pTnb);    

private:
    friend class TypeString;
    friend SString* TypeName::ToString(SString*, BOOL, BOOL, BOOL);
    friend TypeHandle TypeName::GetTypeWorker(BOOL, BOOL, BOOL, Assembly*, BOOL, BOOL, StackCrawlMark*, Assembly*, BOOL);
    virtual HRESULT __stdcall OpenGenericArguments(); 
    virtual HRESULT __stdcall CloseGenericArguments(); 
    virtual HRESULT __stdcall OpenGenericArgument(); 
    virtual HRESULT __stdcall CloseGenericArgument();
    virtual HRESULT __stdcall AddName(LPCWSTR szName); 
    virtual HRESULT __stdcall AddName(LPCWSTR szName, LPCWSTR szNamespace);
    virtual HRESULT __stdcall AddPointer(); 
    virtual HRESULT __stdcall AddByRef(); 
    virtual HRESULT __stdcall AddSzArray(); 
    virtual HRESULT __stdcall AddArray(DWORD rank);
    virtual HRESULT __stdcall AddAssemblySpec(LPCWSTR szAssemblySpec);
    virtual HRESULT __stdcall ToString(BSTR* pszStringRepresentation);
    virtual HRESULT __stdcall Clear();

private:
    class Stack
    {
    public:
        Stack() : m_depth(0) { LEAF_CONTRACT; } 
        
    public:  
        COUNT_T Push(COUNT_T element) { WRAPPER_CONTRACT; *m_stack.Append() = element; m_depth++; return Tos(); }
        COUNT_T Pop() { WRAPPER_CONTRACT; PRECONDITION(GetDepth() > 0); COUNT_T tos = Tos(); m_stack.Delete(m_stack.End() - 1); m_depth--; return tos; }
        COUNT_T Tos() { WRAPPER_CONTRACT; return m_stack.End()[-1]; }
        void Clear() { WRAPPER_CONTRACT; while(GetDepth()) Pop(); }
        COUNT_T GetDepth() { WRAPPER_CONTRACT; return m_depth; }
        
    private:
        INT32 m_depth;
        InlineSArray<COUNT_T, 16> m_stack;    
    };
        

public:
    typedef enum 
    {
        ParseStateSTART         = 0x0001,
        ParseStateNAME          = 0x0004,
        ParseStateGENARGS       = 0x0008,
        ParseStatePTRARR        = 0x0010,
        ParseStateBYREF         = 0x0020,
        ParseStateASSEMSPEC     = 0x0080,
        ParseStateERROR         = 0x0100,
    } 
    ParseState;

public:
    TypeNameBuilder(SString* pStr, ParseState parseState = ParseStateSTART);
    TypeNameBuilder() : m_count(0) { WRAPPER_CONTRACT; m_pStr = &m_str; Clear(); }
    void SetUseAngleBracketsForGenerics(BOOL value) { m_bUseAngleBracketsForGenerics = value; }
    void Append(LPCWSTR pStr) { WRAPPER_CONTRACT; m_pStr->Append(pStr); }
    void Append(WCHAR c) { WRAPPER_CONTRACT; m_pStr->Append(c); }
    SString* GetString() { WRAPPER_CONTRACT; return m_pStr; }

private:
    void EscapeName(LPCWSTR szName);
    void EscapeAssemblyName(LPCWSTR szName);
    void EscapeEmbeddedAssemblyName(LPCWSTR szName);
    BOOL CheckParseState(int validState) { WRAPPER_CONTRACT; return ((int)m_parseState & validState) != 0; }
    //BOOL CheckParseState(int validState) { WRAPPER_CONTRACT; ASSERT(((int)m_parseState & validState) != 0); return TRUE; }
    HRESULT Fail() { WRAPPER_CONTRACT; m_parseState = ParseStateERROR; return E_FAIL; }
    void PushOpenGenericArgument();
    void PopOpenGenericArgument();

private:
    ParseState m_parseState;
    SString* m_pStr;
    InlineSString<256> m_str;       
    DWORD m_instNesting;
    BOOL m_bFirstInstArg;
    BOOL m_bNestedName;
    BOOL m_bHasAssemblySpec;
    BOOL m_bUseAngleBracketsForGenerics;
    DWORD m_count;
    Stack m_stack;
};

// Class that's exposed to COM and wraps TypeNameBuilder (so that it can thunk
// all the entry points in order to perform stack probes).
class TypeNameBuilderWrapper : public ITypeNameBuilder
{
public:
    virtual HRESULT __stdcall QueryInterface(REFIID riid, void **ppUnk);
    virtual ULONG __stdcall AddRef();
    virtual ULONG __stdcall Release();

    virtual HRESULT __stdcall OpenGenericArguments(); 
    virtual HRESULT __stdcall CloseGenericArguments(); 
    virtual HRESULT __stdcall OpenGenericArgument(); 
    virtual HRESULT __stdcall CloseGenericArgument();
    virtual HRESULT __stdcall AddName(LPCWSTR szName); 
    virtual HRESULT __stdcall AddPointer(); 
    virtual HRESULT __stdcall AddByRef(); 
    virtual HRESULT __stdcall AddSzArray(); 
    virtual HRESULT __stdcall AddArray(DWORD rank);
    virtual HRESULT __stdcall AddAssemblySpec(LPCWSTR szAssemblySpec);
    virtual HRESULT __stdcall ToString(BSTR* pszStringRepresentation);
    virtual HRESULT __stdcall Clear();

    TypeNameBuilderWrapper() : m_ref(0) { WRAPPER_CONTRACT; m_tnb.AddRef(); }

private:
    LONG            m_ref;
    TypeNameBuilder m_tnb;
};

// --------------------------------------------------------------------------
// This type can generate names for types. It is used by reflection methods
// like System.RuntimeType.RuntimeTypeCache.ConstructName
//

class TypeString
{
    // -----------------------------------------------------------------------
    // WARNING WARNING WARNING WARNING WARNING WARNING WARNING WARNING WARNING 
    // -----------------------------------------------------------------------
    // Do no change the formatting of these strings as they are used by
    // serialization, and it would break serialization backwards-compatibility.
    
public:

  typedef enum 
  {
      FormatBasic       =   0x00000000, // Not a bitmask, simply the tersest flag settings possible
      FormatNamespace   =   0x00000001, // Include namespace and/or enclosing class names in type names
      FormatFullInst    =   0x00000002, // Include namespace and assembly in generic types (regardless of other flag settings)
      FormatAssembly    =   0x00000004, // Include assembly display name in type names
      FormatSignature   =   0x00000008, // Include signature in method names
      FormatNoVersion   =   0x00000010, // Suppress version and culture information in all assembly names
#ifdef _DEBUG
      FormatDebug       =   0x00000020, // For debug printing of types only
#endif
      FormatAngleBrackets = 0x00000040, // Whether generic types are C<T> or C[T]
      FormatStubInfo    =   0x00000080, // Include stub info like {unbox-stub}
  }
  FormatFlags;

public:   
    // Append the name of the type td to the string
    // The following flags in the FormatFlags argument are significant: FormatNamespace 
    static void AppendTypeDef(SString& tnb, IMDInternalImport *pImport, mdTypeDef td, DWORD format = FormatNamespace);

    // Append a square-bracket-enclosed, comma-separated list of n type parameters in inst to the string s
    // and enclose each parameter in square brackets to disambiguate the commas
    // The following flags in the FormatFlags argument are significant: FormatNamespace FormatFullInst FormatAssembly FormatNoVersion
    static void AppendInst(SString& s, int n, TypeHandle* inst, DWORD format = FormatNamespace);

    // Append a representation of the type t to the string s
    // The following flags in the FormatFlags argument are significant: FormatNamespace FormatFullInst FormatAssembly FormatNoVersion
    static void AppendType(SString& s, TypeHandle t, DWORD format = FormatNamespace);

    // Append a representation of the type t to the string s, using the generic
    // instantiation info provided, instead of the instantiation in the TypeHandle.
    static void AppendType(SString& s, TypeHandle t, TypeHandle* typeInstantiation, DWORD format = FormatNamespace);

    static void AppendTypeKey(SString& s, TypeKey *pTypeKey, DWORD format = FormatNamespace);

    // Appends the method name and generic instantiation info.  This might
    // look like "Namespace.ClassName[T].Foo[U, V]()"
    static void AppendMethod(SString& s, MethodDesc *pMD, TypeHandle* typeInstantiation, const DWORD format = FormatNamespace|FormatSignature);

    // Append a representation of the method m to the string s
    // The following flags in the FormatFlags argument are significant: FormatNamespace FormatFullInst FormatAssembly FormatSignature  FormatNoVersion
    static void AppendMethodInternal(SString& s, MethodDesc *pMD, const DWORD format = FormatNamespace|FormatSignature|FormatStubInfo);

#ifdef _DEBUG
    // These versions are NOTHROWS. They are meant for diagnostic purposes only
    // as they may leave "s" in a bad state if there are any problems/exceptions.
    static void AppendMethodDebug(SString& s, MethodDesc *pMD);
    static void AppendTypeDebug(SString& s, TypeHandle t);  
    static void AppendTypeKeyDebug(SString& s, TypeKey* pTypeKey);
#endif

#ifndef DACCESS_COMPILE
    // Helper function which delegates its work to AppendType and converts the result to a managed string.
    // The following flags in the FormatFlags argument are significant: FormatNamespace FormatFullInst FormatAssembly FormatNoVersion
    static StringObject* ConstructName(TypeHandle typeHandle, DWORD format);
#endif

private:
    friend class TypeLibExporter;
    static void AppendMethodImpl(SString& s, MethodDesc *pMD, TypeHandle* typeInstantiation, const DWORD format);
    static void AppendTypeDef(TypeNameBuilder& tnb, IMDInternalImport *pImport, mdTypeDef td, DWORD format = FormatNamespace);
    static void AppendNestedTypeDef(TypeNameBuilder& tnb, IMDInternalImport *pImport, mdTypeDef td, DWORD format = FormatNamespace);
    static void AppendInst(TypeNameBuilder& tnb, int n, TypeHandle* inst, DWORD format = FormatNamespace);
    static void AppendType(TypeNameBuilder& tnb, TypeHandle t, TypeHandle* typeInstantiation, DWORD format = FormatNamespace);
    static void AppendTypeKey(TypeNameBuilder& tnb, TypeKey *pTypeKey, DWORD format = FormatNamespace);
    static void AppendParamTypeQualifier(TypeNameBuilder& tnb, CorElementType kind, DWORD rank);    
};

#endif

