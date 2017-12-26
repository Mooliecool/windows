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
/* ------------------------------------------------------------------------- *
 * debug\comshell.cpp: com debugger shell functions
 * ------------------------------------------------------------------------- */

#include "stdafx.h"
#include "palclr.h"

#ifdef PLATFORM_UNIX
#define ENVIRONMENT_SEPARATOR L':'
#else
#define ENVIRONMENT_SEPARATOR L';'
#endif

// The Shell project is for internal testing purposes only at this point.
// We are therefore not going to scrub the code.  It should be moved out
// of the dev project longer term.


#include "dshell.h"

#include "corffi.h"


static const GUID CLSID_EmbeddedCLRCorDebugWhidbeyV1 =
        {0x48c2daf2, 0xf746, 0x41d8, {0xb0, 0xa9, 0xd8, 0xfe, 0x3a, 0x7, 0x53, 0xbb}};

static const GUID CLSID_EmbeddedCLRCorDebugWhidbeyV2 =
        {0x83f64acd, 0x26bf, 0x4ffe, {0x87, 0xa8, 0x43, 0xae, 0x2c, 0x98, 0x72, 0xe3}};

/* ------------------------------------------------------------------------- *
 * Def for a signature formatter, stolen from the internals of the Runtime
 * ------------------------------------------------------------------------- */
class SigFormat
{
public:
    SigFormat(IMetaDataImport *importer, PCCOR_SIGNATURE sigBlob, ULONG sigBlobSize, const WCHAR *methodName);
    ~SigFormat();

    HRESULT FormatSig();
    WCHAR *GetString();
    ULONG AddType(PCCOR_SIGNATURE sigBlob);

protected:
    WCHAR*           _fmtSig;
    int              _size;
    int              _pos;
    PCCOR_SIGNATURE  _sigBlob;
    ULONG            _sigBlobSize;
    const WCHAR     *_memberName;
    IMetaDataImport *_importer;

    int AddSpace();
    int AddString(const WCHAR *s);
};

/* ------------------------------------------------------------------------- *
 * Globals
 * ------------------------------------------------------------------------- */

DebuggerShell        *g_pShell = NULL;

#if DOSPEW
#define SPEW(s) s
#else
#define SPEW(s)
#endif

WCHAR *FindSep(                         // Pointer to separator or null.
    const WCHAR *szPath)                // The path to look in.
{
    WCHAR *ptr = const_cast<WCHAR*>(wcsrchr(szPath, L'.'));
    if (ptr && ptr - 1 >= szPath && *(ptr - 1) == L'.')
        --ptr;
    return ptr;
}

// Helper to print an AppDomain name for a given frame.
void PrintADNameFromFrame(ICorDebugFrame * pFrame)
{
    if (pFrame == NULL)
    {
        g_pShell->Write(L"(Unknown)");
        return;

    }
    ReleaseHolder<ICorDebugFunction>  pFunction;
    ReleaseHolder<ICorDebugModule>    pModule;
    ReleaseHolder<ICorDebugAssembly>  pAssembly;
    ReleaseHolder<ICorDebugAppDomain> pAppDomain;

    DebuggerString szName;
    ULONG32 chLen = 0;
    ULONG32 id = (ULONG32)-1;

    pFrame->GetFunction(&pFunction);
    if (pFunction == NULL)
        goto LSkipPrintingAD;

    pFunction->GetModule(&pModule);
    if (pModule == NULL)
        goto LSkipPrintingAD;

    pModule->GetAssembly(&pAssembly);
    if (pAssembly == NULL)
        goto LSkipPrintingAD;

    pAssembly->GetAppDomain(&pAppDomain);
    if (pAppDomain == NULL)
        goto LSkipPrintingAD;

    // Now that we have the AppDomain, print the name.
    if (FAILED(pAppDomain->GetID(&id)))
        goto LSkipPrintingAD;

    if (FAILED(pAppDomain->GetName(0, &chLen, NULL)))
        goto LSkipPrintingAD;

    szName.Allocate(chLen - 1);
    pAppDomain->GetName(chLen, NULL, szName.GetData());

    g_pShell->Write(L"(AD '%s', #%u) ", (szName.GetData() == NULL) ? L"<no name>" : szName.GetData(), id);

LSkipPrintingAD:
    ;
}


// Helper to print a hint string for an AppDomain internal frame.
// This will print something like:
// (AD 'Class1.exe', #1) -->(AD 'Second!', #2)
void PrintInternalFrameADHint(ICorDebugInternalFrame *pFrame)
{
    CorDebugInternalFrameType eType;
    pFrame->GetFrameType(&eType);
    _ASSERTE(eType == STUBFRAME_APPDOMAIN_TRANSITION);

    // Get From & To appdomains.
    ReleaseHolder<ICorDebugFrame> pFrom;
    ReleaseHolder<ICorDebugFrame> pTo;

    pFrame->GetCaller(&pFrom);
    pFrame->GetCallee(&pTo);

    PrintADNameFromFrame(pFrom);
    g_pShell->Write(L"-->");
    PrintADNameFromFrame(pTo);
}

// Print an internal Frame.
// This will not print a trailing new line.
void PrintInternalFrame(ICorDebugInternalFrame * iInternalFrame)
{
    _ASSERTE(iInternalFrame != NULL);
    ReleaseHolder<ICorDebugFunction>      ifunction;
    DebuggerFunction       *function = NULL;

    const WCHAR * szType = L"Unknown Type";
    CorDebugInternalFrameType eType;
    HRESULT hr = iInternalFrame->GetFrameType(&eType);
    if (SUCCEEDED(hr))
    {
        switch(eType)
        {
        case STUBFRAME_M2U:
            szType = L"M-->U";
            break;

        case STUBFRAME_U2M:
            szType = L"U-->M";
            break;

        case STUBFRAME_APPDOMAIN_TRANSITION:
            {
                g_pShell->Write(L"[Internal Frame, 'AD switch':");
                PrintInternalFrameADHint(iInternalFrame);
                g_pShell->Write(L"]");
            }
            return;

        case STUBFRAME_LIGHTWEIGHT_FUNCTION:
            szType = L"Dynamic Method";
            break;


        case STUBFRAME_FUNC_EVAL:
            szType = L"FuncEval";
            break;

        case STUBFRAME_INTERNALCALL:
            szType = L"Internal Call";
            break;

        case STUBFRAME_NONE:
            break; // L"Unknown Type"

        }
    }

    g_pShell->Write(L"[Internal Frame, '%s', ", szType);

    // Get the DebuggerFunction for the function iface
    hr = iInternalFrame->GetFunction(&ifunction);
    if (FAILED(hr))
    {
        g_pShell->Write(L"(no function, hr=0x%08x)]", hr);
    }
    else
    {
        function = DebuggerFunction::FromCorDebug(ifunction);
        _ASSERTE(function);
        g_pShell->Write(L"%s%s::%s]",
        function->GetNamespaceName(),
        function->GetClassName(),
        function->GetName());
    }
}


// Helper to get a string name for a given chain reason...
WCHAR * GetChainReasonName(CorDebugChainReason reason)
{
    LPWSTR reasonString = NULL;

    switch (reason)
    {
    case CHAIN_PROCESS_START:
    case CHAIN_THREAD_START:
        break;

    case CHAIN_ENTER_MANAGED:
        reasonString = L"Managed transition";
        break;

    case CHAIN_ENTER_UNMANAGED:
        reasonString = L"Unmanaged transition";
        break;

    case CHAIN_CLASS_INIT:
        reasonString = L"Class initialization";
        break;

    case CHAIN_DEBUGGER_EVAL:
        reasonString = L"Debugger evaluation";
        break;

    case CHAIN_EXCEPTION_FILTER:
        reasonString = L"Exception filter";
        break;

    case CHAIN_SECURITY:
        reasonString = L"Security";
        break;

    case CHAIN_CONTEXT_POLICY:
        reasonString = L"Context policy";
        break;

    case CHAIN_CONTEXT_SWITCH:
        reasonString = L"Context switch";
        break;

    case CHAIN_INTERCEPTION:
        reasonString = L"Interception";
        break;

    case CHAIN_FUNC_EVAL:
        reasonString = L"Function Evaluation";
        break;

    default:
        reasonString = NULL;
    }


    return reasonString;
}

// Returns controller via *ppController out-param.
// We add-ref the controller and caller must release it. Caller should use smart pointer to manage this.
void GetControllerInterface(ICorDebugAppDomain *pAppDomain, ICorDebugController ** ppController)
{
    ReleaseHolder<ICorDebugProcess> pProcess;
    _ASSERTE(ppController != NULL);
    _ASSERTE(*ppController == NULL);
    HRESULT hr = S_OK;

    hr = pAppDomain->GetProcess(&pProcess);
    if (FAILED(hr))
    {
        // No controller, leave out-param as null.
        return;
    }

    hr = pProcess->QueryInterface(IID_ICorDebugController,
                                  (void**)ppController);

    _ASSERTE(*ppController != NULL);
}

HRESULT DebuggerCallback::CreateProcess(ICorDebugProcess *pProcess)
{
    g_pShell->m_enableCtrlBreak = false;
    DWORD pid = 0;

    SPEW(fprintf(stderr, "[%d] DC::CreateProcess.\n", GetCurrentThreadId()));

    pProcess->GetID(&pid);
    g_pShell->Write(L"Process %d/0x%x created.\n", pid, pid);
    g_pShell->SetTargetProcess(pProcess);

    SPEW(fprintf(stderr, "[%d] DC::CP: creating process.\n", GetCurrentThreadId()));
    SPEW(fprintf(stderr, "[%d] DC::CP: returning.\n", GetCurrentThreadId()));

    // Also initialize the source file search path
    g_pShell->m_FPCache.Init ();

    g_pShell->m_gotFirstThread = false;

    const BOOL bAllowJitOpts = (g_pShell->m_rgfActiveModes & DSM_ENABLE_JIT_OPTIMIZATIONS);
    ReleaseHolder<ICorDebugProcess2> pProcess2;
    HRESULT hr = pProcess->QueryInterface(IID_ICorDebugProcess2, (void**) &pProcess2);
    if (SUCCEEDED(hr))
    {
        DWORD flags = CORDEBUG_JIT_DEFAULT;
        if (! bAllowJitOpts)
        {
            flags |= CORDEBUG_JIT_DISABLE_OPTIMIZATION;
        }
        hr = pProcess2->SetDesiredNGENCompilerFlags(flags);
        if (FAILED(hr) && hr != CORDBG_E_NGEN_NOT_SUPPORTED && hr != CORDBG_E_CANNOT_BE_ON_ATTACH)
        {
            g_pShell->Write(L"Failed to set NGEN compiler flags for Process\n");
            g_pShell->ReportError(hr);
        }
    }

    g_pShell->Continue(pProcess, NULL);

    return (S_OK);
}

HRESULT DebuggerCallback::ExitProcess(ICorDebugProcess *pProcess)
{
    g_pShell->m_enableCtrlBreak = false;
    SPEW(fprintf(stderr, "[%d] DC::ExitProcess.\n", GetCurrentThreadId()));

    g_pShell->Write(L"Process exited.\n");

    g_pShell->ClearHandles();
    if (g_pShell->m_targetProcess == pProcess)
        g_pShell->SetTargetProcess(NULL);

    g_pShell->Stop(NULL, NULL);

    SPEW(fprintf(stderr, "[%d] DC::EP: returning.\n", GetCurrentThreadId()));
    return (S_OK);
}


//  BOOL DebuggerShell::SkipProlog():  Determines if
//  the debuggee is current inside of either a prolog or
//  interceptor (eg, class initializer), and if we don't
//  want to be, then it steps us over or out of (respectively)
//  the code region.
//  This is only called in response to the CreateThread callback
//  because that just stops us at the first managed code, regardless
//  of stop & intercept masks.
//  Returns TRUE if we are in a prolog||interceptor,
//      and we've stepped over XOR out of that region.
//      Otherwise, returns FALSE
BOOL DebuggerShell::SkipProlog(ICorDebugAppDomain *pAD,
                               ICorDebugThread *thread,
                               bool gotFirstThread)
{
    ReleaseHolder<ICorDebugFrame> pFrame;
    ReleaseHolder<ICorDebugILFrame> pilFrame;
    ULONG32 ilOff = 0;
    CorDebugMappingResult mapping;
    bool fStepOver = false;

    ReleaseHolder<ICorDebugChain> chain;
    CorDebugChainReason reason;
    bool fStepOutOf = false;

    ICorDebugStepper *pStepper = NULL;
    // If we're in the prolog or interceptor,
    // but the user doesn't want to see it, create
    // a stepper to step over the prolog
    if (! (m_rgfActiveModes & DSM_UNMAPPED_STOP_PROLOG
         ||m_rgfActiveModes & DSM_UNMAPPED_STOP_ALL) )
    {
        SPEW(fprintf(stderr, "DC::CreateThread: We're not interested in prologs\n"));

        if (FAILED(thread->GetActiveFrame( &pFrame ) ) ||
            (pFrame == NULL) ||
            FAILED(pFrame->QueryInterface(IID_ICorDebugILFrame,
                                          (void**)&pilFrame)) ||
            FAILED(pilFrame->GetIP( &ilOff, &mapping )) )
        {
            Write(L"Unable to  determine existence of prolog, if any\n");
            mapping = (CorDebugMappingResult)~MAPPING_PROLOG;
        }

        if (mapping & MAPPING_PROLOG)
        {
            fStepOver = true;
        }
    }

    // Are we in an interceptor?
    if (FAILED(thread->GetActiveChain( &chain ) ) )
    {
        Write( L"Unable to obtain active chain!\n" );
        goto LExit;
    }

    if ( FAILED( chain->GetReason( &reason)) )
    {
        Write( L"Unable to query current chain!\n" );
        goto LExit;
    }

    if (reason == CHAIN_ENTER_MANAGED)
    {
        ReleaseHolder<ICorDebugChain> chain2;
        chain->GetNext(&chain2);
        if (chain2 != NULL)
        {
            CorDebugChainReason reason2;
            chain2->GetReason(&reason2);
            if (reason2 == CHAIN_ENTER_UNMANAGED)
            {
                ReleaseHolder<ICorDebugChain> chain3;                
                chain2->GetNext(&chain3);
                if (chain3 != NULL)
                {
                    chain3->GetReason(&reason);
                }
            }
        }
    }


    // If there's any interesting reason & we've said stop on everything, then
    // don't step out
    // Otherwise, as long as at least one reason has
    // been flagged by the user as uninteresting, and we happen to be in such
    // an uninteresting frame, then we should step out.
    if (
        !( (reason &(CHAIN_CLASS_INIT|CHAIN_SECURITY|
                    CHAIN_EXCEPTION_FILTER|CHAIN_CONTEXT_POLICY
                    |CHAIN_INTERCEPTION))&&
            (m_rgfActiveModes & DSM_INTERCEPT_STOP_ALL))
        &&
        (((reason & CHAIN_CLASS_INIT) &&
            !(m_rgfActiveModes & DSM_INTERCEPT_STOP_CLASS_INIT))
        ||((reason & CHAIN_SECURITY) &&
            !(m_rgfActiveModes & DSM_INTERCEPT_STOP_SECURITY))
        ||((reason & CHAIN_EXCEPTION_FILTER) &&
            !(m_rgfActiveModes & DSM_INTERCEPT_STOP_EXCEPTION_FILTER))
        ||((reason & CHAIN_CONTEXT_POLICY) &&
            !(m_rgfActiveModes & DSM_INTERCEPT_STOP_CONTEXT_POLICY))
        ||((reason & CHAIN_INTERCEPTION) &&
            !(m_rgfActiveModes & DSM_INTERCEPT_STOP_INTERCEPTION)))
       )
    {
        fStepOutOf = true;
    }

    if ( fStepOutOf || fStepOver )
    {
        // Note that traditional step-out has a big different from JMC step-out.
        // If a traditional step-out returns from managed into UM code, and the
        // unmanaged stuff calls back into managed, we'll stop. This is effectively
        // a step-sibling.
        // A JMC step-out will only stop in a parent on the callstack.
        // So if we're trying to step-out here, we explicitly want it to be a
        // traditional step-out, regardless if we're JMC stepping or not.
        bool fJMCStepper = (m_rgfActiveModes & DSM_ALLOW_JMC_STEPPING) != 0;

        if (fStepOutOf)
        {
            fJMCStepper = false;
        }

        // Strip off the "stop-in-Unmanaged" bit for the stepper that skips through the prolog.
        CorDebugUnmappedStop stopMask = (CorDebugUnmappedStop) (ComputeStopMask() & (CorDebugUnmappedStop) ~STOP_UNMANAGED);

        if ( FAILED(thread->CreateStepper( &pStepper )) ||
             FAILED(pStepper->SetUnmappedStopMask(stopMask)) ||
             FAILED(pStepper->SetInterceptMask(ComputeInterceptMask())))
        {
            Write( L"Unable to step around special code!\n");
            g_pShell->Stop(pAD, thread);
            goto LExit;
        }

        SetJMCStepper(fJMCStepper, pStepper);

        if ( fStepOutOf )
        {
            if (FAILED(pStepper->StepOut()) )
            {
                Write( L"Unable to step out of interceptor\n" );
                g_pShell->Stop(pAD, thread);
                goto LExit;
            }
        }
        else if ( fStepOver && FAILED(pStepper->Step(false)))
        {
            Write( L"Unable to step over prolog,epilog,etc\n" );
            g_pShell->Stop(pAD, thread);
            goto LExit;
        }

        StepStart(thread, pStepper);

        // Remember that we're stepping because we want to stop on a
        // thread create. But don't do this for the first thread,
        // otherwise we'll never stop for it!
        if (gotFirstThread)
        {
            DWORD dwThreadId;

            HRESULT hr;
            hr = thread->GetID(&dwThreadId);
            _ASSERTE(!FAILED(hr));

            DebuggerManagedThread *dmt = (DebuggerManagedThread*)
                m_managedThreads.GetBase(dwThreadId);
            _ASSERTE(dmt != NULL);

            dmt->m_steppingForStartup = true;
        }

        m_showSource = true;

        ReleaseHolder<ICorDebugController> pController;
        GetControllerInterface(pAD, &pController);
        Continue(pController, thread);
    }
LExit:

    return (fStepOver || fStepOutOf)?(TRUE):(FALSE);
}

enum printType
{
    PT_CREATED,
    PT_EXITED,
    PT_IN,
    PT_NONE
};

static void _printAppDomain(ICorDebugAppDomain *pAppDomain,
                            printType pt)
{
    ULONG32 id;

    HRESULT hr = pAppDomain->GetID(&id);
    _ASSERTE(SUCCEEDED(hr));

    WCHAR buff[256];
    ULONG32 s;
    WCHAR *defaultName = L"<Unknown appdomain>";
    WCHAR *name = defaultName;

    hr = pAppDomain->GetName(256, &s, buff);

    if (SUCCEEDED(hr))
        name = buff;

    if (pt == PT_IN)
        g_pShell->Write(L"\tin appdomain #%d, %s\n", id, name);
    else if (pt == PT_CREATED)
        g_pShell->Write(L"Appdomain #%d, %s -- Created\n", id, name);
    else if (pt == PT_EXITED)
        g_pShell->Write(L"Appdomain #%d, %s -- Exited\n", id, name);
    else
        g_pShell->Write(L"Appdomain #%d, %s\n", id, name);
}

static void _printAssembly(ICorDebugAssembly *pAssembly,
                           printType pt)
{
    WCHAR buff[256];
    ULONG32 s;
    WCHAR *defaultName = L"<Unknown assembly>";
    WCHAR *name = defaultName;

    HRESULT hr = pAssembly->GetName(256, &s, buff);

    if (SUCCEEDED(hr))
        name = buff;

    if (pt == PT_IN)
        g_pShell->Write(L"\tin assembly 0x%08x, %s\n", pAssembly, name);
    else if (pt == PT_CREATED)
        g_pShell->Write(L"Assembly 0x%08x, %s -- Loaded\n", pAssembly, name);
    else if (pt == PT_EXITED)
        g_pShell->Write(L"Assembly 0x%08x, %s -- Unloaded\n", pAssembly, name);
    else
        g_pShell->Write(L"Assembly 0x%08x, %s\n", pAssembly, name);
}

static void _printModule(ICorDebugModule *pModule, printType pt)
{
    WCHAR buff[256];
    ULONG32 s;
    WCHAR *defaultName = L"<Unknown module>";
    WCHAR *name = defaultName;

    HRESULT hr = pModule->GetName(256, &s, buff);

    if (SUCCEEDED(hr))
        name = buff;

    BOOL isDynamic = FALSE;
    BOOL isInMemory = FALSE;

    hr = pModule->IsDynamic(&isDynamic);
    _ASSERTE(SUCCEEDED(hr));

    hr = pModule->IsInMemory(&isInMemory);
    _ASSERTE(SUCCEEDED(hr));

    WCHAR *mt;

    if (isDynamic)
        mt = L"Dynamic Module";
    else if (isInMemory)
        mt = L"In-memory Module";
    else
        mt = L"Module";

    if (pt == PT_IN)
        g_pShell->Write(L"\tin %s 0x%08x, %s\n", mt, pModule, name);
    else if (pt == PT_CREATED)
        g_pShell->Write(L"%s 0x%08x, %s -- Loaded\n", mt, pModule, name);
    else if (pt == PT_EXITED)
        g_pShell->Write(L"%s 0x%08x, %s -- Unloaded\n", mt, pModule, name);
    else
    {
        ICorDebugAppDomain *pAD;
        ICorDebugAssembly *pAS;

        hr = pModule->GetAssembly(&pAS);
        _ASSERTE(SUCCEEDED(hr));

        hr = pAS->GetAppDomain(&pAD);
        _ASSERTE(SUCCEEDED(hr));

        ULONG32 adId;
        hr = pAD->GetID(&adId);
        _ASSERTE(SUCCEEDED(hr));

        g_pShell->Write(L"%s 0x%08x, %s -- AD #%d", mt, pModule, name, adId);

        DebuggerModule *m = DebuggerModule::FromCorDebug(pModule);
        _ASSERTE(m != NULL);

        if (m->GetSymbolReader() == NULL)
            g_pShell->Write(L" -- Symbols not loaded\n");
        else
            g_pShell->Write(L"\n");
    }
}

/*
 * CreateAppDomain is called when an app domain is created.
 */
HRESULT DebuggerCallback::CreateAppDomain(ICorDebugProcess *pProcess,
                                          ICorDebugAppDomain *pAppDomain)
{
    g_pShell->m_enableCtrlBreak = false;
    SPEW(fprintf(stderr, "[%d] DC::CreateAppDomain.\n", GetCurrentThreadId()));

    if (g_pShell->m_rgfActiveModes & DSM_SHOW_APP_DOMAIN_ASSEMBLY_LOADS)
        _printAppDomain(pAppDomain, PT_CREATED);

    // Attach to this app domain
    pAppDomain->Attach();
    g_pShell->Continue(pProcess, NULL);

    return S_OK;
}

/*
 * ExitAppDomain is called when an app domain exits.
 */
HRESULT DebuggerCallback::ExitAppDomain(ICorDebugProcess *pProcess,
                                        ICorDebugAppDomain *pAppDomain)
{
    g_pShell->m_enableCtrlBreak = false;
    SPEW(fprintf(stderr, "[%d] DC::ExitAppDomain.\n", GetCurrentThreadId()));

    if (g_pShell->m_rgfActiveModes & DSM_SHOW_APP_DOMAIN_ASSEMBLY_LOADS)
    {
        _printAppDomain(pAppDomain, PT_EXITED);
    }

    ReleaseHolder<ICorDebugController> pController;
    GetControllerInterface(pAppDomain, &pController);

    g_pShell->Continue(pController, NULL);

    return S_OK;
}


/*
 * LoadAssembly is called when a CLR module is successfully
 * loaded.
 */
HRESULT DebuggerCallback::LoadAssembly(ICorDebugAppDomain *pAppDomain,
                                       ICorDebugAssembly *pAssembly)
{
    g_pShell->m_enableCtrlBreak = false;
    SPEW(fprintf(stderr, "[%d] DC::LoadAssembly.\n", GetCurrentThreadId()));

    if (g_pShell->m_rgfActiveModes & DSM_SHOW_APP_DOMAIN_ASSEMBLY_LOADS)
    {
        _printAssembly(pAssembly, PT_CREATED);
        _printAppDomain(pAppDomain, PT_IN);
    }

    ReleaseHolder<ICorDebugController> pController;
    GetControllerInterface(pAppDomain, &pController);

    g_pShell->Continue(pController, NULL);

    return S_OK;
}

/*
 * UnloadAssembly is called when a CLR module (DLL) is unloaded. The module
 * should not be used after this point.
 */
HRESULT DebuggerCallback::UnloadAssembly(ICorDebugAppDomain *pAppDomain,
                                         ICorDebugAssembly *pAssembly)
{
    g_pShell->m_enableCtrlBreak = false;
    SPEW(fprintf(stderr, "[%d] DC::UnloadAssembly.\n", GetCurrentThreadId()));

    if (g_pShell->m_rgfActiveModes & DSM_SHOW_APP_DOMAIN_ASSEMBLY_LOADS)
    {
        _printAssembly(pAssembly, PT_EXITED);
    }

    ReleaseHolder<ICorDebugController> pController;
    GetControllerInterface(pAppDomain, &pController);

    g_pShell->Continue(pController, NULL);

    return S_OK;
}


HRESULT DebuggerCallback::Breakpoint(ICorDebugAppDomain *pAppDomain,
                                     ICorDebugThread *pThread,
                                     ICorDebugBreakpoint *pBreakpoint)
{
    g_pShell->m_enableCtrlBreak = false;
    SPEW(fprintf(stderr, "[%d] DC::Breakpoint.\n", GetCurrentThreadId()));

    ReleaseHolder<ICorDebugProcess> pProcess;
    pAppDomain->GetProcess(&pProcess);

    DebuggerBreakpoint *bp = g_pShell->m_breakpoints;

    while (bp && !bp->Match(pBreakpoint))
        bp = bp->m_next;

    if (bp)
    {
        g_pShell->PrintThreadPrefix(pThread);
        g_pShell->Write(L"break at ");
        g_pShell->PrintBreakpoint(bp);
    }
    else
    {
        g_pShell->Write(L"Unknown breakpoint hit.  Continuing may produce unexpected results.\n");
    }

    g_pShell->Stop(pProcess, pThread);

    return S_OK;
}

const WCHAR *StepTypeToString(CorDebugStepReason reason )
{
    switch (reason)
    {
        case STEP_NORMAL:
            return L"STEP_NORMAL";
            break;
        case STEP_RETURN:
            return L"STEP_RETURN";
            break;
        case STEP_CALL:
            return L"STEP_CALL";
            break;
        case STEP_EXCEPTION_FILTER:
            return L"STEP_EXCEPTION_FILTER";
            break;
        case STEP_EXCEPTION_HANDLER:
            return L"STEP_EXCEPTION_HANDLER";
            break;
        case STEP_INTERCEPT:
            return L"STEP_INTERCEPT";
            break;
        case STEP_EXIT:
            return L"STEP_EXIT";
            break;
        default:
            _ASSERTE( !"StepTypeToString given unknown step type!" );
            return NULL;
            break;
    }
}

HRESULT DebuggerCallback::StepComplete(ICorDebugAppDomain *pAppDomain,
                                       ICorDebugThread *pThread,
                                       ICorDebugStepper *pStepper,
                                       CorDebugStepReason reason)
{
    SPEW(fprintf(stderr, "[%d] DC::StepComplete with reason %d.\n",
                 GetCurrentThreadId(), reason));

    if (g_pShell->m_rgfActiveModes & DSM_ENHANCED_DIAGNOSTICS)
    {
        g_pShell->Write( L"Step complete:");
        g_pShell->Write( StepTypeToString(reason) );
        g_pShell->Write( L"\n" );
    }


    g_pShell->StepNotify(pThread, pStepper);
    RELEASE(pStepper);

    // We only want to skip compiler stubs until we hit non-stub
    // code
    if (!g_pShell->m_needToSkipCompilerStubs ||
        g_pShell->SkipCompilerStubs(pAppDomain, pThread))
    {
        g_pShell->m_needToSkipCompilerStubs = false;

        DWORD dwThreadId;

        HRESULT hr;
        hr = pThread->GetID(&dwThreadId);
        _ASSERTE(!FAILED(hr));

        DebuggerManagedThread *dmt = (DebuggerManagedThread*)
            g_pShell->m_managedThreads.GetBase(dwThreadId);
        _ASSERTE(dmt != NULL);

        ReleaseHolder<ICorDebugController> pController;
        GetControllerInterface(pAppDomain, &pController);

        // If we're were just stepping to get over a new thread's
        // prolog, but we no longer want to catch thread starts, then
        // don't stop...
        if (!(dmt->m_steppingForStartup && !g_pShell->m_catchThread))
            g_pShell->Stop(pController, pThread);
        else
            g_pShell->Continue(pController, pThread);

    }
    // else SkipCompilerStubs Continue()'d for us

    return S_OK;
}

HRESULT DebuggerCallback::Break(ICorDebugAppDomain *pAppDomain,
                                ICorDebugThread *pThread)
{
    g_pShell->m_enableCtrlBreak = false;
    SPEW(fprintf(stderr, "[%d] DC::Break.\n", GetCurrentThreadId()));

    g_pShell->PrintThreadPrefix(pThread);
    g_pShell->Write(L"user break\n");

    ReleaseHolder<ICorDebugProcess> pProcess;
    pAppDomain->GetProcess(&pProcess);

    g_pShell->Stop(pProcess, pThread);

    return S_OK;
}

HRESULT DebuggerCallback::Exception(ICorDebugAppDomain *pAppDomain,
                                    ICorDebugThread *pThread,
                                    BOOL unhandled)
{
    g_pShell->m_enableCtrlBreak = false;
    ReleaseHolder<ICorDebugController> pController;
    GetControllerInterface(pAppDomain, &pController);

    SPEW(fprintf(stderr, "[%d] DC::Exception.\n", GetCurrentThreadId()));

    if (!unhandled)
    {
        g_pShell->PrintThreadPrefix(pThread);
        g_pShell->Write(L"First chance exception generated: ");

        ICorDebugValue *ex;
        HRESULT hr = pThread->GetCurrentException(&ex);
        bool stop = g_pShell->m_catchException;

        if (SUCCEEDED(hr))
        {
            // If we have a valid current exception object, then stop based on its type.
            stop = g_pShell->ShouldHandleSpecificException(ex);

            // If we're stopping, dump the whole thing. Otherwise, just dump the class.
            if (stop)
                g_pShell->PrintVariable(NULL, ex, 0, TRUE);
            else
                g_pShell->PrintVariable(NULL, ex, 0, FALSE);
        }
        else
        {
            g_pShell->Write(L"Unexpected error occured: ");
            g_pShell->ReportError(hr);
        }

        g_pShell->Write(L"\n");

        if (stop)
            g_pShell->Stop(pController, pThread);
        else
            g_pShell->Continue(pController, pThread);
    }
    else if (unhandled && g_pShell->m_catchUnhandled)
    {
        g_pShell->PrintThreadPrefix(pThread);
        g_pShell->Write(L"Unhandled exception generated: ");

        ICorDebugValue *ex;
        HRESULT hr = pThread->GetCurrentException(&ex);
        if (SUCCEEDED(hr))
            g_pShell->PrintVariable(NULL, ex, 0, TRUE);
        else
        {
            g_pShell->Write(L"Unexpected error occured: ");
            g_pShell->ReportError(hr);
        }

        g_pShell->Write(L"\n");

        g_pShell->Stop(pController, pThread);
    }
    else
    {
        g_pShell->Continue(pController, pThread);
    }

    return S_OK;
}


HRESULT DebuggerCallback::EvalComplete(ICorDebugAppDomain *pAppDomain,
                                       ICorDebugThread *pThread,
                                       ICorDebugEval *pEval)
{
    g_pShell->m_enableCtrlBreak = false;
    ReleaseHolder<ICorDebugProcess> pProcess;
    pAppDomain->GetProcess(&pProcess);

    g_pShell->PrintThreadPrefix(pThread);

    // Remember the most recently completed func eval for this thread.
    DebuggerManagedThread *dmt = g_pShell->GetManagedDebuggerThread(pThread);
    _ASSERTE(dmt != NULL);

    dmt->m_lastFuncEval.Assign(pEval);

    // Print any current func eval result.
    ICorDebugValue *pResult;
    HRESULT hr = pEval->GetResult(&pResult);

    if (hr == S_OK)
    {
        g_pShell->Write(L"Function evaluation complete.\n");
        g_pShell->PrintVariable(L"$result", pResult, 0, TRUE);
    }
    else if (hr == CORDBG_S_FUNC_EVAL_ABORTED)
        g_pShell->Write(L"Function evaluation aborted.\n");
    else if (hr == CORDBG_S_FUNC_EVAL_HAS_NO_RESULT)
        g_pShell->Write(L"Function evaluation complete, no result.\n");
    else
        g_pShell->ReportError(hr);

    g_pShell->m_pCurrentEval.Clear();
    g_pShell->m_rudeAbortNextTime = false;

    g_pShell->Stop(pProcess, pThread);

    return S_OK;
}

HRESULT DebuggerCallback::EvalException(ICorDebugAppDomain *pAppDomain,
                                        ICorDebugThread *pThread,
                                        ICorDebugEval *pEval)
{
    g_pShell->m_enableCtrlBreak = false;
    ReleaseHolder<ICorDebugProcess> pProcess;
    pAppDomain->GetProcess(&pProcess);

    g_pShell->PrintThreadPrefix(pThread);
    g_pShell->Write(L"Function evaluation completed with an exception.\n");

    // Remember the most recently completed func eval for this thread.
    DebuggerManagedThread *dmt = g_pShell->GetManagedDebuggerThread(pThread);
    _ASSERTE(dmt != NULL);

    dmt->m_lastFuncEval.Assign(pEval);

    // Print any current func eval result.
    ICorDebugValue *pResult;
    HRESULT hr = pEval->GetResult(&pResult);

    if (hr == S_OK)
        g_pShell->PrintVariable(L"$result", pResult, 0, TRUE);

    g_pShell->m_pCurrentEval.Clear();
    g_pShell->m_rudeAbortNextTime = false;

    g_pShell->Stop(pProcess, pThread);

    return S_OK;
}


HRESULT DebuggerCallback::CreateThread(ICorDebugAppDomain *pAppDomain,
                                       ICorDebugThread *thread)
{
    g_pShell->m_enableCtrlBreak = false;
    SPEW(fprintf(stderr, "[%d] DC::CreateThread.\n", GetCurrentThreadId()));

    DWORD threadID;
    HRESULT hr = thread->GetID(&threadID);
    if (FAILED(hr))
    {
        g_pShell->Write(L"Unexpected error in CreateThread callback:");
        g_pShell->ReportError(hr);
        goto LExit;
    }

    g_pShell->PrintThreadPrefix(thread, true);
    g_pShell->Write(L"Thread created.\n");

    SPEW(fprintf(stderr, "[%d] DC::CT: Thread id is %d\n",
                 GetCurrentThreadId(), threadID));

    hr = g_pShell->AddManagedThread( thread, threadID );
    if (FAILED(hr))
        goto LExit;

    SPEW(g_pShell->Write( L"interc? m_rgfActiveModes:0x%x\n",g_pShell->m_rgfActiveModes));

    if ((!g_pShell->m_gotFirstThread) || (g_pShell->m_catchThread))
    {
        // Try to skip compiler stubs.
        // SkipCompilerStubs returns TRUE if we're NOT in a stub
        if (g_pShell->SkipCompilerStubs(pAppDomain, thread))
        {
            // If we do want to skip the prolog (or an interceptor),
            // then we don't want to skip a stub, and we're finished, so go
            // to the clean-up code immediately
            if (g_pShell->SkipProlog(pAppDomain,
                                     thread,
                                     g_pShell->m_gotFirstThread))
                goto LExit;

            // If we don't need to skip a
            // compiler stub on entry to the first thread, or an
            // interceptor, etc, then we never
            // will, so set the flag appropriately.
            g_pShell->m_needToSkipCompilerStubs = false;


            // Recheck why we're here... we may have spent a long time
            // in SkipProlog.
            if ((!g_pShell->m_gotFirstThread) || (g_pShell->m_catchThread))
            {
                ReleaseHolder<ICorDebugController> pController;
                GetControllerInterface(pAppDomain, &pController);

                g_pShell->Stop(pController, thread);
            }
        }
    }
    else
    {
        ReleaseHolder<ICorDebugController> pController;
        GetControllerInterface(pAppDomain, &pController);

        g_pShell->Continue(pController, thread);

    }

LExit:
    g_pShell->m_gotFirstThread = true;

    return hr;
}


HRESULT DebuggerCallback::ExitThread(ICorDebugAppDomain *pAppDomain,
                                     ICorDebugThread *thread)
{
    g_pShell->m_enableCtrlBreak = false;
    SPEW(fprintf(stderr, "[%d] DC::ExitThread.\n", GetCurrentThreadId()));

    g_pShell->PrintThreadPrefix(thread, true);
    g_pShell->Write(L"Thread exited.\n");

    ReleaseHolder<ICorDebugController> pController;
    GetControllerInterface(pAppDomain, &pController);
    g_pShell->Continue(pController, thread);

    SPEW(fprintf(stderr, "[%d] DC::ET: returning.\n", GetCurrentThreadId()));

    DWORD dwThreadId =0;
    HRESULT hr = thread->GetID( &dwThreadId );
    if (FAILED(hr) )
        return hr;

    DebuggerManagedThread *dmt = g_pShell->GetManagedDebuggerThread(thread);
    if (dmt != NULL)
    {
        dmt->m_lastFuncEval.Clear();
        g_pShell->RemoveManagedThread( dwThreadId );
    }

    return S_OK;
}

//-----------------------------------------------------------------------------
// Set the module (if possible) to JMC enabled. Usually do this when we get symbols for it.
// Notify the user of failures.
//-----------------------------------------------------------------------------
void SetModuleJMC(ICorDebugModule *pModule)
{
    ReleaseHolder<ICorDebugModule2> pModule2;
    pModule->QueryInterface(IID_ICorDebugModule2, (void**) &pModule2);

    // If this version of the LS doesn't support user code, that's ok.
    // We won't bother updating user-code status.
    if (pModule2 != NULL)
    {
        // Set entire module to user code, with no exceptions
        HRESULT hr2 = pModule2->SetJMCStatus(TRUE, 0, NULL);
        if (FAILED(hr2))
        {
            // Only notify the user if they enabled JMC stepping.
            if (g_pShell->m_rgfActiveModes & DSM_ALLOW_JMC_STEPPING)
            {
                DebuggerModule * pDM = DebuggerModule::FromCorDebug(pModule);

                g_pShell->Write(L"Module %s has symbols but failed to set as Just-My-Code.\n", pDM->GetName());
                g_pShell->ReportError(hr2);
            }
        }
    } // pModule2 != NULL
}

HRESULT DebuggerCallback::LoadModule( ICorDebugAppDomain *pAppDomain,
                                      ICorDebugModule *pModule)
{
    g_pShell->m_enableCtrlBreak = false;
    SPEW(fprintf(stderr, "[%d] DC::LoadModule.\n", GetCurrentThreadId()));

    HRESULT hr;

    DebuggerModule *m = new DebuggerModule(pModule);

    if (m == NULL)
    {
        g_pShell->ReportError(E_OUTOFMEMORY);
        return (E_OUTOFMEMORY);
    }

    hr = m->Init(g_pShell->m_currentSourcesPath.GetData());

    if (FAILED(hr))
    {
        g_pShell->ReportError(hr);
        return hr;
    }

    hr = g_pShell->m_modules.AddBase(m);
    _ASSERTE(SUCCEEDED(hr));

    WCHAR moduleName[256];
    ULONG32 s;

    moduleName[0] = L'\0';
    hr = pModule->GetName(256, &s, moduleName);
    m->SetName (moduleName);

    DebuggerBreakpoint *bp = g_pShell->m_breakpoints;

    while (bp != NULL)
    {
        // If (the user specified a module for this bp, and this is the module OR
        //     the user HASN't specified a module for this bp), and
        // the module has a type/method that matches the bp's, then bind
        // the breakpoint here.
        if ((bp->m_moduleName    == NULL ||
            _wcsicmp(bp->m_moduleName, moduleName) == 0) &&
            bp->Bind(m, NULL))
        {
            g_pShell->OnBindBreakpoint(bp, m);
        }

        bp = bp->m_next;
    }

    if ((g_pShell->m_rgfActiveModes & DSM_SHOW_MODULE_LOADS) ||
        (g_pShell->m_catchModule))
    {
        _printModule(pModule, PT_CREATED);

        ICorDebugAssembly *pAssembly;
        hr = pModule->GetAssembly(&pAssembly);
        _ASSERTE(SUCCEEDED(hr));

        _printAssembly(pAssembly, PT_IN);
        _printAppDomain(pAppDomain, PT_IN);
    }


    hr = pModule->EnableClassLoadCallbacks(TRUE);

    if (FAILED(hr))
        g_pShell->Write(L"Failed to enable class load callbacks for %s\n",
                        moduleName);

    // We always want to track-jit info. But we have a mode to toggle Optimizations.
    BOOL bAllowJitOpts = (g_pShell->m_rgfActiveModes & DSM_ENABLE_JIT_OPTIMIZATIONS);
    BOOL bAllowEnC = (g_pShell->m_rgfActiveModes & DSM_ENABLE_ENC);

    ReleaseHolder<ICorDebugModule2> pModule2;
    pModule->QueryInterface(IID_ICorDebugModule2, (void**) &pModule2);
    DWORD flags = CORDEBUG_JIT_DEFAULT;

    if(pModule2 == NULL)
    {
        hr = pModule->EnableJITDebugging(TRUE, bAllowJitOpts);
        if (FAILED(hr))
        {
            g_pShell->Write(L"Failed to enable JIT Optimizations for %s\n", moduleName);
            g_pShell->ReportError(hr);
        }
    }
    else
    {
        if (bAllowEnC)
        {
            flags = CORDEBUG_JIT_ENABLE_ENC;
        }
        else if (! bAllowJitOpts)
        {
            flags = CORDEBUG_JIT_DISABLE_OPTIMIZATION;
        }
        hr = pModule2->SetJITCompilerFlags(flags);
        if (FAILED(hr))
        {
            if (! (hr == CORDBG_E_CANT_CHANGE_JIT_SETTING_FOR_ZAP_MODULE ||
                   hr == CORDBG_E_CANNOT_BE_ON_ATTACH))
            {
                g_pShell->Write(L"Failed (hr=%x) to set JIT compiler flags for %s\n", hr, moduleName);
                g_pShell->ReportError(hr);
            }
        }
    }

    // IF this has symbols, then it's JMC (user-code).
    // Default is that it's not JMC.
    if (m->GetSymbolReader() != NULL)
    {
        bool fSkip = false;

        if (!fSkip)
        {
            SetModuleJMC(pModule);
        } // not a duplicate
    } // Have symbols?

    ReleaseHolder<ICorDebugController> pController;
    GetControllerInterface(pAppDomain, &pController);

    if (g_pShell->m_catchModule)
        g_pShell->Stop(pController, NULL);
    else
        g_pShell->Continue(pController, NULL);

    SPEW(fprintf(stderr, "[%d] DC::LM: continued.\n", GetCurrentThreadId()));

    return S_OK;
}


HRESULT DebuggerCallback::UnloadModule( ICorDebugAppDomain *pAppDomain,
                      ICorDebugModule *pModule)
{
    g_pShell->m_enableCtrlBreak = false;
    SPEW(fprintf(stderr, "[%d] DC::UnloadModule.\n", GetCurrentThreadId()));

    DebuggerModule *m = DebuggerModule::FromCorDebug(pModule);
    _ASSERTE(m != NULL);

    DebuggerBreakpoint *bp = g_pShell->m_breakpoints;
    while (bp != NULL)
    {
        // Detach the breakpoint, which means it is an active bp
        // but doesn't have a CLR object behind it.
        if (bp->m_managed && bp->IsBoundToModule(m))
                bp->DetachFromModule(m);
        bp = bp->m_next;
    }

    g_pShell->m_modules.RemoveBase((ULONG_PTR)pModule);

    if ((g_pShell->m_rgfActiveModes & DSM_SHOW_MODULE_LOADS) ||
        (g_pShell->m_catchModule))
    {
        _printModule(pModule, PT_EXITED);

        ICorDebugAssembly *pAssembly;

        HRESULT hr;
        hr = pModule->GetAssembly(&pAssembly);
        _ASSERTE(SUCCEEDED(hr));

        _printAssembly(pAssembly, PT_IN);
        _printAppDomain(pAppDomain, PT_IN);
    }

    ReleaseHolder<ICorDebugController> pController;
    GetControllerInterface(pAppDomain, &pController);

    if (g_pShell->m_catchModule)
        g_pShell->Stop(pController, NULL);
    else
        g_pShell->Continue(pController, NULL);

    SPEW(fprintf(stderr, "[%d] DC::UM: continued.\n", GetCurrentThreadId()));

    return S_OK;
}


HRESULT DebuggerCallback::LoadClass( ICorDebugAppDomain *pAppDomain,
                   ICorDebugClass *c)
{
    g_pShell->m_enableCtrlBreak = false;
    SPEW(fprintf(stderr, "[%d] DC::LoadClass.\n", GetCurrentThreadId()));

    DebuggerModule *dm = NULL;
    DebuggerClass *cl = new DebuggerClass(c);

    if (cl == NULL)
    {
        g_pShell->ReportError(E_OUTOFMEMORY);
        return (E_OUTOFMEMORY);
    }

    HRESULT hr = S_OK;

    mdTypeDef td;
    hr = c->GetToken(&td);

    if (SUCCEEDED(hr))
    {
        ReleaseHolder<ICorDebugModule> imodule;
        hr = c->GetModule(&imodule);

        if (SUCCEEDED(hr))
        {
            // _ASSERTE(!"Testbreak");
            dm = DebuggerModule::FromCorDebug(imodule);
            _ASSERTE(dm != NULL);

            hr = dm->m_loadedClasses.AddBase(cl);
            _ASSERTE(SUCCEEDED(hr));

            WCHAR className[MAX_CLASSNAME_LENGTH];
            ULONG classNameSize = 0;

            hr = dm->GetMetaData()->GetTypeDefProps(td,
                                                    className, MAX_CLASSNAME_LENGTH,
                                                    &classNameSize,
                                                    NULL, NULL);

            if (SUCCEEDED(hr))
            {
                WCHAR *namespacename;
                WCHAR *name;

                namespacename = className;
                name = wcsrchr(className, L'.');
                if (name)
                    *name++ = 0;
                else
                {
                    namespacename = L"";
                    name = className;
                }

                cl->SetName (name, namespacename);

                if ((g_pShell->m_rgfActiveModes & DSM_SHOW_CLASS_LOADS) ||
                    g_pShell->m_catchClass)
                {
                    if (namespacename != NULL && *namespacename != NULL)
                        g_pShell->Write(L"Loaded class: %s.%s\n", namespacename, name);
                    else
                        g_pShell->Write(L"Loaded class: %s\n", name);
                }
            }
            else
                g_pShell->ReportError(hr);

        }
        else
            g_pShell->ReportError(hr);
    }
    else
        g_pShell->ReportError(hr);

    _ASSERTE( dm );

    // If this module is dynamic, then bind all breakpoints, as
    // they may have been bound to this class.
    ICorDebugModule *pMod = dm->GetICorDebugModule();
    _ASSERTE( pMod != NULL );

    BOOL fDynamic = false;
    hr = pMod->IsDynamic(&fDynamic);
    if (FAILED(hr))
    {
        g_pShell->Write( L"Unable to determine if loaded module is dynamic");
        g_pShell->Write( L"- not attempting\n to bind any breakpoints");
    }
    else
    {
        if (fDynamic)
        {
            DebuggerBreakpoint *bp = g_pShell->m_breakpoints;

            while (bp != NULL)
            {
                if (bp->Bind(dm, NULL))
                    g_pShell->OnBindBreakpoint(bp, dm);

                bp = bp->m_next;
            }
        }
    }

    ReleaseHolder<ICorDebugController> pController;
    GetControllerInterface(pAppDomain, &pController);

    if (g_pShell->m_catchClass)
        g_pShell->Stop(pController, NULL);
    else
        g_pShell->Continue(pController, NULL);

    SPEW(fprintf(stderr, "[%d] DC::LC: continued.\n", GetCurrentThreadId()));

    return S_OK;
}


HRESULT DebuggerCallback::UnloadClass( ICorDebugAppDomain *pAppDomain,
                     ICorDebugClass *c)
{
    g_pShell->m_enableCtrlBreak = false;
    SPEW(fprintf(stderr, "[%d] DC::UnloadClass.\n", GetCurrentThreadId()));

    HRESULT hr = S_OK;
    mdTypeDef td;
    hr = c->GetToken(&td);

    if (SUCCEEDED(hr))
    {
        ReleaseHolder<ICorDebugModule> imodule;
        hr = c->GetModule(&imodule);

        if (SUCCEEDED(hr))
        {
            DebuggerModule *dm = DebuggerModule::FromCorDebug(imodule);
            _ASSERTE(dm != NULL); // want to know if this ever fires.

            if (dm != NULL)
            {
                if ((g_pShell->m_rgfActiveModes & DSM_SHOW_CLASS_LOADS) ||
                    g_pShell->m_catchClass)
                {


                    WCHAR className[MAX_CLASSNAME_LENGTH];
                    ULONG classNameSize = 0;

                    hr = dm->GetMetaData()->GetTypeDefProps(td,
                                                            className, MAX_CLASSNAME_LENGTH,
                                                            &classNameSize,
                                                            NULL, NULL);

                    if (SUCCEEDED(hr))
                        g_pShell->Write(L"Unloaded class: %s\n", className);
                    else
                        g_pShell->ReportError(hr);
                }

                // Cannot assert that we successfully removed because there are allegedly valid cases where 
                // RemoveBase() may fail here.
                dm->m_loadedClasses.RemoveBase((ULONG_PTR)c);
            }

        }
        else
            g_pShell->ReportError(hr);
    }
    else
        g_pShell->ReportError(hr);

    ReleaseHolder<ICorDebugController> pController;
    GetControllerInterface(pAppDomain, &pController);

    if (g_pShell->m_catchClass)
        g_pShell->Stop(pController, NULL);
    else
        g_pShell->Continue(pController, NULL);

    SPEW(fprintf(stderr, "[%d] DC::LC: continued.\n", GetCurrentThreadId()));

    return S_OK;
}



HRESULT DebuggerCallback::DebuggerError(ICorDebugProcess *pProcess,
                                        HRESULT errorHR,
                                        DWORD errorCode)
{
    g_pShell->m_enableCtrlBreak = false;
    SPEW(fprintf(stderr, "[%d] DC::DebuggerError 0x%p (%d).\n",
                 GetCurrentThreadId(), errorHR, errorCode));

    g_pShell->Write(L"The debugger has encountered a fatal error.\n");
    g_pShell->ReportError(errorHR);

    g_pShell->Write(L"Cordbg and the debuggee may be very unstable. You should quit immediately.\n");
    g_pShell->Stop(pProcess, NULL);

    return (S_OK);
}


HRESULT DebuggerCallback::LogMessage(ICorDebugAppDomain *pAppDomain,
                  ICorDebugThread *pThread,
                  LONG lLevel,
                  __in_z WCHAR *pLogSwitchName,
                  __in_z WCHAR *pMessage)
{
    g_pShell->m_enableCtrlBreak = false;
    DWORD dwThreadId = 0;

    pThread->GetID(&dwThreadId);

    if(g_pShell->m_rgfActiveModes & DSM_LOGGING_MESSAGES)
    {
        g_pShell->Write (L"LOG_MESSAGE: TID=0x%x Category:Severity=%s:%d Message=\n%s\n",
            dwThreadId, pLogSwitchName, lLevel, pMessage);
    }
    else
    {
        // If we don't want to get messages, then tell the other side to stop
        // sending them....
        ReleaseHolder<ICorDebugProcess> process;
        HRESULT hr = S_OK;
        hr = pAppDomain->GetProcess(&process);
        if (!FAILED(hr))
        {
            process->EnableLogMessages(FALSE);
        }
    }

    ReleaseHolder<ICorDebugController> pController;
    GetControllerInterface(pAppDomain, &pController);

    g_pShell->Continue(pController, NULL);

    // If we wanted cordbg to stop on log messages, call:
    //g_pShell->Stop(pController, pThread);

    return S_OK;
}


HRESULT DebuggerCallback::LogSwitch(ICorDebugAppDomain *pAppDomain,
                                    ICorDebugThread *pThread,
                                    LONG lLevel,
                                    ULONG ulReason,
                                    __in WCHAR *pLogSwitchName,
                                    __in WCHAR *pParentName)
{
    g_pShell->m_enableCtrlBreak = false;
    ReleaseHolder<ICorDebugController> pController;
    GetControllerInterface(pAppDomain, &pController);
    g_pShell->Continue(pController, NULL);

    return S_OK;
}

HRESULT DebuggerCallback::ControlCTrap(ICorDebugProcess *pProcess)
{
    g_pShell->m_enableCtrlBreak = false;
    SPEW(fprintf(stderr, "[%d] DC::ControlC.\n", GetCurrentThreadId()));

    g_pShell->Write(L"ControlC Trap\n");

    g_pShell->Stop(pProcess, NULL);

    return S_OK;
}

HRESULT DebuggerCallback::NameChange(ICorDebugAppDomain *pAppDomain,
                                     ICorDebugThread *pThread)
{
    g_pShell->m_enableCtrlBreak = false;
    ReleaseHolder<ICorDebugProcess> pProcess;

    if (pAppDomain)
        pAppDomain->GetProcess(&pProcess);
    else
    {
        _ASSERTE (pThread != NULL);
        pThread->GetProcess(&pProcess);
    }

    g_pShell->Continue(pProcess, NULL);

    return S_OK;
}


HRESULT DebuggerCallback::UpdateModuleSymbols(ICorDebugAppDomain *pAppDomain,
                                              ICorDebugModule *pModule,
                                              IStream *pSymbolStream)
{
    g_pShell->m_enableCtrlBreak = false;
    ReleaseHolder<ICorDebugProcess> pProcess;
    pAppDomain->GetProcess(&pProcess);

    HRESULT hr;

    DebuggerModule *m = DebuggerModule::FromCorDebug(pModule);
    _ASSERTE(m != NULL);

    hr = m->UpdateSymbols(pSymbolStream);

    if (SUCCEEDED(hr))
    {
        g_pShell->Write(L"Updated symbols: ");
        SetModuleJMC(pModule);
    }
    else
        g_pShell->Write(L"Update of symbols failed with 0x%08x: \n", hr);

    _printModule(m->GetICorDebugModule(), PT_NONE);


    // Now that we have new symbols, we may be able to bind new breakpoints.
    DebuggerBreakpoint *bp = g_pShell->m_breakpoints;
    
    while (bp != NULL)
    {
        if (bp->Bind(m, NULL))
            g_pShell->OnBindBreakpoint(bp, m);
    
        bp = bp->m_next;
    }
    


    g_pShell->Continue(pProcess, NULL);

    return S_OK;
}

HRESULT DebuggerCallback::EditAndContinueRemap(ICorDebugAppDomain *pAppDomain,
                                               ICorDebugThread *pThread,
                                               ICorDebugFunction *pFunction,
                                               BOOL fAccurate)
{
    return E_NOTIMPL;
}

HRESULT DebuggerCallback::FunctionApplyChange(ICorDebugAppDomain *pAppDomain,
                                               ICorDebugThread *pThread,
                                                ICorDebugFunction *pFunction)
{
    HRESULT hr = S_OK;

    // If we were given a function, then tell the user about the remap.
    if (pFunction != NULL)
    {
        mdMethodDef methodDef;
        hr = pFunction->GetToken(&methodDef);

        g_pShell->Write(L"EnC Remapped method 0x%x:0x%x\n", methodDef);

    }
    else
        g_pShell->Write(L"EnC remap, but no method specified.\n");

        g_pShell->Continue(pAppDomain, NULL);

    return S_OK;
}

// SQL related fiber call back
HRESULT DebuggerCallback::CreateConnection(ICorDebugProcess *pProcess,
                                           CONNID dwConnectionId,
                                           __in_z WCHAR *pConnName)
{
    g_pShell->Write(L"[DebuggerCallback2] CreateConnection is called 0x%08x with name \"%s\".\n", dwConnectionId, pConnName);
    g_pShell->Continue(pProcess, NULL);
    return S_OK;

}

HRESULT DebuggerCallback::ChangeConnection(ICorDebugProcess *pProcess, CONNID dwConnectionId )
{
    g_pShell->Write(L"[DebuggerCallback2] ChangeConnection is called 0x%08x.\n", dwConnectionId);
    g_pShell->Continue(pProcess, NULL);
    return S_OK;

}

HRESULT DebuggerCallback::DestroyConnection(ICorDebugProcess *pProcess, CONNID dwConnectionId )
{
    g_pShell->Write(L"[DebuggerCallback2] DestroyConnection is called 0x%08x.\n", dwConnectionId);
    g_pShell->Continue(pProcess, NULL);
    return S_OK;
}

HRESULT DebuggerCallback::Exception(ICorDebugAppDomain *pAppDomain,
                                    ICorDebugThread *pThread,
                                    ICorDebugFrame *pExceptionFrame,
                                    ULONG32 nOffset,
                                    CorDebugExceptionCallbackType eventType,
                                    DWORD dwFlags )
{
    HRESULT hr = S_OK;
    WCHAR szName[80];
    ULONG32 cchName;
    DWORD dwThreadId;

    if ((g_pShell->m_rgfActiveModes & DSM_ENHANCED_DIAGNOSTICS) || g_pShell->m_catchException)
    {
        g_pShell->Write(L"Exception is called:");

    }

    if (g_pShell->m_rgfActiveModes & DSM_ENHANCED_DIAGNOSTICS)
    {
        g_pShell->Write(L"\n");

        hr = pAppDomain->GetName(80, &cchName, szName);
        if (SUCCEEDED(hr))
        {
            g_pShell->Write(L"\tAppDomain: %s\n", szName);
        }
        else
        {
            g_pShell->Write(L"\tAppDomain: UNKNOWN!\n");
        }

        hr = pThread->GetID(&dwThreadId);
        if (SUCCEEDED(hr))
        {
            g_pShell->Write(L"\tTID      : 0x%X\n", dwThreadId);
        }
        else
        {
            g_pShell->Write(L"\tTID      : UNKNOWN!\n");
        }

        CORDB_ADDRESS stackAddr = 0;
        if (pExceptionFrame != NULL)
        {
            hr = pExceptionFrame->GetStackRange(NULL, &stackAddr);
            if (FAILED(hr))
            {
                stackAddr = 0;
            }
        }
        g_pShell->Write(L"\tStackAddr: 0x%p ", CORDB_ADDRESS_TO_PTR(stackAddr));

        //
        // Find this stack address and the function associated with it.
        //
        ReleaseHolder<ICorDebugChainEnum> pChainEnum;
        ReleaseHolder<ICorDebugChain>     pChain;
        ReleaseHolder<ICorDebugFrameEnum> pFrameEnum;
        ReleaseHolder<ICorDebugFrame>     pFrame;
        ReleaseHolder<ICorDebugFunction>  pFunc;
        BOOL isManaged;

        hr = pThread->EnumerateChains(&pChainEnum);

        if (!SUCCEEDED(hr))
        {
            g_pShell->ReportError(hr);
            goto LDone;
        }

        pChain.Clear();
        hr = pChainEnum->Next(1, &pChain, NULL);

        while (SUCCEEDED(hr) && (pChain != NULL))
        {
            hr = pChain->IsManaged(&isManaged);

            if (!SUCCEEDED(hr))
            {
                g_pShell->ReportError(hr);
                goto LDone;
            }

            if (isManaged)
            {
                pFrameEnum.Clear();
                hr = pChain->EnumerateFrames(&pFrameEnum);

                if (!SUCCEEDED(hr))
                {
                    g_pShell->ReportError(hr);
                    goto LDone;
                }

                // Now look at each frame.

                pFrame.Clear();
                hr = pFrameEnum->Next(1, &pFrame, NULL);

                while (SUCCEEDED(hr) && (pFrame != NULL))
                {
                    // Do a pointer comparison here.  Note that for
                    // unhandled exceptions, the frame pointer will be null.
                    if (pExceptionFrame != NULL && pExceptionFrame == pFrame)
                    {
                        // Special case internal frames since they may not have a regular method to print.
                        {
                            ReleaseHolder<ICorDebugInternalFrame> pInternal;
                            pFrame->QueryInterface(IID_ICorDebugInternalFrame, (void**) &pInternal);
                            if (pInternal != NULL)
                            {
                                g_pShell->Write(L"( ");
                                PrintInternalFrame(pInternal);
                                g_pShell->Write(L" )\n");
                                goto LDone;
                            }
                        }

                        pFunc.Clear();
                        hr = pFrame->GetFunction(&pFunc);

                        if (!SUCCEEDED(hr))
                        {
                            g_pShell->ReportError(hr);
                            goto LDone;
                        }

                        DebuggerFunction *debuggerFunc = DebuggerFunction::FromCorDebug(pFunc);
                        _ASSERTE(debuggerFunc);

                        // Write out the class and method for the current IP
                        g_pShell->Write(L"( %s%s::%s )\n",
                                        debuggerFunc->GetNamespaceName(),
                                        debuggerFunc->GetClassName(),
                                        debuggerFunc->GetName());


                        goto LDone;
                    }

                    pFrame.Clear();
                    hr = pFrameEnum->Next(1, &pFrame, NULL);
                }

                /*
                if (fIsInChain && (reason == CHAIN_FUNC_EVAL))
                {
                    g_pShell->Write(L"( in '%s' chain )\n", GetChainReasonName(reason));
                    goto LDone;
                }
                */
            }

            pChain.Clear();
            hr = pChainEnum->Next(1, &pChain, NULL);
        }

        g_pShell->Write(L"( UNKNOWN! )\n");

LDone:

        g_pShell->Write(L"\tOffset   : 0x%X\n", nOffset);
        g_pShell->Write(L"\tEventType: ");

    }

    if ((g_pShell->m_rgfActiveModes & DSM_ENHANCED_DIAGNOSTICS) || g_pShell->m_catchException)
    {
        switch (eventType)
        {
        case DEBUG_EXCEPTION_FIRST_CHANCE:
            g_pShell->Write(L"FIRST_CHANCE\n");
            break;

        case DEBUG_EXCEPTION_USER_FIRST_CHANCE:
            g_pShell->Write(L"USER_FIRST_CHANCE\n");
            break;

        case DEBUG_EXCEPTION_CATCH_HANDLER_FOUND:
            g_pShell->Write(L"CATCH_HANDLER_FOUND\n");
            break;

        case DEBUG_EXCEPTION_UNHANDLED:
            g_pShell->Write(L"UNHANDLED\n");
            break;

        default:
            g_pShell->Write(L"UNKNOWN!!\n");
            break;

        }
    } // ENHANCED_DIAGNOSTICS

    if (g_pShell->m_rgfActiveModes & DSM_ENHANCED_DIAGNOSTICS)
    {
        if (dwFlags & DEBUG_EXCEPTION_CAN_BE_INTERCEPTED)
        {
            g_pShell->Write(L"\tIntercept? : Can be intercepted\n");
        }
        else
        {
            g_pShell->Write(L"\tIntercept? : Can not be intercepted\n");
        }
    }

    ReleaseHolder<ICorDebugController> pController;
    GetControllerInterface(pAppDomain, &pController);


    bool fUnhandled = (eventType == DEBUG_EXCEPTION_UNHANDLED);
    bool stop = false;

    if (!fUnhandled)
    {
        // We have fancy logic for 1st chance exceptions:
        ReleaseHolder<ICorDebugValue> ex;
        HRESULT hr2 = pThread->GetCurrentException(&ex);
        stop = g_pShell->m_catchException;

        if (SUCCEEDED(hr2))
        {
            // If we have a valid current exception object, then stop based on its type.
            stop = g_pShell->ShouldHandleSpecificException(ex);
        }
        else
        {
            g_pShell->Write(L"Unexpected error occured: ");
            g_pShell->ReportError(hr);
            g_pShell->Write(L"\n");
        }
    }
    else
    {
        // For unhandled exceptions:
        stop = g_pShell->m_catchUnhandled;
    }

    if (stop)
    {
        g_pShell->Stop(pController, pThread);
    }
    else
    {
        g_pShell->Continue(pController, pThread);
    }

    return S_OK;
}

HRESULT DebuggerCallback::ExceptionUnwind(ICorDebugAppDomain *pAppDomain,
                                          ICorDebugThread *pThread,
                                          CorDebugExceptionUnwindCallbackType eventType,
                                          DWORD dwFlags )
{
    HRESULT hr;
    WCHAR szName[80];
    ULONG32 cchName;
    DWORD dwThreadId;

    g_pShell->Write(L"ExceptionUnwind is called:\n");

    hr = pAppDomain->GetName(80, &cchName, szName);
    if (SUCCEEDED(hr))
    {
        g_pShell->Write(L"\tAppDomain: %s\n", szName);
    }
    else
    {
        g_pShell->Write(L"\tAppDomain: UNKNOWN!\n");
    }

    hr = pThread->GetID(&dwThreadId);
    if (SUCCEEDED(hr))
    {
        g_pShell->Write(L"\tTID      : 0x%X\n", dwThreadId);
    }
    else
    {
        g_pShell->Write(L"\tTID      : UNKNOWN!\n");
    }

    g_pShell->Write(L"\tEventType: ");

    switch (eventType)
    {
    case DEBUG_EXCEPTION_UNWIND_BEGIN:
        g_pShell->Write(L"UNWIND_BEGIN\n");
        break;

    case DEBUG_EXCEPTION_INTERCEPTED:
        g_pShell->Write(L"INTERCEPTED\n");
        break;

    default:
        g_pShell->Write(L"UNKNOWN!!\n");
        break;

    }

    ReleaseHolder<ICorDebugController> pController;
    GetControllerInterface(pAppDomain, &pController);

    if (g_pShell->m_catchException)
    {
        g_pShell->Stop(pController, pThread);
    }
    else
    {
        g_pShell->Continue(pController, pThread);
    }

    return S_OK;
}


HRESULT DebuggerCallback::MDANotification(
    ICorDebugController * pController,
    ICorDebugThread *pThread,
    ICorDebugMDA * pMDA
)
{
    g_pShell->Write(L"MDA notification:");

    // Don't take a ref on pMDA so that it can be immediately reclaimed.
    HRESULT hr = S_OK;
    DWORD tid = 0;
    if (pThread != NULL)
    {
        hr = pThread->GetID(&tid);
        g_pShell->Write(L"  [Thread:0x%x]:", tid);
    }

    ULONG32 lenChars;

    // GetName
    DebuggerString name;
    hr = pMDA->GetName(0, &lenChars, NULL);
    if (SUCCEEDED(hr) && !name.Allocate(lenChars))
    {
        hr = E_OUTOFMEMORY;
    }
    if (SUCCEEDED(hr))
    {
        pMDA->GetName(lenChars, NULL, name.GetData());
        _ASSERTE(SUCCEEDED(hr));
        g_pShell->Write(L"  Name:%s,", (const WCHAR*) name);
    }
    else
    {
        g_pShell->Write(L"  Name:<unknown:hr=0x%08x>,", hr);
    }
    g_pShell->Write(L"\n");

    // Flags
    CorDebugMDAFlags flags = (CorDebugMDAFlags) 0;
    hr = pMDA->GetFlags(&flags);
    if (SUCCEEDED(hr))
    {
        g_pShell->Write(L" Flags:0x%08x", flags);
        if (flags & MDA_FLAG_SLIP) g_pShell->Write(L" MDA_FLAG_SLIP");
    }
    else
    {
        g_pShell->Write(L" Flags:<failed:hr=0x%08x", hr);
    }
    g_pShell->Write(L"\n");

    // Print more details.
    if (g_pShell->m_rgfActiveModes & DSM_ENHANCED_DIAGNOSTICS)
    {
        // Print description:
        g_pShell->Write(L"Description:");
        DebuggerString desc;
        hr = pMDA->GetDescription(0, &lenChars, NULL);
        if (SUCCEEDED(hr) && !desc.Allocate(lenChars))
        {
            hr = E_OUTOFMEMORY;
        }
        if (SUCCEEDED(hr))
        {
            pMDA->GetDescription(lenChars, NULL, desc.GetData());
            _ASSERTE(SUCCEEDED(hr));
            g_pShell->Write(desc);
        }
        else
        {
            g_pShell->Write(L"<unknown:hr=0x%08x>", hr);
        }
        g_pShell->Write(L"\n");

        // Print XML:
        g_pShell->Write(L"XML:\n");
        DebuggerString xml;
        hr = pMDA->GetXML(0, &lenChars, NULL);
        if (SUCCEEDED(hr) && !xml.Allocate(lenChars))
        {
            hr = E_OUTOFMEMORY;
        }
        if (SUCCEEDED(hr))
        {
            pMDA->GetXML(lenChars, NULL, xml.GetData());
            _ASSERTE(SUCCEEDED(hr));
            g_pShell->Write(xml);
        }
        else
        {
            g_pShell->Write(L"<unknown:hr=0x%08x>", hr);
        }
        g_pShell->Write(L"\n");
    }

    g_pShell->Write(L"Breaking for MDA.\n");
    g_pShell->Stop(pController, pThread);
    
    return S_OK;
}

HRESULT DebuggerCallback::BreakpointSetError(ICorDebugAppDomain *pAppDomain,
                                             ICorDebugThread *pThread,
                                             ICorDebugBreakpoint *pBreakpoint,
                                             DWORD dwError)
{
    g_pShell->m_enableCtrlBreak = false;
    SPEW(fprintf(stderr, "[%d] DC::BreakpointSetError.\n", GetCurrentThreadId()));

    ReleaseHolder<ICorDebugProcess> pProcess;
    pAppDomain->GetProcess(&pProcess);

    DebuggerBreakpoint *bp = g_pShell->m_breakpoints;

    while (bp && !bp->Match(pBreakpoint))
        bp = bp->m_next;

    if (bp)
    {
        g_pShell->Write(L"Error binding this breakpoint (it will not be hit): ");
        g_pShell->PrintBreakpoint(bp);
    }
    else
    {
        g_pShell->Write(L"Unknown breakpoint had a binding error.\n");
    }

    g_pShell->Continue(pAppDomain, NULL);

    return S_OK;
}



/* ------------------------------------------------------------------------- *
 * DebuggerShell methods
 * ------------------------------------------------------------------------- */

DebuggerShell::DebuggerShell(FILE *i, FILE *o) :
    m_in(i),
    m_out(o),
    m_pHandleNameList(NULL),
    m_cor(NULL),
    m_targetProcess(NULL),
    m_targetProcessHandledFirstException(false),
    m_currentProcess(NULL),
    m_currentThread(NULL),
    m_currentThread2(NULL),
    m_currentChain(NULL),
    m_currentUnmanagedThread(NULL),
    m_lastThread(0),
    m_lastStepper(NULL),
    m_showSource(true),
    m_silentTracing(false),
    m_stopEvent(NULL),
    m_hProcessCreated(NULL),
    m_stop(false),
    m_quit(false),
    m_breakpoints(NULL),
    m_lastBreakpointID(0),
    m_modules(11),
    m_unmanagedThreads(11),
    m_managedThreads(17),
    m_lastRunArgs(NULL),
    m_catchException(false),
    m_catchUnhandled(true),
    m_catchClass(false),
    m_catchModule(false),
    m_catchThread(false),
    m_needToSkipCompilerStubs(true),
    m_rgfActiveModes(DSM_DEFAULT_MODES),
    m_invalidCache(false),
    m_unmanagedDebuggingEnabled(false),
    m_cEditAndContinues(0),
    m_rudeAbortNextTime(false),
    m_enableCtrlBreak(false),
    m_exceptionHandlingList(NULL)
{

}

CorDebugUnmappedStop DebuggerShell::ComputeStopMask(void )
{
    unsigned int us = (unsigned int)STOP_NONE;

    if (m_rgfActiveModes & DSM_UNMAPPED_STOP_PROLOG )
        us |= (unsigned int)STOP_PROLOG;

    if (m_rgfActiveModes & DSM_UNMAPPED_STOP_EPILOG )
        us |= (unsigned int)STOP_EPILOG;



    if (m_rgfActiveModes & DSM_UNMAPPED_STOP_ALL )
    {
        us |= (unsigned int)STOP_ALL;
            us &= ~STOP_UNMANAGED;
    }

    return (CorDebugUnmappedStop)us;
}

CorDebugIntercept DebuggerShell::ComputeInterceptMask( void )
{
    unsigned int is = (unsigned int)INTERCEPT_NONE;

    if (m_rgfActiveModes & DSM_INTERCEPT_STOP_CLASS_INIT )
        is |= (unsigned int)INTERCEPT_CLASS_INIT;

    if (m_rgfActiveModes & DSM_INTERCEPT_STOP_EXCEPTION_FILTER )
        is |= (unsigned int)INTERCEPT_EXCEPTION_FILTER;

    if (m_rgfActiveModes & DSM_INTERCEPT_STOP_SECURITY)
        is |= (unsigned int)INTERCEPT_SECURITY;

    if (m_rgfActiveModes & DSM_INTERCEPT_STOP_CONTEXT_POLICY)
        is |= (unsigned int)INTERCEPT_CONTEXT_POLICY;

    if (m_rgfActiveModes & DSM_INTERCEPT_STOP_INTERCEPTION )
        is |= (unsigned int)INTERCEPT_INTERCEPTION;

    if (m_rgfActiveModes & DSM_INTERCEPT_STOP_ALL)
        is |= (unsigned int)INTERCEPT_ALL;

    return (CorDebugIntercept)is;
}

// Set this to the default based off the mode.
// Return true if we succeeded (trivial if JMC is disabled!).
bool DebuggerShell::SetJMCStepperToDefault(ICorDebugStepper * pStepper)
{
    bool fJMCStepper = (m_rgfActiveModes & DSM_ALLOW_JMC_STEPPING) != 0;
    bool fSet = SetJMCStepper(fJMCStepper, pStepper);

    return !fJMCStepper || fSet;
}

// Return true if we update the JMC status for the stepper, else false.
// This fails if the LS doesn't support JMC. Many times we don't
// care about that though, so we'll often ignore the return code.
bool DebuggerShell::SetJMCStepper(bool fJMC, ICorDebugStepper * pStepper)
{
    _ASSERTE(pStepper != NULL);

    // Set JMC status. If we can't get ICDStepper2, then we don't
    // support JMC, and that's ok.
    ReleaseHolder<ICorDebugStepper2> pStepper2;
    pStepper->QueryInterface(IID_ICorDebugStepper2, (void**) &pStepper2);
    if (pStepper2 != NULL)
    {
        pStepper2->SetJMC(fJMC);
        return true;
    }

    return false;
}

//
// InvokeDebuggerOnBreak is a console control handler which
// breaks into the debugger when a break signal is received.
//

static BOOL WINAPI InvokeDebuggerOnBreak(DWORD dwCtrlType)
{
    if ((dwCtrlType == CTRL_BREAK_EVENT)
         || (dwCtrlType == CTRL_C_EVENT)
       )
    {
        if (dwCtrlType == CTRL_BREAK_EVENT)
            g_pShell->Write(L"<Ctrl-Break>\n");
        else
            g_pShell->Write(L"<Ctrl-C>\n");

        g_pShell->m_stopLooping = true;

        if ((g_pShell->m_targetProcess != NULL) && (g_pShell->m_enableCtrlBreak == true))
        {

            if (g_pShell->m_pCurrentEval == NULL)
            {
                g_pShell->m_enableCtrlBreak = false;
                g_pShell->Write(L"\n\nBreaking current process.\n");
                g_pShell->Interrupt();
            }
            else if (!g_pShell->m_rudeAbortNextTime)
            {
                g_pShell->Write(L"\n\nAborting func eval...\n");
                g_pShell->m_rudeAbortNextTime = true;
                HRESULT hr = g_pShell->m_pCurrentEval->Abort();

                if (FAILED(hr))
                {
                    g_pShell->Write(L"Abort failed\n");
                    g_pShell->ReportError(hr);
                }
            }
            else
            {
                g_pShell->m_enableCtrlBreak = false;

                g_pShell->Write(L"\n\nRude aborting func eval...\n");

                ReleaseHolder<ICorDebugEval2> pEval;

                HRESULT hr;

                hr = g_pShell->m_pCurrentEval->QueryInterface(IID_ICorDebugEval2, (void **)(&pEval));

                if (!FAILED(hr))
                {
                    hr = pEval->RudeAbort();
                }

                if (FAILED(hr))
                {
                    g_pShell->Write(L"Rude abort failed\n");
                    g_pShell->ReportError(hr);
                }
            }
        }
        else if (g_pShell->m_targetProcess == NULL)
            g_pShell->Write(L"No process to break.\n");
        else
            g_pShell->Write(L"Async break not allowed at this time.\n");

        return (TRUE);
    }

    return (FALSE);
}

void DebuggerShell::ClearHandles()
{
    // Clear the handle list
    NamedHandle *pCurHandle = m_pHandleNameList;
    NamedHandle *pNextHandle;
    while (pCurHandle)
    {
        pNextHandle = pCurHandle->m_pNext;
        delete pCurHandle;
        pCurHandle = pNextHandle;
    }
    m_pHandleNameList = NULL;
}

DebuggerShell::~DebuggerShell()
{
    ClearHandles();
    SetTargetProcess(NULL);
    SetCurrentThread(NULL, NULL);
    SetCurrentChain(NULL);

    SetConsoleCtrlHandler(InvokeDebuggerOnBreak, FALSE);

    //clear out any managed threads that were left lieing around
    HASHFIND find;
    DebuggerManagedThread *dmt =NULL;
    for (dmt = (DebuggerManagedThread*)m_managedThreads.FindFirst(&find);
         dmt != NULL;
         dmt = (DebuggerManagedThread*)m_managedThreads.FindNext(&find))
    {
        dmt->m_lastFuncEval.Clear();
        RemoveManagedThread(dmt->GetToken() );
    }

    m_fLockCor = false; // unlock for termination.
    RecycleCorForVersion(NULL);

    if (m_hMscoree != NULL)
    {
        FreeLibrary(m_hMscoree);
        m_hMscoree = NULL;
    }


    if (m_stopEvent)
        CloseHandle(m_stopEvent);

    if (m_hProcessCreated)
        CloseHandle(m_hProcessCreated);


    while (m_breakpoints)
        delete m_breakpoints;

    if (g_pShell == this)
        g_pShell = NULL;

    delete [] m_lastRunArgs;


    ClearExceptionHandlingList();

}

void DebuggerShell::ClearExceptionHandlingList()
{
    // Cleanup any list of specific exception types to handle
    while (m_exceptionHandlingList != NULL)
    {
        ExceptionHandlingInfo *h = m_exceptionHandlingList;

        m_exceptionHandlingList = h->m_next;
        delete h;
    }

    m_exceptionHandlingList = NULL;
}


// This gets a root ICorDebug object which can be used w/ the given version.
// Returns S_OK on success and sets m_cor.
// NULL string will cleanup our old COR object.
HRESULT DebuggerShell::RecycleCorForVersion(__in_z __in_opt WCHAR *strVersion)
{
    // If we've locked our COR objects, then don't recycle them. This can be
    // used when we're doing embedded debugging, or on a v1.0,v1.1 system.
    if (m_fLockCor)
    {
        _ASSERTE(m_cor != NULL);
        return S_OK;
    }

    HRESULT hr = S_OK;

    // Shutdown old COR if we have it.
    if (m_cor)
    {
        m_cor->Terminate();
        m_cor->Release();
        m_cor = NULL;
    }

    if (strVersion == NULL)
    {
        return S_OK;
    }
    _ASSERTE(strVersion[0] != 0);

    ReleaseHolder<IUnknown> pCor;

    // Us ethe ShimAPI to create a Cordb for a given version.
    hr = m_fpCreateCordb(CorDebugVersion_2_0, strVersion, &pCor);

    if (FAILED(hr))
    {
        Error(L"Failed to create ICorDebug for version '%s'\n", strVersion);
        goto Exit;
    }


    hr = pCor->QueryInterface(IID_ICorDebug, (void**) &m_cor);
    if (FAILED(hr))
    {
        // We never expect this to happen, but may as well let the user know.
        Error(L"Debugging support for version '%s' has a debugging component,\n"
            L"but it does not support ICorDebug and is useless.\n", strVersion);

        goto Exit;
    }

    hr = InitCor();

    // m_cor will keep reference. So we let smart ptr dotr release pCor
Exit:

    return hr;
}

HRESULT DebuggerShell::InitCor()
{
    HRESULT hr = S_OK;
    ICorDebugManagedCallback *imc = NULL;

    _ASSERTE(m_cor != NULL);

    // Now init our new object.
    hr = m_cor->Initialize();

    if (FAILED(hr))
    {
        Write(L"Unable to initialize an ICorDebug object.\n");
        goto Exit;
    }

    imc = GetDebuggerCallback();

    if (imc != NULL)
    {
        imc->AddRef();

        hr = m_cor->SetManagedHandler(imc);
        RELEASE(imc);

        if (FAILED(hr))
            goto Exit;
    }
    else
    {
        hr = E_OUTOFMEMORY;
        goto Exit;
    }



    // Verify that debugging is possible on this system.
    hr = m_cor->CanLaunchOrAttach(0,
                                  FALSE
                                 );

    if (FAILED(hr))
    {
        if (hr == CORDBG_E_KERNEL_DEBUGGER_ENABLED)
        {
            Write(L"\nWARNING: there is a kernel debugger enabled on your system. Managed-only\n");
            Write(L"         debugging will cause your system to trap to the kernel debugger!\n\n");
            hr = S_OK;
        }
        else if (hr == CORDBG_E_KERNEL_DEBUGGER_PRESENT)
        {
            Write(L"\nWARNING: there is a kernel debugger present on your system. Managed-only\n");
            Write(L"         debugging will cause your system to trap to the kernel debugger!\n\n");
            hr = S_OK;
        }
        else
            goto Exit;
    }

    _ASSERTE(SUCCEEDED(hr));
Exit:
    if (FAILED(hr))
    {
        if (m_cor != NULL)
        {
            RELEASE(m_cor);
            m_cor = NULL;
        }
    }


    return hr;
}

// Get the current version.
// We cache it so it's just a strcpy.
void DebuggerShell::GetDefaultVersion(__inout_ecount(cSize) WCHAR * szVersionOut, DWORD cSize)
{
    // As this is a purely internal API, we know that our callers all passed
    // us in a large enough buffer.
    _ASSERTE(cSize >= wcslen(m_strDefaultVersion));
    wcsncpy(szVersionOut, m_strDefaultVersion, cSize);
}

// Set a new default version string.
bool DebuggerShell::SetDefaultVersion(const WCHAR * szVersion)
{
    size_t len = wcslen(szVersion);
    if (len >= MAX_PATH)
    {
        return false;
    }
    wcscpy(m_strDefaultVersion, szVersion);
    return true;
}

void DebuggerShell::GetForceVersion(__inout_ecount(cSize) WCHAR * szVersionOut, DWORD cSize)
{
    _ASSERTE(cSize >= wcslen(m_strForceVersion));
    wcsncpy(szVersionOut, m_strForceVersion, cSize);

}
bool DebuggerShell::SetForceVersion(const WCHAR * szVersion)
{
    size_t len = wcslen(szVersion);
    if (len >= MAX_PATH)
    {
        return false;
    }
    wcscpy(m_strForceVersion, szVersion);
    return true;
}


// Given an exe, get the requested version.
// This is useful on 'run' to predict what version of the runtime we expect.
HRESULT DebuggerShell::GetVersionForExe(__in_z WCHAR * szExeName,
                                        __inout_ecount(cSize) WCHAR * szVersionOut,
                                        DWORD cSize)
{
    return E_NOTIMPL;
}

HRESULT DebuggerShell::GetVersionFromPid(DWORD pid,
                                         __inout_ecount(cSize) WCHAR *szVersionOut,
                                         DWORD cSize)
{
    return E_NOTIMPL;
}


// Retrieve an instance of the Publishing API.
void DebuggerShell::GetPublish(ICorPublish ** ppPublish)
{
    // Publish must be recreated every time b/c you can only enumerate once.
    // (Due to a bug: Future enumerations are appended to previous ones.)
    _ASSERTE(ppPublish != NULL);

        CoCreateInstance (CLSID_CorpubPublish,
                                        NULL,
                                        CLSCTX_INPROC_SERVER,
                                        IID_ICorPublish,
                                        (LPVOID *)ppPublish);
}


HRESULT DebuggerShell::Init()
{
    HRESULT hr;

    m_fLockCor = false;
    m_cor = NULL;

    // Get our connection to the shim.


    m_hMscoree = LoadLibraryA(MSCOREE_SHIM_A);


    if (m_hMscoree == NULL)
    {
        Error(L"Couldn't load '" MSCOREE_SHIM_W L"'. Error=%d\n", GetLastError());
        return E_FAIL;
    }
    m_fpCorGetVersion = (FPCORGetVersion) GetProcAddress(m_hMscoree, "GetCORVersion");
    if (m_fpCorGetVersion == NULL)
    {
        Error(L"Couldn't get 'GetCORVersion' from shim\n");
        return E_FAIL;
    }


    m_fpCreateCordb = (FPCreateCordb) GetProcAddress(m_hMscoree, "CreateDebuggingInterfaceFromVersion");
    if (m_fpCreateCordb == NULL)
    {
        Error(L"Couldn't get 'CreateDebuggingInterfaceFromVersion' from shim\n");
        return E_FAIL;
    }

    // Use new so that the string is deletable.
    m_currentSourcesPath.CopyFrom(L".");

    DebuggerString newPath;

    if (ReadSourcesPath(&newPath))
    {
        m_currentSourcesPath.CopyFrom(newPath);
    }

    ReadDebuggerModes();


    {
        m_strForceVersion[0] = 0;

        // Now that we've loaded a DebugFactory, use that version as the default version.
        DWORD dwLength;

        hr = m_fpCorGetVersion(m_strDefaultVersion, NumItems(m_strDefaultVersion), &dwLength);
        if (FAILED(hr))
        {
            Write(L"Failed to query for current version. hr=0x%08x\n", hr);
            return hr;
        }

        if (m_rgfActiveModes & DSM_ENHANCED_DIAGNOSTICS)
        {
            Write(L"Current Default Version:%s\n", m_strDefaultVersion);
        }
    }


    // Note that we don't have a root CorDebug object until we get a version.

    AddCommands();
    m_pPrompt = L"(cordbg)";


    m_stopEvent = CreateEventA(NULL, TRUE, FALSE, NULL);
    _ASSERTE(m_stopEvent != NULL);

    m_hProcessCreated = CreateEventA(NULL, FALSE, FALSE, NULL);
    _ASSERTE(m_hProcessCreated != NULL);

    g_pShell = this;

    SetConsoleCtrlHandler(InvokeDebuggerOnBreak, TRUE);

    // Set the error mode so we never show a dialog box if removable media is not in a drive. This prevents annoying
    // error messages while searching for PDB's for PE's that were compiled to a specific drive, and that drive happens
    // to be removable media on the current system.
    SetErrorMode(SEM_NOOPENFILEERRORBOX | SEM_FAILCRITICALERRORS);

    return (S_OK);
}


static const WCHAR *MappingType( CorDebugMappingResult mr )
{
    switch( mr )
    {
        case MAPPING_PROLOG:
            return L"prolog";
            break;
        case MAPPING_EPILOG:
            return L"epilog";
            break;
        case MAPPING_NO_INFO:
            return L"no mapping info region";
            break;
        case MAPPING_UNMAPPED_ADDRESS:
            return L"unmapped region";
            break;
        case MAPPING_EXACT:
            return L"exactly mapped";
            break;
        case MAPPING_APPROXIMATE:
            return L"approximately mapped";
            break;
        default:
            return L"Unknown mapping";
            break;
    }
}

void DebuggerShell::Run(bool fNoInitialContinue)
{
    m_stop = false;

    SetCurrentThread(m_targetProcess, NULL);
    m_enableCtrlBreak = true;

    while (TRUE)
    {
        ResetEvent(m_stopEvent);

        SPEW(fprintf(stderr, "[%d] DS::R: Continuing process...\n",
                     GetCurrentThreadId()));

        // Don't continue the first time of fNoInitialContinue is set
        // to true.
        if ((m_targetProcess != NULL) && !fNoInitialContinue)
        {
            ICorDebugProcess *p = m_targetProcess;

            p->AddRef();
            p->Continue(FALSE);
            RELEASE(p);
        }

        fNoInitialContinue = false;

        SPEW(fprintf(stderr, "[%d] DS::R: Waiting for a callback...\n",
                     GetCurrentThreadId()));

        WaitForSingleObject(m_stopEvent, INFINITE);

        SPEW(fprintf(stderr, "[%d] DS::R: Done waiting.\n", GetCurrentThreadId()));


        BOOL queued;
        if (m_targetProcess == NULL
            || FAILED(m_targetProcess->HasQueuedCallbacks(NULL, &queued))
            || (!queued && m_stop))
        {
            SPEW(fprintf(stderr, "[%d] DS::R: I'm stopping now (%squeued and %sstop)...\n",
                         GetCurrentThreadId(),
                         queued ? "" : "not ", m_stop ? "" : "not "));
            break;
        }

        SPEW(fprintf(stderr, "[%d] DS::R: I'm gonna do it again (%squeued and %sstop)...\n",
                     GetCurrentThreadId(),
                     queued ? "" : "not ", m_stop ? "" : "not "));
    }

    if ((m_currentThread != NULL) || (m_currentUnmanagedThread != NULL))
    {
        SetDefaultFrame();

        if (!m_silentTracing)
        {
            ULONG32 IP;
            CorDebugMappingResult map;

            if ( m_currentFrame != NULL &&
                 SUCCEEDED( m_currentFrame->GetIP( &IP, &map ) ) )
            {

                if (map & ~(MAPPING_APPROXIMATE | MAPPING_EXACT | MAPPING_PROLOG) )
                {
                    if ((map != MAPPING_EPILOG) || (m_rgfActiveModes & DSM_UNMAPPED_STOP_EPILOG))
                    {
                        g_pShell->Write( L"Source not available when in the %s"
                                         L" of a function(offset 0x%x)\n",
                                         MappingType(map),IP);
                        g_pShell->m_showSource = false;
                    }
                }

            }

            if (m_currentThread != NULL)
            {
                PrintThreadPrefix(m_currentThread);
                Write( L"\n" );
            }

            if (! (m_showSource
                   ? PrintCurrentSourceLine(0)
                   : PrintCurrentInstruction(0, 0, 0)))
                PrintThreadState(m_currentThread);
        }
    }

    //this only has an effect when m_targetProcess == NULL
    // (ie, the target process has exited)
    m_lastStepper = NULL;
}

void DebuggerShell::Kill()
{
    if (m_targetProcess != NULL)
    {
        // Have extra holder because once we call Terminate, ExitProcess callback may come and
        // null out m_targetProcess and release it.
        ReleaseHolder<ICorDebugProcess> proc;
        proc.Assign(m_targetProcess);
        
        HANDLE h;
        HRESULT hr = proc->GetHandle(&h);

        Write(L"Terminating current process...\n");

        {
            HASHFIND find;
            DebuggerManagedThread *dmt =NULL;
            for (dmt = (DebuggerManagedThread*)m_managedThreads.FindFirst(&find);
                 dmt != NULL;
                 dmt = (DebuggerManagedThread*)m_managedThreads.FindNext(&find))
            {
                dmt->m_lastFuncEval.Clear();
                RemoveManagedThread(dmt->GetToken() );
            }
        }

        m_stop = false;
        ResetEvent(m_stopEvent);

        // If this succeeds, our ExitProcess() callback may be invoked immediately.
        // (even before we return from Terminate() here)
        // That will in turn call SetTargetProcess(NULL) thus invalidating our
        // CordbProcess object.
        hr = proc->Terminate(E_FAIL);

        if (g_pShell->m_rgfActiveModes & DSM_ENHANCED_DIAGNOSTICS)
        {
            if (FAILED(hr))
            {
                g_pShell->Write(L"Failed to managed-terminate the process:");
                g_pShell->ReportError(hr);
                g_pShell->Write(L"\n");
            }
        }


        SetCurrentThread(NULL, NULL);

        // Don't call Run. There is no need to Continue from calling
        // ICorDebugProcess::Terminate, and ExitProcess will be called
        // automatically. Instead, we simply wait for ExitProcess to
        // get called before going back to the command prompt.
        WaitForSingleObject(m_stopEvent, INFINITE);

        // At this point, m_targetProcess should be finished.
    }

    ClearDebuggeeState();
}

// List all outstanding steppers.
// This is only useful because Cordbg doesn't automatically kill steppers.
// It also helps in debugging weird stepping-bugs.
// Currently we just print the steppers address (we don't know anything else)
// so this is only used in _INTERNAL_DEBUG_SUPPORT_ cases.
void DebuggerShell::ListAllSteppers()
{
    // Enumerate for all steppers on all threads.
    HRESULT hr = S_OK;

    int cTotal = 0;

    // Steppers can move across appdomains, but they're definitely bound
    // to one thread. So loop through all threads to find the steppers.
    HASHFIND findThread;
    DebuggerManagedThread *dmt =NULL;
    for (dmt = (DebuggerManagedThread*)m_managedThreads.FindFirst(&findThread);
         dmt != NULL;
         dmt = (DebuggerManagedThread*)m_managedThreads.FindNext(&findThread))
    {
        // For each thread, remove all the steppers on that thread.
        StepperHashTable * pTable = dmt->m_pendingSteppers;


        int cTotalOnThread = 0;
        HASHFIND findStepper;
        ICorDebugStepper * pStepper = NULL;
        for (pStepper = pTable->FindFirst(&findStepper);
             pStepper != NULL;
             pStepper = pTable->FindNext(&findStepper))
        {
            cTotalOnThread++;
            cTotal++;
            if (cTotal == 1)
            {
                this->Write(L"List of all active steppers on all threads:\n");
            }
            if (cTotalOnThread == 1)
            {
                DWORD tid;
                hr = dmt->m_thread->GetID(&tid);
                if (FAILED(hr))
                    break;
                this->Write(L"Thread %d:", tid);
            }

            this->Write(L" %p", pStepper);

            BOOL fActive;
            hr = pStepper->IsActive(&fActive);
            if (SUCCEEDED(hr))
            {
                this->Write(L"(%s)", fActive ? L"active" : L"inactive");
            }
        }

        if (cTotalOnThread > 0)
        {
            this->Write(L"\n");
        }
    }

    if (cTotal == 0)
    {
        this->Write(L"There are no active steppers\n");
    }
}


// Kill all steppers in the process. Useful at a stopping event.
// Should only be called when the app is stopped.
void DebuggerShell::KillAllSteppers()
{
    // Enumerate for all steppers on all threads.
    HRESULT hr = S_OK;

    int cTotal = 0;

    // Steppers can move across appdomains, but they're definitely bound
    // to one thread. So loop through all threads to find the steppers.
    HASHFIND findThread;
    DebuggerManagedThread *dmt =NULL;
    for (dmt = (DebuggerManagedThread*)m_managedThreads.FindFirst(&findThread);
         dmt != NULL;
         dmt = (DebuggerManagedThread*)m_managedThreads.FindNext(&findThread))
    {
        // For each thread, remove all the steppers on that thread.
        StepperHashTable * pTable = dmt->m_pendingSteppers;

        HASHFIND findStepper;
        ICorDebugStepper * pStepper = NULL;
        for (pStepper = pTable->FindFirst(&findStepper);
             pStepper != NULL;
             pStepper = pTable->FindNext(&findStepper))
        {
            // This will cancel the outstanding stepper
            // and free the LS's data for the stepper.
            hr = pStepper->Deactivate();
            if (FAILED(hr))
            {
                this->Write(L"Failed to deactivate stepper:");
                this->ReportError(hr);
            } else {
                cTotal++;
            }
        }

        // Clear out the table.
        pTable->ReleaseAll();
    }

    if (cTotal > 0)
    {
        this->Write(L"Cancelled all outstanding steppers (%d total).\n", cTotal);
    }
}

// AsyncStop gets called by the main thread (the one that handles the
// command prompt) to stop an <appdomain> asynchronously.
HRESULT DebuggerShell::AsyncStop(ICorDebugController *controller,
                                 DWORD dwTimeout)
{
    return controller->Stop(dwTimeout);
}

// Stop gets used by callbacks to tell the main loop (the one that
// called Run()) that we want to stop running now. c.f. AsyncStop
void DebuggerShell::Stop(ICorDebugController *controller,
                         ICorDebugThread *thread,
                         DebuggerUnmanagedThread *unmanagedThread)
{
    //
    // Don't stop for any process other than the target.
    //
    ReleaseHolder<ICorDebugProcess> process;
    HRESULT hr = S_OK;

    if (controller != NULL)
        hr = controller->QueryInterface(IID_ICorDebugProcess,
                                        (void **)&process);

    if (hr==E_NOINTERFACE )
    {
        ReleaseHolder<ICorDebugAppDomain> appDomain;

        _ASSERTE(process == NULL);

        hr = controller->QueryInterface(IID_ICorDebugAppDomain,
                                        (void **)&appDomain);
        _ASSERTE(!FAILED(hr));

        hr = appDomain->GetProcess(&process);

        _ASSERTE(!FAILED(hr));
        _ASSERTE(process != NULL);
    }
    if (FAILED(hr))
        g_pShell->ReportError(hr);

    if (!FAILED(hr) &&
        process != m_targetProcess &&
        process != NULL)
    {
        hr = controller->Continue(FALSE);
        if (FAILED(hr))
            g_pShell->ReportError(hr);
    }
    else
    {
        m_stop = true;
        SetCurrentThread(process, thread, unmanagedThread);

        // VS will kill all steppers at each stopping event. Cordbg doesn't.
        // If we wanted Cordbg to act like VS, we could call KillAllSteppers() here.

        SetEvent(m_stopEvent);
    }
}

void DebuggerShell::Continue(ICorDebugController *controller,
                             ICorDebugThread *thread,
                             DebuggerUnmanagedThread *unmanagedThread,
                             BOOL fIsOutOfBand)
{
    HRESULT hr = S_OK;

    if (!m_stop || fIsOutOfBand)
    {
        m_enableCtrlBreak = true;
        hr = controller->Continue(fIsOutOfBand);

        if (FAILED(hr) && !m_stop)
            g_pShell->ReportError(hr);
    }
    else
    {
        //
        // Just go ahead and continue from any events on other processes.
        //
        ReleaseHolder<ICorDebugProcess> process;
        hr = S_OK;
        hr = controller->QueryInterface(IID_ICorDebugProcess,
                                         (void **)&process);

        if (hr==E_NOINTERFACE ||
            process == NULL)
        {
            ReleaseHolder<ICorDebugAppDomain> appDomain;
            hr = controller->QueryInterface(IID_ICorDebugAppDomain,
                                            (void **)&appDomain);
            _ASSERTE(!FAILED(hr));

            hr = appDomain->GetProcess(&process);
            _ASSERTE(!FAILED(hr));
            _ASSERTE(process != NULL);
        }

        if (!FAILED(hr) &&
            process != m_targetProcess &&
            process != NULL)
        {
            m_enableCtrlBreak = true;
            hr = controller->Continue(FALSE);

            if (FAILED(hr))
                g_pShell->ReportError(hr);
        }
        else
        {
            SetEvent(m_stopEvent);
        }

    }
}

void DebuggerShell::Interrupt()
{
    _ASSERTE(m_targetProcess);
    HRESULT hr = m_targetProcess->Stop(INFINITE);

    if (FAILED(hr))
    {
        Write(L"\nError stopping process:  ", hr);
        ReportError(hr);
    }
    else
        Stop(m_targetProcess, NULL);
}

void DebuggerShell::SetTargetProcess(ICorDebugProcess *pProcess)
{
    if (pProcess != m_targetProcess)
    {
        if (m_targetProcess != NULL)
            RELEASE(m_targetProcess);

        m_targetProcess = pProcess;

        if (pProcess != NULL)
            pProcess->AddRef();

        //
        // If we're done with a process, remove all of the modules.
        // This will clean up if we miss some unload module events.
        //

        if (m_targetProcess == NULL)
        {
            g_pShell->m_modules.RemoveAll();
            m_targetProcessHandledFirstException = false;
        }
    }
}

void DebuggerShell::SetCurrentThread(ICorDebugProcess *pProcess,
                                     ICorDebugThread *pThread,
                                     DebuggerUnmanagedThread *pUnmanagedThread)
{
    if (pThread != NULL && pUnmanagedThread == NULL)
    {
        //
        // Lookup the corresponding unmanaged thread
        //

        DWORD threadID;
        HRESULT hr;

        hr = pThread->GetID(&threadID);
        if (SUCCEEDED(hr))
        {
            pUnmanagedThread =
              (DebuggerUnmanagedThread*) m_unmanagedThreads.GetBase(threadID);
        }
    }
    else if (pUnmanagedThread != NULL && pThread == NULL)
    {
        //
        // Lookup the corresponding managed thread
        //

        HRESULT hr;

        hr = pProcess->GetThread((DWORD)pUnmanagedThread->GetId(), &pThread);
        if (pThread != NULL)
            RELEASE(pThread);
    }

    if (pProcess != m_currentProcess)
    {
        if (m_currentProcess != NULL)
            RELEASE(m_currentProcess);

        m_currentProcess = pProcess;

        if (pProcess != NULL)
            pProcess->AddRef();
    }

    if (pThread != m_currentThread)
    {
        if (m_currentThread != NULL)
        {
            RELEASE(m_currentThread);
        }
        m_currentThread = pThread;

        if (m_currentThread2 != NULL)
        {
            RELEASE(m_currentThread2);
            m_currentThread2 = NULL;
        }
        if (pThread != NULL)
        {
            pThread->AddRef();
            pThread->QueryInterface(IID_ICorDebugThread2, (void **)&m_currentThread2);
        }
    }

    m_currentUnmanagedThread = pUnmanagedThread;

    SetCurrentChain(NULL);
    SetCurrentFrame(NULL);
}

void DebuggerShell::SetCurrentChain(ICorDebugChain *chain)
{
    if (chain != m_currentChain)
    {
        if (m_currentChain != NULL)
            RELEASE(m_currentChain);

        m_currentChain = chain;

        if (chain != NULL)
            chain->AddRef();
    }
}

void DebuggerShell::SetCurrentFrame(ICorDebugFrame *frame)
{
    if (frame != m_rawCurrentFrame)
    {
        m_currentFrame.Clear();
        m_rawCurrentFrame.Assign(frame);

        if (frame != NULL)
        {
            frame->QueryInterface(IID_ICorDebugILFrame, (void **) &m_currentFrame);
        }
    }
}

void DebuggerShell::SetDefaultFrame()
{
    if (m_currentThread != NULL)
    {
        ICorDebugChain *ichain;
        HRESULT hr = m_currentThread->GetActiveChain(&ichain);

        if (FAILED(hr))
        {
            g_pShell->ReportError(hr);
            return;
        }

        SetCurrentChain(ichain);

        if (ichain != NULL)
        {
            RELEASE(ichain);

            ICorDebugFrame *iframe;

            hr = m_currentThread->GetActiveFrame(&iframe);

            if (FAILED(hr))
            {
                g_pShell->ReportError(hr);
                return;
            }

            SetCurrentFrame(iframe);

            if (iframe != NULL)
                RELEASE(iframe);
        }
        else
            SetCurrentFrame(NULL);
    }
}

static const WCHAR WcharFromDebugState(CorDebugThreadState debugState)
{
    WCHAR sz;

    switch( debugState )
    {
        case THREAD_RUN:
            sz = L'R';
            break;
        case THREAD_SUSPEND:
            sz = L'S';
            break;
        default:
            _ASSERTE( !"WcharFromDebugState given an invalid value" );
            sz = L'?';
            break;
    }

    return sz;
}

HRESULT DebuggerShell::PrintThreadState(ICorDebugThread *thread)
{
    DWORD threadID;
    HRESULT hr;

    if (thread == NULL)
        return S_OK;

    hr = thread->GetID(&threadID);

    if (FAILED(hr))
        return hr;

    Write(L"Thread 0x%x", threadID);

    CorDebugThreadState ds;
    if( !FAILED(thread->GetDebugState(&ds)))
    {
        Write(L" %c ", WcharFromDebugState(ds));
    }
    else
    {
        Write(L" - ");
    }

    ReleaseHolder<ICorDebugILFrame> ilframe;
    ReleaseHolder<ICorDebugNativeFrame> nativeframe;
    ReleaseHolder<ICorDebugInternalFrame> internalframe;

    if (thread == m_currentThread)
    {
        ilframe.Assign(m_currentFrame);

        if (m_rawCurrentFrame != NULL )
        {
            m_rawCurrentFrame->QueryInterface( IID_ICorDebugNativeFrame,
                                (void **)&nativeframe);

            m_rawCurrentFrame->QueryInterface( IID_ICorDebugInternalFrame,
                                (void **)&internalframe);
        }
    }
    else
    {
        ICorDebugFrame *iframe;
        hr = thread->GetActiveFrame(&iframe);
        if (FAILED(hr))
        {
            if (hr == CORDBG_E_BAD_THREAD_STATE)
                Write(L" no stack, thread is exiting.\n");
            else
                ReportError(hr);

            return hr;
        }

        if (iframe != NULL)
        {
            iframe->QueryInterface(IID_ICorDebugILFrame,
                                        (void **) &ilframe);

            iframe->QueryInterface( IID_ICorDebugNativeFrame,
                                    (void **)&nativeframe);

            iframe->QueryInterface( IID_ICorDebugInternalFrame,
                                    (void **)&internalframe);
        }

    }

    if (internalframe != NULL)
    {
        PrintInternalFrame(internalframe);
    }
    else if ( nativeframe != NULL)
    {
        DWORD id;
        hr = thread->GetID(&id);

        if (SUCCEEDED(hr))
        {
            ICorDebugCode *icode;
            if (ilframe != NULL )
                hr = ilframe->GetCode(&icode);
            else
                hr = nativeframe->GetCode( &icode );

            if (SUCCEEDED(hr))
            {
                ICorDebugFunction *ifunction;
                hr = icode->GetFunction(&ifunction);

                if (SUCCEEDED(hr))
                {
                    DebuggerFunction *function;
                    function = DebuggerFunction::FromCorDebug(ifunction);
                    _ASSERTE(function != NULL);

                    ULONG32 ip = 0;
                    ULONG32 nativeIp = 0;
                    bool fILIP = false;
                    if (nativeframe != NULL )
                    {
                        hr = nativeframe->GetIP(&nativeIp);
                    }
                    if (ilframe != NULL && !FAILED( hr ) )
                    {
                        CorDebugMappingResult mappingResult;
                        if (!FAILED( ilframe->GetIP(&ip, &mappingResult) ) )
                            fILIP = true;
                    }

                    if (SUCCEEDED(hr))
                    {
                        DebuggerSourceFile *sf = NULL;
                        unsigned int lineNumber = 0;

                        if (fILIP)
                            hr = function->FindLineFromIP(ip, &sf,
                                                          &lineNumber);

                        if (SUCCEEDED(hr))
                        {
                            Write(L" at %s::%s", function->m_className.GetData(), function->m_name.GetData());

                            Write(L" +%.4x", nativeIp);
                            if (fILIP
                                && m_rgfActiveModes & DSM_IL_NATIVE_PRINTING)
                                Write( L"[native] +%.4x[IL]", ip );

                            if (sf != NULL)
                                Write(L" in %s:", sf->GetName());
                            if (DebuggerFunction::kNoSourceForIL != lineNumber)
                            {
                                Write(L"%d", lineNumber);
                            }
                            else
                            {
                                Write(L"IP does not map to source code");
                            }
                        }
                        else
                            g_pShell->ReportError(hr);
                    }
                    else
                        g_pShell->ReportError(hr);

                    RELEASE(ifunction);
                }
                else
                    g_pShell->ReportError(hr);

                RELEASE(icode);
            }
            else
                g_pShell->ReportError(hr);
        }
        else
            g_pShell->ReportError(hr);

    }
    else
    {
        //
        // See if we at least have a current chain
        //

        ICorDebugChain *ichain = NULL;

        if (thread == m_currentThread)
        {
            ichain = m_currentChain;
            if (ichain != NULL)
                ichain->AddRef();
        }
        else
        {
            hr = thread->GetActiveChain(&ichain);

            if (FAILED(hr))
                return hr;
        }

        if (ichain != NULL)
        {
            BOOL isManaged;
            hr = ichain->IsManaged(&isManaged);

            if (FAILED(hr))
                return hr;

            if (isManaged)
            {
                //
                // Just print the chain - it has no frames so will
                // be one line
                //

                PrintChain(ichain);
            }

            RELEASE(ichain);
        }
        else
            Write(L" <no information available>");
    }

    Write(L"\n");

    return S_OK;
}


HRESULT DebuggerShell::PrintChain(ICorDebugChain *chain,
                                  int *frameIndex,
                                  int *iNumFramesToShow)
{
    ULONG count;
    BOOL isManaged;
    int frameCount = 0;
    int iNumFrames = 1000;

    if (frameIndex != NULL)
        frameCount = *frameIndex;

    if (iNumFramesToShow != NULL)
        iNumFrames = *iNumFramesToShow;

    // Determined whether or not the chain is managed
    HRESULT hr = chain->IsManaged(&isManaged);

    if (FAILED(hr))
        return hr;

    // Chain is managed, so information can be displayed
    if (isManaged)
    {
        // Enumerate every frame in the chain
        ReleaseHolder<ICorDebugFrameEnum> fe;
        hr = chain->EnumerateFrames(&fe);

        if (FAILED(hr))
            return hr;



        // Print each frame
        while(iNumFrames > 0)
        {
            ReleaseHolder<ICorDebugFrame> iframe;
            hr = fe->Next(1, &iframe, &count);

            if (count == 0)
            {
                break;
            }

            if (FAILED(hr))
            {
                return hr;
            }

            // Indicate the current frame
            if ((chain == m_currentChain) && (iframe == m_rawCurrentFrame))
            {
                Write(L"%d)* ", frameCount);
            }
            else
            {
                Write(L"%d)  ", frameCount);
            }

            frameCount++;
            iNumFrames--;

            PrintFrame(iframe);
        }
    }

    CorDebugChainReason reason;

    // Get & print chain's reason
    hr = chain->GetReason(&reason);

    if (FAILED(hr))
        return hr;

    LPWSTR reasonString = GetChainReasonName(reason);


    // If we're showing verbose interop-debugging info, print stack range for each chain.
    if (g_pShell->m_rgfActiveModes & DSM_SHOW_UNMANAGED_TRACE)
    {
        CORDB_ADDRESS start;
        CORDB_ADDRESS end;
        hr = chain->GetStackRange(&start, &end);

        Write(L"-- StackRange:");
        if (FAILED(hr))
        {
            Write(L"Failed,hr=%08x --", hr);
        } else {
            Write(L"(%08x,%08x) --", CORDB_ADDRESS_TO_PTR(start), CORDB_ADDRESS_TO_PTR(end));
        }
    }



    if (reasonString != NULL)
        Write(L"--- %s ---\n", reasonString);


    if (frameIndex != NULL)
        *frameIndex = frameCount;

    if (iNumFramesToShow != NULL)
        *iNumFramesToShow = iNumFrames;

    return S_OK;
}

HRESULT DebuggerShell::PrintFrame(ICorDebugFrame *frame)
{
    ReleaseHolder<ICorDebugILFrame>       ilframe;
    ReleaseHolder<ICorDebugCode>          icode;
    ReleaseHolder<ICorDebugFunction>      ifunction;
    ReleaseHolder<ICorDebugNativeFrame>   icdNativeFrame;
    ReleaseHolder<ICorDebugFunction2>     ifunction2;
    ReleaseHolder<ICorDebugInternalFrame> iInternalFrame;

    DebuggerFunction       *function = NULL;
    unsigned int            j;
    DebuggerSourceFile     *sf = NULL;
    unsigned int            lineNumber = 0;
    bool                    fILIP = false;
    ULONG32                 nativeIp = 0;
    WCHAR                   wsz[40];
    HRESULT hr;


    // If we're showing verbose interop-debugging info, print stack range for each frame.
    if (g_pShell->m_rgfActiveModes & DSM_SHOW_UNMANAGED_TRACE)
    {
        CORDB_ADDRESS start;
        CORDB_ADDRESS end;
        hr = frame->GetStackRange(&start, &end);

        Write(L" (StackRange:");
        if (FAILED(hr))
        {
            Write(L"Failed,hr=%08x)", hr);
        } else {
            Write(L"%08x,%08x)", CORDB_ADDRESS_TO_PTR(start), CORDB_ADDRESS_TO_PTR(end));
        }
    }


    // Is this a StubFrame?
    hr = frame->QueryInterface(IID_ICorDebugInternalFrame,  (void**) &iInternalFrame);
    if (SUCCEEDED(hr))
    {
        PrintInternalFrame(iInternalFrame);
        Write(L"\n");
        return S_OK;
    }


    // Get the native frame for the current frame
    hr = frame->QueryInterface(IID_ICorDebugNativeFrame,
                                       (void **)&icdNativeFrame);

    if (FAILED(hr))
    {
        icdNativeFrame.Clear();
    }

    // Get the IL frame for the current frame
    hr = frame->QueryInterface(IID_ICorDebugILFrame,
                               (void **) &ilframe);

    if (FAILED(hr))
        ilframe.Clear();

    // Get the code for the frame
    if (ilframe != NULL )
    {
        hr = ilframe->GetCode(&icode);
    }
    else if (icdNativeFrame != NULL )
    {
        hr = icdNativeFrame->GetCode(&icode);
    }
    else
    {
        hr = E_FAIL;
    }

    if (FAILED(hr))
    {
        Write(L"[Unable to obtain any code information]");
        goto LExit;
    }

    // Get the function for the code
    hr = icode->GetFunction(&ifunction);

    if (FAILED(hr))
    {
        Write(L"[Unable to obtain any function information]");
        goto LExit;
    }

    // Get the DebuggerFunction for the function iface
    function = DebuggerFunction::FromCorDebug(ifunction);
    _ASSERTE(function);

    // Get the IP for the current frame
    ULONG32 ip;

    if (ilframe != NULL)
    {
        CorDebugMappingResult mappingResult;

        hr = ilframe->GetIP(&ip, &mappingResult);

        // Find the source line for the IP
        hr = function->FindLineFromIP(ip, &sf, &lineNumber);

        if (FAILED(hr))
            ip = 0;
        else
            fILIP = true;
    }

    // if this isn't supported it's ok, so no need to check HR
    hr = ifunction->QueryInterface(IID_ICorDebugFunction2, (void**) &ifunction2);

    if (m_rgfActiveModes & DSM_ALLOW_JMC_STEPPING)
    {
        BOOL fIsUserCode;

        if (ifunction2 != NULL)
        {
            hr = ifunction2->GetJMCStatus(&fIsUserCode);
            if (FAILED(hr))
            {
                Write(L"(error)   ");
            } else {
                Write(fIsUserCode ? L"(User)    " : L"(non-user)");
            }
        }
    }

    // For AD transitions, we should have chain markers.
    // Print AppDomain for each frame.
    if (g_pShell->m_rgfActiveModes & DSM_ENHANCED_DIAGNOSTICS)
    {
        PrintADNameFromFrame(frame);
    }


    // If the module names are desired, then include the name in front of
    // the class info ntsd-style.
    if (m_rgfActiveModes & DSM_SHOW_MODULES_IN_STACK_TRACE)
    {
        WCHAR       *szModule;
        WCHAR       rcModule[_MAX_PATH];

        DebuggerModule *module = function->GetModule();
        szModule = module->GetName();
        _wsplitpath(szModule, NULL, NULL, rcModule, NULL);
        Write(L"%s!", rcModule);
    }

    // Write out the class and method for the current IP
    Write(L"%s%s::%s",
          function->GetNamespaceName(),
          function->GetClassName(),
          function->GetName());

    if ((g_pShell->m_rgfActiveModes & DSM_ENHANCED_DIAGNOSTICS) &&
        (ifunction2 != NULL))
    {
        UINT encVersion = 0;
        ifunction2->GetVersionNumber(&encVersion);
        UINT codeSize = 0;
        icode->GetSize(&codeSize);

        Write(L"(V %d, size %d)", encVersion, codeSize);
    }

    // Print out the funtion's source file, line and start addr
    if (icdNativeFrame == NULL)
    {
        if (fILIP == true)
            Write( L" +%.4x[IL]", ip);
    }
    else
    {
        if (!FAILED(icdNativeFrame->GetIP(&nativeIp)))
            Write(L" +%.4x", nativeIp);

        if ((m_rgfActiveModes & DSM_IL_NATIVE_PRINTING) && fILIP == true)
            Write( L"[native] +%.4x[IL]", ip);
    }

    if (DebuggerFunction::kNoSourceForIL != lineNumber)
    {
        if (sf && sf->GetPath())
            Write(L" in %s:%d", sf->GetPath(), lineNumber);
        else if (sf && sf->GetName())
            Write(L" in %s:%d", sf->GetName(), lineNumber);
        else
            Write(L" in <Unknown File Name>:%d", lineNumber);
    }
    else
    {
        if (sf && sf->GetPath())
            Write(L" in %s:<Unknown Line Number>", sf->GetPath());
        else if (sf && sf->GetName())
            Write(L" in %s:<Unknown Line Number>", sf->GetName());
        else
            Write(L" in <Unknown File Name>:<Unknown Line Number>");
    }

    // if currently associated source file does not have
    // lineNumber number of lines, warn the user
    if (lineNumber > 0)
    {
        if (sf != NULL)
        {
            if (sf->GetPath() && (sf->TotalLines() < lineNumber))
                Write(L"\tWARNING: The currently associated source file has only %d lines."
                        , sf->TotalLines());
        }
    }

    if (m_rgfActiveModes & DSM_SHOW_ARGS_IN_STACK_TRACE)
    {
        // Now print out the arguments for the method
        ReleaseHolder<ICorDebugILFrame> ilf;

        hr = frame->QueryInterface(IID_ICorDebugILFrame, (void **)&ilf);

        if (FAILED(hr))
            goto LExit;

        ReleaseHolder<ICorDebugValueEnum> pArgs;

        hr = ilf->EnumerateArguments(&pArgs);

        if (!SUCCEEDED(hr))
            goto LExit;

        ilf.Clear();

        ULONG argCount;

        hr = pArgs->GetCount(&argCount);

        if (!SUCCEEDED(hr))
            goto LExit;

#ifdef _DEBUG
        bool fVarArgs = false;
        PCCOR_SIGNATURE sig = function->GetSignature();
        ULONG callConv = CorSigUncompressCallingConv(sig);

        if ((callConv & IMAGE_CEE_CS_CALLCONV_MASK) &
            IMAGE_CEE_CS_CALLCONV_VARARG)
            fVarArgs = true;

        ULONG cTemp = function->GetArgumentCount();
#endif //_DEBUG

        // Var Args functions have call-site-specific numbers of
        // arguments
        _ASSERTE( argCount == cTemp || fVarArgs);

        ICorDebugValue * ival = NULL;
        ULONG celtFetched = 0;

        // Print out each argument first
        // Avoid printing "this" in arg list for static methods
        if (function->IsStatic())
        {
            j = 0;
        }
        else
        {
            j = 1;

            hr = pArgs->Next(1, &ival,&celtFetched);
            if (ival != NULL)
            {
                ival->Release();
                ival = NULL;
            }
        }

        LPWSTR nameWsz;
        for (; j < argCount; j++)
        {
            DebuggerVarInfo* arg = function->GetArgumentAt(j);

            Write(L"\n\t\t");
            if (arg != NULL)
            {
                MAKE_WIDEPTR_FROMUTF8(nameW, arg->GetName());
                nameWsz = nameW;
            }
            else
            {
                wsprintf( wsz, L"Arg%d", j );
                nameWsz = wsz;
            }

            // Get the field value
            _ASSERTE(ival == NULL);
            hr = pArgs->Next(1, &ival,&celtFetched);

            // If successful, print the variable
            if (SUCCEEDED(hr) && celtFetched==1)
            {

                PrintVariable(nameWsz, ival, 0, FALSE);
                _ASSERTE(ival == NULL);
            }

            // Otherwise, indicate that it is unavailable
            else
                Write(L"%s = <unavailable>", nameWsz);
        }
        pArgs.Clear();
    }

    if (m_rgfActiveModes & DSM_SHOW_TYARGS_IN_STACK_TRACE)
    {
        // Now print out the type arguments for the method
        ReleaseHolder<ICorDebugILFrame2> ilf;

        hr = frame->QueryInterface(IID_ICorDebugILFrame2, (void **)&ilf);

        if (FAILED(hr))
            goto LExit;

        ReleaseHolder<ICorDebugTypeEnum> pArgs;

        hr = ilf->EnumerateTypeParameters(&pArgs);

        if (!SUCCEEDED(hr))
            goto LExit;

        ilf.Clear();

        ULONG argCount;

        hr = pArgs->GetCount(&argCount);

        if (!SUCCEEDED(hr))
            goto LExit;

        ICorDebugType *itype;
        ULONG celtFetched = 0;

        LPWSTR nameWsz;
        for (j = 0; j < argCount; j++)
        {
            DebuggerVarInfo* arg = function->GetTypeArgumentAt(j);

            Write(L"\n\t\t");
            if (arg != NULL && arg->GetName() != NULL)
            {
                MAKE_WIDEPTR_FROMUTF8(nameW, arg->GetName());
                nameWsz = nameW;
            }
            else
            {
                wsprintf( wsz, L"T%d", j );
                nameWsz = wsz;
            }

            // Get the field value
            hr = pArgs->Next(1, &itype,&celtFetched);

            // If successful, print the variable
            if (SUCCEEDED(hr) && celtFetched==1)
            {

                PrintType(nameWsz, itype, 0);
            }

            // Otherwise, indicate that it is unavailable
            else
                Write(L"%s = <unavailable>", nameWsz);
        }

        pArgs.Clear();
    }

 LExit:
    Write(L"\n");

    return hr;
}


DebuggerBreakpoint *DebuggerShell::FindBreakpoint(SIZE_T id)
{
    DebuggerBreakpoint *b = m_breakpoints;

    while (b != NULL)
    {
        if (b->m_id == id)
            return (b);

        b = b->m_next;
    }

    return (NULL);
}

void DebuggerShell::RemoveAllBreakpoints()
{
    while (m_breakpoints != NULL)
    {
        delete m_breakpoints;
    }
}

void DebuggerShell::OnActivateBreakpoint(DebuggerBreakpoint *pb)
{
}

void DebuggerShell::OnDeactivateBreakpoint(DebuggerBreakpoint *pb)
{
}

void DebuggerShell::OnBindBreakpoint(DebuggerBreakpoint *pb, DebuggerModule *pm)
{

    if (pb->m_functionVersion == 0)
    {
        if (pb->m_managed)
        {
            Write(L"Breakpoint #%d has bound to %s.\n", pb->GetId(), pm ? pm->GetName() : L"<unknown>");
        }
        else
        {
            Write(L"Breakpoint #%d has bound to address 0x%p.\n", pb->GetId(), CORDB_ADDRESS_TO_PTR(pb->m_address));
        }
    }
    else
    {
        Write(L"Breakpoint #%d has bound to version %d within %s.\n", pb->GetId(),
              pb->m_functionVersion, pm ? pm->GetName() : L"<unknown>");
    }
}

void DebuggerShell::OnUnBindBreakpoint(DebuggerBreakpoint *pb, DebuggerModule *pm)
{
    Write(L"Breakpoint #%d has unbound from %s.\n", pb->GetId(),
          pm ? pm->GetName() : L"<unknown>");
}

bool DebuggerShell::ReadLine(__out_ecount_full(maxCount) WCHAR *buffer, int maxCount)
{
    CQuickBytes mbBuf;
    CHAR *szBufMB  = (CHAR *)mbBuf.Alloc(maxCount * sizeof(CHAR));

    // MultiByteToWideChar fails to terminate the string with 2 null characters
    // Instead it only uses one. That's why we need to zero the memory out.
    _ASSERTE( (buffer != NULL) && (maxCount > 0) );
    memset(buffer, 0, maxCount * sizeof(WCHAR));
    memset(szBufMB, 0, maxCount * sizeof(CHAR));

    // Keep PreFix from complaining about using a possibly empty buffer
    if( maxCount <= 0 )
    {
        return false;
    }

    if (!fgets(szBufMB, maxCount, m_in))
    {
        if (m_in == stdin)
        {
            // Must have piped commands in
            m_quit = true;
        }

        return false;
    }

    // Try the write
    MultiByteToWideChar(GetConsoleCP(), 0, szBufMB, (int)strlen(szBufMB), buffer, maxCount);

    WCHAR *ptr = wcschr(buffer, L'\n');

    if (ptr)
    {
        // Get rid of the newline character it it's there
        *ptr = L'\0';
    }
    else if (fgets(szBufMB, maxCount, m_in))
    {
        Write(L"The input string was too long.\n");

        while(!strchr(szBufMB, L'\n') && fgets(szBufMB, maxCount, m_in))
        {
            ;
        }

        *buffer = L'\0';

        return false;
    }

    if (m_in != stdin)
    {
        Write(L"%s\n", buffer);
    }

    return true;
}

#define INIT_WRITE_BUF_SIZE 4096
HRESULT DebuggerShell::CommonWrite(FILE *out, const WCHAR *buffer, va_list args)
{
    BOOL fNeedToDeleteDB = FALSE;
    // We need to tack a "+1" tacked onto all our allocates so that we can
    // whack a NULL character onto it, but NOT include it in our doublebyte (Wide) count
    // so that we don't actually store any data in it.
    SIZE_T curBufSizeDB = INIT_WRITE_BUF_SIZE;
    CQuickBytes dbBuf;
    WCHAR *szBufDB = (WCHAR *)dbBuf.Alloc( (int)(curBufSizeDB+1) * (int)sizeof(WCHAR));
    int cchWrittenDB = -1;
    if (szBufDB != NULL)
        cchWrittenDB = _vsnwprintf(szBufDB, INIT_WRITE_BUF_SIZE, buffer, args);

    if (cchWrittenDB == -1)
    {
        szBufDB = NULL;

        while (cchWrittenDB == -1)
        {
            delete [] szBufDB;
            szBufDB = new WCHAR[(curBufSizeDB+1) * 4];

            // Out of memory, nothing we can do
            if (!szBufDB)
                return E_OUTOFMEMORY;

            curBufSizeDB *= 4;
            fNeedToDeleteDB = TRUE;

            cchWrittenDB = _vsnwprintf(szBufDB, curBufSizeDB, buffer, args);
        }
    }

    // Double check that we're null-terminated.  Note that this uses the extra
    // space we tacked onto the end
    szBufDB[curBufSizeDB] = L'\0';

    // Allocate buffer
    BOOL fNeedToDeleteMB = FALSE;
    SIZE_T curBufSizeMB = INIT_WRITE_BUF_SIZE+1; // +1 from above percolates through
    CQuickBytes mbBuf;
    CHAR *szBufMB  = (CHAR *)mbBuf.Alloc((int)(curBufSizeMB * sizeof(CHAR)));

    // Try the write
    int cchWrittenMB = 0;
    if(szBufMB != NULL)
        cchWrittenMB = WideCharToMultiByte(GetConsoleOutputCP(), 0, szBufDB, -1, szBufMB, (int)curBufSizeMB, NULL, NULL);

    if (cchWrittenMB == 0)
    {
        // Figure out size required
        int cchReqMB = WideCharToMultiByte(GetConsoleOutputCP(), 0, szBufDB, -1, NULL, 0, NULL, NULL);
        _ASSERTE(cchReqMB > 0);

        // I don't think the +1 is necessary, but I'm doing it to make sure (WideCharToMultiByte is a bit
        // shady in whether or not it writes the null after the end of the buffer)
        szBufMB = new CHAR[cchReqMB+1];

        // Out of memory, nothing we can do
        if (!szBufDB)
        {
            if (fNeedToDeleteDB)
                delete [] szBufDB;

            return E_OUTOFMEMORY;
        }

        curBufSizeMB = cchReqMB;
        fNeedToDeleteMB = TRUE;

        // Try the write
        cchWrittenMB = WideCharToMultiByte(GetConsoleOutputCP(), 0, szBufDB, -1, szBufMB, (int)curBufSizeMB, NULL, NULL);
        _ASSERTE(cchWrittenMB > 0);
    }

    // Finally, write it
    fputs(szBufMB, out);

    // Clean up
    if (fNeedToDeleteDB)
        delete [] szBufDB;

    if (fNeedToDeleteMB)
        delete [] szBufMB;

    return S_OK;
}

HRESULT DebuggerShell::Write(const WCHAR *buffer, ...)
{
    HRESULT hr;
    va_list     args;

    va_start(args, buffer);

    hr = CommonWrite(m_out, buffer, args);

    va_end(args);

    return hr;
}

HRESULT DebuggerShell::WriteBigString(__inout_ecount(count) WCHAR *s, ULONG32 count)
{
    // The idea is that we'll print subparts iteratively,
    // rather than trying to do everything all at once.
    ULONG32 chunksize = 4096;
    ULONG32 iEndOfChunk = 0;
    WCHAR temp;
    HRESULT hr = S_OK;

    while(iEndOfChunk < count && hr == S_OK)
    {
        if (iEndOfChunk + chunksize > count)
            chunksize = count - iEndOfChunk;

        iEndOfChunk += chunksize;
        temp = s[iEndOfChunk];
        s[iEndOfChunk] = '\0';
        hr = Write(L"%s", &(s[iEndOfChunk-chunksize]));
        s[iEndOfChunk] = temp;
    }

    return hr;
}

// Output to the user
void DebuggerShell::Error(const WCHAR *buffer, ...)
{
    va_list     args;

    va_start(args, buffer);

    CommonWrite(m_out, buffer, args);

    va_end(args);
}

//
// Print a little whitespace on the current line for indenting.
//

void DebuggerShell::PrintIndent(unsigned int level)
{
    unsigned int i;

    for (i = 0; i < level; i++)
        Write(L"  ");
}

//
// Write the name of a variable out, but only if it is valid.
//
void DebuggerShell::PrintVarName(const WCHAR* name)
{
    if (name != NULL)
        Write(L"%s=", name);
}

//
// Get all the indicies for an array.
//
HRESULT DebuggerShell::GetArrayIndicies(__inout_z __deref_inout WCHAR **pp,
                                        ICorDebugILFrame *context,
                                        ULONG32 rank,
                                        ULONG32 *indicies)
{
    HRESULT hr = S_OK;
    WCHAR *p = *pp;

    for (unsigned int i = 0; i < rank; i++)
    {
        if (*p != L'[')
        {
            Error(L"Missing open bracked on array index.\n");
            hr = E_FAIL;
            goto exit;
        }

        p++;

        // Check for close bracket
        const WCHAR *indexStart = p;
        int nestLevel = 1;

        while (*p)
        {
            _ASSERTE(nestLevel != 0);

            if (*p == L'[')
                nestLevel++;

            if (*p == L']')
                nestLevel--;

            if (nestLevel == 0)
                break;

            p++;
        }

        if (nestLevel != 0)
        {
            Error(L"Missing close bracket on array index.\n");
            hr = E_FAIL;
            goto exit;
        }

#ifdef _DEBUG
        const WCHAR *indexEnd = p;
#endif
        p++;

        // Get index
        int index;
        bool indexFound = false;

        if (!GetIntArg(indexStart, index))
        {
            WCHAR tmpStr[256];

            _ASSERTE( indexEnd >= indexStart );
            wcsncpy(tmpStr, indexStart, 255);
            tmpStr[255] = L'\0';

            ICorDebugValue *iIndexValue = EvaluateExpression(tmpStr, context);

            if (iIndexValue != NULL)
            {
                ICorDebugGenericValue *igeneric;
                hr = iIndexValue->QueryInterface(IID_ICorDebugGenericValue,
                                                 (void **) &igeneric);

                if (SUCCEEDED(hr))
                {
                    CorElementType indexType;
                    hr = igeneric->GetType(&indexType);

                    if (SUCCEEDED(hr))
                    {
                        if ((indexType == ELEMENT_TYPE_I1)  ||
                            (indexType == ELEMENT_TYPE_U1)  ||
                            (indexType == ELEMENT_TYPE_I2)  ||
                            (indexType == ELEMENT_TYPE_U2)  ||
                            (indexType == ELEMENT_TYPE_I4)  ||
                            (indexType == ELEMENT_TYPE_U4))
                        {
                            hr = igeneric->GetValue(&index);

                            if (SUCCEEDED(hr))
                                indexFound = true;
                            else
                                ReportError(hr);
                        }
                    }
                    else
                        ReportError(hr);

                    RELEASE(igeneric);
                }
                else
                    ReportError(hr);

                RELEASE(iIndexValue);
            }
        }
        else
            indexFound = true;

        if (!indexFound)
        {
            Error(L"Invalid array index. Must use a number or "
                  L"a variable of type: I1, UI1, I2, UI2, I4, UI4.\n");
            hr = E_FAIL;
            goto exit;
        }

        indicies[i] = index;
    }

exit:
    *pp = p;
    return hr;
}

bool DebuggerShell::EvaluateAndPrintGlobals(const WCHAR *exp)
{
    return this->MatchAndPrintSymbols((WCHAR *)exp, FALSE, true );
}

// Caller must release return value.
ICorDebugValue *DebuggerShell::EvaluateExpression(const WCHAR *exp,
                                                  ICorDebugILFrame *context,
                                                  bool silently)
{
    HRESULT hr;
    const WCHAR *p = exp;

    // Skip white space
    while (*p && iswspace(*p))
        p++;

    // First component of expression must be a name (variable or class static)
    const WCHAR *name = p;

    while (*p && !iswspace(*p) && *p != L'[' && *p != L'.')
        p++;

    if (p == name)
    {
        Error(L"Syntax error, name missing in %s\n", exp);
        return (NULL);
    }

    DebuggerString nameAlloc;
    if (!nameAlloc.CopyNFrom(name, (int) (p-name)))
    {
        return NULL;
    }

    bool unavailable;
    ICorDebugValue *value = EvaluateName(nameAlloc, context, &unavailable);

    if (unavailable)
    {
        Error(L"Variable %s is in scope but unavailable.\n", nameAlloc.GetData());
        return (NULL);
    }

    if (value == NULL)
    {
        ICorDebugType *itype;
        mdFieldDef fd;
        bool isStatic;

        // See if we've got a static field name here...
        hr = ResolveQualifiedFieldName(NULL, nameAlloc.GetData(),
                                       &itype, &fd, &isStatic);

        if (FAILED(hr))
        {
            if (!silently)
            {
                Error(L"%s is not an argument, local, or class static.\n", nameAlloc.GetData());
            }
            return (NULL);
        }

        if (isStatic)
        {
            if (!context)
            {
                if (!silently)
                {
                    Error(L"Must have a context to display %s.\n", nameAlloc.GetData());
                }

                return (NULL);
            }

            // We need an ICorDebugFrame to pass in here...
            ReleaseHolder<ICorDebugFrame> pFrame;
            hr = context->QueryInterface(IID_ICorDebugFrame, (void**)&pFrame);
            _ASSERTE(SUCCEEDED(hr));

            // Get the class for containing the static
            ReleaseHolder<ICorDebugClass> iclass;
            hr = itype->GetClass(&iclass);
            _ASSERTE(SUCCEEDED(hr));

            // Grab the value of the static field off of the class.
            hr = iclass->GetStaticFieldValue(fd, pFrame, &value);

            if (FAILED(hr))
            {
                g_pShell->ReportError(hr);
                return (NULL);
            }
        }
        else
        {
            if (!silently)
                Error(L"%s is not a static field.\n", nameAlloc.GetData());

            return (NULL);
        }
    }

    //
    // Now look for suffixes to the name
    //
    _ASSERTE(value != NULL);

    // 'value' should now have 1 outstanding ref. It's an out-param so our caller will release that.

    while (TRUE)
    {
        // Skip white space
        while (*p != L'\0' && iswspace(*p))
            p++;

        if (*p == L'\0')
            return (value);

        switch (*p)
        {
        case L'.':
            {
                p++;

                // Strip off any reference values.
                hr = StripReferences(&value, false);

                if (FAILED(hr) || value == NULL)
                {
                    Error(L"Cannot get field of non-object value.\n");

                    if (value)
                        RELEASE(value);

                    return NULL;
                }

                // If we have a boxed object then unbox the little
                // fella...
                ICorDebugBoxValue *boxVal;

                if (SUCCEEDED(value->QueryInterface(IID_ICorDebugBoxValue,
                                                    reinterpret_cast< void** >(&boxVal))))
                {
                    ICorDebugObjectValue *objVal;
                    hr = boxVal->GetObject(&objVal);

                    if (FAILED(hr))
                    {
                        ReportError(hr);
                        RELEASE(boxVal);
                        RELEASE(value);
                        return NULL;
                    }

                    RELEASE(boxVal);
                    RELEASE(value);

                    // Replace the current value with the unboxed object.
                    value = objVal;
                }

                // Now we should have an object, or we're done.
                ICorDebugValue2 *ivalue2;

                if (FAILED(hr = value->QueryInterface(IID_ICorDebugValue2,
                                                      reinterpret_cast< void** >(&ivalue2))))
                {
                    g_pShell->ReportError(hr);
                    RELEASE(value);
                    return NULL;
                }

                ICorDebugObjectValue *object;

                if (FAILED(hr = value->QueryInterface(IID_ICorDebugObjectValue,
                                                      reinterpret_cast< void** >(&object))))
                {
                    Error(L"Cannot get field of non-object value.\n");
                    RELEASE(value);
                    RELEASE(ivalue2);
                    return NULL;
                }

                RELEASE(value);

                        // We'll let the user look at static fields as if
                        // they belong to objects.
                // Get type, class & module
                ICorDebugType *itype;
                hr = ivalue2->GetExactType(&itype);

                if (FAILED(hr))
                {
                    g_pShell->ReportError(hr);
                    RELEASE(object);
                    RELEASE(ivalue2);
                    return (NULL);
                }

                RELEASE(ivalue2);

                //
                // Get field name
                //

                const WCHAR *field = p;

                while (*p && !iswspace(*p) && *p != '[' && *p != '.')
                    p++;

                if (p == field)
                {
                    Error(L"Syntax error, field name missing in %s\n", exp);
                    return (NULL);
                }

                CQuickBytes fieldBuf;
                WCHAR *fieldAlloc = (WCHAR *) fieldBuf.Alloc((int)((p - field + 1) * sizeof(WCHAR)));
                if (fieldAlloc == NULL)
                {
                    Error(L"Couldn't get enough memory to get the field's name!\n");
                    return (NULL);
                }
                wcsncpy(fieldAlloc, field, p - field);
                fieldAlloc[p-field] = L'\0';

                // Lookup field
                mdFieldDef fd = mdFieldDefNil;
                bool isStatic;

                ICorDebugType *itypeForField;
                hr = ResolveQualifiedFieldName(itype, fieldAlloc,
                                               &itypeForField, &fd,
                                               &isStatic);

                if (FAILED(hr))
                {
                    Error(L"Field %s not found.\n", fieldAlloc);

                    RELEASE(object);
                    RELEASE(itype);
                    return (NULL);
                }
                RELEASE(itype);

                _ASSERTE(object != NULL);

                if (!isStatic)
                {
                        ICorDebugClass * iclassForField;
                        hr = itypeForField->GetClass(&iclassForField);
                        _ASSERTE(SUCCEEDED(hr));

                        object->GetFieldValue(iclassForField, fd, &value);
                        RELEASE(iclassForField);
                }
                else
                {
                    // We'll let the user look at static fields as if
                    // they belong to objects.
                    itypeForField->GetStaticFieldValue(fd, NULL, &value);
                }

                RELEASE(itypeForField);
                RELEASE(object);

                break;
            }

        case L'[':
            {
                if (!context)
                {
                    Error(L"Must have a context to display array.\n");
                    return (NULL);
                }

                if (value == NULL)
                {
                    Error(L"Cannot index a class.\n");
                    return (NULL);
                }

                // Strip off any reference values.
                hr = StripReferences(&value, false);

                if (FAILED(hr) || value == NULL)
                {
                    Error(L"Cannot index non-array value.\n");

                    if (value)
                        RELEASE(value);

                    return NULL;
                }

                // Get Array interface
                ICorDebugArrayValue *array;
                hr = value->QueryInterface(IID_ICorDebugArrayValue,
                                           (void**)&array);

                RELEASE(value);

                if (FAILED(hr))
                {
                    Error(L"Cannot index non-array value.\n");
                    return (NULL);
                }

                _ASSERTE(array != NULL);

                // Get the rank
                ULONG32 rank;
                hr = array->GetRank(&rank);

                if (FAILED(hr))
                {
                    g_pShell->ReportError(hr);
                    RELEASE(array);
                    return NULL;
                }

                CQuickBytes qbBuffer;
                if (!(qbBuffer.Alloc(rank * sizeof(ULONG32)))) {
                    RaiseException(STATUS_NO_MEMORY, 0, 0, NULL);
                }
                ULONG32 *indicies = (ULONG32*) qbBuffer.Ptr();

                hr = GetArrayIndicies((WCHAR**)&p, context, rank, indicies);

                if (FAILED(hr))
                {
                    Error(L"Error getting array indicies.\n");
                    RELEASE(array);
                    return NULL;
                }

                // Get element.
                hr = array->GetElement(rank, indicies, &value);

                RELEASE(array);

                if (FAILED(hr))
                {
                    if (hr == E_INVALIDARG)
                        Error(L"Array index out of range.\n");
                    else
                    {
                        Error(L"Error getting array element: ");
                        ReportError(hr);
                    }

                    return (NULL);
                }

                break;
            }
        default:
            Error(L"syntax error, unrecognized character \'%c\'.\n", *p);
            if (value != NULL)
                RELEASE(value);
            return (NULL);
        }
    }
}


HRESULT DebuggerShell::EvaluateTypeExpressions(const WCHAR *exp,
                                            ICorDebugILFrame *context,
                                            ICorDebugType **tyargArray,
                                            unsigned int *pTyargCount,
                                            const WCHAR **afterParse)
{
    HRESULT hr;
    const WCHAR *p = exp;
    ICorDebugClass *pClass = NULL;
    while (TRUE)
    {

        // Skip white space
        while (*p && iswspace(*p))
            p++;

        // First component of expression must be a type name (variable or class static)
        const WCHAR *name = p;

        while (*p && !iswspace(*p) && *p != L'[' && *p != L'<' && *p != L'>' && *p != L',')
            p++;

        if (p == name)
        {
            Error(L"Syntax error, name missing in %s\n", exp);
            return E_FAIL;
        }

        DebuggerString nameAlloc;
        if (!nameAlloc.CopyNFrom(name, (int) (p-name)))
        {
            return E_FAIL;
        }

        DebuggerModule *pDM = NULL;
        mdTypeDef TD;
        hr = ResolveClassName(nameAlloc,&pDM,&TD);

        if (FAILED(hr))
        {
            Error(L"Could not find class %s.\n", nameAlloc.GetData());
            return E_FAIL;
        }

        pClass = NULL;
        hr = pDM->GetICorDebugModule()->GetClassFromToken(TD, &pClass);

        if (FAILED(hr))
        {
            Error(L"Could not find class: %s\n", nameAlloc.GetData());

            if (hr != E_INVALIDARG)
                ReportError(hr);

            goto error;
        }
        unsigned int subTyargCount = 0;
        ICorDebugType *subTyargArray[256];

        // Now look for suffixes to the name, giving further type arguments
        //
        _ASSERTE(pClass != NULL);


        // Skip white space
        while (*p != L'\0' && iswspace(*p))
            p++;

        switch (*p)
        {
        case L'<':
            {
                p++;
                hr =
                    DebuggerShell::EvaluateTypeExpressions(p,
                                                           context,
                                                           subTyargArray,
                                                           &subTyargCount,
                                                           &p);
                if (FAILED(hr))
                    goto error;
                break;
            }

        case L'[':
            {
                Error(L"Syntax error, CORDBG does not yet support arrays as type expressions: %s.\n", exp);
                goto error;
            }
        }

        // We've got the class and the type parameters.  Note the
        // Debug API lets us use E_T_CLASS when it is not known if the
        // class is a value type or not.  This will be worked out during the Eval.
        ICorDebugClass2* pClass2 = NULL;
        hr = pClass->QueryInterface(IID_ICorDebugClass2,
                                    reinterpret_cast< void ** >(&pClass2));
        if (FAILED(hr))
        {
            goto error;
        }
        ICorDebugType *itype;
        pClass2->GetParameterizedType(ELEMENT_TYPE_CLASS, subTyargCount, subTyargArray, &itype);
        pClass2->Release();
        tyargArray[*pTyargCount] = itype;
        (*pTyargCount)++;

        // OK, we've done a type, now see if there's more to do
        switch (*p)
        {
        case L',':
            {
                p++;
                break;
            }
        case L'>':
            {
                p++;
                goto done;
            }
        default:
            Error(L"syntax error, unrecognized character in type \'%c\'.\n", *p);
            goto error;

        }
    }


done:
    *afterParse = p;
    return S_OK;

error:
    if (pClass != NULL)
      RELEASE(pClass);
    return E_FAIL;

}



// Given a context, get the module that we're currently in.
DebuggerModule * GetCurrentModule(ICorDebugILFrame *context)
{
    ICorDebugCode *icode = NULL;
    HRESULT hr;
    ICorDebugModule * im = NULL;
    DebuggerModule *m = NULL;

    // We can get the current module from the current context.
    // context --> Code --> function --> module
    if (context == NULL)
        goto ErrCantGetModule;

    hr = context->GetCode(&icode);

    if (FAILED(hr))
    {
        _ASSERTE(icode == NULL);
        goto ErrCantGetModule;
    }

    ICorDebugFunction *ifunction;
    hr = icode->GetFunction(&ifunction);

    RELEASE(icode);

    if (FAILED(hr))
    {
        goto ErrCantGetModule;
    }

    ifunction->GetModule(&im);

    RELEASE(ifunction);

    // Now we must convert from a ICorDebugModule to a Debugger Module.
    // do this via a reverse lookup in our module list.
    HASHFIND find;
    for (m = (DebuggerModule*) g_pShell->m_modules.FindFirst(&find);
         m != NULL;
         m = (DebuggerModule*) g_pShell->m_modules.FindNext(&find))
    {
        // GetICorDebugModule() does not addref, so we don't need to release.
        if (m->GetICorDebugModule() == im)
            break;
    }

    _ASSERTE(m != NULL); // should have been in our list.

    im->Release();

    return m;

ErrCantGetModule:
    // If we passed a 'silently' flag in here, we would know whether or not
    // to print this. But since we can call this function on garbage and it
    // has to fail silently, we can't print error messages.
    //if (false)
    //    g_pShell->Write(L"No current module. Specify the full module name.");
    return NULL;

}


// GetAppDomainFromSymbolName
// This has similar semantics to GetModuleFromSymbolName.
// Given a fully qualified name, this finds the (optional) appdomain.
// String format expects the AD friendly name to be enclosed in square brackets:
// ex:
//      [DefaultDomain]thing.dll!Foo::Bar
// Returns:
// S_OK - found an appdomain name, *ppAppDomain points to the AD, *ppUnqualifiedName points
//        to the string after the AD name
//
// S_FALSE - no appdomain name explicitly set. *ppAppDomain set to NULL, *ppUnqualifiedName set to pFullName
//
// E_FAIL - AD name doesn't exist,
// E_INVALIDARG - some parsing error
//
HRESULT GetAppDomainFromSymbolName(
    const WCHAR * pFullName,
    ICorDebugAppDomain ** ppAppDomain,
    const WCHAR ** ppUnqualifiedName)
{
    _ASSERTE(pFullName != NULL);
    _ASSERTE(ppAppDomain != NULL);

    // Set outparams to some sane default
    *ppAppDomain = NULL;
    *ppUnqualifiedName = pFullName;

    // AD names are enclosed in square brackets. Do we have an AD name?
    if (pFullName[0] != '[')
    {
        // No AD name. Out params are already set for us.
        return S_FALSE;
    }

    // Find the closing backet so we know what the end of the AD name is.
    const WCHAR * pClose = wcschr(pFullName, ']');
    if (pClose == NULL)
    {
        // If we have an openeing, no closing bracket. That's a parsing error.
        return E_INVALIDARG;
    }

    const WCHAR * pADName = &pFullName[1];
    const int cNameLen = (int) (pClose - pADName);

    DebuggerString pBuffer;
    if (!pBuffer.Allocate(cNameLen))
    {
        return E_OUTOFMEMORY;
    }

    // Now loop through known ADs and see if the string name matches.
    ICorDebugAppDomain * pAppDomainCheck = NULL;
    ICorDebugAppDomainEnum *pADEnum = NULL;
    HRESULT hr = g_pShell->m_currentProcess->EnumerateAppDomains (&pADEnum);
    ULONG cCount;

    if (pADEnum != NULL)
    {
        hr = pADEnum->Next(1, &pAppDomainCheck, &cCount);

        while (SUCCEEDED(hr) && (pAppDomainCheck != NULL))
        {
            // Does this AD's friendly name match ours?
            // Since we have a name that we're matching against, we only have to compare against a known size.
            ULONG32 cActualLen;
            hr = pAppDomainCheck->GetName(cNameLen + 1, &cActualLen, pBuffer.GetData());
            if (SUCCEEDED(hr))
            {
                if (cActualLen == (ULONG32)(cNameLen + 1))
                {
                    if (wcsncmp(pBuffer, pADName, cNameLen) == 0)
                    {
                        // Match found!
                        *ppAppDomain = pAppDomainCheck;
                        pAppDomainCheck->AddRef();

                        *ppUnqualifiedName = pClose + 1;

                        RELEASE(pAppDomainCheck);
                        break;
                    }
                }
            }

            // Next
            RELEASE(pAppDomainCheck);
            hr = pADEnum->Next(1, &pAppDomainCheck, &cCount);
        }

        pADEnum->Release();
        pADEnum = NULL;
    }


    if (*ppAppDomain)
    {
        return S_OK;
    }

    return E_FAIL;


}

// GetModuleFromSymbolName
//
// Description
//  Extracts the module name from a fully qualified symbol name and returns the DebuggerModule with that name
//
// Parameters
//  fullyQualifiedName -- a string of the form "module!symbol"
//  ppUnqualifiedName -- [out] a pointer to the beginning of the symbol (in the above example, "symbol")
//  ppModule -- [out] the module specified in fullyQualifiedName
//
// Returns
//  S_OK-- the module specified in name
//          Example:    GetModuleFromSymbolName(L"mscorlib!System.Console.WriteLine", &module, &symbolName);
//                      module points to DebuggerModule object associated with mscorlib.dll
//                      symbolName points to "System.Console.WriteLine"
//  S_FALSE -- if no module name is explicitly specified
//          Example:    GetModuleFromSymbolName(L"somefunction", &module, &symbolName);
//                      returns S_FALSE
//                      module points to NULL
//                      symbolName points to "somefunction"
//  E_FAIL  -- if the module doesn't exist
//          Example:    GetModuleFromSymbolName(L"nonexistentmodule!System.Console.WriteLine", NULL, NULL);
//                      returns E_FAIL
//
// Exceptions
//  None
//
HRESULT GetModuleFromSymbolName(const WCHAR * fullyQualifiedName, DebuggerModule** ppModule,
                                const WCHAR** ppUnqualifiedName)
{


    WCHAR szModName [MAX_PATH];
    ModuleSearchList MSL;

    // extract the module name, of the form:
    // <module>!<symbol>
    // <symbol>
    const WCHAR* const pSplit = wcschr(fullyQualifiedName, L'!');
    if (pSplit == NULL)
    {
        // no module name (fullyQualifiedName looks like "func")
        if (NULL != ppUnqualifiedName)
        {
            *ppUnqualifiedName = fullyQualifiedName;
        }
        return S_FALSE;
    }

    if (pSplit == fullyQualifiedName)
    {
        // no module name (fullyQualifiedName looks like "!func")
        if (NULL != ppUnqualifiedName)
        {
            *ppUnqualifiedName = pSplit+1;
        }
        return E_FAIL;
    }



    // normal case (fullyQualifiedName looks like "module!func")

    // copy out the module name
    const int iLength = (int)(pSplit - fullyQualifiedName);
    wcsncpy(szModName, fullyQualifiedName, iLength);
    szModName[iLength] = L'\0';


    // We know there's some module name at this point, so we need to update that
    // field:
    //
    /*
     * FIXME :
     * nickbe 01/14/2003 04:00:04k
     *
     * Fix for 18026
     */
    if (NULL != ppUnqualifiedName)
    {
        *ppUnqualifiedName = pSplit+1;
    }

    // start searching for the given module in our list of modules. To do this, we'll
    // need to do some funky stuff depending on if the user supplied a name with
    // extension. as he may be supplying a name with extension but that extension
    // not being the 'real' one: eg, for symbols in Dot.Net.dll he may be supplying
    // Dot.Net and therefore we cant just blindly say he's searching for Dot.Net, we'll
    // have to search for Dot.Net.*
    WCHAR        searchName[MAX_PATH];
    WCHAR        searchExt[64];

    _wsplitpath(szModName, NULL, NULL, searchName, searchExt);
    wcscat(searchName, searchExt);

    // walk the list of modules looking for the one which
    // matches the given module name
    HASHFIND find;
    DebuggerModule *m;
    for (m = (DebuggerModule*) g_pShell->m_modules.FindFirst(&find);
         m != NULL;
         m = (DebuggerModule*) g_pShell->m_modules.FindNext(&find))
    {
        WCHAR *pszModName = m->GetName();
        if (pszModName == NULL)
        {
            pszModName = L"<UnknownName>";
        }

        WCHAR        moduleName[_MAX_FNAME];
        WCHAR        moduleExt[_MAX_EXT];

        _wsplitpath(pszModName, NULL, NULL, moduleName, moduleExt);


        if (0 != wcslen(moduleExt))
        {
            // try and match the name with the extension
            WCHAR        fullModuleName[_MAX_FNAME];
            wcscpy(fullModuleName, moduleName);
            wcscat(fullModuleName, moduleExt);

            if (0 == _wcsicmp(fullModuleName, searchName))
            {
                if (NULL != ppModule)
                {
                    *ppModule = m;
                }
                return S_OK;
            }
        }

        // Check against name without extension
        if (0 == _wcsicmp(moduleName, searchName))
        {
            if (NULL != ppModule)
            {
                *ppModule = m;
            }
            return S_OK;
        }
    }

    return E_FAIL;
}

// Symbols may have optional qualifiers in front. Example:
//  [AppDomain]module!Symbol  <-- fully qualified
//  module!Symbol  <-- just the module, may be in any appdomain
//  [AppDomain]Symbol <-- just the AD, may be in any module
//  Symbol <-- really ambigious
//
// This may be a partial context; ie either/both of our module / AppDomain
// may be null.
// The only difference between a context and a DebuggerModule is that the context
// may be partially filled out (ie, only an AppDomain).
class SymbolContext
{
public:
    SymbolContext()
    {
        Set(NULL, NULL);
    };
    SymbolContext(DebuggerModule * pModule, ICorDebugAppDomain * pAppDomain)
    {
        Set(pModule, pAppDomain);
    }

    void Set(DebuggerModule * pModule, ICorDebugAppDomain * pAppDomain)
    {
        m_pModule = pModule;
        m_pAppDomain = pAppDomain;
    }

    DebuggerModule * GetModule() {  return m_pModule; }
    ICorDebugAppDomain * GetAppDomain() { return m_pAppDomain; }

    // Return true if the debugger module could be this symbol context.
    // (this the more ambigious this context is, the higher a chance of true)
    bool Match(DebuggerModule * pModule);
protected:

    // We don't maintain a reference count to this.
    ICorDebugAppDomain * m_pAppDomain;

    DebuggerModule * m_pModule;
};

// Return true if the debugger module _could_ be in this symbol context.
// Thus the more vague the context, the higher a chance of returning true.
bool SymbolContext::Match(DebuggerModule * pModule)
{
    _ASSERTE(pModule != NULL);

    if (m_pModule != NULL)
    {
        // If we have a module, then we can do a very strict match
        return (pModule == m_pModule);
    }

    // Now look for partial matches
    if (m_pAppDomain != NULL)
    {
        return (m_pAppDomain == pModule->GetIAppDomain());
    }

    // No AppDomain, No module, this could match anything!
    return true;
}

// Convenience function to get the context for a symbol.
// Given a potential full name, gets the optioanl qualifiers (AppDomain & Module)
// and returns the rest of the string.
//
// Symbols may have optional qualifiers in front. Example:
//  [AppDomain]module!Symbol  <-- fully qualified
//  module!Symbol  <-- just the module, may be in any appdomain
//  [AppDomain]Symbol <-- just the AD, may be in any module
//  Symbol <-- really ambigious

// Returns E_FAIL if there's any error (parsing, module / AD not found). No outparams
//     are valid in this case, *pSymbol = pFullName
// Returns S_OK if no error.
HRESULT GetContextFromSymbolName(
    const WCHAR * pFullName,
    SymbolContext * pCtx,
    const WCHAR ** pSymbol)
{
    HRESULT hr = S_OK;

    ICorDebugAppDomain * pAppDomain = NULL;
    DebuggerModule * pModule = NULL;

    // This will AddRef the AD
    GetAppDomainFromSymbolName(pFullName, &pAppDomain, pSymbol);
    if (FAILED(hr))
        goto Exit;

    hr = GetModuleFromSymbolName(*pSymbol, &pModule, pSymbol);
    if (FAILED(hr))
        goto Exit;


Exit:
    if (FAILED(hr))
    {
        *pSymbol = pFullName;
    } else {
        // On success, set the ctx w/ whatever information we have.
        pCtx->Set(pModule, pAppDomain);
    }

    // Release the AD
    if (pAppDomain != NULL)
    {
        pAppDomain->Release();
    }
    return hr;
}


// Given the string name of a global variable (without a prefixed module name),
// and a module, lookup the var in the module and return the ICorDebugValue.
// Return NULL if not found.
ICorDebugValue * LookupGlobalVarInModule(const WCHAR* szGlobalVarName, DebuggerModule * m)
{
    ICorDebugValue * piValue = NULL;

    MAKE_UTF8PTR_FROMWIDE (utf8VarName, szGlobalVarName);

    // Now look for the symbol name in the module.
    // Enumerate all the fields in the module and check for an exact match.
    IMetaDataImport * pIMetaDataImport = m->GetMetaData();

    HCORENUM phEnum = 0;
    mdMethodDef rTokens[100];
    ULONG count;
    HRESULT hr;
    MDUTF8CSTR name8;
    MDUTF8STR  u_name;
    MDUTF8STR  szMDName;
    ULONG i;

    u_name = new char[MAX_CLASSNAME_LENGTH];

    do
    {
        hr = pIMetaDataImport->EnumFields(&phEnum, NULL, &rTokens[0], 100, &count);

        if (!SUCCEEDED(hr))
        {
            g_pShell->ReportError(hr);
            goto ErrExit;
        }

        for (i = 0; i < count; i++)
        {
            hr = pIMetaDataImport->GetNameFromToken(rTokens[i], &name8);

            if (name8 == NULL)
                continue;

            szMDName = (MDUTF8STR) name8;

            if (!strcmp (szMDName, utf8VarName))
            {
                ICorDebugModule *mod = m->GetICorDebugModule();

                _ASSERTE( mod != NULL );

                hr = mod->GetGlobalVariableValue(rTokens[i], &piValue);
                if (!FAILED(hr))
                {
                    // We found a match!
                    goto ErrExit;
                }

            }

        } // end for-loop
    }
    while (count > 0);

ErrExit:
    delete [] u_name;

    return piValue;
}

// Check if the name matches the name for a global.
// Return a matching ICorDebugValue if found, else return null;
// A global name may be of the form:
// <module name>!<variable name> - uses the explicit module
// <variable name> - uses the module we're currently in (based off context).
ICorDebugValue * CheckForGlobal(const WCHAR *szGlobalVar, ICorDebugILFrame *context)
{
    _ASSERTE(szGlobalVar != NULL);
    _ASSERTE(wcslen(szGlobalVar) > 0);

    ICorDebugValue * piValue = NULL;

    // Get the module.
    DebuggerModule * m = NULL;
    const WCHAR* symbolName;

    HRESULT hr = GetModuleFromSymbolName(szGlobalVar, &m, &symbolName); // lookup based off explicit name.
    if (FAILED(hr))
    {
        // module no found
        return NULL;
    }
    if (S_FALSE == hr)
    {
        m = GetCurrentModule(context); // no explicit module, so use the current one
    }

    if (NULL == m)
    {
        return NULL;
    }

    _ASSERTE(SUCCEEDED(hr));

    _ASSERTE(NULL != m);
    piValue = LookupGlobalVarInModule(symbolName, m);

    return piValue;
}

// Check if the name matches a standard generated name for parameters/locals
HRESULT CheckForGeneratedName(bool fVar,
                              ICorDebugILFrame *context,
                              __in_z WCHAR *name,
                              ICorDebugValue **ppiRet)
{
    WCHAR *wszVarType;

    if (fVar == true)
        wszVarType = L"var";
    else
        wszVarType = L"arg";

    if (_wcsnicmp( name, wszVarType, wcslen(wszVarType))==0)
    {
        //extract numeric & go looking for it.
        WCHAR *wszVal = (WCHAR*)(name + wcslen(wszVarType));
        WCHAR *wszStop = NULL;
        if (wcslen(wszVal)==0 )
            return E_FAIL;

        long number = wcstoul(wszVal, &wszStop, 10);
        if (fVar == true)
            return context->GetLocalVariable(number, ppiRet);
        else
            return context->GetArgument(number, ppiRet);
    }

    return E_FAIL;
}

// Caller must release the ICorDebugValue we return.
ICorDebugValue *DebuggerShell::EvaluateName(const WCHAR *name,
                                            ICorDebugILFrame *context,
                                            bool *unavailable)
{
    HRESULT hr;
    ICorDebugValue* piRet = NULL;
    unsigned int i;
    unsigned int argCount;
    NamedHandle *pNamedHandle = NULL;

    *unavailable = false;

    ReleaseHolder<ICorDebugCode> icode;
    ReleaseHolder<ICorDebugFunction> ifunction;

    // At times, it may be reasonable to have no current managed frame
    // but still want to attempt to display some pseudo-variables. So
    // if we don't have a context, skip most of the work.
    if (context == NULL)
        goto NoContext;

    hr = context->GetCode(&icode);

    if (FAILED(hr))
    {
        g_pShell->ReportError(hr);
        return (NULL);
    }


    hr = icode->GetFunction(&ifunction);

    if (FAILED(hr))
    {
        g_pShell->ReportError(hr);
        return (NULL);
    }

    DebuggerFunction *function;
    function = DebuggerFunction::FromCorDebug(ifunction);
    _ASSERTE(function != NULL);

    //
    // Look for local variable.
    //

    ULONG32 ip;
    CorDebugMappingResult mappingResult;
    context->GetIP(&ip, &mappingResult);

    DebuggerVariable *localVars;
    localVars = NULL;
    unsigned int localVarCount;

    function->GetActiveLocalVars(ip, &localVars, &localVarCount);
    _ASSERTE((localVarCount == 0 && localVars == NULL) ||
             (localVarCount > 0 && localVars != NULL));

    for (i = 0; i < localVarCount; i++)
    {
        DebuggerVariable* pVar = &(localVars[i]);
        _ASSERTE(pVar && (pVar->m_name != NULL));

        if (wcscmp(name, pVar->m_name) == 0)
        {
            hr = context->GetLocalVariable(pVar->m_varNumber, &piRet);

            if (FAILED(hr))
            {
                *unavailable = true;
                delete [] localVars;
                return (NULL);
            }
            else
            {
                delete [] localVars;
                return (piRet);
            }
        }
    }

    delete [] localVars;

    //
    // Look for an argument
    //
    for (i = 0, argCount = function->GetArgumentCount(); i < argCount; i++)
    {
        DebuggerVarInfo* arg = function->GetArgumentAt(i);

        if (arg != NULL && arg->GetName() != NULL)
        {
            MAKE_WIDEPTR_FROMUTF8(wArgName, arg->GetName());

            if (wcscmp(name, wArgName) == 0)
            {
                hr = context->GetArgument(arg->varNumber, &piRet);

                if (FAILED(hr))
                {
                    *unavailable = true;
                    return (NULL);
                }
                else
                    return (piRet);
            }
        }
    }

    //
    // Look for globals
    //
    piRet = CheckForGlobal((WCHAR*) name, context);
    if (piRet != NULL)
        return piRet;


    // at this point we haven't found anything, so assume that
    // the user simply wants to see the nth arg or var.
    // NOTE that this looks the same as what's printed out when
    // we don't have any debugging metadata for the variables
    if ( !FAILED(CheckForGeneratedName( true, context, (WCHAR*)name, &piRet)))
    {
        return piRet;
    }

    if ( !FAILED(CheckForGeneratedName( false, context, (WCHAR*)name, &piRet)))
    {
        return piRet;
    }

NoContext:
    // Do they want to see the result of the last func eval?
    if (!_wcsicmp(name, L"$result"))
    {
        if (m_currentThread != NULL)
        {
            // Grab our managed thread object.
            DebuggerManagedThread *dmt =
                GetManagedDebuggerThread(m_currentThread);
            if (dmt == NULL)
            {
                _ASSERTE(!"shoud not have come here!");
                return NULL;
            }

            // Is there an eval to get a result from?
            if (dmt->m_lastFuncEval)
            {
                hr = dmt->m_lastFuncEval->GetResult(&piRet);

                if (SUCCEEDED(hr))
                    return piRet;
            }
        }
    }

    // Do they want to see the thread object?
    if (!_wcsicmp(name, L"$thread"))
    {
        if (m_currentThread != NULL)
        {
            // Grab our managed thread object.
            hr = m_currentThread->GetObject (&piRet);

            if (SUCCEEDED(hr))
            {
                return piRet;
            }
        }
    }

    // Do they want to see the last exception on this thread?
    if (!_wcsicmp(name, L"$exception"))
    {
        if (m_currentThread != NULL)
        {
            hr = m_currentThread->GetCurrentException(&piRet);

            if (SUCCEEDED(hr))
                return piRet;
        }
    }


    // Now see if we can bind to any known handles
    pNamedHandle = FindHandleWithName((WCHAR *)name);
    if (pNamedHandle)
    {
        // caller will release on the return ICorDebugValue. So make sure we add ref it.
        pNamedHandle->m_pHandle->AddRef();
        return pNamedHandle->m_pHandle;
    }

    return (NULL);
}

//
// Strip all references off of the given value. This simply
// dereferences through references until it hits a non-reference
// value.
//
// ppValue is an in-out parameter. 
// If this successfully derefences:
//   - this will release the initial value coming in,
//   - *ppValue will be set to the dereffed object, and it will have 1 extra addref (to match
//     the release we made on the original value). 
//
// If this can't deref:
//   - *ppValue is unchanged
//
// If this derefs to a null value, 
//   - *ppValue is released and set to NULL.
HRESULT DebuggerShell::StripReferences(ICorDebugValue **ppValue,
                                       bool printAsYouGo)
{
    HRESULT hr = S_OK;

    while (TRUE)
    {
        ReleaseHolder<ICorDebugReferenceValue> reference;
        hr = (*ppValue)->QueryInterface(IID_ICorDebugReferenceValue,
                                        (void **) &reference);

        if (FAILED(hr))
        {
            hr = S_OK;
            break;
        }

        // Check for NULL
        BOOL isNull;
        hr = reference->IsNull(&isNull);

        if (FAILED(hr))
        {
            RELEASE((*ppValue));
            *ppValue = NULL;
            break;
        }

        if (isNull)
        {
            if (printAsYouGo)
                Write(L"<null>");

            RELEASE((*ppValue));
            *ppValue = NULL;

            break;
        }


        CORDB_ADDRESS realObjectPtr;
        hr = reference->GetValue(&realObjectPtr);

        if (FAILED(hr))
        {
            RELEASE((*ppValue));
            *ppValue = NULL;

            break;
        }

        // ref: =1 (no change)

        // Dereference the thing...
        ReleaseHolder<ICorDebugValue> newValue;
        hr = reference->Dereference(&newValue);

        // newValue's ref = 1 (out-param).

        if (hr != S_OK)
        {
            if (printAsYouGo)
                if (hr == CORDBG_E_BAD_REFERENCE_VALUE)
                    Write(L"<invalid reference: 0x%p>", realObjectPtr);
                else if (hr == CORDBG_E_CLASS_NOT_LOADED)
                    Write(L"(0x%p) Note: CLR error -- referenced class "
                          L"not loaded.", realObjectPtr);
                else if (hr == CORDBG_S_VALUE_POINTS_TO_VOID)
                    Write(L"0x%p", realObjectPtr);

            RELEASE((*ppValue));
            *ppValue = NULL;
            break;
        }

        if (printAsYouGo)
            Write(L"(" LFMT_ADDR L") ", DBG_ADDR(realObjectPtr));


        RELEASE((*ppValue));
        *ppValue = newValue;
        (*ppValue)->AddRef();
    }

    return hr;
}


#define GET_VALUE_DATA(pData, size, icdvalue)                   \
    _ASSERTE(icdvalue);                                         \
    ICorDebugGenericValue *__gv##icdvalue;                      \
    HRESULT __hr##icdvalue = icdvalue->QueryInterface(          \
                               IID_ICorDebugGenericValue,       \
                               (void**) &__gv##icdvalue);       \
    if (FAILED(__hr##icdvalue))                                 \
    {                                                           \
        g_pShell->ReportError(__hr##icdvalue);                  \
        goto exit;                                              \
    }                                                           \
    ULONG32 size;                                               \
    __hr##icdvalue = __gv##icdvalue->GetSize(&size);            \
    if (FAILED(__hr##icdvalue))                                 \
    {                                                           \
        g_pShell->ReportError(__hr##icdvalue);                  \
        RELEASE(__gv##icdvalue);                                \
        goto exit;                                              \
    }                                                           \
    void* pData = (void*) _alloca(size);                        \
    __hr##icdvalue = __gv##icdvalue->GetValue(pData);           \
    if (FAILED(__hr##icdvalue))                                 \
    {                                                           \
        g_pShell->ReportError(__hr##icdvalue);                  \
        RELEASE(__gv##icdvalue);                                \
        goto exit;                                              \
    }                                                           \
    RELEASE(__gv##icdvalue);

//
// Print a variable. There are a lot of options here to handle lots of
// different kinds of variables. If subfieldName is set, then it is a
// field within an object to be printed. The indent is used to keep
// indenting proper for recursive calls, and expandObjects allows you
// to specify wether or not you want the fields of an object printed.
//
// !! This will release the ivalue parameter. If caller wants to keep that
// parameter alive, it will need to addref it 1 extra time before calling this.
void DebuggerShell::PrintVariable(const WCHAR *name,
                                  ICorDebugValue * &ivalue,
                                  unsigned int indent,
                                  BOOL expandObjects)
{
    HRESULT hr;

    _ASSERTE(ivalue);

    ReleaseHolder<ICorDebugHandleValue> pHandleValue;

    // Print the variable's name first.
    PrintVarName(name);

    ivalue->QueryInterface(IID_ICorDebugHandleValue, (void **)&pHandleValue);

    if (pHandleValue)
    {
        CorDebugHandleType    typ;
        ULONG32                 cSize;

        // print additional info for Handle
        if (SUCCEEDED(pHandleValue->GetHandleType(&typ)))
        {
            if (typ == HANDLE_WEAK_TRACK_RESURRECTION)
            {
                g_pShell->Write(L"(weak handle, ");
            }
            else
            {
                _ASSERTE(typ == HANDLE_STRONG);
                g_pShell->Write(L"(strong handle, ");
            }
        }
        if (SUCCEEDED(pHandleValue->GetSize(&cSize)))
        {
            g_pShell->Write(L"Size: %d) ", cSize);
        }
    }



    // Strip off any reference values before the real value
    // automatically.  Note: this will release the original
    // ICorDebugValue if it is actually dereferenced for us.
    hr = StripReferences(&ivalue, true);

    if (FAILED(hr) && !((hr == CORDBG_E_BAD_REFERENCE_VALUE) ||
                        (hr == CORDBG_E_CLASS_NOT_LOADED) ||
                        (hr == CORDBG_S_VALUE_POINTS_TO_VOID)))
    {
        g_pShell->ReportError(hr);
        goto exit;
    }

    if ((ivalue == NULL) || (hr != S_OK))
        return;

    // Grab the element type.
    CorElementType type;
    hr = ivalue->GetType(&type);

    if (FAILED(hr))
    {
        g_pShell->ReportError(hr);
        goto exit;
    }

    // Basic types are all printed pretty much the same. See the macro
    // GET_VALUE_DATA for some of the details.
    switch (type)
    {
    case ELEMENT_TYPE_BOOLEAN:
        {
            GET_VALUE_DATA(b, bSize, ivalue);
            _ASSERTE(bSize == sizeof(BYTE));
            Write(L"%s", (*((BYTE*)b) == FALSE) ? L"false" : L"true");
            break;
        }

    case ELEMENT_TYPE_CHAR:
        {
            GET_VALUE_DATA(ch, chSize, ivalue);
            _ASSERTE(chSize == sizeof(WCHAR));
            if ( m_rgfActiveModes & DSM_DISPLAY_REGISTERS_AS_HEX)
                Write( L"0x%.2x", *((WCHAR*) ch));
            else
                Write(L"'%c'", *((WCHAR*) ch));
            break;
        }

    case ELEMENT_TYPE_I1:
        {
            GET_VALUE_DATA(i1, i1Size, ivalue);
            _ASSERTE(i1Size == sizeof(BYTE));
            if ( m_rgfActiveModes & DSM_DISPLAY_REGISTERS_AS_HEX)
                Write( L"0x%.2x", *((BYTE*) i1) );
            else
                Write(L"'%d'", *((BYTE*) i1) );
            break;

        }

    case ELEMENT_TYPE_U1:
        {
            GET_VALUE_DATA(ui1, ui1Size, ivalue);
            _ASSERTE(ui1Size == sizeof(BYTE));
            if ( m_rgfActiveModes & DSM_DISPLAY_REGISTERS_AS_HEX)
                Write( L"0x%.2x",  *((BYTE*) ui1));
            else
                Write(L"'%d",  *((BYTE*) ui1));
            break;
        }

    case ELEMENT_TYPE_I2:
        {
            GET_VALUE_DATA(i2, i2Size, ivalue);
            _ASSERTE(i2Size == sizeof(short));
            if ( m_rgfActiveModes & DSM_DISPLAY_REGISTERS_AS_HEX)
                Write( L"0x%.4x", *((short*) i2) );
            else
                Write(L"%d", *((short*) i2));
            break;
        }

    case ELEMENT_TYPE_U2:
        {
            GET_VALUE_DATA(ui2, ui2Size, ivalue);
            _ASSERTE(ui2Size == sizeof(unsigned short));
            if ( m_rgfActiveModes & DSM_DISPLAY_REGISTERS_AS_HEX)
                Write( L"0x%.4x", *((unsigned short*) ui2) );
            else
                Write(L"%d", *((unsigned short*) ui2));
            break;
        }

    case ELEMENT_TYPE_I4:
    case ELEMENT_TYPE_I:
        {
            GET_VALUE_DATA(i4, i4Size, ivalue);
            _ASSERTE(i4Size == sizeof(int));
            if ( m_rgfActiveModes & DSM_DISPLAY_REGISTERS_AS_HEX)
                Write( L"0x%.8x", *((int*) i4) );
            else
                Write(L"%d", *((int*) i4));
            break;
        }

    case ELEMENT_TYPE_U4:
    case ELEMENT_TYPE_U:
        {
            GET_VALUE_DATA(ui4, ui4Size, ivalue);
            _ASSERTE(ui4Size == sizeof(unsigned int));
            if ( m_rgfActiveModes & DSM_DISPLAY_REGISTERS_AS_HEX)
                Write( L"0x%.8x", *((unsigned int*) ui4) );
            else
                Write(L"%d", *((unsigned int*) ui4));
            break;
        }

    case ELEMENT_TYPE_I8:
        {
            GET_VALUE_DATA(i8, i8Size, ivalue);
            _ASSERTE(i8Size == sizeof(__int64));
            if ( m_rgfActiveModes & DSM_DISPLAY_REGISTERS_AS_HEX)
                Write( L"0x%I64x", *((__int64*) i8) );
            else
                Write(L"%I64d", *((__int64*) i8));
            break;
        }

    case ELEMENT_TYPE_U8:
        {
            GET_VALUE_DATA(ui8, ui8Size, ivalue);
            _ASSERTE(ui8Size == sizeof(unsigned __int64));
            if ( m_rgfActiveModes & DSM_DISPLAY_REGISTERS_AS_HEX)
                Write( L"0x%I64x", *((unsigned __int64*) ui8) );
            else
                Write(L"%I64d", *((unsigned __int64*) ui8) );
            break;
        }

    case ELEMENT_TYPE_R4:
        {
            GET_VALUE_DATA(f4, f4Size, ivalue);
            _ASSERTE(f4Size == sizeof(float));
            Write(L"%.16g", *((float*) f4));
            break;
        }

    case ELEMENT_TYPE_R8:
        {
            GET_VALUE_DATA(f8, f8Size, ivalue);
            _ASSERTE(f8Size == sizeof(double));
            Write(L"%.16g", *((double*) f8));
            break;
        }

    //
    //
    case ELEMENT_TYPE_CLASS:
    case ELEMENT_TYPE_OBJECT:
    case ELEMENT_TYPE_STRING:
    case ELEMENT_TYPE_SZARRAY:
    case ELEMENT_TYPE_ARRAY:
    case ELEMENT_TYPE_VALUETYPE:
        {
            // If we have a boxed object then unbox the little fella...
            ReleaseHolder<ICorDebugBoxValue> boxVal;

            if (SUCCEEDED(ivalue->QueryInterface(IID_ICorDebugBoxValue,
                                                 (void **) &boxVal)))
            {
                ICorDebugObjectValue *objVal;
                hr = boxVal->GetObject(&objVal);

                if (FAILED(hr))
                {
                    ReportError(hr);
                    break;
                }

                RELEASE(ivalue);

                // Replace the current value with the unboxed object.
                ivalue = objVal;

                Write(L"(boxed) ");
            }

            // Is this object a string object?
            ReleaseHolder<ICorDebugStringValue> istring;
            hr = ivalue->QueryInterface(IID_ICorDebugStringValue,
                                        (void**) &istring);

            // If it is a string, print it out.
            if (SUCCEEDED(hr))
            {
                PrintStringVar(istring, name, indent, expandObjects);
                break;
            }

            // Might be an array...
            ReleaseHolder<ICorDebugArrayValue> iarray;
            hr = ivalue->QueryInterface(IID_ICorDebugArrayValue,
                                        (void **) &iarray);

            if (SUCCEEDED(hr))
            {
                PrintArrayVar(iarray, name, indent, expandObjects);
                break;
            }

            // It had better be an object by this point...
            ReleaseHolder<ICorDebugObjectValue> iobject;
            hr = ivalue->QueryInterface(IID_ICorDebugObjectValue,
                                        (void **) &iobject);

            if (SUCCEEDED(hr))
            {
                PrintObjectVar(iobject, name, indent, expandObjects);
                break;
            }

            // Looks like we've got a bad object here...
            ReportError(hr);
            break;
        }

    case ELEMENT_TYPE_BYREF: // should never have a BYREF here.
    case ELEMENT_TYPE_PTR: // should never have a PTR here.
    case ELEMENT_TYPE_TYPEDBYREF: // should never have a REFANY here.
    default:
        Write(L"[unknown variable type 0x%x]", type);
    }

exit:
    // We intentionally release the caller's ivalue parameter.
    if (ivalue)
    {
        // ivalue might be NULL
        RELEASE(ivalue);
    }
    ivalue = NULL;
}

// Print a type when showing the value of a type
// variable.
void DebuggerShell::PrintType(const WCHAR *name,
                                      ICorDebugType *ivalue,
                                      unsigned int indent)
{
    HRESULT hr;

    // Print the variable's name first.
    PrintVarName(name);
    WCHAR className[MAX_CLASSNAME_LENGTH];
    ULONG classNameSize = 0;

    hr = GetTypeName(ivalue,MAX_CLASSNAME_LENGTH - 1,className, &classNameSize);
    if (FAILED(hr))
        goto exit;
    className[classNameSize] = L'\0';

    Write(L"%s", className);

exit:
    RELEASE(ivalue);
}

void DebuggerShell::PrintArrayVar(ICorDebugArrayValue *iarray,
                                  const WCHAR* name,
                                  unsigned int indent,
                                  BOOL expandObjects)
{
    HRESULT hr;
    ULONG32 *dims;
    ULONG32 *bases = NULL;
    unsigned int i;

    // Get the rank
    ULONG32 rank;
    hr = iarray->GetRank(&rank);

    if (FAILED(hr))
    {
        g_pShell->ReportError(hr);
        goto exit;
    }

    // Get the element count
    ULONG32 elementCount;
    hr = iarray->GetCount(&elementCount);

    if (FAILED(hr))
    {
        g_pShell->ReportError(hr);
        goto exit;
    }

    // Get the dimensions
    dims = (ULONG32*)_alloca(rank * sizeof(ULONG32));
    hr = iarray->GetDimensions(rank, dims);

    if (FAILED(hr))
    {
        g_pShell->ReportError(hr);
        goto exit;
    }

    Write(L"array with dims=");

    for (i = 0; i < rank; i++)
        Write(L"[%d]", dims[i]);

    // Does it have base indicies?
    BOOL hasBaseIndicies;
    hr = iarray->HasBaseIndicies(&hasBaseIndicies);

    if (FAILED(hr))
    {
        g_pShell->ReportError(hr);
        goto exit;
    }

    if (hasBaseIndicies)
    {
        bases = (ULONG32*)_alloca(rank * sizeof(ULONG32));
        hr = iarray->GetBaseIndicies(rank, bases);

        if (FAILED(hr))
        {
            g_pShell->ReportError(hr);
            goto exit;
        }

        Write(L", bases=");

        for (i = 0; i < rank; i++)
            Write(L"[%d]", bases[i]);
    }

    // Get the element type of the array
    CorElementType arrayType;
    hr = iarray->GetElementType(&arrayType);

    if (FAILED(hr))
    {
        g_pShell->ReportError(hr);
        goto exit;
    }

    // If desired, print out the contents of the array, if not void.
    if (arrayType != ELEMENT_TYPE_VOID && expandObjects && rank == 1)
    {
        // Get and print each element of the array
        for (i = 0; i < elementCount; i++)
        {
            Write(L"\n");
            PrintIndent(indent + 1);

            if (bases != NULL)
                Write(L"%s[%d] = ", name, i + bases[0]);
            else
                Write(L"%s[%d] = ", name, i);

            ICorDebugValue *ielement;
            hr = iarray->GetElementAtPosition((ULONG32)i, &ielement);

            if (FAILED(hr))
            {
                g_pShell->ReportError(hr);
                goto exit;
            }

            PrintVariable(NULL, ielement, indent + 1, FALSE);
        }
    }

exit:
    ;
}

void DebuggerShell::PrintStringVar(ICorDebugStringValue *istring,
                                   const WCHAR* name,
                                   unsigned int indent,
                                   BOOL expandObjects)
{
    CQuickBytes sBuf;
    WCHAR *s = NULL;

    _ASSERTE(istring != NULL);

    // Get the string
    ULONG32 count;
    HRESULT hr = istring->GetLength(&count);

    if (FAILED(hr))
    {
        g_pShell->ReportError(hr);
        goto LExit;
   }

    s = (WCHAR*) sBuf.Alloc((count + 1) * sizeof(WCHAR));

    if (s == NULL)
    {
        g_pShell->Error(L"Couldn't allocate enough space for string!\n");
        goto LExit;
    }

    if (count > 0)
    {
        hr = istring->GetString(count, &count, s);

        if (FAILED(hr))
        {
            g_pShell->ReportError(hr);
            goto LExit;
        }
    }

    // Null terminate it
    s[count] = L'\0';

    // This will convert all embedded NULL's into spaces
    {
        WCHAR *pStart = &s[0];
        WCHAR *pEnd = &s[count];

        while (pStart != pEnd)
        {
            if (*pStart == L'\0')
            {
                *pStart = L' ';
            }

            pStart++;
        }
    }

    Write(L"\"");

    if (FAILED(Write(L"%s",s)))
        WriteBigString(s, count);

    Write(L"\"");

LExit:
    return;
}


HRESULT DebuggerShell::GetTypeName(ICorDebugClass *iclass,
                                   ULONG bufLength,
                                   __inout_ecount(bufLength) WCHAR* nameBuf,
                                   ULONG *nameSize)
{
    *nameSize = 0;
    DebuggerModule *dm;
    WCHAR *buf = nameBuf;
    ULONG bufRemaining = bufLength;

    // Get the module from this class
    ICorDebugModule *imodule;
    HRESULT hr = iclass->GetModule(&imodule);

    if (FAILED(hr))
    {
        g_pShell->ReportError(hr);
        goto exit;
    }

    dm = DebuggerModule::FromCorDebug(imodule);

    _ASSERTE(dm != NULL);
    RELEASE(imodule);

    // Get the class's token
    mdTypeDef tdClass;
    _ASSERTE(iclass != NULL);
    hr = iclass->GetToken(&tdClass);

    if (FAILED(hr))
    {
        g_pShell->ReportError(hr);
        goto exit;
    }

    // Get the class name
    ULONG       fetched;
    hr = dm->GetMetaData()->GetTypeDefProps(tdClass,
                                            buf, bufRemaining,
                                            &fetched,
                                            NULL, NULL);
    if (FAILED(hr))
        goto exit;

    bufRemaining -= (fetched - 1);
    buf += (fetched - 1);
    (*nameSize) += (fetched - 1);

   return S_OK;

exit:
   g_pShell->ReportError(hr);
   return hr;
}



HRESULT DebuggerShell::AddString(__in_z CHAR* s,
                                 ULONG bufLength,
                                 __inout_z __inout_ecount(bufLength) WCHAR* nameBuf,
                                 ULONG *nameSize)
{
    *nameSize = 0;
    ULONG bufRemaining = bufLength;
    for (; *s; s++) {
        if (bufRemaining) {
            *(nameBuf++) = *s;
            bufRemaining--;
            (*nameSize)++;
        }
    }
    return S_OK;
}

HRESULT DebuggerShell::GetTypeName(ICorDebugType *itype,
                                   ULONG bufLength,
                                   __inout_ecount(bufLength) WCHAR* nameBuf,
                                   ULONG *nameSize)
{
    *nameSize = 0;
    CorElementType ty;
    HRESULT hr = itype->GetType(&ty);
    WCHAR *buf = nameBuf;
    ULONG bufRemaining = bufLength;
    ULONG       fetched;
    ULONG totalTypeParams = 0;
    ICorDebugType *itypar = NULL;
    BOOL constructed = false;
    ULONG typeParamsInspected = 0;
    if (FAILED(hr))
    {
        g_pShell->ReportError(hr);
        return hr;
    }
    switch (ty)
    {
    case ELEMENT_TYPE_CLASS:
    case ELEMENT_TYPE_VALUETYPE:
        {
            ICorDebugClass *iclass;
            ICorDebugTypeEnum *e = NULL;
            hr = itype->GetClass(&iclass);
            if (FAILED(hr))
            {
                g_pShell->ReportError(hr);
                return hr;
            }
            hr = GetTypeName(iclass, bufRemaining, buf,&fetched);
            if (FAILED(hr))
                goto exit;
            buf += fetched;
            bufRemaining -= fetched;
            *nameSize += fetched;
            RELEASE(iclass);

            hr = itype->EnumerateTypeParameters( &e );
            if (FAILED(hr))
                goto exit;
                hr = e->GetCount(&totalTypeParams);

            ULONG count;
            for (hr = e->Next(1, &itypar, &count);
                SUCCEEDED(hr) && (count == 1) && (bufRemaining > 0);
                hr = e->Next(1, &itypar, &count), ++typeParamsInspected)
            {

                if (!constructed && bufRemaining > 0)
                {
                    *(buf++) = L'<';
                    bufRemaining--;
                    (*nameSize)++;
                    constructed = true;
                }
                else if (bufRemaining > 0)
                {
                    *(buf++) = L',';
                    bufRemaining--;
                    (*nameSize)++;
                }

                hr = GetTypeName(itypar, bufRemaining, buf, &fetched);
                RELEASE(itypar);
                if (FAILED(hr))
                    goto exit;
                bufRemaining -= fetched;
                buf += fetched;
                (*nameSize) += fetched;
            }
            if (constructed && bufRemaining > 0)
            {
                *(buf++) = L'>';
                bufRemaining--;
                (*nameSize)++;
            }
exit:
            if (e)
                RELEASE(e);
            if (FAILED(hr) && typeParamsInspected == totalTypeParams)
            {
                hr = S_OK;
            }
            return hr;
        }

    case ELEMENT_TYPE_ARRAY:
    case ELEMENT_TYPE_SZARRAY:
    case ELEMENT_TYPE_BYREF:
    case ELEMENT_TYPE_PTR:
        {
            hr = itype->GetFirstTypeParameter(&itypar);
            if (FAILED(hr))
            {
                g_pShell->ReportError(hr);
                return hr;
            }
            hr = GetTypeName(itypar, bufRemaining, buf, &fetched);
            bufRemaining -= fetched;
            buf += fetched;
            (*nameSize) += fetched;
            if (FAILED(hr))
            {
                g_pShell->ReportError(hr);
                return hr;
            }
            itypar->Release();
            switch (ty)
            {
            case ELEMENT_TYPE_ARRAY:
                AddString("[<rank not yet printed>]", bufRemaining, buf, &fetched);
                break;
            case ELEMENT_TYPE_SZARRAY:
                AddString("[]", bufRemaining, buf, &fetched);
                break;
            case ELEMENT_TYPE_BYREF:
                AddString("&", bufRemaining, buf, &fetched);
                break;
            case ELEMENT_TYPE_PTR:
                AddString("*", bufRemaining, buf, &fetched);
                break;
                default:
                    break;
            }
            bufRemaining -= fetched;
            buf += fetched;
            (*nameSize) += fetched;

            return hr;
        }
    case ELEMENT_TYPE_FNPTR: return AddString("*(...)", bufLength, nameBuf, nameSize);
    case ELEMENT_TYPE_VOID:     return AddString("System.Void", bufLength, nameBuf, nameSize);
    case ELEMENT_TYPE_BOOLEAN:  return AddString("System.Bool", bufLength, nameBuf, nameSize);
    case ELEMENT_TYPE_I1:       return AddString("System.SByte", bufLength, nameBuf, nameSize);
    case ELEMENT_TYPE_U1:       return AddString("System.Byte", bufLength, nameBuf, nameSize);
    case ELEMENT_TYPE_I2:       return AddString("System.Int16", bufLength, nameBuf, nameSize);
    case ELEMENT_TYPE_U2:       return AddString("System.UInt16", bufLength, nameBuf, nameSize);
    case ELEMENT_TYPE_CHAR:     return AddString("System.Char", bufLength, nameBuf, nameSize);
    case ELEMENT_TYPE_I4:       return AddString("System.Int32", bufLength, nameBuf, nameSize);
    case ELEMENT_TYPE_U4:       return AddString("System.UInt32", bufLength, nameBuf, nameSize);
    case ELEMENT_TYPE_I8:       return AddString("System.Int64", bufLength, nameBuf, nameSize);
    case ELEMENT_TYPE_U8:       return AddString("System.UInt64", bufLength, nameBuf, nameSize);
    case ELEMENT_TYPE_R4:       return AddString("System.Single", bufLength, nameBuf, nameSize);
    case ELEMENT_TYPE_R8:       return AddString("System.Double", bufLength, nameBuf, nameSize);
    case ELEMENT_TYPE_OBJECT:   return AddString("System.Object", bufLength, nameBuf, nameSize);
    case ELEMENT_TYPE_STRING:   return AddString("System.String", bufLength, nameBuf, nameSize);
    case ELEMENT_TYPE_I:        return AddString("System.IntPtr", bufLength, nameBuf, nameSize);
    case ELEMENT_TYPE_U:        return AddString("System.UIntPtr", bufLength, nameBuf, nameSize);
    default:
        _ASSERTE(!"unimplemented!");
    }

   return S_OK;

}




void DebuggerShell::PrintObjectVar(ICorDebugObjectValue *iobject,
                                   const WCHAR* name,
                                   unsigned int indent,
                                   BOOL expandObjects)
{
    HRESULT hr = S_OK;
    ReleaseHolder<ICorDebugType> itype;
    ULONG classNameSize = 0;

    _ASSERTE(iobject != NULL);

    // All values should support ICorDebugValue2
    ReleaseHolder<ICorDebugValue2> ivalue2;

    ReleaseHolder<ICorDebugObjectValue2> iobject2;
    hr = iobject->QueryInterface(IID_ICorDebugObjectValue2,
                                 reinterpret_cast< void** >(&iobject2));
    if (FAILED(hr))
    {        
        g_pShell->ReportError(hr);
        goto exit;
    }

    if (FAILED(hr = iobject->QueryInterface(IID_ICorDebugValue2,
                                            reinterpret_cast< void** >(&ivalue2))))
    {
        g_pShell->ReportError(hr);
        goto exit;
    }

    // Snag the object's class.
    hr = ivalue2->GetExactType(&itype);

    ivalue2.Clear();

    if (FAILED(hr))
    {
        g_pShell->ReportError(hr);
        goto exit;
    }

    WCHAR className[MAX_CLASSNAME_LENGTH];

    hr = GetTypeName(itype,MAX_CLASSNAME_LENGTH-1,className, &classNameSize);
    if (FAILED(hr))
    {
        Write(L"*Unknown Type*", className);
        g_pShell->ReportError(hr);
        goto exit;
    }
    className[classNameSize] = L'\0';

    Write(L"<%s>", className);

    // Print all the members of this object.
    if (expandObjects)
    {
        BOOL isValueClass = FALSE;

        hr = iobject->IsValueClass(&isValueClass);
        _ASSERTE(SUCCEEDED(hr));

        BOOL anyMembers = FALSE;
        BOOL isSuperClass = FALSE;

        // itype->AddRef(); // we call Release on every type up the chain, so add a reference here....

        do
        {
            DebuggerModule *dm = ModuleOfType(itype);
            mdTypeDef tdClass = TokenOfType(itype);

            HCORENUM fieldEnum = NULL;

            while (TRUE)
            {
                // Get the fields one at a time
                mdFieldDef field[1];
                ULONG numFields = 0;

                hr = dm->GetMetaData()->EnumFields(&fieldEnum,
                                                   tdClass, field, 1,
                                                   &numFields);

                // No fields left
                if (SUCCEEDED(hr) && (numFields == 0))
                    break;
                // Error
                else if (FAILED(hr))
                    break;

                // Get the field properties
                WCHAR fieldName[MAX_CLASSNAME_LENGTH];
                ULONG nameLen = 0;
                DWORD attr = 0;

                hr = dm->GetMetaData()->GetFieldProps(field[0],
                                                      NULL,
                                                      fieldName,
                                                      MAX_CLASSNAME_LENGTH,
                                                      &nameLen,
                                                      &attr,
                                                      NULL, NULL,
                                                      NULL, NULL, NULL);

                if (FAILED(hr))
                    break;

                // If it's not a static field
                if (((attr & fdStatic) == 0) ||
                    (m_rgfActiveModes & DSM_SHOW_STATICS_ON_PRINT))
                {
                    Write(L"\n");
                    PrintIndent(indent + 1);

                    if (isSuperClass &&
                        (m_rgfActiveModes & DSM_SHOW_SUPERCLASS_ON_PRINT))
                    {
                        // Print superclass field qualifiers in the
                        // syntax required to print them (i.e., use ::
                        // for the seperator in the namespace.
                        WCHAR *pc = className;

                        while (*pc != L'\0')
                        {
                            if (*pc == L'.')
                                Write(L"::");
                            else
                                Write(L"%c", *pc);

                            pc++;
                        }

                        Write(L"::");
                    }

                    ICorDebugValue *fieldValue;
                    ReleaseHolder<ICorDebugClass> iclass;
                    hr = itype->GetClass(&iclass);
                    _ASSERTE(SUCCEEDED(hr));

                    if (attr & fdStatic)
                    {
                        Write(L"<static> ");


                        // We'll let the user look at static fields as if
                        // they belong to objects.
                        hr = iclass->GetStaticFieldValue(field[0], NULL,
                                                         &fieldValue);
                    }
                    else
                    {
                        hr = iobject->GetFieldValue(iclass, field[0],
                                                    &fieldValue);
                    }
                    
                    if (FAILED(hr))
                    {
                        Write(L"%s -- ", fieldName);
                        this->ReportError(hr);
                    }
                    else
                    {
                        PrintVariable(fieldName, fieldValue, indent + 1, FALSE);
                        anyMembers = TRUE;
                    }
                }
            }

            // Release the field enumerator
            if (fieldEnum != NULL)
                dm->GetMetaData()->CloseEnum(fieldEnum);

            // Check for failure from within the loop...
            if (FAILED(hr))
            {
                ReportError(hr);
                goto exit;
            }
            ReleaseHolder<ICorDebugType> iparent;
            hr = itype->GetBase(&iparent); // sets itype to NULL at end
            if (FAILED(hr))
                break;

            itype.Assign(iparent);
            isSuperClass = TRUE;

        } while (itype != NULL);

        // If this object has no members, lets go ahead and see if it has a size. If it does, then we'll just dump the
        // raw memory.
        if (!anyMembers && isValueClass)
        {
            ULONG32 objSize = 0;

            hr = iobject->GetSize(&objSize);

            if (SUCCEEDED(hr) && (objSize > 0))
            {
                BYTE *objContents = new BYTE[objSize];

                if (objContents != NULL)
                {
                    ReleaseHolder<ICorDebugGenericValue> pgv;

                    hr = iobject->QueryInterface(IID_ICorDebugGenericValue, (void**)&pgv);

                    if (SUCCEEDED(hr))
                    {
                        hr = pgv->GetValue(objContents);

                        if (SUCCEEDED(hr))
                        {
                            Write(L"\nObject has no defined fields, but has a defined size of %d bytes.\n", objSize);
                            Write(L"Raw memory dump of object follows:\n");
                            DumpMemory(objContents, PTR_TO_CORDB_ADDRESS(objContents), objSize, 4, 4, FALSE);
                        }
                    }

                    delete [] objContents;
                }
            }
        }

        // In V1.0, we used to call Object::ToString() by calling GetManagedCopy().
        // Since GetManagedCopy is innately flawed, we can't call it, and so we don't
        // both w/ ToString. If we were really desperate, we could func-eval the ToString(),
        // but that's a risky thing to do (because we have to let the process run free while
        // waiting for the func-eval,and who knows what badness will happen then.
    }

exit:
    ;
}

//
// Given a class name, find the DebuggerModule that it is in and its
// mdTypeDef token.
//
HRESULT DebuggerShell::ResolveClassName(const WCHAR *className,
                                        DebuggerModule **pDM,
                                        mdTypeDef *pTD)
{
    HRESULT hr = S_OK;

    SymbolContext ctx;
    hr = GetContextFromSymbolName(className, &ctx, &className);
    if (FAILED(hr))
        return hr;


    // Find the class, by name and namespace, in any module we've loaded.
    HASHFIND find;
    DebuggerModule *m;

    for (m = (DebuggerModule*) g_pShell->m_modules.FindFirst(&find);
         m != NULL;
         m = (DebuggerModule*) g_pShell->m_modules.FindNext(&find))
    {
        if (!ctx.Match(m))
            continue;

        mdTypeDef td;
        hr = FindTypeDefByName(m, className, &td);

        if (SUCCEEDED(hr))
        {
            *pDM = m;
            *pTD = td;
            goto exit;
        }
    }

    hr = E_INVALIDARG;

exit:
    return hr;
}

//
// This will find a typedef in a module, even if its nested, so long
// as the name is specified correctly.
//
// If the typedef does not exist, the value stored in pTD is invalid,
// and the function returns a failing hresult
//
// This function assumes pTD is non-null
//
HRESULT DebuggerShell::FindTypeDefByName(DebuggerModule *m,
                                         const WCHAR *className,
                                         mdTypeDef *pTD)
{
    HRESULT hr = S_OK;

    _ASSERTE(NULL != pTD);

    // Fast-path case for normal lookup, e.g.
    //          "Namespace.ClassName"
    IMetaDataImport* pMetaData = m->GetMetaData();
    hr = pMetaData->FindTypeDefByName(className, mdTokenNil, pTD);

    if (SUCCEEDED(hr))
    {
        return hr;
    }

    // If we reach here, the simple class lookup has failed, but the
    // class might be an inner class of some sort, e.g.
    //          "Namespace.OuterClass+InnerClass"

    // we lookup a potential class in the context of its parent class.
    // Initially, there is no parent class
    mdTypeDef parent = mdTokenNil;

    size_t fullClassNameLen = wcslen(className);
    WCHAR* fullClassName = reinterpret_cast< WCHAR* >(_alloca(sizeof(WCHAR) *
                                                                    (fullClassNameLen+2)));
    WCHAR* unqualifiedClassName = reinterpret_cast< WCHAR* >(_alloca(sizeof(WCHAR) *
                                                                     (fullClassNameLen+2)));
    //
    // It's a pain to parse the string
    //          outermost+inner1+inner2
    // because the name of the last class "inner2" is terminated by a
    // null character instead of a +.
    // Since we're going to have to copy the string anyway, go ahead and add an
    // additional '+' at the end, like this:
    //          outermost+inner1+inner2+
    // Then chop up the string by replacing each + with \0
    //

    // use memcpy since we already know the length of the string
    memcpy(fullClassName, className, sizeof(WCHAR) * fullClassNameLen);
    fullClassName[fullClassNameLen] = L'+';
    fullClassName[fullClassNameLen+1] = L'\0';
    const WCHAR* search = fullClassName;
    const WCHAR* const lastNull = fullClassName + fullClassNameLen + 1;

    do
    {
        // split the class name up by non-escaped '+'s
        const WCHAR* nextDelimiter;
        WCHAR* out = unqualifiedClassName;

        do
        {
            nextDelimiter = search + wcscspn(search, L"+\\");
            _ASSERTE(static_cast< size_t >(nextDelimiter - fullClassName) < fullClassNameLen+1);

            memcpy(out, search, sizeof(WCHAR) * (nextDelimiter - search));
            out += (nextDelimiter - search);

            search = nextDelimiter + 1;
        } while ('+' != *nextDelimiter);

        _ASSERTE(static_cast< size_t >(out - unqualifiedClassName) < fullClassNameLen+1);

        *out = L'\0';
        hr = pMetaData->FindTypeDefByName(unqualifiedClassName, parent, pTD);

        if (FAILED(hr))
        {
            // go ahead and give up if the child class can't be found
            return hr;
        }

        // if a child class does exist, that class may be the
        // parent of another class. Move on to the next class name
        parent = *pTD;
    }
    while (search < lastNull);      // more class names exist

    _ASSERTE(SUCCEEDED(hr));
    return hr;
}

//
// Given a DebuggerModule and a mdTypeRef token, resolve it to
// whatever DebuggerModule and mdTypeDef token the ref is refering to.
//
HRESULT DebuggerShell::ResolveTypeRef(DebuggerModule *currentDM,
                                      mdTypeRef tr,
                                      DebuggerModule **pDM,
                                      mdTypeDef *pTD)
{
    _ASSERTE(TypeFromToken(tr) == mdtTypeRef);

    // Get the name of the type ref.
    WCHAR className[MAX_CLASSNAME_LENGTH];
    HRESULT hr = currentDM->GetMetaData()->GetTypeRefProps(tr,
                                                           NULL,
                                                           className,
                                                           MAX_CLASSNAME_LENGTH,
                                                           NULL);
    if (FAILED(hr))
        return hr;

    return ResolveClassName(className, pDM, pTD);
}

//
// Split a name in the form "ns::ns::ns::class::field" into
// "ns.ns.ns.class" and "field". The output params need to be delete
// []'d by the caller.
//
HRESULT _splitColonQualifiedFieldName(__in_z WCHAR *pWholeName,
                                      __out_z __deref_out_opt WCHAR **ppClassName,
                                      __out_z __deref_out_opt WCHAR **ppFieldName)
{
    HRESULT hr = S_OK;

    // We're gonna be kinda gross about some of the allocations here,
    // basically over allocating for both the classname and the
    // fieldname.
    int len = (int)wcslen(pWholeName);

    WCHAR *fn = NULL;
    WCHAR *cn = NULL;

    fn = new WCHAR[len+1];

    if (fn == NULL)
    {
        hr = E_OUTOFMEMORY;
        goto ErrExit;
    }

    cn = new WCHAR[len+1];

    if (cn == NULL)
    {
        hr = E_OUTOFMEMORY;
        goto ErrExit;
    }

    // Find the field name.
    WCHAR *lastColon;
    lastColon = wcsrchr(pWholeName, L':');

    if (lastColon)
    {
        // The field name is whatever is after the last colon.
        wcscpy(fn, lastColon + 1);

        // The class name is everything up to the last set of colons.
        WCHAR *tmp = pWholeName;
        WCHAR *newCn = cn;

        _ASSERTE(lastColon - 1 >= pWholeName);

        while (tmp < (lastColon - 1))
        {
            // We convert "::" to "."
            if (*tmp == L':')
            {
                *newCn++ = L'.';
                tmp++;

                if (*tmp != L':')
                {
                    // Badly formed name.
                    *ppClassName = NULL;
                    *ppFieldName = NULL;
                    hr = E_FAIL;
                    goto ErrExit;
                }
                else
                    tmp++;
            }
            else
                *newCn++ = *tmp++;
        }

        // Null terminate the class name.
        *newCn++ = L'\0';

        // Make sure we didn't go over our buffer.
        _ASSERTE((newCn - cn) < len);
    }
    else
    {
        // No separator for the field name, so the whole thing is the
        // field name.
        wcscpy(fn, pWholeName);
        wcscpy(cn, L"\0");
    }

    // All went well, so pass out the results.
    *ppClassName = cn;
    *ppFieldName = fn;

ErrExit:
    if ((hr != S_OK) && fn)
        delete [] fn;

    if ((hr != S_OK) && cn)
        delete [] cn;

    return hr;
}


HRESULT DebuggerShell::ResolveQualifiedFieldName(ICorDebugType *itype,  // The full type of the object, if any
                                                 __in_z WCHAR *fieldName,
                                                 ICorDebugType **pIType, // Output: the type of the object where
                                                 mdFieldDef *pFD,        //         the field resides (a supertype)
                                                 bool *pbIsStatic)
{
    HRESULT hr = S_OK;

    // Separate the class name from the field name.
    WCHAR *fn = NULL;
    WCHAR *cn = NULL;
    ICorDebugClass *iclass = NULL;

    hr = _splitColonQualifiedFieldName(fieldName, &cn, &fn);

    if (hr != S_OK)
        goto exit;

    _ASSERTE(fn && cn);

    // If there is no class name, then we must have current scoping info.
    if ((cn[0] == L'\0') && itype == NULL)
    {
        hr = E_INVALIDARG;
        goto exit;
    }

    // If we've got a specific class name to look for, go get it now.  Do this by finding the
    // type corresponding to the given name.  This may not be a generic type.
    if (cn[0] != L'\0')
    {
        DebuggerModule *module;
        mdTypeDef token;
        hr = ResolveClassName(cn, &module, &token);

        if (FAILED(hr))
            goto exit;

        module->GetICorDebugModule()->GetClassFromToken(token, &iclass);
        if (FAILED(hr))
            goto exit;

                //
        IMetaDataImport2 *pIMI = module->GetMetaData2();
        unsigned int typeArgCount = 0;
        if (pIMI == NULL)
        {
            _ASSERTE(!"NYI");
        }
        else
        {
            HCORENUM enumClassTyPars = 0;
            mdGenericParam classTyPar[1];
            ULONG cClassTyPars;
            do {
                hr = pIMI->EnumGenericParams(&enumClassTyPars, token, classTyPar, 1, &cClassTyPars);
                if( FAILED(hr) ) {
                    Error(L"Could not count type parameters for class\n");
                    goto exit;
                }
                typeArgCount+= cClassTyPars;
            } while (cClassTyPars > 0);
            pIMI->CloseEnum(enumClassTyPars);
        }

        if (typeArgCount > 0)
        {
            Error(L"Fields of objects belonging to generic types may not yet be viewed using CORDBG.\n");
            goto exit;
        }

        // We've found a field in a class, now construct the type
        // corresponding to that class.  We need a CorElementType
        // ELEMENT_TYPE_CLASS or E_T_VALUETYPE to indicate if the class is a
        // value class or not in order to make the type.
        // So we use the CorElementType from the type of the object.
        // If no object is around, we're looking for a static field,
        // when it doesn't matter if it's a ValueClass or not, so just
        // use E_T_CLASS.
        CorElementType et = ELEMENT_TYPE_CLASS;
        if (itype != NULL)
        {
            hr = itype->GetType(&et);
            if( FAILED(hr) )
              goto exit;
        }

        ICorDebugClass2* iclass2 = NULL;
        hr = iclass->QueryInterface(IID_ICorDebugClass2, reinterpret_cast< void** >(&iclass2));
        if (FAILED(hr))
        {
            goto exit;
        }

        hr = iclass2->GetParameterizedType(et, 0, NULL, &itype);
        RELEASE(iclass2);

        if (FAILED(hr))
        {
            goto exit;
        }

    }

    DebuggerModule *module;
    mdTypeDef token;
    itype->AddRef();  // add extra ref to input type
    do
    {

        module = ModuleOfType(itype);
        token = TokenOfType(itype);

        if (module == NULL)
            goto exit;
        // Now get the field off of this class.
        hr = module->GetMetaData()->FindField(token, fn, NULL, 0, pFD);

        if (SUCCEEDED(hr))
            break;
        else
        {
            ICorDebugType *isupertype;
            hr = itype->GetBase(&isupertype);
            RELEASE (itype);
            if (FAILED(hr))
                break;
            // Release - GetBase does an AddRef.
            itype = isupertype;
        }

    } while(itype);

    if (itype == NULL || FAILED(hr) || TypeFromToken(*pFD) != mdtFieldDef)
    {
        hr = E_FAIL;
        goto exit;
    }
    *pIType = itype;

    // Finally, figure out if its static or not.
    DWORD attr;
    hr = module->GetMetaData()->GetFieldProps(*pFD, NULL, NULL, 0, NULL, &attr,
                                              NULL, NULL, NULL, NULL, NULL);

    if (FAILED(hr))
        return hr;

    if (attr & fdStatic)
        *pbIsStatic = true;
    else
        *pbIsStatic = false;

exit:
    if (fn)
        delete [] fn;

    if (cn)
        delete [] cn;
    if (NULL != iclass)
    {
        iclass->Release();
    }

    return hr;
}


// Given a string name of a module, set *pMod to a matching ICorDebugModule.
// If pAppDomainHint is set, then the module matches the given appdomain.
// if it's null, then we return any module with the given name
// If not found, sets *ppMod = NULL, returns FAILED(hr)
//
// This is very different than GetModuleFromSymbolName. In this case, the symbol IS
// a module. In GetModuleFromSymbolName, the symbol is NOT a module; but rather just
// prefixed w/ a module name.
HRESULT DebuggerShell::ResolveFullyQualifiedModuleName(
    const WCHAR * moduleName,
    ICorDebugModule **ppMod,
    ICorDebugAppDomain * pAppDomainHint)
{
    _ASSERTE(pAppDomainHint == NULL);
    _ASSERTE(moduleName != NULL);
    _ASSERTE(ppMod != NULL);


    SymbolContext ctx;

    // If NO AD hint is passed in, check for a fully qualified name
    HRESULT hr = S_OK;
    if (pAppDomainHint == NULL)
    {
        hr = GetContextFromSymbolName(moduleName, &ctx, &moduleName);
        if (FAILED(hr))
            return hr;
    }
    else
    {
        ctx.Set(NULL, pAppDomainHint);
    }

    HASHFIND find;
    DebuggerModule *m;

    for (m = (DebuggerModule*) m_modules.FindFirst(&find);
        m != NULL;
        m = (DebuggerModule*) m_modules.FindNext(&find))
    {
        if (!ctx.Match(m))
            continue;

        WCHAR * pName = m->GetName();

        bool fMatch = false;

        // Full name completely matched? ex: c:\bob\user.exe
        fMatch = (_wcsicmp(pName, moduleName) == 0);

        // Just match filename? ex: user.exe
        if (!fMatch)
        {
            WCHAR * pShortName = m->GetShortName();
            fMatch = (_wcsicmp(pShortName, moduleName) == 0);
        }

        if (fMatch)
        {
            // Found match.
            *ppMod = m->GetICorDebugModule();

            (*ppMod)->AddRef();
            return S_OK;
        }
    }

    *ppMod = NULL;
    return E_INVALIDARG;
}

// Resolve a string method name to an ICorDebugFunction
// If pAppDomainHint is non-null, will pull a function from that AD.
HRESULT DebuggerShell::ResolveFullyQualifiedMethodName(
    const WCHAR *methodName,
    ICorDebugFunction **ppFunc,
    DebuggerFunction **ppDebuggerFunc,
    ICorDebugAppDomain * pAppDomainHint // = NULL
)
{
    HRESULT hr = S_OK;
    *ppFunc = NULL;
    if (ppDebuggerFunc)
      *ppDebuggerFunc = NULL;


    SymbolContext ctx;

    // If NO AD hint is passed in, check for a fully qualified name
    if (pAppDomainHint == NULL)
    {
        hr = GetContextFromSymbolName(methodName, &ctx, &methodName);
        if (FAILED(hr))
            return hr;
    }
    else
    {
        ctx.Set(NULL, pAppDomainHint);
    }


    // Split apart the name into namespace, class name, and method name if necessary.
    const WCHAR *className = NULL;
    const WCHAR *methName = NULL;

    // Does it have a classname?
    WCHAR *classEnd = const_cast<WCHAR*>(wcschr(methodName, L':'));

    if ((classEnd != NULL) && (classEnd[1] == L':'))
    {
        // Name is class::method
        methName = classEnd + 2;
        *classEnd = L'\0';
        className = methodName;
    }
    else
        methName = methodName;

    // Whip over the modules looking for either our class or the method (since the method could be global.)
    HASHFIND find;
    DebuggerModule *m;

    for (m = (DebuggerModule*) m_modules.FindFirst(&find); m != NULL; m = (DebuggerModule*) m_modules.FindNext(&find))
    {
        if (!ctx.Match(m))
            continue;


        // Look for the type first, if we have one.
        mdTypeDef td = mdTypeDefNil;

        if (className != NULL)
            hr = FindTypeDefByName(m, className, &td);

        // Whether we found the type or not, look for a method within the type. If we didn't find the type, then td ==
        // mdTypeDefNil and we'll search the global namespace in this module.
        HCORENUM e = NULL;
        mdMethodDef md = mdMethodDefNil;
        ULONG count;

        // Create an enum of all the methods with this name.
        hr = m->GetMetaData()->EnumMethodsWithName(&e, td, methName, NULL, 0, &count);

        if (FAILED(hr))
            continue;

        // Figure out how many methods match.
        hr = m->GetMetaData()->CountEnum(e, &count);

        if (FAILED(hr) || (count == 0))
            continue;

        // Put the enum back at the start.
        hr = m->GetMetaData()->ResetEnum(e, 0);

        if (count == 1)
        {
            // If there is only one, go ahead and use it.
            hr = m->GetMetaData()->EnumMethodsWithName(&e, td, methName, &md, 1, &count);
            _ASSERTE(count == 1);
        }
        else
        {
            // If there are many, get the user to pick just one.
            mdMethodDef *mdArray = new mdMethodDef[count];

            if (mdArray == NULL)
            {
                g_pShell->ReportError(E_OUTOFMEMORY);
                continue;
            }

            // Snagg all of the methods.
            hr = m->GetMetaData()->EnumMethodsWithName(&e, td, methName, mdArray, count, &count);

            if (SUCCEEDED(hr))
            {
                g_pShell->Write(L"There are %d possible matches for the method %s. Pick one:\n", count, methName);
                g_pShell->Write(L"0) none, abort the operation.\n");

                for (unsigned int i = 0; i < count; i++)
                {
                    PCCOR_SIGNATURE sigBlob = NULL;
                    ULONG       sigBlobSize = 0;
                    DWORD       methodAttr = 0;

                    hr = m->GetMetaData()->GetMethodProps(mdArray[i], NULL, NULL, 0, NULL,
                                                          &methodAttr, &sigBlob, &sigBlobSize, NULL, NULL);

                    _ASSERTE(SUCCEEDED(hr));

                    SigFormat *sf = new SigFormat(m->GetMetaData(), sigBlob, sigBlobSize, methName);

                    if (sf != NULL)
                        hr = sf->FormatSig();
                    else
                        hr = E_OUTOFMEMORY;

                    g_pShell->Write(L"%d) [%08x] %s\n", i + 1, mdArray[i], SUCCEEDED(hr) ? sf->GetString() : methName);

                    if (sf != NULL)
                        delete sf;
                }

                g_pShell->Write(L"\nPlease make a selection (0-%d): ", count);

                WCHAR response[256];
                int ires = 0;

                hr = E_FAIL;

                if (ReadLine(response, 256))
                {
                    const WCHAR *p = response;

                    if (GetIntArg(p, ires))
                    {
                        if ((ires > 0) && (ires <= (int)count))
                        {
                            md = mdArray[ires - 1];
                            hr = S_OK;
                        }
                    }
                }
            }

            delete [] mdArray;
        }

        if (SUCCEEDED(hr))
        {
            DebuggerFunction *func = m->ResolveFunction(md, NULL);

            if (func != NULL)
            {
                *ppFunc = func->m_ifunction;
                (*ppFunc)->AddRef();
                if (ppDebuggerFunc)
                    *ppDebuggerFunc = func;
                break;
            }
        }
    }

    if (m == NULL)
        hr = E_INVALIDARG;

    // Leave the input string like we found it.
    if (classEnd)
        *classEnd = L':';

    return hr;
}

void DebuggerShell::PrintBreakpoint(DebuggerBreakpoint *breakpoint)
{
    bool bPrinted = false;

    DebuggerSourceFile *pSource = NULL;
    if (breakpoint->m_managed)
    {
        if ((breakpoint->m_doc != NULL) && (breakpoint->m_pModuleList != NULL))
        {
            if ((pSource = breakpoint->m_pModuleList->m_pModule->
                    ResolveSourceFile (breakpoint->m_doc)) != NULL)
            {
                if (pSource->GetPath() != NULL)
                {
                    g_pShell->Write(L"#%d\t%s:%d\t", breakpoint->m_id,
                            pSource->GetPath(), breakpoint->m_index);

                    bPrinted = true;

                }
            }
        }
    }

    if (bPrinted == false)
    {
        DebuggerModule *m = NULL;
        WCHAR *pszModName = NULL;

        if (breakpoint->m_pModuleList != NULL)
        {
            m = breakpoint->m_pModuleList->m_pModule;
            _ASSERTE (m != NULL);

            if (m != NULL)
                pszModName = m->GetName();
        }
        else if (breakpoint->m_moduleName != NULL)
        {
            pszModName = breakpoint->m_moduleName;
        }

        if (pszModName == NULL)
            pszModName = L"<UnknownModule>";

        g_pShell->Write(L"#%d\t%s!%s:%d\t", breakpoint->m_id,
                        pszModName, breakpoint->m_name,
                        breakpoint->m_index);
    }

    if (breakpoint->m_threadID != (DWORD) NULL_THREAD_ID)
        g_pShell->Write(L"thread 0x%x ", breakpoint->m_threadID);

    if (!breakpoint->m_active)
        g_pShell->Write(L"[disabled]");

    if (breakpoint->m_managed)
    {
        if (breakpoint->m_pModuleList == NULL)
            g_pShell->Write(L"[unbound] ");
        else
        {
            DebuggerCodeBreakpoint *bp = breakpoint->m_pModuleList->m_pModule->m_breakpoints;

            while (bp != NULL)
            {
                if (bp->m_id == (int) breakpoint->m_id)
                {
                    bp->Print();
                    break;
                }
                bp = bp->m_next;
            }
        }
    }
    else
    {
        if (breakpoint->m_process == NULL)
            g_pShell->Write(L"[unbound] ");
    }

    g_pShell->Write(L"\n");
    if (bPrinted == true)
    {
        // Also, check if the number of lines in the source
        // file are >= line number we want to display
        if (pSource->TotalLines() < breakpoint->m_index)
        {
            // Warn user
            g_pShell->Write(L"WARNING: Cannot display source line %d.", breakpoint->m_index);
            g_pShell->Write(L" Currently associated source file %s has only %d lines.\n",
                            pSource->GetPath(), pSource->TotalLines());

        }
    }
}

void DebuggerShell::PrintThreadPrefix(ICorDebugThread *pThread, bool forcePrint)
{
    DWORD               threadID;

    if (pThread)
    {
        HRESULT hr = pThread->GetID(&threadID);

        if (FAILED(hr))
        {
            g_pShell->ReportError(hr);
            return;
        }

        if (threadID != m_lastThread || forcePrint)
        {
            Write(L"[thread 0x%x] ", threadID);
            m_lastThread = threadID;
        }
    }
    else
    {
        Write(L"[No Managed Thread] ");
    }
}

HRESULT DebuggerShell::StepStart(ICorDebugThread *pThread,
                                 ICorDebugStepper *pStepper)
{
    DWORD dwThreadId = 0;

    if( pThread != NULL )
    {
        //figure out which thread to stick the stepper to in case
        //we don't complete the step (ie, the program exits first)

        HRESULT hr;
        hr = pThread->GetID( &dwThreadId);
        _ASSERTE( !FAILED( hr ) );

        DebuggerManagedThread  *dmt = (DebuggerManagedThread  *)
            m_managedThreads.GetBase( dwThreadId );
        _ASSERTE(dmt != NULL);

        //add this to the list of steppers-in-progress
        if (pStepper)
            dmt->m_pendingSteppers->AddStepper( pStepper );
    }

    m_lastStepper = pStepper;
    return S_OK;
}

//called by DebuggerCallback::StepComplete
void DebuggerShell::StepNotify(ICorDebugThread *thread,
                               ICorDebugStepper *pStepper)
{
    g_pShell->m_enableCtrlBreak = false;

    if (pStepper != m_lastStepper)
    {   // mulithreaded debugging: the step just completed is in
        // a different thread than the one that we were last in,
        // so print something so the user will know what's going on.

        // It looks weird to have a thread be created and then immediately
        // complete a step, so we first check to make sure that the thread
        // hasn't just been created.
        DWORD dwThreadId;

        HRESULT hr;
        hr = thread->GetID( &dwThreadId);
        _ASSERTE( !FAILED( hr ) );

        DebuggerManagedThread  *dmt = (DebuggerManagedThread  *)
            m_managedThreads.GetBase( dwThreadId );
        _ASSERTE(dmt != NULL);

        if (!dmt->fSuperfluousFirstStepCompleteMessageSuppressed)
        {
           dmt->fSuperfluousFirstStepCompleteMessageSuppressed = true;
        }
        else
        {
            PrintThreadPrefix(thread);
            Write(L" step complete\n");
        }
    }

    m_lastStepper = NULL;

    //we've completed the step, so elim. the pending step field
    if (pStepper)
    {
        DebuggerManagedThread *dmt = GetManagedDebuggerThread( thread );
        _ASSERTE( dmt != NULL );
        _ASSERTE( dmt->m_pendingSteppers->IsStepperPresent(pStepper) );
        dmt->m_pendingSteppers->RemoveStepper(pStepper);
    }
}

//
// Print the current source line. The parameter around specifies how many
// lines around the current line you want printed, too. If around is 0,
// only the current line is printed.
//
BOOL DebuggerShell::PrintCurrentSourceLine(unsigned int around)
{
    HRESULT hr;
    BOOL ret = FALSE;

    if ((m_currentThread == NULL) && (m_currentUnmanagedThread != NULL))
        return PrintCurrentUnmanagedInstruction(around, 0, 0);

    // Don't do anything if there isn't a current thread.
    if ((m_currentThread == NULL) || (m_rawCurrentFrame == NULL))
        return (ret);

    // Just print native instruction if we dont have an IL frame
    if (m_currentFrame == NULL)
    {
        _ASSERTE(m_rawCurrentFrame);
        return (PrintCurrentInstruction(around, 0, 0));
    }

    ICorDebugCode *icode;
    hr = m_currentFrame->GetCode(&icode);

    if (FAILED(hr))
    {
        g_pShell->ReportError(hr);
        return (FALSE);
    }

    ICorDebugFunction *ifunction;
    icode->GetFunction(&ifunction);

    RELEASE(icode);

    if (FAILED(hr))
    {
        g_pShell->ReportError(hr);
        return (FALSE);
    }

    DebuggerFunction *function = DebuggerFunction::FromCorDebug(ifunction);
    _ASSERTE(function != NULL);

    RELEASE(ifunction);

    ULONG32 ip;
    CorDebugMappingResult mappingResult;
    hr = m_currentFrame->GetIP(&ip, &mappingResult);

    if (FAILED(hr))
    {
        g_pShell->ReportError(hr);
        return (FALSE);
    }

    DebuggerSourceFile* sf;
    unsigned int lineNumber;
    hr = function->FindLineFromIP(ip, &sf, &lineNumber);


    if (hr == S_OK)
    {
        if (DebuggerFunction::kNoSourceForIL == lineNumber)
        {
            Write(L"WARNING: no source code found for current IP. Source displayed may be incorrect.\n");
            function->FindLineClosestToIP(ip, &sf, &lineNumber);
        }
        ret = sf->LoadText(m_currentSourcesPath, false);
    }
    else
    {
        ret = FALSE;
    }

    if (ret && (sf->TotalLines() > 0))
    {
        if (g_pShell->m_rgfActiveModes & DSM_ENHANCED_DIAGNOSTICS)
        {
            _ASSERTE(sf != NULL);
            Write(L"File:%s\n",sf->GetName());
        }

        unsigned int start, stop;

        if (lineNumber > around)
            start = lineNumber - around;
        else
            start = 1;

        if ((lineNumber + around) <= sf->TotalLines())
            stop = lineNumber + around;
        else
            stop = sf->TotalLines();

        while (start <= stop)
        {
            if ((start == lineNumber) && (around != 0))
                Write(L"%03d:*%s\n", start, sf->GetLineText(start));
            else
                Write(L"%03d: %s\n", start, sf->GetLineText(start));

            start++;

            ret = TRUE;
        }

        ActivateSourceView(sf, lineNumber);
    }

    if (!ret)
        return (PrintCurrentInstruction(around, 0, 0));
    else
        return (TRUE);
}

void DebuggerShell::ActivateSourceView(DebuggerSourceFile *psf, unsigned int lineNumber)
{
}



//
// Disassemble unmanaged code. This is different from disassembling
// managed code. For managed code, we know where the function starts
// and ends, and we get the code from a different place. For unmanaged
// code, we only know our current IP, and we have to guess at the
// start and end of the function.
//
BOOL DebuggerShell::PrintCurrentUnmanagedInstruction(
                                            unsigned int around,
                                            int          offset,
                                            DWORD_PTR    startAddr)
{
#if defined(_X86_) || defined(_WIN64)

#else  // !_X86_ && !_WIN64
    PORTABILITY_ASSERT("DebuggerShell::PrintCurrentUnmanagedInstruction NYI for this platform");
#endif // !_X86_ && !_WIN64

    return TRUE;
}


//
// Print the current native instruction. The parameter around
// specifies how many lines around the current line you want printed,
// too. If around is 0, only the current line is printed.
//
BOOL DebuggerShell::PrintCurrentInstruction(unsigned int around,
                                            int          offset,
                                            DWORD        startAddr)
{
    HRESULT hr;

    // Don't do anything if there isn't a current thread.
    if ((m_currentThread == NULL) && (m_currentUnmanagedThread == NULL))
        return (FALSE);

    // We do something very different for unmanaged code...
    if ((m_rawCurrentFrame == NULL) || (startAddr != 0))
        return PrintCurrentUnmanagedInstruction(around,
                                                offset,
                                                startAddr);

    BYTE*                   nativeCode = NULL;
    ICorDebugCode *icode;
    ICorDebugILFrame *ilFrame;
    hr = m_rawCurrentFrame->QueryInterface(IID_ICorDebugILFrame,
                                           (void **)&ilFrame);
    if (FAILED(hr) || NULL == ilFrame) {
        hr = m_rawCurrentFrame->GetCode(&icode);
    } else {
        hr = ilFrame->GetCode(&icode);
        ilFrame->Release();
        ilFrame = NULL;
    }

    if (FAILED(hr))
    {
        g_pShell->ReportError(hr);
        return (FALSE);
    }

    BOOL isIL;
    hr = icode->IsIL(&isIL);

    if (FAILED(hr))
    {
        g_pShell->ReportError(hr);
        RELEASE(icode);
        return (FALSE);
    }

    ICorDebugFunction *ifunction;
    hr = icode->GetFunction(&ifunction);

    RELEASE(icode);

    if (FAILED(hr))
    {
        g_pShell->ReportError(hr);
        return (FALSE);
    }

    DebuggerFunction *function = DebuggerFunction::FromCorDebug(ifunction);
    _ASSERTE(function != NULL);

    RELEASE(ifunction);

    ULONG32 ip;
    CorDebugMappingResult mappingResult;

    if (isIL)
    {
        hr = m_currentFrame->GetIP(&ip, &mappingResult);

        if (FAILED(hr))
        {
            g_pShell->ReportError(hr);
            return (FALSE);
        }
    }
    else
    {
        ReleaseHolder<ICorDebugNativeFrame> inativeFrame;
        hr = m_rawCurrentFrame->QueryInterface(IID_ICorDebugNativeFrame,
                                               (void **)&inativeFrame);

        if (FAILED(hr))
        {
            g_pShell->ReportError(hr);
            return (FALSE);
        }

        hr = inativeFrame->GetIP(&ip);

        if (FAILED(hr))
        {
            g_pShell->ReportError(hr);
            return (FALSE);
        }
    }


    WCHAR buffer[1024];


    // GENERICS: when native, get the code from the frame, not the function.
    if (isIL && FAILED(function->LoadCode(!isIL)))
    {
        Write(L"Unable to provide disassembly.\n");
        return (FALSE);
    }

    BYTE *codeStart;
    BYTE *codeEnd;

    if (isIL)
    {
        codeStart = function->m_ilCode;
        codeEnd = function->m_ilCode + function->m_ilCodeSize;
    }
    else
    {
        // GENERICS: when native, get the code from the frame, not the function.
        // This means we get exactly the right native code regardless of which
        // native version of the function we are running.
        ReleaseHolder<ICorDebugCode> inativecode;
        hr = m_rawCurrentFrame->GetCode(&inativecode);

        if (FAILED(hr))
        {
            g_pShell->ReportError(hr);
            return (FALSE);
        }

        ULONG32 nativeCodeSize;
        inativecode->GetSize(&nativeCodeSize);

        nativeCode = new BYTE [nativeCodeSize];
        _ASSERTE(nativeCode != NULL);

        if (nativeCode == NULL)
        {
            g_pShell->ReportError( E_OUTOFMEMORY );
            return (FALSE);
        }

        ULONG32 fetchedSize;
        hr = inativecode->GetCode(0, nativeCodeSize, nativeCodeSize,
                            nativeCode, &fetchedSize);

        codeStart = nativeCode;
        codeEnd = nativeCode + fetchedSize;
    }

    if (around == 0)
    {
        DebuggerFunction::Disassemble(!isIL,
                                      ip,
                                      codeStart,
                                      codeEnd,
                                      buffer,
                                      FALSE,
                                      function->m_module,
                                      function->m_ilCode);

        Write(buffer);
    }
    else
    {
        // What a pain - we have to trace from the beginning of the method
        // to find the right instruction boundary.
        size_t currentAddress = ip;
        size_t address = 0;

        const size_t endAddress = codeEnd - codeStart;

        unsigned int instructionCount = 0;
        while (address < currentAddress)
        {
            size_t oldAddress = address;

            address = function->WalkInstruction(!isIL,
                                                address,
                                                codeStart,
                                                codeEnd);

            if (address == 0xffff)
                break;

            // If the WalkInstruction didn't advance the address, then
            // break. This means that we failed to disassemble the
            // instruction.  get to next line
            if (address == oldAddress)
                break;

            instructionCount++;
        }

        // Now, walk forward again to get to the starting point.
        address = 0;

        while (around < instructionCount)
        {
            address = function->WalkInstruction(!isIL,
                                                address,
                                                codeStart,
                                                codeEnd);
            instructionCount--;
        }

        unsigned int i;

        for (i = 0; i < instructionCount; i++)
        {
            Write(L" ");
            address = DebuggerFunction::Disassemble(!isIL,
                                                    address,
                                                    codeStart,
                                                    codeEnd,
                                                    buffer,
                                                    FALSE,
                                                    function->m_module,
                                                    function->m_ilCode);
            Write(buffer);
        }

        Write(L"*");
        address = DebuggerFunction::Disassemble(!isIL,
                                                address,
                                                codeStart,
                                                codeEnd,
                                                buffer,
                                                FALSE,
                                                function->m_module,
                                                function->m_ilCode);
        Write(buffer);

        for (i = 0; i < around && address < endAddress; i++)
        {
            Write(L" ");
            address = DebuggerFunction::Disassemble(!isIL,
                                                    address,
                                                    codeStart,
                                                    codeEnd,
                                                    buffer,
                                                    FALSE,
                                                    function->m_module,
                                                    function->m_ilCode);
            Write(buffer);
        }
    }

    if (nativeCode)
        delete [] nativeCode;


    return TRUE;
}

//
// The current source file path is returned in currentPath. Free with
// delete currentPath;
// Returns FALSE if it fails.
//
BOOL DebuggerShell::ReadSourcesPath(DebuggerString *currentPath)
{
    BOOL b;

    _ASSERTE(currentPath != NULL);


    // Space for MAX_PATH_ELEMS paths, plus separators between them
    WCHAR currentPathW[MAX_PATH_ELEMS*(_MAX_PATH+1)];

    b = PAL_FetchConfigurationStringW(FALSE, // fetch per-user configuration
                                      TEXT(REG_SOURCES_KEY),
                                      currentPathW,
                                      sizeof(currentPathW)/sizeof(currentPathW[0]));

    if (!b) {
        return FALSE;
    }

    if (!currentPath->CopyFrom(currentPathW))
    {
        return FALSE;
    }
    return TRUE;
}

//
// Write a new source file path to the config file. If successful, return
// TRUE.
//
BOOL DebuggerShell::WriteSourcesPath(__in_z WCHAR *newPath)
{
    BOOL b;

    b = PAL_SetConfigurationStringW(FALSE, // set per-user configuration
                                    TEXT(REG_SOURCES_KEY),
                                    newPath);

    if (b) {
        return TRUE;
    }

    Write(L"Error %d writing new path to registry.\n", GetLastError());

    return FALSE;
}

BOOL DebuggerShell::AppendSourcesPath(const WCHAR *newpath)
{
    DebuggerString szPath;
    int         ilen;
    ilen = (int)(m_currentSourcesPath.Length() + wcslen(newpath) + 4);

    if (!szPath.Allocate(ilen))
    {
        return (FALSE);
    }

    wcscpy(szPath.GetData(), m_currentSourcesPath);
    wcscat(szPath.GetData(), L";");
    wcscat(szPath.GetData(), newpath);

    m_currentSourcesPath.CopyFrom(szPath);

    return (TRUE);
}


// Called when we failed to find a source file on the default path.  You
// may prompt for path information.
HRESULT DebuggerShell::ResolveSourceFile(
    DebuggerSourceFile *pf,
    __in_z CHAR *pszPath,
    __out_z __inout_ecount(iMaxLen) CHAR *pszFullyQualName,
    int iMaxLen,
    bool bChangeOfName)
{
    HRESULT  hr = S_FALSE;
    CHAR    *pstrFileName = NULL;
    int      fileNameLength;
    char *rcFullPathArray [MAX_PATH_ELEMS]; // to hold full paths for all elems
    char *tempBuf = NULL;

    if (pf->m_name == NULL)
        return S_FALSE;

    MAKE_ANSIPTR_FROMWIDE(nameA, pf->m_name);
    _ASSERTE(pszPath != NULL && nameA != NULL);


    // Initialize the array elements
    for (int j=0; j<MAX_PATH_ELEMS; j++)
    {
        rcFullPathArray [j] = NULL;
    }


    // First off, check the SourceFile cache to see if there's an
    // entry matching the module and document
    ISymUnmanagedDocument *doc = NULL;
    GUID g = {0};
    if ((pf->m_module->GetSymbolReader() != NULL) &&
        SUCCEEDED(pf->m_module->GetSymbolReader()->GetDocument(pf->m_name.GetData(),
                                                                g, g, g,
                                                                &doc)))
    {
        if (bChangeOfName == false)
        {
            m_FPCache.GetFileFromCache (pf->m_module, doc, &pstrFileName);
            if (pstrFileName != NULL)
            {
                strncpy(pszFullyQualName, pstrFileName, iMaxLen);
                pstrFileName[iMaxLen - 1] = '\0';
                delete [] pstrFileName;

                RELEASE(doc);
                doc = NULL;

                hr = S_OK;
                goto CleanUp;
            }
        }
        else
        {
            // We have already determined (in one of the calling func) that this file exists.
            // But we need to get the fully qualified path and also update the cache.
            // Note: These buffers must be large enough for the strcat below.
            CHAR        rcDrive [_MAX_PATH];
            CHAR        rcFile[_MAX_FNAME + _MAX_EXT];
            CHAR        rcPath[_MAX_PATH];
            CHAR        rcExt [_MAX_EXT];
            _splitpath(pszPath, rcDrive, rcPath, rcFile, rcExt);

            strcat (rcDrive, rcPath);
            strcat (rcFile, rcExt);

            fileNameLength = SearchPathA(rcDrive,
                                         rcFile,
                                         NULL,
                                         iMaxLen,
                                         pszFullyQualName,
                                         NULL);

            if ((fileNameLength > 0) && (fileNameLength < iMaxLen))
            {
                m_FPCache.UpdateFileCache (pf->m_module, doc,
                                           pszFullyQualName);
                RELEASE(doc);
                doc = NULL;

                hr = S_OK;
                goto CleanUp;
            }
        }
    }

    // Now, try to locate the file as is:
    fileNameLength = SearchPathA(".", nameA, NULL, iMaxLen,
                                      pszFullyQualName, NULL);

    if (fileNameLength == 0)
    {
        // file name was not located. So, try all the paths

        // extract the filename and extension from the file name
        CHAR        rcFile[_MAX_FNAME];
        CHAR        rcExt [_MAX_EXT];
        _splitpath(nameA, NULL, NULL, rcFile, rcExt);

        strcat (rcFile, rcExt);

        // get the number of elements in the search path
        int iNumElems = m_FPCache.GetPathElemCount();

        // if could be that the search path was earlier null and
        if (iNumElems > 0)
        {
            int iCount = 0;

            int iIndex = 0;

            // for each element in the search path, see if the file exists
            while (iIndex < iNumElems)
            {
                _ASSERTE(iCount < MAX_PATH_ELEMS);

                if (tempBuf == NULL)
                {
                    tempBuf = (char *) new char[MAX_PATH];
                }

                char *pszPathElem = m_FPCache.GetPathElem (iIndex);

                // first, try and use the unsplit name. If that doesn't return a match,
                // use the stripped name

                fileNameLength = SearchPathA(pszPathElem,
                                                nameA,
                                                NULL,
                                                iMaxLen,
                                                tempBuf,
                                                NULL);
                if (fileNameLength == 0)
                {
                    fileNameLength = SearchPathA(pszPathElem,
                                                    rcFile,
                                                    NULL,
                                                    iMaxLen,
                                                    tempBuf,
                                                    NULL);
                }


                if ((fileNameLength > 0) && (fileNameLength < iMaxLen))
                {
                    // copy it over
                    rcFullPathArray [iCount] = tempBuf;
                    tempBuf = NULL;
                    iCount++;
                }

                iIndex++;
            }

            if (iCount > 0)
            {
                // atleast one file was located

                // convert all names to lowercase
                for (int i=0; i<iCount; i++)
                {
                    _ASSERTE(rcFullPathArray [i] != NULL);
                    iIndex = 0;
                    while (rcFullPathArray [i][iIndex] != '\0')
                    {
                        rcFullPathArray [i][iIndex] = tolower (
                                                    rcFullPathArray[i][iIndex]);
                        iIndex++;
                    }
                }


                // remove any duplicate entries
                int iLowerBound = 1;
                for (int iCounter1=1;   iCounter1 < iCount; iCounter1++)
                {
                    bool fDuplicate = false;
                    for (int iCounter2=0; iCounter2 < iLowerBound;
                                                        iCounter2++)
                    {
                        if ((strcmp (rcFullPathArray [iCounter2],
                                    rcFullPathArray [iCounter1]) == 0))
                        {
                            // found a duplicate entry. So break.
                            fDuplicate = true;
                            break;
                        }
                    }

                    if (fDuplicate == false)
                    {
                        // if we've found atleast one duplicate uptil now,
                        // then copy this entry into the entry pointed to
                        // by iLowerbound. Otherwise no need to do so (since
                        // it would be a copy to self).
                        if (iLowerBound != iCounter1)
                            strcpy (rcFullPathArray [iLowerBound],
                                    rcFullPathArray [iCounter1]);

                        iLowerBound++;
                    }
                }

                // new count equals the number of elements in the array (minus
                // the duplicates)
                iCount = iLowerBound;


                if (iCount == 1)
                {
                    // exactly one file was located. So this is the one!!
                    strcpy (pszFullyQualName, rcFullPathArray [0]);

                    // add this to the SourceFile cache
                    if (doc != NULL)
                    {
                        m_FPCache.UpdateFileCache (pf->m_module, doc,
                                                   pszFullyQualName);
                        RELEASE(doc);
                        doc = NULL;
                    }

                    hr = S_OK;
                }
                else
                {
                    // ask user to select which file he wants to open
                    while (true)
                    {
                        int iTempCount = 1;

                        // Print all the file names found
                        while (iTempCount <= iCount)
                        {
                            Write (L"\n%d)\t%S", iTempCount, rcFullPathArray [iTempCount - 1]);
                            iTempCount++;
                        }

                        WCHAR strTemp [10+1];
                        int iResult;
                        while (true)
                        {
                            Write (L"\nPlease select one of the above options (enter the number): ");
                            if (ReadLine (strTemp, 10))
                            {
                                const WCHAR *p = strTemp;
                                if (GetIntArg (p, iResult))
                                {
                                        if (iResult > 0 && iResult <= iCount)
                                        {
                                        strcpy (pszFullyQualName, rcFullPathArray [iResult-1]);

                                        // add this to the SourceFile cache
                                        if (doc != NULL)
                                        {
                                            m_FPCache.UpdateFileCache (
                                                                pf->m_module,
                                                                doc,
                                                                pszFullyQualName
                                                                );
                                            RELEASE(doc);
                                            doc = NULL;
                                        }

                                        hr = S_OK;
                                        goto CleanUp;
                                    }
                                }
                            }

                        }

                    }
                }
            }
        }
    }
    else
    {
        // Should never exceed the maximum path length
        _ASSERTE( 0 < fileNameLength && fileNameLength <= MAX_PATH);

        hr = S_OK;
    }

    if (doc != NULL)
        RELEASE(doc);
CleanUp:
    if (tempBuf != NULL)
    {
        delete [] tempBuf;
    }
    for (int i = 0; i < MAX_PATH_ELEMS; i++)
    {
        if (rcFullPathArray [i] != NULL)
        {
            _ASSERTE( tempBuf != rcFullPathArray [i]);
            delete [] rcFullPathArray [i];
        }
    }
    return hr;
}


// Read the last set of debugger modes from the config file.
BOOL DebuggerShell::ReadDebuggerModes(void)
{
    WCHAR Temp[12]; // space for -2147483647, the largest integer
    BOOL b;

    // if this fires, then change the size of the Temp[] array
    C_ASSERT(sizeof(m_rgfActiveModes) == 4);

    b = PAL_FetchConfigurationStringW(FALSE, // per-user configuration
                                      TEXT(REG_MODE_KEY),
                                      Temp,
                                      sizeof(Temp)/sizeof(Temp[0]));

    if (!b) {
        return FALSE;
    }

    m_rgfActiveModes = _wtoi(Temp);

    return TRUE;
}

// Write the current set of debugger modes to the registry.
BOOL DebuggerShell::WriteDebuggerModes(void)
{
    WCHAR Temp[12]; // space for -2147483647, the largest integer
    BOOL b;

    // if this fires, then change the size of the Temp[] array
    C_ASSERT(sizeof(m_rgfActiveModes) == 4);

    _itow(m_rgfActiveModes, Temp, 10);

    b = PAL_SetConfigurationStringW(FALSE, // per-user configuration
                                    TEXT(REG_MODE_KEY),
                                    Temp);
    if (b) {
        return TRUE;
    }

    Write(L"Error %d writing new path to registry.\n", GetLastError());

    return FALSE;
}

ICorDebugManagedCallback *DebuggerShell::GetDebuggerCallback()
{
    return (new DebuggerCallback());
}




DebuggerModule *DebuggerShell::ResolveModule(ICorDebugModule *m)
{
    DebuggerModule *module = (DebuggerModule *)m_modules.GetBase((ULONG_PTR)m);

    return (module);
}

HRESULT DebuggerShell::NotifyModulesOfEnc(ICorDebugModule *pModule,
                                          IStream *pSymStream)
{
    DebuggerModule *m = DebuggerModule::FromCorDebug(pModule);
    _ASSERTE(m != NULL);

    if (m->m_pISymUnmanagedReader != NULL)
    {

        HRESULT hr = m->m_pISymUnmanagedReader->UpdateSymbolStore(NULL,
                                                                  pSymStream);

        if (FAILED(hr))
            Write(L"Error updating symbols for module: 0x%08x\n", hr);
    }

    return S_OK;
}

void DebuggerShell::ClearDebuggeeState(void)
{
    m_needToSkipCompilerStubs = true;
}


DebuggerSourceFile *DebuggerShell::LookupSourceFile(const WCHAR* name)
{
    HASHFIND find;

    for (DebuggerModule *module = (DebuggerModule *) m_modules.FindFirst(&find);
        module != NULL;
        module = (DebuggerModule *) m_modules.FindNext(&find))
    {
        DebuggerSourceFile *file = module->LookupSourceFile(name);
        if (file != NULL)
            return (file);
    }

    return (NULL);
}

//
// SkipCompilerStubs returns TRUE if the given thread is outside of a
// compiler compiler-generated stub. If inside a compiler stub, it
// creates a stepper on the thread and continues the process.
//
// This is really only a temporary thing in order to get VB apps past
// the compiler generated stubs and down to the real user entry
// point. In the future, we will be able to determine the proper
// entrypoint for an app and set a brekapoint there rather than going
// through all of this to step through compiler generated stubs.
//
bool DebuggerShell::SkipCompilerStubs(ICorDebugAppDomain *pAppDomain,
                                      ICorDebugThread *pThread)
{
    bool ret = true;

    ICorDebugChainEnum *ce;
    ICorDebugChain *ichain;
    ICorDebugFrameEnum *fe;
    ICorDebugFrame *iframe;
    ICorDebugFunction *ifunction;
    DebuggerFunction *function;
    ICorDebugStepper *pStepper;

    HRESULT hr = pThread->EnumerateChains(&ce);

    if (FAILED(hr))
        goto exit;

    DWORD got;
    hr = ce->Next(1, &ichain, &got);

    RELEASE(ce);

    //
    // Next returns E_FAIL if there is no next item, along with
    // the count being 0.  Convert that to just being S_OK.
    //
    if ((hr == E_FAIL) && (got == 0))
    {
        hr = S_OK;
    }

    if (FAILED(hr))
        goto exit;

    if (got == 1)
    {
        hr = ichain->EnumerateFrames(&fe);

        RELEASE(ichain);

        if (FAILED(hr))
            goto exit;

        hr = fe->Next(1, &iframe, &got);

        RELEASE(fe);

        //
        // Next returns E_FAIL if there is no next item, along with
        // the count being 0.  Convert that to just being S_OK.
        //
        if ((hr == E_FAIL) && (got == 0))
        {
            hr = S_OK;
        }

        if (FAILED(hr))
            goto exit;

        if (got == 1)
        {
            hr = iframe->GetFunction(&ifunction);

            RELEASE(iframe);

            if (FAILED(hr))
                goto exit;

            // Get the DebuggerFunction for the function interface
            function = DebuggerFunction::FromCorDebug(ifunction);
            _ASSERTE(function);

            RELEASE(ifunction);

            WCHAR *funcName = function->GetName();

            // These are stub names for the only compiler we know
            // generates such stubs at this point: VB. If your
            // compiler also generates stubs that you don't want the
            // user to see, add the names in here.
            if (!wcscmp(funcName, L"_main") ||
                !wcscmp(funcName, L"mainCRTStartup") ||
                !wcscmp(funcName, L"_mainMSIL") ||
                !wcscmp(funcName, L"_vbHidden_Constructor") ||
                !wcscmp(funcName, L"_vbHidden_Destructor") ||
                !wcscmp(funcName, L"_vbGenerated_MemberConstructor") ||
                !wcscmp(funcName, L"_vbGenerated_StaticConstructor"))
            {
                hr = pThread->CreateStepper(&pStepper);

                if (FAILED(hr))
                    goto exit;

                hr = pStepper->SetUnmappedStopMask( g_pShell->ComputeStopMask() );

                if (FAILED(hr))
                    goto exit;

                hr = pStepper->SetInterceptMask( g_pShell->ComputeInterceptMask() );

                if (FAILED(hr))
                    goto exit;

                g_pShell->SetJMCStepperToDefault(pStepper);

                hr = pStepper->Step(TRUE);

                if (FAILED(hr))
                {
                    RELEASE(pStepper);
                    goto exit;
                }
                m_showSource = true;
                StepStart(pThread, pStepper);

                ReleaseHolder<ICorDebugController> pController;
                GetControllerInterface(pAppDomain, &pController);
                Continue(pController, pThread);

                ret = false;
            }
        }
    }

exit:
    if (FAILED(hr))
        ReportError(hr);

    return ret;
}


void DebuggerShell::HandleUnmanagedThreadCreate(DWORD dwThreadId,
                                                HANDLE hThread)
{
    DebuggerUnmanagedThread *ut = new DebuggerUnmanagedThread(dwThreadId,
                                                              hThread);
    _ASSERTE(ut);

    HRESULT hr;
    hr = g_pShell->m_unmanagedThreads.AddBase(ut);
    _ASSERTE(SUCCEEDED(hr));
}



int DebuggerShell::GetUserSelection(
    DebuggerModule *rgpDebugModule[],
    __in_ecount(iModuleCount) LPWSTR rgpstrFileName[][MAX_FILE_MATCHES_PER_MODULE],
    int rgiCount[],
    int iModuleCount,
    int iCumulCount
    )
{
    int iOptionCounter = 1; // User gets the breakpoint options starting from 1
    WCHAR rgwcModuleName [MAX_PATH+1];
    ULONG32 NameLength;

    for (int i=0; i<iModuleCount; i++)
    {
        if (rgpDebugModule [i] != NULL)
        {
            // Initialize module name to null
            rgwcModuleName [0] = L'\0';

            // Now get the module name
            rgpDebugModule [i]->GetICorDebugModule()->GetName(MAX_PATH, &NameLength, rgwcModuleName);

            for (int j=0; j < rgiCount[i]; j++)
            {
                Write (L"%d]\t%s!%s\n",  iOptionCounter, rgwcModuleName, rgpstrFileName [i][j]);
                iOptionCounter++;
            }
        }
    }

    Write (L"%d\tAll of the above\n", iOptionCounter);
    Write (L"\nPlease select one of the above :");

    WCHAR strTemp [10+1];
    int iResult;
    while (true)
    {
        if (ReadLine (strTemp, 10))
        {
            const WCHAR *p = strTemp;
            if (GetIntArg (p, iResult))
                if ((iResult > 0) && (iResult <= iOptionCounter))
                    return iResult;

        }

    }
}


BOOL    DebuggerShell::ChangeCurrStackFile (const WCHAR *fileName)
{
    // first, check to see if the file even exists. Otherwise error out.
    MAKE_ANSIPTR_FROMWIDE (fnameA, fileName);
    _ASSERTE (fnameA != NULL);

    FILE *stream = fopen (fnameA, "r");
    HRESULT hr;
    BOOL ret = FALSE;


    if (stream != NULL)
    {
        fclose (stream);

        //
        // Don't do anything if there isn't a current thread.
        //
        if ((m_currentThread == NULL) || (m_currentFrame == NULL))
            return (ret);

        ICorDebugCode *icode;
        hr = m_currentFrame->GetCode(&icode);

        if (FAILED(hr))
        {
            g_pShell->ReportError(hr);
            return (FALSE);
        }

        ICorDebugFunction *ifunction;
        icode->GetFunction(&ifunction);

        RELEASE(icode);

        if (FAILED(hr))
        {
            g_pShell->ReportError(hr);
            return (FALSE);
        }

        DebuggerFunction *function = DebuggerFunction::FromCorDebug(ifunction);
        _ASSERTE(function != NULL);

        RELEASE(ifunction);

        ULONG32 ip;
        CorDebugMappingResult mappingResult;
        hr = m_currentFrame->GetIP(&ip, &mappingResult);

        if (FAILED(hr))
        {
            g_pShell->ReportError(hr);
            return (FALSE);
        }

        DebuggerSourceFile* sf;
        hr = function->FindLineFromIP(ip, &sf, NULL);

        if (hr == S_OK)
            ret = sf->ReloadText(fileName, true);
    }
    else
    {
        g_pShell->Write(L"Could not locate/open given file.\n");
    }

    return ret;
}


BOOL DebuggerShell::UpdateCurrentPath (const WCHAR *newPath)
{
    int iLength = (int)wcslen (newPath);

    if (iLength != 0)
    {
        if (!m_currentSourcesPath.CopyFrom(newPath))
        {
            Error(L"Path not set!\n");
            return false;
        }


        // Now, store this in the DebuggerFilePathCache
        HRESULT hr;
        hr = m_FPCache.InitPathArray (m_currentSourcesPath.GetData());
        _ASSERTE (hr == S_OK);
    }

    return (true);
}


// BOOL fSymbol true if we want to print symbols, false if we
//          want to print the values of global variables
bool DebuggerShell::MatchAndPrintSymbols (const WCHAR* pszArg,
                                          BOOL fSymbol,
                                          bool fSilently)
{
    // separate the module name from the string to search for

    const WCHAR* symbolName;
    DebuggerModule* m;
    HRESULT hr = GetModuleFromSymbolName(pszArg, &m, &symbolName);

    if (FAILED(hr))
    {
        // Case 1: Module not found
        //   ex: nonexistentmodule!func
        //
        // give up now.

        if (!fSilently)
        {
            const ptrdiff_t moduleNameLen = symbolName - pszArg - 1;
            WCHAR* const buf = reinterpret_cast< WCHAR* >(_alloca((moduleNameLen + 1)* sizeof(WCHAR)));
            memcpy(buf, pszArg, moduleNameLen * sizeof(WCHAR));
            buf[moduleNameLen] = L'\0';
            Write(L"Cannot find module \"%s\".\n", buf);
            return false;
        }
    }

    if (hr == S_OK)
    {
        // case 2: module found. look for symbols only in that module
        BOOL fAtleastOne;
        if (fSymbol)
        {
            fAtleastOne = m->PrintMatchingSymbols(symbolName, m->GetName());
        }
        else
        {
            fAtleastOne = m->PrintGlobalVariables(symbolName, m->GetName(), m);
        }

        if (!fAtleastOne && !fSilently)
        {
            const ptrdiff_t moduleNameLen = symbolName - pszArg - 1;
            WCHAR* const buf = reinterpret_cast< WCHAR* >(_alloca((moduleNameLen + 1)* sizeof(WCHAR)));
            memcpy(buf, pszArg, moduleNameLen * sizeof(WCHAR));
            buf[moduleNameLen] = L'\0';
            Write(L"No matching symbols found in module: \"%s\".\n", buf);
            return false;
        }

        return true;
    }

    // case 3: no module specified
    //  ex: func
    //
    // here we'll have to walk through all the modules looking for matching symbols
    HASHFIND find;
    ModuleSearchList MSL;

    _ASSERTE(S_FALSE == hr);
    BOOL fAtleastOne = FALSE;
    for (m = (DebuggerModule*) g_pShell->m_modules.FindFirst(&find);
         m != NULL;
         m = (DebuggerModule*) g_pShell->m_modules.FindNext(&find))
    {
        WCHAR *pszModName = m->GetName();
        if (pszModName == NULL)
        {
            pszModName = L"<UnknownName>";
        }


        WCHAR        rcFile[MAX_PATH];
        WCHAR        rcExt[_MAX_EXT];
        _wsplitpath(pszModName, NULL, NULL, rcFile, rcExt);
        wcscat(rcFile, rcExt);

        //
        if (MSL.ModuleAlreadySearched (rcFile))
        {
            continue;
        }

        // add this module to the list of modules already searched
        MSL.AddModuleToAlreadySearchedList (rcFile);

        // get the MetaData
        IMetaDataImport *pMD = m->GetMetaData();
        if (pMD != NULL)
        {
            if (fSymbol)
            {
                fAtleastOne |= m->PrintMatchingSymbols(symbolName, rcFile);
            }
            else
            {
                fAtleastOne |= m->PrintGlobalVariables(symbolName, rcFile, m);
            }
        }
        else
        {
            if (!fSilently)
            {
                Write(L"**ERROR** No MetaData available for module : %s\n", rcFile);
            }
        }
    }

    if (!fAtleastOne)
    {
        Write (L"No matching symbols found in any of the loaded modules.\n");
    }

    return true;
}


// Given the head of linked list of Handles, to find the matching name
NamedHandle *DebuggerShell::FindHandleWithName(
    __in_z WCHAR *pwzHandleName)     // name to find
{
    NamedHandle *pCur = m_pHandleNameList;
    while (pCur)
    {
        if (wcscmp(pwzHandleName, pCur->m_pwzHandleName) == 0)
        {
            // found a match
            break;
        }
        pCur = pCur->m_pNext;
    }
    return pCur;
}   // FindHandleWithName

void DebuggerShell::AddNamedHandle(NamedHandle *pHandle)
{
    // Make sure caller pass in non-null pointer.
    _ASSERTE(pHandle);

    pHandle->m_pNext = m_pHandleNameList;
    m_pHandleNameList = pHandle;
}   // DeleteNamedHandle


// Delete a given handle
HRESULT DebuggerShell::DeleteNamedHandle(NamedHandle *pHandle)
{
    _ASSERTE(pHandle);

    NamedHandle *pCur = m_pHandleNameList;
    if (pCur == pHandle)
    {
        // special case the deletion of head
        m_pHandleNameList = pCur->m_pNext;
        delete pHandle;
        return S_OK;
    }
    while (pCur)
    {
        if (pCur->m_pNext == pHandle)
        {
            // found a match to delete
            pCur->m_pNext = pHandle->m_pNext;
            delete pHandle;
            return S_OK;
        }
        pCur = pCur->m_pNext;
    }

    // cannot find a match
    return E_FAIL;
}   // DeleteNamedHandle

void UndecorateName(MDUTF8CSTR name, MDUTF8STR u_name)
{
    int i, j;
    int len;

    len = (int)strlen(name);
    j = 0;
    for (i = 1; i < len; i++)
    {
        if (j > MAX_CLASSNAME_LENGTH-1) break;
        if (name[i] != '@') u_name[j++] = name[i];
        else break;
    }

    u_name[j] = '\0';
}

void DebuggerShell::ListAllGlobals (DebuggerModule *m)
{
    IMetaDataImport *pIMetaDI;
    HCORENUM phEnum = 0;
    mdMethodDef rTokens[100];
    ULONG i;
    ULONG count;
    HRESULT hr;
    MDUTF8CSTR name;
    MDUTF8STR  u_name;
    bool anythingPrinted = false;

    pIMetaDI = m->GetMetaData();

    u_name = new char[MAX_CLASSNAME_LENGTH];

    do
    {
        hr = pIMetaDI->EnumMethods(&phEnum, NULL, &rTokens[0], 100, &count);

        if (!SUCCEEDED(hr))
        {
            ReportError(hr);
            goto ErrExit;
        }

        for (i = 0; i < count; i++)
        {
            hr = pIMetaDI->GetNameFromToken(rTokens[i], &name);

            if (name == NULL)
                continue;

            Write(L"\t");

            if (name[0] == '?')
            {
                UndecorateName(name, u_name);

                Write(L"%S (%S)\n", u_name, name);
            }
            else
                Write(L"%S\n", name);

            anythingPrinted = true;
        }
    }
    while (count > 0);

ErrExit:
    delete [] u_name;

    if (!anythingPrinted)
        Write(L"No global functions in this module.\n");
}

void DebuggerShell::ListAllModules(ListType lt)
{
    HASHFIND find;
    DebuggerModule *m;

    for (m = (DebuggerModule*) g_pShell->m_modules.FindFirst(&find);
         m != NULL;
         m = (DebuggerModule*) g_pShell->m_modules.FindNext(&find))
    {
        _printModule(m->GetICorDebugModule(), PT_NONE);

        if (lt == LIST_CLASSES)
        {
            HASHFIND classfind;
            DebuggerClass *cl;

            for (cl = (DebuggerClass*) m->m_loadedClasses.FindFirst(&classfind);
                 cl != NULL;
                 cl = (DebuggerClass*) m->m_loadedClasses.FindNext(&classfind))
            {
                WCHAR *pszClassName = cl->GetName();
                WCHAR *pszNamespace = cl->GetNamespace();

                if (pszClassName == NULL)
                    pszClassName = L"<UnknownClassName>";

                Write (L"\t");
                if (pszNamespace != NULL)
                    Write (L"%s.", pszNamespace);
                Write (L"%s\n", pszClassName);
            }
        }

        // List all the global functions here.
        if (lt == LIST_FUNCTIONS)
        {
            ListAllGlobals(m);
        }
    }
}



/* ------------------------------------------------------------------------- *
 * DebuggerBreakpoint
 * ------------------------------------------------------------------------- */

void DebuggerBreakpoint::CommonCtor()
{
    // Real ctors fill in m_threadID & m_index so DON'T overwrite those
    // values here.
    m_next = NULL;
    m_id = 0;
    m_name = NULL;
    m_moduleName = NULL;
    m_active = false;
    m_managed = false;
    m_doc = NULL;
    m_process = NULL;
    m_address= 0;
    m_patchedValue = 0;
    m_skipThread = 0;
    m_unmanagedModuleBase = 0;
    m_pModuleList = NULL;
    m_deleteLater = false;
}

DebuggerBreakpoint::DebuggerBreakpoint(const WCHAR *name, SIZE_T nameLength,
                                       SIZE_T functionVersion, SIZE_T index, DWORD threadID)
    :  m_index(index), m_threadID(threadID), m_functionVersion(functionVersion)
{
    WCHAR *moduleName = NULL;
    CommonCtor();
    // Make a copy of the name
    if (nameLength > 0)
    {
        // check to see if the name contains the "!" character.
        // Anything before the "!" is a module name and will not
        // be stored in the breakpoint name
        const WCHAR *szModuleEnd = wcschr(name, L'!');
        if (szModuleEnd != NULL)
        {
            // We'll store it in the m_moduleName field, instead
            SIZE_T modNameLen = szModuleEnd - name;
            moduleName = (WCHAR *)_alloca( sizeof(WCHAR) *(modNameLen+1));
            wcsncpy(moduleName, name, modNameLen);
            moduleName[modNameLen] = '\0';

            // Adjust the length, since we've trimmed something off the from the start
            nameLength -= (szModuleEnd+1-name);
            name = szModuleEnd+1;
        }

        m_name = new WCHAR[nameLength+1];
        _ASSERTE(m_name != NULL);

        wcsncpy(m_name, name, nameLength);
        m_name[nameLength] = L'\0';
    }

    Init(NULL, false, moduleName);
}

DebuggerBreakpoint::DebuggerBreakpoint(DebuggerFunction *f, SIZE_T functionVersion,
                                       SIZE_T offset, DWORD threadID)
    : m_index(offset), m_threadID(threadID), m_functionVersion(functionVersion)
{
    CommonCtor();

    SIZE_T len = f->m_name.Length() + 1;

    if (f->m_className != NULL)
        len += f->m_className.Length() + 2;

    m_name = new WCHAR[len];

    if (f->m_className != 0)
    {
        wcscpy(m_name, f->m_className);
        wcscat(m_name, L"::");
        wcscat(m_name, f->m_name);
    }
    else
        wcscpy(m_name, f->m_name);

    Init(f->m_module, true, f->m_module->GetName());
}


DebuggerBreakpoint::DebuggerBreakpoint(DebuggerSourceFile *file, SIZE_T functionVersion,
                                       SIZE_T lineNumber, DWORD threadID)
    : m_index(lineNumber), m_threadID(threadID), m_functionVersion(functionVersion)
{
    CommonCtor();

    // Copy the filename
    m_name = new WCHAR[wcslen(file->m_name) + 1];
    wcscpy(m_name, file->m_name);

    // Init the breakpoint
    Init(file->m_module, true, file->m_module->GetName());
}


DebuggerBreakpoint::~DebuggerBreakpoint()
{
    if (m_active)
        Deactivate();

    if (m_name != NULL)
        delete [] m_name;

    // Remove itself from the shell's list of breakpoints
    DebuggerBreakpoint **bp = &g_pShell->m_breakpoints;
    while (*bp != this)
        bp = &(*bp)->m_next;

    while (m_pModuleList)
        RemoveBoundModule(m_pModuleList->m_pModule);

    *bp = m_next;
}

void DebuggerBreakpoint::Init(DebuggerModule *module,
                              bool bProceed,
                              __in_z __in_opt WCHAR *szModuleName)
{
    bool        bFound = false;
    m_id = ++g_pShell->m_lastBreakpointID;

    m_next = g_pShell->m_breakpoints;
    g_pShell->m_breakpoints = this;

    // If the user gave us a module name, we should keep track of it.
    if (szModuleName)
    {
        // The ICorDebugModule's name will include a full path
        // if the user didn't specify a full path, then
        // we'll prepend the current working directory onto it.

        SIZE_T len = wcslen(szModuleName);
        SIZE_T lenPath = 0;
        WCHAR cwd[MAX_PATH];

        // Are we missing any path info?
        WCHAR *szModuleEnd = wcschr(szModuleName, L'\\');
        if (szModuleEnd == NULL)
        {
            // Then get the cwd
            CHAR cdBuffer[MAX_PATH];
            DWORD dw;
            dw = GetCurrentDirectoryA(MAX_PATH, cdBuffer);

            _ASSERTE(dw); // This shouldn't fail

            memset(cwd, 0, MAX_PATH * sizeof(WCHAR)); // MBTWC fails to null terminate strings correctly
            MultiByteToWideChar(GetConsoleCP(), 0, cdBuffer, (int)strlen(cdBuffer), cwd, MAX_PATH);
            cwd[MAX_PATH - 1] = '\0';

            if (cwd != NULL)
            {
                // getcwd won't end with a '\' - we'll need to
                // add that in between the path & the module name.
                lenPath = wcslen(cwd) + 1;
            }
        }

        // Space for path, module name, and terminating NULL.
        m_moduleName = new WCHAR[len+lenPath+1];

        // If we need to prepend the cwd, put it in now
        if (lenPath)
        {
            wcscpy(m_moduleName, cwd);
            m_moduleName[lenPath-1] = '\\'; // put the dir separator in/
        }

        // Put the module name at the end
        wcscpy(&(m_moduleName[lenPath]), szModuleName);
    }

    if (bProceed == false)
        return;

    if (module != NULL && !IsBoundToModule(module))
    {
        bFound = Bind(module, NULL);

        if (!bFound)
            bFound = BindUnmanaged(g_pShell->m_currentProcess);
    }

    if (bFound)
        g_pShell->OnBindBreakpoint(this, module);

}

bool DebuggerBreakpoint::BindUnmanaged(ICorDebugProcess *process,
                                       DWORD_PTR moduleBase)
{
    if (m_name == NULL)
        return FALSE;

    if (m_process != NULL || process == NULL)
        return FALSE;

    HANDLE hProcess;
    HRESULT hr = process->GetHandle(&hProcess);

    if (FAILED(hr))
    {
        return false;
    }

    // If this is an unmanaged breakpoint for an absolute address,
    // then m_address already holds the address. If its 0, then we try
    // to lookup by name.
    if (m_address == 0)
    {
        return false;
    }

    // Find the base of the module that this symbol is in.
    if (moduleBase == 0)
    {
        return false;
    }

    m_managed = false;
    m_process = process;
    m_unmanagedModuleBase = moduleBase;

    if (m_active)
        ApplyUnmanagedPatch();

    return true;
}

// Looks up the function name of the BP within a module and binds it.
// This will catch bogus function names.
// If bound, this will create a DebuggerCodeBreakpoint.
// Return true if we bind, false if not.
bool DebuggerBreakpoint::Bind(DebuggerModule *module, ISymUnmanagedDocument *doc)
{
    if (m_name == NULL)
        return (false);

    // Make sure we are not double-binding
    if (IsBoundToModule(module))
    {
        return false;
    }

    //
    // First, see if our name is a function name
    //

    bool success = false;
    HRESULT hr = S_OK;

    WCHAR *classEnd = wcschr(m_name, L':');
    if (classEnd != NULL && classEnd[1] == L':')
    {
        //
        // Name is class::method.
        //

        WCHAR *method = classEnd + 2;

        *classEnd = 0;
        mdTypeDef td = mdTypeDefNil;

        // Only try to lookup the class if we have a name for one.
        if (classEnd != m_name)
            hr = g_pShell->FindTypeDefByName(module, m_name, &td);

        // Its okay if we have a nil typedef since that simply
        // indicates a global function.
        if (SUCCEEDED(hr))
        {
            HCORENUM e = NULL;
            mdMethodDef md;
            ULONG count;

            while (TRUE)
            {
                hr = module->GetMetaData()->EnumMethodsWithName(&e, td, method,
                                                                &md, 1,
                                                                &count);
                if (FAILED(hr) || count == 0)
                    break;

                DebuggerFunction *function = module->ResolveFunction(md, NULL);

                if (function == NULL)
                    continue;

                //

                bool il;

                ICorDebugCode *icode = NULL;
                hr = function->m_ifunction->GetNativeCode(&icode);

                if (FAILED(hr) && (hr != CORDBG_E_CODE_NOT_AVAILABLE))
                {
                    g_pShell->ReportError(hr);
                    continue;
                }

                if ((SUCCEEDED(hr) || hr == CORDBG_E_CODE_NOT_AVAILABLE) && icode != NULL)
                {
                    RELEASE(icode);
                    il = FALSE;
                }
                else
                    il = TRUE;

                if (SUCCEEDED(function->LoadCode(!il)))
                {
                    //
                    // Our instruction validation fails if we can't load the
                    // code at this point.  For now, just let it slide.
                    //

                    if (!function->ValidateInstruction(!il, m_index))
                        continue;
                }

                DebuggerCodeBreakpoint *bp =
                    new DebuggerCodeBreakpoint((int)m_id, module,
                                               function, 0,
                                               m_index, il,
                                               (DWORD) NULL_THREAD_ID);

                if (bp == NULL)
                {
                    g_pShell->ReportError(E_OUTOFMEMORY);
                    continue;
                }

                if (m_active)
                    bp->Activate();

                // prefast complains that bp is leaked, but in the DebuggerCodeBreakpoint constructor it is added to
                // the breakpoint list.  So here, assign it null to force the error at a known line and then can supress
                bp = NULL;

                success = true;
            }
        }

        *classEnd = L':';
    }

    if (!success)
    {
        if ((doc == NULL) && (module->GetSymbolReader() != NULL))
        {
            // Get the source file token by name
            GUID g = {0};
            hr = module->GetSymbolReader()->GetDocument(m_name, //JAH
                                                                g, g, g,
                                                                &doc);

            // If the source file wasn't found, see if we can find it using
            // the short name, since the meta data stores only relative paths.
            if (hr != S_OK)
            {
                char        rcFile[_MAX_FNAME];
                char        rcExt[_MAX_EXT];

                MAKE_ANSIPTR_FROMWIDE(nameA, m_name);
                _splitpath(nameA, NULL, NULL, rcFile, rcExt);
                strcat(rcFile, rcExt);


                MAKE_WIDEPTR_FROMANSI(nameW, rcFile);
                hr = module->GetSymbolReader()->GetDocument(nameW,
                                                            g, g, g,
                                                            &doc);
            }
        }


        if ((hr == S_OK) && (doc != NULL))
        {
            DebuggerSourceFile *file = module->ResolveSourceFile(doc);
            _ASSERTE(file != NULL);

            //
            // !!! May want to try to adjust line number rather than just
            // having the binding fail.
            //

            if (file->FindClosestLine((unsigned int)m_index, false) == m_index)
            {
                // We don't leak this allocation because the constructor links
                // it's 'this' pointer into a linked list of DSCBreakpoints.
                // see "m_module->m_breakpoints = this;" in DebuggerCodeBreakpoint()

                DebuggerSourceCodeBreakpoint *bp =
                    new DebuggerSourceCodeBreakpoint((int)m_id, file, m_functionVersion, m_index,
                                                     (DWORD)NULL_THREAD_ID);
                if (bp == NULL)
                {
                    g_pShell->ReportError(E_OUTOFMEMORY);
                    return (false);
                } else if (bp->m_initSucceeded == false)
                    return false;

                if (m_active)
                    bp->Activate();

                m_doc = doc;

                success = true;
            }
        }
    }

    if (success)
    {
        m_managed = true;

        if (!IsBoundToModule(module))
            AddBoundModule(module);
    }

    return (success);
}

void DebuggerBreakpoint::Unbind()
{
    if (m_managed)
    {
        while (m_pModuleList != NULL)
        {
            DebuggerCodeBreakpoint *bp = m_pModuleList->m_pModule->m_breakpoints;

            while (bp != NULL)
            {
                g_pShell->OnUnBindBreakpoint(this, m_pModuleList->m_pModule);
                DebuggerCodeBreakpoint *bpNext = bp->m_next;
                delete bp;
                bp = bpNext;
            }

            // Remove the module from the list
            RemoveBoundModule(m_pModuleList->m_pModule);
        }
    }
    else
    {
        if (m_process != 0)
            UnapplyUnmanagedPatch();

        m_process = NULL;
    }
}

void DebuggerBreakpoint::Activate()
{
    if (m_active)
        return;

    if (m_managed)
    {
        BreakpointModuleNode *pCurNode = m_pModuleList;

        while (pCurNode != NULL)
        {
            DebuggerCodeBreakpoint *bp = pCurNode->m_pModule->m_breakpoints;

            while (bp != NULL)
            {
                if (bp->m_id == (int) m_id)
                {
                    bp->Activate();
                    g_pShell->OnActivateBreakpoint(this);
                }
                bp = bp->m_next;
            }

            pCurNode = pCurNode->m_pNext;
        }
    }
    else
        if (m_process)
            ApplyUnmanagedPatch();

    m_active = true;
}

void DebuggerBreakpoint::Deactivate()
{
    if (!m_active)
        return;

    if (m_managed)
    {
        BreakpointModuleNode *pCurNode = m_pModuleList;

        while (pCurNode != NULL)
        {
            DebuggerCodeBreakpoint *bp = pCurNode->m_pModule->m_breakpoints;

            while (bp != NULL)
            {
                if (bp->m_id == (int) m_id)
                {
                    bp->Deactivate();
                    g_pShell->OnDeactivateBreakpoint(this);
                }
                bp = bp->m_next;
            }

            pCurNode = pCurNode->m_pNext;
        }
    }
    else
    {
        if (m_process != 0)
            UnapplyUnmanagedPatch();
    }

    m_active = false;
}

void DebuggerBreakpoint::Detach()
{
    if (m_managed)
    {
        BreakpointModuleNode *pCurNode = m_pModuleList;

        while (pCurNode != NULL)
        {
            DebuggerCodeBreakpoint *bp = pCurNode->m_pModule->m_breakpoints;

            while (bp != NULL)
            {
                if (bp->m_id == (int) m_id)
                {
                    bp->Deactivate();
                }
                bp = bp->m_next;
            }

            pCurNode = pCurNode->m_pNext;
        }
    }
    else
    {
        if (m_process != 0)
            UnapplyUnmanagedPatch();
    }
}

void DebuggerBreakpoint::DetachFromModule(DebuggerModule * pModule)
{
    _ASSERTE(pModule != NULL);
    if (m_managed)
    {
        DebuggerCodeBreakpoint *bp = pModule->m_breakpoints;

        while (bp != NULL)
        {
            if (bp->m_id == (int) m_id)
            {
                bp->Deactivate();
            }
            bp = bp->m_next;
        }
    }
    else
    {
        if (m_process != 0)
            UnapplyUnmanagedPatch();
    }
}

void DebuggerBreakpoint::Attach()
{
    if (m_managed)
    {
        BreakpointModuleNode *pCurNode = m_pModuleList;

        while (pCurNode != NULL)
        {
            DebuggerCodeBreakpoint *bp = pCurNode->m_pModule->m_breakpoints;

            while (bp != NULL)
            {
                if (bp->m_id == (int) m_id)
                {
                    bp->Activate();
                }
                bp = bp->m_next;
            }

            pCurNode = pCurNode->m_pNext;
        }
    }
    else
    {
        if (m_process != 0)
            ApplyUnmanagedPatch();
    }
}

bool DebuggerBreakpoint::Match(ICorDebugBreakpoint *ibreakpoint)
{
    if (m_managed)
    {
        BreakpointModuleNode *pCurNode = m_pModuleList;

        while (pCurNode != NULL)
        {
            DebuggerCodeBreakpoint *bp = pCurNode->m_pModule->m_breakpoints;

            while (bp != NULL)
            {
                if (bp->m_id == (int) m_id && bp->Match(ibreakpoint))
                    return (true);
                bp = bp->m_next;
            }

            pCurNode = pCurNode->m_pNext;
        }
    }

    return false;
}

bool DebuggerBreakpoint::MatchUnmanaged(CORDB_ADDRESS address)
{
    return !m_managed && m_process != NULL && m_address == address;
}

void DebuggerBreakpoint::ApplyUnmanagedPatch()
{
    DWORD read = 0;

    if (m_address == NULL)
    {
        g_pShell->Write( L"Unable to set unmanaged breakpoint at 0x00000000\n" );
        return;
    }

    ICorDebugProcess2 * pProc2 = NULL;
    m_process->QueryInterface(IID_ICorDebugProcess2, reinterpret_cast<void**> (&pProc2));

    if (pProc2 == NULL)
    {
        // For V1.0, we have  to put in BPs ourselves.
        HRESULT hr = m_process->ReadMemory(m_address, 1, &m_patchedValue, (SIZE_T *)&read);
        if (FAILED(hr) )
        {
            g_pShell->ReportError(hr);
            return;
        }

        if (read != 1 )
        {
            g_pShell->Write( L"Unable to read memory\n" );
            return;
        }

        BYTE patchByte = 0xCC;

        hr = m_process->WriteMemory(m_address, 1, &patchByte, (SIZE_T *)&read);

        if (FAILED(hr) )
            g_pShell->ReportError(hr);

        if (read != 1 )
        {
            g_pShell->Write( L"Unable to write memory\n" );
            return;
        }
    }
    else
    {
        // For V2.0+, use the Unmanaged Breakpoint API on ICorDebugProcess.
        ULONG32 cbSize;
        HRESULT hr = pProc2->SetUnmanagedBreakpoint(m_address, sizeof(m_patchedValue), &m_patchedValue, &cbSize);
        if (FAILED(hr))
        {
            g_pShell->ReportError(hr);
        }
        pProc2->Release();
    }
}

void DebuggerBreakpoint::UnapplyUnmanagedPatch()
{
    DWORD written;

    if (m_address == NULL)
        return;

    // If patch is temporarily disabled for skipping, then ignore it.
    if (m_skipThread != 0)
    {
        return;
    }

    HRESULT hr = S_OK;
    ICorDebugProcess2 * pProc2 = NULL;
    m_process->QueryInterface(IID_ICorDebugProcess2, reinterpret_cast<void**> (&pProc2));

    if (pProc2 == NULL)
    {
        // For V1.0, we have to do the BPs ourselves.
        hr = m_process->WriteMemory(m_address,
                                            1,
                                            &m_patchedValue,
                                            (SIZE_T *)&written);

        if (written != 1)
        {
            g_pShell->Write( L"Unable to write memory!\n" );
        }
    }
    else
    {
        // For V2.0+, use the Unmanaged Breakpoint API on ICorDebugProcess.
        hr = pProc2->ClearUnmanagedBreakpoint(m_address);
        pProc2->Release();
    }

    if (FAILED(hr))
    {
        g_pShell->ReportError(hr);
    }
}


void DebuggerBreakpoint::ChangeSourceFile (const WCHAR *filename)
{
    // first, check to see if the file even exists. Otherwise error out.
    MAKE_ANSIPTR_FROMWIDE (fnameA, filename);
    _ASSERTE (fnameA != NULL);

    FILE *stream = fopen (fnameA, "r");
    DebuggerSourceFile *pSource = NULL;

    if (stream != NULL)
    {
        fclose (stream);

        if (m_managed)
        {
            BreakpointModuleNode *pCurNode = m_pModuleList;

            while (pCurNode != NULL)
            {
                if ((m_doc == NULL) &&
                    (pCurNode->m_pModule->GetSymbolReader() != NULL))
                {
                    // Get the source file token by name
                    DebuggerString tmpFilename;
                    tmpFilename.CopyFrom(filename);
                    _ASSERTE(tmpFilename != NULL);
                    GUID g = {0};
                    ISymUnmanagedDocument *doc = NULL;
                    HRESULT hr = pCurNode->m_pModule->GetSymbolReader()->GetDocument(tmpFilename.GetData(),
                                                                        g, g, g,
                                                                        &doc);

                    // If the source file wasn't found, see if we can find it using
                    // the short name, since the meta data stores only relative paths.
                    if (hr == CLDB_E_RECORD_NOTFOUND)
                    {
                        char        rcFile[_MAX_FNAME];
                        char        rcExt[_MAX_EXT];

                        MAKE_ANSIPTR_FROMWIDE(nameA, filename);
                        _splitpath(nameA, NULL, NULL, rcFile, rcExt);
                        strcat(rcFile, rcExt);

                        MAKE_WIDEPTR_FROMANSI(nameW, rcFile);
                        hr = pCurNode->m_pModule->GetSymbolReader()->GetDocument(nameW,
                                                                    g, g, g,
                                                                    &doc);

                    }

                    m_doc = doc;
                }

                if (m_doc != NULL)
                {
                    if ((pSource = pCurNode->m_pModule->ResolveSourceFile (m_doc)) != NULL)
                    {
                        pSource->ReloadText (filename, true);
                    }
                }
                else
                {
                    g_pShell->Write(L"Could not associate the given source file.\n");
                    g_pShell->Write(L"Please check that the file name (and path) was entered correctly.\n");
                    g_pShell->Write(L"This problem could also arise if files were compiled without the debug flag.\n");

                }

                pCurNode = pCurNode->m_pNext;
            }
        }
    }
    else
    {
        g_pShell->Write(L"Could not locate/open given file.\n");
    }
}



void DebuggerBreakpoint::UpdateName (__in_z WCHAR *pstrName)
{
    // save the old name just in case we run out of memory
    // while allocating memory for the new name
    WCHAR *pTemp = m_name;
    int iLength = (int)wcslen (pstrName);

    if ((m_name = new WCHAR [iLength+1]) != NULL)
    {
        wcscpy (m_name, pstrName);
        delete [] pTemp;
    }
    else
        m_name = pTemp;

}

// This will return true if this breakpoint is associated
// with the pModule argument
bool DebuggerBreakpoint::IsBoundToModule(DebuggerModule *pModule)
{
    for (BreakpointModuleNode *pCur = m_pModuleList; pCur != NULL; pCur = pCur->m_pNext)
    {
        if (pCur->m_pModule == pModule)
            return (true);
    }

    return (false);
}

bool DebuggerBreakpoint::AddBoundModule(DebuggerModule *pModule)
{
    // Make sure we don't add it twice.
    if (IsBoundToModule(pModule))
        return (false);

    // Create new node
    BreakpointModuleNode *pNewNode = new BreakpointModuleNode;
    _ASSERTE(pNewNode != NULL && "Out of memory!!!");

    // OOM?
    if (!pNewNode)
        return (false);

    // Tack it onto the front of the list
    pNewNode->m_pModule = pModule;
    pNewNode->m_pNext = m_pModuleList;
    m_pModuleList = pNewNode;

    // Indicate success
    return (true);
}

bool DebuggerBreakpoint::RemoveBoundModule(DebuggerModule *pModule)
{
    if (!IsBoundToModule(pModule))
        return (false);

    // Find the module in the list
    BreakpointModuleNode **ppCur;
    for (ppCur = &m_pModuleList;
        *ppCur != NULL && (*ppCur)->m_pModule != pModule;
        ppCur = &((*ppCur)->m_pNext));

    _ASSERTE(*ppCur != NULL);

    // Remove the module from the list
    BreakpointModuleNode *pDel = *ppCur;

    // First case, the node is the first one in the list
    if (pDel == m_pModuleList) {
        m_pModuleList = pDel->m_pNext;
        pDel->m_pModule = NULL;
        pDel->m_pNext = NULL;
        delete pDel;
        return (true);
    }

    // Otherwise, get the module before pDel in the list
    BreakpointModuleNode *pBefore;
    for (pBefore = m_pModuleList; pBefore != NULL; pBefore = pBefore->m_pNext)
    {
        if (pBefore->m_pNext == pDel)
            break;
    }
    _ASSERTE(pBefore != NULL);
    pBefore->m_pNext = pDel->m_pNext;
    pDel->m_pModule = NULL;
    pDel->m_pNext = NULL;
    delete pDel;

    return (true);
}


/* ------------------------------------------------------------------------- *
 * Debugger FilePathCache
 * ------------------------------------------------------------------------- */

// This function looks for an existing "foo.deb" file and reads in the
// contents and fills the structures.
HRESULT DebuggerFilePathCache::Init (void)
{
    int i=0;

    // Set the path elements from the current path.
    _ASSERTE (g_pShell->m_currentSourcesPath != NULL);

    DebuggerString szTemp;

    if (szTemp.CopyFrom(g_pShell->m_currentSourcesPath))
    {
        g_pShell->UpdateCurrentPath (szTemp);

    }

    // free up the cache
    for (i=0; i<m_iCacheCount; i++)
    {
        delete [] m_rpstrModName [i];
        delete [] m_rpstrFullPath [i];

        m_rpstrModName [i] = NULL;
        m_rpstrFullPath [i] = NULL;
        m_rDocs [i] = NULL;
    }

    m_iCacheCount = 0;

    return S_OK;
}

// This function is used for separating out the individual paths
// from the passed path string
HRESULT DebuggerFilePathCache::InitPathArray (__in_z WCHAR *pstrName)
{
    bool bDone = false;
    int iBegin;
    int iEnd;
    int iCounter = 0;
    int iIndex = 0;

    // first, free the existing array members (if any)
    while (m_iPathCount-- > 0)
    {
        delete [] m_rstrPath [m_iPathCount];
        m_rstrPath [m_iPathCount] = NULL;
    }

    MAKE_ANSIPTR_FROMWIDE(nameA, pstrName);
    _ASSERTE (nameA != NULL);
    if (nameA == NULL)
        return (E_OUTOFMEMORY);


    while (bDone == false)
    {
        iBegin = iCounter;
        while ((nameA [iCounter] != ENVIRONMENT_SEPARATOR) && (nameA [iCounter] != '\0'))
            iCounter++;

        if (nameA [iCounter] == '\0')
            bDone = true;

        iEnd = iCounter++;

        if (iEnd != iBegin)
        {
            int iStrLen = iEnd - iBegin;

            _ASSERTE (iStrLen > 0);
            _ASSERTE (iIndex < MAX_PATH_ELEMS);
            if ((m_rstrPath [iIndex] = new CHAR [iStrLen + 1]) != NULL)
            {
                // copy the extracted string
                strncpy (m_rstrPath [iIndex], &(nameA [iBegin]), iStrLen);

                // null terminate
                m_rstrPath [iIndex][iStrLen] = L'\0';

                iIndex++;
            }
            else
                return (E_OUTOFMEMORY);
        }
    }

    m_iPathCount = iIndex;

    return (S_OK);
}



int DebuggerFilePathCache::GetFileFromCache(DebuggerModule *pModule,
                                            ISymUnmanagedDocument *doc,
                                            __deref_out_z_opt CHAR **ppstrFName)
{
    *ppstrFName = NULL;
    if ((m_iCacheCount == 0) || (pModule == NULL) || !doc)
        return -1;

    for (int i=0; i<m_iCacheCount; i++)
    {

        if (m_rDocs [i] == doc)
        {
            // check if the module names also match

            // allocate memory and store the data
            WCHAR strModuleName [MAX_PATH+1];
            ULONG32 NameLength;

            // Initialize module name to null
            strModuleName [0] = L'\0';

            // Now get the module name
            pModule ->GetICorDebugModule()->GetName(MAX_PATH, &NameLength, strModuleName);

            // Convert module name to ANSI and store
            MAKE_ANSIPTR_FROMWIDE (ModNameA, strModuleName);
            _ASSERTE (ModNameA != NULL);

            // Convert the module name to lowercae before comparing
            char *pszTemp = ModNameA;

            while (*pszTemp != '\0')
            {
                *pszTemp = tolower (*pszTemp);
                pszTemp++;
            }

            if (!strcmp (ModNameA, m_rpstrModName [i]))
            {
                // The names match. So return the source file name
                _ASSERTE (m_rpstrFullPath[i] != NULL);
                if ((*ppstrFName = new char [strlen(m_rpstrFullPath[i]) + 1]) != NULL)
                {
                    strcpy (*ppstrFName, m_rpstrFullPath[i]);
                }

                // found it. So exit loop.
                return (i);
            }
        }
    }

    return (-1);
}


BOOL    DebuggerFilePathCache::UpdateFileCache (DebuggerModule *pModule,
                                                ISymUnmanagedDocument *doc,
                                                __inout_z_opt CHAR *pFullPath)
{
    char *pszString;

    // first, convert the file name to lowercase
    char *pTemp = pFullPath;

    if (pTemp)
    {
        while (*pTemp)
        {
            *pTemp = tolower (*pTemp);
            pTemp++;
        }
    }

    // check if this is an addition or modification
    int iCacheIndex = GetFileFromCache (pModule, doc, &pszString);

    if (iCacheIndex != -1)
    {
        // if the names match, then no need to do anything. Simply return!
        if (!strcmp (pFullPath, pszString))
        {
            delete [] pszString;
            return true;
        }

        delete [] pszString;

        _ASSERTE (iCacheIndex < m_iCacheCount);
        // an entry already exists - so update it

        // first, delete the existing path
        delete [] m_rpstrFullPath [iCacheIndex];

        if ((m_rpstrFullPath [iCacheIndex] = new char [strlen (pFullPath) +1]) == NULL)
        {
            // free up the memory allocated for module name
            delete [] m_rpstrModName [iCacheIndex];
            m_rpstrModName [iCacheIndex] = NULL;
            m_rDocs [iCacheIndex] = NULL;
            return false;
        }

        strcpy (m_rpstrFullPath [iCacheIndex], pFullPath);
        return true;
    }

    // Create a new entry
    if (pFullPath)
    {
        if (m_iCacheCount < MAX_CACHE_ELEMS)
        {
            m_rpstrModName [m_iCacheCount] = NULL;
            m_rpstrFullPath [m_iCacheCount] = NULL;
            m_rDocs [m_iCacheCount] = NULL;

            // allocate memory and store the data
            WCHAR strModuleName [MAX_PATH+1];
            ULONG32 NameLength;

            // Initialize module name to null
            strModuleName [0] = L'\0';

            // Now get the module name
            pModule ->GetICorDebugModule()->GetName(MAX_PATH, &NameLength, strModuleName);

            // Convert module name to ANSI and store
            MAKE_ANSIPTR_FROMWIDE (ModNameA, strModuleName);
            _ASSERTE (ModNameA != NULL);

            if ((m_rpstrModName [m_iCacheCount] = new char [strlen (ModNameA) +1]) == NULL)
                return false;

            strcpy (m_rpstrModName[m_iCacheCount], ModNameA);

            // convert the module name to lowercase
            char *pszTemp = m_rpstrModName [m_iCacheCount];
            while (*pszTemp != '\0')
            {
                *pszTemp = tolower (*pszTemp);
                pszTemp++;
            }

            // Also store full pathname and document
            if ((m_rpstrFullPath [m_iCacheCount] = new char [strlen (pFullPath) +1]) == NULL)
            {
                // free up the memory alloacted for module name
                delete [] m_rpstrModName [m_iCacheCount];
                m_rpstrModName [m_iCacheCount] = NULL;
                return false;
            }

            strcpy (m_rpstrFullPath [m_iCacheCount], pFullPath);

            m_rDocs [m_iCacheCount] = doc;
            doc->AddRef();

            m_iCacheCount++;

        }
        else
            return false;
    }

    return true;
}



// This sets the full file name as well as the stripped file name
BOOL    ModuleSourceFile::SetFullFileName (ISymUnmanagedDocument *doc,
                                           LPCSTR pstrFullFileName)
{

    m_SFDoc = doc;
    m_SFDoc->AddRef();

    int iLen = MultiByteToWideChar (CP_ACP, 0, pstrFullFileName, -1, NULL, 0);

    if (m_pstrFullFileName.Allocate(iLen))
    {
        if (MultiByteToWideChar (CP_ACP, 0, pstrFullFileName, -1, m_pstrFullFileName.GetData(), iLen))
        {
            // strip the path and store just the lowercase file name
            WCHAR       rcFile[_MAX_FNAME];
            WCHAR       rcExt[_MAX_EXT];

            _wsplitpath(m_pstrFullFileName, NULL, NULL, rcFile, rcExt);
            wcscat(rcFile, rcExt);


            m_pstrStrippedFileName.CopyFrom(rcFile);
        }
        else
            return false;
    }
    else
        return false;

    return true;
}


void DebuggerShell::DumpMemory(BYTE *pbMemory,
                               CORDB_ADDRESS ApparantStartAddr,
                               ULONG32 cbMemory,
                               ULONG32 WORD_SIZE,
                               ULONG32 iMaxOnOneLine,
                               BOOL showAddr)
{
    int nBase;
    WCHAR wsz[20];
    ULONG32 iPadding;
    ULONG32 ibPrev;

    if (m_rgfActiveModes & DSM_DISPLAY_REGISTERS_AS_HEX)
        nBase = 16;
    else
        nBase = 10;

    ULONG32 ib = 0;

    while (ib < cbMemory)
    {
        if ((ib % (WORD_SIZE * iMaxOnOneLine)) == 0)
        {
            // beginning or end of line
            if (ib != 0)
            {
                if (WORD_SIZE == 1)
                {
                    // end of 2nd+line: spit out bytes in ASCII/Unicode
                    Write(L"  ");

                    for (ibPrev = ib - (WORD_SIZE * iMaxOnOneLine); ibPrev < ib; ibPrev++)
                    {
                        BYTE b = *(pbMemory + ibPrev);

                        if (b >= 0x21 && b <= 0x7e) // print only printable characters
                            Write(L"%C", b);
                        else
                            Write(L".");
                    }
                }
            }   //spit out address to be displayed

            if (showAddr)
                Write(L"\n" LFMT_ADDR L"", DBG_ADDR(ApparantStartAddr + ib));
        }

        if ((ib % WORD_SIZE) == 0)
        {
            //put spaces between words
            Write(L" ");
        }

        // print bytes in hex
        BYTE *pThisByte = pbMemory + ib + ((ib % WORD_SIZE) - WORD_SIZE) +
            (((2 * WORD_SIZE) - 1) - ((ib % WORD_SIZE) * (WORD_SIZE -1)));

        _itow((int)*pThisByte, wsz, nBase);

        // make sure to always print at least two characters
        if ((*(pThisByte) < 0x10 && nBase == 16) || (*(pThisByte) < 10 && nBase == 10))
            Write(L"0%s", wsz);
        else
            Write(L"%s", wsz);

        ib++;
    }

    if ((ib % (WORD_SIZE * iMaxOnOneLine)) != 0)
    {
        // stopped halfway through last line put the missing spaces in so this doesn't look weird
        for (iPadding = (WORD_SIZE * iMaxOnOneLine) - (ib % (WORD_SIZE * iMaxOnOneLine)); iPadding > 0; iPadding--)
        {
            if ((iPadding % WORD_SIZE) == 0)
                Write(L" ");

            Write(L" ");
        }

        Write(L" ");
    }

    // print out the characters for the final line
    ibPrev = ib - (ib % (WORD_SIZE * iMaxOnOneLine));

    if (ibPrev == ib) //we landed on the line edge
    {
        ibPrev = ib - (WORD_SIZE * iMaxOnOneLine);
        Write(L"  ");
    }

    if (WORD_SIZE == 1)
    {
        for (; ibPrev < ib; ibPrev++)
        {
            BYTE b = *(pbMemory + ibPrev);

            if ((b < 'A' || b > 'z') && (b != '?'))
                Write(L".");
            else
                Write(L"%C", b);
        }
    }
}

//
// Some very basic filtering of first chance exceptions. This builds a list of exception types to catch or ignore. If
// you pass NULL for exType, it will just print the current list.
//
HRESULT DebuggerShell::HandleSpecificException(const WCHAR *exType, bool shouldCatch)
{
    ExceptionHandlingInfo *i;
    ExceptionHandlingInfo *h = NULL;

    // Find any existing entry.
    for (i = m_exceptionHandlingList; i != NULL; i = i->m_next)
    {
        if ((exType != NULL) && !wcscmp(exType, i->m_exceptionType))
            h = i;
        else
            Write(L"%s %s\n", i->m_catch ? L"Catch " : L"Ignore", i->m_exceptionType.GetData());
    }

    if (exType != NULL)
    {
        // If none was found, make a new one and shove it into the front of the list.
        if (h == NULL)
        {
            h = new ExceptionHandlingInfo();

            if (h == NULL)
                return E_OUTOFMEMORY;

            // Make a copy of the exception type.
            if (!h->m_exceptionType.CopyFrom(exType))
            {
                delete h;
                return E_OUTOFMEMORY;
            }

            h->m_next = m_exceptionHandlingList;
            m_exceptionHandlingList = h;
        }

        // Remember if we should catch or ignore this exception type.
        h->m_catch = shouldCatch;

        Write(L"%s %s\n", h->m_catch ? L"Catch " : L"Ignore", h->m_exceptionType.GetData());
    }

    return S_OK;
}

//
// If we have specific exception handling info for a given exception type, this will return S_OK and fill in
// shouldCatch. Otherwise, returns S_FALSE.
// Since we can specify whether to catch/ignore on an individual class level, a module level, or a process-wide level,
// we may get conflicting orders about whether to catch/ignore an exception. This is resolved
// from most specific to least specific (eg, Class, Module, Process).
//
bool DebuggerShell::ShouldHandleSpecificException(ICorDebugValue *pException)
{
    ReleaseHolder<ICorDebugClass> iclass;
    ReleaseHolder<ICorDebugObjectValue> iobject;
    ReleaseHolder<ICorDebugModule> imodule;
    DebuggerModule *dm = NULL;

    // Default to the global catch/ignore setting for first chance exceptions.
    bool stop = g_pShell->m_catchException;

    // Add an extra reference to pException. StripReferences is going to release it as soon as it dereferences it, but
    // the caller is expecting it to still be alive.
    pException->AddRef();

    // We need the type name out of this exception object.
    HRESULT hr = StripReferences(&pException, false);

    if (FAILED(hr))
        goto Exit;

    // Grab the element type so we can verify its an object.
    CorElementType type;
    hr = pException->GetType(&type);

    if (FAILED(hr))
        goto Exit;

    if ((type != ELEMENT_TYPE_CLASS) && (type != ELEMENT_TYPE_OBJECT))
        goto Exit;

    // It had better be an object by this point...
    hr = pException->QueryInterface(IID_ICorDebugObjectValue, (void **) &iobject);

    if (FAILED(hr))
        goto Exit;

    hr = iobject->GetClass(&iclass);

    if (FAILED(hr))
        goto Exit;

    // Get the class's token
    mdTypeDef tdClass;
    hr = iclass->GetToken(&tdClass);

    if (FAILED(hr))
        goto Exit;

    // Get the module from this class
    iclass->GetModule(&imodule);

    if (FAILED(hr))
        goto Exit;

    dm = DebuggerModule::FromCorDebug(imodule);
    if (dm == NULL)
    {
        _ASSERTE(!"should not have come here!");
        goto Exit;
    }

    ExceptionHandlingInfo *i;


    // Get the class name
    WCHAR       className[MAX_CLASSNAME_LENGTH];
    ULONG       classNameSize;
    mdToken     parentTD;

    hr = dm->GetMetaData()->GetTypeDefProps(tdClass, className, MAX_CLASSNAME_LENGTH, &classNameSize, NULL, &parentTD);

    if (FAILED(hr))
        goto Exit;

    // First check for any existing entry against the class.
    for (i = m_exceptionHandlingList; i != NULL; i = i->m_next)
    {
        if (wcscmp(className, i->m_exceptionType) == 0)
            break;
    }

    // If we've found an extry for this exception type, then handle it based on what the user asked for.
    if (i != NULL)
    {
        stop = i->m_catch;
        goto Exit;
    }

    // No class entry specified, now check for it on the module level.
    for (i = m_exceptionHandlingList; i != NULL; i = i->m_next)
    {
        if (_wcsicmp(dm->GetShortName(), i->m_exceptionType) == 0)
        {
            stop = i->m_catch;

            if (!stop && (g_pShell->m_rgfActiveModes & DSM_ENHANCED_DIAGNOSTICS))
            {
                g_pShell->Write(L"Ignoring exception from module '%s'\n", dm->GetName());
            }

            goto Exit;
        }
    }

    // If no module entry found, then we default to going w/ the process-wide switch.

Exit:
    RELEASE(pException); // matches explicit addref above.
    return stop;
}

//
// Do a command once for every thread in the process.
//
void DebuggerShell::DoCommandForAllThreads(const WCHAR *string)
{
    HRESULT hr;
    ICorDebugThreadEnum *e = NULL;
    ICorDebugThread *ithread = NULL;
    ULONG threadsExecuted = 0;

    // Must have a current process.
    if (m_currentProcess == NULL)
    {
        Error(L"Process not running.\n");
        goto Exit;
    }

    // Enumerate the process' threads
    hr = m_currentProcess->EnumerateThreads(&e);

    if (FAILED(hr))
    {
        ReportError(hr);
        goto Exit;
    }

    ULONG totalThreads;  // indicates how many records were retrieved

    hr = e->GetCount(&totalThreads);

    if (FAILED(hr))
    {
        ReportError(hr);
        goto Exit;
    }

    // Alert user if there's no threads.
    if (totalThreads == 0)
    {
        Write(L"There are no managed threads\n");
        goto Exit;
    }

    m_stopLooping = false;

    // Execute the command once for each thread in the process
    ULONG count;
    for (hr = e->Next(1, &ithread, &count);
         SUCCEEDED(hr) && (count == 1) && !m_stopLooping && (m_currentProcess != NULL);
         hr = e->Next(1, &ithread, &count))
    {
        // Make this thread the current thread.
        SetCurrentThread(m_currentProcess, ithread);
        SetDefaultFrame();

        Write(L"\n\n");
        PrintThreadState(ithread);

        // Execute the command in the context of this thread.
        DoCommand(string);

        RELEASE(ithread);
        ++threadsExecuted;
    }

    // If the call to Next fails...
    if (FAILED(hr) && threadsExecuted != totalThreads)
    {
        ReportError(hr);
        goto Exit;
    }

Exit:
    if (e)
        RELEASE(e);
}


/* ------------------------------------------------------------------------- *
 * Methods for a signature formatter, stolen from the internals of the Runtime
 * ------------------------------------------------------------------------- */

SigFormat::SigFormat(IMetaDataImport *importer, PCCOR_SIGNATURE sigBlob, ULONG sigBlobSize, const WCHAR *methodName)
{
    _fmtSig = NULL;
    _size = 0;
    _pos = 0;
    _sigBlob = sigBlob;
    _sigBlobSize = sigBlobSize;
    _memberName = methodName;
    _importer = importer;
}

SigFormat::~SigFormat()
{
    if (_fmtSig)
        delete _fmtSig;
}

WCHAR *SigFormat::GetString()
{
    return _fmtSig;
}

#define SIG_INC 256

int SigFormat::AddSpace()
{
    if (_pos == _size) {
        WCHAR* temp = new WCHAR[_size+SIG_INC];
        if (!temp)
            return 0;
        memcpy(temp,_fmtSig,_size);
        delete _fmtSig;
        _fmtSig = temp;
        _size+=SIG_INC;
    }
    _fmtSig[_pos] = ' ';
    _fmtSig[++_pos] = 0;
    return 1;
}

int SigFormat::AddString(const WCHAR *s)
{
    int len = (int)wcslen(s);
    // Allocate on overflow
    if (_pos + len >= _size) {
        int newSize = (_size+SIG_INC > _pos + len) ? _size+SIG_INC : _pos + len + SIG_INC;
        WCHAR* temp = new WCHAR[newSize];
        if (!temp)
            return 0;
        memcpy(temp,_fmtSig,_size);
        delete _fmtSig;
        _fmtSig = temp;
        _size=newSize;
    }
    wcscpy(&_fmtSig[_pos],s);
    _pos += len;
    return 1;
}

HRESULT SigFormat::FormatSig()
{
    _size = SIG_INC;
    _pos = 0;
    _fmtSig = new WCHAR[_size];

    ULONG cb = 0;

    // Calling convention
    ULONG conv = _sigBlob[cb++];

    // Type arg count
    ULONG cTyArgs = 0;
    if (conv & IMAGE_CEE_CS_CALLCONV_GENERIC)
      cb += CorSigUncompressData(&_sigBlob[cb], &cTyArgs);

    // Arg count
    ULONG cArgs;
    cb += CorSigUncompressData(&_sigBlob[cb], &cArgs);

    // Return type
    cb += AddType(&_sigBlob[cb]);
    AddSpace();

    if (_memberName != NULL)
        AddString(_memberName);
    else
        AddSpace();

    AddString(L"(");

    // Loop through all of the args
    for (UINT i = 0; i < cArgs; i++)
    {
       cb += AddType(&_sigBlob[cb]);

       if (i != cArgs - 1)
           AddString(L", ");
    }

    // Display vararg signature at end
    if (conv == IMAGE_CEE_CS_CALLCONV_VARARG)
    {
        if (cArgs)
            AddString(L", ");

        AddString(L"...");
    }

    AddString(L")");

    return S_OK;
}

ULONG SigFormat::AddType(PCCOR_SIGNATURE sigBlob)
{
    ULONG cb = 0;

    CorElementType type = (CorElementType)sigBlob[cb++];

    // Format the output
    switch (type)
    {
    case ELEMENT_TYPE_VOID:     AddString(L"Void"); break;
    case ELEMENT_TYPE_BOOLEAN:  AddString(L"Boolean"); break;
    case ELEMENT_TYPE_I1:       AddString(L"SByte"); break;
    case ELEMENT_TYPE_U1:       AddString(L"Byte"); break;
    case ELEMENT_TYPE_I2:       AddString(L"Int16"); break;
    case ELEMENT_TYPE_U2:       AddString(L"UInt16"); break;
    case ELEMENT_TYPE_CHAR:     AddString(L"Char"); break;
    case ELEMENT_TYPE_I4:       AddString(L"Int32"); break;
    case ELEMENT_TYPE_U4:       AddString(L"UInt32"); break;
    case ELEMENT_TYPE_I8:       AddString(L"Int64"); break;
    case ELEMENT_TYPE_U8:       AddString(L"UInt64"); break;
    case ELEMENT_TYPE_R4:       AddString(L"Single"); break;
    case ELEMENT_TYPE_R8:       AddString(L"Double"); break;
    case ELEMENT_TYPE_OBJECT:   AddString(L"Object"); break;
    case ELEMENT_TYPE_STRING:   AddString(L"String"); break;
    case ELEMENT_TYPE_I:        AddString(L"Int"); break;
    case ELEMENT_TYPE_U:        AddString(L"UInt"); break;

    case ELEMENT_TYPE_GENERICINST :
      {
        cb += AddType(&sigBlob[cb]);
        DWORD n;
        cb += CorSigUncompressData(&sigBlob[cb], &n);
        AddString(L"<");
        for (DWORD i = 0; i < n; i++)
        {
            if (i > 0)
                AddString(L",");
            cb += AddType(&sigBlob[cb]);
        }
        AddString(L">");
        break;
      }


    case ELEMENT_TYPE_MVAR :
      {
        DWORD ix;
        cb += CorSigUncompressData(&sigBlob[cb], &ix);
        WCHAR smallbuf[20];
        wsprintf(smallbuf, L"!!%d", ix);
        AddString(smallbuf);
      }
      break;

    case ELEMENT_TYPE_VAR :
      {
        DWORD ix;
        cb += CorSigUncompressData(&sigBlob[cb], &ix);
        AddString(L"!");
        WCHAR smallbuf[20];
        wsprintf(smallbuf, L"!%d", ix);
        AddString(smallbuf);
      }
      break;


    case ELEMENT_TYPE_VALUETYPE:
    case ELEMENT_TYPE_CLASS:
        {
            mdToken tk;

            cb += CorSigUncompressToken(&sigBlob[cb], &tk);

            MDUTF8CSTR szUtf8NamePtr;

            HRESULT hr;
            hr = _importer->GetNameFromToken(tk, &szUtf8NamePtr);

            if (SUCCEEDED(hr))
            {
                MAKE_WIDEPTR_FROMUTF8(nameW, szUtf8NamePtr);
                AddString(nameW);
            }
            else
                AddString(L"**Unknown Type**");

            break;
        }
    case ELEMENT_TYPE_TYPEDBYREF:
        {
            AddString(L"TypedReference");
            break;
        }

    case ELEMENT_TYPE_BYREF:
        {
            cb += AddType(&sigBlob[cb]);
            AddString(L" ByRef");
        }
        break;

    case ELEMENT_TYPE_SZARRAY:      // Single Dim, Zero
        {
            cb += AddType(&sigBlob[cb]);
            AddString(L"[]");
        }
        break;

    case ELEMENT_TYPE_ARRAY:        // General Array
        {
            cb += AddType(&sigBlob[cb]);

            AddString(L"[");

            // Skip over rank
            ULONG rank;
            cb += CorSigUncompressData(&sigBlob[cb], &rank);

            if (rank > 0)
            {
                // how many sizes?
                ULONG sizes;
                cb += CorSigUncompressData(&sigBlob[cb], &sizes);

                // read out all the sizes
                unsigned int i;

                for (i = 0; i < sizes; i++)
                {
                    ULONG dimSize;
                    cb += CorSigUncompressData(&sigBlob[cb], &dimSize);

                    if (i > 0)
                        AddString(L",");
                }

                // how many lower bounds?
                ULONG lowers;
                cb += CorSigUncompressData(&sigBlob[cb], &lowers);

                // read out all the lower bounds.
                for (i = 0; i < lowers; i++)
                {
                    int lowerBound;
                    cb += CorSigUncompressSignedInt(&sigBlob[cb], &lowerBound);
                }
            }

            AddString(L"]");
        }
        break;

    case ELEMENT_TYPE_PTR:
        {
            cb += AddType(&sigBlob[cb]);
            AddString(L"*");
            break;
        }

    case ELEMENT_TYPE_CMOD_REQD:
        AddString(L"CMOD_REQD");
        cb += AddType(&sigBlob[cb]);
        break;

    case ELEMENT_TYPE_CMOD_OPT:
        AddString(L"CMOD_OPT");
        cb += AddType(&sigBlob[cb]);
        break;

    case ELEMENT_TYPE_MODIFIER:
        cb += AddType(&sigBlob[cb]);
        break;

    case ELEMENT_TYPE_PINNED:
        AddString(L"pinned");
        cb += AddType(&sigBlob[cb]);
        break;

    case ELEMENT_TYPE_SENTINEL:
        break;

    default:
        AddString(L"**UNKNOWN TYPE**");
    }

    return cb;
}

// Constructor for NamedHandle
NamedHandle::NamedHandle(WCHAR *pwzHandleName, ICorDebugHandleValue *pHandleValue)
{
    m_pwzHandleName = pwzHandleName;
    m_pHandle = pHandleValue;
    m_pNext = NULL;
}   // NamedHandle

// Destructor for NamedHandle
NamedHandle::~NamedHandle()
{
    if (m_pHandle)
    {
        m_pHandle->Release();
    }
    if (m_pwzHandleName)
    {
        delete m_pwzHandleName;
    }
}   // ~NamedHandle

