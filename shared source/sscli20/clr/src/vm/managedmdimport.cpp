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

// TODO: Re-implement with MC++ if we ever compile any mscorlib code with that

#include "common.h"
#include "mlinfo.h"
#include "managedmdimport.hpp"
#include "comstring.h"
#include "wrappers.h"

#define GCPROTECT_INTERIOR_1(P0,A0)  \
        struct                              \
        {                                   \
            P0 A0;                          \
        } gc;                               \
        ZeroMemory( &gc, sizeof( gc ) );    \
        gc.A0 = A0;                         \
        GCPROTECT_BEGININTERIOR( gc );              

#define GCPROTECT_INTERIOR_2(P0,A0, P1,A1)  \
        struct                              \
        {                                   \
            P0 A0;                          \
            P1 A1;                          \
        } gc;                               \
        ZeroMemory( &gc, sizeof( gc ) );    \
        gc.A0 = A0;                         \
        gc.A1 = A1;                         \
        GCPROTECT_BEGININTERIOR( gc );              

#define GCPROTECT_INTERIOR_3(P0,A0, P1,A1, P2,A2)  \
        struct                              \
        {                                   \
            P0 A0;                          \
            P1 A1;                          \
            P2 A2;                          \
        } gc;                               \
        ZeroMemory( &gc, sizeof( gc ) );    \
        gc.A0 = A0;                         \
        gc.A1 = A1;                         \
        gc.A2 = A2;                         \
        GCPROTECT_BEGININTERIOR( gc );              

#define GCPROTECT_INTERIOR_4(P0,A0, P1,A1, P2,A2, P3,A3)  \
        struct                              \
        {                                   \
            P0 A0;                          \
            P1 A1;                          \
            P2 A2;                          \
            P3 A3;                          \
        } gc;                               \
        ZeroMemory( &gc, sizeof( gc ) );    \
        gc.A0 = A0;                         \
        gc.A1 = A1;                         \
        gc.A2 = A2;                         \
        gc.A3 = A3;                         \
        GCPROTECT_BEGININTERIOR( gc );              

#define HELPER_METHOD_FRAME_BEGININTERIOR_1(P0,A0)          \
    HELPER_METHOD_FRAME_BEGIN_0();                          \
    GCPROTECT_INTERIOR_1(P0,A0);

#define HELPER_METHOD_FRAME_BEGININTERIOR_2(P0,A0, P1,A1)   \
    HELPER_METHOD_FRAME_BEGIN_0();                          \
    GCPROTECT_INTERIOR_2(P0,A0, P1,A1);

#define HELPER_METHOD_FRAME_BEGININTERIOR_3(P0,A0, P1,A1, P2,A2)    \
    HELPER_METHOD_FRAME_BEGIN_0();                                  \
    GCPROTECT_INTERIOR_3(P0,A0, P1,A1, P2,A2);

#define HELPER_METHOD_FRAME_BEGININTERIOR_4(P0,A0, P1,A1, P2,A2, P3,A3) \
    HELPER_METHOD_FRAME_BEGIN_0();                                      \
    GCPROTECT_INTERIOR_4(P0,A0, P1,A1, P2,A2, P3,A3);
    
#define HELPER_METHOD_FRAME_ENDINTERIOR()   \
    GCPROTECT_END();                        \
    HELPER_METHOD_FRAME_END();

#define STACK_BUFFER_SIZE 128
#define MdInvokeR(EXPR) \
    do { MDGlue glue(pScope); hr = (glue.EXPR); if(FAILED(hr)) { HELPER_METHOD_FRAME_BEGIN_RET_0(); ThrowMetaDataImportException(hr); HELPER_METHOD_FRAME_END(); } } while (0)
#define MdInvokeV(EXPR) \
    do { MDGlue glue(pScope); hr = (glue.EXPR); if(FAILED(hr)) { HELPER_METHOD_FRAME_BEGIN_0(); ThrowMetaDataImportException(hr); HELPER_METHOD_FRAME_END(); } } while (0)
#define MdInvoke(EXPR) \
    do { MDGlue glue(pScope); hr = (glue.EXPR); if(FAILED(hr)) { ThrowMetaDataImportException(hr); } } while (0)
#define MdInvokeRet(EXPR) \
    do { MdInvokeV(EXPR); if (true) { return; } } while (0) 
#define MdInvokeStr(EXPR) \
    do { MdInvoke(EXPR); str.CloseString(); } while (0) 
#define MdInvokeIfTrunc(EXPR) \
    do { if (hr == CLDB_S_TRUNCATION) { MdInvoke(EXPR); str.CloseString(); } } while (0) 

#define DECL_I4_ARRM(NAME, ARG) I4ArrayMarshaler NAME(&gc.ARG);
#define DECL_I4_ARRMC(NAME, ARG, COUNT) I4ArrayMarshaler NAME(&gc.ARG, COUNT);

#define DECL_STR(NAME) StringMarshaler str(&gc.NAME);
#define GET_STR str.OpenString(), str.GetLength(), str.GetOutLength()
#define SKIP_STR NULL, 0, NULL 

#define DECL_CONST_UTF8_STR(NAME) ConstStringMarshaler str(&gc.NAME, TRUE);
#define DECL_CONST_ANSI_STR(NAME) ConstStringMarshaler str(&gc.NAME, FALSE);
#define GET_CONST_STR str.GetString()

#define GET_OUT_ARR_SIZE(NAME) NULL, 0, NAME.GetOutCount()
#define GET_OUT_ARR(NAME) NAME.GetOutArray(), NAME.GetCount(), NAME.GetOutCount()
#define SKIP_OUT_ARR NULL, 0, NULL

#define GET_SIG(NAME) (PCCOR_SIGNATURE*)NAME.GetOutArray(), NAME.GetOutCount()
#define SKIP_SIG NULL, 0

#define GET_VAL(NAME) (void const **)NAME.GetOutArray(), NAME.GetOutCount()
#define INVERTED_GET_VAL(NAME) NAME.GetOutCount(), (void const **)NAME.GetOutArray()
#define MDTABLE_GET_VAL(NAME) NAME.GetOutCount(), (ULONG **)NAME.GetOutArray()
#define SKIP_VAL NULL, 0

#define CONST_SIG(NAME) (PCCOR_SIGNATURE)NAME.m_array, NAME.m_count
#define GET_CONST_ARRAY(NAME) &NAME->m_array, &NAME->m_count

#define GET_ARR(NAME) NAME->GetDataPtr(), NAME->GetNumComponents()
#define GET_ZARR(NAME) NAME->GetDataPtr()

#define GET_SZ_BUFFER(SZ) SZ != NULL ? SZ->GetBuffer() : NULL

extern HRESULT CountEnum(HCORENUM hEnum, ULONG *pulCount);
   
#define MD_ENUM_ALL(FCIMPL, HOLDER, INTERFACE, NAME, TARGET_TYPE)                                           \
    FCIMPL(void, INTERFACE::NAME, I4ARRAYREF* ppResult, DWORD* pCount)                                      \
    {                                                                                                       \
        CONTRACTL                                                                                           \
        {                                                                                                   \
            MODE_COOPERATIVE;                                                                               \
            DISABLED(GC_TRIGGERS);                                                                          \
            SO_TOLERANT;                                                                             \
            THROWS;                                                                                         \
        }                                                                                                   \
        CONTRACTL_END;                                                                                      \
                                                                                                            \
        SKIP_CHECK_ARGUMENT(ppResult);                                                                      \
        SKIP_CHECK_ARGUMENT(pCount);                                                                        \
                                                                                                            \
        HELPER_METHOD_FRAME_BEGININTERIOR_1(I4ARRAYREF*,ppResult);                                          \
        {                                                                                                   \
            HOLDER hEnum(pScope);                                                                           \
            ULONG count = 0;                                                                                \
            HRESULT hr = S_OK;                                                                              \
            MdInvoke(NAME(&hEnum, NULL, 0, &count));                                                        \
                                                                                                            \
            if (FAILED(CountEnum(hEnum, &count)))                                                           \
                ThrowMetaDataImportException(hr);                                                           \
                                                                                                            \
            if (pCount)                                                                                     \
                *pCount = count;                                                                            \
                                                                                                            \
            if (ppResult)                                                                                   \
            {                                                                                               \
                *ppResult = NULL;                                                                           \
                                                                                                            \
                DECL_I4_ARRMC(result, ppResult, count);                                                     \
                                                                                                            \
                MdInvoke(NAME(&hEnum, (TARGET_TYPE*)GET_OUT_ARR(result)));                                  \
            }                                                                                               \
        }                                                                                                   \
        HELPER_METHOD_FRAME_ENDINTERIOR();                                                                  \
    }                                                                                                       \
    FCIMPLEND

#define MD_ENUM_ON_TARGET(NAME, SOURCE_TYPE, TARGET_TYPE)                                                   \
    MDImpl3(void, MetaDataImport::NAME, SOURCE_TYPE tk, I4ARRAYREF* ppResult, DWORD* pCount)                \
    {                                                                                                       \
        CONTRACTL                                                                                           \
        {                                                                                                   \
            MODE_COOPERATIVE;                                                                               \
            DISABLED(GC_TRIGGERS);                                                                          \
            SO_TOLERANT;                                                                             \
            THROWS;                                                                                         \
        }                                                                                                   \
        CONTRACTL_END;                                                                                      \
                                                                                                            \
        SKIP_CHECK_ARGUMENT(ppResult);                                                                      \
        SKIP_CHECK_ARGUMENT(pCount);                                                                        \
                                                                                                            \
        HELPER_METHOD_FRAME_BEGININTERIOR_1(I4ARRAYREF*,ppResult);                                          \
        {                                                                                                   \
            HCorEnumHolder hEnum(pScope);                                                                   \
            ULONG count = 0;                                                                                \
            HRESULT hr = S_OK;                                                                              \
            MdInvoke(NAME(&hEnum, tk, NULL, 0, &count));                                                    \
            MdInvoke(CountEnum(hEnum, &count));                                                             \
                                                                                                            \
            if (pCount)                                                                                     \
                *pCount = count;                                                                            \
                                                                                                            \
            if (ppResult)                                                                                   \
            {                                                                                               \
                DECL_I4_ARRMC(result, ppResult, count);                                                     \
                                                                                                            \
                MdInvoke(NAME(&hEnum, tk, (TARGET_TYPE*)GET_OUT_ARR(result)));                              \
            }                                                                                               \
        }                                                                                                   \
        HELPER_METHOD_FRAME_ENDINTERIOR();                                                                  \
    }                                                                                                       \
    FCIMPLEND                                                                                               

void ThrowMetaDataImportException(HRESULT hr)
{    
    WRAPPER_CONTRACT;

    if (hr == CLDB_E_RECORD_NOTFOUND)
        return;
   
    MethodDescCallSite throwError(METHOD__METADATA_IMPORT__THROW_ERROR);
    
    ARG_SLOT args[] = { hr };
    throwError.Call(args);
}

#define SKIP_CHECK_ARGUMENT(ARGUMENT) SkipCheckArgument(pSkipAddresses, &ARGUMENT);

void SkipCheckArgument(SkipAddresses* pSkipAddresses, void** ppArgument)
{
    LEAF_CONTRACT;
   
    if (*ppArgument >= (void*)pSkipAddresses && *ppArgument < (void*)(((BYTE*)pSkipAddresses) + sizeof(SkipAddresses)))
        *ppArgument = NULL;
}

void SkipCheckArgument(SkipAddresses* pSkipAddresses, DWORD** ppArg)
{
    WRAPPER_CONTRACT;
    
    SkipCheckArgument(pSkipAddresses, (void**)ppArg);
}

void SkipCheckArgument(SkipAddresses* pSkipAddresses, GUID** ppArg)
{
    WRAPPER_CONTRACT;

    SkipCheckArgument(pSkipAddresses, (void**)ppArg);
}

void SkipCheckArgument(SkipAddresses* pSkipAddresses, ASSEMBLYMETADATA** ppArg)
{
    WRAPPER_CONTRACT;

    SkipCheckArgument(pSkipAddresses, (void**)ppArg);
}

#if !defined(PLATFORM_UNIX)
void SkipCheckArgument(SkipAddresses* pSkipAddresses, mdToken** ppArg)
{
    WRAPPER_CONTRACT;

    SkipCheckArgument(pSkipAddresses, (void**)ppArg);
}
#endif

void SkipCheckArgument(SkipAddresses* pSkipAddresses, ConstArray** ppArg)
{
    WRAPPER_CONTRACT;

    SkipCheckArgument(pSkipAddresses, (void**)ppArg);
}

void SkipCheckArgument(SkipAddresses* pSkipAddresses, STRINGREF** ppArg)
{
    WRAPPER_CONTRACT;

    SkipCheckArgument(pSkipAddresses, (void**)ppArg);
}

void SkipCheckArgument(SkipAddresses* pSkipAddresses, I4ARRAYREF** ppArg)
{
    WRAPPER_CONTRACT;

    SkipCheckArgument(pSkipAddresses, (void**)ppArg);
}

class I4ArrayMarshaler
{        
public:
    
    I4ArrayMarshaler(I4ARRAYREF** pppArray, ULONG count = 0) 
    { 
        WRAPPER_CONTRACT;
        
        m_pppArray = pppArray; 
        m_count = count;
    }

    ULONG* GetOutCount() 
    { 
        LEAF_CONTRACT;

        if (*m_pppArray == NULL)
            return NULL;

        return &m_count; 
    }
    
    ULONG GetCount() 
    { 
        LEAF_CONTRACT;

        if (*m_pppArray == NULL)
            return NULL;

        return m_count; 
    }
    
    void* GetOutArray()
    { 
        WRAPPER_CONTRACT;
        
        if (*m_pppArray == NULL || m_count == 0)
            return NULL;

        SetObjectReference((OBJECTREF*)*m_pppArray, AllocatePrimitiveArray(ELEMENT_TYPE_I4, m_count), GetAppDomain());
        return (void*)(**m_pppArray)->GetDataPtr();
    }

    ~I4ArrayMarshaler()
    {
        WRAPPER_CONTRACT;
        
        // GCHeap::GetGCHeap()->GarbageCollect(0);
        if (*m_pppArray != NULL && m_count == 0)
            SetObjectReference((OBJECTREF*)*m_pppArray, AllocatePrimitiveArray(ELEMENT_TYPE_I4, m_count), GetAppDomain());
    }

private:
    ULONG m_count;
    I4ARRAYREF** m_pppArray;
};

class ObjectArrayMarshaler
{        
public:

    ObjectArrayMarshaler(PtrArray** ppArray, TypeHandle type) 
    { 
        WRAPPER_CONTRACT;
        
        m_type = type;
        m_ppArray = ppArray; 
        m_count = 0;
    }
        
    ULONG* GetOutCount() 
    { 
        LEAF_CONTRACT;

        if (!m_ppArray)
            return NULL;

        return &m_count; 
    }
    
    ULONG GetCount() 
    { 
        LEAF_CONTRACT;

        if (!m_ppArray)
            return NULL;

        return m_count; 
    }
    
    void* GetOutArray()
    { 
        WRAPPER_CONTRACT;
        
        if (!m_ppArray)
            return NULL;

        *m_ppArray = (PtrArray*)OBJECTREFToObject(AllocateObjectArray(m_count, m_type));
        return (void*)(*m_ppArray)->GetDataPtr();
    }

private:
    TypeHandle m_type;
    ULONG m_count;
    PtrArray** m_ppArray;
};

class ConstI4ArrayMarshaler
{        
public:

    ConstI4ArrayMarshaler(ConstArray* pArray) 
    { 
        LEAF_CONTRACT;
        
        m_pArray = pArray; 
    }
    
    ULONG* GetOutCount() 
    { 
        LEAF_CONTRACT;

        if (m_pArray == NULL)
            return NULL;

        return (ULONG*)&m_pArray->m_count; 
    }

    void* GetOutArray()
    { 
        LEAF_CONTRACT;

        if (m_pArray == NULL)
            return NULL;

        return &m_pArray->m_array;
    }

private:
    ConstArray* m_pArray;
};

class ConstStringMarshaler
{        
public:

    ConstStringMarshaler(STRINGREF** pppString, BOOL bIsUtf8)
    {
        LEAF_CONTRACT;
        
        m_pppString = pppString;

        if (*m_pppString)
            **m_pppString = NULL;

        m_pString = NULL;
        m_bIsUtf8 = bIsUtf8;
    }
    
    const char** GetString() 
    { 
        LEAF_CONTRACT;

        if (!*m_pppString)
            return 0; 

        return (const char**)&m_pString;
    }

    ~ConstStringMarshaler()
    { 
        WRAPPER_CONTRACT;
        
        if (*m_pppString)
        {
            if (m_bIsUtf8)
            {
                InlineSString<STACK_BUFFER_SIZE> string(SString::Utf8, m_pString);
                SetObjectReference((OBJECTREF*)*m_pppString, COMString::NewString(m_pString), GetAppDomain());
            }
            else
            {
                InlineSString<STACK_BUFFER_SIZE> string(SString::Ansi, m_pString);
                SetObjectReference((OBJECTREF*)*m_pppString, COMString::NewString(m_pString), GetAppDomain());
            }
        }
    }

private:
    BOOL m_bIsUtf8;
    char* m_pString;
    STRINGREF** m_pppString;
};

class StringMarshaler
{        
public:

    StringMarshaler(STRINGREF** pppString) 
    {
        WRAPPER_CONTRACT;
            
        m_pppString = pppString;
        m_bIsBufferOpen = FALSE;        
        m_length = STACK_BUFFER_SIZE;
        m_outLength = 0;
    }
    
    ULONG GetLength() 
    { 
        LEAF_CONTRACT;
        
        if (*m_pppString == NULL)
            return 0; 

        return m_length;
    }

    ULONG* GetOutLength() 
    { 
        LEAF_CONTRACT;

        if (*m_pppString == NULL)
            return NULL; 

        return &m_outLength; 
    }

    WCHAR* OpenString() 
    {   
        WRAPPER_CONTRACT;

        if (*m_pppString == NULL)
            return NULL; 
    
        m_bIsBufferOpen = TRUE;
        return m_buffer.OpenUnicodeBuffer(m_length);
    } 

    void CloseString()
    {
        WRAPPER_CONTRACT;

        if (*m_pppString == NULL)
            return; 

        m_bIsBufferOpen = FALSE;
        m_buffer.CloseBuffer(m_outLength < m_length ? m_outLength : m_length);
        m_length = m_outLength;
    }

    ~StringMarshaler()
    { 
        WRAPPER_CONTRACT;

        if (m_bIsBufferOpen)
            m_buffer.CloseBuffer();
        
        else if (*m_pppString)
            SetObjectReference((OBJECTREF*)*m_pppString, COMString::NewString(m_buffer), GetAppDomain());
    }

private:
    BOOL m_bIsBufferOpen;
    StackSString m_buffer;
    ULONG m_length;
    ULONG m_outLength;
    STRINGREF** m_pppString;
};

//
// Enumerators
//
#define MDIMPORT_ENUM_ALL(NAME, TARGET_TYPE) MD_ENUM_ALL(MDImpl2, HCorEnumHolder, MetaDataImport, NAME, TARGET_TYPE)
#define MDASMIMPORT_ENUM_ALL(NAME, TARGET_TYPE) MD_ENUM_ALL(MDAIImpl2, HCorAsmEnumHolder, NativeMetaDataAssemblyImport, NAME, TARGET_TYPE)

//
// MetaDataImport
//
extern BOOL ParseNativeTypeInfo(NativeTypeParamInfo* pInfo, PCCOR_SIGNATURE pvNativeType, ULONG cbNativeType);

FCIMPL2(void*, MetaDataImport::QueryInterface, const IID* pIID, IUnknown* pScope)
{
    WRAPPER_CONTRACT;

    HRESULT hr = S_OK;
    IUnknown*pUnk = NULL;

    // QI for the assembly import interface.
    hr = pScope->QueryInterface(*pIID, (void**)&pUnk);
    if (FAILED(hr))
        goto ErrExit;

ErrExit:
    if (FAILED(hr))
    {
        HELPER_METHOD_FRAME_BEGIN_RET_0();
        ThrowMetaDataImportException(hr);
        HELPER_METHOD_FRAME_END();
    }
    
    return pUnk;
}
FCIMPLEND

FCIMPL11(void, MetaDataImport::GetMarshalAs,
    BYTE*           pvNativeType,
    ULONG           cbNativeType,
    INT32*          unmanagedType,
    INT32*          safeArraySubType,
    STRINGREF*      safeArrayUserDefinedSubType,
    INT32*          arraySubType,   
    INT32*          sizeParamIndex,
    INT32*          sizeConst,
    STRINGREF*      marshalType,
    STRINGREF*      marshalCookie,
    INT32*          iidParamIndex)
{
    CONTRACTL                 
    {                         
        MODE_COOPERATIVE;     
        DISABLED(GC_TRIGGERS);
        SO_TOLERANT;   
        THROWS;               
    }                         
    CONTRACTL_END;            

    NativeTypeParamInfo info;

    ZeroMemory(&info, sizeof(NativeTypeParamInfo));

    BEGIN_SO_INTOLERANT_CODE_NOTHROW(GetThread(), FCThrowVoid(kStackOverflowException));
    {
        if (!ParseNativeTypeInfo(&info, pvNativeType, cbNativeType))
            ThrowMetaDataImportException(E_FAIL);
    }
    END_SO_INTOLERANT_CODE;
    
    HELPER_METHOD_FRAME_BEGIN_0();
    {       
        *unmanagedType = info.m_NativeType;
        *sizeParamIndex = info.m_CountParamIdx;
        *sizeConst = info.m_Additive;
        *arraySubType = info.m_ArrayElementType;

        *iidParamIndex = -1;

        *safeArraySubType = VT_EMPTY;

        *safeArrayUserDefinedSubType = NULL;
        
        *marshalType = info.m_strCMMarshalerTypeName == NULL ? NULL :
            COMString::NewString(info.m_strCMMarshalerTypeName, info.m_cCMMarshalerTypeNameBytes);
            
        *marshalCookie = info.m_strCMCookie == NULL ? NULL : 
            COMString::NewString(info.m_strCMCookie, info.m_cCMCookieStrBytes);
    }
    HELPER_METHOD_FRAME_END();
}
FCIMPLEND

MDImpl4(void, MetaDataImport::GetDefaultValue, mdToken tk, INT64* pDefaultValue, INT32* pLength, INT32* pCorElementType)
{   
    CONTRACTL {
        THROWS;
        DISABLED(GC_TRIGGERS);
        MODE_COOPERATIVE;
        SO_TOLERANT;
    }
    CONTRACTL_END;

    BEGIN_SO_INTOLERANT_CODE_NOTHROW(GetThread(), FCThrowVoid(kStackOverflowException));

    IMDInternalImport* _pScope = pScope;

    MDDefaultValue value;
    _pScope->GetDefaultValue(tk, &value);

    *pDefaultValue = value.m_ullValue;
    *pCorElementType = (UINT32)value.m_bType;
    *pLength = (INT32)value.m_cbSize;

    END_SO_INTOLERANT_CODE;
}
FCIMPLEND

MDImpl3(void, MetaDataImport::GetCustomAttributeProps, mdCustomAttribute cv, mdToken* ptkType, ConstArray* ppBlob)
{
    CONTRACTL {
        THROWS;
        DISABLED(GC_TRIGGERS);
        MODE_COOPERATIVE;
        SO_TOLERANT;
    }
    CONTRACTL_END;

    BEGIN_SO_INTOLERANT_CODE_NOTHROW(GetThread(), FCThrowVoid(kStackOverflowException));

    IMDInternalImport* _pScope = pScope;

    _pScope->GetCustomAttributeProps(cv, ptkType);    
    _pScope->GetCustomAttributeAsBlob(cv, (const void **)&ppBlob->m_array, (ULONG *)&ppBlob->m_count);    

    END_SO_INTOLERANT_CODE;
}
FCIMPLEND
                                                                                   
MDImpl3(void, MetaDataImport::EnumCount, mdToken type, mdToken tkParent, DWORD* pCount)                
{                                                                                                       
    CONTRACTL {
        THROWS;
        DISABLED(GC_TRIGGERS);
        MODE_COOPERATIVE;
        SO_TOLERANT;
        PRECONDITION(CheckPointer(pCount));
    }
    CONTRACTL_END;

    BEGIN_SO_INTOLERANT_CODE_NOTHROW(GetThread(), FCThrowVoid(kStackOverflowException));
    {
        IMDInternalImport* _pScope = pScope;
                                                                                                                                                                                                           
        if (TypeFromToken(type) == mdtTypeDef)
        {
            ULONG count = _pScope->GetCountNestedClasses(tkParent);
            *pCount = count;
        }
        else if ((TypeFromToken(tkParent) == mdtProperty || TypeFromToken(tkParent) == mdtEvent) && TypeFromToken(type) == mdtMethodDef)
        {
            HENUMInternalHolder hEnum(pScope);                                                                   
            ULONG count = 0;                                                                                
            hEnum.EnumAssociateInit(tkParent);        
            count = _pScope->EnumGetCount(&hEnum);                                                                                                        
            *pCount = count;                                                                            
        }
        else
        {
            HENUMInternalHolder hEnum(pScope);                                                                   
            ULONG count = 0;                                                                                
            hEnum.EnumInit(type, tkParent);        
            count = _pScope->EnumGetCount(&hEnum);        // [IN] the enumerator to retrieve information                                                                                                          
            *pCount = count;                                                                            
        }
    }
    END_SO_INTOLERANT_CODE;
}                                                                                                       
FCIMPLEND
    

MDImpl4(void, MetaDataImport::Enum, mdToken type, mdToken tkParent, DWORD* pResult, DWORD count)                
{                                                                                                       
    CONTRACTL {
        THROWS;
        DISABLED(GC_TRIGGERS);
        MODE_COOPERATIVE;
        SO_TOLERANT;
        PRECONDITION(CheckPointer(pResult, NULL_OK));
        PRECONDITION(!count || pResult);
    }
    CONTRACTL_END;

    if (!count)
        return;

    BEGIN_SO_INTOLERANT_CODE_NOTHROW(GetThread(), FCThrowVoid(kStackOverflowException));
    {
        IMDInternalImport* _pScope = pScope;
                                                                                                                                                                                                           
        if (TypeFromToken(type) == mdtTypeDef)
        {
            ASSERT(count == (DWORD)_pScope->GetCountNestedClasses(tkParent));

            mdTypeDef* arToken = (mdTypeDef*)pResult;
            _pScope->GetNestedClasses(tkParent, arToken, count);                                                                                                           
        }
        else if ((TypeFromToken(tkParent) == mdtProperty || TypeFromToken(tkParent) == mdtEvent) && TypeFromToken(type) == mdtMethodDef)
        {
            HENUMInternalHolder hEnum(pScope);                                                                   
            hEnum.EnumAssociateInit(tkParent);
            
            ASSERT(count == (DWORD)_pScope->EnumGetCount(&hEnum));
                                                                                                            
            ASSOCIATE_RECORD* arAssocRecord = (ASSOCIATE_RECORD*)pResult;
            _pScope->GetAllAssociates(&hEnum, arAssocRecord, count);
        }
        else
        {
            HENUMInternalHolder hEnum(pScope);                                                                   
            hEnum.EnumInit(type, tkParent);
            
            ASSERT(count == (DWORD)_pScope->EnumGetCount(&hEnum));
                                                                                                            
            mdToken* arToken = (mdToken*)pResult;
            for(COUNT_T i = 0; i < count && _pScope->EnumNext(&hEnum, &arToken[i]); i++)
                ;
        }
    }
    END_SO_INTOLERANT_CODE;   
}                                                                                                       
FCIMPLEND                                                                                               

MDImpl1(FC_BOOL_RET, MetaDataImport::IsValidToken, mdToken tk)
{
    CONTRACTL {
        THROWS;
        DISABLED(GC_TRIGGERS);
        MODE_COOPERATIVE;
        SO_TOLERANT;
    }
    CONTRACTL_END;

    IMDInternalImport* _pScope = pScope;
    
    FC_RETURN_BOOL(_pScope->IsValidToken(tk));
}
FCIMPLEND 

MDImpl3(void, MetaDataImport::GetClassLayout, mdTypeDef td, DWORD* pdwPackSize, ULONG* pulClassSize)
{
    CONTRACTL {
        THROWS;
        DISABLED(GC_TRIGGERS);
        MODE_COOPERATIVE;
        SO_TOLERANT;
    }
    CONTRACTL_END;

    BEGIN_SO_INTOLERANT_CODE_NOTHROW(GetThread(), FCThrowVoid(kStackOverflowException));
    {
        IMDInternalImport* _pScope = pScope;
        
        if (pdwPackSize)
            _pScope->GetClassPackSize(td, (ULONG*)pdwPackSize);
        
        if (pulClassSize)
            _pScope->GetClassTotalSize(td, pulClassSize);
    }
    END_SO_INTOLERANT_CODE;
}
FCIMPLEND 

MDImpl3(FC_BOOL_RET, MetaDataImport::GetFieldOffset, mdTypeDef td, mdFieldDef target, DWORD* pdwFieldOffset)
{
    CONTRACTL {
        THROWS;
        DISABLED(GC_TRIGGERS);
        MODE_COOPERATIVE;
        SO_TOLERANT;
    }
    CONTRACTL_END;

    IMDInternalImport* _pScope = pScope;
       
    MD_CLASS_LAYOUT layout;
    BOOL retVal = FALSE;
    
    BEGIN_SO_INTOLERANT_CODE_NOTHROW(GetThread(), FCThrow(kStackOverflowException));
    _pScope->GetClassLayoutInit(td, &layout);
    
    ULONG cFieldOffset = layout.m_ridFieldEnd - layout.m_ridFieldCur;
    
    for (COUNT_T i = 0; i < cFieldOffset; i ++)
    {
        mdFieldDef fd;
        ULONG offset;
        _pScope->GetClassLayoutNext(&layout, &fd, &offset);

        if (fd == target)
        {
            *pdwFieldOffset = offset;
            retVal = TRUE;
            break;
        }
    }
    END_SO_INTOLERANT_CODE;

    FC_RETURN_BOOL(retVal);
}
FCIMPLEND 

MDImpl3(void, MetaDataImport::GetUserString, mdToken tk, LPCSTR* pszName, ULONG* pCount)
{
    CONTRACTL {
        THROWS;
        DISABLED(GC_TRIGGERS);
        MODE_COOPERATIVE;
        SO_TOLERANT;
    }
    CONTRACTL_END;

    IMDInternalImport* _pScope = pScope;
    BOOL bHasExtendedChars;
    BEGIN_SO_INTOLERANT_CODE_NOTHROW(GetThread(), FCThrowVoid(kStackOverflowException));
    *pszName = (LPCSTR)_pScope->GetUserString(tk, pCount, &bHasExtendedChars);
    END_SO_INTOLERANT_CODE;
}
FCIMPLEND 

MDImpl2(void, MetaDataImport::GetName, mdToken tk, LPCSTR* pszName)
{   
    CONTRACTL {
        THROWS;
        DISABLED(GC_TRIGGERS);
        MODE_COOPERATIVE;
        SO_TOLERANT;
    }
    CONTRACTL_END;

    IMDInternalImport* _pScope = pScope;

    BEGIN_SO_INTOLERANT_CODE_NOTHROW(GetThread(), FCThrowVoid(kStackOverflowException));
    if (TypeFromToken(tk) == mdtMethodDef)
        *pszName = _pScope->GetNameOfMethodDef(tk);
    else if (TypeFromToken(tk) == mdtParamDef)
    {
        USHORT seq;
        DWORD attr;
        *pszName = _pScope->GetParamDefProps(tk, &seq, &attr);
    }
    else if (TypeFromToken(tk) == mdtFieldDef)
        *pszName = _pScope->GetNameOfFieldDef(tk);
    else if (TypeFromToken(tk) == mdtProperty)
        _pScope->GetPropertyProps(tk, pszName, NULL, NULL, NULL);
    else if (TypeFromToken(tk) == mdtEvent)
        _pScope->GetEventProps(tk, pszName, NULL, NULL);
    else if (TypeFromToken(tk) == mdtModule)
        _pScope->GetModuleRefProps(tk, pszName);
    else if (TypeFromToken(tk) == mdtTypeDef)
    {
        LPCSTR szNamespace = NULL;
        _pScope->GetNameOfTypeDef(tk, pszName, &szNamespace);
    }
    else
        ASSERT(!"Unreachable");
    END_SO_INTOLERANT_CODE;
}
FCIMPLEND

MDImpl2(void, MetaDataImport::GetNamespace, mdToken tk, LPCSTR* pszName)
{   
    CONTRACTL {
        THROWS;
        DISABLED(GC_TRIGGERS);
        MODE_COOPERATIVE;
        SO_TOLERANT;
    }
    CONTRACTL_END;

    IMDInternalImport* _pScope = pScope;

    LPCSTR szName = NULL;
    BEGIN_SO_INTOLERANT_CODE_NOTHROW(GetThread(), FCThrowVoid(kStackOverflowException));
    _pScope->GetNameOfTypeDef(tk, &szName, pszName);
    END_SO_INTOLERANT_CODE;
}
FCIMPLEND
    

MDImpl2(void, MetaDataImport::GetGenericParamProps, mdToken tk, DWORD* pAttributes)
{
    CONTRACTL {
        THROWS;
        DISABLED(GC_TRIGGERS);
        MODE_COOPERATIVE;
        SO_TOLERANT;
    }
    CONTRACTL_END;

    IMDInternalImport* _pScope = pScope;    
    BEGIN_SO_INTOLERANT_CODE_NOTHROW(GetThread(), FCThrowVoid(kStackOverflowException));
    _pScope->GetGenericParamProps(tk, NULL, pAttributes, NULL, NULL, NULL);    
    END_SO_INTOLERANT_CODE;
}
FCIMPLEND

MDImpl3(void, MetaDataImport::GetEventProps, mdToken tk, LPCSTR* pszName, INT32 *pdwEventFlags)
{   
    CONTRACTL {
        THROWS;
        DISABLED(GC_TRIGGERS);
        MODE_COOPERATIVE;
        SO_TOLERANT;
    }
    CONTRACTL_END;

    IMDInternalImport* _pScope = pScope;
    BEGIN_SO_INTOLERANT_CODE_NOTHROW(GetThread(), FCThrowVoid(kStackOverflowException));
    _pScope->GetEventProps(tk, pszName, (DWORD*)pdwEventFlags, NULL);
    END_SO_INTOLERANT_CODE;
}
FCIMPLEND

MDImpl4(void, MetaDataImport::GetPinvokeMap, mdToken tk, DWORD* pMappingFlags, LPCSTR* pszImportName, LPCSTR* pszImportDll)
{   
    CONTRACTL {
        THROWS;
        DISABLED(GC_TRIGGERS);
        MODE_COOPERATIVE;
        SO_TOLERANT;
    }
    CONTRACTL_END;

    IMDInternalImport* _pScope = pScope;
    mdModule tkModule;
    BEGIN_SO_INTOLERANT_CODE_NOTHROW(GetThread(), FCThrowVoid(kStackOverflowException));
    HRESULT hr = _pScope->GetPinvokeMap(tk, pMappingFlags, pszImportName, &tkModule);
    if (FAILED(hr))
    {
        *pMappingFlags = 0;
        *pszImportName = NULL;
        *pszImportDll = NULL;
    }
    else
    {
        _pScope->GetModuleRefProps(tkModule, pszImportDll);
    }
    END_SO_INTOLERANT_CODE;
}
FCIMPLEND
    
MDImpl3(void, MetaDataImport::GetParamDefProps, mdToken tk, INT32* pSequence, INT32* pAttributes)
{   
    CONTRACTL {
        THROWS;
        DISABLED(GC_TRIGGERS);
        MODE_COOPERATIVE;
        SO_TOLERANT;
    }
    CONTRACTL_END;

    IMDInternalImport* _pScope = pScope;
    USHORT usSequence = 0;
    BEGIN_SO_INTOLERANT_CODE_NOTHROW(GetThread(), FCThrowVoid(kStackOverflowException));
    _pScope->GetParamDefProps(tk, &usSequence, (DWORD*)pAttributes);
    *pSequence = (INT32) usSequence;
    END_SO_INTOLERANT_CODE;
}
FCIMPLEND
    
MDImpl2(void, MetaDataImport::GetFieldDefProps, mdToken tk, INT32 *pdwFieldFlags)
{
    CONTRACTL {
        THROWS;
        DISABLED(GC_TRIGGERS);
        MODE_COOPERATIVE;
        SO_TOLERANT;
    }
    CONTRACTL_END;

    IMDInternalImport* _pScope = pScope;
    BEGIN_SO_INTOLERANT_CODE_NOTHROW(GetThread(), FCThrowVoid(kStackOverflowException));
    *(DWORD*)pdwFieldFlags = _pScope->GetFieldDefProps(tk);
    END_SO_INTOLERANT_CODE;
}
FCIMPLEND

MDImpl4(void, MetaDataImport::GetPropertyProps, mdToken tk, LPCSTR* pszName, INT32 *pdwPropertyFlags, ConstArray* ppValue)
{   
    CONTRACTL {
        THROWS;
        DISABLED(GC_TRIGGERS);
        MODE_COOPERATIVE;
        SO_TOLERANT;
    }
    CONTRACTL_END;

    IMDInternalImport* _pScope = pScope;   
    BEGIN_SO_INTOLERANT_CODE_NOTHROW(GetThread(), FCThrowVoid(kStackOverflowException));
    ConstI4ArrayMarshaler value(ppValue);
    _pScope->GetPropertyProps(tk, pszName, (DWORD*)pdwPropertyFlags, (PCCOR_SIGNATURE*)GET_VAL(value));
    END_SO_INTOLERANT_CODE;
}
FCIMPLEND
        
MDImpl2(void, MetaDataImport::GetFieldMarshal, mdToken tk, ConstArray* ppValue)
{   
    CONTRACTL {
        THROWS;
        DISABLED(GC_TRIGGERS);
        MODE_COOPERATIVE;
        SO_TOLERANT;
    }
    CONTRACTL_END;

    IMDInternalImport* _pScope = pScope;   
    BEGIN_SO_INTOLERANT_CODE_NOTHROW(GetThread(), FCThrowVoid(kStackOverflowException));
    _pScope->GetFieldMarshal(tk, (PCCOR_SIGNATURE*)&ppValue->m_array, (ULONG*)&ppValue->m_count);
    END_SO_INTOLERANT_CODE;
}
FCIMPLEND
    
MDImpl2(void, MetaDataImport::GetSigOfMethodDef, mdToken tk, ConstArray* ppValue)
{   
    CONTRACTL {
        THROWS;
        DISABLED(GC_TRIGGERS);
        MODE_COOPERATIVE;
        SO_TOLERANT;
    }
    CONTRACTL_END;

    IMDInternalImport* _pScope = pScope;   
    BEGIN_SO_INTOLERANT_CODE(GetThread())
    ppValue->m_array = (void*)_pScope->GetSigOfMethodDef(tk, (ULONG*)&ppValue->m_count);
    END_SO_INTOLERANT_CODE;
}
FCIMPLEND
    
MDImpl2(void, MetaDataImport::GetSignatureFromToken, mdToken tk, ConstArray* ppValue)
{   
    CONTRACTL {
        THROWS;
        DISABLED(GC_TRIGGERS);
        MODE_COOPERATIVE;
        SO_TOLERANT;
    }
    CONTRACTL_END;

    IMDInternalImport* _pScope = pScope;   
    BEGIN_SO_INTOLERANT_CODE_NOTHROW(GetThread(), FCThrowVoid(kStackOverflowException));
    ppValue->m_array = (void*)_pScope->GetSigFromToken(tk, (ULONG*)&ppValue->m_count);
    END_SO_INTOLERANT_CODE;
}
FCIMPLEND

MDImpl2(void, MetaDataImport::GetSigOfFieldDef, mdToken tk, ConstArray* ppValue)
{   
    CONTRACTL {
        THROWS;
        DISABLED(GC_TRIGGERS);
        MODE_COOPERATIVE;
        SO_TOLERANT;
    }
    CONTRACTL_END;

    IMDInternalImport* _pScope = pScope;   
    BEGIN_SO_INTOLERANT_CODE_NOTHROW(GetThread(), FCThrowVoid(kStackOverflowException));
    ppValue->m_array = (void*)_pScope->GetSigOfFieldDef(tk, (ULONG*)&ppValue->m_count);
    END_SO_INTOLERANT_CODE;
}
FCIMPLEND

MDImpl2(void, MetaDataImport::GetParentToken, mdToken tk, mdToken* ptk)
{   
    CONTRACTL {
        THROWS;
        DISABLED(GC_TRIGGERS);
        MODE_COOPERATIVE;
        SO_TOLERANT;
    }
    CONTRACTL_END;

    IMDInternalImport* _pScope = pScope;   
    BEGIN_SO_INTOLERANT_CODE_NOTHROW(GetThread(), FCThrowVoid(kStackOverflowException));
    if (TypeFromToken(tk) == mdtTypeDef)
    {
        _pScope->GetNestedClassProps(tk, ptk);
    }
    else
    {
        _pScope->GetParentToken(tk, ptk);
    }
    END_SO_INTOLERANT_CODE;
}
FCIMPLEND
    
MDImpl1(void, MetaDataImport::GetScopeProps, GUID* pmvid)
{
    CONTRACTL {
        THROWS;
        DISABLED(GC_TRIGGERS);
        MODE_COOPERATIVE;
        SO_TOLERANT;
    }
    CONTRACTL_END;

    LPCSTR szName; 
    BEGIN_SO_INTOLERANT_CODE_NOTHROW(GetThread(), FCThrowVoid(kStackOverflowException));
    IMDInternalImport* _pScope = pScope;   
    _pScope->GetScopeProps(&szName, pmvid);
    END_SO_INTOLERANT_CODE;
}
FCIMPLEND 

    
MDImpl2(void, MetaDataImport::GetMemberRefProps, 
    mdMemberRef mr, 
    ConstArray* ppvSigBlob)
{
    CONTRACTL {
        THROWS;
        DISABLED(GC_TRIGGERS);
        MODE_COOPERATIVE;
        SO_TOLERANT;
    }
    CONTRACTL_END;
  
    IMDInternalImport* _pScope = pScope;   
    BEGIN_SO_INTOLERANT_CODE_NOTHROW(GetThread(), FCThrowVoid(kStackOverflowException));
    _pScope->GetNameAndSigOfMemberRef(mr, (PCCOR_SIGNATURE*)&ppvSigBlob->m_array, (ULONG*)&ppvSigBlob->m_count);
    END_SO_INTOLERANT_CODE;
}
FCIMPLEND 





