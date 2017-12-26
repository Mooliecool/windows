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
// This module contains a helper function used to produce string
// representations of types, with options to control the appearance of
// namespace and assembly information.  Its primary use is in
// reflection (Type.Name, Type.FullName, Type.ToString, etc) but over
// time it could replace the use of TypeHandle.GetName etc for
// diagnostic messages.
//
// See the header file for more details
// ---------------------------------------------------------------------------

#include "common.h"
#include "class.h"
#include "typehandle.h"
#include "sstring.h"
#include "sigformat.h"
#include "typeparse.h"
#include "typestring.h"
#include "comstring.h"
#include "ex.h"
#include "typedesc.h"

#ifndef DACCESS_COMPILE
FCIMPL0(TypeNameBuilder*, TypeNameBuilder::_CreateTypeNameBuilder) 
{ 
    WRAPPER_CONTRACT; 

    TypeNameBuilder*retVal = NULL;

    BEGIN_SO_INTOLERANT_CODE(GetThread());
    
    retVal = new TypeNameBuilder(new SString());

    END_SO_INTOLERANT_CODE;

    return retVal;
} 
FCIMPLEND

FCIMPL1(void, TypeNameBuilder::_ReleaseTypeNameBuilder, TypeNameBuilder *pTnb) 
{ 
    CONTRACTL {
        THROWS;
        DISABLED(GC_TRIGGERS);
        MODE_COOPERATIVE;
        SO_TOLERANT;
    }
    CONTRACTL_END;

    HELPER_METHOD_FRAME_BEGIN_0();
    pTnb->Release();
    HELPER_METHOD_FRAME_END();
} 
FCIMPLEND

FCIMPL1(StringObject*, TypeNameBuilder::_ToString, TypeNameBuilder *pTnb) 
{ 
    CONTRACTL {
        THROWS;
        DISABLED(GC_TRIGGERS);
        MODE_COOPERATIVE;
        SO_TOLERANT;
    }
    CONTRACTL_END;

    STRINGREF toString = NULL;

    HELPER_METHOD_FRAME_BEGIN_RET_0();
    {
        toString = COMString::NewString(*pTnb->GetString());    
    }
    HELPER_METHOD_FRAME_END();
    
    return (StringObject*)( STRINGREFToObject( toString ));
} 
FCIMPLEND

FCIMPL2(void, TypeNameBuilder::_AddName, TypeNameBuilder *pTnb, StringObject* UNSAFEName) 
{ 
    CONTRACTL {
        THROWS;
        DISABLED(GC_TRIGGERS);
        MODE_COOPERATIVE;
        SO_TOLERANT;
    }
    CONTRACTL_END;
    
    STRINGREF sName = (STRINGREF) UNSAFEName;
    
    BEGIN_SO_INTOLERANT_CODE_NOTHROW(GetThread(), FCThrowVoid(kStackOverflowException));
    pTnb->AddName(sName->GetBuffer()); 
    END_SO_INTOLERANT_CODE;
} 
FCIMPLEND

FCIMPL2(void, TypeNameBuilder::_AddAssemblySpec, TypeNameBuilder *pTnb, StringObject* UNSAFEAssemblySpec) 
{ 
    CONTRACTL {
        THROWS;
        DISABLED(GC_TRIGGERS);
        MODE_COOPERATIVE;
        SO_TOLERANT;
    }
    CONTRACTL_END;

    STRINGREF sAssemblySpec = (STRINGREF) UNSAFEAssemblySpec;
    
    BEGIN_SO_INTOLERANT_CODE_NOTHROW(GetThread(), FCThrowVoid(kStackOverflowException));
    pTnb->AddAssemblySpec(sAssemblySpec->GetBuffer()); 
    END_SO_INTOLERANT_CODE;
} 
FCIMPLEND

FCIMPL1(void, TypeNameBuilder::_OpenGenericArguments, TypeNameBuilder *pTnb) 
{ 
    WRAPPER_CONTRACT; 
    
    BEGIN_SO_INTOLERANT_CODE_NOTHROW(GetThread(), FCThrowVoid(kStackOverflowException));
    pTnb->OpenGenericArguments(); 
    END_SO_INTOLERANT_CODE;
} 
FCIMPLEND
    
FCIMPL1(void, TypeNameBuilder::_CloseGenericArguments, TypeNameBuilder *pTnb) 
{ 
    WRAPPER_CONTRACT; 
    
    BEGIN_SO_INTOLERANT_CODE_NOTHROW(GetThread(), FCThrowVoid(kStackOverflowException));
    pTnb->CloseGenericArguments(); 
    END_SO_INTOLERANT_CODE;
} 
FCIMPLEND
    
FCIMPL1(void, TypeNameBuilder::_OpenGenericArgument, TypeNameBuilder *pTnb) 
{ 
    WRAPPER_CONTRACT; 
    
    BEGIN_SO_INTOLERANT_CODE_NOTHROW(GetThread(), FCThrowVoid(kStackOverflowException));
    pTnb->OpenGenericArgument(); 
    END_SO_INTOLERANT_CODE;
} 
FCIMPLEND
    
FCIMPL1(void, TypeNameBuilder::_CloseGenericArgument, TypeNameBuilder *pTnb) 
{ 
    WRAPPER_CONTRACT; 
    
    BEGIN_SO_INTOLERANT_CODE_NOTHROW(GetThread(), FCThrowVoid(kStackOverflowException));
    pTnb->CloseGenericArgument(); 
    END_SO_INTOLERANT_CODE;
} 
FCIMPLEND
    
FCIMPL1(void, TypeNameBuilder::_AddPointer, TypeNameBuilder *pTnb) 
{ 
    WRAPPER_CONTRACT; 
    
    BEGIN_SO_INTOLERANT_CODE_NOTHROW(GetThread(), FCThrowVoid(kStackOverflowException));
    pTnb->AddPointer(); 
    END_SO_INTOLERANT_CODE;
} 
FCIMPLEND
    
FCIMPL1(void, TypeNameBuilder::_AddByRef, TypeNameBuilder *pTnb) 
{ 
    WRAPPER_CONTRACT; 
    
    BEGIN_SO_INTOLERANT_CODE_NOTHROW(GetThread(), FCThrowVoid(kStackOverflowException));
    pTnb->AddByRef(); 
    END_SO_INTOLERANT_CODE;
} 
FCIMPLEND
    
FCIMPL1(void, TypeNameBuilder::_AddSzArray, TypeNameBuilder *pTnb) 
{ 
    WRAPPER_CONTRACT; 
    
    BEGIN_SO_INTOLERANT_CODE_NOTHROW(GetThread(), FCThrowVoid(kStackOverflowException));
    pTnb->AddSzArray(); 
    END_SO_INTOLERANT_CODE;
} 
FCIMPLEND
    
FCIMPL2(void, TypeNameBuilder::_AddArray, TypeNameBuilder *pTnb, DWORD dwRank) 
{ 
    WRAPPER_CONTRACT; 
    
    BEGIN_SO_INTOLERANT_CODE_NOTHROW(GetThread(), FCThrowVoid(kStackOverflowException));
    pTnb->AddArray(dwRank); 
    END_SO_INTOLERANT_CODE;
}
FCIMPLEND
    
FCIMPL1(void, TypeNameBuilder::_Clear, TypeNameBuilder *pTnb) 
{ 
    WRAPPER_CONTRACT; 
    
    BEGIN_SO_INTOLERANT_CODE_NOTHROW(GetThread(), FCThrowVoid(kStackOverflowException));
    pTnb->Clear(); 
    END_SO_INTOLERANT_CODE;
} 
FCIMPLEND    
#endif

//
// TypeNameBuilder
//
TypeNameBuilder::TypeNameBuilder(SString* pStr, ParseState parseState /*= ParseStateSTART*/) : 
    m_pStr(NULL),
    m_count(0)
{ 
    WRAPPER_CONTRACT; 
    Clear(); 
    m_pStr = pStr;
    m_parseState = parseState; 
}

void TypeNameBuilder::PushOpenGenericArgument()
{
    WRAPPER_CONTRACT;

    m_stack.Push(m_pStr->GetCount());
}

void TypeNameBuilder::PopOpenGenericArgument()
{
    WRAPPER_CONTRACT;

    COUNT_T index = m_stack.Pop();
        
    if (!m_bHasAssemblySpec)
        m_pStr->Delete(m_pStr->Begin() + index - 1, 1);
    
    m_bHasAssemblySpec = FALSE;
}

HRESULT __stdcall TypeNameBuilder::QueryInterface(REFIID riid, void **ppUnk)
{
    WRAPPER_CONTRACT;
    
    *ppUnk = 0;

    if (riid == IID_IUnknown)
        *ppUnk = (IUnknown *)this;
    else if (riid == IID_ITypeNameBuilder)
        *ppUnk = (ITypeNameBuilder*)this;
    else
        return (E_NOINTERFACE);

    AddRef();
    return S_OK;
}

ULONG __stdcall TypeNameBuilder::Release()
{ 
    CONTRACTL
    {
        SO_TOLERANT;
        NOTHROW;
        GC_NOTRIGGER;
    }CONTRACTL_END;

    ULONG count = InterlockedDecrement((LONG*)&m_count); 
    if (count == 0) 
    {
        VALIDATE_BACKOUT_STACK_CONSUMPTION;
        delete this; 
    }
    return count; 
}

void TypeNameBuilder::EscapeName(LPCWSTR szName)
{
    WRAPPER_CONTRACT; 

    InlineSString<128> sszName(szName);
    SString::Iterator itr = sszName.Begin();
    BOOL bContainsReservedChar = FALSE;
    WCHAR c;

    while (*itr)
    {
        if (wcschr(TYPE_NAME_RESERVED_CHAR, *itr++))
        {
            bContainsReservedChar = TRUE;
            break;
        }
    }
    
    if (bContainsReservedChar)
    {
        itr = sszName.Begin();
        while (*itr)
        {
            c = *itr++;
            if (wcschr(TYPE_NAME_RESERVED_CHAR, c))
                Append(L"\\");
        
            Append(c);
        }
    }
    else
    {
        Append(szName);
    }
}

void TypeNameBuilder::EscapeAssemblyName(LPCWSTR szName)
{
    WRAPPER_CONTRACT; 

    Append(szName);
}

void TypeNameBuilder::EscapeEmbeddedAssemblyName(LPCWSTR szName)
{
    WRAPPER_CONTRACT; 

    InlineSString<128> sszName(szName);
    SString::Iterator itr = sszName.Begin();
    BOOL bContainsReservedChar = FALSE;
    WCHAR c;

    while (*itr)
    {
        if (L']' == *itr++)
        {
            bContainsReservedChar = TRUE;
            break;
        }
    }
    
    if (bContainsReservedChar)
    {
        itr = sszName.Begin();
        while (*itr)
        {
            c = *itr++;
            if (c == ']')
                Append(L'\\');

            Append(c);
        }
    }
    else
    {
        Append(szName);
    }
}

HRESULT __stdcall TypeNameBuilder::OpenGenericArgument() 
{ 
    WRAPPER_CONTRACT; 

    if (!CheckParseState(ParseStateSTART))
        return Fail();

    if (m_instNesting == 0)
        return Fail();

    HRESULT hr = S_OK;

    m_parseState = ParseStateSTART;
    m_bNestedName = FALSE;

    if (!m_bFirstInstArg)
        Append(L",");

    m_bFirstInstArg = FALSE;

    if (m_bUseAngleBracketsForGenerics)
        Append(L"<");
    else
        Append(L"[");
    PushOpenGenericArgument();
    
    return hr;
}

HRESULT __stdcall TypeNameBuilder::AddName(LPCWSTR szName) 
{ 
    WRAPPER_CONTRACT; 

    if (!szName)
        return Fail();
    
    if (!CheckParseState(ParseStateSTART | ParseStateNAME))
        return Fail();

    HRESULT hr = S_OK;

    m_parseState = ParseStateNAME;

    if (m_bNestedName)
        Append(L"+"); 

    m_bNestedName = TRUE;

    EscapeName(szName); 

    return hr;
}

HRESULT __stdcall TypeNameBuilder::AddName(LPCWSTR szName, LPCWSTR szNamespace) 
{ 
    WRAPPER_CONTRACT; 

    if (!szName)
        return Fail();
    
    if (!CheckParseState(ParseStateSTART | ParseStateNAME))
        return Fail();

    HRESULT hr = S_OK;

    m_parseState = ParseStateNAME;

    if (m_bNestedName)
        Append(L"+"); 

    m_bNestedName = TRUE;

    if (szNamespace && *szNamespace)
    {
        EscapeName(szNamespace); 
        Append(L".");
    }

    EscapeName(szName); 

    return hr;
}

HRESULT __stdcall TypeNameBuilder::OpenGenericArguments() 
{ 
    WRAPPER_CONTRACT; 
    
    if (!CheckParseState(ParseStateNAME))
        return Fail();

    HRESULT hr = S_OK;

    m_parseState = ParseStateSTART;
    m_instNesting ++;
    m_bFirstInstArg = TRUE;

    if (m_bUseAngleBracketsForGenerics)
        Append(L"<");
    else
        Append(L"[");

    return hr;
}

HRESULT __stdcall TypeNameBuilder::CloseGenericArguments() 
{ 
    WRAPPER_CONTRACT; 

    if (!m_instNesting)
        return Fail();
    if (!CheckParseState(ParseStateSTART))
        return Fail();

    HRESULT hr = S_OK;

    m_parseState = ParseStateGENARGS;

    m_instNesting --;

    if (m_bFirstInstArg)
    {
        m_pStr->Truncate(m_pStr->End() - 1);
    }    
    else
    {
        if (m_bUseAngleBracketsForGenerics)
            Append(L">"); 
        else
            Append(L"]"); 
    }

    return hr;
}

HRESULT __stdcall TypeNameBuilder::AddPointer() 
{ 
    WRAPPER_CONTRACT; 
    
    if (!CheckParseState(ParseStateNAME | ParseStateGENARGS | ParseStatePTRARR))
        return Fail();

    m_parseState = ParseStatePTRARR;
    
    Append(L"*"); 

    return S_OK;
}

HRESULT __stdcall TypeNameBuilder::AddByRef() 
{ 
    WRAPPER_CONTRACT; 
    
    if (!CheckParseState(ParseStateNAME | ParseStateGENARGS | ParseStatePTRARR))
        return Fail();

    m_parseState = ParseStateBYREF;
    
    Append(L"&"); 

    return S_OK;
}

HRESULT __stdcall TypeNameBuilder::AddSzArray() 
{ 
    WRAPPER_CONTRACT; 
    
    if (!CheckParseState(ParseStateNAME | ParseStateGENARGS | ParseStatePTRARR))
        return Fail();

    m_parseState = ParseStatePTRARR;
    
    Append(L"[]"); 

    return S_OK;
} 

HRESULT __stdcall TypeNameBuilder::AddArray(DWORD rank)
{
    WRAPPER_CONTRACT;
    
    if (!CheckParseState(ParseStateNAME | ParseStateGENARGS | ParseStatePTRARR))
        return Fail();

    m_parseState = ParseStatePTRARR;
        
    if (rank <= 0)
        return E_INVALIDARG;

    if (rank == 1)
        Append(L"[*]");
    else if (rank > 64)
    {
        // Only taken in an error path, runtime will not load arrays of more than 32 dimentions
        WCHAR wzDim[128];
        _snwprintf_s(wzDim, 128, _TRUNCATE, L"[%d]", rank);
        Append(wzDim);
    }
    else
    {
        WCHAR* wzDim = new (nothrow) WCHAR[rank+3];
            
        if(wzDim == NULL) // allocation failed, do it the long way (each Append -> memory realloc)
        {
            Append(L"[");
            for(COUNT_T i = 1; i < rank; i ++)
                Append(L",");
            Append(L"]");
        }
        else             // allocation OK, do it the fast way
        {
            WCHAR* pwz = wzDim+1;
            *wzDim = '[';
            for(COUNT_T i = 1; i < rank; i++, pwz++) *pwz=',';
            *pwz = ']';
            *(++pwz) = 0;
            Append(wzDim);
            delete [] wzDim;
        }
        
    }

    return S_OK;
}

HRESULT __stdcall TypeNameBuilder::CloseGenericArgument()
{
    WRAPPER_CONTRACT;
    
    if (!CheckParseState(ParseStateNAME | ParseStateGENARGS | ParseStatePTRARR | ParseStateBYREF | ParseStateASSEMSPEC))
        return Fail();

    if (m_instNesting == 0)
        return Fail();
    
    m_parseState = ParseStateSTART;
    
    if (m_bHasAssemblySpec)
    {
        if (m_bUseAngleBracketsForGenerics)
            Append(L">"); 
        else
            Append(L"]"); 
    }

    PopOpenGenericArgument();

    return S_OK;
}

HRESULT __stdcall TypeNameBuilder::AddAssemblySpec(LPCWSTR szAssemblySpec)
{
    WRAPPER_CONTRACT;

    if (!CheckParseState(ParseStateNAME | ParseStateGENARGS | ParseStatePTRARR | ParseStateBYREF))
        return Fail();

    HRESULT hr = S_OK;
    
    BEGIN_SO_INTOLERANT_CODE_NO_THROW_CHECK_THREAD(return COR_E_STACKOVERFLOW);
    
    m_parseState = ParseStateASSEMSPEC;

    if (szAssemblySpec && *szAssemblySpec)
    {
    
        Append(L", ");
        
        if (m_instNesting > 0)
        {
            EscapeEmbeddedAssemblyName(szAssemblySpec);
        }
        else
        {
            EscapeAssemblyName(szAssemblySpec);
        }

        m_bHasAssemblySpec = TRUE;
        hr = S_OK;
    }

    END_SO_INTOLERANT_CODE;
    
    return hr;
}

HRESULT __stdcall TypeNameBuilder::ToString(BSTR* pszStringRepresentation)
{
    WRAPPER_CONTRACT;

    if (!CheckParseState(ParseStateNAME | ParseStateGENARGS | ParseStatePTRARR | ParseStateBYREF | ParseStateASSEMSPEC))
        return Fail();

    if (m_instNesting)
        return Fail();

    *pszStringRepresentation = SysAllocString(m_pStr->GetUnicode());

    return S_OK;
}

HRESULT __stdcall TypeNameBuilder::Clear()
{
    WRAPPER_CONTRACT;
    CONTRACT_VIOLATION(SOToleranceViolation);

    if (m_pStr)
    {   
        m_pStr->Clear();
    }
    m_bNestedName = FALSE;
    m_instNesting = 0;
    m_bFirstInstArg = FALSE;
    m_parseState = ParseStateSTART;
    m_bHasAssemblySpec = FALSE;
    m_bUseAngleBracketsForGenerics = FALSE;
    m_stack.Clear();
    
    return S_OK;
}







// Append the name of the type td to the string
// The following flags in the FormatFlags argument are significant: FormatNamespace 
void TypeString::AppendTypeDef(SString& ss, IMDInternalImport *pImport, mdTypeDef td, DWORD format)
{
    CONTRACT_VOID
    {
        MODE_ANY;
        GC_NOTRIGGER;
        THROWS;
    }
    CONTRACT_END

    BEGIN_SO_INTOLERANT_CODE_NO_THROW_CHECK_THREAD(COMPlusThrowSO());
    {
        TypeNameBuilder tnb(&ss, TypeNameBuilder::ParseStateNAME);
        AppendTypeDef(tnb, pImport, td, format);    
    }
    END_SO_INTOLERANT_CODE;

    RETURN;
}


void TypeString::AppendTypeDef(TypeNameBuilder& tnb, IMDInternalImport *pImport, mdTypeDef td, DWORD format) 
{
    CONTRACT_VOID
    {
        MODE_ANY;
        GC_NOTRIGGER;
        THROWS;
        PRECONDITION(CheckPointer(pImport));
        PRECONDITION(TypeFromToken(td) == mdtTypeDef);
    }
    CONTRACT_END

    LPCUTF8 szName;
    LPCUTF8 szNameSpace;
    pImport->GetNameOfTypeDef(td, &szName, &szNameSpace);

    tnb.AddName(SString(SString::Utf8, szName).GetUnicode(), format & FormatNamespace ? SString(SString::Utf8, szNameSpace).GetUnicode() : NULL);

    RETURN;
}

void TypeString::AppendNestedTypeDef(TypeNameBuilder& tnb, IMDInternalImport *pImport, mdTypeDef td, DWORD format)
{
    CONTRACT_VOID
    {
        MODE_ANY;
        GC_NOTRIGGER;
        THROWS;
        PRECONDITION(CheckPointer(pImport));
        PRECONDITION(TypeFromToken(td) == mdtTypeDef);
    }
    CONTRACT_END

    DWORD dwAttr;
    pImport->GetTypeDefProps(td, &dwAttr, NULL);   

    StackSArray<mdTypeDef> arNames;
    arNames.Append(td);
    if (format & FormatNamespace && IsTdNested(dwAttr)) 
    {   
        while (SUCCEEDED(pImport->GetNestedClassProps(td, &td))) 
            arNames.Append(td);
    }   

    for(SCOUNT_T i = arNames.GetCount() - 1; i >= 0; i --)
        AppendTypeDef(tnb, pImport, arNames[i], format);

    RETURN;
}

// Append a square-bracket-enclosed, comma-separated list of n type parameters in inst to the string s
// and enclose each parameter in square brackets to disambiguate the commas
// The following flags in the FormatFlags argument are significant: FormatNamespace FormatFullInst FormatAssembly FormatNoVersion
void TypeString::AppendInst(SString& ss, int n, TypeHandle *inst, DWORD format) 
{
    CONTRACT_VOID
    {
        MODE_ANY;
        if (format & (FormatAssembly|FormatFullInst)) GC_TRIGGERS; else GC_NOTRIGGER;
        THROWS;
    }
    CONTRACT_END

    BEGIN_SO_INTOLERANT_CODE_NO_THROW_CHECK_THREAD(COMPlusThrowSO());
    {
        TypeNameBuilder tnb(&ss, TypeNameBuilder::ParseStateNAME);
        if ((format & FormatAngleBrackets) != 0)
            tnb.SetUseAngleBracketsForGenerics(TRUE);
        AppendInst(tnb, n, inst, format);
    }
    END_SO_INTOLERANT_CODE;

    RETURN;
}

void TypeString::AppendInst(TypeNameBuilder& tnb, int n, TypeHandle *inst, DWORD format) 
{
    CONTRACT_VOID
    {
        MODE_ANY;
        THROWS;
        if (format & (FormatAssembly|FormatFullInst)) GC_TRIGGERS; else GC_NOTRIGGER;
        PRECONDITION(n > 0);
        PRECONDITION(CheckPointer(inst));
    }
    CONTRACT_END

    tnb.OpenGenericArguments();

    for (int i = 0; i < n; i++) 
    {
        tnb.OpenGenericArgument();

        if ((format & FormatFullInst) != 0 && !inst[i].IsGenericVariable()) 
        {
            AppendType(tnb, inst[i], NULL, format | FormatNamespace | FormatAssembly);
        }
        else
        {
            AppendType(tnb, inst[i], NULL, format & (FormatNamespace | FormatAngleBrackets
#ifdef _DEBUG
                                               | FormatDebug
#endif
                                               ));
        }

        tnb.CloseGenericArgument();
    }

    tnb.CloseGenericArguments();

    RETURN;
}

void TypeString::AppendParamTypeQualifier(TypeNameBuilder& tnb, CorElementType kind, DWORD rank)
{
    CONTRACTL
    {
        MODE_ANY;
        THROWS;
        GC_NOTRIGGER;
        PRECONDITION(CorTypeInfo::IsModifier(kind));
    }
    CONTRACTL_END

    switch (kind)
    {
    case ELEMENT_TYPE_BYREF :
        tnb.AddByRef();
        break;
    case ELEMENT_TYPE_PTR :
        tnb.AddPointer();
        break;
    case ELEMENT_TYPE_SZARRAY :
        tnb.AddSzArray();
        break;
    case ELEMENT_TYPE_ARRAY :        
        tnb.AddArray(rank);
        break;
    default :
        break;
    }
}

// Append a representation of the type t to the string s
// The following flags in the FormatFlags argument are significant: FormatNamespace FormatFullInst FormatAssembly FormatNoVersion

void TypeString::AppendType(SString& ss, TypeHandle ty, DWORD format) 
{
    CONTRACT_VOID
    {
        MODE_ANY;
        if (format & (FormatAssembly|FormatFullInst)) GC_TRIGGERS; else GC_NOTRIGGER;
        THROWS;        
    }
    CONTRACT_END
    
    AppendType(ss, ty, NULL, format);

    RETURN;
}

void TypeString::AppendType(SString& ss, TypeHandle ty, TypeHandle* typeInstantiation, DWORD format) 
{
    CONTRACT_VOID
    {
        MODE_ANY;
        if (format & (FormatAssembly|FormatFullInst)) GC_TRIGGERS; else GC_NOTRIGGER;
        THROWS;        
        PRECONDITION(CheckPointer(typeInstantiation, NULL_OK));
    }
    CONTRACT_END
    
    BEGIN_SO_INTOLERANT_CODE_NO_THROW_CHECK_THREAD(COMPlusThrowSO());
    {
        TypeNameBuilder tnb(&ss);
        if ((format & FormatAngleBrackets) != 0)
            tnb.SetUseAngleBracketsForGenerics(TRUE);
        AppendType(tnb, ty, typeInstantiation, format);
    }
    END_SO_INTOLERANT_CODE;

    RETURN;
}

void TypeString::AppendType(TypeNameBuilder& tnb, TypeHandle ty, TypeHandle* typeInstantiation, DWORD format) 
{
    CONTRACT_VOID
    {
        MODE_ANY;
        
        /* This method calls Assembly::GetDisplayName. Since that function
        uses Fusion which takes some Crsts in some places, it is GC_TRIGGERS. 
        It could be made GC_NOTRIGGER by factoring out Assembly::GetDisplayName.
        However, its better to leave stuff as GC_TRIGGERS unless really needed, 
        as GC_NOTRIGGER ties your hands up. */
        if (format & (FormatAssembly|FormatFullInst)) GC_TRIGGERS; else GC_NOTRIGGER;
        THROWS;        
    }
    CONTRACT_END

    DECLARE_INTERIOR_STACK_PROBE;
    if (ShouldProbeOnThisThread())
    {
        DO_INTERIOR_STACK_PROBE_FOR(GetThread(),10);
    }

    BOOL bToString = (format & (FormatNamespace|FormatFullInst|FormatAssembly)) == FormatNamespace;

    // It's null!
    if (ty.IsNull())
    {
        tnb.AddName(L"(null)");
    }
    else
    // It's not restored yet!
    if (ty.IsEncodedFixup())
    {
        tnb.AddName(L"(fixup)");
    }
    else

    // It's an array, with format 
    //   element_ty[] (1-d, SZARRAY) 
    //   element_ty[*] (1-d, ARRAY)
    //   element_ty[,] (2-d, ARRAY) etc
    // or a pointer (*) or byref (&)
    if (ty.HasTypeParam() || (!ty.IsTypeDesc() && ty.AsMethodTable()->IsArray())) 
    {
        DWORD rank;
        TypeHandle elemType;
        if (ty.HasTypeParam())
        {
            rank = ty.IsArray() ? ty.AsArray()->GetRank() : 0; 
            elemType = ty.GetTypeParam();
        }
        else
        {
            MethodTable *pMT = ty.GetMethodTable();
            PREFIX_ASSUME(pMT != NULL);
            rank = pMT->GetRank();
            elemType = pMT->GetApproxArrayElementTypeHandle();
        }

        _ASSERTE(!elemType.IsNull());
        AppendType(tnb, elemType, NULL, format & ~FormatAssembly);
        AppendParamTypeQualifier(tnb, ty.GetSignatureCorElementType(), rank);
    }

    // ...or type parameter
    else if (ty.IsGenericVariable()) 
    {
        TypeVarTypeDesc* tyvar = (TypeVarTypeDesc*) (ty.AsTypeDesc());
        mdGenericParam token = tyvar->GetToken();

        LPCSTR szName = NULL;

        ty.GetModule()->GetMDImport()->GetGenericParamProps(token, NULL, NULL, NULL, NULL, &szName);

        StackSString pName(SString::Utf8, szName);
        tnb.AddName(pName.GetUnicode());

        format &= ~FormatAssembly;
    }

    // ...or function pointer
    else if (ty.IsFnPtrType())
    {
        // Don't attempt to format this currently, it may trigger GC due to fixups.
        tnb.AddName(L"(fnptr)");
    }

    // ...otherwise it's just a plain type def or an instantiated type
    else 
    {
        // Get the TypeDef token and attributes
        IMDInternalImport *pImport = ty.GetMethodTable()->GetMDImport();
        mdTypeDef td = ty.GetCl();
        _ASSERTE(!IsNilToken(td));

#ifdef _DEBUG
        if (format & FormatDebug)
        {
            WCHAR wzAddress[128];
            _snwprintf_s(wzAddress, 128, _TRUNCATE, L"(%p)", ty.AsPtr());
            tnb.AddName(wzAddress);
        }
#endif
        AppendNestedTypeDef(tnb, pImport, td, format);

        // Append the instantiation
        if ((format & (FormatNamespace|FormatAssembly)) && ty.HasInstantiation() && (!ty.IsGenericTypeDefinition() || bToString))
        {
            if (typeInstantiation == NULL)
                AppendInst(tnb, ty.GetNumGenericArgs(), ty.GetInstantiation(), format);
            else
                AppendInst(tnb, ty.GetNumGenericArgs(), typeInstantiation, format);
        }
    }

    // Now append the assembly
    if (format & FormatAssembly) 
    {
        Assembly* pAssembly = ty.GetAssembly();
        _ASSERTE(pAssembly != NULL);

        StackSString pAssemblyName;
#ifdef DACCESS_COMPILE
        pAssemblyName.SetUTF8(pAssembly->GetSimpleName());
#else
        pAssembly->GetDisplayName(pAssemblyName,
                                  ASM_DISPLAYF_PUBLIC_KEY_TOKEN | (format & FormatNoVersion ? 0 : ASM_DISPLAYF_VERSION | ASM_DISPLAYF_CULTURE));
#endif

        tnb.AddAssemblySpec(pAssemblyName.GetUnicode());

    }  
    
    END_INTERIOR_STACK_PROBE;


  RETURN;
}

#ifndef DACCESS_COMPILE

StringObject* TypeString::ConstructName(TypeHandle typeHandle, DWORD format)
{
    CONTRACTL
    {
        MODE_ANY;
        GC_TRIGGERS;
        THROWS;
        PRECONDITION(CheckPointer(typeHandle));
    }
    CONTRACTL_END

    if (format & FormatFullInst && typeHandle.IsGenericVariable())
        return NULL;

    typeHandle.CheckRestore();
    StackSString ss;    
    TypeString::AppendType(ss, typeHandle, format);
    return (StringObject*)( OBJECTREFToObject( COMString::NewString(ss) ));
}

#endif // DACCESS_COMPILE

void TypeString::AppendMethod(SString& s, MethodDesc *pMD, TypeHandle* typeInstantiation, const DWORD format)
{
    CONTRACTL
    {
        MODE_ANY;
        GC_TRIGGERS;
        THROWS;
        PRECONDITION(CheckPointer(pMD));
        PRECONDITION(pMD->IsRestored());
        PRECONDITION(s.Check());
    }
    CONTRACTL_END

    AppendMethodImpl(s, pMD, typeInstantiation, format);
}

void TypeString::AppendMethodInternal(SString& s, MethodDesc *pMD, const DWORD format)
{
    CONTRACTL
    {
        MODE_ANY;
        GC_TRIGGERS;
        THROWS;
        PRECONDITION(CheckPointer(pMD));
        PRECONDITION(pMD->IsRestored());
        PRECONDITION(s.Check());
    }
    CONTRACTL_END

    AppendMethodImpl(s, pMD, NULL, format);
}

void TypeString::AppendMethodImpl(SString& ss, MethodDesc *pMD, TypeHandle* typeInstantiation, const DWORD format)
{
    CONTRACTL
    {
        MODE_ANY;
        GC_TRIGGERS;
        THROWS;
        PRECONDITION(CheckPointer(pMD));
        PRECONDITION(pMD->IsRestored());
        PRECONDITION(ss.Check());
    }
    CONTRACTL_END

    BEGIN_SO_INTOLERANT_CODE_NO_THROW_CHECK_THREAD(COMPlusThrowSO());
    {
        TypeHandle th;
        
        if (pMD->IsDynamicMethod())
        {
            if (pMD->IsLCGMethod())
            {
                ss += SString(SString::Ascii, "DynamicClass");
            }
        }
        else 
        {
            th = TypeHandle(pMD->GetMethodTable());
            AppendType(ss, th, typeInstantiation, format);
        }
        
        ss += SString(SString::Ascii, NAMESPACE_SEPARATOR_STR);
        ss += SString(SString::Utf8, pMD->GetName());

        if (pMD->HasMethodInstantiation() && !pMD->IsGenericMethodDefinition())
        {
            //if (pTypeContext == NULL)
                AppendInst(ss, pMD->GetNumGenericMethodArgs(), pMD->GetMethodInstantiation(), format);
                /*
            else
                AppendInst(ss, pMD->GetNumGenericMethodArgs()pTypeContext->m_methInstCount, pTypeContext->m_methInst, format);
                */
        }
        
        if (format & FormatSignature)
        {
            
            SigFormat sigFormatter(pMD, th);
            const char* sigStr = sigFormatter.GetCStringParmsOnly();
            ss += SString(SString::Utf8, sigStr);
        }
        
        if (format & FormatStubInfo) {
            if (pMD->IsInstantiatingStub())
                ss += SString(SString::Ascii, "{inst-stub}");
            if (pMD->IsUnboxingStub())
                ss += SString(SString::Ascii, "{unbox-stub}");
            if (pMD->IsSharedByGenericMethodInstantiations())
                ss += SString(SString::Ascii, "{method-shared}");
            else if (pMD->IsSharedByGenericInstantiations())
                ss += SString(SString::Ascii, "{shared}");
            if (pMD->RequiresInstMethodTableArg())
                ss += SString(SString::Ascii, "{requires-mt-arg}");
            if (pMD->RequiresInstMethodDescArg())
                ss += SString(SString::Ascii, "{requires-mdesc-arg}");
        }
    }
    END_SO_INTOLERANT_CODE;
}


#ifdef _DEBUG
void TypeString::AppendMethodDebug(SString& ss, MethodDesc *pMD)
{
    CONTRACTL
    {
        MODE_ANY;
        GC_TRIGGERS;
        NOTHROW;
        PRECONDITION(CheckPointer(pMD));
        PRECONDITION(pMD->IsRestored());
        PRECONDITION(ss.Check());
    }
    CONTRACTL_END

#ifndef DACCESS_COMPILE
    EX_TRY
    {
        AppendMethodInternal(ss, pMD, FormatSignature | FormatNamespace);
    }
    EX_CATCH
    {
        // This function is only used as diagnostic aid in debug builds.
        // If we run out of memory or hit some other problem,
        // tough luck for the debugger.

        // Should we set ss to Empty
    }
    EX_END_CATCH(SwallowAllExceptions);
#endif
}

void TypeString::AppendTypeDebug(SString& ss, TypeHandle t)
{
    CONTRACTL
    {
        MODE_ANY;
        GC_NOTRIGGER;
        NOTHROW;
        PRECONDITION(CheckPointer(t));
        PRECONDITION(ss.Check());
        SO_NOT_MAINLINE;
    }
    CONTRACTL_END

#ifndef DACCESS_COMPILE
    {
        EX_TRY
        {
            AppendType(ss, t, FormatNamespace | FormatDebug);
        }
        EX_CATCH
        {
            // This function is only used as diagnostic aid in debug builds. 
            // If we run out of memory or hit some other problem,
            // tough luck for the debugger.
        }
        EX_END_CATCH(SwallowAllExceptions);
    }
#endif
}

void TypeString::AppendTypeKeyDebug(SString& ss, TypeKey *pTypeKey)
{
    CONTRACTL
    {
        MODE_ANY;
        GC_NOTRIGGER;
        NOTHROW;
        PRECONDITION(CheckPointer(pTypeKey));
        PRECONDITION(ss.Check());
        SO_NOT_MAINLINE;
    }
    CONTRACTL_END

#ifndef DACCESS_COMPILE
    {
        EX_TRY
        {
            AppendTypeKey(ss, pTypeKey, FormatNamespace | FormatDebug);
        }
        EX_CATCH
        {
            // This function is only used as diagnostic aid in debug builds. 
            // If we run out of memory or hit some other problem,
            // tough luck for the debugger.
        }
        EX_END_CATCH(SwallowAllExceptions);
    }
#endif 
}

#endif // _DEBUG


void TypeString::AppendTypeKey(TypeNameBuilder& tnb, TypeKey *pTypeKey, DWORD format)
{
    CONTRACT_VOID
    {
        MODE_ANY;
        THROWS;
        if (format & (FormatAssembly|FormatFullInst)) GC_TRIGGERS; else GC_NOTRIGGER;
        PRECONDITION(CheckPointer(pTypeKey));
        SO_INTOLERANT;
    }
    CONTRACT_END

    Module *pModule = NULL;

    // It's an array, with format 
    //   element_ty[] (1-d, SZARRAY) 
    //   element_ty[*] (1-d, ARRAY)
    //   element_ty[,] (2-d, ARRAY) etc
    // or a pointer (*) or byref (&)
    CorElementType kind = pTypeKey->GetKind();
    if (CorTypeInfo::IsModifier(kind))
    {
        DWORD rank = 0;
        TypeHandle elemType = pTypeKey->GetElementType();
        if (CorTypeInfo::IsArray(kind))
        {
            rank = pTypeKey->GetRank();
        }

        AppendType(tnb, elemType, NULL, format);
        AppendParamTypeQualifier(tnb, kind, rank);
        pModule = elemType.GetModule();
    }
    else if (kind == ELEMENT_TYPE_FNPTR)
    {
        RETURN;
    }

    // ...otherwise it's just a plain type def or an instantiated type
    else 
    {
        // Get the TypeDef token and attributes
        pModule = pTypeKey->GetModule();
        if (pModule != NULL)
        {
            IMDInternalImport *pImport = pModule->GetMDImport();
            mdTypeDef td = pTypeKey->GetTypeToken();
            _ASSERTE(!IsNilToken(td));

            AppendNestedTypeDef(tnb, pImport, td, format);

            // Append the instantiation
            if ((format & (FormatNamespace|FormatAssembly)) && pTypeKey->HasInstantiation())
                AppendInst(tnb, pTypeKey->GetNumGenericArgs(), pTypeKey->GetInstantiation(), format);
        }

    }

    // Now append the assembly
    if (pModule != NULL && (format & FormatAssembly))
    {
        Assembly* pAssembly = pModule->GetAssembly();
        _ASSERTE(pAssembly != NULL);

        StackSString pAssemblyName;
#ifdef DACCESS_COMPILE
        pAssemblyName.SetUTF8(pAssembly->GetSimpleName());
#else
        pAssembly->GetDisplayName(pAssemblyName,
                                  ASM_DISPLAYF_PUBLIC_KEY_TOKEN | (format & FormatNoVersion ? 0 : ASM_DISPLAYF_VERSION | ASM_DISPLAYF_CULTURE));
#endif
        tnb.AddAssemblySpec(pAssemblyName.GetUnicode());
    }            

    RETURN;
}

void TypeString::AppendTypeKey(SString& ss, TypeKey *pTypeKey, DWORD format)
{
    CONTRACT_VOID
    {
        MODE_ANY;
        if (format & (FormatAssembly|FormatFullInst)) GC_TRIGGERS; else GC_NOTRIGGER;
        THROWS;
        PRECONDITION(CheckPointer(pTypeKey));
    }
    CONTRACT_END
    
    BEGIN_SO_INTOLERANT_CODE_NO_THROW_CHECK_THREAD(COMPlusThrowSO());
    {
        TypeNameBuilder tnb(&ss);
        AppendTypeKey(tnb, pTypeKey, format);
    }
    END_SO_INTOLERANT_CODE;

    RETURN;
}

HRESULT __stdcall TypeNameBuilderWrapper::QueryInterface(REFIID riid, void **ppUnk)
{
    CONTRACTL
    {
        NOTHROW;
        GC_NOTRIGGER;
        MODE_ANY;
        SO_TOLERANT;
    }
    CONTRACTL_END;

    *ppUnk = 0;

    if (riid == IID_IUnknown)
        *ppUnk = (IUnknown *)this;
    else if (riid == IID_ITypeNameBuilder)
        *ppUnk = (ITypeNameBuilder*)this;
    else
        return (E_NOINTERFACE);

    AddRef();
    return S_OK;
}

ULONG __stdcall TypeNameBuilderWrapper::AddRef()
{
    CONTRACTL
    {
        NOTHROW;
        GC_NOTRIGGER;
        MODE_ANY;
        SO_TOLERANT;
    }
    CONTRACTL_END;

    return InterlockedIncrement(&m_ref);
}

ULONG __stdcall TypeNameBuilderWrapper::Release()
{
    CONTRACTL
    {
        NOTHROW;
        GC_NOTRIGGER;
        MODE_ANY;
        SO_TOLERANT;
    }
    CONTRACTL_END;

    LONG ref = 0;

    BEGIN_SO_INTOLERANT_CODE_NO_THROW_CHECK_THREAD(return COR_E_STACKOVERFLOW);
    
    ref = InterlockedDecrement(&m_ref);
    if (ref == 0)
        delete this;

    END_SO_INTOLERANT_CODE;

    return ref;
}


HRESULT __stdcall TypeNameBuilderWrapper::OpenGenericArguments()
{
    WRAPPER_CONTRACT;

    HRESULT hr;
    BEGIN_SO_INTOLERANT_CODE_NO_THROW_CHECK_THREAD(return COR_E_STACKOVERFLOW);
    hr = m_tnb.OpenGenericArguments();
    END_SO_INTOLERANT_CODE;
    return hr;
}

HRESULT __stdcall TypeNameBuilderWrapper::CloseGenericArguments()
{
    WRAPPER_CONTRACT;

    HRESULT hr;
    BEGIN_SO_INTOLERANT_CODE_NO_THROW_CHECK_THREAD(return COR_E_STACKOVERFLOW);
    hr = m_tnb.CloseGenericArguments();
    END_SO_INTOLERANT_CODE;
    return hr;
}

HRESULT __stdcall TypeNameBuilderWrapper::OpenGenericArgument()
{
    WRAPPER_CONTRACT;

    HRESULT hr;
    BEGIN_SO_INTOLERANT_CODE_NO_THROW_CHECK_THREAD(return COR_E_STACKOVERFLOW);
    hr = m_tnb.OpenGenericArgument();
    END_SO_INTOLERANT_CODE;
    return hr;
}

HRESULT __stdcall TypeNameBuilderWrapper::CloseGenericArgument()
{
    WRAPPER_CONTRACT;

    HRESULT hr;
    BEGIN_SO_INTOLERANT_CODE_NO_THROW_CHECK_THREAD(return COR_E_STACKOVERFLOW);
    hr = m_tnb.CloseGenericArgument();
    END_SO_INTOLERANT_CODE;
    return hr;
}

HRESULT __stdcall TypeNameBuilderWrapper::AddName(LPCWSTR szName)
{
    WRAPPER_CONTRACT;

    HRESULT hr;
    BEGIN_SO_INTOLERANT_CODE_NO_THROW_CHECK_THREAD(return COR_E_STACKOVERFLOW);
    hr = m_tnb.AddName(szName);
    END_SO_INTOLERANT_CODE;
    return hr;
}

HRESULT __stdcall TypeNameBuilderWrapper::AddPointer()
{
    WRAPPER_CONTRACT;

    HRESULT hr;
    BEGIN_SO_INTOLERANT_CODE_NO_THROW_CHECK_THREAD(return COR_E_STACKOVERFLOW);
    hr = m_tnb.AddPointer();
    END_SO_INTOLERANT_CODE;
    return hr;
}

HRESULT __stdcall TypeNameBuilderWrapper::AddByRef()
{
    WRAPPER_CONTRACT;

    HRESULT hr;
    BEGIN_SO_INTOLERANT_CODE_NO_THROW_CHECK_THREAD(return COR_E_STACKOVERFLOW);
    hr = m_tnb.AddByRef();
    END_SO_INTOLERANT_CODE;
    return hr;
}

HRESULT __stdcall TypeNameBuilderWrapper::AddSzArray()
{
    WRAPPER_CONTRACT;

    HRESULT hr;
    BEGIN_SO_INTOLERANT_CODE_NO_THROW_CHECK_THREAD(return COR_E_STACKOVERFLOW);
    hr = m_tnb.AddSzArray();
    END_SO_INTOLERANT_CODE;
    return hr;
}

HRESULT __stdcall TypeNameBuilderWrapper::AddArray(DWORD rank)
{
    WRAPPER_CONTRACT;

    HRESULT hr;
    BEGIN_SO_INTOLERANT_CODE_NO_THROW_CHECK_THREAD(return COR_E_STACKOVERFLOW);
    hr = m_tnb.AddArray(rank);
    END_SO_INTOLERANT_CODE;
    return hr;
}

HRESULT __stdcall TypeNameBuilderWrapper::AddAssemblySpec(LPCWSTR szAssemblySpec)
{
    WRAPPER_CONTRACT;

    HRESULT hr;
    BEGIN_SO_INTOLERANT_CODE_NO_THROW_CHECK_THREAD(return COR_E_STACKOVERFLOW);
    hr = m_tnb.AddAssemblySpec(szAssemblySpec);
    END_SO_INTOLERANT_CODE;
    return hr;
}

HRESULT __stdcall TypeNameBuilderWrapper::ToString(BSTR* pszStringRepresentation)
{
    WRAPPER_CONTRACT;

    HRESULT hr;
    BEGIN_SO_INTOLERANT_CODE_NO_THROW_CHECK_THREAD(return COR_E_STACKOVERFLOW);
    hr = m_tnb.ToString(pszStringRepresentation);
    END_SO_INTOLERANT_CODE;
    return hr;
}

HRESULT __stdcall TypeNameBuilderWrapper::Clear()
{
    WRAPPER_CONTRACT;

    HRESULT hr;
    BEGIN_SO_INTOLERANT_CODE_NO_THROW_CHECK_THREAD(return COR_E_STACKOVERFLOW);
    hr = m_tnb.Clear();
    END_SO_INTOLERANT_CODE;
    return hr;
}
