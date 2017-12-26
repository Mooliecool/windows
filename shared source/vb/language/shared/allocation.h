//-------------------------------------------------------------------------------------------------
//
//  Copyright (c) Microsoft Corporation.  All rights reserved.
//
//  Common heap management infrastructure for the VB products
//
//-------------------------------------------------------------------------------------------------


#pragma once

//-------------------------------------------------------------------------------------------------
//
// This is the common heap used for tracking VB memory usage.  
//
//-------------------------------------------------------------------------------------------------

extern HANDLE g_vbCommonHeap;

#if DEBUG

#define VBHeapCreate(opts)      VsDebHeapCreate((opts), "VB Compiler heap");
#define VBHeapCreateNamed(opts,name)      VsDebHeapCreate((opts), name);
#define VBHeapDestroy(heap)     VsDebHeapDestroy((heap), true);
#define VBAlloc(cb)             VsDebugAllocInternal(g_vbCommonHeap, HEAP_ZERO_MEMORY | HEAP_GENERATE_EXCEPTIONS, (cb), __FILE__, __LINE__, INSTANCE_GLOBAL, NULL)
#define VBAllocOpt(cb,opts)     VsDebugAllocInternal(g_vbCommonHeap, (opts), (cb), __FILE__, __LINE__, INSTANCE_GLOBAL, NULL)
#define VBRealloc(pvOld, cbNew) VBAllocator::ReallocInternal(g_vbCommonHeap, (pvOld), HEAP_GENERATE_EXCEPTIONS, (cbNew), __FILE__, __LINE__, INSTANCE_GLOBAL, NULL)
#define VBFree(pv)              VsDebugFreeInternal(g_vbCommonHeap, (pv))
#define VBSize(pv)              VsDebugSizeInternal(g_vbCommonHeap, (pv))

//-------------------------------------------------------------------------------------------------
//
// Ensure that we also track allocations onto the other approved heaps
//

#define CoTaskMemAlloc(cb)        VsDebOleAlloc(cb)
#define CoTaskMemRealloc(pv, cb)  VsDebOleRealloc(pv, cb)
#define CoTaskMemFree(pv)         VsDebOleFree(pv)

#define SysAllocString(str)             VsDebSysAllocString((str))
#define SysAllocStringByteLen(str, cb)  VsDebSysAllocStringByteLen((str), (cb))
#define SysAllocStringLen(str, cch)     VsDebSysAllocStringLen((str), (cch))


#else 

#define VBHeapCreate(opts)                  HeapCreate((opts), 0, 0)
#define VBHeapCreateNamed(opts,name)        HeapCreate((opts), 0, 0)
#define VBHeapDestroy(heap)                 HeapDestroy((heap))
#define VBAlloc(cb)                         HeapAlloc(g_vbCommonHeap, HEAP_ZERO_MEMORY | HEAP_GENERATE_EXCEPTIONS, (cb))
#define VBAllocOpt(cb,opts)                 HeapAlloc(g_vbCommonHeap, (opts), (cb))
#define VBRealloc(pvOld, cbNew)             VBAllocator::ReallocInternal(g_vbCommonHeap, (pvOld), HEAP_GENERATE_EXCEPTIONS, (cbNew))
#define VBFree(pv)                          HeapFree(g_vbCommonHeap, 0, (pv))
#define VBSize(pv)                          HeapSize(g_vbCommonHeap, 0, (pv))

#endif


//-------------------------------------------------------------------------------------------------
//
// Used to force memory to be zero'd out for new
//
//-------------------------------------------------------------------------------------------------
struct zeromemory_t
{

};

class NorlsAllocator;
extern const zeromemory_t zeromemory;
void* _cdecl operator new(size_t cbSize, const zeromemory_t&);
void* _cdecl operator new[](size_t cbSize, const zeromemory_t&);
void* _cdecl operator new(size_t cbSize, NorlsAllocator &norls);

#if DEBUG

// In DEBUG mode if you said you are safe in the presence of non-zero memory then 
// you will have a chance to prove yourself.  Note we must also override operator
// delete here because we are allocating off of the VBHeap regardless of where 
// we are hosted.  Not doing so will cause allocs on VBHeap and frees on the process
// heap in VSA
#define NEW_CTOR_SAFE() \
    void* operator new (size_t cbSize) { return VBAllocOpt(cbSize, HEAP_GENERATE_EXCEPTIONS);}           \
    void* operator new[] (size_t cbSize) { return VBAllocOpt(cbSize, HEAP_GENERATE_EXCEPTIONS); }        \
    void* operator new (size_t cbSize, const std::nothrow_t& ) { return VBAllocOpt(cbSize, 0);}           \
    void* operator new[] (size_t cbSize, const std::nothrow_t& ) { return VBAllocOpt(cbSize, 0); }        \
    void* operator new (size_t cbSize, NorlsAllocator &norls) { return norls.Alloc(cbSize); }   \
    void* operator new (size_t cbSize, const zeromemory_t& ) { return VBAlloc(cbSize); }    \
    void* operator new (size_t, void *pWhere) { return pWhere; }                            \
    void operator delete(void *pv) { ::VBFree(pv); }                                        \
    void operator delete[](void *pv) { ::VBFree(pv); }

// No need to override operator delete here because new(zeromemory) will always use
// global new under the hood in a safe fashion.
#define NEW_MUST_ZERO() \
    private:            \
        void* operator new (size_t cbsize); \
    public:             \
        void* operator new (size_t cbsize, const zeromemory_t&) \
        {                                                       \
            return ::operator new(cbsize, zeromemory);          \
        }                                                       
#else
#define NEW_MUST_ZERO()
#define NEW_CTOR_SAFE()
#endif

//-------------------------------------------------------------------------------------------------
//
// If you get a linker error the following function please read the following.  
//
// BasicCompiler.lib is linked into several VSA projects.  When linking we inherit their version 
// of new which does NOT guarantee that memory is zero'd.  Instead we get whatever garbage is 
// returned by the standard new.
//
// To guard against this the below function is defined for DEBUG builds and catches all cases where
// we bind to the default operator new.  If you hit this then look at your change list for anywhere
// you added a call that includes new.  There are 2 ways to fix this problem to guarantee our code
// will run correctly in the VSA environment
//
//  1) Use the zeromemory version of new which guarantees zero'd memory in any circumstance
//      Foo *f = new (zeromemory) Foo();
//  2) If the class in question is one you own and it has constructors which guarantee it will behave
//     properly in the presence of new then add the following macro inside your class definition
//      NEW_CTOR_SAFE
//
// In addition if you know for sure your class must have zero memory to work define NEW_MUST_ZERO
//
//-------------------------------------------------------------------------------------------------
#define ENFORCE_ZERO_MEMORY()                                                           \
void* FunctionDoesNotExist();                                                           \
static inline void* __cdecl operator new(size_t sizeParam)                              \
{ return FunctionDoesNotExist(); }                                                      \
static inline void* __cdecl operator new[](size_t sizeParam)                            \
{ return FunctionDoesNotExist(); }                                                      \
static inline void* __cdecl operator new(size_t sizeParam, const std::nothrow_t&)       \
{ return FunctionDoesNotExist(); }                                                      \
static inline void* __cdecl operator new[](size_t sizeParam, const std::nothrow_t&)     \
{ return FunctionDoesNotExist(); }

