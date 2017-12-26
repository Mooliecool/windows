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
** File:    StackBuilderSink.cpp
**
**                                              
**
** Purpose: Native implementaion for Microsoft.Runtime.StackBuilderSink
**
** Date:    Mar 24, 1999
**
===========================================================*/
#include "common.h"
#include "comstring.h"
#include "comreflectioncommon.h"
#include "excep.h"
#include "message.h"
#include "stackbuildersink.h"
#include "dbginterface.h"
#include "remoting.h"
#include "profilepriv.h"
#include "class.h"

struct ArgInfo
{
    PBYTE             dataLocation;
    INT32             dataSize;
    TypeHandle        dataTypeHandle;
    BYTE              dataType;
    BYTE              byref;
};


//+----------------------------------------------------------------------------
//
//  Method:     CStackBuilderSink::PrivateProcessMessage, public
//
//  Synopsis:   Builds the stack and calls an object
//
//+----------------------------------------------------------------------------
FCIMPL7(Object*, CStackBuilderSink::PrivateProcessMessage,
                                        Object* pSBSinkUNSAFE,
                                        MethodDesc* pMD,
                                        PTRArray* pArgsUNSAFE,
                                        Object* pServerUNSAFE,
                                        CLR_I4 __unused__iMethodPtr,
                                        CLR_BOOL fContext,
                                        PTRARRAYREF* ppVarOutParams)
{
    struct _gc
    {
        PTRARRAYREF pArgs;
        OBJECTREF pServer;
        OBJECTREF pSBSink;
        OBJECTREF ret;
    } gc;
    gc.pArgs = (PTRARRAYREF) pArgsUNSAFE;
    gc.pServer = (OBJECTREF) pServerUNSAFE;
    gc.pSBSink = (OBJECTREF) pSBSinkUNSAFE;
    gc.ret = NULL;

    HELPER_METHOD_FRAME_BEGIN_RET_ATTRIB_PROTECT(Frame::FRAME_ATTR_RETURNOBJ, gc);

    CONTRACTL
    {
        THROWS;
        GC_TRIGGERS;
        MODE_COOPERATIVE;
        SO_TOLERANT;
        PRECONDITION(!__unused__iMethodPtr);
        PRECONDITION(CheckPointer(pMD));
        PRECONDITION(!pMD->IsGenericMethodDefinition());
        PRECONDITION(pMD->IsRuntimeMethodHandle());

        // Either pServer is non-null or the method is static (but not both)
        PRECONDITION( (gc.pServer!=NULL) == !(pMD->IsStatic()) );
    }
    CONTRACTL_END;

    LOG((LF_REMOTING, LL_INFO10, "CStackBuilderSink::PrivateProcessMessage\n"));

    MethodDesc *pResolvedMD = pMD;
    // Check if this is an interface invoke, if yes, then we have to find the
    // real method descriptor on the class of the server object.
    if(pMD->GetMethodTable()->IsInterface())
    {
        _ASSERTE(gc.pServer != NULL);

        // NOTE: This method can trigger GC
        // The last parameter (true) causes the method to take into account that
        // the object passed in is a TP and try to resolve the interface MD into
        // a server MD anyway (normally the call short circuits thunking objects
        // and just returns the interface MD unchanged).
        MethodTable *pRealMT = gc.pServer->GetTrueMethodTable();

        {
            if (pRealMT->ImplementsInterface(pMD->GetMethodTable()))
            {
                pResolvedMD = pRealMT->GetMethodDescForInterfaceMethod(TypeHandle(pMD->GetMethodTable()), pMD);

                // If the method is generic then we have more work to do --
                // we'll get back the generic method descriptor and we'll have
                // to load the version with the right instantiation (get the
                // instantiation from the interface method).
                if (pResolvedMD->HasMethodInstantiation())
                {
                    _ASSERTE(pResolvedMD->IsGenericMethodDefinition());
                    _ASSERTE(pMD->GetNumGenericMethodArgs() == pResolvedMD->GetNumGenericMethodArgs());

                    pResolvedMD = MethodDesc::FindOrCreateAssociatedMethodDesc(pResolvedMD,
                                                                               pRealMT,
                                                                               FALSE,
                                                                               pMD->GetNumGenericMethodArgs(),
                                                                               pMD->GetMethodInstantiation(),
                                                                               FALSE);

                    _ASSERTE(!pResolvedMD->ContainsGenericVariables());
                }
            }
            else
                pResolvedMD = NULL;
        }

        if(!pResolvedMD)
        {
            MAKE_WIDEPTR_FROMUTF8(wName, pMD->GetName());
            COMPlusThrow(kMissingMethodException, IDS_EE_MISSING_METHOD, wName);
        }
    }

    MetaSig mSig(pResolvedMD);

    MethodDesc *pTargetMD;

    // This helps give the exact type context for generics.  
    // It is sufficient just to use the resolved type because the interface 
    // resolution was performed above.
    TypeHandle resolvedType(pResolvedMD->GetMethodTable());
    // get the target depending on whether the method is virtual or non-virtual
    // like a constructor, private or final method
    const BYTE* pTarget = MethodTable::GetTargetFromMethodDescAndServer(resolvedType,
                                                                        pResolvedMD, &(gc.pServer), fContext, &pTargetMD);

    VASigCookie *pCookie = NULL;
    _ASSERTE(NULL != pTarget);

        // this function does the work
    ::CallDescrWithObjectArray(
            gc.pServer,
            pResolvedMD,
            //pRM,
            pTarget,
            &mSig,
            pCookie,
            gc.pServer==NULL?TRUE:FALSE, //fIsStatic
            gc.pArgs,
            &gc.ret,
            ppVarOutParams);

    LOG((LF_REMOTING, LL_INFO10, "CStackBuilderSink::PrivateProcessMessage OUT\n"));

    HELPER_METHOD_FRAME_END();
    
    return OBJECTREFToObject(gc.ret);
}
FCIMPLEND

class ProfilerServerCallbackHolder
{
public:
    ProfilerServerCallbackHolder(Thread* pThread) : m_pThread(pThread)
    {
#ifdef PROFILING_SUPPORTED
        // If we're profiling, notify the profiler that we're about to invoke the remoting target
        if (CORProfilerTrackRemoting())
        {
            PROFILER_CALL;
            g_profControlBlock.pProfInterface->RemotingServerInvocationStarted(reinterpret_cast<ThreadID>(m_pThread));
        }
#endif // PROFILING_SUPPORTED
    }

    ~ProfilerServerCallbackHolder()
    {
#ifdef PROFILING_SUPPORTED
        // If profiling is active, tell profiler we've made the call, received the
        // return value, done any processing necessary, and now remoting is done.
        if (CORProfilerTrackRemoting())
        {
            PROFILER_CALL;
            g_profControlBlock.pProfInterface->RemotingServerInvocationReturned(reinterpret_cast<ThreadID>(m_pThread));
        }
#endif // PROFILING_SUPPORTED
    }

private:
    Thread* m_pThread;
};

//+----------------------------------------------------------------------------
//
//  Function:   CallDescrWithObjectArray, private
//
//  Synopsis:   Builds the stack from a object array and call the object
//
// Note this function triggers GC and assumes that pServer, pArguments, pVarRet, and ppVarOutParams are
// all already protected!!
//+----------------------------------------------------------------------------
void CallDescrWithObjectArray(OBJECTREF& pServer,
                  //ReflectMethod *pRM,
                  MethodDesc *pMeth,
                  const BYTE *pTarget,
                  MetaSig* sig,
                  VASigCookie *pCookie,
                  BOOL fIsStatic,
                  PTRARRAYREF& pArgArray,
                  OBJECTREF *pVarRet,
                  PTRARRAYREF *ppVarOutParams)
{
    CONTRACTL
    {
        THROWS;
        GC_TRIGGERS;
        MODE_COOPERATIVE;
        INJECT_FAULT(COMPlusThrowOM());
    }
    CONTRACTL_END;

    LOG((LF_REMOTING, LL_INFO10, "CallDescrWithObjectArray IN\n"));


    ByRefInfo *pByRefs = NULL;
    FrameWithCookie<ProtectValueClassFrame> *pProtectValueClassFrame = NULL;
    FrameWithCookie<ProtectByRefsFrame> *pProtectionFrame = NULL;
    INT64 retval = 0;
    INT64* pRetVal = &retval;
    UINT  nActualStackBytes = 0;
    LPBYTE pAlloc = 0;
    LPBYTE pFrameBase = 0;
    UINT32 numByRef = 0;
    //DWORD attr = pRM->dwFlags;
#ifdef _DEBUG
    //MethodDesc *pMD = pRM->pMethod;
#endif
    ValueClassInfo *pValueClasses = NULL;
    
    // check the calling convention

    BYTE callingconvention = sig->GetCallingConvention();
    if (!isCallConv(callingconvention, IMAGE_CEE_CS_CALLCONV_DEFAULT))
    {
        _ASSERTE(!"This calling convention is not supported.");
        COMPlusThrow(kInvalidProgramException);
    }

    // Make sure we are properly loaded
    CONSISTENCY_CHECK(GetAppDomain()->CheckCanExecuteManagedCode(pMeth));

    // Note this is redundant with the above but we do it anyway for safety
    pMeth->EnsureActive();

#ifdef DEBUGGING_SUPPORTED
    // debugger goo What does this do? can someone put a comment here?
    if (CORDebuggerTraceCall())
    {
        g_pDebugInterface->TraceCall(pTarget);
    }
#endif // DEBUGGING_SUPPORTED

    {
        ProfilerServerCallbackHolder profilerHolder(GetThread());

    // Create a fake FramedMethodFrame on the stack.
    nActualStackBytes = sig->SizeOfFrameArgumentArray(fIsStatic);

    UINT32 cbAlloc = 0;
    if (!ClrSafeInt<UINT32>::addition(FramedMethodFrame::GetRawNegSpaceSize(), sizeof(FramedMethodFrame), cbAlloc))
        COMPlusThrow(kArgumentException);
    if (!ClrSafeInt<UINT32>::addition(cbAlloc, nActualStackBytes, cbAlloc))
        COMPlusThrow(kArgumentException);

    pAlloc = (LPBYTE)_alloca(cbAlloc);
    pFrameBase = pAlloc + FramedMethodFrame::GetRawNegSpaceSize();

    // cycle through the parameters and see if there are byrefs
    BYTE typ = 0;
    BOOL   fHasByRefs = FALSE;

    //if (attr & RM_ATTR_BYREF_FLAG_SET)
    //    fHasByRefs = attr & RM_ATTR_HAS_BYREF_ARG;
    //else
    {
        sig->Reset();
        while ((typ = sig->NextArg()) != ELEMENT_TYPE_END)
        {
            if (typ == ELEMENT_TYPE_BYREF)
            {
                fHasByRefs = TRUE;
                //attr |= RM_ATTR_HAS_BYREF_ARG;
                break;
            }
        }
        //attr |= RM_ATTR_BYREF_FLAG_SET;
        //pRM->dwFlags = attr;
        sig->Reset();
    }

    UINT32 nFixedArgs = sig->NumFixedArgs();
    // To avoid any security problems with integer overflow/underflow we're
    // going to validate the number of args here (we're about to _alloca an
    // array of ArgInfo structures and maybe a managed object array as well
    // based on this count).
    _ASSERTE(sizeof(Object*) <= sizeof(ArgInfo));
    UINT32 nMaxArgs = UINT32_MAX / sizeof(ArgInfo);
    if (nFixedArgs > nMaxArgs)
        COMPlusThrow(kArgumentException);

    // if there are byrefs allocate and array for the out parameters
    if (fHasByRefs)
    {
        *ppVarOutParams = PTRARRAYREF(AllocateObjectArray(sig->NumFixedArgs(), g_pObjectClass));

        // Null out the array
        memset(&(*ppVarOutParams)->m_Array, 0, sizeof(OBJECTREF) * sig->NumFixedArgs());
    }

    ArgIterator argit(pFrameBase, sig, fIsStatic);

    OBJECTREF *ppThis = NULL;
    
    if (!fIsStatic)
    {
        ppThis = (OBJECTREF*)argit.GetThisAddr();

        // *ppThis is not GC protected. It will be set to the right value
        // after all object allocations are made.
        *ppThis = NULL;
    }

    // if we have the Value Class return, we need to allocate that class and place a pointer to it on the stack.

    *pVarRet = NULL;
    TypeHandle retType = sig->GetRetTypeHandleThrowing();
        // Note that we want the unnormalized (signature) type because GetStackObject
        // boxes as the element type, which if we normalized it would loose information.
    CorElementType retElemType = sig->GetReturnType();      
    if (retElemType == ELEMENT_TYPE_VALUETYPE) 
    {
        *pVarRet = retType.GetMethodTable()->Allocate();
    }
    else  {
        _ASSERTE(!sig->HasRetBuffArg());
    }

#ifdef CALLDESCR_REGTYPEMAP
    UINT64  dwRegTypeMap    = 0;
    BYTE*   pMap            = (BYTE*)&dwRegTypeMap;
    int     regArgNum = 0;

    if (!fIsStatic)
        regArgNum++;

#ifndef RETBUF_ARG_SPECIAL_PARAM
    if (sig->HasRetBuffArg())
        regArgNum++;
#endif
#endif // CALLDESCR_REGTYPEMAP

    // gather data about the parameters by iterating over the sig:
    UINT32 arg = 0;
    UINT32 structSize = 0;
    int    ofs = 0;

    ArgInfo* pArgInfoStart = (ArgInfo*) _alloca(nFixedArgs*sizeof(ArgInfo));

#ifdef _DEBUG
    // We expect to write useful data over every part of this so need
    // not do this in retail!
    memset((void *)pArgInfoStart, 0, sizeof(ArgInfo)*nFixedArgs);
#endif

    for( ; 0 != (ofs = argit.GetNextOffset(&typ, &structSize)); arg++)
    {
        CONSISTENCY_CHECK(arg < nFixedArgs);
        ArgInfo* pArgInfo = pArgInfoStart + arg;

#ifdef CALLDESCR_REGTYPEMAP
        FillInRegTypeMap(&argit, pMap, &regArgNum, &typ, structSize);
#endif

        if (typ == ELEMENT_TYPE_BYREF)
        {
            TypeHandle ty = TypeHandle();
            CorElementType brType = sig->GetByRefType(&ty);
            if (CorIsPrimitiveType(brType))
            {
                pArgInfo->dataSize = gElementTypeInfo[brType].m_cbSize;
            }
            else if (ty.IsValueType())
            {
                pArgInfo->dataSize = ty.GetMethodTable()->GetNumInstanceFieldBytes();
                numByRef ++;
            }
            else
            {
                pArgInfo->dataSize = sizeof(Object *);
                numByRef ++;
            }

            ByRefInfo *brInfo = (ByRefInfo *) _alloca(offsetof(ByRefInfo,data) + pArgInfo->dataSize);
            brInfo->argIndex = arg;
            brInfo->typ = brType;
            brInfo->typeHandle = ty;
            brInfo->pNext = pByRefs;
            pByRefs = brInfo;
            pArgInfo->dataLocation = (BYTE*)brInfo->data;
            *((void**)(pFrameBase + ofs)) = (void*)pArgInfo->dataLocation;
            pArgInfo->dataTypeHandle = ty;
            pArgInfo->dataType = brType;
            pArgInfo->byref = TRUE;
        }
        else
        {
            pArgInfo->dataLocation = pFrameBase + ofs;
            pArgInfo->dataSize = structSize;
            pArgInfo->dataTypeHandle = sig->GetLastTypeHandleThrowing(); // this may cause GC!
            pArgInfo->dataType = typ;
            pArgInfo->byref = FALSE;
        }
    }


    if (ppThis)
    {
        // If this isn't a value class, verify the objectref
#ifdef _DEBUG
        //if (pMD->GetClass()->IsValueClass() == FALSE)
        //{
        //    VALIDATEOBJECTREF(pServer);
        //}
#endif //_DEBUG
        *ppThis = pServer;
    }

    LPVOID *ppRetBuf = NULL;
    if (sig->HasRetBuffArg())
    {
        ppRetBuf = argit.GetRetBuffArgAddr();

        // We use a true boxed nullable for the return value and fix it up below
        *ppRetBuf = (*pVarRet)->GetData();
#ifdef CALLDESCR_RETBUFMARK
        // the CallDescrWorker callsite for methods with return buffer is
        //  different for RISC CPUs - we pass this information along by setting
        //  the lowest bit in pTarget
        pTarget = (const BYTE *)((UINT_PTR)pTarget | 0x1);
#endif
    }

#ifdef CALLDESCR_RETBUF
    void*   pvRetBuff = NULL;
    UINT64  cbRetBuff = 0;
    BYTE    SmallVCBuff[ENREGISTERED_RETURNTYPE_MAXSIZE];

    if (sig->HasRetBuffArg())
    {
        // Note: pvRetBuff set later once all object allocations are made
        cbRetBuff = sig->GetReturnTypeSize();
        pvRetBuff = *ppRetBuf;
    }
    else if (ELEMENT_TYPE_VALUETYPE == sig->GetReturnTypeNormalized())
    {
        ZeroMemory(SmallVCBuff, sizeof(SmallVCBuff));
        pvRetBuff = SmallVCBuff;
        cbRetBuff = sig->GetReturnTypeSize();

        _ASSERTE(cbRetBuff <= ENREGISTERED_RETURNTYPE_MAXSIZE);
    }
#endif // CALLDESCR_RETBUF

    // There should be no GC when we fill up the stack with parameters, as we don't protect them
    // Assignment of "*ppThis" above triggers the point where we become unprotected.
    {
        GCX_FORBID();

        PBYTE             dataLocation;
        INT32             dataSize;
        TypeHandle        dataTypeHandle;
        BYTE              dataType;

        OBJECTREF* pArguments = pArgArray->m_Array;
        UINT32 i;
        for (i=0; i<nFixedArgs; i++)
        {
            ArgInfo* pArgInfo = pArgInfoStart + i;

            dataSize = pArgInfo->dataSize;
            dataLocation = pArgInfo->dataLocation;
            dataTypeHandle = pArgInfo->dataTypeHandle;
            dataType = pArgInfo->dataType;

                // Nullable<T> needs special treatment, even if it is 1, 2, 4, or 8 bytes
            if (dataType == ELEMENT_TYPE_VALUETYPE)
                goto DEFAULT_CASE;
            switch (dataSize)
            {
                case 1:
                    // This "if" statement is necessary to make the assignement big-endian aware
                    if (pArgInfo->byref)
                        *((INT8*)dataLocation) = *((INT8*)pArguments[i]->GetData());
                    else
                        *(StackElemType*)dataLocation = (StackElemType)*((INT8*)pArguments[i]->GetData());
                    break;
                case 2:
                    // This "if" statement is necessary to make the assignement big-endian aware
                    if (pArgInfo->byref)
                    *((INT16*)dataLocation) = *((INT16*)pArguments[i]->GetData());
                    else
                        *(StackElemType*)dataLocation = (StackElemType)*((INT16*)pArguments[i]->GetData());
                    break;
                case 4:
                    if ((dataType == ELEMENT_TYPE_STRING)  ||
                        (dataType == ELEMENT_TYPE_OBJECT)  ||
                        (dataType == ELEMENT_TYPE_CLASS)   ||
                        (dataType == ELEMENT_TYPE_SZARRAY) ||
                        (dataType == ELEMENT_TYPE_ARRAY))
                    {
                        *(OBJECTREF *)dataLocation = pArguments[i];
                    }
                    else
                    {
                        *(StackElemType*)dataLocation = (StackElemType)*((INT32*)pArguments[i]->GetData());
                    }
                    break;

                case 8:
#if DATA_ALIGNMENT > 4
                    CopyMemory((INT32*)dataLocation, (INT32*)pArguments[i]->GetData(), 2 * sizeof(INT32));
#else
                    *((INT64*)dataLocation) = *((INT64*)pArguments[i]->GetData());
#endif
                    break;

                default:
                {
                DEFAULT_CASE:

#ifdef ENREGISTERED_PARAMTYPE_MAXSIZE
                    // We do not need to allocate a buffer if the argument is already passed by reference.
                    if (!pArgInfo->byref && (dataSize > ENREGISTERED_PARAMTYPE_MAXSIZE))
                    {
                        PVOID pvArg = _alloca(dataSize);
                        dataTypeHandle.GetMethodTable()->UnBoxIntoUnchecked(pvArg, pArguments[i]);
                        *(PVOID*)dataLocation = pvArg;

                        ValueClassInfo *pNewValueClass = (ValueClassInfo*)_alloca(sizeof(ValueClassInfo));

                        pNewValueClass->argIndex = i;
                        pNewValueClass->typ = (CorElementType)dataType;
                        pNewValueClass->typeHandle = dataTypeHandle;
                        pNewValueClass->pData = pvArg;

                        pNewValueClass->pNext = pValueClasses;
                        pValueClasses = pNewValueClass;
                    }
                    else
#endif
                    {
                        dataTypeHandle.GetMethodTable()->UnBoxIntoUnchecked(dataLocation, pArguments[i]);
                    }
                }
            }
        }

#ifdef _DEBUG
        // Should not be using this any more
        memset((void *)pArgInfoStart, 0, sizeof(ArgInfo)*nFixedArgs);
#endif

        // if there were byrefs, push a protection frame
        if (pByRefs && numByRef > 0)
        {
            char *pBuffer = (char*)_alloca (sizeof (FrameWithCookie<ProtectByRefsFrame>));
            pProtectionFrame = new (pBuffer) FrameWithCookie<ProtectByRefsFrame>(GetThread(), pByRefs);
        }

        // If there were any value classes that must be protected by the
        // caller, push a ProtectValueClassFrame.
        if (pValueClasses)
        {
            char *pBuffer = (char*)_alloca (sizeof (FrameWithCookie<ProtectValueClassFrame>));
            pProtectValueClassFrame = new (pBuffer) FrameWithCookie<ProtectValueClassFrame>(GetThread(), pValueClasses);
        }

    } // GCX_FORBID

    UINT fpReturnSize = sig->GetFPReturnSize();

    UpdateFPReturnSizeForHFAReturn(sig, &fpReturnSize);

#ifdef _PPC_
    FramedMethodFrame::Enregister(pFrameBase, sig, fIsStatic, nActualStackBytes);
#endif

    retval = CallDescrWorkerWithHandler(
                pFrameBase + sizeof(FramedMethodFrame)
#ifndef CALLDESCR_BOTTOMUP
                + nActualStackBytes
#endif
                ,
                nActualStackBytes / STACK_ELEM_SIZE,
#ifdef CALLDESCR_ARGREGS
                (ArgumentRegisters*)(pFrameBase + FramedMethodFrame::GetOffsetOfArgumentRegisters()),
#endif
#ifdef CALLDESCR_REGTYPEMAP
                dwRegTypeMap,
#endif
#ifdef CALLDESCR_RETBUF
                pvRetBuff,
                cbRetBuff,
#endif
                fpReturnSize,
                (LPVOID)pTarget,
                FALSE);

        // It is still illegal to do a GC here.  The return type might have/contain GC pointers.
    if (retElemType == ELEMENT_TYPE_VALUETYPE) 
    {
        _ASSERTE(*pVarRet != NULL);            // we have already allocated a return object

        // If the return result was passed back in registers, then copy it into the return object
        if (!sig->HasRetBuffArg())
        {
            void* retDataPtr = &retval;
#ifdef CALLDESCR_RETBUF
            if (ELEMENT_TYPE_VALUETYPE == sig->GetReturnTypeNormalized()) 
            {
                _ASSERTE(pvRetBuff == SmallVCBuff);
                retDataPtr = pvRetBuff;
            }
#endif 
            CopyValueClass((*pVarRet)->GetData(), retDataPtr, (*pVarRet)->GetMethodTable(), (*pVarRet)->GetAppDomain());
        }

        if (!retType.IsNull())
            *pVarRet = Nullable::NormalizeBox(*pVarRet);
    }
    else 
        CMessage::GetObjectFromStack(pVarRet, pRetVal, retElemType, retType);

        // You can now do GCs if you want to

    if (pProtectValueClassFrame)
        pProtectValueClassFrame->Pop();

    // extract the out args from the byrefs
    if (pByRefs)
    {
        do
        {
            // Always extract the data ptr every time we enter this loop because
            // calls to GetObjectFromStack below can cause a GC.
            // Even this is not enough, because that we are passing a pointer to GC heap
            // to GetObjectFromStack .  If GC happens, nobody is protecting the passed in pointer.

            OBJECTREF pTmp = NULL;
            void* dataLocation = pByRefs->data;
            CMessage::GetObjectFromStack(&pTmp, &dataLocation, pByRefs->typ, pByRefs->typeHandle, TRUE);
            (*ppVarOutParams)->SetAt(pByRefs->argIndex, pTmp);
            pByRefs = pByRefs->pNext;
        }
        while (pByRefs);

        if (pProtectionFrame)
            pProtectionFrame->Pop();
    }

    }  // ProfilerServerCallbackHolder


    LOG((LF_REMOTING, LL_INFO10, "CallDescrWithObjectArray OUT\n"));
}
