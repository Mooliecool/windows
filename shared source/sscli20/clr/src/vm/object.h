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
// OBJECT.H
//
// Definitions of a Com+ Object
//

#ifndef _OBJECT_H_
#define _OBJECT_H_

#include "util.hpp"
#include "syncblk.h"
#include "gcdesc.h"
#include "specialstatics.h"
#include "sstring.h"


#if USE_INDIRECT_GET_THREAD_APPDOMAIN
// Copied from threads.h, since we can't include it here.
typedef AppDomain* (__stdcall *POPTIMIZEDAPPDOMAINGETTER)();
EXTERN_C POPTIMIZEDAPPDOMAINGETTER GetAppDomain;
#else
EXTERN_C AppDomain* GetAppDomain();
#endif

TypeHandle ElementTypeToTypeHandle(const CorElementType type);
TypeHandle GetTypedByrefHandle();

/*
 * COM+ Internal Object Model
 *
 *
 * Object              - This is the common base part to all COM+ objects
 *  |                        it contains the MethodTable pointer and the
 *  |                        sync block index, which is at a negative offset
 *  |
 *  +-- StringObject       - String objects are specialized objects for string
 *  |                        storage/retrieval for higher performance
 *  |
 *  +-- StringBufferObject - StringBuffer instance layout.  
 *  |
 *  +-- BaseObjectWithCachedData - Object Plus one object field for caching.
 *  |       |
 *  |            +-  ReflectClassBaseObject    - The base object for the class Class
 *  |
 *  +-- ArrayBase          - Base portion of all arrays
 *  |       |
 *  |       +-  I1Array    - Base type arrays
 *  |       |   I2Array
 *  |       |   ...
 *  |       |
 *  |       +-  PtrArray   - Array of OBJECTREFs, different than base arrays because of pObjectClass
 *  |              
 *  +-- AppDomainBaseObject - The base object for the class AppDomain
 *  |              
 *  +-- AssemblyBaseObject - The base object for the class Assembly
 *  |
 *  +-- ContextBaseObject   - base object for class Context
 *
 *
 * PLEASE NOTE THE FOLLOWING WHEN ADDING A NEW OBJECT TYPE:
 *
 *    The size of the object in the heap must be able to be computed
 *    very, very quickly for GC purposes.   Restrictions on the layout
 *    of the object guarantee this is possible.
 *
 *    Any object that inherits from Object must be able to
 *    compute its complete size by using the first 4 bytes of
 *    the object following the Object part and constants
 *    reachable from the MethodTable...
 *
 *    The formula used for this calculation is:
 *        MT->GetBaseSize() + ((OBJECTTYPEREF->GetSizeField() * MT->GetComponentSize())
 *
 *    So for Object, since this is of fixed size, the ComponentSize is 0, which makes the right side
 *    of the equation above equal to 0 no matter what the value of GetSizeField(), so the size is just the base size.
 *       
 */


class MethodTable;
class Thread;
class LocalDataStore;
class BaseDomain;
class Assembly;
class Context;
class CtxStaticData;
class DomainAssembly;
class AssemblyNative;
class WaitHandleNative;


#if CHECK_APP_DOMAIN_LEAKS

class Object;

class SetAppDomainAgilePendingTable
{
public:

    SetAppDomainAgilePendingTable ();
    ~SetAppDomainAgilePendingTable ();

    void PushReference (Object *pObject)
    {
        STATIC_CONTRACT_THROWS;
        STATIC_CONTRACT_GC_NOTRIGGER;

        PendingEntry entry;
        entry.pObject = pObject;

        m_Stack.Push(entry);
    }

    void PushParent (Object *pObject)
    {
        STATIC_CONTRACT_THROWS;
        STATIC_CONTRACT_GC_NOTRIGGER;

        PendingEntry entry;
        entry.pObject = (Object*)((size_t)pObject | 1);

        m_Stack.Push(entry);
    }

    Object *GetPendingObject (bool *pfReturnedToParent)
    {
        STATIC_CONTRACT_THROWS;
        STATIC_CONTRACT_GC_NOTRIGGER;

        if (!m_Stack.Count())
            return NULL;

        PendingEntry *pPending = m_Stack.Pop();

        *pfReturnedToParent = pPending->fMarked != 0;
        return (Object*)((size_t)pPending->pObject & ~1);
    }

private:

    union PendingEntry
    {
        Object *pObject;

        // Indicates whether the current thread set BIT_SBLK_AGILE_IN_PROGRESS
        // on the object.  Entries without this flag set are unexplored
        // objects.
        size_t fMarked:1;
    };

    CStackArray<PendingEntry> m_Stack;
};

#endif


//
// The generational GC requires that every object be at least 12 bytes
// in size.   

#define MIN_OBJECT_SIZE     (2*sizeof(BYTE*) + sizeof(ObjHeader))

/*
 * Object
 *
 * This is the underlying base on which objects are built.   The MethodTable
 * pointer and the sync block index live here.  The sync block index is actually
 * at a negative offset to the instance.  See syncblk.h for details.
 *
 */
 
class Object
{
  protected:
    MethodTable*    m_pMethTab;

  protected:
    Object() { LEAF_CONTRACT; };
   ~Object() { LEAF_CONTRACT; };
   
  public:
#ifndef DACCESS_COMPILE    
    VOID SetMethodTable(MethodTable *pMT)
    { 
        LEAF_CONTRACT;
        m_pMethTab = pMT; 
    }
 
#endif // #ifndef DACCESS_COMPILE

    // An object might be a proxy of some sort, with a thunking VTable.  If so, we can
    // advance to the true method table or class.
    BOOL            IsThunking()                        
    { 
        WRAPPER_CONTRACT;
        return( GetMethodTable()->IsThunking() ); 
    }

#define MARKED_BIT 0x1

    MethodTable    *GetMethodTable() const              
    { 
        LEAF_CONTRACT;

#if !defined(DACCESS_COMPILE)
        // We should always use GetGCSafeMethodTable() if we're running during a GC. 
        // If the mark bit is set then we're running during a GC     
        _ASSERTE((((size_t)m_pMethTab) & MARKED_BIT) == 0);
#endif // !DACCESS_COMPILE

        return PTR_MethodTable((TADDR) m_pMethTab);
    }
    MethodTable    *GetTrueMethodTable();
    
    EEClass*        GetClass()                          
    { 
        WRAPPER_CONTRACT;
        return( GetMethodTable()->GetClass() ); 
    }
    TypeHandle      GetTypeHandle();
    TypeHandle      GetTrueTypeHandle();

        // Methods used to determine if an object supports a given interface.
    static BOOL     SupportsInterface(OBJECTREF pObj, MethodTable *pInterfaceMT);

    inline DWORD    GetNumComponents();
    inline SIZE_T   GetSize();

    CGCDesc*        GetSlotMap()                        
    { 
        WRAPPER_CONTRACT;
        return( CGCDesc::GetCGCDescFromMT(GetMethodTable())); 
    }

    // Sync Block & Synchronization services

    // Access the ObjHeader which is at a negative offset on the object (because of
    // cache lines)
    ObjHeader   *GetHeader()
    {
        LEAF_CONTRACT;
        return PTR_ObjHeader(PTR_HOST_TO_TADDR(this) - sizeof(ObjHeader));
    }

    // Get the current address of the object (works for debug refs, too.)
    BYTE        *GetAddress()
    {
        LEAF_CONTRACT;
        return (BYTE*) PTR_HOST_TO_TADDR(this);
    }

#ifdef _DEBUG
    // TRUE if the header has a real SyncBlockIndex (i.e. it has an entry in the
    // SyncTable, though it doesn't necessarily have an entry in the SyncBlockCache)
    BOOL HasEmptySyncBlockInfo()
    {
        WRAPPER_CONTRACT;
        return GetHeader()->HasEmptySyncBlockInfo();
    }
#endif

    // retrieve or allocate a sync block for this object
    SyncBlock *GetSyncBlock()
    {
        WRAPPER_CONTRACT;
        return GetHeader()->GetSyncBlock();
    }

    DWORD GetSyncBlockIndex()
    {
        WRAPPER_CONTRACT;
        return GetHeader()->GetSyncBlockIndex();
    }

    ADIndex GetAppDomainIndex();

    // Get app domain of object, or NULL if it is agile
    AppDomain *GetAppDomain();

    // Set app domain of object to current domain.
    void SetAppDomain() { WRAPPER_CONTRACT; SetAppDomain(::GetAppDomain()); }
    
    // Set app domain of object to given domain - it can only be set once
    void SetAppDomain(AppDomain *pDomain);

#ifdef _DEBUG
    // For SO-tolerance contract violation purposes, define these DEBUG_ versions to identify
    // the codepaths to SetAppDomain that are called only from DEBUG code.
    void DEBUG_SetAppDomain()
    {
        WRAPPER_CONTRACT;

        DEBUG_SetAppDomain(::GetAppDomain());
    }

    void DEBUG_SetAppDomain(AppDomain *pDomain);
#endif    
    
#if CHECK_APP_DOMAIN_LEAKS

    // Mark object as app domain agile
    BOOL SetAppDomainAgile(BOOL raiseAssert=TRUE, SetAppDomainAgilePendingTable *pTable = NULL);
    BOOL TrySetAppDomainAgile(BOOL raiseAssert=TRUE);

    // Mark sync block as app domain agile
    void SetSyncBlockAppDomainAgile();

    // Check if object is app domain agile
    BOOL IsAppDomainAgile();

    // Check if object is app domain agile
    BOOL IsAppDomainAgileRaw()
    {
        WRAPPER_CONTRACT;

        SyncBlock *psb = PassiveGetSyncBlock();

        return (psb && psb->IsAppDomainAgile());
    }

    BOOL Object::IsCheckedForAppDomainAgile()
    {
        WRAPPER_CONTRACT;

        SyncBlock *psb = PassiveGetSyncBlock();
        return (psb && psb->IsCheckedForAppDomainAgile());
    }

    void Object::SetIsCheckedForAppDomainAgile()
    {
        WRAPPER_CONTRACT;

        SyncBlock *psb = PassiveGetSyncBlock();
        if (psb)
            psb->SetIsCheckedForAppDomainAgile();
    }

    // Check object to see if it is usable in the current domain 
    BOOL CheckAppDomain() { WRAPPER_CONTRACT; return CheckAppDomain(::GetAppDomain()); }

    //Check object to see if it is usable in the given domain 
    BOOL CheckAppDomain(AppDomain *pDomain);

    // Check if the object's type is app domain agile
    BOOL IsTypeAppDomainAgile();

    // Check if the object's type is conditionally app domain agile
    BOOL IsTypeCheckAppDomainAgile();

    // Check if the object's type is naturally app domain agile
    BOOL IsTypeTypesafeAppDomainAgile();

    // Check if the object's type is possibly app domain agile
    BOOL IsTypeNeverAppDomainAgile();

    // Validate object & fields to see that it's usable from the current app domain
    BOOL ValidateAppDomain() { WRAPPER_CONTRACT; return ValidateAppDomain(::GetAppDomain()); }

    // Validate object & fields to see that it's usable from any app domain
    BOOL ValidateAppDomainAgile() { WRAPPER_CONTRACT; return ValidateAppDomain(NULL); }

    // Validate object & fields to see that it's usable from the given app domain (or null for agile)
    BOOL ValidateAppDomain(AppDomain *pAppDomain);

    // Validate fields to see that they are usable from the object's app domain 
    // (or from any domain if the object is agile)
    BOOL ValidateAppDomainFields() { WRAPPER_CONTRACT; return ValidateAppDomainFields(GetAppDomain()); }

    // Validate fields to see that they are usable from the given app domain (or null for agile)
    BOOL ValidateAppDomainFields(AppDomain *pAppDomain);

    // Validate a value type's fields to see that it's usable from the current app domain
    static BOOL ValidateValueTypeAppDomain(MethodTable *pMT, void *base, BOOL raiseAssert = TRUE) 
      { WRAPPER_CONTRACT; return ValidateValueTypeAppDomain(pMT, base, ::GetAppDomain(), raiseAssert); }

    // Validate a value type's fields to see that it's usable from any app domain
    static BOOL ValidateValueTypeAppDomainAgile(MethodTable *pMT, void *base, BOOL raiseAssert = TRUE) 
      { WRAPPER_CONTRACT; return ValidateValueTypeAppDomain(pMT, base, NULL, raiseAssert); }

    // Validate a value type's fields to see that it's usable from the given app domain (or null for agile)
    static BOOL ValidateValueTypeAppDomain(MethodTable *pMT, void *base, AppDomain *pAppDomain, BOOL raiseAssert = TRUE);

    // Call when we are assigning this object to a dangerous field 
    // in an object in a given app domain (or agile if null)
    BOOL AssignAppDomain(AppDomain *pAppDomain, BOOL raiseAssert = TRUE);
    BOOL TryAssignAppDomain(AppDomain *pAppDomain, BOOL raiseAssert = TRUE);

    // Call when we are assigning to a dangerous value type field 
    // in an object in a given app domain (or agile if null)
    static BOOL AssignValueTypeAppDomain(MethodTable *pMT, void *base, AppDomain *pAppDomain, BOOL raiseAssert = TRUE);

#endif // CHECK_APP_DOMAIN_LEAKS

    // DO NOT ADD ANY ASSERTS TO THIS METHOD.
    // DO NOT USE THIS METHOD.
    // Yes folks, for better or worse the debugger pokes supposed object addresses 
    // to try to see if objects are valid, possibly firing an AccessViolation or worse,
    // and then catches the AV and reports a failure to the debug client.  This makes
    // the debugger slightly more robust should any corrupted object references appear
    // in a session. Thus it is "correct" behaviour for this to AV when used with 
    // an invalid object pointer, and incorrect behaviour for it to
    // assert.  
    BOOL ValidateObjectWithPossibleAV();

    // Validate an object ref out of the Promote routine in the GC
    void ValidatePromote(ScanContext *sc, DWORD flags);

    // Validate an object ref out of the VerifyHeap routine in the GC
    void ValidateHeap(Object *from);

    SyncBlock *PassiveGetSyncBlock()
    {
        WRAPPER_CONTRACT;
        return GetHeader()->PassiveGetSyncBlock();
    }

        // COM Interop has special access to sync blocks
    // check .cpp file for more info
    SyncBlock* GetSyncBlockSpecial()
    {
        WRAPPER_CONTRACT;
        return GetHeader()->GetSyncBlockSpecial();
    }

    static DWORD ComputeHashCode();

    // Synchronization
#ifndef DACCESS_COMPILE

    void EnterObjMonitor()
    {
        WRAPPER_CONTRACT;
        GetHeader()->EnterObjMonitor();
    }

    BOOL TryEnterObjMonitor(INT32 timeOut = 0)
    {
        WRAPPER_CONTRACT;
        return GetHeader()->TryEnterObjMonitor(timeOut);
    }

    BOOL LeaveObjMonitor()
    {
        WRAPPER_CONTRACT;
        return GetHeader()->LeaveObjMonitor();
    }
    
    // should be called only from unwind code; used in the
    // case where EnterObjMonitor failed to allocate the
    // sync-object.
    BOOL LeaveObjMonitorAtException()
    {
        WRAPPER_CONTRACT;
        return GetHeader()->LeaveObjMonitorAtException();
    }

    FORCEINLINE AwareLock::LeaveHelperAction LeaveObjMonitorHelper(Thread* pCurThread)
    {
        WRAPPER_CONTRACT;
        return GetHeader()->LeaveObjMonitorHelper(pCurThread);
    }

    BOOL IsObjMonitorOwnedByThread(Thread* pExpectedThread)
    {
        WRAPPER_CONTRACT;
        return GetHeader()->IsObjMonitorOwnedByThread(pExpectedThread);
    }

#endif // #ifndef DACCESS_COMPILE

    BOOL Wait(INT32 timeOut, BOOL exitContext)
    {
        WRAPPER_CONTRACT;
        return GetHeader()->Wait(timeOut, exitContext);
    }

    void Pulse()
    {
        WRAPPER_CONTRACT;
        GetHeader()->Pulse();
    }

    void PulseAll()
    {
        WRAPPER_CONTRACT;
        GetHeader()->PulseAll();
    }

    void* UnBox();      // if it is a value class, get the pointer to the first field
  
    BYTE*   GetData(void)
    {
        LEAF_CONTRACT;
        return (BYTE*)(PTR_HOST_TO_TADDR(this) + sizeof(Object));
    }

    static UINT GetOffsetOfFirstField()
    {
        LEAF_CONTRACT;
        return sizeof(Object);
    }

    DWORD   GetOffset32(DWORD dwOffset)
    { 
        WRAPPER_CONTRACT;
        return *PTR_DWORD((TADDR)GetData() + dwOffset);
    }

    USHORT  GetOffset16(DWORD dwOffset)
    { 
        WRAPPER_CONTRACT;
        return *PTR_USHORT((TADDR)GetData() + dwOffset);
    }

    BYTE    GetOffset8(DWORD dwOffset)
    { 
        WRAPPER_CONTRACT;
        return *PTR_BYTE((TADDR)GetData() + dwOffset);
    }

    __int64 GetOffset64(DWORD dwOffset)
    { 
        WRAPPER_CONTRACT;
        return (__int64)*PTR_ULONG64((TADDR)GetData() + dwOffset);
    }

    void *GetPtrOffset(DWORD dwOffset)
    {
        WRAPPER_CONTRACT;
        return (void*)(TADDR)*PTR_TADDR((TADDR)GetData() + dwOffset);
    }

#ifndef DACCESS_COMPILE
    
    void SetOffsetObjectRef(DWORD dwOffset, size_t dwValue);

    void SetOffsetPtr(DWORD dwOffset, LPVOID value)
    {
        WRAPPER_CONTRACT;
        *(LPVOID *) &GetData()[dwOffset] = value;
    }
        
    void SetOffset32(DWORD dwOffset, DWORD dwValue)
    { 
        WRAPPER_CONTRACT;
        *(DWORD *) &GetData()[dwOffset] = dwValue;
    }

    void SetOffset16(DWORD dwOffset, DWORD dwValue)
    { 
        WRAPPER_CONTRACT;
        *(USHORT *) &GetData()[dwOffset] = (USHORT) dwValue;
    }

    void SetOffset8(DWORD dwOffset, DWORD dwValue)
    { 
        WRAPPER_CONTRACT;
        *(BYTE *) &GetData()[dwOffset] = (BYTE) dwValue;
    }

    void SetOffset64(DWORD dwOffset, __int64 qwValue)
    { 
        WRAPPER_CONTRACT;
        *(__int64 *) &GetData()[dwOffset] = qwValue;
    }

#endif // #ifndef DACCESS_COMPILE

    VOID            Validate(BOOL bDeep=TRUE, BOOL bVerifyNextHeader = TRUE);

    MethodTable *GetGCSafeMethodTable() const
    {
        LEAF_CONTRACT;

        // lose GC marking bit and the pinning bit
        // A method table pointer should always be aligned.  During GC we set the least 
        // significant bit for marked objects and we set the second to least significant
        // bit for pinned objects.  So if we want the actual MT pointer during a GC
        // we must zero out the lowest 2 bits.
        return PTR_MethodTable(((TADDR) m_pMethTab) & ~((UINT_PTR)3));
    }

#ifdef DACCESS_COMPILE
    void EnumMemoryRegions(void);
#endif
    
 private:

#if CHECK_APP_DOMAIN_LEAKS
    friend class ObjHeader;
    BOOL SetFieldsAgile(BOOL raiseAssert = TRUE, SetAppDomainAgilePendingTable *pTable = NULL);
    static BOOL SetClassFieldsAgile(MethodTable *pMT, void *base, BOOL baseIsVT, BOOL raiseAssert = TRUE, SetAppDomainAgilePendingTable *pTable = NULL); 
    static BOOL ValidateClassFields(MethodTable *pMT, void *base, BOOL baseIsVT, AppDomain *pAppDomain, BOOL raiseAssert = TRUE);
    BOOL SetAppDomainAgileWorker(BOOL raiseAssert, SetAppDomainAgilePendingTable *pTable);
    BOOL ShouldCheckAppDomainAgile(BOOL raiseAssert, BOOL *pfResult);

#endif
};

#if defined (_MSC_VER) && _MSC_VER <= 1300
template void DoNothing(OBJECTREF*);
template BOOL CompareDefault(OBJECTREF*,OBJECTREF*);
#endif

/*
 * Object ref setting routines.  You must use these to do 
 * proper write barrier support, as well as app domain 
 * leak checking.
 *
 * Note that the AppDomain parameter is the app domain affinity
 * of the object containing the field or value class.  It should
 * be NULL if the containing object is app domain agile. Note that
 * you typically get this value by calling obj->GetAppDomain() on 
 * the containing object.
 */

// SetObjectReference sets an OBJECTREF field

void SetObjectReferenceUnchecked(OBJECTREF *dst,OBJECTREF ref);

#ifdef _DEBUG
void EnableStressHeapHelper();
#endif

//Used to clear the object reference
inline void ClearObjectReference(OBJECTREF* dst) 
{ 
    LEAF_CONTRACT;
    *(void**)(dst) = NULL; 
}

// CopyValueClass sets a value class field

void __stdcall CopyValueClassUnchecked(void* dest, void* src, MethodTable *pMT);

inline void InitValueClass(void *dest, MethodTable *pMT)
{ 
    WRAPPER_CONTRACT;
    ZeroMemory(dest, pMT->GetNumInstanceFieldBytes()); 
    IA64WriteMemoryBarrier();
}

#if CHECK_APP_DOMAIN_LEAKS

void SetObjectReferenceChecked(OBJECTREF *dst,OBJECTREF ref, AppDomain *pAppDomain);
void CopyValueClassChecked(void* dest, void* src, MethodTable *pMT, AppDomain *pAppDomain);

#define SetObjectReference(_d,_r,_a)        SetObjectReferenceChecked(_d, _r, _a)
#define CopyValueClass(_d,_s,_m,_a)         CopyValueClassChecked(_d,_s,_m,_a)      

#else

#define SetObjectReference(_d,_r,_a)        SetObjectReferenceUnchecked(_d, _r)
#define CopyValueClass(_d,_s,_m,_a)         CopyValueClassUnchecked(_d,_s,_m)       

#endif

#include <pshpack4.h>


// N/Direct marshaling will pin scalar arrays with more than this many
// components (limit is in terms of components rather than byte size to
// speed up the check.)
#define ARRAYPINLIMIT 10


// There are two basic kinds of array layouts in COM+
//      ELEMENT_TYPE_ARRAY  - a multidimensional array with lower bounds on the dims
//      ELMENNT_TYPE_SZARRAY - A zero based single dimensional array
//
// In addition the layout of an array in memory is also affected by
// whether the method table is shared (eg in the case of arrays of object refs)
// or not.  In the shared case, the array has to hold the type handle of
// the element type.  
//
// ArrayBase encapuslates all of these details.  In theory you should never
// have to peek inside this abstraction
//
class ArrayBase : public Object
{
    friend class GCHeap;
    friend class CObjectHeader;
    friend class Object;
    friend OBJECTREF AllocateArrayEx(TypeHandle arrayClass, INT32 *pArgs, DWORD dwNumArgs, BOOL bAllocateInLargeHeap); 
    friend OBJECTREF FastAllocatePrimitiveArray(MethodTable* arrayType, DWORD cElements, BOOL bAllocateInLargeHeap);
    friend class JIT_TrialAlloc;
    friend class CheckAsmOffsets;

private:
    // This MUST be the first field, so that it directly follows Object.  This is because
    // Object::GetSize() looks at m_NumComponents even though it may not be an array (the
    // values is shifted out if not an array, so it's ok). 
    DWORD       m_NumComponents;

    SVAL_DECL(INT32, s_arrayBoundsZero); // = 0

    // What comes after this conceputally is:
    // TypeHandle elementType;        Only present if the method table is shared among many types (arrays of pointers)
    // INT32      bounds[rank];       The bounds are only present for Multidimensional arrays   
    // INT32      lowerBounds[rank];  Valid indexes are lowerBounds[i] <= index[i] < lowerBounds[i] + bounds[i]

    void SetArrayElementTypeHandle(TypeHandle value) 
    {
        LEAF_CONTRACT;

        _ASSERTE_IMPL(value.Verify());
        _ASSERTE(GetMethodTable()->IsSharedByReferenceArrayTypes());
        *((TypeHandle*) (this+1)) = value;
    }

public:
    // Gets the unique type handle for this array object.
    // This will call the loader in don't-load mode - the allocator
    // always makes sure that the particular ArrayTypeDesc for this array
    // type is available before allocating any instances of this array type.
    TypeHandle GetTypeHandle() const;

    // Get the element type for the array, this works whether the the element
    // type is stored in the array or not
    inline TypeHandle GetArrayElementTypeHandle() const;

        // Get the CorElementType for the elements in the array.  Avoids creating a TypeHandle
    inline CorElementType GetArrayElementType() const;

    inline unsigned GetRank() const;

        // Total element count for the array
    inline DWORD GetNumComponents() const;

        // Get pointer to elements, handles any number of dimensions
    BYTE* GetDataPtr(BOOL inGC = FALSE) const {
        WRAPPER_CONTRACT;
#ifdef _DEBUG
#ifndef DACCESS_COMPILE
        EnableStressHeapHelper();
#endif
#endif
        return (BYTE *)(PTR_HOST_TO_TADDR(this) +
                        GetDataPtrOffset(inGC?GetGCSafeMethodTable() : GetMethodTable()));
    }

        // &Array[i]  == GetDataPtr() + GetComponentSize() * i
    unsigned GetComponentSize() const {
        WRAPPER_CONTRACT;
#if CHECK_APP_DOMAIN_LEAKS
        return(GetGCSafeMethodTable()->GetComponentSize());
#else
        return(GetMethodTable()->GetComponentSize());
#endif //CHECK_APP_DOMAIN_LEAKS
    }

        // Note that this can be a multidimensional array of rank 1 
        // (for example if we had a 1-D array with lower bounds
    BOOL IsMultiDimArray() const {
        WRAPPER_CONTRACT;
        return(GetMethodTable()->IsMultiDimArray());
    }

        // Get pointer to the begining of the bounds (counts for each dim)
        // Works for any array type 
    PTR_INT32 GetBoundsPtr() const {
        WRAPPER_CONTRACT;
        if (IsMultiDimArray()) 
        {
            PTR_INT32 ret = PTR_INT32(PTR_HOST_TO_TADDR(this) + sizeof(*this));
            if (GetMethodTable()->IsSharedByReferenceArrayTypes())
            {
                ret = PTR_INT32(PTR_TO_TADDR(ret) + sizeof(TADDR));
            }
            return(ret);
        }
        else
        {
            return PTR_INT32(PTR_HOST_MEMBER_TADDR(ArrayBase, this,
                                                   m_NumComponents));
        }
    }

        // Works for any array type 
    PTR_INT32 GetLowerBoundsPtr() const {
        WRAPPER_CONTRACT;
        if (IsMultiDimArray())
        {
            // Lower bounds info is after total bounds info
            // and total bounds info has rank elements
            return GetBoundsPtr() + GetRank();
        }
        else
            return PTR_INT32(PTR_TO_TADDR(GVAL_ADDR(s_arrayBoundsZero)));
    }

    inline ArrayClass* GetArrayClass() const;

    static unsigned GetOffsetOfNumComponents() {
        LEAF_CONTRACT;
        return offsetof(ArrayBase, m_NumComponents);
    }

    inline static unsigned GetDataPtrOffset(MethodTable* pMT);

    inline static unsigned GetBoundsOffset(MethodTable* pMT);
    inline static unsigned GetLowerBoundsOffset(MethodTable* pMT);
};

//
// Template used to build all the non-object
// arrays of a single dimension
//

template < class KIND >
class Array : public ArrayBase
{
  public:
    KIND          m_Array[1];

    KIND *        GetDirectPointerToNonObjectElements()
    { 
        WRAPPER_CONTRACT;
        // return m_Array; 
        return (KIND *) GetDataPtr(); // This also handles arrays of dim 1 with lower bounds present

    }

    const KIND *  GetDirectConstPointerToNonObjectElements() const
    { 
        WRAPPER_CONTRACT;
        // return m_Array; 
        return (const KIND *) GetDataPtr(); // This also handles arrays of dim 1 with lower bounds present
    }
};


// Warning: Use PtrArray only for single dimensional arrays, not multidim arrays.
class PtrArray : public ArrayBase
{
    friend class GCHeap;
    friend class ClrDataAccess;
    friend OBJECTREF AllocateArrayEx(TypeHandle arrayClass, INT32 *pArgs, DWORD dwNumArgs, BOOL bAllocateInLargeHeap); 
    friend class JIT_TrialAlloc;
    friend class CheckAsmOffsets;


public:

    TypeHandle GetArrayElementTypeHandle()
    {
        LEAF_CONTRACT;
        return m_ElementType;
    }

    static SIZE_T GetDataOffset()
    {
        LEAF_CONTRACT;
        return offsetof(PtrArray, m_Array);
    }

    void SetAt(SIZE_T i, OBJECTREF ref)
    {
        WRAPPER_CONTRACT;
        SetObjectReference(m_Array + i, ref, GetAppDomain());
    }

    void ClearAt(SIZE_T i)
    {
        WRAPPER_CONTRACT;
        ClearObjectReference(m_Array + i);
    }

    OBJECTREF GetAt(SIZE_T i)
    {
        LEAF_CONTRACT;
        return m_Array[i];
    }

    friend class StubLinkerCPU;
private:
    TypeHandle  m_ElementType;
public:
    OBJECTREF    m_Array[1];
};

/* a TypedByRef is a structure that is used to implement VB's BYREF variants.  
   it is basically a tuple of an address of some data along with a TypeHandle
   that indicates the type of the address */
class TypedByRef 
{
public:

    void* data;
    TypeHandle type;  
};

typedef Array<I1>   I1Array;
typedef Array<I2>   I2Array;
typedef Array<I4>   I4Array;
typedef Array<I8>   I8Array;
typedef Array<R4>   R4Array;
typedef Array<R8>   R8Array;
typedef Array<U1>   U1Array;
typedef Array<U1>   BOOLArray;
typedef Array<U2>   U2Array;
typedef Array<WCHAR>   CHARArray;
typedef Array<U4>   U4Array;
typedef Array<U8>   U8Array;
typedef PtrArray    PTRArray;  

typedef DPTR(I1Array)   PTR_I1Array;
typedef DPTR(I2Array)   PTR_I2Array;
typedef DPTR(I4Array)   PTR_I4Array;
typedef DPTR(I8Array)   PTR_I8Array;
typedef DPTR(R4Array)   PTR_R4Array;
typedef DPTR(R8Array)   PTR_R8Array;
typedef DPTR(U1Array)   PTR_U1Array;
typedef DPTR(BOOLArray) PTR_BOOLArray;
typedef DPTR(U2Array)   PTR_U2Array;
typedef DPTR(CHARArray) PTR_CHARArray;
typedef DPTR(U4Array)   PTR_U4Array;
typedef DPTR(U8Array)   PTR_U8Array;
typedef DPTR(PTRArray)  PTR_PTRArray;


#ifdef USE_CHECKED_OBJECTREFS
typedef REF<ArrayBase>  BASEARRAYREF;
typedef REF<I1Array>    I1ARRAYREF;
typedef REF<I2Array>    I2ARRAYREF;
typedef REF<I4Array>    I4ARRAYREF;
typedef REF<I8Array>    I8ARRAYREF;
typedef REF<R4Array>    R4ARRAYREF;
typedef REF<R8Array>    R8ARRAYREF;
typedef REF<U1Array>    U1ARRAYREF;
typedef REF<BOOLArray>  BOOLARRAYREF;
typedef REF<U2Array>    U2ARRAYREF;
typedef REF<U4Array>    U4ARRAYREF;
typedef REF<U8Array>    U8ARRAYREF;
typedef REF<CHARArray>  CHARARRAYREF;
typedef REF<PTRArray>   PTRARRAYREF;  // Warning: Use PtrArray only for single dimensional arrays, not multidim arrays.

#else   // USE_CHECKED_OBJECTREFS

typedef PTR_ArrayBase   BASEARRAYREF;
typedef PTR_I1Array     I1ARRAYREF;
typedef PTR_I2Array     I2ARRAYREF;
typedef PTR_I4Array     I4ARRAYREF;
typedef PTR_I8Array     I8ARRAYREF;
typedef PTR_R4Array     R4ARRAYREF;
typedef PTR_R8Array     R8ARRAYREF;
typedef PTR_U1Array     U1ARRAYREF;
typedef PTR_BOOLArray   BOOLARRAYREF;
typedef PTR_U2Array     U2ARRAYREF;
typedef PTR_U4Array     U4ARRAYREF;
typedef PTR_U8Array     U8ARRAYREF;
typedef PTR_CHARArray   CHARARRAYREF;
typedef PTR_PTRArray    PTRARRAYREF;  // Warning: Use PtrArray only for single dimensional arrays, not multidim arrays.

#endif // USE_CHECKED_OBJECTREFS


#include <poppack.h>


/*
 * StringObject
 *
 * Special String implementation for performance.   
 *
 *   m_ArrayLength  - Length of buffer (m_Characters) in number of WCHARs
 *   m_StringLength - Length of string in number of WCHARs, may be smaller
 *                    than the m_ArrayLength implying that there is extra
 *                    space at the end. The high two bits of this field are used
 *                    to indicate if the String has characters higher than 0x7F
 *   m_Characters   - The string buffer
 *
 */


/**
 *  The high bit state can be one of three value: 
 * STRING_STATE_HIGH_CHARS: We've examined the string and determined that it definitely has values greater than 0x80
 * STRING_STATE_FAST_OPS: We've examined the string and determined that it definitely has no chars greater than 0x80
 * STRING_STATE_UNDETERMINED: We've never examined this string.
 * We've also reserved another bit for future use.
 */

#define STRING_STATE_UNDETERMINED     0x00000000
#define STRING_STATE_HIGH_CHARS       0x40000000
#define STRING_STATE_FAST_OPS         0x80000000
#define STRING_STATE_SPECIAL_SORT     0xC0000000

#ifdef _MSC_VER
#pragma warning(disable : 4200)     // disable zero-sized array warning
#endif
class StringObject : public Object
{
#ifdef DACCESS_COMPILE
    friend class ClrDataAccess;
#endif
    friend class GCHeap;
    friend class JIT_TrialAlloc;
    friend class CheckAsmOffsets;

  private:
    DWORD   m_ArrayLength;
    DWORD   m_StringLength;
    WCHAR   m_Characters[0];

  public:
    VOID    SetArrayLength(DWORD len)                   { LEAF_CONTRACT; m_ArrayLength = len;     }

  protected:
    StringObject() {LEAF_CONTRACT; }
   ~StringObject() {LEAF_CONTRACT; }
   
  public:
    DWORD   GetArrayLength()                            { LEAF_CONTRACT; return( m_ArrayLength ); }
    DWORD   GetStringLength()                           { LEAF_CONTRACT; return( m_StringLength );}
    WCHAR*  GetBuffer()                                 { LEAF_CONTRACT; _ASSERTE(this); return (WCHAR*)( PTR_HOST_TO_TADDR(this) + offsetof(StringObject, m_Characters) );  }
    WCHAR*  GetBufferNullable()                         { LEAF_CONTRACT; return( (this == 0) ? 0 : (WCHAR*)( PTR_HOST_TO_TADDR(this) + offsetof(StringObject, m_Characters) ) );  }

    VOID    SetStringLength(DWORD len) { 
                LEAF_CONTRACT; 
                _ASSERTE( len <= m_ArrayLength );
                m_StringLength = len;
    }

    DWORD GetHighCharState() {
        WRAPPER_CONTRACT;
        DWORD ret = GetHeader()->GetBits() & (BIT_SBLK_STRING_HIGH_CHAR_MASK);
        return ret;
    }

    VOID SetHighCharState(DWORD value) {
        WRAPPER_CONTRACT;
        _ASSERTE(value==STRING_STATE_HIGH_CHARS || value==STRING_STATE_FAST_OPS 
                 || value==STRING_STATE_UNDETERMINED || value==STRING_STATE_SPECIAL_SORT);

        // you need to clear the present state before going to a new state, but we'll allow multiple threads to set it to the same thing.
        _ASSERTE((GetHighCharState() == STRING_STATE_UNDETERMINED) || (GetHighCharState()==value));    

        C_ASSERT(BIT_SBLK_STRING_HAS_NO_HIGH_CHARS == STRING_STATE_FAST_OPS && 
                 STRING_STATE_HIGH_CHARS == BIT_SBLK_STRING_HIGH_CHARS_KNOWN &&
                 STRING_STATE_SPECIAL_SORT == BIT_SBLK_STRING_HAS_SPECIAL_SORT);

        GetHeader()->SetBit(value);
    }

    static UINT GetBufferOffset()
    {
        LEAF_CONTRACT;
        return (UINT)(offsetof(StringObject, m_Characters));
    }

    static UINT GetStringLengthOffset_MaskOffHighBit()
    {
        LEAF_CONTRACT;
        return (UINT)(offsetof(StringObject, m_StringLength));
    }
    
    static UINT GetArrayLengthOffset()
    {
        LEAF_CONTRACT;
        return (UINT)(offsetof(StringObject, m_ArrayLength));
    }

    VOID    GetSString(SString &result)
    {
        WRAPPER_CONTRACT;
        result.Set(GetBuffer(), GetStringLength());
    }
};


// This is used to account for the CachedData member on
//   MemberInfo.
class BaseObjectWithCachedData : public Object
{
    protected:
        OBJECTREF  m_CachedData;   // cached data object (on MemberInfo in managed code, see MemberInfo.cs)
};

// This is the Class version of the Reflection object.
//  A Class has adddition information.
//  For a ReflectClassBaseObject the m_pData is a pointer to a FieldDesc array that
//      contains all of the final static primitives if its defined.
//  m_cnt = the number of elements defined in the m_pData FieldDesc array.  -1 means
//      this hasn't yet been defined.
class ReflectClassBaseObject : public BaseObjectWithCachedData
{
    friend class Binder;

protected:
    OBJECTREF           m_cache;
    TypeHandle          m_typeHandle;

public:
    void SetType(TypeHandle type) {
        LEAF_CONTRACT;
        m_typeHandle = type;
    }

    TypeHandle GetType() {
        LEAF_CONTRACT;
        return m_typeHandle;
    }

};

// ReflectModuleBaseObject 
// This class is the base class for managed Module.
//  This class will connect the Object back to the underlying VM representation
//  m_ReflectClass -- This is the real Class that was used for reflection
//      This class was used to get at this object
//  m_pData -- this is a generic pointer which usually points CorModule
//  
class ReflectModuleBaseObject : public Object
{
    friend class Binder;

  protected:
    // READ ME:
    // Modifying the order or fields of this object may require other changes to the
    //  classlib class definition of this object.
    OBJECTREF          m_TypeBuilderList;
    OBJECTREF          m_ISymWriter;
    OBJECTREF          m_moduleData;    // dynamic module data
    OBJECTREF          m_runtimeType;    
    void*              m_ReflectClass;  // Pointer to the ReflectClass structure
    void*              m_pData;         // Pointer to the ReflectXXXX structure (method,Field, etc)
    void*              m_pInternalSymWriter; // Pointer to the ISymUnmanagedWriter.
    void*              m_pGlobals;      // Global values....
    void*              m_pGlobalsFlds;  // Global Fields....
    mdToken            m_EntryPoint;    // Value type MethodToken is embedded. It only contains one integer field.

  protected:
    ReflectModuleBaseObject() {LEAF_CONTRACT;}
   ~ReflectModuleBaseObject() {LEAF_CONTRACT;}
   
  public:
    // check for classes that wrap Ole classes 

    void SetReflClass(void* classObj)  {
        LEAF_CONTRACT;
        m_ReflectClass = classObj;
    }
    void *GetReflClass() {
        LEAF_CONTRACT;
        return m_ReflectClass;
    }

    void SetData(void* p) {
        LEAF_CONTRACT;
        m_pData = p;
    }
    void* GetData() {
        LEAF_CONTRACT;
        return m_pData;
    }

    void SetInternalSymWriter(void* p) {
        LEAF_CONTRACT;
        m_pInternalSymWriter = p;
    }
    void* GetInternalSymWriter() {
        LEAF_CONTRACT;
        return m_pInternalSymWriter;
    }

    void* GetGlobals() {
        LEAF_CONTRACT;
        return m_pGlobals;
    }
    void SetGlobals(void* p) {
        LEAF_CONTRACT;
        m_pGlobals = p;
    }
    void* GetGlobalFields() {
        LEAF_CONTRACT;
        return m_pGlobalsFlds;
    }
    void SetGlobalFields(void* p) {
        LEAF_CONTRACT;
        m_pGlobalsFlds = p;
    }
};

class SafeHandle;

#ifdef USE_CHECKED_OBJECTREFS
typedef REF<SafeHandle> SAFEHANDLE;
typedef REF<SafeHandle> SAFEHANDLEREF;
#else // USE_CHECKED_OBJECTREFS
typedef SafeHandle * SAFEHANDLE;
typedef SafeHandle * SAFEHANDLEREF;
#endif // USE_CHECKED_OBJECTREFS

class PermissionListSetObject: public Object
{
    friend class Binder;

private:
    OBJECTREF _firstPermSetTriple;
    OBJECTREF _permSetTriples;
    OBJECTREF _zoneList;
    OBJECTREF _originList;

public:
    BOOL IsEmpty() 
    {
        LEAF_CONTRACT;
        return (_firstPermSetTriple == NULL &&
                _permSetTriples == NULL &&
                _zoneList == NULL &&
                _originList == NULL);
    }
};

#ifdef USE_CHECKED_OBJECTREFS
typedef REF<PermissionListSetObject> PERMISSIONLISTSETREF;
#else
typedef PermissionListSetObject*     PERMISSIONLISTSETREF;
#endif

class CompressedStackObject: public Object
{
    friend class Binder;

private:
    // These field are also defined in the managed representation.  (CompressedStack.cs)If you
    // add or change these field you must also change the managed code so that
    // it matches these.  This is necessary so that the object is the proper
    // size. 
    PERMISSIONLISTSETREF m_pls;
    SAFEHANDLEREF m_compressedStackHandle;

public:
    void* GetUnmanagedCompressedStack();
    BOOL IsEmptyPLS() 
    {
        LEAF_CONTRACT;
        return (m_pls == NULL || m_pls->IsEmpty());
    }
};

#ifdef USE_CHECKED_OBJECTREFS
typedef REF<CompressedStackObject> COMPRESSEDSTACKREF;
#else
typedef CompressedStackObject*     COMPRESSEDSTACKREF;
#endif

    

class SecurityContextObject: public Object
{
    friend class Binder;

private:
    
    // These field are also defined in the managed representation.  (SecurityContext.cs)If you
    // add or change these field you must also change the managed code so that
    // it matches these.  This is necessary so that the object is the proper
    // size. 

    OBJECTREF               _executionContext;
    COMPRESSEDSTACKREF      _compressedStack;
    INT32                   _disableFlow;
    CLR_BOOL                _isNewCapture;
public:
    COMPRESSEDSTACKREF GetCompressedStack()
    {
        LEAF_CONTRACT;
        return _compressedStack;
    }
};

#ifdef USE_CHECKED_OBJECTREFS
typedef REF<SecurityContextObject> SECURITYCONTEXTREF;
#else
typedef SecurityContextObject*     SECURITYCONTEXTREF;
#endif

#define SYNCCTXPROPS_REQUIRESWAITNOTIFICATION 0x1 // Keep in sync with SynchronizationContext.cs SynchronizationContextFlags
class ThreadBaseObject;
class SynchronizationContextObject: public Object
{
    friend class Binder;
private:
    // These field are also defined in the managed representation.  (SecurityContext.cs)If you
    // add or change these field you must also change the managed code so that
    // it matches these.  This is necessary so that the object is the proper
    // size. 
    INT32 _props;
public:
    BOOL IsWaitNotificationRequired()
    {
        LEAF_CONTRACT;
        if ((_props & SYNCCTXPROPS_REQUIRESWAITNOTIFICATION) != 0)
            return TRUE;
        return FALSE;
    }
};

class ExecutionContextObject : public Object
{
    friend class Binder;
    
    // These field are also defined in the managed representation.  (ExecutionContext.cs) If you
    // add or change these field you must also change the managed code so that
    // it matches these.  This is necessary so that the object is the proper
    // size.
private :
    OBJECTREF               _hostExecutionContext;
    OBJECTREF               _syncContext;
    SECURITYCONTEXTREF      _securityContext;
    OBJECTREF               _logicalCallContext;
    OBJECTREF               _illogicalCallContext;
    OBJECTREF               _threadBaseObject;
    CLR_BOOL                _isNewCapture;
    CLR_BOOL                _isFlowSuppressed;
public:
    OBJECTREF GetSynchronizationContext()
    {
        LEAF_CONTRACT;
        return _syncContext;
    }   
    SECURITYCONTEXTREF GetSecurityContext()
    {
        LEAF_CONTRACT;
        return _securityContext;
    }
    OBJECTREF GetLogicalCallContext()
    {
        LEAF_CONTRACT;
        return _logicalCallContext;
    }
    void SetLogicalCallContext(OBJECTREF ref)
    { 
        WRAPPER_CONTRACT;
        SetObjectReferenceUnchecked((OBJECTREF*)&_logicalCallContext, ref);
    }
    OBJECTREF GetIllogicalCallContext()
    {
        LEAF_CONTRACT;
        return _illogicalCallContext;
    }
    void SetIllogicalCallContext(OBJECTREF ref)
    { 
        WRAPPER_CONTRACT;
        SetObjectReferenceUnchecked((OBJECTREF*)&_illogicalCallContext, ref);
    }
    COMPRESSEDSTACKREF GetCompressedStack()
    {
        WRAPPER_CONTRACT;
        if (_securityContext != NULL)
            return _securityContext->GetCompressedStack();
        return NULL;
    }
        
};


typedef DPTR(class CultureInfoBaseObject) PTR_CultureInfoBaseObject;

#ifdef USE_CHECKED_OBJECTREFS
typedef REF<SynchronizationContextObject> SYNCHRONIZATIONCONTEXTREF;
typedef REF<ExecutionContextObject> EXECUTIONCONTEXTREF;
typedef REF<CultureInfoBaseObject> CULTUREINFOBASEREF;
typedef REF<StringObject> STRINGREF;
typedef REF<ArrayBase> ARRAYBASEREF;

#else
typedef SynchronizationContextObject*     SYNCHRONIZATIONCONTEXTREF;
typedef ExecutionContextObject*     EXECUTIONCONTEXTREF;
typedef CultureInfoBaseObject*     CULTUREINFOBASEREF;
typedef PTR_StringObject STRINGREF;
typedef PTR_ArrayBase ARRAYBASEREF;
#endif

class CultureInfoBaseObject : public Object
{
    friend class Binder;

private:
    
    OBJECTREF compareInfo;
    OBJECTREF textInfo;
    OBJECTREF numInfo;
    OBJECTREF dateTimeInfo;
    OBJECTREF calendar;
    OBJECTREF m_cultureTableRecord;
    OBJECTREF m_consoleFallbackCulture;
    STRINGREF m_name;                       // "real" name - en-US, de-DE_phoneb or fj-FJ
    STRINGREF m_nonSortName;                // name w/o sort info (de-DE for de-DE_phoneb)
    STRINGREF m_sortName;                   // Sort only name (de-DE_phoneb, en-us for fj-fj (w/us sort)
    STRINGREF m_ietfName;
    CULTUREINFOBASEREF m_parent;
    INT32 cultureID;
    INT32 m_createdDomainID;
    INT32 m_dataItem;
    CLR_BOOL m_isReadOnly;
    CLR_BOOL m_isInherited;
    CLR_BOOL m_isSafeCrossDomain;
    CLR_BOOL m_useUserOverride;

public:

    INT32 GetCultureID() 
    {
        LEAF_CONTRACT;
        return cultureID;
    }// GetCultureID

    CULTUREINFOBASEREF GetParent()
    {
        LEAF_CONTRACT;
        return m_parent;
    }// GetParent


    STRINGREF GetName()
    {
        LEAF_CONTRACT;
        return m_name;
    }// GetName

    BOOL IsSafeCrossDomain()
    {
        return m_isSafeCrossDomain;
    }// IsSafeCrossDomain

    ADID GetCreatedDomainID()
    {
        return ADID(m_createdDomainID);
    }// GetCreatedDomain

}; // class CultureInfoBaseObject




typedef DPTR(class ThreadBaseObject) PTR_ThreadBaseObject;
class ThreadBaseObject : public Object
{
    friend class ClrDataAccess;
    friend class ThreadNative;
    friend class Binder;
    friend class Object;

private:

    // These field are also defined in the managed representation.  If you
    //  add or change these field you must also change the managed code so that
    //  it matches these.  This is necessary so that the object is the proper
    //  size.  The order here must match that order which the loader will choose
    //  when laying out the managed class.  Note that the layouts are checked
    //  at run time, not compile time.
    
    OBJECTREF     m_ExposedContext;
    EXECUTIONCONTEXTREF     m_ExecutionContext;
    OBJECTREF     m_Name;
    OBJECTREF     m_Delegate;
    PTRARRAYREF   m_ThreadStaticsBuckets;
    I4ARRAYREF    m_ThreadStaticsBits;
    CULTUREINFOBASEREF     m_CurrentUserCulture;
    CULTUREINFOBASEREF     m_CurrentUICulture;
    OBJECTREF     m_ThreadStartArg;

    // The next field (m_InternalThread) is declared as IntPtr in the managed
    // definition of Thread.  The loader will sort it next.

    // m_InternalThread is always valid -- unless the thread has finalized and been
    // resurrected.  (The thread stopped running before it was finalized).
    Thread       *m_InternalThread;
    INT32         m_Priority;    

protected:
    // the ctor and dtor can do no useful work.
    ThreadBaseObject() {LEAF_CONTRACT;};
   ~ThreadBaseObject() {LEAF_CONTRACT;};

public:
    Thread   *GetInternal()
    {
        LEAF_CONTRACT;
        return m_InternalThread;
    }

    void      SetInternal(Thread *it)
    {
        LEAF_CONTRACT;
        // either you are transitioning from NULL to non-NULL or vice versa.  But
        // you aren't setting NULL to NULL or non-NULL to non-NULL.
        _ASSERTE((m_InternalThread == NULL) != (it == NULL));
        m_InternalThread = it;
    }

    OBJECTREF GetThreadStartArg() { LEAF_CONTRACT; return m_ThreadStartArg; }
    void SetThreadStartArg(OBJECTREF newThreadStartArg) 
    {
        WRAPPER_CONTRACT;
    
        _ASSERTE(newThreadStartArg == NULL);
        // Note: this is an unchecked assignment.  We are cleaning out the ThreadStartArg field when 
        // a thread starts so that ADU does not cause problems
        SetObjectReferenceUnchecked( (OBJECTREF *)&m_ThreadStartArg, newThreadStartArg);
    
    }

    OBJECTREF GetDelegate()                   { LEAF_CONTRACT; return m_Delegate; }
    void      SetDelegate(OBJECTREF delegate);

    // These expose the remoting context (System\Remoting\Context)
    OBJECTREF GetExposedContext() { LEAF_CONTRACT; return m_ExposedContext; }
    OBJECTREF SetExposedContext(OBJECTREF newContext) 
    {
        WRAPPER_CONTRACT;

        OBJECTREF oldContext = m_ExposedContext;

        // Note: this is a very dangerous unchecked assignment.  We are taking
        // responsibilty here for cleaning out the ExposedContext field when 
        // an app domain is unloaded.
        SetObjectReferenceUnchecked( (OBJECTREF *)&m_ExposedContext, newContext );

        return oldContext;
    }

    CULTUREINFOBASEREF GetCurrentUserCulture()
    { 
        LEAF_CONTRACT; 
        return m_CurrentUserCulture;
    }

    void ResetCurrentUserCulture()
    { 
        WRAPPER_CONTRACT; 
        ClearObjectReference((OBJECTREF *)&m_CurrentUserCulture);
    }

    CULTUREINFOBASEREF GetCurrentUICulture() 
    { 
        LEAF_CONTRACT; 
        return m_CurrentUICulture; 
    }

    void ResetCurrentUICulture()
    { 
        WRAPPER_CONTRACT; 
        ClearObjectReference((OBJECTREF *)&m_CurrentUICulture);
    }

    OBJECTREF GetSynchronizationContext()
    {
        LEAF_CONTRACT; 
        if (m_ExecutionContext != NULL)
            return m_ExecutionContext->GetSynchronizationContext();
        return NULL;
    }

    OBJECTREF GetExecutionContext() 
    { 
        LEAF_CONTRACT; 
        return (OBJECTREF)m_ExecutionContext;
    }
    void SetExecutionContext(OBJECTREF ref)
    { 
        LEAF_CONTRACT;
        SetObjectReferenceUnchecked((OBJECTREF*)&m_ExecutionContext, ref);
    }
    
    COMPRESSEDSTACKREF GetCompressedStack()
    {
        WRAPPER_CONTRACT;
        if (m_ExecutionContext != NULL)
            return m_ExecutionContext->GetCompressedStack();
        return NULL;
    }
    // SetDelegate is our "constructor" for the pathway where the exposed object is
    // created first.  InitExisting is our "constructor" for the pathway where an
    // existing physical thread is later exposed.
    void      InitExisting();
    PTRARRAYREF GetThreadStaticsBucketsHolder() 
    { 
        LEAF_CONTRACT;
        // The code that needs this should have faulted it in by now!
        _ASSERTE(m_ThreadStaticsBuckets!= NULL); 
        return m_ThreadStaticsBuckets; 
    }
    
    I4ARRAYREF GetThreadStaticsBits() 
    { 
        LEAF_CONTRACT;
        // The code that needs this should have faulted it in by now!
        _ASSERTE(m_ThreadStaticsBits != NULL); 

        return m_ThreadStaticsBits; 
    }
    void ResetCulture()
    {
        LEAF_CONTRACT;
        m_CurrentUserCulture = NULL;
        m_CurrentUICulture = NULL;
    }

    void ResetName()
    {
        LEAF_CONTRACT;
        m_Name = NULL;
    }

    void ResetThreadStatics()
    {
        LEAF_CONTRACT;
        m_ThreadStaticsBuckets = NULL;
    }
    
    void SetPriority(INT32 priority)
    {
        LEAF_CONTRACT;
        m_Priority = priority;
    }
    
    INT32 GetPriority() const
    {
        LEAF_CONTRACT;
        return m_Priority;
    }
};

// MarshalByRefObjectBaseObject 
// This class is the base class for MarshalByRefObject
//  
class MarshalByRefObjectBaseObject : public Object
{
    friend class Binder;

  public:
    static int GetOffsetOfServerIdentity() { LEAF_CONTRACT; return offsetof(MarshalByRefObjectBaseObject, m_ServerIdentity); }

  protected:
    // READ ME:
    // Modifying the order or fields of this object may require other changes to the
    //  classlib class definition of this object.
    OBJECTREF     m_ServerIdentity;

  protected:
    MarshalByRefObjectBaseObject() {LEAF_CONTRACT;}
   ~MarshalByRefObjectBaseObject() {LEAF_CONTRACT;}   
};


// ContextBaseObject 
// This class is the base class for Contexts
//  
class ContextBaseObject : public Object
{
    friend class Context;
    friend class Binder;

  private:
    // READ ME:
    // Modifying the order or fields of this object may require other changes to the
    //  classlib class definition of this object.

    OBJECTREF m_ctxProps;   // array of name-value pairs of properties
    OBJECTREF m_dphCtx;     // dynamic property holder
    OBJECTREF m_localDataStore; // context local store
    OBJECTREF m_serverContextChain; // server context sink chain
    OBJECTREF m_clientContextChain; // client context sink chain
    OBJECTREF m_exposedAppDomain;       //appDomain ??
    PTRARRAYREF m_ctxStatics; // holder for context relative statics
    
    Context*  m_internalContext;            // Pointer to the VM context

    INT32 _ctxID;
    INT32 _ctxFlags;
    INT32 _numCtxProps;     // current count of properties

    INT32 _ctxStaticsCurrentBucket;
    INT32 _ctxStaticsFreeIndex;

  protected:
    ContextBaseObject() { LEAF_CONTRACT; }
   ~ContextBaseObject() { LEAF_CONTRACT; }
   
  public:

    void SetInternalContext(Context* pCtx) 
    {
        LEAF_CONTRACT;
        // either transitioning from NULL to non-NULL or vice versa.  
        // But not setting NULL to NULL or non-NULL to non-NULL.
        _ASSERTE((m_internalContext == NULL) != (pCtx == NULL));
        m_internalContext = pCtx;
    }
    
    Context* GetInternalContext() 
    {
        LEAF_CONTRACT;
        return m_internalContext;
    }

    OBJECTREF GetExposedDomain() { return m_exposedAppDomain; }
    OBJECTREF SetExposedDomain(OBJECTREF newDomain) 
    {
        LEAF_CONTRACT;
        OBJECTREF oldDomain = m_exposedAppDomain;
        SetObjectReference( (OBJECTREF *)&m_exposedAppDomain, newDomain, GetAppDomain() );
        return oldDomain;
    }

    PTRARRAYREF GetContextStaticsHolder() 
    { 
        LEAF_CONTRACT;
        // The code that needs this should have faulted it in by now!
        _ASSERTE(m_ctxStatics != NULL); 

        return m_ctxStatics; 
    }
};

// LocalDataStoreBaseObject 
// This class is the base class for local data stores
//  
class LocalDataStoreBaseObject : public Object
{
    friend class LocalDataStore;
    friend class Binder;
    // READ ME:
    // Modifying the order or fields of this object may require other changes to the
    //  classlib class definition of this object.
  protected:
    OBJECTREF m_DataTable;
    OBJECTREF m_Manager;
    LocalDataStore* m_pLDS;  // Pointer to the LocalDataStore Structure    

    LocalDataStoreBaseObject() { LEAF_CONTRACT; }
   ~LocalDataStoreBaseObject() { LEAF_CONTRACT; }

  public:

    void SetLocalDataStore(LocalDataStore* p) 
    {
        LEAF_CONTRACT;
        m_pLDS = p;
    }

    LocalDataStore* GetLocalDataStore() 
    {
        LEAF_CONTRACT;
        return m_pLDS;
    }
};


// AppDomainBaseObject 
// This class is the base class for application domains
//  
class AppDomainBaseObject : public MarshalByRefObjectBaseObject
{
    friend class AppDomain;
    friend class Binder;

  protected:
    // READ ME:
    // Modifying the order or fields of this object may require other changes to the
    //  classlib class definition of this object.
    OBJECTREF    m_pDomainManager;     // AppDomainManager for host settings.
    OBJECTREF    m_LocalStore;
    OBJECTREF    m_FusionTable;
    OBJECTREF    m_pSecurityIdentity;  // Evidence associated with this domain
    OBJECTREF    m_pPolicies;          // Array of context policies associated with this domain
    OBJECTREF    m_pAssemblyEventHandler; // Delegate for 'loading assembly' event
    OBJECTREF    m_pTypeEventHandler;     // Delegate for 'resolve type' event
    OBJECTREF    m_pResourceEventHandler; // Delegate for 'resolve resource' event
    OBJECTREF    m_pAsmResolveEventHandler; // Delegate for 'resolve assembly' event
    OBJECTREF    m_pReflectionAsmResolveEventHandler; //Delegate for 'reflection resolve assembly' event
    OBJECTREF    m_pDefaultContext;     // Default managed context for this AD.
    OBJECTREF    m_pDefaultPrincipal;  // Lazily computed default principle object used by threads
    OBJECTREF    m_pURITable;          // Identity table for remoting
    OBJECTREF    m_pProcessExitEventHandler; // Delegate for 'process exit' event.  Only used in Default appdomain.
    OBJECTREF    m_pDomainUnloadEventHandler; // Delegate for 'about to unload domain' event
    OBJECTREF    m_pUnhandledExceptionEventHandler; // Delegate for 'unhandled exception' event

    AppDomain*   m_pDomain;            // Pointer to the BaseDomain Structure
    INT32        m_iPrincipalPolicy;   // Type of principal to create by default
    CLR_BOOL     m_bHasSetPolicy;      // SetDomainPolicy has been called for this domain

  protected:
    AppDomainBaseObject() { LEAF_CONTRACT; }
   ~AppDomainBaseObject() { LEAF_CONTRACT; }
   
  public:

    void SetDomain(AppDomain* p) 
    {
        LEAF_CONTRACT;
        m_pDomain = p;
    }
    AppDomain* GetDomain() 
    {
        LEAF_CONTRACT;
        return m_pDomain;
    }

    OBJECTREF GetSecurityIdentity()
    {
        LEAF_CONTRACT;
        return m_pSecurityIdentity;
    }

    // Ref needs to be a PTRARRAYREF
    void SetPolicies(OBJECTREF ref)
    {
        WRAPPER_CONTRACT;
        SetObjectReference(&m_pPolicies, ref, m_pDomain );
    }

    void SetDefaultContext(OBJECTREF ref)
    {
        WRAPPER_CONTRACT;
        SetObjectReference(&m_pDefaultContext,ref,m_pDomain);
    }

    BOOL HasSetPolicy()
    {
        LEAF_CONTRACT;
        return m_bHasSetPolicy;
    }
};

// AssemblyBaseObject 
// This class is the base class for assemblies
//  
class AssemblyBaseObject : public Object
{
    friend class Assembly;
    friend class Binder;

  protected:
    // READ ME:
    // Modifying the order or fields of this object may require other changes to the
    //  classlib class definition of this object.
    OBJECTREF     m_pAssemblyBuilderData;
    OBJECTREF     m_pModuleEventHandler;   // Delegate for 'resolve module' event
    OBJECTREF     m_cache;                 // Slot for storing managed cached data.
    DomainAssembly* m_pAssembly;             // Pointer to the Assembly Structure

  protected:
    AssemblyBaseObject() { LEAF_CONTRACT; }
   ~AssemblyBaseObject() { LEAF_CONTRACT; }
   
  public:

    void SetAssembly(DomainAssembly* p) 
    {
        LEAF_CONTRACT;
        m_pAssembly = p;
    }

    DomainAssembly* GetDomainAssembly() 
    {
        LEAF_CONTRACT;
        return m_pAssembly;
    }

    Assembly* GetAssembly();
};
 

// AssemblyNameBaseObject 
// This class is the base class for assembly names
//  
class AssemblyNameBaseObject : public Object
{
    friend class AssemblyNative;
    friend class AppDomainNative;
    friend class Binder;

  protected:
    // READ ME:
    // Modifying the order or fields of this object may require other changes to the
    //  classlib class definition of this object.

    OBJECTREF     m_pSimpleName; 
    U1ARRAYREF    m_pPublicKey;
    U1ARRAYREF    m_pPublicKeyToken;
    OBJECTREF     m_pCultureInfo;
    OBJECTREF     m_pCodeBase;
    OBJECTREF     m_pVersion;
    OBJECTREF     m_StrongNameKeyPair;
    OBJECTREF     m_siInfo;
    U1ARRAYREF    m_HashForControl;
    DWORD         m_HashAlgorithm;
    DWORD         m_HashAlgorithmForControl;
    DWORD         m_VersionCompatibility;
    DWORD         m_Flags;

  protected:
    AssemblyNameBaseObject() { LEAF_CONTRACT; }
   ~AssemblyNameBaseObject() { LEAF_CONTRACT; }
   
  public:
    OBJECTREF GetSimpleName() { LEAF_CONTRACT; return m_pSimpleName; }
    U1ARRAYREF GetPublicKey() { LEAF_CONTRACT; return m_pPublicKey; }
    U1ARRAYREF GetPublicKeyToken() { LEAF_CONTRACT; return m_pPublicKeyToken; }
    OBJECTREF GetStrongNameKeyPair() { LEAF_CONTRACT; return m_StrongNameKeyPair; }
    OBJECTREF GetCultureInfo() { LEAF_CONTRACT; return m_pCultureInfo; }
    OBJECTREF GetAssemblyCodeBase() { LEAF_CONTRACT; return m_pCodeBase; }
    OBJECTREF GetVersion() { LEAF_CONTRACT; return m_pVersion; }
    DWORD GetAssemblyHashAlgorithm() { LEAF_CONTRACT; return m_HashAlgorithm; }
    DWORD GetFlags() { LEAF_CONTRACT; return m_Flags; }
    U1ARRAYREF GetHashForControl() { LEAF_CONTRACT; return m_HashForControl;}
    DWORD GetHashAlgorithmForControl() { LEAF_CONTRACT; return m_HashAlgorithmForControl; }
};

// VersionBaseObject
// This class is the base class for versions
//
class VersionBaseObject : public Object
{
    friend class Binder;

  protected:
    // READ ME:
    // Modifying the order or fields of this object may require other changes to the
    //  classlib class definition of this object.

    int m_Major;
    int m_Minor;
    int m_Build;
    int m_Revision;
 
    VersionBaseObject() {LEAF_CONTRACT;}
   ~VersionBaseObject() {LEAF_CONTRACT;}

  public:    
    int GetMajor() { LEAF_CONTRACT; return m_Major; }
    int GetMinor() { LEAF_CONTRACT; return m_Minor; }
    int GetBuild() { LEAF_CONTRACT; return m_Build; }
    int GetRevision() { LEAF_CONTRACT; return m_Revision; }
};

// FrameSecurityDescriptorBaseObject 
// This class is the base class for the frame security descriptor
//  

class FrameSecurityDescriptorBaseObject : public Object
{
    friend class Binder;

  protected:
    // READ ME:
    // Modifying the order or fields of this object may require other changes to the
    //  classlib class definition of this object.

    OBJECTREF       m_assertions;    // imperative
    OBJECTREF       m_denials;      // imperative
    OBJECTREF       m_restriction;  //  imperative
    OBJECTREF       m_DeclarativeAssertions;
    OBJECTREF       m_DeclarativeDenials;
    OBJECTREF       m_DeclarativeRestrictions;
    CLR_BOOL        m_assertFT;
    CLR_BOOL        m_assertAllPossible;
    CLR_BOOL        m_declSecComputed;
    


  protected:
    FrameSecurityDescriptorBaseObject() {LEAF_CONTRACT;}
   ~FrameSecurityDescriptorBaseObject() {LEAF_CONTRACT;}
   
  public:

    INT32 GetOverridesCount()
    {
        INT32 ret =0;
        if (m_restriction != NULL)
            ret++;
        if (m_denials != NULL)
            ret++;        
        if (m_DeclarativeDenials != NULL)
            ret++;
        if (m_DeclarativeRestrictions != NULL)
            ret++;
        return ret;
    }

    INT32 GetAssertCount()
    {
        INT32 ret =0;
        if (m_assertions != NULL || m_DeclarativeAssertions != NULL || HasAssertAllPossible())
            ret++;
        return ret;
    }  

    BOOL HasAssertFT()
    {
        LEAF_CONTRACT;
        return m_assertFT;
    }

    BOOL IsDeclSecComputed()
    {
        LEAF_CONTRACT;
        return m_declSecComputed;
    }

    BOOL HasAssertAllPossible()
    {
        LEAF_CONTRACT;
        return m_assertAllPossible;
    }

    OBJECTREF GetImperativeAssertions()
    {
        LEAF_CONTRACT;
        return m_assertions;
    }
   OBJECTREF GetDeclarativeAssertions()
    {
        LEAF_CONTRACT;
        return m_DeclarativeAssertions;
    }
    OBJECTREF GetImperativeDenials()
    {
        LEAF_CONTRACT;
        return m_denials;
    }
    OBJECTREF GetDeclarativeDenials()
    {
        LEAF_CONTRACT;
        return m_DeclarativeDenials;
    }
    OBJECTREF GetImperativeRestrictions()
    {
        LEAF_CONTRACT;
        return m_restriction;
    }
   OBJECTREF GetDeclarativeRestrictions()
    {
        LEAF_CONTRACT;
        return m_DeclarativeRestrictions;
    }
    void SetImperativeAssertions(OBJECTREF assertRef)
    {
        LEAF_CONTRACT;
        SetObjectReference(&m_assertions, assertRef, this->GetAppDomain());
    }
    void SetDeclarativeAssertions(OBJECTREF assertRef)
    {
        LEAF_CONTRACT;
        SetObjectReference(&m_DeclarativeAssertions, assertRef, this->GetAppDomain());
    }
    void SetImperativeDenials(OBJECTREF denialRef)
    {
        LEAF_CONTRACT;
        SetObjectReference(&m_denials, denialRef, this->GetAppDomain()); 
    }

    void SetDeclarativeDenials(OBJECTREF denialRef)
    {
        LEAF_CONTRACT;
        SetObjectReference(&m_DeclarativeDenials, denialRef, this->GetAppDomain()); 
    }

    void SetImperativeRestrictions(OBJECTREF restrictRef)
    {
        LEAF_CONTRACT;
        SetObjectReference(&m_restriction, restrictRef, this->GetAppDomain());
    }

    void SetDeclarativeRestrictions(OBJECTREF restrictRef)
    {
        LEAF_CONTRACT;
        SetObjectReference(&m_DeclarativeRestrictions, restrictRef, this->GetAppDomain());
    }
    void SetAssertAllPossible(BOOL assertAllPossible)
    {
        LEAF_CONTRACT;
        m_assertAllPossible = !!assertAllPossible;
    }
    
    void SetAssertFT(BOOL assertFT)
    {
        LEAF_CONTRACT;
        m_assertFT = !!assertFT;
    }
    void SetDeclSecComputed(BOOL declSec)
    {
        LEAF_CONTRACT;
        m_declSecComputed = !!declSec;
    }
};

class WeakReferenceObject : public Object
{
public:
    HANDLE      m_Handle;
    CLR_BOOL    m_IsLongReference;
};


#ifdef USE_CHECKED_OBJECTREFS

typedef REF<ReflectModuleBaseObject> REFLECTMODULEBASEREF;

typedef REF<ReflectClassBaseObject> REFLECTCLASSBASEREF;

typedef REF<ThreadBaseObject> THREADBASEREF;

typedef REF<LocalDataStoreBaseObject> LOCALDATASTOREREF;

typedef REF<AppDomainBaseObject> APPDOMAINREF;

typedef REF<MarshalByRefObjectBaseObject> MARSHALBYREFOBJECTBASEREF;

typedef REF<ContextBaseObject> CONTEXTBASEREF;

typedef REF<AssemblyBaseObject> ASSEMBLYREF;

typedef REF<AssemblyNameBaseObject> ASSEMBLYNAMEREF;

typedef REF<VersionBaseObject> VERSIONREF;

typedef REF<FrameSecurityDescriptorBaseObject> FRAMESECDESCREF;

inline ARG_SLOT ObjToArgSlot(OBJECTREF objRef)
{
    LEAF_CONTRACT;
    LPVOID v;
    v = OBJECTREFToObject(objRef);
    return (ARG_SLOT)(SIZE_T)v;
}

inline OBJECTREF ArgSlotToObj(ARG_SLOT i)
{
    LEAF_CONTRACT;
    LPVOID v;
    v = (LPVOID)(SIZE_T)i;
    return ObjectToOBJECTREF ((Object*)v);
}

inline ARG_SLOT StringToArgSlot(STRINGREF sr)
{
    LEAF_CONTRACT;
    LPVOID v;
    v = OBJECTREFToObject(sr);
    return (ARG_SLOT)(SIZE_T)v;
}

inline STRINGREF ArgSlotToString(ARG_SLOT s)
{
    LEAF_CONTRACT;
    LPVOID v;
    v = (LPVOID)(SIZE_T)s;
    return ObjectToSTRINGREF ((StringObject*)v);
}

#else // _DEBUG

typedef PTR_ReflectModuleBaseObject REFLECTMODULEBASEREF;
typedef PTR_ReflectClassBaseObject REFLECTCLASSBASEREF;
typedef PTR_ThreadBaseObject THREADBASEREF;
typedef PTR_AppDomainBaseObject APPDOMAINREF;
typedef PTR_AssemblyBaseObject ASSEMBLYREF;
typedef PTR_AssemblyNameBaseObject ASSEMBLYNAMEREF;

#ifndef DACCESS_COMPILE
typedef LocalDataStoreBaseObject* LOCALDATASTOREREF;
typedef MarshalByRefObjectBaseObject* MARSHALBYREFOBJECTBASEREF;
typedef ContextBaseObject* CONTEXTBASEREF;
typedef VersionBaseObject* VERSIONREF;
typedef FrameSecurityDescriptorBaseObject* FRAMESECDESCREF;
#endif // #ifndef DACCESS_COMPILE

#define ObjToArgSlot(objref) ((ARG_SLOT)(SIZE_T)(objref))
#define ArgSlotToObj(s) ((OBJECTREF)(SIZE_T)(s))

#define StringToArgSlot(objref) ((ARG_SLOT)(SIZE_T)(objref))
#define ArgSlotToString(s)    ((STRINGREF)(SIZE_T)(s))

#endif //_DEBUG

#define PtrToArgSlot(ptr) ((ARG_SLOT)(SIZE_T)(ptr))
#define ArgSlotToPtr(s)   ((LPVOID)(SIZE_T)(s))

STRINGREF AllocateString(SString sstr);

/*************************
// DON'T CHANGE THE LAYOUT OF THE FOLLOWING CLASSES
// WITHOUT UPDATING THE MANAGED CLASSES IN BCL ALSO
**************************/

class RealProxyObject : public Object
{
    friend class Binder;

public:
    DWORD GetOptFlags() { return _optFlags; }
    DWORD GetDomainID() { return _domainID; }
    VOID SetOptFlags (DWORD flags) { _optFlags = flags; }

    static int GetOffsetOfTP() { LEAF_CONTRACT; return offsetof(RealProxyObject, _tp); }
    static int GetOffsetOfIdentity() { LEAF_CONTRACT; return offsetof(RealProxyObject, _identity); }
    static int GetOffsetOfServerObject() { LEAF_CONTRACT; return offsetof(RealProxyObject, _serverObject); }
    static int GetOffsetOfServerIdentity() { LEAF_CONTRACT; return offsetof(RealProxyObject, _srvIdentity); }
    
protected:
    RealProxyObject()
    {LEAF_CONTRACT;}; // don't instantiate this class directly
    ~RealProxyObject(){LEAF_CONTRACT;};

private:
    OBJECTREF       _tp;
    OBJECTREF       _identity;
    OBJECTREF       _serverObject;
    DWORD           _flags;
    DWORD           _optFlags;
    DWORD           _domainID;
    OBJECTHANDLE    _srvIdentity;
};

#ifdef USE_CHECKED_OBJECTREFS
typedef REF<RealProxyObject> REALPROXYREF;
#else
typedef RealProxyObject*     REALPROXYREF;
#endif


class TransparentProxyObject : public Object
{
    friend class Binder;

public:
    static int GetOffsetOfRP() { LEAF_CONTRACT; return offsetof(TransparentProxyObject, _rp); }
    static int GetOffsetOfMT() { LEAF_CONTRACT; return offsetof(TransparentProxyObject, _pMT); }
    static int GetOffsetOfInterfaceMT() { LEAF_CONTRACT; return offsetof(TransparentProxyObject, _pInterfaceMT); }
    static int GetOffsetOfStub() { LEAF_CONTRACT; return offsetof(TransparentProxyObject, _stub); }
    static int GetOffsetOfStubData() { LEAF_CONTRACT; return offsetof(TransparentProxyObject, _stubData); }
    
protected:
    TransparentProxyObject()
    {LEAF_CONTRACT;}; // don't instantiate this class directly
    ~TransparentProxyObject(){LEAF_CONTRACT;};

private:
    OBJECTREF       _rp;
    OBJECTREF       _stubData;
    MethodTable*    _pMT;
    MethodTable*    _pInterfaceMT;
    void*           _stub;
};

#ifdef USE_CHECKED_OBJECTREFS
typedef REF<TransparentProxyObject> TRANSPARENTPROXYREF;
#else
typedef TransparentProxyObject*     TRANSPARENTPROXYREF;
#endif



//
// StringBufferObject
//
// Note that the "copy on write" bit is buried within the implementation
// of the object in order to make the implementation smaller.
//


class StringBufferObject : public Object
{
    friend class Binder;

  private:
    // READ ME:
    //   Modifying the order or fields of this object may require
    //   other changes to the classlib class definition of this
    //   object or special handling when loading this system class.
    //   The GCDesc stuff must be built correctly to promote the m_orString
    //   reference during garbage collection.                                         
    STRINGREF m_orString;
    void*       m_currentThread; 
    INT32       m_MaxCapacity;

  protected:
   StringBufferObject() { LEAF_CONTRACT; };
   ~StringBufferObject() { LEAF_CONTRACT; };

  public:
    STRINGREF   GetStringRef() { 
        LEAF_CONTRACT;
        // On checked build, we might take an address on a temporary. 
        // MingW C++ compiler complains about following code.
        return m_orString;
    };    

#ifndef DACCESS_COMPILE
    VOID        SetStringRef(STRINGREF orString)        { WRAPPER_CONTRACT; SetObjectReference( (OBJECTREF *) &m_orString, ObjectToOBJECTREF(*(Object**) &orString), GetAppDomain()); };
#endif
    
  void* GetCurrentThread()
  {
     LEAF_CONTRACT;
     return *((void * volatile *)&m_currentThread);
  }

  VOID SetCurrentThread(void* value)
  {
      LEAF_CONTRACT;
      *((void * volatile *)&m_currentThread) = value;
  }

  DWORD GetArrayLength() 
  {
      WRAPPER_CONTRACT;
      return m_orString->GetArrayLength();
  };
  INT32 GetMaxCapacity() 
  {
      LEAF_CONTRACT;
      return m_MaxCapacity;
  }
  VOID SetMaxCapacity(INT32 max) 
  {
      LEAF_CONTRACT;
      m_MaxCapacity=max;
  }
};

#ifdef USE_CHECKED_OBJECTREFS
typedef REF<StringBufferObject> STRINGBUFFERREF;
#else   // USE_CHECKED_OBJECTREFS
typedef StringBufferObject * STRINGBUFFERREF;
#endif  // USE_CHECKED_OBJECTREFS

class SafeHandle : public Object
{
    friend class Binder;

  private:
    // READ ME:
    //   Modifying the order or fields of this object may require
    //   other changes to the classlib class definition of this
    //   object or special handling when loading this system class.
#ifdef _DEBUG
    STRINGREF m_debugStackTrace;   // Where we allocated this SafeHandle
#endif
    volatile LPVOID m_handle;
    volatile INT32 m_state;        // Combined ref count and closed/disposed state (for atomicity)
    volatile CLR_BOOL m_ownsHandle;
    volatile CLR_BOOL m_fullyInitialized;  // Did constructor finish?

    // Describe the bits in the m_state field above.
    enum StateBits
    {
        SH_State_Closed     = 0x00000001,
        SH_State_Disposed   = 0x00000002,
        SH_State_RefCount   = 0xfffffffc,
        SH_RefCountOne      = 4,            // Amount to increment state field to yield a ref count increment of 1
    };

    static MethodDesc *s_pIsInvalidHandleMethod;
    static MethodDesc *s_pReleaseHandleMethod;

    static void RunReleaseMethod(SafeHandle* psh);
    BOOL IsFullyInitialized() const { LEAF_CONTRACT; return m_fullyInitialized; }

  public:
    static void Init();

    // To use the SafeHandle from native, look at the SafeHandleHolder, which
    // will do the AddRef & Release for you.
    LPVOID GetHandle() const { 
        LEAF_CONTRACT;
        _ASSERTE(((unsigned int) m_state) >= SH_RefCountOne);
        return m_handle;
    }

    BOOL OwnsHandle() const
    {
        LEAF_CONTRACT;
        return m_ownsHandle;
    }

    static size_t GetHandleOffset() { LEAF_CONTRACT; return offsetof(SafeHandle, m_handle); }

    void AddRef();
    void Release(bool fDispose = false);
    void Dispose();
    void SetHandle(LPVOID handle);

    static FCDECL1(void, DisposeNative, SafeHandle* refThisUNSAFE);
    static FCDECL1(void, Finalize, SafeHandle* refThisUNSAFE);
    static FCDECL1(void, SetHandleAsInvalid, SafeHandle* refThisUNSAFE);
    static FCDECL2(void, DangerousAddRef, SafeHandle* refThisUNSAFE, CLR_BOOL *pfSuccess);
    static FCDECL1(void, DangerousRelease, SafeHandle* refThisUNSAFE);
};

// SAFEHANDLEREF defined above because CompressedStackObject needs it

void AcquireSafeHandle(SAFEHANDLEREF* s);
void ReleaseSafeHandle(SAFEHANDLEREF* s);

#if defined (_MSC_VER) && _MSC_VER <= 1300
template void DoNothing(SAFEHANDLEREF*);
template BOOL CompareDefault(SAFEHANDLEREF*,SAFEHANDLEREF*);
#endif
typedef Holder<SAFEHANDLEREF*, AcquireSafeHandle, ReleaseSafeHandle> SafeHandleHolder;

class CriticalHandle : public Object
{
    friend class Binder;

  private:
    // READ ME:
    //   Modifying the order or fields of this object may require
    //   other changes to the classlib class definition of this
    //   object or special handling when loading this system class.
#ifdef _DEBUG
    STRINGREF m_debugStackTrace;   // Where we allocated this CriticalHandle
#endif
    volatile LPVOID m_handle;
    volatile CLR_BOOL m_isClosed;

  public:
    LPVOID GetHandle() const { LEAF_CONTRACT; return m_handle; }
    static size_t GetHandleOffset() { LEAF_CONTRACT; return offsetof(CriticalHandle, m_handle); }

    void SetHandle(LPVOID handle) { LEAF_CONTRACT; m_handle = handle; }

    static FCDECL1(void, FireCustomerDebugProbe, CriticalHandle* refThisUNSAFE);
};

#ifdef USE_CHECKED_OBJECTREFS
typedef REF<CriticalHandle> CRITICALHANDLE;
typedef REF<CriticalHandle> CRITICALHANDLEREF;
#else // USE_CHECKED_OBJECTREFS
typedef CriticalHandle * CRITICALHANDLE;
typedef CriticalHandle * CRITICALHANDLEREF;
#endif // USE_CHECKED_OBJECTREFS

// WaitHandleBase
// Base class for WaitHandle 
class WaitHandleBase :public MarshalByRefObjectBaseObject
{
    friend class WaitHandleNative;
    friend class Binder;

public:
    __inline LPVOID GetWaitHandle() {LEAF_CONTRACT; return m_handle;}
    __inline SAFEHANDLEREF GetSafeHandle() {LEAF_CONTRACT; return m_safeHandle;}

private:
    SAFEHANDLEREF   m_safeHandle;
    LPVOID          m_handle;
    CLR_BOOL        m_hasThreadAffinity;
};

#ifdef USE_CHECKED_OBJECTREFS
typedef REF<WaitHandleBase> WAITHANDLEREF;
#else // USE_CHECKED_OBJECTREFS
typedef WaitHandleBase* WAITHANDLEREF;
#endif // USE_CHECKED_OBJECTREFS

// This class corresponds to FileStreamAsyncResult on the managed side.
class AsyncResultBase :public Object
{
    friend class Binder;

public: 
    WAITHANDLEREF GetWaitHandle() { LEAF_CONTRACT; return _waitHandle;}
    void SetErrorCode(int errcode) { LEAF_CONTRACT; _errorCode = errcode;}
    void SetNumBytes(int numBytes) { LEAF_CONTRACT; _numBytes = numBytes;}
    void SetIsComplete() { LEAF_CONTRACT; _isComplete = TRUE; }
    void SetCompletedAsynchronously() { LEAF_CONTRACT; _completedSynchronously = FALSE; }

    // README:
    // If you modify the order of these fields, make sure to update the definition in 
    // BCL for this object.
private:
    OBJECTREF _userCallback;
    OBJECTREF _userStateObject;

    WAITHANDLEREF _waitHandle;
    SAFEHANDLEREF _fileHandle;     // For cancellation.
    int _EndXxxCalled;             // Whether we've called EndXxx already.
    int _numBytes;                 // number of bytes read OR written
    int _errorCode;
    int _numBufferedBytes;

    CLR_BOOL _isWrite;                 // Whether this is a read or a write
    CLR_BOOL _isComplete;
    CLR_BOOL _completedSynchronously;  // Which thread called callback
};

#ifdef USE_CHECKED_OBJECTREFS
typedef REF<AsyncResultBase> ASYNCRESULTREF;
#else // USE_CHECKED_OBJECTREFS
typedef AsyncResultBase* ASYNCRESULTREF;
#endif // USE_CHECKED_OBJECTREFS


// This class corresponds to DelegateObject on the managed side.
class DelegateObject : public Object
{
    friend class CheckAsmOffsets;
    friend class Binder;

public:
    OBJECTREF GetTarget() { LEAF_CONTRACT; return _target; }
    void SetTarget(OBJECTREF target) { WRAPPER_CONTRACT; SetObjectReference(&_target, target, GetAppDomain()); }
    static int GetOffsetOfTarget() { LEAF_CONTRACT; return offsetof(DelegateObject, _target); }

    void* GetMethodPtr() { LEAF_CONTRACT; return _methodPtr; }
    void SetMethodPtr(void* methodPtr) { LEAF_CONTRACT; _methodPtr = methodPtr; }
    static int GetOffsetOfMethodPtr() { LEAF_CONTRACT; return offsetof(DelegateObject, _methodPtr); }

    void* GetMethodPtrAux() { LEAF_CONTRACT; return _methodPtrAux; }
    void SetMethodPtrAux(void* methodPtrAux) { LEAF_CONTRACT; _methodPtrAux = methodPtrAux; }
    static int GetOffsetOfMethodPtrAux() { LEAF_CONTRACT; return offsetof(DelegateObject, _methodPtrAux); }

    OBJECTREF GetInvocationList() { LEAF_CONTRACT; return _invocationList; }
    void SetInvocationList(OBJECTREF invocationList) { WRAPPER_CONTRACT; SetObjectReference(&_invocationList, invocationList, GetAppDomain()); }
    static int GetOffsetOfInvocationList() { LEAF_CONTRACT; return offsetof(DelegateObject, _invocationList); }

    INT_PTR GetInvocationCount() { LEAF_CONTRACT; return _invocationCount; }
    void SetInvocationCount(INT_PTR invocationCount) { LEAF_CONTRACT; _invocationCount = invocationCount; }
    static int GetOffsetOfInvocationCount() { LEAF_CONTRACT; return offsetof(DelegateObject, _invocationCount); }

    // README:
    // If you modify the order of these fields, make sure to update the definition in 
    // BCL for this object.
private:
    // System.Delegate
    OBJECTREF   _target;
    OBJECTREF   _methodBase;
    void*       _methodPtr;
    void*       _methodPtrAux;

    // System.MulticastDelegate
    OBJECTREF   _invocationList;
    INT_PTR     _invocationCount;
};

#ifdef USE_CHECKED_OBJECTREFS
typedef REF<DelegateObject> DELEGATEREF;
#else // USE_CHECKED_OBJECTREFS
typedef DelegateObject* DELEGATEREF;
#endif // USE_CHECKED_OBJECTREFS

// This class corresponds to PermissionSet on the managed side.
class PermissionSetObject : public Object
{
    friend class Binder;

public:
    BOOL CanUnrestrictedOverride()
    {
        LEAF_CONTRACT;
        return _canUnrestrictedOverride == TRUE;
    }

    BOOL AllPermissionsDecoded()
    {
        LEAF_CONTRACT;
        return _allPermissionsDecoded == TRUE;
    }

    BOOL ContainsCas()
    {
        LEAF_CONTRACT;
        return _ContainsCas == TRUE;
    }

    BOOL ContainsNonCas()
    {
        LEAF_CONTRACT;
        return _ContainsNonCas == TRUE;
    }

    BOOL CheckedForNonCas()
    {
        LEAF_CONTRACT;
        return _CheckedForNonCas == TRUE;
    }

    BOOL IsUnrestricted()
    {
        LEAF_CONTRACT;
        return _Unrestricted == TRUE;
    }

    OBJECTREF GetTokenBasedSet()
    {
        LEAF_CONTRACT;
        return _permSet;
    }


    // README:
    // If you modify the order of these fields, make sure to update the definition in 
    // BCL for this object.
private:
    // Order of the fields is important as it mirrors the layout of PermissionSet
    // to access the fields directly from unmanaged code given an OBJECTREF. 
    // Please keep them in sync when you make changes to the fields. 
    OBJECTREF _permSet;
    STRINGREF _serializedPermissionSet;
    OBJECTREF _permSetSaved;
    OBJECTREF _unrestrictedPermSet;
    OBJECTREF _normalPermSet;
    CLR_BOOL _Unrestricted;
    CLR_BOOL _allPermissionsDecoded;
    CLR_BOOL _canUnrestrictedOverride;
    CLR_BOOL _ignoreTypeLoadFailures;
    CLR_BOOL _CheckedForNonCas;
    CLR_BOOL _ContainsCas;
    CLR_BOOL _ContainsNonCas;
    CLR_BOOL _Readable;
};

#ifdef USE_CHECKED_OBJECTREFS
typedef REF<PermissionSetObject> PERMISSIONSETREF;
#else // USE_CHECKED_OBJECTREFS
typedef PermissionSetObject* PERMISSIONSETREF;
#endif // USE_CHECKED_OBJECTREFS

// This class corresponds to TokenBasedSet on the managed side.
class TokenBasedSetObject : public Object
{
public:
    INT32 GetNumElements () {
        LEAF_CONTRACT;
        return _cElt;
    }

    OBJECTREF GetPermSet () {
        LEAF_CONTRACT;
        return _Obj;
    }

private:
    // If you modify the order of these fields, make sure
    // to update the definition in BCL for this object.
    OBJECTREF _objSet;
    OBJECTREF _Obj;
    OBJECTREF _Set;
    INT32 _initSize;
    INT32 _increment;
    INT32 _cElt;
    INT32 _maxIndex;
};

#ifdef USE_CHECKED_OBJECTREFS
typedef REF<TokenBasedSetObject> TOKENBASEDSETREF;
#else // USE_CHECKED_OBJECTREFS
typedef TokenBasedSetObject* TOKENBASEDSETREF;
#endif // USE_CHECKED_OBJECTREFS

// This class corresponds to SecurityPermission on the managed side.
class SecurityPermissionObject : public Object
{
public:
    DWORD GetFlags () {
        LEAF_CONTRACT;
        return _flags;
    }

private:
    // If you modify the order of these fields, make sure
    // to update the definition in BCL for this object.
    DWORD _flags;
};

#ifdef USE_CHECKED_OBJECTREFS
typedef REF<SecurityPermissionObject> SECURITYPERMISSIONREF;
#else // USE_CHECKED_OBJECTREFS
typedef SecurityPermissionObject* SECURITYPERMISSIONREF;
#endif // USE_CHECKED_OBJECTREFS

// This class corresponds to ReflectionPermission on the managed side.
class ReflectionPermissionObject : public Object
{
public:
    DWORD GetFlags () {
        LEAF_CONTRACT;
        return _flags;
    }

private:
    DWORD _flags;
};

#ifdef USE_CHECKED_OBJECTREFS
typedef REF<ReflectionPermissionObject> REFLECTIONPERMISSIONREF;
#else // USE_CHECKED_OBJECTREFS
typedef ReflectionPermissionObject* REFLECTIONPERMISSIONREF;
#endif // USE_CHECKED_OBJECTREFS

struct StackTraceElement;
class ClrDataAccess;

class StackTraceArray
{
    friend class ClrDataAccess;
    
    struct ArrayHeader
    {
        size_t m_size;
        Thread * m_thread;
    };
    
public:
    StackTraceArray()
        : m_array(static_cast<I1Array *>(NULL))
    {
        WRAPPER_CONTRACT;
    }
    
    StackTraceArray(I1ARRAYREF array)
        : m_array(array)
    {
        LEAF_CONTRACT;
    }

    void Swap(StackTraceArray & rhs)
    {
        I1ARRAYREF t = m_array;
        m_array = rhs.m_array;
        rhs.m_array = t;
    }
    
    size_t Size() const
    {
        WRAPPER_CONTRACT;
        if (!m_array)
            return 0;
        else
            return GetSize();
    }
    
    StackTraceElement const & operator[](size_t index) const;
    StackTraceElement & operator[](size_t index);

    void Append(StackTraceElement const * begin, StackTraceElement const * end);
    void AppendSkipLast(StackTraceElement const * begin, StackTraceElement const * end);

    I1ARRAYREF Get() const
    {
        LEAF_CONTRACT;
        return m_array;
    }
    
private:
    StackTraceArray(StackTraceArray const & rhs);

    StackTraceArray & operator=(StackTraceArray const & rhs)
    {
        WRAPPER_CONTRACT;
        StackTraceArray copy(rhs);
        this->Swap(copy);
        return *this;
    }
    
    void Grow(size_t size);
    void EnsureThreadAffinity();
    void CheckState() const;

    size_t Capacity() const
    {
        WRAPPER_CONTRACT;
        assert(!!m_array);

        return m_array->GetNumComponents();
    }
    
    size_t GetSize() const
    {
        WRAPPER_CONTRACT;
        return GetHeader()->m_size;
    }

    void SetSize(size_t size)
    {
        WRAPPER_CONTRACT;
        GetHeader()->m_size = size;
    }

    Thread * GetObjectThread() const
    {
        WRAPPER_CONTRACT;
        return GetHeader()->m_thread;
    }

    void SetObjectThread()
    {
        WRAPPER_CONTRACT;
        GetHeader()->m_thread = ::GetThread();
    }

    StackTraceElement const * GetData() const
    {
        WRAPPER_CONTRACT;
        return reinterpret_cast<StackTraceElement const *>(GetRaw() + sizeof(ArrayHeader));
    }

    StackTraceElement * GetData()
    {
        WRAPPER_CONTRACT;
        return reinterpret_cast<StackTraceElement *>(GetRaw() + sizeof(ArrayHeader));
    }

    I1 const * GetRaw() const
    {
        WRAPPER_CONTRACT;
        assert(!!m_array);

        return const_cast<I1ARRAYREF &>(m_array)->GetDirectPointerToNonObjectElements();
    }

    I1 * GetRaw()
    {
        WRAPPER_CONTRACT;
        assert(!!m_array);

        return m_array->GetDirectPointerToNonObjectElements();
    }

    ArrayHeader const * GetHeader() const
    {
        WRAPPER_CONTRACT;
        return reinterpret_cast<ArrayHeader const *>(GetRaw());
    }

    ArrayHeader * GetHeader()
    {
        WRAPPER_CONTRACT;
        return reinterpret_cast<ArrayHeader *>(GetRaw());
    }

    void SetArray(I1ARRAYREF const & arr)
    {
        LEAF_CONTRACT;
        m_array = arr;
    }

private:
    // put only things here that can be protected with GCPROTECT
    I1ARRAYREF m_array;
};


// This class corresponds to Exception on the managed side.
typedef DPTR(class ExceptionObject) PTR_ExceptionObject;
class ExceptionObject : public Object
{
    friend class Binder;

public:
    void SetHResult(HRESULT hr)
    {
        LEAF_CONTRACT;
        _HResult = hr;
    }

    HRESULT GetHResult()
    {
        LEAF_CONTRACT;
        return _HResult;
    }

    void SetXCode(DWORD code)
    {
        LEAF_CONTRACT;
        _xcode = code;
    }

    DWORD GetXCode()
    {
        LEAF_CONTRACT;
        return _xcode;
    }

    void SetXPtrs(void* xptrs)
    {
        LEAF_CONTRACT;
        _xptrs = xptrs;
    }

    void* GetXPtrs()
    {
        LEAF_CONTRACT;
        return _xptrs;
    }

    void SetStackTrace(StackTraceArray const & stackTrace)
    {        
        WRAPPER_CONTRACT; 
        SetObjectReference((OBJECTREF*)&_stackTrace, (OBJECTREF)stackTrace.Get(), GetAppDomain());
    }

    void GetStackTrace(StackTraceArray & stackTrace) const
    {
        LEAF_CONTRACT;
        StackTraceArray temp(_stackTrace);
        stackTrace.Swap(temp);
    }

    void SetDynamicMethods(PTRARRAYREF dynamicMethods)
    {        
        WRAPPER_CONTRACT; 
        SetObjectReference((OBJECTREF*)&_dynamicMethods, (OBJECTREF)dynamicMethods, GetAppDomain());
    }

    PTRARRAYREF GetDynamicMethods()
    {
        LEAF_CONTRACT;
        return _dynamicMethods;
    }

    void SetInnerException(OBJECTREF innerException)
    {
        WRAPPER_CONTRACT;
        SetObjectReference((OBJECTREF*)&_innerException, (OBJECTREF)innerException, GetAppDomain());
    }

    OBJECTREF GetInnerException()
    {
        LEAF_CONTRACT;
        return _innerException;
    }

    void SetMessage(STRINGREF message)
    {
        WRAPPER_CONTRACT;
        SetObjectReference((OBJECTREF*)&_message, (OBJECTREF)message, GetAppDomain());
    }

    STRINGREF GetMessage()
    {
        LEAF_CONTRACT;
        return _message;
    }

    void SetStackTraceString(STRINGREF stackTraceString)
    {
        WRAPPER_CONTRACT;
        SetObjectReference((OBJECTREF*)&_stackTraceString, (OBJECTREF)stackTraceString, GetAppDomain());
    }

    STRINGREF GetStackTraceString()
    {
        LEAF_CONTRACT;
        return _stackTraceString;
    }

    STRINGREF GetRemoteStackTraceString()
    {
        LEAF_CONTRACT;
        return _remoteStackTraceString;
    }

    void SetHelpURL(STRINGREF helpURL)
    {
        WRAPPER_CONTRACT;
        SetObjectReference((OBJECTREF*)&_helpURL, (OBJECTREF)helpURL, GetAppDomain());
    }

    void SetSource(STRINGREF source)
    {
        WRAPPER_CONTRACT;
        SetObjectReference((OBJECTREF*)&_source, (OBJECTREF)source, GetAppDomain());
    }

    void ClearStackTraceForThrow()
    {
        WRAPPER_CONTRACT;
        SetObjectReferenceUnchecked((OBJECTREF*)&_remoteStackTraceString, NULL);
        SetObjectReferenceUnchecked((OBJECTREF*)&_stackTrace, NULL);
        SetObjectReferenceUnchecked((OBJECTREF*)&_stackTraceString, NULL);
    }

    // README:
    // If you modify the order of these fields, make sure to update the definition in 
    // BCL for this object.
private:
    STRINGREF   _className;  //Needed for serialization.
    OBJECTREF   _exceptionMethod;  //Needed for serialization.
    STRINGREF   _exceptionMethodString; //Needed for serialization.
    STRINGREF   _message;
    OBJECTREF   _data;
    OBJECTREF   _innerException;
    STRINGREF   _helpURL;
    I1ARRAYREF  _stackTrace;
    STRINGREF   _stackTraceString; //Needed for serialization.
    STRINGREF   _remoteStackTraceString;
    PTRARRAYREF _dynamicMethods;
    STRINGREF   _source;         // Mainly used by VB.
    IN_WIN64(void* _xptrs;)
    INT32       _remoteStackIndex;
    INT32       _HResult;
    IN_WIN32(void* _xptrs;)
    INT32       _xcode;
};

//===============================================================================
// Nullable represents the managed generic value type Nullable<T> 
//
// The runtime has special logic for this value class.  When it is boxed
// it becomes either null or a boxed T.   Similarly a boxed T can be unboxed
// either as a T (as normal), or as a Nullable<T>.

class Nullable {
    Nullable();   // This is purposefully undefined.  Do not make instances
                  // of this class.  
public:
    static void CheckFieldOffsets(TypeHandle nullableType);
    static BOOL IsNullableType(TypeHandle nullableType);
    static BOOL IsNullableForType(TypeHandle nullableType, MethodTable* paramMT);

    static OBJECTREF Box(void* src, MethodTable* nullable);
    static BOOL UnBox(void* dest, OBJECTREF boxedVal, MethodTable* destMT);
    static OBJECTREF BoxedNullableNull(TypeHandle nullableType) { return 0; }

        // if 'Obj' is a true boxed nullable, return the form we want (either null or a boxed T)
    static OBJECTREF NormalizeBox(OBJECTREF obj);

private:
    static BOOL IsNullableForTypeHelper(MethodTable* nullableMT, MethodTable* paramMT);

    bool* Nullable::HasValueAddr(MethodTable* nullableMT);
    void* Nullable::ValueAddr(MethodTable* nullableMT);
};

#ifdef USE_CHECKED_OBJECTREFS
typedef REF<ExceptionObject> EXCEPTIONREF;
#else // USE_CHECKED_OBJECTREFS
typedef PTR_ExceptionObject EXCEPTIONREF;
#endif // USE_CHECKED_OBJECTREFS

#endif // _OBJECT_H_
