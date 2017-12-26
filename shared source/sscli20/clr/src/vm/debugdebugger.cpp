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
** File: DebugDebugger.cpp
**
**                                                 
**
** Purpose: Native methods on System.Debug.Debugger
**
** Date:  April 2, 1998
**
===========================================================*/

#include "common.h"

#include <object.h>
#include "ceeload.h"
#include "corpermp.h"

#include "excep.h"
#include "frames.h"
#include "vars.hpp"
#include "field.h"
#include "comstringcommon.h"
#include "comstring.h"
#include "gc.h"
#include "jitinterface.h"
#include "comsystem.h"
#include "debugdebugger.h"
#include "dbginterface.h"
#include "cordebug.h"
#include "corsym.h"
#include "generics.h"
#include "eemessagebox.h"

LogHashTable g_sLogHashTable;

// Call into the unhandled-exception processing code to launch Watson.
void DoDebugBreak(UINT_PTR address)
{
    CONTRACTL
    {
        MODE_ANY;
        GC_TRIGGERS;
        THROWS;
        PRECONDITION(address != NULL);
    }
    CONTRACTL_END;
    
    LOG((LF_EH, LL_INFO10, "DoDebugBreak: break at %0p\n", address));

    EXCEPTION_RECORD exceptionRecord;
    memset(&exceptionRecord, 0, sizeof(exceptionRecord));

    exceptionRecord.ExceptionAddress = reinterpret_cast< PVOID >(address);

    EXCEPTION_POINTERS exceptionPointers;
    memset(&exceptionPointers, 0, sizeof(exceptionPointers));

    CONTEXT context;
    memset(&context, 0, sizeof(context));

    exceptionPointers.ExceptionRecord = &exceptionRecord;
    exceptionPointers.ContextRecord = &context;


    WatsonLastChance(GetThread(), &exceptionPointers, TypeOfReportedError::UserBreakpoint);

} // void DoDebugBreak()


// If a debugger is attached, break to it.  Otherwise call helper to launch Watson
FCIMPL0(void, DebugDebugger::Break)
{
    CONTRACTL
    {
        MODE_COOPERATIVE;
        DISABLED(GC_TRIGGERS);  // can't use this in an FCALL because we're in forbid gc mode until we setup a H_M_F.
        THROWS;
        SO_TOLERANT;
    }
    CONTRACTL_END;

#ifdef DEBUGGING_SUPPORTED
    _ASSERTE (g_pDebugInterface != NULL);

    HELPER_METHOD_FRAME_BEGIN_0();

#ifdef _DEBUG
    {
        static int fBreakOnDebugBreak = -1;
        if (fBreakOnDebugBreak == -1)
            fBreakOnDebugBreak = REGUTIL::GetConfigDWORD(L"BreakOnDebugBreak", 0);
        _ASSERTE(fBreakOnDebugBreak == 0 && "BreakOnDebugBreak");
    }

    static BOOL fDbgInjectFEE = -1;
    if (fDbgInjectFEE == -1)
        fDbgInjectFEE = REGUTIL::GetConfigDWORD(L"DbgInjectFEE", 0);
#endif
    
    if (CORDebuggerAttached() INDEBUG(|| fDbgInjectFEE))
    {   // A debugger is already attached -- let it handle the event.
        g_pDebugInterface->SendUserBreakpoint(GetThread());
    }
    else
    {   // No debugger attached -- Watson up.

        // The HelperMethodFrame knows how to get the return address.
        DoDebugBreak(HELPER_METHOD_FRAME_GET_RETURN_ADDRESS());
    }

    HELPER_METHOD_FRAME_END();
#endif // DEBUGGING_SUPPORTED
}
FCIMPLEND

FCIMPL0(FC_BOOL_RET, DebugDebugger::Launch)
{
    CONTRACTL
    {
        MODE_COOPERATIVE;
        DISABLED(GC_TRIGGERS);  // can't use this in an FCALL because we're in forbid gc mode until we setup a H_M_F.
        THROWS;
        SO_TOLERANT;
    }
    CONTRACTL_END;

#ifdef DEBUGGING_SUPPORTED
    if (CORDebuggerAttached())
    {
        FC_RETURN_BOOL(TRUE);
    }
    else
    {
        _ASSERTE (g_pDebugInterface != NULL);

        HRESULT hr = S_OK;

        HELPER_METHOD_FRAME_BEGIN_RET_0();

        hr = g_pDebugInterface->LaunchDebuggerForUser ();

        HELPER_METHOD_FRAME_END();

        if (SUCCEEDED (hr))
        {
            FC_RETURN_BOOL(TRUE);
        }
    }
#endif // DEBUGGING_SUPPORTED

    FC_RETURN_BOOL(FALSE);
}
FCIMPLEND


FCIMPL0(FC_BOOL_RET, DebugDebugger::IsDebuggerAttached)
{
    CONTRACTL
    {
        MODE_COOPERATIVE;
        DISABLED(GC_TRIGGERS);  // can't use this in an FCALL because we're in forbid gc mode until we setup a H_M_F.
        THROWS;
        SO_TOLERANT;
    }
    CONTRACTL_END;
    
    FC_GC_POLL_RET();

#ifdef DEBUGGING_SUPPORTED
    //verbose so that we'll return (INT32)1 or (INT32)0
    if (GetThread()->GetDomain()->IsDebuggerAttached())
    {
        FC_RETURN_BOOL(TRUE);
    }

#endif // DEBUGGING_SUPPORTED

    FC_RETURN_BOOL(FALSE);
}
FCIMPLEND


/*static*/ BOOL DebugDebugger::IsLoggingHelper()
{
    WRAPPER_CONTRACT;
    
#ifdef DEBUGGING_SUPPORTED
        if (GetThread()->GetDomain()->IsDebuggerAttached())
    {
        return (g_pDebugInterface->IsLoggingEnabled());
    }
#endif // DEBUGGING_SUPPORTED
    return FALSE;
}


// Log to managed debugger. 
// It will send a managed log event, which will faithfully send the two string parameters here without
// appending a newline to anything.
// It will also call OutputDebugString() which will send a native debug event. The message 
// string there will be a composite of the two managed string parameters and may include a newline.
FCIMPL3(void, DebugDebugger::Log, 
        INT32 Level, 
        StringObject* strModuleUNSAFE, 
        StringObject* strMessageUNSAFE
       )
{
    CONTRACTL
    {
        MODE_COOPERATIVE;
        DISABLED(GC_TRIGGERS);  // can't use this in an FCALL because we're in forbid gc mode until we setup a H_M_F.
        THROWS;
        SO_TOLERANT;
        PRECONDITION(CheckPointer(strModuleUNSAFE, NULL_OK));
        PRECONDITION(CheckPointer(strMessageUNSAFE, NULL_OK));
    }
    CONTRACTL_END;

    STRINGREF strModule   = (STRINGREF)ObjectToOBJECTREF(strModuleUNSAFE);
    STRINGREF strMessage  = (STRINGREF)ObjectToOBJECTREF(strMessageUNSAFE);

    HELPER_METHOD_FRAME_BEGIN_2(strModule, strMessage);

    // OutputDebugString will log to native/interop debugger.
    if (strModule != NULL)
    {
        WszOutputDebugString(strModule->GetBuffer());
        WszOutputDebugString(L" : ");
    }

    if (strMessage != NULL)
    {
        WszOutputDebugString(strMessage->GetBuffer());
    }

    // If we're not logging a module prefix, then don't log the newline either.
    // Thus if somebody is just logging messages, there won't be any extra newlines in there.
    // If somebody is also logging category / module information, then this call to OutputDebugString is
    // already prepending that to the message, so we append a newline for readability.
    if (strModule != NULL)
    {
        WszOutputDebugString(L"\n");
    }


#ifdef DEBUGGING_SUPPORTED

    AppDomain *pAppDomain = GetThread()->GetDomain();

    // Send message for logging only if the 
    // debugger is attached and logging is enabled
    // for the given category
    if (pAppDomain->IsDebuggerAttached() || (Level == PanicLevel))
    {
        if (IsLoggingHelper() || (Level == PanicLevel))
        {
            int iCategoryLength = 0;
            int iMessageLength = 0;

            WCHAR   *pstrModuleName=NULL;
            WCHAR   *pstrMessage=NULL;
            WCHAR   wszSwitchName [MAX_LOG_SWITCH_NAME_LEN+1];
            WCHAR   *pwszMessage = NULL;

            wszSwitchName [0] = L'\0';

            if (strModule != NULL)
            {
                RefInterpretGetStringValuesDangerousForGC (
                                    strModule,
                                    &pstrModuleName,
                                    &iCategoryLength);

                if (iCategoryLength > MAX_LOG_SWITCH_NAME_LEN)
                {
                    wcsncpy_s (wszSwitchName, COUNTOF(wszSwitchName), pstrModuleName, _TRUNCATE);
                    iCategoryLength = MAX_LOG_SWITCH_NAME_LEN;
                }
                else
                {
                    wcscpy_s (wszSwitchName, COUNTOF(wszSwitchName), pstrModuleName);
                }
            }

            if (strMessage != NULL)
            {   
                RefInterpretGetStringValuesDangerousForGC (
                                    strMessage,
                                    &pstrMessage,
                                    &iMessageLength);
            }

            bool fMemAllocated = false;
                
            if ((Level == PanicLevel) && (iMessageLength == 0))
            {
                pwszMessage = L"Panic Message received";
                iMessageLength = (int)wcslen (pwszMessage);
            }
            else
            {
                pwszMessage = new WCHAR [iMessageLength + 1];

                if (pwszMessage == NULL)
                {
                    COMPlusThrowOM();
                }
                    
                if (iMessageLength != 0)
                {
                    wcsncpy_s (pwszMessage, iMessageLength + 1, pstrMessage, _TRUNCATE);
                }

                pwszMessage[iMessageLength] = L'\0';
                fMemAllocated = true;
            }

            g_pDebugInterface->SendLogMessage (Level,
                                               wszSwitchName,
                                               iCategoryLength,
                                               pwszMessage,
                                               iMessageLength);
            
            if (fMemAllocated)
            {
                delete [] pwszMessage;
            }
        }
    }

#endif // DEBUGGING_SUPPORTED

    HELPER_METHOD_FRAME_END();
}
FCIMPLEND


FCIMPL0(FC_BOOL_RET, DebugDebugger::IsLogging)
{
    WRAPPER_CONTRACT;
    
    FC_GC_POLL_RET();

    FC_RETURN_BOOL(IsLoggingHelper());
}
FCIMPLEND


FCIMPL3(void, DebugStackTrace::GetStackFramesInternal, 
        StackFrameHelper* pStackFrameHelperUNSAFE, 
        INT32 iSkip, 
        Object* pExceptionUNSAFE
       )
{    
    CONTRACTL
    {
        MODE_COOPERATIVE;
        DISABLED(GC_TRIGGERS);  // can't use this in an FCALL because we're in forbid gc mode until we setup a H_M_F.
        THROWS;
        SO_TOLERANT;
        PRECONDITION(CheckPointer(pStackFrameHelperUNSAFE));
        PRECONDITION(CheckPointer(pExceptionUNSAFE, NULL_OK));
    }
    CONTRACTL_END;   

    STACKFRAMEHELPERREF pStackFrameHelper   = (STACKFRAMEHELPERREF)ObjectToOBJECTREF(pStackFrameHelperUNSAFE);
    OBJECTREF           pException          = ObjectToOBJECTREF(pExceptionUNSAFE);

    HELPER_METHOD_FRAME_BEGIN_2(pStackFrameHelper, pException);

    HRESULT hr;
    ASSERT(iSkip >= 0);

    GetStackFramesData data;

    data.pDomain = GetAppDomain();

    data.skip = iSkip;

    data.NumFramesRequested = pStackFrameHelper->iFrameCount;

    if (pException == NULL)
    {
        // Thread is NULL if it's the current thread.
        data.TargetThread = pStackFrameHelper->TargetThread;
        GetStackFrames(NULL, (void*)-1, &data);
    }
    else
    {
        GetStackFramesFromException(&pException, &data);
    }

    if (data.cElements != 0)
    {

        MethodTable *pMT = g_Mscorlib.GetClass(CLASS__METHOD_HANDLE);
        TypeHandle arrayHandle = ClassLoader::LoadArrayTypeThrowing(TypeHandle(pMT), ELEMENT_TYPE_SZARRAY);

        // Allocate memory for the MethodInfo objects
        BASEARRAYREF MethodInfoArray = (BASEARRAYREF) AllocateArrayEx(arrayHandle,
                                                                      &data.cElements,
                                                                      1);
        //printf("\nmethod table = %X\n", pMT);

        SetObjectReference( (OBJECTREF *)&(pStackFrameHelper->rgMethodHandle), (OBJECTREF)MethodInfoArray,
                            pStackFrameHelper->GetAppDomain());

        // Allocate memory for the Offsets 
        OBJECTREF Offsets = AllocatePrimitiveArray(ELEMENT_TYPE_I4, data.cElements);

        SetObjectReference( (OBJECTREF *)&(pStackFrameHelper->rgiOffset), (OBJECTREF)Offsets,
                            pStackFrameHelper->GetAppDomain());

        // Allocate memory for the ILOffsets 
        OBJECTREF ILOffsets = AllocatePrimitiveArray(ELEMENT_TYPE_I4, data.cElements);

        SetObjectReference( (OBJECTREF *)&(pStackFrameHelper->rgiILOffset), (OBJECTREF)ILOffsets,
                            pStackFrameHelper->GetAppDomain());

        // if we need Filename, linenumber, etc., then allocate memory for the same
        // Allocate memory for the Filename string objects
        PTRARRAYREF FilenameArray = (PTRARRAYREF) AllocateObjectArray(data.cElements, g_pStringClass);

        SetObjectReference( (OBJECTREF *)&(pStackFrameHelper->rgFilename), (OBJECTREF)FilenameArray,
                            pStackFrameHelper->GetAppDomain());

        // Allocate memory for the Offsets 
        OBJECTREF LineNumbers = AllocatePrimitiveArray(ELEMENT_TYPE_I4, data.cElements);

        SetObjectReference( (OBJECTREF *)&(pStackFrameHelper->rgiLineNumber), (OBJECTREF)LineNumbers,
                            pStackFrameHelper->GetAppDomain());

        // Allocate memory for the ILOffsets 
        OBJECTREF ColumnNumbers = AllocatePrimitiveArray(ELEMENT_TYPE_I4, data.cElements);

        SetObjectReference( (OBJECTREF *)&(pStackFrameHelper->rgiColumnNumber), (OBJECTREF)ColumnNumbers,
                            pStackFrameHelper->GetAppDomain());

        // Determine if there are any dynamic methods in the stack trace.  If there are,
        // allocate an ObjectArray large enough to hold an ObjRef to each one.
        unsigned iNumDynamics = 0;
        unsigned iCurDynamic = 0;
        for (int iElement=0; iElement < data.cElements; iElement++)
        {
            MethodDesc *pMethod = data.pElements[iElement].pFunc;    
            if (pMethod->IsLCGMethod())
            {
                iNumDynamics++;
            }
        }
        
        if (iNumDynamics)
        {            
            PTRARRAYREF DynamicDataArray = (PTRARRAYREF) AllocateObjectArray(iNumDynamics, g_pObjectClass);
            
            SetObjectReference( (OBJECTREF *)&(pStackFrameHelper->dynamicMethods), (OBJECTREF)DynamicDataArray,
                                pStackFrameHelper->GetAppDomain());
        }
        
        int iNumValidFrames = 0;
        for (int i=0; i<data.cElements; i++)
        {
            size_t *pElem = (size_t*)pStackFrameHelper->rgMethodHandle->GetDataPtr();

            // The managed stacktrace classes always returns typical method definition, so we don't need to bother providing exact instantiation.
            // Generics::GetExactInstantiationsFromCallInformation(data.pElements[i].pFunc, data.pElements[i].pExactGenericArgsToken, &pExactMethod, &thExactType);
            MethodDesc* pFunc = data.pElements[i].pFunc;

            // Strip the instantiation to make sure that the reflection never gets an evil method desc back.
            if (pFunc->HasMethodInstantiation())
                pFunc = pFunc->StripMethodInstantiation();
            _ASSERTE(pFunc->IsRuntimeMethodHandle());

            pElem[iNumValidFrames] = (size_t)pFunc;

            // native offset
            I4 *pI4 = (I4 *)((I4ARRAYREF)pStackFrameHelper->rgiOffset)
                                        ->GetDirectPointerToNonObjectElements();
            pI4 [iNumValidFrames] = data.pElements[i].dwOffset; 

            // IL offset
            I4 *pILI4 = (I4 *)((I4ARRAYREF)pStackFrameHelper->rgiILOffset)
                                        ->GetDirectPointerToNonObjectElements();
            pILI4 [iNumValidFrames] = data.pElements[i].dwILOffset; 

            BOOL fFileInfoSet = FALSE;
            MethodDesc *pMethod = data.pElements[i].pFunc;
            Module *pModule = pMethod->GetModule();

            // If there are any dynamic methods, and this one is one of them, store
            // a reference to it's managed resolver to keep it alive.
            if (iNumDynamics && pMethod->IsLCGMethod())
            {
                DynamicMethodDesc *pDMD = pMethod->GetDynamicMethodDesc();
                OBJECTREF pResolver = ObjectFromHandle(pDMD->GetLCGMethodResolver()->m_managedResolver);
                _ASSERTE(pResolver != NULL);
                
                ((PTRARRAYREF)pStackFrameHelper->dynamicMethods)->SetAt (iCurDynamic++, pResolver);
            }

            bool fIsEnc = false;
            
            
            // check if the user wants the filenumber, linenumber info...
            if (!fIsEnc && pStackFrameHelper->fNeedFileInfo && CORDebuggerTrackJITInfo(pModule->GetDebuggerInfoBits()))
            {
                // Use the MethodDesc...
                ULONG32 sourceLine = 0;
                ULONG32 sourceColumn = 0;
                WCHAR wszFileName[MAX_PATH];
                ULONG32 fileNameLength = 0;

                {
                    // Note: we need to enable preemptive GC when accessing the unmanages symbol store.
                    GCX_PREEMP();

                    // Note: we use the NoThrow version of GetISymUnmanagedReader. If getting the unmanaged
                    // reader fails, then just leave the pointer NULL and leave any symbol info off of the
                    // stack trace.
                    ISymUnmanagedReader *pISymUnmanagedReader = pModule->GetISymUnmanagedReaderNoThrow();

                    if (pISymUnmanagedReader != NULL)
                    {
                        ReleaseHolder<ISymUnmanagedMethod> pISymUnmanagedMethod;  
                        hr = pISymUnmanagedReader->GetMethod(pMethod->GetMemberDef(), 
                                                &pISymUnmanagedMethod);

                        if (SUCCEEDED(hr))
                        {
                            // get all the sequence points and the documents 
                            // associated with those sequence points.
                            // from the doument get the filename using GetURL()
                            ULONG32 SeqPointCount = 0;
                            ULONG32 RealSeqPointCount = 0;

                            hr = pISymUnmanagedMethod->GetSequencePointCount(&SeqPointCount);
                            _ASSERTE (SUCCEEDED(hr) || (hr == E_OUTOFMEMORY) );

                            if (SUCCEEDED(hr) && SeqPointCount > 0)
                            {
                                // allocate memory for the objects to be fetched
                                NewArrayHolder<ULONG32> offsets    (new (nothrow) ULONG32 [SeqPointCount]);
                                NewArrayHolder<ULONG32> lines      (new (nothrow) ULONG32 [SeqPointCount]);
                                NewArrayHolder<ULONG32> columns    (new (nothrow) ULONG32 [SeqPointCount]);
                                NewArrayHolder<ULONG32> endlines   (new (nothrow) ULONG32 [SeqPointCount]);
                                NewArrayHolder<ULONG32> endcolumns (new (nothrow) ULONG32 [SeqPointCount]);

                                // we free the array automatically, but we have to manually call release
                                // on each element in the array when we're done with it.
                                NewArrayHolder<ISymUnmanagedDocument*> documents ( 
                                    (ISymUnmanagedDocument **)new PVOID [SeqPointCount]);

                                if ((offsets && lines && columns && documents && endlines && endcolumns))
                                {
                                    hr = pISymUnmanagedMethod->GetSequencePoints (
                                                        SeqPointCount,
                                                        &RealSeqPointCount,
                                                        offsets,
                                                        (ISymUnmanagedDocument **)documents,
                                                        lines,
                                                        columns,
                                                        endlines,
                                                        endcolumns);

                                    _ASSERTE(SUCCEEDED(hr) || (hr == E_OUTOFMEMORY) );

                                    if (SUCCEEDED(hr))
                                    {
                                        _ASSERTE(RealSeqPointCount == SeqPointCount);

#ifdef _DEBUG
                                        {
                                            // This is just some debugging code to help ensure that the array
                                            // returned contains valid interface pointers.
                                            for (ULONG32 i = 0; i < RealSeqPointCount; i++)
                                            {
                                                _ASSERTE(documents[i] != NULL);
                                                documents[i]->AddRef();
                                                documents[i]->Release();
                                            }
                                        }
#endif

                                        // This is the IL offset of the current frame
                                        DWORD dwCurILOffset = data.pElements[i].dwILOffset;

                                        // search for the correct IL offset
                                        DWORD j;
                                        for (j=0; j<RealSeqPointCount; j++)
                                        {
                                            // look for the entry matching the one we're looking for
                                            if (offsets[j] >= dwCurILOffset)
                                            {
                                                // if this offset is > what we're looking for, ajdust the index
                                                if (offsets[j] > dwCurILOffset && j > 0)
                                                {
                                                    j--;
                                                }

                                                break;
                                            }
                                        }

                                        // If we didn't find a match, default to the last sequence point
                                        if  (j == RealSeqPointCount)
                                        {
                                            j--;
                                        }

                                        while (lines[j] == 0x00feefee && j > 0)
                                        {
                                            j--;
                                        }

#ifdef DEBUGGING_SUPPORTED
                                        if (lines[j] != 0x00feefee)
                                        {
                                            sourceLine = lines [j];  
                                            sourceColumn = columns [j];  
                                        }
                                        else
#endif // DEBUGGING_SUPPORTED
                                        {
                                            sourceLine = 0;  
                                            sourceColumn = 0;  
                                        }

                                        // Also get the filename from the document...
                                        _ASSERTE (documents [j] != NULL);

                                        hr = documents [j]->GetURL (MAX_PATH, &fileNameLength, wszFileName);
                                        _ASSERTE ( SUCCEEDED(hr) || (hr == E_OUTOFMEMORY) );


                                        // indicate that the requisite information has been set!
                                        fFileInfoSet = TRUE;

                                        // release the documents set by GetSequencePoints
                                        for (DWORD x=0; x<RealSeqPointCount; x++)
                                        {
                                            documents [x]->Release();
                                        }
                                    } // if got sequence points

                                }  // if all memory allocations succeeded                              

                                // holders will now delete the arrays.
                            }                                
                        }
                        // Holder will release pISymUnmanagedMethod
                    }

                } // GCX_PREEMP()
                
                if (fFileInfoSet == TRUE)
                {
                    // Set the line and column numbers
                    I4 *pI4Line = (I4 *)((I4ARRAYREF)pStackFrameHelper->rgiLineNumber)
                        ->GetDirectPointerToNonObjectElements();
                    I4 *pI4Column = (I4 *)((I4ARRAYREF)pStackFrameHelper->rgiColumnNumber)
                        ->GetDirectPointerToNonObjectElements();

                    pI4Line [iNumValidFrames] = sourceLine;  
                    pI4Column [iNumValidFrames] = sourceColumn;  

                    // Set the file name
                    OBJECTREF obj = (OBJECTREF) COMString::NewString(wszFileName);
                    pStackFrameHelper->rgFilename->SetAt(iNumValidFrames, obj);
                }
            }


            if (fFileInfoSet == FALSE)
            {
                I4 *pI4Line = (I4 *)((I4ARRAYREF)pStackFrameHelper->rgiLineNumber)
                                            ->GetDirectPointerToNonObjectElements();
                I4 *pI4Column = (I4 *)((I4ARRAYREF)pStackFrameHelper->rgiColumnNumber)
                                            ->GetDirectPointerToNonObjectElements();
                pI4Line [iNumValidFrames] = 0;
                pI4Column [iNumValidFrames] = 0;

                pStackFrameHelper->rgFilename->SetAt(iNumValidFrames, NULL);
                
            }

            iNumValidFrames++;
        }

        pStackFrameHelper->iFrameCount = iNumValidFrames;

        /*
        int *pArray = (int*)OBJECTREFToObject(pStackFrameHelper->rgMethodHandle);
        printf("array { MT - %X, size = %d", pArray[0], pArray[1]);
        for (int i=0; i<pArray[1]; i++)
        {
            printf(", method desc in array[%d] = %X", i, pArray[i + 2]);
        }
        printf("}\n");
        */

    }
    else
    {
        pStackFrameHelper->iFrameCount = 0;
    }

    HELPER_METHOD_FRAME_END();
}
FCIMPLEND


void DebugStackTrace::GetStackFramesHelper(Frame *pStartFrame, 
                                           void* pStopStack, 
                                           GetStackFramesData *pData
                                          )
{
    CONTRACTL
    {
        MODE_COOPERATIVE;
        GC_TRIGGERS;
        THROWS;
    }
    CONTRACTL_END;

    ASSERT (pData != NULL);
    
    pData->cElements = 0;

    // if the caller specified (< 20) frames are required, then allocate
    // only that many
    if ((pData->NumFramesRequested > 0) && (pData->NumFramesRequested < 20))
    {
        pData->cElementsAllocated = pData->NumFramesRequested;
    }
    else
    {
        pData->cElementsAllocated = 20;
    }

    // Allocate memory for the initial 'n' frames
    pData->pElements = new DebugStackTraceElement[pData->cElementsAllocated];
    
    if (pData->TargetThread == NULL ||
        pData->TargetThread->GetInternal() == GetThread())
    {
        // Null target thread specifies current thread.
        GetThread()->StackWalkFrames(GetStackFramesCallback, pData, FUNCTIONSONLY, pStartFrame);
    }
    else
    {
        Thread *pThread = pData->TargetThread->GetInternal();
        _ASSERTE (pThread != NULL);

        // Here's the timeline for the TS_UserSuspendPending and TS_SyncSuspended bits.
        // 0) Neither TS_UserSuspendPending nor TS_SyncSuspended set.
        // 1) The suspending thread grabs the thread store lock
        //       then sets TS_UserSuspendPending
        //       then puts in place trip wires for the suspendee (if it is in managed code)
        //    and releases the thread store lock.
        // 2) The suspending thread waits for the "SafeEvent".
        // 3) The suspendee continues execution until it tries to enter preemptive mode.
        //    If it trips over the wires put in place by the suspending thread,
        //    it will try to enter preemptive mode.
        // 4) The suspendee sets TS_SyncSuspended and the "SafeEvent".
        //    Then it waits for m_UserSuspendEvent.
        // 5) AT THIS POINT, IT IS SAFE TO WALK THE SUSPENDEE'S STACK.
        // 6) Now, some thread wants to resume the suspendee.
        //    The resuming thread takes the thread store lock
        //       then clears the TS_UserSuspendPending flag
        //       then sets m_UserSuspendEvent
        //    and releases the thread store lock.
        // 7) The suspendee clears the TS_SyncSuspended flag.
        //
        // In other words, it is safe to trace the thread's stack IF we're holding the
        // thread store lock AND TS_UserSuspendPending is set AND TS_SyncSuspended is set.
        //
        // This is because:
        // - If we were not holding the thread store lock, the thread could be resumed
        //   underneath us.
        // - As long as only TS_UserSuspendPending is set (and the thread is in cooperative
        //   mode), the thread can still be executing managed code until it trips.
        // - When only TS_SyncSuspended is set, we race against it resuming execution.

        ThreadStoreLockHolder tsl(TRUE);

        // We erect a barrier so that if the thread tries to disable preemptive GC,
        // it will look at the TS_UserSuspendPending flag.  Otherwise, it could resume
        // execution of managed code during our stack walk.
        CounterHolder trapReturningThreads(&g_TrapReturningThreads);

        Thread::ThreadState state = pThread->GetSnapshotState();
        if (state & (Thread::TS_Unstarted|Thread::TS_Dead|Thread::TS_Detached))
        {
            goto LSafeToTrace;
        }

        if (state & Thread::TS_UserSuspendPending)
        {
            if (state & Thread::TS_SyncSuspended)
            {
                goto LSafeToTrace;
            }

            // The target thread is not actually suspended yet, but if it is
            // in preemptive mode, then it is still safe to trace.  Before we
            // can look at another thread's GC mode, we have to suspend it:
            // The target thread updates its GC mode flag with non-interlocked
            // operations, and Thread::SuspendThread drains the CPU's store
            // buffer (by virtue of calling GetThreadContext).
            switch (pThread->SuspendThread())
            {
            case Thread::STR_Success:
                if (!pThread->PreemptiveGCDisabledOther())
                {
                    pThread->ResumeThread();
                    goto LSafeToTrace;
                }

                // Refuse to trace the stack.
                //
                // Note that there is a pretty large window in-between when the
                // target thread sets the GC mode to cooperative, and when it
                // actually sets the TS_SyncSuspended bit.  In this window, we
                // will refuse to take a stack trace even though it would be
                // safe to do so.
                pThread->ResumeThread();
                break;
            case Thread::STR_Failure:
            case Thread::STR_NoStressLog:
                break;
            case Thread::STR_UnstartedOrDead:
                // We know the thread is not unstarted, because we checked for
                // TS_Unstarted above.
                _ASSERTE(!(state & Thread::TS_Unstarted));

                // Since the thread is dead, it is safe to trace.
                goto LSafeToTrace;
            case Thread::STR_SwitchedOut:
                if (!pThread->PreemptiveGCDisabledOther())
                {
                    goto LSafeToTrace;
                }
                break;
            default:
                UNREACHABLE();
            }
        }

        COMPlusThrow(kThreadStateException, IDS_EE_THREAD_BAD_STATE);

    LSafeToTrace:
        pThread->StackWalkFrames(GetStackFramesCallback, 
                                 pData, 
                                 FUNCTIONSONLY|ALLOW_ASYNC_STACK_WALK, 
                                 pStartFrame);
    }

    // Do a 2nd pass outside of any locks.
    // This will compute IL offsets.
    for(INT32 i = 0; i < pData->cElements; i++)
    {
        pData->pElements[i].InitPass2();
    }
    
}


void DebugStackTrace::GetStackFrames(Frame *pStartFrame, 
                                     void* pStopStack, 
                                     GetStackFramesData *pData
                                    )
{
    CONTRACTL
    {
        THROWS;
        GC_TRIGGERS;
        MODE_COOPERATIVE;
    }
    CONTRACTL_END;

    GetStackFramesHelper(pStartFrame, pStopStack, pData);
}


StackWalkAction DebugStackTrace::GetStackFramesCallback(CrawlFrame* pCf, VOID* data)
{
    CONTRACTL
    {
        THROWS;
        GC_TRIGGERS; 
        MODE_COOPERATIVE;
    }
    CONTRACTL_END;

    GetStackFramesData* pData = (GetStackFramesData*)data;

    if (pData->pDomain != pCf->GetAppDomain())
    {
        return SWA_CONTINUE;
    }

    if (pData->skip > 0) 
    {
        pData->skip--;
        return SWA_CONTINUE;
    }

    //        Can we always assume FramedMethodFrame?
    //        NOT AT ALL!!!, but we can assume it's a function
    //                       because we asked the stackwalker for it!
    MethodDesc* pFunc = pCf->GetFunction();

    if (pData->cElements >= pData->cElementsAllocated) 
    {

        DebugStackTraceElement* pTemp = new (nothrow) DebugStackTraceElement[2*pData->cElementsAllocated];
        
        if (!pTemp)
        {
            return SWA_ABORT;
        }

        memcpy(pTemp, pData->pElements, pData->cElementsAllocated * sizeof(DebugStackTraceElement));

        delete [] pData->pElements;

        pData->pElements = pTemp;
        pData->cElementsAllocated *= 2;
    }    

    SLOT ip;
    DWORD dwNativeOffset;

    if (pCf->IsFrameless())
    {
        // Real method with jitted code.
        dwNativeOffset = pCf->GetRelOffset();
        ip = (PBYTE)GetControlPC(pCf->GetRegisterSet());
    }
    else
    {
        ip = NULL;
        dwNativeOffset = 0; 
    }

    pData->pElements[pData->cElements].InitPass1(
            dwNativeOffset,
            pFunc,
            pCf->GetExactGenericArgsToken(),
            ip);

    // We'll init the IL offsets outside the TSL lock.

    
    ++pData->cElements;

    // Since we may be asynchronously walking another thread's stack,
    // check (frequently) for stack-buffer-overrun corruptions after 
    // any long operation
    pCf->CheckGSCookies();
    
    // check if we already have the number of frames that the user had asked for
    if ((pData->NumFramesRequested != 0) && (pData->NumFramesRequested <= pData->cElements))
    {
        return SWA_ABORT;
    }

    return SWA_CONTINUE;
}


void DebugStackTrace::GetStackFramesFromException(OBJECTREF * e, 
                                                  GetStackFramesData *pData
                                                 )
{
    CONTRACTL
    {
        THROWS;
        GC_TRIGGERS;
        MODE_COOPERATIVE;
        PRECONDITION (IsProtectedByGCFrame (e));
    }
    CONTRACTL_END;

    ASSERT (pData != NULL);

    // Reasonable default, will indicate error on failure
    pData->cElements = 0;

    // Get the class for the exception
    MethodTable *pExcepClass = (*e)->GetMethodTable();

    _ASSERTE(IsException(pExcepClass));     // what is the pathway for this?
    if (!IsException(pExcepClass))
    {
        return;
    }

    // Now get the _stackTrace reference
    StackTraceArray traceData;
    EXCEPTIONREF(*e)->GetStackTrace(traceData);

    GCPROTECT_BEGIN(traceData);
        // The number of frame info elements in the stack trace info
        pData->cElements = static_cast<int>(traceData.Size());

        // Now we know the size, allocate the information for the data struct
        if (pData->cElements != 0)
        {
            // Allocate the memory to contain the data
            pData->pElements = new DebugStackTraceElement[pData->cElements];

            // Fill in the data
            for (unsigned i = 0; i < (unsigned)pData->cElements; i++)
            {
                StackTraceElement const & cur = traceData[i];

                // Fill out the MethodDesc*
                MethodDesc *pMD = cur.pFunc;
                _ASSERTE(pMD);

                DWORD dwNativeOffset;
                
                if (cur.ip)
                {
                    dwNativeOffset = (DWORD)(cur.ip - (UINT_PTR)pMD->GetFunctionAddress());
                }
                else
                {
                    dwNativeOffset = 0;
                }

                pData->pElements[i].InitPass1(dwNativeOffset, pMD, cur.pExactGenericArgsToken, (SLOT) cur.ip);
                pData->pElements[i].InitPass2();            
            }
        }
        else
        {
            pData->pElements = NULL;
        }
    GCPROTECT_END();

    return;
}


// Init a stack-trace element.
// Initialization done potentially under the TSL.
void DebugStackTrace::DebugStackTraceElement::InitPass1(
    DWORD dwNativeOffset,
    MethodDesc *pFunc,
    void *pExactGenericArgsToken,
    SLOT ip
)
{
    _ASSERTE(pFunc != NULL);

    // May have a null IP for ecall frames. If IP is null, then dwNativeOffset should be 0 too.
    _ASSERTE ( (ip != NULL) || (dwNativeOffset == 0) );

    this->pFunc = pFunc;
    this->dwOffset = dwNativeOffset;
    this->ip = ip;
    this->pExactGenericArgsToken = pExactGenericArgsToken;
}

// Initialization done outside the TSL.
// This may need to call locking operations that aren't safe under the TSL.
void DebugStackTrace::DebugStackTraceElement::InitPass2()
{
    CONTRACTL
    {
        MODE_ANY;
        GC_TRIGGERS; 
        THROWS;
    }
    CONTRACTL_END;
    
    _ASSERTE(!ThreadStore::HoldingThreadStore());

    bool bRes = false;

#ifdef DEBUGGING_SUPPORTED
    // Calculate the IL offset using the debugging services
    if (this->ip != NULL)
    {
        bRes = g_pDebugInterface->GetILOffsetFromNative(
            pFunc, (LPCBYTE) this->ip, this->dwOffset, &this->dwILOffset);
    }

#endif // !DEBUGGING_SUPPORTED

    // If there was no mapping information, then set to an invalid value
    if (!bRes)
    {
        this->dwILOffset = (DWORD)-1;
    }
}


FCIMPL2(INT32, DebuggerAssert::ShowDefaultAssertDialog, 
        StringObject* strConditionUNSAFE, 
        StringObject* strMessageUNSAFE
       )
{
    CONTRACTL
    {
        MODE_COOPERATIVE;
        DISABLED(GC_TRIGGERS);  // can't use this in an FCALL because we're in forbid gc mode until we setup a H_M_F.
        THROWS;
        SO_TOLERANT;
        PRECONDITION(CheckPointer(strConditionUNSAFE, NULL_OK));
        PRECONDITION(CheckPointer(strMessageUNSAFE, NULL_OK));
    }
    CONTRACTL_END;
    
#ifdef _DEBUG
    int         result          = IDRETRY;
    STRINGREF   strCondition    = (STRINGREF) ObjectToOBJECTREF(strConditionUNSAFE);
    STRINGREF   strMessage      = (STRINGREF) ObjectToOBJECTREF(strMessageUNSAFE);

    HELPER_METHOD_FRAME_BEGIN_RET_2(strCondition, strMessage);

    StackSString condition;
    StackSString message;

    if (strCondition != NULL)
        strCondition->GetSString(condition);
    if (strMessage != NULL)
        strMessage->GetSString(message);

    StackSString msgText;
    msgText.Append(L"Expression: ");
    msgText.Append(condition);
    msgText.Append(L"\n");
    msgText.Append(L"Description: ");
    msgText.Append(message);
    msgText.Append(L"\n\n");
    msgText.Append(L"Press RETRY to attach debugger\n");

    result = EEMessageBoxNonLocalizedDebugOnly(msgText, L"Assert Failure", MB_ABORTRETRYIGNORE | MB_ICONEXCLAMATION);

    // map the user's choice to the values recognized by 
    // the System.Diagnostics.Assert package
    if (result == IDRETRY)
    {
        result = FailDebug;
    }
    else if (result == IDIGNORE)
    {
        result = FailIgnore;
    }
    else
    {
        result = FailTerminate;
    }

    HELPER_METHOD_FRAME_END();
    return result;
#else  // _DEBUG
    // This should not be called under non debug builds.
    FreeBuildDebugBreak();
    return FailTerminate;
#endif // _DEBUG
}
FCIMPLEND


FCIMPL1( void, Log::AddLogSwitch, 
         LogSwitchObject* logSwitchUNSAFE 
       )
{
    CONTRACTL
    {
        MODE_COOPERATIVE;
        DISABLED(GC_TRIGGERS);  // can't use this in an FCALL because we're in forbid gc mode until we setup a H_M_F.
        THROWS;
        SO_TOLERANT;
        PRECONDITION(CheckPointer(logSwitchUNSAFE));
    }
    CONTRACTL_END;

    Thread *pThread = GetThread();
    _ASSERTE(pThread);

    HRESULT hresult = S_OK;

    OBJECTHANDLE    ObjHandle;
        
    struct _gc {        
        LOGSWITCHREF    m_LogSwitch;
        STRINGREF       Name;
        OBJECTREF       tempObj;
        STRINGREF       strrefParentName;
    } gc;

    ZeroMemory(&gc, sizeof(gc));

    HELPER_METHOD_FRAME_BEGIN_PROTECT(gc);        
        
    gc.m_LogSwitch = (LOGSWITCHREF)ObjectToOBJECTREF(logSwitchUNSAFE);

    // Create a strong reference handle to the LogSwitch object
    ObjHandle = pThread->GetDomain()->CreateStrongHandle(NULL);
    StoreObjectInHandle( ObjHandle, ObjectToOBJECTREF(gc.m_LogSwitch));
    // Use  ObjectFromHandle(ObjHandle) to get back the object. 

    // From the given args, extract the LogSwitch name
    gc.Name = ((LogSwitchObject*) OBJECTREFToObject(gc.m_LogSwitch))->GetName();

    _ASSERTE( gc.Name != NULL );
    WCHAR *pstrCategoryName = NULL;
    int iCategoryLength = 0;
    WCHAR wszParentName [MAX_LOG_SWITCH_NAME_LEN+1];
    WCHAR wszSwitchName [MAX_LOG_SWITCH_NAME_LEN+1];
    wszParentName [0] = L'\0';
    wszSwitchName [0] = L'\0';

    // extract the (WCHAR) name from the STRINGREF object
    RefInterpretGetStringValuesDangerousForGC( gc.Name, &pstrCategoryName, &iCategoryLength );

    _ASSERTE (iCategoryLength > 0);
    wcsncpy_s(wszSwitchName, COUNTOF(wszSwitchName), pstrCategoryName, _TRUNCATE);

    // check if an entry with this name already exists in the hash table.
    // Duplicates are not allowed.
    if( g_sLogHashTable.GetEntryFromHashTable( pstrCategoryName ) != NULL )
    {
        hresult = TYPE_E_DUPLICATEID;
    }
    else
    {
        hresult = g_sLogHashTable.AddEntryToHashTable( pstrCategoryName, ObjHandle );

#ifdef DEBUGGING_SUPPORTED
        if (hresult == S_OK)
        {
            // tell the attached debugger about this switch
            if (GetThread()->GetDomain()->IsDebuggerAttached())
            {
                int iLevel = gc.m_LogSwitch->GetLevel();
                WCHAR *pstrParentName = NULL;
                int iParentNameLength = 0;

                gc.tempObj = gc.m_LogSwitch->GetParent();

                LogSwitchObject* pParent = (LogSwitchObject*) OBJECTREFToObject( gc.tempObj );

                if (pParent != NULL)
                {
                    // From the given args, extract the ParentLogSwitch's name
                    gc.strrefParentName = pParent->GetName();

                    // extract the (WCHAR) name from the STRINGREF object
                    RefInterpretGetStringValuesDangerousForGC( gc.strrefParentName, &pstrParentName, &iParentNameLength );

                    if (iParentNameLength > MAX_LOG_SWITCH_NAME_LEN)
                    {
                        wcsncpy_s (wszParentName, COUNTOF(wszParentName), pstrParentName, _TRUNCATE);
                    }
                    else
                    {
                        wcscpy_s (wszParentName, COUNTOF(wszParentName), pstrParentName);
                    }
                }

                g_pDebugInterface->SendLogSwitchSetting (iLevel, SWITCH_CREATE, wszSwitchName, wszParentName );
            }
        }   
#endif // DEBUGGING_SUPPORTED
    }

    HELPER_METHOD_FRAME_END();
}
FCIMPLEND


FCIMPL3(void, Log::ModifyLogSwitch, 
        INT32 Level, 
        StringObject* strLogSwitchNameUNSAFE, 
        StringObject* strParentNameUNSAFE
       )
{
    CONTRACTL
    {
        MODE_COOPERATIVE;
        DISABLED(GC_TRIGGERS);  // can't use this in an FCALL because we're in forbid gc mode until we setup a H_M_F.
        THROWS;
        SO_TOLERANT;
        PRECONDITION(CheckPointer(strLogSwitchNameUNSAFE));
        PRECONDITION(CheckPointer(strParentNameUNSAFE));
    }
    CONTRACTL_END;
    
    STRINGREF strLogSwitchName = (STRINGREF) ObjectToOBJECTREF(strLogSwitchNameUNSAFE);
    STRINGREF strParentName = (STRINGREF) ObjectToOBJECTREF(strParentNameUNSAFE);
    
    HELPER_METHOD_FRAME_BEGIN_2(strLogSwitchName, strParentName);

    _ASSERTE (strLogSwitchName != NULL);
    
    WCHAR *pstrLogSwitchName = NULL;
    WCHAR *pstrParentName = NULL;
    int iSwitchNameLength = 0;
    int iParentNameLength = 0;
    WCHAR wszParentName [MAX_LOG_SWITCH_NAME_LEN+1];
    WCHAR wszSwitchName [MAX_LOG_SWITCH_NAME_LEN+1];
    wszParentName [0] = L'\0';
    wszSwitchName [0] = L'\0';

    // extract the (WCHAR) name from the STRINGREF object
    RefInterpretGetStringValuesDangerousForGC (
                        strLogSwitchName,
                        &pstrLogSwitchName,
                        &iSwitchNameLength);

    if (iSwitchNameLength > MAX_LOG_SWITCH_NAME_LEN)
    {
        wcsncpy_s (wszSwitchName, COUNTOF(wszSwitchName), pstrLogSwitchName, _TRUNCATE);
    }
    else
    {
        wcscpy_s (wszSwitchName, COUNTOF(wszSwitchName), pstrLogSwitchName);
    }

    // extract the (WCHAR) name from the STRINGREF object
    RefInterpretGetStringValuesDangerousForGC (
                        strParentName,
                        &pstrParentName,
                        &iParentNameLength);

    if (iParentNameLength > MAX_LOG_SWITCH_NAME_LEN)
    {
        wcsncpy_s (wszParentName, COUNTOF(wszParentName), pstrParentName, _TRUNCATE);
    }
    else
    {
        wcscpy_s (wszParentName, COUNTOF(wszParentName), pstrParentName);
    }

#ifdef DEBUGGING_SUPPORTED
    g_pDebugInterface->SendLogSwitchSetting (Level,
                                            SWITCH_MODIFY,
                                            wszSwitchName,
                                            wszParentName
                                            );
#endif // DEBUGGING_SUPPORTED

    HELPER_METHOD_FRAME_END();
}
FCIMPLEND


void Log::DebuggerModifyingLogSwitch (int iNewLevel, 
                                      const WCHAR *pLogSwitchName
                                     )
{
    CONTRACTL
    {
        GC_NOTRIGGER;
    }
    CONTRACTL_END;

    // check if an entry with this name exists in the hash table.
    OBJECTHANDLE ObjHandle = g_sLogHashTable.GetEntryFromHashTable (pLogSwitchName);
    if ( ObjHandle != NULL)
    {
        OBJECTREF obj = ObjectFromHandle (ObjHandle);
        LogSwitchObject *pLogSwitch = (LogSwitchObject *)(OBJECTREFToObject (obj));

        pLogSwitch->SetLevel (iNewLevel);
    }
}


// Note: Caller should ensure that it's not adding a duplicate
// entry by calling GetEntryFromHashTable before calling this
// function.
HRESULT LogHashTable::AddEntryToHashTable (const WCHAR *pKey, 
                                           OBJECTHANDLE pData
                                          )
{
    CONTRACTL
    {
        THROWS;
        GC_NOTRIGGER;
    }
    CONTRACTL_END;

    HashElement *pElement;

    // check that the length is non-zero
    if (pKey == NULL)
    {
        return (E_INVALIDARG);
    }

    int iHashKey = 0;
    int iLength = (int)wcslen (pKey);

    for (int i= 0; i<iLength; i++)
    {
        iHashKey += pKey [i];
    }

    iHashKey = iHashKey % MAX_HASH_BUCKETS;

    // Create a new HashElement. This throws on oom, nothing to cleanup.
    pElement = new HashElement;
    
    pElement->SetData (pData, pKey);

    if (m_Buckets [iHashKey] == NULL)
    {
        m_Buckets [iHashKey] = pElement;
    }
    else
    {
        pElement->SetNext (m_Buckets [iHashKey]);
        m_Buckets [iHashKey] = pElement;
    }

    return S_OK;
}


OBJECTHANDLE LogHashTable::GetEntryFromHashTable (const WCHAR *pKey)
{
    CONTRACTL
    {
        THROWS;
        GC_NOTRIGGER;
    }
    CONTRACTL_END;

    if (pKey == NULL)
    {
        return NULL;
    }

    int iHashKey = 0;
    int iLength = (int)wcslen (pKey);

    // Calculate the hash value of the given key
    for (int i= 0; i<iLength; i++)
    {
        iHashKey += pKey [i];
    }

    iHashKey = iHashKey % MAX_HASH_BUCKETS;

    HashElement *pElement = m_Buckets [iHashKey];

    // Find and return the data
    while (pElement != NULL)
    {
        if (wcscmp(pElement->GetKey(), pKey) == 0)
        {
            return (pElement->GetData());
        }

        pElement = pElement->GetNext();
    }

    return NULL;
}
 
//
// Returns a textual representation of the current stack trace. The format of the stack
// trace is the same as returned by StackTrace.ToString.
//
void GetManagedStackTraceString(BOOL fNeedFileInfo, SString &result)
{
    CONTRACTL
    {
        THROWS;
        GC_TRIGGERS;
        MODE_ANY;
    }
    CONTRACTL_END;

    // Switch to cooperative GC mode before we call into managed code.
    GCX_COOP();

    MethodDescCallSite managedHelper(METHOD__STACK_TRACE__GET_MANAGED_STACK_TRACE_HELPER);
    ARG_SLOT args[] = 
    {
        (ARG_SLOT)(CLR_BOOL)!!fNeedFileInfo
    };

    STRINGREF resultStringRef = (STRINGREF) managedHelper.Call_RetOBJECTREF(args);
    resultStringRef->GetSString(result);
}

