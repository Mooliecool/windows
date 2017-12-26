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
// File: node.cpp
//
// ===========================================================================

#include "pch.h"
#include "csiface.h"
#include "nodes.h"
#include "name.h"
#include "strbuild.h"
#include "tokdata.h"
#include "posdata.h"
#include "stdio.h"


struct PTNAME
{
    PCWSTR  pszFull;
    PCWSTR  pszNice;
};

#define PREDEFTYPEDEF(id, name, required, simple, numeric, kind, ft, et, nicename, zero, qspec, asize, st, attr, arity, inmscorlib) { L##name, nicename },
static  const   PTNAME  rgPredefNames[] = {
#include "predeftype.h"
    { L"", NULL},
    { L"System.Void", L"void"}
};

#define OP(o,p,r,s,t,pn,ek) t,
static  const   long    rgOpTokens[] = {
#include "ops.h"
};


////////////////////////////////////////////////////////////////////////////////
// BASENODE::m_rgNodeGroups

DWORD   BASENODE::m_rgNodeGroups[] = {
#define NODEKIND(n,s,g,p) g,
#include "nodekind.h"
0};

////////////////////////////////////////////////////////////////////////////////
// BASENODE::GetOperatorToken

long BASENODE::GetOperatorToken (long iOp)
{
    return rgOpTokens[iOp];
}

long BASENODE::GetFirstNonAttributeToken()
{
    if (this->InGroup(NG_AGGREGATE))
    {
        return this->asAGGREGATE()->iStart;
    }
    else if (this->InGroup(NG_MEMBER))
    {
        return this->asANYMEMBER()->iStart;
    }
    else if (this->kind == NK_DELEGATE)
    {
        return this->asDELEGATE()->iStart;
    }
    else
    {
        return this->tokidx;
    }
}


////////////////////////////////////////////////////////////////////////////////
// BASENODE::GetDefaultAttrLocation
// Use this to get the default attribute location to a particular node. If the node is not recognised or not 
// implemented it would return 0
ATTRLOC BASENODE::GetDefaultAttrLocation()
{
     // Used as defaults:
     // AL_PARAM, AL_TYPE, AL_METHOD, AL_FIELD, AL_PROPERTY, AL_EVENT, AL_TYPEVAR
     // Not used as defaults:
     // AL_ASSEMBLY, AL_MODULE, AL_RETURN, AL_UNKNOWN

    if (this->InGroup(NG_AGGREGATE) || this->kind == NK_DELEGATE)
    {
        return AL_TYPE;
    }
    else if (this->kind == NK_PROPERTY)
    {
        if (this->other & NFEX_EVENT)
            return AL_EVENT;
        else
            return AL_PROPERTY;
    }
    else if (this->kind == NK_FIELD)
    {
        if (this->other & NFEX_EVENT)
            return AL_EVENT;
        else
            return AL_FIELD;
    }
    else if (this->kind == NK_PARAMETER)
    {
        return AL_PARAM;
    }
    else if (this->InGroup(NG_METHOD) || this->kind == NK_ACCESSOR)
    {
        return AL_METHOD;
    }
    else if (this->kind == NK_TYPEWITHATTR)
    {
        return AL_TYPEVAR;
    }
    else
    {
        return (ATTRLOC) 0;
    }
}

////////////////////////////////////////////////////////////////////////////////
// BASENODE::AppendNameText

HRESULT BASENODE::AppendNameText (CStringBuilder &sb, ICSNameTable *pNameTable)
{
    return AppendNameTextToKey(sb, pNameTable);
}

////////////////////////////////////////////////////////////////////////////////
// BASENODE::AppendNameTextToKey

HRESULT BASENODE::AppendNameTextToKey (CStringBuilder &sb, ICSNameTable *pNameTable)
{
    if (this->IsAnyName())
    {
        if (pNameTable != NULL && pNameTable->IsKeyword (this->asANYNAME()->pName, CompatibilityNone, NULL) == S_OK)
            sb += L"@";
        sb += this->asANYNAME()->pName->text;
        if (this->kind == NK_GENERICNAME)
            AppendParametersToKey( PRT_GENERIC, this->asGENERICNAME()->pParams, sb);
        return S_OK;
    }

    if (this->kind == NK_DOT)
    {
        this->asDOT()->p1->AppendNameTextToKey (sb, pNameTable);
        if (this->IsDblColon())
            sb += L"::";
        else
            sb += L".";
        this->asDOT()->p2->AppendNameTextToKey (sb, pNameTable);
        return S_OK;
    }

    VSFAIL ("AppendNameTextToKey called on non-name node...");
    return E_FAIL;
}

////////////////////////////////////////////////////////////////////////////////
// BASENODE::AppendNameTextToPrototype

HRESULT BASENODE::AppendNameTextToPrototype (CStringBuilder &sb, ICSNameTable *pNameTable)
{
    if (this->IsAnyName())
    {
        if (pNameTable != NULL && pNameTable->IsKeyword (this->asANYNAME()->pName, CompatibilityNone, NULL) == S_OK)
            sb += L"@";
        sb += this->asANYNAME()->pName->text;
        if (this->kind == NK_GENERICNAME)
            AppendParametersToPrototype( PRT_GENERIC, this->asGENERICNAME()->pParams, sb, false);
        return S_OK;
    }

    if (this->kind == NK_DOT)
    {
        this->asDOT()->p1->AppendNameTextToPrototype (sb, pNameTable);
        if (this->IsDblColon())
            sb += L"::";
        else
            sb += L".";
        this->asDOT()->p2->AppendNameTextToPrototype (sb, pNameTable);
        return S_OK;
    }

    VSFAIL ("AppendNameText called on non-name node...");
    return E_FAIL;
}

////////////////////////////////////////////////////////////////////////////////
// BASENODE::AppendTypeText

HRESULT BASENODE::AppendTypeText (CStringBuilder &sb, ICSNameTable *pNameTable)
{
    // Back up to a type node
    BASENODE *pPar;
    for (pPar = this->GetParent(); pPar != NULL; pPar = pPar->GetParent())
        if (pPar->InGroup (NG_AGGREGATE))
            break;

    HRESULT     hr = E_FAIL;        // If nothing is added, then this isn't a type or contained by one
    BOOL        fNeedDot = FALSE;

    if (pPar != NULL)
    {
        // Put it's type text in the bstr first...
        if (FAILED (hr = pPar->AppendTypeText (sb, pNameTable)))
            return hr;

        fNeedDot = TRUE;
    }

    // Now this, if it is in fact a type itself
    if (this->InGroup (NG_AGGREGATE) || this->kind == NK_DELEGATE)
    {
        if (fNeedDot)
            sb += L".";

        NAMENODE    *pName = (this->kind == NK_DELEGATE) ? this->asDELEGATE()->pName : this->asAGGREGATE()->pName;

        if (pNameTable != NULL && pNameTable->IsKeyword (pName->pName, CompatibilityNone, NULL) == S_OK)
            sb += L"@";

        sb += pName->pName->text;
        if ((this->kind == NK_DELEGATE) ? this->asDELEGATE()->pTypeParams : this->asAGGREGATE()->pTypeParams)
            AppendParametersToKey( PRT_GENERIC, (this->kind == NK_DELEGATE) ? this->asDELEGATE()->pTypeParams : this->asAGGREGATE()->pTypeParams, sb);
        hr = S_OK;
    }

    return hr;
}

////////////////////////////////////////////////////////////////////////////////
// BASENODE::BuildKey

HRESULT BASENODE::BuildKey (ICSNameTable *pNameTable, BOOL fIncludeParent, bool fIncludeGenerics, NAME **ppKey)
{
    // Must be in the group of key-able nodes...
    if (!this->InGroup (NG_KEYED))
        return E_FAIL;

    // NG_KEYED contains:
    //  NK_CLASS
    //  NK_CTOR
    //  NK_DELEGATE
    //  NK_DTOR
    //  NK_ENUM
    //  NK_ENUMMBR
    //  NK_INTERFACE
    //  NK_METHOD
    //  NK_NAMESPACE
    //  NK_OPERATOR
    //  NK_PROPERTY
    //  NK_INDEXER
    //  NK_STRUCT
    //  NK_VARDECL      NOTE:  You'll notice FIELD and CONST do not belong; only VARDECL, which is used for each field/const declaration
    
    NAME            **ppCachedKey = NULL;
    CStringBuilder  sb;
    HRESULT         hr;

    if (fIncludeParent)
    {
        // Some nodes cache their keys.  Check for those nodes here, and return the
        // cached key (if created).  Note that the global namespace (the root of everything)
        // has an empty key, which is assigned at parse time -- so this is gauranteed to
        // stop there at least.
        // Also, note that we only do this if our caller asked to include the parent key,
        // since only "full" keys are cached.  Likewise, for elements that can contain
        // Type params, we only cache if the called asked to include the type params     
        if (this->kind == NK_CLASS && fIncludeGenerics)
            ppCachedKey = &this->asAGGREGATE()->pKey;
        else if (this->InGroup (NG_AGGREGATE) && fIncludeGenerics)
            ppCachedKey = &this->asAGGREGATE()->pKey;
        else if (this->kind == NK_NAMESPACE)
            ppCachedKey = &this->asNAMESPACE()->pKey;
        else if (this->kind == NK_DELEGATE && fIncludeGenerics)
            ppCachedKey = &this->asDELEGATE()->pKey;

        if (ppCachedKey != NULL && *ppCachedKey != NULL)
        {
            *ppKey = *ppCachedKey;
            return S_OK;
        }

        // Okay, we have to build it.  All keys consist of <parentkey> + "." + <thiskey>
        // (unless our parent key is empty, in which case it's just <thiskey>), so find our parent's key.
        BASENODE *p;
        for (p = this->GetParent(); p != NULL && !p->InGroup (NG_KEYED); p = p->GetParent())
            ;

        NAME    *pParentKey;
        if (FAILED (hr = p->BuildKey (pNameTable, fIncludeParent, fIncludeGenerics, &pParentKey)))
            return hr;

        ASSERT (pParentKey != NULL);

        sb = pParentKey->text;
        if (pParentKey->text[0] != 0)
            sb += L".";
    }

    // Okay, now do the right thing based on the kind of this node...
    switch (this->kind)
    {
        case NK_NAMESPACE:
            if (this->asNAMESPACE()->pName != NULL)
                this->asNAMESPACE()->pName->AppendNameTextToKey (sb, NULL);
            break;

        case NK_CLASS:
        case NK_STRUCT:
        case NK_INTERFACE:
        case NK_ENUM:
            {
                AGGREGATENODE* pNode = this->asAGGREGATE();                
                pNode->pName->AppendNameTextToKey (sb, NULL);
                
                if (fIncludeGenerics)
                    AppendTypeParametersToKey(pNode->pTypeParams, sb);
            }
            break;

        case NK_DELEGATE:
            {
                DELEGATENODE* pNode = this->asDELEGATE();
                pNode->pName->AppendNameTextToKey (sb, NULL);
                if (fIncludeGenerics)
                    AppendTypeParametersToKey(pNode->pTypeParams, sb);

                //TODO: append parameters?
            }
            break;

        case NK_ENUMMBR:
            sb += this->asENUMMBR()->pName->pName->text;
            break;

        case NK_VARDECL:
            sb += this->asVARDECL()->pName->pName->text;
            break;

        case NK_METHOD:
            this->asMETHOD()->pName->AppendNameTextToKey (sb, NULL);
            AppendParametersToKey (PRT_METHOD, this->asMETHOD()->pParms, sb);
            break;

        case NK_OPERATOR:
        {
            WCHAR   szBuf[16];

            StringCchPrintfW(szBuf, sizeof(szBuf) / sizeof(szBuf[0]), L"#op%d", this->asOPERATOR()->iOp);
            sb += szBuf;
            AppendParametersToKey (PRT_METHOD, this->asOPERATOR()->pParms, sb);
            break;
        }

        case NK_CTOR:
            if (this->flags & NF_MOD_STATIC)
                sb += L"#sctor";
            else
                sb += L"#ctor";
            AppendParametersToKey (PRT_METHOD, this->asCTOR()->pParms, sb);
            break;

        case NK_DTOR:
            sb += L"#dtor()";
            break;

        case NK_INDEXER:
        case NK_PROPERTY:
            if (this->asANYPROPERTY()->pName != NULL)
                this->asANYPROPERTY()->pName->AppendNameTextToKey (sb, NULL);
            if (this->kind == NK_INDEXER)
            {
                if (this->asANYPROPERTY()->pName != NULL)
                    sb += L".";
                sb += L"#this";
                AppendParametersToKey (PRT_INDEXER, this->asANYPROPERTY()->pParms, sb);
            }
            break;

        default:
            VSFAIL ("Unhandled node type in BuildKey!");
            return E_FAIL;
    }

    // Make it into a name and return it, caching it if this is a key caching node
    if (FAILED(hr = sb.CreateName (pNameTable, ppKey)))
        return hr;


    if (SUCCEEDED (hr = sb.CreateName (pNameTable, ppKey)))
    {
        if (ppCachedKey != NULL)
        {
            ASSERT (fIncludeParent);
            *ppCachedKey = *ppKey;
        }
    }

    return hr;
}

////////////////////////////////////////////////////////////////////////////////

HRESULT BASENODE::AppendPrototype (ICSNameTable *pNameTable, PROTOFLAGS ptFlags, CStringBuilder &sb, bool fEscapedName/* = false*/)
{
    // Must be in the group of key-able nodes...
    if (!this->InGroup (NG_KEYED))
        return E_FAIL;

    // NG_KEYED contains:
    //  NK_CLASS            *
    //  NK_CTOR
    //  NK_DELEGATE         *
    //  NK_DTOR
    //  NK_ENUM             *
    //  NK_ENUMMBR
    //  NK_INTERFACE        *
    //  NK_METHOD
    //  NK_NAMESPACE        *
    //  NK_OPERATOR
    //  NK_PROPERTY
    //  NK_INDEXER
    //  NK_STRUCT           *
    //  NK_VARDECL      NOTE:  You'll notice FIELD and CONST do not belong; only VARDECL, which is used for each field/const declaration

    HRESULT     hr;
    ICSNameTable *pTable = fEscapedName ? pNameTable : NULL;
    
    // The starred ones are nodes for which the key will do fine if PTF_FULLNAME is specified,
    // and just this node's name will do otherwise...
    if (this->InGroup (NG_TYPE) || this->kind == NK_NAMESPACE)
    {
        if (ptFlags & PTF_FULLNAME)
        {
            BASENODE *pPar;
            for (pPar = this->pParent; pPar != NULL && !pPar->InGroup (NG_KEYED); pPar = pPar->pParent)
                ;

            if (pPar != NULL)
            {
                if (FAILED (hr = pPar->AppendPrototype (pNameTable, ptFlags, sb, fEscapedName)))
                    return hr;

                if (sb.GetLength() > 0)
                    sb += L".";
            }

            NAME    *pKey;
            if (FAILED (hr = this->BuildKey (pNameTable, false, false /* no <,,,> */, &pKey)))
                return hr;

            if (fEscapedName)
                sb.AppendPossibleKeyword (pTable, pKey->text, (size_t)-1);
            else
                sb += pKey->text;  

        }
        else
        {
            if (this->kind == NK_NAMESPACE)
                return this->asNAMESPACE()->pName->AppendNameTextToPrototype (sb, pTable);

            if (this->kind == NK_DELEGATE)
            {
                this->asDELEGATE()->pName->AppendNameTextToPrototype (sb, pTable);
            }
            else
                this->asAGGREGATE()->pName->AppendNameTextToPrototype (sb, pTable);
        }

        if (ptFlags & PTF_TYPEVARS)
        {
            if (this->kind == NK_DELEGATE && this->asDELEGATE()->pTypeParams)
            {
                AppendParametersToPrototype( PRT_GENERIC, this->asDELEGATE()->pTypeParams, sb, false);
            }
            else if (this->InGroup (NG_AGGREGATE) && this->asAGGREGATE()->pTypeParams)
            {                
                AppendParametersToPrototype( PRT_GENERIC, this->asAGGREGATE()->pTypeParams, sb, false);
            }
        }

        if (ptFlags & PTF_FAILONMISSINGNAME)
        {
            if (wcschr(sb, L'?') != NULL)
                return E_FAIL;
        }
        return S_OK;
    }

    // Okay, this is a member.  If PTF_FULLNAME or PTF_TYPENAME is provided, get our parent's prototype.
    if (ptFlags & (PTF_FULLNAME|PTF_TYPENAME))
    {
        BASENODE *pPar;
        for (pPar = this->pParent; pPar != NULL && !pPar->InGroup (NG_KEYED); pPar = pPar->pParent)
            ;

        if (FAILED (hr = pPar->AppendPrototype (pNameTable, ptFlags, sb, fEscapedName)))
            return hr;

        sb += L".";
    }

    // Okay, now do the right thing based on the kind of this node...
    switch (this->kind)
    {
        case NK_ENUMMBR:
            if (fEscapedName)
                sb.AppendPossibleKeyword (pNameTable, this->asENUMMBR()->pName->pName);
            else
                sb += this->asENUMMBR()->pName->pName->text;
            break;

        case NK_VARDECL:
            if (fEscapedName)
                sb.AppendPossibleKeyword (pNameTable, this->asVARDECL()->pName->pName);
            else
                sb += this->asVARDECL()->pName->pName->text;
            break;

        case NK_METHOD:
            {
                METHODNODE* pMethod = this->asMETHOD();
                if (pMethod->pName->IsAnyName())
                {
                    if (fEscapedName)
                        sb.AppendPossibleKeyword (pNameTable, pMethod->pName->asANYNAME()->pName);
                    else
                        sb.Append(pMethod->pName->asANYNAME()->pName->text);

                    if (ptFlags & PTF_TYPEVARS && (pMethod->pName->kind == NK_GENERICNAME))
                        AppendParametersToPrototype(PRT_GENERIC, pMethod->pName->asGENERICNAME()->pParams, sb, false);
                }
                else
                {
                    ASSERT (pMethod->pName->kind == NK_DOT);

                    if (!(ptFlags & PTF_FULLNAME))
                    {
                        pMethod->pName->asDOT()->p1->AppendNameTextToPrototype (sb, pTable);
                        sb += L".";
                    }
                    
                    if (ptFlags & PTF_TYPEVARS)
                    {
                        pMethod->pName->LastNameOfDottedName()->AppendNameTextToPrototype (sb, pTable);
                    }
                    else
                    {
                        if (fEscapedName)
                            sb.AppendPossibleKeyword (pNameTable, pMethod->pName->LastNameOfDottedName()->pName);
                        else
                            sb.Append(pMethod->pName->LastNameOfDottedName()->pName->text);
                    }
                } 

                if (ptFlags & PTF_PARAMETERS)
                {
                    AppendParametersToPrototype (PRT_METHOD, this->asMETHOD()->pParms, sb, !!(ptFlags & PTF_PARAMETER_NAME));
                }
            }
            break;

        case NK_OPERATOR:
        {
            PCWSTR  pszOpToken;
            long    iOp = this->asOPERATOR()->iOp;

            // Map operator to operator name.  implicit/explicit must be
            // handled differently.
            pNameTable->GetTokenText (rgOpTokens[iOp], &pszOpToken);
            if (iOp == OP_IMPLICIT || iOp == OP_EXPLICIT)
            {
                sb += pszOpToken;
                sb += L" operator";
            }
            else
            {
                sb += L"operator ";
                sb += pszOpToken;
            }

            if (ptFlags & PTF_PARAMETERS)
                AppendParametersToPrototype (PRT_METHOD, this->asOPERATOR()->pParms, sb, !!(ptFlags & PTF_PARAMETER_NAME));
            break;
        }

        case NK_CTOR:
        case NK_DTOR:
        {
            BASENODE *p;
            for (p = this->pParent; p != NULL && !p->InGroup (NG_AGGREGATE); p = p->pParent) {
                ;
            }

            if (this->kind == NK_DTOR) {
                sb += L"~";
            }

            if (fEscapedName) {
                sb.AppendPossibleKeyword (pNameTable, p->asAGGREGATE()->pName->pName);
            } else{
                sb += p->asAGGREGATE()->pName->pName->text;
            }

            if (ptFlags & PTF_PARAMETERS) {
                AppendParametersToPrototype (PRT_METHOD, this->asANYMETHOD()->pParms, sb, !!(ptFlags & PTF_PARAMETER_NAME));
            }

            break;
        }

        case NK_INDEXER:
        case NK_PROPERTY:
            if (this->asANYPROPERTY()->pName != NULL) {
                this->asANYPROPERTY()->pName->AppendNameTextToPrototype (sb, pTable);
            }

            if (this->kind == NK_INDEXER) {
                if (this->asANYPROPERTY()->pName != NULL) {
                    sb += L".";
                }

                sb += L"this";

                if (ptFlags & PTF_PARAMETERS) {
                    AppendParametersToPrototype (PRT_INDEXER, this->asANYPROPERTY()->pParms, sb, !!(ptFlags & PTF_PARAMETER_NAME));
                }
            }
            break;

        default:
            VSFAIL ("Unhandled node type in AppendPrototype!");
            return E_FAIL;
    }

    if (ptFlags & PTF_FAILONMISSINGNAME)
    {
        if (wcschr(sb, L'?') != NULL)
            return E_FAIL;
    }

    return S_OK;
}


void BASENODE::AppendTypeParametersToKey(BASENODE* pTypeParameters, CStringBuilder& sb)
{
    //don't do anything if there were no type parameters
    if (pTypeParameters == NULL)
        return;

    CListIterator it;
    it.Start(pTypeParameters);
    sb.Append (L'<');

    it.Next(); //consume the first type param, it doesn't get a space
    while (it.Next() != NULL)
        sb.Append (L',');

    sb.Append (L'>');
}

////////////////////////////////////////////////////////////////////////////////
// BASENODE::AppendParametersToKey

void BASENODE::AppendParametersToKey (PARAMTYPES pt, BASENODE *pParms, CStringBuilder &sb)
{
    switch (pt)
    {
    default:
        ASSERT(!"Invalid PARAMTYPES");
    case PRT_METHOD:
        sb += L"(";
        break;
    case PRT_INDEXER:
        sb += L"[";
        break;
    case PRT_GENERIC:
        sb += L"<";
        break;
    }


    CListIterator   li;
    long            iParms = 0;

    li.Start (pParms);
    BASENODE* pParm;
    BASENODE* pNext = pNext = li.Next();
    for (pParm = pNext, pNext = li.Next(); pParm != NULL; pParm = pNext, pNext = li.Next())
    {
        if (iParms > 0)
            sb.Append(L',');

        if (pt != PRT_GENERIC)
        {
            if (pParm->flags & NF_PARMMOD_REF)
                sb += L"ref ";
            else if (pParm->flags & NF_PARMMOD_OUT)
                sb += L"out ";

            if ((pParms->pParent->other & NFEX_METHOD_PARAMS) && pNext == NULL)
                sb += L"params ";


            AppendTypeToKey (pParm->asPARAMETER()->pType, sb);
        }

        iParms++;
    }

    switch (pt)
    {
    default:
        // Already ASSERTed above
    case PRT_METHOD:
        sb += L")";
        break;
    case PRT_INDEXER:
        sb += L"]";
        break;
    case PRT_GENERIC:
        sb += L">";
        break;
    }
}

////////////////////////////////////////////////////////////////////////////////
// BASENODE::AppendParametersToPrototype

void BASENODE::AppendParametersToPrototype (PARAMTYPES pt, BASENODE *pParms, CStringBuilder &sb, bool fParamName)
{
    switch (pt)
    {
    default:
        ASSERT(!"Invalid PARAMTYPES");
    case PRT_METHOD:
        sb += L"(";
        break;
    case PRT_INDEXER:
        sb += L"[";
        break;
    case PRT_GENERIC:
        sb += L"<";
        break;
    }

    CListIterator   li;
    long            iParms = 0;
    BASENODE        *pParm, *pNext = NULL;

    li.Start (pParms);
    pNext = li.Next();
    for (pParm = pNext, pNext = li.Next(); pParm != NULL; pParm = pNext, pNext = li.Next())
    {
        if (iParms > 0)
            sb += L", ";

        if (pt == PRT_GENERIC)
        {
            if (pParm->IsAnyName() )
                pParm->AppendNameTextToPrototype(sb, NULL);
            else
                AppendTypeToPrototype(pParm->asANYTYPE(), sb);
        }
        else 
        {
            if (pParm->flags & NF_PARMMOD_REF)
                sb += L"ref ";
            else if (pParm->flags & NF_PARMMOD_OUT)
                sb += L"out ";

            if ((pParms->pParent->other & NFEX_METHOD_PARAMS) && pNext == NULL)
                sb += L"params ";

            AppendTypeToPrototype (pParm->asPARAMETER()->pType, sb);

            // add parameter names
            if (fParamName)
            {
                sb += L" ";
                sb += pParm->asPARAMETER()->pName->pName->text;
            }
        }
        iParms++;
    }

    switch (pt)
    {
    default:
        // Already ASSERTed above
    case PRT_METHOD:
        sb += L")";
        break;
    case PRT_INDEXER:
        sb += L"]";
        break;
    case PRT_GENERIC:
        sb += L">";
        break;
    }
}

////////////////////////////////////////////////////////////////////////////////
// BASENODE::AppendTypeToKey

void BASENODE::AppendTypeToKey (TYPEBASENODE *pType, CStringBuilder &sb)
{
    if (pType == NULL)
    {
        sb += L"?";
        return;
    }

    switch (pType->kind)
    {
    case NK_NAMEDTYPE:
        {
            NAMEDTYPENODE* pNamedType = pType->asNAMEDTYPE();
            if (pNamedType->pName->kind == NK_GENERICNAME)
            {
                sb.Append(pNamedType->pName->asGENERICNAME()->pName->text);
                //We don't use AppendParametersToKey because we need the full names 
                //of the params.  This way we can distinguish:
                //foo(List<int> a) and
                //foo(List<string> a)
                AppendParametersToPrototype(PRT_GENERIC, pNamedType->pName->asGENERICNAME()->pParams, sb, false);
            } 
            else 
            {
                pNamedType->pName->AppendNameTextToKey (sb, NULL);
            }
        }
        break;
    case NK_PREDEFINEDTYPE:
        if (rgPredefNames[pType->asPREDEFINEDTYPE()->iType].pszNice)
            sb += rgPredefNames[pType->asPREDEFINEDTYPE()->iType].pszNice;
        else
            sb += rgPredefNames[pType->asPREDEFINEDTYPE()->iType].pszFull;
        break;
    case NK_ARRAYTYPE:
        AppendTypeToKey (pType->asARRAYTYPE()->pElementType, sb);
        sb += L"[";
        for (int iDim = 1; iDim < pType->asARRAYTYPE()->iDims; iDim++)
        {
            sb += L",";
        }
        sb += L"]";        
        break;
    case NK_POINTERTYPE:
        sb += L"*";
        AppendTypeToKey (pType->asPOINTERTYPE()->pElementType, sb);
        break;
    case NK_NULLABLETYPE:
        sb += L"?";
        AppendTypeToKey (pType->asNULLABLETYPE()->pElementType, sb);
        break;
    default:
        ASSERT(!"Bad type kind");
    }
}

////////////////////////////////////////////////////////////////////////////////
// BASENODE::AppendTypeToPrototype

void BASENODE::AppendTypeToPrototype (TYPEBASENODE *pType, CStringBuilder &sb)
{
    if (pType == NULL)
    {
        sb += L"?";
        return;
    }

    switch (pType->kind)
    {
    case NK_NAMEDTYPE:
        {
            NAMEDTYPENODE* pNamedType = pType->asNAMEDTYPE();
            if (pNamedType->pName->kind == NK_GENERICNAME)
            {
                sb.Append(pNamedType->pName->asGENERICNAME()->pName->text);
                AppendParametersToPrototype(PRT_GENERIC, pNamedType->pName->asGENERICNAME()->pParams, sb, false);
            } 
            else 
            {
                pNamedType->pName->AppendNameTextToPrototype (sb, NULL);
            }
        }
        break;
    case NK_PREDEFINEDTYPE:
        sb += (rgPredefNames[pType->asPREDEFINEDTYPE()->iType].pszNice == NULL) ? rgPredefNames[pType->asPREDEFINEDTYPE()->iType].pszFull : rgPredefNames[pType->asPREDEFINEDTYPE()->iType].pszNice;
        break;
    case NK_ARRAYTYPE:
        AppendTypeToPrototype (pType->asARRAYTYPE()->pElementType, sb);
        sb += L"[";
        for (int iDim = 1; iDim < pType->asARRAYTYPE()->iDims; iDim++)
        {
            sb += L",";
        }
        sb += L"]";
        break;
    case NK_POINTERTYPE:
        AppendTypeToPrototype (pType->asPOINTERTYPE()->pElementType, sb);
        sb += L"*";
        break;
    case NK_NULLABLETYPE:
        AppendTypeToPrototype (pType->asNULLABLETYPE()->pElementType, sb);
        sb += L"?";
        break;
    case NK_TYPEWITHATTR:
        AppendTypeToPrototype (pType->asTYPEWITHATTR()->pType, sb);
        break;
    default:
        ASSERT(!"Bad type kind");
    }
}

////////////////////////////////////////////////////////////////////////////////
// BASENODE::GetGlyph

long BASENODE::GetGlyph ()
{
    ASSERT(false);
    return 0;
}

////////////////////////////////////////////////////////////////////////////////
// BASENODE::GetContainingFileName
//    Gets the file name of the first this parse node is in. Not very efficient,
//    must traverse upward the whole parse tree. Very useful for error reporting.
NAME * BASENODE::GetContainingFileName()
{
    // The top of the parse tree must be a namespace node.
    return GetRootNamespace()->pNameSourceFile;
}

////////////////////////////////////////////////////////////////////////////////
// BASENODE::GetRootNamespace

NAMESPACENODE *BASENODE::GetRootNamespace()
{
    BASENODE * pNode = this;

    while (pNode->pParent != NULL)
        pNode = pNode->pParent;

    return pNode->asNAMESPACE();
}

////////////////////////////////////////////////////////////////////////////////
// Given a single position, this function finds that
// position in the array using a binary search and returns its index.  If the
// position doesn't exist in the token stream, this function returns the index of last
// entry PRIOR to the given position (obviously assuming that the array is
// sorted).  The return value can thus be -1 (meaning all entries in the array
// are beyond the given position).
//

long LEXDATA::FindNearestPosition (const POSDATA &pos) const
{
    long    iTop = 0, iBot = this->TokenCount() - 1, iMid = iBot >> 1;

    // Zero in on a token near pos
    while (iBot - iTop >= 3)
    {
        long    c = this->TokenAt(iMid).Compare (pos);

        if (c == 0)
            return iMid;        // Wham!  exact match
        if (c > 0)
            iBot = iMid - 1;
        else
            iTop = iMid + 1;

        iMid = iTop + ((iBot - iTop) >> 1);
    }

    // Last-ditch -- check from iTop to iBot for a match, or closest to.
    for (iMid = iTop; iMid <= iBot; iMid++)
    {

        if (this->TokenAt(iMid) == pos)
        {
            if (this->TokenAt(iMid).Token() == TID_ENDFILE)
            {
                return iMid - 1;
            }
            return iMid;
        }
        if (this->TokenAt(iMid) > pos)
            return iMid - 1;
    }

    // This is it!
    return iMid - 1;
}


HRESULT LEXDATA::FindPositionOfText(PCWSTR pszText, POSDATA* pPos) const
{
    if (pszSource == NULL) {
        ASSERT(false);
        return E_FAIL;
    }

    for (long iLine = this->iLines - 1; iLine >= 0; iLine--) {
        //Skip this line if it comes after the text we're looking for
        if (TextAt(iLine, 0) > pszText) {
            continue;
        }

        *pPos = POSDATA(iLine, (long)(pszText - TextAt(iLine, 0)));
        return S_OK;
    }

    return E_FAIL;
}

////////////////////////////////////////////////////////////////////////////////

bool LEXDATA::TokenTouchesLine(long iTok, unsigned long iLine) const
{
    POSDATA posStart = this->TokenAt(iTok);
    POSDATA posEnd   = this->TokenAt(iTok).StopPosition();
    return (posStart.iLine <= iLine && iLine <= posEnd.iLine);
}

////////////////////////////////////////////////////////////////////////////////

HRESULT LEXDATA::CloneTokenStream(ITokenAllocator *pAllocator, long iFirstToken, long iTokenCount, long iAllocTokens, TOKENSTREAM *pTokenstream) const
{
    if (iAllocTokens < iTokenCount)
    {
        VSASSERT (iAllocTokens >= iTokenCount, "Have to have allocate enough space");
        return E_UNEXPECTED;
    }

    // pTokens
    {
        pTokenstream->pTokens = NULL;

        size_t    iSize = SizeMul(iAllocTokens, sizeof(CSTOKEN));
        if (iSize > 0)
        {
            pTokenstream->pTokens = (CSTOKEN *)pAllocator->AllocateMemory(iSize);
            if (!pTokenstream->pTokens)
                return E_OUTOFMEMORY;

            memcpy(pTokenstream->pTokens, pTokens + iFirstToken, iSize);
        }
        pTokenstream->iTokens = iTokenCount;

        // Make sure we reset the "HeapAllocated" bit in case we fail in the middle
        // of copying overhead of the tokens. This will prevent a crash when the caller
        // frees the tokens because of a failure.
        for (long i = 0; i < iTokenCount; i++)
        {
            pTokenstream->pTokens[i].iUserBits &= (~TF_HEAPALLOCATED);
        }

        // Clone overhead of each token
        for (long i = 0; i < iTokenCount; i++)
        {
            CSTOKEN &token = TokenAt(i + iFirstToken);
            if (token.HasOverhead())
            {
                HRESULT hr;
                if (FAILED (hr = token.CloneOverhead(pAllocator, pTokenstream->pTokens + i)))
                    return hr;
            }
        }
    }
    return S_OK;
}

////////////////////////////////////////////////////////////////////////////////
// LEXDATA::FreeClonedTokenStream

HRESULT LEXDATA::FreeClonedTokenStream(ITokenAllocator *pAllocator, TOKENSTREAM *pTokenstream) const
{
    if (pTokenstream->pTokens != NULL)
    {
        for (long i=0; i<pTokenstream->iTokens; i++)
            if (pTokenstream->pTokens[i].HasOverhead() && (pTokenstream->pTokens[i].iUserBits & TF_HEAPALLOCATED))
                pAllocator->Free (pTokenstream->pTokens[i].Overhead());

        pAllocator->Free (pTokenstream->pTokens);
        pTokenstream->pTokens = NULL;
        pTokenstream->iTokens = 0;
    }

    return S_OK;
}

////////////////////////////////////////////////////////////////////////////////
// LEXDATA::IsInsideSkippedPreProcessorRegion

bool LEXDATA::IsInsideSkippedPreProcessorRegion(const POSDATA& pos) const
{
    bool inside = false;
    // The array is sorted.  We could do a binary search, but the count should
    // be small!
    long iLine = (long)pos.iLine;
    for (long i = 0; i < this->TransitionLineCount(); i++)
    {
        if (this->TransitionLineAt(i) > iLine)
            break;

        inside = !inside;
    }

    return inside;
}

////////////////////////////////////////////////////////////////////////////////
// LEXDATA::IsInsideString

bool LEXDATA::IsInsideString(const POSDATA& pos) const
{
    // Find the nearest token
    long    i = this->FindNearestPosition(pos);
    if (i < 0)
    {
        return false;
    }

    CSTOKEN& token = this->TokenAt(i);
    if (token == pos || 
       (token.Token() != TID_STRINGLIT &&
        token.Token() != TID_VSLITERAL))
    {
        // We're either not in the string token, or it's not a string token
        return false;
    }
    else
    {
        if (token.IsUnterminated())
        {
            return (token.StopPosition() >= pos);
        }
        else
        {
            return (token.StopPosition() > pos);
        }
    }
}

////////////////////////////////////////////////////////////////////////////////
// LEXDATA::FindFirstTokenOnLine
//
// This function finds the first token on the given line.  If the line contains
// tokens, the return value will be S_OK -- if there are no tokens on the line,
// the return value will be S_FALSE.  *piFirst will always be set to a token
// index; if S_FALSE, the index will be the first token prior to iLine, which
// could be -1 (if all tokens are beyond iLine).
//
// Note that piFirst can be NULL, in which case this function serves as a
// "Are there any tokens on this line?" function.

HRESULT LEXDATA::FindFirstTokenOnLine (long iLine, long *piFirst) const
{
    POSDATA pos(iLine, 0);
    long    iTok = this->FindNearestPosition(pos);
    long    iIndex = -1;
    HRESULT hr = S_FALSE;

    // iTok will either be/start on iLine, or a line < iLine.  If it is on iLine
    // then we're done (there's a token a column zero, which is actually very
    // rare...)
    if (iTok >= 0 && TokenAt(iTok).iLine == (unsigned long) iLine)
    {
        // Boom -- we're done.
        iIndex = iTok;
        hr = S_OK;
    }
    else
    {
        // Look at the next token -- if it starts on iLine, we're golden.
        if (iTok + 1 < iTokens && TokenAt(iTok+1).iLine == (unsigned long) iLine)
        {
            iIndex = iTok + 1;
            hr = S_OK;
        }
        else
        {
            // No dice -- there are no tokens on this line.
            iIndex = iTok;
            hr = S_FALSE;
        }
    }

    if (piFirst != NULL)
        *piFirst = iIndex;

    return hr;
}

////////////////////////////////////////////////////////////////////////////////
// LEXDATA::FindLastTokenOnLine

HRESULT LEXDATA::FindLastTokenOnLine(long iLine, long *piToken) const
{
    HRESULT hr = S_OK;
    if (iLine < 0 || iLine >= this->iLines)
        return E_INVALIDARG;

    long iToken;
    long iTok;

    if (FAILED (hr = FindFirstTokenOnLine(iLine, &iToken)))
        return hr;

    for (iTok = iToken + 1; iTok < iTokens; iTok++)
    {
        if ((long)TokenAt(iTok).iLine > iLine)
            break;
    }

    *piToken = iTok-1;

    return hr;
}

////////////////////////////////////////////////////////////////////////////////
// LEXDATA::FindFirstNonWhiteChar

HRESULT LEXDATA::FindFirstNonWhiteChar (long iLine, long *piChar) const
{
    if (iLine < 0 || iLine >= this->iLines)
        return E_INVALIDARG;

    long    iChar = 0;

    PCWSTR psz;
    for (psz = this->TextAt (iLine, 0); IsWhitespaceChar (*psz); psz++)
        iChar++;

    *piChar = iChar;
    return (IsEndOfLineChar (*psz) || *psz == 0) ? S_FALSE : S_OK;
}

////////////////////////////////////////////////////////////////////////////////
// LEXDATA::FindEndOfComment

HRESULT LEXDATA::FindEndOfComment (long iComment, POSDATA *pposEnd) const
{
    if (iComment < 0 || iComment >= this->iTokens)
        return E_INVALIDARG;

    *pposEnd = this->TokenAt(iComment).StopPosition();
    return (this->TokenAt(iComment).iUserBits & TF_UNTERMINATED) ? S_FALSE : S_OK;
}

////////////////////////////////////////////////////////////////////////////////

bool LEXDATA::IsPreprocessorLine(long iLine, ICSNameTable* pNameTable) const
{
    PPTOKENID iToken;
    long iStart, iEnd;

    return SUCCEEDED(GetPreprocessorDirective(iLine, pNameTable, &iToken, &iStart, &iEnd));
}

////////////////////////////////////////////////////////////////////////////////
// LEXDATA::GetPreprocessorDirective
//
// If the given line is a preprocessor line, this function returns the PPTOKENID
// of the directive (i.e. PPT_DEFINE, etc.).  If not, this will return E_FAIL.

HRESULT LEXDATA::GetPreprocessorDirective (long iLine, ICSNameTable *pNameTable, PPTOKENID *piToken, long *piStart, long *piEnd) const
{
    if (iLine < 0 || iLine >= this->iLines)
        return E_INVALIDARG;

    PCWSTR psz = this->TextAt (iLine, 0);
    PCWSTR pszLine = psz;

    // Skip whitespace...
    while (IsWhitespaceChar (*psz))
        psz++;

    *piStart = (long)(psz - pszLine);

    // Check for '#' -- if not there, fail
    if (*psz++ != '#')
        return E_FAIL;

    // Skip more whitespace...
    while (IsWhitespaceChar (*psz))
        psz++;

    // Scan an identifier
    if (!IsIdentifierChar (*psz))
        return E_FAIL;

    PCWSTR  pszStart = psz;

    while (IsIdentifierCharOrDigit (*psz))
        psz++;

    *piEnd = (long)(psz - pszLine);

    NAME    *pName;
    HRESULT hr = E_FAIL;
    long    iToken;

    // Make a name out of it and see if it's a preprocessor keyword
    if (SUCCEEDED (hr = pNameTable->AddLen (pszStart, (long)(psz - pszStart), &pName)) &&
        SUCCEEDED (hr = pNameTable->GetPreprocessorDirective (pName, &iToken)))
    {
        *piToken = (PPTOKENID)iToken;
    }

    return hr;
}

////////////////////////////////////////////////////////////////////////////////
// LEXDATA::SpanContainsPreprocessorDirective

HRESULT LEXDATA::SpanContainsPreprocessorDirective(long iStartLine, long iEndLine, ICSNameTable *pNameTable, bool *pfContains) const
{
    if (iStartLine < 0 || iEndLine >= iLines)
    {
        VSFAIL("");
        return E_INVALIDARG;
    }

    if (iStartLine > iEndLine)
    {
        VSFAIL("");
        return E_INVALIDARG;
    }

    for (long iLine = iStartLine; iLine <= iEndLine; ++iLine)
    {
        PPTOKENID ppTok;
        long iStart, iEnd;
        if (SUCCEEDED (GetPreprocessorDirective(iLine, pNameTable, &ppTok, &iStart, &iEnd)))
        {
            *pfContains = true;
            return S_OK;
        }
    }
    *pfContains = false;
    return S_OK;
}


////////////////////////////////////////////////////////////////////////////////
// LEXDATA::GetLineLength

long LEXDATA::GetLineLength (long iLine) const
{
    if (iLine < 0 || iLine >= this->iLines)
        return 0;

    if (iLine == this->iLines - 1)
        return (long)wcslen (this->TextAt (iLine, 0));

    PCWSTR  psz = this->TextAt (iLine + 1, 0) - 1, pszThis = this->TextAt (iLine, 0);

    while (psz > pszThis && IsEndOfLineChar (*(psz-1)))
        psz--;

    return (long)(psz - pszThis);
}

////////////////////////////////////////////////////////////////////////////////
// LEXDATA::IsWhitespace

BOOL LEXDATA::IsWhitespace (long iStartLine, long iStartChar, long iEndLine, long iEndChar) const
{
    if (iStartLine < 0 || iEndLine >= this->iLines || iStartLine > iEndLine)
        return FALSE;

    PCWSTR  psz = this->TextAt (iStartLine, iStartChar);
    PCWSTR  pszStop = this->TextAt (iEndLine, iEndChar);

    while (psz < pszStop)
    {
        if (!IsWhitespaceChar (*psz) && !IsEndOfLineChar (*psz))
            return FALSE;
        psz++;
    }

    return TRUE;
}

////////////////////////////////////////////////////////////////////////////////
// LEXDATA::FindFirstPrecedingNonWhiteChar

HRESULT LEXDATA::FindFirstPrecedingNonWhiteChar (const POSDATA &pos, POSDATA *ppos) const
{
    if (pos > POSDATA (this->iLines - 1, (long)wcslen (this->TextAt (this->iLines - 1, 0))))
        return E_INVALIDARG;

    long        iLine = (long)pos.iLine, iChar = (long)pos.iChar;
    PCWSTR      pszLine = this->TextAt (iLine, 0);

    while (iLine > 0 || iChar > 0)
    {
        iChar--;
        while (iChar < 0)
        {
            if (iLine == 0)
            {
                ppos->iLine = 0;
                ppos->iChar = 0;
                return S_FALSE;
            }
            iLine--;
            pszLine = this->TextAt (iLine, 0);
            iChar = this->GetLineLength (iLine) - 1;
        }

        if (!IsWhitespaceChar (pszLine[iChar]))
        {
            ppos->iLine = iLine;
            ppos->iChar = iChar;
            return S_OK;
        }
    }

    // Only way to get here is to call w/ POSDATA(0,0)...
    ppos->iLine = ppos->iChar = 0;
    return S_FALSE;
}

////////////////////////////////////////////////////////////////////////////////

LEXDATA::LEXDATA()
    : pTokens(NULL),
    iTokens(0),
    piLines(NULL),
    iLines(0),
    piTransitionLines(NULL),
    iTransitionLines(0),
    piRegionStart(NULL),
    piRegionEnd(NULL),
    iRegions(0),
    pszSource(NULL),
    pIdentTable(NULL),
    pWarningMap(NULL)
{
}

////////////////////////////////////////////////////////////////////////////////

void LEXDATA::InitTokens (CSTOKEN *pTok, long iTok)
{
    this->pTokens = pTok;
    this->iTokens = iTok;
}

////////////////////////////////////////////////////////////////////////////////

void LEXDATA::UnsafeExposeTokens(CSTOKEN **ppTokens, long *piTokens) const
{
    (*ppTokens) = pTokens;
    (*piTokens) = iTokens;
}

////////////////////////////////////////////////////////////////////////////////

long LEXDATA::TokenCount() const
{ 
    return iTokens;
}

////////////////////////////////////////////////////////////////////////////////

CSTOKEN &LEXDATA::TokenAt (int iTok) const
{
    RETAILVERIFY(iTok >= 0);
    RETAILVERIFY(iTok < this->iTokens);
    return this->pTokens[iTok];
}

////////////////////////////////////////////////////////////////////////////////

void LEXDATA::InitLineOffsets (long *piLines, long iLines)
{
    this->piLines = piLines;
    this->iLines = iLines;
}

////////////////////////////////////////////////////////////////////////////////

void LEXDATA::UnsafeExposeLineOffsets(long **ppLines, long *piLines) const
{
    (*ppLines) = this->piLines;
    (*piLines) = this->iLines;
}

////////////////////////////////////////////////////////////////////////////////

long LEXDATA::LineCount() const
{ 
    return iLines; 
}

////////////////////////////////////////////////////////////////////////////////

long LEXDATA::LineOffsetAt (long iLine) const
{
    RETAILVERIFY(iLine >= 0);
    RETAILVERIFY(iLine < this->iLines);
    return piLines[iLine];
}

////////////////////////////////////////////////////////////////////////////////

long LEXDATA::TransitionLineCount() const
{
    return this->iTransitionLines;
}

////////////////////////////////////////////////////////////////////////////////

long LEXDATA::TransitionLineAt(long iTransitionLine) const
{
    RETAILVERIFY(iTransitionLine >= 0);
    RETAILVERIFY(iTransitionLine < this->iTransitionLines);
    return this->piTransitionLines[iTransitionLine];
}

////////////////////////////////////////////////////////////////////////////////

long LEXDATA::RegionCount() const
{
    return this->iRegions;
}

////////////////////////////////////////////////////////////////////////////////

long LEXDATA::RegionStartAt(long iRegion) const
{
    RETAILVERIFY(iRegion >= 0);
    RETAILVERIFY(iRegion < this->iRegions);
    return this->piRegionStart[iRegion];
}

////////////////////////////////////////////////////////////////////////////////

long LEXDATA::RegionEndAt(long iRegion) const
{
    RETAILVERIFY(iRegion >= 0);
    RETAILVERIFY(iRegion < this->iRegions);
    return this->piRegionEnd[iRegion];
}

////////////////////////////////////////////////////////////////////////////////

void LEXDATA::InitSource(PCWSTR pszSource)
{
    this->pszSource = pszSource;
}

////////////////////////////////////////////////////////////////////////////////

void LEXDATA::UnsafeExposeSource(PCWSTR *ppszSource) const
{
    (*ppszSource) = this->pszSource;
}

////////////////////////////////////////////////////////////////////////////////

bool LEXDATA::HasSource() const
{
    return (this->pszSource != NULL);
}

////////////////////////////////////////////////////////////////////////////////
// Helpful little translator functions

PCWSTR LEXDATA::TextAt (long iLine, long iChar)  const
{
    RETAILVERIFY(pszSource != NULL);
    return pszSource + LineOffsetAt(iLine) + iChar; 
}

////////////////////////////////////////////////////////////////////////////////

HRESULT LEXDATA::ExtractText(long iLine, BSTR *pbstrText) const
{
    return ExtractText(POSDATA(iLine, 0), pbstrText);
}

////////////////////////////////////////////////////////////////////////////////

HRESULT LEXDATA::ExtractText(const POSDATA &posStart, BSTR *pbstrText) const
{
    const POSDATA posEnd(posStart.iLine, this->GetLineLength(posStart.iLine));
    return ExtractText(posStart, posEnd, pbstrText);
}

////////////////////////////////////////////////////////////////////////////////

HRESULT LEXDATA::ExtractText(const POSDATA &posStart, const POSDATA &posEnd, BSTR *pbstrText) const
{
    if (posStart > posEnd)
    {
        VSFAIL("Bad positions");
        return E_INVALIDARG;
    }

    if (TokenCount() == 0)
    {
        return E_INVALIDARG;
    }

    if (posStart < POSDATA(0,0))
    {
        VSFAIL("Bad start position");
        return E_INVALIDARG;
    }

    if (posEnd > TokenAt(TokenCount() -1).StopPosition())
    {
        VSFAIL("Bad end position");
        return E_INVALIDARG;
    }

    PCWSTR psz1 = TextAt(posStart);
    PCWSTR psz2 = TextAt(posEnd);
    if (psz1 > psz2)
    {
        PCWSTR pszTemp = psz1;
        psz1 = psz2;
        psz2 = pszTemp;
    }
    long iLen = (long)(psz2 - psz1);

    CComBSTR bstr;
    bstr.Append(psz1, iLen);
    (*pbstrText) = bstr.Detach();
    return S_OK;
}

////////////////////////////////////////////////////////////////////////////////

BOOL LEXDATA::IsNoisyToken(long iToken) const
{
    return TokenAt(iToken).IsNoisy();
}

////////////////////////////////////////////////////////////////////////////////

BOOL LEXDATA::IsFirstTokenOnLine (long iToken, bool bSkipNoisyTokens)  const
{ 
    long iPrevToken = iToken;
    do 
    {
        iPrevToken--;
    }
    while(bSkipNoisyTokens && (iPrevToken >= 0) && IsNoisyToken(iPrevToken));

    return (iPrevToken < 0) || (TokenAt(iToken).iLine > TokenAt(iPrevToken).iLine); 
}

////////////////////////////////////////////////////////////////////////////////

BOOL LEXDATA::IsLastTokenOnLine (long iToken, bool bSkipNoisyTokens)  const
{ 
    long iNextToken = iToken;
    do 
    {
        iNextToken++;
    }
    while(bSkipNoisyTokens && (iNextToken < iTokens) && IsNoisyToken(iNextToken));

    // The token just before the ENDFILE token should be considered the last one on the line.
    return (iNextToken >= iTokens - 1) || (TokenAt(iNextToken).StopPosition().iLine > TokenAt(iToken).iLine); 
}

////////////////////////////////////////////////////////////////////////////////
// Use this when you want to skip tokens while ignoring TTF_NOISE tokens (i.e. comments).

long LEXDATA::PeekTokenIndexFrom (long iCur, long iPeek/* = 1*/) const
{ 
    return PeekTokenIndexFromInternal (pTokens, iTokens, iCur, iPeek); 
}

////////////////////////////////////////////////////////////////////////////////

long LEXDATA::NextNonNoisyToken (long iCur) const
{
    ASSERT (iCur >= 0);
    ASSERT (iCur < iTokens);
    while (iCur < iTokens && TokenAt(iCur).IsComment())
        ++iCur;
    return min (iCur, iTokens - 1);
}

////////////////////////////////////////////////////////////////////////////////

long LEXDATA::SkipNoisyTokens(long iCur, long iInc) const
{
    ASSERT(iInc == 1 || iInc == -1);
    ASSERT(iCur >= 0);
    ASSERT(iCur < iTokens);

    if (iInc == 1)
    {
        long i;
        for (i = iCur; i < iTokens - 1; i++)
        {
            if (!TokenAt(i).IsComment())
                break;
        }
        return i;
    }
    else if (iInc == -1)
    {
        long i;
        for (i = iCur; i >= 0; i--)
        {
            if (!TokenAt(i).IsComment())
                break;
        }
        return i;
    }
    else
        return iCur;
}

////////////////////////////////////////////////////////////////////////////////

/*static*/
long LEXDATA::PeekTokenIndexFromInternal (CSTOKEN * pTokens, long iTokens, const long iCur, long iPeek)     
{
    if (iPeek > 0)
    {
        long i;
        for (i=iCur + 1; i<iTokens-1; i++)
        {
            if (!pTokens[i].IsComment())
            {
                if (--iPeek == 0)
                    break;
            }
        }

        return min (i, iTokens - 1);
    }

    if (iPeek < 0)
    {
        long i;
        for (i=iCur - 1; i>=0; i--)
        {
            if (!pTokens[i].IsComment())
            {
                if (++iPeek == 0)
                    break;
            }
        }

        return max (i, 0);
    }
    else
    {
        return iCur;
    }
}

////////////////////////////////////////////////////////////////////////////////

PCWSTR LEXDATA::TextAt (const POSDATA &pos) const
{
    return TextAt (pos.iLine, pos.iChar);
}

////////////////////////////////////////////////////////////////////////////////

WCHAR LEXDATA::CharAt (long iLine, long iChar) const
{
    return TextAt (iLine, iChar)[0];
}

////////////////////////////////////////////////////////////////////////////////

WCHAR LEXDATA::CharAt (const POSDATA &pos) const
{
    return CharAt (pos.iLine, pos.iChar);
}

////////////////////////////////////////////////////////////////////////////////

BOOL LEXDATA::IsFirstTokenOnLine (long iToken) const
{
    return IsFirstTokenOnLine (iToken, false /*bSkipNosiyTokens*/);
}

////////////////////////////////////////////////////////////////////////////////

BOOL LEXDATA::IsLastTokenOnLine (long iToken) const
{
    return IsLastTokenOnLine (iToken, false /*bSkipNosiyTokens*/);
}

////////////////////////////////////////////////////////////////////////////////

BOOL LEXDATA::IsLineWhitespaceBefore (long iLine, long iChar) const
{
    return IsWhitespace (iLine, 0, iLine, iChar);
}

////////////////////////////////////////////////////////////////////////////////

BOOL LEXDATA::IsLineWhitespaceBefore (const POSDATA &pos) const
{
    return IsWhitespace (pos.iLine, 0, pos.iLine, pos.iChar);
}

////////////////////////////////////////////////////////////////////////////////

BOOL LEXDATA::IsLineWhitespaceAfter (long iLine, long iChar) const
{
    return IsWhitespace (iLine, iChar, iLine, GetLineLength (iLine));
}

////////////////////////////////////////////////////////////////////////////////

BOOL LEXDATA::IsLineWhitespaceAfter (const POSDATA &pos) const
{
    return IsWhitespace (pos.iLine, pos.iChar, pos.iLine, GetLineLength (pos.iLine)); 
}

////////////////////////////////////////////////////////////////////////////////

long LEXDATA::GetAbsolutePosition (long iToken) const
{
    return (long)(TextAt(TokenAt(iToken)) - TextAt(0,0));
}



////////////////////////////////////////////////////////////////////////////////
// CSTOKEN::IsEqual

bool CSTOKEN::IsEqual(CSTOKEN &other, bool fComparePositions)
{
    if (Token() != other.Token())
        return false;

    if (fComparePositions)
    {
        POSDATA p1 = *this;
        POSDATA p2 = other;
        if (p1 != p2)
            return false;

        p1 = this->StopPosition();
        p2 = other.StopPosition();
        if (p1 != p2)
            return false;
    }
    switch(Token())
    {
    case TID_IDENTIFIER:
        {
            if (HasOverhead())
            {
                if (other.HasOverhead())
                    return pEscName->pName == other.pEscName->pName;
                else
                    return false;
            }
            else
            {
                if (other.HasOverhead())
                    return false;
                else
                    return pName == other.pName;
            }
        }
        break;

    case TID_STRINGLIT:
        {
            STRLITERAL *pLit1 = StringLiteral();
            STRLITERAL *pLit2 = other.StringLiteral();
            if ( (pLit1->iSourceLength != pLit2->iSourceLength) ||
                 (pLit1->str.length    != pLit2->str.length))
            {
                return false;
            }

            return (wcsncmp(pLit1->str.text, pLit2->str.text, pLit1->str.length) == 0);
        }
        break;

    case TID_VSLITERAL:
        {
            VSLITERAL *pLit1 = VSLiteral();
            VSLITERAL *pLit2 = other.VSLiteral();
            if (pLit1->str.length != pLit2->str.length)
            {
                return false;
            }

            return (wcsncmp(pLit1->str.text, pLit2->str.text, pLit1->str.length) == 0);
        }
        break;


    case TID_NUMBER:
        {
            LITERAL *pLit1 = Literal();
            LITERAL *pLit2 = other.Literal();
            if ( (pLit1->iSourceLength != pLit2->iSourceLength))
            {
                return false;
            }

            return (wcscmp(pLit1->szText, pLit2->szText) == 0);
        }
        break;

    case TID_DOCCOMMENT:
    case TID_MLDOCCOMMENT:
        {
            DOCLITERAL *pLit1 = DocLiteral();
            DOCLITERAL *pLit2 = other.DocLiteral();

            return (wcscmp(pLit1->szText, pLit2->szText) == 0);
        }
        break;

    case TID_CHARLIT:
        {
            return chr.cCharValue == other.chr.cCharValue;
        }
        break;

    default:
        return true;
    }
}

////////////////////////////////////////////////////////////////////////////////

bool CSTOKEN::IsKeyword()
{
    return Token() < TID_IDENTIFIER;
}

////////////////////////////////////////////////////////////////////////////////
// CSTOKEN::IsModifier

bool CSTOKEN::IsAccessibilityModifier()
{
    switch (Token())
    {
    case TID_INTERNAL:
    case TID_PRIVATE:
    case TID_PROTECTED:
    case TID_PUBLIC:
        return true;

    default:
        return false;
    }
}

////////////////////////////////////////////////////////////////////////////////
// CSTOKEN::IsModifier

bool CSTOKEN::IsModifier()
{
    switch (Token())
    {
    case TID_EXTERN:
    case TID_OVERRIDE:
    case TID_READONLY:
    case TID_VIRTUAL:
    case TID_VOLATILE:
    case TID_ABSTRACT:
    case TID_INTERNAL:
    case TID_PRIVATE:
    case TID_PROTECTED:
    case TID_PUBLIC:
    case TID_SEALED:
    case TID_UNSAFE:
    case TID_NEW:
        return true;
    default:
        break;
    }

    return false;
}

////////////////////////////////////////////////////////////////////////////////

long CSTOKEN::Length () const
{
    switch (Token())
    {
        case TID_MLCOMMENT:
        case TID_VSLITERAL:
        {
            POSDATA &pos ((Token() == TID_MLCOMMENT) ? *this->pposEnd : this->pVSLiteral->posEnd);

            // If the token ends on the same line we can return a length
            if (pos.iLine == this->iLine)
                return pos.iChar - this->iChar;

            // Otherwise, the caller has to deal with this differently
            return -1;
        }

        case TID_UNKNOWN:
        case TID_IDENTIFIER:
        {
            if (HasOverhead())
                return this->pEscName->iLen;
            return (long)wcslen (this->pName->text);
        }

        case TID_STRINGLIT:
            return this->StringLiteral()->iSourceLength;

        case TID_DOCCOMMENT:
        case TID_MLDOCCOMMENT:
        {
            POSDATA &pos (this->pDocLiteral->posEnd);

            // If the token ends on the same line we can return a length
            if (pos.iLine == this->iLine)
                return pos.iChar - this->iChar;

            ASSERT(Token() == TID_MLDOCCOMMENT);

            // Otherwise, the caller has to deal with this differently
            return -1;
        }

 
        case TID_NUMBER:
            return this->Literal()->iSourceLength;

        case TID_CHARLIT:
            return this->chr.iCharLen;

        case TID_INVALID:
            return 1;

        default:
            return this->iLength;
    }
}

////////////////////////////////////////////////////////////////////////////////
// CSTOKEN::StopPosition

POSDATA CSTOKEN::StopPosition () const
{
    if (Token() == TID_MLCOMMENT)
        return *this->pposEnd;

    if (Token() == TID_VSLITERAL)
        return this->pVSLiteral->posEnd;

    if (Token() == TID_DOCCOMMENT || Token() == TID_MLDOCCOMMENT)
        return this->pDocLiteral->posEnd;

    POSDATA p(*this);
    p.iChar += this->Length();
    return p;
}

////////////////////////////////////////////////////////////////////////////////
// CSTOKEN::UpdateStartAndEndPositions

void CSTOKEN::UpdateStartAndEndPositions(int iLineDelta, int iCharDelta)
{
    // Update the "end" position if there is one
    POSDATA *pPosEnd = NULL;
    switch (Token()) 
    {
        case TID_DOCCOMMENT:
        case TID_MLDOCCOMMENT:
            if (DocLiteral())
                pPosEnd = &DocLiteral()->posEnd;
            break;
        case TID_MLCOMMENT:
            if (pposEnd)
                pPosEnd = pposEnd;
            break;
        case TID_VSLITERAL:
            if (VSLiteral())
                pPosEnd = &VSLiteral()->posEnd;
            break;
        default:
            break;
    }

    // Ok, the token actually has an "end" position, update it.
    if (pPosEnd != NULL)
    {
        // We only update "iChar" if posEnd is exactly on the same line as the start position
        if (iLine == pPosEnd->iLine)
            pPosEnd->iChar += iCharDelta;
        pPosEnd->iLine += iLineDelta;
    }

    // Update the token position. We only do it now because we needed to check
    // the old "iLine" value when updating pPosEnd.
    iLine += iLineDelta;
    iChar += iCharDelta;
}


////////////////////////////////////////////////////////////////////////////////
// CopyTokenOverhead [static]

static
HRESULT CopyTokenOverhead(CSTOKEN *pTokenDest, CSTOKEN *pTokenSrc, size_t cbSize, ITokenAllocator *pAllocator)
{
    ASSERT(pTokenSrc->HasOverhead());
    if (pTokenSrc->Overhead() != NULL)
    { 
        void *ptr = pAllocator->AllocateMemory(cbSize);
        if (ptr == NULL)  
            return E_OUTOFMEMORY;
        memcpy(ptr, pTokenSrc->Overhead(), cbSize);
        pTokenDest->SetRawOverhead(ptr);
        pTokenDest->iUserBits |= TF_HEAPALLOCATED;
    }
    else
    {
        pTokenDest->SetRawOverhead(NULL);
        pTokenDest->iUserBits |= TF_HEAPALLOCATED;
    }
    return S_OK;
}

////////////////////////////////////////////////////////////////////////////////
// CSTOKEN::CloneOverhead

HRESULT CSTOKEN::CloneOverhead(ITokenAllocator *pAllocator, CSTOKEN *pToken)
{
#define COPY_OVERHEAD(size)                                                     \
    {                                                                           \
        HRESULT hr_Temp = CopyTokenOverhead(pToken, this, size, pAllocator);    \
        if FAILED(hr_Temp)                                                      \
            return hr_Temp;                                                     \
    }

    // see CLexer::ScanToken to understand what we do here (look for calls to TokenMemAlloc)
    switch(Token())
    {
    case TID_STRINGLIT:
        {
            size_t cbSize = sizeof(STRLITERAL) + sizeof(WCHAR) * pStringLiteral->str.length;
            COPY_OVERHEAD(cbSize);
            pToken->pStringLiteral->str.text = (WCHAR *)(pToken->pStringLiteral + 1);
        }
        break;

    case TID_VSLITERAL:
        {
            size_t cbSize = sizeof(VSLITERAL) + sizeof(WCHAR) * pVSLiteral->str.length;
            COPY_OVERHEAD(cbSize);
            pToken->pVSLiteral->str.text = (WCHAR *)(pToken->pVSLiteral + 1);
        }
        break;

    case TID_MLDOCCOMMENT:
    case TID_DOCCOMMENT: 
        {
            size_t cbSize = sizeof(DOCLITERAL) + sizeof(WCHAR) * ((long)wcslen(pDocLiteral->szText) + 1);
            COPY_OVERHEAD(cbSize);
        }
        break;

    case TID_NUMBER:
        {
            size_t cbSize = sizeof(LITERAL) + sizeof(WCHAR) * pLiteral->iSourceLength;
            COPY_OVERHEAD(cbSize);
        }
        break;

    case TID_MLCOMMENT:
        {
            size_t cbSize = sizeof(POSDATA);
            COPY_OVERHEAD(cbSize);
        }
        break;

    case TID_IDENTIFIER:
        {
            if (HasOverhead())
            {
                size_t cbSize = sizeof(ESCAPEDNAME);
                COPY_OVERHEAD(cbSize);
            }
        }
        break;
    default:
        break;
    }

    return S_OK;
}


/////////////////////////////////////////////////////////////////////////////////
// Node Iterator Logic

#define LAST_FIELD_INFO { 0, NFK_NONE }

const NODE_INFO BASENODE_INFO = 
{
    NULL,
    { LAST_FIELD_INFO, }
};

#define DECLARE_NODE(nodename, nodebase)    \
const NODE_INFO nodename ## NODE_INFO =     \
{                                           \
    & (nodebase ## NODE_INFO),              \
    {
    
#define END_NODE()                          \
        LAST_FIELD_INFO, },                 \
};

#define CHILD_NODE(type, name)      { offsetof(CURRENT_TYPE, name), NFK_NODE },
#define CHILD_OPTIONAL_NODE(type, name) { offsetof(CURRENT_TYPE, name), NFK_OPTIONAL_NODE },
#define FIRST_NODE(type, name)      { offsetof(CURRENT_TYPE, name), NFK_FIRST_NODE },
#define PARENT_NODE(type, name)     { offsetof(CURRENT_TYPE, name), NFK_PARENT_NODE },
#define NEXT_NODE(type, name)       { offsetof(CURRENT_TYPE, name), NFK_NEXT_NODE },
#define CHILD_NAME(name)            { offsetof(CURRENT_TYPE, name), NFK_NAME },
#define INTERIOR_NODE(name)         { offsetof(CURRENT_TYPE, name), NFK_INTERIOR },
#define NODE_INDEX(name)            { offsetof(CURRENT_TYPE, name), NFK_INDEX },
#define CHILD_MEMBER(type, name)    { offsetof(CURRENT_TYPE, name), NFK_OTHER },

#include "allnodes.h"

////////////////////////////////////////////////////////////////////////////////
// NODEKIND enum -- describes all parse tree node kinds

const NODE_INFO * const g_NodeKindToInfo[] = {
#define NODEKIND(n,s,g,p) & (s ## NODE_INFO),
    #include "nodekind.h"
};

const NODE_INFO * NodeKindToInfo(NODEKIND kind) { return g_NodeKindToInfo[kind]; }

