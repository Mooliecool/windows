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


#ifndef EECONFIG_H
#define EECONFIG_H

class MethodDesc;

#ifndef _EE_HASH_H
class EEUnicodeStringHashTable;
#endif

#include "corhost.h"

#ifdef _DEBUG
class TypeNamesList
{
    class TypeName
    {
        LPUTF8      typeName;
        TypeName *next;           // Next name

        friend class TypeNamesList;
    };

    TypeName     *pNames;         // List of names

public:
    TypeNamesList();
    ~TypeNamesList();

    HRESULT Init(__in_z LPCWSTR str);
    bool IsInList(LPCUTF8 typeName);
};
#endif

class ConfigList;

class ConfigSource
{
    friend class ConfigList;
public:
    ConfigSource();
    ~ConfigSource();

    HRESULT Init();

    EEUnicodeStringHashTable* Table();

    void Add(ConfigSource* prev);

    ConfigSource* Next()
    {
        LEAF_CONTRACT;
        return m_pNext;
    }

    ConfigSource* Previous()
    {
        LEAF_CONTRACT;
        return m_pPrev;
    }


private:
    EEUnicodeStringHashTable *m_Table;
    ConfigSource *m_pNext;
    ConfigSource *m_pPrev;
};


class ConfigList
{
public:
    class ConfigIter
    {
    public:
        ConfigIter(ConfigList* pList)
        {
            CONTRACTL {
                NOTHROW;
                GC_NOTRIGGER;
                // MODE_ANY;
                FORBID_FAULT;
            } CONTRACTL_END;
            
            pEnd = &(pList->m_pElement);
            pCurrent = pEnd;
        }

        EEUnicodeStringHashTable* Next()
        {
            CONTRACT (EEUnicodeStringHashTable*) {
                NOTHROW;
                GC_NOTRIGGER;
                // MODE_ANY;
                POSTCONDITION(CheckPointer(RETVAL, NULL_OK));
            } CONTRACT_END;
        
            pCurrent = pCurrent->Next();;
            if(pCurrent == pEnd)
                RETURN NULL;
            else
                RETURN pCurrent->Table();
        }

        EEUnicodeStringHashTable* Previous()
        {
            CONTRACT (EEUnicodeStringHashTable*) {
                NOTHROW;
                GC_NOTRIGGER;
                // MODE_ANY;
                POSTCONDITION(CheckPointer(RETVAL, NULL_OK));
            } CONTRACT_END;

            pCurrent = pCurrent->Previous();
            if(pCurrent == pEnd)
                RETURN NULL;
            else
                RETURN pCurrent->Table();
        }

    private:
        ConfigSource* pEnd;
        ConfigSource* pCurrent;
    };

    EEUnicodeStringHashTable* Add()
    {
        CONTRACT (EEUnicodeStringHashTable*) {
            NOTHROW;
            GC_NOTRIGGER;
            // MODE_ANY;
            POSTCONDITION(CheckPointer(RETVAL, NULL_OK));
        } CONTRACT_END;

        ConfigSource* pEntry = new (nothrow) ConfigSource();

        if (pEntry == NULL)
            RETURN NULL;

        
        if (FAILED(pEntry->Init()))
            RETURN NULL;
        
        pEntry->Add(&m_pElement);
        RETURN pEntry->Table();
    }

    EEUnicodeStringHashTable* Append()
    {
        CONTRACT (EEUnicodeStringHashTable*) {
            NOTHROW;
            GC_NOTRIGGER;
            // MODE_ANY;
            POSTCONDITION(CheckPointer(RETVAL, NULL_OK));
        } CONTRACT_END;
        
        ConfigSource* pEntry = new (nothrow) ConfigSource();
        if (pEntry == NULL)
            RETURN NULL;
        
        if (FAILED(pEntry->Init()))
            RETURN NULL;
        pEntry->Add(m_pElement.Previous());
        RETURN pEntry->Table();
    }

    ~ConfigList()
    {
        CONTRACTL {
            NOTHROW;
            GC_NOTRIGGER;
            // MODE_ANY;
            FORBID_FAULT;
        } CONTRACTL_END;

        ConfigSource* pNext = m_pElement.Next();
        while(pNext != &m_pElement) {
            ConfigSource *last = pNext;
            pNext = pNext->m_pNext;
            delete last;
        }
    }

friend class ConfigIter;

private:
    ConfigSource m_pElement;
};

enum { OPT_BLENDED,
       OPT_SIZE,
       OPT_SPEED,
       OPT_RANDOM,
       OPT_DEFAULT = OPT_BLENDED };

/* Control of impersonation flow:
	FASTFLOW means that impersonation is flowed only if it has been achieved through managed means. This is the default and avoids a kernel call.
	NOFLOW is the Everett default where we don't flow the impersonation at all
	ALWAYSFLOW is the (potentially) slow mode where we will always flow the impersonation, regardless of how it was achieved (managed or p/invoke). Includes
	a kernel call.
	Keep in sync with values in SecurityContext.cs
	*/
enum { 
	IMP_FASTFLOW = 0,
       IMP_NOFLOW = 1,
       IMP_ALWAYSFLOW = 2,
       IMP_DEFAULT = IMP_FASTFLOW };

enum ParseCtl {
    parseAll,               // parse entire config file
    stopAfterRuntimeSection // stop after <runtime>...</runtime> section
};

extern CorHostProtectionManager s_CorHostProtectionManager;

#ifdef THREAD_DELAY

class ThreadDelay 
{
    int m_iThreadDelayFrequency;            //probability of a short delay
    int m_iThreadDelayDuration;             //length of a short delay in ms
    int m_iThreadSpikeDelayFrequency;       //number of calls before a long delay occurs
    int m_iThreadSpikeDelayDuration;        //length of a long delay in ms
    int m_iSpikeCount;                      //counter to keep track of calls to SpikeDelay
    bool m_fSpikeOn, m_fDelayOn;      

public:
    ThreadDelay (); 
    void init ();                               //initialize private data members to values specified via complus registry keys

    inline bool SpikeOn () {return m_fSpikeOn;} //quickly determine if spike delays are turned on
    inline bool DelayOn () {return m_fDelayOn;} //quickly determine if short random delays are turned on

    void ThreadDelay::SpikeDelay ();            //conditionally generate a long delay (m_iThreadSpikeDelayDuration ms) 
                                                //every m_iThreadSpikeDelayFrequency calls
    void ThreadDelay::ShortDelay ();            //conditionally generate a short delay (m_iThreadDelayDuration ms) 
                                                //with a probability of m_iThreadDelayFrequency
};

#endif //THREAD_DELAY

class EEConfig
{
public:
    typedef enum {
        CONFIG_SYSTEM,
        CONFIG_APPLICATION
    } ConfigSearch;

    static HRESULT Setup();

    void *operator new(size_t size);

    HRESULT Init();
    HRESULT Cleanup();

        // Jit-config
    
    unsigned int  GenOptimizeType(void)             const {LEAF_CONTRACT;  return iJitOptimizeType; }
    bool          GenLooseExceptOrder(void)         const {LEAF_CONTRACT;  return fJitLooseExceptOrder; }
    bool          JitFramed(void)                   const {LEAF_CONTRACT;  return fJitFramed; }
    bool          JitAlignLoops(void)               const {LEAF_CONTRACT;  return fJitAlignLoops; }

    bool LegacyNullReferenceExceptionPolicy(void)   const {LEAF_CONTRACT;  return fLegacyNullReferenceExceptionPolicy; }
    bool LegacyUnhandledExceptionPolicy(void)       const {LEAF_CONTRACT;  return fLegacyUnhandledExceptionPolicy; }

    bool LegacyApartmentInitPolicy(void)            const {LEAF_CONTRACT;  return fLegacyApartmentInitPolicy; }
    bool LegacyComHierarchyVisibility(void)         const {LEAF_CONTRACT;  return fLegacyComHierarchyVisibility; }
    bool LegacyComVTableLayout(void)                const {LEAF_CONTRACT;  return fLegacyComVTableLayout; }
    bool NewComVTableLayout(void)                   const {LEAF_CONTRACT;  return fNewComVTableLayout; }
    
    // SECURITY
    unsigned    ImpersonationMode(void)           const 
    { 
        CONTRACTL 
        {
            NOTHROW;
            GC_NOTRIGGER;
            // MODE_ANY;
            SO_TOLERANT;
        } CONTRACTL_END;
        return iImpersonationPolicy ; 
    }
    void    SetLegacyImpersonationPolicy()              { LEAF_CONTRACT; iImpersonationPolicy = IMP_NOFLOW; }
    void    SetAlwaysFlowImpersonationPolicy()              { LEAF_CONTRACT; iImpersonationPolicy = IMP_ALWAYSFLOW; }
    bool    TransparencyDisabled(void) const 
    {
        LEAF_CONTRACT; 
        return fTransparencyEnforcementDisabled; 
    }
    
    bool    LegacyV1CASPolicy(void)           const 
    { 
        CONTRACTL 
        {
            NOTHROW;
            GC_NOTRIGGER;
            // MODE_ANY;
            SO_TOLERANT;
        } CONTRACTL_END;
        return fLegacyV1CASPolicy; 
    }
    void    SetLegacyV1CASPolicy(bool val)              { LEAF_CONTRACT; fLegacyV1CASPolicy = val; }

    void SetLegacyLoadMscorsnOnStartup(bool val) { LEAF_CONTRACT; fLegacyLoadMscorsnOnStartup = val; }
    bool LegacyLoadMscorsnOnStartup(void) const { LEAF_CONTRACT; return fLegacyLoadMscorsnOnStartup; }

#ifdef _DEBUG
    bool GenDebugInfo(void)                         const {LEAF_CONTRACT;  return fDebugInfo; }
    bool GenDebuggableCode(void)                    const {LEAF_CONTRACT;  return fDebuggable; }
    bool IsStressOn(void)                           const {LEAF_CONTRACT;  return fStressOn; }
    int GetAPIThreadStressCount(void)               const {LEAF_CONTRACT;  return apiThreadStressCount; }
    bool TlbImpSkipLoading()                        const {LEAF_CONTRACT;  return fTlbImpSkipLoading; }

    bool ShouldExposeExceptionsInCOMToConsole()     const {LEAF_CONTRACT;  return (iExposeExceptionsInCOM & 1) != 0; }
    bool ShouldExposeExceptionsInCOMToMsgBox()      const {LEAF_CONTRACT;  return (iExposeExceptionsInCOM & 2) != 0; }

    inline bool ShouldPrestubHalt(MethodDesc* pMethodInfo) const
    {
        WRAPPER_CONTRACT;
        return IsInMethList(pPrestubHalt, pMethodInfo);
    }

    inline bool ShouldInvokeHalt(MethodDesc* pMethodInfo) const
    {
        WRAPPER_CONTRACT;
        return IsInMethList(pInvokeHalt, pMethodInfo);
    }


    inline bool ShouldPrestubGC(MethodDesc* pMethodInfo) const
    { 
        WRAPPER_CONTRACT;
        return IsInMethList(pPrestubGC, pMethodInfo);
    }
    inline bool ShouldBreakOnClassLoad(LPCUTF8 className) const
    { 
        CONTRACTL {
            NOTHROW;
            GC_NOTRIGGER;
            // MODE_ANY;
            PRECONDITION(CheckPointer(className, NULL_OK));
        } CONTRACTL_END
        return (pszBreakOnClassLoad != 0 && className != 0 && strcmp(pszBreakOnClassLoad, className) == 0);
    }
    inline bool ShouldBreakOnClassBuild(LPCUTF8 className) const
    { 
        CONTRACTL {
            NOTHROW;
            GC_NOTRIGGER;
            // MODE_ANY;
            PRECONDITION(CheckPointer(className, NULL_OK));
        } CONTRACTL_END
        return (pszBreakOnClassBuild != 0 && className != 0 && strcmp(pszBreakOnClassBuild, className) == 0);
    }
    inline bool ShouldAppendFileNameToTypeName() const
    { 
        CONTRACTL {
            NOTHROW;
            GC_NOTRIGGER;
            // MODE_ANY;
        } CONTRACTL_END
        return fAppendFileNameToTypeName;
    }
    inline bool ShouldBreakOnMethod(LPCUTF8 methodName) const
    {
        CONTRACTL {
            NOTHROW;
            GC_NOTRIGGER;
            // MODE_ANY;
            PRECONDITION(CheckPointer(methodName, NULL_OK));
        } CONTRACTL_END

        return (pszBreakOnMethodName != 0 && methodName != 0 && strcmp(pszBreakOnMethodName, methodName) == 0);
    }
    inline bool ShouldDumpOnClassLoad(LPCUTF8 className) const
    {
        CONTRACTL {
            NOTHROW;
            GC_NOTRIGGER;
            // MODE_ANY;
            PRECONDITION(CheckPointer(className, NULL_OK));
        } CONTRACTL_END
        return (pszDumpOnClassLoad != 0 && className != 0 && strcmp(pszDumpOnClassLoad, className) == 0);
    }
    inline bool ShouldBreakOnInteropStubSetup(LPCUTF8 methodName) const
    {
        CONTRACTL {
            NOTHROW;
            GC_NOTRIGGER;
            // MODE_ANY;
            PRECONDITION(CheckPointer(methodName, NULL_OK));
        } CONTRACTL_END

        return (pszBreakOnInteropStubSetup != 0 && methodName != 0 && strcmp(pszBreakOnInteropStubSetup, methodName) == 0);
    }
    inline bool ShouldBreakOnComToClrNativeInfoInit(LPCUTF8 methodName) const
    {
        CONTRACTL {
            NOTHROW;
            GC_NOTRIGGER;
            // MODE_ANY;
            PRECONDITION(CheckPointer(methodName, NULL_OK));
        } CONTRACTL_END

        return (pszBreakOnComToClrNativeInfoInit != 0 && methodName != 0 && strcmp(pszBreakOnComToClrNativeInfoInit, methodName) == 0);
    }
    inline bool ShouldBreakOnStructMarshalSetup(LPCUTF8 className) const
    { 
        CONTRACTL {
            NOTHROW;
            GC_NOTRIGGER;
            // MODE_ANY;
            PRECONDITION(CheckPointer(className, NULL_OK));
        } CONTRACTL_END

        return (pszBreakOnStructMarshalSetup != 0 && className != 0 && strcmp(pszBreakOnStructMarshalSetup, className) == 0);
    }
    static HRESULT ParseTypeList(__in_z LPWSTR str, TypeNamesList** out);
    static void DestroyTypeList(TypeNamesList* list);

    inline bool ShouldGcCoverageOnMethod(LPCUTF8 methodName) const
    { 
        CONTRACTL {
            NOTHROW;
            GC_NOTRIGGER;
            // MODE_ANY;
            PRECONDITION(CheckPointer(methodName, NULL_OK));
        } CONTRACTL_END
        return (pszGcCoverageOnMethod == 0 || methodName == 0 || strcmp(pszGcCoverageOnMethod, methodName) == 0);
    }

    bool IsJitVerificationDisabled(void)    const {LEAF_CONTRACT;  return fJitVerificationDisable; }

#ifdef STUBLINKER_GENERATES_UNWIND_INFO
    bool IsStubLinkerUnwindInfoVerificationOn() const { LEAF_CONTRACT; return fStubLinkerUnwindInfoVerificationOn; }
#endif
#endif // _DEBUG

    // Because the large object heap is 8 byte aligned, we want to put
    // arrays of doubles there more agressively than normal objects.
    // This is the threshold for this.  It is the number of doubles,
    // not the number of bytes in the array, this constant
    unsigned int  GetDoubleArrayToLargeObjectHeap() const { LEAF_CONTRACT; return DoubleArrayToLargeObjectHeap; }

    inline DWORD DefaultSharePolicy() const
    {
        LEAF_CONTRACT;
        return dwSharePolicy;
    }


    inline BOOL CacheBindingFailures() const
    {
        LEAF_CONTRACT;
        return fCacheBindingFailures;
    }

    inline BOOL UseLegacyIdentityFormat() const
    {
        LEAF_CONTRACT;
        return fUseLegacyIdentityFormat;
    }

    inline void SetDisableCommitThreadStack(bool val)   {LEAF_CONTRACT; fDisableCommitThreadStack = val;}
    inline bool GetDisableCommitThreadStack() const     {LEAF_CONTRACT; return fDisableCommitThreadStack;}

    inline BOOL ProbeForStackOverflow() const
    {
        LEAF_CONTRACT;
        return fProbeForStackOverflow;
    }

    inline bool AppDomainUnload() const
    {LEAF_CONTRACT;  return fAppDomainUnload; }

    inline DWORD AppDomainUnloadRetryCount() const
    {LEAF_CONTRACT;  return dwADURetryCount; }
	

#ifdef _DEBUG
    inline bool AppDomainLeaks() const
    {LEAF_CONTRACT;  return fAppDomainLeaks; }
#endif

    inline bool DeveloperInstallation() const
    {LEAF_CONTRACT;  return m_fDeveloperInstallation; }

#ifdef _DEBUG

    
    unsigned SuspendDeadlockTimeout() const
    {LEAF_CONTRACT; return m_SuspendDeadlockTimeout; }

    // Verifier
    bool    IsVerifierOff()                 const {LEAF_CONTRACT;  return fVerifierOff; }

    inline bool fAssertOnBadImageFormat() const
    {LEAF_CONTRACT;  return m_fAssertOnBadImageFormat; }

    inline bool fAssertOnFailFast() const
    {LEAF_CONTRACT;  return m_fAssertOnFailFast; }

    inline bool SuppressChecks() const
    {LEAF_CONTRACT;  return fSuppressChecks; }

    // Verifier Break routines.
    inline bool IsVerifierBreakOnErrorEnabled() const
    {LEAF_CONTRACT;  return fVerifierBreakOnError; }

    // Skip verifiation routine
    inline bool ShouldVerifierSkip(MethodDesc* pMethodInfo) const
    { WRAPPER_CONTRACT; return IsInMethList(pVerifierSkip, pMethodInfo); }

    // Verifier break routines
    inline bool ShouldVerifierBreak(MethodDesc* pMethodInfo) const
    { WRAPPER_CONTRACT; return IsInMethList(pVerifierBreak, pMethodInfo); }

    inline bool IsVerifierBreakOffsetEnabled() const
    {LEAF_CONTRACT;  return fVerifierBreakOffset; }
    inline bool IsVerifierBreakPassEnabled() const
    {LEAF_CONTRACT;  return fVerifierBreakPass; }
    inline int GetVerifierBreakOffset() const
    {LEAF_CONTRACT;  return iVerifierBreakOffset; }
    inline int GetVerifierBreakPass() const
    {LEAF_CONTRACT;  return iVerifierBreakPass; }

    // Printing of detailed error message, default is ON
    inline bool IsVerifierMsgMethodInfoOff() const
    {LEAF_CONTRACT;  return fVerifierMsgMethodInfoOff; }

    inline bool Do_AllowUntrustedCaller_Checks()
    {LEAF_CONTRACT;  return fDoAllowUntrustedCallerChecks; }

    inline bool EnableFullDebug() const
    {LEAF_CONTRACT;  return fEnableFullDebug; }

#endif

    // CPU flags/capabilities

    void  SetCpuFlag(DWORD val) {LEAF_CONTRACT;  dwCpuFlag = val;  }
    DWORD GetCpuFlag()          {LEAF_CONTRACT;  return dwCpuFlag; }
    void  SetCpuStepping(DWORD val) {LEAF_CONTRACT;  dwCpuStepping = val;  }
    DWORD GetCpuStepping()          {LEAF_CONTRACT;  return dwCpuStepping; }
    
    void  SetCpuModel(DWORD val) {LEAF_CONTRACT;  dwCpuModel = val;  }
    DWORD GetCpuModel()          {LEAF_CONTRACT;  return dwCpuModel; }

    void  SetCpuCapabilities(DWORD val) {LEAF_CONTRACT;  dwCpuCapabilities = val;  }
    DWORD GetCpuCapabilities()          {LEAF_CONTRACT;  return dwCpuCapabilities; }

    // GC config
    enum HeapVerifyFlags {
        HEAPVERIFY_NONE             = 0,
        HEAPVERIFY_GC               = 1,   // Verify the heap at beginning and end of GC
        HEAPVERIFY_BARRIERCHECK     = 2,   // Verify the brick table
        HEAPVERIFY_SYNCBLK          = 4    // Verify sync block scanning
    };
    
    int     GetHeapVerifyLevel()                  {LEAF_CONTRACT;  return iGCHeapVerify;  }
    bool    IsHeapVerifyEnabled()           const {LEAF_CONTRACT;  return iGCHeapVerify != 0; }
    void    SetGCStressLevel(int val)             {LEAF_CONTRACT;  iGCStress = val;  }

    enum  GCStressFlags {
        GCSTRESS_NONE               = 0,
        GCSTRESS_ALLOC              = 1,    // GC on all allocs and 'easy' places
        GCSTRESS_TRANSITION         = 2,    // GC on transitions to preemtive GC
        GCSTRESS_INSTR_JIT          = 4,    // GC on every allowable JITed instr
        GCSTRESS_INSTR_NGEN         = 8,    // GC on every allowable NGEN instr
        GCSTRESS_UNIQUE             = 16,   // GC only on a unique stack trace
    };
    GCStressFlags GetGCStressLevel()        const { WRAPPER_CONTRACT; return GCStressFlags(iGCStress); }

    int     GetGCtraceStart()               const { LEAF_CONTRACT; return iGCtraceStart; }
    int     GetGCtraceEnd  ()               const {LEAF_CONTRACT;  return iGCtraceEnd;   }
    int     GetGCtraceFac  ()               const {LEAF_CONTRACT;  return iGCtraceFac;   }
    int     GetGCprnLvl    ()               const {LEAF_CONTRACT; return iGCprnLvl;     }
    bool    IsGCBreakOnOOMEnabled()         const {LEAF_CONTRACT;  return fGCBreakOnOOM;}
    int     GetGCgen0size  ()               const 
    { 
        LEAF_CONTRACT; 
        return iGCgen0size;
    }
    void    SetGCgen0size  (int iSize)         { LEAF_CONTRACT; iGCgen0size = iSize;  }

    int     GetSegmentSize ()               const {LEAF_CONTRACT;  return iGCSegmentSize; }
    void    SetSegmentSize (int iSize)         {LEAF_CONTRACT;  iGCSegmentSize = iSize; }
    int     GetGCconcurrent()               const {LEAF_CONTRACT;  return iGCconcurrent; }
    void    SetGCconcurrent(int val)           {LEAF_CONTRACT;  iGCconcurrent = val; }
    int     GetGCForceCompact()             const {LEAF_CONTRACT; return iGCForceCompact; }


    DWORD GetStressLoadThreadCount() const
    { LEAF_CONTRACT; return m_dwStressLoadThreadCount; }

    // thread stress: number of threads to run
#ifdef STRESS_THREAD
    DWORD GetStressThreadCount ()           const {LEAF_CONTRACT; return dwStressThreadCount;}
#endif

#ifdef _DEBUG
    inline DWORD FastGCStressLevel() const
    {LEAF_CONTRACT;  return iFastGCStress;}

    inline DWORD InjectFatalError() const
    {
        LEAF_CONTRACT;
        return iInjectFatalError;
    }

    inline BOOL SaveThreadInfo() const
    {
        return fSaveThreadInfo;
    }

    inline DWORD SaveThreadInfoMask() const
    {
        return dwSaveThreadInfoMask;
    }
#endif


    // Interop config
    IUnknown* GetTraceIUnknown()            const {LEAF_CONTRACT;  return m_pTraceIUnknown; }
    int     GetTraceWrapper()               const {LEAF_CONTRACT;  return m_TraceWrapper;      }

    // Loader
    
    bool    UseZaps()                       const {LEAF_CONTRACT;  return fUseZaps; }
    
    enum RequireZapsType
    {
        REQUIRE_ZAPS_NONE,      // Dont care if native image is used or not
        REQUIRE_ZAPS_ALL,       // All assemblies must have native images
        REQUIRE_ZAPS_ALL_JIT_OK,// All assemblies must have native images, but its OK if the JIT-compiler also gets used (if some function was not ngenned)
        REQUIRE_ZAPS_SUPPORTED, // All assemblies must have native images, unless the loader does not support the scenario. Its OK if the JIT-compiler also gets used
        
        REQUIRE_ZAPS_COUNT
    };
    RequireZapsType RequireZaps()           const {LEAF_CONTRACT;  return iRequireZaps; }
    bool    RequireZap(LPCUTF8 assemblyName) const;
    
    LPCWSTR ZapSet()                        const { LEAF_CONTRACT; return pZapSet; }

    // Temporary codegen feature flags
    bool    SecurityNeutralCode()           const { LEAF_CONTRACT; return fSecurityNeutralCode; }
    bool    LazyActivation()                const { LEAF_CONTRACT; return fLazyActivation; }

    bool    UsePcRel32Calls()               const { LEAF_CONTRACT; return fUsePcRel32Calls; }
    bool    UseRipData()                    const { LEAF_CONTRACT; return fUseRipData; }

    bool    ExactInterfaceCalls()           const {LEAF_CONTRACT;  return fExactInterfaceCalls; }
    bool    LazyInterfaceRestore()          const {LEAF_CONTRACT;  return fLazyInterfaceRestore; }


    //@GENERICS:

    // Is code-sharing for generics enabled?
#if defined(FEATURE_SHARE_GENERIC_CODE)  
    bool    ShareGenericCode()              const {LEAF_CONTRACT;  return fShareGenericCode; }
#endif // FEATURE_SHARE_GENERIC_CODE
    bool    FullGenericsNGEN()              const {LEAF_CONTRACT;  return fFullGenericsNGEN; }
    bool    FullArraysNGEN()              const {LEAF_CONTRACT;  return fFullArraysNGEN; }
    bool    DontLoadOpenTypes()             const {LEAF_CONTRACT; return fDontLoadOpenTypes; }

    // ZapMonitor
    // 0 == no monitor
    // 1 == print summary only
    // 2 == print dirty pages, no stack trace
    // 3 == print dirty pages, w/ stack trace
    // 4 == print all pages

    DWORD   MonitorZapStartup()             const { LEAF_CONTRACT; return dwMonitorZapStartup; }
    DWORD   MonitorZapExecution()           const { LEAF_CONTRACT; return dwMonitorZapExecution; }
    LPUTF8  MonitorZapExecutionModule()     const { LEAF_CONTRACT; return szMonitorZapExecutionModule; }
    DWORD   MemmapEnabled()                 const { LEAF_CONTRACT; return dwMemmapEnabled; }


    #define LOADORDER_INSTR                 0x00000001
    #define RID_ACCESSORDER_INSTR           0x00000002
    #define METHODDESC_ACCESS_INSTR         0x00000004
    #define ALL_INSTR                       (LOADORDER_INSTR | RID_ACCESSORDER_INSTR | METHODDESC_ACCESS_INSTR)

    bool    DoBBInstr()                     const {LEAF_CONTRACT;  return fDoBBInstr; }
    LPUTF8  GetZapBBInstr()                 const { LEAF_CONTRACT; return szZapBBInstr; }
    LPWSTR  GetZapBBInstrDir()              const { LEAF_CONTRACT; return szZapBBInstrDir; }
    DWORD   ShowMetaDataAccess()            const {LEAF_CONTRACT;  return fShowMetaDataAccess; }
    DWORD   DisableStackwalkCache()         const {LEAF_CONTRACT;  return dwDisableStackwalkCache; }
    DWORD   UseNewCrossDomainRemoting()     const { LEAF_CONTRACT; return fUseNewCrossDomainRemoting; }

    bool    StressLog()                     const { LEAF_CONTRACT; return fStressLog; }

    // Optimizations to improve working set

    HRESULT sync();    // check the registry again and update local state

    // Helpers to read configuration
    static HRESULT GetConfigString(__in_z LPCWSTR name, __deref_out_z LPWSTR*out, BOOL fPrependCOMPLUS = TRUE,
                                  ConfigSearch direction = CONFIG_SYSTEM); // Note that you own the returned string!

    static DWORD GetConfigDWORD(__in_z LPCWSTR name, DWORD defValue,
                                DWORD level=(DWORD) REGUTIL::COR_CONFIG_ALL,
                                BOOL fPrependCOMPLUS = TRUE,
                                ConfigSearch direction = CONFIG_SYSTEM);
    static DWORD GetConfigDWORDFavoringConfigFile(__in_z LPCWSTR name, DWORD defValue,
                                                  DWORD level=(DWORD) REGUTIL::COR_CONFIG_ALL,
                                                  BOOL fPrependCOMPLUS = TRUE,
                                                  ConfigSearch direction = CONFIG_SYSTEM);

    static DWORD GetConfigFlag(__in_z LPCWSTR name, DWORD bitToSet, bool defValue = FALSE);

#ifdef _DEBUG
    // GC alloc logging
    bool ShouldLogAlloc(const char *pClass) const { LEAF_CONTRACT; return pPerfTypesToLog && pPerfTypesToLog->IsInList(pClass);}
    int AllocSizeThreshold()                const {LEAF_CONTRACT;  return iPerfAllocsSizeThreshold; }
    int AllocNumThreshold()                 const { LEAF_CONTRACT; return iPerfNumAllocsThreshold;  }

#endif // _DEBUG

    BOOL ContinueAfterFatalError()          const {LEAF_CONTRACT;  return fContinueAfterFatalError; }

    enum NgenHardBindType
    {
        NGEN_HARD_BIND_NONE,    // Do not hardbind at all
        NGEN_HARD_BIND_LIST,    // Only hardbind to what is specified by CustomAttributes (and any default assemblies specified by the CLR)
        NGEN_HARD_BIND_ALL,     // Hardbind to any existing ngen images if possible
        NGEN_HARD_BIND_COUNT,
        
        NGEN_HARD_BIND_DEFAULT = NGEN_HARD_BIND_LIST,
    };
    
    NgenHardBindType NgenHardBind() { LEAF_CONTRACT; return iNgenHardBind; }

#ifdef _DEBUG
    BOOL ShouldGenerateStubForHost() const {LEAF_CONTRACT; return fGenerateStubForHost;}
    void DisableGenerateStubForHost() {LEAF_CONTRACT; fGenerateStubForHost = FALSE;}

    DWORD GetHostTestADUnload() const {LEAF_CONTRACT; return testADUnload;}

    DWORD GetHostTestThreadAbort() const {LEAF_CONTRACT; return testThreadAbort;}

#define INJECTFAULT_LOADERHEAP      0x1
#define INJECTFAULT_HANDLETABLE     0x1
#define INJECTFAULT_GCHEAP          0x2
#define INJECTFAULT_SO              0x4
#define INJECTFAULT_GMHEAP          0x8
#define INJECTFAULT_DYNAMICCODEHEAP 0x10
#define INJECTFAULT_MAPVIEWOFFILE   0x20

    DWORD ShouldInjectFault(DWORD faultType) const {LEAF_CONTRACT; return fShouldInjectFault & faultType;}
    
#endif

    BOOL ShouldRejectSafeHandleFinalizers() const {LEAF_CONTRACT; return m_fRejectSafeHandleFinalizers;}

private:

    BOOL fCacheBindingFailures;
    BOOL fUseLegacyIdentityFormat;

    bool fInited;                   // have we synced to the registry at least once?

    // Jit-config

    unsigned iJitOptimizeType; // 0=Blended,1=SmallCode,2=FastCode,              default is 0=Blended
    bool fJitLooseExceptOrder; // Enable/Disable strict exception order.         default is false
    bool fJitFramed;           // Enable/Disable EBP based frames
    bool fJitAlignLoops;       // Enable/Disable loop alignment

    bool fLegacyNullReferenceExceptionPolicy; // Old AV's as NullRef behavior
    bool fLegacyUnhandledExceptionPolicy;     // Old unhandled exception policy (many are swallowed)

    bool fLegacyApartmentInitPolicy;          // Old nondeterministic COM apartment initialization switch
    bool fLegacyComHierarchyVisibility;       // Old behavior allowing QIs for classes with invisible parents
    bool fLegacyComVTableLayout;              // Old behavior passing out IClassX interface for IUnknown and IDispatch.
    bool fNewComVTableLayout;                 // New behavior passing out Basic interface for IUnknown and IDispatch.
    
    // SECURITY
    unsigned  iImpersonationPolicy; //control flow of impersonation in the SecurityContext. 0=FASTFLOW 1=
    bool fLegacyV1CASPolicy;
    bool fTransparencyEnforcementDisabled; // disable Security Transparency enforcement
    bool fLegacyLoadMscorsnOnStartup; // load mscorsn.dll when starting up the runtime.

    LPUTF8 pszBreakOnClassLoad;         // Halt just before loading this class

#ifdef _DEBUG
    static HRESULT ParseMethList(__in_z LPWSTR str, MethodNamesList* * out);
    static void DestroyMethList(MethodNamesList* list);
    static bool IsInMethList(MethodNamesList* list, MethodDesc* pMD);

    bool fDebugInfo;
    bool fDebuggable;
    bool fStressOn;
    int apiThreadStressCount;

    MethodNamesList* pPrestubHalt;      // list of methods on which to break when hit prestub
    MethodNamesList* pPrestubGC;        // list of methods on which to cause a GC when hit prestub
    MethodNamesList* pInvokeHalt;      // list of methods on which to break when hit prestub


    LPUTF8 pszBreakOnClassBuild;         // Halt just before loading this class
    bool   fAppendFileNameToTypeName;
    LPUTF8 pszBreakOnMethodName;         // Halt when doing something with this method in the class defined in ClassBuild
    LPUTF8 pszDumpOnClassLoad;           // Dump the class to the log

    LPUTF8 pszBreakOnInteropStubSetup;   // Halt before we set up the interop stub for a method
    LPUTF8 pszBreakOnComToClrNativeInfoInit; // Halt before we init the native info for a COM to CLR call
    LPUTF8 pszBreakOnStructMarshalSetup; // Halt before the field marshallers are set up for a struct

    bool   fAppDomainLeaks;             // Enable appdomain leak detection for object refs

    bool   m_fAssertOnBadImageFormat;   // If false, don't assert on invalid IL (for testing)
    bool   m_fAssertOnFailFast;         // If false, don't assert if we detect a stack corruption

    bool   fConditionalContracts;       // Conditional contracts (off inside asserts)
    bool   fSuppressChecks;             // Disable checks (including contracts)

    DWORD  iExposeExceptionsInCOM;      // Should we exposed exceptions that will be transformed into HRs?

    // Tlb Tools
    bool fTlbImpSkipLoading;

    unsigned m_SuspendDeadlockTimeout; // Used in SysSuspendForGC. 

    bool fEnableFullDebug;
#endif // _DEBUG
    unsigned int DoubleArrayToLargeObjectHeap;  // double arrays of more than this number of elems go in large object heap

    DWORD  dwSharePolicy;               // Default policy for loading assemblies into the domain neutral area
    DWORD  dwMultiDomainHostType;        // meaning of MultiDomainHost policy

    // Only developer machines are allowed to use DEVPATH. This value is set when there is an appropriate entry
    // in the machine configuration file. This should not be sent out in the redist.
    bool   m_fDeveloperInstallation;      // We are on a developers machine
    bool   fAppDomainUnload;            // Enable appdomain unloading
    DWORD  dwADURetryCount;
#ifdef _DEBUG
    bool fJitVerificationDisable;       // Turn off jit verification (for testing purposes only)

    // Verifier
    bool fVerifierOff;

    //
    // Verifier debugging options
    //
    // "VerBreakOnError" to break on verification error.
    //
    // To Skip verifiation of a methods, set "VerSkip" to a list of methods.
    //
    // Set "VerBreak" to a list of methods and verifier will halt when
    // the method is being verified.
    //
    // To break on an IL offset, set "VerOffset"
    // To break on Pass0 / Pass1, set "VerPass"
    //
    // NOTE : If there are more than one methods in the list and an offset
    // is specified, this offset is applicable to all methods in the list
    //

    bool fVerifierBreakOnError;  // Break on error
    MethodNamesList*  pVerifierSkip;  // methods Skipping verifier
    MethodNamesList*  pVerifierBreak; // methods to break in the verifier
    int  iVerifierBreakOffset;   // break while parsing this offset
    int  iVerifierBreakPass;     // break in pass0 / pass1
    bool fVerifierBreakOffset;   // Offset is valid if true
    bool fVerifierBreakPass;     // Pass is valid if true
    bool fVerifierMsgMethodInfoOff; // detailed errorMessage Off
    bool fDoAllowUntrustedCallerChecks; // do AllowUntrustedCallerChecks

#ifdef STUBLINKER_GENERATES_UNWIND_INFO
    bool fStubLinkerUnwindInfoVerificationOn;
#endif
#endif // _DEBUG

    // GC config
    int  iGCHeapVerify;
    int  iGCStress;
    int  iGCtraceStart;
    int  iGCtraceEnd;
    int  iGCtraceFac;
#define DEFAULT_GC_PRN_LVL 3
    int  iGCprnLvl;
    int  iGCgen0size;
    int  iGCSegmentSize;
    int  iGCconcurrent;
    int  iGCForceCompact;
    DWORD m_dwStressLoadThreadCount;

#ifdef THREAD_DELAY
public:
    ThreadDelay *m_pThreadDelay;
private:
#endif //THREAD_DELAY    

    bool fGCBreakOnOOM;

#ifdef  STRESS_THREAD
    DWORD dwStressThreadCount;
#endif

#ifdef _DEBUG
    DWORD iFastGCStress;
    LPUTF8 pszGcCoverageOnMethod;

    DWORD iInjectFatalError;

    BOOL fSaveThreadInfo;
    DWORD dwSaveThreadInfoMask;
#endif

    // Loader
    bool fUseZaps;

    RequireZapsType iRequireZaps;
    // Assemblies which need to have native images. 
    // This is only used if iRequireZaps!=REQUIRE_ZAPS_NONE
    // This can be used to enforce that ngen images are used only selectively for some assemblies
    AssemblyNamesList * pRequireZapsList;
    // assemblies which need NOT have native images.
    // This is only used if iRequireZaps!=REQUIRE_ZAPS_NONE
    // This overrides pRequireZapsList.
    AssemblyNamesList * pRequireZapsExcludeList;

    bool fExactInterfaceCalls;
    bool fLazyInterfaceRestore;

    LPCWSTR pZapSet;

    // Temporary codegen features
    bool fSecurityNeutralCode; // security neutral codegen
    bool fLazyActivation; // Lazy module activation

    bool fUsePcRel32Calls; // Use pcrel32 calls for external calls
    bool fUseRipData;      // Use rip relative for data references

    //@GENERICS: controls for generics; see accessor methods for details
#if defined(FEATURE_SHARE_GENERIC_CODE)  
    bool fShareGenericCode;
#endif
    bool fFullGenericsNGEN;
    bool fFullArraysNGEN;
    bool fDontLoadOpenTypes;
    bool fStressLog;

    // Zap monitor
    DWORD dwMonitorZapStartup;
    DWORD dwMonitorZapExecution;
    LPUTF8 szMonitorZapExecutionModule;
    DWORD dwMemmapEnabled;

    // Metadata tracker
    DWORD fShowMetaDataAccess;
    DWORD dwMetaDataPageNumber;
    LPCWSTR szMetaDataFileName;

    // Stackwalk optimization flag
    DWORD dwDisableStackwalkCache;

    // New cross domain remoting
    DWORD fUseNewCrossDomainRemoting;
    
    bool fDoBBInstr;
    DWORD dwInstrEnabled;
    LPUTF8 szZapBBInstr;
    LPWSTR szZapBBInstrDir;

    bool fDisableCommitThreadStack;
    BOOL fProbeForStackOverflow;

#define COM_SLOT_MODE_ORIGINAL  0       // Use com slot data in metadata
#define COM_SLOT_MODE_LOG       1       // Ignore com slot, log descrepencies
#define COM_SLOT_MODE_ASSERT    2       // Ignore com slot, assert on descrepencies
    // CPU flags

    DWORD dwCpuFlag;
    DWORD dwCpuCapabilities;
    // RDG
    DWORD dwCpuStepping;
    DWORD dwCpuModel;
    // interop logging
    IUnknown* m_pTraceIUnknown;
    int       m_TraceWrapper;

    // pump flags
    int     m_fPumpAllUser;

    // Flag to keep track of memory
    int     m_fFreepZapSet;

#ifdef _DEBUG
    // GC Alloc perf flags
    int iPerfNumAllocsThreshold;            // Start logging after this many allocations are made
    int iPerfAllocsSizeThreshold;           // Log allocations of this size or above
    TypeNamesList* pPerfTypesToLog;     // List of types whose allocations are to be logged

#endif // _DEBUG

    // New configuration
    ConfigList  m_Configuration;

    // Behavior on fatal errors.
    BOOL fContinueAfterFatalError;

    NgenHardBindType iNgenHardBind;

#ifdef _DEBUG
    BOOL fGenerateStubForHost;

    DWORD fShouldInjectFault;

    DWORD testADUnload;
    DWORD testThreadAbort;
#endif

    BOOL m_fRejectSafeHandleFinalizers;
public:
    HRESULT AppendConfigurationFile(
        LPCWSTR pszFileName,
        LPCWSTR version,
        ParseCtl parseCtl = parseAll); 
    HRESULT SetupConfiguration();

    HRESULT GetConfiguration(__in_z LPCWSTR pKey, ConfigSearch direction, __deref_out_opt LPCWSTR* value);
    LPCWSTR  GetProcessBindingFile();  // All flavors must support this method
    SIZE_T  GetSizeOfProcessBindingFile();  // All flavors must support this method

    DWORD GetConfigDWORDInternal (__in_z LPCWSTR name, DWORD defValue,    //for getting data in the constructor of EEConfig
                                    DWORD level=(DWORD) REGUTIL::COR_CONFIG_ALL,
                                    BOOL fPrependCOMPLUS = TRUE,
                                    ConfigSearch direction = CONFIG_SYSTEM);

#if defined(_DEBUG)
public:
    // Just return false when we're in DEBUG but not on AMD64
    BOOL ShouldGenerateLongJumpDispatchStub()
    {
        return FALSE;
    }
#endif // _DEBUG


};



#ifdef _DEBUG_IMPL

    // We actually want our asserts for illegal IL, but testers need to test that
    // we fail gracefully under those conditions.  Thus we have to hide them for those runs.
#define BAD_FORMAT_NOTHROW_ASSERT(str)                                  \
    do {                                                                \
        if (g_pConfig->fAssertOnBadImageFormat()) {                     \
            _ASSERTE(str);                                              \
        }                                                               \
        else if (!(str)) {                                              \
            if (IsDebuggerPresent()) DebugBreak();                      \
        }                                                               \
    } while(0)

    // STRESS_ASSERT is meant to be temperary additions to the code base that stop the
    // runtime quickly when running stress
#define STRESS_ASSERT(cond)   do { if (!(cond) && g_pConfig->IsStressOn())  DebugBreak();    } while(0)

#define FILE_FORMAT_CHECK_MSG(_condition, _message)                     \
    do {                                                                \
        if (g_pConfig != NULL && g_pConfig->fAssertOnBadImageFormat())  \
             ASSERT_CHECK(_condition, _message, "Bad file format");     \
        else if (!(_condition))                                         \
            DebugBreak();                                               \
    } while (0)

#define FILE_FORMAT_CHECK(_condition)  FILE_FORMAT_CHECK_MSG(_condition, "")

#else

#define STRESS_ASSERT(cond)
#define BAD_FORMAT_NOTHROW_ASSERT(str)

#define FILE_FORMAT_CHECK_MSG(_condition, _message)
#define FILE_FORMAT_CHECK(_condition)

#endif

void InitHostProtectionManager();

extern BYTE g_CorHostProtectionManagerInstance[];

inline CorHostProtectionManager* GetHostProtectionManager()
{
    CONTRACTL
    {
        NOTHROW;
        GC_NOTRIGGER;
//        MODE_ANY;
        SO_TOLERANT;
    }
    CONTRACTL_END;

    return (CorHostProtectionManager*)g_CorHostProtectionManagerInstance;
}

extern BOOL g_CLRPolicyRequested;

bool IsRunningUnderZapmon();

#endif // EECONFIG_H
