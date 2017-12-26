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
#include "common.h"
#include "customattribute.h"
#include "invokeutil.h"
#include "comstring.h"
#include "method.hpp"
#include "threads.h"
#include "excep.h"
#include "corerror.h"
#include "security.h"
#include "securitydescriptor.h"
#include "classnames.h"
#include "fcall.h"
#include "assemblynative.hpp"
#include "typeparse.h"
#include "../md/compiler/custattr.h"
#include "securityattributes.h"
#include "reflectioninvocation.h"
#include "runtimehandles.h"

typedef InlineFactory<InlineSString<64>, 16> SStringFactory;

CorSerializationType GetCorSerializationTypeForEnum(LPCUTF8 szEnumName, COUNT_T cbEnumName, DomainAssembly* pDomainAssembly)
{ 
    CONTRACTL
    {
        PRECONDITION(CheckPointer(pDomainAssembly));
        PRECONDITION(CheckPointer(szEnumName));
        PRECONDITION(cbEnumName);
        if (pDomainAssembly) THROWS; else NOTHROW;
        if (pDomainAssembly) GC_TRIGGERS; else GC_NOTRIGGER;
        MODE_ANY;
    }
    CONTRACTL_END;

    StackScratchBuffer buff;
    StackSString sszEnumName(SString::Utf8, szEnumName, cbEnumName);
    TypeHandle th = TypeName::GetTypeUsingCASearchRules(sszEnumName.GetUTF8(buff), pDomainAssembly->GetAssembly());

    if (th.IsNull() || !th.IsEnum())
        return (CorSerializationType)0;

    return (CorSerializationType)th.GetVerifierCorElementType();
}


void SetBlittableCaValue(CustomAttributeValue* pVal, CaValue* pCaVal, BOOL* pbAllBlittableCa)
{
    WRAPPER_CONTRACT;

    CorSerializationType type = pCaVal->type.tag;

    pVal->m_type.m_tag = pCaVal->type.tag;
    pVal->m_type.m_arrayType = pCaVal->type.arrayType;
    pVal->m_type.m_enumType = pCaVal->type.enumType;
    pVal->m_rawValue = 0;
    
    if (type == SERIALIZATION_TYPE_STRING || 
        type == SERIALIZATION_TYPE_SZARRAY || 
        type == SERIALIZATION_TYPE_TYPE)
    {
        *pbAllBlittableCa = FALSE;
    }
    else
    {
        // Enum arg -> Object param
        if (type == SERIALIZATION_TYPE_ENUM && pCaVal->type.cEnumName)
            *pbAllBlittableCa = FALSE;    
        
        pVal->m_rawValue = pCaVal->i8;
    }
}

typedef struct {
    STRINGREF string;
    CaValueArrayREF array;
} CustomAttributeManagedValues;

void SetManagedValue(CustomAttributeManagedValues gc, CustomAttributeValue* pValue)
{
    WRAPPER_CONTRACT;

    CorSerializationType type = pValue->m_type.m_tag;

    if (type == SERIALIZATION_TYPE_TYPE || type == SERIALIZATION_TYPE_STRING)
    {
        SetObjectReference((OBJECTREF*)&pValue->m_enumOrTypeName, gc.string, GetAppDomain());
    }
    else if (type == SERIALIZATION_TYPE_ENUM)
    {
        SetObjectReference((OBJECTREF*)&pValue->m_type.m_enumName, gc.string, GetAppDomain());
    }
    else if (type == SERIALIZATION_TYPE_SZARRAY)
    {
        SetObjectReference((OBJECTREF*)&pValue->m_value, gc.array, GetAppDomain());
        
        if (pValue->m_type.m_arrayType == SERIALIZATION_TYPE_ENUM)
            SetObjectReference((OBJECTREF*)&pValue->m_type.m_enumName, gc.string, GetAppDomain());
    }   
}

TypeHandle g_thCustomAttributeValue;

CustomAttributeManagedValues GetManagedCaValue(CaValue* pCaVal)
{
    WRAPPER_CONTRACT;

    CustomAttributeManagedValues gc;
    ZeroMemory(&gc, sizeof(gc));
   
    CorSerializationType type = pCaVal->type.tag;
    
    if (type == SERIALIZATION_TYPE_ENUM)
    {
        gc.string = COMString::NewString(pCaVal->type.szEnumName, pCaVal->type.cEnumName);                      
    }
    else if (type == SERIALIZATION_TYPE_STRING)
    {
        gc.string = NULL;
        
        if (pCaVal->str.pStr)
            gc.string = COMString::NewString(pCaVal->str.pStr, pCaVal->str.cbStr);
    }
    else if (type == SERIALIZATION_TYPE_TYPE)
    {
        gc.string = COMString::NewString(pCaVal->str.pStr, pCaVal->str.cbStr);              
    }
    else if (type == SERIALIZATION_TYPE_SZARRAY)
    {
        CorSerializationType arrayType = pCaVal->type.arrayType;
        ULONG length = pCaVal->arr.length;
        BOOL bAllBlittableCa = arrayType != SERIALIZATION_TYPE_ENUM;

        if (length == (ULONG)-1)
            return gc;
        
        if (g_thCustomAttributeValue.IsNull())
            g_thCustomAttributeValue = TypeHandle(g_Mscorlib.GetClass(CLASS__CUSTOM_ATTRIBUTE_ENCODED_ARGUMENT));

        gc.array = (CaValueArrayREF)AllocateValueSzArray(g_thCustomAttributeValue, length);
        CustomAttributeValue* pValues = gc.array->GetDirectPointerToNonObjectElements();

        for (COUNT_T i = 0; i < length; i ++)
            SetBlittableCaValue(&pValues[i], &pCaVal->arr[i], &bAllBlittableCa); 

        if (!bAllBlittableCa)
        {
            GCPROTECT_BEGIN(gc)
            {   
                if (arrayType == SERIALIZATION_TYPE_ENUM)
                    gc.string = COMString::NewString(pCaVal->type.szEnumName, pCaVal->type.cEnumName);                      
                
                for (COUNT_T i = 0; i < length; i ++)
                {
                    CustomAttributeManagedValues managedCaValue = GetManagedCaValue(&pCaVal->arr[i]);
                    SetManagedValue(
                        managedCaValue,
                        &gc.array->GetDirectPointerToNonObjectElements()[i]);
                }
            }
            GCPROTECT_END();
        }
    }

    return gc;
}

FCIMPL5(VOID, COMCustomAttribute::ParseAttributeUsageAttribute, PVOID pData, ULONG cData, ULONG* pTargets, CLR_BOOL* pInherited, CLR_BOOL* pAllowMultiple)
{
    CONTRACTL {
        THROWS;
        DISABLED(GC_TRIGGERS);  // can't use this in an FCALL because we're in forbid gc mode until we setup a H_M_F.
        MODE_COOPERATIVE;
        SO_TOLERANT;
    } CONTRACTL_END;

    int inherited = 0;
    int allowMultiple = 1;    
        
    BEGIN_SO_INTOLERANT_CODE_NOTHROW(GetThread(), FCThrowVoid(kStackOverflowException));
    {
        CustomAttributeParser ca(pData, cData);
        
        CaArg args[1];
        args[0].InitEnum(SERIALIZATION_TYPE_I4, 0);
        if (FAILED(ParseKnownCaArgs(ca, args, lengthof(args), NULL, NULL)))
        {
            HELPER_METHOD_FRAME_BEGIN_0();
            COMPlusThrow(kCustomAttributeFormatException);   
            HELPER_METHOD_FRAME_END();
        }
            
        *pTargets = args[0].val.u4;

        CaNamedArg namedArgs[2];
        CaType namedArgTypes[2];
        namedArgTypes[inherited].Init(SERIALIZATION_TYPE_BOOLEAN);
        namedArgTypes[allowMultiple].Init(SERIALIZATION_TYPE_BOOLEAN);
        namedArgs[inherited].Init("Inherited", SERIALIZATION_TYPE_PROPERTY, namedArgTypes[inherited], TRUE);
        namedArgs[allowMultiple].Init("AllowMultiple", SERIALIZATION_TYPE_PROPERTY, namedArgTypes[allowMultiple], FALSE);
        if (FAILED(ParseKnownCaNamedArgs(ca, namedArgs, lengthof(namedArgs), NULL, NULL)))
        {
            HELPER_METHOD_FRAME_BEGIN_0();
            COMPlusThrow(kCustomAttributeFormatException);   
            HELPER_METHOD_FRAME_END();
        }

        *pInherited = namedArgs[inherited].val.boolean == TRUE;
        *pAllowMultiple = namedArgs[allowMultiple].val.boolean == TRUE;
    }
    END_SO_INTOLERANT_CODE;    
}
FCIMPLEND

HRESULT ParseAttributeArguments(
    void* pCa, INT32 cCa,
    CaValueArrayFactory* pCaValueArrayFactory,
    CaArg* pCaArgs, COUNT_T cArgs,  
    CaNamedArg* pCaNamedArgs, COUNT_T cNamedArgs,  
    DomainAssembly* pDomainAssembly)
{
    HRESULT hr = S_OK;
    CustomAttributeParser cap(pCa, cCa);
    IfFailGo(ParseKnownCaArgs(cap, pCaArgs, cArgs, pCaValueArrayFactory, pDomainAssembly));
    IfFailGo(ParseKnownCaNamedArgs(cap, pCaNamedArgs, cNamedArgs, pCaValueArrayFactory, pDomainAssembly));

ErrExit:  
    return hr;
}

CaType InitCaValue(CustomAttributeType* pType, Factory<SString>* pSstringFactory, Factory<StackScratchBuffer>* pStackScratchBufferFactory)
{
    CONTRACTL {
        THROWS;
    } CONTRACTL_END;

    SString* psszName = pSstringFactory->Create();
    psszName->Set(pType->m_enumName == NULL ? NULL : pType->m_enumName->GetBuffer());
    CaTypeCtor caType(
        pType->m_tag, 
        pType->m_arrayType, 
        pType->m_enumType, 
        psszName->GetUTF8(*pStackScratchBufferFactory->Create()),
        (ULONG)psszName->GetCount());
    
    return caType;
}

FCIMPL5(VOID, Attribute::ParseAttributeArguments, void* pCa, INT32 cCa,
        CaArgArrayREF* ppCustomAttributeArguments,
        CaNamedArgArrayREF* ppCustomAttributeNamedArguments,
        DomainAssembly* pDomainAssembly)
{
    CONTRACTL {
        THROWS;
        DISABLED(GC_TRIGGERS);  // can't use this in an FCALL because we're in forbid gc mode until we setup a H_M_F.
        MODE_COOPERATIVE;
        SO_TOLERANT;
    } CONTRACTL_END;

    HELPER_METHOD_FRAME_BEGIN_0()
    {
        struct 
        {
            CustomAttributeArgument* pArgs;
            CustomAttributeNamedArgument* pNamedArgs;
        } gc;

        gc.pArgs = NULL;
        gc.pNamedArgs = NULL;

        HRESULT hr = S_OK;

        GCPROTECT_BEGININTERIOR(gc);

        BOOL bAllBlittableCa = TRUE;
        COUNT_T cArgs = 0;
        COUNT_T cNamedArgs = 0;
        CaArg* pCaArgs = NULL;
        CaNamedArg* pCaNamedArgs = NULL;
        CaValueArrayFactory caValueArrayFactory;
        InlineFactory<StackScratchBuffer, 16> stackScratchBufferFactory;
        InlineFactory<SString, 16> sstringFactory;

        cArgs = (*ppCustomAttributeArguments)->GetNumComponents();

        if (cArgs)
        {        
            gc.pArgs = (*ppCustomAttributeArguments)->GetDirectPointerToNonObjectElements();

            size_t size = sizeof(CaArg) * cArgs;
            if ((size / sizeof(CaArg)) != cArgs) // uint over/underflow
                IfFailGo(E_INVALIDARG);
            pCaArgs = (CaArg*)_alloca(size);
            
            for (COUNT_T i = 0; i < cArgs; i ++)
                pCaArgs[i].Init(InitCaValue(&gc.pArgs[i].m_type, &sstringFactory, &stackScratchBufferFactory));        
        }
        
        cNamedArgs = (*ppCustomAttributeNamedArguments)->GetNumComponents();
        
        if (cNamedArgs) 
        {
            gc.pNamedArgs = (*ppCustomAttributeNamedArguments)->GetDirectPointerToNonObjectElements();

            size_t size = sizeof(CaNamedArg) * cNamedArgs;
            if ((size / sizeof(CaNamedArg)) != cNamedArgs) // uint over/underflow
                IfFailGo(E_INVALIDARG);
            pCaNamedArgs = (CaNamedArg*)_alloca(size);

            for (COUNT_T i = 0; i < cNamedArgs; i ++)
            {
                CustomAttributeNamedArgument* pNamedArg = &gc.pNamedArgs[i];

                SString* psszName = sstringFactory.Create();
                psszName->Set(pNamedArg->m_argumentName->GetBuffer());
                
                pCaNamedArgs[i].Init(
                    psszName->GetUTF8(*stackScratchBufferFactory.Create()),
                    pNamedArg->m_propertyOrField,
                    InitCaValue(&pNamedArg->m_type, &sstringFactory, &stackScratchBufferFactory));
            }
        }

        if (FAILED(::ParseAttributeArguments(pCa, cCa, &caValueArrayFactory, pCaArgs, cArgs, pCaNamedArgs, cNamedArgs, NULL)))
        {
            if (!pDomainAssembly)
                IfFailGo(E_FAIL);
        
            // Handle enum argument stored in object parameter
            // Size of enum is not encoded in this case -- we need to use the loader to determine its size
            hr = ::ParseAttributeArguments(pCa, cCa, &caValueArrayFactory, pCaArgs, cArgs, pCaNamedArgs, cNamedArgs, pDomainAssembly);
        
            IfFailGo(hr);
        }
        
        for (COUNT_T i = 0; i < cArgs; i ++)
            SetBlittableCaValue(&gc.pArgs[i].m_value, &pCaArgs[i].val, &bAllBlittableCa);        
        
        for (COUNT_T i = 0; i < cNamedArgs; i ++)
            SetBlittableCaValue(&gc.pNamedArgs[i].m_value, &pCaNamedArgs[i].val, &bAllBlittableCa);        
        
        if (!bAllBlittableCa)
        {
            for (COUNT_T i = 0; i < cArgs; i ++)
            {
                CustomAttributeManagedValues managedCaValue = GetManagedCaValue(&pCaArgs[i].val);
                SetManagedValue(managedCaValue, &(gc.pArgs[i].m_value));
            }
    
            for (COUNT_T i = 0; i < cNamedArgs; i++)
            {
                CustomAttributeManagedValues managedCaValue = GetManagedCaValue(&pCaNamedArgs[i].val);
                SetManagedValue(managedCaValue, &(gc.pNamedArgs[i].m_value));                
            }
        }    
        
    ErrExit:

        ; // Need empty statement to get GCPROTECT_END below to work.

        GCPROTECT_END();


        if (hr != S_OK)
        {
            COMPlusThrow(kCustomAttributeFormatException);
        }
    }
    HELPER_METHOD_FRAME_END();
}
FCIMPLEND


// internal utility functions defined atthe end of this file                                               
TypeHandle GetTypeHandleFromBlob(Assembly *pCtorAssembly,
                                    CorSerializationType objType, 
                                    BYTE **pBlob, 
                                    const BYTE *endBlob,
                                    Module *pModule);
int GetStringSize(BYTE **pBlob, const BYTE *endBlob);
ARG_SLOT GetDataFromBlob(Assembly *pCtorAssembly,
                      CorSerializationType type, 
                      TypeHandle th, 
                      BYTE **pBlob, 
                      const BYTE *endBlob, 
                      Module *pModule, 
                      BOOL *bObjectCreated);
void ReadArray(Assembly *pCtorAssembly,
               CorSerializationType arrayType, 
               int size, 
               TypeHandle th,
               BYTE **pBlob, 
               const BYTE *endBlob, 
               Module *pModule,
               BASEARRAYREF *pArray);

BYTE s_noArgsCaCtor[sizeof(MetaSig)];
MetaSig* s_pNoArgsCaCtor = NULL;
LONG s_caVoidVoidCaCtorCacheState = 0;

enum {
    CaVoidVoidCaCtorUninitialized = 0,
    CaVoidVoidCaCtorInitializing = 1,
    CaVoidVoidCaCtorInitialized = 2,
};

static BOOL InterlockCompareExchange(volatile LONG* destination, volatile LONG exchange, volatile LONG comperand)
{
    return FastInterlockCompareExchange(destination, exchange, comperand) == comperand;
}


FCIMPL2(Object*, RuntimeTypeHandle::CreateCaInstance, TypeHandle* pCaType, MethodDesc* pCtor) {
    CONTRACTL {
        THROWS;
        DISABLED(GC_TRIGGERS);
        MODE_COOPERATIVE;
        SO_TOLERANT;

        PRECONDITION(CheckPointer(pCaType));
        PRECONDITION(!pCaType->IsGenericVariable()); 
        PRECONDITION(pCaType->IsValueType() || CheckPointer(pCtor));
    }
    CONTRACTL_END;

    MethodTable* pCaMT = pCaType->GetMethodTable();

    OBJECTREF o = NULL;
    HELPER_METHOD_FRAME_BEGIN_RET_1(o);
    {
        PRECONDITION(
            (!pCtor && pCaType->IsValueType() && !pCaType->GetMethodTable()->HasDefaultConstructor()) || 
            (pCtor == pCaType->GetMethodTable()->GetDefaultConstructor()));

        // If we relax this, we need to insure custom attributes construct properly for Nullable<T>
        if (pCaType->HasInstantiation())
            COMPlusThrow(kNotSupportedException, L"Argument_GenericsInvalid");
        
        o = pCaMT->Allocate();

        if (pCtor)
        {
            
            ARG_SLOT args;
            
            if (pCaMT->IsValueClass())
            {
                MethodDescCallSite ctor(pCtor, &o);
                args = PtrToArgSlot(o->UnBox());
                ctor.CallWithValueTypes(&args);
            }
            else
            {
#ifdef DACCESS_COMPILE 
                MethodDescCallSite ctor(pCtor, &o);
                args = ObjToArgSlot(o);
                ctor.CallWithValueTypes(&args);
#else // !DACCESS_COMPILE
                if (s_caVoidVoidCaCtorCacheState == CaVoidVoidCaCtorInitialized)
                {
                    MethodDescCallSite ctor(pCtor, s_pNoArgsCaCtor, &o);
                    args = ObjToArgSlot(o);
                    ctor.CallWithValueTypes(&args);
                }
                else if (s_caVoidVoidCaCtorCacheState == CaVoidVoidCaCtorUninitialized)
                {                   
                    if (InterlockCompareExchange(&s_caVoidVoidCaCtorCacheState, CaVoidVoidCaCtorInitializing, CaVoidVoidCaCtorUninitialized))
                    {
                        memset(&s_noArgsCaCtor, 0, sizeof(MetaSig));
                        MetaSig voidVoidCtor(g_Mscorlib.GetMethod(METHOD__OBJECT__CTOR));
                        voidVoidCtor.ForceSigWalk(FALSE);
                        s_pNoArgsCaCtor = new (&s_noArgsCaCtor) MetaSig(&voidVoidCtor);
                        FastInterlockExchange(&s_caVoidVoidCaCtorCacheState, CaVoidVoidCaCtorInitialized);
                        
                        MethodDescCallSite ctor(pCtor, s_pNoArgsCaCtor, &o);
                        args = ObjToArgSlot(o);
                        ctor.CallWithValueTypes(&args);
                    }
                    else
                    {
                        MethodDescCallSite ctor(pCtor, &o);
                        args = ObjToArgSlot(o);
                        ctor.CallWithValueTypes(&args);
                    }
                }                
                
#endif // !DACCESS_COMPILE
            }
            
        }
    }
    HELPER_METHOD_FRAME_END();
    
    return OBJECTREFToObject(o);
}
FCIMPLEND

FCIMPL5(LPVOID, COMCustomAttribute::CreateCaObject, Module* pAttributedModule, MethodDesc* pCtorMD, BYTE** ppBlob, BYTE* pEndBlob, INT32* pcNamedArgs)
{
    CONTRACTL
    {
        MODE_COOPERATIVE;
        DISABLED(GC_TRIGGERS);  // can't use this in an FCALL because we're in forbid gc mode until we setup a H_M_F.
        THROWS;
        SO_TOLERANT;
    }
    CONTRACTL_END;

    OBJECTREF ca = NULL;
    HELPER_METHOD_FRAME_BEGIN_RET_ATTRIB_1(Frame::FRAME_ATTR_RETURNOBJ, ca);
    {
        MethodDescCallSite ctorCallSite(pCtorMD);
        MetaSig* pSig = ctorCallSite.GetMetaSig();
        BYTE* pBlob = *ppBlob;

        // get the number of arguments and allocate an array for the args
        ARG_SLOT *args = NULL;
        UINT cArgs = pSig->NumFixedArgs() + 1; // make room for the this pointer
        UINT i = 1; // used to flag that we actually get the right number of arg from the blob
        
        args = (ARG_SLOT*)_alloca(cArgs * sizeof(ARG_SLOT));
        memset((void*)args, 0, cArgs * sizeof(ARG_SLOT));
        
        OBJECTREF *argToProtect = (OBJECTREF*)_alloca(cArgs * sizeof(OBJECTREF));
        memset((void*)argToProtect, 0, cArgs * sizeof(OBJECTREF));

        // If we relax this, we need to insure custom attributes construct properly for Nullable<T>
        if (pCtorMD->GetMethodTable()->HasInstantiation())
            COMPlusThrow(kNotSupportedException, L"Argument_GenericsInvalid");

        // load the this pointer
        argToProtect[0] = pCtorMD->GetMethodTable()->Allocate(); // this is the value to return after the ctor invocation

        if (pBlob) 
        {
            if (pBlob < pEndBlob) 
            {
                INT16 prolog = GET_UNALIGNED_VAL16(pBlob);
                if (prolog != 1) 
                    COMPlusThrow(kCustomAttributeFormatException);
                pBlob += 2;
            }

            if (cArgs > 1) 
            {
                GCPROTECT_ARRAY_BEGIN(*argToProtect, cArgs);
                {
                    // loop through the args
                    for (i = 1; i < cArgs; i++) {
                        CorElementType type = pSig->NextArg();
                        if (type == ELEMENT_TYPE_END) 
                            break;
                        BOOL bObjectCreated = FALSE;
                        TypeHandle th = pSig->GetLastTypeHandleThrowing();
                        if (th.IsArray())
                            // get the array element 
                            th = th.AsArray()->GetArrayElementTypeHandle();
                        ARG_SLOT data = GetDataFromBlob(pCtorMD->GetAssembly(), (CorSerializationType)type, th, &pBlob, pEndBlob, pAttributedModule, &bObjectCreated);
                        if (bObjectCreated) 
                            argToProtect[i] = ArgSlotToObj(data);
                        else
                            args[i] = data;
                    }
                }
                GCPROTECT_END();
                
                for (i = 1; i < cArgs; i++) 
                {
                    if (argToProtect[i] != NULL) 
                    {
                        _ASSERTE(args[i] == NULL);
                        args[i] = ObjToArgSlot(argToProtect[i]);
                    }
                }
            }
        }
        args[0] = ObjToArgSlot(argToProtect[0]);
        
        if (i != cArgs)
            COMPlusThrow(kCustomAttributeFormatException);
        
        // check if there are any named properties to invoke, 
        // if so set the by ref int passed in to point 
        // to the blob position where name properties start
        *pcNamedArgs = 0;
        
        if (pBlob && pBlob != pEndBlob) 
        {
            if (pBlob + 2 > pEndBlob) 
                COMPlusThrow(kCustomAttributeFormatException);
            
            *pcNamedArgs = GET_UNALIGNED_VAL16(pBlob);
            
            pBlob += 2;            
        }
        
        *ppBlob = pBlob;
        
        if (*pcNamedArgs == 0 && pBlob != pEndBlob) 
            COMPlusThrow(kCustomAttributeFormatException);
        
        // make the invocation to the ctor
        ca = ArgSlotToObj(args[0]);
        if (pCtorMD->GetMethodTable()->IsValueClass()) 
            args[0] = PtrToArgSlot(OBJECTREFToObject(ca)->UnBox());

        ctorCallSite.CallWithValueTypes(args);
    }
    HELPER_METHOD_FRAME_END();

    return OBJECTREFToObject(ca);
}
FCIMPLEND

FCIMPL3(VOID, COMCustomAttribute::GetSecurityAttributes, Module *pModule, DWORD tkToken, PTRARRAYREF* ppArray)
{
    CONTRACTL
    {
        MODE_COOPERATIVE;
        DISABLED(GC_TRIGGERS);  // can't use this in an FCALL because we're in forbid gc mode until we setup a H_M_F.
        THROWS;
        SO_TOLERANT;
    }
    CONTRACTL_END;

    OBJECTREF throwable = NULL;
    HELPER_METHOD_FRAME_BEGIN_1(throwable);
    {
        IMDInternalImport* pScope = pModule->GetMDImport();

        DWORD action;    

        CORSEC_ATTRSET_ARRAY aAttrset;
        for(action = 1; action <= dclMaximumValue; action++)
        {
            HENUMInternalHolder hEnum(pScope);                                                                   
            if (!hEnum.EnumPermissionSetsInit(tkToken, (CorDeclSecurity)action))
                continue;

            mdPermission tkPerm;
            BYTE* pbBlob;
            ULONG cbBlob;
            DWORD dwAction;

            while (pScope->EnumNext(&hEnum, &tkPerm))
            {
                pScope->GetPermissionSetProps(
                    tkPerm,
                    &dwAction,
                    (void const **)&pbBlob,
                    &cbBlob);

                CORSEC_ATTRSET* pAttrSet = &*aAttrset.Append();
                HRESULT hr = BlobToAttributeSet(pbBlob, pAttrSet, dwAction);
                if (FAILED(hr))
                    COMPlusThrowHR(hr);
            }
        }

        DWORD dwCount = 0;
        for (COUNT_T i = 0; i < aAttrset.GetCount(); i ++)
            dwCount += aAttrset[i].dwAttrCount;

        *ppArray = (PTRARRAYREF)AllocateObjectArray(dwCount, g_pObjectClass);

        CQuickBytes qb;

        COUNT_T c = 0;
        for (COUNT_T i = 0; i < aAttrset.GetCount(); i ++)
        {
            CORSEC_ATTRSET& attrset = aAttrset[i];
            OBJECTREF* attrArray = (OBJECTREF*)qb.AllocThrows(attrset.dwAttrCount * sizeof(OBJECTREF));
            memset(attrArray, 0, attrset.dwAttrCount * sizeof(OBJECTREF));
            GCPROTECT_ARRAY_BEGIN(*attrArray, attrset.dwAttrCount);
            {
                // Convert to a managed array of attribute objects
                DWORD dwErrorIndex;
                HRESULT hr = SecurityAttributes::AttributeSetToManaged(attrArray, &attrset, &throwable, &dwErrorIndex, true);
                if (FAILED(hr))
                    COMPlusThrowHR(hr);

                for (COUNT_T j = 0; j < attrset.dwAttrCount; j ++)
                    (*ppArray)->SetAt(c++, attrArray[j]);
            }
            GCPROTECT_END();
            
        }
    }
    HELPER_METHOD_FRAME_END();
}
FCIMPLEND
    
FCIMPL7(void, COMCustomAttribute::GetPropertyOrFieldData, Module* pModule, BYTE** ppBlobStart, BYTE* pBlobEnd, STRINGREF* pName, CLR_BOOL* pbIsProperty, OBJECTREF* pType, OBJECTREF* value)
{
    CONTRACTL
    {
        MODE_COOPERATIVE;
        DISABLED(GC_TRIGGERS);  // can't use this in an FCALL because we're in forbid gc mode until we setup a H_M_F.
        THROWS;
        SO_TOLERANT;
    }
    CONTRACTL_END;

    BYTE* pBlob = *ppBlobStart;
    *pType = NULL;

    HELPER_METHOD_FRAME_BEGIN_0();
    {
        Assembly *pCtorAssembly = NULL;

        MethodTable *pMTValue = NULL;
        CorSerializationType arrayType = SERIALIZATION_TYPE_BOOLEAN;
        BOOL bObjectCreated = FALSE;
        TypeHandle nullTH;

        if (pBlob + 2 > pBlobEnd) 
            COMPlusThrow(kCustomAttributeFormatException);
        
        // get whether it is a field or a property
        CorSerializationType propOrField = (CorSerializationType)*pBlob;
        pBlob++;
        if (propOrField == SERIALIZATION_TYPE_FIELD) 
            *pbIsProperty = FALSE;
        else if (propOrField == SERIALIZATION_TYPE_PROPERTY) 
            *pbIsProperty = TRUE;
        else 
            COMPlusThrow(kCustomAttributeFormatException);
        
        // get the type of the field
        CorSerializationType fieldType = (CorSerializationType)*pBlob;
        pBlob++;
        if (fieldType == SERIALIZATION_TYPE_SZARRAY) 
        {
            arrayType = (CorSerializationType)*pBlob;
            
            if (pBlob + 1 > pBlobEnd) 
                COMPlusThrow(kCustomAttributeFormatException);
            
            pBlob++;
        }
        if (fieldType == SERIALIZATION_TYPE_ENUM || arrayType == SERIALIZATION_TYPE_ENUM) 
        {
            // get the enum type
            ReflectClassBaseObject *pEnum = 
                (ReflectClassBaseObject*)OBJECTREFToObject(ArgSlotToObj(GetDataFromBlob(
                    pCtorAssembly, SERIALIZATION_TYPE_TYPE, nullTH, &pBlob, pBlobEnd, pModule, &bObjectCreated)));

            if (pEnum == NULL)
                COMPlusThrow(kCustomAttributeFormatException);

            _ASSERTE(bObjectCreated);
            
            TypeHandle th = pEnum->GetType();
            _ASSERTE(th.IsEnum());
            
            pMTValue = th.AsMethodTable();
            if (fieldType == SERIALIZATION_TYPE_ENUM) 
                // load the enum type to pass it back
                *pType = th.GetManagedClassObject();
            else 
                nullTH = th;
        }

        //
        // get the string representing the field/property name
        *pName = ArgSlotToString(GetDataFromBlob(
            pCtorAssembly, SERIALIZATION_TYPE_STRING, nullTH, &pBlob, pBlobEnd, pModule, &bObjectCreated));    
        _ASSERTE(bObjectCreated || *pName == NULL);

        // create the object and return it
        switch (fieldType) 
        {
            case SERIALIZATION_TYPE_TAGGED_OBJECT:
                *pType = g_Mscorlib.GetClass(CLASS__OBJECT)->GetManagedClassObject();
            case SERIALIZATION_TYPE_TYPE:
            case SERIALIZATION_TYPE_STRING:
                *value = ArgSlotToObj(GetDataFromBlob(
                    pCtorAssembly, fieldType, nullTH, &pBlob, pBlobEnd, pModule, &bObjectCreated));
                _ASSERTE(bObjectCreated || *value == NULL);
                
                if (*value == NULL) 
                {
                    // load the proper type so that code in managed knows which property to load
                    if (fieldType == SERIALIZATION_TYPE_STRING) 
                        *pType = g_Mscorlib.GetElementType(ELEMENT_TYPE_STRING)->GetManagedClassObject();
                    else if (fieldType == SERIALIZATION_TYPE_TYPE) 
                        *pType = g_Mscorlib.GetClass(CLASS__TYPE)->GetManagedClassObject();
                }
                break;
            case SERIALIZATION_TYPE_SZARRAY:
            {
                int arraySize = (int)GetDataFromBlob(pCtorAssembly, SERIALIZATION_TYPE_I4, nullTH, &pBlob, pBlobEnd, pModule, &bObjectCreated);
                
                if (arraySize != -1) 
                {
                    _ASSERTE(!bObjectCreated);
                    if (arrayType == SERIALIZATION_TYPE_STRING) 
                        nullTH = TypeHandle(g_Mscorlib.GetElementType(ELEMENT_TYPE_STRING));
                    else if (arrayType == SERIALIZATION_TYPE_TYPE) 
                        nullTH = TypeHandle(g_Mscorlib.GetClass(CLASS__TYPE));
                    else if (arrayType == SERIALIZATION_TYPE_TAGGED_OBJECT)
                        nullTH = TypeHandle(g_Mscorlib.GetClass(CLASS__OBJECT));
                    ReadArray(pCtorAssembly, arrayType, arraySize, nullTH, &pBlob, pBlobEnd, pModule, (BASEARRAYREF*)value);
                }
                if (*value == NULL) 
                {
                    TypeHandle arrayTH;
                    switch (arrayType) 
                    {
                        case SERIALIZATION_TYPE_STRING:
                            arrayTH = TypeHandle(g_Mscorlib.GetElementType(ELEMENT_TYPE_STRING));
                            break;
                        case SERIALIZATION_TYPE_TYPE:
                            arrayTH = TypeHandle(g_Mscorlib.GetClass(CLASS__TYPE));
                            break;
                        case SERIALIZATION_TYPE_TAGGED_OBJECT:
                            arrayTH = TypeHandle(g_Mscorlib.GetClass(CLASS__OBJECT));
                            break;
                        default:
                            if (SERIALIZATION_TYPE_BOOLEAN <= arrayType && arrayType <= SERIALIZATION_TYPE_R8) 
                                arrayTH = TypeHandle(g_Mscorlib.GetElementType((CorElementType)arrayType));
                    }
                    if (!arrayTH.IsNull()) 
                    {
                        arrayTH = ClassLoader::LoadArrayTypeThrowing(arrayTH);
                        *pType = arrayTH.GetManagedClassObject();
                    }
                }
                break;
            }
            default:
                if (SERIALIZATION_TYPE_BOOLEAN <= fieldType && fieldType <= SERIALIZATION_TYPE_R8) 
                    pMTValue = g_Mscorlib.GetElementType((CorElementType)fieldType);
                else if(fieldType == SERIALIZATION_TYPE_ENUM)
                    fieldType = (CorSerializationType)pMTValue->GetInternalCorElementType();
                else
                    COMPlusThrow(kCustomAttributeFormatException);
                
                ARG_SLOT val = GetDataFromBlob(pCtorAssembly, fieldType, nullTH, &pBlob, pBlobEnd, pModule, &bObjectCreated);
                _ASSERTE(!bObjectCreated);
                
                *value = pMTValue->Box((void*)ArgSlotEndianessFixup(&val, pMTValue->GetNumInstanceFieldBytes()));
        }

        *ppBlobStart = pBlob;
    }
    HELPER_METHOD_FRAME_END();
}
FCIMPLEND
  
// utility functions
TypeHandle GetTypeHandleFromBlob(Assembly *pCtorAssembly,
                                    CorSerializationType objType, 
                                    BYTE **pBlob, 
                                    const BYTE *endBlob,
                                    Module *pModule)
{
    CONTRACTL 
    {
        THROWS;
    }
    CONTRACTL_END;

    // we must box which means we must get the method table, switch again on the element type
    MethodTable *pMTType = NULL;
    TypeHandle nullTH;
    TypeHandle RtnTypeHnd;

    switch (objType) {
    case SERIALIZATION_TYPE_BOOLEAN:
    case SERIALIZATION_TYPE_I1:
    case SERIALIZATION_TYPE_U1:
    case SERIALIZATION_TYPE_CHAR:
    case SERIALIZATION_TYPE_I2:
    case SERIALIZATION_TYPE_U2:
    case SERIALIZATION_TYPE_I4:
    case SERIALIZATION_TYPE_U4:
    case SERIALIZATION_TYPE_R4:
    case SERIALIZATION_TYPE_I8:
    case SERIALIZATION_TYPE_U8:
    case SERIALIZATION_TYPE_R8:
    case SERIALIZATION_TYPE_STRING:
        pMTType = g_Mscorlib.GetElementType((CorElementType)objType);
        RtnTypeHnd = TypeHandle(pMTType);
        break;

    case ELEMENT_TYPE_CLASS:
        pMTType = g_Mscorlib.GetClass(CLASS__TYPE);
        RtnTypeHnd = TypeHandle(pMTType);
        break;

    case SERIALIZATION_TYPE_TAGGED_OBJECT:
        pMTType = g_Mscorlib.GetClass(CLASS__OBJECT);
        RtnTypeHnd = TypeHandle(pMTType);
        break;

    case SERIALIZATION_TYPE_TYPE:
    {
        int size = GetStringSize(pBlob, endBlob);
        if (size == -1) 
            return nullTH;

        if ((size+1 <= 1) || (size > endBlob - *pBlob))
            COMPlusThrow(kCustomAttributeFormatException);

        LPUTF8 szName = (LPUTF8)_alloca(size + 1);
        memcpy(szName, *pBlob, size);
        *pBlob += size;
        szName[size] = 0;

        RtnTypeHnd = TypeName::GetTypeUsingCASearchRules(szName, pModule->GetAssembly(), NULL, FALSE);
        break;
    }

    case SERIALIZATION_TYPE_ENUM:
    {
        // get the enum type
        BOOL isObject = FALSE;
        ReflectClassBaseObject *pType = (ReflectClassBaseObject*)OBJECTREFToObject(ArgSlotToObj(GetDataFromBlob(pCtorAssembly,
                                                                                                                SERIALIZATION_TYPE_TYPE, 
                                                                                                                nullTH, 
                                                                                                                pBlob, 
                                                                                                                endBlob, 
                                                                                                                pModule, 
                                                                                                                &isObject)));
        if (pType != NULL)
        {
            _ASSERTE(isObject);
            RtnTypeHnd = pType->GetType();
            _ASSERTE((objType == SERIALIZATION_TYPE_ENUM) ? RtnTypeHnd.GetClass()->IsEnum() : TRUE);
        }
        else
        {
            RtnTypeHnd = TypeHandle();
        }
        break;
    }

    default:
        COMPlusThrow(kCustomAttributeFormatException);
    }

    return RtnTypeHnd;
}

// retrieve the string size in a CA blob. Advance the blob pointer to point to
// the beginning of the string immediately following the size
int GetStringSize(BYTE **pBlob, const BYTE *endBlob)
{
    CONTRACTL 
    {
        THROWS;
    }
    CONTRACTL_END;

    int size = -1;

    // Null string - encoded as a single byte
    if (**pBlob != 0xFF) {
        if ((**pBlob & 0x80) == 0) 
            // encoded as a single byte
            size = **pBlob;
        else if ((**pBlob & 0xC0) == 0x80) {
            if (*pBlob + 1 > endBlob) 
                COMPlusThrow(kCustomAttributeFormatException);
            // encoded in two bytes
            size = (**pBlob & 0x3F) << 8;
            size |= *(++*pBlob); // This is in big-endian format
        }
        else {
            if (*pBlob + 3 > endBlob)
                COMPlusThrow(kCustomAttributeFormatException);
            // encoded in four bytes
            size = (**pBlob & ~0xC0) << 24;
            size |= *(++*pBlob) << 16;
            size |= *(++*pBlob) << 8;
            size |= *(++*pBlob);
        }
    }

    if (*pBlob + 1 > endBlob) 
        COMPlusThrow(kCustomAttributeFormatException);
    *pBlob += 1;

    return size;
}

// read the whole array as a chunk
void ReadArray(Assembly *pCtorAssembly,
               CorSerializationType arrayType, 
               int size, 
               TypeHandle th,
               BYTE **pBlob, 
               const BYTE *endBlob, 
               Module *pModule,
               BASEARRAYREF *pArray)
{    
    CONTRACTL 
    {
        THROWS;
    }
    CONTRACTL_END;
    
    BYTE *pData = NULL;
    ARG_SLOT element = 0;

    switch (arrayType) {
    case SERIALIZATION_TYPE_BOOLEAN:
    case SERIALIZATION_TYPE_I1:
    case SERIALIZATION_TYPE_U1:
        *pArray = (BASEARRAYREF)AllocatePrimitiveArray((CorElementType)arrayType, size);
        pData = (*pArray)->GetDataPtr();
        if (*pBlob + size > endBlob) 
            goto badBlob;
        memcpyNoGCRefs(pData, *pBlob, size);
        *pBlob += size;
        break;

    case SERIALIZATION_TYPE_CHAR:
    case SERIALIZATION_TYPE_I2:
    case SERIALIZATION_TYPE_U2:
        *pArray = (BASEARRAYREF)AllocatePrimitiveArray((CorElementType)arrayType, size);
        pData = (*pArray)->GetDataPtr();
        if (*pBlob + (size * 2) > endBlob) 
            goto badBlob;
        memcpyNoGCRefs(pData, *pBlob, size * 2);
        *pBlob += size * 2;
        break;

    case SERIALIZATION_TYPE_I4:
    case SERIALIZATION_TYPE_U4:
    case SERIALIZATION_TYPE_R4:
        *pArray = (BASEARRAYREF)AllocatePrimitiveArray((CorElementType)arrayType, size);
        pData = (*pArray)->GetDataPtr();
        if (*pBlob + (size * 4) > endBlob) 
            goto badBlob;
        memcpyNoGCRefs(pData, *pBlob, size * 4);
        *pBlob += size * 4;
        break;

    case SERIALIZATION_TYPE_I8:
    case SERIALIZATION_TYPE_U8:
    case SERIALIZATION_TYPE_R8:
        *pArray = (BASEARRAYREF)AllocatePrimitiveArray((CorElementType)arrayType, size);
        pData = (*pArray)->GetDataPtr();
        if (*pBlob + (size * 8) > endBlob) 
            goto badBlob;
        memcpyNoGCRefs(pData, *pBlob, size * 8);
        *pBlob += size * 8;
        break;

    case ELEMENT_TYPE_CLASS:
    case SERIALIZATION_TYPE_TYPE:
    case SERIALIZATION_TYPE_STRING:
    case SERIALIZATION_TYPE_SZARRAY:
    case SERIALIZATION_TYPE_TAGGED_OBJECT:
    {
        BOOL isObject;
        *pArray = (BASEARRAYREF)AllocateObjectArray(size, th);
        if (arrayType == SERIALIZATION_TYPE_SZARRAY) 
            // switch the th to be the proper one 
            th = th.AsArray()->GetArrayElementTypeHandle();
        for (int i = 0; i < size; i++) {
            element = GetDataFromBlob(pCtorAssembly, arrayType, th, pBlob, endBlob, pModule, &isObject);
            _ASSERTE(isObject || element == NULL);
            ((PTRARRAYREF)(*pArray))->SetAt(i, ArgSlotToObj(element));
        }
        break;
    }

    case SERIALIZATION_TYPE_ENUM:
    {
        INT32 bounds = size;
        unsigned elementSize = th.GetSize();
        TypeHandle arrayHandle = ClassLoader::LoadArrayTypeThrowing(th);
        if (arrayHandle.IsNull()) 
            goto badBlob;
        *pArray = (BASEARRAYREF)AllocateArrayEx(arrayHandle, &bounds, 1);
        pData = (*pArray)->GetDataPtr();
        size *= elementSize;
        if (*pBlob + size > endBlob) 
            goto badBlob;
        memcpyNoGCRefs(pData, *pBlob, size);
        *pBlob += size;
        break;
    }

    default:
    badBlob:
        COMPlusThrow(kCustomAttributeFormatException);
    }

}

// get data out of the blob according to a CorElementType
ARG_SLOT GetDataFromBlob(Assembly *pCtorAssembly,
                      CorSerializationType type, 
                      TypeHandle th, 
                      BYTE **pBlob, 
                      const BYTE *endBlob, 
                      Module *pModule, 
                      BOOL *bObjectCreated)
{
    CONTRACTL 
    {
        THROWS;
    }
    CONTRACTL_END;

    ARG_SLOT retValue = 0;
    *bObjectCreated = FALSE;
    TypeHandle nullTH;
    TypeHandle typeHnd;

    switch (type) {

    case SERIALIZATION_TYPE_BOOLEAN:
    case SERIALIZATION_TYPE_I1:
    case SERIALIZATION_TYPE_U1:
        if (*pBlob + 1 <= endBlob) {
            retValue = (ARG_SLOT)**pBlob;
            *pBlob += 1;
            break;
        }
        goto badBlob;

    case SERIALIZATION_TYPE_CHAR:
    case SERIALIZATION_TYPE_I2:
    case SERIALIZATION_TYPE_U2:
        if (*pBlob + 2 <= endBlob) {
            retValue = (ARG_SLOT)GET_UNALIGNED_VAL16(*pBlob);
            *pBlob += 2;
            break;
        }
        goto badBlob;

    case SERIALIZATION_TYPE_I4:
    case SERIALIZATION_TYPE_U4:
    case SERIALIZATION_TYPE_R4:
        if (*pBlob + 4 <= endBlob) {
            retValue = (ARG_SLOT)GET_UNALIGNED_VAL32(*pBlob);
            *pBlob += 4;
            break;
        }
        goto badBlob;

    case SERIALIZATION_TYPE_I8:
    case SERIALIZATION_TYPE_U8:
    case SERIALIZATION_TYPE_R8:
        if (*pBlob + 8 <= endBlob) {
            retValue = (ARG_SLOT)GET_UNALIGNED_VAL64(*pBlob);
            *pBlob += 8;
            break;
        }
        goto badBlob;

    case SERIALIZATION_TYPE_STRING:
    stringType:
    {
        int size = GetStringSize(pBlob, endBlob);
        *bObjectCreated = TRUE;
        if (size > 0) {
            if (*pBlob + size > endBlob) 
                goto badBlob;
            retValue = ObjToArgSlot(COMString::NewString((LPCUTF8)*pBlob, size));
            *pBlob += size;
        }
        else if (size == 0) 
            retValue = ObjToArgSlot(COMString::NewString(0));
        else
            *bObjectCreated = FALSE;

        break;
    }

    // this is coming back from sig but it's not a serialization type, 
    // essentialy the type in the blob and the type in the sig don't match
    case ELEMENT_TYPE_VALUETYPE:
    {
        if (!th.IsEnum()) 
            goto badBlob;
        CorSerializationType enumType = (CorSerializationType)th.GetInternalCorElementType();
        BOOL cannotBeObject = FALSE;
        retValue = GetDataFromBlob(pCtorAssembly, enumType, nullTH, pBlob, endBlob, pModule, &cannotBeObject);
        _ASSERTE(!cannotBeObject);
        break;
    }

    // this is coming back from sig but it's not a serialization type, 
    // essentialy the type in the blob and the type in the sig don't match
    case ELEMENT_TYPE_CLASS:
        if (th.IsArray())
            goto typeArray;
        else {
            MethodTable *pMT = th.AsMethodTable();
            if (pMT == g_Mscorlib.GetClass(CLASS__STRING)) 
                goto stringType;
            else if (pMT == g_Mscorlib.GetClass(CLASS__OBJECT)) 
                goto typeObject;
            else if (pMT == g_Mscorlib.GetClass(CLASS__TYPE)) 
                goto typeType;
        }

        goto badBlob;

    case SERIALIZATION_TYPE_TYPE:
    typeType:
    {
        typeHnd = GetTypeHandleFromBlob(pCtorAssembly, SERIALIZATION_TYPE_TYPE, pBlob, endBlob, pModule);
        if (!typeHnd.IsNull())
            retValue = ObjToArgSlot(typeHnd.GetManagedClassObject());
        *bObjectCreated = TRUE;
        break;
    }

    // this is coming back from sig but it's not a serialization type, 
    // essentialy the type in the blob and the type in the sig don't match
    case ELEMENT_TYPE_OBJECT:
    case SERIALIZATION_TYPE_TAGGED_OBJECT:
    typeObject:
    {
        // get the byte representing the real type and call GetDataFromBlob again
        if (*pBlob + 1 > endBlob) 
            goto badBlob;
        CorSerializationType objType = (CorSerializationType)**pBlob;
        *pBlob += 1;
        switch (objType) {
        case SERIALIZATION_TYPE_SZARRAY:
        {
            if (*pBlob + 1 > endBlob) 
                goto badBlob;
            CorSerializationType arrayType = (CorSerializationType)**pBlob;
            *pBlob += 1;
            if (arrayType == SERIALIZATION_TYPE_TYPE) 
                arrayType = (CorSerializationType)ELEMENT_TYPE_CLASS;
            // grab the array type and make a type handle for it
            nullTH = GetTypeHandleFromBlob(pCtorAssembly, arrayType, pBlob, endBlob, pModule);
        }
        case SERIALIZATION_TYPE_TYPE:
        case SERIALIZATION_TYPE_STRING:
            // notice that the nullTH is actually not null in the array case (see case above)
            retValue = GetDataFromBlob(pCtorAssembly, objType, nullTH, pBlob, endBlob, pModule, bObjectCreated);
            _ASSERTE(*bObjectCreated || retValue == 0);
            break;
        case SERIALIZATION_TYPE_ENUM:
        {
            //
            // get the enum type
            typeHnd = GetTypeHandleFromBlob(pCtorAssembly, SERIALIZATION_TYPE_ENUM, pBlob, endBlob, pModule);
            _ASSERTE(typeHnd.IsTypeDesc() == false);
            
            // ok we have the class, now we go and read the data
            MethodTable *pMT = typeHnd.AsMethodTable();
            PREFIX_ASSUME(pMT != NULL);
            CorSerializationType objNormType = (CorSerializationType)pMT->GetInternalCorElementType();
            BOOL isObject = FALSE;
            retValue = GetDataFromBlob(pCtorAssembly, objNormType, nullTH, pBlob, endBlob, pModule, &isObject);
            _ASSERTE(!isObject);
            retValue= ObjToArgSlot(pMT->Box((void*)&retValue));
            *bObjectCreated = TRUE;
            break;
        }
        default:
        {
            // the common primitive type case. We need to box the primitive
            typeHnd = GetTypeHandleFromBlob(pCtorAssembly, objType, pBlob, endBlob, pModule);
            _ASSERTE(typeHnd.IsTypeDesc() == false);
            retValue = GetDataFromBlob(pCtorAssembly, objType, nullTH, pBlob, endBlob, pModule, bObjectCreated);
            _ASSERTE(!*bObjectCreated);
            retValue= ObjToArgSlot(typeHnd.AsMethodTable()->Box((void*)&retValue));
            *bObjectCreated = TRUE;
            break;
        }
        }
        break;
    }

    case SERIALIZATION_TYPE_SZARRAY:      
    typeArray:
    {
        // read size
        BOOL isObject = FALSE;
        int size = (int)GetDataFromBlob(pCtorAssembly, SERIALIZATION_TYPE_I4, nullTH, pBlob, endBlob, pModule, &isObject);
        _ASSERTE(!isObject);
        
        if (size != -1) {
            CorSerializationType arrayType;
            if (th.IsEnum()) 
                arrayType = SERIALIZATION_TYPE_ENUM;
            else
                arrayType = (CorSerializationType)th.GetInternalCorElementType();
        
            BASEARRAYREF array = NULL;
            GCPROTECT_BEGIN(array);
            ReadArray(pCtorAssembly, arrayType, size, th, pBlob, endBlob, pModule, &array);
            retValue = ObjToArgSlot(array);
            GCPROTECT_END();
        }
        *bObjectCreated = TRUE;
        break;
    }

    default:
    badBlob:
        COMPlusThrow(kCustomAttributeFormatException);
    }

    return retValue;
}

FCIMPL2(VOID, COMCustomAttribute::PushSecurityContextFrame, SecurityContextFrame *pFrame, AssemblyBaseObject *pAssemblyObjectUNSAFE)
{
    CONTRACTL
    {
        MODE_COOPERATIVE;
        DISABLED(GC_TRIGGERS);
        THROWS;
        SO_TOLERANT;
    }
    CONTRACTL_END;

    BEGIN_SO_INTOLERANT_CODE_NOTHROW(GetThread(), FCThrowVoid(kStackOverflowException));

    // Adjust frame pointer for the presence of the GSCookie at a negative
    // offset (it's hard for us to express neginfo in the managed definition of
    // the frame).
    pFrame = (SecurityContextFrame*)((BYTE*)pFrame + sizeof(GSCookie));

    *((TADDR*)pFrame) = SecurityContextFrame::GetMethodFrameVPtr();
    pFrame->SetAssembly(pAssemblyObjectUNSAFE->GetAssembly());
    *pFrame->GetGSCookiePtr() = GetProcessGSCookie();
    pFrame->Push();

    END_SO_INTOLERANT_CODE;
}
FCIMPLEND

FCIMPL1(VOID, COMCustomAttribute::PopSecurityContextFrame, SecurityContextFrame *pFrame)
{
    CONTRACTL
    {
        MODE_COOPERATIVE;
        DISABLED(GC_TRIGGERS);
        THROWS;
        SO_TOLERANT;
    }
    CONTRACTL_END;

    BEGIN_SO_INTOLERANT_CODE_NOTHROW(GetThread(), FCThrowVoid(kStackOverflowException));

    // Adjust frame pointer for the presence of the GSCookie at a negative
    // offset (it's hard for us to express neginfo in the managed definition of
    // the frame).
    pFrame = (SecurityContextFrame*)((BYTE*)pFrame + sizeof(GSCookie));

    pFrame->Pop();

    END_SO_INTOLERANT_CODE;
}
FCIMPLEND
