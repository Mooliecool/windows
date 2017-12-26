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


#ifndef __DebugInfoStore_H_
#define __DebugInfoStore_H_

// Debugging information is described in CorInfo.h
#include "corinfo.h"

//-----------------------------------------------------------------------------
// Information to request Debug info.
//-----------------------------------------------------------------------------
class DebugInfoRequest
{
public:
#ifdef _DEBUG
    // Must initialize via an Init*() function, not just a ctor.
    // In debug, ctor sets fields to values that will cause asserts if not initialized.
    DebugInfoRequest() 
    { 
        m_pMD = NULL; 
        m_addrStart = NULL; 
    }
#endif
    // Eventually we may have many ways to initialize a request.
    
    // Init given a method desc and starting address for a native code blob.
    void InitFromStartingAddr(MethodDesc * pDesc, TADDR addrCode);
    

    MethodDesc * GetMD() const { LEAF_CONTRACT; return m_pMD; }
    TADDR GetStartAddress() const { LEAF_CONTRACT; return m_addrStart; }

protected:    
    MethodDesc * m_pMD;
    TADDR        m_addrStart;
    
};

//-----------------------------------------------------------------------------
// A Debug-Info Store abstracts the storage of debugging information
//-----------------------------------------------------------------------------


// We pass the IDS an allocator which it uses to hand the data back.
// pData is data the allocator may use for 'new'. 
// Eg, perhaps we have multiple heaps (eg, loader-heaps per appdomain).
typedef BYTE* (*FP_IDS_NEW)(void * pData, size_t cBytes);   


// 1) An IDebugInfoStore is very related to an IJitManager. The IDS describes the methods
//    owned by the IJM. A single component may implement both interfaces.        
// 2) Caller passes an allocator which these functions use to allocate memory. 
//    This is b/c the store may need to decompress the information just to figure out the size.
// 3) Note that these methods use Uncompressed (Normal) jit data. 
//    Compression is just an implementation detail.
// 4) These throw on OOM (exceptional case), and may return a 
//    failing HR if no data is available (not exceptional)
typedef VPTR(class IDebugInfoStore) PTR_IDebugInfoStore;

class IDebugInfoStore
{
public:
    VPTR_BASE_VTABLE_CLASS(IDebugInfoStore)

    IDebugInfoStore() { }

    virtual HRESULT GetVars(
        const DebugInfoRequest & request,
        IN FP_IDS_NEW fpNew, IN void * pNewData,
        OUT ULONG32 * pcVars, 
        OUT ICorDebugInfo::NativeVarInfo **pVars) = 0;
        
    virtual HRESULT GetBoundaries(
        const DebugInfoRequest & request,
        IN FP_IDS_NEW fpNew, IN void * pNewData,
        OUT ULONG32 * pcMap, 
        OUT ICorDebugInfo::OffsetMapping **pMap) = 0;

#ifdef DACCESS_COMPILE
    // Used when taking a minidump to dump the DebugInfo associated with this method.
    virtual void EnumMemoryRegionsForMethodDebugInfo(CLRDataEnumMemoryFlags flags, MethodDesc * pMD) = 0;
#endif        

};



//-----------------------------------------------------------------------------
// Utility routines used for compression
// Note that the compression is just an implementation detail of the stores,
// and so these are just utility routines exposed to the stores.
//-----------------------------------------------------------------------------
class CompressDebugInfo
{
public:
    // Abstract storage for holding the compressed versions.
    // These contain allocated memory from FPNEW.
    typedef DPTR(struct MyBuffer) PTR_MyBuffer;
    class MyBuffer
    {
    public:
        MyBuffer() { LEAF_CONTRACT; m_pData = NULL; m_cBytes = 0; }
        

        // Deserialize.
        void Deserialize(TADDR addr, ULONG32 cBytes) 
        { 
            LEAF_CONTRACT; 
        
            m_pData  = addr; 
            m_cBytes = cBytes;
        }

        
        // Get local copy of the buffer.
        BYTE * GetBuffer();
        
        // Get size of just the buffer
        size_t GetSize();

#ifdef DACCESS_COMPILE
        // Used when taking a minidump to dump the DebugInfo associated with this method.
        void EnumMemoryRegions(CLRDataEnumMemoryFlags flags);
#endif        

    protected:        
        // Array of bytes.
        TADDR m_pData;

        ULONG32 m_cBytes;
    };

    typedef MyBuffer Vars;
    typedef MyBuffer Boundaries;

#ifndef DACCESS_COMPILE   
    // Compress incoming data and write it to the provided SBuffer.
    static void CompressBoundaries(
        IN ULONG32                       cMap,
        IN ICorDebugInfo::OffsetMapping *pMap,
        IN OUT SBuffer * pBuffer
    );

    static void CompressVars(
        IN ULONG32                         cVars,
        IN ICorDebugInfo::NativeVarInfo    *vars,
        IN OUT SBuffer * pBuffer
    );

private:    
    // Helpers for compression routines.
    
    // Provide a conservative estimate for the size of the compression
    // returns the number of bytes. The compression routines can't accurately predict
    // the exact number of bytes needed w/o actually compressing it (since we may use a variable length compression).
    // Caller uses this to allocate an array and then passes that into CompressBoundaries.
    static ULONG32 EstimateBoundCompressionSize(
        IN ULONG32                       cMap,
        IN ICorDebugInfo::OffsetMapping *pMap
    );

    // Given a set of boundaries, compress it.
    // This will fill in a pre-allocated buffer and return a pointer to it.
    // On input, *pcBytes is the size of the buffer passed in,
    // on output, it's the size of the buffer actually used.
    static void CompressBoundaries(
        IN ULONG32                       cMap,
        IN ICorDebugInfo::OffsetMapping *pMap,
        IN OUT ULONG32 * pcBytes,
        OUT BYTE * pBuffer        
    );


    // Compress Vars.
    static ULONG32 EstimateVarCompressionSize(
        IN ULONG32                         cVars,
        IN ICorDebugInfo::NativeVarInfo    *vars
    );
    
    static void CompressVars(
        IN ULONG32                         cVars,
        IN ICorDebugInfo::NativeVarInfo    *vars,
        IN OUT ULONG32 * pcBytes,
        OUT BYTE * pBuffer
    );
#endif    

public:
    // Uncompress data supplied by Compress functions.
    static void RestoreBoundaries(
        IN FP_IDS_NEW fpNew, IN void * pNewData,
        IN CompressDebugInfo::Boundaries * pCompressBoundsInfo,
        OUT ULONG32                       * pcMap, // number of entries in ppMap
        OUT ICorDebugInfo::OffsetMapping **pMap // pointer to newly allocated array
    ); 

    static ULONG32 GetNumBounds(IN CompressDebugInfo::Boundaries * pCompressBoundsInfo);
    
    static void RestoreVars(
        IN FP_IDS_NEW fpNew, IN void * pNewData,
        IN CompressDebugInfo::Vars * pCompressVarsInfo,
        OUT ULONG32                         *cVars,
        OUT ICorDebugInfo::NativeVarInfo    **vars 
    );

    static ULONG32 GetNumVars(IN CompressDebugInfo::Vars * pCompressVarsInfo);
   
};

bool ShouldTrackJitInfo(CodeHeader* pHeader);


//-----------------------------------------------------------------------------
// Debug-Info-manager. This is like a process-wide store.
// There should be only 1 instance of this and it's process-wide.
// It will delegate to sub-stores as needed
//-----------------------------------------------------------------------------
class DebugInfoManager : public IDebugInfoStore
{
    VPTR_VTABLE_CLASS(DebugInfoManager, IDebugInfoStore)
public:
    DebugInfoManager();    
    ~DebugInfoManager();

//.............................................................................
// Implementation for IDebugInfoStore
//.............................................................................
    virtual HRESULT GetVars(
        const DebugInfoRequest & request,
        IN FP_IDS_NEW fpNew, IN void * pNewData,
        OUT ULONG32 * pcVars, 
        OUT ICorDebugInfo::NativeVarInfo ** ppVars);
        
    virtual HRESULT GetBoundaries(
        const DebugInfoRequest & request,
        IN FP_IDS_NEW fpNew, IN void * pNewData,
        OUT ULONG32 * pcMap, 
        OUT ICorDebugInfo::OffsetMapping ** ppMap);    

#ifdef DACCESS_COMPILE
    virtual void EnumMemoryRegionsForMethodDebugInfo(CLRDataEnumMemoryFlags flags, MethodDesc * pMD);
#endif

protected:
    IDebugInfoStore * GetStoreForRequest(const DebugInfoRequest & request);
};



#endif // __DebugInfoStore_H_
