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
// File: enummem.cpp
//
// ICLRDataEnumMemoryRegions implementation.
//
//*****************************************************************************

#include "stdafx.h"

#include <eeconfig.h>
#include <ecall.h>

#include "typestring.h"
#include "daccess.h"
#include "ipcmanagerinterface.h"


//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//
// Collecting all Ngen images
//
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
HRESULT ClrDataAccess::EnumMemCollectNgenImages()
{
    ProcessModIter modIter;
    Module* modDef = NULL;
    HRESULT status = S_OK;
    PEFile  *file;
    TADDR pStartAddr = 0;
    ULONG32 ulSize = 0;
    ULONG32 ulSizeBlock;

    UINT cbMemoryReported = m_cbMemoryReported;

    //
    // Collect the ngen images - Iterating through module list
    //
    EX_TRY
    {
        while ((modDef = modIter.NextModule()))
        {
            EX_TRY
            {
                ulSize = 0;
                file = modDef->GetFile();

                if (file->HasNativeImage())
                {
                    // We should only skip if signed by Microsoft!
                    pStartAddr = (TADDR)file->GetLoadedNative()->GetBase();
                    ulSize = file->GetLoadedNative()->GetSize();
                }
                else if (file->HasMetadata() && file->IsLoaded(FALSE))
                {
                    pStartAddr = (TADDR)file->GetLoadedIL()->GetBase();
                    ulSize = file->GetLoadedIL()->GetSize();
                }

                // memory are mapped in in OS_PAGE_SIZE size.
                // Some memory are mapped in but some are not. You cannot
                // write all in one block. So iterating through page size
                //
                while (ulSize > 0)
                {
                    ulSizeBlock = ulSize > OS_PAGE_SIZE ? OS_PAGE_SIZE : ulSize;
                    ReportMem(pStartAddr, ulSizeBlock);
                    pStartAddr += ulSizeBlock;
                    ulSize -= ulSizeBlock;
                }
            }
            EX_CATCH
            {
                // Just swallow the exception and keep continue
            }
            EX_END_CATCH(SwallowAllExceptions)
        }
    }
    EX_CATCH
    {
        // Just swallow the exception and keep continue
    }
    EX_END_CATCH(SwallowAllExceptions)

    m_dumpStats.m_cbNgen = m_cbMemoryReported - cbMemoryReported;
    return status;
}



//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//
// collecting memory for mscorwks's heap dump critical statics
// This include the stress log, config structure, and IPC block
//
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
HRESULT ClrDataAccess::EnumMemCLRHeapCrticalStatic(IN CLRDataEnumMemoryFlags flags)
{
    UINT cbMemoryReported = m_cbMemoryReported;


    // This is pointing to a static buffer
    DacEnumHostDPtrMem(g_pConfig);

    // dump GC heap structures. Note that the managed heap is not dumped out. 
    // We are just dump the GC heap structures.
    //
    EnumWksGlobalMemoryRegions(flags);
#ifndef GC_SMP
    EnumSvrGlobalMemoryRegions(flags);
#endif

    //
    // Write Out IPC Blocks
    //
    EX_TRY
    {
        g_pIPCManagerInterface.EnumMem();
        if (g_pIPCManagerInterface.IsValid())
        {
            // write out the instance
            DacEnumHostDPtrMem(g_pIPCManagerInterface);

            // Then write out the public and private block
            ReportMem((TADDR)g_pIPCManagerInterface->GetBlockStart(), g_pIPCManagerInterface->GetBlockSize());
            ReportMem((TADDR)g_pIPCManagerInterface->GetPublicBlockStart(), g_pIPCManagerInterface->GetPublicBlockSize());
        }
    }
    EX_CATCH
    {
        // Just swallow the exception and keep continue
    }
    EX_END_CATCH(SwallowAllExceptions)

    m_dumpStats.m_cbClrHeapStatics = m_cbMemoryReported - cbMemoryReported;

    return S_OK;


}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//
// collecting memory for mscorwks's statics. This is the minimal 
// set of global and statics that we need to have !threads, !pe, !ClrStack
// to work. 
//
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
HRESULT ClrDataAccess::EnumMemCLRStatic(IN CLRDataEnumMemoryFlags flags)
{
    UINT cbMemoryReported = m_cbMemoryReported;

    //
    // write out the static and global content that we care.
    //

    EX_TRY
    {
        // The followig macro will report memory all of the dac related mscorwks static and 
        // global variables. But it won't report the structures that are pointed by 
        // global pointers.
        //
#define DEFINE_DACVAR(id_type, size_type, id)   \
        ReportMem(m_globalBase + g_dacGlobals.id, sizeof(size_type));
#include "dacvars.h"

        // These two static pointers are pointed to static data of byte[]
        // then run constructor in place
        //
        ReportMem(m_globalBase + g_dacGlobals.SystemDomain__m_pSystemDomain,
                  sizeof(SystemDomain));
        ReportMem(m_globalBase + g_dacGlobals.SharedDomain__m_pSharedDomain,
                  sizeof(SharedDomain));

        // We need GCHeap pointer to make EEVersion work
        ReportMem(m_globalBase + g_dacGlobals.dac__g_pGCHeap,
              sizeof(GCHeap *));

        // see synblk.cpp, the pointer is pointed to a static byte[]
        SyncBlockCache::s_pSyncBlockCache.EnumMem();

        ReportMem(m_globalBase + g_dacGlobals.dac__gThreadTLSIndex,
                  sizeof(DWORD));
        ReportMem(m_globalBase + g_dacGlobals.dac__gAppDomainTLSIndex,
                  sizeof(DWORD));

        ReportMem( m_globalBase + g_dacGlobals.dac__g_FCDynamicallyAssignedImplementations,
                  sizeof(TADDR)*ECall::NUM_DYNAMICALLY_ASSIGNED_FCALL_IMPLEMENTATIONS);


        // These are the structures that are pointed by global pointers and we care.
        // Some may reside in heap and some may reside as a static byte array in mscorwks.dll
        // That is ok. We will report them explicitly.
        //
        g_pConfig.EnumMem();
        g_pPredefinedArrayTypes.EnumMem();
        g_pObjectClass.EnumMem();
        g_pStringClass.EnumMem();
        g_pArrayClass.EnumMem();
        g_pExceptionClass.EnumMem();
        g_pThreadAbortExceptionClass.EnumMem();
        g_pOutOfMemoryExceptionClass.EnumMem();
        g_pStackOverflowExceptionClass.EnumMem();
        g_pExecutionEngineExceptionClass.EnumMem();
        g_pDelegateClass.EnumMem();
        g_pMultiDelegateClass.EnumMem();
        g_pValueTypeClass.EnumMem();
        g_pEnumClass.EnumMem();
        g_pThreadClass.EnumMem();
        g_pCriticalFinalizerObjectClass.EnumMem();
        g_pFreeObjectMethodTable.EnumMem();
        g_pObjectCtorMD.EnumMem();
        g_fHostConfig.EnumMem();

        // These two static pointers are pointed to static data of byte[]
        // then run constructor in place
        //
        SystemDomain::m_pSystemDomain.EnumMem();
        SharedDomain::m_pSharedDomain.EnumMem();
        g_pDebugger.EnumMem();
        g_pEEInterface.EnumMem();
        g_pDebugInterface.EnumMem();
        g_pDebugInfoStore.EnumMem();
        g_pEEDbgInterfaceImpl.EnumMem();
        g_CORDebuggerControlFlags.EnumMem();
        g_Mscorlib.EnumMem();
        GCHeap::FinalizerThread.EnumMem();
        GCHeap::GcThread.EnumMem();
        GCHeap::gcHeapType.EnumMem();

        g_lowest_address.EnumMem();
        g_highest_address.EnumMem();
        g_card_table.EnumMem();

        g_pPredefinedArrayTypes[ELEMENT_TYPE_OBJECT]->EnumMemoryRegions(flags);
    }
    EX_CATCH
    {
        // Just swallow the exception and keep continue
    }
    EX_END_CATCH(SwallowAllExceptions)

    m_dumpStats.m_cbClrStatics = m_cbMemoryReported - cbMemoryReported;

    return S_OK;
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//
// This function reports memory that a heap dump need to debug CLR
// and managed code efficiently.
//
// We will write out -
// 1. mscorwks.dll's image read/write pages
// 2. IPC blocks - shared memory (needed for debugging service and perf counter)
// 3. ngen images excluding Metadata and IL for size perf
// 4. We may want to touch the code pages on the stack - to be safe....
//
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
HRESULT ClrDataAccess::EnumMemoryRegionsWorkerHeap(IN ICLRDataEnumMemoryRegionsCallback *callback, IN CLRDataEnumMemoryFlags flags)
{
    HRESULT status = S_OK;

    m_memStatus = S_OK;
    m_enumMemCb = callback;
    m_instances.ClearEnumMemMarker();

    // clear all of the previous cached memory
    Flush();

    // collect ngen image
    status = EnumMemCollectNgenImages();

    // collect CLR static
    status = EnumMemCLRStatic(flags);
    status = EnumMemCLRHeapCrticalStatic(flags);

    // Note that we do not need to flush out all of the dac instance manager's instance.
    // This is because it is a heap dump here. Assembly and AppDomain objects will be reported
    // by the default heap collection mechanism by dbghelp.lib
    //
    status = EnumMemDumpModuleList(flags);

    // end of code
    status = m_memStatus;
    m_enumMemCb = NULL;

    return S_OK;
}   // EnumMemoryRegionsWorkerHeap

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//
// Helper function for skinny mini-dump
// Pass in an managed object, this function will dump the EEClass hierachy
// and field desc of object so SOS's !DumpObj will work
//
//
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
HRESULT ClrDataAccess::DumpManagedObject(CLRDataEnumMemoryFlags flags, OBJECTREF objRef)
{
    HRESULT     status = S_OK;
    EEClass     *pEEClass = NULL;
    MethodTable *pMethodTable = NULL;

    if (objRef == NULL)
    {
        return status;
    }
    
    if (!CNameSpace::GetGcRuntimeStructuresValid ())
    {
        // GC is in progress, don't dump this object
        return S_OK;
    }
    
    EX_TRY
    {
        // write out the current EE class and the direct/indirect inherited EE Classes
        pMethodTable = objRef->GetGCSafeMethodTable();
        if (pMethodTable)
        {
            pEEClass = pMethodTable->GetClass();
        }
        while (pEEClass)
        {
            // just dump out the field desc
            pEEClass->EnumMemoryRegions(flags);

            // walk up to parent EEClass
            pEEClass = pEEClass->GetParentClass();
        }
        
        while (pMethodTable)
        {
            StackSString s;
            
            // This might look odd. We are not using variable s after forming it.
            // That is because our DAC inspecting API is using TypeString to form
            // full type name. Form the full name here is a implicit reference to needed
            // memory. 
            // 
            TypeString::AppendType(s, TypeHandle(pMethodTable), TypeString::FormatNamespace|TypeString::FormatFullInst);
            
            // Write out the MethodTable structure
            DacEnumHostDPtrMem(pMethodTable);

            // Walk up to parent MethodTable
            pMethodTable = pMethodTable->GetParentMethodTable();
        }

        // now dump the content for the managed object
        objRef->EnumMemoryRegions();
    }
    EX_CATCH
    {
        status = E_FAIL;
    }
    EX_END_CATCH(SwallowAllExceptions)
    return status;

}


//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//
// Helper function for skinny mini-dump
// Pass in an managed excption object, this function will dump
// the managed exception object and some of its fields, such as message, stack trace string,
// inner exception.
//
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
HRESULT ClrDataAccess::DumpManagedExcepObject(CLRDataEnumMemoryFlags flags, OBJECTREF objRef)
{
    if (objRef == NULL)
    {
        return S_OK;
    }

    if (!CNameSpace::GetGcRuntimeStructuresValid ())
    {
        // GC is in progress, don't dump this object
        return S_OK;
    }

    // write out the managed object for exception. This will only write out the 
    // direct field value. After this, we need to visit some selected fields, such as 
    // exception message and stack trace field, and dump out the object referenced via
    // the fields. 
    //
    DumpManagedObject(flags, objRef);

    EXCEPTIONREF exceptRef = (EXCEPTIONREF)objRef;

    // dump the exception message field
    DumpManagedObject(flags, (OBJECTREF)exceptRef->GetMessage());


    // dump the exception's stack trace field
    DumpManagedObject(flags, (OBJECTREF)exceptRef->GetStackTraceString());

    // dump the exception's remote stack trace field
    DumpManagedObject(flags, (OBJECTREF)exceptRef->GetRemoteStackTraceString());
    
    // Dump inner exception
    DumpManagedExcepObject(flags, exceptRef->GetInnerException());

    StackTraceArray stackTrace;
    exceptRef->GetStackTrace(stackTrace);
    if (I1ARRAYREF pTraceData = stackTrace.Get())
    {
        DWORD cbTraceData = pTraceData->GetNumComponents();

        // This is will make the StackTraceElement's array structure be reported.
        StackTraceArray::ArrayHeader * header = reinterpret_cast<StackTraceArray::ArrayHeader *>(
                DacInstantiateTypeByAddress(reinterpret_cast<TADDR>(pTraceData->GetDirectPointerToNonObjectElements()),
                                            cbTraceData,
                                            false));

        StackTraceElement * pStackTraceElementsOnHost = reinterpret_cast<StackTraceElement *>(header + 1);

        for (size_t i = 0; i < header->m_size; ++i)
        {
            MethodDesc* pMD = PTR_MethodDesc((TADDR) (pStackTraceElementsOnHost[i].pFunc));
            if (!DacHasMethodDescBeenEnumerated(pMD) && DacValidateMD(pMD, (HINSTANCE) m_globalBase))
            {
                pMD->EnumMemoryRegions(flags);
            }
        }
    }
            
    return S_OK;
}

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//
// Iterating through module list and report the memory.
// Remember to call
//  m_instances.DumpAllInstances(m_enumMemCb);
// when all memory enumeration are done if you call this function!
// This is because using ProcessModIter will drag in some memory implicitly.
//
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
HRESULT ClrDataAccess::EnumMemDumpModuleList(CLRDataEnumMemoryFlags flags)
{
    ProcessModIter  modIter;
    Module*         modDef;
    TADDR           base;
    ULONG32         length;
    PEFile          *file;
    UINT            cbMemoryReported = m_cbMemoryReported;

    //
    // Iterating through module list
    //
    EX_TRY
    {

        while ((modDef = modIter.NextModule()))
        {
            file = modDef->GetFile();
            base = (TADDR)file->GetLoadedImageContents(&length);
            file->EnumMemoryRegions(flags);
        }
    }
    EX_CATCH
    {
        // Just swallow the exception and keep continue
    }
    EX_END_CATCH(SwallowAllExceptions)

    m_dumpStats.m_cbModuleList = m_cbMemoryReported - cbMemoryReported;

    return S_OK;
}

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//
// Iterating through each frame to make sure
// we dump out MethodDesc, DJI etc related info
// This is a generic helper for walking stack. However, if you call
// this function, make sure to flush instance in the DAC Instance manager.
//
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
HRESULT ClrDataAccess::EnumMemWalkStackHelper(CLRDataEnumMemoryFlags flags, IXCLRDataStackWalk  *pStackWalk)
{


    HRESULT status = S_OK;
    IXCLRDataFrame      *pFrame = NULL;
    IXCLRDataMethodInstance *pMethod = NULL;
    IXCLRDataMethodDefinition *pMethodDefinition = NULL;
    IXCLRDataTypeInstance   *pTypeInstance = NULL;
    MethodDesc     *pMethodDesc = NULL;
    EX_TRY
    {
        // exhaust the frames using DAC api
        for (; status == S_OK; )
        {
            status = pStackWalk->GetFrame(&pFrame);
            if (status == S_OK && pFrame != NULL)
            {
                // write out the code that ip pointed to
                CONTEXT context;
                REGDISPLAY regDisp;
                if ((status=pFrame->GetContext(CONTEXT_CONTROL, sizeof(CONTEXT),
                                                   NULL, (BYTE *)&context))==S_OK)
                {
                    ::FillRegDisplay(&regDisp, &context);
                    TADDR callEnd = taGetControlPC(&regDisp);

                    DacEnumMemoryRegion((TADDR)(callEnd - ((DWORD)MAX_INSTRUCTION_LENGTH)), MAX_INSTRUCTION_LENGTH * 2);
#if defined(_X86_)
                    // If it was an indirect call we also need
                    // to save the data indirected through.
                    PTR_BYTE callCode = PTR_BYTE(callEnd - 6);
                    PTR_BYTE callMrm = PTR_BYTE(callEnd - 5);
                    PTR_TADDR callInd = PTR_TADDR(callEnd - 4);
                    if (callCode.IsValid() &&
                        *callCode == 0xff &&
                        callMrm.IsValid() &&
                        (*callMrm & 0x30) == 0x10 &&
                        callInd.IsValid())
                    {
                        DacEnumMemoryRegion(*callInd, sizeof(TADDR));
                    }
#endif // #ifdef _X86_

                }

                status = pFrame->GetMethodInstance(&pMethod);
                if (status == S_OK && pMethod != NULL)
                {
                    // managed frame
                    pMethod->GetTypeInstance(&pTypeInstance);
                    if (pTypeInstance)
                    {
                        pTypeInstance->Release();
                    }
                    pMethod->GetDefinition(&pMethodDefinition);
                    if (pMethodDefinition != NULL)
                    {
                        // This is really not necessary if we can form name
                        // through metadata and tokens in DAC api!!!
                        //
                        pMethodDesc = ((ClrDataMethodDefinition *)pMethodDefinition)->GetMethodDesc();
                        if (pMethodDesc)
                        {
                            pMethodDesc->EnumMemoryRegions(flags);
                        }
                        pMethodDefinition->Release();
                    }
                    pMethod->Release();

                }
                pFrame->Release();
                pFrame = NULL;
            }
            status = pStackWalk->Next();
        }

    }
    EX_CATCH
    {
        status = E_FAIL;
    }
    EX_END_CATCH(SwallowAllExceptions)

    
    return status;
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//
// This function will walk all threads, all the context in the
// exception state to report memory. This can also drag in memory implicitly.
// So do call
//      m_instances.DumpAllInstances(m_enumMemCb);
// when function is done.
//
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
HRESULT ClrDataAccess::EnumMemDumpAllThreadsStack(CLRDataEnumMemoryFlags flags)
{
    HRESULT     status = S_OK;
    UINT        cbMemoryReported = m_cbMemoryReported;

    EX_TRY
    {
        CLRDATA_ENUM        handle;
        IXCLRDataTask       *pIXCLRDataTask = NULL;
        IXCLRDataExceptionState *pExcepState = NULL;
        IXCLRDataExceptionState *pExcepStatePrev = NULL;
        IXCLRDataStackWalk  *pStackWalk = NULL;
        Thread              *pThread = NULL;

        // enumerating through each thread's each frame, dump out some interesting
        // code memory needed to debugger to recognize frame
        //
        ThreadStore::EnumMemoryRegions(flags);

        // enumerating through each thread
        StartEnumTasks(&handle);
        status = EnumTask(&handle, &pIXCLRDataTask);
        for (;status == S_OK && pIXCLRDataTask != NULL;)
        {
            // get Thread *
            pThread = ((ClrDataTask *)pIXCLRDataTask)->GetThread();

            // Write out the Thread instance
            DacEnumHostDPtrMem(pThread);

            // Write out the context pointed by the thread
            DacEnumHostDPtrMem(pThread->GetContext());



            DumpManagedObject(flags, pThread->GetExposedObjectRaw());

            // dump the exception object
            DumpManagedExcepObject(flags, pThread->LastThrownObject());

            // stack walk
            status = pIXCLRDataTask->CreateStackWalk(CLRDATA_SIMPFRAME_UNRECOGNIZED | CLRDATA_SIMPFRAME_MANAGED_METHOD | CLRDATA_SIMPFRAME_RUNTIME_MANAGED_CODE | CLRDATA_SIMPFRAME_RUNTIME_UNMANAGED_CODE,
                        &pStackWalk);
            if (status == S_OK && pStackWalk != NULL)
            {
                status = EnumMemWalkStackHelper(flags, pStackWalk);
                pStackWalk->Release();
                pStackWalk = NULL;
            }

            // Now probe into the exception info
            status = pIXCLRDataTask->GetCurrentExceptionState(&pExcepState);
            while (status == S_OK && pExcepState != NULL)
            {

                // touch the throwable in exception state
                PTR_UNCHECKED_OBJECTREF throwRef(((ClrDataExceptionState*)pExcepState)->m_throwable);
                DumpManagedExcepObject(flags, *throwRef);

                // get the type of the exception
                IXCLRDataValue *pValue = NULL;
                status = pExcepState->GetManagedObject(&pValue);
                if (status == S_OK && pValue != NULL)
                {
                    IXCLRDataTypeInstance *pTypeInstance = NULL;
                    // Make sure that we can get back a TypeInstance during inspection
                    status = pValue->GetType(&pTypeInstance);
                    if (status == S_OK && pTypeInstance != NULL)
                    {
                        pTypeInstance->Release();
                    }
                    pValue->Release();
                }

                // If Exception state has a new context, we will walk with the stashed context as well.
                // Note that in stack overflow exception's case, m_pContext is null..
                //
                // It is possible that we are in exception's catch clause when we
                // try to walk the stack below. This is a very weird situation where
                // stack is logically unwind and not physically unwind. We may not be able
                // to walk the stack correctly here. Anyway, we try to catch exception thrown
                // by bad stack walk in EnumMemWalkStackHelper.
                //
                PTR_CONTEXT pContext = ((ClrDataExceptionState*)pExcepState)->GetCurrentContextRecord();
                if (pContext != NULL)
                {
                    CONTEXT newContext;
                    newContext = *pContext;

                    // We need to trigger stack walk again using the exception's context!
                    status = pIXCLRDataTask->CreateStackWalk(CLRDATA_SIMPFRAME_UNRECOGNIZED | CLRDATA_SIMPFRAME_MANAGED_METHOD | CLRDATA_SIMPFRAME_RUNTIME_MANAGED_CODE | CLRDATA_SIMPFRAME_RUNTIME_UNMANAGED_CODE,
                                &pStackWalk);
                    if (status == S_OK && pStackWalk != NULL)
                    {
                        status = pStackWalk->SetContext2(CLRDATA_STACK_SET_CURRENT_CONTEXT, sizeof(CONTEXT), (BYTE *) &newContext);
                        if (status == S_OK)
                        {
                            status = EnumMemWalkStackHelper(flags, pStackWalk);
                        }
                        pStackWalk->Release();
                        pStackWalk = NULL;
                    }
                }

                // get the previous exception
                status = pExcepState->GetPrevious(&pExcepStatePrev);
                pExcepState->Release();
                pExcepState = pExcepStatePrev;
            }

            // get next thread
            pIXCLRDataTask->Release();
            pIXCLRDataTask = NULL;
            status = EnumTask(&handle, &pIXCLRDataTask);

        }
        EndEnumTasks(handle);
    }
    EX_CATCH
    {
        // Just swallow the exception and keep continue
    }
    EX_END_CATCH(SwallowAllExceptions)

    // updating the statistics
    m_dumpStats.m_cbStack = m_cbMemoryReported - cbMemoryReported;

    return status;
}
typedef DPTR(StackTraceElement) PTR_StackTraceElement;

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//
// Generating skinny mini-dump. Skinny mini-dump will only support stack trace, module list,
// and Exception list viewing.
//
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
HRESULT ClrDataAccess::EnumMemoryRegionsWorkerSkinny(IN ICLRDataEnumMemoryRegionsCallback *callback, IN CLRDataEnumMemoryFlags flags)
{
    HRESULT status = S_OK;

    m_memStatus = S_OK;
    m_enumMemCb = callback;

    // clear all of the previous cached memory
    Flush();

    // Iterating to all threads' stacks
    status = EnumMemDumpAllThreadsStack(flags);

    // Iterating to module list.
    status = EnumMemDumpModuleList(flags);

    //
    // iterating through static that we care
    //
    // collect CLR static
    status = EnumMemCLRStatic(flags);

    // now dump the memory get dragged in by using DAC API implicitly.
    m_dumpStats.m_cbImplicity = m_instances.DumpAllInstances(m_enumMemCb);

    status = m_memStatus;
    m_enumMemCb = NULL;

    return S_OK;
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//
// Write out mscorwks's data segment. This will write out the whole
// data segment for mscorwks. It is about 200 or 300K. Most of it (90%) are
// vtable definition that we don't really care. But we don't have a
// good walk to just write out all globals and statics. 
//
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
HRESULT ClrDataAccess::EnumMemWriteDataSegment()
{
    PEDecoder *pedecoder = NULL;
    EX_TRY
    {
    
        // Collecting mscorwks's data segment
        {
            // m_globalBase is the base address of target process's mscorwks module
            pedecoder = new PEDecoder((void *) m_globalBase);
            if (pedecoder == NULL)
            {
                // nothing we can do more here but continue other part of dumping
                return S_OK;
            }
            PTR_IMAGE_SECTION_HEADER pSection = (PTR_IMAGE_SECTION_HEADER) pedecoder->FindFirstSection();
            PTR_IMAGE_SECTION_HEADER pSectionEnd = pSection + VAL16(pedecoder->GetNumberOfSections());

            while (pSection < pSectionEnd)
            {
                if (pSection->Name[0] == '.' &&
                    pSection->Name[1] == 'd' &&
                    pSection->Name[2] == 'a' &&
                    pSection->Name[3] == 't' &&
                    pSection->Name[4] == 'a')
                {   
                    // This is the .data section of mscorwks
                    ReportMem(m_globalBase + pSection->VirtualAddress, pSection->Misc.VirtualSize);
                }
                pSection++;
             }
        }
    }
    EX_CATCH
    {
        // Just swallow the exception and keep continue
    }
    EX_END_CATCH(SwallowAllExceptions)

    if (pedecoder != NULL)
    {
        delete pedecoder;
    }
    return S_OK;        
}    

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//
// Custom Dump. Depending on the value of g_ECustomDumpFlavor, different dump
// will be taken. You can set this global variable using hosting API
// ICLRErrorReportingManager::BeginCustomDump. 
//
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
HRESULT ClrDataAccess::EnumMemoryRegionsWorkerCustom(IN ICLRDataEnumMemoryRegionsCallback* callback)
{
    HRESULT status = S_OK;
    ECustomDumpFlavor eFlavor = g_ECustomDumpFlavor;
    CLRDataEnumMemoryFlags flags = CLRDATA_ENUM_MEM_MINI;

    m_memStatus = S_OK;
    m_enumMemCb = callback;

    // clear all of the previous cached memory
    Flush();

    if (eFlavor == DUMP_FLAVOR_Mini)
    {
        // Iterating to all threads' stacks
        status = EnumMemDumpAllThreadsStack(flags);

        // Iterating to module list.
        status = EnumMemDumpModuleList(flags);

        //
        // iterating through static that we care
        //
        // collect CLR static
        status = EnumMemCLRStatic(flags);

        // we are done...

        // now dump the memory get dragged in implicitly
        m_dumpStats.m_cbImplicity = m_instances.DumpAllInstances(m_enumMemCb);

    }
    else if (eFlavor == DUMP_FLAVOR_CriticalCLRState)
    {
        // We need to walk Threads stack to view managed frames.
        // Iterating through module list

        // Iterating to all threads' stacks
        status = EnumMemDumpAllThreadsStack(flags);

        // Iterating to module list.
        status = EnumMemDumpModuleList(flags);

        //
        // iterating through static that we care
        //
        // collect CLR static
        status = EnumMemCLRStatic(flags);

        // Collecting some CLR secondary critical data
        status = EnumMemCLRHeapCrticalStatic(flags);
        status = EnumMemWriteDataSegment();

        // we are done...

        // now dump the memory get dragged in implicitly
        m_dumpStats.m_cbImplicity = m_instances.DumpAllInstances(m_enumMemCb);

    }
    else if (eFlavor == DUMP_FLAVOR_NonHeapCLRState)
    {
        // since all CLR hosted heap will be dump by the host,
        // the EE structures that are not loaded using LoadLibrary will
        // be included by the host.
        //
        // Thus we only need to include mscorwks's critical data and ngen images

        flags = CLRDATA_ENUM_MEM_HEAP;

        status = EnumMemCLRStatic(flags);

        // Collecting some CLR secondary critical data
        status = EnumMemCLRHeapCrticalStatic(flags);

        status = EnumMemWriteDataSegment();
        status = EnumMemCollectNgenImages();
    }
    else
    {
        status = E_INVALIDARG;
    }

    status = m_memStatus;
    m_enumMemCb = NULL;

    return S_OK;
}



//
// 
//
LONG EnumGlobalMemoryRegionFilter(
    EXCEPTION_POINTERS *pExceptionInfo, // the pExceptionInfo passed to a filter function.
    PVOID       pData)      // data that we don't use.
{
    // If we ever hit this assert, then look at the callstack and see who threw the unhandled exception.
    // If it's a EnumMemoryRegion function, then it shouldn't have thrown unhandled. Either
    // use ptr.IsValid() to avoid a bad reference or it should catch its own exceptions.
    // It should never be an AV or null ref.
    // If it's some other random exception, then deal with it accordingly.

    _ASSERTE(!"Unhandled exception coming through EnumGlobalMemoryRegions");

    return EXCEPTION_CONTINUE_SEARCH;
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//
// Internal API to support minidump and heap dump. It just delegate
// to proper function but with a top level catch. 
//
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
HRESULT ClrDataAccess::EnumMemoryRegionsWrapper(IN ICLRDataEnumMemoryRegionsCallback* callback,
                                                IN CLRDataEnumMemoryFlags flags)
{
    HRESULT status = S_OK;

    // This should never ever throw.
    PAL_TRY
    {
        if ( flags == CLRDATA_ENUM_MEM_MINI)
        {
            // skinny mini-dump
            status = EnumMemoryRegionsWorkerSkinny(callback, flags);
        }
        else if ( flags == CLRDATA_ENUM_MEM_HEAP)
        {
            status = EnumMemoryRegionsWorkerHeap(callback, flags);
        }
        else
        {
            _ASSERTE(!"Bad flags passing to EnumMemoryRegionsWrapper!");
        }
    }
    PAL_EXCEPT_FILTER(EnumGlobalMemoryRegionFilter, NULL)
    {
        // This will never get executed.
    }
    PAL_ENDTRY

    return status;
}

#define     MiniDumpWithPrivateReadWriteMemory     0x00000200
#define     MiniDumpWithFullAuxiliaryState         0x00008000



//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//
// Entry function for generating CLR aware dump. This function is called
// for minidump, heap dump, and custom dumps. CLR specific memory will
// be reported to outter level dumper (usually dbgHelp's MiniDumpWriteDump api)
// through the callback. We do not write out to file directly. 
//
// CLR may report duplicate memory chuck and it is up to debugger
// host to trim them. 
//
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
STDMETHODIMP
ClrDataAccess::EnumMemoryRegions(IN ICLRDataEnumMemoryRegionsCallback* callback,
                                 IN ULONG32 miniDumpFlags,
                                 IN CLRDataEnumMemoryFlags flags)    // reserved not used
{
    HRESULT status;



    DAC_ENTER();

    EX_TRY
    {
        ClearDumpStats();
        if (miniDumpFlags & MiniDumpWithPrivateReadWriteMemory)
        {
            // heap dump
            status = EnumMemoryRegionsWrapper(callback, CLRDATA_ENUM_MEM_HEAP);
        }
        else if (miniDumpFlags & MiniDumpWithFullAuxiliaryState)
        {
            // This is the host custom dump. Not implemented yet.
            status = EnumMemoryRegionsWorkerCustom(callback);
        }
        else
        {
            // minidump
            status = EnumMemoryRegionsWrapper(callback, CLRDATA_ENUM_MEM_MINI);
        }
    }
    EX_CATCH
    {
        // We should never actually be here b/c none of the EMR functions should throw.
        // They should all either be written robustly w/ ptr.IsValid() and catching their
        // own exceptions.
        if (!DacExceptionFilter(GET_EXCEPTION(), this, &status))
        {
            EX_RETHROW;
        }
    }
    EX_END_CATCH(SwallowAllExceptions)

    DAC_LEAVE();

    
    return status;
}


//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//
// Clear the statistics for the dump. For each dump generation, we
// clear the dump statistics. At the end of the dump generation, you can 
// view the statics data member m_dumpStats and see how many bytes that
// we have reported to our debugger host. 
//
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
void ClrDataAccess::ClearDumpStats()
{
    m_cbMemoryReported = 0;
    memset(&m_dumpStats, 0, sizeof(DumpMemoryReportStatics));
}
