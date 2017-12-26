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
// File: ObjectClone.H
//
#ifndef _OBJECTCLONE_H_
#define _OBJECTCLONE_H_

#include "invokeutil.h"
#include "comstring.h"
#include "runtimehandles.h"

BOOL
IsSerializerRelatedInterface(MethodTable *pItfMT);

#if defined(_X86_) || defined(_WIN64)
#ifndef DACCESS_COMPILE

VOID GCScanRootsInCollection(promote_func *fn, ScanContext* sc, void *context);

enum QueueType
{
    FIFO,
    LIFO
};

enum ObjectProperties
{
    enum_Array = 0x01,
    enum_NeedsUnboxing = 0x02,
    enum_ISerializableMember = 0x04,        // This is set on member of an ISerializable instance
    enum_Iserializable = 0x08,              // This is set on an ISerializable instance
    enum_IObjRef = 0x10,                    // This is set on an IObjRef instance
};

// This is the base class of all the different records that get 
// stored in different tables during cloning
class QueuedObjectInfo
{
protected:
    BYTE    m_properties;
public:
    QueuedObjectInfo() { LEAF_CONTRACT; m_properties = 0; }
    BOOL    IsArray() { LEAF_CONTRACT; return m_properties & enum_Array; }
    BOOL    NeedsUnboxing() { LEAF_CONTRACT; return m_properties & enum_NeedsUnboxing; }
    void    SetIsArray() { LEAF_CONTRACT; m_properties |= enum_Array; }
    void    SetNeedsUnboxing() { LEAF_CONTRACT; m_properties |= enum_NeedsUnboxing; }
    BOOL    IsISerializableMember() { LEAF_CONTRACT; return m_properties & enum_ISerializableMember; }
    void    SetIsISerializableMember() { LEAF_CONTRACT; m_properties |= enum_ISerializableMember; }
    BOOL    IsISerializableInstance() { LEAF_CONTRACT; return m_properties & enum_Iserializable; }
    void    SetIsISerializableInstance() { LEAF_CONTRACT; m_properties |= enum_Iserializable; }
    BOOL    IsIObjRefInstance() { LEAF_CONTRACT; return m_properties & enum_IObjRef; }
    void    SetIsIObjRefInstance() { LEAF_CONTRACT; m_properties |= enum_IObjRef; }
    virtual DWORD GetSize()
        { 
            LEAF_CONTRACT; 
            STATIC_CONTRACT_SO_TOLERANT;            
            DWORD size = sizeof(QueuedObjectInfo);
#if defined(_WIN64) || defined(ALIGN_ACCESS)
            size = (DWORD)ALIGN_UP(size, sizeof(SIZE_T));
#endif // _WIN64 || ALIGN_ACCESS
            return size;
        }
};

// These are records in QOF. Represents a parent object which has at least one member to
// be marshalled and fixed up.
class ParentInfo : public QueuedObjectInfo
{
    DWORD   m_fixupCount;
    DWORD   m_numSpecialMembers;
    DWORD   m_IserIndexInTSOTable;
    DWORD   m_IObjRefIndexInTSOTable;
    DWORD   m_BoxedValIndexIntoTSOTable;
public:
    ParentInfo(DWORD count) 
        {   
            LEAF_CONTRACT; 
            m_fixupCount = count; 
            m_numSpecialMembers = 0;
            m_IserIndexInTSOTable = (DWORD) -1;
            m_IObjRefIndexInTSOTable = (DWORD) -1;
            m_BoxedValIndexIntoTSOTable = (DWORD) -1;
        }
    DWORD DecrementFixupCount() { LEAF_CONTRACT; return --m_fixupCount; }
    DWORD GetNumSpecialMembers() { LEAF_CONTRACT; return m_numSpecialMembers; }
    DWORD IncrementSpecialMembers() { LEAF_CONTRACT; return ++m_numSpecialMembers; }
    DWORD GetISerIndexIntoTSO() { LEAF_CONTRACT; return m_IserIndexInTSOTable; }
    void SetISerIndexIntoTSO(DWORD index) { LEAF_CONTRACT; m_IserIndexInTSOTable = index; }
    DWORD GetIObjRefIndexIntoTSO() { LEAF_CONTRACT; return m_IObjRefIndexInTSOTable; }
    void SetIObjRefIndexIntoTSO(DWORD index) { LEAF_CONTRACT; m_IObjRefIndexInTSOTable = index; }
    DWORD GetBoxedValIndexIntoTSO() { LEAF_CONTRACT; return m_BoxedValIndexIntoTSOTable; }
    void SetBoxedValIndexIntoTSO(DWORD index) { LEAF_CONTRACT; m_BoxedValIndexIntoTSOTable = index; }
    virtual DWORD GetSize()
        { 
            LEAF_CONTRACT; 
            STATIC_CONTRACT_SO_TOLERANT;
            DWORD size = sizeof(ParentInfo);
#if defined(_WIN64) || defined(ALIGN_ACCESS)
            size = (DWORD)ALIGN_UP(size, sizeof(SIZE_T));
#endif // _WIN64 || ALIGN_ACCESS
            return size;
        }
};

// Represents an object whose parent is a regular object (not an array, not ISerializable etc)
// Contains enough information to fix this object into its parent
class ObjectMemberInfo : public QueuedObjectInfo
{
    FieldDesc   *m_fieldDesc;
public:
    ObjectMemberInfo(FieldDesc *field) { LEAF_CONTRACT; m_fieldDesc = field; }
    FieldDesc *GetFieldDesc() { LEAF_CONTRACT; return m_fieldDesc; }
    VOID  SetFieldDesc(FieldDesc* field) { LEAF_CONTRACT; m_fieldDesc = field; }
    virtual DWORD GetSize()
        { 
            LEAF_CONTRACT; 
            STATIC_CONTRACT_SO_TOLERANT;            
            DWORD size = sizeof(ObjectMemberInfo);
#if defined(_WIN64) || defined(ALIGN_ACCESS)
            size = (DWORD)ALIGN_UP(size, sizeof(SIZE_T));
#endif // _WIN64 || ALIGN_ACCESS
            return size;
        }
};

// Represents an object whose parent is an array 
// Contains index information to fix this object into its parent
class NDimArrayMemberInfo : public QueuedObjectInfo
{
    DWORD   m_numDimensions;
    DWORD   m_index[0];
public:
    NDimArrayMemberInfo(DWORD rank)
        { 
            LEAF_CONTRACT; 
            m_numDimensions = rank;
            SetIsArray();
        }
    virtual DWORD GetSize()
        { 
            LEAF_CONTRACT; 
            STATIC_CONTRACT_SO_TOLERANT;            
            DWORD size = sizeof(NDimArrayMemberInfo) + (sizeof(DWORD) * (m_numDimensions));
#if defined(_WIN64) || defined(ALIGN_ACCESS)
            size = (DWORD)ALIGN_UP(size, sizeof(SIZE_T));
#endif // _WIN64 || ALIGN_ACCESS
            return size;
        }
    DWORD *GetIndices() 
        { LEAF_CONTRACT; return &m_index[0]; }
    void SetIndices(DWORD* indices)
        {
            LEAF_CONTRACT; 
            memcpy(GetIndices(), indices, GetNumDimensions() * sizeof(DWORD));
        }
    DWORD GetNumDimensions()
        { LEAF_CONTRACT; return m_numDimensions; }
    void SetNumDimensions(DWORD rank)
        { LEAF_CONTRACT; m_numDimensions = rank; }
};

// Represents an object whose parent is an ISerializable object 
// Contains index information to fix this object into its parent
class ISerializableMemberInfo : public QueuedObjectInfo
{
    DWORD           m_TIOIndex;
    DWORD           m_fieldIndex;
public:
    ISerializableMemberInfo(DWORD tableIndex, DWORD fieldIndex)
        {
            WRAPPER_CONTRACT; 
            m_TIOIndex = tableIndex;
            m_fieldIndex = fieldIndex;
            SetIsISerializableMember();
        }
    DWORD GetTableIndex() 
        { LEAF_CONTRACT; return m_TIOIndex; }
    DWORD GetFieldIndex()
        { LEAF_CONTRACT; STATIC_CONTRACT_SO_TOLERANT; return m_fieldIndex; }
    virtual DWORD GetSize()
        { 
            LEAF_CONTRACT; 
            STATIC_CONTRACT_SO_TOLERANT;            
            DWORD size = sizeof(ISerializableMemberInfo);
#if defined(_WIN64) || defined(ALIGN_ACCESS)
            size = (DWORD)ALIGN_UP(size, sizeof(SIZE_T));
#endif // _WIN64 || ALIGN_ACCESS
            return size;
        }
};

// Represents a special object (ISerializable, Boxed value type, IObjectReference)
// Entries in TSO are of this type
class SpecialObjectInfo : public QueuedObjectInfo
{
protected:
    DWORD       m_specialObjectProperties;
    int         m_objectId;
    DWORD       m_numSpecialMembers;
    DWORD       m_mappingTableIndex;
public:
    SpecialObjectInfo() 
    { 
        LEAF_CONTRACT; 
        m_specialObjectProperties = 0; 
        m_mappingTableIndex = 0;
        m_numSpecialMembers  = 0;
        m_objectId = 0;
    }
    void SetHasBeenProcessed()  { LEAF_CONTRACT; m_specialObjectProperties |= 0x01; }
    DWORD HasBeenProcessed()    { LEAF_CONTRACT; return m_specialObjectProperties & 0x01; }
    void SetHasFixupInfo()  { LEAF_CONTRACT; m_specialObjectProperties |= 0x02; }
    DWORD HasFixupInfo()    { LEAF_CONTRACT; return m_specialObjectProperties & 0x02; }
    void SetIsRepeatObject()  { LEAF_CONTRACT; m_specialObjectProperties |= 0x04; }
    DWORD IsRepeatObject()    { LEAF_CONTRACT; return m_specialObjectProperties & 0x04; }
    void SetIsBoxedObject()  { LEAF_CONTRACT; m_specialObjectProperties |= 0x08; }
    DWORD IsBoxedObject()    { LEAF_CONTRACT; return m_specialObjectProperties & 0x08; }
    void SetTargetNotISerializable()  { LEAF_CONTRACT; m_specialObjectProperties |= 0x10; }
    DWORD IsTargetNotISerializable()    { LEAF_CONTRACT; return m_specialObjectProperties & 0x10; }
    
    void SetMappingTableIndex(DWORD index)  { LEAF_CONTRACT; m_mappingTableIndex = index; }
    DWORD GetMappingTableIndex()    { LEAF_CONTRACT; return m_mappingTableIndex; }
    DWORD GetNumSpecialMembers() { LEAF_CONTRACT; return m_numSpecialMembers; }
    void SetNumSpecialMembers(DWORD numSpecialMembers) { LEAF_CONTRACT; m_numSpecialMembers = numSpecialMembers;}
    void SetObjectId(int id) { LEAF_CONTRACT; m_objectId = id; }
    int GetObjectId() { LEAF_CONTRACT; return m_objectId; }
};

// Represents a special object (ISerializable)
// Contains the number of IObjRef members it has
class ISerializableInstanceInfo : public SpecialObjectInfo
{
public:
    ISerializableInstanceInfo(int objectId, DWORD numIObjRefMembers)
        {
            LEAF_CONTRACT; 
            m_numSpecialMembers = numIObjRefMembers;
            m_objectId = objectId;
            SetIsISerializableInstance();
        }
    virtual DWORD GetSize()
        { 
            LEAF_CONTRACT; 
            STATIC_CONTRACT_SO_TOLERANT;            
            DWORD size = sizeof(ISerializableInstanceInfo);
#if defined(_WIN64) || defined(ALIGN_ACCESS)
            size = (DWORD)ALIGN_UP(size, sizeof(SIZE_T));
#endif // _WIN64 || ALIGN_ACCESS
            return size;
        }
};

// Represents a special object (IObjectReference)
// Contains fixup information to fix the completed object into its parent
class IObjRefInstanceInfo : public SpecialObjectInfo
{
    DWORD           m_ISerTSOIndex;     // If this is also an Iserializable instance, index of the iser entry in TSO
#if defined(_WIN64) || defined(ALIGN_ACCESS)
    DWORD           m_padding;
#endif // _WIN64 || ALIGN_ACCESS
    BYTE            m_fixupData[0];
public:
    IObjRefInstanceInfo(int objectId, DWORD numIObjRefMembers, DWORD numISerMembers)
        {
            WRAPPER_CONTRACT; 
            C_ASSERT((offsetof(IObjRefInstanceInfo, m_fixupData) % sizeof(SIZE_T)) == 0);
            m_numSpecialMembers = numIObjRefMembers + numISerMembers;
            m_ISerTSOIndex = (DWORD) -1;
            m_objectId = objectId;
            SetIsIObjRefInstance();
        }
    DWORD GetISerTSOIndex()   {LEAF_CONTRACT; return m_ISerTSOIndex; }
    void SetISerTSOIndex(DWORD index)
        {   LEAF_CONTRACT; m_ISerTSOIndex = index; }
    void SetFixupInfo(QueuedObjectInfo *pData)
        { 
            WRAPPER_CONTRACT; 
            if (pData->GetSize() > 0)
            {
                SetHasFixupInfo();
                memcpy(m_fixupData, pData, pData->GetSize());
            }
        }
    QueuedObjectInfo *GetFixupInfo()
    {
        WRAPPER_CONTRACT;
        STATIC_CONTRACT_SO_TOLERANT;
        return (HasFixupInfo() ? (QueuedObjectInfo *)&m_fixupData[0] : NULL);
    }
    virtual DWORD GetSize()
        { 
            WRAPPER_CONTRACT; 
            STATIC_CONTRACT_SO_TOLERANT;
            DWORD size = sizeof(IObjRefInstanceInfo) + (HasFixupInfo() ? ((QueuedObjectInfo *)&m_fixupData[0])->GetSize() : 0); 
#if defined(_WIN64) || defined(ALIGN_ACCESS)
            size = (DWORD)ALIGN_UP(size, sizeof(SIZE_T));
#endif // _WIN64 || ALIGN_ACCESS
            return size;
        }
};

// Represents a special object (Boxed value type)
// Contains fixup information to fix the completed object into its parent
class ValueTypeInfo : public SpecialObjectInfo
{
protected:
    DWORD           m_ISerTSOIndex;     // If this is also an Iserializable instance, index of the iser entry in TSO
    DWORD           m_IObjRefTSOIndex;  // If this is also an IObjRef instance, index of the iser entry in TSO
    BYTE            m_fixupData[0];
public:
    ValueTypeInfo(int objectId, QueuedObjectInfo *pFixupInfo) 
        { 
            WRAPPER_CONTRACT;
            C_ASSERT((offsetof(ValueTypeInfo, m_fixupData) % sizeof(SIZE_T)) == 0);
            m_ISerTSOIndex = (DWORD) -1;
            m_IObjRefTSOIndex = (DWORD) -1;
            m_objectId = objectId;
            SetNeedsUnboxing();
            SetIsBoxedObject();
            SetFixupInfo(pFixupInfo);
        }
    DWORD GetISerTSOIndex()   {LEAF_CONTRACT; return m_ISerTSOIndex; }
    void SetISerTSOIndex(DWORD index)
        {   LEAF_CONTRACT; m_ISerTSOIndex = index; }
    DWORD GetIObjRefTSOIndex()   {LEAF_CONTRACT; return m_IObjRefTSOIndex; }
    void SetIObjRefTSOIndex(DWORD index)
        {   LEAF_CONTRACT; m_IObjRefTSOIndex = index; }
    void SetFixupInfo(QueuedObjectInfo *pData)
        { 
            WRAPPER_CONTRACT;
            if (pData->GetSize() > 0)
            {
                SetHasFixupInfo();
                memcpy(m_fixupData, pData, pData->GetSize());
            }
        }
    QueuedObjectInfo *GetFixupInfo()
    {
        WRAPPER_CONTRACT;
        STATIC_CONTRACT_SO_TOLERANT;
        return (HasFixupInfo() ? (QueuedObjectInfo *)&m_fixupData[0] : NULL);
    }
    virtual DWORD GetSize()
        { 
            WRAPPER_CONTRACT; 
            STATIC_CONTRACT_SO_TOLERANT;                                   
            DWORD size = sizeof(ValueTypeInfo) + (HasFixupInfo() ? ((QueuedObjectInfo *)&m_fixupData[0])->GetSize() : 0); 
#if defined(_WIN64) || defined(ALIGN_ACCESS)
            size = (DWORD)ALIGN_UP(size, sizeof(SIZE_T));
#endif // _WIN64 || ALIGN_ACCESS
            return size;
        }
};

// Threshold beyond which the collections switch to using the heap
#ifdef _DEBUG
#define STACK_TO_HEAP_THRESHOLD 5
#define QOM_STACK_TO_HEAP_THRESHOLD 5
#define QOF_STACK_TO_HEAP_THRESHOLD 5
#define TSO_STACK_TO_HEAP_THRESHOLD 5
#define TDC_STACK_TO_HEAP_THRESHOLD 5
#define VSC_STACK_TO_HEAP_THRESHOLD 5
#define VDC_STACK_TO_HEAP_THRESHOLD 5
#else
#define STACK_TO_HEAP_THRESHOLD 100
#define QOM_STACK_TO_HEAP_THRESHOLD 100
#define QOF_STACK_TO_HEAP_THRESHOLD 16
#define TSO_STACK_TO_HEAP_THRESHOLD 8
#define TDC_STACK_TO_HEAP_THRESHOLD 8
#define VSC_STACK_TO_HEAP_THRESHOLD 8
#define VDC_STACK_TO_HEAP_THRESHOLD 8
#endif

#define MAGIC_FACTOR 12

VOID GCScanRootsInCollection(promote_func *fn, ScanContext* sc, void *context);

#define LIFO_QUEUE 1
#define FIFO_QUEUE 2

class GCSafeCollection
{
protected:
    // AppDomain object leak protection: pointer to predicate which flips to false once we should stop reporting GC references.
    BOOL           *m_pfReportRefs;
    
public:
    GCSafeCollection(){}
    virtual void Cleanup() = 0;
    virtual void ReportGCRefs(promote_func *fn, ScanContext* sc) = 0;
};

class GCSafeObjectTable : public GCSafeCollection
{
protected:

    OBJECTREF       *m_Objects1;
    OBJECTREF       *m_Objects2;
    OBJECTREF       *m_Objects3;

    DWORD           *m_dataIndices;
    BYTE            *m_data;

    DWORD           m_currArraySize;
    // Objects
    DWORD           m_count;
    DWORD           m_head;
    // Data
    DWORD           m_numDataBytes;
    DWORD           m_dataHead;
    
    // LIFO/FIFO
    DWORD           m_QueueType;
    BOOL            m_usingHeap;

    BOOL            m_fCleanedUp;

    void EnsureSize(DWORD requiredDataSize);
    void Resize();

public:

    void Init(OBJECTREF *ref1, OBJECTREF *ref2, OBJECTREF *ref3, DWORD *dwIndices, BYTE *bData, DWORD currArraySize, DWORD qType, BOOL *pfReportRefs)
    {
        WRAPPER_CONTRACT;
        STATIC_CONTRACT_SO_INTOLERANT;                               
        m_Objects1 = ref1;
        m_Objects2 = ref2;
        m_Objects3 = ref3;
        m_dataIndices = dwIndices;
        m_data = bData;
        m_QueueType = qType;
        m_currArraySize = currArraySize;
        m_usingHeap = FALSE;
        m_count = 0;
        m_head = 0;
        m_numDataBytes = 0;
        m_dataHead = 0;
        _ASSERTE(m_QueueType == LIFO_QUEUE || m_QueueType == FIFO_QUEUE);
        // If this is a lifo queue, then the data indices are definitely needed
        _ASSERTE(m_QueueType != LIFO_QUEUE || m_dataIndices != NULL);
        m_pfReportRefs = pfReportRefs;
        m_fCleanedUp = FALSE;
#ifdef USE_CHECKED_OBJECTREFS
        ZeroMemory(m_Objects1, sizeof(OBJECTREF) * m_currArraySize);
        if (m_Objects2 != NULL)
            ZeroMemory(m_Objects2, sizeof(OBJECTREF) * m_currArraySize);
        if (m_Objects3 != NULL)
            ZeroMemory(m_Objects3, sizeof(OBJECTREF) * m_currArraySize);
        for(DWORD i = 0; i < m_currArraySize; i++)
        {
            Thread::ObjectRefProtected(&m_Objects1[i]);
            if (m_Objects2)
                Thread::ObjectRefProtected(&m_Objects2[i]);
            if (m_Objects3)
                Thread::ObjectRefProtected(&m_Objects3[i]);
        }
#endif
    }
    
    void Init(OBJECTREF *ref1, BYTE *bData, DWORD currArraySize, DWORD qType, BOOL *pfReportRefs)
    {
        WRAPPER_CONTRACT;
        STATIC_CONTRACT_SO_INTOLERANT;        
        
        m_Objects1 = ref1;
        m_Objects2 = NULL;
        m_Objects3 = NULL;
        m_dataIndices = NULL;
        m_data = bData;
        m_QueueType = qType;
        m_currArraySize = currArraySize;
        m_usingHeap = FALSE;
        m_count = 0;
        m_head = 0;
        m_numDataBytes = 0;
        m_dataHead = 0;
        _ASSERTE(m_QueueType == LIFO_QUEUE || m_QueueType == FIFO_QUEUE);
        // If this is a lifo queue, then the data indices are definitely needed
        _ASSERTE(m_QueueType != LIFO_QUEUE || m_dataIndices != NULL);
        m_pfReportRefs = pfReportRefs;
        m_fCleanedUp = FALSE;
#ifdef USE_CHECKED_OBJECTREFS
        ZeroMemory(m_Objects1, sizeof(OBJECTREF) * m_currArraySize);
        if (m_Objects2 != NULL)
            ZeroMemory(m_Objects2, sizeof(OBJECTREF) * m_currArraySize);
        if (m_Objects3 != NULL)
            ZeroMemory(m_Objects3, sizeof(OBJECTREF) * m_currArraySize);
        for(DWORD i = 0; i < m_currArraySize; i++)
        {
            Thread::ObjectRefProtected(&m_Objects1[i]);
            if (m_Objects2)
                Thread::ObjectRefProtected(&m_Objects2[i]);
            if (m_Objects3)
                Thread::ObjectRefProtected(&m_Objects3[i]);
        }
#endif
    }

   
    virtual void Cleanup()
    {
        LEAF_CONTRACT;
        STATIC_CONTRACT_SO_TOLERANT;        
        // Set this first, we must disable GC reporting of our objects before we start ripping down the data structures that record
        // those objects. See ReportGCRefs for a more detailed explanation.
        m_fCleanedUp = TRUE;

        if (m_usingHeap == TRUE)
        {
            if (m_Objects1)
                delete[] m_Objects1;
            m_Objects1 = NULL;
            if (m_Objects2)
                delete[] m_Objects2;
            m_Objects2 = NULL;
            if (m_Objects3)
                delete[] m_Objects3;
            m_Objects3 = NULL;
            if (m_data)
                delete[] m_data;
            m_data = NULL;
            if (m_dataIndices)
                delete[] m_dataIndices;
            m_dataIndices = NULL;
        }
    }
    
    virtual void ReportGCRefs(promote_func *fn, ScanContext* sc)
    {
        WRAPPER_CONTRACT;
        STATIC_CONTRACT_SO_TOLERANT;

        if (m_fCleanedUp)
            return;

        // We track a predicate (actually embedded in the cloner which owns us) that tells us whether to report any GC refs at all.
        // This is used as a rip cord if the server appdomain is unloaded while we're processing in it (because this collection can
        // survive for a little while after that which is long enough to cause server objects to outlive their domain).
        if (!*m_pfReportRefs)
        {
            m_count = 0;
            if (m_Objects1)
                ZeroMemory(m_Objects1, m_currArraySize * sizeof(OBJECTREF));
            if (m_Objects2)
                ZeroMemory(m_Objects2, m_currArraySize * sizeof(OBJECTREF));
            if (m_Objects3)
                ZeroMemory(m_Objects3, m_currArraySize * sizeof(OBJECTREF));
            return;
        }

        Object **pRefs1 = (Object**) m_Objects1;
        Object **pRefs2 = (Object**) m_Objects2;
        Object **pRefs3 = (Object**) m_Objects3;

        if (m_QueueType == LIFO_QUEUE)
        {
            for (DWORD i = 0; i < m_count; i++)
            {
                _ASSERTE(i < m_currArraySize);
                if (m_Objects1)
                    (*fn)(pRefs1[i], sc, 0);
                if (m_Objects2)
                    (*fn)(pRefs2[i], sc, 0);
                if (m_Objects3)
                    (*fn)(pRefs3[i], sc, 0);
            }
        }
        else
        {
            for (DWORD i = m_head, count = 0; count < m_count; i++, count++)
            {
                i = i % m_currArraySize;
                if (m_Objects1)
                    (*fn)(pRefs1[i], sc, 0);
                if (m_Objects2)
                    (*fn)(pRefs2[i], sc, 0);
                if (m_Objects3)
                    (*fn)(pRefs3[i], sc, 0);
            }
        }
    }
    void Enqueue(OBJECTREF refObj, OBJECTREF refParent, OBJECTREF refAux, QueuedObjectInfo *pQOI);
    void Push(OBJECTREF refObj, OBJECTREF refParent, OBJECTREF refAux, QueuedObjectInfo *pQOI);
    void SetAt(DWORD index, OBJECTREF refObj, OBJECTREF refParent, OBJECTREF refAux, QueuedObjectInfo *pQOI);
    OBJECTREF Dequeue(OBJECTREF *refParent, OBJECTREF *refAux, QueuedObjectInfo **pQOI);
    OBJECTREF Pop(OBJECTREF *refParent, OBJECTREF *refAux, QueuedObjectInfo **pQOI);
    OBJECTREF GetAt(DWORD index, OBJECTREF *refParent, OBJECTREF *refAux, QueuedObjectInfo **pQOI);
    OBJECTREF Peek(OBJECTREF *refParent, OBJECTREF *refAux, QueuedObjectInfo **pQOI);
    void BeginEnumeration(DWORD *dwIndex) 
    { 
        LEAF_CONTRACT;
        if (m_QueueType == LIFO_QUEUE)
            *dwIndex = m_count; 
        else
            *dwIndex = 0;
    }
    
    OBJECTREF GetNext(DWORD *dwIndex, OBJECTREF *refParent, OBJECTREF *refAux, QueuedObjectInfo **pQOI)
    {
        WRAPPER_CONTRACT;
        
        OBJECTREF refRet = NULL;
        if (m_QueueType == LIFO_QUEUE)
        {
            if (*dwIndex == 0)
                return NULL;

            (*dwIndex)--;
            refRet = GetAt(*dwIndex, refParent, refAux, pQOI);
        }
        else
        {
            if (*dwIndex == m_count)
                return NULL;

            refRet = GetAt(*dwIndex, refParent, refAux, pQOI);
            (*dwIndex)++;
        }
        return refRet;
    }
    
    DWORD GetCount()  { LEAF_CONTRACT; return m_count; }
    
};

class DwordArrayList
{
    DWORD       m_dwordsOnStack[STACK_TO_HEAP_THRESHOLD];
    DWORD       *m_dwords;

    DWORD       m_count;
    DWORD       m_currSize;
public:
    
    void Init()
    {
        LEAF_CONTRACT;
        m_dwords = &m_dwordsOnStack[0];
        m_currSize = STACK_TO_HEAP_THRESHOLD;
        m_count = 0;
    }

    void        Add(DWORD i)
    {
        WRAPPER_CONTRACT;
        EnsureSize();
        m_dwords[m_count++] = i;
    }

    void EnsureSize()
    {
        CONTRACTL
        {
            THROWS;
            GC_NOTRIGGER;
            MODE_COOPERATIVE;
            STATIC_CONTRACT_SO_INTOLERANT;            
        }
        CONTRACTL_END
        if (m_count < m_currSize)
            return;

        DWORD newSize = m_currSize * 2;
        // Does not need a holder because this is the only allocation in this method
        DWORD *pTemp = new DWORD[newSize]; 
        ZeroMemory(pTemp, sizeof(DWORD) * newSize);

        memcpy((BYTE*)pTemp, m_dwords, sizeof(DWORD) * m_currSize);
        if (m_dwords != &m_dwordsOnStack[0])
        {
            delete[] m_dwords;
        }
        m_dwords = pTemp;
        m_count = m_currSize;
        m_currSize = newSize;
    }

    void Cleanup()
    {
        LEAF_CONTRACT;
        STATIC_CONTRACT_SO_TOLERANT;        
        if (m_dwords != &m_dwordsOnStack[0])
        {
            delete[] m_dwords;
            m_dwords = &m_dwordsOnStack[0];
        }
    }

    DWORD GetCount() { LEAF_CONTRACT; return m_count; }
    DWORD GetAt(DWORD index)    
    {
        LEAF_CONTRACT;
        _ASSERTE(index >= 0 && index < m_count);
        return m_dwords[index]; 
    }
};


#define NUM_SLOTS_PER_BUCKET 4

class GCSafeObjectHashTable : public GCSafeCollection
{
private:
    OBJECTREF   m_objectsOnStack[STACK_TO_HEAP_THRESHOLD];
    OBJECTREF   m_newObjectsOnStack[STACK_TO_HEAP_THRESHOLD];
    DWORD       m_dataOnStack[STACK_TO_HEAP_THRESHOLD];
    DWORD       m_count;
    DWORD       m_currArraySize;
    int         *m_ids;
    OBJECTREF   *m_objects;
    OBJECTREF   *m_newObjects;
    BOOL        m_fCleanedUp;
    
    void Resize();
    int FindElement(OBJECTREF refObj, BOOL &seenBefore);

public:
    virtual void Init(BOOL *pfReportRefs)
    {
        WRAPPER_CONTRACT;
        STATIC_CONTRACT_SO_TOLERANT;
        ZeroMemory(&m_objectsOnStack[0], sizeof(m_objectsOnStack));
        ZeroMemory(&m_dataOnStack[0], sizeof(m_dataOnStack));
        m_objects = &m_objectsOnStack[0];
        m_newObjects = &m_newObjectsOnStack[0];
        m_currArraySize = STACK_TO_HEAP_THRESHOLD;
        m_count = 0;
        m_ids = (int *) &m_dataOnStack[0];
        m_pfReportRefs = pfReportRefs;
        m_fCleanedUp = FALSE;
#ifdef USE_CHECKED_OBJECTREFS
        ZeroMemory(&m_newObjects[0], sizeof(m_newObjectsOnStack));
        for(DWORD i = 0; i < m_currArraySize; i++)
        {
            Thread::ObjectRefProtected(&m_objects[i]);
            Thread::ObjectRefProtected(&m_newObjects[i]);
        }
#endif
    }

    virtual void Cleanup()
    {
        LEAF_CONTRACT;
        STATIC_CONTRACT_SO_TOLERANT;                       
        // Set this first, we must disable GC reporting of our objects before we start ripping down the data structures that record
        // those objects. See ReportGCRefs for a more detailed explanation.
        m_fCleanedUp = TRUE;

        if (m_newObjects != &m_newObjectsOnStack[0])
        {
            delete[] m_ids;
            m_ids = (int *) &m_dataOnStack[0];
            delete[] m_newObjects;
            m_newObjects = &m_newObjectsOnStack[0];
            delete[] m_objects;
            m_objects = &m_objectsOnStack[0];
            m_currArraySize = STACK_TO_HEAP_THRESHOLD;
        }
    }
    
    virtual void ReportGCRefs(promote_func *fn, ScanContext* sc)
    {
        WRAPPER_CONTRACT;
        STATIC_CONTRACT_SO_TOLERANT;
        if (m_fCleanedUp)
            return;

        // We track a predicate (actually embedded in the cloner which owns us) that tells us whether to report any GC refs at all.
        // This is used as a rip cord if the server appdomain is unloaded while we're processing in it (because this collection can
        // survive for a little while after that which is long enough to cause server objects to outlive their domain).
        if (!*m_pfReportRefs)
        {
            m_count = 0;
            ZeroMemory(m_ids, m_currArraySize * sizeof(int));
            ZeroMemory(m_objects, m_currArraySize * sizeof(OBJECTREF));
            ZeroMemory(m_newObjects, m_currArraySize * sizeof(OBJECTREF));
            return;
        }

        Object **pRefs = (Object**) m_objects;
        Object **pNewRefs = (Object**) m_newObjects;
        
        for (DWORD i = 0; i < m_currArraySize; i++)
        {
            if (m_ids[i] != 0)
            {
                (*fn)(pRefs[i], sc, 0);
                (*fn)(pNewRefs[i], sc, 0);
            }
        }
    }
    int HasID(OBJECTREF refObj, OBJECTREF *newObj);
    int AddObject(OBJECTREF refObj, OBJECTREF newObj); 
    int UpdateObject(OBJECTREF refObj, OBJECTREF newObj);
};

enum SpecialObjects
{
    ISerializable = 1,
    IObjectReference,
    BoxedValueType
};

enum CloningContext
{
    CrossAppDomain = 1,
    ObjectFreezer
};

class IObjectCloneCallback
{
    public:
        virtual OBJECTREF AllocateObject(OBJECTREF refSrc, MethodTable *pMT) = 0;
        virtual OBJECTREF AllocateArray(OBJECTREF refSrc, TypeHandle arrayType, INT32 *pArgs, DWORD dwNumArgs, BOOL bAllocateInLargeHeap) = 0;
        virtual STRINGREF AllocateString(STRINGREF refSrc) = 0;
        virtual void ValidateFromType(MethodTable *pFromMT) = 0;
        virtual void ValidateToType(MethodTable *pToMT) = 0;
        virtual BOOL IsRemotedType(MethodTable *pMT, AppDomain* pFromDomain, AppDomain* pToDomain) = 0;
        virtual BOOL IsISerializableType(MethodTable *pMT) = 0;
        virtual BOOL IsIObjectReferenceType(MethodTable *pMT) = 0;
        virtual BOOL RequiresDeserializationCallback(MethodTable *pMT) = 0;
        virtual BOOL RequiresDeepCopy(OBJECTREF refObj) = 0;
};

class CrossAppDomainClonerCallback : public IObjectCloneCallback
{
    public:
        OBJECTREF AllocateObject(OBJECTREF, MethodTable * pMT)
        {
            WRAPPER_CONTRACT;
            STATIC_CONTRACT_SO_TOLERANT;            
            pMT->EnsureInstanceActive();
            return ::AllocateObject(pMT);
        }

        OBJECTREF AllocateArray(OBJECTREF, TypeHandle arrayType, INT32 *pArgs, DWORD dwNumArgs, BOOL bAllocateInLargeHeap)
        {
            WRAPPER_CONTRACT;
            STATIC_CONTRACT_SO_TOLERANT;
            return ::AllocateArrayEx(arrayType, pArgs, dwNumArgs, bAllocateInLargeHeap);
        }

        STRINGREF AllocateString(STRINGREF refSrc)
        {
            LEAF_CONTRACT;
            STATIC_CONTRACT_SO_TOLERANT;            
            return refSrc;
        }

        void ValidateFromType(MethodTable *pFromMT)
        {
           WRAPPER_CONTRACT;
            STATIC_CONTRACT_SO_TOLERANT;                                  
            CheckSerializable(pFromMT);
        }
        
        void ValidateToType(MethodTable *pToMT)
        {
           WRAPPER_CONTRACT;
            CheckSerializable(pToMT);
        }
        
        BOOL IsRemotedType(MethodTable *pMT, AppDomain* pFromAD, AppDomain* pToDomain)
        {
           WRAPPER_CONTRACT;
            STATIC_CONTRACT_SO_TOLERANT;                       
            if ((pMT->IsMarshaledByRef() && pFromAD != pToDomain) ||
                pMT->IsTransparentProxyType())
                return TRUE;
            
            return FALSE;
        }
        
        BOOL IsISerializableType(MethodTable *pMT)
        {
           WRAPPER_CONTRACT;
            STATIC_CONTRACT_SO_TOLERANT;                       
            return pMT->CanCastToNonVariantInterface(MethodTable::GetISerializableMT());
        }
        
        BOOL IsIObjectReferenceType(MethodTable *pMT)
        {
            WRAPPER_CONTRACT;
            STATIC_CONTRACT_SO_TOLERANT;            
           return pMT->CanCastToNonVariantInterface(MethodTable::GetIObjectReferenceMT());
        }

        BOOL RequiresDeserializationCallback(MethodTable *pMT)
        {
           WRAPPER_CONTRACT;
            STATIC_CONTRACT_SO_TOLERANT;                                  
            return pMT->CanCastToNonVariantInterface(MethodTable::GetIDeserializationCBMT());
        }

        BOOL RequiresDeepCopy(OBJECTREF refObj)
        {
            LEAF_CONTRACT;
            return TRUE;
        }

    private:
        void CheckSerializable(MethodTable *pCurrMT)
        {
            CONTRACTL
            {
                GC_TRIGGERS;
                MODE_COOPERATIVE;
                THROWS;
            }
            CONTRACTL_END;

            // Checking whether the type is marked as Serializable is not enough, all of its ancestor types must be marked this way
            // also. THe only exception is that any type that also implements ISerializable doesn't require a serializable parent.
            if (IsTdSerializable(pCurrMT->GetClass()->GetAttrClass()))
            {
                MethodTable *pISerializableMT = g_Mscorlib.FetchClass(CLASS__ISERIALIZABLE);
                MethodTable *pMT = pCurrMT;
                for (;;)
                {
                    // We've already checked this particular type is marked Serializable, so if it implements ISerializable then
                    // we're done.
                    if (pMT->ImplementsInterface(pISerializableMT))
                        return;

                    // Else we get the parent type and check it is marked Serializable as well.
                    pMT = pMT->GetParentMethodTable();

                    // If we've run out of parents we're done and the type is serializable.
                    if (pMT == NULL)
                        return;

                    // Otherwise check for the attribute.
                    if (!IsTdSerializable(pMT->GetClass()->GetAttrClass()))
                        break;
                }
            }
         
            if (pCurrMT->IsMarshaledByRef())
                return;

            if (pCurrMT->IsTransparentProxyType())
                return;

            if (pCurrMT->IsEnum())
                return;

            if (pCurrMT->IsAnyDelegateClass())
                return;

            DefineFullyQualifiedNameForClassW();
            LPCWSTR wszCliTypeName = GetFullyQualifiedNameForClassNestedAwareW(pCurrMT->GetClass());

            SString ssAssemblyName;
            pCurrMT->GetAssembly()->GetDisplayName(ssAssemblyName);
            COMPlusThrow(kSerializationException, IDS_SERIALIZATION_NONSERTYPE, wszCliTypeName, ssAssemblyName.GetUnicode());
        }
};


class CrossDomainFieldMap
{
    struct FieldMapEntry
    {
        ADID         m_dwSrcDomain;
        ADID         m_dwDstDomain;
        MethodTable *m_pSrcMT;
        MethodTable *m_pDstMT;
        FieldDesc  **m_pFieldMap;

        FieldMapEntry(MethodTable *pSrcMT, MethodTable *pDstMT, FieldDesc **pFieldMap);
        ~FieldMapEntry()
        {
            LEAF_CONTRACT;
            delete [] m_pFieldMap;
        }

        UPTR GetHash()
        {
            LEAF_CONTRACT;
            return (UINT)(SIZE_T)m_pSrcMT + ((UINT)(SIZE_T)m_pDstMT >> 2);
        }
    };

    static PtrHashMap      *s_pFieldMap;
    static SimpleRWLock    *s_pFieldMapLock;

    static BOOL CompareFieldMapEntry(UPTR val1, UPTR val2);

public:
    static void FlushStaleEntries();
    static FieldDesc **LookupOrCreateFieldMapping(MethodTable *pDstMT, MethodTable *pSrcMT);
};

// Currently the object cloner uses DWORDs as indices.  We may have to use QWORDs instead 
// if we start to have extremely large object graphs.
class ObjectClone
{
    OBJECTREF   m_QOMObjects[QOM_STACK_TO_HEAP_THRESHOLD];
    BYTE        m_QOMData[QOM_STACK_TO_HEAP_THRESHOLD * MAGIC_FACTOR];

    OBJECTREF   m_QOFObjects[QOF_STACK_TO_HEAP_THRESHOLD];
    BYTE        m_QOFData[QOF_STACK_TO_HEAP_THRESHOLD * MAGIC_FACTOR];

    OBJECTREF   m_TSOObjects1[TSO_STACK_TO_HEAP_THRESHOLD];
    OBJECTREF   m_TSOObjects2[TSO_STACK_TO_HEAP_THRESHOLD];
    OBJECTREF   m_TSOObjects3[TSO_STACK_TO_HEAP_THRESHOLD];
    BYTE        m_TSOData[TSO_STACK_TO_HEAP_THRESHOLD * MAGIC_FACTOR];
    DWORD       m_TSOIndices[TSO_STACK_TO_HEAP_THRESHOLD];

    OBJECTREF   m_TDCObjects[TDC_STACK_TO_HEAP_THRESHOLD];
    BYTE        m_TDCData[TDC_STACK_TO_HEAP_THRESHOLD * MAGIC_FACTOR];

    OBJECTREF   m_VSCObjects[VSC_STACK_TO_HEAP_THRESHOLD];

    OBJECTREF   m_VDCObjects[VDC_STACK_TO_HEAP_THRESHOLD];

    GCSafeObjectTable                QOM;   // Queue_of_Object_to_be_Marshalled
    GCSafeObjectTable                QOF;   // Queue_of_Objects_to_be_Fixed_Up
    GCSafeObjectHashTable            TOS;   // Table_of_Objects_Seen
    GCSafeObjectTable                TSO;   // Table_of_Special_Objects
    GCSafeObjectTable                TDC;   // Table_of_Deserialization_Callbacks
    GCSafeObjectTable                VSC;   // Vts_Serialization_Callbacks
    GCSafeObjectTable                VDC;   // Vts_Deserialization_Callbacks
    DwordArrayList                   TMappings;

    FrameWithCookie<CustomGCFrame>  QOM_Protector;
    FrameWithCookie<CustomGCFrame>  QOF_Protector;
    FrameWithCookie<CustomGCFrame>  TOS_Protector;
    FrameWithCookie<CustomGCFrame>  TSO_Protector;
    FrameWithCookie<CustomGCFrame>  TDC_Protector;
    FrameWithCookie<CustomGCFrame>  VSC_Protector;
    FrameWithCookie<CustomGCFrame>  VDC_Protector;
    
    BOOL                m_skipFieldScan;
    
    AppDomain*           m_fromDomain;
    AppDomain*           m_toDomain;
    
    OBJECTREF           m_currObject;   // Updated within the loop in Clone method
    OBJECTREF           m_newObject;    // Updated within the loop in Clone method
    OBJECTREF           m_topObject;
    OBJECTREF           m_fromExecutionContext; // Copy of the execution context on the way in (used during callbacks to the from domain)

    BOOL                m_securityChecked;

    // AppDomain object leak protection: predicate which flips to false once we should stop reporting GC references in the
    // collections this cloner owns.
    BOOL                m_fReportRefs;
    
    IObjectCloneCallback    *m_cbInterface;
    CloningContext      m_context;
        
    static MethodDesc   *s_pMDPrepareData;
    static MethodDesc   *s_pMDGetObjData;
    static BYTE         *s_pAddrOfGetObjData;
    static BYTE         *s_pAddrOfPrepareData;
    static MethodTable  *s_pMTObjRef;

    void EnsureMDInitialized();

    PTRARRAYREF AllocateISerializable(int objectId, BOOL bIsRemotingObject);
    void AllocateArray();
    void AllocateObject();

    PTRARRAYREF MakeObjectLookLikeISerializable(int objectId);
    
    void HandleISerializableFixup(OBJECTREF refParent, QueuedObjectInfo *currObjFixupInfo);
    void HandleArrayFixup(OBJECTREF refParent, QueuedObjectInfo *currObjFixupInfo);
    void HandleObjectFixup(OBJECTREF refParent, QueuedObjectInfo *currObjFixupInfo);
    void Fixup(OBJECTREF newObj, OBJECTREF refParent, QueuedObjectInfo *currObjFixupInfo);

    void ScanMemberFields(DWORD IObjRefTSOIndex, DWORD BoxedValTSOIndex);
    DWORD CloneField(FieldDesc *pSrcField, FieldDesc *pDstField);
    static BOOL AreTypesEmittedIdentically(MethodTable *pMT1, MethodTable *pMT2);
    void ScanISerializableMembers(DWORD IObjRefTSOIndex, DWORD ISerTSOIndex, DWORD BoxedValTSOIndex, PTRARRAYREF refValues);
    void ScanArrayMembers();
    Object *GetObjectFromArray(BASEARRAYREF* arrObj, DWORD dwOffset);

    void CompleteValueTypeFields(OBJECTREF newObj, OBJECTREF refParent, QueuedObjectInfo *valTypeInfo);
    void CompleteSpecialObjects();
    void CompleteISerializableObject(OBJECTREF IserObj, OBJECTREF refNames, OBJECTREF refValues, ISerializableInstanceInfo *);
    BOOL CompleteIObjRefObject(OBJECTREF IObjRef, DWORD index, IObjRefInstanceInfo *iorInfo);
    void CompleteIDeserializationCallbacks();
    void CompleteVtsOnDeserializedCallbacks();
    void CompleteVtsOnSerializedCallbacks();
    BOOL CheckForUnresolvedMembers(SpecialObjectInfo *splInfo);

    TypeHandle GetCorrespondingTypeForTargetDomain(TypeHandle thCli);
    MethodTable * GetCorrespondingTypeForTargetDomain(MethodTable * pCliMT);
    TypeHandle GetType(const SString &ssTypeName, const SString &ssAssemName);

    DWORD FindObjectInTSO(int objId, SpecialObjects kind);
    ARG_SLOT HandleFieldTypeMismatch(CorElementType srcTy, CorElementType destTy, void *pData, MethodTable *pSrcMT);
    BOOL IsDelayedFixup(MethodTable *newMT, QueuedObjectInfo *);
    OBJECTREF BoxValueTypeInWrongDomain(OBJECTREF refParent, DWORD offset, MethodTable *pValueTypeMT);    

    BOOL HasVtsCallbacks(MethodTable *pMT, RemotingVtsInfo::VtsCallbackType eCallbackType);
    void InvokeVtsCallbacks(OBJECTREF refTarget, RemotingVtsInfo::VtsCallbackType eCallbackType, AppDomain* pDomain);

    RuntimeMethodHandle::StreamingContextStates GetStreamingContextState()
    {
        LEAF_CONTRACT;

        if (m_context == CrossAppDomain)
            return RuntimeMethodHandle::CONTEXTSTATE_CrossAppDomain;

        if (m_context == ObjectFreezer)
            return RuntimeMethodHandle::CONTEXTSTATE_Other;

        _ASSERTE(!"Should not get here; using the cloner with a context we don't understand");
        return RuntimeMethodHandle::CONTEXTSTATE_Other;
    }
public:
    
    void Init(BOOL bInitialInit)
    {
        WRAPPER_CONTRACT;

        if (bInitialInit)
        {
            TOS.Init(&m_fReportRefs);
            TSO.Init(&m_TSOObjects1[0], &m_TSOObjects2[0], &m_TSOObjects3[0], &m_TSOIndices[0], &m_TSOData[0], TSO_STACK_TO_HEAP_THRESHOLD, LIFO_QUEUE, &m_fReportRefs);
        }
        QOM.Init(&m_QOMObjects[0], &m_QOMData[0], QOM_STACK_TO_HEAP_THRESHOLD, FIFO_QUEUE, &m_fReportRefs);
        QOF.Init(&m_QOFObjects[0], &m_QOFData[0], QOF_STACK_TO_HEAP_THRESHOLD, FIFO_QUEUE, &m_fReportRefs);
        TDC.Init(&m_TDCObjects[0], &m_TDCData[0], TDC_STACK_TO_HEAP_THRESHOLD, FIFO_QUEUE, &m_fReportRefs);
        VSC.Init(&m_VSCObjects[0], NULL, VSC_STACK_TO_HEAP_THRESHOLD, FIFO_QUEUE, &m_fReportRefs);
        VDC.Init(&m_VDCObjects[0], NULL, VDC_STACK_TO_HEAP_THRESHOLD, FIFO_QUEUE, &m_fReportRefs);
        TMappings.Init();
    }
    void Cleanup(BOOL bFinalCleanup)
    {
        WRAPPER_CONTRACT;
        if (bFinalCleanup)
        {
            TOS.Cleanup();
            TSO.Cleanup();
        }

        QOM.Cleanup();
        QOF.Cleanup();
        TDC.Cleanup();
        VSC.Cleanup();
        VDC.Cleanup();
        TMappings.Cleanup();
    }

    ObjectClone(IObjectCloneCallback *cbInterface, CloningContext cc=CrossAppDomain, BOOL bNeedSecurityCheck = TRUE)
    {
        C_ASSERT((offsetof(ObjectClone, m_QOMObjects) % sizeof(SIZE_T)) == 0);
        C_ASSERT((offsetof(ObjectClone, m_QOFObjects) % sizeof(SIZE_T)) == 0);
        C_ASSERT((offsetof(ObjectClone, m_TSOData)    % sizeof(SIZE_T)) == 0);
        C_ASSERT((offsetof(ObjectClone, m_TDCData)    % sizeof(SIZE_T)) == 0);
        C_ASSERT((offsetof(ObjectClone, m_VSCObjects) % sizeof(SIZE_T)) == 0);
        C_ASSERT((offsetof(ObjectClone, m_VDCObjects) % sizeof(SIZE_T)) == 0);

        m_securityChecked = !bNeedSecurityCheck; 
        m_context = cc;
        m_cbInterface = cbInterface;
        m_fReportRefs = true;

        Init(TRUE);

        // Order of these is important. The frame lowest on the stack (ie declared last inside ObjectClone) has to be pushed first
        (void)new (VDC_Protector.GetGSCookiePtr()) FrameWithCookie<CustomGCFrame>(GCScanRootsInCollection, &VDC);
        (void)new (VSC_Protector.GetGSCookiePtr()) FrameWithCookie<CustomGCFrame>(GCScanRootsInCollection, &VSC);
        (void)new (TDC_Protector.GetGSCookiePtr()) FrameWithCookie<CustomGCFrame>(GCScanRootsInCollection, &TDC);
        (void)new (TSO_Protector.GetGSCookiePtr()) FrameWithCookie<CustomGCFrame>(GCScanRootsInCollection, &TSO);
        (void)new (TOS_Protector.GetGSCookiePtr()) FrameWithCookie<CustomGCFrame>(GCScanRootsInCollection, &TOS);
        (void)new (QOF_Protector.GetGSCookiePtr()) FrameWithCookie<CustomGCFrame>(GCScanRootsInCollection, &QOF);
        (void)new (QOM_Protector.GetGSCookiePtr()) FrameWithCookie<CustomGCFrame>(GCScanRootsInCollection, &QOM);
    }

    void RemoveGCFrames()
    {
        // Order of these is important. The frame highest on the stack has to be pushed first
        QOM_Protector.Pop();
        QOF_Protector.Pop();
        TOS_Protector.Pop();
        TSO_Protector.Pop();
        TDC_Protector.Pop();
        VSC_Protector.Pop();
        VDC_Protector.Pop();
    }

    ~ObjectClone()
    {
        Cleanup(TRUE);
    }
    
    OBJECTREF Clone(OBJECTREF refObj, AppDomain* fromDomain, AppDomain* toDomain, OBJECTREF refExecutionContext)
    {
        TypeHandle thDummy;
        OBJECTREF refResult = Clone(refObj, thDummy, fromDomain, toDomain, refExecutionContext);
        return refResult;
    }
    
    OBJECTREF Clone(OBJECTREF refObj, 
            TypeHandle expectedType, 
            AppDomain *fromDomain, 
            AppDomain *toDomain, 
            OBJECTREF refExecutionContext);

    static void StopReportingRefs(ObjectClone *pThis)
    {
        pThis->m_fReportRefs = false;
    }
};

typedef Holder<ObjectClone *, DoNothing<ObjectClone*>, ObjectClone::StopReportingRefs> ReportClonerRefsHolder;


#endif
#endif
#endif
