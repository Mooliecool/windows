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
//*****************************************************************************
// File: daccess.cpp
//
// ClrDataAccess implementation.
//
//*****************************************************************************

#include "stdafx.h"
#include <clrdata.h>
#include "typestring.h"
#include "holder.h"
#include "debuginfostore.h"
#include "peimagelayout.inl"


// XXX drewb - Not sure why this isn't in a header somewhere.
STDAPI  GetMDInternalInterface(
    LPVOID      pData,
    ULONG       cbData,
    DWORD       flags,                  // [IN] MDInternal_OpenForRead or MDInternal_OpenForENC
    REFIID      riid,                   // [in] The interface desired.
    void        **ppIUnk);              // [out] Return interface on success.

CRITICAL_SECTION g_dacCritSec;
ClrDataAccess* g_dacImpl;
HINSTANCE g_thisModule;

static const GUID IID_ICorDataAccess_Obsolete =
{
    0x6222a81f, 0x3aab, 0x4926,
    { 0xa5, 0x83, 0x84, 0x95, 0x74, 0x35, 0x23, 0xfb }
};

class ICorDataAccessRedirect
{
public:
    ICorDataAccessRedirect(IXCLRDataProcess* redir)
    {
        m_redir = redir;
    };

    STDMETHOD(QueryInterface)(
        IN REFIID InterfaceId,
        OUT PVOID* Interface
        )
    {
        return m_redir->QueryInterface(InterfaceId, Interface);
    }
    STDMETHOD_(ULONG, AddRef)(
        THIS
        )
    {
        return 1;
    }
    STDMETHOD_(ULONG, Release)(
        THIS
        )
    {
        return 0;
    }

    STDMETHOD(Flush)(void)
    {
        return m_redir->Flush();
    }
    STDMETHOD(IsCorCode)(CLRDATA_ADDRESS address)
    {
        return E_NOTIMPL;
    }
    STDMETHOD(GetILOffsetFromTargetAddress)(CLRDATA_ADDRESS address,
                                            CLRDATA_ADDRESS* moduleBase,
                                            mdMethodDef* methodDef,
                                            ULONG32* offset)
    {
        return E_NOTIMPL;
    }
    STDMETHOD(GetCodeSymbolForTargetAddress)(CLRDATA_ADDRESS address,
                                             LPWSTR symbol,
                                             ULONG32 symbolChars,
                                             CLRDATA_ADDRESS* displacement)
    {
        return E_NOTIMPL;
    }
    STDMETHOD(StartStackWalk)(ULONG32 corThreadId,
                              int flags,
                              PVOID walk)
    {
        return E_NOTIMPL;
    }
    STDMETHOD(EnumMemoryRegions)(PVOID callback,
                                 int flags)
    {
        return E_NOTIMPL;
    }
    STDMETHOD(Request)(ULONG32 reqCode,
                       BYTE* inBuffer,
                       ULONG32 outBufferSize,
                       BYTE* outBuffer)
    {
        return E_NOTIMPL;
    }

private:
    IXCLRDataProcess* m_redir;
};



EXTERN_C BOOL WINAPI
DllMain(HANDLE instance, DWORD reason, LPVOID reserved)
{
    switch(reason)
    {
    case DLL_PROCESS_ATTACH:
        InitializeCriticalSection(&g_dacCritSec);
        // Save the module handle.
        g_thisModule = (HINSTANCE)instance;
        break;
    case DLL_PROCESS_DETACH:
        DeleteCriticalSection(&g_dacCritSec);
        break;
    }

    return TRUE;
}




HINSTANCE
GetModuleInst(void)
{
    return g_thisModule;
}

HRESULT
ConvertUtf8(__in LPCUTF8 utf8,
            ULONG32 bufLen,
            ULONG32* nameLen,
            __inout_ecount_part_opt(bufLen, nameLen) PWSTR buffer)
{
    if (nameLen)
    {
        *nameLen = WszMultiByteToWideChar(CP_UTF8, 0, utf8, -1, NULL, 0);
        if (!*nameLen)
        {
            return HRESULT_FROM_GetLastError();
        }
    }

    if (buffer && bufLen)
    {
        if (!WszMultiByteToWideChar(CP_UTF8, 0, utf8, -1, buffer, bufLen))
        {
            return HRESULT_FROM_GetLastError();
        }
    }

    return S_OK;
}

HRESULT
AllocUtf8(__in_opt LPCWSTR wstr,
          ULONG32 srcChars,
          __deref_out LPUTF8* utf8)
{
    ULONG32 chars = WszWideCharToMultiByte(CP_UTF8, 0, wstr, srcChars,
                                           NULL, 0, NULL, NULL);
    if (!chars)
    {
        return HRESULT_FROM_GetLastError();
    }

    // Make sure the converted string is always terminated.
    if (srcChars != (ULONG32)-1)
    {
        chars++;
    }

    char* mem = new (nothrow) char[chars];
    if (!mem)
    {
        return E_OUTOFMEMORY;
    }

    if (!WszWideCharToMultiByte(CP_UTF8, 0, wstr, srcChars,
                                mem, chars, NULL, NULL))
    {
        delete [] mem;
        return HRESULT_FROM_GetLastError();
    }

    if (srcChars != (ULONG32)-1)
    {
        mem[chars - 1] = 0;
    }

    *utf8 = mem;
    return S_OK;
}

HRESULT
GetFullClassNameFromMetadata(IMDInternalImport* mdImport,
                             mdTypeDef classToken,
                             ULONG32 bufferChars,
                             __inout_ecount(bufferChars) LPUTF8 buffer)
{
    LPCUTF8 baseName, namespaceName;

    mdImport->GetNameOfTypeDef(classToken, &baseName, &namespaceName);
    return ns::MakePath(buffer, bufferChars, namespaceName, baseName) ?
        S_OK : E_OUTOFMEMORY;
}

HRESULT
GetFullMethodNameFromMetadata(IMDInternalImport* mdImport,
                              mdMethodDef methodToken,
                              ULONG32 bufferChars,
                              __inout_ecount(bufferChars) LPUTF8 buffer)
{
    HRESULT status;
    mdTypeDef classToken;
    ULONG32 len;

    if (mdImport->GetParentToken(methodToken, &classToken) == S_OK)
    {
        if ((status =
             GetFullClassNameFromMetadata(mdImport, classToken,
                                          bufferChars, buffer)) != S_OK)
        {
            return status;
        }

        len = strlen(buffer);
        buffer += len;
        bufferChars -= len + 1;

        if (!bufferChars)
        {
            return E_OUTOFMEMORY;
        }

        *buffer++ = NAMESPACE_SEPARATOR_CHAR;
    }

    LPCUTF8 methodName = mdImport->GetNameOfMethodDef(methodToken);
    len = strlen(methodName);
    if (len >= bufferChars)
    {
        return E_OUTOFMEMORY;
    }

    strcpy_s(buffer, bufferChars, methodName);
    return S_OK;
}

HRESULT
SplitFullName(__in_z __in PCWSTR fullName,
              SplitSyntax syntax,
              ULONG32 memberDots,
              __deref_out_opt LPUTF8* namespaceName,
              __deref_out_opt LPUTF8* typeName,
              __deref_out_opt LPUTF8* memberName,
              __deref_out_opt LPUTF8* params)
{
    HRESULT status;
    PCWSTR paramsStart, memberStart, memberEnd, typeStart;

    if (!*fullName)
    {
        return E_INVALIDARG;
    }

    //
    // Split off parameters.
    //

    paramsStart = wcschr(fullName, L'(');
    if (paramsStart)
    {
        if (syntax != SPLIT_METHOD ||
            paramsStart == fullName)
        {
            return E_INVALIDARG;
        }

        if ((status = AllocUtf8(paramsStart, (ULONG32)-1, params)) != S_OK)
        {
            return status;
        }

        memberEnd = paramsStart - 1;
    }
    else
    {
        *params = NULL;
        memberEnd = fullName + (wcslen(fullName) - 1);
    }

    if (syntax != SPLIT_TYPE)
    {
        //
        // Split off member name.
        //

        memberStart = memberEnd;

        for (;;)
        {
            while (memberStart >= fullName &&
                   *memberStart != L'.')
            {
                memberStart--;
            }

            // Some member names (e.g. .ctor and .dtor) have
            // dots, so go back to the first dot.
            while (memberStart > fullName &&
                   memberStart[-1] == L'.')
            {
                memberStart--;
            }

            if (memberStart <= fullName)
            {
                if (memberDots > 0)
                {
                    // Caller expected dots in the
                    // member name and they weren't found.
                    status = E_INVALIDARG;
                    goto DelParams;
                }

                break;
            }
            else if (memberDots == 0)
            {
                break;
            }

            memberStart--;
            memberDots--;
        }

        memberStart++;
        if (memberStart > memberEnd)
        {
            status = E_INVALIDARG;
            goto DelParams;
        }

        if ((status = AllocUtf8(memberStart, (ULONG32)
                                (memberEnd - memberStart) + 1,
                                memberName)) != S_OK)
        {
            goto DelParams;
        }
    }
    else
    {
        *memberName = NULL;
        memberStart = memberEnd + 2;
    }

    //
    // Split off type name.
    //

    if (memberStart > fullName)
    {
        // Must have at least one character for the type
        // name.  If there was a member name, there must
        // also be a separator.
        if (memberStart < fullName + 2)
        {
            status = E_INVALIDARG;
            goto DelMember;
        }

        typeStart = memberStart - 2;
        while (typeStart >= fullName &&
               *typeStart != L'.')
        {
            typeStart--;
        }
        typeStart++;

        if ((status = AllocUtf8(typeStart, (ULONG32)
                                (memberStart - typeStart) - 1,
                                typeName)) != S_OK)
        {
            goto DelMember;
        }
    }
    else
    {
        *typeName = NULL;
        typeStart = fullName;
    }

    //
    // Namespace must be the rest.
    //

    if (typeStart > fullName)
    {
        if ((status = AllocUtf8(fullName, (ULONG32)
                                (typeStart - fullName) - 1,
                                namespaceName)) != S_OK)
        {
            goto DelType;
        }
    }
    else
    {
        *namespaceName = NULL;
    }

    return S_OK;

 DelType:
    delete [] (*typeName);
 DelMember:
    delete [] (*memberName);
 DelParams:
    delete [] (*params);
    return status;
}

int
CompareUtf8(__in LPCUTF8 str1, __in LPCUTF8 str2, __in ULONG32 nameFlags)
{
    if (nameFlags & CLRDATA_BYNAME_CASE_INSENSITIVE)
    {
        // XXX drewb - Convert to Unicode?
        return SString::_stricmp(str1, str2);
    }

    return strcmp(str1, str2);
}

//----------------------------------------------------------------------------
//
// MetaEnum.
//
//----------------------------------------------------------------------------

HRESULT
MetaEnum::Start(IMDInternalImport* mdImport, ULONG32 kind,
                mdToken container)
{
    HRESULT status;

    switch(kind)
    {
    case mdtTypeDef:
        status = mdImport->EnumTypeDefInit(&m_enum);
        break;
    case mdtMethodDef:
    case mdtFieldDef:
        status = mdImport->EnumInit(kind, container, &m_enum);
        break;
    default:
        return E_INVALIDARG;
    }
    if (status != S_OK)
    {
        return status;
    }

    m_mdImport = mdImport;
    m_kind = kind;

    return S_OK;
}

void
MetaEnum::End(void)
{
    if (!m_mdImport)
    {
        return;
    }

    switch(m_kind)
    {
    case mdtTypeDef:
        m_mdImport->EnumTypeDefClose(&m_enum);
        break;
    case mdtMethodDef:
    case mdtFieldDef:
        m_mdImport->EnumClose(&m_enum);
        break;
    }

    Clear();
}

HRESULT
MetaEnum::NextToken(mdToken* token,
                    __deref_opt_out_opt LPCUTF8* namespaceName,
                    __deref_opt_out_opt LPCUTF8* name)
{
    if (!m_mdImport)
    {
        return E_INVALIDARG;
    }

    switch(m_kind)
    {
    case mdtTypeDef:
        if (!m_mdImport->EnumTypeDefNext(&m_enum, token))
        {
            return S_FALSE;
        }
        m_lastToken = *token;
        if (namespaceName || name)
        {
            LPCSTR _name, _namespaceName;

            m_mdImport->GetNameOfTypeDef(*token, &_name, &_namespaceName);
            if (namespaceName)
            {
                *namespaceName = _namespaceName;
            }
            if (name)
            {
                *name = _name;
            }
        }
        return S_OK;

    case mdtMethodDef:
        if (!m_mdImport->EnumNext(&m_enum, token))
        {
            return S_FALSE;
        }
        m_lastToken = *token;
        if (namespaceName)
        {
            *namespaceName = NULL;
        }
        if (name)
        {
            *name = m_mdImport->GetNameOfMethodDef(*token);
        }
        return S_OK;

    case mdtFieldDef:
        if (!m_mdImport->EnumNext(&m_enum, token))
        {
            return S_FALSE;
        }
        m_lastToken = *token;
        if (namespaceName)
        {
            *namespaceName = NULL;
        }
        if (name)
        {
            *name = m_mdImport->GetNameOfFieldDef(*token);
        }
        return S_OK;

    default:
        return E_INVALIDARG;
    }
}

HRESULT
MetaEnum::NextDomainToken(AppDomain** appDomain,
                          mdToken* token)
{
    HRESULT status;

    if (m_appDomain)
    {
        // Use only the caller-provided app domain.
        *appDomain = m_appDomain;
        return NextToken(token, NULL, NULL);
    }

    //
    // Splay tokens across all app domains.
    //

    for (;;)
    {
        if (m_lastToken == mdTokenNil)
        {
            // Need to fetch a token.
            if ((status = NextToken(token, NULL, NULL)) != S_OK)
            {
                return status;
            }

            m_domainIter.Init(FALSE);
        }

        if (m_domainIter.Next())
        {
            break;
        }

        m_lastToken = mdTokenNil;
    }

    *appDomain = m_domainIter.GetDomain();
    *token = m_lastToken;

    return S_OK;
}

HRESULT
MetaEnum::NextTokenByName(__in_opt LPCUTF8 namespaceName,
                          __in_opt LPCUTF8 name,
                          ULONG32 nameFlags,
                          mdToken* token)
{
    HRESULT status;
    LPCUTF8 tokNamespace, tokName;

    for (;;)
    {
        if ((status = NextToken(token, &tokNamespace, &tokName)) != S_OK)
        {
            return status;
        }

        if (namespaceName &&
            (!tokNamespace ||
             CompareUtf8(namespaceName, tokNamespace, nameFlags) != 0))
        {
            continue;
        }
        if (name &&
            (!tokName ||
             CompareUtf8(name, tokName, nameFlags) != 0))
        {
            continue;
        }

        return S_OK;
    }
}

HRESULT
MetaEnum::NextDomainTokenByName(__in_opt LPCUTF8 namespaceName,
                                __in_opt LPCUTF8 name,
                                ULONG32 nameFlags,
                                AppDomain** appDomain, mdToken* token)
{
    HRESULT status;

    if (m_appDomain)
    {
        // Use only the caller-provided app domain.
        *appDomain = m_appDomain;
        return NextTokenByName(namespaceName, name, nameFlags, token);
    }

    //
    // Splay tokens across all app domains.
    //

    for (;;)
    {
        if (m_lastToken == mdTokenNil)
        {
            // Need to fetch a token.
            if ((status = NextTokenByName(namespaceName, name, nameFlags,
                                          token)) != S_OK)
            {
                return status;
            }

            m_domainIter.Init(FALSE);
        }

        if (m_domainIter.Next())
        {
            break;
        }

        m_lastToken = mdTokenNil;
    }

    *appDomain = m_domainIter.GetDomain();
    *token = m_lastToken;

    return S_OK;
}

HRESULT
MetaEnum::New(Module* mod,
              ULONG32 kind,
              mdToken container,
              IXCLRDataAppDomain* pubAppDomain,
              MetaEnum** metaEnumRet,
              CLRDATA_ENUM* handle)
{
    HRESULT status;
    MetaEnum* metaEnum;

    if (handle)
    {
        *handle = TO_CDENUM(NULL);
    }

    if (!mod->GetFile()->HasMetadata())
    {
        return S_FALSE;
    }

    metaEnum = new (nothrow) MetaEnum;
    if (!metaEnum)
    {
        return E_OUTOFMEMORY;
    }

    if ((status = metaEnum->
         Start(mod->GetMDImport(), kind, container)) != S_OK)
    {
        delete metaEnum;
        return status;
    }

    if (pubAppDomain)
    {
        metaEnum->m_appDomain =
            ((ClrDataAppDomain*)pubAppDomain)->GetAppDomain();
    }

    if (metaEnumRet)
    {
        *metaEnumRet = metaEnum;
    }
    if (handle)
    {
        *handle = TO_CDENUM(metaEnum);
    }
    return S_OK;
}

//----------------------------------------------------------------------------
//
// SplitName
//
//----------------------------------------------------------------------------

SplitName::SplitName(SplitSyntax syntax, ULONG32 nameFlags,
                     ULONG32 memberDots)
{
    m_syntax = syntax;
    m_nameFlags = nameFlags;
    m_memberDots = memberDots;

    Clear();
}

void
SplitName::Delete(void)
{
    delete [] m_namespaceName;
    m_namespaceName = NULL;
    delete [] m_typeName;
    m_typeName = NULL;
    delete [] m_memberName;
    m_memberName = NULL;
    delete [] m_params;
    m_params = NULL;
}

void
SplitName::Clear(void)
{
    m_namespaceName = NULL;
    m_typeName = NULL;
    m_typeToken = mdTypeDefNil;
    m_memberName = NULL;
    m_memberToken = mdTokenNil;
    m_params = NULL;

    m_tlsThread = NULL;
    m_metaEnum.m_appDomain = NULL;
    m_module = NULL;
    m_lastField = NULL;
}

HRESULT
SplitName::SplitString(__in_opt PCWSTR fullName)
{
    if (m_syntax == SPLIT_NO_NAME)
    {
        if (fullName)
        {
            return E_INVALIDARG;
        }

        return S_OK;
    }

    return SplitFullName(fullName,
                         m_syntax,
                         m_memberDots,
                         &m_namespaceName,
                         &m_typeName,
                         &m_memberName,
                         &m_params);
}

bool
SplitName::FindType(IMDInternalImport* mdInternal)
{
    if (m_typeToken != mdTypeDefNil)
    {
        return true;
    }

    if (!m_typeName)
    {
        return false;
    }

    if ((m_namespaceName == NULL || m_namespaceName[0] == '\0') 
        && (CompareUtf8(COR_MODULE_CLASS, m_typeName, m_nameFlags)==0))
    {
        m_typeToken = TokenFromRid(1, mdtTypeDef);  // <Module> class always has a RID of 1.
        return true;
    }

    MetaEnum metaEnum;

    if (metaEnum.Start(mdInternal, mdtTypeDef, mdTypeDefNil) != S_OK)
    {
        return false;
    }

    return metaEnum.NextTokenByName(m_namespaceName,
                                    m_typeName,
                                    m_nameFlags,
                                    &m_typeToken) == S_OK;
}

bool
SplitName::FindMethod(IMDInternalImport* mdInternal)
{
    if (m_memberToken != mdTokenNil)
    {
        return true;
    }

    if (m_typeToken == mdTypeDefNil ||
        !m_memberName)
    {
        return false;
    }

    ULONG32 EmptySig = 0;

    // XXX drewb - Compare using signature when available.
    if (mdInternal->FindMethodDefUsingCompare(m_typeToken,
                                              m_memberName,
                                              (PCCOR_SIGNATURE)&EmptySig,
                                              sizeof(EmptySig),
                                              NULL,
                                              NULL,
                                              &m_memberToken) != S_OK)
    {
        m_memberToken = mdTokenNil;
        return false;
    }

    return true;
}

bool
SplitName::FindField(IMDInternalImport* mdInternal)
{
    if (m_memberToken != mdTokenNil)
    {
        return true;
    }

    if (m_typeToken == mdTypeDefNil ||
        !m_memberName ||
        m_params)
    {
        // Can't have params with a field.
        return false;
    }

    MetaEnum metaEnum;

    if (metaEnum.Start(mdInternal, mdtFieldDef, m_typeToken) != S_OK)
    {
        return false;
    }

    return metaEnum.NextTokenByName(NULL,
                                    m_memberName,
                                    m_nameFlags,
                                    &m_memberToken) == S_OK;
}

HRESULT
SplitName::AllocAndSplitString(__in_opt PCWSTR fullName,
                               SplitSyntax syntax,
                               ULONG32 nameFlags,
                               ULONG32 memberDots,
                               SplitName** split)
{
    HRESULT status;

    if (nameFlags & ~(CLRDATA_BYNAME_CASE_SENSITIVE |
                      CLRDATA_BYNAME_CASE_INSENSITIVE))
    {
        return E_INVALIDARG;
    }

    *split = new (nothrow) SplitName(syntax, nameFlags, memberDots);
    if (!*split)
    {
        return E_OUTOFMEMORY;
    }

    if ((status = (*split)->SplitString(fullName)) != S_OK)
    {
        delete (*split);
        return status;
    }

    return S_OK;
}

HRESULT
SplitName::CdStartMethod(__in_opt PCWSTR fullName,
                         ULONG32 nameFlags,
                         Module* mod,
                         mdTypeDef typeToken,
                         AppDomain* appDomain,
                         IXCLRDataAppDomain* pubAppDomain,
                         SplitName** splitRet,
                         CLRDATA_ENUM* handle)
{
    HRESULT status;
    SplitName* split;
    ULONG methDots = 0;

    *handle = TO_CDENUM(NULL);

 Retry:
    if ((status = SplitName::
         AllocAndSplitString(fullName, SPLIT_METHOD, nameFlags,
                             methDots, &split)) != S_OK)
    {
        return status;
    }

    if (typeToken == mdTypeDefNil)
    {
        if (!split->FindType(mod->GetMDImport()))
        {
            bool hasNamespace = split->m_namespaceName != NULL;

            delete split;

            //
            // We may have a case where there's an
            // explicitly implemented method which
            // has dots in the name.  If it's possible
            // to move the method name dot split
            // back, go ahead and retry that way.
            //

            if (hasNamespace)
            {
                methDots++;
                goto Retry;
            }

            return E_INVALIDARG;
        }

        typeToken = split->m_typeToken;
    }
    else
    {
        if (split->m_namespaceName || split->m_typeName)
        {
            delete split;
            return E_INVALIDARG;
        }
    }

    if ((status = split->m_metaEnum.
         Start(mod->GetMDImport(), mdtMethodDef, typeToken)) != S_OK)
    {
        delete split;
        return status;
    }

    split->m_metaEnum.m_appDomain = appDomain;
    if (pubAppDomain)
    {
        split->m_metaEnum.m_appDomain =
            ((ClrDataAppDomain*)pubAppDomain)->GetAppDomain();
    }
    split->m_module = mod;

    *handle = TO_CDENUM(split);
    if (splitRet)
    {
        *splitRet = split;
    }
    return S_OK;
}

HRESULT
SplitName::CdNextMethod(CLRDATA_ENUM* handle,
                        mdMethodDef* token)
{
    SplitName* split = FROM_CDENUM(SplitName, *handle);
    if (!split)
    {
        return E_INVALIDARG;
    }

    return split->m_metaEnum.
        NextTokenByName(NULL, split->m_memberName, split->m_nameFlags,
                        token);
}

HRESULT
SplitName::CdNextDomainMethod(CLRDATA_ENUM* handle,
                              AppDomain** appDomain,
                              mdMethodDef* token)
{
    SplitName* split = FROM_CDENUM(SplitName, *handle);
    if (!split)
    {
        return E_INVALIDARG;
    }

    return split->m_metaEnum.
        NextDomainTokenByName(NULL, split->m_memberName, split->m_nameFlags,
                              appDomain, token);
}

HRESULT
SplitName::CdStartField(__in_opt PCWSTR fullName,
                        ULONG32 nameFlags,
                        ULONG32 fieldFlags,
                        IXCLRDataTypeInstance* fromTypeInst,
                        TypeHandle typeHandle,
                        Module* mod,
                        mdTypeDef typeToken,
                        ULONG64 objBase,
                        Thread* tlsThread,
                        IXCLRDataTask* pubTlsThread,
                        AppDomain* appDomain,
                        IXCLRDataAppDomain* pubAppDomain,
                        SplitName** splitRet,
                        CLRDATA_ENUM* handle)
{
    HRESULT status;
    SplitName* split;

    *handle = TO_CDENUM(NULL);

    if ((status = SplitName::
         AllocAndSplitString(fullName,
                             fullName ? SPLIT_FIELD : SPLIT_NO_NAME,
                             nameFlags, 0,
                             &split)) != S_OK)
    {
        return status;
    }

    if (typeHandle.IsNull())
    {
        if (typeToken == mdTypeDefNil)
        {
            if (!split->FindType(mod->GetMDImport()))
            {
                status = E_INVALIDARG;
                goto Fail;
            }

            typeToken = split->m_typeToken;
        }
        else
        {
            if (split->m_namespaceName || split->m_typeName)
            {
                status = E_INVALIDARG;
                goto Fail;
            }
        }

        typeHandle = mod->LookupTypeDef(split->m_typeToken);
        if (typeHandle.IsNull())
        {
            status = E_UNEXPECTED;
            goto Fail;
        }
    }

    if ((status = InitFieldIter(&split->m_fieldEnum,
                                typeHandle,
                                true,
                                fieldFlags,
                                fromTypeInst)) != S_OK)
    {
        goto Fail;
    }

    split->m_objBase = objBase;
    split->m_tlsThread = tlsThread;
    if (pubTlsThread)
    {
        split->m_tlsThread = ((ClrDataTask*)pubTlsThread)->GetThread();
    }
    split->m_metaEnum.m_appDomain = appDomain;
    if (pubAppDomain)
    {
        split->m_metaEnum.m_appDomain =
            ((ClrDataAppDomain*)pubAppDomain)->GetAppDomain();
    }
    split->m_module = mod;

    *handle = TO_CDENUM(split);
    if (splitRet)
    {
        *splitRet = split;
    }
    return S_OK;

 Fail:
    delete split;
    return status;
}

HRESULT
SplitName::CdNextField(ClrDataAccess* dac,
                       CLRDATA_ENUM* handle,
                       IXCLRDataTypeDefinition** fieldType,
                       ULONG32* fieldFlags,
                       IXCLRDataValue** value,
                       ULONG32 nameBufRetLen,
                       ULONG32* nameLenRet,
                       __inout_ecount_opt(nameBufRetLen) WCHAR nameBufRet[  ],
                       IXCLRDataModule** tokenScopeRet,
                       mdFieldDef* tokenRet)
{
    HRESULT status;

    SplitName* split = FROM_CDENUM(SplitName, *handle);
    if (!split)
    {
        return E_INVALIDARG;
    }

    FieldDesc* fieldDesc;

    while ((fieldDesc = split->m_fieldEnum.Next()))
    {
        if (split->m_syntax != SPLIT_NO_NAME)
        {
            LPCUTF8 fieldName = fieldDesc->GetName();
            if (!fieldName ||
                split->Compare(split->m_memberName, fieldName) != 0)
            {
                continue;
            }
        }

        split->m_lastField = fieldDesc;

        if (fieldFlags)
        {
            *fieldFlags =
                GetTypeFieldValueFlags(fieldDesc->GetFieldTypeHandleThrowing(),
                                       fieldDesc,
                                       split->m_fieldEnum.
                                       IsFieldFromParentClass() ?
                                       CLRDATA_FIELD_IS_INHERITED : 0,
                                       false);
        }

        if (nameBufRetLen || nameLenRet)
        {
            if ((status = ConvertUtf8(fieldDesc->GetName(),
                                      nameBufRetLen, nameLenRet,
                                      nameBufRet)) != S_OK)
            {
                return status;
            }
        }

        if (tokenScopeRet && !value)
        {
            *tokenScopeRet = new (nothrow)
                ClrDataModule(dac, fieldDesc->GetModule());
            if (!*tokenScopeRet)
            {
                return E_OUTOFMEMORY;
            }
        }

        if (tokenRet)
        {
            *tokenRet = fieldDesc->GetMemberDef();
        }

        if (fieldType)
        {
            TypeHandle fieldTypeHandle = fieldDesc->GetFieldTypeHandleThrowing();
            *fieldType = new (nothrow)
                ClrDataTypeDefinition(dac,
                                      fieldTypeHandle.GetModule(),
                                      fieldTypeHandle.GetClass()->GetCl(),
                                      fieldTypeHandle);
            if (!*fieldType && tokenScopeRet)
            {
                delete *tokenScopeRet;
            }
            return *fieldType ? S_OK : E_OUTOFMEMORY;
        }

        if (value)
        {
            return ClrDataValue::
                NewFromFieldDesc(dac,
                                 split->m_metaEnum.m_appDomain,
                                 split->m_fieldEnum.IsFieldFromParentClass() ?
                                 CLRDATA_VALUE_IS_INHERITED : 0,
                                 fieldDesc,
                                 split->m_objBase,
                                 split->m_tlsThread,
                                 NULL,
                                 value,
                                 nameBufRetLen,
                                 nameLenRet,
                                 nameBufRet,
                                 tokenScopeRet,
                                 tokenRet);
        }

        return S_OK;
    }

    return S_FALSE;
}

HRESULT
SplitName::CdNextDomainField(ClrDataAccess* dac,
                             CLRDATA_ENUM* handle,
                             IXCLRDataValue** value)
{
    HRESULT status;

    SplitName* split = FROM_CDENUM(SplitName, *handle);
    if (!split)
    {
        return E_INVALIDARG;
    }

    if (split->m_metaEnum.m_appDomain)
    {
        // Use only the caller-provided app domain.
        return CdNextField(dac, handle, NULL, NULL, value,
                           0, NULL, NULL, NULL, NULL);
    }

    //
    // Splay fields across all app domains.
    //

    for (;;)
    {
        if (!split->m_lastField)
        {
            // Need to fetch a field.
            if ((status = CdNextField(dac, handle, NULL, NULL, NULL,
                                      0, NULL, NULL, NULL, NULL)) != S_OK)
            {
                return status;
            }

            split->m_metaEnum.m_domainIter.Init(FALSE);
        }

        if (split->m_metaEnum.m_domainIter.Next())
        {
            break;
        }

        split->m_lastField = NULL;
    }

    return ClrDataValue::
        NewFromFieldDesc(dac,
                         split->m_metaEnum.m_domainIter.GetDomain(),
                         split->m_fieldEnum.IsFieldFromParentClass() ?
                         CLRDATA_VALUE_IS_INHERITED : 0,
                         split->m_lastField,
                         split->m_objBase,
                         split->m_tlsThread,
                         NULL,
                         value,
                         0,
                         NULL,
                         NULL,
                         NULL,
                         NULL);
}

HRESULT
SplitName::CdStartType(__in_opt PCWSTR fullName,
                       ULONG32 nameFlags,
                       Module* mod,
                       AppDomain* appDomain,
                       IXCLRDataAppDomain* pubAppDomain,
                       SplitName** splitRet,
                       CLRDATA_ENUM* handle)
{
    HRESULT status;
    SplitName* split;

    *handle = TO_CDENUM(NULL);

    if ((status = SplitName::
         AllocAndSplitString(fullName, SPLIT_TYPE, nameFlags, 0,
                             &split)) != S_OK)
    {
        return status;
    }

    if ((status = split->m_metaEnum.
         Start(mod->GetMDImport(), mdtTypeDef, mdTokenNil)) != S_OK)
    {
        delete split;
        return status;
    }

    split->m_metaEnum.m_appDomain = appDomain;
    if (pubAppDomain)
    {
        split->m_metaEnum.m_appDomain =
            ((ClrDataAppDomain*)pubAppDomain)->GetAppDomain();
    }
    split->m_module = mod;

    *handle = TO_CDENUM(split);
    if (splitRet)
    {
        *splitRet = split;
    }
    return S_OK;
}

HRESULT
SplitName::CdNextType(CLRDATA_ENUM* handle,
                      mdTypeDef* token)
{
    SplitName* split = FROM_CDENUM(SplitName, *handle);
    if (!split)
    {
        return E_INVALIDARG;
    }

    return split->m_metaEnum.
        NextTokenByName(split->m_namespaceName, split->m_typeName,
                        split->m_nameFlags, token);
}

HRESULT
SplitName::CdNextDomainType(CLRDATA_ENUM* handle,
                            AppDomain** appDomain,
                            mdTypeDef* token)
{
    SplitName* split = FROM_CDENUM(SplitName, *handle);
    if (!split)
    {
        return E_INVALIDARG;
    }

    return split->m_metaEnum.
        NextDomainTokenByName(split->m_namespaceName, split->m_typeName,
                              split->m_nameFlags, appDomain, token);
}

//----------------------------------------------------------------------------
//
// DacInstanceManager.
//
// Data retrieved from the target process is cached for two reasons:
//
// 1. It may be necessary to map from the host address back to the target
//    address.  For example, if any code uses a 'this' pointer or
//    takes the address of a field the address has to be translated from
//    host to target.  This requires instances to be held as long as
//    they may be referenced.
//
// 2. Data is often referenced multiple times so caching is an important
//    performance advantage.
//
// Ideally we'd like to implement a simple page cache but this is
// complicated by the fact that user minidump memory can have
// arbitrary granularity and also that the member operator (->)
// needs to return a pointer to an object.  That means that all of
// the data for an object must be sequential and cannot be split
// at page boundaries.
//
// Data can also be accessed with different sizes.  For example,
// a base struct can be accessed, then cast to a derived struct and
// accessed again with the larger derived size.  The cache must
// be able to replace data to maintain the largest amount of data
// touched.
//
// We keep track of each access and the recovered memory for it.
// A hash on target address allows quick access to instance data
// by target address.  The data for each access has a header on it
// for bookkeeping purposes, so host address to target address translation
// is just a matter of backing up to the header and pulling the target
// address from it.  Keeping each access separately allows easy
// replacement by larger accesses.
//
//----------------------------------------------------------------------------

DacInstanceManager::DacInstanceManager(void)
{
    InitEmpty();
}

DacInstanceManager::~DacInstanceManager(void)
{
    Flush();
}

DAC_INSTANCE*
DacInstanceManager::Add(DAC_INSTANCE* inst)
{
    // Assert that we don't add NULL instances. This allows us to assert that found instances
    // are not NULL in DacInstanceManager::Find
    _ASSERTE(inst != NULL);
    
    DWORD nHash = DAC_INSTANCE_HASH(inst->addr);
    HashInstanceKeyBlock* block = m_hash[nHash];
    
    if (!block || block->firstElement == 0)
    {

        HashInstanceKeyBlock* newBlock;
        if (block)
        {
            newBlock = (HashInstanceKeyBlock*) new (nothrow) BYTE[HASH_INSTANCE_BLOCK_ALLOC_SIZE];
        }
        else
        {
            // We allocate one big memory chunk that has a block for every index of the hash table to
            // improve data locality and reduce the number of allocs. In most cases, a hash bucket will
            // use only one block, so improving data locality across blocks (i.e. keeping the buckets of the 
            // hash table together) should help. 
            newBlock = (HashInstanceKeyBlock*) 
                ClrVirtualAlloc(NULL, HASH_INSTANCE_BLOCK_ALLOC_SIZE*NumItems(m_hash), MEM_COMMIT, PAGE_READWRITE);
        }
        if (!newBlock)
        {
            return NULL;
        }
        if (block)
        {  
            // We add the newest block to the start of the list assuming that most accesses are for
            // recently added elements.
            newBlock->next = block;
            m_hash[nHash] = newBlock; // The previously allocated block
            newBlock->firstElement = HASH_INSTANCE_BLOCK_NUM_ELEMENTS;
            block = newBlock;
        }
        else
        {
            for (DWORD j = 0; j < NumItems(m_hash); j++)
            {
                m_hash[j] = newBlock;
                newBlock->next = NULL; // The previously allocated block
                newBlock->firstElement = HASH_INSTANCE_BLOCK_NUM_ELEMENTS;
                newBlock = (HashInstanceKeyBlock*) (((BYTE*) newBlock) + HASH_INSTANCE_BLOCK_ALLOC_SIZE);
            }
            block = m_hash[nHash];
        }
    }
    _ASSERTE(block->firstElement > 0);
    block->firstElement--;
    block->instanceKeys[block->firstElement].addr = inst->addr;
    block->instanceKeys[block->firstElement].instance = inst;

    inst->next = NULL;
    return inst;
}


DAC_INSTANCE*
DacInstanceManager::Alloc(TADDR addr, ULONG32 size, DAC_USAGE_TYPE usage)
{
    DAC_INSTANCE_BLOCK* block;
    DAC_INSTANCE* inst;
    ULONG32 fullSize;

    C_ASSERT(sizeof(DAC_INSTANCE_BLOCK) <= DAC_INSTANCE_ALIGN);
    C_ASSERT((sizeof(DAC_INSTANCE) & (DAC_INSTANCE_ALIGN - 1)) == 0);

    //
    // All allocated instances must be kept alive as long
    // as anybody may have a host pointer for one of them.
    // This means that we cannot delete an arbitrary instance
    // unless we are sure no pointers exist, which currently
    // is not possible to determine, thus we just hold everything
    // until a Flush.  This greatly simplifies instance allocation
    // as we can then just sweep through large blocks rather
    // than having to use a real allocator.  The only
    // complication is that we need to keep all instance
    // data aligned.  We have guaranteed that the header will
    // preserve alignment of the data following if the header
    // is aligned, so as long as we round up all allocations
    // to a multiple of the alignment size everything just works.
    //

    fullSize = (size + DAC_INSTANCE_ALIGN - 1) & ~(DAC_INSTANCE_ALIGN - 1);
    _ASSERTE(fullSize && fullSize <= 0xffffffff - 2 * sizeof(*inst));
    fullSize += sizeof(*inst);

    //
    // Check for an existing block with space.
    //

    for (block = m_blocks; block; block = block->next)
    {
        if (fullSize <= block->bytesFree)
        {
            break;
        }
    }

    if (!block)
    {
        //
        // No existing block has enough space, so allocate a new
        // one and link it in.  We know we're allocating large
        // blocks so directly VirtualAlloc.
        //

        ULONG32 blockSize = fullSize + DAC_INSTANCE_ALIGN;
        if (blockSize < DAC_INSTANCE_BLOCK_ALLOCATION)
        {
            blockSize = DAC_INSTANCE_BLOCK_ALLOCATION;
        }

        block = (DAC_INSTANCE_BLOCK*)
            ClrVirtualAlloc(NULL, blockSize, MEM_COMMIT, PAGE_READWRITE);
        if (!block)
        {
            return NULL;
        }

        // Keep the first aligned unit for the block header.
        block->bytesUsed = DAC_INSTANCE_ALIGN;
        block->bytesFree = blockSize - DAC_INSTANCE_ALIGN;

        block->next = m_blocks;
        m_blocks = block;

        m_blockMemUsage += blockSize;
    }

    inst = (DAC_INSTANCE*)((PBYTE)block + block->bytesUsed);
    block->bytesUsed += fullSize;
    _ASSERTE(block->bytesFree >= fullSize);
    block->bytesFree -= fullSize;

    inst->next = NULL;
    inst->addr = addr;
    inst->size = size;
    inst->sig = DAC_INSTANCE_SIG;
    inst->usage = usage;
    inst->enumMem = 0;
    inst->MDEnumed = 0;

    m_numInst++;
    m_instMemUsage += fullSize;
    return inst;
}

void
DacInstanceManager::ReturnAlloc(DAC_INSTANCE* inst)
{
    DAC_INSTANCE_BLOCK* block;
    ULONG32 fullSize;

    //
    // This special routine handles cleanup in
    // cases where an instances has been allocated
    // but must be returned due to a following error.
    // The given instance must be the last instance
    // in an existing block.
    //

    fullSize =
        ((inst->size + DAC_INSTANCE_ALIGN - 1) & ~(DAC_INSTANCE_ALIGN - 1)) +
        sizeof(*inst);

    for (block = m_blocks; block; block = block->next)
    {
        if ((PBYTE)inst == (PBYTE)block + (block->bytesUsed - fullSize))
        {
            break;
        }
    }

    _ASSERTE(block != NULL);
    if (!block)
    {
        return;
    }

    block->bytesUsed -= fullSize;
    block->bytesFree += fullSize;
    m_numInst--;
    m_instMemUsage -= fullSize;
}


DAC_INSTANCE*
DacInstanceManager::Find(TADDR addr)
{

    
    HashInstanceKeyBlock* block = m_hash[DAC_INSTANCE_HASH(addr)];

    
    while (block)
    {
        DWORD nIndex = block->firstElement;
        for (; nIndex < HASH_INSTANCE_BLOCK_NUM_ELEMENTS; nIndex++)
        {
            if (block->instanceKeys[nIndex].addr == addr)
            {

                DAC_INSTANCE* inst = block->instanceKeys[nIndex].instance;

                // inst should not be NULL even if the address was superseded. We search 
                // the entries in the reverse order they were added. So we should have 
                // found the superseding entry before this one. (Of course, if a NULL instance
                // has been added, this assert is meaningless. DacInstanceManager::Add 
                // asserts that NULL instances aren't added.)
                
                _ASSERTE(inst != NULL);
                
                return inst;
            }
        }
        block = block->next;
    }

    
    return NULL;
}

HRESULT
DacInstanceManager::Write(DAC_INSTANCE* inst, bool throwEx)
{
    HRESULT status;

    if (inst->usage == DAC_VPTR)
    {
        // Skip over the host-side vtable pointer when
        // writing back.
        status = DacWriteAll(inst->addr + sizeof(TADDR),
                             (PBYTE)(inst + 1) + sizeof(PVOID),
                             inst->size - sizeof(TADDR),
                             throwEx);
    }
    else
    {
        // Write the whole instance back.
        status = DacWriteAll(inst->addr, inst + 1, inst->size, throwEx);
    }

    return status;
}

void
DacInstanceManager::Supersede(DAC_INSTANCE* inst)
{
    _ASSERTE(inst != NULL);
                
    //
    // This instance has been superseded by a larger
    // one and so must be removed from the hash.  However,
    // code may be holding the instance pointer so it
    // can't just be deleted.  Put it on a list for
    // later cleanup.
    //

    HashInstanceKeyBlock* block = m_hash[DAC_INSTANCE_HASH(inst->addr)];
    while (block)
    {
        DWORD nIndex = block->firstElement;
        for (; nIndex < HASH_INSTANCE_BLOCK_NUM_ELEMENTS; nIndex++)
        {
            if (block->instanceKeys[nIndex].instance == inst)
            {
                block->instanceKeys[nIndex].instance = NULL;
                break;
            }
        }
        if (nIndex < HASH_INSTANCE_BLOCK_NUM_ELEMENTS)
        {
            break;
        }
        block = block->next;
    }

    AddSuperseded(inst);
}

void
DacInstanceManager::Flush(void)
{
    //
    // All allocated memory is in the block
    // list, so just free the blocks and
    // forget all the internal pointers.
    //

    for (;;)
    {
        FreeAllBlocks();

        DAC_INSTANCE_PUSH* push = m_instPushed;
        if (!push)
        {
            break;
        }

        m_instPushed = push->next;
        m_blocks = push->blocks;
        delete push;
    }

    for (int i = NumItems(m_hash) - 1; i >= 0; i--)
    {
        HashInstanceKeyBlock* block = m_hash[i];
        HashInstanceKeyBlock* next;
        while (block)
        {
            next = block->next;
            if (next) 
            {
                delete [] block;
            }
            else if (i == 0)
            {
                ClrVirtualFree(block, 0, MEM_RELEASE);
            }
            block = next;
        }
    }

    InitEmpty();
}

void
DacInstanceManager::ClearEnumMemMarker(void)
{
    ULONG i;
    DAC_INSTANCE* inst;

    for (i = 0; i < NumItems(m_hash); i++)
    {
        HashInstanceKeyBlock* block = m_hash[i];
        while (block)
        {
            DWORD j;
            for (j = block->firstElement; j < HASH_INSTANCE_BLOCK_NUM_ELEMENTS; j++)
            {
                inst = block->instanceKeys[j].instance;
                if (inst != NULL)
                {
                    inst->enumMem = 0;
                }
            }
            block = block->next;
        }
    }
    for (inst = m_superseded; inst; inst = inst->next)
    {
        inst->enumMem = 0;
    }
}


//+++++++++++++++++++++++++++++++++++++++++++++++++++++++
//
// Iterating through all of the hash entry and report the memory
// instance to minidump
//
// This function returns the total number of bytes that it reported.
//
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++
UINT
DacInstanceManager::DumpAllInstances(
    ICLRDataEnumMemoryRegionsCallback *pCallBack)       // memory report call back
{
    ULONG           i;
    DAC_INSTANCE*   inst;
    UINT            cbTotal = 0;


    for (i = 0; i < NumItems(m_hash); i++)
    {


        HashInstanceKeyBlock* block = m_hash[i];
        while (block)
        {
            DWORD j;
            for (j = block->firstElement; j < HASH_INSTANCE_BLOCK_NUM_ELEMENTS; j++)
            {
                inst = block->instanceKeys[j].instance;
                
                // Only report those we intended to.
                // So far, only metadata is excluded!
                //
                if (inst && inst->noReport == 0)
                {
                    cbTotal += inst->size;
                    pCallBack->EnumMemoryRegion(inst->addr, inst->size);
                }

            }
            block = block->next;
        }


    }


    return cbTotal;

}

bool
DacInstanceManager::PushState(void)
{
    // Not implemented, no callers currently
    _ASSERTE(FALSE);
    return false;
}

void
DacInstanceManager::PopState(void)
{
    return;
}

DAC_INSTANCE_BLOCK*
DacInstanceManager::FindInstanceBlock(DAC_INSTANCE* inst)
{
    for (DAC_INSTANCE_BLOCK* block = m_blocks; block; block = block->next)
    {
        if ((PBYTE)inst >= (PBYTE)block &&
            (PBYTE)inst < (PBYTE)block + block->bytesUsed)
        {
            return block;
        }
    }

    return NULL;
}

void
DacInstanceManager::FreeAllBlocks(void)
{
    DAC_INSTANCE_BLOCK* block;

    while ((block = m_blocks))
    {
        m_blocks = block->next;
        ClrVirtualFree(block, 0, MEM_RELEASE);
    }
}

//----------------------------------------------------------------------------
//
// ClrDataAccess.
//
//----------------------------------------------------------------------------

LONG ClrDataAccess::s_procInit;

ClrDataAccess::ClrDataAccess(ICLRDataTarget* target)
{
    m_target = target;
    m_target->AddRef();
    if (m_target->QueryInterface(__uuidof(ICLRDataTarget2),
                                 (void**)&m_target2) != S_OK)
    {
        m_target2 = NULL;
    }


    if (m_target->QueryInterface(__uuidof(ICLRMetadataLocator),
                                 (void**)&m_metadataLocator) != S_OK)
    {
        // The debugger doesn't implement IMetadataLocator. Use
        // IXCLRDataTarget3 if that exists.
        
        m_metadataLocator = NULL;
        if (m_target->QueryInterface(__uuidof(IXCLRDataTarget3),
                                     (void**)&m_target3) != S_OK)
        {
            m_target3 = NULL;
        }
    }
    else
    {
        // If the debugger implements IMetadataLocator,
        // we don't need m_target3.
        m_target3 = NULL;
    }

    m_globalBase = 0;
    m_mdImports = NULL;
    m_refs = 1;
    m_instanceAge = 0;
    m_debugMode = GetEnvironmentVariableA("MSCORDACWKS_DEBUG", NULL, 0) != 0;

    m_jitNotificationTable = NULL;
}

ClrDataAccess::~ClrDataAccess(void)
{
    delete [] m_jitNotificationTable;
    if (m_metadataLocator)
    {
        m_metadataLocator->Release();
    }
    if (m_target3)
    {
        m_target3->Release();
    }
    if (m_target2)
    {
        m_target2->Release();
    }
    m_target->Release();
}

STDMETHODIMP
ClrDataAccess::QueryInterface(THIS_
                              IN REFIID interfaceId,
                              OUT PVOID* iface)
{
    void* ifaceRet;

    if (IsEqualIID(interfaceId, IID_IUnknown) ||
        IsEqualIID(interfaceId, __uuidof(IXCLRDataProcess)))
    {
        ifaceRet = static_cast<IXCLRDataProcess*>(this);
    }
    else if (IsEqualIID(interfaceId, __uuidof(ICLRDataEnumMemoryRegions)))
    {
        ifaceRet = static_cast<ICLRDataEnumMemoryRegions*>(this);
    }
    else if (IsEqualIID(interfaceId, IID_ICorDataAccess_Obsolete))
    {
        ICorDataAccessRedirect* redir = new ICorDataAccessRedirect(this);
        if (!redir)
        {
            *iface = NULL;
            return E_OUTOFMEMORY;
        }

        ifaceRet = redir;
    }
    else
    {
        *iface = NULL;
        return E_NOINTERFACE;
    }

    AddRef();
    *iface = ifaceRet;
    return S_OK;
}

STDMETHODIMP_(ULONG)
ClrDataAccess::AddRef(THIS)
{
    return InterlockedIncrement(&m_refs);
}

STDMETHODIMP_(ULONG)
ClrDataAccess::Release(THIS)
{
    LONG newRefs = InterlockedDecrement(&m_refs);
    if (newRefs == 0)
    {
        delete this;
    }
    return newRefs;
}

HRESULT STDMETHODCALLTYPE
ClrDataAccess::Flush(void)
{
    //
    // Free MD import objects.
    //

    DAC_MD_IMPORT* importList;

    while (m_mdImports)
    {
        importList = m_mdImports;
        m_mdImports = importList->next;
        importList->impl->Release();
        delete importList;
    }

    // Free instance memory.
    m_instances.Flush();

    // When the host instance cache is flushed we
    // update the instance age count so that
    // all child objects automatically become
    // invalid.  This prevents them from using
    // any pointers they've kept to host instances
    // which are now gone.
    m_instanceAge++;

    return S_OK;
}

HRESULT STDMETHODCALLTYPE
ClrDataAccess::StartEnumTasks(
    /* [out] */ CLRDATA_ENUM* handle)
{
    HRESULT status;

    DAC_ENTER();

    EX_TRY
    {
        if (ThreadStore::s_pThreadStore)
        {
            Thread* thread = ThreadStore::GetAllThreadList(NULL, 0, 0);
            *handle = TO_CDENUM(thread);
            status = *handle ? S_OK : S_FALSE;
        }
        else
        {
            status = S_FALSE;
        }
    }
    EX_CATCH
    {
        if (!DacExceptionFilter(GET_EXCEPTION(), this, &status))
        {
            EX_RETHROW;
        }
    }
    EX_END_CATCH(SwallowAllExceptions)

    DAC_LEAVE();
    return status;
}

HRESULT STDMETHODCALLTYPE
ClrDataAccess::EnumTask(
    /* [in, out] */ CLRDATA_ENUM* handle,
    /* [out] */ IXCLRDataTask **task)
{
    HRESULT status;

    DAC_ENTER();

    EX_TRY
    {
        if (*handle)
        {
            Thread* thread = FROM_CDENUM(Thread, *handle);
            *task = new (nothrow) ClrDataTask(this, thread);
            if (*task)
            {
                thread = ThreadStore::GetAllThreadList(thread, 0, 0);
                *handle = TO_CDENUM(thread);
                status = S_OK;
            }
            else
            {
                status = E_OUTOFMEMORY;
            }
        }
        else
        {
            status = S_FALSE;
        }
    }
    EX_CATCH
    {
        if (!DacExceptionFilter(GET_EXCEPTION(), this, &status))
        {
            EX_RETHROW;
        }
    }
    EX_END_CATCH(SwallowAllExceptions)

    DAC_LEAVE();
    return status;
}

HRESULT STDMETHODCALLTYPE
ClrDataAccess::EndEnumTasks(
    /* [in] */ CLRDATA_ENUM handle)
{
    HRESULT status;

    DAC_ENTER();

    EX_TRY
    {
        // Enumerator holds no resources.
        status = S_OK;
    }
    EX_CATCH
    {
        if (!DacExceptionFilter(GET_EXCEPTION(), this, &status))
        {
            EX_RETHROW;
        }
    }
    EX_END_CATCH(SwallowAllExceptions)

    DAC_LEAVE();
    return status;
}

HRESULT STDMETHODCALLTYPE
ClrDataAccess::GetTaskByOSThreadID(
    /* [in] */ ULONG32 osThreadID,
    /* [out] */ IXCLRDataTask **task)
{
    HRESULT status;

    DAC_ENTER();

    EX_TRY
    {
        status = E_INVALIDARG;
        Thread* thread = DacGetThread(osThreadID);
        if (thread != NULL)
        {
            *task = new (nothrow) ClrDataTask(this, thread);
            status = *task ? S_OK : E_OUTOFMEMORY;
        }
    }
    EX_CATCH
    {
        if (!DacExceptionFilter(GET_EXCEPTION(), this, &status))
        {
            EX_RETHROW;
        }
    }
    EX_END_CATCH(SwallowAllExceptions)

    DAC_LEAVE();
    return status;
}

HRESULT STDMETHODCALLTYPE
ClrDataAccess::GetTaskByUniqueID(
    /* [in] */ ULONG64 uniqueID,
    /* [out] */ IXCLRDataTask **task)
{
    HRESULT status;

    DAC_ENTER();

    EX_TRY
    {
        Thread* thread = FindClrThreadByTaskId(uniqueID);
        if (thread)
        {
            *task = new (nothrow) ClrDataTask(this, thread);
            status = *task ? S_OK : E_OUTOFMEMORY;
        }
        else
        {
            status = E_INVALIDARG;
        }
    }
    EX_CATCH
    {
        if (!DacExceptionFilter(GET_EXCEPTION(), this, &status))
        {
            EX_RETHROW;
        }
    }
    EX_END_CATCH(SwallowAllExceptions)

    DAC_LEAVE();
    return status;
}

HRESULT STDMETHODCALLTYPE
ClrDataAccess::GetFlags(
    /* [out] */ ULONG32 *flags)
{
    HRESULT status;

    DAC_ENTER();

    EX_TRY
    {
        // XXX drewb - GC check.
        *flags = CLRDATA_PROCESS_DEFAULT;
        status = S_OK;
    }
    EX_CATCH
    {
        if (!DacExceptionFilter(GET_EXCEPTION(), this, &status))
        {
            EX_RETHROW;
        }
    }
    EX_END_CATCH(SwallowAllExceptions)

    DAC_LEAVE();
    return status;
}

HRESULT STDMETHODCALLTYPE
ClrDataAccess::IsSameObject(
    /* [in] */ IXCLRDataProcess* process)
{
    HRESULT status;

    DAC_ENTER();

    EX_TRY
    {
        status = m_target == ((ClrDataAccess*)process)->m_target ?
            S_OK : S_FALSE;
    }
    EX_CATCH
    {
        if (!DacExceptionFilter(GET_EXCEPTION(), this, &status))
        {
            EX_RETHROW;
        }
    }
    EX_END_CATCH(SwallowAllExceptions)

    DAC_LEAVE();
    return status;
}

HRESULT STDMETHODCALLTYPE
ClrDataAccess::GetManagedObject(
    /* [out] */ IXCLRDataValue **value)
{
    HRESULT status;

    DAC_ENTER();

    EX_TRY
    {
        // XXX drewb.
        status = E_NOTIMPL;
    }
    EX_CATCH
    {
        if (!DacExceptionFilter(GET_EXCEPTION(), this, &status))
        {
            EX_RETHROW;
        }
    }
    EX_END_CATCH(SwallowAllExceptions)

    DAC_LEAVE();
    return status;
}

HRESULT STDMETHODCALLTYPE
ClrDataAccess::GetDesiredExecutionState(
    /* [out] */ ULONG32 *state)
{
    HRESULT status;

    DAC_ENTER();

    EX_TRY
    {
        // XXX drewb.
        status = E_NOTIMPL;
    }
    EX_CATCH
    {
        if (!DacExceptionFilter(GET_EXCEPTION(), this, &status))
        {
            EX_RETHROW;
        }
    }
    EX_END_CATCH(SwallowAllExceptions)

    DAC_LEAVE();
    return status;
}

HRESULT STDMETHODCALLTYPE
ClrDataAccess::SetDesiredExecutionState(
    /* [in] */ ULONG32 state)
{
    HRESULT status;

    DAC_ENTER();

    EX_TRY
    {
        // XXX drewb.
        status = E_NOTIMPL;
    }
    EX_CATCH
    {
        if (!DacExceptionFilter(GET_EXCEPTION(), this, &status))
        {
            EX_RETHROW;
        }
    }
    EX_END_CATCH(SwallowAllExceptions)

    DAC_LEAVE();
    return status;
}

HRESULT STDMETHODCALLTYPE
ClrDataAccess::GetAddressType(
    /* [in] */ CLRDATA_ADDRESS address,
    /* [out] */ CLRDataAddressType* type)
{
    HRESULT status;

    DAC_ENTER();

    EX_TRY
    {
        // The only thing that constitutes a failure is some
        // dac failure while checking things.
        status = S_OK;

        if (IsPossibleCodeAddress(address) == S_OK)
        {
            IJitManager* jitMan =
                ExecutionManager::FindJitMan(TO_SLOT(TO_TADDR(address)),
                                             IJitManager::ScanNoReaderLock);
            if (jitMan)
            {
                *type = CLRDATA_ADDRESS_MANAGED_METHOD;
                goto Exit;
            }

            if (StubManager::IsStub(TO_TADDR(address)))
            {
                *type = CLRDATA_ADDRESS_RUNTIME_UNMANAGED_STUB;
                goto Exit;
            }
        }

        *type = CLRDATA_ADDRESS_UNRECOGNIZED;

    Exit: ;
    }
    EX_CATCH
    {
        if (!DacExceptionFilter(GET_EXCEPTION(), this, &status))
        {
            EX_RETHROW;
        }
    }
    EX_END_CATCH(SwallowAllExceptions)

    DAC_LEAVE();
    return status;
}

HRESULT STDMETHODCALLTYPE
ClrDataAccess::GetRuntimeNameByAddress(
    /* [in] */ CLRDATA_ADDRESS address,
    /* [in] */ ULONG32 flags,
    /* [in] */ ULONG32 bufLen,
    /* [out] */ ULONG32 *symbolLen,
    /* [size_is][out] */ WCHAR symbolBuf[  ],
    /* [out] */ CLRDATA_ADDRESS* displacement)
{
    HRESULT status;

    DAC_ENTER();

    EX_TRY
    {
        status = RawGetMethodName(address, flags, bufLen, symbolLen, symbolBuf,
                                  displacement);
    }
    EX_CATCH
    {
        if (!DacExceptionFilter(GET_EXCEPTION(), this, &status))
        {
            EX_RETHROW;
        }
    }
    EX_END_CATCH(SwallowAllExceptions)

    DAC_LEAVE();
    return status;
}

HRESULT STDMETHODCALLTYPE
ClrDataAccess::StartEnumAppDomains(
    /* [out] */ CLRDATA_ENUM* handle)
{
    HRESULT status;

    DAC_ENTER();

    EX_TRY
    {
        AppDomainIterator* iter = new (nothrow) AppDomainIterator(FALSE);
        if (iter)
        {
            *handle = TO_CDENUM(iter);
            status = S_OK;
        }
        else
        {
            status = E_OUTOFMEMORY;
        }
    }
    EX_CATCH
    {
        if (!DacExceptionFilter(GET_EXCEPTION(), this, &status))
        {
            EX_RETHROW;
        }
    }
    EX_END_CATCH(SwallowAllExceptions)

    DAC_LEAVE();
    return status;
}

HRESULT STDMETHODCALLTYPE
ClrDataAccess::EnumAppDomain(
    /* [in, out] */ CLRDATA_ENUM* handle,
    /* [out] */ IXCLRDataAppDomain **appDomain)
{
    HRESULT status;

    DAC_ENTER();

    EX_TRY
    {
        AppDomainIterator* iter = FROM_CDENUM(AppDomainIterator, *handle);
        if (iter->Next())
        {
            *appDomain = new (nothrow)
                ClrDataAppDomain(this, iter->GetDomain());
            status = *appDomain ? S_OK : E_OUTOFMEMORY;
        }
        else
        {
            status = S_FALSE;
        }
    }
    EX_CATCH
    {
        if (!DacExceptionFilter(GET_EXCEPTION(), this, &status))
        {
            EX_RETHROW;
        }
    }
    EX_END_CATCH(SwallowAllExceptions)

    DAC_LEAVE();
    return status;
}

HRESULT STDMETHODCALLTYPE
ClrDataAccess::EndEnumAppDomains(
    /* [in] */ CLRDATA_ENUM handle)
{
    HRESULT status;

    DAC_ENTER();

    EX_TRY
    {
        AppDomainIterator* iter = FROM_CDENUM(AppDomainIterator, handle);
        delete iter;
        status = S_OK;
    }
    EX_CATCH
    {
        if (!DacExceptionFilter(GET_EXCEPTION(), this, &status))
        {
            EX_RETHROW;
        }
    }
    EX_END_CATCH(SwallowAllExceptions)

    DAC_LEAVE();
    return status;
}

HRESULT STDMETHODCALLTYPE
ClrDataAccess::GetAppDomainByUniqueID(
    /* [in] */ ULONG64 uniqueID,
    /* [out] */ IXCLRDataAppDomain **appDomain)
{
    HRESULT status;

    DAC_ENTER();

    EX_TRY
    {
        AppDomainIterator iter(FALSE);

        status = E_INVALIDARG;
        while (iter.Next())
        {
            if (iter.GetDomain()->GetId().m_dwId == uniqueID)
            {
                *appDomain = new (nothrow)
                    ClrDataAppDomain(this, iter.GetDomain());
                status = *appDomain ? S_OK : E_OUTOFMEMORY;
                break;
            }
        }
    }
    EX_CATCH
    {
        if (!DacExceptionFilter(GET_EXCEPTION(), this, &status))
        {
            EX_RETHROW;
        }
    }
    EX_END_CATCH(SwallowAllExceptions)

    DAC_LEAVE();
    return status;
}

HRESULT STDMETHODCALLTYPE
ClrDataAccess::StartEnumAssemblies(
    /* [out] */ CLRDATA_ENUM* handle)
{
    HRESULT status;

    DAC_ENTER();

    EX_TRY
    {
        ProcessModIter* iter = new (nothrow) ProcessModIter;
        if (iter)
        {
            *handle = TO_CDENUM(iter);
            status = S_OK;
        }
        else
        {
            status = E_OUTOFMEMORY;
        }
    }
    EX_CATCH
    {
        if (!DacExceptionFilter(GET_EXCEPTION(), this, &status))
        {
            EX_RETHROW;
        }
    }
    EX_END_CATCH(SwallowAllExceptions)

    DAC_LEAVE();
    return status;
}

HRESULT STDMETHODCALLTYPE
ClrDataAccess::EnumAssembly(
    /* [in, out] */ CLRDATA_ENUM* handle,
    /* [out] */ IXCLRDataAssembly **assembly)
{
    HRESULT status;

    DAC_ENTER();

    EX_TRY
    {
        ProcessModIter* iter = FROM_CDENUM(ProcessModIter, *handle);
        Assembly* assem;

        if ((assem = iter->NextAssem()))
        {
            *assembly = new (nothrow)
                ClrDataAssembly(this, assem);
            status = *assembly ? S_OK : E_OUTOFMEMORY;
        }
        else
        {
            status = S_FALSE;
        }
    }
    EX_CATCH
    {
        if (!DacExceptionFilter(GET_EXCEPTION(), this, &status))
        {
            EX_RETHROW;
        }
    }
    EX_END_CATCH(SwallowAllExceptions)

    DAC_LEAVE();
    return status;
}

HRESULT STDMETHODCALLTYPE
ClrDataAccess::EndEnumAssemblies(
    /* [in] */ CLRDATA_ENUM handle)
{
    HRESULT status;

    DAC_ENTER();

    EX_TRY
    {
        ProcessModIter* iter = FROM_CDENUM(ProcessModIter, handle);
        delete iter;
        status = S_OK;
    }
    EX_CATCH
    {
        if (!DacExceptionFilter(GET_EXCEPTION(), this, &status))
        {
            EX_RETHROW;
        }
    }
    EX_END_CATCH(SwallowAllExceptions)

    DAC_LEAVE();
    return status;
}

HRESULT STDMETHODCALLTYPE
ClrDataAccess::StartEnumModules(
    /* [out] */ CLRDATA_ENUM* handle)
{
    HRESULT status;

    DAC_ENTER();

    EX_TRY
    {
        ProcessModIter* iter = new (nothrow) ProcessModIter;
        if (iter)
        {
            *handle = TO_CDENUM(iter);
            status = S_OK;
        }
        else
        {
            status = E_OUTOFMEMORY;
        }
    }
    EX_CATCH
    {
        if (!DacExceptionFilter(GET_EXCEPTION(), this, &status))
        {
            EX_RETHROW;
        }
    }
    EX_END_CATCH(SwallowAllExceptions)

    DAC_LEAVE();
    return status;
}

HRESULT STDMETHODCALLTYPE
ClrDataAccess::EnumModule(
    /* [in, out] */ CLRDATA_ENUM* handle,
    /* [out] */ IXCLRDataModule **mod)
{
    HRESULT status;

    DAC_ENTER();

    EX_TRY
    {
        ProcessModIter* iter = FROM_CDENUM(ProcessModIter, *handle);
        Module* curMod;

        if ((curMod = iter->NextModule()))
        {
            *mod = new (nothrow)
                ClrDataModule(this, curMod);
            status = *mod ? S_OK : E_OUTOFMEMORY;
        }
        else
        {
            status = S_FALSE;
        }
    }
    EX_CATCH
    {
        if (!DacExceptionFilter(GET_EXCEPTION(), this, &status))
        {
            EX_RETHROW;
        }
    }
    EX_END_CATCH(SwallowAllExceptions)

    DAC_LEAVE();
    return status;
}

HRESULT STDMETHODCALLTYPE
ClrDataAccess::EndEnumModules(
    /* [in] */ CLRDATA_ENUM handle)
{
    HRESULT status;

    DAC_ENTER();

    EX_TRY
    {
        ProcessModIter* iter = FROM_CDENUM(ProcessModIter, handle);
        delete iter;
        status = S_OK;
    }
    EX_CATCH
    {
        if (!DacExceptionFilter(GET_EXCEPTION(), this, &status))
        {
            EX_RETHROW;
        }
    }
    EX_END_CATCH(SwallowAllExceptions)

    DAC_LEAVE();
    return status;
}

HRESULT STDMETHODCALLTYPE
ClrDataAccess::GetModuleByAddress(
    /* [in] */ CLRDATA_ADDRESS address,
    /* [out] */ IXCLRDataModule** mod)
{
    HRESULT status;

    DAC_ENTER();

    EX_TRY
    {
        ProcessModIter modIter;
        Module* modDef;

        while ((modDef = modIter.NextModule()))
        {
            TADDR base;
            ULONG32 length;
            PEFile* file = modDef->GetFile();

            if ((base = (TADDR)file->GetLoadedImageContents(&length)))
            {
                if (TO_CDADDR(base) <= address &&
                    TO_CDADDR(base + length) > address)
                {
                    break;
                }
            }

            if (file->HasNativeImage())
            {
                base = (TADDR)file->GetLoadedNative()->GetBase();
                length = file->GetLoadedNative()->GetVirtualSize();
                if (TO_CDADDR(base) <= address &&
                    TO_CDADDR(base + length) > address)
                {
                    break;
                }
            }
        }

        if (modDef)
        {
            *mod = new (nothrow)
                ClrDataModule(this, modDef);
            status = *mod ? S_OK : E_OUTOFMEMORY;
        }
        else
        {
            status = S_FALSE;
        }
    }
    EX_CATCH
    {
        if (!DacExceptionFilter(GET_EXCEPTION(), this, &status))
        {
            EX_RETHROW;
        }
    }
    EX_END_CATCH(SwallowAllExceptions)

    DAC_LEAVE();
    return status;
}

HRESULT STDMETHODCALLTYPE
ClrDataAccess::StartEnumMethodDefinitionsByAddress(
    /* [in] */ CLRDATA_ADDRESS address,
    /* [out] */ CLRDATA_ENUM *handle)
{
    HRESULT status;

    DAC_ENTER();

    EX_TRY
    {
        ProcessModIter modIter;
        Module* modDef;

        while ((modDef = modIter.NextModule()))
        {
            TADDR base;
            ULONG32 length;
            PEFile* file = modDef->GetFile();

            if ((base = (TADDR)file->GetLoadedImageContents(&length)))
            {
                if (TO_CDADDR(base) <= address &&
                    TO_CDADDR(base + length) > address)
                {
                    break;
                }
            }
        }

        status = EnumMethodDefinitions::
            CdStart(modDef, true, address, handle);
    }
    EX_CATCH
    {
        if (!DacExceptionFilter(GET_EXCEPTION(), this, &status))
        {
            EX_RETHROW;
        }
    }
    EX_END_CATCH(SwallowAllExceptions)

    DAC_LEAVE();
    return status;
}

HRESULT STDMETHODCALLTYPE
ClrDataAccess::EnumMethodDefinitionByAddress(
    /* [out][in] */ CLRDATA_ENUM* handle,
    /* [out] */ IXCLRDataMethodDefinition **method)
{
    HRESULT status;

    DAC_ENTER();

    EX_TRY
    {
        status = EnumMethodDefinitions::CdNext(this, handle, method);
    }
    EX_CATCH
    {
        if (!DacExceptionFilter(GET_EXCEPTION(), this, &status))
        {
            EX_RETHROW;
        }
    }
    EX_END_CATCH(SwallowAllExceptions)

    DAC_LEAVE();
    return status;
}

HRESULT STDMETHODCALLTYPE
ClrDataAccess::EndEnumMethodDefinitionsByAddress(
    /* [in] */ CLRDATA_ENUM handle)
{
    HRESULT status;

    DAC_ENTER();

    EX_TRY
    {
        status = EnumMethodDefinitions::CdEnd(handle);
    }
    EX_CATCH
    {
        if (!DacExceptionFilter(GET_EXCEPTION(), this, &status))
        {
            EX_RETHROW;
        }
    }
    EX_END_CATCH(SwallowAllExceptions)

    DAC_LEAVE();
    return status;
}

HRESULT STDMETHODCALLTYPE
ClrDataAccess::StartEnumMethodInstancesByAddress(
    /* [in] */ CLRDATA_ADDRESS address,
    /* [in] */ IXCLRDataAppDomain* appDomain,
    /* [out] */ CLRDATA_ENUM *handle)
{
    HRESULT status;

    DAC_ENTER();

    EX_TRY
    {
        IJitManager* jitMan;
        MethodDesc* methodDesc;

        *handle = 0;
        status = S_FALSE;

        if (IsPossibleCodeAddress(address) != S_OK)
        {
            goto Exit;
        }

        jitMan = ExecutionManager::FindJitMan(TO_SLOT(TO_TADDR(address)),
                                              IJitManager::ScanNoReaderLock);
        if (!jitMan)
        {
            goto Exit;
        }

        jitMan->JitCodeToMethodInfo(TO_SLOT(TO_TADDR(address)),
                                    &methodDesc, NULL, NULL,
                                    IJitManager::ScanNoReaderLock);
        if (!methodDesc)
        {
            goto Exit;
        }

        status = EnumMethodInstances::CdStart(methodDesc, appDomain,
                                              handle);

    Exit: ;
    }
    EX_CATCH
    {
        if (!DacExceptionFilter(GET_EXCEPTION(), this, &status))
        {
            EX_RETHROW;
        }
    }
    EX_END_CATCH(SwallowAllExceptions)

    DAC_LEAVE();
    return status;
}

HRESULT STDMETHODCALLTYPE
ClrDataAccess::EnumMethodInstanceByAddress(
    /* [out][in] */ CLRDATA_ENUM* handle,
    /* [out] */ IXCLRDataMethodInstance **method)
{
    HRESULT status;

    DAC_ENTER();

    EX_TRY
    {
        status = EnumMethodInstances::CdNext(this, handle, method);
    }
    EX_CATCH
    {
        if (!DacExceptionFilter(GET_EXCEPTION(), this, &status))
        {
            EX_RETHROW;
        }
    }
    EX_END_CATCH(SwallowAllExceptions)

    DAC_LEAVE();
    return status;
}

HRESULT STDMETHODCALLTYPE
ClrDataAccess::EndEnumMethodInstancesByAddress(
    /* [in] */ CLRDATA_ENUM handle)
{
    HRESULT status;

    DAC_ENTER();

    EX_TRY
    {
        status = EnumMethodInstances::CdEnd(handle);
    }
    EX_CATCH
    {
        if (!DacExceptionFilter(GET_EXCEPTION(), this, &status))
        {
            EX_RETHROW;
        }
    }
    EX_END_CATCH(SwallowAllExceptions)

    DAC_LEAVE();
    return status;
}

HRESULT STDMETHODCALLTYPE
ClrDataAccess::GetDataByAddress(
    /* [in] */ CLRDATA_ADDRESS address,
    /* [in] */ ULONG32 flags,
    /* [in] */ IXCLRDataAppDomain* appDomain,
    /* [in] */ IXCLRDataTask* tlsTask,
    /* [in] */ ULONG32 bufLen,
    /* [out] */ ULONG32 *nameLen,
    /* [size_is][out] */ WCHAR nameBuf[  ],
    /* [out] */ IXCLRDataValue **value,
    /* [out] */ CLRDATA_ADDRESS *displacement)
{
    HRESULT status;

    if (flags != 0)
    {
        return E_INVALIDARG;
    }

    DAC_ENTER();

    EX_TRY
    {
        // XXX drewb.
        status = E_NOTIMPL;
    }
    EX_CATCH
    {
        if (!DacExceptionFilter(GET_EXCEPTION(), this, &status))
        {
            EX_RETHROW;
        }
    }
    EX_END_CATCH(SwallowAllExceptions)

    DAC_LEAVE();
    return status;
}

HRESULT STDMETHODCALLTYPE
ClrDataAccess::GetExceptionStateByExceptionRecord(
    /* [in] */ EXCEPTION_RECORD64 *record,
    /* [out] */ IXCLRDataExceptionState **exception)
{
    HRESULT status;

    DAC_ENTER();

    EX_TRY
    {
        // XXX drewb.
        status = E_NOTIMPL;
    }
    EX_CATCH
    {
        if (!DacExceptionFilter(GET_EXCEPTION(), this, &status))
        {
            EX_RETHROW;
        }
    }
    EX_END_CATCH(SwallowAllExceptions)

    DAC_LEAVE();
    return status;
}

HRESULT STDMETHODCALLTYPE
ClrDataAccess::TranslateExceptionRecordToNotification(
    /* [in] */ EXCEPTION_RECORD64 *record,
    /* [in] */ IXCLRDataExceptionNotification *notify)
{
    HRESULT status = E_FAIL;
    ClrDataModule* pubModule = NULL;
    ClrDataMethodInstance* pubMethodInst = NULL;
    ClrDataExceptionState* pubExState = NULL;
    ULONG32 notifyType = 0;

    DAC_ENTER();

    EX_TRY
    {
        //
        // We cannot hold the dac lock while calling
        // out as the external code can do arbitrary things.
        // Instead we make a pass over the exception
        // information and create all necessary objects.
        // We then leave the lock and make the callbac.
        //

        TADDR exInfo[EXCEPTION_MAXIMUM_PARAMETERS];
        for (UINT i = 0; i < EXCEPTION_MAXIMUM_PARAMETERS; i++)
        {
            exInfo[i] = TO_TADDR(record->ExceptionInformation[i]);
        }

        notifyType = DACNotify::GetType(exInfo);
        switch(notifyType)
        {
        case DACNotify::MODULE_LOAD_NOTIFICATION:
        {
            TADDR modulePtr;

            if (DACNotify::ParseModuleLoadNotification(exInfo, modulePtr))
            {
                Module* clrModule = PTR_Module(modulePtr);
                pubModule = new (nothrow) ClrDataModule(this, clrModule);
                if (pubModule == NULL)
                {
                    status = E_OUTOFMEMORY;
                }
                else
                {
                    status = S_OK;
                }
            }
            break;
        }

        case DACNotify::MODULE_UNLOAD_NOTIFICATION:
        {
            TADDR modulePtr;

            if (DACNotify::ParseModuleUnloadNotification(exInfo, modulePtr))
            {
                Module* clrModule = PTR_Module(modulePtr);
                pubModule = new (nothrow) ClrDataModule(this, clrModule);
                if (pubModule == NULL)
                {
                    status = E_OUTOFMEMORY;
                }
                else
                {
                    status = S_OK;
                }
            }
            break;
        }

        case DACNotify::JIT_NOTIFICATION:
        {
            TADDR methodDescPtr;

            if (DACNotify::ParseJITNotification(exInfo, methodDescPtr))
            {
                // Try and find the right appdomain
                MethodDesc* methodDesc = PTR_MethodDesc(methodDescPtr);
                BaseDomain* baseDomain = methodDesc->GetDomain();
                AppDomain* appDomain = NULL;

                if (baseDomain->IsAppDomain())
                {
                    appDomain = PTR_AppDomain(PTR_HOST_TO_TADDR(baseDomain));
                }
                else
                {
                    // Find a likely domain, because it's the shared domain.
                    AppDomainIterator adi(FALSE);
                    appDomain = adi.GetDomain();
                }

                pubMethodInst =
                    new (nothrow) ClrDataMethodInstance(this,
                                                        appDomain,
                                                        methodDesc);
                if (pubMethodInst == NULL)
                {
                    status = E_OUTOFMEMORY;
                }
                else
                {
                    status = S_OK;
                }
            }
            break;
        }

        case DACNotify::EXCEPTION_NOTIFICATION:
        {
            TADDR threadPtr;

            if (DACNotify::ParseExceptionNotification(exInfo, threadPtr))
            {
                // Translation can only occur at the time of
                // receipt of the notify exception, so we assume
                // that the Thread's current exception state
                // is the state we want.
                status = ClrDataExceptionState::
                    NewFromThread(this,
                                  PTR_Thread(threadPtr),
                                  &pubExState,
                                  NULL);
            }
            break;
        }

        default:
            status = E_INVALIDARG;
            break;
        }
    }
    EX_CATCH
    {
        if (!DacExceptionFilter(GET_EXCEPTION(), this, &status))
        {
            EX_RETHROW;
        }
    }
    EX_END_CATCH(SwallowAllExceptions)

    DAC_LEAVE();

    if (status == S_OK)
    {
        IXCLRDataExceptionNotification2* notify2;

        if (notify->QueryInterface(__uuidof(IXCLRDataExceptionNotification),
                                   (void**)&notify2) != S_OK)
        {
            notify2 = NULL;
        }

        switch(notifyType)
        {
        case DACNotify::MODULE_LOAD_NOTIFICATION:
            notify->OnModuleLoaded(pubModule);
            break;

        case DACNotify::MODULE_UNLOAD_NOTIFICATION:
            notify->OnModuleUnloaded(pubModule);
            break;

        case DACNotify::JIT_NOTIFICATION:
            notify->OnCodeGenerated(pubMethodInst);
            break;

        case DACNotify::EXCEPTION_NOTIFICATION:
            if (notify2)
            {
                notify2->OnException(pubExState);
            }
            else
            {
                status = E_INVALIDARG;
            }
            break;

        default:
            // notifyType has already been validated.
            _ASSERTE(FALSE);
            break;
        }

        if (notify2)
        {
            notify2->Release();
        }
    }

    if (pubModule)
    {
        pubModule->Release();
    }
    if (pubMethodInst)
    {
        pubMethodInst->Release();
    }
    if (pubExState)
    {
        pubExState->Release();
    }

    return status;
}

HRESULT STDMETHODCALLTYPE
ClrDataAccess::CreateMemoryValue(
    /* [in] */ IXCLRDataAppDomain* appDomain,
    /* [in] */ IXCLRDataTask* tlsTask,
    /* [in] */ IXCLRDataTypeInstance* type,
    /* [in] */ CLRDATA_ADDRESS addr,
    /* [out] */ IXCLRDataValue** value)
{
    HRESULT status;

    DAC_ENTER();

    EX_TRY
    {
        AppDomain* dacDomain;
        Thread* dacThread;
        TypeHandle dacType;
        ULONG32 flags;
        NativeVarLocation loc;

        dacDomain = ((ClrDataAppDomain*)appDomain)->GetAppDomain();
        if (tlsTask)
        {
            dacThread = ((ClrDataTask*)tlsTask)->GetThread();
        }
        else
        {
            dacThread = NULL;
        }
        dacType = ((ClrDataTypeInstance*)type)->GetTypeHandle();

        flags = GetTypeFieldValueFlags(dacType, NULL, 0, false);

        loc.addr = addr;
        loc.size = dacType.GetSize();
        loc.contextReg = false;

        *value = new (nothrow)
            ClrDataValue(this, dacDomain, dacThread, flags,
                         dacType, addr, 1, &loc);
        status = *value ? S_OK : E_OUTOFMEMORY;
    }
    EX_CATCH
    {
        if (!DacExceptionFilter(GET_EXCEPTION(), this, &status))
        {
            EX_RETHROW;
        }
    }
    EX_END_CATCH(SwallowAllExceptions)

    DAC_LEAVE();
    return status;
}

HRESULT STDMETHODCALLTYPE
ClrDataAccess::SetAllTypeNotifications(
    /* [in] */ IXCLRDataModule* mod,
    /* [in] */ ULONG32 flags)
{
    HRESULT status;

    DAC_ENTER();

    EX_TRY
    {
        // XXX drewb.
        status = E_NOTIMPL;
    }
    EX_CATCH
    {
        if (!DacExceptionFilter(GET_EXCEPTION(), this, &status))
        {
            EX_RETHROW;
        }
    }
    EX_END_CATCH(SwallowAllExceptions)

    DAC_LEAVE();
    return status;
}

HRESULT STDMETHODCALLTYPE
ClrDataAccess::SetAllCodeNotifications(
    /* [in] */ IXCLRDataModule* mod,
    /* [in] */ ULONG32 flags)
{
    HRESULT status;

    DAC_ENTER();

    EX_TRY
    {
        status = E_FAIL;

        if (!IsValidMethodCodeNotification(flags))
        {
            status = E_INVALIDARG;
        }
        else
        {
            JITNotifications jn(GetHostJitNotificationTable());
            if (!jn.IsActive())
            {
                status = E_OUTOFMEMORY;
            }
            else
            {
                BOOL changedTable;
                TADDR modulePtr = mod ?
                    PTR_HOST_TO_TADDR(((ClrDataModule*)mod)->GetModule()) :
                    NULL;

                if (jn.SetAllNotifications(modulePtr, flags, &changedTable))
                {
                    if (!changedTable ||
                        (changedTable && jn.UpdateOutOfProcTable()))
                    {
                        status = S_OK;
                    }
                }
            }
        }
    }
    EX_CATCH
    {
        if (!DacExceptionFilter(GET_EXCEPTION(), this, &status))
        {
            EX_RETHROW;
        }
    }
    EX_END_CATCH(SwallowAllExceptions)

    DAC_LEAVE();
    return status;
}

HRESULT STDMETHODCALLTYPE
ClrDataAccess::GetTypeNotifications(
    /* [in] */ ULONG32 numTokens,
    /* [in, size_is(numTokens)] */ IXCLRDataModule* mods[],
    /* [in] */ IXCLRDataModule* singleMod,
    /* [in, size_is(numTokens)] */ mdTypeDef tokens[],
    /* [out, size_is(numTokens)] */ ULONG32 flags[])
{
    HRESULT status;

    DAC_ENTER();

    EX_TRY
    {
        // XXX drewb.
        status = E_NOTIMPL;
    }
    EX_CATCH
    {
        if (!DacExceptionFilter(GET_EXCEPTION(), this, &status))
        {
            EX_RETHROW;
        }
    }
    EX_END_CATCH(SwallowAllExceptions)

    DAC_LEAVE();
    return status;
}

HRESULT STDMETHODCALLTYPE
ClrDataAccess::SetTypeNotifications(
    /* [in] */ ULONG32 numTokens,
    /* [in, size_is(numTokens)] */ IXCLRDataModule* mods[],
    /* [in] */ IXCLRDataModule* singleMod,
    /* [in, size_is(numTokens)] */ mdTypeDef tokens[],
    /* [in, size_is(numTokens)] */ ULONG32 flags[],
    /* [in] */ ULONG32 singleFlags)
{
    HRESULT status;

    DAC_ENTER();

    EX_TRY
    {
        // XXX drewb.
        status = E_NOTIMPL;
    }
    EX_CATCH
    {
        if (!DacExceptionFilter(GET_EXCEPTION(), this, &status))
        {
            EX_RETHROW;
        }
    }
    EX_END_CATCH(SwallowAllExceptions)

    DAC_LEAVE();
    return status;
}

HRESULT STDMETHODCALLTYPE
ClrDataAccess::GetCodeNotifications(
    /* [in] */ ULONG32 numTokens,
    /* [in, size_is(numTokens)] */ IXCLRDataModule* mods[],
    /* [in] */ IXCLRDataModule* singleMod,
    /* [in, size_is(numTokens)] */ mdMethodDef tokens[],
    /* [out, size_is(numTokens)] */ ULONG32 flags[])
{
    HRESULT status;

    DAC_ENTER();

    EX_TRY
    {
        if ((flags == NULL || tokens == NULL) ||
            (mods == NULL && singleMod == NULL) ||
            (mods != NULL && singleMod != NULL))
        {
            status = E_INVALIDARG;
        }
        else
        {
            JITNotifications jn(GetHostJitNotificationTable());
            if (!jn.IsActive())
            {
                status = E_OUTOFMEMORY;
            }
            else
            {
                TADDR modulePtr = NULL;
                if (singleMod)
                {
                    modulePtr = PTR_HOST_TO_TADDR(((ClrDataModule*)singleMod)->
                                                  GetModule());
                }

                for (ULONG32 i = 0; i < numTokens; i++)
                {
                    if (singleMod == NULL)
                    {
                        modulePtr =
                            PTR_HOST_TO_TADDR(((ClrDataModule*)mods[i])->
                                              GetModule());
                    }
                    USHORT jt = jn.Requested(modulePtr, tokens[i]);
                    flags[i] = jt;
                }

                status = S_OK;
            }
        }
    }
    EX_CATCH
    {
        if (!DacExceptionFilter(GET_EXCEPTION(), this, &status))
        {
            EX_RETHROW;
        }
    }
    EX_END_CATCH(SwallowAllExceptions)

    DAC_LEAVE();
    return status;
}

HRESULT STDMETHODCALLTYPE
ClrDataAccess::SetCodeNotifications(
    /* [in] */ ULONG32 numTokens,
    /* [in, size_is(numTokens)] */ IXCLRDataModule* mods[],
    /* [in] */ IXCLRDataModule* singleMod,
    /* [in, size_is(numTokens)] */ mdMethodDef tokens[],
    /* [in, size_is(numTokens)] */ ULONG32 flags[],
    /* [in] */ ULONG32 singleFlags)
{
    HRESULT status;

    DAC_ENTER();

    EX_TRY
    {
        if ((tokens == NULL) ||
            (mods == NULL && singleMod == NULL) ||
            (mods != NULL && singleMod != NULL))
        {
            status = E_INVALIDARG;
        }
        else
        {
            JITNotifications jn(GetHostJitNotificationTable());
            if (!jn.IsActive() || numTokens > jn.GetTableSize())
            {
                status = E_OUTOFMEMORY;
            }
            else
            {
                BOOL changedTable = FALSE;

                // Are flags valid?
                if (flags)
                {
                    for (ULONG32 check = 0; check < numTokens; check++)
                    {
                        if (!IsValidMethodCodeNotification(flags[check]))
                        {
                            status = E_INVALIDARG;
                            goto Exit;
                        }
                    }
                }
                else if (!IsValidMethodCodeNotification(singleFlags))
                {
                    status = E_INVALIDARG;
                    goto Exit;
                }

                TADDR modulePtr = NULL;
                if (singleMod)
                {
                    modulePtr =
                        PTR_HOST_TO_TADDR(((ClrDataModule*)singleMod)->
                                          GetModule());
                }

                for (ULONG32 i = 0; i < numTokens; i++)
                {
                    if (singleMod == NULL)
                    {
                        modulePtr =
                            PTR_HOST_TO_TADDR(((ClrDataModule*)mods[i])->
                                              GetModule());
                    }

                    USHORT curFlags = jn.Requested(modulePtr, tokens[i]);
                    USHORT setFlags = (USHORT)(flags ? flags[i] : singleFlags);

                    if (curFlags != setFlags)
                    {
                        if (!jn.SetNotification(modulePtr, tokens[i],
                                                setFlags))
                        {
                            status = E_FAIL;
                            goto Exit;
                        }

                        changedTable = TRUE;
                    }
                }

                if (!changedTable ||
                    (changedTable && jn.UpdateOutOfProcTable()))
                {
                    status = S_OK;
                }
            }
        }

Exit: ;
    }
    EX_CATCH
    {
        if (!DacExceptionFilter(GET_EXCEPTION(), this, &status))
        {
            EX_RETHROW;
        }
    }
    EX_END_CATCH(SwallowAllExceptions)

    DAC_LEAVE();
    return status;
}

HRESULT
ClrDataAccess::GetOtherNotificationFlags(
    /* [out] */ ULONG32* flags)
{
    HRESULT status;

    DAC_ENTER();

    EX_TRY
    {
        *flags = g_dacNotificationFlags;
        status = S_OK;
    }
    EX_CATCH
    {
        if (!DacExceptionFilter(GET_EXCEPTION(), this, &status))
        {
            EX_RETHROW;
        }
    }
    EX_END_CATCH(SwallowAllExceptions)

    DAC_LEAVE();
    return status;
}

HRESULT
ClrDataAccess::SetOtherNotificationFlags(
    /* [in] */ ULONG32 flags)
{
    HRESULT status;

    if ((flags & ~(CLRDATA_NOTIFY_ON_MODULE_LOAD |
                   CLRDATA_NOTIFY_ON_MODULE_UNLOAD |
                   CLRDATA_NOTIFY_ON_EXCEPTION)) != 0)
    {
        return E_INVALIDARG;
    }

    DAC_ENTER();

    EX_TRY
    {
        g_dacNotificationFlags = flags;
        status = S_OK;
    }
    EX_CATCH
    {
        if (!DacExceptionFilter(GET_EXCEPTION(), this, &status))
        {
            EX_RETHROW;
        }
    }
    EX_END_CATCH(SwallowAllExceptions)

    DAC_LEAVE();
    return status;
}

enum
{
    STUB_BUF_FLAGS_START,

    STUB_BUF_METHOD_JITTED,
    STUB_BUF_FRAME_PUSHED,
    STUB_BUF_STUB_MANAGER_PUSHED,

    STUB_BUF_FLAGS_END,
};

union STUB_BUF
{
    CLRDATA_FOLLOW_STUB_BUFFER apiBuf;
    struct
    {
        ULONG64 flags;
        ULONG64 addr;
        ULONG64 arg1;
    } u;
};

HRESULT
ClrDataAccess::FollowStubStep(
    /* [in] */ Thread* thread,
    /* [in] */ ULONG32 inFlags,
    /* [in] */ TADDR inAddr,
    /* [in] */ union STUB_BUF* inBuffer,
    /* [out] */ TADDR* outAddr,
    /* [out] */ union STUB_BUF* outBuffer,
    /* [out] */ ULONG32* outFlags)
{
    TraceDestination trace;
    bool traceDone = false;
    BYTE* retAddr;
    CONTEXT localContext;
    REGDISPLAY regDisp;
    MethodDesc* methodDesc;

    ZeroMemory(outBuffer, sizeof(*outBuffer));

    if (inBuffer)
    {
        switch(inBuffer->u.flags)
        {
        case STUB_BUF_METHOD_JITTED:
            if (inAddr != GFN_TADDR(DACNotifyCompilationFinished))
            {
                return E_INVALIDARG;
            }

            // It's possible that this notification is
            // for a different method, so double-check
            // and recycle the notification if necessary.
            methodDesc = PTR_MethodDesc(inBuffer->u.addr);
            if (methodDesc->HasNativeCode())
            {
                *outAddr = methodDesc->GetNativeCode();
                *outFlags = CLRDATA_FOLLOW_STUB_EXIT;
                return S_OK;
            }

            // We didn't end up with native code so try again.
            trace.InitForUnjittedMethod(methodDesc);
            traceDone = true;
            break;

        case STUB_BUF_FRAME_PUSHED:
            if (!thread ||
                inAddr != inBuffer->u.addr)
            {
                return E_INVALIDARG;
            }

            trace.InitForFramePush(inBuffer->u.addr);
            DacGetThreadContext(thread, &localContext);
            thread->FillRegDisplay(&regDisp, &localContext);
            if (!thread->GetFrame()->
                TraceFrame(thread,
                           TRUE,
                           &trace,
                           &regDisp))
            {
                return E_FAIL;
            }

            traceDone = true;
            break;

        case STUB_BUF_STUB_MANAGER_PUSHED:
            if (!thread ||
                inAddr != inBuffer->u.addr ||
                !inBuffer->u.arg1)
            {
                return E_INVALIDARG;
            }

            trace.InitForManagerPush(inBuffer->u.addr,
                                     PTR_StubManager(inBuffer->u.arg1));
            DacGetThreadContext(thread, &localContext);
            if (!trace.GetStubManager()->
                TraceManager(thread,
                             &trace,
                             &localContext,
                             &retAddr))
            {
                return E_FAIL;
            }

            traceDone = true;
            break;

        default:
            return E_INVALIDARG;
        }
    }

    if ((!traceDone &&
         !StubManager::TraceStub(inAddr, &trace)) ||
        !StubManager::FollowTrace(&trace))
    {
        return E_NOINTERFACE;
    }

    switch(trace.GetTraceType())
    {
    case TRACE_UNMANAGED:
    case TRACE_MANAGED:
        // We've hit non-stub code so we're done.
        *outAddr = trace.GetAddress();
        *outFlags = CLRDATA_FOLLOW_STUB_EXIT;
        break;

    case TRACE_UNJITTED_METHOD:
        // The stub causes jitting, so return
        // the address of the jit-complete routine
        // so that the real native address can
        // be picked up once the JIT is done.

        // One special case is ngen'ed code that
        // needs the prestub run.  This results in
        // an unjitted trace but no jitting will actually
        // occur since the code is ngen'ed.  Detect
        // this and redirect to the actual code.
        methodDesc = trace.GetMethodDesc();
        if (methodDesc->IsPreImplemented() &&
            !methodDesc->IsPointingToNativeCode() &&
            !methodDesc->IsGenericMethodDefinition() &&
            methodDesc->HasNativeCode())
        {
            *outAddr = methodDesc->GetNativeCode();
            *outFlags = CLRDATA_FOLLOW_STUB_EXIT;
            break;
        }

        *outAddr = GFN_TADDR(DACNotifyCompilationFinished);
        outBuffer->u.flags = STUB_BUF_METHOD_JITTED;
        outBuffer->u.addr = PTR_HOST_TO_TADDR(methodDesc);
        *outFlags = CLRDATA_FOLLOW_STUB_INTERMEDIATE;
        break;

    case TRACE_FRAME_PUSH:
        if (!thread)
        {
            return E_INVALIDARG;
        }

        *outAddr = trace.GetAddress();
        outBuffer->u.flags = STUB_BUF_FRAME_PUSHED;
        outBuffer->u.addr = trace.GetAddress();
        *outFlags = CLRDATA_FOLLOW_STUB_INTERMEDIATE;
        break;

    case TRACE_MGR_PUSH:
        if (!thread)
        {
            return E_INVALIDARG;
        }

        *outAddr = trace.GetAddress();
        outBuffer->u.flags = STUB_BUF_STUB_MANAGER_PUSHED;
        outBuffer->u.addr = trace.GetAddress();
        outBuffer->u.arg1 = PTR_HOST_TO_TADDR(trace.GetStubManager());
        *outFlags = CLRDATA_FOLLOW_STUB_INTERMEDIATE;
        break;

    default:
        return E_INVALIDARG;
    }

    return S_OK;
}

HRESULT STDMETHODCALLTYPE
ClrDataAccess::FollowStub(
    /* [in] */ ULONG32 inFlags,
    /* [in] */ CLRDATA_ADDRESS inAddr,
    /* [in] */ CLRDATA_FOLLOW_STUB_BUFFER* _inBuffer,
    /* [out] */ CLRDATA_ADDRESS* outAddr,
    /* [out] */ CLRDATA_FOLLOW_STUB_BUFFER* _outBuffer,
    /* [out] */ ULONG32* outFlags)
{
    return FollowStub2(NULL, inFlags, inAddr, _inBuffer,
                       outAddr, _outBuffer, outFlags);
}

HRESULT STDMETHODCALLTYPE
ClrDataAccess::FollowStub2(
    /* [in] */ IXCLRDataTask* task,
    /* [in] */ ULONG32 inFlags,
    /* [in] */ CLRDATA_ADDRESS _inAddr,
    /* [in] */ CLRDATA_FOLLOW_STUB_BUFFER* _inBuffer,
    /* [out] */ CLRDATA_ADDRESS* _outAddr,
    /* [out] */ CLRDATA_FOLLOW_STUB_BUFFER* _outBuffer,
    /* [out] */ ULONG32* outFlags)
{
    HRESULT status;

    if ((inFlags & ~(CLRDATA_FOLLOW_STUB_DEFAULT)) != 0)
    {
        return E_INVALIDARG;
    }

    STUB_BUF* inBuffer = (STUB_BUF*)_inBuffer;
    STUB_BUF* outBuffer = (STUB_BUF*)_outBuffer;

    if (inBuffer &&
        (inBuffer->u.flags <= STUB_BUF_FLAGS_START ||
         inBuffer->u.flags >= STUB_BUF_FLAGS_END))
    {
        return E_INVALIDARG;
    }

    DAC_ENTER();

    EX_TRY
    {
        STUB_BUF cycleBuf;
        TADDR inAddr = TO_TADDR(_inAddr);
        TADDR outAddr;
        Thread* thread = task ? ((ClrDataTask*)task)->GetThread() : NULL;
        ULONG32 loops = 4;

        for (;;)
        {
            if ((status = FollowStubStep(thread,
                                         inFlags,
                                         inAddr,
                                         inBuffer,
                                         &outAddr,
                                         outBuffer,
                                         outFlags)) != S_OK)
            {
                break;
            }

            // Some stub tracing just requests further iterations
            // of processing, so detect that case and loop.
            if (outAddr != inAddr)
            {
                // We can make forward progress, we're done.
                *_outAddr = TO_CDADDR(outAddr);
                break;
            }

            // We need more processing.  As a protection
            // against infinite loops in corrupted or buggy
            // situations, we only allow this to happen a
            // small number of times.
            if (--loops == 0)
            {
                ZeroMemory(outBuffer, sizeof(*outBuffer));
                status = E_FAIL;
                break;
            }

            cycleBuf = *outBuffer;
            inBuffer = &cycleBuf;
        }
    }
    EX_CATCH
    {
        if (!DacExceptionFilter(GET_EXCEPTION(), this, &status))
        {
            EX_RETHROW;
        }
    }
    EX_END_CATCH(SwallowAllExceptions)

    DAC_LEAVE();
    return status;
}

HRESULT
ClrDataAccess::Initialize(void)
{
    HRESULT status;
    CLRDATA_ADDRESS base;

    // We don't need to try too hard to prevent
    // multiple initializations as each one will
    // copy the same data into the globals and so
    // cannot interfere with each other.
    if (!s_procInit)
    {
        if ((status = GetDacGlobals()) != S_OK)
        {
            return status;
        }
        if ((status = DacGetHostVtPtrs()) != S_OK)
        {
            return status;
        }

        s_procInit = true;
    }

    //
    // We do not currently support cross-platform
    // debugging.  Verify that cross-platform is not
    // being attempted.
    //

#if defined(_X86_)
    ULONG32 compilationMachine = IMAGE_FILE_MACHINE_I386;
#elif defined(_PPC_)
    // This isn't quite the correct value to use, since this is
    // for little-endian PPC
    ULONG32 compilationMachine = IMAGE_FILE_MACHINE_POWERPC;
#else
#error Unknown processor.
#endif

    ULONG32 compilationPointer = sizeof(TADDR);

    ULONG32 machineType;
    ULONG32 pointerSize;

    if ((status = m_target->GetMachineType(&machineType)) != S_OK ||
        (status = m_target->GetPointerSize(&pointerSize)) != S_OK)
    {
        return status;
    }

    if (machineType != compilationMachine ||
        pointerSize != compilationPointer)
    {
        return E_INVALIDARG;
    }

    //
    // Get the current DLL base for mscorwks globals.
    //

    if ((status = m_target->GetImageBase(L"mscorwks.dll", &base)) != S_OK)
    {
        return status;
    }

    m_globalBase = TO_TADDR(base);

    return S_OK;
}

Thread*
ClrDataAccess::FindClrThreadByTaskId(ULONG64 taskId)
{
    Thread* thread = NULL;

    if (!ThreadStore::s_pThreadStore)
    {
        return NULL;
    }

    while ((thread = ThreadStore::GetAllThreadList(thread, 0, 0)))
    {
        if (thread->GetThreadId() == (DWORD)taskId)
        {
            return thread;
        }
    }

    return NULL;
}

HRESULT
ClrDataAccess::IsPossibleCodeAddress(IN CLRDATA_ADDRESS address)
{
    BYTE testRead;
    ULONG32 testDone;

    // First do a trivial check on the readability of the
    // address.  This makes for quick rejection of bogus
    // addresses that the debugger sends in when searching
    // stacks for return addresses.
    // XXX drewb - Will this cause problems in minidumps
    // where it's possible the stub is identifiable but
    // the stub code isn't present?  Yes, but the lack
    // of that code could confuse the walker on its own
    // if it does code analysis.
    if ((m_target->ReadVirtual(address, &testRead, sizeof(testRead),
                               &testDone) != S_OK) ||
        !testDone)
    {
        return E_INVALIDARG;
    }

    return S_OK;
}

HRESULT
ClrDataAccess::GetFullMethodName(
    IN MethodDesc* methodDesc,
    IN ULONG32 symbolChars,
    IN ULONG32* symbolLen,
    __inout_ecount_part_opt(symbolChars, symbolLen) LPWSTR symbol
    )
{
    StackSString s;
    TypeString::AppendMethodInternal(s, methodDesc, TypeString::FormatSignature|TypeString::FormatNamespace|TypeString::FormatFullInst);

    if (symbol)
    {
        // Copy as much as we can and truncate the rest.
        wcsncpy_s(symbol, symbolChars, s.GetUnicode(), _TRUNCATE);
    }

    if (symbolLen)
        *symbolLen = s.GetCount() + 1;

    if (symbol != NULL && symbolChars < (s.GetCount() + 1))
        return S_FALSE;
    else
        return S_OK;
}

HRESULT
ClrDataAccess::RawGetMethodName(
    /* [in] */ CLRDATA_ADDRESS address,
    /* [in] */ ULONG32 flags,
    /* [in] */ ULONG32 bufLen,
    /* [out] */ ULONG32 *symbolLen,
    /* [size_is][out] */ WCHAR symbolBuf[  ],
    /* [out] */ CLRDATA_ADDRESS* displacement)
{
    HRESULT status;

    if (flags != 0)
    {
        return E_INVALIDARG;
    }

    if ((status = IsPossibleCodeAddress(address)) != S_OK)
    {
        return status;
    }

    MethodDesc* methodDesc = NULL;

    IJitManager* jitMan =
        ExecutionManager::FindJitMan(TO_SLOT(TO_TADDR(address)),
                                     IJitManager::ScanNoReaderLock);
    if (jitMan)
    {
        DWORD codeOffs;

        jitMan->JitCodeToMethodInfo(TO_SLOT(TO_TADDR(address)),
                                    &methodDesc, NULL, &codeOffs,
                                    IJitManager::ScanNoReaderLock);
        if (!methodDesc)
        {
            return E_NOINTERFACE;
        }

        if (displacement)
        {
            *displacement = codeOffs;
        }

        goto NameFromMethodDesc;
    }

    {
        TADDR alignedAddress = AlignDown(address, PRECODE_ALIGNMENT);

        for (int i = 0; i < (int)(sizeof(Precode) / PRECODE_ALIGNMENT); i++)
        {
            // Try to find matching precode entrypoint
            if (MethodDescPrestubManager::g_pManager->CheckIsStub_Worker(TO_TADDR(alignedAddress)))
            {
                Precode* pPrecode = Precode::GetPrecodeFromEntryPoint(TO_TADDR(alignedAddress), TRUE);
                if (pPrecode != NULL)
                {
                    methodDesc = pPrecode->GetMethodDesc();
                    if (methodDesc != NULL)
                    {
                        if (displacement)
                        {
                            *displacement = address - alignedAddress;
                        }

                        goto NameFromMethodDesc;
                    }
                }
            }
            alignedAddress -= PRECODE_ALIGNMENT;
        }
    }

    if (StubManager::IsStub(TO_TADDR(address)))
    {
        if (displacement)
        {
            *displacement = 0;
        }

        if (symbolLen)
        {
            *symbolLen = 32;
        }

        return StringCchPrintfW(symbolBuf, bufLen, L"CLRStub[%s]@%I64x",
                                StubManager::FindStubManagerName(address), address)
            == S_OK ? S_OK : S_FALSE;
    }
    else
    {
        return E_NOINTERFACE;
    }


NameFromMethodDesc:
    if (methodDesc->GetClassification() == mcDynamic &&
        !methodDesc->GetSig())
    {
        // XXX drewb - Should this case have a more specific name?
        if (symbolLen)
        {
            *symbolLen = 32;
        }
        return StringCchPrintfW(symbolBuf, bufLen,
                                L"CLRStub@%I64x", address) == S_OK ?
            S_OK : S_FALSE;
    }

    return GetFullMethodName(methodDesc, bufLen, symbolLen, symbolBuf);
}

HRESULT
ClrDataAccess::GetMethodExtents(MethodDesc* methodDesc,
                                METH_EXTENTS** extents)
{
    CLRDATA_ADDRESS_RANGE* curExtent;

    {
        //
        // Get the information from the methoddesc.
        // We'll go through the CodeManager + JitManagers, so this should work
        // for all types of managed code.
        //

        IJitManager* jitMan = NULL;
        TADDR methodStart = (TADDR)methodDesc->GetFunctionAddress(&jitMan);
        if (!jitMan)
        {
            return E_NOINTERFACE;
        }

        METHODTOKEN methodToken;
        jitMan->JitCodeToMethodInfo((SLOT)methodStart, NULL, &methodToken);

        TADDR methodInfo = (TADDR)jitMan->GetGCInfo(methodToken);
        ICodeManager* codeMan = jitMan->GetCodeManager();
        TADDR codeSize = codeMan->GetFunctionSize((PVOID)methodInfo);

        *extents = new (nothrow) METH_EXTENTS;
        if (!*extents)
        {
            return E_OUTOFMEMORY;
        }

        (*extents)->numExtents = 1;
        curExtent = (*extents)->extents;
        curExtent->startAddress =
            TO_CDADDR((TADDR)methodDesc->GetFunctionAddress());
        curExtent->endAddress =
            curExtent->startAddress + codeSize;
        curExtent++;
    }

    (*extents)->curExtent = 0;

    return S_OK;
}

// Allocator to pass to the debug-info-stores...
BYTE* DebugInfoStoreNew(void * pData, size_t cBytes)
{
    return new (nothrow) BYTE[cBytes];
}

HRESULT
ClrDataAccess::GetMethodVarInfo(MethodDesc* methodDesc,
                                TADDR address,
                                ULONG32* numVarInfo,
                                ICorDebugInfo::NativeVarInfo** varInfo,
                                ULONG32* codeOffset)
{
    COUNT_T countNativeVarInfo;
    NewHolder<ICorDebugInfo::NativeVarInfo> nativeVars(NULL);

    DebugInfoRequest request;
    TADDR  addr = (TADDR) methodDesc->GetFunctionAddress();
    request.InitFromStartingAddr(methodDesc, addr);

    HRESULT status = g_pDebugInfoStore->GetVars(
        request,
        DebugInfoStoreNew, NULL, // allocator
        &countNativeVarInfo, &nativeVars);


    if (FAILED(status))
    {
        return status;
    }

    if (!nativeVars || !countNativeVarInfo)
    {
        return E_NOINTERFACE;
    }

    *numVarInfo = countNativeVarInfo;
    *varInfo = nativeVars;
    nativeVars.SuppressRelease(); // To prevent NewHolder from releasing the memory

    if (codeOffset)
    {
        *codeOffset = (ULONG32)
            (address - (TADDR)methodDesc->GetFunctionAddress());
    }
    return S_OK;
}

HRESULT
ClrDataAccess::GetMethodNativeMap(MethodDesc* methodDesc,
                                  TADDR address,
                                  ULONG32* numMap,
                                  DebuggerILToNativeMap** map,
                                  bool* mapAllocated,
                                  CLRDATA_ADDRESS* codeStart,
                                  ULONG32* codeOffset)
{
    _ASSERTE((codeOffset == NULL) || (address != NULL));

    // Use the DebugInfoStore to get IL->Native maps.
    // It doesn't matter whether we're jitted, ngenned etc.

    DebugInfoRequest request;
    TADDR  addr = (TADDR) methodDesc->GetFunctionAddress();
    request.InitFromStartingAddr(methodDesc, addr);


    // Bounds info.
    ULONG32 countMapCopy;
    NewHolder<ICorDebugInfo::OffsetMapping> mapCopy(NULL);

    HRESULT status = g_pDebugInfoStore->GetBoundaries(
        request,
        DebugInfoStoreNew, NULL, // allocator
        &countMapCopy, &mapCopy);

    if (FAILED(status))
    {
        return status;
    }


    // Need to convert map formats.
    *numMap = countMapCopy;

    *map = new (nothrow) DebuggerILToNativeMap[countMapCopy];
    if (!*map)
    {
        return E_OUTOFMEMORY;
    }

    ULONG32 i;
    for (i = 0; i < *numMap; i++)
    {
        (*map)[i].ilOffset = mapCopy[i].ilOffset;
        (*map)[i].nativeStartOffset = mapCopy[i].nativeOffset;
        if (i > 0)
        {
            (*map)[i - 1].nativeEndOffset = (*map)[i].nativeStartOffset;
        }
        (*map)[i].source = mapCopy[i].source;
    }
    if (*numMap >= 1)
    {
        (*map)[i - 1].nativeEndOffset = 0;
    }


    // Update varion out params.
    if (codeStart)
    {
        *codeStart = TO_CDADDR((TADDR)methodDesc->GetFunctionAddress());
    }
    if (codeOffset)
    {
        *codeOffset = (ULONG32)
            (address - (TADDR)methodDesc->GetFunctionAddress());
    }

    *mapAllocated = true;
    return S_OK;
}

JITNotification*
ClrDataAccess::GetHostJitNotificationTable()
{
    if (m_jitNotificationTable == NULL)
    {
        m_jitNotificationTable =
            JITNotifications::InitializeNotificationTable(1000);
    }

    return m_jitNotificationTable;
}

void*
ClrDataAccess::GetMetaDataFromHost(PEFile* peFile,
                                   bool* isAlternate)
{
    PEImage* mdImage = NULL;
    ULONG32 imageTimestamp, imageSize, dataSize;
    void* buffer;
    WCHAR uniPath[MAX_PATH];
    COUNT_T uniPathChars;
    bool isAlt = false;
    bool isNGEN = false;
    DAC_INSTANCE* inst = NULL;
    HRESULT  hr = S_OK;
    IMAGE_DATA_DIRECTORY *pDir = NULL;
    PEImageLayout   *layout;
    ULONG32     ulRvaHint;
    //
    // We always ask for the IL image metadata,
    // as we expect that to be more
    // available than others.  The drawback is that
    // there may be differences between the IL image
    // metadata and native image metadata, so we
    // have to mark such alternate metadata so that
    // we can fail unsupported usage of it.
    //
    if (!peFile->HasNativeImage())
    {
        mdImage = peFile->GetILimage();
        if (mdImage != NULL)
        {
            layout = mdImage->GetLoadedLayout();
            pDir = &layout->GetCorHeader()->MetaData;

            // In IL image case, we do not have any hint to IL metadata since it is stored
            // in the corheader
            //
            ulRvaHint = 0;
            dataSize = pDir->Size;
        }
        else
        {
            return NULL;
        }
    }
    else
    {
        mdImage = peFile->GetNativeImage();
        _ASSERTE(mdImage != NULL);
        isNGEN = true;
        layout = mdImage->GetLoadedLayout();
        // For ngen image, the IL metadata is stored for private use. So we need to pass
        // the RVA hint to find it to debuggers
        //
        ulRvaHint = pDir->VirtualAddress;
        dataSize = pDir->Size;
    }

    if (!mdImage->HasNTHeaders() ||
        !mdImage->HasCorHeader() ||
        !mdImage->HasLoadedLayout() ||
        !mdImage->GetPath().DacGetUnicode(NumItems(uniPath), uniPath,
                                          &uniPathChars) ||
        uniPathChars > NumItems(uniPath))
    {
        return NULL;
    }

    // It is possible that the module is in-memory. That is the uniPath here is empty.
    // We will try to use the module name instead in this case for hosting debugger
    // to find match
    if (wcslen(uniPath) == 0)
    {
        if (!mdImage->GetModuleFileNameHintForDAC().DacGetUnicode(NumItems(uniPath), uniPath,
                                          &uniPathChars) ||
            uniPathChars > NumItems(uniPath))
        {
            return NULL;
        }
    }
    
    // try direct match for the image that is loaded into the managed process
    imageTimestamp = layout->GetTimeDateStamp();
    imageSize = (ULONG32)layout->GetVirtualSize();
    peFile->GetMetadata(&dataSize);

    inst = m_instances.Alloc(0, dataSize + sizeof(DAC_INSTANCE), DAC_DPTR);
    if (!inst)
    {
        DacError(E_OUTOFMEMORY);
        return NULL;
    }

    buffer = (void*)(inst + 1);

    // API implemented by hosting debugger. It will use the path/filename, timestamp and
    // file size to find the exact match of the image
    //
    if (m_metadataLocator)
    {
        hr = m_metadataLocator->GetMetadata(
            uniPath,
            imageTimestamp,
            imageSize,
            NULL,           // MVID - not used yet
            ulRvaHint,
            0,              // flags - reserved for future.
            dataSize,
            (BYTE*)buffer,
            NULL);
    }
    else
    {
        hr = m_target3->GetMetaData(
            uniPath,
            imageTimestamp,
            imageSize,
            NULL,           // MVID - not used yet
            ulRvaHint,
            0,              // flags - reserved for future.
            dataSize,
            (BYTE*)buffer,
            NULL);
    }
    if (FAILED(hr) && isNGEN)
    {
        // We failed to locate the ngen'ed image. We should try to
        // find the matching IL image
        //
        isAlt = true;


        // use the IL File name
        if (!peFile->GetPath().DacGetUnicode(NumItems(uniPath), uniPath,
                                          &uniPathChars))
        {
            goto ErrExit;
        }
        // RVA size in ngen image and IL image is the same. Because the only
        // different is in RVA. That is 4 bytes column fixed.
        //

        // try again
        if (m_metadataLocator)
        {
            hr = m_metadataLocator->GetMetadata(
                uniPath,
                imageTimestamp,
                imageSize,
                NULL,           // MVID - not used yet
                0,              // pass zero hint here... important
                0,              // flags - reserved for future.
                dataSize,
                (BYTE*)buffer,
                NULL);
        }
        else
        {
            hr = m_target3->GetMetaData(
                uniPath,
                imageTimestamp,
                imageSize,
                NULL,           // MVID - not used yet
                0,              // pass zero hint here... important
                0,              // flags - reserved for future.
                dataSize,
                (BYTE*)buffer,
                NULL);
        }
    }

    if (FAILED(hr))
    {
        goto ErrExit;
    }

    *isAlternate = isAlt;
    m_instances.AddSuperseded(inst);
    return buffer;

ErrExit:
    if (inst != NULL)
    {
        m_instances.ReturnAlloc(inst);
    }
    return NULL;
}


//++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//
// Given a PEFile and try to find the corresponding metadata
// We will first ask debugger to locate it. If fail, we will try
// to get it from the target process
//
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++
IMDInternalImport*
ClrDataAccess::GetMDImport(const PEFile* peFile, bool throwEx)
{
    HRESULT     status;
    const void *mdBaseTarget = NULL;
    COUNT_T     mdSize;
    IMDInternalImport* mdImport = NULL;
    PVOID       mdBaseHost = NULL;
    DAC_MD_IMPORT* importList = NULL;
    TADDR       peFileAddr = PTR_HOST_TO_TADDR((PVOID)peFile);
    bool        isAlternate = false;

    //
    // Look for one we've already created.
    //
    for (importList = m_mdImports; importList; importList = importList->next)
    {
        if (importList->peFile == peFileAddr)
        {
            return importList->impl;
        }
    }

    // Get the metadata size
    mdBaseTarget = ((PEFile*)peFile)->GetMetadata(&mdSize);

    if (!mdBaseTarget)
    {
        mdBaseHost = NULL;
    }
    else
    {

        //
        // Maybe the target process has the metadata
        // Find out where the metadata for the image is
        // in the target's memory.
        //
        //
        // Read the metadata into the host process. Make sure pass in false in the last
        // parameter. This is only matters when producing skinny mini-dump. This will
        // prevent metadata gets reported into mini-dump.
        //

        mdBaseHost = DacInstantiateTypeByAddressNoReport((TADDR)mdBaseTarget, mdSize,
                                                 false);
    }

    // Try to see if debugger can locate it
    if (mdBaseHost == NULL && (m_target3 || m_metadataLocator))
    {
        // We couldn't read the metadata from memory.  Ask
        // the target for metadata as it may be able to
        // provide it from some alternate means.
        mdBaseHost = GetMetaDataFromHost((PEFile*)peFile, &isAlternate);
    }

    if (mdBaseHost == NULL)
    {
        // cannot locate metadata anywhere
        if (throwEx)
        {
            DacError(E_INVALIDARG);
        }
        return NULL;
    }

    //
    // Open the MD interface on the host copy of the metadata.
    //

    status = GetMDInternalInterface(mdBaseHost, mdSize, ofRead,
                                    IID_IMDInternalImport,
                                    (void**)&mdImport);
    if (status != S_OK)
    {
        if (throwEx)
        {
            DacError(status);
        }
        return NULL;
    }

    //
    // Remember the object for this module for
    // possible later use.
    //

    importList = new (nothrow) DAC_MD_IMPORT;
    if (!importList)
    {
        mdImport->Release();
        DacError(E_OUTOFMEMORY);
    }

    importList->peFile = peFileAddr;
    importList->isAlternate = isAlternate;
    importList->impl = mdImport;
    importList->next = m_mdImports;
    m_mdImports = importList;

    return mdImport;
}

// Needed for RT_RCDATA.
#define MAKEINTRESOURCE(v) MAKEINTRESOURCEA(v)

HRESULT
ClrDataAccess::GetDacGlobals(void)
{
    HRESULT status = E_FAIL;
    LPVOID rsrcData = NULL;


    WCHAR wszPath[MAX_PATH + 1];
    WCHAR *pwsz;
    HANDLE hFile = NULL;
    HANDLE hFileMapping = NULL;

    //
    // Since the PAL does not support Win32 resources look for the globals
    // information in the file "mscordacwks.bin," located in the same
    // directory as this DLL. By mapping the file into memory we can
    // make it look like a resource to the code below that does the
    // parsing and verification.
    //

    if (!GetModuleFileName(g_thisModule, wszPath, MAX_PATH))
    {
        return HRESULT_FROM_GetLastError();
    }

#ifndef PLATFORM_UNIX
#define PATH_SEPARATOR L'\\'
#else
#define PATH_SEPARATOR L'/'
#endif

    pwsz = wcsrchr(wszPath, PATH_SEPARATOR);
    if (!pwsz)
    {
        return E_UNEXPECTED;
    }
    pwsz += 1;
    *pwsz = NULL;
    wcsncat(wszPath, L"mscordacwks.bin", MAX_PATH - wcslen(wszPath));

    hFile = CreateFile(
                wszPath,
                GENERIC_READ,
                FILE_SHARE_READ,
                NULL,
                OPEN_EXISTING,
                FILE_ATTRIBUTE_NORMAL,
                NULL
                );
    if (INVALID_HANDLE_VALUE == hFile)
    {
        return HRESULT_FROM_GetLastError();
    }

    hFileMapping = CreateFileMapping(hFile, NULL, PAGE_READONLY, 0, 0, NULL);
    if (!hFileMapping)
    {
        CloseHandle(hFile);
        return HRESULT_FROM_GetLastError();
    }

    rsrcData = MapViewOfFile(hFileMapping, FILE_MAP_READ, 0, 0, 0);
    if (!rsrcData)
    {
        CloseHandle(hFileMapping);
        CloseHandle(hFile);
        return HRESULT_FROM_GetLastError();
    }


    EX_TRY
    {
        PBYTE rawData = (PBYTE)rsrcData;

        ULONG numGlobals = *(PULONG)rawData;
        rawData += sizeof(ULONG);
        ULONG numVptrs = *(PULONG)rawData;
        rawData += sizeof(ULONG);

        if ((offsetof(DacGlobals, Thread__vtAddr) !=
             numGlobals * sizeof(ULONG)) ||
            (sizeof(DacGlobals) !=
             (numGlobals + numVptrs) * sizeof(ULONG)))
        {
            status = E_INVALIDARG;
            goto Exit;
        }

        g_dacGlobals = *(DacGlobals*)rawData;

        status = S_OK;

    Exit: ;
    }
    EX_CATCH
    {
        status = E_FAIL;
    }
    EX_END_CATCH(SwallowAllExceptions)

    UnmapViewOfFile(rsrcData);
    CloseHandle(hFileMapping);
    CloseHandle(hFile);

    return status;
}

#undef MAKEINTRESOURCE

//----------------------------------------------------------------------------
//
// CLRDataCreateInstance.
//
//----------------------------------------------------------------------------

STDAPI
CLRDataCreateInstance(REFIID iid,
                      ICLRDataTarget* target,
                      void** iface)
{
    OnUnicodeSystem();

    *iface = NULL;

    ClrDataAccess* dacClass = new (nothrow) ClrDataAccess(target);
    if (!dacClass)
    {
        return E_OUTOFMEMORY;
    }

    HRESULT status = dacClass->Initialize();
    if (status == S_OK)
    {
        status = dacClass->QueryInterface(iid, iface);
    }

    dacClass->Release();
    return status;
}
