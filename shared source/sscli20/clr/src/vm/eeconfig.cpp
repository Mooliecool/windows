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
// EEConfig.H -
//
// Fetched configuration data from the registry (should we Jit, run GC checks ...)
//
//
#include "common.h"
#include <xmlparser.h>
#include <mscorcfg.h>
#include "eeconfig.h"
#include "method.hpp"
#include "eeconfigfactory.h"
#include "fusionsetup.h"
#include "eventtrace.h"
#include "timeline.h"
#include "eehash.h"
#include "eemessagebox.h"
#include "corhost.h"

#define DEFAULT_ZAP_SET L""

#if defined(_DEBUG) && !defined(GC_SMP)
#define DEFAULT_APP_DOMAIN_LEAKS 1
#else
#define DEFAULT_APP_DOMAIN_LEAKS 0
#endif

ConfigSource::ConfigSource()
{
    CONTRACTL {
        NOTHROW;
        GC_NOTRIGGER;
        MODE_ANY;
        FORBID_FAULT;
    } CONTRACTL_END;

        
    m_pNext = this;
    m_pPrev = this;
    m_Table = NULL;
}// ConfigSource::ConfigSource

ConfigSource::~ConfigSource()
{
    CONTRACTL {
        NOTHROW;
        FORBID_FAULT;
        GC_NOTRIGGER;
        MODE_ANY;
    } CONTRACTL_END;
    EEHashTableIteration iter;
    m_Table->IterateStart(&iter);
    while(m_Table->IterateNext(&iter)) {
        LPWSTR pValue = (LPWSTR) m_Table->IterateGetValue(&iter);
        delete [] pValue;
    }
    delete m_Table;
    m_Table = NULL;
}// ConfigSource::~ConfigSource

EEUnicodeStringHashTable* ConfigSource::Table()
{   
    LEAF_CONTRACT;
    return m_Table;
}// ConfigSource::Table

HRESULT ConfigSource::Init()
{
    CONTRACTL {
        NOTHROW;
        GC_NOTRIGGER;
        MODE_ANY;
    } CONTRACTL_END;

    HRESULT hr = S_OK;

    EX_TRY
    {
        _ASSERTE(m_Table == NULL);
        m_Table = new EEUnicodeStringHashTable();
        if (!m_Table->Init(100,NULL))
            hr = E_OUTOFMEMORY;
    }
    EX_CATCH_HRESULT(hr);

    return hr;
}// Init

void ConfigSource::Add(ConfigSource* prev)
{
    CONTRACTL {
        NOTHROW;
        GC_NOTRIGGER;
        MODE_ANY;
        PRECONDITION(CheckPointer(prev));
        PRECONDITION(CheckPointer(prev->m_pNext));
    } CONTRACTL_END;
    
    m_pPrev = prev;
    m_pNext = prev->m_pNext;

    m_pNext->m_pPrev = this;
    prev->m_pNext = this;
}// ConfigSource::Add



/**************************************************************/
// Poor mans narrow
LPUTF8 NarrowWideChar(__inout_z LPWSTR str)
{
    CONTRACT (LPUTF8)
    {
        NOTHROW;
        GC_NOTRIGGER;
        MODE_ANY;
        PRECONDITION(CheckPointer(str, NULL_OK));
        POSTCONDITION(CheckPointer(RETVAL, NULL_OK));
    } CONTRACT_END;

    if (str != 0) { 
        LPWSTR fromPtr = str;
        LPUTF8 toPtr = (LPUTF8) str;
        LPUTF8 result = toPtr;
        while(*fromPtr != 0)
            *toPtr++ = (char) *fromPtr++;
        *toPtr = 0;
        RETURN result;
    }
    RETURN NULL;
}

extern void UpdateGCSettingFromHost ();

HRESULT EEConfig::Setup()
{
    CONTRACTL {
        NOTHROW;
        GC_TRIGGERS;
        MODE_ANY;
    } CONTRACTL_END;

    ETWTraceStartup trace(ETW_TYPE_STARTUP_EECONFIG_SETUP);
    TIMELINE_AUTO(STARTUP, "EEConfig::Setup");
        
    // This 'new' uses EEConfig's overloaded new, which uses a static memory buffer and will
    // not fail
    EEConfig *pConfig = new EEConfig();

    HRESULT hr = pConfig->Init();

    if (FAILED(hr))
        return hr;

    PVOID pv = NULL;
    pv = InterlockedCompareExchangePointer(
        (PVOID *) &g_pConfig, (PVOID) pConfig, NULL);

    _ASSERTE(pv == NULL && "EEConfig::Setup called multiple times!");
    
    UpdateGCSettingFromHost();

    return S_OK;
}

//<><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><>
#ifdef THREAD_DELAY
ThreadDelay::ThreadDelay ()
{
    m_iThreadDelayFrequency = 0;
    m_iThreadDelayDuration = 0;
    m_iThreadSpikeDelayFrequency = 0;
    m_iThreadSpikeDelayDuration = 0;
    m_iSpikeCount = 0;
    m_fSpikeOn = m_fDelayOn = false;
    srand( (unsigned)time( NULL ) );
}     

void ThreadDelay::init ()
{
    //initialize short delay values
    m_iThreadDelayFrequency = EEConfig::GetConfigDWORD (L"ThreadDelayFrequency", m_iThreadDelayFrequency);

    //Frequency must be between 0 (no delay) and 100 milliseconds
    m_iThreadDelayFrequency = m_iThreadDelayFrequency > 0 ? m_iThreadDelayFrequency %101 : 0;   

    m_iThreadDelayDuration = EEConfig::GetConfigDWORD (L"ThreadDelayDuration", m_iThreadDelayDuration);

    //Duration must be between 0 (no delay) and 100 milliseconds
    m_iThreadDelayDuration = m_iThreadDelayDuration > 0 ?  m_iThreadDelayDuration % 101 : 0;    

    m_fDelayOn = m_iThreadDelayFrequency > 0 && m_iThreadDelayDuration > 0;
    


  //initialize long delay values
    m_iThreadSpikeDelayFrequency = EEConfig::GetConfigDWORD(L"ThreadSpikeDelayFrequency", m_iThreadSpikeDelayFrequency);
        if (m_iThreadSpikeDelayFrequency < 0)
          m_iThreadSpikeDelayFrequency = 0;

    m_iThreadSpikeDelayDuration = EEConfig::GetConfigDWORD(L"ThreadSpikeDelayDuration", m_iThreadSpikeDelayDuration);
    if (m_iThreadSpikeDelayDuration < 0)
      m_iThreadSpikeDelayDuration = 0;

    m_fSpikeOn = m_iThreadSpikeDelayFrequency > 0 && m_iThreadSpikeDelayDuration > 0;
}  

 
void ThreadDelay::SpikeDelay ()
{
    //implements a long delay to simulate something like a page fault. 
    //The frequency value should be relatively large--the larger the value, the less often the spike will occur
    if (m_fSpikeOn)
    {
        ++m_iSpikeCount;
        if (m_iSpikeCount > m_iThreadSpikeDelayFrequency)
        {
            m_iSpikeCount = 0;
            ClrSleepEx (m_iThreadSpikeDelayDuration, FALSE); 
        }
    }
}

void ThreadDelay::ShortDelay ()
{
    //implements a short delay 
    int iRand = 0;

    if (m_fDelayOn) 
    {
        iRand = (rand() % 100);

        // determine whether to delay
        if (iRand < m_iThreadDelayFrequency) 
        { 
            ClrSleepEx(m_iThreadDelayDuration, FALSE);   
        }
    }
}

#endif //THREAD_DELAY
//<><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><>

/**************************************************************/
// For in-place constructor
BYTE g_EEConfigMemory[sizeof(EEConfig)];

void *EEConfig::operator new(size_t size)
{
    CONTRACT(void*) {
        FORBID_FAULT;
        GC_NOTRIGGER;
        NOTHROW;
        MODE_ANY;
        POSTCONDITION(CheckPointer(RETVAL));
    } CONTRACT_END;

    RETURN g_EEConfigMemory;
}

/**************************************************************/
HRESULT EEConfig::Init()
{
    CONTRACTL {
        NOTHROW;
        GC_TRIGGERS;
        MODE_ANY;
    } CONTRACTL_END;

    fInited = false;

    iGCStress = 0;
    iGCHeapVerify = 0;          // Heap Verification OFF by default
    iGCtraceStart = INT_MAX; // Set to huge value so GCtrace is off by default
    iGCtraceEnd = INT_MAX;
    iGCtraceFac = 0;
    iGCprnLvl = DEFAULT_GC_PRN_LVL;
#ifdef THREAD_DELAY
   m_pThreadDelay = NULL;
#endif //THREAD_DELAY
    fGCBreakOnOOM = false;
    iGCgen0size = 0;
    iGCSegmentSize = 0;
    iGCconcurrent = 0;
    iGCForceCompact = 0;
    m_fFreepZapSet = false;
    iJitOptimizeType = OPT_DEFAULT;
    fJitLooseExceptOrder = false;
    fJitFramed = false;
    fJitAlignLoops = false;
    fLegacyNullReferenceExceptionPolicy = false;
    fLegacyUnhandledExceptionPolicy = false;
    fLegacyApartmentInitPolicy = false;
    fLegacyComHierarchyVisibility = false;
    fLegacyComVTableLayout = false;
    fNewComVTableLayout = false;
    iImpersonationPolicy = IMP_DEFAULT;
    fLegacyV1CASPolicy = false;
    fLegacyLoadMscorsnOnStartup = false;
    fTransparencyEnforcementDisabled = false;
    fStressLog = false;
    fCacheBindingFailures = true;
    fDisableCommitThreadStack = false;
    fProbeForStackOverflow = true;
    
    INDEBUG(fStressLog = true;)

#ifdef _DEBUG
    fDebugInfo = false;
    fDebuggable = false;
    fStressOn = false;
    apiThreadStressCount = 0;
    pPrestubHalt = 0;
    pPrestubGC = 0;
    pszBreakOnClassLoad = 0;
    pszBreakOnClassBuild = 0;
    fAppendFileNameToTypeName = true;
    pszBreakOnMethodName = 0;
    pszDumpOnClassLoad = 0;
    pszBreakOnInteropStubSetup = 0;
    pszBreakOnComToClrNativeInfoInit = 0;
    pszBreakOnStructMarshalSetup = 0;
    fJitVerificationDisable= false;
    fVerifierOff           = false;
    fVerifierBreakOnError  = false;
    pVerifierSkip          = NULL;
    pVerifierBreak         = NULL;
    iVerifierBreakOffset   = -1;
    iVerifierBreakPass     = -1;
    fVerifierBreakOffset   = false;
    fVerifierBreakPass     = false;
    fVerifierMsgMethodInfoOff = false;
    fDoAllowUntrustedCallerChecks = true;
    iPerfNumAllocsThreshold = 0;
    iPerfAllocsSizeThreshold = 0;
    pPerfTypesToLog = NULL;
    iFastGCStress = 0;
    iInjectFatalError = 0;
    fSaveThreadInfo = FALSE;
    dwSaveThreadInfoMask = (DWORD)-1;
    
    // TlbImp Stuff
    fTlbImpSkipLoading = false;


    // For now, give our suspension attempts 40 seconds to succeed before trapping to
    // the debugger.   Note that we should probably lower this when the JIT is run in
    // preemtive mode, as we really should not be starving the GC for 10's of seconds
    m_SuspendDeadlockTimeout = 40000;
#endif // _DEBUG
#ifdef _X86_
    dwCpuFlag  = 0;
    dwCpuCapabilities = 0;
#endif

#ifdef _DEBUG
    m_fAssertOnBadImageFormat = false;
    m_fAssertOnFailFast = true;

    fSuppressChecks = false;
    fConditionalContracts = false;
    fEnableFullDebug = false;
#endif
    DoubleArrayToLargeObjectHeap = 1000;

    fUseZaps = true;
    iRequireZaps = REQUIRE_ZAPS_NONE;

    fSecurityNeutralCode = false;
    fLazyActivation = false;

    fUsePcRel32Calls = true;
    fUseRipData      = false;

    // new loader behavior switches

    fExactInterfaceCalls = true;
    fLazyInterfaceRestore = false; // true                                              

    m_fDeveloperInstallation = false;

    //@GENERICS:
#if defined(FEATURE_SHARE_GENERIC_CODE)  
    fShareGenericCode = true;
    fFullGenericsNGEN = true;
#else
    fFullGenericsNGEN = false;
#endif

    fFullArraysNGEN = false;
    fDontLoadOpenTypes = false;

    pZapSet = DEFAULT_ZAP_SET;

    dwSharePolicy = AppDomain::SHARE_POLICY_UNSPECIFIED;

    dwMonitorZapStartup = 0;
    dwMonitorZapExecution = 0;
    szMonitorZapExecutionModule = NULL;
    dwMemmapEnabled = 0;

    dwMetaDataPageNumber = 0xFFFFFFFF;
    fShowMetaDataAccess = 0;
    szMetaDataFileName = L"";

    dwDisableStackwalkCache = 1;

    fUseNewCrossDomainRemoting = 1;
    
    fDoBBInstr       = false;
    szZapBBInstr     = NULL;
    szZapBBInstrDir  = NULL;
    dwInstrEnabled    = 0;

    fAppDomainUnload = true;
    dwADURetryCount=1000;

#ifdef _DEBUG
    fAppDomainLeaks = DEFAULT_APP_DOMAIN_LEAKS;
#endif

    // interop logging
    m_pTraceIUnknown = NULL;
    m_TraceWrapper = 0;

    fContinueAfterFatalError = FALSE;

    iNgenHardBind = NGEN_HARD_BIND_DEFAULT;

#ifdef _DEBUG
    fGenerateStubForHost = FALSE;

    fShouldInjectFault = 0;

    testThreadAbort = 0;

    testADUnload = 0;
#endif

    m_fRejectSafeHandleFinalizers = FALSE;

#if defined(_DEBUG) && defined(STUBLINKER_GENERATES_UNWIND_INFO)
    fStubLinkerUnwindInfoVerificationOn = FALSE;
#endif


    return S_OK;
}


#ifdef _DEBUG
static int DumpConfigTable(EEUnicodeStringHashTable* table, __in_z LPCSTR label, int count)
{
    EEHashTableIteration tableIter;
    table->IterateStart(&tableIter);
    LOG((LF_ALWAYS, LL_ALWAYS, label, count++));
    LOG((LF_ALWAYS, LL_ALWAYS, "*********************************\n", count++));
    while(table->IterateNext(&tableIter)) 
    {
        EEStringData* key = table->IterateGetKey(&tableIter);
        LPCWSTR keyString = key->GetStringBuffer();
        LPCWSTR data = (LPCWSTR) table->IterateGetValue(&tableIter);
        LOG((LF_ALWAYS, LL_ALWAYS, "%S = %S\n", keyString, data));
    }
    LOG((LF_ALWAYS, LL_ALWAYS, "\n"));
    return count;
}
#endif

/**************************************************************/
HRESULT EEConfig::Cleanup()
{
    CONTRACTL {
        FORBID_FAULT;
        NOTHROW;
        GC_NOTRIGGER;
        MODE_ANY;
    } CONTRACTL_END;
    
#ifdef _DEBUG
    if (g_pConfig) {
        FAULT_NOT_FATAL();  // if GetConfigDWORD fails the alloc, that's ok
        DWORD setting = g_pConfig->GetConfigDWORD(L"DumpConfiguration", 0);
        if (setting != 0) 
       {
            ConfigList::ConfigIter iter(&m_Configuration);
            int count = 0;
            for(EEUnicodeStringHashTable* table = iter.Next();table; table = iter.Next()) 
            {
                count = DumpConfigTable(table, "\nSystem Configuration Table: %d\n", count);
            }
            ConfigList::ConfigIter iter2(&m_Configuration);
            count = 0;
            for (EEUnicodeStringHashTable* table = iter2.Previous();table; table = iter2.Previous()) 
            {
                count = DumpConfigTable(table, "\nApplication Configuration Table: %d\n", count);
            }
        }
    }
#endif

    if (m_fFreepZapSet)
        delete[] pZapSet;
    delete[] szZapBBInstr;
    
    if (pRequireZapsList)
        delete pRequireZapsList;
    
    if (pRequireZapsExcludeList)
        delete pRequireZapsExcludeList;

#ifdef _DEBUG
    if (pPrestubHalt)
    {
        DestroyMethList(pPrestubHalt);
        pPrestubHalt = NULL;
    }
    if (pPrestubGC)
    {
        DestroyMethList(pPrestubGC);
        pPrestubGC = NULL;
    }
    delete [] pszBreakOnClassLoad;
    delete [] pszBreakOnClassBuild;
    delete [] pszBreakOnMethodName;
    delete [] pszDumpOnClassLoad;
    delete [] pszBreakOnInteropStubSetup;
    delete [] pszBreakOnComToClrNativeInfoInit;
    delete [] pszBreakOnStructMarshalSetup;
    delete [] pszGcCoverageOnMethod;
#endif
#ifdef _DEBUG
    if (pVerifierSkip)
    {
        DestroyMethList(pVerifierSkip);
        pVerifierSkip = NULL;
    }
    if (pVerifierBreak)
    {    
        DestroyMethList(pVerifierBreak);
        pVerifierBreak = NULL;
    }
    if (pPerfTypesToLog)
    {
        DestroyTypeList(pPerfTypesToLog);
        pPerfTypesToLog = NULL;
    }
#endif

    return S_OK;
}

/**************************************************************/

HRESULT EEConfig::GetConfigString(__in_z LPCWSTR name, __deref_out_z LPWSTR *outVal, BOOL fPrependCOMPLUS, ConfigSearch direction)
{ 
    CONTRACT(HRESULT) {
        NOTHROW;
        GC_NOTRIGGER;
        MODE_ANY;
        INJECT_FAULT (CONTRACT_RETURN E_OUTOFMEMORY);
        PRECONDITION(CheckPointer(name));
        POSTCONDITION(CheckPointer(outVal, NULL_OK));
    } CONTRACT_END;

    LPWSTR pvalue = REGUTIL::GetConfigString(name, fPrependCOMPLUS); 
    if(pvalue == NULL && g_pConfig != NULL)
    {
        LPCWSTR pResult;
        if(SUCCEEDED(g_pConfig->GetConfiguration(name, direction, &pResult)) && pResult != NULL)
        {
            size_t len = wcslen(pResult) + 1;
            pvalue = new (nothrow) WCHAR[len];
            if (pvalue == NULL)
            {
                RETURN E_OUTOFMEMORY;
            }
            
            wcscpy_s(pvalue,len,pResult);
        }
    }

    *outVal = pvalue;
        
    RETURN S_OK;
}

DWORD EEConfig::GetConfigDWORD(__in_z LPCWSTR name, DWORD defValue, DWORD level, BOOL fPrependCOMPLUS, ConfigSearch direction)
{    
    CONTRACTL {
        NOTHROW;
        GC_NOTRIGGER;
        MODE_ANY;
        PRECONDITION(CheckPointer(name));
    } CONTRACTL_END;

    DWORD result = REGUTIL::GetConfigDWORD(name, defValue, (REGUTIL::CORConfigLevel)level, fPrependCOMPLUS); 
    if(result == defValue && g_pConfig != NULL)
    {
        LPCWSTR pvalue;
        if(SUCCEEDED(g_pConfig->GetConfiguration(name, direction, &pvalue)) && pvalue != NULL)
        {
            WCHAR *end;
            errno = 0;
            result = wcstoul(pvalue, &end, 0);
            // errno is ERANGE if the number is out of range, and end is set to pvalue if
            // no valid conversion exists.
            if (errno == ERANGE || end == pvalue)
            {
                result = defValue;
            }
        }
    }

    return result;
}

//
// This is very similar to GetConfigDWORD, except that it favors the settings in config files over those in the
// registry. This is the Shim's policy with configuration flags, and there are a few flags in EEConfig that adhere
// to this policy.
//
DWORD EEConfig::GetConfigDWORDFavoringConfigFile(__in_z LPCWSTR name,
                                                 DWORD defValue,
                                                 DWORD level,
                                                 BOOL fPrependCOMPLUS,
                                                 ConfigSearch direction)
{    
    CONTRACTL
    {
        NOTHROW;
        GC_NOTRIGGER;
        MODE_ANY;
        PRECONDITION(CheckPointer(name));
    } CONTRACTL_END;

    DWORD result = defValue;

    if (g_pConfig != NULL)
    {
        LPCWSTR pvalue;
        if (SUCCEEDED(g_pConfig->GetConfiguration(name, direction, &pvalue)) && pvalue != NULL)
        {
            WCHAR *end = NULL;
            errno = 0;
            result = wcstoul(pvalue, &end, 0);
            // errno is ERANGE if the number is out of range, and end is set to pvalue if
            // no valid conversion exists.
            if (errno == ERANGE || end == pvalue)
            {
                result = defValue;
            }
        }
        else
        {
            result = REGUTIL::GetConfigDWORD(name, defValue, (REGUTIL::CORConfigLevel)level, fPrependCOMPLUS);
        }
    }

    return result;
}

DWORD EEConfig::GetConfigDWORDInternal(__in_z LPCWSTR name, DWORD defValue, DWORD level, BOOL fPrependCOMPLUS, ConfigSearch direction)
{    
    CONTRACTL {
        NOTHROW;
        GC_NOTRIGGER;
        MODE_ANY;
        PRECONDITION(CheckPointer(name));
    } CONTRACTL_END;

    DWORD result = REGUTIL::GetConfigDWORD(name, defValue, (REGUTIL::CORConfigLevel)level, fPrependCOMPLUS); 
    if(result == defValue)
    {
        LPCWSTR pvalue;
        if(SUCCEEDED(GetConfiguration(name, direction, &pvalue)) && pvalue != NULL)
        {
            WCHAR *end = NULL;
            errno = 0;
            result = wcstoul(pvalue, &end, 0);
            // errno is ERANGE if the number is out of range, and end is set to pvalue if
            // no valid conversion exists.
            if (errno == ERANGE || end == pvalue)
            {
                result = defValue;
            }
        }
    }
    return result;
}

DWORD EEConfig::GetConfigFlag(__in_z LPCWSTR name, DWORD bitToSet, bool defValue)
{ 
    CONTRACTL {
        NOTHROW;
        GC_NOTRIGGER;
        MODE_ANY;
        PRECONDITION(CheckPointer(name));
    } CONTRACTL_END;
    return REGUTIL::GetConfigFlag(name, bitToSet, defValue); 
}

/**************************************************************/

HRESULT EEConfig::sync()
{
    CONTRACTL {
        THROWS;
        GC_NOTRIGGER;
        MODE_ANY;
        INJECT_FAULT (return E_OUTOFMEMORY);
    } CONTRACTL_END;

    ETWTraceStartup trace(ETW_TYPE_STARTUP_EECONFIG_SYNC);

    HRESULT hr = S_OK;

    // Note the global variable is not updated directly by the GetRegKey function
    // so we only update it once (to avoid reentrancy windows)

#ifdef _DEBUG
    iFastGCStress       = GetConfigDWORD(L"FastGCStress", iFastGCStress);

    IfFailRet(GetConfigString(L"GcCoverage", (LPWSTR*)&pszGcCoverageOnMethod));
    pszGcCoverageOnMethod = NarrowWideChar((LPWSTR)pszGcCoverageOnMethod);
#endif

    int forceGCconcurrent = GetConfigDWORD(L"gcConcurrent", (DWORD) -1);
    if ((forceGCconcurrent > 0) || (forceGCconcurrent == -1 && g_IGCconcurrent))
        iGCconcurrent = TRUE;

#ifdef STRESS_HEAP    
    BOOL bGCStressAndHeapVerifyAllowed = true;
    iGCStress           =  GetConfigDWORD(L"GCStress", iGCStress );

    if (iGCStress)
    {
        LPWSTR pszGCStressExe = NULL;
        IfFailRet(GetConfigString(L"RestrictedGCStressExe", &pszGCStressExe));
        if (pszGCStressExe != NULL)
        {
            if (*pszGCStressExe != L'\0')
            {
                bGCStressAndHeapVerifyAllowed = false;
                
                WCHAR wszFileName[_MAX_PATH];
                if (WszGetModuleFileName(NULL, wszFileName, _MAX_PATH) != 0)
                {
                    // just keep the name
                    LPWSTR pwszName = wcsrchr(wszFileName, L'\\');
                    pwszName = (pwszName == NULL) ? wszFileName : (pwszName + 1);
                    
                    if (SString::_wcsicmp(pwszName,pszGCStressExe) == 0)
                    {
                        bGCStressAndHeapVerifyAllowed = true;
                    }
                }
            }    
            delete [] pszGCStressExe;
        }

        if (bGCStressAndHeapVerifyAllowed)
        {
            if (forceGCconcurrent > 0)
            {
                // We want stress heap called from a lot of places to ensure that the
                // concurrent thread keeps on running.
                _ASSERTE(iFastGCStress == 0);
#ifdef _DEBUG
                iFastGCStress = 0;
#endif
                iGCStress |= int(GCSTRESS_ALLOC) | int(GCSTRESS_TRANSITION);
            }
            else
            {
                // By default, GCStress turns off concurrent GC since it make objects move less
                iGCconcurrent   =
                g_IGCconcurrent = 0;
            }
        }
        else
        {
            iGCStress = 0;
        }
    }

    if (bGCStressAndHeapVerifyAllowed)
    {
        iGCHeapVerify       =  GetConfigDWORD(L"HeapVerify", iGCHeapVerify);
    }
    
#endif //STRESS_HEAP

#ifdef GC_SIZE
    if (!iGCSegmentSize) iGCSegmentSize =  GetConfigDWORD(L"GCSegmentSize", iGCSegmentSize);
    if (!iGCgen0size) iGCgen0size = GetConfigDWORD(L"GCgen0size"  , iGCgen0size);
#endif //GC_SIZE


#ifdef THREAD_DELAY
    m_pThreadDelay = new ThreadDelay;
    m_pThreadDelay->init ();
#endif //THREAD_DELAY

    fGCBreakOnOOM   =  (GetConfigDWORD(L"GCBreakOnOOM", fGCBreakOnOOM) != 0);

    iGCtraceStart       =  GetConfigDWORD(L"GCtraceStart", iGCtraceStart);
    iGCtraceEnd         =  GetConfigDWORD(L"GCtraceEnd"  , iGCtraceEnd);
    iGCprnLvl           =  GetConfigDWORD(L"GCprnLvl"    , iGCprnLvl);

#ifdef _DEBUG
    iGCtraceFac         =  GetConfigDWORD(L"GCtraceFacility"  , iGCtraceFac);
    iInjectFatalError   = GetConfigDWORD(L"InjectFatalError", iInjectFatalError);

    fSaveThreadInfo     =  (GetConfigDWORD(L"SaveThreadInfo", fSaveThreadInfo) != 0);

    dwSaveThreadInfoMask     =  GetConfigDWORD(L"SaveThreadInfoMask", dwSaveThreadInfoMask);
#endif

    iGCForceCompact     =  GetConfigDWORD(L"gcForceCompact"  , iGCForceCompact);
    fStressLog        =  GetConfigDWORD(L"StressLog", fStressLog) != 0;
    
    m_dwStressLoadThreadCount = GetConfigDWORD(L"StressLoadThreadCount", m_dwStressLoadThreadCount);
#ifdef STRESS_THREAD
    dwStressThreadCount =  GetConfigDWORD(L"StressThreadCount", dwStressThreadCount);
#endif
    
    fUseZaps            = !(GetConfigDWORD(L"ZapDisable", !fUseZaps) != 0);

    iRequireZaps        = RequireZapsType(GetConfigDWORD(L"ZapRequire", iRequireZaps));
    if (iRequireZaps >= REQUIRE_ZAPS_COUNT)
        iRequireZaps = REQUIRE_ZAPS_NONE;
    
    if (iRequireZaps != REQUIRE_ZAPS_NONE)
    {
        LPWSTR wszZapRequireList = NULL, wszZapRequireExcludeList = NULL;
        IfFailRet(GetConfigString(L"ZapRequireList", &wszZapRequireList));
        IfFailRet(GetConfigString(L"ZapRequireExcludeList", &wszZapRequireExcludeList));

        EX_TRY
        {
            if (wszZapRequireList)
                pRequireZapsList = new AssemblyNamesList(wszZapRequireList);
            if (wszZapRequireExcludeList)
                pRequireZapsExcludeList = new AssemblyNamesList(wszZapRequireExcludeList);
        }
        EX_CATCH_HRESULT(hr);
        IfFailRet(hr);
    }
            
    IfFailRet(GetConfigString(L"ZapSet", (LPWSTR*)&pZapSet));

    fLazyActivation = (GetConfigDWORD(L"LazyActivation", fLazyActivation) != 0);
    fSecurityNeutralCode = (GetConfigDWORD(L"SecurityNeutralCode", fSecurityNeutralCode) != 0);

    DWORD dwUsePcRel32Calls = GetConfigDWORD(L"UsePcRel32Calls", fUsePcRel32Calls ? 2 : 0);
    DWORD dwUseRipData      = GetConfigDWORD(L"UseRipData", fUseRipData ? 2 : 0);

    // UsePcRel32Calls         PreJit       Jit
    //                       --------------------
    // UsePcRel32Calls == 0   disabled   disabled
    // UsePcRel32Calls == 1    enabled   disabled
    // UsePcRel32Calls == 2   disabled    enabled
    // UsePcRel32Calls == 3    enabled    enabled
    // default behavior        enabled    enabled 
    //
    fUsePcRel32Calls = ((dwUsePcRel32Calls & 2) == 2);

    // UseRipData         PreJit      Jit
    //                  --------------------
    // UseRipData == 0   disabled   disabled
    // UseRipData == 1    enabled   disabled
    // UseRipData == 2   disabled    enabled
    // UseRipData == 3    enabled    enabled
    // default behavior  disabled   disabled 
    //
    fUseRipData      = ((dwUseRipData & 2) == 2);


    dwSharePolicy           = GetConfigDWORD(L"LoaderOptimization", dwSharePolicy);
    dwMultiDomainHostType   = GetConfigDWORD(L"MultiDomainHostGAC", dwMultiDomainHostType);

    DoubleArrayToLargeObjectHeap = GetConfigDWORD(L"DoubleArrayToLargeObjectHeap", DoubleArrayToLargeObjectHeap);

    // 
    // On IA64 creating the IBC data causes data misalignment faults
    // So the creation of BBT/IBC is disabled on IA64 until we fix this.
    // 
    fDoBBInstr              = GetConfigDWORD(L"BBINSTR", fDoBBInstr) != 0;

    IfFailRet(GetConfigString(L"ZapBBINSTR", (LPWSTR*)&szZapBBInstr));
    if (szZapBBInstr)
    {
        // If szZapBBInstr only contains white space, then there's nothing to instrument (this
        // is the case with some test cases, and it's easier to fix all of them here).
        LPWSTR pStr = (LPWSTR) szZapBBInstr;
        while (*pStr == L' ') pStr++;
        if (*pStr == 0)
            szZapBBInstr = NULL;
    }
    szZapBBInstr            = NarrowWideChar((LPWSTR)szZapBBInstr);
    IfFailRet(GetConfigString(L"ZapBBInstrDir", &szZapBBInstrDir));

    
    m_fFreepZapSet = true;
    
    if (pZapSet == NULL)
    {
        m_fFreepZapSet = false;
        pZapSet = L"";
    }
    if (wcslen(pZapSet) > 3)
    {
        _ASSERTE(!"Zap Set String must be less than 3 chars");
        delete[] pZapSet;
        m_fFreepZapSet = false;
        pZapSet = L"";
    }

    fShowMetaDataAccess     = GetConfigDWORD(L"ShowMetadataAccess", fShowMetaDataAccess);

    dwDisableStackwalkCache = GetConfigDWORD(L"DisableStackwalkCache", dwDisableStackwalkCache);

#ifdef _DEBUG
    fExactInterfaceCalls = (GetConfigDWORD(L"ExactInterfaceCalls", fExactInterfaceCalls) != 0);
#endif
    fLazyInterfaceRestore = (GetConfigDWORD(L"LazyInterfaceRestore", fLazyInterfaceRestore) != 0);
    CONSISTENCY_CHECK_MSG(!fLazyInterfaceRestore,
                          "Lazy interface restoring not currently supported. Continuing will yield undefined results.");

    fUseNewCrossDomainRemoting = GetConfigDWORD(L"UseNewCrossDomainRemoting", fUseNewCrossDomainRemoting);

    //@GENERICS
#ifdef _DEBUG
#if defined(FEATURE_SHARE_GENERIC_CODE)  
    fShareGenericCode    = (GetConfigDWORD(L"ShareGenericCode", fShareGenericCode) != 0);
#endif
    fFullGenericsNGEN    = (GetConfigDWORD(L"FullGenericsNGEN", fFullGenericsNGEN) != 0);
    fFullArraysNGEN    = (GetConfigDWORD(L"FullArraysNGEN", fFullArraysNGEN) != 0);
    fDontLoadOpenTypes    = (GetConfigDWORD(L"DontLoadOpenTypes", fDontLoadOpenTypes) != 0);

    IfFailRet (GetConfigString(L"BreakOnClassLoad", (LPWSTR*) &pszBreakOnClassLoad));
    pszBreakOnClassLoad = NarrowWideChar((LPWSTR)pszBreakOnClassLoad);
#endif

    fJitFramed = (GetConfigDWORD(L"JitFramed", fJitFramed) != 0);
    fJitAlignLoops = (GetConfigDWORD(L"JitAlignLoops", fJitAlignLoops) != 0);
    iJitOptimizeType      =  GetConfigDWORD(L"JitOptimizeType",     iJitOptimizeType);
    if (iJitOptimizeType > OPT_RANDOM)     iJitOptimizeType = OPT_DEFAULT;

    // These two values respect the Shim's policy of favoring config files over registry settings.
    fLegacyNullReferenceExceptionPolicy = (GetConfigDWORDFavoringConfigFile(L"legacyNullReferenceExceptionPolicy",
                                                                            fLegacyNullReferenceExceptionPolicy) != 0);
    fLegacyUnhandledExceptionPolicy = (GetConfigDWORDFavoringConfigFile(L"legacyUnhandledExceptionPolicy",
                                                                        fLegacyUnhandledExceptionPolicy) != 0);

    fLegacyApartmentInitPolicy = (GetConfigDWORDFavoringConfigFile(L"legacyApartmentInitPolicy", 
                                                                    fLegacyApartmentInitPolicy) != 0);

    fLegacyComHierarchyVisibility = (GetConfigDWORDFavoringConfigFile(L"legacyComHierarchyVisibility", 
                                                                    fLegacyComHierarchyVisibility) != 0);

    fLegacyComVTableLayout = (GetConfigDWORDFavoringConfigFile(L"legacyComVTableLayout", 
                                                                    fLegacyComVTableLayout) != 0);
    fNewComVTableLayout = (GetConfigDWORDFavoringConfigFile(L"newComVTableLayout", 
                                                                    fNewComVTableLayout) != 0);
        
    if (GetConfigDWORDFavoringConfigFile(L"legacyImpersonationPolicy", false) != 0)
        iImpersonationPolicy = IMP_NOFLOW;
    else if (GetConfigDWORDFavoringConfigFile(L"alwaysFlowImpersonationPolicy", false) != 0)
        iImpersonationPolicy = IMP_ALWAYSFLOW;                                                                    
        
    fLegacyV1CASPolicy = (GetConfigDWORDFavoringConfigFile(L"legacyV1CASPolicy", 
                                                                    fLegacyV1CASPolicy) != 0);
    fLegacyLoadMscorsnOnStartup = (GetConfigDWORDFavoringConfigFile(L"legacyLoadMscorsnOnStartup", 
                                                                    fLegacyLoadMscorsnOnStartup) != 0);
	fTransparencyEnforcementDisabled = (GetConfigDWORDFavoringConfigFile(L"DisableSecurityTransparency", 
                                                                    fTransparencyEnforcementDisabled) != 0);

	fCacheBindingFailures = !(GetConfigDWORDFavoringConfigFile(L"disableCachingBindingFailures", 0));
	fUseLegacyIdentityFormat = (GetConfigDWORDFavoringConfigFile(L"useLegacyIdentityFormat", 0));
    fDisableCommitThreadStack = (GetConfigDWORDFavoringConfigFile(L"disableCommitThreadStack", fDisableCommitThreadStack) != 0);
    fProbeForStackOverflow = !(GetConfigDWORDFavoringConfigFile(L"disableStackOverflowProbing", 0));
    
#ifdef _DEBUG
    fJitLooseExceptOrder  = (GetConfigDWORD(L"JitLooseExceptOrder", fJitLooseExceptOrder) != 0);

    fDebugInfo          = (GetConfigDWORD(L"JitDebugInfo",       fDebugInfo)          != 0);
    fDebuggable         = (GetConfigDWORD(L"JitDebuggable",      fDebuggable)         != 0);
    fStressOn           = (GetConfigDWORD(L"StressOn",           fStressOn)           != 0);
    apiThreadStressCount = GetConfigDWORD(L"APIThreadStress",     apiThreadStressCount);

    LPWSTR wszPreStubStuff = NULL;

    IfFailRet(GetConfigString(L"PrestubHalt", &wszPreStubStuff));
    IfFailRet(ParseMethList(wszPreStubStuff, &pPrestubHalt));

    LPWSTR wszInvokeStuff = NULL;
    IfFailRet(GetConfigString(L"InvokeHalt", &wszInvokeStuff));
    IfFailRet(ParseMethList(wszInvokeStuff, &pInvokeHalt));    
    
    IfFailRet(GetConfigString(L"PrestubGC", &wszPreStubStuff));
    IfFailRet(ParseMethList(wszPreStubStuff, &pPrestubGC));
    
    IfFailRet(GetConfigString(L"BreakOnClassBuild", (LPWSTR*)&pszBreakOnClassBuild));
    pszBreakOnClassBuild = NarrowWideChar((LPWSTR)pszBreakOnClassBuild);

    fAppendFileNameToTypeName = (GetConfigDWORD(L"AppendFileNameToTypeName", TRUE) != 0);

    IfFailRet(GetConfigString(L"BreakOnMethodName", (LPWSTR*)&pszBreakOnMethodName));
    pszBreakOnMethodName = NarrowWideChar((LPWSTR)pszBreakOnMethodName);

    IfFailRet(GetConfigString(L"DumpOnClassLoad", (LPWSTR*)&pszDumpOnClassLoad));
    pszDumpOnClassLoad = NarrowWideChar((LPWSTR)pszDumpOnClassLoad);

    IfFailRet(GetConfigString(L"BreakOnInteropStubSetup", (LPWSTR*)&pszBreakOnInteropStubSetup));
    pszBreakOnInteropStubSetup = NarrowWideChar((LPWSTR)pszBreakOnInteropStubSetup);    

    IfFailRet(GetConfigString(L"BreakOnComToClrNativeInfoInit", (LPWSTR*)&pszBreakOnComToClrNativeInfoInit));
    pszBreakOnComToClrNativeInfoInit = NarrowWideChar((LPWSTR)pszBreakOnComToClrNativeInfoInit);    

    IfFailRet(GetConfigString(L"BreakOnStructMarshalSetup", (LPWSTR*)&pszBreakOnStructMarshalSetup));
    pszBreakOnStructMarshalSetup = NarrowWideChar((LPWSTR)pszBreakOnStructMarshalSetup);    

    m_fAssertOnBadImageFormat = (GetConfigDWORD(L"AssertOnBadImageFormat", m_fAssertOnBadImageFormat) != 0);
    m_fAssertOnFailFast = (GetConfigDWORD(L"AssertOnFailFast", m_fAssertOnFailFast) != 0);
   
    fSuppressChecks = (GetConfigDWORD(L"SuppressChecks", fSuppressChecks) != 0);
    CHECK::SetAssertEnforcement(!fSuppressChecks);

    fConditionalContracts = (GetConfigDWORD(L"ConditionalContracts", fConditionalContracts) != 0);

   
    fEnableFullDebug = (GetConfigDWORD(L"EnableFullDebug", fEnableFullDebug) != 0);

    fJitVerificationDisable = (GetConfigDWORD(L"JitVerificationDisable", fJitVerificationDisable)         != 0);

    // TlbImp stuff
    fTlbImpSkipLoading = (GetConfigDWORD(L"TlbImpSkipLoading", fTlbImpSkipLoading) != 0);

    iExposeExceptionsInCOM = GetConfigDWORD(L"ExposeExceptionsInCOM", iExposeExceptionsInCOM);
#endif

    if(g_pConfig) {
        LPCWSTR result = NULL;
        if(SUCCEEDED(g_pConfig->GetConfiguration(L"developerInstallation", CONFIG_SYSTEM, &result)) && result)
        {
            if(SString::_wcsicmp(result, L"true") == 0)
                m_fDeveloperInstallation = true;
        }
    }

#ifdef AD_NO_UNLOAD
    fAppDomainUnload = (GetConfigDWORD(L"AppDomainNoUnload", 0) == 0);
#endif
    dwADURetryCount=GetConfigDWORD(L"ADURetryCount", dwADURetryCount);
    if (dwADURetryCount==(DWORD)-1)
    {
        _ASSERTE(!"Reserved value");
        dwADURetryCount=(DWORD)-2;
    }
#ifdef _DEBUG
    fAppDomainLeaks = GetConfigDWORD(L"AppDomainAgilityChecked", DEFAULT_APP_DOMAIN_LEAKS) == 1;
#endif

#ifdef _DEBUG
    fJitVerificationDisable = (GetConfigDWORD(L"JitVerificationDisable", fJitVerificationDisable)         != 0);

    fVerifierOff    = (GetConfigDWORD(L"VerifierOff", fVerifierOff) != 0);
    fVerifierBreakOnError =  
        (GetConfigDWORD(L"VerBreakOnError", (DWORD) -1) == 1);

    LPWSTR wszVerifierString = NULL;

    IfFailRet(GetConfigString(L"VerSkip", &wszVerifierString));
    IfFailRet(ParseMethList(wszVerifierString, &pVerifierSkip));

    IfFailRet(GetConfigString(L"VerBreak", &wszVerifierString));
    IfFailRet(ParseMethList(wszVerifierString, &pVerifierBreak));
 
    iVerifierBreakOffset =  GetConfigDWORD(L"VerOffset", iVerifierBreakOffset);
    iVerifierBreakPass   =  GetConfigDWORD(L"VerPass",   iVerifierBreakPass);
    if (iVerifierBreakOffset != -1)
        fVerifierBreakOffset = true;
    if (iVerifierBreakPass != -1)
        fVerifierBreakPass = true;

    fVerifierMsgMethodInfoOff =  
        (GetConfigDWORD(L"VerMsgMethodInfoOff", (DWORD) -1) == 1);

    fDoAllowUntrustedCallerChecks =  
        (GetConfigDWORD(L"SupressAllowUntrustedCallerChecks", 0) != 1);

    m_SuspendDeadlockTimeout = GetConfigDWORD(L"SuspendDeadlockTimeout",40000);
#endif // _DEBUG
    fInited = true;

    m_pTraceIUnknown = (IUnknown*)(DWORD_PTR)(GetConfigDWORD(L"TraceIUnknown", (DWORD)(DWORD_PTR)(m_pTraceIUnknown)));
    m_TraceWrapper = GetConfigDWORD(L"TraceWrap", m_TraceWrapper);

    // can't have both
    if (m_pTraceIUnknown != 0)
    {
        m_TraceWrapper = 0;
    }
    else
    if (m_TraceWrapper != 0)
    {
        m_pTraceIUnknown = (IUnknown*)-1;
    }

#ifdef _DEBUG

    LPWSTR wszPerfTypes = NULL;
    IfFailRet(GetConfigString(L"PerfTypesToLog", &wszPerfTypes));
    IfFailRet(ParseTypeList(wszPerfTypes, &pPerfTypesToLog));

    iPerfNumAllocsThreshold = GetConfigDWORD(L"PerfNumAllocsThreshold", 0x3FFFFFFF);
    iPerfAllocsSizeThreshold    = GetConfigDWORD(L"PerfAllocsSizeThreshold", 0x3FFFFFFF);

    fGenerateStubForHost = GetConfigDWORD(L"GenerateStubForHost", 0);

    fShouldInjectFault = GetConfigDWORD(L"InjectFault", 0);

    testThreadAbort = GetConfigDWORD(L"HostTestThreadAbort", 0);
    testADUnload = GetConfigDWORD(L"HostTestADUnload", 0);

#endif //_DEBUG

    fContinueAfterFatalError = GetConfigDWORD(L"ContinueAfterFatalError", 0);

    DWORD iNgenHardBindOverride = GetConfigDWORD(L"HardPrejitEnabled", iNgenHardBind);
    _ASSERTE(iNgenHardBindOverride < NGEN_HARD_BIND_COUNT);
    if (iNgenHardBindOverride < NGEN_HARD_BIND_COUNT)
        iNgenHardBind = NgenHardBindType(iNgenHardBindOverride);

#if defined(_DEBUG) && defined(STUBLINKER_GENERATES_UNWIND_INFO)
    fStubLinkerUnwindInfoVerificationOn = GetConfigDWORD(L"StubLinkerUnwindInfoVerificationOn", fStubLinkerUnwindInfoVerificationOn);
#endif

    m_fRejectSafeHandleFinalizers = GetConfigDWORD(L"RejectSafeHandleFinalizers", m_fRejectSafeHandleFinalizers);

    if (GetConfigDWORD(L"UseMethodDataCache", FALSE) != 0) {
        MethodTable::AllowMethodDataCaching();
    }

    if (GetConfigDWORD(L"UseParentMethodData", TRUE) != 0) {
        MethodTable::AllowParentMethodDataCopy();
    }

    if (GetConfigDWORD(L"UseMethodDataCacheInFindDispatchImpl", FALSE) != 0) {
        MethodTable::AllowMethodDataCacheUseInFindDispatchImpl();
    }

    // Get the symbol reading policy setting which is maintained by the hosting API (since it can be overridden there)
    const DWORD notSetToken = 0xFFFFFFFF;
    DWORD iSymbolReadingConfig = GetConfigDWORDFavoringConfigFile(L"SymbolReadingPolicy", notSetToken );
    if( iSymbolReadingConfig != notSetToken &&
        iSymbolReadingConfig <= eSymbolReadingFullTrustOnly )
    {
        ESymbolReadingPolicy policy = ESymbolReadingPolicy(iSymbolReadingConfig);
        CCLRDebugManager::SetSymbolReadingPolicy( policy, eSymbolReadingSetByConfig );
    }



    return hr;
}


/**************************************************************/
static void MessageBoxParseError(HRESULT hr, __in_z LPCWSTR wszFile);

/**************************************************************/
HRESULT EEConfig::SetupConfiguration()
{
    CONTRACTL {
        NOTHROW;
        GC_TRIGGERS;
        MODE_ANY;
    } CONTRACTL_END;

    // Get the system configuration file
    WCHAR systemDir[_MAX_PATH+9]; // +9 for '.config\0\0'
    DWORD dwSize = _MAX_PATH;
    static const WCHAR configFile[] = MACHINE_CONFIGURATION_FILE;
    WCHAR version[_MAX_PATH];
    DWORD dwVersion = _MAX_PATH;

    HRESULT hr = S_OK;
    // Get the version location
    IfFailRet(GetCORVersion(version, _MAX_PATH, & dwVersion));

    // See if the environment has specified an XML file
    NewArrayHolder<WCHAR> file(REGUTIL::GetConfigString(L"CONFIG", TRUE));
    if(file != NULL) {
        hr = AppendConfigurationFile(file, version);
    }

    if (FAILED(hr))
        MessageBoxParseError(hr, file);

    // We need to read configuration information from 3 sources... the app config file, the
    // host supplied config file, and the machine.config file. The order in which we
    // read them are very import. If the different config sources specify the same config
    // setting, we will use the setting of the first one read.
    //
    // In the pecking order, machine.config should always have the final say. The host supplied config
    // file should follow, and lastly, the app config file.
    
    // ----------------------------------------------------
    // First, read machine.config
    // ----------------------------------------------------
    IfFailRet(GetInternalSystemDirectory(systemDir, &dwSize));

    // dwSize already includes the NULL

    DWORD configSize = COUNTOF(configFile) - 1; // don't duplicate NULLs
    if(configSize + dwSize <= _MAX_PATH) 
    {
        wcscat_s(systemDir, COUNTOF(systemDir), configFile);

        // CLR_STARTUP_OPT:
        // The machine.config file can be very large.  We cannot afford
        // to parse all of it at CLR startup time.
        //
        // Accordingly, we instruct the XML parser to stop parsing the
        // machine.config file when it sees the end of the
        // <runtime>...</runtime> section that holds our data (if any).
        //
        // By construction, this section is now placed near the top
        // of machine.config.
        // 
        hr = AppendConfigurationFile(systemDir, version, stopAfterRuntimeSection);

        if (FAILED(hr))
            MessageBoxParseError(hr, systemDir);

        if (hr == S_FALSE) // means that we couldn't find machine.config
            hr = S_OK;

        // ----------------------------------------------------
        // Secondly, read the host supplied config file
        // ----------------------------------------------------

        if (GetProcessBindingFile() != NULL && GetSizeOfProcessBindingFile() > 0)
            IfFailRet(AppendConfigurationFile(GetProcessBindingFile(), version));

    }

    // ----------------------------------------------------
    // And finally, read the app config file
    // ----------------------------------------------------

    int len = 0;

    // get name of file used to create process
    if (g_pCachedModuleFileName) {
        wcscpy_s(systemDir, COUNTOF(systemDir), g_pCachedModuleFileName);
        len = (int)wcslen(systemDir);
    }
    else {
        len = WszGetModuleFileName(NULL, systemDir, _MAX_PATH);
    }
   
    if (len) {
        wcscat_s(systemDir, COUNTOF(systemDir), L".config\0");
        
        hr = AppendConfigurationFile(systemDir, version);

        if (FAILED(hr) && GetConfigDWORDInternal(L"NotifyBadAppCfg",false))
            MessageBoxParseError(hr, systemDir);

        hr=S_OK;
    }



    return hr;
}

// There was an error 'hr' parsing the file 'wszFile'.
// Pop up a MessageBox reporting the error, unless the config setting
// 'NoGuiFromShim' is in effect.
//
static void MessageBoxParseError(HRESULT hr, __in_z LPCWSTR wszFile) {
    CONTRACTL {
        NOTHROW;
        GC_TRIGGERS;
        MODE_ANY;
        PRECONDITION(FAILED(hr)); 
    } CONTRACTL_END;

    if (!REGUTIL::GetConfigDWORD(L"NoGuiFromShim", FALSE)) {
        EEMessageBoxCatastrophic(IDS_EE_CONFIGPARSER_ERROR, IDS_EE_CONFIGPARSER_ERROR_CAPTION, wszFile, hr);
    }
}

HRESULT EEConfig::GetConfiguration(__in_z LPCWSTR pKey, ConfigSearch direction, __deref_out_opt LPCWSTR* pValue)
{
    CONTRACT (HRESULT) {
        NOTHROW;
        GC_NOTRIGGER;
        MODE_ANY;
        PRECONDITION(CheckPointer(pValue)); 
        PRECONDITION(CheckPointer(pKey)); 
    } CONTRACT_END;
    *pValue = NULL;
    ConfigList::ConfigIter iter(&m_Configuration);
    EEStringData sKey((DWORD)wcslen(pKey)+1,pKey);
    HashDatum datum;

    switch(direction) {
    case CONFIG_SYSTEM: {
        for(EEUnicodeStringHashTable* table = iter.Next();table; table = iter.Next()) {
            if(table->GetValue(&sKey, &datum)) {
                *pValue = (LPWSTR) datum;
                RETURN S_OK;
            }
        }
    }
    case CONFIG_APPLICATION: {
        for(EEUnicodeStringHashTable* table = iter.Previous();table != NULL; table = iter.Previous()) {
            if(table->GetValue(&sKey, &datum)) {
                *pValue = (LPWSTR) datum;
                RETURN S_OK;
            }
        }
    }
    default:
        RETURN E_FAIL;
    }
}        

LPCWSTR EEConfig::GetProcessBindingFile()
{
    LEAF_CONTRACT;
    return g_pszHostConfigFile;
}

SIZE_T EEConfig::GetSizeOfProcessBindingFile()
{
    LEAF_CONTRACT;
    return g_dwHostConfigFile;
}

/**************************************************************/

STDAPI GetXMLObjectEx(IXMLParser **ppv, CrstLevel crstLevel);


HRESULT EEConfig::AppendConfigurationFile(
    LPCWSTR pszFileName,
    LPCWSTR version,
    ParseCtl parseCtl)
{
    CONTRACTL {
        NOTHROW;
        GC_NOTRIGGER;
        MODE_ANY;
        PRECONDITION(CheckPointer(pszFileName));
        PRECONDITION(CheckPointer(version));
        INJECT_FAULT(return E_OUTOFMEMORY);
    } CONTRACTL_END;

    EEUnicodeStringHashTable* pTable = m_Configuration.Append();
    if(pTable == NULL) return E_OUTOFMEMORY;

    NonVMComHolder<IXMLParser>         pIXMLParser(NULL);
    NonVMComHolder<IStream>            pFile(NULL);
    NonVMComHolder<EEConfigFactory>    factory(NULL); 

    HRESULT hr = CreateConfigStream(pszFileName, &pFile);
    if(FAILED(hr)) goto Exit;

    hr = GetXMLObjectEx(&pIXMLParser, CrstXMLParserManaged);
    if(FAILED(hr)) goto Exit;

    factory = new (nothrow) EEConfigFactory(pTable, version, parseCtl);
    
    if ( ! factory) { 
        hr = E_OUTOFMEMORY; 
        goto Exit; 
    }
    factory->AddRef(); // RefCount = 1 

    
    hr = pIXMLParser->SetInput(pFile); // filestream's RefCount=2
    if ( ! SUCCEEDED(hr)) 
        goto Exit;

    hr = pIXMLParser->SetFactory(factory); // factory's RefCount=2
    if ( ! SUCCEEDED(hr)) 
        goto Exit;

    {
        CONTRACT_VIOLATION(ThrowsViolation);
        hr = pIXMLParser->Run(-1);
    }
    
Exit:  
    if (hr == (HRESULT) XML_E_MISSINGROOT)
        hr = S_OK;
    else if (Assembly::FileNotFound(hr))
        hr = S_FALSE;

    return hr;
}

bool EEConfig::RequireZap(LPCUTF8 assemblyName) const
{
    if (iRequireZaps == REQUIRE_ZAPS_NONE)
        return false;

    if (pRequireZapsExcludeList != NULL && pRequireZapsExcludeList->IsInList(assemblyName))
        return false;

    if (pRequireZapsList == NULL || pRequireZapsList->IsInList(assemblyName))
        return true;

    return false;
}

/**************************************************************/
#ifdef _DEBUG
/**************************************************************/

// Ownership of the string buffer passes to ParseMethList

/* static */
HRESULT EEConfig::ParseMethList(__in_z LPWSTR str, MethodNamesList** out) {
    CONTRACTL {
        NOTHROW;
        GC_NOTRIGGER;
        MODE_ANY;
        INJECT_FAULT(return E_OUTOFMEMORY);
        PRECONDITION(CheckPointer(str, NULL_OK));
        PRECONDITION(CheckPointer(out));
    } CONTRACTL_END;   

    HRESULT hr = S_OK;
    
    *out = NULL;

        // we are now done with the string passed in
    if (str == NULL)
    {
        return S_OK;
    }

    EX_TRY
    {
        *out = new MethodNamesList(str);
    } EX_CATCH_HRESULT(hr);
    
    delete [] str;

    return hr;
}

/**************************************************************/
/* static */
void EEConfig::DestroyMethList(MethodNamesList* list) {
    CONTRACTL {
        NOTHROW;
        GC_NOTRIGGER;
        MODE_ANY;
        PRECONDITION(CheckPointer(list));
    } CONTRACTL_END;

    if (list == 0)
        return;
    delete list;
}

/**************************************************************/
/* static */
bool EEConfig::IsInMethList(MethodNamesList* list, MethodDesc* pMD)
{
    CONTRACTL {
        THROWS;
        GC_TRIGGERS;
        MODE_ANY;
        PRECONDITION(CheckPointer(list, NULL_OK));
        PRECONDITION(CheckPointer(pMD));
    } CONTRACTL_END;

    if (list == 0)
        return(false);
    else
    {
        DefineFullyQualifiedNameForClass();

        LPCUTF8 name = pMD->GetName();
        if (name == NULL)
        {
            return false;
        }
        LPCUTF8 className = pMD->GetClass() ? GetFullyQualifiedNameForClass(pMD->GetClass()) : "";
        if (className == NULL)
        {
            return false;
        }
        PCCOR_SIGNATURE sig = pMD->GetSig();

        return list->IsInList(name, className, sig);
    }
}

// Ownership of the string buffer passes to ParseTypeList
/* static */
HRESULT EEConfig::ParseTypeList(__in_z LPWSTR str, TypeNamesList** out)
{
    CONTRACTL {
        NOTHROW;
        GC_NOTRIGGER;
        MODE_ANY;
        PRECONDITION(CheckPointer(out));
        PRECONDITION(CheckPointer(str, NULL_OK));
        INJECT_FAULT(return E_OUTOFMEMORY);
    } CONTRACTL_END;

    HRESULT hr = S_OK;
    
    *out = NULL;

    if (str == NULL)
        return S_OK;

    NewHolder<TypeNamesList> newTypeNameList(new (nothrow) TypeNamesList());
    if (newTypeNameList != NULL)
        IfFailRet(newTypeNameList->Init(str));

    delete [] str;

    newTypeNameList.SuppressRelease();
    *out = newTypeNameList;
    
    return (*out != NULL)?S_OK:E_OUTOFMEMORY;
}

void EEConfig::DestroyTypeList(TypeNamesList* list) {

    CONTRACTL {
        NOTHROW;
        GC_NOTRIGGER;
        MODE_ANY;
        PRECONDITION(CheckPointer(list));
    } CONTRACTL_END;
    
    if (list == 0)
        return;
    delete list;
}

TypeNamesList::TypeNamesList()
{
    LEAF_CONTRACT;
}

HRESULT TypeNamesList::Init(__in_z LPCWSTR str)
{
    CONTRACTL {
        NOTHROW;
        GC_NOTRIGGER;
        MODE_ANY;
        PRECONDITION(CheckPointer(str));
        INJECT_FAULT(return E_OUTOFMEMORY);
    } CONTRACTL_END;

    pNames = NULL;

    LPCWSTR currentType = str;
    int length = 0;
    bool typeFound = false;

    for (; *str != '\0'; str++)
    {
        switch(*str)
        {
        case ' ':
            {
                if (!typeFound)
                    break;

                NewHolder<TypeName> tn(new (nothrow) TypeName());
                if (tn == NULL)
                    return E_OUTOFMEMORY;
                
                tn->typeName = new (nothrow) char[length + 1];
                if (tn->typeName == NULL)
                    return E_OUTOFMEMORY;

                tn.SuppressRelease();
                MAKE_UTF8PTR_FROMWIDE_NOTHROW(temp, currentType);
                if (temp == NULL)
                    return E_OUTOFMEMORY;

                memcpy(tn->typeName, temp, length * sizeof(char));
                tn->typeName[length] = '\0';

                tn->next = pNames;
                pNames = tn;

                typeFound = false;
                length = 0;

                break;
            }

        default:
            if (!typeFound)
                currentType = str;

            typeFound = true;
            length++;
            break;
        }
    }

    if (typeFound)
    {
        NewHolder<TypeName> tn(new (nothrow) TypeName());
        if (tn == NULL)
            return E_OUTOFMEMORY;
        
        tn->typeName = new (nothrow) char[length + 1];

        if (tn->typeName == NULL)
            return E_OUTOFMEMORY;

        tn.SuppressRelease();
        MAKE_UTF8PTR_FROMWIDE_NOTHROW(temp, currentType);
        if (temp == NULL)
            return E_OUTOFMEMORY;

        memcpy(tn->typeName, temp, length * sizeof(char));
        tn->typeName[length] = '\0';

        tn->next = pNames;
        pNames = tn;
    }
    return S_OK;
}

TypeNamesList::~TypeNamesList()
{
    CONTRACTL {
        NOTHROW;
        FORBID_FAULT;
        GC_NOTRIGGER;
        MODE_ANY;
    } CONTRACTL_END;


    while (pNames)
    {
        delete [] pNames->typeName;

        TypeName *tmp = pNames;
        pNames = pNames->next;

        delete tmp;
    }
}

bool TypeNamesList::IsInList(LPCUTF8 typeName)
{
    CONTRACTL {
        NOTHROW;
        GC_NOTRIGGER;
        MODE_ANY;
        PRECONDITION(CheckPointer(typeName));
    } CONTRACTL_END;

    TypeName *tnTemp = pNames;
    while (tnTemp)
    {
        if (strstr(typeName, tnTemp->typeName) != typeName)
            tnTemp = tnTemp->next;
        else
            return true;
    }

    return false;
}

#endif // _DEBUG

bool IsRunningUnderZapmon()
{
    WRAPPER_CONTRACT;
    return g_pConfig->MonitorZapStartup() || g_pConfig->MonitorZapExecution() || g_pConfig->MemmapEnabled();
}

