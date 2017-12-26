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
//
// siginfo.cpp
//
// Signature parsing code
//
#include "common.h"

#include "siginfo.hpp"
#include "clsload.hpp"
#include "vars.hpp"
#include "excep.h"
#include "gc.h"
#include "field.h"
#include "comvariant.h"    // for Element type to class lookup table.
#include "eeconfig.h"
#include "runtimehandles.h" // for SignatureNative
#include "security.h" // for CanSkipVerification
#include "winwrap.h"

TypeHandle ElementTypeToTypeHandle(const CorElementType type)
{
    CONTRACTL
    {
        THROWS;
        GC_TRIGGERS;
        INJECT_FAULT(COMPlusThrowOM());
        MODE_ANY;
    }
    CONTRACTL_END

    return( TypeHandle((&g_Mscorlib)->FetchElementType(type)) );
}

/*******************************************************************/
/* static */
CorInfoGCType CorTypeInfo::GetGCType(CorElementType type)
{
    LEAF_CONTRACT;
    
    CONSISTENCY_CHECK(type != ELEMENT_TYPE_MODULE);
    CONSISTENCY_CHECK(type < infoSize);
    BAD_FORMAT_NOTHROW_ASSERT(type != ELEMENT_TYPE_GENERICINST);
    return info[type].gcType;
}

/* static */
unsigned CorTypeInfo::Size(CorElementType type)
{
    LEAF_CONTRACT;
    
    CONSISTENCY_CHECK(type != ELEMENT_TYPE_MODULE);
    CONSISTENCY_CHECK(type < infoSize);
    BAD_FORMAT_NOTHROW_ASSERT(type != ELEMENT_TYPE_GENERICINST);
    return info[type].size;
}


CorTypeInfo::CorTypeInfoEntry CorTypeInfo::info[] = {
#define TYPEINFO(enumName,nameSpace,className,size,gcType,isArray,isPrim,isFloat,isModifier) \
    { enumName, nameSpace, className, size, gcType, isArray, isPrim, isFloat, isModifier },
#include "cortypeinfo.h"
#   undef TYPEINFO
};

const int CorTypeInfo::infoSize =
    sizeof(CorTypeInfo::info) / sizeof(CorTypeInfo::info[0]);

/*******************************************************************/
/* static */
CorElementType CorTypeInfo::FindPrimitiveType(LPCUTF8 nameSpace, LPCUTF8 name)
{
    LEAF_CONTRACT;

    _ASSERTE(nameSpace && name);

    if (strcmp(nameSpace, g_SystemNS))
        return(ELEMENT_TYPE_END);

    for (int i =1; i < CorTypeInfo::infoSize; i++) {    // can skip ELEMENT_TYPE_END
      //        _ASSERTE(info[i].className == 0 || strncmp(info[i].className, "System.", 7) == 0);
        if (info[i].className != 0 && strcmp(name, info[i].className) == 0)
            return(info[i].type);
    }

    return(ELEMENT_TYPE_END);
}

#ifndef DACCESS_COMPILE

CrstStatic HardCodedMetaSig::m_Crst;

/*static*/
void HardCodedMetaSig::Init()
{
    WRAPPER_CONTRACT
    m_Crst.Init("HardCodedMetaSig", CrstSigConvert, CRST_UNSAFE_ANYMODE);
}

#endif // #ifndef DACCESS_COMPILE

const ElementTypeInfo gElementTypeInfo[] = {

#ifdef _DEBUG
#define DEFINEELEMENTTYPEINFO(etname, cbsize, gcness, isfp, inreg, base) {(int)(etname),cbsize,gcness,isfp,inreg,base},
#else
#define DEFINEELEMENTTYPEINFO(etname, cbsize, gcness, isfp, inreg, base) {cbsize,gcness,isfp,inreg,base},
#endif


// Meaning of columns:
//
//     name     - The checked build uses this to verify that the table is sorted
//                correctly. This is a lookup table that uses ELEMENT_TYPE_*
//                as an array index.
//
//     cbsize   - The byte size of this value as returned by SizeOf(). SPECIAL VALUE: -1
//                requires type-specific treatment.
//
//     gc       - 0    no embedded objectrefs
//                1    value is an objectref
//                2    value is an interior pointer - promote it but don't scan it
//                3    requires type-specific treatment
//
//
//     fp       - boolean: does this require special fpu treatment on return?
//
//     reg      - put in a register?
//
//                    name                         cbsize               gc      fp reg Base
DEFINEELEMENTTYPEINFO(ELEMENT_TYPE_END,            -1,             TYPE_GC_NONE, 0, 0,  0)
DEFINEELEMENTTYPEINFO(ELEMENT_TYPE_VOID,           0,              TYPE_GC_NONE, 0, 0,  0)
DEFINEELEMENTTYPEINFO(ELEMENT_TYPE_BOOLEAN,        1,              TYPE_GC_NONE, 0, 1,  1)
DEFINEELEMENTTYPEINFO(ELEMENT_TYPE_CHAR,           2,              TYPE_GC_NONE, 0, 1,  1)

DEFINEELEMENTTYPEINFO(ELEMENT_TYPE_I1,             1,              TYPE_GC_NONE, 0, 1,  1)
DEFINEELEMENTTYPEINFO(ELEMENT_TYPE_U1,             1,              TYPE_GC_NONE, 0, 1,  1)
DEFINEELEMENTTYPEINFO(ELEMENT_TYPE_I2,             2,              TYPE_GC_NONE, 0, 1,  1)
DEFINEELEMENTTYPEINFO(ELEMENT_TYPE_U2,             2,              TYPE_GC_NONE, 0, 1,  1)

DEFINEELEMENTTYPEINFO(ELEMENT_TYPE_I4,             4,              TYPE_GC_NONE, 0, 1,  1)
DEFINEELEMENTTYPEINFO(ELEMENT_TYPE_U4,             4,              TYPE_GC_NONE, 0, 1,  1)
DEFINEELEMENTTYPEINFO(ELEMENT_TYPE_I8,             8,              TYPE_GC_NONE, 0, 0,  1)
DEFINEELEMENTTYPEINFO(ELEMENT_TYPE_U8,             8,              TYPE_GC_NONE, 0, 0,  1)

DEFINEELEMENTTYPEINFO(ELEMENT_TYPE_R4,             4,              TYPE_GC_NONE, 1, 0,  1)
DEFINEELEMENTTYPEINFO(ELEMENT_TYPE_R8,             8,              TYPE_GC_NONE, 1, 0,  1)
DEFINEELEMENTTYPEINFO(ELEMENT_TYPE_STRING,         sizeof(LPVOID), TYPE_GC_REF,  0, 1,  0)
DEFINEELEMENTTYPEINFO(ELEMENT_TYPE_PTR,            sizeof(LPVOID), TYPE_GC_NONE, 0, 1,  0)

DEFINEELEMENTTYPEINFO(ELEMENT_TYPE_BYREF,          sizeof(LPVOID), TYPE_GC_BYREF, 0, 1, 0)
DEFINEELEMENTTYPEINFO(ELEMENT_TYPE_VALUETYPE,      -1,             TYPE_GC_OTHER, 0, 0,  0)
DEFINEELEMENTTYPEINFO(ELEMENT_TYPE_CLASS,          sizeof(LPVOID), TYPE_GC_REF,   0, 1,  0)
DEFINEELEMENTTYPEINFO(ELEMENT_TYPE_VAR,            -1,             TYPE_GC_OTHER, 0, 1,  0)

DEFINEELEMENTTYPEINFO(ELEMENT_TYPE_ARRAY,          sizeof(LPVOID), TYPE_GC_REF,  0, 1,  0)

DEFINEELEMENTTYPEINFO(ELEMENT_TYPE_GENERICINST,           -1,             TYPE_GC_OTHER, 0, 0,  0)

DEFINEELEMENTTYPEINFO(ELEMENT_TYPE_TYPEDBYREF,         sizeof(LPVOID)*2,TYPE_GC_BYREF, 0, 0,0)
DEFINEELEMENTTYPEINFO(ELEMENT_TYPE_VALUEARRAY,     -1,             TYPE_GC_OTHER, 0, 0, 0)
DEFINEELEMENTTYPEINFO(ELEMENT_TYPE_I,              sizeof(LPVOID), TYPE_GC_NONE, 0, 1,  1)
DEFINEELEMENTTYPEINFO(ELEMENT_TYPE_U,              sizeof(LPVOID), TYPE_GC_NONE, 0, 1,  1)
DEFINEELEMENTTYPEINFO(ELEMENT_TYPE_R,              8,              TYPE_GC_NONE, 1, 0,  1)

DEFINEELEMENTTYPEINFO(ELEMENT_TYPE_FNPTR,          sizeof(LPVOID), TYPE_GC_NONE, 0, 1,  0)
DEFINEELEMENTTYPEINFO(ELEMENT_TYPE_OBJECT,         sizeof(LPVOID), TYPE_GC_REF, 0, 1,  0)
DEFINEELEMENTTYPEINFO(ELEMENT_TYPE_SZARRAY,        sizeof(LPVOID), TYPE_GC_REF,  0, 1,  0)

DEFINEELEMENTTYPEINFO(ELEMENT_TYPE_MVAR,            -1,            TYPE_GC_OTHER, 0, 1,  0)

DEFINEELEMENTTYPEINFO(ELEMENT_TYPE_CMOD_REQD,      -1,             TYPE_GC_NONE,  0, 1,  0)

DEFINEELEMENTTYPEINFO(ELEMENT_TYPE_CMOD_OPT,       -1,             TYPE_GC_NONE,  0, 1,  0)

DEFINEELEMENTTYPEINFO(ELEMENT_TYPE_INTERNAL,       -1,             TYPE_GC_NONE,  0, 0,  0)       
};

unsigned GetSizeForCorElementType(CorElementType etyp)
{
        LEAF_CONTRACT;
        _ASSERTE(gElementTypeInfo[etyp].m_elementType == etyp);
        return gElementTypeInfo[etyp].m_cbSize;
}

const ElementTypeInfo* GetElementTypeInfo(CorElementType etyp)
{
        LEAF_CONTRACT;
        _ASSERTE(gElementTypeInfo[etyp].m_elementType == etyp);
        return &gElementTypeInfo[etyp];
}

BOOL    IsFP(CorElementType etyp)
{
        LEAF_CONTRACT;
        _ASSERTE(gElementTypeInfo[etyp].m_elementType == etyp);
        return gElementTypeInfo[etyp].m_fp;
}

BOOL    IsBaseElementType(CorElementType etyp)
{
        LEAF_CONTRACT;
        _ASSERTE(gElementTypeInfo[etyp].m_elementType == etyp);
        return gElementTypeInfo[etyp].m_isBaseType;

}


size_t SigPointer::ConvertToInternalExactlyOne(Module* pSigModule, BYTE* pbBuffer)
{
    CONTRACTL
    {
        INSTANCE_CHECK;
        THROWS;
        MODE_ANY;
        GC_TRIGGERS;

        PRECONDITION(CheckPointer(pSigModule, NULL_OK));
        PRECONDITION(CheckPointer(pbBuffer, NULL_OK));
    }
    CONTRACTL_END

    size_t cbType = 0;
    size_t cbTemp;

    CorElementType typ;

    PCCOR_SIGNATURE pSigStart = m_ptr;

    IfFailThrowBF(GetElemType(&typ), BFA_BAD_COMPLUS_SIG, pSigModule);

    if (pbBuffer)
    {
        (*pbBuffer) = typ;
        pbBuffer++;
    }
    cbType++;


    if (!CorIsPrimitiveType(typ))
    {
        MethodTable* pMT;
        BYTE*   pbDataStart = 0;
        size_t  cbData = 0;
        switch (typ)
        {
            default:
                THROW_BAD_FORMAT(BFA_BAD_COMPLUS_SIG, pSigModule);
                break;
            case ELEMENT_TYPE_VAR:
            case ELEMENT_TYPE_MVAR:
                pbDataStart = (BYTE*)m_ptr;
                // Skip variable number
                IfFailThrowBF(GetData(NULL), BFA_BAD_COMPLUS_SIG, pSigModule);
                cbData = ((BYTE*)m_ptr) - pbDataStart;
                break;
            case ELEMENT_TYPE_OBJECT:
            case ELEMENT_TYPE_STRING:
            case ELEMENT_TYPE_TYPEDBYREF:
                break;

            case ELEMENT_TYPE_BYREF: //fallthru
            case ELEMENT_TYPE_PTR:
            case ELEMENT_TYPE_PINNED:
            case ELEMENT_TYPE_SZARRAY:
                cbTemp = ConvertToInternalExactlyOne(pSigModule, pbBuffer);
                cbType += cbTemp;
                if (pbBuffer)
                {
                    pbBuffer += cbTemp;
                }
                break;

            case ELEMENT_TYPE_VALUETYPE: //fallthru
            case ELEMENT_TYPE_CLASS:
                {
                    IfFailThrowBF(GetToken(NULL), BFA_BAD_COMPLUS_SIG, pSigModule);

                    if (pbBuffer)
                    {
                        pbBuffer[-1] = ELEMENT_TYPE_INTERNAL;
                        SigTypeContext typeContext; // An empty SigTypeContext is OK - loads the generic type
                        SigPointer ptr(pSigStart);
                        pMT = ptr.GetTypeHandleThrowing(pSigModule, &typeContext).GetMethodTable();
                    }
                    cbData = sizeof(MethodTable*);
                    pbDataStart = (BYTE*)&pMT;
                }
                break;

            case ELEMENT_TYPE_MODULE:
                pbDataStart = (BYTE*)m_ptr;
                // Skip index
                IfFailThrowBF(GetData(NULL), BFA_BAD_COMPLUS_SIG, pSigModule);

                cbData = ((BYTE*)m_ptr) - pbDataStart;
                cbType += cbData;
                if (pbBuffer)
                {
                    memcpy(pbBuffer, pbDataStart, cbData);
                    pbBuffer += cbData;
                }
                cbTemp = ConvertToInternalExactlyOne(pSigModule, pbBuffer);
                cbType += cbTemp;
                if (pbBuffer)
                {
                    pbBuffer += cbTemp;
                }
                cbData = 0;
                break;

#ifdef VALUE_ARRAYS
            case ELEMENT_TYPE_VALUEARRAY: 
                cbTemp = ConvertToInternalExactlyOne(pSigModule, pbBuffer);
                cbType += cbTemp;
                pbBuffer += cbTemp;
                pbDataStart = (BYTE*)m_ptr;
                // Skip array size
                IfFailThrowBF(GetData(NULL), BFA_BAD_COMPLUS_SIG, pSigModule);
                cbData = ((BYTE*)m_ptr) - pbDataStart;
                break;
#endif

            case ELEMENT_TYPE_FNPTR:
                cbTemp = ConvertToInternalSignature(pSigModule, pbBuffer);
                cbType += cbTemp;
                if (pbBuffer)
                {
                    pbBuffer += cbTemp;
                }
                break;

            case ELEMENT_TYPE_ARRAY:
                {
                    cbTemp = ConvertToInternalExactlyOne(pSigModule, pbBuffer);
                    cbType += cbTemp;
                    if (pbBuffer)
                    {
                        pbBuffer += cbTemp;
                    }
                    pbDataStart = (BYTE*)m_ptr;
                    ULONG rank; // Get rank
                    IfFailThrowBF(GetData(&rank), BFA_BAD_COMPLUS_SIG, pSigModule);    
                    if (rank)
                    {
                        ULONG nsizes; // Get # of sizes
                        IfFailThrowBF(GetData(&nsizes), BFA_BAD_COMPLUS_SIG, pSigModule);

                        while (nsizes--)
                        {
                            IfFailThrowBF(GetData(NULL), BFA_BAD_COMPLUS_SIG, pSigModule);
                        }

                        ULONG nlbounds; // Get # of lower bounds
                        IfFailThrowBF(GetData(&nlbounds), BFA_BAD_COMPLUS_SIG, pSigModule);

                        while (nlbounds--)
                        {
                            // Skip lower bounds
                            IfFailThrowBF(GetData(NULL), BFA_BAD_COMPLUS_SIG, pSigModule);

                        }
                    }
                    cbData = ((BYTE*)m_ptr) - pbDataStart;
                }
                break;

            case ELEMENT_TYPE_SENTINEL:
                // Should be unreachable since GetElem strips it
                break;

            case ELEMENT_TYPE_INTERNAL:
              pbDataStart = (BYTE*)m_ptr;
              m_ptr += sizeof(void*);
              cbData = ((BYTE*)m_ptr) - pbDataStart;
              break;

            case ELEMENT_TYPE_GENERICINST:

                
              IfFailThrowBF(GetElemType(&typ), BFA_BAD_COMPLUS_SIG, pSigModule);
              
              if (pbBuffer)
              {
                  (*pbBuffer) = typ;
                  pbBuffer++;
              }
              cbType++;

              IfFailThrowBF(GetToken(NULL), BFA_BAD_COMPLUS_SIG, pSigModule);

              if (pbBuffer)
              {
                  pbBuffer[-1] = ELEMENT_TYPE_INTERNAL;
                  SigTypeContext typeContext; // An empty SigTypeContext is OK - loads the generic type
                  SigPointer ptr(pSigStart);
                  pMT = ptr.GetTypeHandleThrowing(pSigModule, &typeContext).GetMethodTable();
              }
              cbType += sizeof(MethodTable*);
              if (pbBuffer)
              {
                  memcpy(pbBuffer, (BYTE*)&pMT, sizeof(MethodTable*));
                  pbBuffer += sizeof(MethodTable*);
              }

              pbDataStart = (BYTE*)m_ptr;
              ULONG argCnt; // Get number of parameters
              IfFailThrowBF(GetData(&argCnt), BFA_BAD_COMPLUS_SIG, pSigModule);

              cbData = ((BYTE*)m_ptr) - pbDataStart;
              cbType += cbData;
              if (pbBuffer)
              {
                  memcpy(pbBuffer, pbDataStart, cbData);
                  pbBuffer += cbData;
              }
              
              while (argCnt--)
              {
                  cbTemp = ConvertToInternalExactlyOne(pSigModule, pbBuffer);
                  cbType += cbTemp;
                  if (pbBuffer)
                  {
                      pbBuffer += cbTemp;
                  }
              }

              cbData = 0;
              break;
        }

        if (cbData)
        {
            cbType += cbData;
            if (pbBuffer)
            {
                memcpy(pbBuffer, pbDataStart, cbData);
                pbBuffer += cbData;
            }
        }
    }

    return cbType;
}

size_t SigPointer::ConvertToInternalSignature(Module* pSigModule, BYTE* pbBuffer)
{
    CONTRACTL
    {
        INSTANCE_CHECK;
        THROWS;
        MODE_ANY;
        GC_TRIGGERS;

        PRECONDITION(CheckPointer(pSigModule, NULL_OK));
        PRECONDITION(CheckPointer(pbBuffer, NULL_OK));
    }
    CONTRACTL_END

    size_t  cbNewSignature = 0;
    BYTE*   pbSigStart = (BYTE*)m_ptr;
    ULONG   uCallConv;

    IfFailThrowBF(GetData(&uCallConv), BFA_BAD_COMPLUS_SIG, pSigModule);

    if ((uCallConv & IMAGE_CEE_CS_CALLCONV_MASK) == IMAGE_CEE_CS_CALLCONV_FIELD)
        THROW_BAD_FORMAT(BFA_UNEXPECTED_FIELD_SIGNATURE, pSigModule);

    // Skip type parameter count
    if (uCallConv & IMAGE_CEE_CS_CALLCONV_GENERIC)
    {
        IfFailThrowBF(GetData(NULL), BFA_BAD_COMPLUS_SIG, pSigModule);
    }

    // Get arg count;
    ULONG   cArgs;
    IfFailThrowBF(GetData(&cArgs), BFA_BAD_COMPLUS_SIG, pSigModule);
    cArgs++; // +1 for return type
    size_t  cbChunk = ((BYTE*)m_ptr) - pbSigStart;

    cbNewSignature += cbChunk;

    if (pbBuffer)
    {
        memcpy(pbBuffer, pbSigStart, cbChunk);
        pbBuffer += cbChunk;
    }

    // Skip args.
    while (cArgs) 
    {
        size_t cbType = ConvertToInternalExactlyOne(pSigModule, pbBuffer);
        
        cbNewSignature += cbType;
        if (pbBuffer)
        {
            pbBuffer += cbType;
        }
        cArgs--;
    }

    return cbNewSignature;
}


//------------------------------------------------------------------
// Constructor.
//------------------------------------------------------------------

MetaSig::MetaSig(PCCOR_SIGNATURE szMetaSig, 
                 DWORD cbMetaSig,
                 Module* pModule, 
                 const SigTypeContext *pTypeContext,
                 BOOL fConvertSigAsVarArg, 
                 MetaSigKind kind, 
                 BOOL fParamTypeArg)

{
    CONTRACTL
    {
        CONSTRUCTOR_CHECK;
        NOTHROW;
        MODE_ANY;
        GC_NOTRIGGER;
        FORBID_FAULT;
        PRECONDITION(CheckPointer(szMetaSig));
        PRECONDITION(CheckPointer(pModule));
        PRECONDITION(CheckPointer(pTypeContext, NULL_OK));

    }
    CONTRACTL_END


#ifdef _DEBUG
    FillMemory(this, sizeof(*this), 0xcc);
#endif
    // Copy the type context
    SigTypeContext::InitTypeContext(pTypeContext,&m_typeContext);
    m_pModule = pModule;
    m_pszMetaSig = szMetaSig;
    m_cbSigSize = cbMetaSig;
    SigPointer psig(szMetaSig, cbMetaSig);

    switch(kind)
    {
        case sigLocalVars:
        {
            ULONG data = 0;
            psig.GetCallingConvInfo(&data); // Store calling convention
            m_CallConv = (BYTE)data;
                    
            psig.GetData(&data);  // Store number of arguments.
            m_nArgs = data;

            m_pRetType = NULL;
            break;
        }
        case sigMember:
        {
            ULONG data = 0;
            psig.GetCallingConvInfo(&data); // Store calling convention
            m_CallConv = (BYTE)data;

            // Store type parameter count
            if (m_CallConv & IMAGE_CEE_CS_CALLCONV_GENERIC)
              psig.GetData(NULL);

            psig.GetData(&data);  // Store number of arguments.
            m_nArgs = data;
            m_pRetType  = psig;
            psig.SkipExactlyOne();
            break;
        }
        case sigField:
        {
            ULONG data = 0;
            psig.GetCallingConvInfo(&data); // Store calling convention
            m_CallConv = (BYTE)data;

            m_nArgs = 1; //There's only 1 'arg' - the type.
            m_pRetType = NULL;
            break;
        }
    }


    m_pStart = psig;

    m_flags = 0;
    
    // used to treat some sigs as special case vararg
    // used by calli to unmanaged target
    if (fConvertSigAsVarArg)
    {
        m_flags |= TREAT_AS_VARARG;
    }

    // Intialize the actual sizes
    m_nActualStack = (UINT32) -1;
    m_nVirtualStack = (UINT32) -1;
    m_nNumVirtualFixedArgs = (UINT32) -1;
    m_cbRetType = (UINT32) -1;

    // Reset the iterator fields
    Reset();

    if (fParamTypeArg)
      m_CallConv = m_CallConv | CORINFO_CALLCONV_PARAMTYPE;
}

// Helper constructor that constructs a method signature MetaSig from a MethodDesc
// IMPORTANT: if classInst/methodInst is omitted and the MethodDesc is shared between generic
// instantiations then the instantiation info for the method will be representative.  This
// is OK for GC, field layout etc. but not OK where exact types matter.
//
// Also, if used on a shared instantiated method descriptor or instance method in a shared generic struct
// then the calling convention is fixed up to include the extra dictionary argument
//
// For method descs from array types the "instantiation" is set to the element type of the array
// This lets us use VAR in the signatures for Get, Set and Address
MetaSig::MetaSig(MethodDesc *pMD, TypeHandle *classInst, TypeHandle *methodInst)
{
    WRAPPER_CONTRACT;

    SigTypeContext typeContext(pMD, classInst, methodInst);

    PCCOR_SIGNATURE pSig;
    DWORD cbSigSize;
    pMD->GetSig(&pSig, &cbSigSize);

    *this = MetaSig(pSig, cbSigSize, pMD->GetModule(),&typeContext,FALSE,sigMember,pMD->RequiresInstArg());
}

MetaSig::MetaSig(MethodDesc *pMD, TypeHandle declaringType)
{
    WRAPPER_CONTRACT;

    SigTypeContext typeContext(pMD, declaringType);
    PCCOR_SIGNATURE pSig;
    DWORD cbSigSize;
    pMD->GetSig(&pSig, &cbSigSize);

    *this = MetaSig(pSig, cbSigSize, pMD->GetModule(),&typeContext,FALSE,sigMember,pMD->RequiresInstArg());
}

#ifndef DACCESS_COMPILE
MetaSig::MetaSig(MethodDesc* pMD, BinderMethodID id)
{
    CONTRACTL
    {
        CONSTRUCTOR_CHECK;
        THROWS;
        MODE_ANY;
        GC_TRIGGERS;
        INJECT_FAULT(COMPlusThrowOM());
    }
    CONTRACTL_END

    _ASSERTE(MethodDescMatchesBinderSig(pMD, id));

    DWORD cbSigSize;

    PCCOR_SIGNATURE pMethodBinarySig;
    
    g_Mscorlib.GetMethodBinarySigAndSize(id, &pMethodBinarySig, &cbSigSize);

    *this = MetaSig(pMethodBinarySig, cbSigSize, SystemDomain::SystemModule(), NULL);
}
#endif

// Helper constructor that constructs a field signature MetaSig from a FieldDesc
// IMPORTANT: the classInst is omitted then the instantiation info for the field
// will be representative only as FieldDescs can be shared
//
MetaSig::MetaSig(FieldDesc *pFD, TypeHandle declaringType)
{
    CONTRACTL
    {
        CONSTRUCTOR_CHECK;
        NOTHROW;
        MODE_ANY;
        GC_NOTRIGGER;
        PRECONDITION(CheckPointer(pFD));
    }
    CONTRACTL_END

    PCCOR_SIGNATURE pSig;
    DWORD           cSig;

    pFD->GetSig(&pSig, &cSig);

    SigTypeContext typeContext(pFD, declaringType);

    *this = MetaSig(pSig, cSig, pFD->GetModule(),&typeContext, FALSE, sigField, FALSE);
}

static BOOL InterlockCompareExchange(volatile LONG* destination, volatile LONG exchange, volatile LONG comperand)
{
    WRAPPER_CONTRACT;
    
    return FastInterlockCompareExchange(destination, exchange, comperand) == comperand;
}

BOOL MetaSigCache::AquireWriterLock()
{
    WRAPPER_CONTRACT;
    
    return InterlockCompareExchange(&m_cacheState, -1, 1);
}

void MetaSigCache::ReleaseWriterLock()
{
    WRAPPER_CONTRACT;
    
    FastInterlockExchange(&m_cacheState, 1);
}

BOOL MetaSigCache::AquireReaderLock()
{
    WRAPPER_CONTRACT;
    
    LONG readers = m_cacheState;

    if (readers == -1)
        return FALSE;
    
    return InterlockCompareExchange(&m_cacheState, readers + 1, readers);
}

void MetaSigCache::ReleaseReaderLock()
{
    WRAPPER_CONTRACT;
    
    FastInterlockDecrement(&m_cacheState);
}

MetaSig* MetaSigCache::GetCachedMetaSig(PCCOR_SIGNATURE pCorSig, Module* pModule)
{
    CONTRACTL
    {
        THROWS;
        MODE_ANY;
        GC_TRIGGERS;
        PRECONDITION(CheckPointer(pCorSig));
        PRECONDITION(CheckPointer(pModule));
    }
    CONTRACTL_END   
    
    if (m_pCorSig == pCorSig && m_pModule == pModule)       
        return (MetaSig*)&m_metaSig;

    return NULL;
}

void MetaSigCache::CacheMetaSig(MetaSig* pMetaSig, PCCOR_SIGNATURE pCorSig, Module* pModule)
{
    CONTRACTL
    {
        THROWS;
        MODE_ANY;
        GC_TRIGGERS;
        PRECONDITION(CheckPointer(pMetaSig));
        PRECONDITION(CheckPointer(pCorSig));
        PRECONDITION(CheckPointer(pModule));
    }
    CONTRACTL_END   
    
    m_pCorSig = pCorSig;
    m_pModule = pModule;                
    new (&m_metaSig) MetaSig(pMetaSig);
}

void MetaSigCache::Initialize(AppDomain* pAD, SIZE_T index, MetaSig* pMetaSig, MethodDesc* pMD, PCCOR_SIGNATURE pCorSig, DWORD cpCorSig, Module* pModule)
{
    CONTRACTL
    {
        THROWS;
        MODE_ANY;
        GC_TRIGGERS;
        PRECONDITION(CheckPointer(pMD));
        PRECONDITION(CheckPointer(pCorSig));
        PRECONDITION(CheckPointer(pModule));
    }
    CONTRACTL_END  
    
    new (pMetaSig) MetaSig(pCorSig, cpCorSig, pModule, NULL);
    pMetaSig->ForceSigWalk(pMD->IsStatic());
    
    if (InterlockCompareExchange(&pAD->m_metaSigCacheState, MetaSigCacheInitializing, MetaSigCacheUninitialized))
    {
        _ASSERTE(!pAD->m_pMetaSigCache);
        pAD->m_pMetaSigCache = new MetaSigCache [META_SIG_CACHE_SIZE];
        
        // Update slot
        pAD->m_pMetaSigCache[index].CacheMetaSig(pMetaSig, pCorSig, pModule);        
    
        FastInterlockExchange(&pAD->m_metaSigCacheState, MetaSigCacheInitialized);
    }
}

void MetaSigCache::LookUp(MetaSig* pMetaSig, MethodDesc* pMD, PCCOR_SIGNATURE pCorSig, DWORD cpCorSig, Module* pModule)
{
    CONTRACTL
    {
        THROWS;
        MODE_ANY;
        GC_TRIGGERS;
        PRECONDITION(CheckPointer(pMD));
        PRECONDITION(CheckPointer(pCorSig));
        PRECONDITION(CheckPointer(pModule));
    }
    CONTRACTL_END

    SIZE_T index = (((SIZE_T)pCorSig >> 2) % META_SIG_CACHE_SIZE);

    AppDomain* pAD = GetAppDomain();    
    _ASSERTE(pAD);

    if (pAD->m_metaSigCacheState != MetaSigCacheInitialized)
    {          
        MetaSigCache::Initialize(pAD, index, pMetaSig, pMD, pCorSig, cpCorSig, pModule);
        return;
    }
    
    MetaSigCache* pMetaSigCache = &pAD->m_pMetaSigCache[index];
    
    if (!pMetaSigCache->AquireReaderLock())
    {
        new (pMetaSig) MetaSig(pCorSig, cpCorSig, pModule, NULL);
    }
    else
    {
        MetaSig* pCachedMetaSig = pMetaSigCache->GetCachedMetaSig(pCorSig, pModule);
        
        if (pCachedMetaSig)
        {
            // Cache hit
            new (pMetaSig) MetaSig(pCachedMetaSig);
            pMetaSigCache->ReleaseReaderLock();
            return;
        }
        
        // Cache miss
        new (pMetaSig) MetaSig(pCorSig, cpCorSig, pModule, NULL);
        pMetaSig->ForceSigWalk(pMD->IsStatic());
    
        if (pMetaSigCache->AquireWriterLock())
        {
            // Update slot
            pMetaSigCache->CacheMetaSig(pMetaSig, pCorSig, pModule);        
            pMetaSigCache->ReleaseWriterLock();
        }
        
        pMetaSigCache->ReleaseReaderLock();
    }
}

//------------------------------------------------------------------
// Constructor. Copy state from existing SignatureNative (used by reflection)
//------------------------------------------------------------------
MetaSig::MetaSig(MethodDesc* pMD, SignatureNative* pSigNative, BOOL bCanCacheTargetAndCrackedSig)
{
    CONTRACTL
    {
        CONSTRUCTOR_CHECK;
        THROWS;
        MODE_ANY;
        GC_TRIGGERS;
        PRECONDITION(CheckPointer(pMD));
        PRECONDITION(CheckPointer(pSigNative));
    }
    CONTRACTL_END

    if (!bCanCacheTargetAndCrackedSig)
    {
        SigTypeContext typeContext;
        new (this) MetaSig(
            pSigNative->GetCorSig(), 
            pSigNative->GetCorSigSize(),
            pSigNative->GetModule(), 
            pSigNative->GetTypeContext(&typeContext));
    }
    else
    {
        _ASSERTE(!pSigNative->GetTypeContext(&m_typeContext));
        MetaSigCache::LookUp(this, pMD, pSigNative->GetCorSig(), pSigNative->GetCorSigSize(), pSigNative->GetModule());
    }
}

//------------------------------------------------------------------
// Returns type of current argument index. Returns ELEMENT_TYPE_END
// if already past end of arguments.
//------------------------------------------------------------------
CorElementType MetaSig::PeekArg() const
{
    WRAPPER_CONTRACT;

    if (m_iCurArg == m_nArgs)
        return(ELEMENT_TYPE_END);
    else
        return(m_pWalk.PeekElemTypeClosed(&m_typeContext));
}

//------------------------------------------------------------------
// Returns type of current argument, then advances the argument
// index. Returns ELEMENT_TYPE_END if already past end of arguments.
//------------------------------------------------------------------
CorElementType MetaSig::NextArg()
{
    CONTRACTL
    {
        INSTANCE_CHECK;
        NOTHROW;
        MODE_ANY;
        GC_NOTRIGGER;
        FORBID_FAULT;
    }
    CONTRACTL_END

    m_pLastType = m_pWalk;

    if (m_iCurArg == m_nArgs)
        return(ELEMENT_TYPE_END);
    else
    {
        m_iCurArg++;
        CorElementType mt = m_pWalk.PeekElemTypeClosed(&m_typeContext);
        m_pWalk.SkipExactlyOne();
        return(mt);
    }
}

//------------------------------------------------------------------
// Retreats argument index, then returns type of the argument
// under the new index. Returns ELEMENT_TYPE_END if already at first
// argument.
//------------------------------------------------------------------
CorElementType MetaSig::PrevArg()
{
    CONTRACTL
    {
        INSTANCE_CHECK;
        NOTHROW;
        MODE_ANY;
        GC_NOTRIGGER;
        FORBID_FAULT;
    }
    CONTRACTL_END

    if (m_iCurArg == 0)
        return(ELEMENT_TYPE_END);
    else
    {
        m_iCurArg--;
        m_pWalk = m_pStart;
        for (UINT32 i = 0; i < m_iCurArg; i++) 
            m_pWalk.SkipExactlyOne();

        m_pLastType = m_pWalk;
        return(m_pWalk.PeekElemTypeClosed(&m_typeContext));
    }
}


//------------------------------------------------------------------
// reset: goto start pos
//------------------------------------------------------------------
VOID MetaSig::Reset()
{
    LEAF_CONTRACT;

    m_pWalk = m_pStart;
    m_iCurArg  = 0;
    return;
}

//------------------------------------------------------------------
// Moves index to end of argument list.
//------------------------------------------------------------------
VOID MetaSig::GotoEnd()
{
    CONTRACTL
    {
        INSTANCE_CHECK;
        NOTHROW;
        GC_NOTRIGGER;
        FORBID_FAULT;
        MODE_ANY;
    }
    CONTRACTL_END

    UINT32 i;

    m_pWalk = m_pStart;
    for (i = 0; i < NumFixedArgs(); i++)
    {
        m_pWalk.SkipExactlyOne();
    }
    m_iCurArg = i;
}

#ifndef DACCESS_COMPILE

//------------------------------------------------------------------------

/*******************************************************************/
BOOL IsTypeRefOrDef(LPCSTR szClassName, Module *pModule, mdToken token)
{
    CONTRACTL
    {
        NOTHROW;
        GC_NOTRIGGER;
        FORBID_FAULT;
        MODE_ANY;
    }
    CONTRACTL_END

    LPCUTF8  pclsname;
    LPCUTF8 pszNamespace;

    IMDInternalImport *pInternalImport = pModule->GetMDImport();

    if (TypeFromToken(token) == mdtTypeDef)
        pInternalImport->GetNameOfTypeDef(token, &pclsname, &pszNamespace);
    else if (TypeFromToken(token) == mdtTypeRef)
        pInternalImport->GetNameOfTypeRef(token, &pszNamespace, &pclsname);
    else
        return(false);

    // If the namespace is not the same.
    int iLen = (int)strlen(pszNamespace);
    if (iLen)
    {
        if (strncmp(szClassName, pszNamespace, iLen) != 0)
            return(false);

        if (szClassName[iLen] != NAMESPACE_SEPARATOR_CHAR)
            return(false);
        ++iLen;
    }

    if (strcmp(&szClassName[iLen], pclsname) != 0)
        return(false);
    return(true);
}

#endif // #ifndef DACCESS_COMPILE

TypeHandle SigPointer::GetTypeHandle(Module* pModule,
                                     const SigTypeContext *pTypeContext,
                                     OBJECTREF *pThrowable,
                                     ClassLoader::LoadTypesFlag fLoadTypes/*=LoadTypes*/,
                                     ClassLoadLevel level,
                                     BOOL dropGenericArgumentLevel,
                                     const Substitution *pSubst,
                                     // pCurrentModule is only set when decoding zapsigs
                                     Module *pCurrentModule) const
{
    CONTRACTL
    {
        INSTANCE_CHECK;
        NOTHROW;
        MODE_ANY;
        if (FORBIDGC_LOADER_USE_ENABLED()) GC_NOTRIGGER; else GC_TRIGGERS;
        if (FORBIDGC_LOADER_USE_ENABLED()) FORBID_FAULT; else { INJECT_FAULT(return TypeHandle();); }
    }
    CONTRACTL_END

    SCAN_IGNORE_TRIGGER;    // this function may either trigger or not (see disabled contract above)
    SCAN_IGNORE_THROW;      // this function doesn't throw if pThrowable is NULL, it may throw otherwise
    SCAN_IGNORE_FAULT;

#ifndef DACCESS_COMPILE

    TypeHandle th;
    EX_TRY
    {
        th = GetTypeHandleThrowing(pModule, pTypeContext, fLoadTypes, level, dropGenericArgumentLevel,
                                   pSubst, pCurrentModule);
    }
    EX_CATCH
    {
        if (pThrowable != NULL && (*pThrowable) == NULL)
        {
            GCX_COOP();
            *pThrowable = GETTHROWABLE();
        }
    }
    EX_END_CATCH(SwallowAllExceptions)
    return(th);
#else
    DacNotImpl();
    return TypeHandle();
#endif // #ifndef DACCESS_COMPILE
}

TypeHandle SigPointer::GetTypeHandleThrowing(Module* pModule,
                                             const SigTypeContext *pTypeContext,
                                             ClassLoader::LoadTypesFlag fLoadTypes/*=LoadTypes*/,
                                             ClassLoadLevel level,
                                             BOOL dropGenericArgumentLevel,
                                             const Substitution *pSubst,
                                             // pCurrentModule is only set when decoding zapsigs
                                             Module *pCurrentModule) const
{
    CONTRACT(TypeHandle)
    {
        INSTANCE_CHECK;
        if (FORBIDGC_LOADER_USE_ENABLED()) NOTHROW; else THROWS;
        MODE_ANY;
        if (FORBIDGC_LOADER_USE_ENABLED()) GC_NOTRIGGER; else GC_TRIGGERS;
        if (FORBIDGC_LOADER_USE_ENABLED()) FORBID_FAULT; else { INJECT_FAULT(COMPlusThrowOM()); }
        POSTCONDITION(CheckPointer(RETVAL, ((fLoadTypes == ClassLoader::LoadTypes) ? NULL_NOT_OK : NULL_OK)));
    }
    CONTRACT_END

    TypeHandle thRet;

    // This function is recursive, so it must have an interior probe
    DECLARE_INTERIOR_STACK_PROBE;
    if (ShouldProbeOnThisThread())
    {
        DO_INTERIOR_STACK_PROBE_FOR_NOTHROW(GetThread(),10, NO_FORBIDGC_LOADER_USE_ThrowSO(););
    }

    {

    #ifdef _DEBUG_IMPL
    // This verifies that we won't try and load a type
    // if FORBIDGC_LOADER_USE_ENABLED is true.
    //
    // The FORBIDGC_LOADER_USE is limited to very specific scenarios that need to retrieve
    // GC_OTHER typehandles for size and gcroot information. This assert attempts to prevent
    // this abuse from proliferating.
    //
    if (FORBIDGC_LOADER_USE_ENABLED() && (fLoadTypes == ClassLoader::LoadTypes))
    {
        TypeHandle th = GetTypeHandleThrowing(pModule,
                                              pTypeContext,
                                              // == FailIfNotLoaded: we expect no failures
                                              ClassLoader::DontLoadTypes,
                                              level,
                                              dropGenericArgumentLevel,
                                              pSubst,
                                              pCurrentModule);
        _ASSERTE(!th.IsNull());
    }
#endif

    SigPointer psig = *this;
    CorElementType typ;
    IfFailThrowBF(psig.GetElemType(&typ), BFA_BAD_SIGNATURE, pModule);


    switch(typ) {
        case ELEMENT_TYPE_STRING:
        case ELEMENT_TYPE_U:
        case ELEMENT_TYPE_I:
        case ELEMENT_TYPE_I1:
        case ELEMENT_TYPE_U1:
        case ELEMENT_TYPE_BOOLEAN:
        case ELEMENT_TYPE_I2:
        case ELEMENT_TYPE_U2:
        case ELEMENT_TYPE_CHAR:
        case ELEMENT_TYPE_I4:
        case ELEMENT_TYPE_U4:
        case ELEMENT_TYPE_I8:
        case ELEMENT_TYPE_U8:
        case ELEMENT_TYPE_R4:
        case ELEMENT_TYPE_R8:
        case ELEMENT_TYPE_VOID:
        case ELEMENT_TYPE_OBJECT:
        case ELEMENT_TYPE_TYPEDBYREF:

            // fLoadTypes == FailIfNotLoaded or fLoadTypes == FailIfNotLoadedOrNotRestored
            if ((fLoadTypes == ClassLoader::DontLoadTypes))
            {
                thRet = (TypeHandle((&g_Mscorlib)->LookupElementType(typ, level)));
            }
            else
            {
                thRet = (TypeHandle((&g_Mscorlib)->FetchElementType(typ, (fLoadTypes == ClassLoader::LoadTypes), level)));
            }
            goto Exit;

        case ELEMENT_TYPE_VAR:
            if (pSubst && pSubst->GetInstSig())
            {
#ifdef _DEBUG_IMPL
                _ASSERTE(!FORBIDGC_LOADER_USE_ENABLED());
#endif
                DWORD index;

                IfFailThrow(psig.GetData(&index));

                SigPointer inst = SigPointer(pSubst->GetInstSig());
                for (DWORD i = 0; i < index; i++) IfFailThrowBF(inst.SkipExactlyOne(), BFA_BAD_SIGNATURE, pModule);

                thRet =  inst.GetTypeHandleThrowing(pSubst->GetModule(), 
                                                    pTypeContext,
                                                    fLoadTypes, 
                                                    level,
                                                    dropGenericArgumentLevel,
                                                    pSubst->GetNext(), 
                                                    pCurrentModule);
                goto Exit;                                                    
            }
            thRet = (psig.GetTypeVariableThrowing(pModule, typ, fLoadTypes, pTypeContext));
            goto Exit;

        case ELEMENT_TYPE_MVAR:
            thRet = (psig.GetTypeVariableThrowing(pModule, typ, fLoadTypes, pTypeContext));
            goto Exit;

        case ELEMENT_TYPE_GENERICINST:
        {

            IfFailThrowBF(psig.GetElemType(&typ), BFA_BAD_SIGNATURE, pModule);
            
            mdTypeDef typeDef = mdTypeDefNil;
            Module* pDefModule = NULL;

            if (typ == ELEMENT_TYPE_INTERNAL) 
            {
                TypeHandle genericType;
                ULONG skipBytes = CorSigUncompressPointer(psig.GetPtr(), (void**)&genericType);
                psig = psig.GetPtr() + skipBytes;
                if (genericType.IsNull()) 
                {
                    thRet = (genericType);
                    goto Exit;
                }
                
                pDefModule = genericType.GetModule();
                typeDef = genericType.GetCl();
            }
            else 
            {
                mdTypeRef typeref;
                IfFailThrowBF(psig.GetToken(&typeref), BFA_BAD_SIGNATURE, pModule);

                if (TypeFromToken(typeref) != mdtTypeRef && TypeFromToken(typeref) != mdtTypeDef)
                    THROW_BAD_FORMAT(BFA_UNEXPECTED_TOKEN_AFTER_GENINST, pModule);
               
                if (typeref == mdTypeDefNil || typeref == mdTypeRefNil)
                    THROW_BAD_FORMAT(BFA_UNEXPECTED_TOKEN_AFTER_GENINST, pModule);
            
                if (g_pConfig->DontLoadOpenTypes())
                {
                    if (!ClassLoader::ResolveTokenToTypeDefThrowing(pModule, typeref, &pDefModule, &typeDef))
                    {
                        TypeHandle retVal = TypeHandle();
                        thRet = (retVal);
                        goto Exit;
                    }
                }
                else
                {
                    TypeHandle genericType = 
                        ClassLoader::LoadTypeDefOrRefThrowing(pModule, typeref, 
                                                              ((fLoadTypes == ClassLoader::LoadTypes) ? ClassLoader::ThrowIfNotFound : ClassLoader::ReturnNullIfNotFound), 
                                                              ClassLoader::PermitUninstDefOrRef,
                                                              ((fLoadTypes == ClassLoader::LoadTypes) ? tdNoTypes : tdAllTypes), 
                                                              level);

                    if (genericType.IsNull()) 
                    {
                        thRet = (genericType);
                        goto Exit;
                    }

                    pDefModule = genericType.GetModule();
                    typeDef = genericType.GetCl();
                    if ((fLoadTypes == ClassLoader::LoadTypes))
                    {
                        bool typFromSigIsClass = (typ == ELEMENT_TYPE_CLASS);
                        bool typLoadedIsClass  = (genericType.GetSignatureCorElementType() == ELEMENT_TYPE_CLASS);
                    
                        if (typFromSigIsClass != typLoadedIsClass)
#ifndef DACCESS_COMPILE
                        {
                            pModule->GetAssembly()->ThrowTypeLoadException(pModule->GetMDImport(),
                                                                           typeref, 
                                                                           BFA_CLASSLOAD_VALUETYPEMISMATCH);
                        }
#else
                        DacNotImpl();
#endif // #ifndef DACCESS_COMPILE
                    }

                    if (genericType.IsInterface() && level == CLASS_LOAD_APPROXPARENTS && dropGenericArgumentLevel)
                    {
                        thRet = genericType;
                        goto Exit;
                    }
                }

            }

            // The number of type parameters follows
            {
            DWORD ntypars;
            IfFailThrowBF(psig.GetData(&ntypars), BFA_BAD_SIGNATURE, pModule);

            DWORD dwAllocaSize = 0;
            if (!ClrSafeInt<DWORD>::multiply(ntypars, sizeof(TypeHandle), dwAllocaSize))
                ThrowHR(COR_E_OVERFLOW);

            if (ShouldProbeOnThisThread() && (dwAllocaSize/PAGE_SIZE+1) >= 2)
            {
                DO_INTERIOR_STACK_PROBE_FOR_NOTHROW(GetThread(), (10+dwAllocaSize/PAGE_SIZE+1), NO_FORBIDGC_LOADER_USE_ThrowSO(););
            }

            TypeHandle *thisinst = (TypeHandle*) _alloca(dwAllocaSize);

            // Finally we gather up the type arguments themselves, loading at the level specified for generic arguments
            for (unsigned i = 0; i < ntypars; i++)
            {
                ClassLoadLevel argLevel = level;
                TypeHandle typeHnd = TypeHandle();
                BOOL argDrop = FALSE;

                if (dropGenericArgumentLevel)
                {
                    if (level == CLASS_LOAD_APPROXPARENTS)
                    {
                        SigPointer tempsig = psig;

                        CorElementType elemType;
                        IfFailThrowBF(tempsig.GetElemType(&elemType), BFA_BAD_SIGNATURE, pModule);

                        if (elemType == ELEMENT_TYPE_MODULE)
                        {
                            IfFailThrowBF(tempsig.GetData(NULL), BFA_BAD_SIGNATURE, pModule);
                            IfFailThrowBF(tempsig.GetElemType(&elemType), BFA_BAD_SIGNATURE, pModule);
                        }
                        if (elemType == ELEMENT_TYPE_GENERICINST)
                        {
                            CorElementType tmpEType;
                            IfFailThrowBF(tempsig.PeekElemType(&tmpEType), BFA_BAD_SIGNATURE, pModule);

                            if (tmpEType == ELEMENT_TYPE_CLASS)
                                typeHnd = TypeHandle(g_pHiddenMethodTableClass);
                            else
                                argDrop = TRUE;                            
                        }
                        else if (CorTypeInfo::GetGCType(elemType) == TYPE_GC_REF)
                        {
                            typeHnd = TypeHandle(g_pHiddenMethodTableClass);
                        }                    
                    }
                    else
                    {
                        argLevel = (ClassLoadLevel) (level-1);
                    }
                }
                if (typeHnd.IsNull())
                {
                    typeHnd = psig.GetTypeHandleThrowing(pModule, 
                                                         pTypeContext, 
                                                         fLoadTypes, 
                                                         argLevel,
                                                         argDrop,
                                                         pSubst, 
                                                         pCurrentModule);

                }
                if (typeHnd.IsNull()) 
                {
                    thRet = TypeHandle();
                    goto Exit;
                }
                thisinst[i] = typeHnd;

                IfFailThrowBF(psig.SkipExactlyOne(), BFA_BAD_SIGNATURE, pModule);

            }

            // Now make the instantiated type
            // The class loader will check the arity 
            thRet = (ClassLoader::LoadGenericInstantiationThrowing(pDefModule,
                                                                 typeDef,
                                                                 ntypars, thisinst, 
                                                                 fLoadTypes, level));
            }
            goto Exit;                                                                 
        }

        case ELEMENT_TYPE_CLASS:
            // intentional fallthru to ELEMENT_TYPE_VALUETYPE
        case ELEMENT_TYPE_VALUETYPE:
        {
            mdTypeRef typeref;
            IfFailThrowBF(psig.GetToken(&typeref), BFA_BAD_SIGNATURE, pModule);

            if (TypeFromToken(typeref) != mdtTypeRef && TypeFromToken(typeref) != mdtTypeDef)
                THROW_BAD_FORMAT(BFA_UNEXPECTED_TOKEN_AFTER_CLASSVALTYPE, pModule);

            if (typeref == mdTypeDefNil || typeref == mdTypeRefNil)
                THROW_BAD_FORMAT(BFA_UNEXPECTED_TOKEN_AFTER_CLASSVALTYPE, pModule);
            
            TypeHandle loadedType = 
                ClassLoader::LoadTypeDefOrRefThrowing(pModule, typeref, 
                                                      ((fLoadTypes == ClassLoader::LoadTypes) ? ClassLoader::ThrowIfNotFound : ClassLoader::ReturnNullIfNotFound), 
                                                      // pCurrentModule is only set when decoding zapsigs
                                                      // ZapSigs use uninstantiated tokens to represent the GenericTypeDefinition
                                                      (pCurrentModule ? ClassLoader::PermitUninstDefOrRef : ClassLoader::FailIfUninstDefOrRef),
                                                      ((fLoadTypes == ClassLoader::LoadTypes) ? tdNoTypes : tdAllTypes), 
                                                      level);

            if (loadedType.IsNull())
            {
                if(TypeFromToken(typeref)==mdtTypeRef)
                {
                    if ((fLoadTypes == ClassLoader::DontLoadTypes))
                    {
                        loadedType = (TypeHandle((&g_Mscorlib)->LookupElementType(ELEMENT_TYPE_VOID, level)));
                    }
                    else
                    {
                        loadedType = (TypeHandle((&g_Mscorlib)->FetchElementType(
                            ELEMENT_TYPE_VOID, (fLoadTypes == ClassLoader::LoadTypes), level)));
                    }
                }

                thRet = (loadedType);
                goto Exit;
            }
            
            if ((fLoadTypes == ClassLoader::LoadTypes))
            {
                bool typFromSigIsClass = (typ == ELEMENT_TYPE_CLASS);
                bool typLoadedIsClass  = (loadedType.GetSignatureCorElementType() == ELEMENT_TYPE_CLASS);
            
                if (typFromSigIsClass != typLoadedIsClass)
#ifndef DACCESS_COMPILE
                {
                    if((pModule->GetMDImport()->GetMetadataStreamVersion() != MD_STREAM_VER_1X)
                        || !Security::CanSkipVerification(pModule->GetDomainAssembly()))
                            pModule->GetAssembly()->ThrowTypeLoadException(pModule->GetMDImport(),
                                                                           typeref, 
                                                                           BFA_CLASSLOAD_VALUETYPEMISMATCH);
                }
#else
                DacNotImpl();
#endif // #ifndef DACCESS_COMPILE
            }

            thRet = (loadedType);
            goto Exit;
        }

        case ELEMENT_TYPE_ARRAY:
        case ELEMENT_TYPE_SZARRAY:
        {
            TypeHandle elemType = psig.GetTypeHandleThrowing(pModule, 
                                                             pTypeContext, 
                                                             fLoadTypes, 
                                                             level,
                                                             dropGenericArgumentLevel,
                                                             pSubst, 
                                                             pCurrentModule);
            if (elemType.IsNull())
            {
                thRet = (elemType);
                goto Exit;
            }

            ULONG rank = 0;

            if (typ == ELEMENT_TYPE_ARRAY) {
                IfFailThrowBF(psig.SkipExactlyOne(), BFA_BAD_SIGNATURE, pModule);
                IfFailThrowBF(psig.GetData(&rank), BFA_BAD_SIGNATURE, pModule);

                _ASSERTE(0 < rank);
            }
            thRet = ClassLoader::LoadArrayTypeThrowing(elemType, typ, rank, fLoadTypes, level);
            goto Exit;
        }

        case ELEMENT_TYPE_PINNED:
            // Return what follows
            thRet = psig.GetTypeHandleThrowing(pModule, 
                                               pTypeContext, 
                                               fLoadTypes, 
                                               level,
                                               dropGenericArgumentLevel,
                                               pSubst, 
                                               pCurrentModule);
            goto Exit;                                              

        case ELEMENT_TYPE_BYREF:
        case ELEMENT_TYPE_PTR:
        {
            TypeHandle baseType = psig.GetTypeHandleThrowing(pModule, 
                                                             pTypeContext, 
                                                             fLoadTypes, 
                                                             level,
                                                             dropGenericArgumentLevel,
                                                             pSubst, 
                                                             pCurrentModule);
            
            if (baseType.IsNull())
            {
                thRet = (baseType);
            }
            else
            {
                thRet = (ClassLoader::LoadPointerOrByrefTypeThrowing(typ, baseType, fLoadTypes, level));
            }
            goto Exit;
        }

        case ELEMENT_TYPE_FNPTR:
#ifndef DACCESS_COMPILE
            
            {
                ULONG uCallConv;
                
                IfFailThrowBF(psig.GetData(&uCallConv), BFA_BAD_SIGNATURE, pModule);

                if ((uCallConv & IMAGE_CEE_CS_CALLCONV_MASK) == IMAGE_CEE_CS_CALLCONV_FIELD)
                    THROW_BAD_FORMAT(BFA_FNPTR_CANNOT_BE_A_FIELD, pModule);

                if ((uCallConv & IMAGE_CEE_CS_CALLCONV_GENERIC) > 0)
                    THROW_BAD_FORMAT(BFA_FNPTR_CANNOT_BE_GENERIC, pModule);
                
                // Get arg count;
                ULONG cArgs;
                IfFailThrowBF(psig.GetData(&cArgs), BFA_BAD_SIGNATURE, pModule);

                ULONG cAllocaSize;
                if (!ClrSafeInt<ULONG>::addition(cArgs, 1, cAllocaSize) ||
                    !ClrSafeInt<ULONG>::multiply(cAllocaSize, sizeof(TypeHandle), cAllocaSize))
                {
                    ThrowHR(COR_E_OVERFLOW);
                }
                
                if (ShouldProbeOnThisThread() && (cAllocaSize/PAGE_SIZE+1) >= 2)
                {
                    DO_INTERIOR_STACK_PROBE_FOR_NOTHROW(GetThread(), (10+cAllocaSize/PAGE_SIZE+1), NO_FORBIDGC_LOADER_USE_ThrowSO(););
                }

                TypeHandle *retAndArgTypes = (TypeHandle*) _alloca(cAllocaSize);

                for (unsigned i = 0; i <= cArgs; i++)
                {
                    retAndArgTypes[i] = psig.GetTypeHandleThrowing(pModule, 
                                                                   pTypeContext, 
                                                                   fLoadTypes, 
                                                                   level,
                                                                   dropGenericArgumentLevel,
                                                                   pSubst, 
                                                                   pCurrentModule);
                    if (retAndArgTypes[i].IsNull())
                    {
                        TypeHandle thNull;
                        thRet = (thNull);
                        goto Exit;
                    }

                    IfFailThrowBF(psig.SkipExactlyOne(), BFA_BAD_SIGNATURE, pModule);
                }

                // Now make the function pointer type
                thRet = ClassLoader::LoadFnptrTypeThrowing((BYTE) uCallConv, cArgs, retAndArgTypes, fLoadTypes, level);
                goto Exit;
            }
#else
            DacNotImpl();
            break;
#endif // #ifndef DACCESS_COMPILE

        case ELEMENT_TYPE_INTERNAL :
            {
                TypeHandle hType;
                CorSigUncompressPointer(psig.GetPtr(), (void**)&hType);
                // workaround unreachable code warning
                // RETURN(hType);
                thRet = hType;
                break;
            }

        default:
#ifdef _DEBUG_IMPL
            _ASSERTE(!FORBIDGC_LOADER_USE_ENABLED());
#endif
            THROW_BAD_FORMAT(BFA_BAD_COMPLUS_SIG, pModule);

#ifdef VALUE_ARRAYS
        case ELEMENT_TYPE_VALUEARRAY: // For now type handles to value arrays unsupported
#endif
        case ELEMENT_TYPE_SENTINEL:
#ifndef DACCESS_COMPILE

            mdToken token;

            IfFailThrowBF(psig.GetToken(&token), BFA_BAD_SIGNATURE, pModule);
          
            pModule->GetAssembly()->ThrowTypeLoadException(pModule->GetMDImport(),
                                                        token,
                                                        IDS_CLASSLOAD_GENERAL);
#else
            DacNotImpl();
            break;
#endif // #ifndef DACCESS_COMPILE
    }
    }

Exit:
    ;
    END_INTERIOR_STACK_PROBE;
    
    RETURN (thRet);
}

// SigPointer should be just after E_T_VAR or E_T_MVAR
TypeHandle SigPointer::GetTypeVariableThrowing(Module *pModule, // unused - may be used later for better error reporting
                                               CorElementType et,
                                               ClassLoader::LoadTypesFlag fLoadTypes/*=LoadTypes*/,
                                               const SigTypeContext *pTypeContext)
{
    CONTRACT(TypeHandle)
    {
        INSTANCE_CHECK;
        PRECONDITION(et == ELEMENT_TYPE_VAR || et == ELEMENT_TYPE_MVAR);
        if (FORBIDGC_LOADER_USE_ENABLED()) NOTHROW; else THROWS;
        MODE_ANY;
        if (FORBIDGC_LOADER_USE_ENABLED()) GC_NOTRIGGER; else GC_TRIGGERS;
        if (FORBIDGC_LOADER_USE_ENABLED()) FORBID_FAULT; else { INJECT_FAULT(COMPlusThrowOM()); }
        POSTCONDITION(CheckPointer(RETVAL, ((fLoadTypes == ClassLoader::LoadTypes) ? NULL_NOT_OK : NULL_OK)));
    }
    CONTRACT_END

    TypeHandle res = GetTypeVariable(et, pTypeContext);
#ifndef DACCESS_COMPILE
    if (res.IsNull() && (fLoadTypes == ClassLoader::LoadTypes))
    {
       COMPlusThrowHR(COR_E_BADIMAGEFORMAT);
    }
#endif
    RETURN(res);
}

// SigPointer should be just after E_T_VAR or E_T_MVAR
TypeHandle SigPointer::GetTypeVariable(CorElementType et,
                                       const SigTypeContext *pTypeContext)
{

    CONTRACT(TypeHandle)
    {
        INSTANCE_CHECK;
        PRECONDITION(et == ELEMENT_TYPE_VAR || et == ELEMENT_TYPE_MVAR);
        NOTHROW;
        GC_NOTRIGGER;
        POSTCONDITION(CheckPointer(RETVAL, NULL_OK)); // will return TypeHandle() if index is out of range
#ifndef DACCESS_COMPILE
        //        POSTCONDITION(RETVAL.IsNull() || RETVAL.IsRestored() || RETVAL.GetMethodTable()->IsRestoring());
#endif
        MODE_ANY;
    }
    CONTRACT_END

    DWORD index;
    if (FAILED(GetData(&index)))
    {
        TypeHandle thNull;
        RETURN(thNull);
    }

    if (!pTypeContext 
        ||
        (et == ELEMENT_TYPE_VAR && 
         (index >= pTypeContext->m_classInstCount 
          || pTypeContext->m_classInst == NULL
          || pTypeContext->m_classInst[index].IsNull()))
        ||
        (et == ELEMENT_TYPE_MVAR && 
         (index >= pTypeContext->m_methInstCount
          || pTypeContext->m_methInst == NULL
          || pTypeContext->m_methInst[index].IsNull())))
    {
        LOG((LF_ALWAYS, LL_INFO1000, "GENERICS: Error: GetTypeVariable on out-of-range type variable\n"));
        BAD_FORMAT_NOTHROW_ASSERT(!"Invalid type context: either this is an ill-formed signature (e.g. an invalid type variable number) or you have not provided a non-empty SigTypeContext where one is required.  Check back on the callstack for where the value of pTypeContext is first provided, and see if it is acquired from the correct place.  For calls originating from a JIT it should be acquired from the context parameter, which indicates the method being compiled.  For calls from other locations it should be acquired from the MethodTable, EEClass, TypeHandle, FieldDesc or MethodDesc being analyzed.");
        TypeHandle thNull;
        RETURN(thNull);
    }
    if (et == ELEMENT_TYPE_VAR)
    {
        RETURN(pTypeContext->m_classInst[index]);
    }
    else
    {
        RETURN(pTypeContext->m_methInst[index]);
    }
}


#ifndef DACCESS_COMPILE

// Does this type contain class or method type parameters whose instantiation cannot
// be determined at JIT-compile time from the instantiations in the method context?
// Return a combination of hasClassVar and hasMethodVar flags.
// See header file for more info.
VarKind SigPointer::IsPolyType(const SigTypeContext *pTypeContext) const
{
    CONTRACTL
    {
        INSTANCE_CHECK;
        NOTHROW;
        GC_NOTRIGGER;
        MODE_ANY;
    }
    CONTRACTL_END

    SigPointer psig = *this;
    CorElementType typ;

    if (FAILED(psig.GetElemType(&typ)))
        return hasNoVars;

    switch(typ) {
        case ELEMENT_TYPE_VAR:
        case ELEMENT_TYPE_MVAR:
        {
            VarKind res = (typ == ELEMENT_TYPE_VAR ? hasClassVar : hasMethodVar);
            TypeHandle ty = psig.GetTypeVariable(typ, pTypeContext);
            if (ty.GetTypeArgumentSharingInfo() != TypeHandle::NonShared)
                res = (VarKind) (res | (typ == ELEMENT_TYPE_VAR ? hasSharableClassVar : hasSharableMethodVar));
            return (res);
        }

        case ELEMENT_TYPE_U:
        case ELEMENT_TYPE_I:
        case ELEMENT_TYPE_STRING:
        case ELEMENT_TYPE_OBJECT:
        case ELEMENT_TYPE_I1:
        case ELEMENT_TYPE_U1:
        case ELEMENT_TYPE_BOOLEAN:
        case ELEMENT_TYPE_I2:
        case ELEMENT_TYPE_U2:
        case ELEMENT_TYPE_CHAR:
        case ELEMENT_TYPE_I4:
        case ELEMENT_TYPE_U4:
        case ELEMENT_TYPE_I8:
        case ELEMENT_TYPE_U8:
        case ELEMENT_TYPE_R4:
        case ELEMENT_TYPE_R8:
        case ELEMENT_TYPE_VOID:
        case ELEMENT_TYPE_CLASS:
        case ELEMENT_TYPE_VALUETYPE:
        case ELEMENT_TYPE_TYPEDBYREF:
            return(hasNoVars);

        case ELEMENT_TYPE_GENERICINST:
          {
            VarKind k = psig.IsPolyType(pTypeContext);
            if (FAILED(psig.SkipExactlyOne()))
                return hasNoVars;

            ULONG ntypars;
            if(FAILED(psig.GetData(&ntypars)))
                return hasNoVars;
            
            for (ULONG i = 0; i < ntypars; i++)
            {
              k = (VarKind) (psig.IsPolyType(pTypeContext) | k);
              if (FAILED(psig.SkipExactlyOne()))
                return hasNoVars;
            }
            return(k);
          }

        case ELEMENT_TYPE_ARRAY:
        case ELEMENT_TYPE_SZARRAY:
#ifdef VALUE_ARRAYS
        case ELEMENT_TYPE_VALUEARRAY:
#endif
        case ELEMENT_TYPE_PINNED:
        case ELEMENT_TYPE_BYREF:
        case ELEMENT_TYPE_PTR:
        {
            return(psig.IsPolyType(pTypeContext));
        }

        case ELEMENT_TYPE_FNPTR:
        {
            if (FAILED(psig.GetData(NULL)))
                return hasNoVars;

            // Get arg count;
            ULONG cArgs;
            if (FAILED(psig.GetData(&cArgs)))
                return hasNoVars;

            VarKind k = psig.IsPolyType(pTypeContext);
            if (FAILED(psig.SkipExactlyOne()))
                return hasNoVars;
            
            for (unsigned i = 0; i < cArgs; i++)
            {
                k = (VarKind) (psig.IsPolyType(pTypeContext) | k);
                if (FAILED(psig.SkipExactlyOne()))
                    return hasNoVars;
            }

            return(k);
        }

        default:
            BAD_FORMAT_NOTHROW_ASSERT(!"Bad type");
    }
    return(hasNoVars);
}

BOOL SigPointer::IsStringType(Module* pModule, const SigTypeContext *pTypeContext) const
{
    WRAPPER_CONTRACT;

    return IsStringTypeHelper(pModule, pTypeContext, FALSE);
}


BOOL SigPointer::IsStringTypeThrowing(Module* pModule, const SigTypeContext *pTypeContext) const
{
    WRAPPER_CONTRACT;

    return IsStringTypeHelper(pModule, pTypeContext, TRUE);
}

BOOL SigPointer::IsStringTypeHelper(Module* pModule, const SigTypeContext* pTypeContext, BOOL fThrow) const
{
    CONTRACTL
    {
        INSTANCE_CHECK;
        if (fThrow)
        {
            THROWS;
            GC_TRIGGERS;
        }
        else
        {
            NOTHROW;
            GC_NOTRIGGER;
        }
        
        FORBID_FAULT;
        MODE_ANY;
        PRECONDITION(CheckPointer(pModule));
    }
    CONTRACTL_END

    IMDInternalImport *pInternalImport = pModule->GetMDImport();
    SigPointer psig = *this;
    CorElementType typ;
    if (FAILED(psig.GetElemType(&typ)))
    {
        if (fThrow)
            ThrowHR(META_E_BAD_SIGNATURE);
        else
            return FALSE;
    }
        
    switch (typ)
    {
        case ELEMENT_TYPE_STRING :
            return(TRUE);

        case ELEMENT_TYPE_CLASS :
        {
            LPCUTF8 pclsname;
            LPCUTF8 pszNamespace;
            mdToken token;

            if (FAILED( psig.GetToken(&token)))
            {
                if (fThrow)
                    ThrowHR(META_E_BAD_SIGNATURE);
                else
                    return FALSE;
            }

            if (TypeFromToken(token) == mdtTypeDef)
                pInternalImport->GetNameOfTypeDef(token, &pclsname, &pszNamespace);
            else
            {
                BAD_FORMAT_NOTHROW_ASSERT(TypeFromToken(token) == mdtTypeRef);
                pInternalImport->GetNameOfTypeRef(token, &pszNamespace, &pclsname);
            }

            if (strcmp(pclsname, g_StringName) != 0)
                return(FALSE);

            if (!pszNamespace)
                return(FALSE);

            return(!strcmp(pszNamespace, g_SystemNS));
        }

        case ELEMENT_TYPE_VAR :
        case ELEMENT_TYPE_MVAR :
        {
            TypeHandle ty;

            if (fThrow)
                ty = psig.GetTypeVariableThrowing(pModule, typ, ClassLoader::LoadTypes, pTypeContext);
            else
                ty = psig.GetTypeVariable(typ, pTypeContext);
            
            TypeHandle th(g_pStringClass);
            return(ty == th);
        }
        
        default:
            break;
    }
    return(FALSE);
}


//------------------------------------------------------------------------
// Tests if the element class name is szClassName.
//------------------------------------------------------------------------
BOOL SigPointer::IsClass(Module* pModule, LPCUTF8 szClassName, const SigTypeContext *pTypeContext) const
{
    WRAPPER_CONTRACT;

    return IsClassHelper(pModule, szClassName, pTypeContext, FALSE);
}


//------------------------------------------------------------------------
// Tests if the element class name is szClassName.
//------------------------------------------------------------------------
BOOL SigPointer::IsClassThrowing(Module* pModule, LPCUTF8 szClassName, const SigTypeContext *pTypeContext) const
{
    WRAPPER_CONTRACT;

    return IsClassHelper(pModule, szClassName, pTypeContext, TRUE);
}

BOOL SigPointer::IsClassHelper(Module* pModule, LPCUTF8 szClassName, const SigTypeContext* pTypeContext, BOOL fThrow) const
{
    CONTRACTL
    {
        INSTANCE_CHECK;
        
        if (fThrow)
        {
            THROWS;
            GC_TRIGGERS;
        }
        else
        {
            NOTHROW;
            GC_NOTRIGGER;
        }
        
        FORBID_FAULT;
        MODE_ANY;
        PRECONDITION(CheckPointer(pModule));
        PRECONDITION(CheckPointer(szClassName));
    }
    CONTRACTL_END

    SigPointer psig = *this;
    CorElementType typ;
    if (FAILED(psig.GetElemType(&typ)))
    {
        if (fThrow)
            ThrowHR(META_E_BAD_SIGNATURE);
        else
            return FALSE;
    }

    BAD_FORMAT_NOTHROW_ASSERT((typ == ELEMENT_TYPE_CLASS)  || (typ == ELEMENT_TYPE_VALUETYPE) ||
                      (typ == ELEMENT_TYPE_OBJECT) || (typ == ELEMENT_TYPE_STRING) ||
                      (typ == ELEMENT_TYPE_GENERICINST) || (typ == ELEMENT_TYPE_VAR) || (typ == ELEMENT_TYPE_MVAR) ||
                      (typ == ELEMENT_TYPE_INTERNAL));


    if (typ == ELEMENT_TYPE_VAR || typ == ELEMENT_TYPE_MVAR)
    {
        TypeHandle ty;

        if (fThrow)
            ty = psig.GetTypeVariableThrowing(pModule, typ, ClassLoader::LoadTypes, pTypeContext);
        else
            ty = psig.GetTypeVariable(typ, pTypeContext);
        
        return(!ty.IsNull() && IsTypeRefOrDef(szClassName, ty.GetModule(), ty.GetCl()));
    }
    else if ((typ == ELEMENT_TYPE_CLASS) || (typ == ELEMENT_TYPE_VALUETYPE))
    {
        mdTypeRef typeref;
        if (FAILED(psig.GetToken(&typeref)))
        {
            if (fThrow)
                ThrowHR(META_E_BAD_SIGNATURE);
            else
                return FALSE;
        }

        return( IsTypeRefOrDef(szClassName, pModule, typeref) );
    }
    else if (typ == ELEMENT_TYPE_OBJECT)
    {
        return( !strcmp(szClassName, g_ObjectClassName) );
    }
    else if (typ == ELEMENT_TYPE_STRING)
    {
        return( !strcmp(szClassName, g_StringClassName) );
    }
    else if (typ == ELEMENT_TYPE_INTERNAL) 
    {
        TypeHandle th;
        CorSigUncompressPointer(psig.GetPtr(), (void**)&th);
        _ASSERTE(!th.IsNull());
        return(IsTypeRefOrDef(szClassName, th.GetModule(), th.GetCl()));
    }

    return( false );
}

//------------------------------------------------------------------------
// Tests for the existence of a custom modifier
//------------------------------------------------------------------------
BOOL SigPointer::HasCustomModifier(Module *pModule, LPCSTR szModName, CorElementType cmodtype) const
{
    CONTRACTL
    {
        INSTANCE_CHECK;
        NOTHROW;
        GC_NOTRIGGER;
        FORBID_FAULT;
        MODE_ANY;
    }
    CONTRACTL_END


    BAD_FORMAT_NOTHROW_ASSERT(cmodtype == ELEMENT_TYPE_CMOD_OPT || cmodtype == ELEMENT_TYPE_CMOD_REQD);

    SigPointer sp = *this;
    CorElementType etyp;
    if (sp.AtSentinel())
        sp.m_ptr++;

    BYTE data;

    if (FAILED(sp.GetByte(&data)))
        return FALSE;

    etyp = (CorElementType)data;

    
    while (etyp == ELEMENT_TYPE_CMOD_OPT || etyp == ELEMENT_TYPE_CMOD_REQD) {

        mdToken tk;
        if (FAILED(sp.GetToken(&tk)))
            return FALSE;

        if (etyp == cmodtype && IsTypeRefOrDef(szModName, pModule, tk))
        {
            return(TRUE);
        }

        if (FAILED(sp.GetByte(&data)))
            return FALSE;

        etyp = (CorElementType)data;


    }
    return(FALSE);
}

#endif // #ifndef DACCESS_COMPILE

CorElementType SigPointer::PeekElemTypeNormalized(Module* pModule, const SigTypeContext *pTypeContext) const
{
    CONTRACTL
    {
        INSTANCE_CHECK;
        if (FORBIDGC_LOADER_USE_ENABLED()) NOTHROW; else THROWS;
        if (FORBIDGC_LOADER_USE_ENABLED()) GC_NOTRIGGER; else GC_TRIGGERS;
        if (FORBIDGC_LOADER_USE_ENABLED()) FORBID_FAULT; else { INJECT_FAULT(COMPlusThrowOM()); }
        MODE_ANY;
    }
    CONTRACTL_END

    CorElementType type = PeekElemTypeClosed(pTypeContext);

    if (type == ELEMENT_TYPE_VALUETYPE || type == ELEMENT_TYPE_INTERNAL)
    {
        TypeHandle th = GetTypeHandleThrowing(pModule, pTypeContext, ClassLoader::LoadTypes, CLASS_LOAD_UNRESTORED, TRUE);
        if(th.IsNull())
            return ELEMENT_TYPE_VOID;
        else
            return th.GetInternalCorElementType();
    }

    return(type);
}

CorElementType SigPointer::PeekElemTypeClosed(const SigTypeContext *pTypeContext) const
{
    CONTRACTL
    {
        INSTANCE_CHECK;
        NOTHROW; 
        GC_NOTRIGGER;
        FORBID_FAULT;
        MODE_ANY;
    }
    CONTRACTL_END

    
    CorElementType type;

    if (FAILED(PeekElemType(&type)))
        return ELEMENT_TYPE_END;

    if (type == ELEMENT_TYPE_GENERICINST ||
        type == ELEMENT_TYPE_VAR ||
        type == ELEMENT_TYPE_MVAR ||
        type == ELEMENT_TYPE_INTERNAL)
    {
        SigPointer sp(this->GetPtr());
        if (FAILED(sp.GetElemType(NULL))) // skip over E_T_XXX
            return ELEMENT_TYPE_END;

        switch (type)
        {
        case ELEMENT_TYPE_GENERICINST:
            if (FAILED(sp.GetElemType(&type)))
                return ELEMENT_TYPE_END;

            if (type != ELEMENT_TYPE_INTERNAL)
                return type;

            // intentionally fall through
        case ELEMENT_TYPE_INTERNAL:
            {
                TypeHandle th;
                CorSigUncompressPointer(sp.GetPtr(), (void**)&th);
                _ASSERTE(!th.IsNull());

                return th.GetSignatureCorElementType();
            }
        case ELEMENT_TYPE_VAR :
        case ELEMENT_TYPE_MVAR :
            {          
                TypeHandle th = sp.GetTypeVariable(type, pTypeContext);
                if (th.IsNull())
                {
                    BAD_FORMAT_NOTHROW_ASSERT(!"You either have bad signature or caller forget to pass valid type context");
                    return ELEMENT_TYPE_END;
                }

                return th.GetSignatureCorElementType();
            }
        default:
            UNREACHABLE();
        }
    }

    return(type);
}


mdTypeRef SigPointer::PeekValueTypeTokenClosed(const SigTypeContext *pTypeContext) const
{
    CONTRACTL
    {
        INSTANCE_CHECK;
        NOTHROW;
        GC_NOTRIGGER;
        PRECONDITION(PeekElemTypeClosed(pTypeContext) == ELEMENT_TYPE_VALUETYPE);
        FORBID_FAULT;
        MODE_ANY;
    }
    CONTRACTL_END


    mdToken token;
    CorElementType type;
    if (FAILED(PeekElemType(&type)))
        return mdTokenNil;

    switch (type)
    {
    case ELEMENT_TYPE_GENERICINST:
        {
            SigPointer sp(this->GetPtr());
            if (FAILED(sp.GetElemType(NULL)))
                return mdTokenNil;
            
            if (FAILED(sp.GetElemType(NULL)))
                return mdTokenNil;

            if (FAILED(sp.GetToken(&token)))
                return mdTokenNil;

            return token;
        }
    case ELEMENT_TYPE_VAR :
    case ELEMENT_TYPE_MVAR :
        {
            SigPointer sp(this->GetPtr());

            if (FAILED(sp.GetElemType(NULL)))
                return mdTokenNil;

            TypeHandle th = sp.GetTypeVariable(type, pTypeContext);
            _ASSERTE(!th.IsNull());
            return(th.GetCl());
        }
    case ELEMENT_TYPE_INTERNAL:
        // we have no way to give back a token for the E_T_INTERNAL so we return  a null one
        // and make the caller deal with it
        return mdTokenNil;
    
    default:
        {
            _ASSERTE(type == ELEMENT_TYPE_VALUETYPE);
            SigPointer sp(this->GetPtr());

            if (FAILED(sp.GetElemType(NULL)))
                return mdTokenNil;

            if (FAILED(sp.GetToken(&token)))
                return mdTokenNil;

            return token;
        }
    }
}


//------------------------------------------------------------------------
// Assumes that the SigPointer points to the start of an element type.
// Returns size of that element in bytes. This is the minimum size that a
// field of this type would occupy inside an object.
//------------------------------------------------------------------------
UINT SigPointer::SizeOf(Module* pModule, const SigTypeContext *pTypeContext) const
{
    CONTRACTL
    {
        INSTANCE_CHECK;
        if (FORBIDGC_LOADER_USE_ENABLED()) NOTHROW; else THROWS;
        if (FORBIDGC_LOADER_USE_ENABLED()) GC_NOTRIGGER; else GC_TRIGGERS;
        if (FORBIDGC_LOADER_USE_ENABLED()) FORBID_FAULT; else { INJECT_FAULT(COMPlusThrowOM()); }
        MODE_ANY;
        UNCHECKED(PRECONDITION(CheckPointer(pModule)));
        UNCHECKED(PRECONDITION(CheckPointer(pTypeContext, NULL_OK)));
    }
    CONTRACTL_END

    CorElementType etype = PeekElemTypeClosed(pTypeContext);

#ifdef _DEBUG
    for (int etypeindex = 0; etypeindex < ELEMENT_TYPE_MAX; etypeindex++)
        _ASSERTE(etypeindex == gElementTypeInfo[etypeindex].m_elementType);
#endif

    if (etype < 0 || etype >= ELEMENT_TYPE_MAX)
        THROW_BAD_FORMAT(BFA_BAD_COMPLUS_SIG, pModule);

    int cbsize = gElementTypeInfo[etype].m_cbSize;
    if (cbsize != -1)
        return(cbsize);

    TypeHandle th = TypeHandle();

    if (etype == ELEMENT_TYPE_INTERNAL)
    {
        // Internal types are just raw type handles, which could have any size.
        // Map them back into a cor element type and try again (this time we
        // can't be ELEMENT_TYPE_INTERNAL).
        th = GetTypeHandleThrowing(pModule, pTypeContext);
        _ASSERTE(!th.IsNull());
        etype = th.GetInternalCorElementType();
        cbsize = gElementTypeInfo[etype].m_cbSize;
        if (cbsize != -1)
            return(cbsize);
    }

    switch (etype)
    {

      case ELEMENT_TYPE_VALUETYPE :
      {
          th = GetTypeHandleThrowing(pModule, 
                                     pTypeContext,
                                     ClassLoader::LoadTypes,
                                     CLASS_LOAD_UNRESTOREDTYPEKEY);
          _ASSERTE(!th.IsNull());
          g_IBCLogger.LogEEClassAndMethodTableAccess(th.GetMethodTable()->GetClass());
          return th.GetSize();
      }

#ifdef VALUE_ARRAYS
      case ELEMENT_TYPE_VALUEARRAY :
      {
          SigPointer elemType;
          ULONG count;
          PeekSDArrayElementProps(&elemType, &count);
          UINT ret = elemType.SizeOf(pModule, pTypeContext) * count;
          ret = (ret + 3) & ~3;       // round up to dword alignment
          return(ret);
      }
#endif

      // For class and method variables we just look up the instantiation; if not present assume reference inst
      case ELEMENT_TYPE_VAR :
      case ELEMENT_TYPE_MVAR :
      {
          LOG((LF_ALWAYS, LL_INFO1000, "GENERICS: Warning: SizeOf on VAR without instantiation\n"));
          return(sizeof(LPVOID));
      }

    default :
        ThrowHR(COR_E_BADIMAGEFORMAT, BFA_BAD_ELEM_IN_SIZEOF);
          //return(sizeof(LPVOID));
    }
}

#ifndef DACCESS_COMPILE


//------------------------------------------------------------------
// Determines if the current argument is System.String.
// Caller must determine first that the argument type is ELEMENT_TYPE_CLASS.
//------------------------------------------------------------------

BOOL MetaSig::IsStringType() const
{
    WRAPPER_CONTRACT

    return m_pLastType.IsStringType(m_pModule, &m_typeContext);
}


//------------------------------------------------------------------
// Determines if the current argument is a particular class.
// Caller must determine first that the argument type is ELEMENT_TYPE_CLASS.
//------------------------------------------------------------------
BOOL MetaSig::IsClass(LPCUTF8 szClassName) const
{
    WRAPPER_CONTRACT

    return m_pLastType.IsClass(m_pModule, szClassName, &m_typeContext);
}





//------------------------------------------------------------------
// Return the type of an reference if the array is the param type
//  The arg type must be an ELEMENT_TYPE_BYREF
//  ref to array needs additional arg
//------------------------------------------------------------------
CorElementType MetaSig::GetByRefType(TypeHandle *pTy) const
{
    CONTRACTL
    {
        INSTANCE_CHECK;
        THROWS;
        GC_TRIGGERS;
        INJECT_FAULT(COMPlusThrowOM());
        MODE_ANY;
    }
    CONTRACTL_END

    SigPointer sigptr(m_pLastType);

    CorElementType typ;
    IfFailThrowBF(sigptr.GetElemType(&typ), BFA_BAD_SIGNATURE, GetModule());

    _ASSERTE(typ == ELEMENT_TYPE_BYREF);
    typ = (CorElementType)sigptr.PeekElemTypeClosed(&m_typeContext);

    if (!CorIsPrimitiveType(typ))
    {
        if (typ == ELEMENT_TYPE_TYPEDBYREF)
            THROW_BAD_FORMAT(BFA_TYPEDBYREFCANNOTHAVEBYREF, GetModule());
        TypeHandle th = sigptr.GetTypeHandleThrowing(m_pModule, &m_typeContext);
        *pTy = th;
        return(th.GetSignatureCorElementType());
    }
    return(typ);
}


HRESULT CompareTypeTokensNT(mdToken tk1, mdToken tk2, Module *pModule1, Module *pModule2)
{
    STATIC_CONTRACT_NOTHROW;

    HRESULT hr;
    BEGIN_EXCEPTION_GLUE(&hr, NULL)
    {
        if (CompareTypeTokens(tk1, tk2, pModule1, pModule2))
            hr = S_OK;
        else
            hr = S_FALSE;
    }
    END_EXCEPTION_GLUE
    return hr;
}

#endif // #ifndef DACCESS_COMPILE

BOOL CompareTypeTokens(mdToken tk1, mdToken tk2, Module *pModule1, Module *pModule2)
{
    CONTRACTL
    {
        THROWS;
        GC_TRIGGERS;
        INJECT_FAULT(COMPlusThrowOM());
        MODE_ANY;
    }
    CONTRACTL_END

    if (PTR_HOST_TO_TADDR(pModule1) == PTR_HOST_TO_TADDR(pModule2) &&
        tk1 == tk2)
        return(TRUE);

    IMDInternalImport *pInternalImport1 = pModule1->GetMDImport();
    if (!pInternalImport1->IsValidToken(tk1)) {
        BAD_FORMAT_NOTHROW_ASSERT(!"Invalid token");
#ifdef DACCESS_COMPILE
        ThrowHR(COR_E_BADIMAGEFORMAT);
#else
        EEFileLoadException::Throw(pModule1->GetFile(), COR_E_BADIMAGEFORMAT);
#endif //!DACCESS_COMPILE
    }

    LPCUTF8 pszName1;
    LPCUTF8 pszNamespace1 = NULL;
    if (TypeFromToken(tk1) == mdtTypeRef)
        pInternalImport1->GetNameOfTypeRef(tk1, &pszNamespace1, &pszName1);
    else if (TypeFromToken(tk1) == mdtTypeDef) {
        if (TypeFromToken(tk2) == mdtTypeDef) // two defs can't be the same unless they are identical
            return(FALSE);
        pInternalImport1->GetNameOfTypeDef(tk1, &pszName1, &pszNamespace1);
    }
    else
        return(FALSE); // comparing a type against a module or assemblyref, no match

    IMDInternalImport *pInternalImport2 = pModule2->GetMDImport();
    if (!pInternalImport2->IsValidToken(tk2)) {
        BAD_FORMAT_NOTHROW_ASSERT(!"Invalid token");
#ifdef DACCESS_COMPILE
        ThrowHR(COR_E_BADIMAGEFORMAT);
#else
        EEFileLoadException::Throw(pModule2->GetFile(), COR_E_BADIMAGEFORMAT);
#endif //!DACCESS_COMPILE
    }

    LPCUTF8 pszName2;
    LPCUTF8 pszNamespace2 = NULL;
    if (TypeFromToken(tk2) == mdtTypeRef)
        pInternalImport2->GetNameOfTypeRef(tk2, &pszNamespace2, &pszName2);
    else if (TypeFromToken(tk2) == mdtTypeDef)
        pInternalImport2->GetNameOfTypeDef(tk2, &pszName2, &pszNamespace2);
    else
        return(FALSE);       // comparing a type against a module or assemblyref, no match

    _ASSERTE(pszNamespace1 && pszNamespace2);
    if (strcmp(pszName1, pszName2) != 0 || strcmp(pszNamespace1, pszNamespace2) != 0)
        return(FALSE);

    //////////////////////////////////////////////////////////////////////
    // OK names pass, see if it is nested, and if so that the nested classes are the same

    mdToken enclosingTypeTk1 = mdTokenNil;
    if (TypeFromToken(tk1) == mdtTypeRef) {
        enclosingTypeTk1 = pInternalImport1->GetResolutionScopeOfTypeRef(tk1);
        if (enclosingTypeTk1 == mdTypeRefNil)
            enclosingTypeTk1 = mdTokenNil;
    }
    else
         pInternalImport1->GetNestedClassProps(tk1, &enclosingTypeTk1);


    mdToken enclosingTypeTk2 = mdTokenNil;
    if (TypeFromToken(tk2) == mdtTypeRef) {
        enclosingTypeTk2 = pInternalImport2->GetResolutionScopeOfTypeRef(tk2);
        if (enclosingTypeTk2 == mdTypeRefNil)
            enclosingTypeTk2 = mdTokenNil;
    }
    else
         pInternalImport2->GetNestedClassProps(tk2, &enclosingTypeTk2);

    if (TypeFromToken(enclosingTypeTk1) == mdtTypeRef || TypeFromToken(enclosingTypeTk1) == mdtTypeDef)
        return(CompareTypeTokens(enclosingTypeTk1, enclosingTypeTk2, pModule1, pModule2));

    // Check if tk1 is non-nested, but tk2 is nested
    if (TypeFromToken(enclosingTypeTk2) == mdtTypeRef || TypeFromToken(enclosingTypeTk2) == mdtTypeDef)
        return(FALSE);

    //////////////////////////////////////////////////////////////////////
    // OK, we have non-nested types

    
    // Do not load the type! (Or else you may run into circular dependency loading problems.)
    Module* pFoundModule1;
    if (!ClassLoader::ResolveTokenToTypeDefThrowing(pModule1,
                                                    tk1,
                                                    &pFoundModule1,
                                                    NULL))
        return(FALSE);

    Module* pFoundModule2;
    if (!ClassLoader::ResolveTokenToTypeDefThrowing(pModule2,
                                                    tk2,
                                                    &pFoundModule2,
                                                    NULL))
        return(FALSE);
    
    return(PTR_HOST_TO_TADDR(pFoundModule1) ==
           PTR_HOST_TO_TADDR(pFoundModule2));
}

//
// Compare the next elements in two sigs.
//
/*static*/
BOOL MetaSig::CompareElementType(
    PCCOR_SIGNATURE &pSig1,
    PCCOR_SIGNATURE &pSig2,
    PCCOR_SIGNATURE pEndSig1, // end of sig1
    PCCOR_SIGNATURE pEndSig2, // end of sig2
    Module*         pModule1,
    Module*         pModule2,
    const Substitution*   pSubst1,
    const Substitution*   pSubst2,
    SigPointer *pMethodInst1,
    SigPointer *pMethodInst2
)
{
    CONTRACTL
    {
        THROWS;
        GC_TRIGGERS;
        INJECT_FAULT(COMPlusThrowOM());
        MODE_ANY;
    }
    CONTRACTL_END

    CorElementType Type1;
    CorElementType Type2;


 redo:  // We jump here if the Type was a ET_CMOD prefix.
        // The caller expects us to handle CMOD's but not
        // present them as types on their own.

    if (pSig1 >= pEndSig1 || pSig2 >= pEndSig2)
        return(FALSE); // end of sig encountered prematurely

    if (*pSig2 == ELEMENT_TYPE_VAR && pSubst2 != NULL && pSubst2->GetInstSig() != NULL)
      {
        SigPointer inst = SigPointer(pSubst2->GetInstSig());
        pSig2++;
        DWORD index = CorSigUncompressData(pSig2);
        for (DWORD i = 0; i < index; i++) inst.SkipExactlyOne();
        PCCOR_SIGNATURE pSig3 = inst.GetPtr();
        inst.SkipExactlyOne();
        PCCOR_SIGNATURE pEndSig3 = inst.GetPtr();
        return(CompareElementType(pSig1, pSig3, pEndSig1, pEndSig3, pModule1, pSubst2->GetModule(), pSubst1, pSubst2->GetNext(), pMethodInst1, NULL));
      }

    if (*pSig1 == ELEMENT_TYPE_VAR && pSubst1 != NULL && pSubst1->GetInstSig() != NULL)
      {
        SigPointer inst = SigPointer(pSubst1->GetInstSig());
        pSig1++;
        DWORD index = CorSigUncompressData(pSig1);
        for (DWORD i = 0; i < index; i++) inst.SkipExactlyOne();
        PCCOR_SIGNATURE pSig3 = inst.GetPtr();
        inst.SkipExactlyOne();
        PCCOR_SIGNATURE pEndSig3 = inst.GetPtr();
        return(CompareElementType(pSig3, pSig2, pEndSig3, pEndSig2, pSubst1->GetModule(), pModule2, pSubst1->GetNext(), pSubst2, NULL, pMethodInst2));
      }

    if (*pSig1 == ELEMENT_TYPE_MVAR && pMethodInst1 != NULL)
      {
        SigPointer inst = *pMethodInst1;
        pSig1++;
        DWORD index = CorSigUncompressData(pSig1);
        for (DWORD i = 0; i < index; i++) inst.SkipExactlyOne();
        PCCOR_SIGNATURE pSig3 = inst.GetPtr();
        inst.SkipExactlyOne();
        PCCOR_SIGNATURE pEndSig3 = inst.GetPtr();
        return(CompareElementType(pSig3, pSig2, pEndSig3, pEndSig2, pModule1, pModule2, NULL, pSubst2, NULL, pMethodInst2));
      }

    if (*pSig2 == ELEMENT_TYPE_MVAR && pMethodInst2 != NULL)
      {
        SigPointer inst = *pMethodInst2;
        pSig2++;
        DWORD index = CorSigUncompressData(pSig2);
        for (DWORD i = 0; i < index; i++) inst.SkipExactlyOne();
        PCCOR_SIGNATURE pSig3 = inst.GetPtr();
        inst.SkipExactlyOne();
        PCCOR_SIGNATURE pEndSig3 = inst.GetPtr();
        return(CompareElementType(pSig1, pSig3, pEndSig1, pEndSig3, pModule1, pModule2, pSubst1, NULL, pMethodInst1, NULL));
      }

    Type1 = CorSigUncompressElementType(pSig1);
    Type2 = CorSigUncompressElementType(pSig2);

    if (Type1 != Type2)
    {
        if (Type1 == ELEMENT_TYPE_INTERNAL || Type2 == ELEMENT_TYPE_INTERNAL)
        {
            TypeHandle      hInternal;
            PCCOR_SIGNATURE pOtherSig;
            CorElementType  eOtherType;
            Module         *pOtherModule;

            // One type is already loaded, collect all the necessary information
            // to identify the other type.
            if (Type1 == ELEMENT_TYPE_INTERNAL)
            {
                pSig1 += CorSigUncompressPointer(pSig1, (void**)&hInternal);
                eOtherType = Type2;
                pOtherSig = pSig2;
                pOtherModule = pModule2;
            }
            else
            {
                pSig2 += CorSigUncompressPointer(pSig2, (void**)&hInternal);
                eOtherType = Type1;
                pOtherSig = pSig1;
                pOtherModule = pModule1;
            }

            // Internal types can only correspond to types or value types.
            switch (eOtherType)
            {
            case ELEMENT_TYPE_OBJECT:
                return(hInternal.AsMethodTable() == g_pObjectClass);
            case ELEMENT_TYPE_STRING:
                return(hInternal.AsMethodTable() == g_pStringClass);
            case ELEMENT_TYPE_VALUETYPE:
            case ELEMENT_TYPE_CLASS:
            {
                mdToken tkOther;
                ULONG count = CorSigUncompressToken(pOtherSig, &tkOther);
                (Type1 == ELEMENT_TYPE_INTERNAL) ? pSig2 += count : pSig1 += count;
                TypeHandle hOtherType;

                // We need to load the other type to check for type identity.
                GCX_COOP();
                hOtherType = ClassLoader::LoadTypeDefOrRefThrowing(pOtherModule, tkOther, 
                                                                   ClassLoader::ReturnNullIfNotFound, 
                                                                   ClassLoader::FailIfUninstDefOrRef);

                return(hInternal == hOtherType);
            }
            default:
                return(FALSE);
            }

#ifdef _DEBUG
            // Shouldn't get here.
            _ASSERTE(FALSE);
            return(FALSE);
#endif
        }
        else
            return(FALSE); // types must be the same
    }

    switch (Type1)
    {
        default:
        {
            // Unknown type!
            THROW_BAD_FORMAT(BFA_BAD_COMPLUS_SIG, pModule1);
        }

        case ELEMENT_TYPE_U:
        case ELEMENT_TYPE_I:
        case ELEMENT_TYPE_VOID:
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
        case ELEMENT_TYPE_BOOLEAN:
        case ELEMENT_TYPE_CHAR:
        case ELEMENT_TYPE_TYPEDBYREF:
        case ELEMENT_TYPE_STRING:
        case ELEMENT_TYPE_OBJECT:
            break;


        case ELEMENT_TYPE_VAR:
        case ELEMENT_TYPE_MVAR:
        {
            unsigned varNum1 = CorSigUncompressData(pSig1);
            unsigned varNum2 = CorSigUncompressData(pSig2);
            return(varNum1 == varNum2);
        }
        case ELEMENT_TYPE_CMOD_REQD:
        case ELEMENT_TYPE_CMOD_OPT:
        {
            mdToken      tk1, tk2;

            pSig1 += CorSigUncompressToken(pSig1, &tk1);
            pSig2 += CorSigUncompressToken(pSig2, &tk2);

#ifndef DACCESS_COMPILE
            if (!CompareTypeDefOrRefOrSpec(pModule1, tk1, pSubst1, pModule2, tk2, pSubst2))
                return(FALSE);
#endif

            goto redo;
        }
        break;

        // These take an additional argument, which is the element type
        case ELEMENT_TYPE_SZARRAY:
        case ELEMENT_TYPE_PTR:
        case ELEMENT_TYPE_BYREF:
        {
            if (!CompareElementType(pSig1, pSig2, pEndSig1, pEndSig2, pModule1, pModule2, pSubst1, pSubst2, pMethodInst1, pMethodInst2))
                return(FALSE);
            return(TRUE);
        }

        case ELEMENT_TYPE_VALUETYPE:
        case ELEMENT_TYPE_CLASS:
        {
            mdToken      tk1, tk2;

            pSig1 += CorSigUncompressToken(pSig1, &tk1);
            pSig2 += CorSigUncompressToken(pSig2, &tk2);

            return(CompareTypeTokens(tk1, tk2, pModule1, pModule2));
        }
        case ELEMENT_TYPE_FNPTR:
        {
                // compare calling conventions
            if (CorSigUncompressData(pSig1) != CorSigUncompressData(pSig2))
                return(FALSE);

            DWORD argCnt1 = CorSigUncompressData(pSig1);    // Get Arg Counts
            DWORD argCnt2 = CorSigUncompressData(pSig2);

            if (argCnt1 != argCnt2)
                return(FALSE);
            if (!CompareElementType(pSig1, pSig2, pEndSig1, pEndSig2, pModule1, pModule2, pSubst1, pSubst2, pMethodInst1, pMethodInst2))
                return(FALSE);
            while(argCnt1 > 0) {
                if (!CompareElementType(pSig1, pSig2, pEndSig1, pEndSig2, pModule1, pModule2, pSubst1, pSubst2, pMethodInst1, pMethodInst2))
                    return(FALSE);
                --argCnt1;
            }
            break;
        }
        case ELEMENT_TYPE_GENERICINST:
        {
          // Type constructors
            if (!CompareElementType(pSig1, pSig2, pEndSig1, pEndSig2, pModule1, pModule2, pSubst1, pSubst2, pMethodInst1, pMethodInst2))
                return(FALSE);
            DWORD argCnt1 = CorSigUncompressData(pSig1);    // Get Arg Counts
            DWORD argCnt2 = CorSigUncompressData(pSig2);

            if (argCnt1 != argCnt2)
                return(FALSE);
            while(argCnt1 > 0) {
                if (!CompareElementType(pSig1, pSig2, pEndSig1, pEndSig2, pModule1, pModule2, pSubst1, pSubst2, pMethodInst1, pMethodInst2))
                    return(FALSE);
                --argCnt1;
            }
            break;
        }
        case ELEMENT_TYPE_ARRAY:
        {
            // syntax: ARRAY <base type> rank <count n> <size 1> .... <size n> <lower bound m>
            // <lb 1> .... <lb m>
            DWORD rank1,rank2,dimension_sizes1,dimension_sizes2,dimension_lowerb1,dimension_lowerb2,i;

            // element type
            if (CompareElementType(pSig1, pSig2, pEndSig1, pEndSig2, pModule1, pModule2, pSubst1, pSubst2, pMethodInst1, pMethodInst2) == FALSE)
                return(FALSE);

            rank1 = CorSigUncompressData(pSig1);
            rank2 = CorSigUncompressData(pSig2);

            if (rank1 != rank2)
                return(FALSE);

            // A zero ends the array spec
            if (rank1 == 0)
                break;

            dimension_sizes1 = CorSigUncompressData(pSig1);
            dimension_sizes2 = CorSigUncompressData(pSig2);

            if (dimension_sizes1 != dimension_sizes2)
                return(FALSE);

            for (i = 0; i < dimension_sizes1; i++)
            {
                DWORD size1, size2;

                if (pSig1 == pEndSig1)
                    return(TRUE); // premature end ok

                size1 = CorSigUncompressData(pSig1);
                size2 = CorSigUncompressData(pSig2);

                if (size1 != size2)
                    return(FALSE);
            }

            if (pSig1 == pEndSig1)
                return(TRUE); // premature end ok

            // # dimensions for lower bounds
            dimension_lowerb1 = CorSigUncompressData(pSig1);
            dimension_lowerb2 = CorSigUncompressData(pSig2);

            if (dimension_lowerb1 != dimension_lowerb2)
                return(FALSE);

            for (i = 0; i < dimension_lowerb1; i++)
            {
                DWORD size1, size2;

                if (pSig1 == pEndSig1)
                    return(TRUE); // premature end

                size1 = CorSigUncompressData(pSig1);
                size2 = CorSigUncompressData(pSig2);

                if (size1 != size2)
                    return(FALSE);
            }

            break;
        }
        case ELEMENT_TYPE_INTERNAL:
        {
            TypeHandle hType1, hType2;

            CorSigUncompressPointer(pSig1, (void**)&hType1);
            CorSigUncompressPointer(pSig2, (void**)&hType2);

            return(hType1 == hType2);
        }
    }

    return(TRUE);
}


BOOL MetaSig::CompareTypeDefsUnderSubstitutions(
    MethodTable *pTypeDef1,
    MethodTable *pTypeDef2,
    const Substitution*   pSubst1,
    const Substitution*   pSubst2)
{
    CONTRACTL
    {
        THROWS;
        GC_TRIGGERS;
        INJECT_FAULT(COMPlusThrowOM());
        MODE_ANY;
    }
    CONTRACTL_END

    if (pTypeDef1->GetModule() != pTypeDef2->GetModule())
        return FALSE;

    if (pTypeDef1->GetCl() != pTypeDef2->GetCl())
        return FALSE;

    _ASSERTE(pTypeDef1->HasSameTypeDefAs(pTypeDef2));

    if (pTypeDef1->GetNumGenericArgs() != pTypeDef2->GetNumGenericArgs())
        return FALSE;

    if (pTypeDef1->GetNumGenericArgs() == 0)
        return TRUE;

    if (!pSubst1 || !pSubst2 || !pSubst1->GetInstSig() || !pSubst2->GetInstSig())
        return FALSE;
        
    SigPointer inst1 = SigPointer(pSubst1->GetInstSig());
    SigPointer inst2 = SigPointer(pSubst2->GetInstSig());
    for (DWORD i = 0; i < pTypeDef1->GetNumGenericArgs(); i++)
    {
        PCCOR_SIGNATURE startInst1 = inst1.GetPtr();
        inst1.SkipExactlyOne();
        PCCOR_SIGNATURE endInst1ptr = inst1.GetPtr();
        PCCOR_SIGNATURE startInst2 = inst2.GetPtr();
        inst2.SkipExactlyOne();
        PCCOR_SIGNATURE endInst2ptr = inst2.GetPtr();
        if (!CompareElementType(startInst1, startInst2, endInst1ptr, endInst2ptr, pSubst1->GetModule(), pSubst2->GetModule(), pSubst1->GetNext(), pSubst2->GetNext(), NULL, NULL))
            return FALSE;
    }
    return TRUE;

}


/* static */
BOOL MetaSig::CompareMethodSigs(MetaSig &msig1,
                                MetaSig &msig2,
                                BOOL ignoreCallconv)

{
    CONTRACTL
    {
        THROWS;
        GC_TRIGGERS;
        INJECT_FAULT(COMPlusThrowOM());
        MODE_ANY;
    }
    CONTRACTL_END

    if (!ignoreCallconv && 
        ((msig1.GetCallingConventionInfo() & IMAGE_CEE_CS_CALLCONV_MASK) 
         != (msig2.GetCallingConventionInfo() & IMAGE_CEE_CS_CALLCONV_MASK)))
        return FALSE; // calling convention mismatch

    if (msig1.NumFixedArgs() != msig2.NumFixedArgs())
        return FALSE; // number of arguments don't match

    // check that the argument types are equal
    for (DWORD i = 0; i<msig1.NumFixedArgs(); i++) //@GENERICSVER: does this really do the return type too?
    {
        CorElementType  et1 = msig1.NextArg();
        CorElementType  et2 = msig2.NextArg();
        if (et1 != et2) 
            return FALSE;
        if (!CorTypeInfo::IsPrimitiveType(et1)) 
        {
            if (msig1.GetLastTypeHandleThrowing() != msig2.GetLastTypeHandleThrowing())
                return FALSE; 
        }
    } 

    CorElementType  ret1 = msig1.GetReturnType();
    CorElementType  ret2 = msig2.GetReturnType();
    if (ret1 != ret2)
        return FALSE;

    if (!CorTypeInfo::IsPrimitiveType(ret1)) 
    {
        if (msig1.GetRetTypeHandleThrowing() != msig2.GetRetTypeHandleThrowing())
            return FALSE; 
    }
    
    return TRUE;


}


#ifndef DACCESS_COMPILE

/*static*/
HRESULT MetaSig::CompareMethodSigsNT(
    PCCOR_SIGNATURE pSignature1,
    DWORD       cSig1,
    Module*     pModule1,
    const Substitution *pSubst1,
    PCCOR_SIGNATURE pSignature2,
    DWORD       cSig2,
    Module*     pModule2,
    const Substitution *pSubst2
)
{
    STATIC_CONTRACT_NOTHROW;

    HRESULT hr;
    BEGIN_EXCEPTION_GLUE(&hr, NULL)
    {
        if (CompareMethodSigs(pSignature1, cSig1, pModule1, pSubst1, pSignature2, cSig2, pModule2, pSubst2))
            hr = S_OK;
        else
            hr = S_FALSE;
    }
    END_EXCEPTION_GLUE
    return hr;
}

#endif // #ifndef DACCESS_COMPILE

//
// Compare two method sigs and return whether they are the same.
// @GENERICS: instantiation of the type variables in the second signature
//
/*static*/
BOOL MetaSig::CompareMethodSigs(
    PCCOR_SIGNATURE pSignature1,
    DWORD       cSig1,
    Module*     pModule1,
    const Substitution *pSubst1,
    PCCOR_SIGNATURE pSignature2,
    DWORD       cSig2,
    Module*     pModule2,
    const Substitution *pSubst2
)
{
    CONTRACTL
    {
        THROWS;
        GC_TRIGGERS;
        INJECT_FAULT(COMPlusThrowOM());
        MODE_ANY;
    }
    CONTRACTL_END

    PCCOR_SIGNATURE pSig1 = pSignature1;
    PCCOR_SIGNATURE pSig2 = pSignature2;
    PCCOR_SIGNATURE pEndSig1 = pSignature1 + cSig1;
    PCCOR_SIGNATURE pEndSig2 = pSignature2 + cSig2;
    DWORD           ArgCount1;
    DWORD           ArgCount2;
    DWORD           i;

    // If scopes are the same, and sigs are same, can return.
    // If the sigs aren't the same, but same scope, can't return yet, in
    // case there are two AssemblyRefs pointing to the same assembly or such.
    if ((pModule1 == pModule2) && (cSig1 == cSig2) && pSubst1 == NULL && pSubst2 == NULL &&
        (memcmp(pSig1, pSig2, cSig1) == 0))
        return(TRUE);

    if ((*pSig1 & ~CORINFO_CALLCONV_PARAMTYPE) != (*pSig2 & ~CORINFO_CALLCONV_PARAMTYPE))
        return(FALSE);               // calling convention or hasThis mismatch

    __int8 callConv = *pSig1;

    pSig1++;
    pSig2++;

    if (callConv & IMAGE_CEE_CS_CALLCONV_GENERIC)
    {
      DWORD TyArgCount1 = CorSigUncompressData(pSig1);
      DWORD TyArgCount2 = CorSigUncompressData(pSig2);

      if (TyArgCount1 != TyArgCount2)
        return(FALSE);
    }

    ArgCount1 = CorSigUncompressData(pSig1);
    ArgCount2 = CorSigUncompressData(pSig2);

    if (ArgCount1 != ArgCount2)
    {
        if ((callConv & IMAGE_CEE_CS_CALLCONV_MASK) != IMAGE_CEE_CS_CALLCONV_VARARG)
            return(FALSE);

        // Signature #1 is the caller.  We proceed until we hit the sentinel, or we hit
        // the end of the signature (which is an implied sentinel).  We never worry about
        // what follows the sentinel, because that is the ... part, which is not
        // involved in matching.
        //
        // Theoretically, it's illegal for a sentinel to be the last element in the
        // caller's signature, because it's redundant.  We don't waste our time checking
        // that case, but the metadata validator should.  Also, it is always illegal
        // for a sentinel to appear in a callee's signature.  We assert against this,
        // but in the shipping product the comparison would simply fail.
        //
        // Signature #2 is the callee.  We must hit the exact end of the callee, because
        // we are trying to match on everything up to the variable part.  This allows us
        // to correctly handle overloads, where there are a number of varargs methods
        // to pick from, like m1(int,...) and m2(int,int,...), etc.

        // <= because we want to include a check of the return value!
        for (i=0; i <= ArgCount1; i++)
        {
            // We may be just going out of bounds on the callee, but no further than that.
            _ASSERTE(i <= ArgCount2 + 1);

            // If we matched all the way on the caller, is the callee now complete?
            if (*pSig1 == ELEMENT_TYPE_SENTINEL)
                return(i > ArgCount2);

            // if we have more to compare on the caller side, but the callee side is
            // exhausted, this isn't our match
            if (i > ArgCount2)
                return(FALSE);

            _ASSERT(*pSig2 != ELEMENT_TYPE_SENTINEL);

            // We are in bounds on both sides.  Compare the element.
            if (CompareElementType(pSig1, pSig2, pEndSig1, pEndSig2, pModule1, pModule2, pSubst1, pSubst2) == FALSE)
                return(FALSE);
        }

        // If we didn't consume all of the callee signature, then we failed.
        if (i <= ArgCount2)
            return(FALSE);

        return(TRUE);
    }

    // do return type as well
    for (i = 0; i <= ArgCount1; i++)
    {
        if (CompareElementType(pSig1, pSig2, pEndSig1, pEndSig2, pModule1, pModule2, pSubst1, pSubst2) == FALSE)
            return(FALSE);
    }

    return(TRUE);
}

/*static*/
BOOL MetaSig::CompareFieldSigs(
    PCCOR_SIGNATURE pSignature1,
    DWORD       cSig1,
    Module*     pModule1,
    PCCOR_SIGNATURE pSignature2,
    DWORD       cSig2,
    Module*     pModule2
)
{
    WRAPPER_CONTRACT;

    PCCOR_SIGNATURE pSig1 = pSignature1;
    PCCOR_SIGNATURE pSig2 = pSignature2;
    PCCOR_SIGNATURE pEndSig1;
    PCCOR_SIGNATURE pEndSig2;


    if (*pSig1 != *pSig2)
        return(FALSE); // calling convention, must be IMAGE_CEE_CS_CALLCONV_FIELD

    pEndSig1 = pSig1 + cSig1;
    pEndSig2 = pSig2 + cSig2;

    return(CompareElementType(++pSig1, ++pSig2, pEndSig1, pEndSig2, pModule1, pModule2, NULL, NULL));
}

#ifndef DACCESS_COMPILE

/* static */
BOOL MetaSig::CompareElementTypeToToken(
    PCCOR_SIGNATURE &pSig1,
    PCCOR_SIGNATURE pEndSig1, // end of sig1
    mdToken         tk2,
    Module*         pModule1,
    Module*         pModule2,
    const Substitution*   pSubst1,
    SigPointer *pMethodInst1 //not used by any caller so far...
)
{
    CONTRACTL
    {
        THROWS;
        GC_TRIGGERS;
        INJECT_FAULT(COMPlusThrowOM());
        MODE_ANY;
    }
    CONTRACTL_END

    _ASSERTE(TypeFromToken(tk2) == mdtTypeDef || 
             TypeFromToken(tk2) == mdtTypeRef);

    CorElementType Type1;

    if (pSig1 >= pEndSig1)
        return(FALSE); // end of sig encountered prematurely

    if (*pSig1 == ELEMENT_TYPE_VAR && pSubst1 != NULL && pSubst1->GetInstSig() != NULL)
      {
        SigPointer inst = SigPointer(pSubst1->GetInstSig());
        pSig1++;
        DWORD index = CorSigUncompressData(pSig1);
        for (DWORD i = 0; i < index; i++) inst.SkipExactlyOne();
        PCCOR_SIGNATURE pSig3 = inst.GetPtr();
        inst.SkipExactlyOne();
        PCCOR_SIGNATURE pEndSig3 = inst.GetPtr();
        return(CompareElementTypeToToken(pSig3, pEndSig3, tk2, pSubst1->GetModule(), pModule2, pSubst1->GetNext(), NULL));
      }

    if (*pSig1 == ELEMENT_TYPE_MVAR && pMethodInst1 != NULL)
      {
        SigPointer inst = *pMethodInst1;
        pSig1++;
        DWORD index = CorSigUncompressData(pSig1);
        for (DWORD i = 0; i < index; i++) inst.SkipExactlyOne();
        PCCOR_SIGNATURE pSig3 = inst.GetPtr();
        inst.SkipExactlyOne();
        PCCOR_SIGNATURE pEndSig3 = inst.GetPtr();
        return(CompareElementTypeToToken(pSig3, pEndSig3, tk2, pModule1, pModule2, NULL, NULL));
      }

    Type1 = CorSigUncompressElementType(pSig1);
    _ASSERTE(!(Type1 == ELEMENT_TYPE_INTERNAL));

    BinderClassID id = CLASS__NIL;
  
    switch (Type1)
    {
        default:
        {
            // Unknown type!
            THROW_BAD_FORMAT(BFA_BAD_COMPLUS_SIG, pModule1);
        }

        case ELEMENT_TYPE_U:
        {
            id = CLASS__UINTPTR; 
            break;           
        }
        case ELEMENT_TYPE_I:
        {
            id = CLASS__INTPTR; 
            break;           
        }
        case ELEMENT_TYPE_VOID:
        {
            id = CLASS__VOID;
            break;           
        }
        case ELEMENT_TYPE_I1:
        {
            id = CLASS__SBYTE;
            break;           
        }
        case ELEMENT_TYPE_U1:
        {
            id = CLASS__BYTE;
            break;           
        }
        case ELEMENT_TYPE_I2:
        {
            id = CLASS__INT16;
            break;           
        }
        case ELEMENT_TYPE_U2:
        {
            id = CLASS__UINT16;
            break;           
        }
        case ELEMENT_TYPE_I4:
        {
            id = CLASS__INT32;
            break;           
        }
        case ELEMENT_TYPE_U4:
        {
            id = CLASS__UINT32;
            break;           
        }
        case ELEMENT_TYPE_I8:
        {
            id = CLASS__INT64;
            break;           
        }
        case ELEMENT_TYPE_U8:
        {
            id = CLASS__UINT64;
            break;           
        }
        case ELEMENT_TYPE_R4:
        {   
            id = CLASS__SINGLE;
            break;
        }
        case ELEMENT_TYPE_R8:
        {   
            id = CLASS__DOUBLE;
            break;
        }
        case ELEMENT_TYPE_BOOLEAN:
        {    
            id = CLASS__BOOLEAN;
            break;
        }
        case ELEMENT_TYPE_CHAR:
        {   
            id = CLASS__CHAR;
            break;
        }
        case ELEMENT_TYPE_TYPEDBYREF:
        {    
            id = CLASS__TYPED_REFERENCE;
            break;
        }
        case ELEMENT_TYPE_STRING:
        {   
            id = CLASS__STRING;
            break;
        }
        case ELEMENT_TYPE_OBJECT:
        {
            id = CLASS__OBJECT;
            break;
        }

        case ELEMENT_TYPE_VAR:
        case ELEMENT_TYPE_MVAR:
        {
           return(FALSE);
        }
        case ELEMENT_TYPE_CMOD_REQD:
        case ELEMENT_TYPE_CMOD_OPT:
        {
            return(FALSE);
        }
        // These take an additional argument, which is the element type
        case ELEMENT_TYPE_SZARRAY:
        case ELEMENT_TYPE_PTR:
        case ELEMENT_TYPE_BYREF:
        {
           return(FALSE);
        }
        case ELEMENT_TYPE_VALUETYPE:
        case ELEMENT_TYPE_CLASS:
        {
            mdToken      tk1;

            pSig1 += CorSigUncompressToken(pSig1, &tk1);

            return(CompareTypeTokens(tk1, tk2, pModule1, pModule2));
        }
        case ELEMENT_TYPE_FNPTR:
        {
            return(FALSE);
        }
        case ELEMENT_TYPE_GENERICINST:
        {
            return(FALSE);
        }
        case ELEMENT_TYPE_ARRAY:
        {
            return(FALSE);
        }
        case ELEMENT_TYPE_INTERNAL:
        {
            return(FALSE);
        }
    }

    _ASSERTE(id != CLASS__NIL);
    return CompareTypeTokens(g_Mscorlib.GetTypeDef(id),
                             tk2,
                             g_Mscorlib.GetModule(),
                             pModule2);
}

/* static */
BOOL MetaSig::CompareTypeSpecToToken(mdTypeSpec tk1,
                            mdToken tk2,
                            Module *pModule1,
                            Module *pModule2,
                            const Substitution *pSubst1,
                            SigPointer *pMethodInst1)
{
    WRAPPER_CONTRACT;

    _ASSERTE(TypeFromToken(tk1) == mdtTypeSpec);
    _ASSERTE(TypeFromToken(tk2) == mdtTypeDef || 
             TypeFromToken(tk2) == mdtTypeRef);
    IMDInternalImport *pInternalImport = pModule1->GetMDImport();
    PCCOR_SIGNATURE pSig1;
    ULONG cSig1;
    pInternalImport->GetTypeSpecFromToken(tk1, &pSig1, &cSig1);
    return CompareElementTypeToToken(pSig1,pSig1+cSig1,tk2,pModule1,pModule2,pSubst1,pMethodInst1);
}



/* static */
BOOL MetaSig::CompareTypeDefOrRefOrSpec(Module *pModule1, mdToken tok1,
                                        const Substitution *pSubst1,
                                        Module *pModule2, mdToken tok2,
                                        const Substitution *pSubst2)
{

    if (TypeFromToken(tok1) != mdtTypeSpec && TypeFromToken(tok2) != mdtTypeSpec)
    {
        _ASSERTE(TypeFromToken(tok1) == mdtTypeDef || TypeFromToken(tok1) == mdtTypeRef);
        _ASSERTE(TypeFromToken(tok2) == mdtTypeDef || TypeFromToken(tok2) == mdtTypeRef);
        return CompareTypeTokens(tok1,tok2,pModule1,pModule2);
    }
    
    if (TypeFromToken(tok1) != TypeFromToken(tok2)) 
    {   if (TypeFromToken(tok1) == mdtTypeSpec)
            return CompareTypeSpecToToken(tok1,tok2,pModule1,pModule2,pSubst1,NULL);
        else
        {   
            _ASSERTE(TypeFromToken(tok2) == mdtTypeSpec);
            return CompareTypeSpecToToken(tok2,tok1,pModule2,pModule1,pSubst2,NULL);
        }
    }

    _ASSERTE(TypeFromToken(tok1) == mdtTypeSpec &&
             TypeFromToken(tok2) == mdtTypeSpec);

    IMDInternalImport *pInternalImport1 = pModule1->GetMDImport();
    IMDInternalImport *pInternalImport2 = pModule2->GetMDImport();

    PCCOR_SIGNATURE pSig1,pSig2;
    ULONG cSig1,cSig2;
    pInternalImport1->GetTypeSpecFromToken(tok1, &pSig1, &cSig1);
    pInternalImport2->GetTypeSpecFromToken(tok2, &pSig2, &cSig2);
    return MetaSig::CompareElementType(pSig1,pSig2,pSig1+cSig1,pSig2+cSig2,pModule1,pModule2,pSubst1,pSubst2,NULL,NULL);
}

/* static */
BOOL MetaSig::CompareVariableConstraints(Module *pModule1, mdGenericParam tok1, //overriding
                                         const Substitution *pSubst2,
                                         Module *pModule2, mdGenericParam tok2) //overridden
{
    CONTRACTL
    {
        THROWS;
        GC_TRIGGERS;
        INJECT_FAULT(COMPlusThrowOM());
        MODE_ANY;
    }
    CONTRACTL_END

    IMDInternalImport *pInternalImport1 = pModule1->GetMDImport();
    IMDInternalImport *pInternalImport2 = pModule2->GetMDImport();

    DWORD specialConstraints1,specialConstraints2;

     // check special constraints 
    {
          pInternalImport1->GetGenericParamProps(tok1, NULL, &specialConstraints1, NULL, NULL, NULL);
          pInternalImport2->GetGenericParamProps(tok2, NULL, &specialConstraints2, NULL, NULL, NULL);
          specialConstraints1 = specialConstraints1 & gpSpecialConstraintMask;
          specialConstraints2 = specialConstraints2 & gpSpecialConstraintMask;

          if ((specialConstraints1 & gpNotNullableValueTypeConstraint) != 0)
          { 
              if ((specialConstraints2 & gpNotNullableValueTypeConstraint) == 0)
                return FALSE;
          }
          if ((specialConstraints1 & gpReferenceTypeConstraint) != 0)
          {
              if ((specialConstraints2 & gpReferenceTypeConstraint) == 0)
                return FALSE;
          }
      if ((specialConstraints1 & gpDefaultConstructorConstraint) != 0)
          {
              if ((specialConstraints2 & (gpDefaultConstructorConstraint | gpNotNullableValueTypeConstraint)) == 0)
                    return FALSE;
          }
    }


    HENUMInternalHolder hEnum1(pInternalImport1);
    mdGenericParamConstraint tkConstraint1;
    hEnum1.EnumInit(mdtGenericParamConstraint, tok1);

    while (pInternalImport1->EnumNext(&hEnum1, &tkConstraint1))
    {
        mdToken tkConstraintType1, tkParam1;
        pInternalImport1->GetGenericParamConstraintProps(tkConstraint1, &tkParam1, &tkConstraintType1);
        _ASSERTE(tkParam1 == tok1);

        // for each non-object constraint,
    // and, in the case of a notNullableValueType, each non-ValueType constraint,
        // find an equivalent constraint on tok2
        // NB: we do not attempt to match constraints equivalent to object (and ValueType when tok1 is notNullable)
        // because they
        // a) are vacuous, and 
        // b) may be implicit (ie. absent) in the overriden variable's declaration
        if (!(CompareTypeDefOrRefOrSpec(pModule1, tkConstraintType1, NULL, 
                                       g_Mscorlib.GetModule(), g_Mscorlib.GetTypeDef(CLASS__OBJECT), NULL)
          ||
          ((specialConstraints1 & gpNotNullableValueTypeConstraint) != 0) && 
           (CompareTypeDefOrRefOrSpec(pModule1, tkConstraintType1, NULL, 
                      g_Mscorlib.GetModule(), g_Mscorlib.GetTypeDef(CLASS__VALUE_TYPE), NULL))))
        
        {
            HENUMInternalHolder hEnum2(pInternalImport2);
            mdGenericParamConstraint tkConstraint2;
            hEnum2.EnumInit(mdtGenericParamConstraint, tok2);
        
            BOOL found = FALSE;
            while (!found && pInternalImport2->EnumNext(&hEnum2, &tkConstraint2) )
            {
                mdToken tkConstraintType2, tkParam2;
                pInternalImport2->GetGenericParamConstraintProps(tkConstraint2, &tkParam2, &tkConstraintType2);
                _ASSERTE(tkParam2 == tok2);
            
                found = CompareTypeDefOrRefOrSpec(pModule1, tkConstraintType1, NULL, pModule2, tkConstraintType2, pSubst2);
            }
            if (!found) 
            {
                //none of the constrains on tyvar2 match, exit early
                return FALSE; 
            }
        }
        //check next constraint of tok1
    }
       
    return TRUE;
}

/* static */
BOOL MetaSig::CompareMethodConstraints(Module *pModule1, 
                                       mdMethodDef tok1, //implementation
                                       const Substitution *pSubst2,
                                       Module *pModule2,
                                       mdMethodDef tok2) //declaration w.r.t subsitution
{
    CONTRACTL
    {
        THROWS;
        GC_TRIGGERS;
        INJECT_FAULT(COMPlusThrowOM());
        MODE_ANY;
    }
    CONTRACTL_END

    IMDInternalImport *pInternalImport1 = pModule1->GetMDImport();
    IMDInternalImport *pInternalImport2 = pModule2->GetMDImport();

    HENUMInternalHolder hEnumTyPars1(pInternalImport1); 
    HENUMInternalHolder hEnumTyPars2(pInternalImport2); 

    hEnumTyPars1.EnumInit(mdtGenericParam, tok1);
    hEnumTyPars2.EnumInit(mdtGenericParam, tok2);

    mdGenericParam    tkTyPar1,tkTyPar2;       

    // enumerate the variables
    DWORD numTyPars1 = pInternalImport1->EnumGetCount(&hEnumTyPars1);
    DWORD numTyPars2 = pInternalImport2->EnumGetCount(&hEnumTyPars2);

    _ASSERTE(numTyPars1 == numTyPars2);
    if (numTyPars1 != numTyPars2) //play it safe
        return FALSE; //throw bad format exception?

    for(unsigned int i = 0; i < numTyPars1; i++)
    { 
        pInternalImport1->EnumNext(&hEnumTyPars1, &tkTyPar1);
        pInternalImport2->EnumNext(&hEnumTyPars2, &tkTyPar2);
        if (!CompareVariableConstraints(pModule1,tkTyPar1,pSubst2,pModule2,tkTyPar2))
        {
           return FALSE; 
        }
    }     
    return TRUE;
}

#endif // #ifndef DACCESS_COMPILE

#ifndef DACCESS_COMPILE

//------------------------------------------------------------------
// Resolve type references in the hardcoded metasig.
// Returns a new signature with type refences resolved.
//------------------------------------------------------------------
static ULONG CorSigResolveSig(const BYTE* pSig, BYTE *pBuffer, BYTE *pMax)
{
    CONTRACTL
    {
        THROWS;
        GC_TRIGGERS;
        INJECT_FAULT(COMPlusThrowOM());
        MODE_ANY;
        PRECONDITION(CheckPointer(pSig));
        PRECONDITION(CheckPointer(pBuffer));
        PRECONDITION(CheckPointer(pMax));
    }
    CONTRACTL_END

    BYTE* pEnd = pBuffer;
    unsigned argCount;
    unsigned callConv;
    INDEBUG(bool bSomethingResolved = false;)

    // calling convention
    callConv = *pSig++;
    pEnd += CorSigCompressDataSafe(callConv, pEnd, pMax);

    if ((callConv & IMAGE_CEE_CS_CALLCONV_MASK) == IMAGE_CEE_CS_CALLCONV_DEFAULT) {
        // arg count
        argCount = *pSig++;
        pEnd += CorSigCompressDataSafe(argCount, pEnd, pMax);
    }
    else {
        if ((callConv & IMAGE_CEE_CS_CALLCONV_MASK) != IMAGE_CEE_CS_CALLCONV_FIELD)
            THROW_BAD_FORMAT(BFA_BAD_SIGNATURE, (Module*)NULL);
        argCount = 0;
    }

    // <= because we want to include the return value or the field
    for (unsigned i = 0; i <= argCount; i++) {

        for (;;) {
            BinderClassID id = CLASS__NIL;
            bool again = false;

            CorElementType type = (CorElementType)*pSig++;

            switch (type)
            {
            case ELEMENT_TYPE_BYREF:
            case ELEMENT_TYPE_PTR:
            case ELEMENT_TYPE_SZARRAY:
                again = true;
                break;

            case ELEMENT_TYPE_CLASS:
            case ELEMENT_TYPE_VALUETYPE:
                id = (BinderClassID)*pSig++;
                break;

            case ELEMENT_TYPE_VOID:
                if (i != 0) {
                    if (pSig[-2] != ELEMENT_TYPE_PTR)
                        THROW_BAD_FORMAT(BFA_ONLY_VOID_PTR_IN_ARGS, (Module*)NULL); // only pointer to void allowed in arguments
                }
                break;

            default:
                break;
            }

            pEnd += CorSigCompressElementTypeSafe(type, pEnd, pMax);

            if (id != CLASS__NIL)
            {
                pEnd += CorSigCompressTokenSafe(g_Mscorlib.GetTypeDef(id), pEnd, pMax);

                // Make sure we've loaded the type.  This is to prevent the situation where
                // a metasig's signature is describing a value type/enum argument on the stack
                // during gc, but that type has not been loaded yet.

                g_Mscorlib.FetchClass(id);

                INDEBUG(bSomethingResolved = true;)
            }

            if (!again)
                break;
        }
    }

    _ASSERTE(bSomethingResolved);
    return( (ULONG)(pEnd - pBuffer) );
}

#endif // #ifndef DACCESS_COMPILE

// Get the metasig, do a one-time conversion if necessary
void HardCodedMetaSig::GetBinarySig(PCCOR_SIGNATURE *ppBinarySig, ULONG *pcbBinarySigLength) const
{
    CONTRACTL
    {
        INSTANCE_CHECK;
        THROWS;
        GC_TRIGGERS;
        INJECT_FAULT(COMPlusThrowOM());
        MODE_ANY;
    }
    CONTRACTL_END



// Make sure all HardCodedMetaSig's are global. Because there is no individual
// cleanup of converted binary sigs, using allocated HardCodedMetaSig's
// can lead to a quiet memory leak.
#ifdef _DEBUG_IMPL

    if (! (0
#define METASIG_BODY(varname, types)    || this==&gsig_ ## varname
#include "metasig.h"
    ))
    {
        _ASSERTE(!"The HardCodedMetaSig struct can only be declared as a global in metasig.h.");
    }
#endif

    const BYTE* pMetaSig = m_pMetaSig;

    // To minimize code and data size, the hardcoded metasigs are baked as much as possible
    // at compile time. Only the signatures with type references require one-time conversion at runtime.

    // the negative size means signature with unresolved type references
    if ((INT8)*PTR_BYTE((TADDR)pMetaSig) < 0)
    {
#ifndef DACCESS_COMPILE
        ULONG cbCount;
        CQuickBytes cqb;

        cqb.Maximize();

        cbCount = CorSigResolveSig(pMetaSig+1,
                                         (BYTE*) cqb.Ptr(), (BYTE*) cqb.Ptr() + cqb.Size());
        if (cbCount > cqb.Size())
        {
            cqb.ReSizeThrows(cbCount);

            cbCount = CorSigResolveSig(pMetaSig+1,
                                             (BYTE*) cqb.Ptr(), (BYTE*) cqb.Ptr() + cqb.Size());

            _ASSERTE(cbCount <= cqb.Size());
        }

        {
            CrstHolder ch(&m_Crst);

            if (*(INT8*)m_pMetaSig < 0) {

                BYTE* pResolved = (BYTE*)(void*)(SystemDomain::System()->GetHighFrequencyHeap()->AllocMem(1 + cbCount));

#ifdef _DEBUG
                SystemDomain::Loader()->m_dwDebugConvertedSigSize += cbCount;
#endif

                *(INT8*)pResolved = cbCount;
                CopyMemory(pResolved+1, cqb.Ptr(), cbCount);

                // this has to be last, overwrite the pointer to the metasig with the resolved one
                ((HardCodedMetaSig*)this)->m_pMetaSig = pResolved;
            }
        }

        pMetaSig = m_pMetaSig;
#else
        DacNotImpl();
        return;
#endif
    }

    // The metasig has to be resolved at this point
    _ASSERTE((INT8)*PTR_BYTE((TADDR)pMetaSig) > 0);

#ifdef DACCESS_COMPILE
    *ppBinarySig = (PCCOR_SIGNATURE)
        DacInstantiateTypeByAddress((TADDR)pMetaSig + 1,
                                    *PTR_BYTE((TADDR)pMetaSig),
                                    true);
#else
    *ppBinarySig        = pMetaSig+1;
#endif
    *pcbBinarySigLength = (INT8)*PTR_BYTE((TADDR)pMetaSig);
}


// This always returns MSCORLIB's Module
Module* HardCodedMetaSig::GetModule() const
{
    WRAPPER_CONTRACT;

    _ASSERTE(SystemDomain::SystemModule());
    return SystemDomain::SystemModule();
}


//=========================================================================
// Indicates whether an argument is to be put in a register using the
// default IL calling convention. This should be called on each parameter
// in the order it appears in the call signature. For a non-static method,
// this function should also be called once for the "this" argument, prior
// to calling it for the "real" arguments. Pass in a typ of ELEMENT_TYPE_CLASS.
//
//  *pNumRegistersUsed:  [in,out]: keeps track of the number of argument
//                       registers assigned previously. The caller should
//                       initialize this variable to 0 - then each call
//                       will update it.
//
//  typ:                 the signature type
//  structSize:          for structs, the size in bytes
//  fThis:               is this about the "this" pointer?
//  callconv:            see IMAGE_CEE_CS_CALLCONV_*
//  *pOffsetIntoArgumentRegisters:
//                       If this function returns TRUE, then this out variable
//                       receives the identity of the register, expressed as a
//                       byte offset into the ArgumentRegisters structure.
//
// On IA64, a value type larger than 64-bit can occupy multiple registers and can be split
//    between registers and the memory stack if it doesn't fit in the 8 input registers.
//    To preserve the IsArgumentInRegister and ArgIterator abstraction, we put the enregistered arguments and
//    the stacked arguments in two contiguous areas in the FramedMethodFrame.
//    IsArgumentInRegister always returns false, so that all arguments are accessed as if they are on the stack.
//    See the comments above the definition of FramedMethodFrame for further details.
//
//=========================================================================
BOOL IsArgumentInRegister(int   *pNumRegistersUsed,
                          BYTE   typ,
                          UINT32 structSize,
                          BOOL   fThis,
                          BYTE   callconv,
                          int   *pOffsetIntoArgumentRegisters)
{
    CONTRACTL
    {
        NOTHROW;
        GC_NOTRIGGER;
        FORBID_FAULT;
        MODE_ANY;
    }
    CONTRACTL_END

#if defined(_X86_)
    if ( (*pNumRegistersUsed) < NUM_ARGUMENT_REGISTERS && (callconv != IMAGE_CEE_CS_CALLCONV_VARARG || fThis) ) {
        if (gElementTypeInfo[typ].m_enregister) {
            int registerIndex = (*pNumRegistersUsed)++;
            if (pOffsetIntoArgumentRegisters != NULL)
                *pOffsetIntoArgumentRegisters = sizeof(ArgumentRegisters) - sizeof(UINT_PTR)*(1+registerIndex);
            return(TRUE);
        }
    }
#endif

    return(FALSE);
}

void HandleInstParam(MetaSig& msig, int& numregsused, UINT& cb, BYTE callconv, int* paramTypeReg, UINT32* nNumVirtualFixedArgs)
{
    CONTRACTL
    {
        NOTHROW;
        GC_NOTRIGGER;
        FORBID_FAULT;
        MODE_ANY;
    }
    CONTRACTL_END
        
        // Parameterized type passed as secret parameter, but not mentioned in the sig
    if (msig.GetCallingConventionInfo() & CORINFO_CALLCONV_PARAMTYPE)
    {
        if (nNumVirtualFixedArgs)
        {
            (*nNumVirtualFixedArgs)++;
        }
        
        if (!IsArgumentInRegister(&numregsused, ELEMENT_TYPE_I, sizeof(void*), FALSE, callconv, NULL))
        {
            if (paramTypeReg)
            {
                *paramTypeReg = -1;
            }
            cb += sizeof(void*);
        }
        else if (paramTypeReg)
        {
            *paramTypeReg = numregsused-1;
        }
    }
}

#ifndef DACCESS_COMPILE

//------------------------------------------------------------------
// Perform type-specific GC promotion on the value (based upon the
// last type retrieved by NextArg()).
//------------------------------------------------------------------
VOID MetaSig::GcScanRoots(LPVOID pValue,
                          promote_func *fn,
                          ScanContext* sc,
                          promote_carefully_func *fnc)
{

    CONTRACTL
    {
        INSTANCE_CHECK;
        if (FORBIDGC_LOADER_USE_ENABLED()) NOTHROW; else THROWS;
        if (FORBIDGC_LOADER_USE_ENABLED()) GC_NOTRIGGER; else GC_TRIGGERS;
        if (FORBIDGC_LOADER_USE_ENABLED()) FORBID_FAULT; else { INJECT_FAULT(COMPlusThrowOM()); }
        MODE_ANY;
    }
    CONTRACTL_END


    Object **pArgPtr = (Object**)pValue;
    if (fnc == NULL)
        fnc = &PromoteCarefully;

    int  etype = m_pLastType.PeekElemTypeNormalized(m_pModule, &m_typeContext);

    _ASSERTE(etype >= 0 && etype < ELEMENT_TYPE_MAX);
    
    switch (gElementTypeInfo[etype].m_gc)
    {
#ifdef _DEBUG
        Object *pOldLocation;
#endif
        case TYPE_GC_NONE:
            // do nothing
            break;

        case TYPE_GC_REF:
            LOG((LF_GC, INFO3,
                 "        Argument at" FMT_ADDR "causes promotion of " FMT_OBJECT "\n",
                 DBG_ADDR(pArgPtr), DBG_ADDR(*pArgPtr) ));
#ifdef _DEBUG
            pOldLocation = *pArgPtr;
#endif
            (*fn)( *pArgPtr, sc, GC_CALL_CHECK_APP_DOMAIN );

            // !!! Do not cast to (OBJECTREF*)
            // !!! If we are in the relocate phase, we may have updated root,
            // !!! but we have not moved the GC heap yet.
            // !!! The root then points to bad locations until GC is done.
#ifdef LOGGING
            if (pOldLocation != *pArgPtr)
                LOG((LF_GC, INFO3,
                     "        Relocating from" FMT_ADDR "to " FMT_ADDR "\n",
                     DBG_ADDR(pOldLocation), DBG_ADDR(*pArgPtr)));
#endif
            break;

        case TYPE_GC_BYREF:
#ifdef ENREGISTERED_PARAMTYPE_MAXSIZE
        case_TYPE_GC_BYREF:
#endif // ENREGISTERED_PARAMTYPE_MAXSIZE

            // value is an interior pointer
            LOG((LF_GC, INFO3,
                 "        Argument at" FMT_ADDR "causes promotion of interior pointer" FMT_ADDR "\n",
                 DBG_ADDR(pArgPtr), DBG_ADDR(*pArgPtr) ));

            if (etype == ELEMENT_TYPE_TYPEDBYREF) 
            {
            }

#ifdef _DEBUG
            pOldLocation = *pArgPtr;
#endif

            (*fnc)(fn, *pArgPtr, sc, GC_CALL_INTERIOR|GC_CALL_CHECK_APP_DOMAIN);

            // !!! Do not cast to (OBJECTREF*)
            // !!! If we are in the relocate phase, we may have updated root,
            // !!! but we have not moved the GC heap yet.
            // !!! The root then points to bad locations until GC is done.
#ifdef LOGGING
            if (pOldLocation != *pArgPtr)
                LOG((LF_GC, INFO3,
                     "        Relocating from" FMT_ADDR "to " FMT_ADDR "\n",
                     DBG_ADDR(pOldLocation), DBG_ADDR(*pArgPtr)));
#endif
            break;

        case TYPE_GC_OTHER:
            // value is a ValueClass, generic type parameter 
            // See one of the go_through_object() macros in
            // gc.cpp for the code we are emulating here.  But note that the GCDesc
            // for value classes describes the state of the instance in its boxed
            // state.  Here we are dealing with an unboxed instance, so we must adjust
            // the object size and series offsets appropriately.
            _ASSERTE(etype == ELEMENT_TYPE_VALUETYPE);
            {
                // == FailIfNotLoaded
                TypeHandle th = GetLastTypeHandleThrowing(ClassLoader::DontLoadTypes);

                MethodTable *pMT = th.AsMethodTable();


#ifdef ENREGISTERED_PARAMTYPE_MAXSIZE
                if (MetaSig::IsArgPassedByRef(pMT->GetNumInstanceFieldBytes()))
                {
                    goto case_TYPE_GC_BYREF;
                }
#endif // ENREGISTERED_PARAMTYPE_MAXSIZE

                if (pMT->ContainsPointers())
                {
                    BYTE        *obj = (BYTE *) pArgPtr;


                    // size of instance when unboxed must be adjusted for the syncblock
                    // index and the VTable pointer.
                    DWORD       size = pMT->GetBaseSize();

                    // we don't include this term in our 'ppstop' calculation below.
                    _ASSERTE(pMT->GetComponentSize() == 0);

                    CGCDesc* map = CGCDesc::GetCGCDescFromMT(pMT);
                    CGCDescSeries* cur = map->GetHighestSeries();
                    CGCDescSeries* last = map->GetLowestSeries();

                    _ASSERTE(cur >= last);
                    do
                    {
                        // offset to embedded references in this series must be
                        // adjusted by the VTable pointer, when in the unboxed state.
                        size_t   adjustOffset = cur->GetSeriesOffset() - sizeof(void *);

                        Object** parm = (Object**)(obj + adjustOffset);
                        BYTE** ppstop =
                            (BYTE**)((BYTE*)parm + cur->GetSeriesSize() + size);
                        while ((BYTE **) parm < ppstop)
                        {

                            (*fn)(*parm, sc, GC_CALL_CHECK_APP_DOMAIN);

                            (*(BYTE ***) &parm)++;
                        }
                        cur--;

                    } while (cur >= last);

                }
            }
            break;

        default:
            _ASSERTE(0); // can't get here.
    }
}

//------------------------------------------------------------------------
// Returns # of stack bytes required to create a call-stack using
// the internal calling convention.
// Includes indication of "this" pointer since that's not reflected
// in the sig.
//------------------------------------------------------------------------
/*static*/
UINT MetaSig::SizeOfVirtualFixedArgStack(Module* pModule, 
                                         PCCOR_SIGNATURE szMetaSig, 
                                         DWORD cbMetaSigSize,
                                         BOOL fIsStatic, 
                                         const SigTypeContext *pTypeContext)
{
    CONTRACTL
    {
        if (FORBIDGC_LOADER_USE_ENABLED()) NOTHROW; else THROWS;
        if (FORBIDGC_LOADER_USE_ENABLED()) GC_NOTRIGGER; else GC_TRIGGERS;
        if (FORBIDGC_LOADER_USE_ENABLED()) FORBID_FAULT; else { INJECT_FAULT(COMPlusThrowOM()); }
        MODE_ANY;
    }
    CONTRACTL_END

    UINT cb = 0;
    MetaSig msig(szMetaSig, cbMetaSigSize, pModule, pTypeContext);

    if (!fIsStatic)
        cb += StackElemSize(sizeof(OBJECTREF));
    if (msig.HasRetBuffArg())
        cb += StackElemSize(sizeof(OBJECTREF));

    while (ELEMENT_TYPE_END != msig.NextArg())
        cb += StackElemSize(msig.GetArgProps().SizeOf(pModule, pTypeContext));

    return(cb);
}

//------------------------------------------------------------------------
// Returns # of stack bytes required to create a call-stack using
// the actual calling convention.
// Includes indication of "this" pointer since that's not reflected
// in the sig.
//------------------------------------------------------------------------
/*static*/
UINT MetaSig::SizeOfActualFixedArgStack(Module *pModule, 
                                        PCCOR_SIGNATURE szMetaSig, 
                                        DWORD cbMetaSigSize,
                                        BOOL fIsStatic, 
                                        const SigTypeContext *pTypeContext, 
                                        BOOL fParamTypeArg, 
                                        int* paramTypeReg)
{
    CONTRACTL
    {
        THROWS;
        GC_TRIGGERS;
        INJECT_FAULT(COMPlusThrowOM());
        MODE_ANY;
    }
    CONTRACTL_END

    UINT cb = 0;
    MetaSig msig(szMetaSig, cbMetaSigSize, pModule, pTypeContext, FALSE, sigMember, fParamTypeArg);
    int numregsused = 0;
    BOOL fIsVarArg = msig.IsVarArg();
    BYTE callconv  = msig.GetCallingConvention();

    if (!fIsStatic) 
    {
        if (!IsArgumentInRegister(&numregsused, ELEMENT_TYPE_CLASS, sizeof(void*), TRUE, callconv, NULL))
        {
            cb += StackElemSize(sizeof(OBJECTREF));
        }
    }

    if (msig.HasRetBuffArg())
    {
#if defined(_PPC_) // retbuf
        cb += StackElemSize(sizeof(OBJECTREF));
#elif !defined(RETBUF_ARG_SPECIAL_PARAM) // Return buffer is not in a normal parameter register
        numregsused++;
#endif
    }

    // JIT64 puts inst param after this/retbuffarg and before vararg cookie
    WIN64_ONLY(HandleInstParam(msig, numregsused, cb, callconv, paramTypeReg, NULL));

    if (fIsVarArg || msig.IsTreatAsVarArg())
    {
        // No other params in registers
        numregsused = NUM_ARGUMENT_REGISTERS;   

        if (!IsArgumentInRegister(&numregsused, ELEMENT_TYPE_I, sizeof(void*), TRUE, callconv, NULL))
        {
            cb += StackElemSize(sizeof(LPVOID));    // VASigCookie
        }
    }

    CorElementType mtype;
    while (ELEMENT_TYPE_END != (mtype = msig.NextArgNormalized()))
    {
        UINT cbSize = msig.GetLastTypeSize();
        UINT stackSize = StackElemSize(cbSize);

        if (!IsArgumentInRegister(&numregsused, mtype, cbSize, FALSE, callconv, NULL))
        {
#ifdef ENREGISTERED_PARAMTYPE_MAXSIZE
            if (MetaSig::IsArgPassedByRef(stackSize))
            {
                stackSize = sizeof(void*);
            }
#endif
            cb += stackSize;
        }
    }

    NOT_WIN64(HandleInstParam(msig, numregsused, cb, callconv, paramTypeReg, NULL));
    
    return(cb);
}

#endif // #ifndef DACCESS_COMPILE


//
void MetaSig::ForceSigWalk(BOOL fIsStatic)
{
    CONTRACTL
    {
        INSTANCE_CHECK;
        if (FORBIDGC_LOADER_USE_ENABLED()) NOTHROW; else THROWS;
        if (FORBIDGC_LOADER_USE_ENABLED()) GC_NOTRIGGER; else GC_TRIGGERS;
        if (FORBIDGC_LOADER_USE_ENABLED()) FORBID_FAULT; else { INJECT_FAULT(COMPlusThrowOM()); }
        MODE_ANY;
    }
    CONTRACTL_END


    BOOL fVarArg = IsVarArg();
    BYTE callconv = GetCallingConvention();

    // We must use temporaries rather than members here.  That's because the decision
    // of whether to Force a SigWalk is based on a member being -1.  If the last thing
    // we do is post to that member, then multiple threads won't read partially complete
    // signature state.  (Of course, this mechanism depends on the fact that ForceSigWalk
    // can be called multiple times without change.
    //
    // Normally MetaSig isn't supposed to be thread-safe anyway.  For example, the
    // iterator is held inside the MetaSig rather than outside.  
    //
    // There are 2 values to compute:
    // - size of actual stack space used, per architecture calling convention
    // - total number of arguments, including implicit arguments such as
    //   'this', varargs cookie, instantiation parameter, etc.  This determines
    //   the length of the ARG_SLOT array to pass to MethodDesc::CallDescr.

    UINT32  tmp_nActualStack = 0;
    UINT32  tmp_nNumVirtualFixedArgs = 0;

    int numregsused = 0;

    SigPointer p = m_pStart;

    if (fVarArg || IsTreatAsVarArg()) {
        tmp_nActualStack += StackElemSize(sizeof(LPVOID));
        tmp_nNumVirtualFixedArgs++;
    }

    if (!fIsStatic) {
        tmp_nNumVirtualFixedArgs++;
        if (!IsArgumentInRegister(&numregsused, ELEMENT_TYPE_CLASS, sizeof(void*), TRUE, callconv, NULL))
            tmp_nActualStack += StackElemSize(sizeof(OBJECTREF));
    }
    if (HasRetBuffArg()) {
        tmp_nNumVirtualFixedArgs++;
#if defined(_PPC_) // retbuf
        tmp_nActualStack += StackElemSize(sizeof(LPVOID));
#elif !defined(RETBUF_ARG_SPECIAL_PARAM) // Return buffer is not in a normal parameter register
        numregsused++;
#endif
    }

    WIN64_ONLY(HandleInstParam(*this, numregsused, tmp_nActualStack, callconv, NULL, &tmp_nNumVirtualFixedArgs));

    for (DWORD i=0;i<m_nArgs;i++) {
        CorElementType type = p.PeekElemTypeNormalized(m_pModule, &m_typeContext);
        UINT cbSize = p.SizeOf(m_pModule, &m_typeContext);
        UINT stackSize = cbSize;

#if ENREGISTERED_PARAMTYPE_MAXSIZE
        if (MetaSig::IsArgPassedByRef(cbSize)) {
            stackSize = sizeof(PVOID);
        }
#endif

        if (m_nArgs <= MAX_CACHED_SIG_SIZE)
        {
            m_types[i] = type;
            m_sizes[i] = cbSize;
            // The value of m_offsets is determined by IsArgumentInRegister.
            // We can not initialize it to -1, because it may trash
            // what has been set by another thread.
            int tmp_offsets = -1;
            if (!IsArgumentInRegister(&numregsused, type, stackSize, FALSE, callconv, &tmp_offsets))
                tmp_nActualStack += StackElemSize(stackSize);
            m_offsets[i] = (short)tmp_offsets;
        }
        else
        {
            if (!IsArgumentInRegister(&numregsused, type, stackSize, FALSE, callconv, NULL))
                tmp_nActualStack += StackElemSize(stackSize);
        }
        p.SkipExactlyOne();
    }
    if (m_nArgs <= MAX_CACHED_SIG_SIZE)
    {
        m_types[m_nArgs] = ELEMENT_TYPE_END;
        m_flags |= SIG_OFFSETS_INITTED;
    }

    tmp_nNumVirtualFixedArgs += m_nArgs;

    NOT_WIN64(HandleInstParam(*this, numregsused, tmp_nActualStack, callconv, NULL, &tmp_nNumVirtualFixedArgs));

    _ASSERTE(tmp_nActualStack == StackElemSize(tmp_nActualStack));

    m_nActualStack = tmp_nActualStack;
    m_nNumVirtualFixedArgs = tmp_nNumVirtualFixedArgs;
    m_WalkStatic = fIsStatic;

    // Final post.  This is the trigger for avoiding subsequent calls to ForceSigWalk.
    // See NeedsSigWalk to understand how this achieves thread safety.
    m_nVirtualStack = tmp_nNumVirtualFixedArgs * sizeof(ARG_SLOT);
    GetReturnTypeSize();    // updates m_cbRetType
    CalculateHasFPReturn(); // will SetHasFPReturn if needed
}

void MetaSig::CalculateHasFPReturn()
{
    CONTRACTL
    {
        INSTANCE_CHECK;
        if (FORBIDGC_LOADER_USE_ENABLED()) NOTHROW; else THROWS;
        if (FORBIDGC_LOADER_USE_ENABLED()) GC_NOTRIGGER; else GC_TRIGGERS;
        if (FORBIDGC_LOADER_USE_ENABLED()) FORBID_FAULT; else { INJECT_FAULT(COMPlusThrowOM()); }
        MODE_ANY;
    }
    CONTRACTL_END
        
    CorElementType rt = GetReturnTypeNormalized();
    if (rt == ELEMENT_TYPE_R4 || rt == ELEMENT_TYPE_R8)
    {
        SetHasFPReturn();
    }
    else if (rt == ELEMENT_TYPE_VALUETYPE)
    {
    }

    m_flags |= HAS_FP_RETVAL_INITTED;
}

#ifndef DACCESS_COMPILE

// this walks the sig and checks to see if all  types in the sig can be loaded

// This is used by ComCallableWrapper to give good error reporting
/*static*/
void MetaSig::CheckSigTypesCanBeLoaded(PCCOR_SIGNATURE pSig, Module *pModule, const SigTypeContext *pTypeContext)
{
    CONTRACTL
    {
        THROWS;
        GC_TRIGGERS;
        INJECT_FAULT(COMPlusThrowOM());
        MODE_ANY;
    }
    CONTRACTL_END


    // The signature format is approximately:
    // CallingConvention   NumberOfArguments    ReturnType   Arg1  ...
    // There is also a blob length at pSig-1.
    SigPointer ptr(pSig);

    // Skip over calling convention.
    IfFailThrowBF(ptr.GetCallingConv(NULL), BFA_BAD_SIGNATURE, pModule);

    ULONG numArgs;
    IfFailThrowBF(ptr.GetData(&numArgs), BFA_BAD_SIGNATURE, pModule);

    // must do a skip so we skip any class tokens associated with the return type
    IfFailThrowBF(ptr.SkipExactlyOne(), BFA_BAD_SIGNATURE, pModule);

    // Force a load of value type arguments.

    for(ULONG i=0; i < numArgs; i++)
    {
        unsigned type = ptr.PeekElemTypeNormalized(pModule,pTypeContext);
        if (type == ELEMENT_TYPE_VALUETYPE || type == ELEMENT_TYPE_CLASS)
        {
            ptr.GetTypeHandleThrowing(pModule, pTypeContext);
        }
        // Move to next argument token.
        IfFailThrowBF(ptr.SkipExactlyOne(), BFA_BAD_SIGNATURE, pModule);
    }
}

#endif // #ifndef DACCESS_COMPILE

CorElementType MetaSig::GetReturnTypeNormalized() const
{
    WRAPPER_CONTRACT;

    if (m_flags & SIG_RET_TYPE_INITTED)
        return( m_corNormalizedRetType );
    MetaSig *tempSig = (MetaSig *)this;
    tempSig->m_corNormalizedRetType = m_pRetType.PeekElemTypeNormalized(m_pModule, &m_typeContext);
    tempSig->m_flags |= SIG_RET_TYPE_INITTED;
    return( tempSig->m_corNormalizedRetType );
}

MetaSig::RETURNTYPE MetaSig::GetReturnObjectKind() const
{
    WRAPPER_CONTRACT;

    CorElementType et = GetReturnType();
    switch (et)
    {
        case ELEMENT_TYPE_STRING:
        case ELEMENT_TYPE_CLASS:
        case ELEMENT_TYPE_SZARRAY:
        case ELEMENT_TYPE_ARRAY:
        case ELEMENT_TYPE_OBJECT:
        case ELEMENT_TYPE_VAR:
            return(RETOBJ);

        case ELEMENT_TYPE_BYREF:
            return(RETBYREF);
        default:
            return(RETNONOBJ);
    }
}

BOOL MetaSig::IsObjectRefReturnType()
{
    WRAPPER_CONTRACT;

    switch (GetReturnTypeNormalized())
        {
        case ELEMENT_TYPE_CLASS:
        case ELEMENT_TYPE_SZARRAY:
        case ELEMENT_TYPE_ARRAY:
        case ELEMENT_TYPE_STRING:
        case ELEMENT_TYPE_OBJECT:
        case ELEMENT_TYPE_VAR:
            return( TRUE );
        default:
            break;
        }
    return( FALSE );
}

CorElementType MetaSig::GetReturnType() const
{
    WRAPPER_CONTRACT;
    return m_pRetType.PeekElemTypeClosed(&m_typeContext);
}

BOOL MetaSig::IsReturnTypeVoid() const
{
    WRAPPER_CONTRACT;
    return (GetReturnType() == ELEMENT_TYPE_VOID);
}

#ifndef DACCESS_COMPILE

//----------------------------------------------------------
// Returns the unmanaged calling convention.
//----------------------------------------------------------
/*static*/
BOOL MetaSig::GetUnmanagedCallingConvention(Module *pModule, PCCOR_SIGNATURE pSig, ULONG cSig, CorPinvokeMap *pPinvokeMapOut)
{
    CONTRACTL
    {
        NOTHROW;
        GC_NOTRIGGER;
        FORBID_FAULT;
        MODE_ANY;
    }
    CONTRACTL_END


    // Instantiations aren't relevant here
    MetaSig msig(pSig, cSig, pModule, NULL, NULL);
    PCCOR_SIGNATURE pWalk = msig.m_pRetType.GetPtr();
    _ASSERTE(pWalk <= pSig + cSig);
    while (pWalk < pSig + cSig && (*pWalk == ELEMENT_TYPE_CMOD_OPT || *pWalk == ELEMENT_TYPE_CMOD_REQD))
    {
        BOOL fIsOptional = (*pWalk == ELEMENT_TYPE_CMOD_OPT);

        pWalk++;
        if (pWalk + CorSigUncompressedDataSize(pWalk) > pSig + cSig)
        {
            return FALSE; // Bad formatting
        }
        mdToken tk;
        pWalk += CorSigUncompressToken(pWalk, &tk);

        if (fIsOptional)
        {
            if (IsTypeRefOrDef("System.Runtime.CompilerServices.CallConvCdecl", pModule, tk))
            {
                *pPinvokeMapOut = pmCallConvCdecl;
                return TRUE;
            }
            else if (IsTypeRefOrDef("System.Runtime.CompilerServices.CallConvStdcall", pModule, tk))
            {
                *pPinvokeMapOut = pmCallConvStdcall;
                return TRUE;
            }
            else if (IsTypeRefOrDef("System.Runtime.CompilerServices.CallConvThiscall", pModule, tk))
            {
                *pPinvokeMapOut = pmCallConvThiscall;
                return TRUE;
            }
            else if (IsTypeRefOrDef("System.Runtime.CompilerServices.CallConvFastcall", pModule, tk))
            {
                *pPinvokeMapOut = pmCallConvFastcall;
                return TRUE;
            }
        }
    }

    *pPinvokeMapOut = (CorPinvokeMap)0;
    return TRUE;
}


Substitution::Substitution(mdToken parentTypeDefOrRefOrSpec, Module* pModuleArg, const Substitution *nextArg)
{
    LEAF_CONTRACT;

    m_pModule = pModuleArg; 
    m_inst = NULL;
    m_pNext = nextArg;


    if (IsNilToken(parentTypeDefOrRefOrSpec) || TypeFromToken(parentTypeDefOrRefOrSpec) != mdtTypeSpec)
        return;

    ULONG cSig;
    PCCOR_SIGNATURE pSig;
    pModuleArg->GetMDImport()->GetTypeSpecFromToken(parentTypeDefOrRefOrSpec, &pSig, &cSig);
    SigPointer sigptr = SigPointer(pSig, cSig);
    CorElementType type;

    if (FAILED(sigptr.GetElemType(&type)))
        return;

    // The only kind of type specs that we recognise are instantiated types
    if (type != ELEMENT_TYPE_GENERICINST)
        return;

    if (FAILED(sigptr.GetElemType(&type)))
        return;
        
    if (type != ELEMENT_TYPE_CLASS)
        return;

    /* mdToken genericTok = */ 
    if (FAILED(sigptr.GetToken(NULL)))
        return;
    /* DWORD ntypars = */ 
    if (FAILED(sigptr.GetData(NULL)))
        return;

    m_inst = sigptr.GetPtr();

}

void Substitution::CopyToArray(Substitution *pTarget) const
{
    LEAF_CONTRACT;

    const Substitution *pChain = this;
    DWORD i = 0;
    for (; pChain != NULL; pChain = pChain->GetNext())
    {
        CONSISTENCY_CHECK(CheckPointer(pChain->GetModule()));

        Substitution *pNext = (pChain->GetNext() != NULL) ? &pTarget[i+1] : NULL;
        pTarget[i++] = Substitution(pChain->GetModule(), pChain->GetInstSig(), pNext);
    }
}

DWORD Substitution::GetLength() const 
{ 
    LEAF_CONTRACT; 
    DWORD res = 0; 
    for (const Substitution *pChain = this; pChain != NULL; pChain = pChain->m_pNext) 
        res++; 
    return res; 
}
void Substitution::DeleteChain() 
{
    LEAF_CONTRACT;
    if(m_pNext) ((Substitution*)m_pNext)->DeleteChain();
    delete this;
}

#endif // #ifndef DACCESS_COMPILE
