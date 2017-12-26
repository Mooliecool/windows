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
/*
 * GCSCAN.H
 *
 * GC Root Scanning
 *
 */

#ifndef _GCSCAN_H_
#define _GCSCAN_H_

#include "gc.h"
typedef void promote_func(Object*&, ScanContext*, DWORD);
typedef void promote_carefully_func(promote_func*, Object*&, ScanContext*, DWORD);

void PromoteCarefully(promote_func fn,
                      Object*& obj, 
                      ScanContext* sc, 
                      DWORD        flags = GC_CALL_INTERIOR);

class CrawlFrame;

typedef struct
{
    promote_func*  f;
    ScanContext*   sc;
    CrawlFrame *   cf;
} GCCONTEXT;



typedef void enum_alloc_context_func(alloc_context*, void*); 

class CNameSpace
{
  public:
    
    // Called on gc start
    static void GcStartDoWork();

    // Regular stack Roots
    static void GcScanRoots (promote_func* fn, int condemned, int max_gen, 
                             ScanContext* sc, GCHeap* Hp=0);
    // After stack scanning, before the user thread start 
    static void AfterGcScanRoots (int condemned, int max_gen, 
                                  ScanContext* sc, GCHeap* Hp=0);
    //
    static void GcScanHandles (promote_func* fn, int condemned, int max_gen, 
                               ScanContext* sc);

    static void GcRuntimeStructuresValid (BOOL bValid);

    static BOOL GetGcRuntimeStructuresValid ();
#ifdef DACCESS_COMPILE    
    static void EnumMemoryRegions(CLRDataEnumMemoryFlags flags);
#endif // DACCESS_COMPILE

#ifdef GC_PROFILING
    //
    static void GcScanHandlesForProfiler (int max_gen, ScanContext* sc);
#endif // GC_PROFILING

    // scan for dead weak pointers
    static void GcWeakPtrScan (promote_func* fn, int condemned, int max_gen, ScanContext*sc );
    static void GcWeakPtrScanBySingleThread (int condemned, int max_gen, ScanContext*sc );

    // scan for dead weak pointers
    static void GcShortWeakPtrScan (promote_func* fn, int condemned, int max_gen, 
                                    ScanContext* sc);

    // start of GC call back - single threaded - 
    static void GcStartWork (int condemned, int max_gen);

    // post-promotions callback
    static void GcPromotionsGranted (int condemned, int max_gen, 
                                     ScanContext* sc);

    // post-promotions callback some roots were demoted
    static void GcDemote (int condemned, int max_gen, ScanContext* sc);

    static void GcEnumAllocContexts (enum_alloc_context_func* fn, void* arg);

    static void GcFixAllocContexts (void* arg, void *heap);

    // post-gc callback.
    static void GcDoneAndThreadsResumed ()
    {
        LEAF_CONTRACT;

        _ASSERTE(0);
    }
	static size_t AskForMoreReservedMemory (size_t old_size, size_t need_size);

    static void VerifyHandleTable(int condemned, int max_gen);
    
private:
#ifdef DACCESS_COMPILE    
    SVAL_DECL(LONG, m_GcStructuresInvalidCnt);
#else
    static volatile LONG m_GcStructuresInvalidCnt;
#endif //DACCESS_COMPILE
};



/*
 * Allocation Helpers
 *
 */

OBJECTREF AllocateValueSzArray(TypeHandle elementType, INT32 length);
    // The main Array allocation routine, can do multi-dimensional
OBJECTREF AllocateArrayEx(TypeHandle arrayClass, INT32 *pArgs, DWORD dwNumArgs, BOOL bAllocateInLargeHeap = FALSE); 
    // Optimized verion of above
OBJECTREF FastAllocatePrimitiveArray(MethodTable* arrayType, DWORD cElements, BOOL bAllocateInLargeHeap = FALSE);


#if defined(_X86_)

    // for x86, we generate efficient allocators for some special cases
    // these are called via inline wrappers that call the generated allocators
    // via function pointers.


    // Create a SD array of primitive types
typedef HCCALL2_PTR(Object*, FastPrimitiveArrayAllocatorFuncPtr, CorElementType type, DWORD cElements);

extern FastPrimitiveArrayAllocatorFuncPtr fastPrimitiveArrayAllocator;

    // The fast version always allocates in the normal heap
inline OBJECTREF AllocatePrimitiveArray(CorElementType type, DWORD cElements)
{
    CONTRACTL {
        THROWS;
        GC_TRIGGERS;
        MODE_COOPERATIVE; // returns an objref without pinning it => cooperative
    } CONTRACTL_END;

    return OBJECTREF( HCCALL2(fastPrimitiveArrayAllocator, type, cElements) );
}

    // The slow version is distinguished via overloading by an additional parameter
OBJECTREF AllocatePrimitiveArray(CorElementType type, DWORD cElements, BOOL bAllocateInLargeHeap);


// Allocate SD array of object pointers.  StubLinker-generated asm code might
// implement this, so the element TypeHandle is passed as a PVOID to avoid any
// struct calling convention weirdness.
typedef HCCALL2_PTR(Object*, FastObjectArrayAllocatorFuncPtr, /*TypeHandle*/PVOID ElementType, DWORD cElements);

extern FastObjectArrayAllocatorFuncPtr fastObjectArrayAllocator;

    // The fast version always allocates in the normal heap
OBJECTREF AllocateObjectArray(DWORD cElements, TypeHandle ElementType);

    // The slow version is distinguished via overloading by an additional parameter
OBJECTREF AllocateObjectArray(DWORD cElements, TypeHandle ElementType, BOOL bAllocateInLargeHeap);


    // Allocate string
typedef HCCALL1_PTR(StringObject*, FastStringAllocatorFuncPtr, DWORD cchArrayLength);

extern FastStringAllocatorFuncPtr fastStringAllocator;

inline STRINGREF AllocateString( DWORD cchArrayLength )
{
    CONTRACTL {
        THROWS;
        GC_TRIGGERS;
        MODE_COOPERATIVE; // returns an objref without pinning it => cooperative
    } CONTRACTL_END;

    return STRINGREF(HCCALL1(fastStringAllocator, (cchArrayLength-1)));
}

    // The slow version, implemented in gcscan.cpp
STRINGREF SlowAllocateString( DWORD cchArrayLength );

#else

// On other platforms, go to the (somewhat less efficient) implementations in gcscan.cpp

    // Create a SD array of primitive types
OBJECTREF AllocatePrimitiveArray(CorElementType type, DWORD cElements, BOOL bAllocateInLargeHeap = FALSE);

    // Allocate SD array of object pointers
OBJECTREF AllocateObjectArray(DWORD cElements, TypeHandle ElementType, BOOL bAllocateInLargeHeap = FALSE);

STRINGREF SlowAllocateString( DWORD cchArrayLength );

inline STRINGREF AllocateString( DWORD cchArrayLength )
{
    WRAPPER_CONTRACT;

    return SlowAllocateString( cchArrayLength );
}

#endif

OBJECTREF DupArrayForCloning(BASEARRAYREF pRef, BOOL bAllocateInLargeHeap = FALSE);

OBJECTREF AllocateUninitializedStruct(MethodTable *pMT);

// The JIT requests the EE to specify an allocation helper to use at each new-site.
// The EE makes this choice based on whether context boundaries may be involved,
// whether the type is a COM object, whether it is a large object,
// whether the object requires finalization.
// These functions will throw OutOfMemoryException so don't need to check
// for NULL return value from them.

OBJECTREF AllocateObject( MethodTable *pMT );
OBJECTREF AllocateObjectSpecial( MethodTable *pMT );
OBJECTREF FastAllocateObject( MethodTable *pMT );

#ifdef MAXALLOC
AllocRequestManager* GetGCAllocManager();
#endif

extern void StompWriteBarrierEphemeral();
extern void StompWriteBarrierResize(BOOL bReqUpperBoundsCheck);


#endif // _GCSCAN_H_
