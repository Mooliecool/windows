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

#include "stubgen.h"
#include "binder.h"

class ILStubMarshalHome
{
public:
    typedef enum 
    {
        HomeType_Unspecified = 0,
        HomeType_ILLocal = 1,
        HomeType_ILArgument = 2,
    } MarshalHomeType;

private:
    MarshalHomeType     m_homeType;
    DWORD               m_dwHomeIndex;
    
public:
    void InitHome(MarshalHomeType homeType, DWORD dwHomeIndex)
    {
        LEAF_CONTRACT;
        m_homeType = homeType;
        m_dwHomeIndex = dwHomeIndex;
    }
        
    void EmitLoadHome(ILCodeStream* pslILEmit)
    {
        CONTRACTL
        {
            THROWS;
            GC_TRIGGERS;
            MODE_ANY;
        }
        CONTRACTL_END;

        switch (m_homeType)
        {
            case HomeType_ILLocal:      pslILEmit->EmitLDLOC(m_dwHomeIndex); break;
            case HomeType_ILArgument:   pslILEmit->EmitLDARG(m_dwHomeIndex); break;
        
            default:
                UNREACHABLE_MSG("unexpected homeType passed to EmitLoadHome");
                break;
        }
    }

    void EmitLoadHomeAddr(ILCodeStream* pslILEmit)
    {
        CONTRACTL
        {
            THROWS;
            GC_TRIGGERS;
            MODE_ANY;
        }
        CONTRACTL_END;
        
        switch (m_homeType)
        {
            case HomeType_ILLocal:      pslILEmit->EmitLDLOCA(m_dwHomeIndex); break;
            case HomeType_ILArgument:   pslILEmit->EmitLDARGA(m_dwHomeIndex); break;

            default:
                UNREACHABLE_MSG("unexpected homeType passed to EmitLoadHomeAddr");
                break;
    }
    }        

    void EmitStoreHome(ILCodeStream* pslILEmit)
    {
        CONTRACTL
        {
            THROWS;
            GC_TRIGGERS;
            MODE_ANY;
        }
        CONTRACTL_END;
        
        switch (m_homeType)
        {
            case HomeType_ILLocal:      pslILEmit->EmitSTLOC(m_dwHomeIndex); break;
            case HomeType_ILArgument:   pslILEmit->EmitSTARG(m_dwHomeIndex); break;

            default:
                UNREACHABLE_MSG("unexpected homeType passed to EmitStoreHome");
                break;
        }
    }

    void EmitCopyFromByrefArg(ILCodeStream* pslILEmit, LocalDesc* pManagedType, DWORD argidx)
    {
        CONTRACTL
        {
            THROWS;
            GC_TRIGGERS;
            MODE_ANY;
        }
        CONTRACTL_END;
        
        CONSISTENCY_CHECK(pManagedType->cbType == 1);
        if (pManagedType->IsValueClass())
        {
            EmitLoadHomeAddr(pslILEmit);    // dest
            pslILEmit->EmitLDARG(argidx);   // src
            pslILEmit->EmitCPOBJ(pslILEmit->GetToken(pManagedType->InternalToken));
        }
        else
        {
            pslILEmit->EmitLDARG(argidx);
            pslILEmit->EmitLDIND_T(pManagedType);
            EmitStoreHome(pslILEmit);
        }
    }

    void EmitCopyToByrefArg(ILCodeStream* pslILEmit, LocalDesc* pManagedType, DWORD argidx)
    {
        CONTRACTL
        {
            THROWS;
            GC_TRIGGERS;
            MODE_ANY;
        }
        CONTRACTL_END;

        if (pManagedType->IsValueClass())
        {
            pslILEmit->EmitLDARG(argidx);   // dest
            EmitLoadHomeAddr(pslILEmit);    // src
            pslILEmit->EmitCPOBJ(pslILEmit->GetToken(pManagedType->InternalToken));
        }
        else
        {
            pslILEmit->EmitLDARG(argidx);
            EmitLoadHome(pslILEmit);
            pslILEmit->EmitSTIND_T(pManagedType);
        }
    }

};
        

class ILMarshaler 
{
protected:

#ifdef _DEBUG
    const static UINT   s_cbStackAllocThreshold = 128;
#else
    const static UINT   s_cbStackAllocThreshold = 2048;
#endif // _DEBUG

    OverrideProcArgs*   m_pargs;
    NDirectStubLinker*  m_pslNDirect;
    ILCodeStream*       m_pcsMarshal;
    ILCodeStream*       m_pcsUnmarshal;
    UINT                m_argidx;

    DWORD               m_dwMarshalFlags;

    ILStubMarshalHome   m_nativeHome;
    ILStubMarshalHome   m_managedHome;

    DWORD               m_dwMngdMarshalerLocalNum;

public:

    ILMarshaler() : 
        m_pslNDirect(NULL)
    {
    }

    void SetNDirectStubLinker(NDirectStubLinker* pslNDirect)
    {
        CONSISTENCY_CHECK(NULL == m_pslNDirect);
        m_pslNDirect = pslNDirect;
    }

    void Init(ILCodeStream* pcsMarshal, 
            ILCodeStream* pcsUnmarshal,
            UINT argidx,
            DWORD dwMarshalFlags, 
            OverrideProcArgs* pargs)
    {
        CONSISTENCY_CHECK_MSG(m_pslNDirect != NULL, "please call SetNDirectStubLinker() before EmitMarshalArgument or EmitMarshalReturnValue");
        m_pcsMarshal = pcsMarshal;
        m_pcsUnmarshal = pcsUnmarshal;
        m_pargs = pargs;
        m_dwMarshalFlags = dwMarshalFlags;
        m_argidx = argidx;
        m_dwMngdMarshalerLocalNum = -1;
    }

protected:
    static inline bool IsCLRToNative(DWORD dwMarshalFlags)
    {
        LEAF_CONTRACT;
        return (0 != (dwMarshalFlags & MARSHAL_FLAG_CLR_TO_NATIVE));
    }
        
    static inline bool IsIn(DWORD dwMarshalFlags)
    {
        LEAF_CONTRACT;
        return (0 != (dwMarshalFlags & MARSHAL_FLAG_IN));
    }

    static inline bool IsOut(DWORD dwMarshalFlags)
    {
        LEAF_CONTRACT;
        return (0 != (dwMarshalFlags & MARSHAL_FLAG_OUT));
    }

    static inline bool IsByref(DWORD dwMarshalFlags)
    {
        LEAF_CONTRACT;
        return (0 != (dwMarshalFlags & MARSHAL_FLAG_BYREF));
    }

    static inline bool IsHresultSwap(DWORD dwMarshalFlags)
    {
        LEAF_CONTRACT;
        return (0 != (dwMarshalFlags & MARSHAL_FLAG_HRESULT_SWAP));
    }

    void EmitLoadManagedValue(ILCodeStream* pslILEmit)
    {
        WRAPPER_CONTRACT;
        m_managedHome.EmitLoadHome(pslILEmit);
    }

    void EmitLoadNativeValue(ILCodeStream* pslILEmit)
    {
        WRAPPER_CONTRACT;
        m_nativeHome.EmitLoadHome(pslILEmit);
    }

    void EmitLoadManagedHomeAddr(ILCodeStream* pslILEmit)
    {
        WRAPPER_CONTRACT;
        m_managedHome.EmitLoadHomeAddr(pslILEmit);
    }

    void EmitLoadNativeHomeAddr(ILCodeStream* pslILEmit)
    {
        WRAPPER_CONTRACT;
        m_nativeHome.EmitLoadHomeAddr(pslILEmit);
    }
        
    void EmitStoreManagedValue(ILCodeStream* pslILEmit)
    {
        WRAPPER_CONTRACT;
        m_managedHome.EmitStoreHome(pslILEmit);
    }

    void EmitStoreNativeValue(ILCodeStream* pslILEmit)
    {
        WRAPPER_CONTRACT;
        m_nativeHome.EmitStoreHome(pslILEmit);
    }

public:

    virtual bool SupportsArgumentMarshal(DWORD dwMarshalFlags, UINT* pErrorResID)
    {
        LEAF_CONTRACT;
        return true;
    }

    virtual bool SupportsReturnMarshal(DWORD dwMarshalFlags, UINT* pErrorResID)
    {
        LEAF_CONTRACT;
        return true;
    }

    void EmitMarshalArgument(
                ILCodeStream*   pcsMarshal, 
                ILCodeStream*   pcsUnmarshal, 
                UINT            argidx, 
                DWORD           dwMarshalFlags,
                OverrideProcArgs*  pargs)
    {
        CONTRACTL
        {
            THROWS;
            GC_TRIGGERS;
            MODE_ANY;
        }
        CONTRACTL_END;

        Init(pcsMarshal, pcsUnmarshal, argidx, dwMarshalFlags, pargs);

        EmitCreateMngdMarshaler(m_pcsMarshal);

        if (IsCLRToNative(dwMarshalFlags))
        {
            if (IsByref(dwMarshalFlags))
            {
                EmitMarshalArgumentCLRToNativeByref();
            }
            else
            {
                EmitMarshalArgumentCLRToNative();
            }
        }
        else
        {
            if (IsByref(dwMarshalFlags))
            {
                EmitMarshalArgumentNativeToCLRByref();
            }
            else
            {
                EmitMarshalArgumentNativeToCLR();
            }
        }
    }

    virtual void EmitSetupArgument(ILCodeStream* pslILEmit)
    {
        CONTRACTL
        {
            THROWS;
            GC_TRIGGERS;
            MODE_ANY;
        }
        CONTRACTL_END;

        if (IsCLRToNative(m_dwMarshalFlags))
        {
            if (IsNativePassedByRef())
            {
                EmitLoadNativeHomeAddr(pslILEmit);
            }
            else
            {
                EmitLoadNativeValue(pslILEmit);
            }
        }
        else
        {
            if (IsManagedPassedByRef())
            {
                EmitLoadManagedHomeAddr(pslILEmit);
            }
            else
            {
                EmitLoadManagedValue(pslILEmit);
            }
        }
    }

    virtual void EmitMarshalReturnValue(
                ILCodeStream* pcsMarshal, 
                ILCodeStream* pcsUnmarshal,
                ILCodeStream* pcsDispatch,
                UINT argidx,
                DWORD dwMarshalFlags,
                OverrideProcArgs*  pargs)
    {
        CONTRACTL
        {
            THROWS;
            GC_TRIGGERS;
            MODE_ANY;
        }
        CONTRACTL_END;

        Init(pcsMarshal, pcsUnmarshal, argidx, dwMarshalFlags, pargs);

        LocalDesc nativeType = GetNativeType();
        LocalDesc managedType = GetManagedType();
        
        if (IsHresultSwap(dwMarshalFlags))
        {
            LocalDesc extraParamType = nativeType;
            extraParamType.MakeByRef();

            m_pcsMarshal->SetStubTargetArgType(&extraParamType, false);
            m_pcsMarshal->SetStubTargetReturnType(ELEMENT_TYPE_I4);    // native method returns an HRESULT
        }
        else
        {
            m_pcsMarshal->SetStubTargetReturnType(&nativeType);
        }
       
        m_managedHome.InitHome(ILStubMarshalHome::HomeType_ILLocal, m_pcsMarshal->NewLocal(managedType));
        m_nativeHome.InitHome(ILStubMarshalHome::HomeType_ILLocal, m_pcsMarshal->NewLocal(nativeType));

        EmitCreateMngdMarshaler(m_pcsMarshal);
       
        if (IsCLRToNative(dwMarshalFlags))
        {
            if (IsHresultSwap(dwMarshalFlags))
            {
                EmitReInitNative(m_pcsMarshal);
                EmitLoadNativeHomeAddr(pcsDispatch);    // load up the byref native type as an extra arg
            }
            else
            {
                EmitStoreNativeValue(m_pcsUnmarshal);
            }

            m_pslNDirect->EmitSetArgMarshalIndex(m_pcsUnmarshal, m_argidx);

            EmitConvertSpaceAndContentsNativeToCLR(m_pcsUnmarshal);

            EmitCancelCleanup(m_pcsUnmarshal);
            EmitCleanupCLRToNative();

            EmitLoadManagedValue(m_pcsUnmarshal);
        }
        else
        {
            EmitStoreManagedValue(m_pcsUnmarshal);

            m_pslNDirect->EmitSetArgMarshalIndex(m_pcsUnmarshal, m_argidx);
            
            EmitConvertSpaceAndContentsCLRToNative(m_pcsUnmarshal);
            EmitCleanupNativeToCLR();

            if (IsHresultSwap(dwMarshalFlags))
            {
                m_nativeHome.EmitCopyToByrefArg(m_pcsUnmarshal, &managedType, argidx);
                m_pcsUnmarshal->EmitLDC(S_OK);
            }
            else
            {
                EmitLoadNativeValue(m_pcsUnmarshal);
            }
        }
    }        


protected:

    virtual void EmitCreateMngdMarshaler(ILCodeStream* pslILEmit)
    {
        LEAF_CONTRACT;
    }

    void EmitLoadMngdMarshaler(ILCodeStream* pslILEmit)
    {
        WRAPPER_CONTRACT;
        CONSISTENCY_CHECK(-1 != m_dwMngdMarshalerLocalNum);
        pslILEmit->EmitLDLOC(m_dwMngdMarshalerLocalNum);
    }

    void EmitSetupSigAndDefaultHomesCLRToNative()
    {
        CONTRACTL
        {
            THROWS;
            GC_TRIGGERS;
            MODE_ANY;
            PRECONDITION(IsCLRToNative(m_dwMarshalFlags) && !IsByref(m_dwMarshalFlags));
        }
        CONTRACTL_END;

        LocalDesc nativeArgType = GetNativeType();
        DWORD     dwNativeHomeLocalNum = m_pcsMarshal->NewLocal(nativeArgType);
        m_pcsMarshal->SetStubTargetArgType(&nativeArgType);

        m_managedHome.InitHome(ILStubMarshalHome::HomeType_ILArgument, m_argidx);
        m_nativeHome.InitHome(ILStubMarshalHome::HomeType_ILLocal, dwNativeHomeLocalNum);
    }

    void EmitCleanupCLRToNativeTemp()
    {
        if (NeedsClearNative())
        {
            ILCodeStream* pcsCleanup = m_pslNDirect->GetCleanupCodeStream();
            ILCodeLabel*  pSkipClearNativeLabel = pcsCleanup->NewCodeLabel();
            m_pslNDirect->EmitCheckForArgCleanup(pcsCleanup, m_argidx, pSkipClearNativeLabel);
            EmitClearNativeTemp(pcsCleanup);
            pcsCleanup->EmitLabel(pSkipClearNativeLabel);
        }
    }

    void EmitCleanupCLRToNative()
    {
        if (NeedsClearNative())
        {
            ILCodeStream* pcsCleanup = m_pslNDirect->GetCleanupCodeStream();
            ILCodeLabel*  pSkipClearNativeLabel = pcsCleanup->NewCodeLabel();
            m_pslNDirect->EmitCheckForArgCleanup(pcsCleanup, m_argidx, pSkipClearNativeLabel);
            EmitClearNative(pcsCleanup);
            pcsCleanup->EmitLabel(pSkipClearNativeLabel);
        }
    }

    virtual void EmitMarshalArgumentCLRToNative()
    {
        CONTRACTL
        {
            THROWS;
            GC_TRIGGERS;
            MODE_ANY;
            PRECONDITION(IsCLRToNative(m_dwMarshalFlags) && !IsByref(m_dwMarshalFlags));
        }
        CONTRACTL_END;

        EmitSetupSigAndDefaultHomesCLRToNative();

        //
        // marshal
        //
        if (IsIn(m_dwMarshalFlags))
        {
            EmitConvertSpaceAndContentsCLRToNativeTemp(m_pcsMarshal);
        }
        else
        {
            EmitConvertSpaceCLRToNativeTemp(m_pcsMarshal);
        }

        //
        // unmarshal
        //
        if (IsOut(m_dwMarshalFlags))
        {
            if (IsIn(m_dwMarshalFlags))
            {
                EmitClearCLRContents(m_pcsUnmarshal);
            }
            EmitConvertContentsNativeToCLR(m_pcsUnmarshal);
        }

        EmitCancelCleanup(m_pcsUnmarshal);
        EmitCleanupCLRToNativeTemp();
   }

    void EmitSetupSigAndDefaultHomesCLRToNativeByref()
    {
        CONTRACTL
        {
            THROWS;
            GC_TRIGGERS;
            MODE_ANY;
            PRECONDITION(IsCLRToNative(m_dwMarshalFlags) && IsByref(m_dwMarshalFlags));
        }
        CONTRACTL_END;

        LocalDesc nativeType = GetNativeType();
        LocalDesc managedType = GetManagedType();
        LocalDesc nativeArgType = nativeType;
        nativeArgType.MakeByRef();
        m_pcsMarshal->SetStubTargetArgType(&nativeArgType);

        m_managedHome.InitHome(ILStubMarshalHome::HomeType_ILLocal, m_pcsMarshal->NewLocal(managedType));
        m_nativeHome.InitHome(ILStubMarshalHome::HomeType_ILLocal, m_pcsMarshal->NewLocal(nativeType));
    }

    virtual void EmitMarshalArgumentCLRToNativeByref()
    {
        CONTRACTL
        {
            THROWS;
            GC_TRIGGERS;
            MODE_ANY;
            PRECONDITION(IsCLRToNative(m_dwMarshalFlags) && IsByref(m_dwMarshalFlags));
        }
        CONTRACTL_END;

        LocalDesc managedType = GetManagedType();

        EmitSetupSigAndDefaultHomesCLRToNativeByref();
            
        //
        // marshal
        //
        if (IsIn(m_dwMarshalFlags))
        {
            m_managedHome.EmitCopyFromByrefArg(m_pcsMarshal, &managedType, m_argidx);

            EmitConvertSpaceAndContentsCLRToNativeTemp(m_pcsMarshal);
        }
        else
        {
            EmitReInitNative(m_pcsMarshal);
        }

        //
        // unmarshal
        //
        if (IsOut(m_dwMarshalFlags))
        {
            EmitClearCLR(m_pcsUnmarshal);

            EmitConvertSpaceAndContentsNativeToCLR(m_pcsUnmarshal);

            m_managedHome.EmitCopyToByrefArg(m_pcsUnmarshal, &managedType, m_argidx);
        }

        EmitCancelCleanup(m_pcsUnmarshal);
        EmitCleanupCLRToNativeTemp();
    }

    void EmitSetupSigAndDefaultHomesNativeToCLR()
    {
        CONTRACTL
        {
            THROWS;
            GC_TRIGGERS;
            MODE_ANY;
            PRECONDITION(!IsCLRToNative(m_dwMarshalFlags) && !IsByref(m_dwMarshalFlags));
        }
        CONTRACTL_END;
        
        LocalDesc nativeArgType = GetNativeType();
        m_pcsMarshal->SetStubTargetArgType(&nativeArgType);

        m_managedHome.InitHome(ILStubMarshalHome::HomeType_ILLocal, m_pcsMarshal->NewLocal(GetManagedType()));
        m_nativeHome.InitHome(ILStubMarshalHome::HomeType_ILArgument, m_argidx);
    }

    void EmitCleanupNativeToCLR()
    {
        if (NeedsClearCLR())
        {
            ILCodeStream* pcsCleanup = m_pslNDirect->GetCleanupCodeStream();
            ILCodeLabel*  pSkipClearCLRLabel = pcsCleanup->NewCodeLabel();
            m_pslNDirect->EmitCheckForArgCleanup(pcsCleanup, m_argidx, pSkipClearCLRLabel);
            EmitClearCLR(pcsCleanup);
            pcsCleanup->EmitLabel(pSkipClearCLRLabel);
        }
    }

    virtual void EmitMarshalArgumentNativeToCLR()
    {
        CONTRACTL
        {
            THROWS;
            GC_TRIGGERS;
            MODE_ANY;
            PRECONDITION(!IsCLRToNative(m_dwMarshalFlags) && !IsByref(m_dwMarshalFlags));
        }
        CONTRACTL_END;
                    
        EmitSetupSigAndDefaultHomesNativeToCLR();

        //
        // marshal
        //
        if (IsIn(m_dwMarshalFlags))
        {
            EmitConvertSpaceAndContentsNativeToCLR(m_pcsMarshal);
        }
        else
        {
            EmitConvertSpaceNativeToCLR(m_pcsMarshal);
        }

        //
        // unmarshal
        //
        if (IsOut(m_dwMarshalFlags))
        {
            if (IsIn(m_dwMarshalFlags))
            {
                EmitCancelCleanup(m_pcsUnmarshal);
                EmitClearNativeContents(m_pcsUnmarshal);
            }
            EmitConvertContentsCLRToNative(m_pcsUnmarshal);
        }
        EmitCleanupNativeToCLR();
    }

    void EmitSetupSigAndDefaultHomesNativeToCLRByref()
    {
        CONTRACTL
        {
            THROWS;
            GC_TRIGGERS;
            MODE_ANY;
            PRECONDITION(!IsCLRToNative(m_dwMarshalFlags) && IsByref(m_dwMarshalFlags));
        }
        CONTRACTL_END;
    
        LocalDesc nativeType = GetNativeType();
        LocalDesc managedType = GetManagedType();
        LocalDesc nativeArgType = nativeType;
        nativeArgType.MakeByRef();
        m_pcsMarshal->SetStubTargetArgType(&nativeArgType);

        m_managedHome.InitHome(ILStubMarshalHome::HomeType_ILLocal, m_pcsMarshal->NewLocal(managedType));
        m_nativeHome.InitHome(ILStubMarshalHome::HomeType_ILLocal, m_pcsMarshal->NewLocal(nativeType));
    }

    virtual void EmitMarshalArgumentNativeToCLRByref()
    {
        CONTRACTL
        {
            THROWS;
            GC_TRIGGERS;
            MODE_ANY;
            PRECONDITION(!IsCLRToNative(m_dwMarshalFlags) && IsByref(m_dwMarshalFlags));
        }
        CONTRACTL_END;
        
        LocalDesc nativeType = GetNativeType();

        EmitSetupSigAndDefaultHomesNativeToCLRByref();
    
        //
        // marshal
        //
        if (IsIn(m_dwMarshalFlags))
        {
            m_nativeHome.EmitCopyFromByrefArg(m_pcsMarshal, &nativeType, m_argidx);

            EmitConvertSpaceAndContentsNativeToCLR(m_pcsMarshal);
        }
        
        //
        // unmarshal
        //
        if (IsOut(m_dwMarshalFlags))
        {
            if (IsIn(m_dwMarshalFlags))
            {
                EmitCancelCleanup(m_pcsUnmarshal);
                EmitClearNative(m_pcsUnmarshal);
                EmitReInitNative(m_pcsUnmarshal);
            }

            EmitConvertSpaceAndContentsCLRToNative(m_pcsUnmarshal);

            m_nativeHome.EmitCopyToByrefArg(m_pcsUnmarshal, &nativeType, m_argidx);
        }

        EmitCleanupNativeToCLR();
    }

    virtual LocalDesc GetNativeType() = 0;
    virtual LocalDesc GetManagedType() = 0;

    //
    // Native-to-CLR
    //
    virtual void EmitConvertSpaceNativeToCLR(ILCodeStream* pslILEmit)
    {
        LEAF_CONTRACT;
    }
        
    virtual void EmitConvertContentsNativeToCLR(ILCodeStream* pslILEmit)
    {
        LEAF_CONTRACT;
    }

    virtual void EmitConvertSpaceAndContentsNativeToCLR(ILCodeStream* pslILEmit)
    {
        WRAPPER_CONTRACT;
        EmitConvertSpaceNativeToCLR(pslILEmit);
        EmitConvertContentsNativeToCLR(pslILEmit);
    }


    //
    // CLR-to-Native
    //
    virtual void EmitConvertSpaceCLRToNative(ILCodeStream* pslILEmit)
    {
        LEAF_CONTRACT;
    }

    virtual void EmitConvertSpaceCLRToNativeTemp(ILCodeStream* pslILEmit)
    {
        LEAF_CONTRACT;
        EmitConvertSpaceCLRToNative(pslILEmit);
    }

    virtual void EmitConvertContentsCLRToNative(ILCodeStream* pslILEmit)
    {
        LEAF_CONTRACT;
    }
    
    virtual void EmitConvertSpaceAndContentsCLRToNative(ILCodeStream* pslILEmit)
    {
        WRAPPER_CONTRACT;
        EmitConvertSpaceCLRToNative(pslILEmit);
        EmitConvertContentsCLRToNative(pslILEmit);
    }
        
    virtual void EmitConvertSpaceAndContentsCLRToNativeTemp(ILCodeStream* pslILEmit)
    {
        WRAPPER_CONTRACT;
        EmitConvertSpaceAndContentsCLRToNative(pslILEmit);
    }

    //
    // Misc
    //
    virtual void EmitClearCLRContents(ILCodeStream* pslILEmit)
    {
        LEAF_CONTRACT;
    }

    virtual void EmitCancelCleanup(ILCodeStream* pslILEmit)
    {
        LEAF_CONTRACT;
    }

    virtual bool NeedsClearNative()
    {
        LEAF_CONTRACT;
        return false;
    }

    virtual void EmitClearNative(ILCodeStream* pslILEmit)
    {
        LEAF_CONTRACT;
    }

    virtual void EmitClearNativeTemp(ILCodeStream* pslILEmit)
    {
        LEAF_CONTRACT;
        EmitClearNative(pslILEmit);
    }

    virtual void EmitClearNativeContents(ILCodeStream* pslILEmit)
    {
        LEAF_CONTRACT;
    }

    virtual bool NeedsClearCLR()
    {
        LEAF_CONTRACT;
        return false;
    }

    virtual void EmitClearCLR(ILCodeStream* pslILEmit)
    {
        LEAF_CONTRACT;
    }

    virtual void EmitReInitNative(ILCodeStream* pslILEmit)
    {
        LEAF_CONTRACT;
        pslILEmit->EmitLDC(0);
        EmitStoreNativeValue(pslILEmit);
    }

    virtual bool IsManagedPassedByRef()
    {
        LEAF_CONTRACT;
        return IsByref(m_dwMarshalFlags);
    }

    virtual bool IsNativePassedByRef()
    {
        LEAF_CONTRACT;
        return IsByref(m_dwMarshalFlags);
    }

public:
    static MarshalerOverrideStatus ArgumentOverride(InteropStubLinker* psl,       // don't use this, use pslIL from macro
                                                    InteropStubLinker* pslPost,   // don't use this, use pslPostIL from macro
                                                    BOOL               byref,
                                                    BOOL               fin,
                                                    BOOL               fout,
                                                    BOOL               fManagedToNative,
                                                    OverrideProcArgs*  pargs,
                                                    UINT*              pResID,
                                                    UINT               argidx)
    {
        LEAF_CONTRACT;
        return HANDLEASNORMAL;
    }

    static MarshalerOverrideStatus ReturnOverride(InteropStubLinker*  psl,
                                                  InteropStubLinker*  pslPost,
                                                  BOOL                fManagedToNative,
                                                  BOOL                fThruBuffer,
                                                  OverrideProcArgs*   pargs,
                                                  UINT*               pResID)
    {
        LEAF_CONTRACT;
        return HANDLEASNORMAL;
    }
};

        
class ILCopyMarshalerBase : public ILMarshaler
{
    virtual LocalDesc GetManagedType()
    {
        WRAPPER_CONTRACT;
        return GetNativeType();
    }

    virtual void EmitConvertContentsCLRToNative(ILCodeStream* pslILEmit)
    {
        EmitLoadManagedValue(pslILEmit);
        EmitStoreNativeValue(pslILEmit);
    }

    virtual void EmitConvertContentsNativeToCLR(ILCodeStream* pslILEmit)
    {
        EmitLoadNativeValue(pslILEmit);
        EmitStoreManagedValue(pslILEmit);
    }

    //
    // It's very unforunate that x86 uses ML_COPYPINNEDGCREF for byref args.
    // The result is that developers can get away with being lazy about their 
    // in/out semantics and often times in/out byref params are marked out-
    // only, but because of ML_COPYPINNEDGCREF, they get in/out behavior.  
    //
    // The unforunate part is that we need special code to maintain this 
    // inadvertent 'feature'.  
    //
    virtual void EmitMarshalArgumentCLRToNativeByref()
    {
        CONTRACTL
        {
            THROWS;
            GC_TRIGGERS;
            MODE_ANY;
            PRECONDITION(IsCLRToNative(m_dwMarshalFlags) && IsByref(m_dwMarshalFlags));
        }
        CONTRACTL_END;


        LocalDesc managedType = GetManagedType();

        EmitSetupSigAndDefaultHomesCLRToNativeByref();
            
        //
        // marshal
        //
        m_managedHome.EmitCopyFromByrefArg(m_pcsMarshal, &managedType, m_argidx);
        EmitConvertSpaceAndContentsCLRToNativeTemp(m_pcsMarshal);

        //
        // unmarshal
        //
        EmitConvertSpaceAndContentsNativeToCLR(m_pcsUnmarshal);
        m_managedHome.EmitCopyToByrefArg(m_pcsUnmarshal, &managedType, m_argidx);
    }
};

template <CorElementType ELEMENT_TYPE>
class ILCopyMarshalerSimple : public ILCopyMarshalerBase
{
public:
    enum
    {
        c_fNeedsClearNative     = FALSE,
        c_fInOnly               = TRUE,
    };

    bool IsSmallValueTypeSpecialCase()
    {
        //
        // Special case for small value types that get
        // mapped to MARSHAL_TYPE_GENERIC_8 -- use the 
        // valuetype type so the JIT is happy.
        //
        
        return (ELEMENT_TYPE == 
                    ELEMENT_TYPE_I4
                    ) && (NULL != m_pargs->m_pMT);
    }
    
    bool NeedToPromoteTo8Bytes()
    {
        WRAPPER_CONTRACT;


        return false;
    }

    CorElementType GetConversionType(CorElementType type)
    {
        LEAF_CONTRACT;

        // I4 <-> I8; U4 <-> U8
        if (type == ELEMENT_TYPE_I4)
        {
            return ELEMENT_TYPE_I8;
        }
        else if (type == ELEMENT_TYPE_U4)
        {
            return ELEMENT_TYPE_U8;
        }
        else
        {
            return ELEMENT_TYPE_END;
        }
    }

    void EmitTypePromotion(ILCodeStream* pslILEmit)
    {
        WRAPPER_CONTRACT;

        CorElementType promotedType = GetConversionType(ELEMENT_TYPE);
        if (promotedType == ELEMENT_TYPE_I8)
        {
            pslILEmit->EmitCONV_I8();
        }
        else if (promotedType == ELEMENT_TYPE_U8)
        {
            pslILEmit->EmitCONV_U8();
        }
    }

    virtual LocalDesc GetNativeType()
    {
        WRAPPER_CONTRACT;
        
        LocalDesc nativeType;

        if (NeedToPromoteTo8Bytes())
        {
            nativeType.cbType = 1;
            nativeType.ElementType[0] = GetConversionType(ELEMENT_TYPE);
        }
        else
        {
            nativeType = GetManagedType();
        }

        return nativeType;
    }

    virtual LocalDesc GetManagedType()
    {
        WRAPPER_CONTRACT;

        LocalDesc managedType;

        if (IsSmallValueTypeSpecialCase())
        {
            managedType.cbType = 1;
            managedType.ElementType[0] = ELEMENT_TYPE_INTERNAL;
            managedType.InternalToken = TypeHandle(m_pargs->m_pMT);
        }
        else
        {
            managedType.cbType = 1;
            managedType.ElementType[0] = ELEMENT_TYPE;
        }

        return managedType;
    }

    virtual void EmitConvertContentsCLRToNative(ILCodeStream* pslILEmit)
    {
        EmitLoadManagedValue(pslILEmit);
        if (NeedToPromoteTo8Bytes())
        {
            EmitTypePromotion(pslILEmit);
        }
        EmitStoreNativeValue(pslILEmit);
    }

    virtual void EmitReInitNative(ILCodeStream* pslILEmit)
    {
        CONTRACTL
        {
            MODE_ANY;
            GC_TRIGGERS;
            THROWS;
        }
        CONTRACTL_END;

        if (IsSmallValueTypeSpecialCase())
        {
            EmitLoadNativeHomeAddr(pslILEmit);
            pslILEmit->EmitINITOBJ(pslILEmit->GetToken(m_pargs->m_pMT));
        }
        else
        {
            ILCopyMarshalerBase::EmitReInitNative(pslILEmit);
        }
    }
};

typedef ILCopyMarshalerSimple<ELEMENT_TYPE_I1> ILCopyMarshaler1;
typedef ILCopyMarshalerSimple<ELEMENT_TYPE_U1> ILCopyMarshalerU1;
typedef ILCopyMarshalerSimple<ELEMENT_TYPE_I2> ILCopyMarshaler2;
typedef ILCopyMarshalerSimple<ELEMENT_TYPE_U2> ILCopyMarshalerU2;
typedef ILCopyMarshalerSimple<ELEMENT_TYPE_I4> ILCopyMarshaler4;
typedef ILCopyMarshalerSimple<ELEMENT_TYPE_U4> ILCopyMarshalerU4;
typedef ILCopyMarshalerSimple<ELEMENT_TYPE_I8> ILCopyMarshaler8;
typedef ILCopyMarshalerSimple<ELEMENT_TYPE_R4> ILFloatMarshaler;
typedef ILCopyMarshalerSimple<ELEMENT_TYPE_R8> ILDoubleMarshaler;

template <BinderClassID CLASS__ID>
class ILCopyMarshalerKnownStruct : public ILCopyMarshalerBase
{
public:
    enum
    {
        c_fNeedsClearNative     = FALSE,
        c_fInOnly               = TRUE,
    };

    virtual void EmitReInitNative(ILCodeStream* pslILEmit)
    {
        CONTRACTL
        {
            MODE_ANY;
            GC_TRIGGERS;
            THROWS;
        }
        CONTRACTL_END;
        
        EmitLoadNativeHomeAddr(pslILEmit);
        pslILEmit->EmitINITOBJ(pslILEmit->GetToken(g_Mscorlib.GetClass(CLASS__ID)));
    }

    virtual LocalDesc GetNativeType()
    {
        LEAF_CONTRACT;

        LocalDesc nativeType;

        nativeType.cbType = 1;
        nativeType.ElementType[0] = ELEMENT_TYPE_INTERNAL;
        nativeType.InternalToken = TypeHandle(g_Mscorlib.GetClass(CLASS__ID));

        return nativeType;
    }
};

typedef ILCopyMarshalerKnownStruct<CLASS__NATIVE_DECIMAL> ILDecimalMarshaler;
typedef ILCopyMarshalerKnownStruct<CLASS__GUID> ILGuidMarshaler;

class ILBlittableValueClassMarshaler : public ILCopyMarshalerBase
{
public:
    enum
    {
        c_fNeedsClearNative     = FALSE,
        c_fInOnly               = TRUE,
    };
        
    virtual void EmitReInitNative(ILCodeStream* pslILEmit)
    {
        CONTRACTL
        {
            MODE_ANY;
            GC_TRIGGERS;
            THROWS;
        }
        CONTRACTL_END;
        
        EmitLoadNativeHomeAddr(pslILEmit);
        pslILEmit->EmitINITOBJ(pslILEmit->GetToken(m_pargs->m_pMT));
    }

    virtual LocalDesc GetNativeType()
    {
        LEAF_CONTRACT;
        
        LocalDesc nativeType;

        nativeType.cbType = 1;
        nativeType.ElementType[0] = ELEMENT_TYPE_INTERNAL;
        nativeType.InternalToken = TypeHandle(m_pargs->m_pMT);

        return nativeType;
    }
};


class ILDelegateMarshaler : public ILMarshaler
{
public:
    enum
    {
        c_fNeedsClearNative     = FALSE,
        c_fInOnly               = TRUE,
    };

protected:
    virtual LocalDesc GetNativeType();
    virtual LocalDesc GetManagedType();
    virtual void EmitConvertContentsCLRToNative(ILCodeStream* pslILEmit);
    virtual void EmitConvertContentsNativeToCLR(ILCodeStream* pslILEmit);
};


class ILBoolMarshaler : public ILMarshaler
{
public:

    virtual CorElementType GetNativeBoolElementType() = 0;
    virtual int GetNativeTrueValue() = 0;
    virtual int GetNativeFalseValue() = 0;

protected:
    virtual LocalDesc GetNativeType();
    virtual LocalDesc GetManagedType();
    virtual void EmitConvertContentsCLRToNative(ILCodeStream* pslILEmit);
    virtual void EmitConvertContentsNativeToCLR(ILCodeStream* pslILEmit);
};

class ILWinBoolMarshaler : public ILBoolMarshaler
{
public:
    enum
    {
        c_fNeedsClearNative     = FALSE,
        c_fInOnly               = TRUE,
    };
        
protected:    
    virtual CorElementType GetNativeBoolElementType()
    {
        LEAF_CONTRACT;
        return ELEMENT_TYPE_I4;
    }

    virtual int GetNativeTrueValue()
    {
        LEAF_CONTRACT;
        return 1;
    }
        
    virtual int GetNativeFalseValue()
    {
        LEAF_CONTRACT;
        return 0;
    }
};

class ILCBoolMarshaler : public ILBoolMarshaler
{
public:
    enum
    {
        c_fNeedsClearNative     = FALSE,
        c_fInOnly               = TRUE,
    };

protected:
    virtual CorElementType GetNativeBoolElementType()
    {
        LEAF_CONTRACT;
        return ELEMENT_TYPE_I1;
    }

    virtual int GetNativeTrueValue()
    {
        LEAF_CONTRACT;
        return 1;
    }
                
    virtual int GetNativeFalseValue()
    {
        LEAF_CONTRACT;
        return 0;
    }
};

class ILVtBoolMarshaler : public ILBoolMarshaler
{
public:    
    enum
    {
        c_fNeedsClearNative     = FALSE,
        c_fInOnly               = TRUE,
    };

protected:    
    virtual CorElementType GetNativeBoolElementType()
    {
        LEAF_CONTRACT;
        return ELEMENT_TYPE_I2;
    }

    virtual int GetNativeTrueValue()
    {
        LEAF_CONTRACT;
        return VARIANT_TRUE;
    }

    virtual int GetNativeFalseValue()
    {
        LEAF_CONTRACT;
        return VARIANT_FALSE;
    }
};


class ILWSTRMarshaler : public ILMarshaler
{
public:
    enum
    {
        c_fNeedsClearNative     = FALSE,
        c_fInOnly               = TRUE,
    };

protected:
    virtual LocalDesc GetNativeType();
    virtual LocalDesc GetManagedType();

    virtual void EmitConvertSpaceCLRToNative(ILCodeStream* pslILEmit);
    virtual void EmitConvertContentsCLRToNative(ILCodeStream* pslILEmit);
    virtual void EmitConvertSpaceAndContentsCLRToNative(ILCodeStream* pslILEmit);

    virtual void EmitConvertSpaceNativeToCLR(ILCodeStream* pslILEmit);
    virtual void EmitConvertContentsNativeToCLR(ILCodeStream* pslILEmit);
    virtual void EmitConvertSpaceAndContentsNativeToCLR(ILCodeStream* pslILEmit);

    static bool CanUsePinnedManagedString(DWORD dwMarshalFlags);
    static void EmitCheckManagedStringLength(ILCodeStream* pslILEmit);
    static void EmitCheckNativeStringLength(ILCodeStream* pslILEmit);
};


class ILWSTRBufferMarshaler : public ILMarshaler
{
public:
    enum
    {
        c_fNeedsClearNative     = TRUE,
        c_fInOnly               = FALSE,
    };

    virtual LocalDesc GetNativeType();
    virtual LocalDesc GetManagedType();
    virtual void EmitConvertSpaceCLRToNative(ILCodeStream* pslILEmit);
    virtual void EmitConvertContentsCLRToNative(ILCodeStream* pslILEmit);
    virtual bool NeedsClearNative();
    virtual void EmitClearNative(ILCodeStream* pslILEmit);
    virtual void EmitConvertSpaceNativeToCLR(ILCodeStream* pslILEmit);
    virtual void EmitConvertContentsNativeToCLR(ILCodeStream* pslILEmit);
};

class ILCSTRBufferMarshaler : public ILMarshaler
{
public:
    enum
    {
        c_fNeedsClearNative     = TRUE,
        c_fInOnly               = FALSE,
    };

    virtual LocalDesc GetNativeType();
    virtual LocalDesc GetManagedType();
    virtual void EmitConvertSpaceCLRToNative(ILCodeStream* pslILEmit);
    virtual void EmitConvertContentsCLRToNative(ILCodeStream* pslILEmit);
    virtual bool NeedsClearNative();
    virtual void EmitClearNative(ILCodeStream* pslILEmit);
    virtual void EmitConvertSpaceNativeToCLR(ILCodeStream* pslILEmit);
    virtual void EmitConvertContentsNativeToCLR(ILCodeStream* pslILEmit);
};

        
class ILHandleRefMarshaler : public ILMarshaler
{
public:
    enum
    {
        c_fNeedsClearNative     = FALSE,
        c_fInOnly               = FALSE,
    };
        
    LocalDesc GetManagedType()
    {
        LEAF_CONTRACT;
        return LocalDesc();
    }
        
    LocalDesc GetNativeType()
    {
        LEAF_CONTRACT;
        return LocalDesc();
    }

    static MarshalerOverrideStatus ArgumentOverride(InteropStubLinker* psl,       // don't use this, use pslIL from macro
                                                    InteropStubLinker* pslPost,   // don't use this, use pslPostIL from macro
                                                    BOOL               byref,
                                                    BOOL               fin,
                                                    BOOL               fout,
                                                    BOOL               fManagedToNative,
                                                    OverrideProcArgs*  pargs,
                                                    UINT*              pResID,
                                                    UINT               argidx);

    static MarshalerOverrideStatus ReturnOverride(InteropStubLinker* psl,
                                                  InteropStubLinker* pslPost,
                                                  BOOL               fManagedToNative,
                                                  BOOL               fThruBuffer,
                                                  OverrideProcArgs*  pargs,
                                                  UINT*              pResID);
};

class ILSafeHandleMarshaler : public ILMarshaler
{
public:
    enum
    {
        c_fNeedsClearNative     = FALSE,
        c_fInOnly               = FALSE,
    };

    LocalDesc GetManagedType()
    {
        LEAF_CONTRACT;
        return LocalDesc();
    }
    
    LocalDesc GetNativeType()
    {
        LEAF_CONTRACT;
        return LocalDesc();
    }

        
    static MarshalerOverrideStatus ArgumentOverride(InteropStubLinker* psl,       // don't use this, use pslIL from macro
                                                    InteropStubLinker* pslPost,   // don't use this, use pslPostIL from macro
                                                    BOOL               byref,
                                                    BOOL               fin,
                                                    BOOL               fout,
                                                    BOOL               fManagedToNative,
                                                    OverrideProcArgs*  pargs,
                                                    UINT*              pResID,
                                                    UINT               argidx);
        
    static MarshalerOverrideStatus ReturnOverride(InteropStubLinker *psl,
                                                  InteropStubLinker *pslPost,
                                                  BOOL        fManagedToNative,
                                                  BOOL        fThruBuffer,
                                                  OverrideProcArgs *pargs,
                                                  UINT       *pResID);
};


class ILCriticalHandleMarshaler : public ILMarshaler
{
public:
    enum
    {
        c_fNeedsClearNative     = FALSE,
        c_fInOnly               = FALSE,
    };
        
public:

    LocalDesc GetManagedType()
    {
        LEAF_CONTRACT;
        return LocalDesc();
    }
    
    LocalDesc GetNativeType()
    {
        LEAF_CONTRACT;
        return LocalDesc();
    }
    
    static MarshalerOverrideStatus ArgumentOverride(InteropStubLinker* psl,       // don't use this, use pslIL from macro
                                                    InteropStubLinker* pslPost,   // don't use this, use pslPostIL from macro
                                                    BOOL               byref,
                                                    BOOL               fin,
                                                    BOOL               fout,
                                                    BOOL               fManagedToNative,
                                                    OverrideProcArgs*  pargs,
                                                    UINT*              pResID,
                                                    UINT               argidx);

    static MarshalerOverrideStatus ReturnOverride(InteropStubLinker *psl,
                                                  InteropStubLinker *pslPost,
                                                  BOOL        fManagedToNative,
                                                  BOOL        fThruBuffer,
                                                  OverrideProcArgs *pargs,
                                                  UINT       *pResID);
};


class ILValueClassMarshaler : public ILMarshaler
{
public:
    enum
    {
        c_fNeedsClearNative     = TRUE,
        c_fInOnly               = TRUE,
    };

protected:
    virtual LocalDesc GetNativeType();
    virtual LocalDesc GetManagedType();
    virtual void EmitReInitNative(ILCodeStream* pslILEmit);
    virtual bool NeedsClearNative();
    virtual void EmitClearNative(ILCodeStream * pslILEmit);
    virtual void EmitConvertContentsCLRToNative(ILCodeStream* pslILEmit);
    virtual void EmitConvertContentsNativeToCLR(ILCodeStream* pslILEmit);
};
        

class ILObjectMarshaler : public ILMarshaler
{
public:
    enum
    {
        c_fNeedsClearNative     = TRUE,
        c_fInOnly               = TRUE,
    };

protected:
    virtual LocalDesc GetNativeType();
    virtual LocalDesc GetManagedType();
    virtual void EmitConvertContentsCLRToNative(ILCodeStream* pslILEmit);
    virtual void EmitConvertContentsNativeToCLR(ILCodeStream* pslILEmit);
    virtual bool NeedsClearNative();
    virtual void EmitClearNative(ILCodeStream* pslILEmit);
    virtual void EmitReInitNative(ILCodeStream* pslILEmit);
};

class ILDateMarshaler : public ILMarshaler
{
public:
    enum
    {
        c_fNeedsClearNative     = FALSE,
        c_fInOnly               = TRUE,
    };
                
protected:
    virtual LocalDesc GetNativeType();
    virtual LocalDesc GetManagedType();
    virtual void EmitConvertContentsCLRToNative(ILCodeStream* pslILEmit);
    virtual void EmitConvertContentsNativeToCLR(ILCodeStream* pslILEmit);
};
                

class ILCurrencyMarshaler : public ILMarshaler
{
public:
    enum
    {
        c_fNeedsClearNative     = FALSE,
        c_fInOnly               = TRUE,
    };

protected:    
    virtual LocalDesc GetNativeType();
    virtual LocalDesc GetManagedType();
    virtual void EmitReInitNative(ILCodeStream* pslILEmit);
    virtual void EmitConvertContentsCLRToNative(ILCodeStream* pslILEmit);
    virtual void EmitConvertContentsNativeToCLR(ILCodeStream* pslILEmit);
};


class ILInterfaceMarshaler : public ILMarshaler
{
public:
    enum
    {
        c_fNeedsClearNative     = FALSE,
        c_fInOnly               = TRUE,
    };

protected:    
    virtual LocalDesc GetNativeType();
    virtual LocalDesc GetManagedType();
    virtual void EmitConvertContentsCLRToNative(ILCodeStream* pslILEmit);
    virtual void EmitConvertContentsNativeToCLR(ILCodeStream* pslILEmit);
    virtual bool NeedsClearNative();
    virtual void EmitClearNative(ILCodeStream* pslILEmit);
};


class ILAnsiCharMarshaler : public ILMarshaler
{
public:
    enum
    {
        c_fNeedsClearNative     = FALSE,
        c_fInOnly               = TRUE,
    };

protected:
    virtual LocalDesc GetNativeType();
    virtual LocalDesc GetManagedType();
    virtual void EmitConvertContentsCLRToNative(ILCodeStream* pslILEmit);
    virtual void EmitConvertContentsNativeToCLR(ILCodeStream* pslILEmit);
};


template <BinderClassID CLASS__ID>
class ILValueClassPtrMarshaler : public ILMarshaler
{
public:
    enum
    {
        c_fNeedsClearNative     = FALSE,
        c_fInOnly               = TRUE,
    };

protected:
    virtual LocalDesc GetNativeType()
    {
        LEAF_CONTRACT;

        //
        // pointer to value class
        //
        LocalDesc nativeType;
        nativeType.cbType          = 1;
        nativeType.ElementType[0]  = ELEMENT_TYPE_I;
        return nativeType;
    }

    virtual LocalDesc GetManagedType()
    {
        LEAF_CONTRACT;

        //
        // value class
        //
        LocalDesc managedType;
        managedType.cbType          = 1;
        managedType.ElementType[0]  = ELEMENT_TYPE_INTERNAL;
        managedType.InternalToken   = g_Mscorlib.GetClass(CLASS__ID);
        return managedType;
    }

    virtual void EmitConvertContentsCLRToNative(ILCodeStream* pslILEmit)
    {
        CONTRACTL
        {
            THROWS;
            GC_TRIGGERS;
            MODE_ANY;
        }
        CONTRACTL_END;

        EmitLoadManagedHomeAddr(pslILEmit);
        EmitStoreNativeValue(pslILEmit);
    }

    virtual void EmitConvertContentsNativeToCLR(ILCodeStream* pslILEmit)
    {
        CONTRACTL
        {
            THROWS;
            GC_TRIGGERS;
            MODE_ANY;
        }
        CONTRACTL_END;

        EmitLoadManagedHomeAddr(pslILEmit); // dest
        EmitLoadNativeValue(pslILEmit);     // src
        pslILEmit->EmitCPOBJ(pslILEmit->GetToken(g_Mscorlib.GetClass(CLASS__ID)));
    }
};

typedef ILValueClassPtrMarshaler<CLASS__GUID> ILGuidPtrMarshaler;
typedef ILValueClassPtrMarshaler<CLASS__NATIVE_DECIMAL> ILDecimalPtrMarshaler;
        
        
class ILOleColorMarshaler : public ILMarshaler
{
public:
    enum
    {
        c_fNeedsClearNative     = FALSE,
        c_fInOnly               = TRUE,
    };

protected:
    virtual LocalDesc GetNativeType();
    virtual LocalDesc GetManagedType();
    virtual void EmitConvertContentsCLRToNative(ILCodeStream* pslILEmit);
    virtual void EmitConvertContentsNativeToCLR(ILCodeStream* pslILEmit);
};

class ILVBByValStrWMarshaler : public ILMarshaler
{
public:
    enum
    {
        c_fNeedsClearNative     = FALSE,
        c_fInOnly               = FALSE,
    };

    ILVBByValStrWMarshaler() : 
        m_dwCCHLocal(-1)
    {
    }

    virtual bool SupportsArgumentMarshal(DWORD dwMarshalFlags, UINT* pErrorResID);
    virtual bool SupportsReturnMarshal(DWORD dwMarshalFlags, UINT* pErrorResID);

protected:
    virtual LocalDesc GetNativeType();
    virtual LocalDesc GetManagedType();
    virtual void EmitConvertContentsCLRToNative(ILCodeStream* pslILEmit);
    virtual void EmitConvertContentsNativeToCLR(ILCodeStream* pslILEmit);
    virtual bool IsNativePassedByRef();
        
    DWORD m_dwCCHLocal;
};

class ILVBByValStrMarshaler : public ILMarshaler
{
public:
    enum
    {
        c_fNeedsClearNative     = FALSE,
        c_fInOnly               = FALSE,
    };

    ILVBByValStrMarshaler() : 
        m_dwCCHLocal(-1)
    {
    }

    virtual bool SupportsArgumentMarshal(DWORD dwMarshalFlags, UINT* pErrorResID);
    virtual bool SupportsReturnMarshal(DWORD dwMarshalFlags, UINT* pErrorResID);

protected:
    virtual LocalDesc GetNativeType();
    virtual LocalDesc GetManagedType();
    virtual void EmitConvertContentsCLRToNative(ILCodeStream* pslILEmit);
    virtual void EmitConvertContentsNativeToCLR(ILCodeStream* pslILEmit);
    virtual bool NeedsClearNative();
    virtual void EmitClearNative(ILCodeStream* pslILEmit);
    virtual bool IsNativePassedByRef();

    DWORD m_dwCCHLocal;
};

class ILCSTRMarshaler : public ILMarshaler
{
public:
    enum
    {
        c_fNeedsClearNative     = TRUE,
        c_fInOnly               = TRUE,
    };

protected:    
    virtual LocalDesc GetNativeType();
    virtual LocalDesc GetManagedType();
    virtual void EmitConvertContentsCLRToNative(ILCodeStream* pslILEmit);
    virtual void EmitConvertContentsNativeToCLR(ILCodeStream* pslILEmit);
    virtual bool NeedsClearNative();
    virtual void EmitClearNative(ILCodeStream* pslILEmit);
};

class ILBSTRMarshaler : public ILMarshaler
{
public:
    enum
    {
        c_fNeedsClearNative     = TRUE,
        c_fInOnly               = TRUE,
    };

protected:    
    virtual LocalDesc GetNativeType();
    virtual LocalDesc GetManagedType();
    virtual void EmitConvertContentsCLRToNative(ILCodeStream* pslILEmit);
    virtual void EmitConvertContentsNativeToCLR(ILCodeStream* pslILEmit);
    virtual bool NeedsClearNative();
    virtual void EmitClearNative(ILCodeStream* pslILEmit);
};


class ILAnsiBSTRMarshaler : public ILMarshaler
{
public:
    enum
    {
        c_fNeedsClearNative     = TRUE,
        c_fInOnly               = TRUE,
    };

protected:    
    virtual LocalDesc GetNativeType();
    virtual LocalDesc GetManagedType();
    virtual void EmitConvertContentsCLRToNative(ILCodeStream* pslILEmit);
    virtual void EmitConvertContentsNativeToCLR(ILCodeStream* pslILEmit);
    virtual bool NeedsClearNative();
    virtual void EmitClearNative(ILCodeStream* pslILEmit);
};

class ILLayoutClassPtrMarshalerBase : public ILMarshaler
{
protected:    
    virtual LocalDesc GetNativeType();
    virtual LocalDesc GetManagedType();
    virtual void EmitConvertSpaceCLRToNative(ILCodeStream* pslILEmit);
    virtual void EmitConvertSpaceCLRToNativeTemp(ILCodeStream* pslILEmit);
    virtual void EmitConvertSpaceAndContentsCLRToNativeTemp(ILCodeStream* pslILEmit);
    virtual void EmitConvertSpaceNativeToCLR(ILCodeStream* pslILEmit);
    virtual bool NeedsClearNative();
    virtual void EmitClearNative(ILCodeStream* pslILEmit);
    virtual void EmitClearNativeTemp(ILCodeStream* pslILEmit);
};

class ILLayoutClassPtrMarshaler : public ILLayoutClassPtrMarshalerBase
{
public:
    enum
    {
        c_fNeedsClearNative     = TRUE,
        c_fInOnly               = FALSE,
    };
        
protected:    
    virtual void EmitConvertContentsCLRToNative(ILCodeStream* pslILEmit);
    virtual void EmitConvertContentsNativeToCLR(ILCodeStream* pslILEmit);
    virtual void EmitClearNativeContents(ILCodeStream * pslILEmit);
};

class ILBlittablePtrMarshaler : public ILLayoutClassPtrMarshalerBase
{
public:
    enum
    {
        c_fNeedsClearNative     = TRUE,
        c_fInOnly               = FALSE,
    };
            
protected:    
    virtual void EmitMarshalArgumentCLRToNative();
    virtual void EmitConvertContentsCLRToNative(ILCodeStream* pslILEmit);
    virtual void EmitConvertContentsNativeToCLR(ILCodeStream* pslILEmit);
};


class ILBlittableValueClassWithCopyCtorMarshaler : public ILMarshaler
{
public:
    enum
    {
        c_fNeedsClearNative     = FALSE,
        c_fInOnly               = TRUE,
    };

    LocalDesc GetManagedType()
    {
        LEAF_CONTRACT;
        return LocalDesc();
    }

    LocalDesc GetNativeType()
    {
        LEAF_CONTRACT;
        return LocalDesc();
    }

    static MarshalerOverrideStatus ArgumentOverride(InteropStubLinker* psl,       // don't use this, use pslIL from macro
                                            InteropStubLinker* pslPost,   // don't use this, use pslPostIL from macro
                                            BOOL               byref,
                                            BOOL               fin,
                                            BOOL               fout,
                                            BOOL               fManagedToNative,
                                            OverrideProcArgs*  pargs,
                                            UINT*              pResID,
                                            UINT               argidx);


};


class ILArgIteratorMarshaler : public ILMarshaler
{
public:
    enum
    {
        c_fNeedsClearNative     = FALSE,
        c_fInOnly               = TRUE,
    };

protected:
    virtual LocalDesc GetNativeType();
    virtual LocalDesc GetManagedType();
    virtual bool SupportsArgumentMarshal(DWORD dwMarshalFlags, UINT* pErrorResID);
    virtual void EmitMarshalArgumentCLRToNative();
    virtual void EmitMarshalArgumentNativeToCLR();
};
        
class ILArrayWithOffsetMarshaler : public ILMarshaler
{
public:
    enum
    {
        c_fNeedsClearNative     = FALSE,
        c_fInOnly               = FALSE,
    };

    ILArrayWithOffsetMarshaler() : 
        m_dwCountLocalNum(-1),
        m_dwOffsetLocalNum(-1),
        m_dwPinnedLocalNum(-1)
    {
    }

protected:
    virtual LocalDesc GetNativeType();
    virtual LocalDesc GetManagedType();
    virtual bool SupportsArgumentMarshal(DWORD dwMarshalFlags, UINT* pErrorResID);

    virtual void EmitConvertSpaceAndContentsCLRToNativeTemp(ILCodeStream* pslILEmit);
    virtual void EmitConvertContentsNativeToCLR(ILCodeStream* pslILEmit);
    virtual void EmitClearNativeTemp(ILCodeStream* pslILEmit);

        
    DWORD m_dwCountLocalNum;
    DWORD m_dwOffsetLocalNum;
    DWORD m_dwPinnedLocalNum;
};

class ILAsAnyMarshalerBase : public ILMarshaler
{
protected:
    virtual bool IsAnsi() = 0;
    virtual LocalDesc GetNativeType();
    virtual LocalDesc GetManagedType();
    virtual bool SupportsArgumentMarshal(DWORD dwMarshalFlags, UINT* pErrorResID);
    virtual bool SupportsReturnMarshal(DWORD dwMarshalFlags, UINT* pErrorResID);
    virtual void EmitMarshalArgumentCLRToNative();
};

class ILAsAnyWMarshaler : public ILAsAnyMarshalerBase
{
public:
    enum
    {
        c_fNeedsClearNative     = TRUE,
        c_fInOnly               = FALSE,
    };

protected:
    virtual bool IsAnsi() 
    {
        return false;
    }
};

class ILAsAnyAMarshaler : public ILAsAnyMarshalerBase
{
public:
    enum
    {
        c_fNeedsClearNative     = TRUE,
        c_fInOnly               = FALSE,
    };

protected:
    virtual bool IsAnsi() 
    {
        return true;
    }
};


class ILMngdMarshaler : public ILMarshaler
{
public:
    ILMngdMarshaler(BinderMethodID space2Man, 
                    BinderMethodID contents2Man, 
                    BinderMethodID space2Nat, 
                    BinderMethodID contents2Nat, 
                    BinderMethodID clearNat, 
                    BinderMethodID clearNatContents,
                    BinderMethodID clearMan) :
        m_idConvertSpaceToManaged(space2Man),
        m_idConvertContentsToManaged(contents2Man),
        m_idConvertSpaceToNative(space2Nat),
        m_idConvertContentsToNative(contents2Nat),
        m_idClearNative(clearNat),
        m_idClearNativeContents(clearNatContents),
        m_idClearManaged(clearMan)
    {
    }
    
protected:    
    virtual LocalDesc GetNativeType();
    virtual LocalDesc GetManagedType();

    virtual void EmitCreateMngdMarshaler(ILCodeStream* pslILEmit) = 0;

    virtual void EmitCallMngdMarshalerMethod(ILCodeStream* pslILEmit, BinderMethodID methodID);

    virtual void EmitConvertSpaceNativeToCLR(ILCodeStream* pslILEmit)
    {
        WRAPPER_CONTRACT;
        EmitCallMngdMarshalerMethod(pslILEmit, m_idConvertSpaceToManaged);
    }
    
    virtual void EmitConvertContentsNativeToCLR(ILCodeStream* pslILEmit)
    {
        WRAPPER_CONTRACT;
        EmitCallMngdMarshalerMethod(pslILEmit, m_idConvertContentsToManaged);
    }
    
    virtual void EmitConvertSpaceCLRToNative(ILCodeStream* pslILEmit)
    {
        WRAPPER_CONTRACT;
        EmitCallMngdMarshalerMethod(pslILEmit, m_idConvertSpaceToNative);
    }
    
    virtual void EmitConvertContentsCLRToNative(ILCodeStream* pslILEmit)
    {
        WRAPPER_CONTRACT;
        EmitCallMngdMarshalerMethod(pslILEmit, m_idConvertContentsToNative);
    }

    virtual bool NeedsClearNative()
    {
        LEAF_CONTRACT;

        if (METHOD__NIL != m_idClearNative)
        {
            return true;
        }
            
        return false;
    }
    
    virtual void EmitClearNative(ILCodeStream* pslILEmit)
    {
        WRAPPER_CONTRACT;
        EmitCallMngdMarshalerMethod(pslILEmit, m_idClearNative);
    }
    
    virtual void EmitClearNativeContents(ILCodeStream* pslILEmit)
    {
        WRAPPER_CONTRACT;
        EmitCallMngdMarshalerMethod(pslILEmit, m_idClearNativeContents);
    }

    
    virtual bool NeedsClearCLR()
    {
        LEAF_CONTRACT;

        if (METHOD__NIL != m_idClearManaged)
        {
            return true;
        }
            
        return false;
    }

    virtual void EmitClearCLR(ILCodeStream* pslILEmit)
    {
        WRAPPER_CONTRACT;
        EmitCallMngdMarshalerMethod(pslILEmit, m_idClearManaged);
    }

    const BinderMethodID m_idConvertSpaceToManaged;
    const BinderMethodID m_idConvertContentsToManaged;
    const BinderMethodID m_idConvertSpaceToNative;
    const BinderMethodID m_idConvertContentsToNative;
    const BinderMethodID m_idClearNative;
    const BinderMethodID m_idClearNativeContents;
    const BinderMethodID m_idClearManaged;
};

class ILNativeArrayMarshaler : public ILMngdMarshaler
{
public:    
    enum
    {
        c_fNeedsClearNative     = TRUE,
        c_fInOnly               = FALSE,
    };

    ILNativeArrayMarshaler() : 
        ILMngdMarshaler(
            METHOD__MNGD_NATIVE_ARRAY_MARSHALER__CONVERT_SPACE_TO_MANAGED,
            METHOD__MNGD_NATIVE_ARRAY_MARSHALER__CONVERT_CONTENTS_TO_MANAGED,
            METHOD__MNGD_NATIVE_ARRAY_MARSHALER__CONVERT_SPACE_TO_NATIVE,
            METHOD__MNGD_NATIVE_ARRAY_MARSHALER__CONVERT_CONTENTS_TO_NATIVE,
            METHOD__MNGD_NATIVE_ARRAY_MARSHALER__CLEAR_NATIVE,
            METHOD__MNGD_NATIVE_ARRAY_MARSHALER__CLEAR_NATIVE_CONTENTS,
            METHOD__NIL
            )
    {
    }

    virtual void EmitMarshalArgumentCLRToNative();
    
protected:

    bool UsePinnedArraySpecialCase();

    virtual void EmitCreateMngdMarshaler(ILCodeStream* pslILEmit);
};

class MngdNativeArrayMarshaler
{
public:
    static FCDECL6(void, CreateMarshaler,           MngdNativeArrayMarshaler* pThis, UINT32 SizeParamValue, MethodTable* pMT, UINT32 additive, UINT32 multiplier, UINT32 dwFlags);
    static FCDECL3(void, ConvertSpaceToNative,      MngdNativeArrayMarshaler* pThis, OBJECTREF* pManagedHome, void** pNativeHome);
    static FCDECL3(void, ConvertContentsToNative,   MngdNativeArrayMarshaler* pThis, OBJECTREF* pManagedHome, void** pNativeHome);
    static FCDECL3(void, ConvertSpaceToManaged,     MngdNativeArrayMarshaler* pThis, OBJECTREF* pManagedHome, void** pNativeHome);
    static FCDECL3(void, ConvertContentsToManaged,  MngdNativeArrayMarshaler* pThis, OBJECTREF* pManagedHome, void** pNativeHome);
    static FCDECL3(void, ClearNative,               MngdNativeArrayMarshaler* pThis, OBJECTREF* pManagedHome, void** pNativeHome);
    static FCDECL3(void, ClearNativeContents,       MngdNativeArrayMarshaler* pThis, OBJECTREF* pManagedHome, void** pNativeHome);

    static void DoClearNativeContents(MngdNativeArrayMarshaler* pThis, OBJECTREF* pManagedHome, void** pNativeHome);
        
    MethodTable*            m_pElementMT;
    TypeHandle              m_Array;
    INT32                   m_elementCount;
    BOOL                    m_BestFitMap;
    BOOL                    m_ThrowOnUnmappableChar;
    VARTYPE                 m_vt;
};


class ILSafeArrayMarshaler : public ILMngdMarshaler
{
public:    
    enum
    {
        c_fNeedsClearNative     = TRUE,
        c_fInOnly               = FALSE,
    };

    ILSafeArrayMarshaler() : 
        ILMngdMarshaler(
            METHOD__MNGD_SAFE_ARRAY_MARSHALER__CONVERT_SPACE_TO_MANAGED,
            METHOD__MNGD_SAFE_ARRAY_MARSHALER__CONVERT_CONTENTS_TO_MANAGED,
            METHOD__MNGD_SAFE_ARRAY_MARSHALER__CONVERT_SPACE_TO_NATIVE,
            METHOD__MNGD_SAFE_ARRAY_MARSHALER__CONVERT_CONTENTS_TO_NATIVE,
            METHOD__MNGD_SAFE_ARRAY_MARSHALER__CLEAR_NATIVE,
            METHOD__NIL,
            METHOD__NIL
            ), 
        m_dwOriginalManagedLocalNum(-1)
    {
    }
    
protected:

    virtual void EmitCreateMngdMarshaler(ILCodeStream* pslILEmit);
    virtual void EmitConvertContentsNativeToCLR(ILCodeStream* pslILEmit);
    virtual void EmitConvertContentsCLRToNative(ILCodeStream* pslILEmit);

    bool NeedsCheckForStatic()
    {
        WRAPPER_CONTRACT;
        return IsByref(m_dwMarshalFlags) && !IsCLRToNative(m_dwMarshalFlags) && IsIn(m_dwMarshalFlags) && IsOut(m_dwMarshalFlags);
    }

    DWORD m_dwOriginalManagedLocalNum;
};

class MngdSafeArrayMarshaler
{
public:
    static FCDECL4(void, CreateMarshaler,           MngdSafeArrayMarshaler* pThis, MethodTable* pMT, UINT32 iRank, UINT32 dwFlags);
    static FCDECL3(void, ConvertSpaceToNative,      MngdSafeArrayMarshaler* pThis, OBJECTREF* pManagedHome, void** pNativeHome);
    static FCDECL4(void, ConvertContentsToNative,   MngdSafeArrayMarshaler* pThis, OBJECTREF* pManagedHome, void** pNativeHome, Object* pOriginalManagedUNSAFE);
    static FCDECL3(void, ConvertSpaceToManaged,     MngdSafeArrayMarshaler* pThis, OBJECTREF* pManagedHome, void** pNativeHome);
    static FCDECL3(void, ConvertContentsToManaged,  MngdSafeArrayMarshaler* pThis, OBJECTREF* pManagedHome, void** pNativeHome);
    static FCDECL3(void, ClearNative,               MngdSafeArrayMarshaler* pThis, OBJECTREF* pManagedHome, void** pNativeHome);

    enum StaticCheckStateFlags
    {
        SCSF_CheckForStatic = 1,
        SCSF_IsStatic = 2
    };

    MethodTable*    m_pElementMT;
    int             m_iRank;
    VARTYPE         m_vt;
    BYTE            m_fStatic;     // StaticCheckStateFlags
    BYTE            m_nolowerbounds;
};


class ILReferenceCustomMarshaler : public ILMngdMarshaler
{
public:    
    enum
    {
        c_fNeedsClearNative     = TRUE,
        c_fInOnly               = FALSE,
    };

    ILReferenceCustomMarshaler() : 
        ILMngdMarshaler(
            METHOD__NIL,
            METHOD__MNGD_REF_CUSTOM_MARSHALER__CONVERT_CONTENTS_TO_MANAGED,
            METHOD__NIL,
            METHOD__MNGD_REF_CUSTOM_MARSHALER__CONVERT_CONTENTS_TO_NATIVE,
            METHOD__MNGD_REF_CUSTOM_MARSHALER__CLEAR_NATIVE,
            METHOD__NIL,
            METHOD__MNGD_REF_CUSTOM_MARSHALER__CLEAR_MANAGED
            )
    {
    }
        
protected:
    virtual void EmitCreateMngdMarshaler(ILCodeStream* pslILEmit);
};

class MngdRefCustomMarshaler
{
public:
    static FCDECL2(void, CreateMarshaler,           MngdRefCustomMarshaler* pThis, void* pCMHelper);
    static FCDECL3(void, ConvertContentsToNative,   MngdRefCustomMarshaler* pThis, OBJECTREF* pManagedHome, void** pNativeHome);
    static FCDECL3(void, ConvertContentsToManaged,  MngdRefCustomMarshaler* pThis, OBJECTREF* pManagedHome, void** pNativeHome);
    static FCDECL3(void, ClearNative,               MngdRefCustomMarshaler* pThis, OBJECTREF* pManagedHome, void** pNativeHome);
    static FCDECL3(void, ClearManaged,              MngdRefCustomMarshaler* pThis, OBJECTREF* pManagedHome, void** pNativeHome);

    static void DoClearNativeContents(MngdRefCustomMarshaler* pThis, OBJECTREF* pManagedHome, void** pNativeHome);

    CustomMarshalerHelper*  m_pCMHelper;
};

