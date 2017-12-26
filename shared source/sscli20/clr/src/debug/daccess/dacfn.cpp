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
// File: dacfn.cpp
//
// Dac function implementations.
//
//*****************************************************************************

#include "stdafx.h"

#include <encee.h>
#include <remoting.h>
#include <virtualcallstub.h>
#include "codeman.inl"
#include "peimagelayout.inl"

DacGlobals g_dacGlobals;

struct DacHostVtPtrs
{
#define VPTR_CLASS(name) PVOID name;
#define VPTR_MULTI_CLASS(name, keyBase) PVOID name##__##keyBase;
#include <vptr_list.h>
#undef VPTR_CLASS
#undef VPTR_MULTI_CLASS
};


WCHAR *g_dacVtStrings[] =
{
#define VPTR_CLASS(name) L###name,
#define VPTR_MULTI_CLASS(name, keyBase) L###name,
#include <vptr_list.h>
#undef VPTR_CLASS
#undef VPTR_MULTI_CLASS
};

DacHostVtPtrs g_dacHostVtPtrs;

HRESULT
DacGetHostVtPtrs(void)
{
#define VPTR_CLASS(name) \
    g_dacHostVtPtrs.name = name::VPtrHostVTable();
#define VPTR_MULTI_CLASS(name, keyBase) \
    g_dacHostVtPtrs.name##__##keyBase = name::VPtrHostVTable();
#include <vptr_list.h>
#undef VPTR_CLASS
#undef VPTR_MULTI_CLASS

    return S_OK;
}

bool
DacExceptionFilter(Exception* ex, ClrDataAccess* access,
                   HRESULT* status)
{
    // The DAC support functions throw HRExceptions and
    // the underlying code can throw the normal set of
    // CLR exceptions.  Handle any exception
    // other than an unexpected SEH exception.
    // If we're not debugging, handle SEH exceptions also
    // so that dac absorbs all exceptions by default.
    if ((access && access->m_debugMode) &&
        ex->IsType(SEHException::GetType()))
    {
        // Indicate this exception should be rethrown.
        return FALSE;
    }

    *status = ex->GetHR();
    if (!FAILED(*status))
    {
        *status = E_FAIL;
    }
    return TRUE;
}

void __cdecl
DacWarning(__in char* format, ...)
{
    char text[256];
    va_list args;

    va_start(args, format);
    _vsnprintf_s(text, sizeof(text), _TRUNCATE, format, args);
    text[sizeof(text) - 1] = 0;
    va_end(args);
    OutputDebugStringA(text);
}

void
DacNotImpl(void)
{
    EX_THROW(HRException, (E_NOTIMPL));
}

void
DacError(HRESULT err)
{
    EX_THROW(HRException, (err));
}

TADDR
DacGlobalBase(void)
{
    if (!g_dacImpl)
    {
        DacError(E_UNEXPECTED);
        UNREACHABLE();
    }

    return g_dacImpl->m_globalBase;
}

HRESULT
DacReadAll(TADDR addr, PVOID buffer, ULONG32 size, bool throwEx)
{
    if (!g_dacImpl)
    {
        DacError(E_UNEXPECTED);
        UNREACHABLE();
    }

    HRESULT status;
    ULONG32 returned;

    
    status = g_dacImpl->m_target->
        ReadVirtual(TO_CDADDR(addr), (PBYTE)buffer, size, &returned);

    
    if (status != S_OK)
    {
        if (throwEx)
        {
            DacError(status);
        }
        return status;
    }
    if (returned != size)
    {
        if (throwEx)
        {
            DacError(HRESULT_FROM_WIN32(ERROR_PARTIAL_COPY));
        }
        return HRESULT_FROM_WIN32(ERROR_PARTIAL_COPY);
    }

    return S_OK;
}

HRESULT
DacWriteAll(TADDR addr, PVOID buffer, ULONG32 size, bool throwEx)
{
    if (!g_dacImpl)
    {
        DacError(E_UNEXPECTED);
        UNREACHABLE();
    }

    HRESULT status;
    ULONG32 returned;

    status = g_dacImpl->m_target->
        WriteVirtual(TO_CDADDR(addr), (PBYTE)buffer, size, &returned);
    if (status != S_OK)
    {
        if (throwEx)
        {
            DacError(status);
        }
        return status;
    }
    if (returned != size)
    {
        if (throwEx)
        {
            DacError(HRESULT_FROM_WIN32(ERROR_PARTIAL_COPY));
        }
        return HRESULT_FROM_WIN32(ERROR_PARTIAL_COPY);
    }

    return S_OK;
}

HRESULT
DacAllocVirtual(TADDR addr, ULONG32 size,
                ULONG32 typeFlags, ULONG32 protectFlags,
                bool throwEx, TADDR* mem)
{
    if (!g_dacImpl ||
        !g_dacImpl->m_target2)
    {
        DacError(E_UNEXPECTED);
        UNREACHABLE();
    }

    CLRDATA_ADDRESS cdMem;
    HRESULT status = g_dacImpl->m_target2->
        AllocVirtual(TO_CDADDR(addr), size, typeFlags, protectFlags,
                     &cdMem);
    if (status != S_OK)
    {
        if (throwEx)
        {
            DacError(status);
            UNREACHABLE();
        }

        return status;
    }

    *mem = TO_TADDR(cdMem);
    return S_OK;
}

HRESULT
DacFreeVirtual(TADDR mem, ULONG32 size, ULONG32 typeFlags,
               bool throwEx)
{
    if (!g_dacImpl ||
        !g_dacImpl->m_target2)
    {
        DacError(E_UNEXPECTED);
        UNREACHABLE();
    }

    HRESULT status = g_dacImpl->m_target2->
        FreeVirtual(TO_CDADDR(mem), size, typeFlags);

    if (status != S_OK && throwEx)
    {
        DacError(status);
        UNREACHABLE();
    }

    return status;
}

PVOID
DacInstantiateTypeByAddressHelper(TADDR addr, ULONG32 size, bool throwEx, bool fReport)
{

    if (!g_dacImpl)
    {
        DacError(E_UNEXPECTED);
        UNREACHABLE();
    }

    // Preserve special pointer values.
    if (!addr || addr == (TADDR)-1)
    {
        return (PVOID)addr;
    }

    //
    // Check the cache for an existing DPTR instance.
    // It's possible that a previous access may have been
    // smaller than the current access, so we have to
    // allow an existing instance to be superseded.
    //

    DAC_INSTANCE* inst = g_dacImpl->m_instances.Find(addr);
    DAC_INSTANCE* oldInst = NULL;
    if (inst)
    {
        // If the existing instance is large enough we
        // can reuse it, otherwise we need to promote.
        // We cannot promote a VPTR as the VPTR data
        // has been updated with a host vtable and we
        // don't want to lose that.  This shouldn't
        // happen anyway.
        if (inst->size >= size)
        {
            return inst + 1;
        }
        else
        {
            // Existing instance is too small and must
            // be superseded.
            if (inst->usage == DAC_VPTR)
            {
                // DPTR/VPTR usage conflict.
                DacError(E_INVALIDARG);
                UNREACHABLE();
            }

            // Promote the larger instance into the hash
            // in place of the smaller, but keep the
            // smaller instance around in case code still
            // has a pointer to it. But ensure that we can
            // create the larger instance and add it to the
            // hash table before removing the old one.
            oldInst = inst;
        }
    }

    inst = g_dacImpl->m_instances.Alloc(addr, size, DAC_DPTR);
    if (!inst)
    {
        DacError(E_OUTOFMEMORY);
        UNREACHABLE();
    }

    if (fReport == false)
    {
        // mark the bit if necessary
        inst->noReport = 1;
    }
    else
    {
        // clear the bit
        inst->noReport = 0;
    }
    HRESULT status = DacReadAll(addr, inst + 1, size, false);
    if (status != S_OK)
    {
        g_dacImpl->m_instances.ReturnAlloc(inst);
        if (throwEx)
        {
            DacError(status);
        }
        return NULL;
    }

    if (!g_dacImpl->m_instances.Add(inst))
    {
        g_dacImpl->m_instances.ReturnAlloc(inst);
        DacError(E_OUTOFMEMORY);
        UNREACHABLE();        
    }

    if (oldInst)
    {
        g_dacImpl->m_instances.Supersede(oldInst);
    }

    return inst + 1;

}

PVOID   DacInstantiateTypeByAddress(TADDR addr, ULONG32 size, bool throwEx)
{
    return DacInstantiateTypeByAddressHelper(addr, size, throwEx, true);
}

PVOID   DacInstantiateTypeByAddressNoReport(TADDR addr, ULONG32 size, bool throwEx)
{
    return DacInstantiateTypeByAddressHelper(addr, size, throwEx, false);
}


PVOID
DacInstantiateClassByVTable(TADDR addr, bool throwEx)
{

    if (!g_dacImpl)
    {
        DacError(E_UNEXPECTED);
        UNREACHABLE();
    }

    // Preserve special pointer values.
    if (!addr || addr == (TADDR)-1)
    {
        return (PVOID)addr;
    }

    //
    // Check the cache for an existing VPTR instance.
    // If there is an instance we assume that it's
    // the right object.
    //

    DAC_INSTANCE* inst = g_dacImpl->m_instances.Find(addr);
    DAC_INSTANCE* oldInst = NULL;
    if (inst)
    {
        // If the existing instance is a VPTR we can
        // reuse it, otherwise we need to promote.
        if (inst->usage == DAC_VPTR)
        {
            return inst + 1;
        }
        else
        {
            // Existing instance is not a match and must
            // be superseded.
            // Promote the new instance into the hash
            // in place of the old, but keep the
            // old instance around in case code still
            // has a pointer to it. But ensure that we can
            // create the larger instance and add it to the
            // hash table before removing the old one.
            oldInst = inst;
        }
    }

    HRESULT status;
    TADDR vtAddr;
    ULONG32 size;
    PVOID hostVtPtr;

    // Read the vtable pointer to get the actual
    // implementation class identity.
    if ((status = DacReadAll(addr, &vtAddr, sizeof(vtAddr), throwEx)) != S_OK)
    {
        return NULL;
    }

    //
    // Instantiate the right class, using the vtable as
    // class identity.
    //

#define VPTR_CLASS(name)                       \
    if (vtAddr == g_dacImpl->m_globalBase +    \
        g_dacGlobals.name##__vtAddr)           \
    {                                          \
        size = sizeof(name);                   \
        hostVtPtr = g_dacHostVtPtrs.name;      \
    }                                          \
    else
#define VPTR_MULTI_CLASS(name, keyBase)        \
    if (vtAddr == g_dacImpl->m_globalBase +    \
        g_dacGlobals.name##__##keyBase##__mvtAddr) \
    {                                          \
        size = sizeof(name);                   \
        hostVtPtr = g_dacHostVtPtrs.name##__##keyBase; \
    }                                          \
    else
#include <vptr_list.h>
#undef VPTR_CLASS
#undef VPTR_MULTI_CLASS

    {
        // Can't identify the vtable pointer.
        if (throwEx)
        {
            DacError(E_INVALIDARG);
        }
        return NULL;
    }

    inst = g_dacImpl->m_instances.Alloc(addr, size, DAC_VPTR);
    if (!inst)
    {
        DacError(E_OUTOFMEMORY);
        UNREACHABLE();
    }

    if ((status = DacReadAll(addr, inst + 1, size, false)) != S_OK)
    {
        g_dacImpl->m_instances.ReturnAlloc(inst);
        if (throwEx)
        {
            DacError(status);
        }
        return NULL;
    }

    // We now have a proper target object with a target
    // vtable.  We need to patch the vtable to the appropriate
    // host vtable so that the virtual functions can be
    // called in the host process.
    *(PVOID*)(inst + 1) = hostVtPtr;

    if (!g_dacImpl->m_instances.Add(inst))
    {
        g_dacImpl->m_instances.ReturnAlloc(inst);
        DacError(E_OUTOFMEMORY);
        UNREACHABLE();        
    }

    if (oldInst)
    {
        g_dacImpl->m_instances.Supersede(oldInst);
    }
    return inst + 1;

}

#define LOCAL_STR_BUF 256

PSTR
DacInstantiateStringA(TADDR addr, ULONG32 maxChars, bool throwEx)
{

    HRESULT status;

    if (!g_dacImpl)
    {
        DacError(E_UNEXPECTED);
        UNREACHABLE();
    }

    // Preserve special pointer values.
    if (!addr || addr == (TADDR)-1)
    {
        return (PSTR)addr;
    }

    //
    // Look for an existing string instance.
    //

    DAC_INSTANCE* inst = g_dacImpl->m_instances.Find(addr);
    if (inst && inst->usage == DAC_STRA)
    {
        return (PSTR)(inst + 1);
    }

    //
    // Determine the length of the string
    // by iteratively reading blocks and scanning them
    // for a terminator.
    //

    char buf[LOCAL_STR_BUF];
    TADDR scanAddr = addr;
    ULONG32 curBytes = 0;
    ULONG32 returned;

    for (;;)
    {
        status = g_dacImpl->m_target->
            ReadVirtual(TO_CDADDR(scanAddr), (PBYTE)buf, sizeof(buf),
                        &returned);
        if (status != S_OK)
        {
            // We hit invalid memory before finding a terminator.
            if (throwEx)
            {
                DacError(status);
            }
            return NULL;
        }

        PSTR scan = (PSTR)buf;
        PSTR scanEnd = scan + (returned / sizeof(*scan));
        while (scan < scanEnd)
        {
            if (!*scan)
            {
                break;
            }

            scan++;
        }

        if (!*scan)
        {
            // Found a terminator.
            scanAddr += ((scan + 1) - buf) * sizeof(*scan);
            break;
        }

        // Ignore any partial character reads.  The character
        // will be reread on the next loop if necessary.
        returned &= ~(sizeof(buf[0]) - 1);

        // The assumption is that a memory read cannot wrap
        // around the address space, thus if we have read to
        // the top of memory scanAddr cannot wrap farther
        // than to zero.
        curBytes += returned;
        scanAddr += returned;

        if (!scanAddr ||
            (curBytes + sizeof(buf[0]) - 1) / sizeof(buf[0]) >= maxChars)
        {
            // Wrapped around the top of memory or
            // we didn't find a terminator within the given bound.
            if (throwEx)
            {
                DacError(E_INVALIDARG);
            }
            return NULL;
        }
    }

    // Now that we know the length we can create a
    // host copy of the string.
    PSTR retVal = (PSTR)
        DacInstantiateTypeByAddress(addr, (ULONG32)(scanAddr - addr), throwEx);
    if (retVal &&
        (inst = g_dacImpl->m_instances.Find(addr)))
    {
        inst->usage = DAC_STRA;
    }
    return retVal;

}

PWSTR
DacInstantiateStringW(TADDR addr, ULONG32 maxChars, bool throwEx)
{

    HRESULT status;

    if (!g_dacImpl)
    {
        DacError(E_UNEXPECTED);
        UNREACHABLE();
    }

    // Preserve special pointer values.
    if (!addr || addr == (TADDR)-1)
    {
        return (PWSTR)addr;
    }

    //
    // Look for an existing string instance.
    //

    DAC_INSTANCE* inst = g_dacImpl->m_instances.Find(addr);
    if (inst && inst->usage == DAC_STRW)
    {
        return (PWSTR)(inst + 1);
    }

    //
    // Determine the length of the string
    // by iteratively reading blocks and scanning them
    // for a terminator.
    //

    WCHAR buf[LOCAL_STR_BUF];
    TADDR scanAddr = addr;
    ULONG32 curBytes = 0;
    ULONG32 returned;

    for (;;)
    {
        status = g_dacImpl->m_target->
            ReadVirtual(TO_CDADDR(scanAddr), (PBYTE)buf, sizeof(buf),
                        &returned);
        if (status != S_OK)
        {
            // We hit invalid memory before finding a terminator.
            if (throwEx)
            {
                DacError(status);
            }
            return NULL;
        }

        PWSTR scan = (PWSTR)buf;
        PWSTR scanEnd = scan + (returned / sizeof(*scan));
        while (scan < scanEnd)
        {
            if (!*scan)
            {
                break;
            }

            scan++;
        }

        if (!*scan)
        {
            // Found a terminator.
            scanAddr += ((scan + 1) - buf) * sizeof(*scan);
            break;
        }

        // Ignore any partial character reads.  The character
        // will be reread on the next loop if necessary.
        returned &= ~(sizeof(buf[0]) - 1);

        // The assumption is that a memory read cannot wrap
        // around the address space, thus if we have read to
        // the top of memory scanAddr cannot wrap farther
        // than to zero.
        curBytes += returned;
        scanAddr += returned;

        if (!scanAddr ||
            (curBytes + sizeof(buf[0]) - 1) / sizeof(buf[0]) >= maxChars)
        {
            // Wrapped around the top of memory or
            // we didn't find a terminator within the given bound.
            if (throwEx)
            {
                DacError(E_INVALIDARG);
            }
            return NULL;
        }
    }

    // Now that we know the length we can create a
    // host copy of the string.
    PWSTR retVal = (PWSTR)
        DacInstantiateTypeByAddress(addr, (ULONG32)(scanAddr - addr), throwEx);
    if (retVal &&
        (inst = g_dacImpl->m_instances.Find(addr)))
    {
        inst->usage = DAC_STRW;
    }
    return retVal;

}

TADDR
DacGetTargetAddrForHostAddr(LPCVOID ptr, bool throwEx)
{

    // Preserve special pointer values.
    if (ptr == NULL || ((TADDR) ptr == (TADDR)-1))
    {
        return 0;
    }
    else
    {
        TADDR addr = 0;
        HRESULT status = E_FAIL;

        EX_TRY
        {
            DAC_INSTANCE* inst = (DAC_INSTANCE*)ptr - 1;
            if (inst->sig == DAC_INSTANCE_SIG)
            {
                addr = inst->addr;
                status = S_OK;
            }
            else
            {
                status = E_INVALIDARG;
            }
        }
        EX_CATCH
        {
            status = E_INVALIDARG;
        }
        EX_END_CATCH(SwallowAllExceptions)

        if (status != S_OK)
        {
            if (g_dacImpl && g_dacImpl->m_debugMode)
            {
                DebugBreak();
            }

            if (throwEx)
            {
                DacError(status);
            }
        }

        return addr;
    }

}

PWSTR    DacGetVtNameW(TADDR targetVtable)
{
    PWSTR pszRet = NULL;

    ULONG *targ = &g_dacGlobals.Thread__vtAddr;
    ULONG *targStart = targ;
    for (ULONG i = 0; i < sizeof(g_dacHostVtPtrs) / sizeof(PVOID); i++)
    {
        if (targetVtable == (*targ + DacGlobalBase()))
        {
            pszRet = *(g_dacVtStrings + (targ - targStart));
        }

        targ++;
    }
    return pszRet;
}

TADDR
DacGetTargetVtForHostVt(LPCVOID vtHost, bool throwEx)
{
    PVOID* host;
    ULONG* targ;
    ULONG i;

    // The host vtable table exactly parallels the
    // target vtable table, so just iterate to a match
    // return the matching entry.
    host = &g_dacHostVtPtrs.Thread;
    targ = &g_dacGlobals.Thread__vtAddr;
    for (i = 0; i < sizeof(g_dacHostVtPtrs) / sizeof(PVOID); i++)
    {
        if (*host == vtHost)
        {
            return *targ + DacGlobalBase();
        }

        host++;
        targ++;
    }

    if (throwEx)
    {
        DacError(E_INVALIDARG);
    }
    return 0;
}

void
DacEnumMemoryRegion(TADDR addr, ULONG32 size)
{
    if (!g_dacImpl)
    {
        DacError(E_UNEXPECTED);
        UNREACHABLE();
    }

    g_dacImpl->ReportMem(addr, size);
}

HRESULT
DacWriteHostInstance(PVOID host, bool throwEx)
{
    if (!g_dacImpl)
    {
        DacError(E_UNEXPECTED);
        UNREACHABLE();
    }

    TADDR addr = DacGetTargetAddrForHostAddr(host, throwEx);
    if (!addr)
    {
        return S_OK;
    }

    DAC_INSTANCE* inst = (DAC_INSTANCE*)host - 1;
    return g_dacImpl->m_instances.Write(inst, throwEx);
}

HRESULT
DacPushInstanceState(bool throwEx)
{
    if (!g_dacImpl)
    {
        DacError(E_UNEXPECTED);
        UNREACHABLE();
    }

    if (!g_dacImpl->m_instances.PushState())
    {
        if (throwEx)
        {
            DacError(E_OUTOFMEMORY);
        }
        return E_OUTOFMEMORY;
    }

    return S_OK;
}

void
DacPopInstanceState(void)
{
    if (!g_dacImpl)
    {
        DacError(E_UNEXPECTED);
        UNREACHABLE();
    }

    g_dacImpl->m_instances.PopState();
}

bool
DacHostPtrHasEnumMark(LPCVOID host)
{
    if (!DacGetTargetAddrForHostAddr(host, false))
    {
        // Make it easy to ignore invalid pointers when enumerating.
        return true;
    }

    DAC_INSTANCE* inst = ((DAC_INSTANCE*)host) - 1;
    bool marked = inst->enumMem ? true : false;
    inst->enumMem = true;
    return marked;
}

bool
DacHasMethodDescBeenEnumerated(LPCVOID pMD)
{
    if (!DacGetTargetAddrForHostAddr(pMD, false))
    {
        // Make it easy to ignore invalid pointers when enumerating.
        return true;
    }

    DAC_INSTANCE* inst = ((DAC_INSTANCE*) pMD) - 1;
    bool MDEnumed = inst->MDEnumed ? true : false;
    return MDEnumed;
}

bool
DacSetMethodDescEnumerated(LPCVOID pMD)
{
    if (!DacGetTargetAddrForHostAddr(pMD, false))
    {
        // Make it easy to ignore invalid pointers when enumerating.
        return true;
    }

    DAC_INSTANCE* inst = ((DAC_INSTANCE*) pMD) - 1;
    bool MDEnumed = inst->MDEnumed ? true : false;
    inst->MDEnumed = true;
    return MDEnumed;
}

IMDInternalImport*
DacGetMDImport(const PEFile* peFile, bool throwEx)
{
    if (!g_dacImpl)
    {
        DacError(E_UNEXPECTED);
        UNREACHABLE();
    }

    return g_dacImpl->GetMDImport(peFile, throwEx);
}

int
DacGetIlMethodSize(TADDR methAddr)
{
    COR_ILMETHOD_TINY* tiny = DPTR(COR_ILMETHOD_TINY)(methAddr);

    if (tiny->IsTiny())
    {
        return (int)(tiny->GetCode() + tiny->GetCodeSize() - (BYTE*)tiny);
    }

    COR_ILMETHOD_FAT* fat = DPTR(COR_ILMETHOD_FAT)(methAddr);

    if (fat->IsFat())
    {
        TADDR lastAddr;

        const COR_ILMETHOD_SECT* sect = fat->GetSect();
        if (sect)
        {
            lastAddr = methAddr + (TADDR)((BYTE*)sect - (BYTE*)fat);

            do
            {
                sect = DPTR(COR_ILMETHOD_SECT)(lastAddr);
                const COR_ILMETHOD_SECT* nextSect = sect->NextLoc();
                lastAddr = PTR_HOST_TO_TADDR(sect) +
                    (TADDR)((BYTE*)nextSect - (BYTE*)sect);
            }
            while (sect->More());
        }
        else
        {
            lastAddr = methAddr + (TADDR)
                (fat->GetCode() + fat->GetCodeSize() - (BYTE*)fat);
        }

        return lastAddr - methAddr;
    }

    DacError(E_INVALIDARG);
    return 0;
}

COR_ILMETHOD*
DacGetIlMethod(TADDR methAddr)
{
    return (COR_ILMETHOD*)
        DacInstantiateTypeByAddress(methAddr, DacGetIlMethodSize(methAddr),
                                    true);
}

PVOID
DacAllocHostOnlyInstance(ULONG32 size, bool throwEx)
{
    if (!g_dacImpl)
    {
        DacError(E_UNEXPECTED);
        UNREACHABLE();
    }

    DAC_INSTANCE* inst = g_dacImpl->m_instances.Alloc(0, size, DAC_DPTR);
    if (!inst)
    {
        DacError(E_OUTOFMEMORY);
        UNREACHABLE();
    }

    g_dacImpl->m_instances.AddSuperseded(inst);

    return inst + 1;
}
