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
// File: rstype.cpp
//
// Define implementation of ICorDebugType
//*****************************************************************************


#include "stdafx.h"
#include "winbase.h"
#include "corpriv.h"


HRESULT CordbType::GetStaticFieldValue(mdFieldDef fieldDef,
                       ICorDebugFrame *pFrame,
                       ICorDebugValue **ppValue)
{
    PUBLIC_REENTRANT_API_ENTRY(this);
    FAIL_IF_NEUTERED(this);
    VALIDATE_POINTER_TO_OBJECT(ppValue, ICorDebugValue **);

    if ((m_elementType != ELEMENT_TYPE_CLASS && m_elementType != ELEMENT_TYPE_VALUETYPE) || m_class == NULL)
    {
        return E_INVALIDARG;
    }
    ATT_REQUIRE_STOPPED_MAY_FAIL(GetProcess());

    HRESULT          hr = S_OK;
    BOOL             fSyncBlockField = FALSE;

    if (m_inst.m_cInst == 0)
    {
        hr = m_class->GetStaticFieldValue(fieldDef,
                                            pFrame,
                                            ppValue);
        goto LExit;
    }

    *ppValue = NULL;

    // Validate the token.
    if (!m_class->GetModule()->m_pIMImport->IsValidToken(fieldDef))
    {
        hr = E_INVALIDARG;
        goto LExit;
    }

    // Make sure we have enough info about the class.
    hr = Init(FALSE);

    if (!SUCCEEDED(hr))
        goto LExit;

    // Lookup the field given its metadata token.
    DebuggerIPCE_FieldData *pFieldData;

    hr = GetFieldInfo(fieldDef, &pFieldData);

    if (hr == CORDBG_E_ENC_HANGING_FIELD)
    {
        hr = CORDBG_E_STATIC_VAR_NOT_AVAILABLE;
    }

    if (!SUCCEEDED(hr))
        goto LExit;

    hr = CordbClass::GetStaticFieldValue2(m_class->GetModule(),
                                          pFieldData,
                                          fSyncBlockField,
                                          m_inst,
                                          pFrame,
                                          ppValue);

LExit:
    hr = CordbClass::PostProcessUnavailableHRESULT(hr, m_class->GetModule()->m_pIMImport, fieldDef);

    return hr;
}

// Combine E_T_s and rank together to get an id for the m_sharedtypes table
#define CORDBTYPE_ID(elementType,rank) ((unsigned int) elementType * (rank + 1) + 1)

CordbType::CordbType(CordbAppDomain *appdomain, CorElementType et, unsigned int rank)
: CordbBase(appdomain->GetProcess(), CORDBTYPE_ID(et,rank) , enumCordbType),
  m_elementType(et),
  m_appdomain(appdomain),
  m_class(NULL),
  m_rank(rank),
  m_spinetypes(2),
  m_objectSize(0),
  m_fields(0),
  m_fieldInfoNeedsInit(TRUE)
{
    m_typeHandleExact = LSPTR_TYPEHANDLE::NullPtr();

    _ASSERTE(m_elementType != ELEMENT_TYPE_VALUETYPE);

    m_appdomain->AddToTypeList(this);
}


CordbType::CordbType(CordbAppDomain *appdomain, CorElementType et, CordbClass *cls)
: CordbBase(appdomain->GetProcess(), et, enumCordbType),
  m_elementType(et),
  m_appdomain(appdomain),
  m_class(cls),
  m_rank(0),
  m_spinetypes(2),
  m_objectSize(0),
  m_fields(0),
  m_fieldInfoNeedsInit(TRUE)
{
    m_typeHandleExact = LSPTR_TYPEHANDLE::NullPtr();
    _ASSERTE(m_elementType != ELEMENT_TYPE_VALUETYPE);

    m_appdomain->AddToTypeList(this);
}


CordbType::CordbType(CordbType *tycon, CordbType *tyarg)
: CordbBase(tycon->GetProcess(), (UINT_PTR)tyarg, enumCordbType),
  m_elementType(tycon->m_elementType),
  m_appdomain(tycon->m_appdomain),
  m_class(tycon->m_class),
  m_rank(tycon->m_rank),
  m_spinetypes(2),
  m_objectSize(0),
  m_fields(0),
  m_fieldInfoNeedsInit(TRUE)
    // tyarg is added as part of instantiation -see below...
{
    m_typeHandleExact = LSPTR_TYPEHANDLE::NullPtr();
    _ASSERTE(m_elementType != ELEMENT_TYPE_VALUETYPE);

    m_appdomain->AddToTypeList(this);
}


ULONG STDMETHODCALLTYPE CordbType::AddRef()
{
    // This AddRef/Release pair creates a weak ref-counted reference to the class for this
    // type.  This avoids a circularity in ref-counted references between
    // classes and types - if we had a circularity the objects would never get
    // collected at all...
    //if (m_class)
    //  m_class->AddRef();
    return (BaseAddRef());
}
ULONG STDMETHODCALLTYPE CordbType::Release()
{
    //  if (m_class)
    //  m_class->Release();
    return (BaseRelease());
}

/*
    A list of which resources owened by this object are accounted for.

    HANDLED:
        CordbClass *m_class;  Weakly referenced by increasing count directly in AddRef() and Release()
        Instantiation   m_inst; // Internal pointers to CordbClass released in CordbClass::Neuter
        CordbHashTable   m_spinetypes; // Neutered
        CordbHashTable   m_fields; // Deleted in ~CordbType
*/

CordbType::~CordbType()
{
    _ASSERTE(IsNeutered());

    if(m_inst.m_ppInst)
        delete [] m_inst.m_ppInst;
    if(m_fields)
        delete [] m_fields;
}

// Neutered by CordbModule
void CordbType::Neuter(NeuterTicket ticket)
{
    if (!IsNeutered())
    {
        AddRef();
        {
            for (unsigned int i = 0; i < m_inst.m_cInst; i++) {
                m_inst.m_ppInst[i]->Release();
            }
            NeuterAndClearHashtable(&m_spinetypes, ticket);
            CordbBase::Neuter(ticket);
        }
        Release();
    }
}

HRESULT CordbType::QueryInterface(REFIID id, void **pInterface)
{
    if (id == IID_ICorDebugType)
        *pInterface = static_cast<ICorDebugType*>(this);
    else if (id == IID_IUnknown)
        *pInterface = static_cast<IUnknown*>(static_cast<ICorDebugType*>(this));
    else
    {
        *pInterface = NULL;
        return E_NOINTERFACE;
    }

    ExternalAddRef();
    return S_OK;
}


// Make a simple type with no type arguments by specifying a CorEleemntType,
// e.g. ELEMENT_TYPE_I1
//
// CordbType's are effectively a full representation of
// structured types.  They are hashed via a combination of their constituent
// elements (e.g. CordbClass's or CordbType's) and the element type that is used to
// combine the elements, or if they have no elements then via
// the element type alone.  The following  is used to create all CordbTypes.
//
//
HRESULT CordbType::MkType(CordbAppDomain *appdomain, CorElementType et, CordbType **pRes)
{
    _ASSERTE(appdomain != NULL);
    _ASSERTE(pRes != NULL);

    // Some points in the code create types via element types that are clearly objects but where
    // no further information is given.  This is always done shen creating a CordbValue, prior
    // to actually going over to the EE to discover what kind of value it is.  In all these
    // cases we can just use the type for "Object" - the code for dereferencing the value
    // will update the type correctly once it has been determined.  We don't do this for ELEMENT_TYPE_STRING
    // as that is actually a NullaryType and at other places in the code we will want exactly that type!
    if (et == ELEMENT_TYPE_CLASS ||
        et == ELEMENT_TYPE_SZARRAY ||
        et == ELEMENT_TYPE_ARRAY)
        et = ELEMENT_TYPE_OBJECT;

    switch (et)
    {
    case ELEMENT_TYPE_VOID:
    case ELEMENT_TYPE_FNPTR: // this one is included because we need a "seed" type to uniquely hash FNPTR types, i.e. the nullary FNPTR type is used as the type constructor for all function pointer types, when combined with an approproiate instantiation.
    case ELEMENT_TYPE_BOOLEAN:
    case ELEMENT_TYPE_CHAR:
    case ELEMENT_TYPE_I1:
    case ELEMENT_TYPE_U1:
    case ELEMENT_TYPE_I2:
    case ELEMENT_TYPE_U2:
    case ELEMENT_TYPE_I4:
    case ELEMENT_TYPE_U4:
    case ELEMENT_TYPE_I8:
    case ELEMENT_TYPE_U8:
    case ELEMENT_TYPE_R4:
    case ELEMENT_TYPE_R8:
    case ELEMENT_TYPE_STRING:
    case ELEMENT_TYPE_OBJECT:
    case ELEMENT_TYPE_TYPEDBYREF:
    case ELEMENT_TYPE_I:
    case ELEMENT_TYPE_U:
    case ELEMENT_TYPE_R:
        *pRes = appdomain->m_sharedtypes.GetBase(CORDBTYPE_ID(et,0));
        if (*pRes == NULL)
        {
            CordbType *pGP = new (nothrow) CordbType(appdomain, et, (unsigned int) 0);
            if (pGP == NULL)
                return E_OUTOFMEMORY;
            HRESULT hr = appdomain->m_sharedtypes.AddBase(pGP);
            if (SUCCEEDED(hr))
                *pRes = pGP;
            else {
                _ASSERTE(0);
                delete pGP;
            }

            return hr;
        }
        return S_OK;
    default:
        _ASSERTE(!"unexpected element type!");
        return E_FAIL;
    }

}

// Make a type with exactly one type argument by specifying
// ELEMENT_TYPE_PTR, ELEMENT_TYPE_BYREF, ELEMENT_TYPE_SZARRAY or
// ELEMENT_TYPE_ARRAY.
HRESULT CordbType::MkType(CordbAppDomain *appdomain, CorElementType et, ULONG rank, CordbType *tyarg, CordbType **pRes)
{
    _ASSERTE(appdomain != NULL);
    _ASSERTE(pRes != NULL);
    switch (et)
    {

    case ELEMENT_TYPE_PTR:
    case ELEMENT_TYPE_BYREF:
        _ASSERTE(rank == 0);
        goto unary;

    case ELEMENT_TYPE_SZARRAY:
        _ASSERTE(rank == 1);
        goto unary;

    case ELEMENT_TYPE_ARRAY:
unary:
        {
            CordbType *tycon = appdomain->m_sharedtypes.GetBase(CORDBTYPE_ID(et,rank));
            if (tycon == NULL)
            {
                tycon = new (nothrow) CordbType(appdomain, et, rank);
                if (tycon == NULL)
                    return E_OUTOFMEMORY;
                HRESULT hr = appdomain->m_sharedtypes.AddBase(tycon);
                if (FAILED(hr))
                {
                    _ASSERTE(0);
                    delete tycon;
                    return hr;
                }
            }
            Instantiation inst(1, &tyarg);
            return MkTyAppType(appdomain, tycon, inst, pRes);

        }
    case ELEMENT_TYPE_VALUEARRAY:
        _ASSERTE(!"unimplemented!");
        return E_FAIL;
    default:
        _ASSERTE(!"unexpected element type!");
        return E_FAIL;
    }

}

// Make a type for an instantiation of a class or value type, or just for the
// class or value type if it accepts no type parameters.
HRESULT CordbType::MkTyAppType(CordbAppDomain *appdomain, CordbType *tycon, const Instantiation &inst, CordbType **pRes)
{
    CordbType *c = tycon;
    for (unsigned int i = 0; i<inst.m_cClassTyPars; i++) {
        CordbType *c2 = c->m_spinetypes.GetBase((UINT_PTR) (inst.m_ppInst[i]));

        if (c2 == NULL)
        {
            c2 = new (nothrow) CordbType(c, inst.m_ppInst[i]);

            if (c2 == NULL)
                return E_OUTOFMEMORY;

            HRESULT hr = c->m_spinetypes.AddBase(c2);

            if (FAILED(hr))
            {
                _ASSERTE(0);
                delete c2;
                return (hr);
            }
            c2->m_inst.m_cInst = i+1;
            c2->m_inst.m_cClassTyPars = i+1;
            c2->m_inst.m_ppInst = new (nothrow) CordbType *[i+1];
            if (c2->m_inst.m_ppInst == NULL) {
                delete c2;
                return E_OUTOFMEMORY;
            }
            for (unsigned int j = 0; j<i+1; j++) {
                // Constructed types include pointers across to other types - increase
                // the reference counts on these....
                inst.m_ppInst[j]->AddRef();
                c2->m_inst.m_ppInst[j] = inst.m_ppInst[j];
//                printf("                                       tyarg %d = 0x%08x.\n", j, inst.m_ppInst[j]);
            }
        }
        c = c2;
    }
    *pRes = c;
    return S_OK;
}

HRESULT CordbType::MkType(CordbAppDomain *appdomain, CorElementType et,  CordbClass *tycon, const Instantiation &inst, CordbType **pRes)
{


    _ASSERTE(appdomain != NULL);
    _ASSERTE(pRes != NULL);
    switch (et)
    {
      // Normalize E_T_VALUETYPE away, so types do not record whether they are VCs or not, but CorDebugClass does.
      // Update our view of whether a class is a VC based on the evidence we have here.
    case ELEMENT_TYPE_VALUETYPE:
      _ASSERTE(tycon && (!tycon-> m_isValueClassKnown || tycon->m_isValueClass || !"A non-value class is being used with ELEMENT_TYPE_VALUETYPE"));
      tycon->m_isValueClass = true;
      tycon->m_isValueClassKnown = true;
      // drop through

    case ELEMENT_TYPE_CLASS:
        {
            // This probably isn't needed...
            if (tycon == NULL)
            {
                et = ELEMENT_TYPE_OBJECT;
                goto reallyObject;
            }
            CordbType *tyconty = NULL;

            tyconty = tycon->m_type;
            if (tyconty == NULL)
            {
                tyconty = new (nothrow) CordbType(appdomain, ELEMENT_TYPE_CLASS, tycon);
                if (tyconty == NULL)
                {
                    return E_OUTOFMEMORY;
                }
                tycon->m_type.Assign(tyconty);
            }
            _ASSERTE(tycon->m_type != NULL);

            return CordbType::MkTyAppType(appdomain, tyconty, inst, pRes);
        }
    default:
reallyObject:
        _ASSERTE(inst.m_cInst == 0);
        return MkType(appdomain, et, pRes);

    }
}


HRESULT CordbType::MkType(CordbAppDomain *appdomain, CorElementType et, const Instantiation &inst, CordbType **pRes)
{
    CordbType *tycon;
    _ASSERTE(et == ELEMENT_TYPE_FNPTR);
    HRESULT hr = MkType(appdomain, et, &tycon);
    if (!SUCCEEDED(hr)) {
        return hr;
    }
    return CordbType::MkTyAppType(appdomain, tycon, inst, pRes);
}



HRESULT CordbType::GetType(CorElementType *pType)
{
    PUBLIC_REENTRANT_API_ENTRY(this);
    FAIL_IF_NEUTERED(this);
    // See if this E_T_ClASS is really a value type?
    if (m_elementType == ELEMENT_TYPE_CLASS)
    {
        _ASSERTE(m_class);
        bool isVC;
        // Determining if something is a VC or not can involve asking the EE.
        // We could do it ourselves based on the metadata but it's non-trivial
        // determining if a class has System.ValueType as a parent (we have
        // to find and OpenScope the mscorlib.dll which we don't currently do
        // on the right-side).  But the IsValueClass call can fail if the
        // class is not yet loaded on the right side.  In that case we
        // ignore the failure and return ELEMENT_TYPE_CLASS
        HRESULT hr = m_class->IsValueClass(&isVC);
        if (!FAILED(hr) && isVC)
        {
            *pType = ELEMENT_TYPE_VALUETYPE;
            return S_OK;
        }
    }
    *pType = m_elementType;
    return S_OK;
}

HRESULT CordbType::GetClass(ICorDebugClass **pClass)
{
    PUBLIC_REENTRANT_API_ENTRY(this);
    ATT_REQUIRE_STOPPED_MAY_FAIL(GetProcess());

    if ((m_class == NULL) && (m_elementType == ELEMENT_TYPE_STRING ||
                              m_elementType == ELEMENT_TYPE_OBJECT))
    {
        Init(FALSE);
    }
    if (m_class == NULL)
    {
        *pClass = NULL;
        return CORDBG_E_CLASS_NOT_LOADED;
    }
    *pClass = m_class;
    m_class->ExternalAddRef();
    return S_OK;
}

HRESULT CordbType::GetRank(ULONG32 *pnRank)
{
    PUBLIC_REENTRANT_API_ENTRY(this);
    VALIDATE_POINTER_TO_OBJECT(pnRank, ULONG32 *);

    if (m_elementType != ELEMENT_TYPE_SZARRAY &&
        m_elementType != ELEMENT_TYPE_ARRAY)
        return E_INVALIDARG;

    *pnRank = (ULONG32) m_rank;

    return S_OK;
}
HRESULT CordbType::GetFirstTypeParameter(ICorDebugType **pType)
{
    PUBLIC_REENTRANT_API_ENTRY(this);
    VALIDATE_POINTER_TO_OBJECT(pType, ICorDebugType **);

    _ASSERTE(m_inst.m_ppInst != NULL);
    _ASSERTE(m_inst.m_ppInst[0] != NULL);
    *pType = m_inst.m_ppInst[0];
    if (*pType)
        (*pType)->AddRef();
    return S_OK;
}


HRESULT CordbType::MkUnparameterizedType(CordbAppDomain *appdomain, CorElementType et, CordbClass *cl,CordbType **pRes)
{
    return CordbType::MkType(appdomain, et, cl, Instantiation(), pRes);
}

void
CordbType::DestUnaryType(CordbType **pRes)
{
    _ASSERTE(m_elementType == ELEMENT_TYPE_PTR
        || m_elementType == ELEMENT_TYPE_BYREF
        || m_elementType == ELEMENT_TYPE_ARRAY
        || m_elementType == ELEMENT_TYPE_SZARRAY);
    _ASSERTE(m_inst.m_cInst == 1);
    _ASSERTE(m_inst.m_ppInst != NULL);
    *pRes = m_inst.m_ppInst[0];
}


void
CordbType::DestConstructedType(CordbClass **cls, Instantiation *inst)
{
    ASSERT(m_elementType == ELEMENT_TYPE_CLASS);
    *cls = m_class;
    *inst = m_inst;
}

void
CordbType::DestNaryType(Instantiation *inst)
{
    ASSERT(m_elementType == ELEMENT_TYPE_FNPTR);
    *inst = m_inst;
}


HRESULT
CordbType::SigToType(CordbModule *module, SigParser *pSigParser, const Instantiation &inst, CordbType **pRes)
{
    FAIL_IF_NEUTERED(module);
    INTERNAL_SYNC_API_ENTRY(module->GetProcess()); //

    _ASSERTE(pSigParser != NULL);


    //
    // Make a local copy of the SigParser since we are going to mutate it.
    //
    SigParser sigParser = *pSigParser;
    
    CorElementType elementType;
    HRESULT hr;
     
    IfFailRet(sigParser.GetElemType(&elementType));

    switch (elementType)
    {
    case ELEMENT_TYPE_VAR:
    case ELEMENT_TYPE_MVAR:
        {
            ULONG tyvar_num;

            IfFailRet(sigParser.GetData(&tyvar_num));

            _ASSERTE (tyvar_num < (elementType == ELEMENT_TYPE_VAR ? inst.m_cClassTyPars : inst.m_cInst - inst.m_cClassTyPars));
            _ASSERTE (inst.m_ppInst != NULL);
            *pRes = (elementType == ELEMENT_TYPE_VAR ? inst.m_ppInst[tyvar_num] : inst.m_ppInst[tyvar_num + inst.m_cClassTyPars]);
            return S_OK;
        }
    case ELEMENT_TYPE_GENERICINST:
        {
            // ignore "WITH", look at next ELEMENT_TYPE to get CLASS or VALUE
            
            IfFailRet(sigParser.GetElemType(&elementType));

            mdToken tycon;

            IfFailRet(sigParser.GetToken(&tycon));

            CordbClass *tyconcc;
            IfFailRet( module->ResolveTypeRefOrDef(tycon, &tyconcc));

            // The use of a class in a signature provides definite evidence as to whether it is a VC or not.
            _ASSERTE(!tyconcc->m_isValueClassKnown || (tyconcc->m_isValueClass ==  (elementType == ELEMENT_TYPE_VALUETYPE)) || !"A non-value class is being used with ELEMENT_TYPE_VALUETYPE");
            tyconcc->m_isValueClass = (elementType ==  ELEMENT_TYPE_VALUETYPE);
            tyconcc->m_isValueClassKnown = true;

            ULONG argCnt;

            IfFailRet(sigParser.GetData(&argCnt));

            S_UINT32 allocSize = S_UINT32( argCnt ) * S_UINT32( sizeof(CordbType*) );
            if (allocSize.IsOverflow() )
            {
                IfFailRet( E_OUTOFMEMORY );
            }
            CordbType **ppInst = (CordbType **) _alloca( allocSize.Value() );

            for (unsigned int i = 0; i<argCnt;i++) {

                IfFailRet(CordbType::SigToType(module, &sigParser, inst, &ppInst[i]));

                IfFailRet(sigParser.SkipExactlyOne());

            }

            Instantiation tyinst(argCnt,ppInst);
            return CordbType::MkType(module->GetAppDomain(), elementType, tyconcc, tyinst, pRes);
        }
    case ELEMENT_TYPE_CLASS:
    case ELEMENT_TYPE_VALUETYPE:  // OK: this E_T_VALUETYPE comes from signature
        {

            mdToken tycon;
            
            IfFailRet(sigParser.GetToken(&tycon));

            CordbClass *tyconcc;
            IfFailRet(module->ResolveTypeRefOrDef(tycon, &tyconcc));

            // The use of a class in a signature provides definite evidence as to whether it is a VC or not.
            _ASSERTE(!tyconcc->m_isValueClassKnown || (tyconcc->m_isValueClass ==  (elementType == ELEMENT_TYPE_VALUETYPE)) || !"A non-value class is being used with ELEMENT_TYPE_VALUETYPE");
            tyconcc->m_isValueClass = (elementType ==  ELEMENT_TYPE_VALUETYPE);
            tyconcc->m_isValueClassKnown = true;

            return CordbType::MkUnparameterizedType(module->GetAppDomain(), elementType, tyconcc, pRes);
        }
    case ELEMENT_TYPE_SENTINEL:
        case ELEMENT_TYPE_MODIFIER:
        case ELEMENT_TYPE_PINNED:
        {
            IfFailRet( CordbType::SigToType(module, &sigParser, inst, pRes) );
            // Throw away SENTINELS on all CordbTypes...
            return S_OK;
        }
    case ELEMENT_TYPE_CMOD_REQD:
    case ELEMENT_TYPE_CMOD_OPT:
        {
            mdToken typeRef;

            IfFailRet(sigParser.GetToken(&typeRef));

            IfFailRet( CordbType::SigToType(module, &sigParser, inst, pRes) );
            // Throw away CMOD on all CordbTypes...
            return S_OK;
        }

    case ELEMENT_TYPE_ARRAY:
        {
            CordbType *tyarg;

            IfFailRet( CordbType::SigToType(module, &sigParser, inst, &tyarg) );

            sigParser.SkipExactlyOne();

            ULONG rank;

            IfFailRet(sigParser.GetData(&rank));

            return CordbType::MkType(module->GetAppDomain(), elementType, rank, tyarg, pRes);
        }
    case ELEMENT_TYPE_SZARRAY:
        {
            CordbType *tyarg;

            IfFailRet( CordbType::SigToType(module, &sigParser, inst, &tyarg) );

            return CordbType::MkType(module->GetAppDomain(), elementType, 1, tyarg, pRes);
        }

    case ELEMENT_TYPE_PTR:
    case ELEMENT_TYPE_BYREF:
        {
            CordbType *tyarg;

            IfFailRet( CordbType::SigToType(module, &sigParser, inst, &tyarg) );

            return CordbType::MkType(module->GetAppDomain(),elementType, 0, tyarg, pRes);
        }

    case ELEMENT_TYPE_FNPTR:
        {
            ULONG argCnt;

            IfFailRet(sigParser.GetData(&argCnt)); // Skip callingConv

            IfFailRet(sigParser.GetData(&argCnt)); // Get number of parameters

            S_UINT32 allocSize = ( S_UINT32(argCnt) + S_UINT32(1) ) * S_UINT32( sizeof(CordbType*) );

            if ( allocSize.IsOverflow() )
            {
                IfFailRet( E_OUTOFMEMORY );
            }

            CordbType **ppInst = (CordbType **) _alloca( allocSize.Value() );

            for (unsigned int i = 0; i<=argCnt;i++) {

                IfFailRet( CordbType::SigToType(module, &sigParser, inst, &ppInst[i]) );

                sigParser.SkipExactlyOne();
            }

            Instantiation tyinst(argCnt+1,ppInst);

            return CordbType::MkType(module->GetAppDomain(), elementType, tyinst, pRes);
        }

    case ELEMENT_TYPE_VALUEARRAY:
        _ASSERTE(!"unimplemented!");
        return E_FAIL;
    case ELEMENT_TYPE_VOID:
    case ELEMENT_TYPE_BOOLEAN:
    case ELEMENT_TYPE_CHAR:
    case ELEMENT_TYPE_I1:
    case ELEMENT_TYPE_U1:
    case ELEMENT_TYPE_I2:
    case ELEMENT_TYPE_U2:
    case ELEMENT_TYPE_I4:
    case ELEMENT_TYPE_U4:
    case ELEMENT_TYPE_I8:
    case ELEMENT_TYPE_U8:
    case ELEMENT_TYPE_R4:
    case ELEMENT_TYPE_R8:
    case ELEMENT_TYPE_STRING:
    case ELEMENT_TYPE_TYPEDBYREF:
    case ELEMENT_TYPE_OBJECT:
    case ELEMENT_TYPE_I:
    case ELEMENT_TYPE_U:
    case ELEMENT_TYPE_R:
        return CordbType::MkType(module->GetAppDomain(), elementType, pRes);
    default:
        _ASSERTE(!"unexpected element type!");
        return E_FAIL;

    }
}

HRESULT CordbType::TypeDataToType(CordbAppDomain *pAppDomain, DebuggerIPCE_BasicTypeData *data, CordbType **pRes)
{
    FAIL_IF_NEUTERED(pAppDomain);
    INTERNAL_SYNC_API_ENTRY(pAppDomain->GetProcess()); //



    HRESULT hr = S_OK;
    CorElementType et = data->elementType;
    switch (et)
    {
    case ELEMENT_TYPE_ARRAY:
    case ELEMENT_TYPE_VALUEARRAY:
    case ELEMENT_TYPE_SZARRAY:
    case ELEMENT_TYPE_PTR:
    case ELEMENT_TYPE_BYREF:
        // For these element types the "Basic" type data only contains the type handle.
        // So we fetch some more data, and the go onto the "Expanded" case...
        {
            DebuggerIPCEvent event;
            pAppDomain->GetProcess()->InitIPCEvent(&event,
                DB_IPCE_GET_EXPANDED_TYPE_INFO,
                true,
                pAppDomain->GetADToken());
            event.ExpandType.typeHandle = data->typeHandle;

            // two-way event
            IfFailRet(pAppDomain->GetProcess()->m_cordb->SendIPCEvent(pAppDomain->GetProcess(), &event,
                sizeof(DebuggerIPCEvent)));

            IfFailRet(event.hr);

            _ASSERTE(event.type == DB_IPCE_GET_EXPANDED_TYPE_INFO_RESULT);
            IfFailRet(CordbType::TypeDataToType(pAppDomain,&event.ExpandTypeResult, pRes));
            return S_OK;
        }

    case ELEMENT_TYPE_FNPTR:
        {
            DebuggerIPCE_ExpandedTypeData e;
            e.elementType = et;
            e.NaryTypeData.typeHandle = data->typeHandle;
            return CordbType::TypeDataToType(pAppDomain, &e, pRes);
        }
    default:
        // For all other element types the "Basic" view of a type
        // contains the same information as the "expanded"
        // view, so just reuse the code for the Expanded view...
        DebuggerIPCE_ExpandedTypeData e;
        e.elementType = et;
        e.ClassTypeData.metadataToken = data->metadataToken;
        e.ClassTypeData.debuggerModuleToken = data->debuggerModuleToken;
        e.ClassTypeData.typeHandle = data->typeHandle;
        return CordbType::TypeDataToType(pAppDomain, &e, pRes);
    }
}


HRESULT CordbType::TypeDataToType(CordbAppDomain *pAppDomain, DebuggerIPCE_ExpandedTypeData *data, CordbType **pRes)
{
    INTERNAL_SYNC_API_ENTRY(pAppDomain->GetProcess()); //

    CorElementType et = data->elementType;
    HRESULT hr;
    switch (et)
    {

    case ELEMENT_TYPE_OBJECT:
    case ELEMENT_TYPE_VOID:
    case ELEMENT_TYPE_BOOLEAN:
    case ELEMENT_TYPE_CHAR:
    case ELEMENT_TYPE_I1:
    case ELEMENT_TYPE_U1:
    case ELEMENT_TYPE_I2:
    case ELEMENT_TYPE_U2:
    case ELEMENT_TYPE_I4:
    case ELEMENT_TYPE_U4:
    case ELEMENT_TYPE_I8:
    case ELEMENT_TYPE_U8:
    case ELEMENT_TYPE_R4:
    case ELEMENT_TYPE_R8:
    case ELEMENT_TYPE_STRING:
    case ELEMENT_TYPE_TYPEDBYREF:
    case ELEMENT_TYPE_I:
    case ELEMENT_TYPE_U:
    case ELEMENT_TYPE_R:
ETObject:
        IfFailRet (CordbType::MkType(pAppDomain, et, pRes));
        break;

    case ELEMENT_TYPE_CLASS:
    case ELEMENT_TYPE_VALUETYPE:  // OK: this E_T_VALUETYPE comes from the EE
    {
        if (data->ClassTypeData.metadataToken == mdTokenNil) {
            et = ELEMENT_TYPE_OBJECT;
            goto ETObject;
        }
        CordbModule* pClassModule = pAppDomain->LookupModule(data->ClassTypeData.debuggerModuleToken);
        if( pClassModule == NULL )
        {
            // We don't know anything about this module - shouldn't happen.
            _ASSERTE(!"Unrecognized module");
            return CORDBG_E_MODULE_NOT_LOADED;
        }

        CordbClass *tycon;
        IfFailRet (pClassModule->LookupOrCreateClass(data->ClassTypeData.metadataToken,&tycon));
        if (!(data->ClassTypeData.typeHandle.IsNull()))
        {
            IfFailRet (CordbType::InstantiateFromTypeHandle(pAppDomain, data->ClassTypeData.typeHandle, et, tycon, pRes));
            // Set the type handle regardless of how we found
            // the type.  For example if type was already
            // constructed without the type handle still set
            // it here.
            if (*pRes)
            {
                (*pRes)->m_typeHandleExact = data->ClassTypeData.typeHandle;
            }
            break;
        }
        else
        {
            IfFailRet (CordbType::MkUnparameterizedType(pAppDomain, et,tycon,pRes));
            break;
        }

    }
    case ELEMENT_TYPE_ARRAY:
    case ELEMENT_TYPE_SZARRAY:
    {
        CordbType *argty;
        IfFailRet (CordbType::TypeDataToType(pAppDomain, &(data->ArrayTypeData.arrayTypeArg), &argty));
        IfFailRet (CordbType::MkType(pAppDomain, et, data->ArrayTypeData.arrayRank, argty, pRes));
        break;
    }

    case ELEMENT_TYPE_PTR:
    case ELEMENT_TYPE_BYREF:
    {
        CordbType *argty;
        IfFailRet (CordbType::TypeDataToType(pAppDomain, &(data->UnaryTypeData.unaryTypeArg), &argty));
        IfFailRet (CordbType::MkType(pAppDomain, et, 0, argty, pRes));
        break;
    }
    case ELEMENT_TYPE_FNPTR:
    {
        IfFailRet (CordbType::InstantiateFromTypeHandle(pAppDomain, data->NaryTypeData.typeHandle, et, NULL, pRes));
        if (*pRes)
            (*pRes)->m_typeHandleExact = data->NaryTypeData.typeHandle;
        break;
    }

    default:
        _ASSERTE(!"unexpected element type!");
        return E_FAIL;

    }
    return S_OK;
}

HRESULT CordbType::InstantiateFromTypeHandle(CordbAppDomain *appdomain, LSPTR_TYPEHANDLE typeHandle, CorElementType et, CordbClass *tycon, CordbType **pRes)
{
    HRESULT hr;
    CQuickBytes genericArgsBuffer;
    BYTE eventBuffer[CorDBIPC_BUFFER_SIZE];
    DebuggerIPCEvent *retEvent = (DebuggerIPCEvent *) eventBuffer;

    // Should already by synced by caller.
    INTERNAL_SYNC_API_ENTRY(appdomain->GetProcess()); //
    _ASSERTE(appdomain->GetProcess()->GetSynchronized());

    // We've got a remote address that points to a type handle.
    // We need to send to the left side to get real information about
    // the type handle, including the type parameters.
    DebuggerIPCEvent event;
    // GENERICS: Collect up the class type parameters
    appdomain->GetProcess()->InitIPCEvent(&event,
                      DB_IPCE_GET_TYPE_HANDLE_PARAMS,
                      false,
                      appdomain->GetADToken());
    event.GetTypeHandleParams.typeHandle = typeHandle;

    hr = appdomain->GetProcess()->m_cordb->SendIPCEvent(appdomain->GetProcess(), &event, sizeof(DebuggerIPCEvent));
    hr = WORST_HR(hr, event.hr);
    // Stop now if we can't even send the event.
    if (!SUCCEEDED(hr))
        goto exit;

    // Wait for events to return from the RC. We expect at least one
    // result event.

    bool wait;
    bool fFirstEvent;
    unsigned int totalGenericArgsCount;
    unsigned int typarIndex;
    CordbType **ppInst;

    wait = true;
    fFirstEvent = true;
    totalGenericArgsCount = 0;
    typarIndex = 0;
    ppInst = NULL;

    while (wait)
    {
    hr = appdomain->GetProcess()->m_cordb->WaitForIPCEventFromProcess(appdomain->GetProcess(),
                                      appdomain,
                                      retEvent);

    if (!SUCCEEDED(hr))
      goto exit;

    _ASSERTE(retEvent->type == DB_IPCE_GET_TYPE_HANDLE_PARAMS_RESULT);

    // If this is the first event back from the RC, then create the
    // array.
    if (fFirstEvent)
      {
        fFirstEvent = false;
        totalGenericArgsCount = retEvent->GetTypeHandleParamsResult.totalGenericArgsCount;
        ppInst = (CordbType **) genericArgsBuffer.AllocNoThrow(sizeof(CordbType *) * totalGenericArgsCount);
        if (ppInst == NULL)
        {
            hr = E_OUTOFMEMORY;
            break;
        }
      }

    DebuggerIPCE_ExpandedTypeData *currentTyParData =
      &(retEvent->GetTypeHandleParamsResult.genericArgs);

    for (unsigned int i = 0; i < retEvent->GetTypeHandleParamsResult.genericArgsCount;i++)
    {
        hr = CordbType::TypeDataToType(appdomain, currentTyParData, &ppInst[typarIndex]);
        if (!SUCCEEDED(hr))
            goto exit;
        typarIndex++;
        currentTyParData++;
    }

    if (typarIndex >= totalGenericArgsCount)
        wait = false;
    }
    {
        Instantiation inst(totalGenericArgsCount, ppInst);
        if (et == ELEMENT_TYPE_FNPTR)
        {
            hr = CordbType::MkType(appdomain, et,inst, pRes);
        }
        else
        {
            hr = CordbType::MkType(appdomain, et,tycon,inst, pRes);
        }
    }

exit:
    return hr;
}



// Setting fForFieldInfoOnly = TRUE means it is OK if some of the collection
// of information fails, as long as we're able to at least collect some
// information about instance fields.
HRESULT CordbType::Init(BOOL fForceInit)
{
    INTERNAL_SYNC_API_ENTRY(GetProcess()); //

    HRESULT hr = S_OK;

    if (m_class && m_class->NeedsInit())
        fForceInit = TRUE;

    // Step 1. initialize the type constructor (if one exists)
    // and the (class) type parameters....
    if (m_elementType == ELEMENT_TYPE_CLASS)
    {
        _ASSERTE(m_class != NULL);
        IfFailRet( m_class->Init(fForceInit) );
        // That's all that's needed for simple classes and
        // value types, the normal case.
        if (m_class->m_typarCount == 0)
            return S_OK; // Non-generic, that's all - no clean-up required
    }

    _ASSERTE(m_elementType != ELEMENT_TYPE_CLASS || m_class->m_typarCount != 0);

    for (unsigned int i = 0; i<m_inst.m_cClassTyPars; i++)
    {
        _ASSERTE(m_inst.m_ppInst != NULL);
        _ASSERTE(m_inst.m_ppInst[i] != NULL);
        IfFailRet( m_inst.m_ppInst[i]->Init(fForceInit) );
    }

    // Step 2. Try to fetch the type handle if necessary (only
    // for instantiated class types, pointer types etc.)
    // We do this by preparing an event specifying the type and
    // then fetching the type handle from the left-side.  This
    // will not always succeed, as forcing the load of the type handle would be the
    // equivalent of doing a FuncEval, i.e. the instantiation may
    // not have been created.  But we try anyway to reduce the number of
    // failures.
    //
    // Note that in the normal case we will have the type handle from the EE
    // anyway, e.g. if the CordbType was created when reporting the type
    // of an actual object.

     // Initialize m_typeHandleExact if it needs it
     if (m_elementType == ELEMENT_TYPE_ARRAY ||
         m_elementType == ELEMENT_TYPE_SZARRAY ||
         m_elementType == ELEMENT_TYPE_BYREF ||
         m_elementType == ELEMENT_TYPE_PTR ||
         m_elementType == ELEMENT_TYPE_FNPTR ||
         (m_elementType == ELEMENT_TYPE_CLASS && m_class->m_typarCount > 0))
      {
         // It is OK if getting an exact type handle
         // fails with CORDBG_E_CLASS_NOT_LOADED.  In that case we leave
         // the type information incomplete and subsequent operations
         // will try to call Init() again.  The immediate operation will fail later if
         // TypeToBasicTypeData requests the exact type information for this type.
         hr = InitInstantiationTypeHandle(fForceInit);
         if (hr != CORDBG_E_CLASS_NOT_LOADED)
             IfFailRet(hr);
      }


     // For OBJECT and STRING we may not have a value for m_class
     // object.  Go try and get it.
     if (m_elementType == ELEMENT_TYPE_STRING ||
         m_elementType == ELEMENT_TYPE_OBJECT)
     {
         IfFailRet(InitStringOrObjectClass(fForceInit));
     }

    // Step 3. Fetch the information that is specific to the type where necessary...
    // Now we have the type handle for the constructed type, we can ask for the size of
    // the object.  Only do this for constructed value types.
    //
    // Note that the exact and/or approximate type handles may not be available.
    if ((m_elementType == ELEMENT_TYPE_CLASS) && m_class->m_typarCount > 0)
    {
        IfFailRet(InitInstantiationFieldInfo(fForceInit));
    }

    return S_OK;
}

// Setting fForFieldInfoOnly = TRUE means it is OK if some of the collection
// of information fails, as long as we're able to at least collect some
// information about instance fields.
HRESULT CordbType::InitInstantiationTypeHandle(BOOL fForceInit)
{

    // Check if we've already done this Init
    if (!fForceInit && !m_typeHandleExact.IsNull())
        return S_OK;

    HRESULT hr;

    CordbProcess *pProcess = GetProcess();
    S_UINT32 bufferSize = S_UINT32( sizeof(DebuggerIPCE_BasicTypeData) ) * 
                                            S_UINT32( m_inst.m_cClassTyPars );
    if( bufferSize.IsOverflow() )
    {
        return E_INVALIDARG;
    }
    DebuggerIPCE_BasicTypeData *bufferFrom = (DebuggerIPCE_BasicTypeData *) _alloca( bufferSize.Value() );

    // We will have already called Init on each of the type parameters further above.
    for (unsigned int i = 0; i<m_inst.m_cClassTyPars; i++)
    {
        _ASSERTE(m_inst.m_ppInst != NULL);
        _ASSERTE(m_inst.m_ppInst[i] != NULL);
       IfFailRet(m_inst.m_ppInst[i]->TypeToBasicTypeData(&bufferFrom[i]));
    }


    void *buffer = NULL;
    IfFailRet(pProcess->GetAndWriteRemoteBuffer(GetAppDomain(), bufferSize.Value(), bufferFrom, &buffer));

    DebuggerIPCEvent event;
    pProcess->InitIPCEvent(&event,
        DB_IPCE_GET_TYPE_HANDLE,
        true,
        GetAppDomain()->GetADToken());
    event.GetTypeHandle.genericArgsCount = m_inst.m_cClassTyPars;
    event.GetTypeHandle.genericArgsBuffer = buffer;
    TypeToExpandedTypeData(&(event.GetTypeHandle.typeData));

    // Note: two-way event here...
    IfFailRet(pProcess->m_cordb->SendIPCEvent(pProcess,
        &event,
        sizeof(DebuggerIPCEvent)));

    IfFailRet(event.hr);

    _ASSERTE(event.type == DB_IPCE_GET_TYPE_HANDLE_RESULT);
    _ASSERTE(event.GetTypeHandleResult.typeHandleExact != NULL);
    m_typeHandleExact = event.GetTypeHandleResult.typeHandleExact;

    return S_OK;

}

HRESULT CordbType::InitStringOrObjectClass(BOOL fForceInit)
{
    HRESULT hr;

    // Check if we've already done this Init
    if (!fForceInit && m_class != NULL)
        return S_OK;

    CordbProcess *pProcess = GetProcess();
    DebuggerIPCEvent event;
    pProcess->InitIPCEvent(&event,
        DB_IPCE_GET_SIMPLE_TYPE,
        true,
        m_appdomain->GetADToken());

    event.GetSimpleType = m_elementType;
    // Note: two-way event here...
    IfFailRet(pProcess->m_cordb->SendIPCEvent(pProcess,
        &event,
        sizeof(DebuggerIPCEvent)));

    IfFailRet(event.hr);
    _ASSERTE(event.type == DB_IPCE_GET_SIMPLE_TYPE_RESULT);

    _ASSERTE(event.GetSimpleTypeResult.debuggerModuleToken != NULL);
    CordbModule* pTypeModule = m_appdomain->LookupModule(event.GetSimpleTypeResult.debuggerModuleToken);
    _ASSERTE(pTypeModule != NULL);
    IfFailRet(pTypeModule->LookupOrCreateClass(event.GetSimpleTypeResult.metadataToken, &m_class));

    _ASSERTE(m_class != NULL);

    m_class->AddRef();

    return S_OK;
}

HRESULT CordbType::InitInstantiationFieldInfo(BOOL fForceInit)
{
    HRESULT hr;

    // Check if we've already done this Init
    if (!m_fieldInfoNeedsInit && !fForceInit)
    {
        return S_OK;
    }

    _ASSERTE(m_elementType == ELEMENT_TYPE_CLASS);
    _ASSERTE(m_class->m_typarCount > 0);

    LSPTR_TYPEHANDLE typeHandleApprox = m_typeHandleExact;

    // If the exact type handle is not available then get the approximate type handle.
    if (typeHandleApprox.IsNull())
    {
        unsigned int typeDataNodeCount = 0;
        CordbType::CountTypeDataNodes(this, &typeDataNodeCount);

        S_UINT32 bufferSize = S_UINT32( sizeof(DebuggerIPCE_TypeArgData) ) * S_UINT32( typeDataNodeCount );
        if( bufferSize.IsOverflow() )
            return E_INVALIDARG;
        DebuggerIPCE_TypeArgData *bufferFrom = (DebuggerIPCE_TypeArgData *) _alloca( bufferSize.Value() );

        DebuggerIPCE_TypeArgData *curr = bufferFrom;
        GatherTypeData(this, &curr);

        CordbProcess *pProcess = GetProcess();
        void *buffer;
        IfFailRet(pProcess->GetAndWriteRemoteBuffer(GetAppDomain(), bufferSize.Value(), bufferFrom, &buffer));

        DebuggerIPCEvent event;
        pProcess->InitIPCEvent(&event,
            DB_IPCE_GET_APPROX_TYPE_HANDLE,
            true,
            GetAppDomain()->GetADToken());
        event.GetApproxTypeHandle.typeDataNodeCount = typeDataNodeCount;
        event.GetApproxTypeHandle.typeDataBuffer = buffer;

        // Note: two-way event here...
        IfFailRet(pProcess->m_cordb->SendIPCEvent(pProcess,
            &event,
            sizeof(DebuggerIPCEvent)));

        IfFailRet(event.hr);

        _ASSERTE(event.type == DB_IPCE_GET_APPROX_TYPE_HANDLE_RESULT);
        _ASSERTE(!event.GetApproxTypeHandleResult.typeHandleApprox.IsNull());
        typeHandleApprox = event.GetApproxTypeHandleResult.typeHandleApprox;

    }

    // OK, now get the field info if we can.
    CordbProcess *pProcess = GetProcess();
    DebuggerIPCEvent event;
    pProcess->InitIPCEvent(&event,
        DB_IPCE_GET_CLASS_INFO,
        false,
        m_appdomain->GetADToken());
    event.GetClassInfo.metadataToken = 0;
    event.GetClassInfo.debuggerModuleToken.Set(NULL);
    event.GetClassInfo.typeHandleExact = m_typeHandleExact;
    event.GetClassInfo.typeHandleApprox = typeHandleApprox;

    IfFailRet(pProcess->m_cordb->SendIPCEvent(pProcess, &event,
        sizeof(DebuggerIPCEvent)));
    IfFailRet(event.hr);

    // Wait for events to return from the RC. We expect at least one
    // class info result event.
    DebuggerIPCEvent *retEvent;
    retEvent = (DebuggerIPCEvent *) _alloca(CorDBIPC_BUFFER_SIZE);

    bool fFirstEvent = true;
    bool fGotUnallocatedStatic = false;
    unsigned int fieldIndex = 0;
    unsigned int totalFieldCount = 0;

    do
    {
        IfFailRet(pProcess->m_cordb->WaitForIPCEventFromProcess(pProcess,
            GetAppDomain(),
            retEvent));

        _ASSERTE(retEvent->type == DB_IPCE_GET_CLASS_INFO_RESULT);

        // If this is the first event back from the RC, then create the
        // array to hold the field.
        if (fFirstEvent)
        {
            fFirstEvent = false;

            m_objectSize = retEvent->GetClassInfoResult.objectSize;
            // Shouldn't ever loose fields, and should only get back information on
            // instance fields.  This should be _exactly_ the number of fields reported
            // by the parent class, as that will have been updated to take into account
            // EnC fields when it was initialized above.
            _ASSERTE(retEvent->GetClassInfoResult.varCount == m_class->m_varCount);

            totalFieldCount = retEvent->GetClassInfoResult.varCount;
            // Since we don't keep pointers to the m_fields elements,
            // just toss it & get a new one.
            if (m_fields != NULL)
            {
                delete m_fields;
                m_fields = NULL;
            }
            if (totalFieldCount > 0)
            {
                m_fields = new (nothrow) DebuggerIPCE_FieldData[totalFieldCount];

                if (m_fields == NULL)
                    IfFailRet(E_OUTOFMEMORY);
            }
        }

        DebuggerIPCE_FieldData *currentFieldData =
            &(retEvent->GetClassInfoResult.fieldData);

        for (unsigned int i = 0; i < retEvent->GetClassInfoResult.fieldCount; i++)
        {
            // If we have an exact type handle, then any "normal" statics should
            // have their address filled in.
            if( m_typeHandleExact.IsNull() &&
                currentFieldData->fldIsStatic &&
                !currentFieldData->fldIsContextStatic &&
                !currentFieldData->fldIsTLS &&
                currentFieldData->GetStaticAddress() == NULL )
            {
                // The address for a regular static field isn't available yet, even though we
                // have an exact type handle.
                // How can this happen?  Statics appear to get allocated during domain load.
                // There may be some lazieness or a race-condition involved.
                fGotUnallocatedStatic = true;
            }

            m_fields[fieldIndex] = *currentFieldData;

            currentFieldData++;
            fieldIndex++;
        }

    }
    while( fieldIndex < totalFieldCount );

    // Only record "we're done" if we had exact type information in the first place
    // and we were able to retreive all the static variables
    if (!m_typeHandleExact.IsNull() && !fGotUnallocatedStatic )
    {
        m_fieldInfoNeedsInit = FALSE;
    }

    return S_OK;
}

HRESULT
CordbType::GetUnboxedObjectSize(ULONG32 *pObjectSize)
{
    INTERNAL_SYNC_API_ENTRY(GetProcess()); //

    HRESULT hr = S_OK;
    bool isVC;
    IfFailRet (IsValueType(&isVC));

    if (isVC)
    {
        *pObjectSize = 0;

        hr = Init(FALSE);

        if (!SUCCEEDED(hr))
            return hr;

        *pObjectSize = (ULONG) ((m_class->m_typarCount == 0) ? m_class->m_objectSize : this->m_objectSize);

        return hr;
    }
    else
    {
        // We need to use a temporary variable here -- attempting to cast among pointer types
        // (i.e., (PCCOR_SIGNATURE) &m_elementType) yields incorrect results on big-endian machines
        COR_SIGNATURE corSig = (COR_SIGNATURE) m_elementType;
        
        SigParser sigParser(&corSig, sizeof(corSig));

        ULONG size;

        IfFailRet(sigParser.PeekElemTypeSize(&size));

        *pObjectSize = size;
        return hr;
    }


}

// Nb. CordbType::Init will call this.  The operation
// fails if the exact type information has been requested but was not availab.e
HRESULT CordbType::TypeToBasicTypeData(DebuggerIPCE_BasicTypeData *data)
{
    switch (m_elementType)
    {
    case ELEMENT_TYPE_ARRAY:
    case ELEMENT_TYPE_VALUEARRAY:
    case ELEMENT_TYPE_SZARRAY:
    case ELEMENT_TYPE_BYREF:
    case ELEMENT_TYPE_PTR:
        data->elementType = m_elementType;
        data->metadataToken = mdTokenNil;
        data->debuggerModuleToken.Set(NULL);
        data->typeHandle = m_typeHandleExact;
        if (data->typeHandle.IsNull())
        {
            //_ASSERTE(!"CORDBG_E_CLASS_NOT_LOADED");
            return CORDBG_E_CLASS_NOT_LOADED;
        }
        _ASSERTE(data->typeHandle != NULL );
        break;

    case ELEMENT_TYPE_CLASS:
        _ASSERTE(m_class != NULL);
        data->elementType = m_class->m_isValueClass ? ELEMENT_TYPE_VALUETYPE : ELEMENT_TYPE_CLASS;
        data->metadataToken = m_class->m_token;
        data->debuggerModuleToken = m_class->GetModule()->m_debuggerModuleToken;
        data->typeHandle = m_typeHandleExact;
        if (m_class->m_typarCount > 0 && data->typeHandle.IsNull())
        {
            //_ASSERTE(!"CORDBG_E_CLASS_NOT_LOADED");
            return CORDBG_E_CLASS_NOT_LOADED;
        }
        break;
    default:
        data->elementType = m_elementType;
        data->metadataToken = mdTokenNil;
        data->debuggerModuleToken.Set(NULL);
        data->typeHandle = LSPTR_TYPEHANDLE::NullPtr();
        break;
    }
    return S_OK;
}

// Nb. CordbType::Init need NOT have been called before this...
// Also, this does not write the type arguments.  How this is done depends
// depends on where this is called from.
void CordbType::TypeToExpandedTypeData(DebuggerIPCE_ExpandedTypeData *data)
{

    switch (m_elementType)
    {
    case ELEMENT_TYPE_ARRAY:
    case ELEMENT_TYPE_SZARRAY:

        data->ArrayTypeData.arrayRank = m_rank;
        data->elementType = m_elementType;
        break;

    case ELEMENT_TYPE_BYREF:
    case ELEMENT_TYPE_PTR:
    case ELEMENT_TYPE_FNPTR:

        data->elementType = m_elementType;
        break;

    case ELEMENT_TYPE_CLASS:
        {
            data->elementType = m_class->m_isValueClass ? ELEMENT_TYPE_VALUETYPE : ELEMENT_TYPE_CLASS;
            data->ClassTypeData.metadataToken = m_class->m_token;
            data->ClassTypeData.debuggerModuleToken = m_class->m_module->m_debuggerModuleToken;
            data->ClassTypeData.typeHandle = LSPTR_TYPEHANDLE::NullPtr();

            break;
        }
    case ELEMENT_TYPE_VALUEARRAY:
        _ASSERTE(!"unimplemented!");
    case ELEMENT_TYPE_END:
        _ASSERTE(!"bad element type!");

    default:
        data->elementType = m_elementType;
        break;
    }
}


void CordbType::TypeToTypeArgData(DebuggerIPCE_TypeArgData *data)
{
  TypeToExpandedTypeData(&(data->data));
  data->numTypeArgs = m_inst.m_cClassTyPars;
}


// Does not include primitives.
HRESULT CordbType::IsValueType(bool *isValueClass)
{
  *isValueClass = false;
  HRESULT hr = S_OK;
  if (m_elementType == ELEMENT_TYPE_CLASS)
      hr = m_class->IsValueClass(isValueClass);
  return hr;
}


// If this is a ptr type, get the CordbType that it points to.
// Eg, for CordbType("Int*") or CordbType("Int&"), returns CordbType("Int").
// If not a ptr type, returns null.
// Since it's all internal, no reference counting.
// This is effectively a specialized version of DestUnaryType.
CordbType * CordbType::GetPointerElementType()
{
    if ((m_elementType != ELEMENT_TYPE_PTR) && (m_elementType != ELEMENT_TYPE_BYREF))
    {
        return NULL;
    }

    CordbType * pOut;
    DestUnaryType(&pOut);

    _ASSERTE(pOut != NULL);
    return pOut;
}

// Helper for IsGcRoot.
// Determine if the element type is a non GC-root candidate.
static inline bool IsElementTypeNonGcRoot(CorElementType et)
{
    // Functon ptrs are raw data, not GC-roots.
    if (et == ELEMENT_TYPE_FNPTR)
    {
        return true;
    }

    // This is almost exactly if we're a primitive, but
    // primitives include some things that could be GC-roots, so we strip those out,
    return CorIsPrimitiveType(et)
        && (et != ELEMENT_TYPE_STRING) && (et != ELEMENT_TYPE_VOID); // exlcude these from primitives

}

// Helper for IsGcRoot
// Non-gc roots include Value types + non-gc elemement types (like E_T_I4, E_T_FNPTR)
// Returns true if we know we're not a GC-root
// false if we still might be (so caller must do further checkin)
static inline bool _IsNonGCRootHelper(CordbType * pType)
{
    _ASSERTE(pType != NULL);

    CorElementType et = pType->GetElementType();
    if (IsElementTypeNonGcRoot(et))
    {
        return true;
    }

    HRESULT hr = S_OK;
    bool fValueClass;

    // If we are a value-type, then we can't be a Gc-root.
    hr = pType->IsValueType(&fValueClass);
    if (FAILED(hr) || fValueClass)
    {
        return true;
    }

    // Don't know
    return false;
}

//-----------------------------------------------------------------------------
// Is this type a GC-root. (Not to be confused w/ "does this contain embedded GC roots")
// All object references are GC-roots. E_T_PTR are actually not GC-roots.
//-----------------------------------------------------------------------------
bool CordbType::IsGCRoot()
{
    // If it's a E_T_PTR type, then look at what it's a a pointer of.
    CordbType * pPtr = this->GetPointerElementType();
    if (pPtr == NULL)
    {
        // If non pointer, than we can just look at our current type.
        return !_IsNonGCRootHelper(this);
    }

    return !_IsNonGCRootHelper(pPtr);
}


HRESULT CordbType::EnumerateTypeParameters(ICorDebugTypeEnum **ppTypeParameterEnum)
{
    PUBLIC_API_ENTRY(this);
    VALIDATE_POINTER_TO_OBJECT(ppTypeParameterEnum, ICorDebugTypeEnum **);
    ATT_REQUIRE_STOPPED_MAY_FAIL(GetProcess());


    CordbTypeEnum *icdTPE = CordbTypeEnum::Build(this->m_appdomain, this->m_inst.m_cInst, this->m_inst.m_ppInst);
    if ( icdTPE == NULL )
    {
        (*ppTypeParameterEnum) = NULL;
        return E_OUTOFMEMORY;
    }

    (*ppTypeParameterEnum) = static_cast<ICorDebugTypeEnum*> (icdTPE);
    icdTPE->ExternalAddRef();
    return S_OK;
}

HRESULT CordbType::GetBase(ICorDebugType **ppType)
{
    PUBLIC_REENTRANT_API_ENTRY(this);
    ATT_ALLOW_LIVE_DO_STOPGO(this->GetProcess());
    HRESULT hr;
    LOG((LF_CORDB, LL_EVERYTHING, "CordbType::GetBase called\n"));
    VALIDATE_POINTER_TO_OBJECT(ppType, ICorDebugType **);

    if (m_elementType != ELEMENT_TYPE_CLASS)
        return E_INVALIDARG;

    CordbType *res = NULL;

    _ASSERTE(m_class != NULL);

    // Get the supertype from metadata for m_class
    mdToken extends;
    _ASSERTE(m_class->GetModule()->m_pIMImport != NULL);
    IfFailRet (m_class->GetModule()->m_pIMImport->GetTypeDefProps(m_class->m_token, NULL, 0, NULL, NULL, &extends));

    if (extends == mdTypeDefNil || extends == mdTypeRefNil || extends == mdTokenNil)
    {
        res = NULL;
    }
    else if (TypeFromToken(extends) == mdtTypeSpec)
    {
        PCCOR_SIGNATURE sig;
        ULONG sigsz;

        // Get the signature for the constructed supertype...
        IfFailRet(m_class->GetModule()->m_pIMImport->GetTypeSpecFromToken(extends, &sig, &sigsz));

        _ASSERTE(sig != NULL);

        SigParser sigParser(sig, sigsz);

        // Instantiate the signature of the supertype using the type instantiation for
        // the current type....
        IfFailRet( SigToType(m_class->GetModule(), &sigParser, m_inst, &res) );
    }
    else if (TypeFromToken(extends) == mdtTypeRef || TypeFromToken(extends) == mdtTypeDef)
    {
        CordbClass *superclass;
        IfFailRet( m_class->GetModule()->ResolveTypeRefOrDef(extends,&superclass));
        _ASSERTE(superclass != NULL);
        IfFailRet( MkUnparameterizedType(m_appdomain, ELEMENT_TYPE_CLASS, superclass, &res) );
    }
    else
    {
        res = NULL;
        _ASSERTE(!"unexpected token!");
    }

    (*ppType) = res;
    if (*ppType)
        res->AddRef();
    return S_OK;
}

HRESULT CordbType::GetFieldInfo(mdFieldDef fldToken, DebuggerIPCE_FieldData **ppFieldData)
{
    INTERNAL_SYNC_API_ENTRY(GetProcess()); //
    HRESULT hr = S_OK;

    if (m_elementType != ELEMENT_TYPE_CLASS)
      return E_INVALIDARG;

    *ppFieldData = NULL;

    // Initialize so that the field information is up-to-date.
    hr = Init(FALSE);

    if (!SUCCEEDED(hr))
        return hr;

    if (m_class->m_typarCount > 0)
    {
      if (m_fields == NULL)

        return CORDBG_E_FIELD_NOT_AVAILABLE;

      else
      {

          // Use a static helper function in CordbClass, though we're really
          // searching through this->m_fields
          return CordbClass::SearchFieldInfo(m_class->GetModule(), m_class->m_varCount, m_fields, m_class->m_token, fldToken, ppFieldData);
      }

    }
    else
    {
        return m_class->GetFieldInfo(fldToken, ppFieldData); // this is for non-generic types....
    }
}

//
HRESULT CordbType::GetParentType(CordbClass *baseClass, CordbType **ppRes)
{
    INTERNAL_SYNC_API_ENTRY(GetProcess()); //

    HRESULT hr = S_OK;
    _ASSERTE(ppRes);
    *ppRes = NULL;
    CordbType *res = this;
    res->AddRef();
    int safety = 20000; // no inheritance hierarchy is 20000 deep... we include this just in case there's a bug below and we don't terminate
    while (safety--)
    {
        if (res->m_class == NULL)
        {
            if (FAILED(hr = res->Init(FALSE)))
            {
                res->Release();
                return hr;
            }
        }
        _ASSERTE(res->m_class);
        if (res->m_class == baseClass)
        {
            // Found it!
            break;
        }

        // Another way to determine if we're talking about the
        // same class...  Compare tokens and module.
        mdTypeDef tok;
        mdTypeDef targetTok;
        if (FAILED(hr = res->m_class->GetToken(&tok))
            || FAILED(hr = baseClass->GetToken(&targetTok)))
        {
            res->Release();
            return hr;
        }
        if (tok == targetTok && res->m_class->GetModule() == baseClass->GetModule())
        {
            // Found it!
            break;
        }

        // OK, this is not the right class so look up the inheritance chain
        ICorDebugType *nextType = NULL;
        if (FAILED(hr = res->GetBase(&nextType)))
        {
            res->Release();
            return hr;
        }
        res->Release(); // matches the AddRef above and/or the one implicit in GetBase, for all but last time around the loop
        res = (CordbType *) nextType;
        if (!res || res->m_elementType == ELEMENT_TYPE_OBJECT)
        {
            // Did not find it...
            break;
        }
    }

    if (!res || res->m_elementType == ELEMENT_TYPE_OBJECT)
    {
        if (res)
            res->Release();  // matches the one left over from the loop
        IfFailRet(CordbType::MkUnparameterizedType(baseClass->GetAppDomain(), ELEMENT_TYPE_CLASS, baseClass, &res));
        res->AddRef();
    }


    *ppRes = res;
    return hr;
}


// Walk a type tree, writing the number of type args including internal nodes.
void CordbType::CountTypeDataNodes(CordbType *type, unsigned int *count)
{
  (*count)++;
  for (unsigned int i = 0; i < type->m_inst.m_cClassTyPars; i++)
      CountTypeDataNodes(type->m_inst.m_ppInst[i], count);
}

void CordbType::CountTypeDataNodesForInstantiation(unsigned int genericArgsCount, ICorDebugType *genericArgs[], unsigned int *count)
{
  for (unsigned int i = 0; i < genericArgsCount; i++)
    CountTypeDataNodes((CordbType *) (genericArgs[i]), count);
}

// Walk a type tree, writing the type args.
void CordbType::GatherTypeData(CordbType *type, DebuggerIPCE_TypeArgData **curr_tyargData)
{
  type->TypeToTypeArgData(*curr_tyargData);
  (*curr_tyargData)++;
  for (unsigned int i = 0; i < type->m_inst.m_cClassTyPars; i++)
    GatherTypeData(type->m_inst.m_ppInst[i], curr_tyargData);
}

void CordbType::GatherTypeDataForInstantiation(unsigned int genericArgsCount, ICorDebugType *genericArgs[], DebuggerIPCE_TypeArgData **curr_tyargData)
{
    for (unsigned int i = 0; i < genericArgsCount; i++)
      GatherTypeData((CordbType *) (genericArgs[i]), curr_tyargData);
}

/* ------------------------------------------------------------------------- *
 * TypeParameter Enumerator class
 * ------------------------------------------------------------------------- */

// Factory methods
CordbTypeEnum* CordbTypeEnum::Build(CordbAppDomain * pAppDomain, unsigned int cTypars, CordbType **ppTypars)
{
    return BuildImpl( pAppDomain, cTypars, ppTypars );
}

CordbTypeEnum* CordbTypeEnum::Build(CordbAppDomain * pAppDomain, unsigned int cTypars, RSSmartPtr<CordbType> *ppTypars)
{
    return BuildImpl( pAppDomain, cTypars, ppTypars );
}

// We need to support taking both an array of CordbType* and an array of RSSmartPtr<CordbType>, 
// but the code is identical in both cases.  Rather than duplicate any code explicity, it's better to 
// have the compiler do it for us using this template method.
// Another option would be to create an IList<T> interface and implementations for both arrays 
// of T* and arrays of RSSmartPtr<T>.  This would be more generally usefull, but much more code.
template<class T> CordbTypeEnum* CordbTypeEnum::BuildImpl(CordbAppDomain * pAppDomain, unsigned int cTypars, T* ppTypars)
{
    CordbTypeEnum* newEnum = new (nothrow) CordbTypeEnum( pAppDomain );
    if( NULL == newEnum )
    {
        return NULL;
    }

    _ASSERTE( newEnum->m_ppTypars == NULL );
    newEnum->m_ppTypars = new (nothrow) RSSmartPtr<CordbType> [cTypars];
    if( newEnum->m_ppTypars == NULL )
    {
        delete newEnum;
        return NULL;
    }
    
    newEnum->m_iMax = cTypars;
    for (unsigned int i = 0; i < cTypars; i++)
    {
        newEnum->m_ppTypars[i].Assign(ppTypars[i]);
    }

    return newEnum;
}

// Private, called only by Build above
CordbTypeEnum::CordbTypeEnum(CordbAppDomain * pAppDomain) :
    CordbBase(pAppDomain->GetProcess(), 0),
    m_ppTypars(NULL),
    m_iCurrent(0),
    m_iMax(0)
{
    _ASSERTE(pAppDomain != NULL);
    m_pAppDomain =  pAppDomain;
    pAppDomain->AddToTypeList(this);
}

CordbTypeEnum::~CordbTypeEnum()
{
    _ASSERTE(this->IsNeutered());
}

void CordbTypeEnum::Neuter(NeuterTicket ticket)
{
    // We're definitely on the Appdomain's neuter list.
    // But we may also be on other more-restrictive neuter-lists too (eg, the Thread's RefreshStack
    // if we're enumerating a frame's type-params)
    delete [] m_ppTypars;
    m_ppTypars = NULL;
    m_pAppDomain = NULL;

    CordbBase::Neuter(ticket);
}


HRESULT CordbTypeEnum::QueryInterface(REFIID id, void **pInterface)
{
    if (id == IID_ICorDebugEnum)
        *pInterface = static_cast<ICorDebugEnum*>(this);
    else if (id == IID_ICorDebugTypeEnum)
        *pInterface = static_cast<ICorDebugTypeEnum*>(this);
    else if (id == IID_IUnknown)
        *pInterface = static_cast<IUnknown*>(static_cast<ICorDebugTypeEnum*>(this));
    else
    {
        *pInterface = NULL;
        return E_NOINTERFACE;
    }

    ExternalAddRef();
    return S_OK;
}

HRESULT CordbTypeEnum::Skip(ULONG celt)
{
    PUBLIC_API_ENTRY(this);
    FAIL_IF_NEUTERED(this);
    ATT_REQUIRE_STOPPED_MAY_FAIL(this->GetProcess());

    HRESULT hr = E_FAIL;
    if ( (m_iCurrent+celt) < m_iMax ||
         celt == 0)
    {
        m_iCurrent += celt;
        hr = S_OK;
    }

    return hr;
}

HRESULT CordbTypeEnum::Reset(void)
{
    PUBLIC_API_ENTRY(this);
    FAIL_IF_NEUTERED(this);
    ATT_REQUIRE_STOPPED_MAY_FAIL(this->GetProcess());

    m_iCurrent = 0;
    return S_OK;
}

HRESULT CordbTypeEnum::Clone(ICorDebugEnum **ppEnum)
{
    PUBLIC_API_ENTRY(this);
    FAIL_IF_NEUTERED(this);
    ATT_REQUIRE_STOPPED_MAY_FAIL(this->GetProcess());


    VALIDATE_POINTER_TO_OBJECT(ppEnum, ICorDebugEnum **);

    HRESULT hr = S_OK;

    CordbTypeEnum *pCVE = CordbTypeEnum::Build(m_pAppDomain, m_iMax, m_ppTypars );
    if ( pCVE == NULL )
    {
        (*ppEnum) = NULL;
        hr = E_OUTOFMEMORY;
        goto LExit;
    }

    pCVE->AddRef();
    (*ppEnum) = (ICorDebugEnum*)pCVE;

LExit:
    return hr;
}

HRESULT CordbTypeEnum::GetCount(ULONG *pcelt)
{
    PUBLIC_API_ENTRY(this);
    FAIL_IF_NEUTERED(this);
    ATT_REQUIRE_STOPPED_MAY_FAIL(this->GetProcess());

    VALIDATE_POINTER_TO_OBJECT(pcelt, ULONG *);

    if( pcelt == NULL)
        return E_INVALIDARG;

    (*pcelt) = m_iMax;
    return S_OK;
}

//
// In the event of failure, the current pointer will be left at
// one element past the troublesome element.  Thus, if one were
// to repeatedly ask for one element to iterate through the
// array, you would iterate exactly m_iMax times, regardless
// of individual failures.
HRESULT CordbTypeEnum::Next(ULONG celt, ICorDebugType *values[], ULONG *pceltFetched)
{
    PUBLIC_API_ENTRY(this);
    FAIL_IF_NEUTERED(this);
    ATT_REQUIRE_STOPPED_MAY_FAIL(this->GetProcess());


    VALIDATE_POINTER_TO_OBJECT_ARRAY(values, ICorDebugClass *,
        celt, true, true);
    VALIDATE_POINTER_TO_OBJECT_OR_NULL(pceltFetched, ULONG *);

    if ((pceltFetched == NULL) && (celt != 1))
    {
        return E_INVALIDARG;
    }

    if (celt == 0)
    {
        if (pceltFetched != NULL)
        {
            *pceltFetched = 0;
        }
        return S_OK;
    }

    HRESULT hr = S_OK;

    int iMax = min( m_iMax, m_iCurrent+celt);
    int i;

    for (i = m_iCurrent; i < iMax; i++)
    {
         //printf("CordbTypeEnum::Next, returning = 0x%08x.\n", m_ppTypars[i]);
        values[i-m_iCurrent] = m_ppTypars[i];
        values[i-m_iCurrent]->AddRef();
    }

    int count = (i - m_iCurrent);

    if ( FAILED( hr ) )
    {   //we failed: +1 pushes us past troublesome element
        m_iCurrent += 1 + count;
    }
    else
    {
        m_iCurrent += count;
    }

    if (pceltFetched != NULL)
    {
        *pceltFetched = count;
    }

    //
    // If we reached the end of the enumeration, but not the end
    // of the number of requested items, we return S_FALSE.
    //
    if (((ULONG)count) < celt)
    {
        return S_FALSE;
    }

    return hr;
}

