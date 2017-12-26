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
//
// Support for external access of runtime data structures.  These
// macros and templates hide the details of pointer and data handling
// so that data structures and code can be compiled to work both
// in-process and through a special memory access layer.
//
// This code assumes the existence of two different pieces of code,
// the target, the runtime code that is going to be examined, and
// the host, the code that's doing the examining.  Access to the
// target is abstracted so the target may be a live process on the
// same machine, a live process on a different machine, a dump file
// or whatever.  No assumptions should be made about accessibility
// of the target.
//
// This code assumes that the data in the target is static.  Any
// time the target's data changes the interfaces must be reset so
// that potentially stale data is discarded.
//
// This code is intended for read access and there is no
// way to write data back currently.
//
// This code works by hiding the details of access to target memory.
// Access is divided into two types:
// 1. DPTR - access to a piece of data.
// 2. VPTR - access to a class with a vtable.  The class can only have
//           a single vtable pointer at the beginning of the class instance.
// Things only need to be declared as VPTRs when it is necessary to
// call virtual functions in the host.  In that case the access layer
// must do extra work to provide a host vtable for the object when
// it is retrieved so that virtual functions can be called.
//
// When compiling with DACCESS_COMPILE the macros turn into templates
// which replace pointers with smart pointers that know how to fetch
// data from the target process and provide a host process version of it.
// Normal data structure access will transparently receive a host copy
// of the data and proceed, so code such as
//     typedef DPTR(Class) PTR_Class;
//     PTR_Class cls;
//     int val = cls->m_Int;
// will work without modification.  The appropriate operators are overloaded
// to provide transparent access, such as the -> operator in this case.
// Note that the convention is to create an appropriate typedef for
// each type that will be accessed.  This hides the particular details
// of the type declaration and makes the usage look more like regular code.
//
// The ?PTR classes also have an implicit base type cast operator to
// produce a host-pointer instance of the given type.  For example
//     Class* cls = PTR_Class(addr);
// works by implicit conversion from the PTR_Class created by wrapping
// to a host-side Class instance.  Again, this means that existing code
// can work without modification.
//
// Code Example:
//
// typedef struct _rangesection
// {
//     PTR_IJitManager pjit;
//     PTR_RangeSection pright;
//     PTR_RangeSection pleft;
//     ... Other fields omitted ...
// } RangeSection;
// 
//     RangeSection* pRS = m_RangeTree;
//
//     while (pRS != NULL)
//     {
//         if (currentPC < pRS->LowAddress)
//             pRS=pRS->pleft;
//         else if (currentPC > pRS->HighAddress)
//             pRS=pRS->pright;
//         else
//         {
//             return pRS->pjit;
//         }
//     }
//
// This code does not require any modifications.  The global reference
// provided by m_RangeTree will be a host version of the RangeSection
// instantiated by conversion.  The references to pRS->pleft and
// pRS->pright will refer to DPTRs due to the modified declaration.
// In the assignment statement the compiler will automatically use
// the implicit conversion from PTR_RangeSection to RangeSection*,
// causing a host instance to be created.  Finally, if an appropriate
// section is found the use of pRS->pjit will cause an implicit
// conversion from PTR_IJitManager to IJitManager.  The VPTR code
// will look at target memory to determine the actual derived class
// for the JitManager and instantiate the right class in the host so
// that host virtual functions can be used just as they would in
// the target.
//
// There are situations where code modifications are required, though.
//
// 1.  Any time the actual value of an address matters, such as using
//     it as a search key in a tree, the target address must be used.
//
// An example of this is the RangeSection tree used to locate JIT
// managers.  A portion of this code is shown above.  Each
// RangeSection node in the tree describes a range of addresses
// managed by the JitMan.  These addresses are just being used as
// values, not to dereference through, so there are not DPTRs.  When
// searching the range tree for an address the address used in the
// search must be a target address as that's what values are kept in
// the RangeSections.  In the code shown above, currentPC must be a
// target address as the RangeSections in the tree are all target
// addresses.  Use the PTR_TO_TADDR macro to retrieve the target address
// of a ?PTR.  Use PTR_HOST_TO_TADDR to convert a host address to the
// target address used to retrieve that particular instance.
//
// 2.  Any time an address is modified, such as by address arithmetic,
//     the arithmetic must be performed on the target address.
//
// When a host instance is created it is created for the type in use.
// There is no particular relation to any other instance, so address
// arithmetic cannot be used to get from one instance to any other
// part of memory.  For example
//     char* Func(Class* cls)
//     {
//         // String follows the basic Class data.
//         return (char*)(cls + 1);
//     }
// does not work with external access because the Class* used would
// have retrieved only a Class worth of data.  There is no string
// following the host instance.  Instead, this code should use
// PTR_HOST_TO_TADDR() to get the target address of the Class
// instance, add sizeof(*cls) and then create a new ?PTR to access
// the desired data.  Note that the newly retrieved data will not
// be contiguous with the Class instance, so address arithmetic
// will still not work.
//
// Previous Code:
//
//     BOOL IsTarget(LPVOID ip)
//     {
//         StubCallInstrs* pStubCallInstrs = GetStubCallInstrs();
//
//         if (ip == (LPVOID) &(pStubCallInstrs->m_op))
//         {
//             return TRUE;
//         }
//
// Modified Code:
//
//     BOOL IsTarget(LPVOID ip)
//     {
//         StubCallInstrs* pStubCallInstrs = GetStubCallInstrs();
//
//         if ((TADDR)ip == PTR_HOST_TO_TADDR(pStubCallInstrs) +
//             (TADDR)offsetof(StubCallInstrs, m_op))
//         {
//             return TRUE;
//         }
//
// The parameter ip is a target address, so the host pStubCallInstrs
// cannot be used to derive an address from.  The member & reference
// has to be replaced with a conversion from host to target address
// followed by explicit offsetting for the field.
//
// PTR_HOST_MEMBER_TADDR is a convenience macro that encapsulates
// these two operations, so the above code could also be:
//
//     if ((TADDR)ip ==
//         PTR_HOST_MEMBER_TADDR(StubCallInstrs, pStubCallInstrs, m_op))
//
// 3.  Any time the amount of memory referenced through an address
//     changes, such as by casting to a different type, a new ?PTR
//     must be created.
//
// Host instances are created and stored based on both the target
// address and size of access.  The access code has no way of knowing
// all possible ways that data will be retrieved for a given address
// so if code changes the way it accesses through an address a new
// ?PTR must be used, which may lead to a difference instance and
// different host address.  This means that pointer identity does not hold
// across casts, so code like
//     Class* cls = PTR_Class(addr);
//     Class2* cls2 = PTR_Class2(addr);
//     return cls == cls2;
// will fail because the host-side instances have no relation to each
// other.  That isn't a problem, since by rule #1 you shouldn't be
// relying on specific host address values.
//
// Previous Code:
//
//     return (ArrayClass *) m_pMethTab->GetClass();
//
// Modified Code:
//
//     return PTR_ArrayClass(m_pMethTab->GetClass());
//
// The ?PTR templates have an implicit conversion from a host pointer
// to a target address, so the cast above constructs a new
// PTR_ArrayClass by implicitly converting the host pointer result
// from GetClass() to its target address and using that as the address
// of the new PTR_ArrayClass.  As mentioned, the actual host-side
// pointer values may not be the same.
//
// Host pointer identity can be assumed as long as the type of access
// is the same.  In the example above, if both accesses were of type
// Class then the host pointer will be the same, so it is safe to
// retrieve the target address of an instance and then later get
// a new host pointer for the target address using the same type as
// the host pointer in that case will be the same.  This is enabled
// by caching all of the retrieved host instances.  This cache is searched
// by the addr:size pair and when there's a match the existing instance
// is reused.  This increases performance and also allows simple
// pointer identity to hold.  It does mean that host memory grows
// in proportion to the amount of target memory being referenced,
// so retrieving extraneous data should be avoided.
// The host-side data cache grows until the Flush() method is called,
// at which point all host-side data is discarded.  No host
// instance pointers should be held across a Flush().
//
// Accessing into an object can lead to some unusual behavior.  For
// example, the SList class relies on objects to contain an SLink
// instance that it uses for list maintenance.  This SLink can be
// embedded anywhere in the larger object.  The SList access is always
// purely to an SLink, so when using the access layer it will only
// retrieve an SLink's worth of data.  The SList template will then
// do some address arithmetic to determine the start of the real
// object and cast the resulting pointer to the final object type.
// When using the access layer this results in a new ?PTR being
// created and used, so a new instance will result.  The internal
// SLink instance will have no relation to the new object instance
// even though in target address terms one is embedded in the other.
// The assumption of data stability means that this won't cause
// a problem, but care must be taken with the address arithmetic,
// as layed out in rules #2 and #3.
//
// 4.  Global address references cannot be used.  Any reference to a
//     global piece of code or data, such as a function address, global
//     variable or class static variable, must be changed.
//
// The external access code may load at a different base address than
// the target process code.  Global addresses are therefore not
// meaningful and must be replaced with something else.  There isn't
// a single solution, so replacements must be done on a case-by-case
// basis.
//
// The simplest case is a global or class static variable.  All
// declarations must be replaced with a special declaration that
// compiles into a modified accessor template value when compiled for
// external data access.  Uses of the variable automatically are fixed
// up by the template instance.  Note that assignment to the global
// must be independently ifdef'ed as the external access layer should
// not make any modifications.
//
// Macros allow for simple declaration of a class static and global
// values that compile into an appropriate templated value.
//
// Previous Code:
//
//     static RangeSection* m_RangeTree;
//     RangeSection* ExecutionManager::m_RangeTree;
//
//     extern ThreadStore* g_pThreadStore;
//     ThreadStore* g_pThreadStore = &StaticStore;
//
// Modified Code:
//
//     SPTR_DECL(RangeSection, m_RangeTree);
//     SPTR_IMPL(RangeSection, ExecutionManager, m_RangeTree);
//
//     GPTR_DECL(ThreadStore, g_pThreadStore);
//     GPTR_IMPL_INIT(ThreadStore, g_pThreadStore, &StaticStore);
//
// When declaring the variable, the first argument declares the
// variable's type and the second argument declares the variable's
// name.  When defining the variable the arguments are similar, with
// an extra class name parameter for the static class variable case.
// If an initializer is need the IMPL_INIT macro should be used.
//
// Things get slightly more complicated when declaring an embedded
// array.  In this case the data element is not a single element and
// therefore cannot be represented by a ?PTR.
//
// All uses of ?VAR_DECL must have a corresponding entry given in the
// DacGlobals structure in src\inc\daccess.h.  For SVAR_DECL the entry
// is class__name.  For GVAR_DECL the entry is dac__name.
//
// Certain pieces of code, such as the stack walker, rely on identifying
// an object from its vtable address.  As the target vtable addresses
// do not necessarily correspond to the vtables used in the host, these
// references must be translated.  The access layer maintains translation
// tables for all classes used with VPTR and can return the target
// vtable pointer for any host vtable in the known list of VPTR classes.
//
// ----- Errors:
//
// All errors in the access layer are reported via exceptions.  The
// formal access layer methods catch all such exceptions and turn
// them into the appropriate error, so this generally isn't visible
// to users of the access layer.
//
// ----- DPTR Declaration:
//
// Create a typedef for the type with typedef DPTR(type) PTR_type;
// Replace type* with PTR_type.
//
// ----- VPTR Declaration:
//
// VPTR can only be used on classes that have a single vtable
// pointer at the beginning of the object.  This should be true
// for a normal single-inheritance object.
//
// All of the classes that may be instantiated need to be identified
// and marked.  In the base class declaration add either
// VPTR_BASE_VTABLE_CLASS if the class is abstract or
// VPTR_BASE_CONCRETE_VTABLE_CLASS if the class is concrete.  In each
// derived class add VPTR_VTABLE_CLASS.  If you end up with compile or
// link errors for an unresolved method called VPtrSize you missed a
// derived class declaration.
//
// As described above, dac can only handle classes with a single
// vtable.  However, there's a special case for multiple inheritance
// situations when only one of the classes is needed for dac.  If
// the base class needed is the first class in the derived class's
// layout then it can be used with dac via using the VPTR_MULTI_CLASS
// macros.  Use with extreme care.
//
// All classes to be instantiated must be listed in src\inc\vptr_list.h.
//
// Create a typedef for the type with typedef VPTR(type) PTR_type;
// When using a VPTR, replace Class* with PTR_Class.
//
// ----- Specific Macros:
//
// PTR_TO_TADDR(ptr)
// Retrieves the raw target address for a ?PTR.
//
// PTR_HOST_TO_TADDR(host)
// Given a host address of an instance produced by a ?PTR reference,
// return the original target address.  The host address must
// be an exact match for an instance.
//
// VPTR_HOST_VTABLE_TO_TADDR(host)
// Given the host vtable pointer for a known VPTR class, return
// the target vtable pointer.
//
// PTR_HOST_MEMBER_TADDR(type, host, memb)
// Retrieves the target address of a host instance pointer and
// offsets it by the given member's offset within the type.
//
// PTR_READ(addr, size)
// Reads a block of memory from the target and returns a host
// pointer for it.  Useful for reading blocks of data from the target
// whose size is only known at runtime, such as raw code for a jitted
// method.  If the data being read is actually an object, use SPTR
// instead to get better type semantics.
//
// DAC_EMPTY()
// DAC_EMPTY_ERR()
// DAC_EMPTY_RET(retVal)
// Provides an empty method implementation when compiled
// for DACCESS_COMPILE.  For example, use to stub out methods needed
// for vtable entries but otherwise unused.
//
// These macros are designed to turn into normal code when compiled
// without DACCESS_COMPILE.
//
//*****************************************************************************

#ifndef __daccess_h__
#define __daccess_h__

#include "switches.h"

#ifndef __in
#include <specstrings.h>
#endif

#define DACCESS_TABLE_RESOURCE "COREXTERNALDATAACCESSRESOURCE"

#ifdef DACCESS_COMPILE

//

typedef ULONG_PTR TADDR;

//
// The following table contains all the global information
// that data access needs to begin operation.  All of the
// values stored here are RVAs.  DacGlobalBase() returns
// the current base address to combine with to get a
// full target address.
//

typedef struct _DacGlobals
{
// These will define all of the dac related mscorwks static and global variables    
#define DEFINE_DACVAR(id_type, size, id)                 id_type id;
#define DEFINE_DACVAR_NO_DUMP(id_type, size, id)        id_type id;
#include "dacvars.h"

    union {
        ULONG dac__rgJITHelperRVAs[1];
        struct {
            // Some functions are filled in dynamically and have NULL for the function
            // name, so we have to add the helper code into the field name.
#define JITHELPER(code,fn) ULONG dac__JIT_HELPER_FUNCTION__##code##__##fn;
#define SPECIALJITHELPER(code,fn) ULONG dac__JIT_HELPER_FUNCTION__##code##__NULL;
#include <jithelpers.h>
#undef SPECIALJITHELPER
#undef JITHELPER
        } _helpers_;
    };

    // Global functions.
    ULONG fn__QueueUserWorkItemCallback;
    ULONG fn__ThreadpoolMgr__AsyncCallbackCompletion;
    ULONG fn__ThreadpoolMgr__AsyncTimerCallbackCompletion;
    ULONG fn__DACNotifyCompilationFinished;
#ifdef _X86_
    ULONG fn__NativeDelayFixupAsmStub;
    ULONG fn__NativeDelayFixupAsmStubRet;
#endif
    ULONG fn__PInvokeCalliReturnFromCall;
    ULONG fn__NDirectGenericStubReturnFromCall;
    ULONG fn__DllImportForDelegateGenericStubReturnFromCall;

    // Vtable pointer values for all classes that must
    // be instanted using vtable pointers as the identity.
#define VPTR_CLASS(name) ULONG name##__vtAddr;
#define VPTR_MULTI_CLASS(name, keyBase) ULONG name##__##keyBase##__mvtAddr;
#include <vptr_list.h>
#undef VPTR_CLASS
#undef VPTR_MULTI_CLASS
} DacGlobals;

extern DacGlobals g_dacGlobals;

#ifdef __cplusplus
extern "C" {
#endif

// These two functions are largely just for marking code
// that is not fully converted.  DacWarning prints a debug
// message, while DacNotImpl throws a not-implemented exception.
void __cdecl DacWarning(__in __in_z char* format, ...);
void DacNotImpl(void);

void    DacError(HRESULT err);
TADDR   DacGlobalBase(void);
HRESULT DacReadAll(TADDR addr, PVOID buffer, ULONG32 size, bool throwEx);
HRESULT DacWriteAll(TADDR addr, PVOID buffer, ULONG32 size, bool throwEx);
HRESULT DacAllocVirtual(TADDR addr, ULONG32 size,
                        ULONG32 typeFlags, ULONG32 protectFlags,
                        bool throwEx, TADDR* mem);
HRESULT DacFreeVirtual(TADDR mem, ULONG32 size, ULONG32 typeFlags,
                       bool throwEx);
PVOID   DacInstantiateTypeByAddress(TADDR addr, ULONG32 size, bool throwEx);
PVOID   DacInstantiateTypeByAddressNoReport(TADDR addr, ULONG32 size, bool throwEx);
PVOID   DacInstantiateClassByVTable(TADDR addr, bool throwEx);
PSTR    DacInstantiateStringA(TADDR addr, ULONG32 maxChars, bool throwEx);
PWSTR   DacInstantiateStringW(TADDR addr, ULONG32 maxChars, bool throwEx);
TADDR   DacGetTargetAddrForHostAddr(LPCVOID ptr, bool throwEx);
TADDR   DacGetTargetVtForHostVt(LPCVOID vtHost, bool throwEx);
PWSTR   DacGetVtNameW(TADDR targetVtable);
void    DacEnumMemoryRegion(TADDR addr, ULONG32 size);
HRESULT DacWriteHostInstance(PVOID host, bool throwEx);
HRESULT DacPushInstanceState(bool throwEx);
void    DacPopInstanceState(void);

// Occasionally it's necessary to allocate some host memory for
// instance data that's created on the fly and so doesn't directly
// correspond to target memory.  These are held and freed on flush
// like other instances but can't be looked up by address.
PVOID DacAllocHostOnlyInstance(ULONG32 size, bool throwEx);

// Host instances can be marked as they are enumerated in
// order to break cycles.  This function returns true if
// the instance is already marked, otherwise it marks the
// instance and returns false.
bool DacHostPtrHasEnumMark(LPCVOID host);

// Determines if EnumMemoryRegions has been called on a method descriptor.
// This helps perf for minidumps of apps with large managed stacks.
bool DacHasMethodDescBeenEnumerated(LPCVOID pMD);

// Sets a flag indicating that EnumMemoryRegions on a method desciptor 
// has been successfully called. The function returns true if
// this flag had been previously set.
bool DacSetMethodDescEnumerated(LPCVOID pMD);

// Determines if a method descriptor is valid
BOOL DacValidateMD(LPCVOID pMD);

//
// Convenience macros for EnumMemoryRegions implementations.
//
    
// Enumerate the given host instance and return
// true if the instance hasn't already been enumerated.
#define DacEnumHostDPtrMem(host) \
    (!DacHostPtrHasEnumMark(host) ? \
     (DacEnumMemoryRegion(PTR_HOST_TO_TADDR(host), sizeof(*host)), \
      true) : false)
#define DacEnumHostSPtrMem(host, type) \
    (!DacHostPtrHasEnumMark(host) ? \
     (DacEnumMemoryRegion(PTR_HOST_TO_TADDR(host), \
                          type::DacSize(PTR_HOST_TO_TADDR(host))), \
      true) : false)
#define DacEnumHostVPtrMem(host) \
    (!DacHostPtrHasEnumMark(host) ? \
     (DacEnumMemoryRegion(PTR_HOST_TO_TADDR(host), (host)->VPtrSize()), \
      true) : false)

// Check enumeration of 'this' and return if this has already been
// enumerated.  Making this the first line of an object's EnumMemoryRegions
// method will prevent cycles.
#define DAC_CHECK_ENUM_THIS() \
    if (DacHostPtrHasEnumMark(this)) return
#define DAC_ENUM_DTHIS() \
    if (!DacEnumHostDPtrMem(this)) return
#define DAC_ENUM_STHIS(type) \
    if (!DacEnumHostSPtrMem(this, type)) return
#define DAC_ENUM_VTHIS() \
    if (!DacEnumHostVPtrMem(this)) return
    
#ifdef __cplusplus
}

interface IMDInternalImport* DacGetMDImport(const class PEFile* peFile,
                                            bool throwEx);

int DacGetIlMethodSize(TADDR methAddr);
struct COR_ILMETHOD* DacGetIlMethod(TADDR methAddr);

// Base pointer wrapper which provides common behavior.
class __TPtrBase
{
public:
    __TPtrBase(void)
    {
        // Make uninitialized pointers obvious.
        m_addr = (TADDR)-1;
    }
    __TPtrBase(TADDR addr)
    {
        m_addr = addr;
    }

    bool operator!() const
    {
        return m_addr == 0;
    }
    bool operator==(TADDR addr) const
    {
        return m_addr == addr;
    }
    bool operator!=(TADDR addr) const
    {
        return m_addr != addr;
    }
    bool operator<(TADDR addr) const
    {
        return m_addr < addr;
    }
    bool operator>(TADDR addr) const
    {
        return m_addr > addr;
    }
    bool operator<=(TADDR addr) const
    {
        return m_addr <= addr;
    }
    bool operator>=(TADDR addr) const
    {
        return m_addr >= addr;
    }

    TADDR GetAddr(void) const
    {
        return m_addr;
    }
    TADDR SetAddr(TADDR addr)
    {
        m_addr = addr;
        return addr;
    }

protected:
    TADDR m_addr;
};

// Pointer wrapper for objects which are just plain data
// and need no special handling.
template<typename type>
class __DPtr : public __TPtrBase
{
public:
    typedef type _Type;
    typedef type* _Ptr;
    
    __DPtr< type >(void) : __TPtrBase() {}
    __DPtr< type >(TADDR addr) : __TPtrBase(addr) {}
    explicit __DPtr< type >(__TPtrBase addr)
    {
        m_addr = addr.GetAddr();
    }
    explicit __DPtr< type >(type* host)
    {
        m_addr = DacGetTargetAddrForHostAddr(host, true);
    }

    __DPtr< type >& operator=(const __TPtrBase& ptr)
    {
        m_addr = ptr.m_addr;
        return *this;
    }
    __DPtr< type >& operator=(TADDR addr)
    {
        m_addr = addr;
        return *this;
    }

    operator type*() const
    {
        return (type*)DacInstantiateTypeByAddress(m_addr, sizeof(type), true);
    }
    type* operator->() const
    {
        return (type*)DacInstantiateTypeByAddress(m_addr, sizeof(type), true);
    }
    type& operator*(void) const
    {
        return *(type*)DacInstantiateTypeByAddress(m_addr, sizeof(type), true);
    }

    bool operator==(const __DPtr< type >& ptr) const
    {
        return m_addr == ptr.m_addr;
    }
    bool operator==(TADDR addr) const
    {
        return m_addr == addr;
    }
    bool operator!=(const __DPtr< type >& ptr) const
    {
        return !operator==(ptr);
    }
    bool operator!=(TADDR addr) const
    {
        return m_addr != addr;
    }
    bool operator<(const __DPtr< type >& ptr) const
    {
        return m_addr < ptr.m_addr;
    }
    bool operator>(const __DPtr< type >& ptr) const
    {
        return m_addr > ptr.m_addr;
    }
    bool operator<=(const __DPtr< type >& ptr) const
    {
        return m_addr <= ptr.m_addr;
    }
    bool operator>=(const __DPtr< type >& ptr) const
    {
        return m_addr >= ptr.m_addr;
    }

    type& operator[](signed char index)
    {
        return *(type*)
            DacInstantiateTypeByAddress(m_addr + index * sizeof(type),
                                        sizeof(type), true);
    }
    type& operator[](unsigned char index)
    {
        return *(type*)
            DacInstantiateTypeByAddress(m_addr + index * sizeof(type),
                                        sizeof(type), true);
    }
    type& operator[](short index)
    {
        return *(type*)
            DacInstantiateTypeByAddress(m_addr + index * sizeof(type),
                                        sizeof(type), true);
    }
    type& operator[](unsigned short index)
    {
        return *(type*)
            DacInstantiateTypeByAddress(m_addr + index * sizeof(type),
                                        sizeof(type), true);
    }
    type& operator[](int index)
    {
        return *(type*)
            DacInstantiateTypeByAddress(m_addr + index * sizeof(type),
                                        sizeof(type), true);
    }
    type& operator[](unsigned int index)
    {
        return *(type*)
            DacInstantiateTypeByAddress(m_addr + index * sizeof(type),
                                        sizeof(type), true);
    }

// long is defined to int on 64-bit PAL
#if !defined(FEATURE_PAL) || !defined(long)
    type& operator[](long index)
    {
        return *(type*)
            DacInstantiateTypeByAddress(m_addr + index * sizeof(type),
                                        sizeof(type), true);
    }
    type& operator[](unsigned long index)
    {
        return *(type*)
            DacInstantiateTypeByAddress(m_addr + index * sizeof(type),
                                        sizeof(type), true);
    }
#endif // #if !defined(FEATURE_PAL) || !defined(long)


    //-------------------------------------------------------------------------
    // operator+
    
    __DPtr< type > operator+(unsigned short val)
    {
        return __DPtr< type >(m_addr + val * sizeof(type));
    }
    __DPtr< type > operator+(short val)
    {
        return __DPtr< type >(m_addr + val * sizeof(type));
    }
    __DPtr< type > operator+(size_t val)
    {
        return __DPtr< type >(m_addr + val * sizeof(type));
    }
#if !defined (_X86_)
    __DPtr< type > operator+(unsigned int val)
    {
        return __DPtr< type >(m_addr + val * sizeof(type));
    }
#endif
    __DPtr< type > operator+(int val)
    {
        return __DPtr< type >(m_addr + val * sizeof(type));
    }
#ifndef PLATFORM_UNIX // for now, everything else is 32 bit
    __DPtr< type > operator+(unsigned long val)
    {
        return __DPtr< type >(m_addr + val * sizeof(type));
    }
    __DPtr< type > operator+(long val)
    {
        return __DPtr< type >(m_addr + val * sizeof(type));
    }
#endif

    //-------------------------------------------------------------------------
    // operator-
    
    __DPtr< type > operator-(unsigned short val)
    {
        return __DPtr< type >(m_addr - val * sizeof(type));
    }
    __DPtr< type > operator-(short val)
    {
        return __DPtr< type >(m_addr - val * sizeof(type));
    }
    __DPtr< type > operator-(size_t val)
    {
        return __DPtr< type >(m_addr - val * sizeof(type));
    }
#if !defined (_X86_)
    __DPtr< type > operator-(unsigned int val)
    {
        return __DPtr< type >(m_addr - val * sizeof(type));
    }
#endif
    __DPtr< type > operator-(int val)
    {
        return __DPtr< type >(m_addr - val * sizeof(type));
    }
#ifdef _MSC_VER // for now, everything else is 32 bit
    __DPtr< type > operator-(unsigned long val)
    {
        return __DPtr< type >(m_addr - val * sizeof(type));
    }
    __DPtr< type > operator-(long val)
    {
        return __DPtr< type >(m_addr - val * sizeof(type));
    }
#endif

    //-------------------------------------------------------------------------
    
    __DPtr< type >& operator+=(size_t val)
    {
        m_addr += val * sizeof(type);
        return *this;
    }
    __DPtr< type >& operator-=(size_t val)
    {
        m_addr -= val * sizeof(type);
        return *this;
    }
    
    __DPtr< type >& operator++()
    {
        m_addr += sizeof(type);
        return *this;
    }
    __DPtr< type >& operator--()
    {
        m_addr -= sizeof(type);
        return *this;
    }
    __DPtr< type > operator++(int postfix)
    {
        __DPtr< type > orig = *this;
        m_addr += sizeof(type);
        return orig;
    }
    __DPtr< type > operator--(int postfix)
    {
        __DPtr< type > orig = *this;
        m_addr -= sizeof(type);
        return orig;
    }

    bool IsValid(void) const
    {
        return m_addr &&
            DacInstantiateTypeByAddress(m_addr, sizeof(type),
                                        false) != NULL;
    }
    void EnumMem(void) const
    {
        DacEnumMemoryRegion(m_addr, sizeof(type));
    }
};

#define DPTR(type) __DPtr< type >

// Pointer wrapper for objects which are just plain data
// but whose size is not the same as the base type size.
// This can be used for prefetching data for arrays or
// for cases where an object has a variable size.
template<typename type>
class __SPtr : public __TPtrBase
{
public:
    typedef type _Type;
    typedef type* _Ptr;
    
    __SPtr< type >(void) : __TPtrBase() {}
    __SPtr< type >(TADDR addr) : __TPtrBase(addr) {}
    explicit __SPtr< type >(__TPtrBase addr)
    {
        m_addr = addr.GetAddr();
    }
    explicit __SPtr< type >(type* host)
    {
        m_addr = DacGetTargetAddrForHostAddr(host, true);
    }

    __SPtr< type >& operator=(const __TPtrBase& ptr)
    {
        m_addr = ptr.m_addr;
        return *this;
    }
    __SPtr< type >& operator=(TADDR addr)
    {
        m_addr = addr;
        return *this;
    }

    operator type*() const
    {
        if (m_addr)
        {
            return (type*)DacInstantiateTypeByAddress(m_addr,
                                                      type::DacSize(m_addr),
                                                      true);
        }
        else
        {
            return (type*)NULL;
        }
    }
    type* operator->() const
    {
        if (m_addr)
        {
            return (type*)DacInstantiateTypeByAddress(m_addr,
                                                      type::DacSize(m_addr),
                                                      true);
        }
        else
        {
            return (type*)NULL;
        }
    }
    type& operator*(void) const
    {
        if (!m_addr)
        {
            DacError(E_INVALIDARG);
        }
        
        return *(type*)DacInstantiateTypeByAddress(m_addr,
                                                   type::DacSize(m_addr),
                                                   true);
    }

    bool IsValid(void) const
    {
        return m_addr &&
            DacInstantiateTypeByAddress(m_addr, type::DacSize(m_addr),
                                        false) != NULL;
    }
    void EnumMem(void) const
    {
        if (m_addr)
        {
            DacEnumMemoryRegion(m_addr, type::DacSize(m_addr));
        }
    }
};

#define SPTR(type) __SPtr< type >

// Pointer wrapper for objects which have a single leading
// vtable, such as objects in a single-inheritance tree.
// The base class of all such trees must have use
// VPTR_BASE_VTABLE_CLASS in their declaration and all
// instantiable members of the tree must be listed in vptr_list.h.
template<class type>
class __VPtr : public __TPtrBase
{
public:
    // VPtr::_Type has to be a pointer as
    // often the type is an abstract class.
    // This type is not expected to be used anyway.
    typedef type* _Type;
    typedef type* _Ptr;
    
    __VPtr< type >(void) : __TPtrBase() {}
    __VPtr< type >(TADDR addr) : __TPtrBase(addr) {}
    explicit __VPtr< type >(__TPtrBase addr)
    {
        m_addr = addr.GetAddr();
    }
    explicit __VPtr< type >(type* host)
    {
        m_addr = DacGetTargetAddrForHostAddr(host, true);
    }
    
    __VPtr< type >& operator=(const __TPtrBase& ptr)
    {
        m_addr = ptr.m_addr;
        return *this;
    }
    __VPtr< type >& operator=(TADDR addr)
    {
        m_addr = addr;
        return *this;
    }

    operator type*() const
    {
        return (type*)DacInstantiateClassByVTable(m_addr, true);
    }
    type* operator->() const
    {
        return (type*)DacInstantiateClassByVTable(m_addr, true);
    }
    
    bool operator==(const __VPtr< type >& ptr) const
    {
        return m_addr == ptr.m_addr;
    }
    bool operator==(TADDR addr) const
    {
        return m_addr == addr;
    }
    bool operator!=(const __VPtr< type >& ptr) const
    {
        return !operator==(ptr);
    }
    bool operator!=(TADDR addr) const
    {
        return m_addr != addr;
    }

    bool IsValid(void) const
    {
        return m_addr &&
            DacInstantiateClassByVTable(m_addr, false) != NULL;
    }
    void EnumMem(void) const
    {
        if (IsValid())
        {
            DacEnumMemoryRegion(m_addr, (operator->())->VPtrSize());
        }
    }
};

#define VPTR(type) __VPtr< type >

// Pointer wrapper for 8-bit strings.
template<typename type, ULONG32 maxChars = 32760>
class __Str8Ptr : public __DPtr<char>
{
public:
    typedef type _Type;
    typedef type* _Ptr;
    
    __Str8Ptr< type, maxChars >(void) : __DPtr<char>() {}
    __Str8Ptr< type, maxChars >(TADDR addr) : __DPtr<char>(addr) {}
    explicit __Str8Ptr< type, maxChars >(__TPtrBase addr)
    {
        m_addr = addr.GetAddr();
    }
    explicit __Str8Ptr< type, maxChars >(type* host)
    {
        m_addr = DacGetTargetAddrForHostAddr(host, true);
    }

    __Str8Ptr< type, maxChars >& operator=(const __TPtrBase& ptr)
    {
        m_addr = ptr.m_addr;
        return *this;
    }
    __Str8Ptr< type, maxChars >& operator=(TADDR addr)
    {
        m_addr = addr;
        return *this;
    }

    operator type*() const
    {
        return (type*)DacInstantiateStringA(m_addr, maxChars, true);
    }

    bool IsValid(void) const
    {
        return m_addr &&
            DacInstantiateStringA(m_addr, maxChars, false) != NULL;
    }
    void EnumMem(void) const
    {
        char* str = DacInstantiateStringA(m_addr, maxChars, false);
        if (str)
        {
            DacEnumMemoryRegion(m_addr, strlen(str) + 1);
        }
    }
};

#define S8PTR(type) __Str8Ptr< type >
#define S8PTRMAX(type, maxChars) __Str8Ptr< type, maxChars >

// Pointer wrapper for 16-bit strings.
template<typename type, ULONG32 maxChars = 32760>
class __Str16Ptr : public __DPtr<wchar_t>
{
public:
    typedef type _Type;
    typedef type* _Ptr;
    
    __Str16Ptr< type, maxChars >(void) : __DPtr<wchar_t>() {}
    __Str16Ptr< type, maxChars >(TADDR addr) : __DPtr<wchar_t>(addr) {}
    explicit __Str16Ptr< type, maxChars >(__TPtrBase addr)
    {
        m_addr = addr.GetAddr();
    }
    explicit __Str16Ptr< type, maxChars >(type* host)
    {
        m_addr = DacGetTargetAddrForHostAddr(host, true);
    }

    __Str16Ptr< type, maxChars >& operator=(const __TPtrBase& ptr)
    {
        m_addr = ptr.m_addr;
        return *this;
    }
    __Str16Ptr< type, maxChars >& operator=(TADDR addr)
    {
        m_addr = addr;
        return *this;
    }

    operator type*() const
    {
        return (type*)DacInstantiateStringW(m_addr, maxChars, true);
    }

    bool IsValid(void) const
    {
        return m_addr &&
            DacInstantiateStringW(m_addr, maxChars, false) != NULL;
    }
    void EnumMem(void) const
    {
        char* str = DacInstantiateStringW(m_addr, maxChars, false);
        if (str)
        {
            DacEnumMemoryRegion(m_addr, strlen(str) + 1);
        }
    }
};

#define S16PTR(type) __Str16Ptr< type >
#define S16PTRMAX(type, maxChars) __Str16Ptr< type, maxChars >

template<typename type>
class __GlobalVal
{
public:
    __GlobalVal< type >(PULONG rvaPtr)
    {
        m_rvaPtr = rvaPtr;
    }

    operator type() const
    {
        return (type)*__DPtr< type >(DacGlobalBase() + *m_rvaPtr);
    }

    __DPtr< type > operator&() const
    {
        return __DPtr< type >(DacGlobalBase() + *m_rvaPtr);
    }

    type & operator=(type & val)
    {
        type* ptr = __DPtr< type >(DacGlobalBase() + *m_rvaPtr);
        // Update the host copy;
        *ptr = val;
        // Write back to the target.
        DacWriteHostInstance(ptr, true);
        return val;
    }
    
    bool IsValid(void) const
    {
        return __DPtr< type >(DacGlobalBase() + *m_rvaPtr).IsValid();
    }
    void EnumMem(void) const
    {
        TADDR p = DacGlobalBase() + *m_rvaPtr;
        __DPtr< type >(p).EnumMem();
    }

private:
    PULONG m_rvaPtr;
};

template<typename type, size_t size>
class __GlobalArray
{
public:
    __GlobalArray< type, size >(PULONG rvaPtr)
    {
        m_rvaPtr = rvaPtr;
    }

    __DPtr< type > operator&() const
    {
        return __DPtr< type >(DacGlobalBase() + *m_rvaPtr);
    }
    
    type& operator[](unsigned int index) const
    {
        return __DPtr< type >(DacGlobalBase() + *m_rvaPtr)[index];
    }
    
    bool IsValid(void) const
    {
        // Only validates the base pointer, not the full array range.
        return __DPtr< type >(DacGlobalBase() + *m_rvaPtr).IsValid();
    }
    void EnumMem(void) const
    {
        DacEnumMemoryRegion(DacGlobalBase() + *m_rvaPtr, sizeof(type) * size);
    }

private:
    PULONG m_rvaPtr;
};

template<typename acc_type, typename store_type>
class __GlobalPtr
{
public:
    __GlobalPtr< acc_type, store_type >(PULONG rvaPtr)
    {
        m_rvaPtr = rvaPtr;
    }

    __DPtr< store_type > operator&() const
    {
        return __DPtr< store_type >(DacGlobalBase() + *m_rvaPtr);
    }
      
    store_type & operator=(store_type & val)
    {
        store_type* ptr = __DPtr< store_type >(DacGlobalBase() + *m_rvaPtr);
        // Update the host copy;
        *ptr = val;
        // Write back to the target.
        DacWriteHostInstance(ptr, true);
        return val;
    }
    
    acc_type operator->() const
    {
        return (acc_type)*__DPtr< store_type >(DacGlobalBase() + *m_rvaPtr);
    }
    operator acc_type() const
    {
        return (acc_type)*__DPtr< store_type >(DacGlobalBase() + *m_rvaPtr);
    }
    bool operator!() const
    {
        return !*__DPtr< store_type >(DacGlobalBase() + *m_rvaPtr);
    }

    typename store_type::_Type operator[](int index)
    {
        return (*__DPtr< store_type >(DacGlobalBase() + *m_rvaPtr))[index];
    }
    
    TADDR GetAddr() const
    {
        return (*__DPtr< store_type >(DacGlobalBase() + *m_rvaPtr)).GetAddr();
    }

    TADDR GetAddrRaw () const
    {
        return DacGlobalBase() + *m_rvaPtr;
    }
    
    // This is only testing the the pointer memory is available but does not verify
    // the memory that it points to.
    //
    bool IsValidPtr(void) const
    {
        return __DPtr< store_type >(DacGlobalBase() + *m_rvaPtr).IsValid();
    }
    
    bool IsValid(void) const
    {
        return __DPtr< store_type >(DacGlobalBase() + *m_rvaPtr).IsValid() &&
            (*__DPtr< store_type >(DacGlobalBase() + *m_rvaPtr)).IsValid();
    }
    void EnumMem(void) const
    {
        __DPtr< store_type > ptr(DacGlobalBase() + *m_rvaPtr);
        ptr.EnumMem();
        if (ptr.IsValid())
        {
            (*ptr).EnumMem();
        }
    }

    PULONG m_rvaPtr;
};

template<typename acc_type, typename store_type>
inline bool operator==(const __GlobalPtr<acc_type, store_type>& gptr,
                       acc_type host)
{
    return DacGetTargetAddrForHostAddr(host, true) ==
        *__DPtr< TADDR >(DacGlobalBase() + *gptr.m_rvaPtr);
}
template<typename acc_type, typename store_type>
inline bool operator!=(const __GlobalPtr<acc_type, store_type>& gptr,
                       acc_type host)
{
    return !operator==(gptr, host);
}

template<typename acc_type, typename store_type>
inline bool operator==(acc_type host,
                       const __GlobalPtr<acc_type, store_type>& gptr)
{
    return DacGetTargetAddrForHostAddr(host, true) ==
        *__DPtr< TADDR >(DacGlobalBase() + *gptr.m_rvaPtr);
}
template<typename acc_type, typename store_type>
inline bool operator!=(acc_type host,
                       const __GlobalPtr<acc_type, store_type>& gptr)
{
    return !operator==(host, gptr);
}

// The special empty ctor declared here allows the whole
// class hierarchy to be instantiated easily by the
// external access code.  The actual class body will be
// read externally so no members should be initialized.


#define VPTR_ANY_CLASS_METHODS(name)                            \
        virtual ULONG32 VPtrSize(void) { return sizeof(name); } \
        static PVOID VPtrHostVTable() {                         \
            name dummy((TADDR)0, (TADDR)0);                     \
            return *((PVOID*)&dummy); }



#define VPTR_CLASS_METHODS(name)                                \
        VPTR_ANY_CLASS_METHODS(name)                            \
        static TADDR VPtrTargetVTable() {                       \
            return DacGlobalBase() + g_dacGlobals.name##__vtAddr; }

#define VPTR_MULTI_CLASS_METHODS(name, keyBase)                 \
        VPTR_ANY_CLASS_METHODS(name)                            \
        static TADDR VPtrTargetVTable() {                       \
            return DacGlobalBase() + g_dacGlobals.name##__##keyBase##__mvtAddr; }

#define VPTR_VTABLE_CLASS(name, base)                           \
public: name(TADDR addr, TADDR vtAddr) : base(addr, vtAddr) {}  \
        VPTR_CLASS_METHODS(name)

#define VPTR_MULTI_VTABLE_CLASS(name, base)                     \
public: name(TADDR addr, TADDR vtAddr) : base(addr, vtAddr) {}  \
        VPTR_MULTI_CLASS_METHODS(name, base)

// Used for base classes that can be instantiated directly.
// The fake vfn is still used to force a vtable even when
// all the normal vfns are ifdef'ed out.
#define VPTR_BASE_CONCRETE_VTABLE_CLASS(name)                   \
public: name(TADDR addr, TADDR vtAddr) {}                       \
        VPTR_CLASS_METHODS(name)

#define VPTR_BASE_CONCRETE_VTABLE_CLASS_NO_CTOR_BODY(name)      \
public: name(TADDR addr, TADDR vtAddr);                         \
        VPTR_CLASS_METHODS(name)

// The pure virtual method forces all derivations to use
// VPTR_VTABLE_CLASS to compile.
#define VPTR_BASE_VTABLE_CLASS(name)                            \
public: name(TADDR addr, TADDR vtAddr) {}                       \
        virtual ULONG32 VPtrSize(void) = 0;

#define VPTR_BASE_VTABLE_CLASS_NO_CTOR_BODY(name)               \
public: name(TADDR addr, TADDR vtAddr);                         \
        virtual ULONG32 VPtrSize(void) = 0;

#define VPTR_ABSTRACT_VTABLE_CLASS(name, base)                  \
public: name(TADDR addr, TADDR vtAddr) : base(addr, vtAddr) {}

#define VPTR_ABSTRACT_VTABLE_CLASS_NO_CTOR_BODY(name, base)     \
public: name(TADDR addr, TADDR vtAddr);

// helper macro to make the vtables unique for DAC
#define VPTR_UNIQUE(unique)

// Safe access for retrieving the target address of a PTR.
#define PTR_TO_TADDR(ptr) ((ptr).GetAddr())

#define GFN_TADDR(name) (DacGlobalBase() + g_dacGlobals.fn__ ## name)

// ROTORTODO - g++ 3 doesn't like the use of the operator& in __GlobalVal
// here. Putting GVAL_ADDR in to get things to compile while I discuss
// this matter with the g++ authors.

#define GVAL_ADDR(g) \
    ((g).operator&())

//
// References to class static and global data.
// These all need to be redirected through the global
// data table.
//

#define _SPTR_DECL(acc_type, store_type, var) \
    static __GlobalPtr< acc_type, store_type > var
#define _SPTR_IMPL(acc_type, store_type, cls, var) \
    __GlobalPtr< acc_type, store_type > cls::var(&g_dacGlobals.cls##__##var)
#define _SPTR_IMPL_INIT(acc_type, store_type, cls, var, init) \
    __GlobalPtr< acc_type, store_type > cls::var(&g_dacGlobals.cls##__##var)

#define _GPTR_DECL(acc_type, store_type, var) \
    extern __GlobalPtr< acc_type, store_type > var
#define _GPTR_IMPL(acc_type, store_type, var) \
    __GlobalPtr< acc_type, store_type > var(&g_dacGlobals.dac__##var)
#define _GPTR_IMPL_INIT(acc_type, store_type, var, init) \
    __GlobalPtr< acc_type, store_type > var(&g_dacGlobals.dac__##var)

#define SVAL_DECL(type, var) \
    static __GlobalVal< type > var
#define SVAL_IMPL(type, cls, var) \
    __GlobalVal< type > cls::var(&g_dacGlobals.cls##__##var)
#define SVAL_IMPL_INIT(type, cls, var, init) \
    __GlobalVal< type > cls::var(&g_dacGlobals.cls##__##var)

#define GVAL_DECL(type, var) \
    extern __GlobalVal< type > var
#define GVAL_IMPL(type, var) \
    __GlobalVal< type > var(&g_dacGlobals.dac__##var)
#define GVAL_IMPL_INIT(type, var, init) \
    __GlobalVal< type > var(&g_dacGlobals.dac__##var)

#define GARY_DECL(type, var, size) \
    extern __GlobalArray< type, size > var
#define GARY_IMPL(type, var, size) \
    __GlobalArray< type, size > var(&g_dacGlobals.dac__##var)

// Translation from a host pointer back to the target address
// that was used to retrieve the data for the host pointer.
#define PTR_HOST_TO_TADDR(host) DacGetTargetAddrForHostAddr(host, true)
// Translation from a host vtable pointer to a target vtable pointer.
#define VPTR_HOST_VTABLE_TO_TADDR(host) DacGetTargetVtForHostVt(host, true)

// Construct a pointer to a member of the given type.
#define PTR_HOST_MEMBER_TADDR(type, host, memb) \
    (PTR_HOST_TO_TADDR(host) + (TADDR)offsetof(type, memb))

// Constructs an arbitrary data instance for a piece of
// memory in the target.
#define PTR_READ(addr, size) \
    DacInstantiateTypeByAddress(addr, size, true)

#define PTR_NULL __TPtrBase((TADDR)0)

#define PTR_VOLATILE

// Provides an empty method implementation when compiled
// for DACCESS_COMPILE.  For example, use to stub out methods needed
// for vtable entries but otherwise unused.
#define DAC_EMPTY() { LEAF_CONTRACT; }
#define DAC_EMPTY_ERR() { LEAF_CONTRACT; DacError(E_UNEXPECTED); }
#define DAC_EMPTY_RET(retVal) { LEAF_CONTRACT; DacError(E_UNEXPECTED); return retVal; }

#endif // #ifdef __cplusplus

#else // #ifdef DACCESS_COMPILE

//
// This version of the macros turns into normal pointers
// for unmodified in-proc compilation.
//

// Declare TADDR as a non-pointer type so that arithmetic
// can be done on it directly, as with the DACCESS_COMPILE definition.
// This also helps expose pointer usage that may need to be changed.
typedef ULONG_PTR TADDR;
    
#define DPTR(type) type*
#define SPTR(type) type*
#define VPTR(type) type*
#define S8PTR(type) type*
#define S8PTRMAX(type, maxChars) type*
#define S16PTR(type) type*
#define S16PTRMAX(type, maxChars) type*
#define VPTR_VTABLE_CLASS(name, base)
#define VPTR_MULTI_VTABLE_CLASS(name, base)
#define VPTR_BASE_CONCRETE_VTABLE_CLASS(name)
#define VPTR_BASE_VTABLE_CLASS(name)
#define VPTR_ABSTRACT_VTABLE_CLASS(name, base)

// helper macro to make the vtables unique for DAC
#define VPTR_UNIQUE(unique) virtual int MakeVTableUniqueForDAC() {    STATIC_CONTRACT_SO_TOLERANT; return unique; }
#define VPTR_UNIQUE_BaseDomain                          (100000)
#define VPTR_UNIQUE_SystemDomain                        (VPTR_UNIQUE_BaseDomain + 1)
#define VPTR_UNIQUE_NDirectMethodFrameStandalone        (VPTR_UNIQUE_SystemDomain + 1)
#define VPTR_UNIQUE_NDirectMethodFrameStandaloneCleanup (VPTR_UNIQUE_NDirectMethodFrameStandalone + 1)

#define PTR_TO_TADDR(ptr) ((TADDR)(ptr))
#define GFN_TADDR(name) ((TADDR)&(name))

#define GVAL_ADDR(g) (&(g))
#define _SPTR_DECL(acc_type, store_type, var) \
    static store_type var
#define _SPTR_IMPL(acc_type, store_type, cls, var) \
    store_type cls::var
#define _SPTR_IMPL_INIT(acc_type, store_type, cls, var, init) \
    store_type cls::var = init
#define _GPTR_DECL(acc_type, store_type, var) \
    extern store_type var
#define _GPTR_IMPL(acc_type, store_type, var) \
    store_type var
#define _GPTR_IMPL_INIT(acc_type, store_type, var, init) \
    store_type var = init
#define SVAL_DECL(type, var) \
    static type var
#define SVAL_IMPL(type, cls, var) \
    type cls::var
#define SVAL_IMPL_INIT(type, cls, var, init) \
    type cls::var = init
#define GVAL_DECL(type, var) \
    extern type var
#define GVAL_IMPL(type, var) \
    type var
#define GVAL_IMPL_INIT(type, var, init) \
    type var = init
#define GARY_DECL(type, var, size) \
    extern type var[size]
#define GARY_IMPL(type, var, size) \
    type var[size]
#define PTR_HOST_TO_TADDR(host) ((TADDR)(host))
#define VPTR_HOST_VTABLE_TO_TADDR(host) ((TADDR)(host))
#define PTR_HOST_MEMBER_TADDR(type, host, memb) ((TADDR)&(host)->memb)
#define PTR_READ(addr, size) ((PVOID)(addr))

#define PTR_NULL NULL

#define PTR_VOLATILE volatile

#define DAC_EMPTY()
#define DAC_EMPTY_ERR()
#define DAC_EMPTY_RET(retVal)

#endif // #ifdef DACCESS_COMPILE

//----------------------------------------------------------------------------
//
// Convenience macros which work for either mode.
//
//----------------------------------------------------------------------------

#define SPTR_DECL(type, var) _SPTR_DECL(type*, PTR_##type, var)
#define SPTR_IMPL(type, cls, var) _SPTR_IMPL(type*, PTR_##type, cls, var)
#define SPTR_IMPL_INIT(type, cls, var, init) _SPTR_IMPL_INIT(type*, PTR_##type, cls, var, init)
#define GPTR_DECL(type, var) _GPTR_DECL(type*, PTR_##type, var)
#define GPTR_IMPL(type, var) _GPTR_IMPL(type*, PTR_##type, var)
#define GPTR_IMPL_INIT(type, var, init) _GPTR_IMPL_INIT(type*, PTR_##type, var, init)

//----------------------------------------------------------------------------
//
// Forward typedefs for system types.  This is a convenient place
// to declare things for system types, plus it gives us a central
// place to look at when deciding what types may cause issues for
// cross-platform compilation.
//
//----------------------------------------------------------------------------

typedef DPTR(BYTE)    PTR_BYTE;
typedef DPTR(signed char) PTR_SBYTE;
typedef DPTR(const BYTE) PTR_CBYTE;
typedef DPTR(WORD)    PTR_WORD;
typedef DPTR(USHORT)  PTR_USHORT;
typedef DPTR(DWORD)   PTR_DWORD;
typedef DPTR(ULONG)   PTR_ULONG;
typedef DPTR(INT32)   PTR_INT32;
typedef DPTR(UINT32)  PTR_UINT32;
typedef DPTR(ULONG64) PTR_ULONG64;
typedef DPTR(INT64)   PTR_INT64;
typedef DPTR(UINT64)  PTR_UINT64;
typedef DPTR(SIZE_T)  PTR_SIZE_T;
typedef DPTR(size_t)  PTR_size_t;
typedef DPTR(TADDR)   PTR_TADDR;
typedef DPTR(int)     PTR_int;
typedef DPTR(unsigned) PTR_unsigned;

typedef S8PTR(char)           PTR_STR;
typedef S8PTR(const char)     PTR_CSTR;
typedef S8PTR(char)           PTR_UTF8;
typedef S8PTR(const char)     PTR_CUTF8;
typedef S16PTR(wchar_t)       PTR_WSTR;
typedef S16PTR(const wchar_t) PTR_CWSTR;

typedef DPTR(struct _CONTEXT)            PTR_CONTEXT;
typedef DPTR(struct _EXCEPTION_POINTERS) PTR_EXCEPTION_POINTERS;
typedef DPTR(struct _EXCEPTION_RECORD)   PTR_EXCEPTION_RECORD;

typedef DPTR(struct IMAGE_COR_VTABLEFIXUP) PTR_IMAGE_COR_VTABLEFIXUP;
typedef DPTR(IMAGE_DATA_DIRECTORY)  PTR_IMAGE_DATA_DIRECTORY;
typedef DPTR(IMAGE_DEBUG_DIRECTORY)  PTR_IMAGE_DEBUG_DIRECTORY;
typedef DPTR(IMAGE_DOS_HEADER)      PTR_IMAGE_DOS_HEADER;
typedef DPTR(IMAGE_NT_HEADERS)      PTR_IMAGE_NT_HEADERS;
typedef DPTR(IMAGE_NT_HEADERS32)    PTR_IMAGE_NT_HEADERS32;
typedef DPTR(IMAGE_NT_HEADERS64)    PTR_IMAGE_NT_HEADERS64;
typedef DPTR(IMAGE_COR20_HEADER)    PTR_IMAGE_COR20_HEADER;
typedef DPTR(IMAGE_SECTION_HEADER)  PTR_IMAGE_SECTION_HEADER;
typedef DPTR(IMAGE_TLS_DIRECTORY)   PTR_IMAGE_TLS_DIRECTORY;

#if defined(DACCESS_COMPILE)
#include <corhdr.h>
#include <clrdata.h>
#include <xclrdata.h>
#endif


//----------------------------------------------------------------------------
//
// The access code compile must compile data structures that exactly
// match the real structures for access to work.  The access code
// doesn't want all of the debugging validation code, though, so
// distinguish between _DEBUG, for declaring general debugging data
// and always-on debug code, and _DEBUG_IMPL, for debugging code
// which will be disabled when compiling for external access.
//
//----------------------------------------------------------------------------

#if !defined(_DEBUG_IMPL) && defined(_DEBUG) && !defined(DACCESS_COMPILE)
#define _DEBUG_IMPL 1
#endif

// Helper macro for tracking EnumMemoryRegions progress.
#define EMEM_OUT(args)

#endif // #ifndef __daccess_h__
