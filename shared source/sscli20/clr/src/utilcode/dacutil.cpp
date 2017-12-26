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
// Internal data access functionality.
//
//*****************************************************************************

#include "stdafx.h"

#include <winwrap.h>
#include <utilcode.h>
#include <dacprivate.h>
#include <ipcmanagerinterface.h>

//----------------------------------------------------------------------------
//
// LiveProcDataTarget.
//
//----------------------------------------------------------------------------

LiveProcDataTarget::LiveProcDataTarget(HANDLE process,
                                       DWORD processId)
{
    m_process = process;
    m_processId = processId;
}

STDMETHODIMP
LiveProcDataTarget::QueryInterface(
    THIS_
    IN REFIID InterfaceId,
    OUT PVOID* Interface
    )
{
    if (InterfaceId == IID_IUnknown ||
        InterfaceId == IID_ICLRDataTarget)
    {
        *Interface = (ICLRDataTarget*)this;
        // No need to refcount as this class is contained.
        return S_OK;
    }
    else
    {
        *Interface = NULL;
        return E_NOINTERFACE;
    }
}

STDMETHODIMP_(ULONG)
LiveProcDataTarget::AddRef(
    THIS
    )
{
    // No need to refcount as this class is contained.
    return 1;
}

STDMETHODIMP_(ULONG)
LiveProcDataTarget::Release(
    THIS
    )
{
    // No need to refcount as this class is contained.
    return 0;
}

HRESULT STDMETHODCALLTYPE
LiveProcDataTarget::GetMachineType( 
    /* [out] */ ULONG32 *machine)
{
    LEAF_CONTRACT;

#if defined(_X86_)
    *machine = IMAGE_FILE_MACHINE_I386;
#else
    PORTABILITY_ASSERT("Unknown Processor");
#endif
    return S_OK;
}

HRESULT STDMETHODCALLTYPE
LiveProcDataTarget::GetPointerSize( 
    /* [out] */ ULONG32 *size)
{
    LEAF_CONTRACT;

    *size = sizeof(void*);
    return S_OK;
}

HRESULT STDMETHODCALLTYPE
LiveProcDataTarget::GetImageBase( 
    /* [string][in] */ LPCWSTR name,
    /* [out] */ CLRDATA_ADDRESS *base)
{
    HRESULT status;

    //
    // The only image base that the access code cares
    // about right now is the base of mscorwks.  Look
    // that up from shared memory.
    //
    
    if (wcscmp(name, MAKEDLLNAME_W(L"mscorwks")))
    {
        return E_NOINTERFACE;
    }
    
    IPCReaderInterface* ipc = (IPCReaderInterface *)
        new(nothrow) IPCReaderImpl();
    if (!ipc)
    {
        return E_OUTOFMEMORY;
    }

    if ((status = ipc->OpenPrivateBlockOnPidReadOnly(m_processId)) == S_OK)
    {
        // Cast to LONG_PTR to get sign extension if necessary.
        *base = (LONG_PTR)ipc->GetInstance();
        ipc->ClosePrivateBlock();
    }
    
    delete ipc;

    return status;
}

HRESULT STDMETHODCALLTYPE
LiveProcDataTarget::ReadVirtual( 
    /* [in] */ CLRDATA_ADDRESS address,
    /* [length_is][size_is][out] */ PBYTE buffer,
    /* [in] */ ULONG32 request,
    /* [optional][out] */ ULONG32 *done)
{
    // ReadProcessMemory will fail if any part of the
    // region to read does not have read access.  This
    // routine attempts to read the largest valid prefix
    // so it has to break up reads on page boundaries.

    HRESULT status = S_OK;
    ULONG32 totalDone = 0;
    SIZE_T read;
    ULONG32 readSize;

    while (request > 0)
    {
        // Calculate bytes to read and don't let read cross
        // a page boundary.
        readSize = OS_PAGE_SIZE - (ULONG32)(address & (OS_PAGE_SIZE - 1));
        readSize = min(request, readSize);

        if (!ReadProcessMemory(m_process, (PVOID)(ULONG_PTR)address,
                               buffer, readSize, &read))
        {
            if (totalDone == 0)
            {
                // If we haven't read anything indicate failure.
                status = E_FAIL;
            }
            break;
        }

        totalDone += (ULONG32)read;
        address += read;
        buffer += read;
        request -= (ULONG32)read;
    }

    *done = totalDone;
    return status;
}

HRESULT STDMETHODCALLTYPE
LiveProcDataTarget::WriteVirtual( 
    /* [in] */ CLRDATA_ADDRESS address,
    /* [size_is][in] */ PBYTE buffer,
    /* [in] */ ULONG32 request,
    /* [optional][out] */ ULONG32 *done)
{
    // Not necessary yet.
    return E_NOTIMPL;
}

HRESULT STDMETHODCALLTYPE
LiveProcDataTarget::GetTLSValue(
    /* [in] */ ULONG32 threadID,
    /* [in] */ ULONG32 index,
    /* [out] */ CLRDATA_ADDRESS* value)
{
    // Not necessary yet.
    return E_NOTIMPL;
}

HRESULT STDMETHODCALLTYPE
LiveProcDataTarget::SetTLSValue(
    /* [in] */ ULONG32 threadID,
    /* [in] */ ULONG32 index,
    /* [in] */ CLRDATA_ADDRESS value)
{
    // Not necessary yet.
    return E_NOTIMPL;
}

HRESULT STDMETHODCALLTYPE
LiveProcDataTarget::GetCurrentThreadID(
    /* [out] */ ULONG32* threadID)
{
    // Not necessary yet.
    return E_NOTIMPL;
}

HRESULT STDMETHODCALLTYPE
LiveProcDataTarget::GetThreadContext(
    /* [in] */ ULONG32 threadID,
    /* [in] */ ULONG32 contextFlags,
    /* [in] */ ULONG32 contextSize,
    /* [out, size_is(contextSize)] */ PBYTE context)
{
    // Not necessary yet.
    return E_NOTIMPL;
}

HRESULT STDMETHODCALLTYPE
LiveProcDataTarget::SetThreadContext(
    /* [in] */ ULONG32 threadID,
    /* [in] */ ULONG32 contextSize,
    /* [out, size_is(contextSize)] */ PBYTE context)
{
    // Not necessary yet.
    return E_NOTIMPL;
}

HRESULT STDMETHODCALLTYPE
LiveProcDataTarget::Request( 
    /* [in] */ ULONG32 reqCode,
    /* [in] */ ULONG32 inBufferSize,
    /* [size_is][in] */ BYTE *inBuffer,
    /* [in] */ ULONG32 outBufferSize,
    /* [size_is][out] */ BYTE *outBuffer)
{
    // None supported.
    return E_INVALIDARG;
}

//----------------------------------------------------------------------------
//
// LoadDataAccessDll.
//
//----------------------------------------------------------------------------

HRESULT
LoadDataAccessDll(HINSTANCE mscorModule,
                  REFIID ifaceId,
                  ICLRDataTarget* target,
                  HMODULE* dllHandle,
                  void** iface)
{
    HRESULT status;
    WCHAR accessDllPath[MAX_PATH];
    HMODULE accessDll;

    //
    // Load the access DLL from the same directory
    // as the runtime DLL.
    //

    if (!WszGetModuleFileName(mscorModule,
                              accessDllPath, NumItems(accessDllPath)))
    {
        return HRESULT_FROM_GetLastError();
    }

    PWSTR pathTail = wcsrchr(accessDllPath, '\\');
    if (!pathTail)
    {
        return E_INVALIDARG;
    }
    pathTail++;

    PWSTR eeFlavor = L"wks";    
    if (_snwprintf_s(pathTail, _countof(accessDllPath) + (accessDllPath - pathTail),
                   NumItems(accessDllPath) - (pathTail - accessDllPath),
                   MAKEDLLNAME_W(L"mscordac%s"), eeFlavor) <= 0)
    {
        return E_INVALIDARG;
    }

    accessDll = WszLoadLibrary(accessDllPath);
    if (!accessDll)
    {
        return HRESULT_FROM_GetLastError();
    }

    //
    // Get the access interface and have it
    // enumerate the interesting memory in the target process.
    //

    void* ifacePtr;
    PFN_CLRDataCreateInstance entry = (PFN_CLRDataCreateInstance)
        GetProcAddress(accessDll, "CLRDataCreateInstance");
    if (!entry)
    {
        status = HRESULT_FROM_GetLastError();
        FreeLibrary(accessDll);
    }
    else if ((status = entry(ifaceId, target, &ifacePtr)) != S_OK)
    {
        FreeLibrary(accessDll);
    }
    else
    {
        *dllHandle = accessDll;
        *iface = ifacePtr;
    }

    return status;
}
