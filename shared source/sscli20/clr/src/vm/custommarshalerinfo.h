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
/*============================================================
**
** Header:  Custom marshaler information used when marshaling
**          a parameter with a custom marshaler. 
**         
                                                   </STRIP>
===========================================================*/

#ifndef _CUSTOMMARSHALERINFO_H_
#define _CUSTOMMARSHALERINFO_H_


#include "vars.hpp"
#include "list.h"


// This enumeration is used to retrieve a method desc from CustomMarshalerInfo::GetCustomMarshalerMD().
enum EnumCustomMarshalerMethods
{
    CustomMarshalerMethods_MarshalNativeToManaged = 0,
    CustomMarshalerMethods_MarshalManagedToNative,
    CustomMarshalerMethods_CleanUpNativeData,
    CustomMarshalerMethods_CleanUpManagedData,
    CustomMarshalerMethods_GetNativeDataSize,
    CustomMarshalerMethods_GetInstance,
    CustomMarshalerMethods_LastMember
};


class CustomMarshalerInfo
{
public:
    // Constructor and destructor.
    CustomMarshalerInfo(BaseDomain* pDomain, TypeHandle hndCustomMarshalerType, TypeHandle hndManagedType, LPCUTF8 strCookie, DWORD cCookieStrBytes);
    ~CustomMarshalerInfo();

    // CustomMarshalerInfo's are always allocated on the loader heap so we need to redefine
    // the new and delete operators to ensure this.
    void* operator      new(size_t size, LoaderHeap* pHeap);
    void  operator      delete(void* pMem);

    // Helpers used to invoke the different methods in the ICustomMarshaler interface.
    OBJECTREF           InvokeMarshalNativeToManagedMeth(void* pNative);
    void*               InvokeMarshalManagedToNativeMeth(OBJECTREF MngObj);
    void                InvokeCleanUpNativeMeth(void* pNative);
    void                InvokeCleanUpManagedMeth(OBJECTREF MngObj);

    // Accessors.
    int GetNativeSize()
    {
        LEAF_CONTRACT;
        return m_NativeSize;
    }

    int GetManagedSize()
    {
        WRAPPER_CONTRACT;
        return m_hndManagedType.GetSize();
    }

    TypeHandle GetManagedType()
    {
        LEAF_CONTRACT;
        return m_hndManagedType;
    }

    BOOL IsDataByValue()
    {
        LEAF_CONTRACT;
        return m_bDataIsByValue;
    }

    OBJECTHANDLE GetCustomMarshaler()
    {
        LEAF_CONTRACT;
        return m_hndCustomMarshaler;
    }

    TypeHandle GetCustomMarshalerType()
    {
        return ObjectFromHandle(m_hndCustomMarshaler)->GetTypeHandle();
    }

    // Helper function to retrieve a custom marshaler method desc.
    static MethodDesc*  GetCustomMarshalerMD(EnumCustomMarshalerMethods Method, TypeHandle hndCustomMarshalertype); 

    // Link used to contain this CM info in a linked list.
    SLink               m_Link;

private:
    int                 m_NativeSize;
    TypeHandle          m_hndManagedType;
    OBJECTHANDLE        m_hndCustomMarshaler;
    MethodDesc*         m_pMarshalNativeToManagedMD;
    MethodDesc*         m_pMarshalManagedToNativeMD;
    MethodDesc*         m_pCleanUpNativeDataMD;
    MethodDesc*         m_pCleanUpManagedDataMD;
    BOOL                m_bDataIsByValue;
};


typedef SList<CustomMarshalerInfo, offsetof(CustomMarshalerInfo, m_Link), true> CMINFOLIST;


class EECMHelperHashtableKey
{
public:
    EECMHelperHashtableKey(DWORD cMarshalerTypeNameBytes, LPCSTR strMarshalerTypeName, DWORD cCookieStrBytes, LPCSTR strCookie, BOOL bSharedHelper) 
    : m_cMarshalerTypeNameBytes(cMarshalerTypeNameBytes)
    , m_strMarshalerTypeName(strMarshalerTypeName)
    , m_cCookieStrBytes(cCookieStrBytes)
    , m_strCookie(strCookie)
    , m_bSharedHelper(bSharedHelper)
    {
        LEAF_CONTRACT;
    }

    inline DWORD GetMarshalerTypeNameByteCount() const
    {
        LEAF_CONTRACT;
        return m_cMarshalerTypeNameBytes;
    }
    inline LPCSTR GetMarshalerTypeName() const
    {
        LEAF_CONTRACT;
        return m_strMarshalerTypeName;
    }
    inline LPCSTR GetCookieString() const
    {
        LEAF_CONTRACT;
        return m_strCookie;
    }
    inline ULONG GetCookieStringByteCount() const
    {
        LEAF_CONTRACT;
        return m_cCookieStrBytes;
    }
    inline BOOL IsSharedHelper() const
    {
        LEAF_CONTRACT;
        return m_bSharedHelper;
    }


    DWORD           m_cMarshalerTypeNameBytes;
    LPCSTR          m_strMarshalerTypeName;
    DWORD           m_cCookieStrBytes;
    LPCSTR          m_strCookie;
    BOOL            m_bSharedHelper;
};


class EECMHelperHashtableHelper
{
public:
    static EEHashEntry_t*  AllocateEntry(EECMHelperHashtableKey* pKey, BOOL bDeepCopy, AllocationHeap Heap);
    static void            DeleteEntry(EEHashEntry_t* pEntry, AllocationHeap Heap);
    static BOOL            CompareKeys(EEHashEntry_t* pEntry, EECMHelperHashtableKey* pKey);
    static DWORD           Hash(EECMHelperHashtableKey* pKey);
};


typedef EEHashTable<EECMHelperHashtableKey*, EECMHelperHashtableHelper, TRUE> EECMHelperHashTable;


class CustomMarshalerHelper
{
public:
    // Helpers used to invoke the different methods in the ICustomMarshaler interface.
    OBJECTREF           InvokeMarshalNativeToManagedMeth(void* pNative);
    void*               InvokeMarshalManagedToNativeMeth(OBJECTREF MngObj);
    void                InvokeCleanUpNativeMeth(void* pNative);
    void                InvokeCleanUpManagedMeth(OBJECTREF MngObj);

    // Accessors.
    int GetNativeSize()
    {
        WRAPPER_CONTRACT;
        return GetCustomMarshalerInfo()->GetNativeSize();
    }
    
    int GetManagedSize()
    {
        WRAPPER_CONTRACT;
        return GetCustomMarshalerInfo()->GetManagedSize();
    }
    
    TypeHandle GetManagedType()
    {
        WRAPPER_CONTRACT;
        return GetCustomMarshalerInfo()->GetManagedType();
    }
    
    BOOL IsDataByValue()
    {
        WRAPPER_CONTRACT;
        return GetCustomMarshalerInfo()->IsDataByValue();
    }

    // Helper function to retrieve the custom marshaler object.
    virtual CustomMarshalerInfo* GetCustomMarshalerInfo() = 0;

protected:
    ~CustomMarshalerHelper( void )
    {
        LEAF_CONTRACT;
    }
};


class NonSharedCustomMarshalerHelper : public CustomMarshalerHelper
{
public:
    // Constructor.
    NonSharedCustomMarshalerHelper(CustomMarshalerInfo* pCMInfo) : m_pCMInfo(pCMInfo)
    {
        WRAPPER_CONTRACT;
    }

    // CustomMarshalerHelpers's are always allocated on the loader heap so we need to redefine
    // the new and delete operators to ensure this.
    void *operator new(size_t size, LoaderHeap *pHeap);
    void operator delete(void* pMem);

protected:
    // Helper function to retrieve the custom marshaler object.
    virtual CustomMarshalerInfo* GetCustomMarshalerInfo()
    {
        LEAF_CONTRACT;
        return m_pCMInfo;
    }

private:
    CustomMarshalerInfo* m_pCMInfo;
};


class SharedCustomMarshalerHelper : public CustomMarshalerHelper
{
public:
    // Constructor.
    SharedCustomMarshalerHelper(Assembly* pAssembly, TypeHandle hndManagedType, LPCUTF8 strMarshalerTypeName, DWORD cMarshalerTypeNameBytes, LPCUTF8 strCookie, DWORD cCookieStrBytes);

    // CustomMarshalerHelpers's are always allocated on the loader heap so we need to redefine
    // the new and delete operators to ensure this.
    void* operator new(size_t size, LoaderHeap* pHeap);
    void  operator delete(void* pMem);

    // Accessors.
    inline Assembly* GetAssembly()
    {
        LEAF_CONTRACT;
        return m_pAssembly;
    }

    inline TypeHandle GetManagedType()
    {
        LEAF_CONTRACT;
        return m_hndManagedType;
    }

    inline DWORD GetMarshalerTypeNameByteCount()
    {
        LEAF_CONTRACT;
        return m_cMarshalerTypeNameBytes;
    }

    inline LPCSTR GetMarshalerTypeName()
    {
        LEAF_CONTRACT;
        return m_strMarshalerTypeName;
    }

    inline LPCSTR GetCookieString()
    {
        LEAF_CONTRACT;
        return m_strCookie;
    }

    inline ULONG GetCookieStringByteCount()
    {
        LEAF_CONTRACT;
        return m_cCookieStrBytes;
    }

protected:
    // Helper function to retrieve the custom marshaler object.
    virtual CustomMarshalerInfo* GetCustomMarshalerInfo();

private:
    Assembly*       m_pAssembly;
    TypeHandle      m_hndManagedType;
    DWORD           m_cMarshalerTypeNameBytes;
    LPCUTF8         m_strMarshalerTypeName;
    DWORD           m_cCookieStrBytes;
    LPCUTF8         m_strCookie;
};


#endif // _CUSTOMMARSHALERINFO_H_

