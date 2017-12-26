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
// NDIRECT.CPP -
//
// N/Direct support.


#include "common.h"

#include "vars.hpp"
#include "ml.h"
#include "stublink.h"
#include "threads.h"
#include "excep.h"
#include "mlgen.h"
#include "dllimport.h"
#include "cgensys.h"
#include "method.hpp"
#include "siginfo.hpp"
#include "mlcache.h"
#include "security.h"
#include "comdelegate.h"
#include "ceeload.h"
#include "mlinfo.h"
#include "eeconfig.h"
#include "eeconfig.h"
#include "comutilnative.h"
#include "corhost.h"
#include "asmconstants.h"
#include "mdaassistantsptr.h"
#include "customattribute.h"

#include "prettyprintsig.h"
#include "../md/compiler/custattr.h"
#include "comnlsinfo.h"


VOID PopulateEHSect(COR_ILMETHOD_SECT_EH* pEHSect, DWORD dwTryOffset, DWORD cbTryLength, DWORD dwHandlerOffset, DWORD cbHandlerLength, CorExceptionFlag flags);

#ifdef _DEBUG
// ml.cpp
VOID DisassembleMLStream(const MLCode *pMLCode);
#endif

#define CLR_EE_MODULE    L"mscorwks.dll"

#ifndef DACCESS_COMPILE

class StubState
{
public:
    virtual void SetStackPopSize(UINT cbStackPop) = 0;
    virtual void SetHas64BitReturn(BOOL fHas64BitReturn) = 0;
    virtual void SetLastError(BOOL fSetLastError) = 0;
    virtual void SetThisCall(BOOL fThisCall) = 0;
#ifdef LOP_FRIENDLY_FRAMES
    virtual void SetCallerPopsArgs(BOOL fCallerPopsArgs) = 0;
#endif //LOP_FRIENDLY_FRAMES
    virtual void SetDoHRESULTSwapping(BOOL fDoHRESULTSwapping) = 0;
    virtual void SetRetValIsGcRef(void) = 0;
    virtual void BeginEmit(DWORD dwStubFlags) = 0;
    virtual void SetHasThisCallHiddenArg(BOOL fHasHiddenArg) = 0;
    virtual void SetSrc(UINT uOffset) = 0;
    virtual void SetManagedReturnBuffSizeSmall(UINT cbSize) = 0;
    virtual void SetManagedReturnBuffSizeLarge(UINT cbSize) = 0;
    virtual void MarshalBlittableValueTypeReturn(UINT cbSize) = 0;
    virtual void MarshalByRefStructReturnValue(MethodTable *pMT) = 0;
    virtual void MarshalReturn(MarshalInfo* pInfo, DWORD dwStubFlags) = 0;
    virtual void MarshalArgument(MarshalInfo* pInfo, int argOffset, DWORD dwStubFlags) = 0;
    virtual void MarshalLCID(void) = 0;
    virtual void DoFixup(INT cbFixup) = 0;
    virtual void DoDstFixup(UINT cbFixup) = 0;
    virtual Stub* FinishEmit(MethodDesc* pMD,
                             VASigCookie *pVASigCookie = NULL
                             ) = 0;
};



class MLStubState : public StubState
{
public:
    MLStubState() :
      m_sl(StubTypeML),
      m_slPost(StubTypeML),
      m__slRet(StubTypeML)
    {
        CONTRACTL
        {
            THROWS;
            GC_TRIGGERS;
            MODE_ANY;
        }
        CONTRACTL_END;

        ZeroMemory(&m_header, sizeof(m_header));

        m_pslRet = &m__slRet;
    }

    void SetStackPopSize(UINT cbStackPop)  // pinvoke ML
    {
        LEAF_CONTRACT;

        m_header.m_cbStackPop = cbStackPop;
    }

    void SetHas64BitReturn(BOOL fHas64BitReturn)
    {
        LEAF_CONTRACT;

        if (fHas64BitReturn)
        {
            m_header.m_Flags |= MLHF_64BITMANAGEDRETVAL;
        }
        else
        {
            m_header.m_Flags &= ~MLHF_64BITMANAGEDRETVAL;
        }
    }

    void SetLastError(BOOL fSetLastError)  // pinvoke ML
    {
        LEAF_CONTRACT;

        if (fSetLastError)
        {
            m_header.m_Flags |= MLHF_SETLASTERROR;
        }
        else
        {
            m_header.m_Flags &= ~MLHF_SETLASTERROR;
        }
    }

    void SetThisCall(BOOL fThisCall)
    {
        LEAF_CONTRACT;

        if (fThisCall)
        {
            m_header.m_Flags |= MLHF_THISCALL;
        }
        else
        {
            m_header.m_Flags &= ~MLHF_THISCALL;
        }
    }

#ifdef LOP_FRIENDLY_FRAMES
    void SetCallerPopsArgs(BOOL fCallerPopsArgs)
    {
        LEAF_CONTRACT;

        if (fCallerPopsArgs)
        {
            m_header.m_Flags |= MLHF_LOP_FRIENDLY_FRAMES_CALLER_POPS_ARGS;
        }
        else
        {
            m_header.m_Flags &= ~MLHF_LOP_FRIENDLY_FRAMES_CALLER_POPS_ARGS;
        }
    }
#endif //LOP_FRIENDLY_FRAMES

    void SetDoHRESULTSwapping(BOOL fDoHRESULTSwapping)
    {
        LEAF_CONTRACT;

        m_fDoHRESULTSwapping = fDoHRESULTSwapping;
    }

    void SetRetValIsGcRef(void)
    {
        WRAPPER_CONTRACT;

        m_header.SetManagedRetValTypeCat(MLHF_TYPECAT_GCREF);
    }

    void BeginEmit(DWORD dwStubFlags)  // pinvoke ML
    {
        WRAPPER_CONTRACT;

        m_sl.MLEmitSpace(sizeof(m_header));
    }

    void SetHasThisCallHiddenArg(BOOL fHasHiddenArg)
    {
        LEAF_CONTRACT;

        if (fHasHiddenArg)
        {
            m_header.m_Flags |= MLHF_THISCALLHIDDENARG;
        }
        else
        {
            m_header.m_Flags &= ~MLHF_THISCALLHIDDENARG;
        }
    }

    void SetSrc(UINT uOffset)
    {
        CONTRACTL
        {
            THROWS;
            GC_TRIGGERS;
            MODE_ANY;

            PRECONDITION((INT32)uOffset == (INT32)((INT16)uOffset));
        }
        CONTRACTL_END;


        m_sl.MLEmit(ML_CAPTURE_PSRC);
        m_sl.Emit16((UINT16)uOffset);
    }

    void SetManagedReturnBuffSizeSmall(UINT cbSize)
    {
        CONTRACTL
        {
            THROWS;
            GC_TRIGGERS;
            MODE_ANY;
        }
        CONTRACTL_END;

        m_pslRet->MLEmit(ML_MARSHAL_RETVAL_SMBLITTABLEVALUETYPE_C2N);
        m_pslRet->Emit32(cbSize);
        m_pslRet->Emit16(m_sl.MLNewLocal(sizeof(BYTE*)));
    }

    void SetManagedReturnBuffSizeLarge(UINT cbSize)
    {
        CONTRACTL
        {
            THROWS;
            GC_TRIGGERS;
            MODE_ANY;
        }
        CONTRACTL_END;

        m_sl.MLEmit(ML_MARSHAL_RETVAL_LGBLITTABLEVALUETYPE_C2N);
        m_sl.Emit32(cbSize);
        m_slPost.MLEmit(ML_MARSHAL_RETVAL_LGBLITTABLEVALUETYPE_C2N_POST);
        m_slPost.Emit16(m_sl.MLNewLocal(sizeof(ML_MARSHAL_RETVAL_LGBLITTABLEVALUETYPE_C2N_SR)));
        if (!SafeAddUINT15(&(m_header.m_cbDstBuffer), StackElemSize(sizeof(LPVOID))))
        {
            COMPlusThrow(kMarshalDirectiveException, IDS_EE_SIGTOOCOMPLEX);
        }
    }

    void MarshalBlittableValueTypeReturn(UINT cbSize)
    {
        m_pslRet->MLEmit(ML_COPY_SMBLITTABLEVALUETYPE);
        m_pslRet->Emit32(cbSize);
    }
    
    void MarshalByRefStructReturnValue(MethodTable *pMT)
    {
        LEAF_CONTRACT;

        _ASSERTE(!"Should not get here");
    }

    void MarshalReturn(MarshalInfo* pInfo, DWORD dwStubFlags)  // pinvoke ML
    {
        CONTRACTL
        {
            THROWS;
            GC_TRIGGERS;
            MODE_ANY;

            PRECONDITION(CheckPointer(pInfo));
        }
        CONTRACTL_END;

        pInfo->GenerateReturnML(&m_sl,
                                m_pslRet,
                                TRUE,
                                m_fDoHRESULTSwapping);

        if (pInfo->IsFpuReturn())
        {
            if (pInfo->GetMarshalType() == MarshalInfo::MARSHAL_TYPE_DOUBLE && !m_fDoHRESULTSwapping)
            {
                m_header.m_Flags |= MLHF_64BITUNMANAGEDRETVAL;
            }

            m_header.SetManagedRetValTypeCat(MLHF_TYPECAT_FPU);
            if (!m_fDoHRESULTSwapping)
            {
                m_header.SetUnmanagedRetValTypeCat(MLHF_TYPECAT_FPU);
            }
        }

        if (!SafeAddUINT15(&(m_header.m_cbDstBuffer), pInfo->GetNativeArgSize()))
        {
            COMPlusThrow(kMarshalDirectiveException, IDS_EE_SIGTOOCOMPLEX);
        }
    }

    void MarshalArgument(MarshalInfo* pInfo, int argOffset, DWORD dwStubFlags)  // pinvoke ML
    {
        CONTRACTL
        {
            THROWS;
            GC_TRIGGERS;
            MODE_ANY;

            PRECONDITION(CheckPointer(pInfo));
        }
        CONTRACTL_END;

        pInfo->GenerateArgumentML(&m_sl, &m_slPost, argOffset, TRUE);
        if (!SafeAddUINT15(&(m_header.m_cbDstBuffer), pInfo->GetNativeArgSize()))
        {
            COMPlusThrow(kMarshalDirectiveException, IDS_EE_SIGTOOCOMPLEX);
        }

    }

    void MarshalLCID(void)  // pinvoke ML
    {
        CONTRACTL
        {
            THROWS;
            GC_TRIGGERS;
            MODE_ANY;
        }
        CONTRACTL_END;

        m_sl.MLEmit(ML_LCID_C2N);
        if (!SafeAddUINT15(&(m_header.m_cbDstBuffer), sizeof(LCID)))
        {
            COMPlusThrow(kMarshalDirectiveException, IDS_EE_SIGTOOCOMPLEX);
        }
    }

    void DoFixup(INT cbFixup)  // pinvoke ML
    {
        CONTRACTL
        {
            THROWS;
            GC_TRIGGERS;
            MODE_ANY;
        }
        CONTRACTL_END;

        if (!FitsInI2(cbFixup))
        {
            COMPlusThrow(kTypeLoadException, IDS_EE_SIGTOOCOMPLEX);
        }

        m_sl.Emit8(ML_BUMPSRC);
        m_sl.Emit16((INT16)cbFixup);
    }

    void DoDstFixup(UINT cbFixup)
    {
        CONTRACTL
        {
            THROWS;
            GC_TRIGGERS;
            MODE_ANY;

            PRECONDITION((INT32)cbFixup == (INT32)((INT16)cbFixup));
        }
        CONTRACTL_END;


        m_sl.Emit8(ML_BUMPDST);
        m_sl.Emit16((INT16)cbFixup);
    }

    Stub* FinishEmit(MethodDesc* pMD,  // pinvoke ML
                     VASigCookie *pVASigCookie
                     )
    {
        CONTRACT(Stub*)
        {
            THROWS;
            GC_TRIGGERS;
            MODE_ANY;

            POSTCONDITION(CheckPointer(RETVAL, NULL_NOT_OK));
        }
        CONTRACT_END;

        // This marker separates the pre from the post work.
        m_sl.MLEmit(ML_INTERRUPT);

        // First emit code to do any backpropagation/cleanup work (this
        // was generated into a separate stublinker during the argument phase.)
        // Then emit the code to do the return value marshaling.
        m_slPost.MLEmit(ML_END);

        if (m_pslRet == &m__slRet)
            m__slRet.MLEmit(ML_END);

        StubHolder<Stub> pStubPost(NULL);
        StubHolder<Stub> pStubRet(NULL);

        pStubPost = m_slPost.Link();

#ifdef _DEBUG
        LOG((LF_INTEROP, LL_INFO10000, "---------- POST ---------\n"));
        DisassembleMLStream((const MLCode *)pStubPost->GetEntryPoint());
#endif

        if (m_pslRet == &m__slRet)
        {
            pStubRet = m__slRet.Link();

#ifdef _DEBUG
            LOG((LF_INTEROP, LL_INFO10000, "------ POST RETURN ------\n"));
            DisassembleMLStream((const MLCode *)pStubRet->GetEntryPoint());
#endif
        }

        if (m_fDoHRESULTSwapping)
        {
            m_sl.MLEmit(ML_THROWIFHRFAILED);
        }

        m_sl.EmitBytes(pStubPost->GetEntryPoint(), MLStreamLength((const UINT8 *)(pStubPost->GetEntryPoint())) - 1);

        if (pStubRet)
        {
        m_sl.EmitBytes(pStubRet->GetEntryPoint(), MLStreamLength((const UINT8 *)(pStubRet->GetEntryPoint())) - 1);
        }

        m_sl.MLEmit(ML_END);

        Stub* pStub = m_sl.Link();

#ifdef _DEBUG
        LOG((LF_INTEROP, LL_INFO10000, "---------- COMPLETE ----------\n"));
        DisassembleMLStream((const MLCode *)(pStub->GetEntryPoint() + sizeof(MLHeader)));
#endif

        m_header.m_cbLocals = m_sl.GetLocalSize();

        *((MLHeader *)(pStub->GetEntryPoint())) = m_header;

#ifdef _DEBUG
        {
            MLHeader *pheader = (MLHeader*)(pStub->GetEntryPoint());
            UINT16 locals = 0;
            MLCode opcode;
            const MLCode *pMLCode = pheader->GetMLCode();
            bool fAnyVariableSizeLocals = false;

            while (ML_INTERRUPT != (opcode = *(pMLCode++)))
            {
#ifdef ENREGISTERED_PARAMTYPE_MAXSIZE
                if (ML_COPYCTOR_C2N == opcode)
                {
                    // For large blittable types we may need to make a copy of
                    // the struct.
                    fAnyVariableSizeLocals = true;
                }
#endif // ENREGISTERED_PARAMTYPE_MAXSIZE

                locals += gMLInfo[opcode].m_cbLocal;
                pMLCode += gMLInfo[opcode].m_numOperandBytes;
            }

            if (fAnyVariableSizeLocals)
                _ASSERTE(locals <= pheader->m_cbLocals);
            else
                _ASSERTE(locals == pheader->m_cbLocals);
        }
#endif //_DEBUG

        RETURN pStub;
    }

protected:
    InteropStubLinker m_sl;
    InteropStubLinker m_slPost;
    InteropStubLinker m__slRet;
    InteropStubLinker* m_pslRet;

    MLHeader     m_header;
    BOOL         m_fDoHRESULTSwapping;
};




VOID NDirect_Prelink(MethodDesc *pMeth);

Stub* g_pGenericPInvokeCalliStub = NULL;



NDirectMLStubCache* NDirect::m_pNDirectStubCache        = NULL;
ArgBasedStubCache*  NDirect::m_pNDirectGenericStubCache = NULL;
ArgBasedStubCache*  NDirect::m_pNDirectSlimStubCache    = NULL;

class NDirectMLStubCache : public MLStubCache
{
    public:
        NDirectMLStubCache(LoaderHeap *heap = 0) : MLStubCache(heap) { WRAPPER_CONTRACT; }

    private:
        //---------------------------------------------------------
        // Compile a native (ASM) version of the ML stub.
        //
        // This method should compile into the provided stublinker (but
        // not call the Link method.)
        //
        // It should return the chosen compilation mode.
        //
        // If the method fails for some reason, it should return
        // INTERPRETED so that the EE can fall back on the already
        // created ML code.
        //---------------------------------------------------------
        virtual MLStubCompilationMode CompileMLStub(const BYTE *pRawMLStub,
                                                    StubLinker *pstublinker,
                                                    void *callerContext);

        //---------------------------------------------------------
        // Tells the MLStubCache the length of an ML stub.
        //---------------------------------------------------------
        virtual UINT Length(const BYTE *pRawMLStub)
        {
            CONTRACTL
            {
                NOTHROW;
                GC_NOTRIGGER;
                MODE_ANY;

                PRECONDITION(CheckPointer(pRawMLStub));
            }
            CONTRACTL_END;

            MLHeader *pmlstub = (MLHeader *)pRawMLStub;
            return sizeof(MLHeader) + MLStreamLength(pmlstub->GetMLCode());
        }
};


//---------------------------------------------------------
// Compile a native (ASM) version of the ML stub.
//
// This method should compile into the provided stublinker (but
// not call the Link method.)
//
// It should return the chosen compilation mode.
//
// If the method fails for some reason, it should return
// INTERPRETED so that the EE can fall back on the already
// created ML code.
//---------------------------------------------------------
MLStubCache::MLStubCompilationMode NDirectMLStubCache::CompileMLStub(const BYTE *pRawMLStub,
                     StubLinker *pstublinker, void *callerContext)
{
    CONTRACTL
    {
        THROWS;
        GC_TRIGGERS;
        MODE_ANY;

        PRECONDITION(CheckPointer(pRawMLStub));
        PRECONDITION(CheckPointer(pstublinker));
        PRECONDITION(callerContext == 0);
    }
    CONTRACTL_END;

    MLStubCompilationMode ret = INTERPRETED;

    EX_TRY
    {
        CPUSTUBLINKER *psl = (CPUSTUBLINKER *)pstublinker;
        const MLHeader *pheader = (const MLHeader *)pRawMLStub;

#ifdef _DEBUG
        if (LoggingEnabled() && (g_pConfig->GetConfigDWORD(L"LogFacility",0) & LF_IJW))
        {
            goto lDone;
        }
#endif

            if (NDirect::CreateStandaloneNDirectStubSys(pheader, (CPUSTUBLINKER*)psl, FALSE))
            {
                ret = STANDALONE;
                goto lDone;
            }

lDone: ;
    }
    EX_CATCH
    {
        ret = INTERPRETED;
    }
    EX_END_CATCH(RethrowTerminalExceptions);


    return ret;
}

/*static*/
LPVOID NDirect::NDirectGetEntryPoint(NDirectMethodDesc *pMD, HINSTANCE hMod, UINT16 numParamBytes)
{
    // GetProcAddress cannot be called while preemptive GC is disabled.
    // It requires the OS to take the loader lock.
    CONTRACT(LPVOID)
    {
        THROWS;
        GC_TRIGGERS;
        MODE_PREEMPTIVE;

        PRECONDITION(CheckPointer(pMD));
        POSTCONDITION(CheckPointer(RETVAL, NULL_OK));
    }
    CONTRACT_END;

    g_IBCLogger.LogStoredMethodDataAccess(pMD);

    void * func = NULL;


    //  check if we can resolve the entry point by the ordinal
    WORD ordinal = pMD->ndirect.m_wEntryPointOrdinal;
    if ( ordinal != 0 )
    {
        EX_TRY
        {
            PEDecoder decoder( hMod );

            //  check the identity of the library module
            if ( pMD->ndirect.m_dwLibIdentity == decoder.GetImageIdentity() )
            {
                func = (void *) GetProcAddress( hMod, (LPCSTR)(size_t)((UINT16)ordinal) );

#if defined( _DEBUG )
                if ( RunningOnWinNT() )
                    // dynamic function loading is a complete mess on win9x; can't do a simple entry point compare
                {
                    //  verify that fast and slow paths are identical
                    void * debug_func = NULL;
                    pMD->FindEntryPoint( hMod, numParamBytes, &debug_func, NULL );
                    CONSISTENCY_CHECK( func == NULL || func == debug_func );
                }
#endif
            }
        }
        EX_CATCH {}
        EX_END_CATCH( SwallowAllExceptions );
    }

    if ( ! func )
        pMD->FindEntryPoint( hMod, numParamBytes, &func, NULL );

    RETURN func;
}

static BOOL AbsolutePath(LPCWSTR wszLibName, DWORD* pdwSize)
{
    CONTRACTL
    {
        NOTHROW;
        GC_NOTRIGGER;
        MODE_ANY;

        PRECONDITION(CheckPointer(wszLibName));
    }
    CONTRACTL_END;

    // check for UNC or a drive
    WCHAR* ptr = (WCHAR*) wszLibName;
    WCHAR* start = ptr;
    *pdwSize = 0;
    start = ptr;

    // Check for UNC path
    while(*ptr)
    {
        if(*ptr != L'\\')
            break;
        ptr++;
    }

    if((ptr - wszLibName) == 2)
        return TRUE;
    else
    {
        // Check to see if there is a colon indicating a drive or protocal
        for(ptr = start; *ptr; ptr++)
        {
            if(*ptr == L':')
                break;
        }
        if(*ptr != NULL)
            return TRUE;
    }

    // We did not find a
    *pdwSize = (DWORD)(ptr - wszLibName);
    return FALSE;
}

VOID NDirectMethodDesc::SetNDirectTarget(LPVOID pTarget)
{
    CONTRACTL
    {
        THROWS;
        GC_TRIGGERS;
        MODE_ANY;

        PRECONDITION(IsNDirect());
        PRECONDITION(pTarget != NULL);
    }
    CONTRACTL_END;
    

    Stub *pHostHookStub = NULL;

    if (NDirect::IsHostHookEnabled())
    {
        // The m_pNativeNDirectTarget is initialized only if host hook is enabled 
        // to save ngen private working set
        ndirect.m_pNativeNDirectTarget = pTarget;

        pHostHookStub = CheckForHostHook(pTarget);
    }

    NDirectWriteableData* pWriteableData = GetWriteableData();
    g_IBCLogger.LogStoredMethodDataAccess(this);

    if (pHostHookStub != NULL)
    {
        LPVOID oldTarget = GetNDirectImportThunkGlue()->GetEntrypoint();
        if (FastInterlockCompareExchangePointer(&pWriteableData->m_pNDirectTarget, (PVOID)pHostHookStub->GetEntryPoint(),
                                                oldTarget) != oldTarget)
        {
            pHostHookStub->DecRef();
        }
    }
    else
    {
        pWriteableData->m_pNDirectTarget = pTarget;
    }
}

BOOL NDirect::IsHostHookEnabled()
{
    WRAPPER_CONTRACT;
    //
    // WARNING: The non-debug portion of this logic is inlined into 
    //          UMThunkStubIA64 and UMThunkStubAMD64!
    //
    return CLRTaskHosted() INDEBUG(|| g_pConfig->ShouldGenerateStubForHost());
}

EXTERN_C BOOL CallNeedsHostHook(size_t target)
{
    BOOL fHook = FALSE;
    IHostTaskManager *pManager = CorHost2::GetHostTaskManager();
    if (pManager)
    {
        HRESULT hr;
        BEGIN_SO_TOLERANT_CODE_CALLING_HOST(GetThread());
        hr = pManager->CallNeedsHostHook(target,&fHook);
        END_SO_TOLERANT_CODE_CALLING_HOST;
        _ASSERTE (hr == S_OK);
    }
#ifdef _DEBUG
    else
    {
        if (g_pConfig->ShouldGenerateStubForHost())
        {
            fHook = TRUE;
        }
    }
#endif
    return fHook;
}

#ifdef _X86_
#endif  // _AMD64_

Stub* NDirectMethodDesc::CheckForHostHook(LPVOID pNativeTarget)
{
    CONTRACTL
    {
        THROWS;
        GC_TRIGGERS;
        MODE_ANY;
    }
    CONTRACTL_END;

    BOOL fHook = CallNeedsHostHook((size_t)pNativeTarget);

#ifdef _DEBUG
    if (g_pConfig->ShouldGenerateStubForHost())
    {
        fHook = TRUE;
    }
#endif

    if (!fHook)
        return NULL;

    InterlockedSetNDirectFlags(kNeedHostHook);

#ifdef _X86_
    MLHeader *pMLHeader = NULL;
    StubHolder<Stub>  pTempMLStub(NULL);

    // the ML header will already be set if we were prejitted
    pMLHeader = GetMLHeader();

    if (pMLHeader == NULL) {
        GCX_COOP();
        pTempMLStub = NDirect::ComputeNDirectMLStub(this);
    }

    return GenerateStubForHost(pNativeTarget);
#else
    // WIN64 calls the host hooks in the static assembly helpers (e.g. DoNDirectCall(), DoCLRToCOMCall(), etc.).
    // On AMD64 we could potentially do what x86 does, to generate a wrapper stub if host hooks need to be called.
    // Then we don't need to check CallNeedsHostHook() every time in the static assembly helpers.
    return NULL;
#endif
}


// Preserving good error info from DllImport-driven LoadLibrary is tricky because we keep loading from different places
// if earlier loads fail and those later loads obliterate error codes.
//
// This tracker object will keep track of the error code in accordance to priority:
//
//   low-priority:      unknown error code (should never happen)
//   medium-priority:   dll not found
//   high-priority:     dll found but error during loading
//   
// We will overwrite the previous load's error code only if the new error code is higher priority.
//

class LoadLibErrorTracker
{
    public:
        LoadLibErrorTracker()
        {
            LEAF_CONTRACT;
            m_hr = E_FAIL;
            m_priorityOfLastError = 0;
        }

        VOID TrackErrorCode(DWORD dwLastError)
        {
            LEAF_CONTRACT;

            DWORD priority;

            switch (dwLastError)
            {
                case ERROR_FILE_NOT_FOUND:
                case ERROR_PATH_NOT_FOUND:
                case ERROR_MOD_NOT_FOUND:
                case ERROR_DLL_NOT_FOUND:
                    priority = 10;
                    break;

                // If we can't access a location, we can't know if the dll's there or if it's good.
                // Still, this is probably more unusual (and thus of more interest) than a dll-not-found
                // so give it an intermediate priority.
                case ERROR_ACCESS_DENIED:
                    priority = 20;

                // Assume all others are "dll found but couldn't load."
                default:
                    priority = 99999;
                    break;
            }

            if (priority > m_priorityOfLastError)
            {
                m_hr                  = HRESULT_FROM_WIN32(dwLastError);
                m_priorityOfLastError = priority;
            }
        }

        HRESULT GetHR()
        {
            return m_hr;
        }

    private:
        HRESULT m_hr;
        DWORD   m_priorityOfLastError;
};


namespace
{
    //  Local helper function for the LoadLibraryModule below
    HMODULE LocalLoadLibraryHelper( LPCWSTR name, DWORD )
    {
        return CLRLoadLibrary( name );
    }
}


#define CHECK_LOAD_RESULT_AND_SET_FLAG(__hmod, __pErrorTracker)                                     \
    if (((__hmod) == NULL) && ((__pErrorTracker) != NULL))                                          \
    {                                                                                               \
        (__pErrorTracker)->TrackErrorCode(GetLastError());                                          \
    }                                                                                               \




/* static */
HINSTANCE NDirect::LoadLibraryModule( NDirectMethodDesc * pMD, LPCUTF8 name, bool loadToExecute, LoadLibErrorTracker *pErrorTracker /*= NULL*/ )
{
    CONTRACTL
    {
        THROWS;
        GC_TRIGGERS;
        MODE_ANY;

        PRECONDITION( CheckPointer( pMD ) );
        PRECONDITION( CheckPointer( name, NULL_OK ) );
    }
    CONTRACTL_END;
    
    if ( !name || !*name )
        return NULL;
    
    ModuleHandleHolder hmod;
    DWORD loadFlags = 0;
    DWORD loadWithAlteredPathFlags = 0;

    PREFIX_ASSUME( name != NULL );
    MAKE_WIDEPTR_FROMUTF8( wszLibName, name );

    Thread *pThread = GetThread();
    {
        GCX_PREEMP();

        AppDomain* pDomain = pThread->GetDomain();

        hmod = pDomain->FindUnmanagedImageInCache(wszLibName);

        if(hmod == NULL)
        {
            DWORD dwSize = 0;


            if (hmod == NULL)
            {
                if(AbsolutePath(wszLibName, &dwSize))
                {
                    hmod = LocalLoadLibraryHelper(wszLibName, loadWithAlteredPathFlags);
                    CHECK_LOAD_RESULT_AND_SET_FLAG(hmod, pErrorTracker);
                }
                else
                {
                    // Try to load the DLL alongside the assembly where the PInvoke was 
                    // declared using the path of the assembly.
                    Assembly* pAssembly = pMD->GetMethodTable()->GetAssembly();

                    SString path = pAssembly->GetManifestFile()->GetPath();
                    SString::Iterator i = path.End();

                    if (PEAssembly::FindLastPathSeparator(path, i))
                    {
                        i++;
                        path.Truncate(i);

                        path.Append(wszLibName);

                        hmod = LocalLoadLibraryHelper(path, loadWithAlteredPathFlags);
                        CHECK_LOAD_RESULT_AND_SET_FLAG(hmod, pErrorTracker);
                    }
                    
                    if (hmod == NULL)
                    {                
                        // Try to load the DLL alongside the assembly where the PInvoke was 
                        // declared using the codebase of the assembly. This is required for download
                        // and shadow copy scenarios.
                        const WCHAR* ptr;
                        SString codebase;                                            
                        pAssembly->GetCodeBase(codebase);
                        DWORD dwCodebaseLength = codebase.GetCount();
                                            
                        // Strip off the protocol
                        for (ptr = codebase.GetUnicode(); *ptr && *ptr != L':'; ptr++);

                        // If we have a code base then prepend it to the library name
                        if (*ptr) 
                        {
                            SString pathFromCodebase;

                            // After finding the colon move forward until no more forward slashes
                            for(ptr++; *ptr && *ptr == L'/'; ptr++);
                            if (*ptr) 
                            {
                                // Calculate the number of characters we are interested in
                                if (dwCodebaseLength > (DWORD)(ptr - codebase.GetUnicode()) )
                                {
                                    // Back up to the last slash (forward or backwards)
                                    const WCHAR* tail;

                                    for (tail = codebase.GetUnicode() + (dwCodebaseLength - 1); tail > ptr && *tail != L'/' && *tail != L'\\'; tail--);

                                    if (tail > ptr) 
                                    {
                                        for(;ptr <= tail; ptr++) 
                                        {
                                            if(*ptr == L'/') 
                                                pathFromCodebase.Append(L'\\');
                                            else
                                                pathFromCodebase.Append(*ptr);
                                        }
                                    }
                                }
                            }

                            pathFromCodebase.Append(wszLibName);

                            if (!pathFromCodebase.EqualsCaseInsensitive(path, PEImage::GetFileSystemLocale()))
                            {
                                hmod = LocalLoadLibraryHelper(pathFromCodebase, loadWithAlteredPathFlags);
                                CHECK_LOAD_RESULT_AND_SET_FLAG(hmod, pErrorTracker);
                            }
                        }
                    }
                }
            }

            // Do we really need to do this. This call searches the application directory
            // instead of the location for the library.
            if(hmod == NULL)
            {
                hmod = LocalLoadLibraryHelper(wszLibName, loadFlags);
                CHECK_LOAD_RESULT_AND_SET_FLAG(hmod, pErrorTracker);
            }

            // This may be an assembly name
            if (!hmod)
            {
                // Format is "fileName, assemblyDisplayName"
                MAKE_UTF8PTR_FROMWIDE(szLibName, wszLibName);
                char *szComma = strchr(szLibName, ',');
                if (szComma)
                {
                    *szComma = '\0';
                    while (COMCharacter::nativeIsWhiteSpace(*(++szComma)));

                    AssemblySpec spec;
                    if (SUCCEEDED(spec.Init(szComma)))
                    {
                        // Need to perform case insensitive hashing.
                        CQuickBytes qbLC;
                        {
                            UTF8_TO_LOWER_CASE(szLibName, qbLC);
                            szLibName = (LPUTF8) qbLC.Ptr();
                        }

                        Assembly *pAssembly = spec.LoadAssembly(FILE_LOADED);
                        Module *pModule = pAssembly->FindModuleByName(szLibName);

                        hmod = LocalLoadLibraryHelper(pModule->GetPath(), loadWithAlteredPathFlags);
                        CHECK_LOAD_RESULT_AND_SET_FLAG(hmod, pErrorTracker);
                    }
                }
            }

            // After all this, if we have a handle add it to the cache.
            if (hmod && loadToExecute)
            {
                pDomain->AddUnmanagedImageToCache(wszLibName, hmod);
            }
        }
    }

    return hmod.Extract();
}

#undef CHECK_LOAD_RESULT_AND_SET_FLAG


//---------------------------------------------------------
// Loads the DLL and finds the procaddress for an N/Direct call.
//---------------------------------------------------------
/* static */
VOID NDirect::NDirectLink(NDirectMethodDesc *pMD, UINT16 numParamBytes)
{
    CONTRACTL
    {
        THROWS;
        GC_TRIGGERS;
        MODE_COOPERATIVE;

        PRECONDITION(CheckPointer(pMD));
    }
    CONTRACTL_END;

    // Loading unmanaged dlls can trigger dllmains which certainly count as code execution!
    pMD->EnsureActive();

    BOOL fSuccess = FALSE;
    LoadLibErrorTracker errorTracker;


    HINSTANCE hmod;

    {
        GCX_PREEMP();

        hmod = LoadLibraryModule( pMD, pMD->GetLibName(), true, &errorTracker );
        if ( hmod )
        {
            LPVOID pvTarget = NDirectGetEntryPoint(pMD, hmod, numParamBytes);
            if (pvTarget)
            {

                pMD->SetNDirectTarget(pvTarget);
                fSuccess = TRUE;
            }
        }
    }

    if (!fSuccess)
    {
        if (pMD->GetLibName() == NULL)
            COMPlusThrow(kEntryPointNotFoundException, IDS_EE_NDIRECT_GETPROCADDRESS_NONAME);
        
        MAKE_WIDEPTR_FROMUTF8(wszLibName, pMD->GetLibName());

        if (!hmod)
        {
            HRESULT theHRESULT = errorTracker.GetHR();
            if (theHRESULT == HRESULT_FROM_WIN32(ERROR_BAD_EXE_FORMAT))
            {
                COMPlusThrow(kBadImageFormatException);
            }
            else
            {
                SString hrString;
                GetHRMsg(theHRESULT, hrString);
                COMPlusThrow(kDllNotFoundException, IDS_EE_NDIRECT_LOADLIB, wszLibName, hrString);
            }
        }

        WCHAR wszEPName[50];
        if(WszMultiByteToWideChar(CP_UTF8, 0, (LPCSTR)pMD->GetEntrypointName(), -1, wszEPName, sizeof(wszEPName)/sizeof(WCHAR)) == 0)
        {
            wszEPName[0] = L'?';
            wszEPName[1] = L'\0';
        }

        COMPlusThrow(kEntryPointNotFoundException, IDS_EE_NDIRECT_GETPROCADDRESS, wszLibName, wszEPName);
    }
}






//---------------------------------------------------------
// One-time init
//---------------------------------------------------------
/*static*/ void NDirect::Init()
{
    CONTRACTL
    {
        THROWS;
        GC_TRIGGERS;
        MODE_ANY;
        INJECT_FAULT(COMPlusThrowOM());
    }
    CONTRACTL_END;



    m_pNDirectStubCache        = new NDirectStubCache(SystemDomain::System()->GetStubHeap());
    m_pNDirectGenericStubCache = new ArgBasedStubCache();
    m_pNDirectSlimStubCache    = new ArgBasedStubCache();
}


//---------------------------------------------------------
// Computes an ML stub for the ndirect method, and fills
// in the corresponding fields of the method desc. Note that
// this does not set the m_pMLHeader field of the method desc,
// since the stub may end up being replaced by a compiled one
//---------------------------------------------------------
/* static */
Stub* NDirect::ComputeNDirectMLStub(NDirectMethodDesc *pMD, bool computeToExecute /*=true*/)
{
    CONTRACT(Stub*)
    {
        THROWS;
        GC_TRIGGERS;
        MODE_ANY;

        PRECONDITION(CheckPointer(pMD));
        POSTCONDITION(CheckPointer(RETVAL, NULL_OK));
    }
    CONTRACT_END;

    PInvokeStaticSigInfo sigInfo;
    NDirect::PopulateNDirectMethodDesc(pMD, &sigInfo);

    LPCUTF8 szEntryPointName = pMD->GetEntrypointName();
    BOOL fVarArg = pMD->MethodDesc::IsVarArg();

    if (fVarArg && (szEntryPointName != NULL))
        RETURN NULL;

    if (pMD->IsVarArgs())
        RETURN NULL;


    Stub *pMLStub = NULL;
    pMLStub = CreateNDirectStub(&sigInfo, NDIRECTSTUB_FL_FORCEML, pMD, NULL
        DEBUG_ARG(pMD->m_pszDebugMethodName) DEBUG_ARG(pMD->m_pszDebugClassName) DEBUG_ARG(NULL));

    MLHeader *pMLHeader = (MLHeader*) pMLStub->GetEntryPoint();

    pMD->SetStackArgumentSize(pMLHeader->m_cbDstBuffer, sigInfo.GetCallConv());

    RETURN pMLStub;
}

//---------------------------------------------------------
// Either creates or retrieves from the cache, a stub to
// invoke NDirect methods. Each call refcounts the returned stub.
// This routines throws a COM+ exception rather than returning
// NULL.
//---------------------------------------------------------
/* static */
Stub* NDirect::GetNDirectMethodStub(StubLinker *pstublinker, NDirectMethodDesc *pMD)
{
    CONTRACT(Stub*)
    {
        THROWS;
        GC_TRIGGERS;
        MODE_ANY;

        PRECONDITION(CheckPointer(pstublinker));
        PRECONDITION(CheckPointer(pMD));
        POSTCONDITION(CheckPointer(RETVAL, NULL_OK));
    }
    CONTRACT_END;

    MLHeader *pOldMLHeader = NULL;
    MLHeader *pMLHeader = NULL;
    StubHolder<Stub>  pTempMLStub(NULL);
    StubHolder<Stub>  pReturnStub(NULL);

   
    // the ML header will already be set if we were prejitted
    g_IBCLogger.LogStoredMethodDataAccess(pMD);
    pOldMLHeader = pMD->GetMLHeader();
    pMLHeader = pOldMLHeader;

    if (pMLHeader == NULL)
    {
        pTempMLStub = ComputeNDirectMLStub(pMD);
        if (pTempMLStub != NULL)
            pMLHeader = (MLHeader *) pTempMLStub->GetEntryPoint();
    }

    if (pMD->IsEarlyBound())
    {
    }
    else
    {
        NDirectLink(pMD, pMD->IsStdCall() ? pMLHeader->m_cbDstBuffer : 0xffff);
    }

    MLStubCache::MLStubCompilationMode mode;
    StubHolder<Stub> pCanonicalStub;

    if (pMLHeader == NULL)
    {
        pCanonicalStub = NULL;
        mode = MLStubCache::INTERPRETED;
    }
    else
    {
        pCanonicalStub = NDirect::m_pNDirectStubCache->Canonicalize((const BYTE *) pMLHeader,
                                                                          &mode);
        if (!pCanonicalStub)
            COMPlusThrowOM();
    }

    switch (mode)
    {
        case MLStubCache::INTERPRETED:

            if (pCanonicalStub != NULL) // it will be null for varags case
            {
                if (pMD->InterlockedReplaceMLHeader((MLHeader*)pCanonicalStub->GetEntryPoint(), pOldMLHeader))
                {
                    pCanonicalStub.SuppressRelease();
                }
            }

            if ( (pMLHeader == NULL) 
                 INDEBUG( || (LoggingEnabled() && (LF_IJW & g_pConfig->GetConfigDWORD(L"LogFacility", 0))) )
               )
            {
                    pReturnStub = NULL;
            }
            else
            {
                {
                    pReturnStub = CreateSlimNDirectStub(pstublinker, pMD, pMLHeader->m_cbStackPop);
                }
            }

            if (!pReturnStub)
            {
                // Note that we don't want to call CbStackBytes unless
                // strictly necessary since this will touch metadata.
                // Right now it will only happen for the varags case,
                // which probably needs other tuning anyway.
                pReturnStub = CreateGenericNDirectStub(pstublinker,
                                                        pMLHeader != NULL ?
                                                        pMLHeader->m_cbStackPop :
                                                        pMD->CbStackPop());
            }
            break;

        case MLStubCache::STANDALONE:
            pReturnStub.Assign(pCanonicalStub);
            pCanonicalStub.SuppressRelease();
            break;

        default:
            _ASSERTE(0);
    }

    if (pReturnStub)
        pReturnStub.SuppressRelease();

    RETURN pReturnStub;
}

//---------------------------------------------------------
// Creates or retrives from the cache, the generic NDirect stub.
//---------------------------------------------------------
/* static */
Stub* NDirect::CreateGenericNDirectStub(StubLinker *pstublinker, UINT numStackBytes)
{
    CONTRACT(Stub*)
    {
        THROWS;
        GC_TRIGGERS;
        MODE_ANY;

        PRECONDITION(CheckPointer(pstublinker));
        POSTCONDITION(CheckPointer(RETVAL, NULL_OK));
    }
    CONTRACT_END;

    Stub *pStub = m_pNDirectGenericStubCache->GetStub(numStackBytes);
    if (pStub) {
        RETURN pStub;
    }

    CPUSTUBLINKER *psl = (CPUSTUBLINKER*)pstublinker;

    CreateGenericNDirectStubSys(psl, numStackBytes);

    // Process-wide stubs based on number of stack bytes.  We never unload these.
    StubHolder<Stub> pCandidate(psl->Link(SystemDomain::System()->GetStubHeap()));
    Stub *pWinner = m_pNDirectGenericStubCache->AttemptToSetStub(numStackBytes,pCandidate);
    if (!pWinner) {
        COMPlusThrowOM();
    }
    RETURN pWinner;
}

//---------------------------------------------------------
// Helper function to checkpoint the thread allocator for cleanup.
//---------------------------------------------------------

VOID __stdcall DoCheckPointForCleanup(NDirectMethodFrameEx *pFrame, Thread *pThread)
{
    // note in retail this function is actually nothrow/notrigger but IsVisibleToGC() doesn't conform to that
    // contract which means in any build in which contracts are checked the function is THROW/TRIGGER
    // so it is annotated accordingly
    CONTRACTL
    {
        THROWS;
        GC_TRIGGERS;
        MODE_ANY;
        ENTRY_POINT;
        PRECONDITION(CheckPointer(pFrame));
        PRECONDITION(CheckPointer(pThread));
    }
    CONTRACTL_END;

#ifdef _X86_
	// SO transition here costs about 15% perf
    // Checkpoint the current thread's fast allocator (used for temporary
    // buffers over the call) and schedule a collapse back to the checkpoint in
    // the cleanup list. Note that if we need the allocator, it is
    // guaranteed that a cleanup list has been allocated.
     pThread->m_MarshalAlloc.StoreCheckpoint(&pFrame->GetNegInfo()->m_checkpoint);

#ifdef _DEBUG        
    BEGIN_DEBUG_ONLY_CODE;
    CleanupWorkList *pCleanup = pFrame->GetCleanupWorkList();
    if (pCleanup)
    {
        pCleanup->IsVisibleToGc();
    }
    END_DEBUG_ONLY_CODE;
#endif //_DEBUG

#else // non X86 code

    BEGIN_ENTRYPOINT_THROWS_WITH_THREAD(pThread);

    CleanupWorkList *pCleanup = pFrame->GetCleanupWorkList();

    if (pCleanup)
    {
        // Checkpoint the current thread's fast allocator (used for temporary
        // buffers over the call) and schedule a collapse back to the checkpoint in
        // the cleanup list. Note that if we need the allocator, it is
        // guaranteed that a cleanup list has been allocated.
        void* cp = pThread->m_MarshalAlloc.GetCheckpointSafeThrow();
        pCleanup->ScheduleFastFree(cp);
#ifdef _DEBUG        
        pCleanup->IsVisibleToGc();
#endif //_DEBUG
    }

	END_ENTRYPOINT_THROWS_WITH_THREAD; 
#endif

}



//---------------------------------------------------------
// Performs an N/Direct call. This is a generic version
// that can handle any N/Direct call but is not as fast
// as more specialized versions.
//---------------------------------------------------------

EXTERN_C
size_t
__stdcall
NDirectGenericStubComputeFrameSize(
    Thread *pThread,
    NDirectMethodFrameGeneric *pFrame,
    MLHeader **ppheader
    )
{
    CONTRACTL
    {
        THROWS;
        GC_TRIGGERS;
        MODE_COOPERATIVE;
        ENTRY_POINT;

        PRECONDITION(CheckPointer(pThread));
        PRECONDITION(CheckPointer(pFrame));
    }
    CONTRACTL_END;
    size_t retVal = 0;

    // this function is called by CLR to native assembly stubs which are called by 
    // managed code as a result, we need an unwind and continue handler to translate 
    // any of our internal exceptions into managed exceptions.
    INSTALL_UNWIND_AND_CONTINUE_HANDLER

    NDirectMethodDesc *pMD = (NDirectMethodDesc*)(pFrame->GetFunction());
    MLHeader *pheader;

    LOG((LF_STUBS, LL_INFO1000000, "Calling NDirectGenericStubWorker %s::%s \n", pMD->m_pszDebugClassName, pMD->m_pszDebugMethodName));

    if (pMD->IsVarArgs()) 
    {
        VASigCookie *pVASigCookie = pFrame->GetVASigCookie();

#ifdef LOGGING
        if (LoggingEnabled() && LoggingOn(LF_STUBS, LL_INFO1000000))
        {
            CQuickBytes     qbManaged;
            ULONG           cManagedSig;
            PCCOR_SIGNATURE pManagedSig;
        
            pManagedSig = pVASigCookie->mdVASig;
            cManagedSig = SigParser::LengthOfSig(pVASigCookie->mdVASig);
        
            PrettyPrintSigInternal(pManagedSig,  cManagedSig, "*",  &qbManaged, pVASigCookie->pModule->GetMDImport());

            LOG((LF_STUBS, LL_INFO1000000, "varargs NDirect sig: %s\n", qbManaged.Ptr()));
        }
#endif // LOGGING

        StubHolder<Stub> pTempMLStub;

        if (pVASigCookie->pNDirectMLStub == NULL)
        {
            PInvokeStaticSigInfo sigInfo(pMD);

            pTempMLStub = CreateNDirectStub(pVASigCookie->mdVASig, 
                                            SigParser::LengthOfSig(pVASigCookie->mdVASig),
                                            pVASigCookie->pModule, 
                                            mdMethodDefNil,
                                            sigInfo.GetCharSet(), 
                                            sigInfo.GetLinkFlags(), 
                                            sigInfo.GetCallConv(), 
                                            sigInfo.GetStubFlags(), 
                                            pMD, 
                                            NULL);

            if (NULL == FastInterlockCompareExchangePointer( (void*volatile*)&(pVASigCookie->pNDirectMLStub),
                                                            (Stub*) pTempMLStub,
                                                             NULL ))
            {
                pTempMLStub.SuppressRelease();
            }
        }
        pheader = (MLHeader*)(pVASigCookie->pNDirectMLStub->GetEntryPoint());
    } 
    else 
    {
        pheader = pMD->GetMLHeader();
    }


    // Tell our caller to allocate enough memory to store both the
    // destination buffer and the locals.
    *ppheader = pheader;

    retVal = pheader->m_cbDstBuffer + pheader->m_cbLocals;
#ifdef _PPC_
    retVal += NUM_FLOAT_ARGUMENT_REGISTERS * sizeof(DOUBLE);
#endif
    retVal = ALIGN_UP(retVal, STACK_ALIGN_SIZE);

    UNINSTALL_UNWIND_AND_CONTINUE_HANDLER;    
    return retVal;
}

EXTERN_C
int
__stdcall
NDirectGenericStubBuildArguments(
    Thread *pThread,
    NDirectMethodFrameGeneric *pFrame,
    MLHeader *pheader,
    MLCode const **ppMLCode,
    BYTE **pplocals,
    LPVOID *ppvFn,
    BYTE *pAlloc
    )
{
    CONTRACTL
    {
        THROWS;
        GC_TRIGGERS;
        MODE_COOPERATIVE;
        SO_TOLERANT;

        PRECONDITION(CheckPointer(pThread));
        PRECONDITION(CheckPointer(pFrame));
        PRECONDITION(CheckPointer(pheader));
    }
    CONTRACTL_END;

    int retVal = 0;

    // this function is called by CLR to native assembly stubs which are called by 
    // managed code as a result, we need an unwind and continue handler to translate 
    // any of our internal exceptions into managed exceptions.
    INSTALL_UNWIND_AND_CONTINUE_HANDLER;
    
#ifdef _DEBUG
    FillMemory(pAlloc, pheader->m_cbDstBuffer + pheader->m_cbLocals, 0xcc);
#endif

    BYTE   *pdst    = pAlloc;
    BYTE   *plocals = pdst + pheader->m_cbDstBuffer;

#ifdef STACK_GROWS_DOWN_ON_ARGS_WALK
    pdst += pheader->m_cbDstBuffer;
#endif // STACK_GROWS_DOWN_ON_ARGS_WALK

    VOID   *psrc          = (VOID*)pFrame;

    CleanupWorkList *pCleanup = pFrame->GetCleanupWorkList();

    if (pCleanup)
    {
        // Checkpoint the current thread's fast allocator (used for temporary
        // buffers over the call) and schedule a collapse back to the
        // checkpoint in the cleanup list. Note that if we need the allocator,
        // it is guaranteed that a cleanup list has been allocated.
        void* cp = pThread->m_MarshalAlloc.GetCheckpoint();
        pCleanup->ScheduleFastFree(cp);
        pCleanup->IsVisibleToGc();
    }

#ifdef _PPC_
    // float registers
    DOUBLE *pFloatRegs = (DOUBLE*)(pdst + 
        AlignStack(pheader->m_cbLocals + pheader->m_cbDstBuffer 
        + NUM_FLOAT_ARGUMENT_REGISTERS * sizeof(DOUBLE)) - NUM_FLOAT_ARGUMENT_REGISTERS * sizeof(DOUBLE));
    ZeroMemory(pFloatRegs, NUM_FLOAT_ARGUMENT_REGISTERS * sizeof(DOUBLE));
#endif

    // Call the ML interpreter to translate the arguments. Assuming
    // it returns, we get back a pointer to the succeeding code stream
    // which we will save away for post-call execution.
    *ppMLCode = RunML(pheader->GetMLCode(),
                      psrc,
                      pdst,
                      (UINT8*const) plocals,
                      pCleanup
                      PPC_ARG(pFloatRegs)
                      );

    NDirectMethodDesc *pMD = (NDirectMethodDesc*)(pFrame->GetFunction());

    LOG((LF_IJW, LL_INFO1000, "P/Invoke call (\"%s.%s%s\")\n", pMD->m_pszDebugClassName, pMD->m_pszDebugMethodName, pMD->m_pszDebugMethodSignature));



    // Call the target.
    pThread->EnablePreemptiveGC();

#ifdef PROFILING_SUPPORTED
    // Notify the profiler of call out of the runtime
    if (CORProfilerTrackTransitions())
    {
        PROFILER_CALL;
        g_profControlBlock.pProfInterface->
            ManagedToUnmanagedTransition((CodeID) pMD,
                                               COR_PRF_TRANSITION_CALL);
    }
#endif // PROFILING_SUPPORTED

    *ppvFn = pMD->GetNativeNDirectTarget();
    if (pMD->NeedHostHook())
    {
        Thread::LeaveRuntime(*(size_t*)ppvFn);
        pFrame->MarkEnterRuntimeRequired();
    }

#if _DEBUG
    //
    // Call through debugger routines to double check their
    // implementation
    //
    *ppvFn = (void*) CheckExitFrameDebuggerCalls;
#endif

    *pplocals = plocals;
    retVal = pheader->m_Flags;

    UNINSTALL_UNWIND_AND_CONTINUE_HANDLER;   

    return retVal;
}

EXTERN_C
INT64
__stdcall
NDirectGenericStubPostCall(
    Thread *pThread,
    NDirectMethodFrameGeneric *pFrame,
    MLHeader *pheader,
    MLCode const *pMLCode,
    BYTE *plocals,
    INT64 nativeReturnValue
#if defined(_X86_)
    ,
    DWORD PreESP,
    DWORD PostESP
#endif
    )
{
    // Moved above the contract because Contract doesn't respect GetLastError
    if (pheader->m_Flags & MLHF_SETLASTERROR)
        pThread->m_dwLastError = GetLastError();
    
    CONTRACTL
    {
        THROWS;
        GC_TRIGGERS;
        MODE_PREEMPTIVE;
        SO_TOLERANT;

        PRECONDITION(CheckPointer(pThread));
        PRECONDITION(CheckPointer(pFrame));
        PRECONDITION(CheckPointer(pheader));
    }
    CONTRACTL_END;

    INT64 returnValue = 0;   
 
    // this function is called by ndirect stubs which are called by managed code
    // as a result, we need an unwind and continue handler to translate any of our
    // internal exceptions into managed exceptions.
    INSTALL_UNWIND_AND_CONTINUE_HANDLER;   

    {
        // Ensure we don't respond to a thread abort request until after we
        // have marshaled our return result(s). In particular native handles
        // need to be back propagated into safe handles before it's safe to
        // take a thread abort.
        ThreadPreventAbortHolder preventAbort(TRUE);

        NDirectMethodDesc *pMD;
        pMD = (NDirectMethodDesc*)(pFrame->GetFunction());

#if defined(_X86_)

#if defined(_X86_) && defined(_DEBUG)
        _ASSERTE ((PreESP <= PostESP && PostESP <= PreESP + pheader->m_cbDstBuffer)
                  ||!"esp is trashed by PInvoke call, possibly wrong signature");
#endif

#endif // _X86_

#if BIGENDIAN
        // returnValue and nativeReturnValue are treated as 8 bytes buffers. The 32 bit values
        // will be in the high dword!
#endif

        if (pheader->GetUnmanagedRetValTypeCat() == MLHF_TYPECAT_FPU)
        {
            int fpNativeSize;
            if (pheader->m_Flags & MLHF_64BITUNMANAGEDRETVAL)
                fpNativeSize = 8;
            else
                fpNativeSize = 4;

            getFPReturn(fpNativeSize, &nativeReturnValue);
        }

#ifdef PROFILING_SUPPORTED
        // Notify the profiler of return from call out of the runtime
        if (CORProfilerTrackTransitions())
        {
            PROFILER_CALL;
            g_profControlBlock.pProfInterface->
                UnmanagedToManagedTransition((CodeID) pMD,
                                             COR_PRF_TRANSITION_RETURN);
        }
#endif // PROFILING_SUPPORTED

        if (pFrame->EnterRuntimeRequired())
        {
            Thread::EnterRuntime();
            pFrame->UnMarkEnterRuntimeRequired();
        }

        pThread->DisablePreemptiveGC();


        CleanupWorkList *pCleanup = pFrame->GetCleanupWorkList();

        // Marshal the return value and propagate any [out] parameters back.
        RunML(pMLCode,
              &nativeReturnValue,
              ((BYTE*)&returnValue)
#ifdef STACK_GROWS_DOWN_ON_ARGS_WALK
              + ((pheader->m_Flags & MLHF_64BITMANAGEDRETVAL) ? 8 : 4)
#endif // STACK_GROWS_DOWN_ON_ARGS_WALK
              ,
              (UINT8*const)plocals,
              pCleanup);

        int managedRetValTypeCat = pheader->GetManagedRetValTypeCat();

        if (pCleanup)
        {
            if (managedRetValTypeCat == MLHF_TYPECAT_GCREF)
            {
                GCPROTECT_BEGIN(*(VOID**)&returnValue);
                pCleanup->Cleanup(FALSE);
                GCPROTECT_END();
            }
            else
            {
                pCleanup->Cleanup(FALSE);
            }
        }

        if (managedRetValTypeCat == MLHF_TYPECAT_FPU)
        {
            int fpComPlusSize;
            if (pheader->m_Flags & MLHF_64BITMANAGEDRETVAL)
                fpComPlusSize = 8;
            else
                fpComPlusSize = 4;

            setFPReturn(fpComPlusSize, returnValue);
        }

    }

    if (g_TrapReturningThreads)
        pThread->HandleThreadAbort();

    UNINSTALL_UNWIND_AND_CONTINUE_HANDLER;

    return returnValue;
}


// factorization of CreateNDirectStubWorker
static MarshalInfo::MarshalType DoMarshalReturnValue(MetaSig&       msig,
                                                    ArgIterator&   ai,
                                                    Module*        pModule,
                                                    mdParamDef*    params,
                                                    BYTE           nlType,
                                                    BYTE           nlFlags,
                                                    UINT           argidx,  // this is used for reverse pinvoke hresult swapping
                                                    StubState*     pss,
                                                    BOOL           fThisCall,
                                                    int&           argOffset,
                                                    int&           lastArgSize,
                                                    DWORD          dwStubFlags,
                                                    MethodDesc     *pMD,
                                                    bool&          fRequiresPostCallFixup
                                                    DEBUG_ARG(LPCUTF8 pDebugName)
                                                    DEBUG_ARG(LPCUTF8 pDebugClassName)
                                                    DEBUG_ARG(LPCUTF8 pDebugNameSpace)
                                                    )
{
    CONTRACTL
    {
        THROWS;
        GC_TRIGGERS;
        MODE_ANY;

        PRECONDITION(CheckPointer(pModule));
        PRECONDITION(CheckPointer(params));
        PRECONDITION(CheckPointer(pss));
        PRECONDITION(CheckPointer(pMD, NULL_OK));
    }
    CONTRACTL_END;

    MarshalInfo::MarshalType marshalType = (MarshalInfo::MarshalType) 0xcccccccc;

    if (msig.GetReturnType() != ELEMENT_TYPE_VOID)
    {
        SigPointer pSig;
        pSig = msig.GetReturnProps();

        MarshalInfo returnInfo(pModule,
                                pSig,
                                params[0],
                                MarshalInfo::MARSHAL_SCENARIO_NDIRECT,
                                nlType,
                                nlFlags,
                                FALSE,
                                argidx,
                                0 != (dwStubFlags & NDIRECTSTUB_FL_BESTFIT),
                                0 != (dwStubFlags & NDIRECTSTUB_FL_THROWONUNMAPPABLECHAR),
                                0 != (dwStubFlags & NDIRECTSTUB_FL_USEIL),
                                pMD,
                                TRUE
                                DEBUG_ARG(pDebugName)
                                DEBUG_ARG(pDebugClassName)
                                DEBUG_ARG(pDebugNameSpace)
                                DEBUG_ARG(0)
                                );

        marshalType = returnInfo.GetMarshalType();

        {
            bool fMarshalNormally = true;

            if (marshalType > MarshalInfo::MARSHAL_TYPE_DOUBLE && IsUnsupportedValueTypeReturn(msig))
            {
               if (marshalType == MarshalInfo::MARSHAL_TYPE_BLITTABLEVALUECLASS ||
                   marshalType == MarshalInfo::MARSHAL_TYPE_GUID ||
                   marshalType == MarshalInfo::MARSHAL_TYPE_DECIMAL)
                {
                    if (dwStubFlags & NDIRECTSTUB_FL_DOHRESULTSWAPPING)
                    {
                        // V1 restriction: we could implement this but it's late in the game to do so.
                        COMPlusThrow(kMarshalDirectiveException, IDS_EE_NDIRECT_UNSUPPORTED_SIG);
                    }


                    if (!(dwStubFlags & NDIRECTSTUB_FL_USEIL))
                    {
                        MethodTable *pMT = msig.GetRetTypeHandleThrowing().AsMethodTable();
                        UINT         managedSize = msig.GetRetTypeHandleThrowing().GetSize();
                        UINT         unmanagedSize = pMT->GetNativeSize();

                        if (fThisCall)
                        {
                            pss->SetHasThisCallHiddenArg(true);
                        }

#if defined(ENREGISTERED_RETURNTYPE_MAXSIZE)
#if ENREGISTERED_RETURNTYPE_MAXSIZE > 8     // 8 is the max primitive type size
                        if ((marshalType == MarshalInfo::MARSHAL_TYPE_BLITTABLEVALUECLASS) &&
                            !IsManagedValueTypeReturnedByRef(managedSize) && 
                            !IsUnmanagedValueTypeReturnedByRef(unmanagedSize))
                        {
                            CONSISTENCY_CHECK_MSG(managedSize == unmanagedSize, "blittable value type doesn't have the same size in managed and unmanaged!");
                            fMarshalNormally = false;
                            pss->MarshalBlittableValueTypeReturn(managedSize);
                        }
                        else
#endif // ENREGISTERED_RETURNTYPE_MAXSIZE > 8     // 8 is the max primitive type size
#endif // defined(ENREGISTERED_RETURNTYPE_MAXSIZE)
                        if (IsManagedValueTypeReturnedByRef(managedSize))
                        {
                            fMarshalNormally = false;
                            if (fThisCall || IsUnmanagedValueTypeReturnedByRef(unmanagedSize))
                            {
                                fRequiresPostCallFixup = true;
                            }
                            else
                            {
                                pss->SetSrc(ai.GetRetBuffArgOffset());
                                pss->SetManagedReturnBuffSizeSmall(managedSize);
                            }
                        }
                        else
                        {
                            COMPlusThrow(kTypeLoadException, IDS_EE_NDIRECT_UNSUPPORTED_SIG);
                        }
                    }
                }
                else if (marshalType == MarshalInfo::MARSHAL_TYPE_CURRENCY)
                {
                    if (dwStubFlags & NDIRECTSTUB_FL_DOHRESULTSWAPPING)
                    {
                        // V1 restriction: we could implement this but it's late in the game to do so.
                        COMPlusThrow(kMarshalDirectiveException, IDS_EE_NDIRECT_UNSUPPORTED_SIG);
                    }
                    COMPlusThrow(kMarshalDirectiveException, IDS_EE_NDIRECT_UNSUPPORTED_SIG);
                }
                else if (marshalType == MarshalInfo::MARSHAL_TYPE_HANDLEREF)
                {
                    COMPlusThrow(kMarshalDirectiveException, IDS_EE_BADMARSHAL_HANDLEREFRESTRICTION);
                }
                else
                {
                    COMPlusThrow(kMarshalDirectiveException, IDS_EE_NDIRECT_UNSUPPORTED_SIG);
                }
            }

            if (fMarshalNormally)
            {

                pss->MarshalReturn(&returnInfo, dwStubFlags);

                lastArgSize = returnInfo.GetComArgSize();
            }
        }
    }

    return marshalType;
}


#ifdef LOP_FRIENDLY_FRAMES
ConfigDWORD g_LOPFriendlyFrames;
#endif //LOP_FRIENDLY_FRAMES

//---------------------------------------------------------
// Creates a new stub for a N/Direct call. Return refcount is 1.
// Note that this function may now throw if it fails to create
// a stub.
//---------------------------------------------------------
static Stub*  CreateNDirectStubWorker(StubState*        pss,
                                      PCCOR_SIGNATURE szMetaSig,
                                      DWORD      cbMetaSigSize,
                                      Module*    pModule,
                                      BYTE       nlType,
                                      BYTE       nlFlags,
                                      CorPinvokeMap unmgdCallConv,
                                      DWORD dwStubFlags,
                                      MethodDesc* pMD,
                                      VASigCookie*      pVASigCookie,
                                      mdParamDef*       pParamTokenArray,
                                      int               iLCIDArg
                                      DEBUG_ARG(LPCUTF8 pDebugName)
                                      DEBUG_ARG(LPCUTF8 pDebugClassName)
                                      DEBUG_ARG(LPCUTF8 pDebugNameSpace)
                                      )
{
    CONTRACT(Stub *)
    {
        THROWS;
        GC_TRIGGERS;
        MODE_ANY;

        PRECONDITION(CheckPointer(pss));
        PRECONDITION(CheckPointer(szMetaSig));
        PRECONDITION(CheckPointer(pModule));
        PRECONDITION(CheckPointer(pMD, NULL_OK));
        PRECONDITION(CheckPointer(pVASigCookie, NULL_OK));
        PRECONDITION(!pMD || pMD->IsILStub() || (0 != pMD->GetMethodTable()->GetClass()->IsAnyDelegateClass()) == (0 != (dwStubFlags & NDIRECTSTUB_FL_DELEGATE)));

        POSTCONDITION(CheckPointer(RETVAL, NULL_OK));
    }
    CONTRACT_END;

#ifdef _DEBUG
    if (g_pConfig->ShouldBreakOnInteropStubSetup(pDebugName))
        _ASSERTE(!"BreakOnInteropStubSetup");
#endif // _DEBUG

    Stub* pstub = NULL;

    {
        _ASSERTE(nlType == nltAnsi || nlType == nltUnicode);
    }

    //
    // Set up signature walking objects.
    //

    SigTypeContext typeContext;  // this is an empty type context: ndirect and COM calls are guaranteed to not be generics.
    MetaSig msig(szMetaSig, 
                 cbMetaSigSize,
                 pModule, 
                 &typeContext,
                 0 != (dwStubFlags & NDIRECTSTUB_FL_CONVSIGASVARARG));
    MetaSig sig = msig;
    ArgIterator ai(
            NULL,                                   // pFrame
            &sig,
            TRUE
            );   // fIsStatic

    bool fThisCall = (unmgdCallConv == pmCallConvThiscall);

    pss->SetStackPopSize(msig.CbStackPop(
                                         TRUE
                                                                                         ));

    pss->SetHas64BitReturn(msig.Is64BitReturn());
    pss->SetLastError(nlFlags & nlfLastError);
    pss->SetThisCall(fThisCall);
#ifdef LOP_FRIENDLY_FRAMES
    DWORD useLOPFriendlyFrames = g_LOPFriendlyFrames.val(L"LOPFriendlyFrames", 0);
    pss->SetCallerPopsArgs(useLOPFriendlyFrames && (unmgdCallConv == pmCallConvCdecl));
#endif //LOP_FRIENDLY_FRAMES
    pss->SetDoHRESULTSwapping(0 != (dwStubFlags & NDIRECTSTUB_FL_DOHRESULTSWAPPING));

    if (dwStubFlags & NDIRECTSTUB_FL_DELEGATE)
        pss->SetLastError(TRUE);

    switch (msig.GetReturnType())
    {
        case ELEMENT_TYPE_STRING:
        case ELEMENT_TYPE_OBJECT:
        case ELEMENT_TYPE_CLASS:
        case ELEMENT_TYPE_SZARRAY:
        case ELEMENT_TYPE_ARRAY:
        case ELEMENT_TYPE_VAR:
        // case ELEMENT_TYPE_PTR:  -- cwb. PTR is unmanaged & should not be GC promoted
            pss->SetRetValIsGcRef();
            break;
        default:
            break;
    }

    pss->BeginEmit(dwStubFlags);

#ifdef STACK_GROWS_UP_ON_ARGS_WALK
    if (-1 != iLCIDArg)
    {
        // the code to handle the LCID below is setup for x86
        // where we marshal arguments in reverse order.  As such,
        // it will call MarshalLCID before calling MarshalArgument
        // on the argument the LCID should go after.  This is the
        // right order if we're going backwards, but it's one too
        // early if we're going forward.  So we just bump up the
        // index here.
        iLCIDArg++;
    }
#endif // STACK_GROWS_UP_ON_ARGS_WALK

    //
    // Get a list of the COM+ argument offsets.  We
    // need this since we have to iterate the arguments
    // backwards.
    // Note that the first argument listed may
    // be a byref for a value class return value
    //

    int numArgs = msig.NumFixedArgs();

    int allocSize = numArgs * sizeof(int);
    if (allocSize < numArgs)
        COMPlusThrow(kTypeLoadException, IDS_EE_SIGTOOCOMPLEX);

    int *offsets = (int*)_alloca((size_t)allocSize);
    int *o = offsets;
    int *oEnd = o + numArgs;

#ifdef CALLDESCR_REGTYPEMAP
    UINT64 regTypeMap;
    BYTE* pMap;
    int   regArgNum  = 1;

    {
        pMap = (BYTE*)&regTypeMap;
    }

    *pMap = ELEMENT_TYPE_U;  // 'this'

    while (o < oEnd)
    {
        BYTE   typ;
        UINT32 structSize;
        int argOffset = ai.GetNextOffsetFaster(&typ, &structSize);

        CONSISTENCY_CHECK((o - offsets) != -1);
        if ((o - offsets) == iLCIDArg)
        {
            // make sure we skip a slot in the reg type map for
            // the LCID argument.
            regArgNum++;
        }

        if ((o - offsets) >= iLCIDArg)
        {
            //
            // for all arguments after the LCID argument, we 
            // well be at a different offset.
            //
            argOffset += StackElemSize(sizeof(LCID));
        }

        if ((regArgNum < NUM_ARGUMENT_REGISTERS) && (fparg < NUM_FP_ARG_REGISTERS))
        {
            FillInRegTypeMap(&ai, pMap, &regArgNum, &typ, structSize);
        }


        *o++ = argOffset;
    }
#else // !CALLDESCR_REGTYPEMAP
    while (o < oEnd)
    {
        *o++ = ai.GetNextOffset();
    }
#endif // !CALLDESCR_REGTYPEMAP

#ifdef STACK_GROWS_UP_ON_ARGS_WALK
    o = offsets;
#endif // STACK_GROWS_UP_ON_ARGS_WALK


    //
    // Now, emit the ML.
    //

    int argOffset = 0;
    int lastArgSize = 0;

    MarshalInfo::MarshalType marshalType = (MarshalInfo::MarshalType) 0xcccccccc;

    //
    // Marshal the return value.
    //

    bool fRequiresPostCallFixup = false;

#ifdef STACK_GROWS_UP_ON_ARGS_WALK
    int nativeStackSize = 0;
    msig.Reset();
    if (msig.HasRetBuffArg())
        nativeStackSize += sizeof(LPVOID);
#else
    marshalType = DoMarshalReturnValue(msig,
                                       ai,
                                       pModule,
                                       pParamTokenArray,
                                       nlType,
                                       nlFlags,
                                       0,
                                       pss,
                                       fThisCall,
                                       argOffset,
                                       lastArgSize,
                                       dwStubFlags,
                                       pMD,
                                       fRequiresPostCallFixup
                                       DEBUG_ARG(pDebugName)
                                       DEBUG_ARG(pDebugClassName)
                                       DEBUG_ARG(pDebugNameSpace)
                                       );

    msig.GotoEnd();
#endif  // STACK_GROWS_UP_ON_ARGS_WALK

#if THISPTR_LOCATION > 0
    // We marshal arguments first, then return value
    // so skip the return buffer if there is one
    if (msig.HasRetBuffArg())
        pss->DoDstFixup(sizeof(LPVOID));
#endif

    //
    // Marshal the arguments
    //



    int argidx;
#ifdef STACK_GROWS_UP_ON_ARGS_WALK
    argidx = 1;
    while (o < oEnd)
#else // STACK_GROWS_UP_ON_ARGS_WALK
    argidx = msig.NumFixedArgs();
    while (o > offsets)
#endif // STACK_GROWS_UP_ON_ARGS_WALK
    {
#ifndef STACK_GROWS_UP_ON_ARGS_WALK
        --o;
#endif // STACK_GROWS_UP_ON_ARGS_WALK

        //
        // Check to see if this is the parameter after which we need to insert the LCID.
        //

        if (argidx == iLCIDArg)
        {
            pss->MarshalLCID();

#ifdef STACK_GROWS_UP_ON_ARGS_WALK
            nativeStackSize += sizeof(LPVOID);
#endif
        }

        //
        // Adjust src pointer if necessary (for register params or
        // for return value order differences)
        //

        int fixup = *o - (argOffset + lastArgSize);
        argOffset = *o;

        if (!FitsInI2(fixup))
        {
            COMPlusThrow(kTypeLoadException, IDS_EE_SIGTOOCOMPLEX);
        }
        if (fixup != 0)
        {
            pss->DoFixup(fixup);
        }

#ifdef STACK_GROWS_UP_ON_ARGS_WALK
        msig.NextArg();
#else
        msig.PrevArg();
#endif

        MarshalInfo info(pModule,
                         msig.GetArgProps(),
                         pParamTokenArray[argidx],
                         MarshalInfo::MARSHAL_SCENARIO_NDIRECT,
                         nlType,
                         nlFlags,
                         TRUE,
                         argidx,
                         0 != (dwStubFlags & NDIRECTSTUB_FL_BESTFIT),
                         0 != (dwStubFlags & NDIRECTSTUB_FL_THROWONUNMAPPABLECHAR),
                         0 != (dwStubFlags & NDIRECTSTUB_FL_USEIL),
                         pMD,
                         TRUE
                         DEBUG_ARG(pDebugName)
                         DEBUG_ARG(pDebugClassName)
                         DEBUG_ARG(pDebugNameSpace)
                         DEBUG_ARG(argidx)
                         );


        pss->MarshalArgument(&info, argOffset, dwStubFlags);

        lastArgSize = info.GetComArgSize();

#ifdef STACK_GROWS_UP_ON_ARGS_WALK
        nativeStackSize += info.GetNativeArgSize();
#endif

#ifdef STACK_GROWS_UP_ON_ARGS_WALK
        o++;
        argidx++;
#else  // STACK_GROWS_UP_ON_ARGS_WALK
        --argidx;
#endif  // STACK_GROWS_UP_ON_ARGS_WALK
    }

    // Check to see if this is the parameter after which we need to insert the LCID.
    if (argidx == iLCIDArg)
    {
        pss->MarshalLCID();

#ifdef STACK_GROWS_UP_ON_ARGS_WALK
        nativeStackSize += sizeof(LPVOID);
#endif
    }

#ifdef STACK_GROWS_UP_ON_ARGS_WALK

    marshalType = DoMarshalReturnValue(msig,
                         ai,
                         pModule,
                         pParamTokenArray,
                         nlType,
                         nlFlags,
                         argidx,
                         pss,
                         fThisCall,
                         argOffset,
                         lastArgSize,
                         dwStubFlags,
                         pMD,
                         fRequiresPostCallFixup
                         DEBUG_ARG(pDebugName)
                         DEBUG_ARG(pDebugClassName)
                         DEBUG_ARG(pDebugNameSpace)
                         );


#endif // STACK_GROWS_UP_ON_ARGS_WALK

    if (fRequiresPostCallFixup)
    {

                UINT         managedSize = msig.GetRetTypeHandleThrowing().GetSize();

                    int fixup = ai.GetRetBuffArgOffset() - (argOffset + lastArgSize);
                    argOffset = ai.GetRetBuffArgOffset();

                    if (!FitsInI2(fixup))
                        COMPlusThrow(kTypeLoadException, IDS_EE_SIGTOOCOMPLEX);

                    if (fixup != 0)
                    {
                        pss->DoFixup(fixup);
#ifdef STACK_GROWS_UP_ON_ARGS_WALK
                        pss->DoDstFixup(-nativeStackSize);
#endif
                    }

                    pss->SetManagedReturnBuffSizeLarge(managedSize);
                }

    pstub = pss->FinishEmit(pMD,
                            pVASigCookie
                            );

    RETURN pstub;
}


Stub * CreateNDirectStubNoThrow(PCCOR_SIGNATURE    szMetaSig,
                                DWORD              cbMetaSigSize,
                                Module*            pModule,
                                mdMethodDef        md,                     // this doesn't affect the hash
                                CorNativeLinkType  nlType,
                                CorNativeLinkFlags nlFlags,
                                CorPinvokeMap      unmgdCallConv,
                                DWORD              dwStubFlags,            // NDirectStubFlags
                                MethodDesc*        pMD,                    // this doesn't affect the hash
                                VASigCookie*       pVASigCookie
                       DEBUG_ARG(LPCUTF8            pDebugName)
                       DEBUG_ARG(LPCUTF8            pDebugClassName)
                       DEBUG_ARG(LPCUTF8            pDebugNameSpace)
                                )
{
    CONTRACT (Stub*)
    {
        THROWS;
        GC_TRIGGERS;
        MODE_ANY;
        POSTCONDITION(CheckPointer(RETVAL, NULL_OK));
    }
    CONTRACT_END;
    
    Stub* pRetVal = NULL;
    
    EX_TRY
    {
        pRetVal = CreateNDirectStub(szMetaSig, 
                                    cbMetaSigSize,
                                    pModule, 
                                    md, 
                                    nlType, 
                                    nlFlags, 
                                    unmgdCallConv, 
                                    dwStubFlags, 
                                    pMD, 
                                    pVASigCookie
                                    DEBUG_ARG(pDebugName)
                                    DEBUG_ARG(pDebugClassName)
                                    DEBUG_ARG(pDebugNameSpace));
    }
    EX_SWALLOW_NONTERMINAL

    RETURN pRetVal;
}


Stub * CreateNDirectStub(PInvokeStaticSigInfo* pSigInfo,
                         DWORD dwStubFlags,
                         MethodDesc* pMD,
                         VASigCookie *pVASigCookie
                         DEBUG_ARG(LPCUTF8 pDebugName)
                         DEBUG_ARG(LPCUTF8 pDebugClassName)
                         DEBUG_ARG(LPCUTF8 pDebugNameSpace))
{
    CONTRACT(Stub*)
    {
        THROWS;
        GC_TRIGGERS;
        MODE_ANY;
        POSTCONDITION(CheckPointer(RETVAL));
    }
    CONTRACT_END;
        

    RETURN CreateNDirectStub(pSigInfo->GetSig(), 
                             pSigInfo->GetSigCount(),
                             pSigInfo->GetModule(), 
                             pSigInfo->GetMethodToken(),
                             pSigInfo->GetCharSet(), 
                             pSigInfo->GetLinkFlags(), 
                             pSigInfo->GetCallConv(), 
                             pSigInfo->GetStubFlags() | dwStubFlags, 
                             pMD, 
                             NULL
                             DEBUG_ARG(pDebugName) 
                             DEBUG_ARG(pDebugClassName) 
                             DEBUG_ARG(pDebugNameSpace) );
}

//
// Additional factorization of CreateNDirectStub.  This hoists all the metadata accesses
// into one location so that we can leave CreateNDirectStubWorker to just generate the 
// ML or IL.  This allows us to cache a stub based on the inputs to CreateNDirectStubWorker
// instead of having to generate the ML/IL first before doing the caching.
//
void CreateNDirectStubAccessMetadata(PCCOR_SIGNATURE    szMetaSig,      // IN
                                     DWORD              cbMetaSigSize,  // IN
                                     Module*            pModule,        // IN 
                                     mdMethodDef        md,             // IN
                                     CorPinvokeMap      unmgdCallConv,  // IN
                                     DWORD*             pdwStubFlags,   // IN/OUT
                                     int*               piLCIDArg,      // OUT
                                     int*               pNumArgs        // OUT
                                     )
{
    {
        if (unmgdCallConv != pmCallConvStdcall &&
            unmgdCallConv != pmCallConvCdecl &&
            unmgdCallConv != pmCallConvThiscall)
        {
            COMPlusThrow(kTypeLoadException, IDS_INVALID_PINVOKE_CALLCONV);
        }
    }
    
    SigTypeContext typeContext;  // this is an empty type context: ndirect calls are guranteed to not be generics.
    MetaSig msig(szMetaSig, 
                 cbMetaSigSize,
                 pModule, 
                 &typeContext,
                 0 != ((*pdwStubFlags) & NDIRECTSTUB_FL_CONVSIGASVARARG));

    (*pNumArgs) = msig.NumFixedArgs();
    //if ( msig.IsVarArg() )
    //    ++( *pNumArgs );
    
    IMDInternalImport* pInternalImport = pModule->GetMDImport();

    _ASSERTE(!((*pdwStubFlags) & NDIRECTSTUB_FL_DOHRESULTSWAPPING));

    if (md != mdMethodDefNil)
    {
        DWORD           dwDescrOffset;
        DWORD           dwImplFlags;
        pInternalImport->GetMethodImplProps(md,
                                            &dwDescrOffset,
                                            &dwImplFlags
                                           );

        if ((*pdwStubFlags) & NDIRECTSTUB_FL_REVERSE_INTEROP)
        {
        }
        else
        {
            // fwd pinvoke, fwd com interop support hresult swapping.
            // delegate to an unmanaged method does not.
        if (!IsMiPreserveSig(dwImplFlags) && !((*pdwStubFlags) & NDIRECTSTUB_FL_DELEGATE))
        {
            (*pdwStubFlags) |= NDIRECTSTUB_FL_DOHRESULTSWAPPING;
        }
    }
    }

    (*piLCIDArg) = GetLCIDParameterIndex(pInternalImport, md);

    // Check to see if we need to do LCID conversion.
    if ((*piLCIDArg) != -1 && (*piLCIDArg) > (*pNumArgs))
    {
        COMPlusThrow(kIndexOutOfRangeException, IDS_EE_INVALIDLCIDPARAM);
    }

    {
        if (msig.HasThis() && !((*pdwStubFlags) & NDIRECTSTUB_FL_DELEGATE))
        {
            COMPlusThrow(kInvalidProgramException, VLDTR_E_FMD_PINVOKENOTSTATIC);
        }
    }
}

void NDirect::PopulateNDirectMethodDesc(NDirectMethodDesc* pNMD, PInvokeStaticSigInfo* pSigInfo)
{
    if (pNMD->IsSynchronized())
        COMPlusThrow(kTypeLoadException, IDS_EE_NOSYNCHRONIZED);

    WORD ndirectflags = 0;
    if (pNMD->MethodDesc::IsVarArg())
        ndirectflags |= NDirectMethodDesc::kVarArgs;

    LPCUTF8 szLibName = NULL, szEntryPointName = NULL;
    new (pSigInfo) PInvokeStaticSigInfo(pNMD, &szLibName, &szEntryPointName);

    pNMD->ndirect.m_pszLibName = szLibName;
    pNMD->ndirect.m_pszEntrypointName = szEntryPointName;

    if (pSigInfo->GetCharSet() == nltAnsi)
        ndirectflags |= NDirectMethodDesc::kNativeAnsi;

    CorNativeLinkFlags linkflags = pSigInfo->GetLinkFlags();    
    if (linkflags & nlfLastError)
        ndirectflags |= NDirectMethodDesc::kLastError;
    if (linkflags & nlfNoMangle)
        ndirectflags |= NDirectMethodDesc::kNativeNoMangle;
    
    CorPinvokeMap callConv = pSigInfo->GetCallConv();
    if (callConv == pmCallConvStdcall)
        ndirectflags |= NDirectMethodDesc::kStdCall;
    if (callConv == pmCallConvThiscall)
        ndirectflags |= NDirectMethodDesc::kThisCall;

    // Call this exactly ONCE per thread. Do not publish incomplete prestub flags
    // or you will introduce a race condition.
    pNMD->InterlockedSetNDirectFlags(ndirectflags);
}



//---------------------------------------------------------
// Creates a new stub for a N/Direct call. Return refcount is 1.
// If failed, this function now throws an exception instead
// of returning a throwable parameter.
//---------------------------------------------------------
Stub * CreateNDirectStub(PCCOR_SIGNATURE    szMetaSig,
                         DWORD              cbMetaSigSize,
                         Module*            pModule,
                         mdMethodDef        md,                     // this doesn't affect the hash
                         CorNativeLinkType  nlType,
                         CorNativeLinkFlags nlFlags,
                         CorPinvokeMap      unmgdCallConv,
                         DWORD              dwStubFlags,            // NDirectStubFlags
                         MethodDesc*        pMD,                    // this doesn't affect the hash
                         VASigCookie*       pVASigCookie
               DEBUG_ARG(LPCUTF8            pDebugName)
               DEBUG_ARG(LPCUTF8            pDebugClassName)
               DEBUG_ARG(LPCUTF8            pDebugNameSpace)
                           )
{
    CONTRACT(Stub*)
    {
        THROWS;
        GC_TRIGGERS;
        MODE_ANY;

        PRECONDITION(CheckPointer(szMetaSig));
        PRECONDITION(CheckPointer(pModule));
        PRECONDITION(CheckPointer(pVASigCookie, NULL_OK));
        PRECONDITION(CheckPointer(pMD, NULL_OK));
        POSTCONDITION(CheckPointer(RETVAL, NULL_OK));
    }
    CONTRACT_END;


    int         iLCIDArg = 0;
    int         numArgs = 0;
    int         numParamTokens = 0;
    mdParamDef* pParamTokenArray = NULL;

    CreateNDirectStubAccessMetadata(szMetaSig,
                                    cbMetaSigSize,
                                    pModule,
                                    md,
                                    unmgdCallConv,
                                    &dwStubFlags,
                                    &iLCIDArg,
                                    &numArgs);

    numParamTokens = numArgs + 1;
    pParamTokenArray = (mdParamDef*)_alloca(numParamTokens * sizeof(mdParamDef));
    mdMethodDef modMD = ( pMD && pMD->IsVarArg() ) ? mdMethodDefNil : md;  // Nil method token has always been passed for VarArg NDirects
    CollateParamTokens(pModule->GetMDImport(), modMD, numArgs, pParamTokenArray);
    

        MLStubState MyStubState;

    RETURN CreateNDirectStubWorker(&MyStubState,
                                   szMetaSig,
                                   cbMetaSigSize,
                                   pModule,
                                   nlType,
                                   nlFlags,
                                   unmgdCallConv,
                                   dwStubFlags,
                                   pMD,
                                   pVASigCookie,
                                   pParamTokenArray,
                                   iLCIDArg
                                   DEBUG_ARG(pDebugName)
                                   DEBUG_ARG(pDebugClassName)
                                   DEBUG_ARG(pDebugNameSpace)
                                   );
    }




void PInvokeStaticSigInfo::PreInit(Module* pModule, mdTypeDef tkDeclType)
{
    CONTRACTL
    {
        NOTHROW;
        GC_NOTRIGGER;
        MODE_ANY;
    }
    CONTRACTL_END;
    
    // initialize data members
    m_wFlags = 0;
    m_pModule = pModule;
    m_callConv = (CorPinvokeMap)0;
    SetBestFitMapping (TRUE);
    SetThrowOnUnmappableChar (FALSE);
    SetLinkFlags (nlfNone);
    SetCharSet (nltAnsi);
    m_tkMethod = mdMethodDefNil;
    m_error = 0;

    // assembly level m_bestFit & m_bThrowOnUnmappableChar
    BOOL bBestFit;
    BOOL bThrowOnUnmappableChar;
    ReadBestFitCustomAttribute(m_pModule->GetMDImport(), tkDeclType, &bBestFit, &bThrowOnUnmappableChar);
    SetBestFitMapping (bBestFit);
    SetThrowOnUnmappableChar (bThrowOnUnmappableChar);
}

void PInvokeStaticSigInfo::PreInit(MethodDesc* pMD)
{
    CONTRACTL
    {
        NOTHROW;
        GC_NOTRIGGER;
        MODE_ANY;
    }
    CONTRACTL_END;

    PreInit(pMD->GetModule(), pMD->GetClass()->GetCl());
    SetIsStatic (pMD->IsStatic());
    pMD->GetSig(&m_pSig, &m_cSig);
    m_tkMethod = pMD->GetMemberDef();
    if (pMD->IsEEImpl())
    {
        CONSISTENCY_CHECK(pMD->GetMethodTable()->IsAnyDelegateClass());
        SetIsDelegateInterop(TRUE);
    }
}

PInvokeStaticSigInfo::PInvokeStaticSigInfo(
    MethodDesc* pMD, LPCUTF8 *pLibName, LPCUTF8 *pEntryPointName, ThrowOnError throwOnError) 
{ 
    CONTRACTL
    {
        if (throwOnError) THROWS; else NOTHROW;
        GC_NOTRIGGER;
        MODE_ANY;
    }
    CONTRACTL_END;
    
    DllImportInit(pMD, pLibName, pEntryPointName);

    if (throwOnError)
        ReportErrors();
}

PInvokeStaticSigInfo::PInvokeStaticSigInfo(MethodDesc* pMD, ThrowOnError throwOnError)
{
    CONTRACTL
    {
        if (throwOnError) THROWS; else NOTHROW;
        GC_NOTRIGGER;
        MODE_ANY;

        PRECONDITION(CheckPointer(pMD));
    }
    CONTRACTL_END;

    HRESULT hr = S_OK;

    if (!pMD->GetMethodTable()->IsAnyDelegateClass())
    {
        DllImportInit(pMD, NULL, NULL);
        return;
    }

    // initialize data members to defaults
    PreInit(pMD);

    DelegateEEClass* pEE = (DelegateEEClass*)pMD->GetMethodTable()->GetClass();

    // System.Runtime.InteropServices.UnmanagedFunctionPointerAttribute
    BYTE* pData = NULL;
    LONG cData = 0;
    CorPinvokeMap callConv = (CorPinvokeMap)0;

    if (SUCCEEDED(pEE->GetModule()->GetMDImport()->GetCustomAttributeByName(
        pEE->GetCl(), "System.Runtime.InteropServices.UnmanagedFunctionPointerAttribute", (const VOID**)(&pData), (ULONG*)&cData)) 
        && cData)
    {
        CustomAttributeParser ca(pData, cData);

        CaArg args[1];
        args[0].InitEnum(SERIALIZATION_TYPE_I4, (ULONG)m_callConv);

        IfFailGo(ParseKnownCaArgs(ca, args, lengthof(args)));

        enum UnmanagedFunctionPointerNamedArgs
        {
            MDA_CharSet,
            MDA_BestFitMapping,
            MDA_ThrowOnUnmappableChar,
            MDA_SetLastError,
            MDA_PreserveSig,
            MDA_Last,
        };

        CaNamedArg namedArgs[MDA_Last];
        namedArgs[MDA_CharSet].InitI4FieldEnum("CharSet", "System.Runtime.InteropServices.CharSet", (ULONG)GetCharSet());
        namedArgs[MDA_BestFitMapping].InitBoolField("BestFitMapping", (ULONG)GetBestFitMapping());
        namedArgs[MDA_ThrowOnUnmappableChar].InitBoolField("ThrowOnUnmappableChar", (ULONG)GetThrowOnUnmappableChar());
        namedArgs[MDA_SetLastError].InitBoolField("SetLastError", 0);
        namedArgs[MDA_PreserveSig].InitBoolField("PreserveSig", 0);

        IfFailGo(ParseKnownCaNamedArgs(ca, namedArgs, lengthof(namedArgs)));

        callConv = (CorPinvokeMap)(args[0].val.u4 << 8);
        CorNativeLinkType nlt = (CorNativeLinkType)0;

        // XXX Tue 07/19/2005
        // Keep in sync with the handling of CorPInvokeMap in
        // PInvokeStaticSigInfo::DllImportInit.
        switch( namedArgs[MDA_CharSet].val.u4 )
        {
        case 0:
        case nltAnsi:
            nlt = nltAnsi; break;
        case nltUnicode:
            nlt = nltUnicode; break;
        case nltAuto:
            nlt = NDirectOnUnicodeSystem() ? nltUnicode : nltAnsi; break;
        default:
            hr = E_FAIL; goto ErrExit;
        }
        SetCharSet ( nlt );
        SetBestFitMapping (namedArgs[MDA_BestFitMapping].val.u1);
        SetThrowOnUnmappableChar (namedArgs[MDA_ThrowOnUnmappableChar].val.u1);
        if (namedArgs[MDA_SetLastError].val.u1) 
            SetLinkFlags ((CorNativeLinkFlags)(nlfLastError | GetLinkFlags()));
        if (namedArgs[MDA_PreserveSig].val.u1)
            SetLinkFlags ((CorNativeLinkFlags)(nlfNoMangle | GetLinkFlags()));
    }

            
ErrExit:    
    if (hr != S_OK)
        SetError(IDS_EE_NDIRECT_BADNATL);   

    InitCallConv(callConv, pMD->IsVarArg()); 

    if (throwOnError)
        ReportErrors();   
}

PInvokeStaticSigInfo::PInvokeStaticSigInfo(
    PCCOR_SIGNATURE pSig, DWORD cSig, Module* pModule, BOOL bIsStatic, ThrowOnError throwOnError)
{
    CONTRACTL
    {
        if (throwOnError) THROWS; else NOTHROW;
        GC_NOTRIGGER;
        MODE_ANY;

        PRECONDITION(CheckPointer(pModule));
    }
    CONTRACTL_END;

    PreInit(pModule, mdTypeDefNil);
    m_pSig = pSig;
    m_cSig = cSig;
    SetIsStatic (bIsStatic);
    InitCallConv((CorPinvokeMap)0, FALSE);
    
    if (throwOnError)
        ReportErrors();    
}

void PInvokeStaticSigInfo::DllImportInit(MethodDesc* pMD, LPCUTF8 *ppLibName, LPCUTF8 *ppEntryPointName)
{
    CONTRACTL
    {
        NOTHROW;
        GC_NOTRIGGER;
        MODE_ANY;

        PRECONDITION(CheckPointer(pMD));

        // These preconditions to prevent multithreaded regression 
        // where pMD->ndirect.m_szLibName was passed in directly, cleared 
        // by this API, then accessed on another thread before being reset here.
        PRECONDITION(CheckPointer(ppLibName, NULL_OK) && (!ppLibName || *ppLibName == NULL)); 
        PRECONDITION(CheckPointer(ppEntryPointName, NULL_OK) && (!ppEntryPointName || *ppEntryPointName == NULL));
    }
    CONTRACTL_END;

    // initialize data members to defaults
    PreInit(pMD);

    // System.Runtime.InteropServices.DLLImportAttribute
    IMDInternalImport  *pInternalImport = pMD->GetMDImport();
    CorPinvokeMap mappingFlags = pmMaxValue;
    mdModuleRef modref = mdModuleRefNil;
    if (!SUCCEEDED(pInternalImport->GetPinvokeMap(pMD->GetMemberDef(), (DWORD*)&mappingFlags, ppEntryPointName, &modref)))
    {
        InitCallConv((CorPinvokeMap)0, pMD->IsVarArg());
        return;
    }

    // out parameter pEntryPointName
    if (ppEntryPointName && *ppEntryPointName == NULL)
        *ppEntryPointName = pMD->GetName();

    // out parameter pLibName
    if (ppLibName)
        pInternalImport->GetModuleRefProps(modref, ppLibName);

    // m_callConv
    InitCallConv((CorPinvokeMap)(mappingFlags & pmCallConvMask), pMD->IsVarArg());

    // m_bestFit
    CorPinvokeMap bestFitMask = (CorPinvokeMap)(mappingFlags & pmBestFitMask);
    if (bestFitMask == pmBestFitEnabled)
        SetBestFitMapping (TRUE);
    else if (bestFitMask == pmBestFitDisabled)
        SetBestFitMapping (FALSE);

    // m_bThrowOnUnmappableChar
    CorPinvokeMap unmappableMask = (CorPinvokeMap)(mappingFlags & pmThrowOnUnmappableCharMask);
    if (unmappableMask == pmThrowOnUnmappableCharEnabled)
        SetThrowOnUnmappableChar (TRUE);
    else if (unmappableMask == pmThrowOnUnmappableCharDisabled)
        SetThrowOnUnmappableChar (FALSE);

    // inkFlags : CorPinvoke -> CorNativeLinkFlags
    if (mappingFlags & pmSupportsLastError)
        SetLinkFlags ((CorNativeLinkFlags)(GetLinkFlags() | nlfLastError));
    if (mappingFlags & pmNoMangle)
        SetLinkFlags ((CorNativeLinkFlags)(GetLinkFlags() | nlfNoMangle));

    // XXX Tue 07/19/2005
    // Keep in sync with the handling of CorNativeLinkType in
    // PInvokeStaticSigInfo::PInvokeStaticSigInfo.
    
    // charset : CorPinvoke -> CorNativeLinkType
    CorPinvokeMap charSetMask = (CorPinvokeMap)(mappingFlags & (pmCharSetNotSpec | pmCharSetAnsi | pmCharSetUnicode | pmCharSetAuto));
    if (charSetMask == pmCharSetNotSpec || charSetMask == pmCharSetAnsi)
        SetCharSet (nltAnsi);
    else if (charSetMask == pmCharSetUnicode)
        SetCharSet (nltUnicode);
    else if (charSetMask == pmCharSetAuto)
        SetCharSet ((NDirectOnUnicodeSystem() ? nltUnicode : nltAnsi));
    else
        SetError(IDS_EE_NDIRECT_BADNATL);
}


void PInvokeStaticSigInfo::InitCallConv(CorPinvokeMap callConv, BOOL bIsVarArg)
{
    WRAPPER_CONTRACT;

    // Convert WinAPI methods to either StdCall or CDecl based on if they are varargs or not.
    if (callConv == pmCallConvWinapi)
        callConv = bIsVarArg ? pmCallConvCdecl : pmCallConvStdcall;

    CorPinvokeMap sigCallConv = (CorPinvokeMap)0;
    BOOL fSuccess = MetaSig::GetUnmanagedCallingConvention(m_pModule, m_pSig, m_cSig, &sigCallConv);

    if (!fSuccess)
    {
        SetError(IDS_EE_NDIRECT_BADNATL); //Bad metadata format
    }

    // Do the same WinAPI to StdCall or CDecl for the signature calling convention as well. We need
    // to do this before we check to make sure the PInvoke map calling convention and the 
    // signature calling convention match for compatibility reasons.
    if (sigCallConv == pmCallConvWinapi)
        sigCallConv = bIsVarArg ? pmCallConvCdecl : pmCallConvStdcall;

    if (callConv != 0 && sigCallConv != 0 && callConv != sigCallConv)
        SetError(IDS_EE_NDIRECT_BADNATL_CALLCONV);

    if (callConv == 0 && sigCallConv == 0)
        m_callConv = bIsVarArg ? pmCallConvCdecl : pmCallConvStdcall;
    else if (callConv != 0)
        m_callConv = callConv;
    else
        m_callConv = sigCallConv;

    if (bIsVarArg && m_callConv != pmCallConvCdecl)
        SetError(IDS_EE_NDIRECT_BADNATL_VARARGS_CALLCONV);
}

void PInvokeStaticSigInfo::ReportErrors()
{
    CONTRACTL
    {
        THROWS;
        GC_NOTRIGGER;
        MODE_ANY;
    }
    CONTRACTL_END;

    if (m_error != 0)
        COMPlusThrow(kTypeLoadException, m_error);      
}


//---------------------------------------------------------
// Does a class or method have a NAT_L CustomAttribute?
//
// S_OK    = yes
// S_FALSE = no
// FAILED  = unknown because something failed.
//---------------------------------------------------------
/*static*/
HRESULT NDirect::HasNAT_LAttribute(IMDInternalImport *pInternalImport, mdToken token)
{
    CONTRACTL
    {
        NOTHROW;
        GC_NOTRIGGER;
        MODE_ANY;

        PRECONDITION(CheckPointer(pInternalImport));
        PRECONDITION(TypeFromToken(token) == mdtMethodDef);
    }
    CONTRACTL_END;

    // Check method flags first before trying to find the custom value
    DWORD dwAttr;
    dwAttr = pInternalImport->GetMethodDefProps(token);
    if (!IsReallyMdPinvokeImpl(dwAttr))
        return S_FALSE;

    DWORD   mappingFlags = 0xcccccccc;
    LPCSTR  pszImportName = (LPCSTR)POISONC;
    mdModuleRef modref = 0xcccccccc;


    if (SUCCEEDED(pInternalImport->GetPinvokeMap(token, &mappingFlags, &pszImportName, &modref)))
        return S_OK;

    return S_FALSE;
}


//
// NumParamBytes
// Counts # of parameter bytes
FCIMPL1(INT32, NDirect_NumParamBytes, MethodDesc* pMD)
{
    INT32 cbParamBytes = 0;
    HELPER_METHOD_FRAME_BEGIN_RET_NOPOLL();

    CONTRACTL
    {
        THROWS;
        GC_TRIGGERS;
        MODE_COOPERATIVE;
        SO_TOLERANT;
    }
    CONTRACTL_END;

    if (!(pMD->IsNDirect()))
        COMPlusThrow(kArgumentException, IDS_EE_NOTNDIRECT);

    // This is revolting but the alternative is even worse. The unmanaged param count
    // isn't stored anywhere permanent so the only way to retrieve this value is
    // to recreate the MLStream. So this api will have horrible perf but almost
    // nobody uses it anyway.

    Stub  *pTempMLStub = NULL;
    PInvokeStaticSigInfo sigInfo(pMD);

    pTempMLStub = CreateNDirectStub(&sigInfo,NDIRECTSTUB_FL_FORCEML, pMD, NULL);

    cbParamBytes = ((MLHeader*)(pTempMLStub->GetEntryPoint()))->m_cbDstBuffer;
    pTempMLStub->DecRef();

    HELPER_METHOD_FRAME_END();
    return cbParamBytes;
}
FCIMPLEND



// Prelink
// Does advance loading of an N/Direct library
FCIMPL1(VOID, NDirect_Prelink_Wrapper, MethodDesc *pMeth)
{
    HELPER_METHOD_FRAME_BEGIN_0();

    CONTRACTL
    {
        THROWS;
        GC_TRIGGERS;
        MODE_COOPERATIVE;
        SO_TOLERANT;
    }
    CONTRACTL_END;

    NDirect_Prelink(pMeth);

    HELPER_METHOD_FRAME_END();
}
FCIMPLEND


VOID NDirect_Prelink(MethodDesc *pMeth)
{
    CONTRACTL
    {
        THROWS;
        GC_TRIGGERS;
        MODE_ANY;

        PRECONDITION(CheckPointer(pMeth));
    }
    CONTRACTL_END;

    // If the code is already ready, we are done. Else, we need to execute the prestub
    // This is a perf thing since it's always safe to execute the prestub twice.
    if (pMeth->IsCodeReady())
        return;

    // Silently ignore if not N/Direct and not runtime generated.
    if (!(pMeth->IsNDirect()) && !(pMeth->IsRuntimeSupplied()))
        return;

    pMeth->CheckRestore();
    pMeth->DoPrestub(NULL);
}



#endif // #ifndef DACCESS_COMPILE




#ifdef _X86_
// do not call this - it is pointer to instruction within NDirectGenericStubWorker
extern "C" void __stdcall NDirectGenericStubReturnFromCall(void);
#else
extern "C" PVOID NDirectGenericStubReturnFromCall;
#endif

static const int NDirectGenericWorkerStackSize = NDirectGenericWorkerFrameSize;


//==========================================================================
// NDirect debugger support
//==========================================================================

void NDirectMethodFrameGeneric::GetUnmanagedCallSite(TADDR* ip,
                                                     TADDR* returnIP,
                                                     TADDR* returnSP)
{
    CONTRACTL
    {
        THROWS;
        if (GetThread()) GC_TRIGGERS; else GC_NOTRIGGER;
        MODE_ANY;
    }
    CONTRACTL_END;

    if (ip != NULL)
        AskStubForUnmanagedCallSite(ip, NULL, NULL);

    NDirectMethodDesc *pMD =
        PTR_NDirectMethodDesc(PTR_HOST_TO_TADDR(GetFunction()));

    //
    // Get ML header
    //

    MLHeader *pheader;

    DWORD cbLocals;
    DWORD cbDstBuffer;
    DWORD cbThisCallInfo;

    if (pMD->IsVarArgs())
    {
        VASigCookie *pVASigCookie = GetVASigCookie();

        Stub *pTempMLStub;

        if (pVASigCookie->pNDirectMLStub != NULL)
        {
            pTempMLStub = pVASigCookie->pNDirectMLStub;

            pheader = PTR_MLHeader((TADDR)pTempMLStub->GetEntryPoint());

            cbLocals = pheader->m_cbLocals;
            cbDstBuffer = pheader->m_cbDstBuffer;
            cbThisCallInfo = (pheader->m_Flags & MLHF_THISCALL) ? 4 : 0;
        }
        else
        {
            // If we don't have a stub when we get here then we simply can't compute the return SP below. However, we
            // never actually ask for the return IP or SP when we don't have a stub yet anyway, so this is just fine.
            pheader = NULL;
            cbLocals = 0;
            cbDstBuffer = 0;
            cbThisCallInfo = 0;
        }
    }
    else
    {
        pheader = pMD->GetMLHeader();

        cbLocals = pheader->m_cbLocals;
        cbDstBuffer = pheader->m_cbDstBuffer;
        cbThisCallInfo = (pheader->m_Flags & MLHF_THISCALL) ? 4 : 0;
    }

    //
    // Compute call site info for NDirectGenericStubWorker
    //

    if (returnIP != NULL)
        *returnIP = GFN_TADDR(NDirectGenericStubReturnFromCall);

    //
    // !!! yow, this is a bit fragile...
    //

    if (returnSP != NULL)
#ifdef _X86_
        *returnSP = PTR_HOST_TO_TADDR(this)
                            - GetNegSpaceSize()
                            - NDirectGenericWorkerStackSize
                            - cbLocals
                            - cbDstBuffer
                            + cbThisCallInfo
                            - sizeof(TADDR);
#elif defined(_PPC_)
        *returnSP = PTR_HOST_TO_TADDR(this)
                            - offsetof(StubStackFrame, methodframe)
                            - NDirectGenericWorkerStackSize
                            - AlignStack(cbLocals + cbDstBuffer + NUM_FLOAT_ARGUMENT_REGISTERS * sizeof(DOUBLE))
                            ;
#else
        PORTABILITY_WARNING("NDirectMethodFrameGeneric::GetUnmanagedCallSite");
        *returnSP = 0;
#endif
}


#ifdef _X86_

void NDirectMethodFrameSlim::GetUnmanagedCallSite(TADDR* ip,
                                                  TADDR* returnIP,
                                                  TADDR* returnSP)
{
    CONTRACTL
    {
        THROWS;
        if (GetThread()) GC_TRIGGERS; else GC_NOTRIGGER;
        MODE_ANY;
    }
    CONTRACTL_END;


    AskStubForUnmanagedCallSite(ip, returnIP, returnSP);

    if (returnSP != NULL)
    {
        //
        // The slim worker pushes extra stack space, so
        // adjust our result obtained directly from the stub.
        //

        //
        // Get ML header
        //

        NDirectMethodDesc *pMD =
            PTR_NDirectMethodDesc(PTR_HOST_TO_TADDR(GetFunction()));

        MLHeader *pheader;
        if (pMD->IsVarArgs())
        {
            VASigCookie *pVASigCookie = GetVASigCookie();

            Stub *pTempMLStub = NULL;

            if (pVASigCookie->pNDirectMLStub != NULL)
                pTempMLStub = pVASigCookie->pNDirectMLStub;
            else
            {
                //
                // We don't support generating an ML stub inside the debugger
                // callback right now.
                //
                _ASSERTE(!"NYI");

                // Adding this line to prevent PREFast complaints
                pTempMLStub = (Stub*)0xBADF00D;
            }

            pheader = PTR_MLHeader((TADDR)pTempMLStub->GetEntryPoint());
            (*returnSP) -= pheader->m_cbLocals;
        }
        else
        {
            pheader = pMD->GetMLHeader();
            (*returnSP) -= pheader->m_cbLocals;
        }
    }
}

#endif // _X86_


//-----------------------------------------------------------------------------
// Gets information computed by the stublinker
// *targetIP will be set to the unmanaged target of the stub
// *returnIP will be IP (within the stub) right after the unmanaged call
// *returnSP will be set to the stack-pointer when the unmanaged call returns
//

void FramedMethodFrame::AskStubForUnmanagedCallSite(TADDR* targetIP,
                                                    TADDR* returnIP,
                                                    TADDR* returnSP)
{
    CONTRACTL
    {
        THROWS;
        if (GetThread()) { GC_TRIGGERS; } else { GC_NOTRIGGER; }
        MODE_ANY;
    }
    CONTRACTL_END;

    MethodDesc *pMD = GetFunction();

    // If we were purists, we'd create a new subclass that parents NDirect and ComPlus method frames
    // so we don't have these funky methods that only work for a subset.

    _ASSERTE(pMD->IsNDirect() || pMD->IsComPlusCall());

    if (returnIP != NULL || returnSP != NULL)
    {

        //

        //

        Stub *stub = pMD->GetStub();

        //
        // NDirect stub may have interceptors - skip them
        //

        while (stub && stub->IsIntercept())
            stub = *(PTR_InterceptStub(PTR_HOST_TO_TADDR(stub))->
                     GetInterceptedStub());

        //
        // Compute the pointers from the call site info in the stub
        //

        if (stub && (returnIP != NULL))
            *returnIP = stub->GetCallSiteReturnAddress();

        if (stub && (returnSP != NULL))
            *returnSP = PTR_HOST_TO_TADDR(this)
                            + FramedMethodFrame::GetOffsetOfReturnAddress()
                            - stub->GetCallSiteStackSize()
                            - sizeof(void*);
    }

    if (targetIP != NULL)
    {
        if (pMD->IsNDirect())
        {
            NDirectMethodDesc *pNMD =
                PTR_NDirectMethodDesc(PTR_HOST_TO_TADDR(pMD));

            *targetIP = (TADDR)pNMD->GetNativeNDirectTarget();
        }
        else
            _ASSERTE(0);
    }
}


BOOL NDirectMethodFrame::TraceFrame(Thread *thread, BOOL fromPatch,
                                    TraceDestination *trace, REGDISPLAY *regs)
{
    CONTRACTL
    {
        THROWS;
        if (GetThread()) GC_TRIGGERS; else GC_NOTRIGGER;
        MODE_ANY;

        PRECONDITION(CheckPointer(thread));
        PRECONDITION(CheckPointer(trace));
    }
    CONTRACTL_END;


    //
    // Get the call site info
    //

    TADDR ip, returnIP, returnSP;
    GetUnmanagedCallSite(&ip, &returnIP, &returnSP);

    //
    // If we've already made the call, we can't trace any more.
    //
    // !!! Note that this test isn't exact.
    //

    if (!fromPatch
        && (PTR_HOST_TO_TADDR(thread->GetFrame()) != PTR_HOST_TO_TADDR(this)
            || !thread->m_fPreemptiveGCDisabled
            || *PTR_TADDR(returnSP) == returnIP))
    {
        LOG((LF_CORDB, LL_INFO10000,
             "NDirectMethodFrame::TraceFrame: can't trace...\n"));
        return FALSE;
    }

    //
    // Otherwise, return the unmanaged destination.
    //

    trace->InitForUnmanaged(ip);

    LOG((LF_CORDB, LL_INFO10000,
         "NDirectMethodFrame::TraceFrame: ip=0x%p\n", ip));

    return TRUE;
}

//===========================================================================
//  Support for Pinvoke Calli instruction
//
//===========================================================================

#ifndef DACCESS_COMPILE

Stub* GetMLStubForCalli(VASigCookie *pVASigCookie, BOOL fVarargPinvoke)
{
    CONTRACT(Stub*)
    {
        THROWS;
        GC_TRIGGERS;
        ENTRY_POINT;
        MODE_ANY;
        PRECONDITION(CheckPointer(pVASigCookie));
        POSTCONDITION(CheckPointer(RETVAL, NULL_NOT_OK));
    }
    CONTRACT_END;

    Stub  *pTempMLStub = NULL;

    // this function is called by CLR to native assembly stubs which are called by 
    // managed code as a result, we need an unwind and continue handler to translate 
    // any of our internal exceptions into managed exceptions.
    INSTALL_UNWIND_AND_CONTINUE_HANDLER;


    DWORD       dwStubFlags = NDIRECTSTUB_FL_CONVSIGASVARARG | NDIRECTSTUB_FL_BESTFIT;
    MethodDesc* pMD = NULL;
    
    CorPinvokeMap unmgdCallConv = pmNoMangle;

    if (fVarargPinvoke)
    {
        unmgdCallConv = pmCallConvStdcall;
    }
    else
    {
        switch (MetaSig::GetCallingConvention(pVASigCookie->pModule, pVASigCookie->mdVASig))
        {
            case IMAGE_CEE_CS_CALLCONV_C:
                    unmgdCallConv = pmCallConvCdecl;
                    break;
            case IMAGE_CEE_CS_CALLCONV_STDCALL:
                    unmgdCallConv = pmCallConvStdcall;
                    break;
            case IMAGE_CEE_CS_CALLCONV_THISCALL:
                    unmgdCallConv = pmCallConvThiscall;
                    break;
            case IMAGE_CEE_CS_CALLCONV_FASTCALL:
                    unmgdCallConv = pmCallConvFastcall;
                    break;
            default:
                    COMPlusThrow(kTypeLoadException, IDS_INVALID_PINVOKE_CALLCONV);
        }
    }

    pTempMLStub = CreateNDirectStub(pVASigCookie->mdVASig, 
                                    SigParser::LengthOfSig(pVASigCookie->mdVASig),
                                    pVASigCookie->pModule, 
                                    mdMethodDefNil, 
                                    nltAnsi, 
                                    nlfNone, 
                                    unmgdCallConv,
                                    dwStubFlags, 
                                    pMD, 
                                    pVASigCookie);

    //
    // 
    //

    if (NULL != FastInterlockCompareExchangePointer( (void*volatile*)&(pVASigCookie->pNDirectMLStub),
                                                   pTempMLStub,
                                                   NULL ))
    {
        pTempMLStub->DecRef();
        pTempMLStub = pVASigCookie->pNDirectMLStub;
    }

    UNINSTALL_UNWIND_AND_CONTINUE_HANDLER;

    RETURN pTempMLStub;
}

#endif // #ifndef DACCESS_COMPILE



#ifndef DACCESS_COMPILE


#endif // #ifndef DACCESS_COMPILE

