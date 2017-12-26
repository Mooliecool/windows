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

#ifndef __ILSTUBRESOLVER_H__
#define __ILSTUBRESOLVER_H__

#include "stubgen.h"
class ILStubResolver : DynamicResolver
{
    friend class ILStubCache;
    friend class ILStubLinker;

public:

    // -----------------------------------
    // DynamicResolver interface methods
    // -----------------------------------

    void FreeCompileTimeState();
    void GetJitContext(DWORD* pdwSecurityControlFlags, TypeHandle* pTypeOwner);
    ChunkAllocator* GetJitMetaHeap();

    BYTE* GetCodeInfo(unsigned* pCodeSize, unsigned short* pStackSize, CorInfoOptions* pOptions, unsigned short* pEHSize);
    PCCOR_SIGNATURE GetLocalSig(DWORD* pSigSize);
    
    StringObject* GetStringLiteral(mdToken token);
    void* ResolveToken(mdToken token);
    unsigned ResolveParentToken(mdToken token);
    PCCOR_SIGNATURE ResolveSignature(mdToken token);
    PCCOR_SIGNATURE ResolveSignatureForVarArg(mdToken token);
    void GetEHInfo(unsigned EHnumber, CORINFO_EH_CLAUSE* clause);
    BOOL IsValidToken(mdToken token);
    void* GetInstantiationInfo(mdToken token);

    static LPUTF8 GetStubClassName(MethodDesc* pMD);

    MethodDesc* GetDynamicMethod() { LEAF_CONTRACT; return m_pStubMD; }
    
    // -----------------------------------
    // ILStubResolver-specific methods
    // -----------------------------------
    bool IsSecurityStub();
    bool IsNativeToCLRInteropStub();
    MethodDesc* GetStubMethodDesc();
    MethodDesc* GetStubTargetMethodDesc();
    void SetStubTargetMethodDesc(MethodDesc* pStubTargetMD);
    void SetStubTargetMethodSig(PCCOR_SIGNATURE pStubTargetMethodSig, size_t cbStubTargetSigLength);
    void SetStubMethodDesc(MethodDesc* pStubMD);

    COR_ILMETHOD_DECODER* AllocGeneratedIL(size_t cbCode, size_t cbLocalSig, UINT maxStack);
    COR_ILMETHOD_DECODER* GetILHeader();
    COR_ILMETHOD_SECT_EH* AllocEHSect(size_t nClauses);

    bool IsCompiled();
    bool IsILGenerated();

    ILStubResolver();

    void SetTokenLookupMap(TokenLookupMap* pMap);

    void SetJitFlags(DWORD dwJitFlags);
    DWORD GetJitFlags();

#ifdef CALLDESCR_REGTYPEMAP
    UINT_PTR* GetRegTypeMapLocation();
#endif // CALLDESCR_REGTYPEMAP

    static void StubGenFailed(ILStubResolver* pResolver);
    mdMethodDef GetStubMethodDef();


protected:    
    enum ILStubType
    {
        Unassigned = 0,
        SecurityStub,
        CLRToNativeInteropStub,
        CLRToCOMInteropStub,
        NativeToCLRInteropStub,
        COMToCLRInteropStub,
    };

    enum CompileTimeStatePtrSpecialValues
    {
        ILNotYetGenerated   = NULL,
        ILGeneratedAndFreed = 1,
    };

    void ClearCompileTimeState();
    void SetStubType(ILStubType stubType);

    //
    // This stuff is only needed during JIT
    //
    struct CompileTimeState
    {
        COR_ILMETHOD_DECODER    m_ILHeader;
        COR_ILMETHOD_SECT_EH*   m_pEHSect;
        PCCOR_SIGNATURE         m_pStubTargetMethodSig;
        TokenLookupMap          m_tokenLookupMap;
    };
    typedef DPTR(struct CompileTimeState) PTR_CompileTimeState;

    PTR_CompileTimeState    m_pCompileTimeState;

#ifdef CALLDESCR_REGTYPEMAP
    UINT_PTR                m_uRegTypeMap;
#endif // CALLDESCR_REGTYPEMAP
    PTR_MethodDesc          m_pStubMD;
    PTR_MethodDesc          m_pStubTargetMD;
    ILStubType              m_type;
    DWORD                   m_dwJitFlags;
};

typedef Holder<ILStubResolver*, DoNothing<ILStubResolver*>, ILStubResolver::StubGenFailed, NULL> ILStubGenHolder;


#endif // __ILSTUBRESOLVER_H__

