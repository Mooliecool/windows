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
// typeparse.cpp
// ---------------------------------------------------------------------------

#include "common.h"
#include "class.h"
#include "typehandle.h"
#include "sstring.h"
#include "typeparse.h"
#include "typestring.h"
#include "assemblynative.hpp"
#include "comstring.h"
#include"stackprobe.h"

//
// TypeNameFactory
//
HRESULT __stdcall TypeNameFactory::QueryInterface(REFIID riid, void **ppUnk)
{
    WRAPPER_CONTRACT;
    
    *ppUnk = 0;

    if (riid == IID_IUnknown)
        *ppUnk = (IUnknown *)this;
    else if (riid == IID_ITypeNameFactory)
        *ppUnk = (ITypeNameFactory*)this;
    else
        return (E_NOINTERFACE);

    AddRef();
    return S_OK;
}

HRESULT __stdcall TypeNameFactoryCreateObject(REFIID riid, void **ppUnk)
{ 
    CONTRACTL
    {
        NOTHROW;
        GC_NOTRIGGER;
    }
    CONTRACTL_END;

    HRESULT hr = S_OK;

    TypeNameFactory *pTypeNameFactory = new (nothrow) TypeNameFactory();
    
    if (!pTypeNameFactory)
        return (E_OUTOFMEMORY);

    hr = pTypeNameFactory->QueryInterface(riid, ppUnk);
    
    if (FAILED(hr))
        delete pTypeNameFactory;

    return hr;
}


HRESULT __stdcall TypeNameFactory::ParseTypeName(LPCWSTR szTypeName, DWORD* pError, ITypeName** ppTypeName)
{
    CONTRACTL
    {
        SO_TOLERANT;
        WRAPPER(THROWS);
    }CONTRACTL_END;

    if (!ppTypeName || !pError)
        return E_INVALIDARG;

	HRESULT hr = S_OK;
	BEGIN_SO_INTOLERANT_CODE_NO_THROW_CHECK_THREAD(return COR_E_STACKOVERFLOW);

    *ppTypeName = NULL;
    *pError = (DWORD)-1;

    ITypeName* pTypeName = new (nothrow) TypeName(szTypeName, pError);

    if (! pTypeName)
    {
        hr = E_OUTOFMEMORY;
    }
    else
    {
        pTypeName->AddRef();

        if (*pError != (DWORD)-1)
        {
            pTypeName->Release();       
            hr = S_FALSE;
        }
        else
        {
            *ppTypeName = pTypeName;
        }
    }

	END_SO_INTOLERANT_CODE;

    return hr;
}

HRESULT __stdcall TypeNameFactory::GetTypeNameBuilder(ITypeNameBuilder** ppTypeNameBuilder)
{
    WRAPPER_CONTRACT;

    if (!ppTypeNameBuilder)
        return E_INVALIDARG;

    *ppTypeNameBuilder = NULL;

    HRESULT hr = S_OK;

    BEGIN_SO_INTOLERANT_CODE_NO_THROW_CHECK_THREAD(return COR_E_STACKOVERFLOW);

    ITypeNameBuilder* pTypeNameBuilder = new (nothrow) TypeNameBuilderWrapper();

    if (pTypeNameBuilder)
    {
        pTypeNameBuilder->AddRef();

        *ppTypeNameBuilder = pTypeNameBuilder;
    }
    else
    {
        hr = E_OUTOFMEMORY;
    }

    END_SO_INTOLERANT_CODE;

    return hr;
}

//
// TypeName
//
SString* TypeName::ToString(SString* pBuf, BOOL bAssemblySpec, BOOL bSignature, BOOL bGenericArguments)
{
    WRAPPER_CONTRACT;

    PRECONDITION(!bGenericArguments & !bSignature &! bAssemblySpec);

    BEGIN_SO_INTOLERANT_CODE_NO_THROW_CHECK_THREAD(return pBuf);
    {
    TypeNameBuilder tnb(pBuf);

    for (COUNT_T i = 0; i < m_names.GetCount(); i ++)
        tnb.AddName(m_names[i]->GetUnicode());
    }
    END_SO_INTOLERANT_CODE;

    return pBuf;
}


DWORD __stdcall TypeName::AddRef() 
{ 
    LEAF_CONTRACT; 

    m_count++; 

    return m_count; 
}

DWORD __stdcall TypeName::Release() 
{ 
    CONTRACTL
    {
        THROWS;
        GC_TRIGGERS;
        SO_TOLERANT;
    }
    CONTRACTL_END;
    VALIDATE_BACKOUT_STACK_CONSUMPTION;

    m_count--; 

    DWORD dwCount = m_count;
    if (dwCount == 0) 
        delete this; 
    
    return dwCount; 
}

TypeName::~TypeName() 
{ 
    CONTRACTL
    {
        THROWS;
        GC_TRIGGERS;
        SO_TOLERANT;
    }
    CONTRACTL_END;
    VALIDATE_BACKOUT_STACK_CONSUMPTION;
 
    for(COUNT_T i = 0; i < m_genericArguments.GetCount(); i ++)
        m_genericArguments[i]->Release();
}

HRESULT __stdcall TypeName::QueryInterface(REFIID riid, void **ppUnk)
{
    WRAPPER_CONTRACT;
    
    *ppUnk = 0;

    if (riid == IID_IUnknown)
        *ppUnk = (IUnknown *)this;
    else if (riid == IID_ITypeName)
        *ppUnk = (ITypeName*)this;
    else
        return (E_NOINTERFACE);

    AddRef();
    return S_OK;
}

HRESULT __stdcall TypeName::GetNameCount(DWORD* pCount)
{
    WRAPPER_CONTRACT;

    if (!pCount)
        return E_INVALIDARG;

    *pCount = m_names.GetCount();

    return S_OK;
}

HRESULT __stdcall TypeName::GetNames(DWORD count, BSTR* bszName, DWORD* pFetched)
{
    CONTRACTL
    {
        SO_TOLERANT;
        WRAPPER(THROWS);
    }CONTRACTL_END;

    HRESULT hr = S_OK;
    
    if (!pFetched)
        return E_INVALIDARG;

    *pFetched = m_names.GetCount();
    
    if (m_names.GetCount() > count)
        return S_FALSE;

    if (!bszName)
        return E_INVALIDARG;

    BEGIN_SO_INTOLERANT_CODE_NO_THROW_CHECK_THREAD(return COR_E_STACKOVERFLOW);
    {
    for (COUNT_T i = 0; i < m_names.GetCount(); i ++)
        bszName[i] = SysAllocString(m_names[i]->GetUnicode());
    }
    END_SO_INTOLERANT_CODE;

    return hr;
}

HRESULT __stdcall TypeName::GetTypeArgumentCount(DWORD* pCount)
{
    WRAPPER_CONTRACT;

    if (!pCount)
        return E_INVALIDARG;

    *pCount = m_genericArguments.GetCount();

    return S_OK;
}

HRESULT __stdcall TypeName::GetTypeArguments(DWORD count, ITypeName** ppArguments, DWORD* pFetched)
{
    WRAPPER_CONTRACT;

    if (!pFetched)
        return E_INVALIDARG;

    *pFetched = m_genericArguments.GetCount();

    if (m_genericArguments.GetCount() > count)
        return S_FALSE;

    if (!ppArguments)
        return E_INVALIDARG;

    for (COUNT_T i = 0; i < m_genericArguments.GetCount(); i ++)
    {
        ppArguments[i] = m_genericArguments[i];
        m_genericArguments[i]->AddRef();
    }

    return S_OK;
}

HRESULT __stdcall TypeName::GetModifierLength(DWORD* pCount)
{
    WRAPPER_CONTRACT;

    if (pCount == NULL)
        return E_INVALIDARG;

    *pCount = m_signature.GetCount();

    return S_OK;
}

HRESULT __stdcall TypeName::GetModifiers(DWORD count, DWORD* pModifiers, DWORD* pFetched)
{
    WRAPPER_CONTRACT;

    if (!pFetched)
        return E_INVALIDARG;

    *pFetched = m_signature.GetCount();

    if (m_signature.GetCount() > count)
        return S_FALSE;

    if (!pModifiers)
        return E_INVALIDARG;

    for (COUNT_T i = 0; i < m_signature.GetCount(); i ++)
        pModifiers[i] = m_signature[i];    

    return S_OK;
}

HRESULT __stdcall TypeName::GetAssemblyName(BSTR* pszAssemblyName)
{
    CONTRACTL
    {
        SO_TOLERANT;
        WRAPPER(THROWS);
    }CONTRACTL_END;



    HRESULT hr = S_OK;

    if (pszAssemblyName == NULL)
        return E_INVALIDARG;

    BEGIN_SO_INTOLERANT_CODE_NO_THROW_CHECK_THREAD(return COR_E_STACKOVERFLOW);
    {
    *pszAssemblyName = SysAllocString(m_assembly.GetUnicode());
    }
    END_SO_INTOLERANT_CODE;

    if (*pszAssemblyName == NULL)
        hr= E_OUTOFMEMORY;

    return hr;
}


//
// TypeName::TypeNameParser
//
#undef IfFailGo
#define IfFailGo(P) if (!P) return FALSE;

TypeName* TypeName::AddGenericArgument() 
{ 
    WRAPPER_CONTRACT;

    TypeName* pGenArg = new TypeName();
    pGenArg->AddRef();

    pGenArg->m_bIsGenericArgument = TRUE;
    return m_genericArguments.AppendEx(pGenArg);
}

TypeName::TypeNameParser::TypeNameTokens TypeName::TypeNameParser::LexAToken()
{
    WRAPPER_CONTRACT;

    if (m_nextToken == TypeNameIdentifier)
        return TypeNamePostIdentifier;

    if (m_nextToken == TypeNameEnd)
        return TypeNameEnd;

    if (*m_itr == L'\0')
        return TypeNameEnd;
    
    if (COMCharacter::nativeIsWhiteSpace(*m_itr))
    {
        m_itr++;
        return LexAToken();
    }

    WCHAR c = *m_itr;
    m_itr++; 
    switch(c)
    {
        case L',': return TypeNameComma;
        case L'[': return TypeNameOpenSqBracket;
        case L']': return TypeNameCloseSqBracket;
        case L'&': return TypeNameAmperstand;
        case L'*': return TypeNameAstrix;
        case L'+': return TypeNamePlus;
        case L'\\': 
            m_itr--;
            return TypeNameIdentifier;
    }

    ASSERT(!wcschr(TYPE_NAME_RESERVED_CHAR, c));

    m_itr--;
    return TypeNameIdentifier;
}

BOOL TypeName::TypeNameParser::GetIdentifier(SString* sszId, TypeName::TypeNameParser::TypeNameIdentifiers identifierType)
{
    WRAPPER_CONTRACT;
    PRECONDITION(m_currentToken == TypeNameIdentifier && m_nextToken == TypeNamePostIdentifier);

    sszId->Clear();

    SString::Iterator start = m_currentItr;
    InlineSArray<SString::Iterator, 32> m_escape;
    
    if (identifierType == TypeNameId)
    {
        do
        {               
            while(*m_currentItr == L'\\')
            {
                m_escape.Append(m_currentItr++);
                                
                if (!wcschr(TYPE_NAME_RESERVED_CHAR, *m_currentItr) || *m_currentItr == '\0')
                    return FALSE;

                m_currentItr++;
            }
        }
        while(!wcschr(TYPE_NAME_RESERVED_CHAR, *m_currentItr++));
        
        m_currentItr--;
    }
    else if (identifierType == TypeNameFusionName)
    {       
        while(*m_currentItr != L'\0')
            m_currentItr++;
    }
    else if (identifierType == TypeNameEmbeddedFusionName)
    {
        do
        {   
            if (*m_currentItr == L'\\')
            {
                if (*(m_currentItr + 1) == L']')
                {
                    m_escape.Append(m_currentItr);
                    m_currentItr += 2;
                }
            }
        }
        while(*m_currentItr++ != L']');

        m_currentItr--;
    }
    else 
        return FALSE;
    
    sszId->Set(m_sszTypeName, start, m_currentItr);

    for (SCOUNT_T i = m_escape.GetCount() - 1; i >= 0; i--)
        sszId->Delete(sszId->Begin() + (m_escape[i] - start), 1);

    m_itr = m_currentItr;
    m_nextToken = LexAToken();
    return TRUE;
}

BOOL TypeName::TypeNameParser::START()
{
    WRAPPER_CONTRACT;

    NextToken();
    NextToken();
    return AQN();
}

// FULLNAME ',' ASSEMSPEC
// FULLNAME
// /* empty */
BOOL TypeName::TypeNameParser::AQN()
{
    WRAPPER_CONTRACT;
    IfFailGo(TokenIs(TypeNameAQN));

    if (TokenIs(TypeNameEnd))
        return TRUE;

    IfFailGo(FULLNAME());

    if (TokenIs(TypeNameComma))
    {
        NextToken();
        IfFailGo(ASSEMSPEC());
    }

    IfFailGo(TokenIs(TypeNameEnd));

    return TRUE;
}

// fusionName
BOOL TypeName::TypeNameParser::ASSEMSPEC()
{
    WRAPPER_CONTRACT;
    IfFailGo(TokenIs(TypeNameASSEMSPEC));

    GetIdentifier(m_pTypeName->GetAssembly(), TypeNameFusionName);

    NextToken();
    
    return TRUE;
}

// NAME GENPARAMS QUALIFIER
BOOL TypeName::TypeNameParser::FULLNAME()
{
    WRAPPER_CONTRACT;
    IfFailGo(TokenIs(TypeNameFULLNAME));
    IfFailGo(NAME());

    IfFailGo(GENPARAMS());

    IfFailGo(QUALIFIER());

    return TRUE;
}

// *empty*
// '[' GENARGS ']'        
BOOL TypeName::TypeNameParser::GENPARAMS()
{
    WRAPPER_CONTRACT;

    if (!TokenIs(TypeNameGENPARAM))
        return TRUE;    

    if (!NextTokenIs(TypeNameGENARGS))
        return TRUE;

    NextToken();
    IfFailGo(GENARGS());

    IfFailGo(TokenIs(TypeNameCloseSqBracket));
    NextToken();

    return TRUE;
}

// GENARG
// GENARG ',' GENARGS
BOOL TypeName::TypeNameParser::GENARGS()
{
    WRAPPER_CONTRACT;
    IfFailGo(TokenIs(TypeNameGENARGS));

    IfFailGo(GENARG());

    if (TokenIs(TypeNameComma))
    {
        NextToken();
        IfFailGo(GENARGS());
    }

    return TRUE;
}
        
// '[' EAQN ']'
// FULLNAME
BOOL TypeName::TypeNameParser::GENARG()
{
    WRAPPER_CONTRACT;
    IfFailGo(TokenIs(TypeNameGENARG));

    TypeName* pEnclosingTypeName = m_pTypeName;
    m_pTypeName = m_pTypeName->AddGenericArgument();
    {
        if (TokenIs(TypeNameOpenSqBracket))
        {
            NextToken();
            IfFailGo(EAQN());

            IfFailGo(TokenIs(TypeNameCloseSqBracket));
            NextToken();
        }
        else
        {
            IfFailGo(FULLNAME());
        }
    }
    m_pTypeName = pEnclosingTypeName;

    return TRUE;
}

// FULLNAME ',' EASSEMSPEC
// FULLNAME
BOOL TypeName::TypeNameParser::EAQN()
{
    WRAPPER_CONTRACT;
    IfFailGo(TokenIs(TypeNameEAQN));

    IfFailGo(FULLNAME());

    if (TokenIs(TypeNameComma))
    {
        NextToken();
        IfFailGo(EASSEMSPEC());
    }

    return TRUE;
}

// embeddedFusionName
BOOL TypeName::TypeNameParser::EASSEMSPEC()
{
    WRAPPER_CONTRACT;
    IfFailGo(TokenIs(TypeNameEASSEMSPEC));

    GetIdentifier(m_pTypeName->GetAssembly(), TypeNameEmbeddedFusionName);

    NextToken();
    
    return TRUE;
}

// *empty*
// '&'
// '*' QUALIFIER
// ARRAY QUALIFIER
BOOL TypeName::TypeNameParser::QUALIFIER()
{
    WRAPPER_CONTRACT;

    if (!TokenIs(TypeNameQUALIFIER))
        return TRUE;

    if (TokenIs(TypeNameAmperstand))
    {
        m_pTypeName->SetByRef();

        NextToken();
    }
    else if (TokenIs(TypeNameAstrix))
    {
        m_pTypeName->SetPointer();

        NextToken();
        IfFailGo(QUALIFIER());
    }
    else
    {
        IfFailGo(ARRAY());
        IfFailGo(QUALIFIER());
    }

    return TRUE;
}

// '[' RANK ']'
// '[' '*' ']'
BOOL TypeName::TypeNameParser::ARRAY()
{
    WRAPPER_CONTRACT;
    IfFailGo(TokenIs(TypeNameARRAY));

    NextToken();

    if (TokenIs(TypeNameAstrix))
    {
        m_pTypeName->SetArray(1);
        
        NextToken();
    }
    else
    {
        DWORD dwRank = 1;
        IfFailGo(RANK(&dwRank));

        if (dwRank == 1)
            m_pTypeName->SetSzArray();
        else
            m_pTypeName->SetArray(dwRank);
    }

    IfFailGo(TokenIs(TypeNameCloseSqBracket));
    NextToken();

    return TRUE;
}

// *empty*
// ',' RANK
BOOL TypeName::TypeNameParser::RANK(DWORD* pdwRank)
{
    WRAPPER_CONTRACT;
    
    if (!TokenIs(TypeNameRANK))
        return TRUE;

    NextToken();
    *pdwRank = *pdwRank + 1;
    IfFailGo(RANK(pdwRank));

    return TRUE;
}

// id
// id '+' NESTNAME
BOOL TypeName::TypeNameParser::NAME()
{
    WRAPPER_CONTRACT;
    IfFailGo(TokenIs(TypeNameNAME));

    GetIdentifier(m_pTypeName->AddName(), TypeNameId);

    NextToken();
    if (TokenIs(TypeNamePlus))
    {
        NextToken();
        IfFailGo(NESTNAME());
    }

    return TRUE;
}

// id
// id '+' NESTNAME
BOOL TypeName::TypeNameParser::NESTNAME()
{
    WRAPPER_CONTRACT;
    IfFailGo(TokenIs(TypeNameNESTNAME));

    GetIdentifier(m_pTypeName->AddName(), TypeNameId);

    NextToken();
    if (TokenIs(TypeNamePlus))
    {
        NextToken();
        IfFailGo(NESTNAME());
    }

    return TRUE;
}

//
// TypeName::GetType
//
DomainAssembly* LoadDomainAssembly(SString* psszAssemblySpec, Assembly* pCallersAssembly, BOOL bThrowIfNotFound, BOOL bIntrospectionOnly);





//--------------------------------------------------------------------------------------------------------------
// This version is used for resolving types named in custom attributes such as those used
// for interop. Thus, it follows a well-known multistage set of rules for determining which
// assembly the type is in. It will also enforce that the requesting assembly has access
// rights to the type being loaded.
//
// The search logic is:
//
//    if szTypeName is ASM-qualified, only that assembly will be searched.
//    if szTypeName is not ASM-qualified, we will search for the types in the following order:
//       - in pRequestingAssembly (if not NULL). pRequestingAssembly is the assembly that contained
//         the custom attribute from which the typename was derived.
//       - in mscorlib.dll
//       - raise an AssemblyResolveEvent() in the current appdomain
//
// pRequestingAssembly may be NULL. In that case, the "visibility" check will simply check that
// the loaded type has public access.
//--------------------------------------------------------------------------------------------------------------
/* public static */
TypeHandle TypeName::GetTypeUsingCASearchRules(LPCUTF8 szTypeName, Assembly *pRequestingAssembly, BOOL *pfNameIsAsmQualified/* = NULL*/, BOOL bDoVisibilityChecks/* = TRUE*/)
{
    STATIC_CONTRACT_THROWS;
    STATIC_CONTRACT_GC_TRIGGERS;
    STATIC_CONTRACT_FAULT;

    StackSString sszAssemblyQualifiedName(SString::Utf8, szTypeName);
    return GetTypeUsingCASearchRules(sszAssemblyQualifiedName.GetUnicode(), pRequestingAssembly, pfNameIsAsmQualified, bDoVisibilityChecks);
}

TypeHandle TypeName::GetTypeUsingCASearchRules(LPCWSTR szTypeName, Assembly *pRequestingAssembly, BOOL *pfNameIsAsmQualified/* = NULL*/, BOOL bDoVisibilityChecks/* = TRUE*/)
{
    STATIC_CONTRACT_THROWS;
    STATIC_CONTRACT_GC_TRIGGERS;
    STATIC_CONTRACT_FAULT;

    BOOL bIntrospectionOnly = pRequestingAssembly ? pRequestingAssembly->IsIntrospectionOnly() : FALSE; // classfactory loads are always for execution

    DWORD error = (DWORD)-1;
    
    TypeName typeName(szTypeName, &error);
    
    if (error != (DWORD)-1)
    {
        StackSString buf;
        StackSString msg(L"typeName@");
        COUNT_T size = buf.GetUnicodeAllocation();
        _itow_s(error,buf.OpenUnicodeBuffer(size),size,10);
        buf.CloseBuffer();
        msg.Append(buf);
        COMPlusThrowArgumentException(msg.GetUnicode(), NULL);
    }

    if (pfNameIsAsmQualified)
    {
        *pfNameIsAsmQualified = TRUE;
        if (typeName.GetAssembly()->IsEmpty())
            *pfNameIsAsmQualified = FALSE;
    }
    
    TypeHandle th = typeName.GetTypeWorker(
        /*bThrowIfNotFound = */ TRUE, 
        /*bIgnoreCase = */ FALSE, 
        bIntrospectionOnly, 
        /*pAssemblyGetType =*/ NULL, 
        /*fEnableCASearchRules = */ TRUE, 
        /*fProhibitAsmQualifiedName = */ FALSE, 
        NULL, 
        pRequestingAssembly, 
        FALSE);        

    ASSERT(!th.IsNull());
    return th;
}






//--------------------------------------------------------------------------------------------------------------
// This everything-but-the-kitchen-sink version is what used to be called "GetType()". It exposes all the
// funky knobs needed for implementing the specific requirements of the managed Type.GetType() apis and friends.
//--------------------------------------------------------------------------------------------------------------
/*public static */ TypeHandle TypeName::GetTypeManaged(
    LPCWSTR szTypeName, 
    DomainAssembly* pAssemblyGetType, 
    BOOL bThrowIfNotFound, 
    BOOL bIgnoreCase, 
    BOOL bIntrospectionOnly, 
    BOOL bProhibitAsmQualifiedName,
    StackCrawlMark* pStackMark, 
    BOOL bLoadTypeFromPartialNameHack)
{
    WRAPPER_CONTRACT;

    if (!*szTypeName)
      COMPlusThrow(kArgumentException, L"Format_StringZeroLength");

    DWORD error = (DWORD)-1;
    
    TypeName typeName(szTypeName, &error);

    if (error != (DWORD)-1)
    {
        if (!bThrowIfNotFound)
            return TypeHandle();
            
        StackSString buf;
        StackSString msg(L"typeName@");
        COUNT_T size = buf.GetUnicodeAllocation();
        _itow_s(error,buf.OpenUnicodeBuffer(size),size,10);
        buf.CloseBuffer();
        msg.Append(buf);
        COMPlusThrowArgumentException(msg.GetUnicode(), NULL);
    }

    BOOL bPeriodPrefix = szTypeName[0] == L'.';
    
    TypeHandle result = typeName.GetTypeWorker(
        bPeriodPrefix ? FALSE : bThrowIfNotFound, 
        bIgnoreCase, 
        bIntrospectionOnly, 
        pAssemblyGetType ? pAssemblyGetType->GetAssembly() : NULL, 
        /*fEnableCASearchRules = */TRUE, 
        bProhibitAsmQualifiedName, 
        pStackMark, 
        NULL, 
        bLoadTypeFromPartialNameHack);      

    if (bPeriodPrefix && result.IsNull())
    {
        new (&typeName) TypeName(szTypeName + 1, &error);

        if (error != (DWORD)-1)
        {
            if (!bThrowIfNotFound)
                return TypeHandle();
                
            StackSString buf;
            StackSString msg(L"typeName@");
            COUNT_T size = buf.GetUnicodeAllocation();
            _itow_s(error-1,buf.OpenUnicodeBuffer(size),size,10);
            buf.CloseBuffer();
            msg.Append(buf);
            COMPlusThrowArgumentException(msg.GetUnicode(), NULL);
        }
        
        result = typeName.GetTypeWorker(
            bThrowIfNotFound, 
            bIgnoreCase, 
            bIntrospectionOnly, 
            pAssemblyGetType ? pAssemblyGetType->GetAssembly() : NULL, 
            /*fEnableCASearchRules = */TRUE, 
            bProhibitAsmQualifiedName, 
            pStackMark, 
            NULL, 
            bLoadTypeFromPartialNameHack);      
    }

    return result;
}




//-------------------------------------------------------------------------------------------
// Retrieves a type from an assembly. It requires the caller to know which assembly
// the type is in.
//-------------------------------------------------------------------------------------------
/* public static */ TypeHandle TypeName::GetTypeFromAssembly(LPCWSTR szTypeName, Assembly *pAssembly)
{
    STATIC_CONTRACT_THROWS;
    STATIC_CONTRACT_GC_TRIGGERS;
    STATIC_CONTRACT_FAULT;

    _ASSERTE(szTypeName != NULL);
    _ASSERTE(pAssembly != NULL);

    if (!*szTypeName)
      COMPlusThrow(kArgumentException, L"Format_StringZeroLength");

    DWORD error = (DWORD)-1;
    
    TypeName typeName(szTypeName, &error);
    
    if (error != (DWORD)-1)
    {
        StackSString buf;
        StackSString msg(L"typeName@");
        COUNT_T size = buf.GetUnicodeAllocation();
        _itow_s(error,buf.OpenUnicodeBuffer(size),size,10);
        buf.CloseBuffer();
        msg.Append(buf);
        COMPlusThrowArgumentException(msg.GetUnicode(), NULL);
    }

    // Because the typename can come from untrusted input, we will throw an exception rather than assert.
    // (This also assures that the shipping build does the right thing.)
    if (!(typeName.GetAssembly()->IsEmpty()))
    {
        COMPlusThrow(kArgumentException, IDS_EE_CANNOT_HAVE_ASSEMBLY_SPEC);
    }

    return typeName.GetTypeWorker(/*bThrowIfNotFound =*/TRUE, /*bIgnoreCase = */FALSE, pAssembly->IsIntrospectionOnly(), pAssembly, /*fEnableCASearchRules = */FALSE, FALSE, NULL, NULL, FALSE);


    

}

//-------------------------------------------------------------------------------------------
// Retrieves a type. Will assert if the name is not fully qualified.
//-------------------------------------------------------------------------------------------
/* public static */ TypeHandle TypeName::GetTypeFromAsmQualifiedName(LPCWSTR szFullyQualifiedName, BOOL bForIntrospection)
{
    STATIC_CONTRACT_THROWS;
    STATIC_CONTRACT_GC_TRIGGERS;
    STATIC_CONTRACT_FAULT;

    _ASSERTE(szFullyQualifiedName != NULL);

    if (!*szFullyQualifiedName)
      COMPlusThrow(kArgumentException, L"Format_StringZeroLength");

    DWORD error = (DWORD)-1;
    
    TypeName typeName(szFullyQualifiedName, &error);
    
    if (error != (DWORD)-1)
    {
        StackSString buf;
        StackSString msg(L"typeName@");
        COUNT_T size = buf.GetUnicodeAllocation();
        _itow_s(error,buf.OpenUnicodeBuffer(size),size,10);
        buf.CloseBuffer();
        msg.Append(buf);
        COMPlusThrowArgumentException(msg.GetUnicode(), NULL);
    }

    // Because the typename can come from untrusted input, we will throw an exception rather than assert.
    // (This also assures that the shipping build does the right thing.)
    if (typeName.GetAssembly()->IsEmpty())
    {
        COMPlusThrow(kArgumentException, IDS_EE_NEEDS_ASSEMBLY_SPEC);
    }

    return typeName.GetTypeWorker(/*bThrowIfNotFound =*/TRUE, /*bIgnoreCase = */FALSE, bForIntrospection, NULL, /*fEnableCASearchRules = */FALSE, FALSE, NULL, NULL, FALSE);


}



// -------------------------------------------------------------------------------------------------------------
// This is the "uber" GetType() that all public GetType() funnels through. It's main job is to figure out which
// Assembly to load the type from and then invoke GetTypeHaveAssembly.
//
// It's got a highly baroque interface partly for historical reasons and partly because it's the uber-function
// for all of the possible GetTypes.
// -------------------------------------------------------------------------------------------------------------
/* private instance */ TypeHandle TypeName::GetTypeWorker(
    BOOL bThrowIfNotFound, 
    BOOL bIgnoreCase, 
    BOOL bIntrospectionOnly, 
    Assembly* pAssemblyGetType,

    BOOL fEnableCASearchRules,
    BOOL bProhibitAsmQualifiedName,
    StackCrawlMark* pStackMark, 
    Assembly* pRequestingAssembly, 
    BOOL bLoadTypeFromPartialNameHack)
{
    WRAPPER_CONTRACT;

    TypeHandle th = TypeHandle();

    //requires a lot of space
    DECLARE_INTERIOR_STACK_PROBE;
    if (ShouldProbeOnThisThread())
    {
        // This function is recursive, so it must have an interior probe
        if (bThrowIfNotFound)
        {
            DO_INTERIOR_STACK_PROBE_FOR(GetThread(),12);
        }
        else
        {
            DO_INTERIOR_STACK_PROBE_FOR_NOTHROW(GetThread(),12, goto Exit;);
        }
    }

    // An explicit assembly has been specified so look for the type there
    if (!GetAssembly()->IsEmpty()) 
    {
        
        if (bProhibitAsmQualifiedName && !m_bIsGenericArgument)
        {
            if (bThrowIfNotFound)
            {
                COMPlusThrow(kArgumentException, IDS_EE_ASSEMBLY_GETTYPE_CANNONT_HAVE_ASSEMBLY_SPEC);
            }
            else
            {
                th = TypeHandle();
                goto Exit;
            }
        }

        if (!pRequestingAssembly && pStackMark)
            pRequestingAssembly = SystemDomain::GetCallersAssembly(pStackMark);

        // We want to catch the exception if we're going to later try a partial bind.
        if (bLoadTypeFromPartialNameHack)
        {
            EX_TRY
            {
                DomainAssembly *pDomainAssembly = LoadDomainAssembly(GetAssembly(), pRequestingAssembly, bThrowIfNotFound, bIntrospectionOnly);
                if (pDomainAssembly)
                {
                    th = GetTypeHaveAssembly(pDomainAssembly->GetAssembly(), bThrowIfNotFound, bIgnoreCase);
                }
            }
            EX_CATCH
            {
                th = TypeHandle();
            }
            EX_END_CATCH(RethrowTransientExceptions);
        }
        else
        {
            DomainAssembly *pDomainAssembly = LoadDomainAssembly(GetAssembly(), pRequestingAssembly, bThrowIfNotFound, bIntrospectionOnly);
            if (pDomainAssembly)
            {
                th = GetTypeHaveAssembly(pDomainAssembly->GetAssembly(), bThrowIfNotFound, bIgnoreCase);
            }
        }
    }

    // There's no explicit assembly so look in the assembly specified by the original caller (Assembly.GetType)
    else if (pAssemblyGetType) 
    {
        th = GetTypeHaveAssembly(pAssemblyGetType, bThrowIfNotFound, bIgnoreCase);
    }
    
    // Otherwise look in the caller's assembly then the system assembly
    else if (fEnableCASearchRules)
    {
        if (bIntrospectionOnly)
        {
            if (pStackMark != NULL) // This is our test to see if we're being because of a managed api or because we are parsing a CA. 
            {
                COMPlusThrow(kArgumentException, IDS_EE_REFLECTIONONLYGETTYPE_NOASSEMBLY);
            }
        }

        if (!pRequestingAssembly && pStackMark)
            pRequestingAssembly = SystemDomain::GetCallersAssembly(pStackMark); 
        
        // Look for type in caller's assembly
        if (pRequestingAssembly)
            th = GetTypeHaveAssembly(pRequestingAssembly, bThrowIfNotFound, bIgnoreCase);
        
        // Look for type in system assembly
        if (th.IsNull())
        {
            if (pRequestingAssembly != SystemDomain::SystemAssembly())
                th = GetTypeHaveAssembly(SystemDomain::SystemAssembly(), bThrowIfNotFound, bIgnoreCase);
        }
        
        // Raise AssemblyResolveEvent to try to resolve assembly
        if (th.IsNull() && !bIntrospectionOnly) 
        {
            AppDomain *pDomain = (AppDomain*)SystemDomain::GetCurrentDomain();

            if ((BaseDomain*)pDomain != SystemDomain::System())
            {
                TypeNameBuilder tnb;   
                for (COUNT_T i = 0; i < GetNames().GetCount(); i ++)
                    tnb.AddName(GetNames()[i]->GetUnicode());
                                
                StackScratchBuffer bufFullName;
                DomainAssembly* pDomainAssembly = pDomain->RaiseTypeResolveEventThrowing(tnb.GetString()->GetANSI(bufFullName));
                if (pDomainAssembly)
                    th = GetTypeHaveAssembly(pDomainAssembly->GetAssembly(), bThrowIfNotFound, bIgnoreCase);
            }            
        }            
    }
    else
    {
        _ASSERTE(!"You must pass either a asm-qualified typename or an actual Assembly.");
    }

    if (th.IsNull() && bLoadTypeFromPartialNameHack && GetAssembly() && !GetAssembly()->IsEmpty())
    {
        DomainAssembly* pPartialBindAssemblyHack = LoadAssemblyFromPartialNameHack(GetAssembly());

        if (pPartialBindAssemblyHack)
            th = GetTypeHaveAssembly(pPartialBindAssemblyHack->GetAssembly(), bThrowIfNotFound, bIgnoreCase);
    }

    if (!th.IsNull() && (!m_genericArguments.IsEmpty() || !m_signature.IsEmpty()))
    {
        // Instantiate generic arguments
        InlineSArray<TypeHandle, 32> genericArguments;
        for (COUNT_T i = 0; i < m_genericArguments.GetCount(); i++)
            genericArguments.Append(m_genericArguments[i]->GetTypeWorker(
                bThrowIfNotFound, bIgnoreCase, bIntrospectionOnly, 
                pAssemblyGetType, fEnableCASearchRules, bProhibitAsmQualifiedName, pStackMark, pRequestingAssembly, 
                bLoadTypeFromPartialNameHack));
        
        MethodDescCallSite getTypeHelper(METHOD__RT_TYPE_HANDLE__GET_TYPE_HELPER);   
        
        ARG_SLOT args[5] = {
            (ARG_SLOT)th.AsTAddr(),
            (ARG_SLOT)(SIZE_T)genericArguments.OpenRawBuffer(),
            genericArguments.GetCount(),
            (ARG_SLOT)(SIZE_T)m_signature.OpenRawBuffer(),
            m_signature.GetCount(),
        };

        th = TypeHandle::FromData(getTypeHelper.Call_RetArgSlot(args));
    }
    
    if (th.IsNull() && bThrowIfNotFound) 
    {
        StackSString buf;
        LPCWSTR wszName = ToString(&buf)->GetUnicode();
        MAKE_UTF8PTR_FROMWIDE(szName, wszName);

        if (GetAssembly() && !GetAssembly()->IsEmpty())
        {
            ThrowTypeLoadException(NULL, szName, GetAssembly()->GetUnicode(), NULL, IDS_CLASSLOAD_GENERAL);
        }        
        else if (pAssemblyGetType) 
        {
            pAssemblyGetType->ThrowTypeLoadException(NULL, szName, IDS_CLASSLOAD_GENERAL);
        }
        else if (pRequestingAssembly)
        {
            pRequestingAssembly->ThrowTypeLoadException(NULL, szName, IDS_CLASSLOAD_GENERAL);
        }
        else
        {
            ThrowTypeLoadException(NULL, szName, NULL, NULL, IDS_CLASSLOAD_GENERAL);
        }
    }

Exit:
    ;
    END_INTERIOR_STACK_PROBE;
    
    return th;
}

//----------------------------------------------------------------------------------------------------------------
// This is the one that actually loads the type once we've pinned down the Assembly it's in.
//----------------------------------------------------------------------------------------------------------------
/* private instance */ TypeHandle TypeName::GetTypeHaveAssembly(Assembly* pAssembly, BOOL bThrowIfNotFound, BOOL bIgnoreCase, BOOL bRecurse)
{
    WRAPPER_CONTRACT;
    
    TypeHandle th = TypeHandle();
    SArray<SString*> &names = GetNames();
    Module* pManifestModule = pAssembly->GetManifestModule();

    NameHandle typeName(pManifestModule, mdtBaseType);
    
    // Set up the name handle
    if(bIgnoreCase)
        typeName.SetCaseInsensitive();

    EX_TRY
    {
        for (COUNT_T i = 0; i < names.GetCount(); i ++)
        {
            LPCWSTR wname = names[i]->GetUnicode();
            MAKE_UTF8PTR_FROMWIDE(name, wname);
            typeName.SetName(name);       
            th = pAssembly->GetLoader()->LoadTypeHandleThrowing(&typeName);
        }

        if (th.IsNull() && bRecurse)
        {
            IMDInternalImport* pManifestImport = pManifestModule->GetMDImport();
            HENUMInternalHolder phEnum(pManifestImport);
            phEnum.EnumInit(mdtFile, mdTokenNil);
            mdToken mdFile;
        
            while (pManifestImport->EnumNext(&phEnum, &mdFile)) 
            {
                if (pManifestModule->LookupFile(mdFile))
                    continue;
                
                pManifestModule->LoadModule(GetAppDomain(), mdFile, FALSE);

                th = GetTypeHaveAssembly(pAssembly, bThrowIfNotFound, bIgnoreCase, FALSE);
                
                if (!th.IsNull())
                    break;            
            }
        }
    }
    EX_CATCH
    {
        if (bThrowIfNotFound)
            EX_RETHROW;
    
        Exception *ex = GET_EXCEPTION();
    
        // Let non-File-not-found exceptions propagate
        if (EEFileLoadException::GetFileLoadKind(ex->GetHR()) != kFileNotFoundException)
            EX_RETHROW;
    }
    EX_END_CATCH(RethrowTerminalExceptions);

    return th;
}

DomainAssembly* LoadAssemblyFromPartialNameHack(SString* psszAssemblySpec, BOOL fCropPublicKey)
{
    CONTRACTL
    {
        MODE_ANY;
        THROWS;
        GC_TRIGGERS;
        INJECT_FAULT(COMPlusThrowOM(););
    }
    CONTRACTL_END;
    
    MethodDescCallSite loadWithPartialNameHack(METHOD__ASSEMBLY__LOAD_WITH_PARTIAL_NAME_HACK);   
    ARG_SLOT args[2];
    STRINGREF mszAssembly = NULL;
    DomainAssembly* pPartialBindAssemblyHack = NULL;
    GCPROTECT_BEGIN(mszAssembly);
    {
        mszAssembly = COMString::NewString(psszAssemblySpec->GetUnicode());
        args[0] = ObjToArgSlot(mszAssembly);
        args[1] = (ARG_SLOT)(CLR_BOOL)!!fCropPublicKey;
        
        pPartialBindAssemblyHack = (DomainAssembly*) loadWithPartialNameHack.Call_RetLPVOID(args);
    }
    GCPROTECT_END();

    return pPartialBindAssemblyHack;
}

DomainAssembly* LoadDomainAssembly(SString* psszAssemblySpec, Assembly* pRequestingAssembly, BOOL bThrowIfNotFound, BOOL bIntrospectionOnly)
{
    CONTRACTL
    {
        MODE_ANY;
        THROWS;
        GC_TRIGGERS;
        INJECT_FAULT(COMPlusThrowOM(););
    }
    CONTRACTL_END;
        
    AssemblySpec spec;
    DomainAssembly *pDomainAssembly = NULL;

    if (bIntrospectionOnly)
        spec.SetIntrospectionOnly(TRUE);

    StackScratchBuffer buffer;
    LPCUTF8 szAssemblySpec = psszAssemblySpec->GetUTF8(buffer);
    IfFailThrow(spec.Init(szAssemblySpec));

    if (pRequestingAssembly && (!pRequestingAssembly->IsDomainNeutral())) 
    {
        GCX_PREEMP();
        spec.GetCodeBase()->SetParentAssembly(pRequestingAssembly->GetFusionAssembly());
    }
    
    if (bThrowIfNotFound)
    {
        pDomainAssembly = spec.LoadDomainAssembly(FILE_LOADED);
    }
    else
    {
        EX_TRY
        {
            pDomainAssembly = spec.LoadDomainAssembly(FILE_LOADED);
        }
        EX_CATCH
        {
            Exception *ex = GET_EXCEPTION();

            // Let non-File-not-found execeptions propagate
            if (EEFileLoadException::GetFileLoadKind(ex->GetHR()) != kFileNotFoundException)
                EX_RETHROW;
        }
        EX_END_CATCH(RethrowTerminalExceptions);
    }
        
    return pDomainAssembly;
}


