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
 *
 * Purpose: Provide IValidate implementation.
 *          IValidate is used to validate PE stub, Metadata and IL.
 *
 *                                     
 *                                                         
 *
 * Date created : 14 March 2000
 *
 */

#include "common.h"

#include "corerror.h"
#include "vererror.h"
#include "ivalidator.h"
#include "securityattributes.h"
#include "corhost.h"
#include "verifier.hpp"
#include "comstring.h"
#include "pedecoder.h"
#include "comcallablewrapper.h"
#include "../dlls/mscorrc/resource.h"
#include "posterror.h"
#include "comcallablewrapper.h"
#include "eeconfig.h"
#include "corhost.h"
#include "security.h"
#include "appdomain.inl"

typedef void (*VerifyErrorHandler)(void* pThis, HRESULT hrError, struct VerErrorStruct* pError);

class CValidator
{
public:
    CValidator(IVEHandler *veh) : m_veh(veh) 
    {
        LEAF_CONTRACT;
    }
    HRESULT VerifyAllMethodsForClass(Module *pModule, mdTypeDef cl, ValidateWorkerArgs* pArgs);
    HRESULT VerifyAllGlobalFunctions(Module *pModule, ValidateWorkerArgs* pArgs);
    HRESULT VerifyAssembly(Assembly *pAssembly, ValidateWorkerArgs* pArgs);
    HRESULT VerifyModule(Module* pModule, ValidateWorkerArgs* pArgs);
    HRESULT ReportError(HRESULT hr, ValidateWorkerArgs* pArgs, mdToken tok=0);
    HRESULT VerifyMethod(COR_ILMETHOD_DECODER* pILHeader, IVEHandler* pVEHandler, WORD wFlags, ValidateWorkerArgs* pArgs);
    void HandleError(HRESULT hrError, struct VerErrorStruct* pError);

private:
    IVEHandler *m_veh;
    ValidateWorkerArgs* m_pArgs;
};

HRESULT CValidator::ReportError(HRESULT hr, ValidateWorkerArgs* pArgs, mdToken tok /* = 0 */)
{
    WRAPPER_CONTRACT;
    if (m_veh == NULL)
        return hr;

    HRESULT hr2 = E_FAIL;
    BEGIN_SO_INTOLERANT_CODE(GetThread());
    VEContext vec;

    memset(&vec, 0, sizeof(VEContext));

    if (tok != 0)
    {
        vec.flags = VER_ERR_TOKEN;
        vec.Token = tok;
    }

    hr2 =  Verifier::ReportError(m_veh, hr, &vec, pArgs);
    END_SO_INTOLERANT_CODE;
    return hr2;
}

HRESULT CValidator::VerifyAllMethodsForClass(Module *pModule, mdTypeDef cl, ValidateWorkerArgs* pArgs)
{
    CONTRACTL {
        THROWS;
        GC_TRIGGERS;
        MODE_ANY;
    } CONTRACTL_END;

    HRESULT hr = S_OK;
    MethodTable *pMT = NULL;
     
    // In the case of COR_GLOBAL_PARENT_TOKEN (i.e. global functions), it is guaranteed
    // that the module has a method table or our caller will have skipped this step.
    TypeHandle th;
    {
        // Although there's no assert to disable here, there is an OOM-reliability bug here
        // that needs to be fixed. We are ignoring the HRESULT from the loader here.
        // That could cause an OOM failure to be disguised as something else. OOM's
        // need to be handled or propagated up to the caller.
        CONTRACT_VIOLATION(0);
        
        hr = ClassLoader::LoadTypeDefOrRefNoThrow(pModule, cl, &th, NULL, 
                                             ClassLoader::ReturnNullIfNotFound, 
                                             ClassLoader::PermitUninstDefOrRef);

        if (FAILED(hr)) {
            if ((hr==COR_E_TYPELOAD) || (hr==VER_E_TYPELOAD)) {
                hr = ReportError(hr, pArgs,cl);
            } else {
                hr = ReportError(hr, pArgs);
            }
            goto Exit;
        } else {
            pMT = th.GetMethodTable();
        }
    }

    if (pMT == NULL)
    {
        hr = ReportError(VER_E_TYPELOAD, pArgs, cl);
        goto Exit;
    }

    g_fVerifierOff = false;

    {
        // Verify all methods in class - excluding inherited methods
        MethodTable::MethodIterator it(pMT);
        for (; it.IsValid(); it.Next())
        {
            pArgs->pMethodDesc = it.GetMethodDesc();

            if (pArgs->pMethodDesc && 
                pArgs->pMethodDesc->GetMethodTable() == pMT &&
                pArgs->pMethodDesc->IsIL() && 
                !pArgs->pMethodDesc->IsAbstract() && 
                !pArgs->pMethodDesc->IsUnboxingStub())
            {
                COR_ILMETHOD_DECODER::DecoderStatus status;
                COR_ILMETHOD_DECODER ILHeader(pArgs->pMethodDesc->GetILHeader(), 
                                              pArgs->pMethodDesc->GetMDImport(), &status); 

                if (status == COR_ILMETHOD_DECODER::SUCCESS)
                {
                    hr = VerifyMethod(&ILHeader, m_veh, VER_FORCE_VERIFY, pArgs);
                }
                else if (status == COR_ILMETHOD_DECODER::VERIFICATION_ERROR)
                {
                    hr = COR_E_VERIFICATION;
                }
                else if (status == COR_ILMETHOD_DECODER::FORMAT_ERROR)
                {
                    hr = COR_E_BADIMAGEFORMAT;
                }
                else
                {
                    _ASSERTE(!"Unhandled status from COR_ILMETHOD_DECODER");
                }

                if (FAILED(hr))
                    hr = ReportError(hr, pArgs);

                if (FAILED(hr))
                    goto Exit;
            }
            if (CLRTaskHosted())
                ClrSleepEx(0, FALSE);
        }
    }

Exit:
    pArgs->pMethodDesc = NULL;
    return hr;
}

void MethodDescAndCorILMethodDecoderToCorInfoMethodInfo(MethodDesc* ftn, COR_ILMETHOD_DECODER* ILHeader, CORINFO_METHOD_INFO* pMethodInfo)
{
    pMethodInfo->ftn = CORINFO_METHOD_HANDLE(ftn);
    pMethodInfo->scope = CORINFO_MODULE_HANDLE(ftn->GetModule());
    pMethodInfo->ILCode = const_cast<BYTE*>(ILHeader->Code);
    pMethodInfo->ILCodeSize = ILHeader->GetCodeSize();
    pMethodInfo->maxStack = ILHeader->GetMaxStack();
    pMethodInfo->EHcount = ILHeader->EHCount();
    pMethodInfo->options =
        (CorInfoOptions)
        (((ILHeader->GetFlags() & CorILMethod_InitLocals) ? CORINFO_OPT_INIT_LOCALS : 0) |
         (ftn->AcquiresInstMethodTableFromThis() ? CORINFO_GENERICS_CTXT_FROM_THIS : 0) |
         (ftn->RequiresInstArg() ? CORINFO_GENERICS_CTXT_FROM_PARAMTYPEARG : 0));

    PCCOR_SIGNATURE  sigToConvert = NULL;
    sigToConvert = ftn->GetSig();
    CONSISTENCY_CHECK(NULL != sigToConvert);
    // fetch the method signature
    CEEInfo::ConvToJitSig(sigToConvert, pMethodInfo->scope, mdTokenNil, &pMethodInfo->args, (CORINFO_METHOD_HANDLE) ftn, false);

    //@GENERICS:
    // Shared generic methods and shared methods on generic structs take an extra argument representing their instantiation
    if (ftn->RequiresInstArg())
        pMethodInfo->args.callConv = (CorInfoCallConv) (pMethodInfo->args.callConv | CORINFO_CALLCONV_PARAMTYPE);

    // method attributes and signature are consistant
    _ASSERTE(!!ftn->IsStatic() == ((pMethodInfo->args.callConv & CORINFO_CALLCONV_HASTHIS) == 0));

    // And its local variables
    CEEInfo::ConvToJitSig(ILHeader->LocalVarSig, pMethodInfo->scope, mdTokenNil, &pMethodInfo->locals, (CORINFO_METHOD_HANDLE) ftn, true);
}

void PEVerifyErrorHandler(void* pThis, HRESULT hrError, struct VerErrorStruct* pError)
{
    WRAPPER_CONTRACT;
    STATIC_CONTRACT_SO_TOLERANT;
    ((CValidator*)pThis)->HandleError(hrError, pError);
}

void CValidator::HandleError(HRESULT hrError, struct VerErrorStruct* pError)
{
    WRAPPER_CONTRACT;
    BEGIN_SO_INTOLERANT_CODE(GetThread());
    _ASSERTE(sizeof(VEContext) == sizeof(struct VerErrorStruct));
    Verifier::ReportError(m_veh, hrError, (VEContext*)pError, m_pArgs);
    END_SO_INTOLERANT_CODE;
}
typedef void (__stdcall* VerifyFunc)(ICorJitInfo* pJitInfo, CORINFO_METHOD_INFO* pMethodInfo, VerifyErrorHandler pErrorHandler, void* pThis);
static void VerifyMethodHelper(VerifyFunc pVerFunc, CEEJitInfo* pJI, CORINFO_METHOD_INFO* pMethodInfo, void* pThis)
{
    // Helper method to allow us to use SO_TOLERANT_CODE macro
    WRAPPER_CONTRACT;
    BEGIN_SO_TOLERANT_CODE(GetThread());
    // Verify the method
    pVerFunc(pJI, pMethodInfo, PEVerifyErrorHandler, pThis);
    END_SO_TOLERANT_CODE;
    
}

HRESULT CValidator::VerifyMethod(COR_ILMETHOD_DECODER* pILHeader, IVEHandler* pVEHandler, WORD wFlags, ValidateWorkerArgs* pArgs)
{
    
    CONTRACTL {
        THROWS;
        GC_TRIGGERS;
        MODE_ANY;
    } CONTRACTL_END;

    HRESULT hr = S_OK;
    EX_TRY
    {
        // Find the DLL entrypoint
        m_pArgs = pArgs;
        static volatile VerifyFunc pVerFunc = NULL;
        if (pVerFunc == NULL)
        {
            HINSTANCE hJit64 = CLRLoadSystemLibrary(L"peverify.dll");

            pVerFunc = (VerifyFunc)GetProcAddress(hJit64, "VerifyMethod");            
        }

        if(!pVerFunc)
        {
            _ASSERTE(!"Failed to load peverify.dll or find VerifyMethod proc address");
            hr = E_FAIL;
        }
        else
        {
            Thread *pThread = GetThread();
            if (pThread->IsAbortRequested())
            {
                pThread->HandleThreadAbort();
            }
            // Prepare the args
            MethodDesc* ftn = pArgs->pMethodDesc;
            IJitManager* jitMan;
            /* const BYTE* methodStart = */ ftn->GetFunctionAddress(&jitMan);
            CEEJitInfo ji(pArgs->pMethodDesc, pILHeader, jitMan, CORJIT_FLG_IMPORT_ONLY);
            CORINFO_METHOD_INFO methodInfo;
            MethodDescAndCorILMethodDecoderToCorInfoMethodInfo(ftn, pILHeader, &methodInfo);

            // don't block the GC while verifying the method because it might take a while
            GCX_PREEMP();

            // Verify the method
            VerifyMethodHelper(pVerFunc, &ji, &methodInfo, this);
        }
    }
    EX_CATCH
    {
        // Catch and report any errors that peverify.dll lets fall through (ideally that should never happen)
        hr = Security::MapToHR(GETTHROWABLE());
        hr = ReportError(hr, pArgs);
    }
    EX_END_CATCH(RethrowTerminalExceptions)

    return hr;
}
// Helper function to verify the global functions
HRESULT CValidator::VerifyAllGlobalFunctions(Module *pModule, ValidateWorkerArgs* pArgs)
{
    WRAPPER_CONTRACT;

    HRESULT hr = S_OK;
    BEGIN_SO_INTOLERANT_CODE(GetThread());
    // Is there anything worth verifying?
    if (pModule->GetGlobalMethodTable())
        hr =  VerifyAllMethodsForClass(pModule, COR_GLOBAL_PARENT_TOKEN, pArgs);
    END_SO_INTOLERANT_CODE;
    return hr;
}

HRESULT CValidator::VerifyModule(Module* pModule, ValidateWorkerArgs* pArgs)
{
    CONTRACTL {
        THROWS;
        GC_TRIGGERS;
        MODE_ANY;
    } CONTRACTL_END;

    // Get a count of all the classdefs and enumerate them.
    HRESULT   hr;
    mdTypeDef td;
    HENUMInternal      hEnum;
    IMDInternalImport *pMDI;

    if (pModule == NULL)
    {
        hr = ReportError(VER_E_BAD_MD, pArgs);
        goto Exit;
    }

    pMDI = pModule->GetMDImport();

    if (pMDI == NULL)
    {
        hr = ReportError(VER_E_BAD_MD, pArgs);
        goto Exit;
    }

    hr = pMDI->EnumTypeDefInit(&hEnum);

    if (FAILED(hr))
    {
        hr = ReportError(hr, pArgs);
        goto Exit;
    }

    // First verify all global functions - if there are any
    hr = VerifyAllGlobalFunctions(pModule, pArgs);

    if (FAILED(hr))
        goto Cleanup;
    
    while (pModule->GetMDImport()->EnumTypeDefNext(&hEnum, &td))
    {
        hr = VerifyAllMethodsForClass(pModule, td, pArgs);

        if (FAILED(hr))
            goto Cleanup;
    }

Cleanup:
    pModule->GetMDImport()->EnumTypeDefClose(&hEnum);

Exit:
    return hr;
}

HRESULT CValidator::VerifyAssembly(Assembly *pAssembly, ValidateWorkerArgs* pArgs)
{
    CONTRACTL {
        THROWS;
        GC_TRIGGERS;
        MODE_ANY;
    } CONTRACTL_END;

    HRESULT hr;
    mdToken mdFile;
    DomainFile* pModule;
    HENUMInternal phEnum;

    _ASSERTE(pAssembly->GetManifestImport());

    // Verify the module containing the manifest. There is no
    // FileRefence so will no show up in the list.
    hr = VerifyModule(pAssembly->GetManifestModule(), pArgs);

    if (FAILED(hr))
        goto Exit;

    hr = pAssembly->GetManifestImport()->EnumInit(mdtFile, mdTokenNil, &phEnum);

    if (FAILED(hr)) 
    {
        hr = ReportError(hr, pArgs);
        goto Exit;
    }

    while(pAssembly->GetManifestImport()->EnumNext(&phEnum, &mdFile)) 
    {
        pModule = pAssembly->GetManifestModule()->LoadModule(GetAppDomain(), mdFile, FALSE);

        if (pModule != NULL)
        {
            hr = VerifyModule(pModule->GetModule(), pArgs);

            if (FAILED(hr)) 
                goto Exit;
        }
    }

Exit:
    return hr;
}

static void ValidateWorker(LPVOID /* ValidateWorker_Args */ ptr)
{
    CONTRACTL {
        THROWS;
        GC_TRIGGERS;
        MODE_ANY;
    } CONTRACTL_END;

    ValidateWorkerArgs *args = (ValidateWorkerArgs *) ptr;
    AppDomain *pDomain = GetThread()->GetDomain();

    {
        // Set up the domain to resolve all dependency assemblies for introspection
        struct _gc {
            OBJECTREF orAppDomain;
        } gc;
        ZeroMemory(&gc, sizeof(gc));
        GCX_COOP();
        GCPROTECT_BEGIN(gc);
        gc.orAppDomain = pDomain->GetExposedObject();
        MethodDescCallSite meth(METHOD__APP_DOMAIN__ENABLE_RESOLVE_ASSEMBLIES_FOR_INTROSPECTION, &gc.orAppDomain);
        ARG_SLOT args[1] = { ObjToArgSlot(gc.orAppDomain) };
        meth.Call(args);
        GCPROTECT_END();
    }

    Assembly *pAssembly;
    if (args->wszFileName)
    {
        // Load the primary assembly for introspection
        AssemblySpec spec;
        spec.SetCodeBase(args->wszFileName, (DWORD) wcslen(args->wszFileName) + 1);
        spec.SetIntrospectionOnly(TRUE);
        pAssembly = spec.LoadAssembly(FILE_LOADED);
    }
    else
    {
        PEAssemblyHolder pFile(PEAssembly::OpenMemory(SystemDomain::System()->SystemFile(),
                                                      args->pe, args->size, TRUE));
        pAssembly = pDomain->LoadAssembly(NULL, pFile, FILE_LOADED);
    }

    // Verify the assembly
    args->hr = args->val->VerifyAssembly(pAssembly, args);
}

static HRESULT ValidateHelper(
        IVEHandler        *veh,
        IUnknown          *pAppDomain,
        DWORD              ulAppDomainId,
        BOOL               UseId,
        unsigned long      ulFlags,
        unsigned long      ulMaxError,
        unsigned long      token,
        __in_z LPWSTR             fileName,
        BYTE               *pe,
        unsigned long      ulSize)
{
    CONTRACTL {
        THROWS;
        GC_TRIGGERS;
        MODE_ANY;
        SO_TOLERANT;
    } CONTRACTL_END;

    Thread  *pThread = GetThread();

    if (pe == NULL)
        return E_POINTER;

    HRESULT hr = S_OK;
    BEGIN_SO_INTOLERANT_CODE(pThread);
    ADID pDomain;
    ValidateWorkerArgs args;
    CValidator val(veh);
    AppDomainFromIDHolder ad;

    BOOL Chk = FALSE;
    BOOL UnloadDomain = FALSE;

    GCX_COOP();

    EX_TRY {
        PEDecoder pev(pe, (COUNT_T)ulSize);

        args.wszFileName = fileName;
        args.fVerbose = (ulFlags & VALIDATOR_EXTRA_VERBOSE) ? true : false;
        args.fShowSourceLines = (ulFlags & VALIDATOR_SHOW_SOURCE_LINES) ? true : false;
        args.val = &val;
        args.pe = pe;
        args.size = ulSize;

        if((ulFlags & VALIDATOR_NOCHECK_PEFORMAT) == 0)
        {
            // Verify the PE header / native stubs first
            // @bug?: This validation is not performed on non-manifest modules
            Chk = ((ulFlags & VALIDATOR_CHECK_ILONLY) != 0) ? (BOOL) pev.CheckILOnlyFormat() :
                                                              (BOOL) pev.CheckILFormat();
            if (!Chk)
            {
                hr = val.ReportError(VER_E_BAD_PE, &args);

                if (FAILED(hr))
                    goto End;
            }
        }
        if((ulFlags & VALIDATOR_CHECK_PEFORMAT_ONLY) != 0)
            goto End;

        if (fileName)
        {
            AppDomain* pAD = AppDomain::CreateDomainContext(fileName);
            UnloadDomain = TRUE;
            pAD->SetPassiveDomain();
            pDomain=pAD->GetId();
        }
        else if (UseId)
        {
            pDomain = (ADID)ulAppDomainId;
        }
        else
        {
            SystemDomain::LockHolder lh;
            ComCallWrapper* pWrap = GetCCWFromIUnknown(pAppDomain);
            if (pWrap == NULL)
            {
                hr = COR_E_APPDOMAINUNLOADED;
                goto End;
            }
            pDomain = pWrap->GetDomainID();
        }

        if (FAILED(hr)) 
        {
            hr = val.ReportError(hr, &args);
            goto End;
        }

        ad.Assign(pDomain, TRUE);
        if (ad.IsUnloaded())
            COMPlusThrow(kAppDomainUnloadedException);
        if (ad->IsIllegalVerificationDomain())
            COMPlusThrow(kFileLoadException, IDS_LOADINTROSPECTION_DISALLOWED);
        ad->SetVerificationDomain();
        ad.Release();

        args.val = &val;

        // We need a file path here.  This is to do a fusion bind, and also
        // to make sure we can find any modules in the assembly.  We assume
        // that the path points to the same place the bytes came from, which is true
        // with PEVerify, but perhaps not with other clients.

        if (pDomain != pThread->GetDomain()->GetId())
        {
            pThread->DoADCallBack(
                pDomain, ValidateWorker, &args);
        }
        else
        {
            ValidateWorker(&args);
        }

        if (FAILED(args.hr))
            hr = val.ReportError(args.hr, &args);

        // Only Unload the domain if we created it.
        if (UnloadDomain)
            AppDomain::UnloadById(pDomain,TRUE);
End:;

    }
    EX_CATCH
    {
        hr = Security::MapToHR(GETTHROWABLE());
        hr = val.ReportError(hr, &args);
    }
    EX_END_CATCH(RethrowSOExceptions)

    END_SO_INTOLERANT_CODE;
    return hr;
}

HRESULT CorValidator::Validate(
        IVEHandler        *veh,
        IUnknown          *pAppDomain,
        unsigned long      ulFlags,
        unsigned long      ulMaxError,
        unsigned long      token,
        __in_z LPWSTR             fileName,
        BYTE               *pe,
        unsigned long      ulSize)
{
    WRAPPER_CONTRACT;
    STATIC_CONTRACT_SO_TOLERANT;
    return ValidateHelper(veh, pAppDomain, 0, FALSE, ulFlags, ulMaxError,
                          token, fileName, pe, ulSize);
}

HRESULT CLRValidator::Validate(
        IVEHandler        *veh,
        unsigned long      ulAppDomainId,
        unsigned long      ulFlags,
        unsigned long      ulMaxError,
        unsigned long      token,
        __in_z LPWSTR             fileName,
        BYTE               *pe,
        unsigned long      ulSize)
{
    WRAPPER_CONTRACT;
    STATIC_CONTRACT_SO_TOLERANT;
    return ValidateHelper(veh, NULL, ulAppDomainId, TRUE, ulFlags, ulMaxError,
                          token, fileName, pe, ulSize);
}

void GetFormattingErrorMsg(__out_ecount(ulMaxLength) __out_z LPWSTR msg, unsigned long ulMaxLength)
{
    CONTRACTL {
        NOTHROW;
        GC_TRIGGERS;
        MODE_ANY;
        PRECONDITION(ulMaxLength >= 30);
    } CONTRACTL_END;

    EX_TRY
    {
        SString s;
        s.LoadResource(IDS_VER_E_FORMATTING);
        wcsncpy_s(msg, ulMaxLength, s.GetUnicode(), _TRUNCATE);
    }
    EX_CATCH
    {
        wcscpy_s(msg, ulMaxLength, L"Error loading resource string");
    }
    EX_END_CATCH(SwallowAllExceptions)
}

static HRESULT FormatEventInfoHelper(
        HRESULT            hVECode,
        VEContext          Context,
        LPWSTR             msg,
        unsigned long      ulMaxLength,
        SAFEARRAY          *psa)
{
    CONTRACTL {
        THROWS;
        GC_TRIGGERS;
        MODE_ANY;
        PRECONDITION(ulMaxLength >= 30);
        SO_TOLERANT;
    } CONTRACTL_END;

    BEGIN_SO_INTOLERANT_CODE(GetThread());

    VerError err;
    memcpy(&err, &Context, sizeof(VerError));

    ValidateWorkerArgs argsDefault;
    ValidateWorkerArgs* pArgs = &argsDefault;

    // We passed a pointer to the ValidateWorkerArgs object through
    // the SAFEARRAY casted as a UINT because there was no room left in the
    // interface to pass information through it.
    {
        UINT dim;
        LONG l;
        VARIANT var;

        if(!psa) {
            goto lDone;
        }

        dim = SafeArrayGetDim(psa);            
        if (dim != 1) {
            _ASSERTE(!"There should be one element in the SafeArray");
            goto lDone;
        }

        if (FAILED(SafeArrayGetLBound(psa, 1, &l))) {
            _ASSERTE(false);
            goto lDone;
        }
        if (l != 0) {
            _ASSERTE(!"expected the lower bound to be zero");
            goto lDone;
        }

        if (FAILED(SafeArrayGetUBound(psa, 1, &l))) {
            _ASSERTE(false);
            goto lDone;
        }
        if (l != 0) {
            _ASSERTE(!"expected the upper bound to be zero");
            goto lDone;
        }
        l = 0;
        SafeArrayGetElement(psa, &l, &var);

        // We don't check that the type is V_UINT here because that check fails on Win2K when it should pass
        pArgs = (ValidateWorkerArgs*)(size_t)V_UINT(&var);

    }
lDone: ;

    EX_TRY
    {
        Verifier::GetErrorMsg(hVECode, err, msg, ulMaxLength, pArgs);
    }
    EX_CATCH
    {
        GetFormattingErrorMsg(msg, ulMaxLength);
    }
    EX_END_CATCH(SwallowAllExceptions)

    END_SO_INTOLERANT_CODE;
    return S_OK;
}

HRESULT CorValidator::FormatEventInfo(
        HRESULT            hVECode,
        VEContext          Context,
        __out_ecount(ulMaxLength) LPWSTR             msg,
        unsigned long      ulMaxLength,
        SAFEARRAY          *psa)
{
    WRAPPER_CONTRACT;
    return FormatEventInfoHelper(hVECode, Context, msg, ulMaxLength, psa);
}

HRESULT CLRValidator::FormatEventInfo(
        HRESULT            hVECode,
        VEContext          Context,
        __out_ecount(ulMaxLength) LPWSTR             msg,
        unsigned long      ulMaxLength,
        SAFEARRAY          *psa)
{
    WRAPPER_CONTRACT;
    STATIC_CONTRACT_SO_TOLERANT;
    return FormatEventInfoHelper(hVECode, Context, msg, ulMaxLength, psa);
}
