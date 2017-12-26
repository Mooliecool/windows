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
// DebugInfoStore

#include "common.h"
#include "debuginfostore.h"
#include "nibblestream.h"


#ifdef _DEBUG
// For debug builds only. 
static bool Dbg_ShouldUseCookies()
{
    // Normally we want this as false b/c it would bloat the image.
    // But give us a hook to enable it in case we need it.
    return false;
}
#endif

//-----------------------------------------------------------------------------
// We have "Transfer" objects that sit on top of the streams.
// The objects look identical, but one serializes and the other deserializes.
// This lets the compression + restoration routines share all their compression
// logic and just swap out Transfer objects.
//
// It's not ideal that we have a lot of redundancy maintaining both Transfer
// objects, but at least the compiler can enforce that the Reader & Writer are 
// in sync. It can't enforce that a 2 separate routines for Compression &
// restoration are in sync.
// 
// We could have the TransferReader + Writer be polymorphic off a base class,
// but the virtual function calls will be extra overhead. May as well use
// templates and let the compiler resolve it all statically at compile time.
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
// Serialize to a NibbleWriter stream.
//-----------------------------------------------------------------------------
class TransferWriter
{
public:
    TransferWriter(NibbleWriter & w) : m_w(w)
    {
    }

    // Write an raw U32 in nibble encoded form.
    void DoEncodedU32(DWORD dw) { m_w.WriteEncodedU32(dw); }

    // Use to encode a monotonically increasing delta.
    void DoEncodedDeltaU32(DWORD & dw, DWORD dwLast) 
    { 
        _ASSERTE(dw >= dwLast);
        DWORD dwDelta = dw - dwLast;
        m_w.WriteEncodedU32(dwDelta);         
    }


    // Some U32 may have a few sentinal negative values . 
    // We adjust it to be a real U32 and then encode that.
    // dwAdjust should be the lower bound on the enum.
    void DoEncodedAdjustedU32(DWORD dw, DWORD dwAdjust) 
    { 
        //_ASSERTE(dwAdjust < 0); // some negative lower bound.
        m_w.WriteEncodedU32(dw - dwAdjust); 
    }

    // Typesafe versions of EncodeU32.
    void DoEncodedSourceType(ICorDebugInfo::SourceTypes & dw) { m_w.WriteEncodedU32(dw); }
    void DoEncodedVarLocType(ICorDebugInfo::VarLocType & dw) { m_w.WriteEncodedU32(dw); }
    void DoEncodedUnsigned(unsigned & dw) { m_w.WriteEncodedU32(dw); }

    // Stack offsets are aligned on a DWORD boundary, so that lets us shave off 2 bits.
    void DoEncodedStackOffset(signed & dwOffset)
    {        
#ifdef _X86_    
        _ASSERTE(dwOffset % sizeof(DWORD) == 0); // should be dword aligned. That'll save us 2 bits.
        m_w.WriteEncodedI32(dwOffset / sizeof(DWORD));
#else
        // Non x86 platforms don't need it to be dword aligned.
        m_w.WriteEncodedI32(dwOffset);
#endif
    }

    void DoEncodedRegIdx(ICorDebugInfo::RegNum & reg) { m_w.WriteEncodedU32(reg); }

    void DoRawPtr(void * & ptr) { m_w.WritePtr(ptr); }

    // For debugging purposes, inject cookies into the Compression.
    void DoCookie(BYTE b) { 
#ifdef _DEBUG
        if (Dbg_ShouldUseCookies()) 
        {
            m_w.WriteNibble(b); 
        }
#endif    
    }
    
protected:
    NibbleWriter & m_w;

};

//-----------------------------------------------------------------------------
// Deserializer that sits on top of a NibbleReader
// This class interface matches TransferWriter exactly. See that for details.
//-----------------------------------------------------------------------------
class TransferReader
{
public:
    TransferReader(NibbleReader & r) : m_r(r)
    {
    }

    void DoEncodedU32(DWORD & dw) { dw = m_r.ReadEncodedU32(); }

    // Use to decode a monotonically increasing delta.
    // dwLast was the last value; we update it to the current value on output.
    void DoEncodedDeltaU32(DWORD & dw, DWORD dwLast) 
    {         
        DWORD dwDelta = m_r.ReadEncodedU32();
        dw = dwLast + dwDelta;
    }
    
    void DoEncodedAdjustedU32(DWORD & dw, DWORD dwAdjust) 
    { 
        //_ASSERTE(dwAdjust < 0);
        dw = m_r.ReadEncodedU32() + dwAdjust; 
    }
    void DoEncodedSourceType(ICorDebugInfo::SourceTypes & dw) { dw = (ICorDebugInfo::SourceTypes) m_r.ReadEncodedU32(); }
    void DoEncodedVarLocType(ICorDebugInfo::VarLocType & dw) { dw = (ICorDebugInfo::VarLocType) m_r.ReadEncodedU32(); }
    void DoEncodedUnsigned(unsigned & dw) { dw = (unsigned) m_r.ReadEncodedU32(); }


    // Stack offsets are aligned on a DWORD boundary, so that lets us shave off 2 bits.
    void DoEncodedStackOffset(signed & dwOffset)
    {    
#ifdef _X86_    
        dwOffset = m_r.ReadEncodedI32() * sizeof(DWORD);        
#else
        // Non x86 platforms don't need it to be dword aligned.
        dwOffset = m_r.ReadEncodedI32();
#endif        
    }

    void DoEncodedRegIdx(ICorDebugInfo::RegNum & reg) { reg = (ICorDebugInfo::RegNum) m_r.ReadEncodedU32(); }

    void DoRawPtr(void * & ptr) { ptr = m_r.ReadPtr(); }

    // For debugging purposes, inject cookies into the Compression.
    void DoCookie(BYTE b) 
    { 
#ifdef _DEBUG    
        if (Dbg_ShouldUseCookies())
        {
            BYTE b2 = m_r.ReadNibble(); 
            _ASSERTE(b == b2); 
        }
#endif        
    }


protected:
    NibbleReader & m_r;
};


#if defined(_DEBUG) && !defined(DACCESS_COMPILE)
// Perf tracking 
static int g_CDI_TotalMethods           = 0;
static int g_CDI_bMethodTotalUncompress = 0;
static int g_CDI_bMethodTotalCompress   = 0;

static int g_CDI_bVarsTotalUncompress   = 0;
static int g_CDI_bVarsTotalCompress     = 0;

BYTE* MyDummyNew(void * pData, size_t cBytes)
{
    return new BYTE[cBytes];
}
#endif

//-----------------------------------------------------------------------------
// Serialize Bounds info. 
//-----------------------------------------------------------------------------
template <class T>
void DoBounds(
    T trans, // transfer object.
    ULONG32                       cMap,
    ICorDebugInfo::OffsetMapping *pMap
)
{
    CONTRACTL
    {
        THROWS;
        GC_NOTRIGGER;
        MODE_ANY;
    }
    CONTRACTL_END;
    

    // Bounds info contains (Native Offset, IL Offset, flags)
    // - Sorted by native offset (so use a delta encoding for that).
    // - IL offsets aren't sorted, but they should be close to each other (so a signed delta encoding)
    //   They may also include a sentinel value from MappingTypes.
    // - flags is 3 indepedent bits.

    // Loop through and transfer each Entry in the Mapping.
    DWORD dwLastNativeOffset = 0;        
    for(DWORD i = 0; i < cMap; i++)
    {
        ICorDebugInfo::OffsetMapping * pBound = &pMap[i];

        trans.DoEncodedDeltaU32(pBound->nativeOffset, dwLastNativeOffset);
        dwLastNativeOffset = pBound->nativeOffset;


        trans.DoEncodedAdjustedU32(pBound->ilOffset, ICorDebugInfo::MAX_MAPPING_VALUE);

        trans.DoEncodedSourceType(pBound->source);

        trans.DoCookie(0xA);
    }
}



// Helper to write a compressed Native Var Info
template<class T>
void DoNativeVarInfo(
    T trans, 
    ICorDebugInfo::NativeVarInfo * pVar
)
{
    CONTRACTL
    {
        THROWS;
        GC_NOTRIGGER;
        MODE_ANY;
    }
    CONTRACTL_END;
    

    // Each Varinfo has a:
    // - native start +End offset. We can use a delta for the end offset. 
    // - Il variable number. These are usually small.
    // - VarLoc information. This is a tagged variant. 
    // The entries aren't sorted in any particular order.
    trans.DoCookie(0xB);
    trans.DoEncodedU32(pVar->startOffset);


    trans.DoEncodedDeltaU32(pVar->endOffset, pVar->startOffset);

    // record var number.   
    trans.DoEncodedAdjustedU32(pVar->varNumber, ICorDebugInfo::MAX_ILNUM);


    // Now write the VarLoc... This is a variant like structure and so we'll get different 
    // compressioned depending on what we've got.
    trans.DoEncodedVarLocType(pVar->loc.vlType);

    switch(pVar->loc.vlType)
    {
    case ICorDebugInfo::VLT_REG:
    case ICorDebugInfo::VLT_REG_FP:     // fall through
    case ICorDebugInfo::VLT_REG_BYREF:  // fall through
        trans.DoEncodedRegIdx(pVar->loc.vlReg.vlrReg);
        break;

    case ICorDebugInfo::VLT_STK:
    case ICorDebugInfo::VLT_STK_BYREF:  // fall through
        trans.DoEncodedRegIdx(pVar->loc.vlStk.vlsBaseReg);
        trans.DoEncodedStackOffset(pVar->loc.vlStk.vlsOffset);
        break;

    case ICorDebugInfo::VLT_REG_REG:
        trans.DoEncodedRegIdx(pVar->loc.vlRegReg.vlrrReg1);
        trans.DoEncodedRegIdx(pVar->loc.vlRegReg.vlrrReg2);
        break;

    case ICorDebugInfo::VLT_REG_STK:
        trans.DoEncodedRegIdx(pVar->loc.vlRegStk.vlrsReg);
        trans.DoEncodedRegIdx(pVar->loc.vlRegStk.vlrsStk.vlrssBaseReg);
        trans.DoEncodedStackOffset(pVar->loc.vlRegStk.vlrsStk.vlrssOffset);
        break;


    case ICorDebugInfo::VLT_STK_REG:
        trans.DoEncodedStackOffset(pVar->loc.vlStkReg.vlsrStk.vlsrsOffset);
        trans.DoEncodedRegIdx(pVar->loc.vlStkReg.vlsrStk.vlsrsBaseReg);            
        trans.DoEncodedRegIdx(pVar->loc.vlStkReg.vlsrReg);
        
        break;

    case ICorDebugInfo::VLT_STK2:
        trans.DoEncodedRegIdx(pVar->loc.vlStk2.vls2BaseReg);
        trans.DoEncodedStackOffset(pVar->loc.vlStk2.vls2Offset);        
        break; 

    case ICorDebugInfo::VLT_FPSTK:
        trans.DoEncodedUnsigned(pVar->loc.vlFPstk.vlfReg);
        break;

    case ICorDebugInfo::VLT_FIXED_VA:
        trans.DoEncodedUnsigned(pVar->loc.vlFixedVarArg.vlfvOffset);
        break;

    default:
        _ASSERTE(!"Unknown varloc type!");
        break;
    }


    trans.DoCookie(0xC);
}


#ifndef DACCESS_COMPILE
// Compress incoming data and write it to the provided SBuffer.
void CompressDebugInfo::CompressBoundaries(
    IN ULONG32                       cMap,
    IN ICorDebugInfo::OffsetMapping *pMap,
    IN OUT SBuffer * pBuffer
)
{
    CONTRACTL
    {
        THROWS;
        GC_NOTRIGGER;
        MODE_ANY;

        PRECONDITION(cMap > 0); // caller ensures we have something to compress
        PRECONDITION(CheckPointer(pMap));
        PRECONDITION(CheckPointer(pBuffer));
    }
    CONTRACTL_END;    

    ULONG32 cbGuess = EstimateBoundCompressionSize(cMap, pMap);
    BYTE * ptr = pBuffer->OpenRawBuffer(cbGuess);
    _ASSERTE(ptr != NULL); // throws on oom.
    
    CompressBoundaries(cMap, pMap, &cbGuess, ptr);
    // cbGuess now set to actual number of bytes.

    pBuffer->CloseRawBuffer(cbGuess);
    
}

// Compress incoming data and write it to the provided SBuffer.
void CompressDebugInfo::CompressVars(
    IN ULONG32                         cVars,
    IN ICorDebugInfo::NativeVarInfo    *pVars,
    IN OUT SBuffer * pBuffer
)
{
    CONTRACTL
    {
        THROWS;
        GC_NOTRIGGER;
        MODE_ANY;

        PRECONDITION(cVars > 0); // caller ensures we have something to compress
        PRECONDITION(CheckPointer(pVars));
        PRECONDITION(CheckPointer(pBuffer));
    }
    CONTRACTL_END;    

    ULONG32 cbGuess = EstimateVarCompressionSize(cVars, pVars);
    BYTE * ptr = pBuffer->OpenRawBuffer(cbGuess);
    _ASSERTE(ptr != NULL); // throws on oom.
    
    CompressVars(cVars, pVars, &cbGuess, ptr);
    // cbGuess now set to actual number of bytes.

    pBuffer->CloseRawBuffer(cbGuess);
}


    
ULONG32 CompressDebugInfo::EstimateBoundCompressionSize(
    IN ULONG32                       cMap,
    IN ICorDebugInfo::OffsetMapping *pMap
)
{
    LEAF_CONTRACT;
    
    // Generally we compress at about 20%, so returning a buffer that's 150% should be just fine.
    return (cMap * sizeof(ICorDebugInfo::OffsetMapping) * 3) / 2;
}

ULONG32 CompressDebugInfo::EstimateVarCompressionSize(
    IN ULONG32                         cVars,
    IN ICorDebugInfo::NativeVarInfo    *vars
)
{
    LEAF_CONTRACT;
    
    // Generally we compress at about 20%, so returning a buffer that's 150% should be just fine.
    return (cVars* sizeof(ICorDebugInfo::NativeVarInfo) * 3) / 2;
}



void CompressDebugInfo::CompressBoundaries(    
    IN ULONG32                       cMap,
    IN ICorDebugInfo::OffsetMapping *pMap,
    IN OUT ULONG32                  *pcBytes,
    OUT BYTE                        *pBuffer        
)
{
    CONTRACTL
    {
        THROWS;
        GC_NOTRIGGER;
        MODE_ANY;
    }
    CONTRACTL_END;    

    _ASSERTE(pcBytes != NULL);
    _ASSERTE((pMap == NULL) == (cMap == 0));

    size_t size = 0;
        

    if (cMap == 0)
    {
        *pcBytes = 0;
    }
    else
    {    
        _ASSERTE(pBuffer != NULL);
        
        size_t bigSize = (size_t) *pcBytes;
        NibbleWriter w(pBuffer, bigSize);
        TransferWriter t(w);        

        void * pFinalBuffer = NULL;

        w.WriteEncodedU32(cMap);
        DoBounds(t, cMap, pMap);

        size = w.GetFinalByteCount();
        pFinalBuffer = w.GetFinalBuffer();

        // Update out param for size. Caller can copy over to a well-fitted buffer.
        *pcBytes = (ULONG32) size;
    }

#ifdef _DEBUG
    // Track perf #s for compression...
    g_CDI_TotalMethods++;
    g_CDI_bMethodTotalUncompress += sizeof(ICorDebugInfo::OffsetMapping) * cMap;
    g_CDI_bMethodTotalCompress   += (int) size;
    

    static DWORD fCheck = 99;

    if (fCheck == 99)
        fCheck = REGUTIL::GetConfigDWORD(L"DbgTC", 0);


    if (fCheck) 
    {
        ULONG32 c2;
        ICorDebugInfo::OffsetMapping * pMap2 = NULL;

        CompressDebugInfo::Boundaries info;
        info.Deserialize((TADDR) pBuffer, *pcBytes);
        RestoreBoundaries(MyDummyNew, NULL, &info, &c2, &pMap2);   

        _ASSERTE(c2 == cMap);
        
        bool fSame = memcmp(pMap2, pMap, c2 * sizeof(ICorDebugInfo::OffsetMapping)) == 0;
        _ASSERTE(fSame);
        delete pMap2;
    }
#endif // _DEBUG

} // end CompressBoundaries


void CompressDebugInfo::CompressVars(
    IN ULONG32                         cVars,
    IN ICorDebugInfo::NativeVarInfo    *vars,
    IN OUT ULONG32                     *pcBytes,
    OUT BYTE                           *pBuffer
)
{
    CONTRACTL
    {
        THROWS;
        GC_NOTRIGGER;
        MODE_ANY;
    }
    CONTRACTL_END;
    
    _ASSERTE(pcBytes != NULL);
    _ASSERTE((cVars == 0) == (vars == NULL));

    size_t size = 0;

    if (cVars == 0)
    {
        *pcBytes = 0;
    }
    else
    {
        _ASSERTE(pBuffer != NULL);
    
        size_t bigSize = (size_t) *pcBytes;
        void * pFinalBuffer = NULL;
        NibbleWriter w(pBuffer, bigSize);
        TransferWriter t(w);
            

        w.WriteEncodedU32(cVars);
        for(ULONG32 i = 0; i < cVars; i ++)
        {
            DoNativeVarInfo(t, &vars[i]);
        }
        size         = w.GetFinalByteCount();
        pFinalBuffer = w.GetFinalBuffer();

        // Update out-param
        *pcBytes = (ULONG32) size;
    
    }

#ifdef _DEBUG
    g_CDI_bVarsTotalUncompress += cVars * sizeof(ICorDebugInfo::NativeVarInfo);
    g_CDI_bVarsTotalCompress   += (int) size;

    static DWORD fCheck = 99;

    if (fCheck == 99)
        fCheck = REGUTIL::GetConfigDWORD(L"DbgTC", 0);

    if (fCheck) 
    {
        ULONG32 c2;
        ICorDebugInfo::NativeVarInfo * pVars2 = NULL;
        size_t size2 = cVars * sizeof(ICorDebugInfo::NativeVarInfo);

        CompressDebugInfo::Vars info;
        info.Deserialize((TADDR) pBuffer, *pcBytes);

        RestoreVars(MyDummyNew, NULL, &info, &c2, &pVars2);   

        _ASSERTE(c2 == cVars);

        bool fSame = memcmp(pVars2, vars, size2) == 0;
        if (!fSame)
        {
        }

        
        delete pVars2;
    }
#endif

    
}

#endif // DACCESS_COMPILE   

// For deserializing, we may need to quickly extract just the number of bounds.
ULONG32 CompressDebugInfo::GetNumBounds(
    IN CompressDebugInfo::Boundaries * pCompressBoundsInfo
)
{
    CONTRACTL
    {
        THROWS; // reading from nibble stream may throw on invalid data.
        GC_NOTRIGGER;
        MODE_ANY;
    }
    CONTRACTL_END;
    

    ULONG32 size = (ULONG32) pCompressBoundsInfo->GetSize();
    NibbleReader r(pCompressBoundsInfo->GetBuffer(), size);
    
    ULONG32 cNumEntries = r.ReadEncodedU32();    

    return cNumEntries;
}


// Uncompress data supplied by Compress functions.
void CompressDebugInfo::RestoreBoundaries(
    IN FP_IDS_NEW fpNew, IN void * pNewData,
    IN CompressDebugInfo::Boundaries * pCompressBoundsInfo,
    OUT ULONG32                       * pcMap, // number of entries in ppMap
    OUT ICorDebugInfo::OffsetMapping **ppMap // pointer to newly allocated array
)
{
    CONTRACTL
    {
        THROWS;
        WRAPPER(GC_TRIGGERS); // depends on fpNew
    }
    CONTRACTL_END;
    
    _ASSERTE(fpNew != NULL);
    _ASSERTE(ppMap != NULL);
    _ASSERTE(pCompressBoundsInfo != NULL);   

    int cNumEntries = 0;
    ICorDebugInfo::OffsetMapping *pMap = NULL;
    ULONG32 size = (ULONG32) pCompressBoundsInfo->GetSize();

    if (size != 0)
    {
        {
            NibbleReader r(pCompressBoundsInfo->GetBuffer(), size);
            TransferReader t(r);            

            cNumEntries = r.ReadEncodedU32();
            _ASSERTE(cNumEntries > 0);
            pMap = reinterpret_cast<ICorDebugInfo::OffsetMapping *> 
                (fpNew(pNewData, cNumEntries * sizeof(ICorDebugInfo::OffsetMapping)));

            if (pMap == NULL)
            {
                ThrowOutOfMemory();
            }
            
            // Main decompression routine.
            DoBounds(t, cNumEntries, pMap);
        }            
           
        *ppMap = pMap;
        *pcMap = cNumEntries;        
    } 
    else
    {
        *ppMap = NULL;
        *pcMap = 0;
    }
}

ULONG32 CompressDebugInfo::GetNumVars(IN CompressDebugInfo::Vars * pCompressVarsInfo)
{
    CONTRACTL
    {
        THROWS; // reading from nibble stream may throw on invalid data.
        GC_NOTRIGGER;
        MODE_ANY;
    }
    CONTRACTL_END;
    

    ULONG32 size = (ULONG32) pCompressVarsInfo->GetSize();
    NibbleReader r(pCompressVarsInfo->GetBuffer(), size);

    ULONG32 cNumEntries = r.ReadEncodedU32();   

    return cNumEntries;
}


void CompressDebugInfo::RestoreVars(
    IN FP_IDS_NEW fpNew, IN void * pNewData,
    IN CompressDebugInfo::Vars * pCompressVarsInfo,
    OUT ULONG32                         * cVars,
    OUT ICorDebugInfo::NativeVarInfo    **vars
)
{
    CONTRACTL
    {
        THROWS;
        WRAPPER(GC_TRIGGERS); // depends on fpNew
    }
    CONTRACTL_END;
    
    _ASSERTE(fpNew != NULL);
    _ASSERTE(vars != NULL);
    _ASSERTE(pCompressVarsInfo != NULL);   

    ICorDebugInfo::NativeVarInfo *pVars = NULL;
    ULONG32 size = (ULONG32) pCompressVarsInfo->GetSize();
    if (size != 0)
    {
        int cNumEntries = 0;
    
        {    
            NibbleReader r(pCompressVarsInfo->GetBuffer(), size);
            TransferReader t(r);

            cNumEntries = r.ReadEncodedU32();
            
            _ASSERTE(cNumEntries > 0);
            pVars = reinterpret_cast<ICorDebugInfo::NativeVarInfo *> 
                (fpNew(pNewData, cNumEntries * sizeof(ICorDebugInfo::NativeVarInfo)));
            
            if (pVars == NULL)
            {
                ThrowOutOfMemory();
            }

            for(int i = 0; i < cNumEntries; i++)
            {
                //ReadNativeVarInfo(r, &pVars[i]);
                DoNativeVarInfo(t, &pVars[i]);
            }
        }

        *cVars = cNumEntries;
        *vars = pVars;
    }
    else
    {
        *vars = NULL;
        *cVars = 0;
    }
}




// Get a local copy of the buffer
BYTE * CompressDebugInfo::MyBuffer::GetBuffer()
{
    LEAF_CONTRACT;

    BYTE* pData = (BYTE*) PTR_READ(m_pData, (ULONG32) this->GetSize());
    return pData;
}
size_t CompressDebugInfo::MyBuffer::GetSize()
{   
    LEAF_CONTRACT;
    return m_cBytes;
}

#ifdef DACCESS_COMPILE
// Used when taking a minidump to dump the DebugInfo.
void CompressDebugInfo::MyBuffer::EnumMemoryRegions(CLRDataEnumMemoryFlags flags)
{
    if (m_pData != NULL)
    {
        ULONG32 size = (ULONG32) this->GetSize();
        DacEnumMemoryRegion(m_pData, size);
    }
}
#endif        

       
//-----------------------------------------------------------------------------
// Compression routines
// DAC only needs to run the uncompression routines.
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
// Uncompression (restore) routines
//-----------------------------------------------------------------------------




// Init given a starting address from the start of code.
void DebugInfoRequest::InitFromStartingAddr(MethodDesc * pMD, TADDR addrCode)
{
    LEAF_CONTRACT;
    
    // For now, we'll just demand both
    _ASSERTE(pMD != NULL);
    _ASSERTE(addrCode != NULL);
    

    _ASSERTE((pMD != NULL) || (addrCode != NULL));
    this->m_pMD       = pMD;
    this->m_addrStart = addrCode;
    
    if ((pMD != NULL) && pMD->HasNativeCode())
    {
        if (addrCode != NULL)
        {
            _ASSERTE(addrCode == (TADDR) pMD->GetFunctionAddress());
        }
        else
        {
            this->m_addrStart  = (TADDR) pMD->GetFunctionAddress();
        }
    }
}
    

bool ShouldTrackJitInfo(CodeHeader* pHeader)
{
    // Don't track JIT info for DynamicMethods.
    if (((MethodDesc *)pHeader->GetMethodDesc())->IsDynamicMethod())
    {
        return false;
    }
    else
    {
        return true;
    }
}


//-----------------------------------------------------------------------------
// Ctor for manager
//-----------------------------------------------------------------------------
DebugInfoManager::DebugInfoManager()
{
    LEAF_CONTRACT;
    // Should only have 1 instance of this. 
    // Caller will set it to this ptr.
    _ASSERTE(g_pDebugInfoStore == NULL);
}

DebugInfoManager::~DebugInfoManager()
{
    LEAF_CONTRACT;
}



//-----------------------------------------------------------------------------
// Based off a request, get the given store.
//-----------------------------------------------------------------------------
IDebugInfoStore * DebugInfoManager::GetStoreForRequest(const DebugInfoRequest & request)
{
    CONTRACTL {
        NOTHROW;
        GC_NOTRIGGER;
    } CONTRACTL_END;

        
    TADDR address = request.GetStartAddress();
    _ASSERTE(address != NULL);
    
    IJitManager* pJitMan = ExecutionManager::FindJitMan((SLOT)address);
    IDebugInfoStore * pStore = NULL;
    pJitMan->GetDebugInfoStore(&pStore);

    return pStore;    
}

//-----------------------------------------------------------------------------
// Impl for DebugInfoManager's IDebugInfoStore
//-----------------------------------------------------------------------------
HRESULT DebugInfoManager::GetVars(
    const DebugInfoRequest & request,
    IN FP_IDS_NEW fpNew, IN void * pNewData,
    OUT ULONG32 * pcVars, 
    OUT ICorDebugInfo::NativeVarInfo ** ppVars)
{
    CONTRACTL
    {
        THROWS;
        WRAPPER(GC_TRIGGERS); // depends on fpNew
        PRECONDITION(CheckPointer(pcVars));
        PRECONDITION(CheckPointer(ppVars));
    }
    CONTRACTL_END;

    *pcVars = 0;
    *ppVars = NULL;
    
    IDebugInfoStore * pStore = NULL;    
    pStore = GetStoreForRequest(request);
    if (pStore == NULL)
    {
        return E_FAIL; // no info available.
    }

    return pStore->GetVars(request, fpNew, pNewData, pcVars, ppVars);
    
}

//-----------------------------------------------------------------------------
// Top level (process-wide) GetBoundaries.
//-----------------------------------------------------------------------------    
HRESULT DebugInfoManager::GetBoundaries(
    const DebugInfoRequest & request,
    IN FP_IDS_NEW fpNew, IN void * pNewData,
    OUT ULONG32 * pcMap, 
    OUT ICorDebugInfo::OffsetMapping ** ppMap)
{
    CONTRACTL
    {
        THROWS;
        WRAPPER(GC_TRIGGERS); // depends on fpNew
        PRECONDITION(CheckPointer(pcMap));
        PRECONDITION(CheckPointer(ppMap));        
    }
    CONTRACTL_END;

    *pcMap = 0;
    *ppMap = NULL;
    
    IDebugInfoStore * pStore = NULL;    
    pStore = GetStoreForRequest(request);
    if (pStore == NULL)
    {
        return E_FAIL; // no info available.
    }
    
    return pStore->GetBoundaries(request, fpNew, pNewData, pcMap, ppMap);
}

#ifdef DACCESS_COMPILE
void DebugInfoManager::EnumMemoryRegionsForMethodDebugInfo(CLRDataEnumMemoryFlags flags, MethodDesc * pMD)
{
    CONTRACTL
    {
        NOTHROW;
        GC_NOTRIGGER;
    }
    CONTRACTL_END;
    
    DebugInfoRequest request;
    TADDR addrCode = (TADDR) pMD->GetFunctionAddress();
    if (addrCode == NULL)
    {
        return;
    }
    request.InitFromStartingAddr(pMD, addrCode);
    
    IDebugInfoStore * pStore = NULL;    
    pStore = GetStoreForRequest(request);
    if (pStore == NULL)
    {
        return; // no info available.
    }

    pStore->EnumMemoryRegionsForMethodDebugInfo(flags, pMD);
}
#endif
