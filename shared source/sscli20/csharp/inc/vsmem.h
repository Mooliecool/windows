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
//---------------------------------------------------------------------------
// VsMem.H
//
// Common memory allocation routines
//---------------------------------------------------------------------------
#ifndef _INC_VSMEM_H
#define _INC_VSMEM_H

PVOID WINAPI VSRetAlloc (SIZE_T cb);
PVOID WINAPI VSRetAllocZero (SIZE_T cb);
PVOID WINAPI VSRetRealloc (PVOID pv, SIZE_T cb);
PVOID WINAPI VSRetReallocZero (PVOID pv, SIZE_T cb);
BOOL  WINAPI VSRetFree (PVOID pv);
SIZE_T WINAPI VSRetSize (PVOID pv);


//=----------------------------------------------------------------------=
// Retail allocation routines
//

#ifndef VSMEM_NO_VSALLOC

    #ifdef VSMEM_RET_ALLOC
        // olgaark: not sure why we need an extra function in retail alloc
        // so it is not a default now 
        #define VSAlloc(cb)       VSRetAlloc(cb)
        #define VSAllocZero(cb)   VSRetAllocZero(cb)
        #define VSRealloc(pv, cb) VSRetRealloc(pv, cb)
        #define VSReallocZero(pv, cb) VSRetReallocZero(pv, cb)
        #define VSFree(pv)        VSRetFree(pv)
        #define VSSize(pv)        VSRetSize(pv)
        
    #else //VSMEM_NO_RET_ALLOC

        #define VSAlloc(cb)             HeapAlloc(GetProcessHeap(), 0, (cb))
        #define VSAllocZero(cb)         HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, (cb))
        #define VSRealloc(pv, cb)       HeapReAlloc(GetProcessHeap(), 0, (pv), (cb))
        #define VSReallocZero(pv, cb)   HeapReAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, (pv), (cb))
        #define VSFree(pv)              HeapFree(GetProcessHeap(), 0, (pv))
        #define VSSize(pv)              HeapSize(GetProcessHeap(), 0, (pv))
        
    #endif //VSMEM_NO_RET_ALLOC    
    
#endif //VSMEM_NO_VSALLOC

#define VSHeapCreate(flags, name)       HeapCreate(flags, 0, 0)
#define VSHeapDestroy(heap, fLeakCheck) HeapDestroy(heap)
#define VSHeapAlloc(heap, cb)           HeapAlloc(heap, 0, cb)
#define VSHeapAllocZero(heap, cb)       HeapAlloc(heap, HEAP_ZERO_MEMORY, cb)
#define VSHeapRealloc(heap, pv, cb)     HeapReAlloc(heap, 0, pv, cb)
#define VSHeapReallocZero(heap, pv, cb) HeapReAlloc(heap, HEAP_ZERO_MEMORY, pv, cb)
#define VSHeapFree(heap, pv)            HeapFree(heap, 0, pv)
#define VSHeapSize(heap, pv)            HeapSize(heap, 0, pv)

#define VSOleAlloc(cb)        CoTaskMemAlloc(cb)
#define VSOleFree(pv)         CoTaskMemFree(pv)
#define VSOleRealloc(pv, cb)  CoTaskMemRealloc(pv, cb)

#ifndef VSMEM_NO_REDEFINE_NEW

    #define vs_new new
    #define DECLARE_CLASS_NEW(sizevar) void * _cdecl operator new (size_t sizevar)
    #define DEFINE_CLASS_NEW(c,sizevar) void * _cdecl c::operator new (size_t sizevar)

#endif //VSMEM_NO_REDEFINE_NEW


#endif // _INC_VSMEM_H

