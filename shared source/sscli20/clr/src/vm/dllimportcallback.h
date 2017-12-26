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
// DllImportCallback.h -
//
//


#ifndef __dllimportcallback_h__
#define __dllimportcallback_h__

#include "object.h"
#include "stublink.h"
#include "ml.h"
#include "ceeload.h"
#include "class.h"
#include "dllimport.h"
#include "mdaassistantsptr.h"

enum UMThunkMLStubFlags
{
    umtmlIsStatic           = 0x0001,
    umtmlThisCall           = 0x0002,
    umtmlThisCallHiddenArg  = 0x0004,
    umtmlFpu                = 0x0008,
    umtmlRetValRequiredGCProtect = 0x0010,
    umtmlHasReturnBuffer    = 0x0020,
};

#include <pshpack1.h>
//--------------------------------------------------------------------------
// This structure forms a header for the marshaling code for an UMThunk.
//--------------------------------------------------------------------------
struct UMThunkMLStub
{
    const MLCode *GetMLCode() const
    {
        LEAF_CONTRACT;
        return (const MLCode *)(this+1);
    }

    UINT16        m_cbDstStack;         //# of bytes of stack portion of managed args
    UINT16        m_cbSrcStack;         //# of bytes of stack portion of unmanaged args
    UINT16        m_cbRetPop;           //# of bytes to pop on return to unmanaged
    UINT16        m_cbLocals;           //# of bytes required in the local array
    UINT16        m_cbRetValSize;       //# of bytes of retval (including stack promotion)
    UINT16        m_wFlags;             // UMThunkMLStubFlags enum
};
#include <poppack.h>

//

class UMThunkMarshInfo
{
    friend class UMThunkStubCache;
    friend class CheckAsmOffsets;

private:
    enum
    {
        kLoadTimeInited = 0x4c55544d,   //'LUTM'
        kRunTimeInited  = 0x5255544d,   //'RUTM'
    };

public:
    //----------------------------------------------------------
    // This initializer can be called during load time.
    // It does not do any ML stub initialization or sigparsing.
    // The RunTimeInit() must be called subsequently before this
    // can safely be used.
    //----------------------------------------------------------
    VOID LoadTimeInit(MethodDesc* pMD);
    VOID LoadTimeInit(PInvokeStaticSigInfo* pSigInfo);
    VOID _LoadTimeInit(PInvokeStaticSigInfo* pSigInfo);

    //----------------------------------------------------------
    // This initializer finishes the init started by LoadTimeInit.
    // It does all the ML stub creation, and can throw a COM+
    // exception.
    //
    // It can safely be called multiple times and by concurrent
    // threads.
    //----------------------------------------------------------
    VOID RunTimeInit();

    // Destructor.
    //----------------------------------------------------------
    ~UMThunkMarshInfo();

    //----------------------------------------------------------
    // Accessor functions
    //----------------------------------------------------------
    PCCOR_SIGNATURE GetSig(DWORD *pcbSigSize = NULL)
    {
        CONTRACT (PCCOR_SIGNATURE)
        {
            NOTHROW;
            GC_NOTRIGGER;
            MODE_ANY;
            PRECONDITION(IsAtLeastLoadTimeInited());
        }
        CONTRACT_END;

        if (pcbSigSize != NULL)
        {
            *pcbSigSize = m_sigInfo.GetSigCount();
        }

        RETURN m_sigInfo.GetSig();
    }

    Module* GetModule()
    {
        CONTRACT (Module*)
        {
            NOTHROW;
            GC_NOTRIGGER;
            MODE_ANY;
            PRECONDITION(IsAtLeastLoadTimeInited());
            POSTCONDITION(CheckPointer(RETVAL, NULL_OK));
        }
        CONTRACT_END;

        RETURN m_sigInfo.GetModule();
    }

    BOOL IsStatic()
    {
        CONTRACTL
        {
            NOTHROW;
            GC_NOTRIGGER;
            MODE_ANY;
            PRECONDITION(IsAtLeastLoadTimeInited());
        }
        CONTRACTL_END;

        return m_sigInfo.IsStatic();
    }

    Stub* GetMLStub()
    {
        CONTRACT (Stub*)
        {
            NOTHROW;
            GC_NOTRIGGER;
            MODE_ANY;
            PRECONDITION(IsCompletelyInited());
            POSTCONDITION(CheckPointer(RETVAL, NULL_OK));
        }
        CONTRACT_END;

        RETURN m_pMLStub;
    }

    Stub* GetExecStub()
    {
        CONTRACT (Stub*)
        {
            NOTHROW;
            GC_NOTRIGGER;
            MODE_ANY;
            PRECONDITION(IsCompletelyInited());
            POSTCONDITION(CheckPointer(RETVAL, NULL_OK));
        }
        CONTRACT_END;

        RETURN m_pExecStub;
    }

    UINT32 GetCbRetPop()
    {
        CONTRACTL
        {
            NOTHROW;
            GC_NOTRIGGER;
            MODE_ANY;
            PRECONDITION(IsCompletelyInited());
        }
        CONTRACTL_END;

        return m_cbRetPop;
    }

    UINT32 GetCbActualArgSize()
    {
        CONTRACTL
        {
            NOTHROW;
            GC_NOTRIGGER;
            MODE_ANY;
            PRECONDITION(IsCompletelyInited());
        }
        CONTRACTL_END;

        return m_cbActualArgSize;
    }

    CorPinvokeMap GetUnmanagedCallConv()
    {
        // In the future, we'll derive the unmgdCallConv from the signature
        // rather than having it passed in separately. To avoid having
        // to parse the signature at loadtimeinit (not necessarily a problem
        // but we want to keep the amount of loadtimeinit processing at
        // a minimum), we'll be extra-strict here to prevent other code
        // from depending on the callconv being available at loadtime.

        CONTRACTL
        {
            NOTHROW;
            GC_NOTRIGGER;
            MODE_ANY;
            PRECONDITION(IsCompletelyInited());
        }
        CONTRACTL_END;

        return m_sigInfo.GetCallConv();
    }

    BYTE GetNLType()
    {
        CONTRACTL
        {
            NOTHROW;
            GC_NOTRIGGER;
            MODE_ANY;
            PRECONDITION(IsAtLeastLoadTimeInited());
        }
        CONTRACTL_END;

        return (BYTE)m_sigInfo.GetCharSet();
    }

#ifdef _DEBUG
    BOOL IsAtLeastLoadTimeInited()
    {
        LEAF_CONTRACT;
        return (m_state == kLoadTimeInited || m_state == kRunTimeInited);
    }


    BOOL IsCompletelyInited()
    {
        LEAF_CONTRACT;
        return (m_state == kRunTimeInited);
    }
#endif


private:
    size_t            m_state;              // the initialization state
    PInvokeStaticSigInfo m_sigInfo;         // information obtained from static sources (CustAttr & MetaSig)
    SigTypeContext    m_typeContext;        // instantiations of any type parameters: actually all code involved in these UMThunks 
                                            // should be non-generic but we propogate a type context just for uniformity 
                                            // and because it's easy to do.
    Stub*             m_pMLStub;            // if interpreted, UmThunkMLHeader-prefixed ML stub for marshaling - NULL otherwise
    Stub*             m_pExecStub;          // UMEntryThunk jumps directly here
    UINT32            m_cbRetPop;           // stack bytes popped by callee (for UpdateRegDisplay)
    UINT32            m_cbActualArgSize;    // caches m_pSig.SizeOfFrameArgumentArray()
};



//----------------------------------------------------------------------
// This structure contains the minimal information required to
// distinguish one function pointer from another, with the rest
// being stored in a shared UMThunkMarshInfo.
//
// This structure also contains the actual code bytes that form the
// front end of the thunk. A pointer to the m_code[] byte array is
// what is actually handed to unmanaged client code.
//----------------------------------------------------------------------
class UMEntryThunk
{
    friend class CheckAsmOffsets;
    friend class UMThunkStubCache;
    friend class NDirectStubLinker;

private:
#ifdef _DEBUG
    enum
    {
        kLoadTimeInited = 0x4c554554,   //'LUET'
        kRunTimeInited  = 0x52554554,   //'RUET'
    };
#endif

public:
    static UMEntryThunk* CreateUMEntryThunk();
    static VOID FreeUMEntryThunk(UMEntryThunk* p);

#ifndef DACCESS_COMPILE
    VOID LoadTimeInit(const BYTE             *pManagedTarget,
                      OBJECTHANDLE            pObjectHandle,
                      UMThunkMarshInfo       *pUMThunkMarshInfo,
                      MethodDesc             *pMD,
                      ADID                    dwDomainId)
    {
        CONTRACTL
        {
            NOTHROW;
            GC_NOTRIGGER;
            MODE_ANY;
            PRECONDITION(CheckPointer(pUMThunkMarshInfo));
            PRECONDITION(pUMThunkMarshInfo->IsAtLeastLoadTimeInited());
            PRECONDITION(pMD != NULL);
            // At most one of pManagedTarget and pObjectHandle should be specified
            PRECONDITION(pManagedTarget == NULL || pObjectHandle == NULL);
        }
        CONTRACTL_END;

        m_pManagedTarget = pManagedTarget;
        m_pObjectHandle     = pObjectHandle;
        m_pUMThunkMarshInfo = pUMThunkMarshInfo;
        m_dwDomainId        = dwDomainId;

        m_pMD = pMD;    // For debugging and profiling, so they can identify the target

        m_code.Encode((BYTE*)TheUMThunkPreStub()->GetEntryPoint(), this);

#ifdef _DEBUG
        m_state = kLoadTimeInited;
#endif
    }

    ~UMEntryThunk()
    {
        CONTRACTL
        {
            NOTHROW;
            GC_NOTRIGGER;
            MODE_COOPERATIVE;
        }
        CONTRACTL_END;

        if (GetObjectHandle())
        {
            DestroyLongWeakHandle(GetObjectHandle());
        }

#ifdef _DEBUG
        FillMemory(this, sizeof(*this), 0xcc);
#endif
    }

    void Terminate()
    {
        WRAPPER_CONTRACT;

        DeleteExecutable(this);
    }

    void OnADUnload();

    VOID RunTimeInit()
    {
        CONTRACTL
        {
            THROWS;
            GC_TRIGGERS;
            MODE_ANY;
        }
        CONTRACTL_END;

        // Ensure method's module is activate in app domain
        m_pMD->EnsureActive();

        m_pUMThunkMarshInfo->RunTimeInit();

        // Ensure that we have either the managed target or the delegate.
        if (m_pObjectHandle == NULL && m_pManagedTarget == NULL)
            m_pManagedTarget = m_pMD->GetMultiCallableAddrOfCode();

        m_code.Encode((BYTE*)m_pUMThunkMarshInfo->GetExecStub()->GetEntryPoint(), this);

#ifdef _DEBUG
        m_state = kRunTimeInited;
#endif
    }

    // asm entrypoint
    static VOID __stdcall DoRunTimeInit(UMEntryThunk* pThis);

    VOID CompleteInit(const BYTE             *pManagedTarget,
                      OBJECTHANDLE            pObjectHandle,
                      UMThunkMarshInfo       *pUMThunkMarshInfo,
                      MethodDesc             *pMD,
                      ADID                    dwAppDomainId)
    {
        CONTRACTL
        {
            THROWS;
            GC_TRIGGERS;
            MODE_ANY;
        }
        CONTRACTL_END;

        LoadTimeInit(pManagedTarget, pObjectHandle, pUMThunkMarshInfo, pMD, dwAppDomainId);
        RunTimeInit();
    }
#endif


    const BYTE* GetManagedTarget() const
    {
        CONTRACT (const BYTE*)
        {
            THROWS;
            GC_TRIGGERS;
            MODE_ANY;
            PRECONDITION(m_state == kRunTimeInited || m_state == kLoadTimeInited);
            POSTCONDITION(CheckPointer(RETVAL));
        }
        CONTRACT_END;

        if (m_pManagedTarget)
        {
            RETURN m_pManagedTarget;
        }
        else
        {
            RETURN m_pMD->GetMultiCallableAddrOfCode();
        }
    }

    OBJECTHANDLE GetObjectHandle() const
    {
        CONTRACT (OBJECTHANDLE)
        {
            NOTHROW;
            GC_NOTRIGGER;
            MODE_COOPERATIVE;
            // If we OOM after we create the holder but
            // before we set the m_state we can have
            // m_state == 0 and m_pObjectHandle == NULL
            PRECONDITION(m_state == kRunTimeInited  ||
                         m_state == kLoadTimeInited || 
                         m_pObjectHandle == NULL);
        }
        CONTRACT_END;

        RETURN m_pObjectHandle;
    }

    UMThunkMarshInfo* GetUMThunkMarshInfo() const
    {
        CONTRACT (UMThunkMarshInfo*)
        {
            NOTHROW;
            GC_NOTRIGGER;
            MODE_ANY;
            PRECONDITION(m_state == kRunTimeInited || m_state == kLoadTimeInited);
            POSTCONDITION(CheckPointer(RETVAL));
        }
        CONTRACT_END;
    
        RETURN m_pUMThunkMarshInfo;
    }
    

    const BYTE* GetCode() const
    {
        CONTRACT (const BYTE*)
        {
            NOTHROW;
            GC_NOTRIGGER;
            MODE_ANY;
            PRECONDITION(m_state == kRunTimeInited || m_state == kLoadTimeInited);
            POSTCONDITION(CheckPointer(RETVAL, NULL_OK));
        }
        CONTRACT_END;

        RETURN m_code.GetEntryPoint();
    }

    static UMEntryThunk* RecoverUMEntryThunk(const VOID* pCode)
    {
        LEAF_CONTRACT;
        return (UMEntryThunk*)( ((LPBYTE)pCode) - offsetof(UMEntryThunk, m_code) );
    }


    MethodDesc* GetMethod() const
    {
        CONTRACT (MethodDesc*)
        {
            NOTHROW;
            GC_NOTRIGGER;
            MODE_ANY;
            PRECONDITION(m_state == kRunTimeInited || m_state == kLoadTimeInited);
            POSTCONDITION(CheckPointer(RETVAL,NULL_OK));
        }
        CONTRACT_END;

        RETURN m_pMD;
    }

    ADID GetDomainId() const
    {
        CONTRACT (ADID)
        {
            NOTHROW;
            GC_NOTRIGGER;
            MODE_ANY;
            PRECONDITION(m_state == kRunTimeInited || m_state == kLoadTimeInited);
        }
        CONTRACT_END;

        RETURN m_dwDomainId;
    }

    static DWORD GetOffsetOfMethodDesc()
    {
        LEAF_CONTRACT;
        return offsetof(class UMEntryThunk, m_pMD);
    }

    static DWORD GetCodeOffset()
    {
        LEAF_CONTRACT;
        return offsetof(UMEntryThunk, m_code);
    }

    static UMEntryThunk* Decode(LPVOID pCallback);

    BOOL DeadTarget() const
    {
        LEAF_CONTRACT;
        return (m_pManagedTarget == NULL);
    }


private:
    // The start of the managed code
    const BYTE*             m_pManagedTarget;

    // This is used for profiling.
    PTR_MethodDesc          m_pMD;

    // Object handle holding "this" reference. May be a strong or weak handle.
    // Field is NULL for a static method.
    OBJECTHANDLE            m_pObjectHandle;

    // Pointer to the shared structure containing everything else
    PTR_UMThunkMarshInfo    m_pUMThunkMarshInfo;

    ADID                    m_dwDomainId;   // appdomain of module (cached for fast access)
#ifdef _DEBUG
    DWORD                   m_state;        // the initialization state
#endif


    UMEntryThunkCode        m_code;
};


//--------------------------------------------------------------------------
// Onetime Init
//--------------------------------------------------------------------------
void UMThunkInit();

//-------------------------------------------------------------------------
// One-time creation of special prestub to initialize UMEntryThunks.
//-------------------------------------------------------------------------
Stub *GenerateUMThunkPrestub();

//-------------------------------------------------------------------------
// NExport stub
//-------------------------------------------------------------------------
Stub *CreateGenericNExportStub(UMThunkMLStub* pHeader);

EXCEPTION_HANDLER_DECL(FastNExportExceptHandler);
EXCEPTION_HANDLER_DECL(UMThunkPrestubHandler);

PlatformDefaultReturnType __stdcall DoUMThunkCall(Thread *pThread, UMThkCallFrame *pFrame);

extern "C" void TheUMEntryPrestub(void);

#ifdef _DEBUG
void __stdcall LogUMTransition(UMEntryThunk* thunk);
#endif

#endif //__dllimportcallback_h__


