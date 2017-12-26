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
// DllImport.h -
//
//

#ifndef __dllimport_h__
#define __dllimport_h__

#include "util.hpp"
#include "ml.h"

class NDirectMLStubCache;
typedef NDirectMLStubCache  NDirectStubCache;
class ArgBasedStubCache;
struct PInvokeStaticSigInfo;
class LoadLibErrorTracker;

//=======================================================================
// Collects code and data pertaining to the NDirect interface.
//=======================================================================
class NDirect
{
    friend class NDirectMethodDesc;

public:
    //---------------------------------------------------------
    // One-time init
    //---------------------------------------------------------
    static void Init();

    //---------------------------------------------------------
    // Handles system specfic portion of generic NDirect stub creation
    //---------------------------------------------------------
    static void CreateGenericNDirectStubSys(CPUSTUBLINKER *psl, UINT numStackBytes);

    //---------------------------------------------------------
    // Handles system specfic portion of fully optimized NDirect stub creation
    //
    // Results:
    //     TRUE     - was able to create a standalone asm stub (generated into
    //                psl)
    //     FALSE    - decided not to create a standalone asm stub due to
    //                to the method's complexity. Stublinker remains empty!
    //
    //     COM+ exception - error - don't trust state of stublinker.
    //---------------------------------------------------------
    static BOOL CreateStandaloneNDirectStubSys(const MLHeader *pheader, CPUSTUBLINKER *psl, BOOL fDoComInterop);

    //---------------------------------------------------------
    // Does a class or method have a NAT_L CustomAttribute?
    //
    // S_OK    = yes
    // S_FALSE = no
    // FAILED  = unknown because something failed.
    //---------------------------------------------------------
    static HRESULT HasNAT_LAttribute(IMDInternalImport *pInternalImport, mdToken token);


    static LPVOID NDirectGetEntryPoint(NDirectMethodDesc *pMD, HINSTANCE hMod, UINT16 numParamBytes);
    static HINSTANCE LoadLibraryModule( NDirectMethodDesc * pMD, LPCUTF8 name, bool loadToExecute, LoadLibErrorTracker *pErrorTracker = NULL );
    static VOID NDirectLink(NDirectMethodDesc *pMD, UINT16 numParamBytes);
    static Stub* ComputeNDirectMLStub(NDirectMethodDesc *pMD, bool computeToExecute = true);
    static void PopulateNDirectMethodDesc(NDirectMethodDesc* pNMD, PInvokeStaticSigInfo* pSigInfo);

    static Stub* GetNDirectMethodStub(StubLinker *pstublinker, NDirectMethodDesc *pMD);
    static Stub* CreateGenericNDirectStub(StubLinker *pstublinker, UINT cbStackPop);
    static Stub* CreateSlimNDirectStub(StubLinker *psl, NDirectMethodDesc *pMD, UINT cbStackPop);

    static BOOL IsHostHookEnabled();

private:
    NDirect() {LEAF_CONTRACT;};     // prevent "new"'s on this class

    //---------------------------------------------------------
    // Stub caches for NDirect Method stubs
    //---------------------------------------------------------
    static NDirectStubCache* m_pNDirectStubCache;
    static ArgBasedStubCache *m_pNDirectGenericStubCache;
    static ArgBasedStubCache *m_pNDirectSlimStubCache;
};

#ifdef _X86_
#endif

//----------------------------------------------------------------
// Flags passed to CreateNDirectStub that control stub generation
//----------------------------------------------------------------
enum NDirectStubFlags
{
    NDIRECTSTUB_FL_CONVSIGASVARARG          = 0x00000001,
    NDIRECTSTUB_FL_BESTFIT                  = 0x00000002,
    NDIRECTSTUB_FL_THROWONUNMAPPABLECHAR    = 0x00000004,
    NDIRECTSTUB_FL_NGENEDSTUB               = 0x00000008,
    NDIRECTSTUB_FL_DELEGATE                 = 0x00000010,
    NDIRECTSTUB_FL_DOHRESULTSWAPPING        = 0x00000020,
    NDIRECTSTUB_FL_REVERSE_INTEROP          = 0x00000040,
    NDIRECTSTUB_FL_NGENEDSTUBFORPROFILING   = 0x00000100,
    NDIRECTSTUB_FL_GENERATEDEBUGGABLEIL     = 0x00000200,
                                           // 0x00000400 // UNUSED

// NOTE: if you add more flags, you will also need to update the bit fields in
// NDirectStubHashBlob in dllimport.cpp
#define NDIRECTSTUB_FLAGS_BITS_USED 11

    // internal flags -- these won't ever show up in an NDirectStubHashBlob
    NDIRECTSTUB_FL_USEIL                    = 0x00010000,
    NDIRECTSTUB_FL_FORCEML                  = 0x00020000,
};


struct PInvokeStaticSigInfo
{
public:
    enum ThrowOnError { THROW_ON_ERROR = TRUE, NO_THROW_ON_ERROR = FALSE };

public:     
    PInvokeStaticSigInfo() { LEAF_CONTRACT; }

    PInvokeStaticSigInfo(
        PCCOR_SIGNATURE pSig, DWORD cSig, Module* pModule, BOOL bIsStatic, ThrowOnError throwOnError = THROW_ON_ERROR);
    
    PInvokeStaticSigInfo(MethodDesc* pMdDelegate, ThrowOnError throwOnError = THROW_ON_ERROR);
    
    PInvokeStaticSigInfo(MethodDesc* pMD, LPCUTF8 *pLibName, LPCUTF8 *pEntryPointName, ThrowOnError throwOnError = THROW_ON_ERROR);

public:     
    void ReportErrors();
    
private:
    void InitCallConv(CorPinvokeMap callConv, BOOL bIsVarArg);
    void DllImportInit(MethodDesc* pMD, LPCUTF8 *pLibName, LPCUTF8 *pEntryPointName);
    void PreInit(Module* pModule, mdTypeDef tkDeclType);
    void PreInit(MethodDesc* pMD);
    void SetError(WORD error) { if (!m_error) m_error = error; }

public:     
    DWORD GetStubFlags() 
    { 
        WRAPPER_CONTRACT;
        return (GetThrowOnUnmappableChar() ? NDIRECTSTUB_FL_THROWONUNMAPPABLECHAR : 0) | 
               (GetBestFitMapping() ? NDIRECTSTUB_FL_BESTFIT : 0) | 
               (IsDelegateInterop() ? NDIRECTSTUB_FL_DELEGATE : 0);
    }
    Module* GetModule() { LEAF_CONTRACT; return m_pModule; }
    BOOL IsStatic() { LEAF_CONTRACT; return m_wFlags & PINVOKE_STATIC_SIGINFO_IS_STATIC; }
    void SetIsStatic (BOOL isStatic) 
    { 
        LEAF_CONTRACT; 
        if (isStatic) 
            m_wFlags |= PINVOKE_STATIC_SIGINFO_IS_STATIC; 
        else 
            m_wFlags &= ~PINVOKE_STATIC_SIGINFO_IS_STATIC; 
    }
    BOOL GetThrowOnUnmappableChar() { LEAF_CONTRACT; return m_wFlags & PINVOKE_STATIC_SIGINFO_THROW_ON_UNMAPPABLE_CHAR; }
    void SetThrowOnUnmappableChar (BOOL throwOnUnmappableChar) 
    { 
        LEAF_CONTRACT; 
        if (throwOnUnmappableChar) 
            m_wFlags |= PINVOKE_STATIC_SIGINFO_THROW_ON_UNMAPPABLE_CHAR; 
        else 
            m_wFlags &= ~PINVOKE_STATIC_SIGINFO_THROW_ON_UNMAPPABLE_CHAR; 
    }
    BOOL GetBestFitMapping() { LEAF_CONTRACT; return m_wFlags & PINVOKE_STATIC_SIGINFO_BEST_FIT; }
    void SetBestFitMapping (BOOL bestFit) 
    { 
        LEAF_CONTRACT; 
        if (bestFit) 
            m_wFlags |= PINVOKE_STATIC_SIGINFO_BEST_FIT; 
        else 
            m_wFlags &= ~PINVOKE_STATIC_SIGINFO_BEST_FIT; 
    }
    BOOL IsDelegateInterop() { LEAF_CONTRACT; return m_wFlags & PINVOKE_STATIC_SIGINFO_IS_DELEGATE_INTEROP; } 
    void SetIsDelegateInterop (BOOL delegateInterop) 
    { 
        LEAF_CONTRACT; 
        if (delegateInterop) 
            m_wFlags |= PINVOKE_STATIC_SIGINFO_IS_DELEGATE_INTEROP; 
        else 
            m_wFlags &= ~PINVOKE_STATIC_SIGINFO_IS_DELEGATE_INTEROP; 
    }
    CorPinvokeMap GetCallConv() { LEAF_CONTRACT; return m_callConv; }
    PCCOR_SIGNATURE GetSig() { LEAF_CONTRACT; return m_pSig; }
    DWORD GetSigCount() { LEAF_CONTRACT; return m_cSig; }
    mdMethodDef GetMethodToken() { LEAF_CONTRACT; return m_tkMethod; }
    
private:
    Module* m_pModule;
    mdMethodDef m_tkMethod;
    DWORD m_cSig;
    PCCOR_SIGNATURE m_pSig;
    CorPinvokeMap m_callConv;
    WORD m_error;

    enum 
    {
        PINVOKE_STATIC_SIGINFO_IS_STATIC = 0x0001,
        PINVOKE_STATIC_SIGINFO_THROW_ON_UNMAPPABLE_CHAR = 0x0002,
        PINVOKE_STATIC_SIGINFO_BEST_FIT = 0x0004,

        COR_NATIVE_LINK_TYPE_MASK = 0x0038,  // 0000 0000 0011 1000  <--- These 3 1's make the link type mask
        
        COR_NATIVE_LINK_FLAGS_MASK = 0x00C0, //0000 0000 1100 0000  <---- These 2 bits make up the link flags

        PINVOKE_STATIC_SIGINFO_IS_DELEGATE_INTEROP = 0x0100,
        
    };
    #define COR_NATIVE_LINK_TYPE_SHIFT 3 // Keep in synch with above mask
    #define COR_NATIVE_LINK_FLAGS_SHIFT 6  // Keep in synch with above mask
    WORD m_wFlags;

  public:
    CorNativeLinkType GetCharSet() { LEAF_CONTRACT; return (CorNativeLinkType)((m_wFlags & COR_NATIVE_LINK_TYPE_MASK) >> COR_NATIVE_LINK_TYPE_SHIFT); }
    CorNativeLinkFlags GetLinkFlags() { LEAF_CONTRACT; return (CorNativeLinkFlags)((m_wFlags & COR_NATIVE_LINK_FLAGS_MASK) >> COR_NATIVE_LINK_FLAGS_SHIFT); }
    void SetCharSet(CorNativeLinkType linktype) 
    { 
        LEAF_CONTRACT; 
        _ASSERTE( linktype == (linktype & (COR_NATIVE_LINK_TYPE_MASK >> COR_NATIVE_LINK_TYPE_SHIFT))); 
        // Clear out the old value first
        m_wFlags &= (~COR_NATIVE_LINK_TYPE_MASK);
        // Then set the given value
        m_wFlags |= (linktype << COR_NATIVE_LINK_TYPE_SHIFT); 
    }
    void SetLinkFlags(CorNativeLinkFlags linkflags) 
    { 
        LEAF_CONTRACT; 
        _ASSERTE( linkflags == (linkflags & (COR_NATIVE_LINK_FLAGS_MASK >> COR_NATIVE_LINK_FLAGS_SHIFT)));
        // Clear out the old value first
        m_wFlags &= (~COR_NATIVE_LINK_FLAGS_MASK);
        // Then set the given value
        m_wFlags |= (linkflags << COR_NATIVE_LINK_FLAGS_SHIFT);
    }
};


//=======================================================================
// The ML stub for N/Direct begins with this header. Immediately following
// this header are the ML codes for marshaling the arguments, terminated
// by ML_INTERRUPT. Immediately following that are the ML codes for
// marshaling the return value, terminated by ML_END.
//=======================================================================





inline BOOL NDirectOnUnicodeSystem()
{
    WRAPPER_CONTRACT;
    return RunningOnWinNT();
}

Stub * CreateNDirectStub(PInvokeStaticSigInfo* pSigInfo,
                         DWORD dwStubFlags,
                         MethodDesc* pMD = NULL,
                         VASigCookie *pVASigCookie = NULL
                         DEBUG_ARG(LPCUTF8 pDebugName = NULL)
                         DEBUG_ARG(LPCUTF8 pDebugClassName = NULL)
                         DEBUG_ARG(LPCUTF8 pDebugNameSpace = NULL));


Stub * CreateNDirectStub(PCCOR_SIGNATURE szMetaSig,
                         DWORD cbMetaSigSize,
                         Module*    pModule,
                         mdMethodDef md,
                         CorNativeLinkType nlType,
                         CorNativeLinkFlags nlFlags,
                         CorPinvokeMap unmgdCallConv,
                                DWORD              dwStubFlags,  // NDirectStubFlags
                                MethodDesc*        pMD,
                                VASigCookie*       pVASigCookie
                         DEBUG_ARG(LPCUTF8 pDebugName = NULL)
                         DEBUG_ARG(LPCUTF8 pDebugClassName = NULL)
                         DEBUG_ARG(LPCUTF8 pDebugNameSpace = NULL)
                         );


Stub * CreateNDirectStubNoThrow(PCCOR_SIGNATURE    szMetaSig,
                                DWORD              cbMetaSigSize,
                                Module*            pModule,
                                mdMethodDef        md,
                                CorNativeLinkType  nlType,
                                CorNativeLinkFlags nlFlags,
                                CorPinvokeMap      unmgdCallConv,
                                DWORD              dwStubFlags,  // NDirectStubFlags
                                MethodDesc*        pMD,
                                VASigCookie*       pVASigCookie
                       DEBUG_ARG(LPCUTF8   pDebugName = NULL)
                       DEBUG_ARG(LPCUTF8   pDebugClassName = NULL)
                       DEBUG_ARG(LPCUTF8   pDebugNameSpace = NULL)
                                );


FCDECL1(VOID, NDirect_Prelink_Wrapper, MethodDesc* pMethod);
VOID NDirect_Prelink(MethodDesc *pMeth);
FCDECL1(INT32, NDirect_NumParamBytes, MethodDesc* pMethod);



//---------------------------------------------------------
// Helper function to checkpoint the thread allocator for cleanup.
//---------------------------------------------------------
VOID __stdcall DoCheckPointForCleanup(NDirectMethodFrameEx *pFrame, Thread *pThread);




Stub  * GetMLStubForCalli(VASigCookie *pVASigCookie, BOOL fVarargPinvoke);

#ifdef LOP_FRIENDLY_FRAMES
extern ConfigDWORD g_LOPFriendlyFrames;
#endif //LOP_FRIENDLY_FRAMES

Stub* GetStubForInteropMethod(MethodDesc* pMD, DWORD dwStubFlags = 0);

#endif // __ndirect_h__

