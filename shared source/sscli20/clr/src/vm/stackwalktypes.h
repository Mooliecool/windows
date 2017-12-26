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
// ============================================================================
// File: stackwalktypes.h
//
// ============================================================================
// Contains types used by stackwalk.h.

#ifndef __STACKWALKTYPES_H__
#define __STACKWALKTYPES_H__

class CrawlFrame;

struct _METHODTOKEN;
typedef struct _METHODTOKEN * METHODTOKEN;
struct StackwalkCacheEntry;

//************************************************************************
// Stack walking
//************************************************************************
enum StackCrawlMark
{
    LookForMe = 0,
    LookForMyCaller = 1,
    LookForMyCallersCaller = 2,
    LookForThread = 3
};

enum StackWalkAction 
{
    SWA_CONTINUE    = 0,    // continue walking
    SWA_ABORT       = 1,    // stop walking, early out in "failure case"
    SWA_FAILED      = 2     // couldn't walk stack
};

#define SWA_DONE SWA_CONTINUE


// Pointer to the StackWalk callback function.
typedef StackWalkAction (*PSTACKWALKFRAMESCALLBACK)(
    CrawlFrame       *pCF,      //
    VOID*             pData     // Caller's private data

);

/******************************************************************************
   StackwalkCache: new class implements stackwalk perf optimization features.
   StackwalkCacheEntry array: very simple per thread hash table, keeping cached data.
   StackwalkCacheUnwindInfo: used by EECodeManager::UnwindStackFrame to return
   stackwalk cache flags.
   Cf. Ilyakoz for any questions.
*/

struct StackwalkCacheUnwindInfo
{
    size_t securityObjectOffset;    // offset of SecurityObject. 0 if there is no security object
    BOOL fUseEbp;                   // Is EBP modified by the method - either for a frame-pointer or for a scratch-register?
    BOOL fUseEbpAsFrameReg;         // use EBP as the frame pointer?

    inline StackwalkCacheUnwindInfo() { ZeroMemory(this, sizeof(StackwalkCacheUnwindInfo)); }
    StackwalkCacheUnwindInfo(StackwalkCacheEntry * pCacheEntry);
};

//************************************************************************

    #define STACKWALK_CACHE_ENTRY_ALIGN_BOUNDARY 0x8

DECLSPEC_ALIGN(STACKWALK_CACHE_ENTRY_ALIGN_BOUNDARY) 
struct StackwalkCacheEntry
{
    //
    //  don't rearrange the fields, so that invalid value 0x8000000000000000 will never appear
    //  as StackwalkCacheEntry, it's required for atomicMOVQ using FILD/FISTP instructions
    //
    UINT_PTR IP;
    WORD ESPOffset:15;          // stack offset (frame size + pending arguments + etc)
    WORD securityObjectOffset:3;// offset of SecurityObject. 0 if there is no security object
    WORD fUseEbp:1;             // For ESP methods, is EBP touched at all?
    WORD fUseEbpAsFrameReg:1;   // use EBP as the frame register?
    WORD argSize:11;            // size of args pushed on stack

    // Initialize IP to 0, which indicates an empty entry.
    inline StackwalkCacheEntry()
    {
        IP = 0;
    }

    inline BOOL Init(UINT_PTR   IP,
                     UINT_PTR   SPOffset,
                     StackwalkCacheUnwindInfo *pUnwindInfo,
                     UINT_PTR   argSize)
    {
        LEAF_CONTRACT;

        this->IP              = IP;

#if defined(_X86_)
        this->ESPOffset         = SPOffset;
        this->argSize           = argSize;
        
        this->securityObjectOffset = (WORD)pUnwindInfo->securityObjectOffset;
        _ASSERTE(this->securityObjectOffset == pUnwindInfo->securityObjectOffset);
        
        this->fUseEbp           = pUnwindInfo->fUseEbp;
        this->fUseEbpAsFrameReg = pUnwindInfo->fUseEbpAsFrameReg;
        _ASSERTE(!fUseEbpAsFrameReg || fUseEbp);

        // return success if we fit SPOffset and argSize into
        return ((this->ESPOffset == SPOffset) && 
                (this->argSize == argSize));
#else  // !_X86_ && !_AMD64_
        return FALSE;
#endif // !_X86_ && !_AMD64_
    }

    inline BOOL HasSecurityObject()
    {
        LEAF_CONTRACT;

#if defined(_X86_)
        return securityObjectOffset != 0;
#else  // !_X86_
        // On AMD64 we don't save anything by grabbing the security object before it is needed.  This is because
        // we need to crack the GC info in order to find the security object, and to unwind we only need to
        // crack the unwind info.
        return FALSE;
#endif // !_X86_
    }

    inline BOOL IsSafeToUseCache()
    {
        LEAF_CONTRACT;

#if defined(_X86_)
        return (!fUseEbp || fUseEbpAsFrameReg);
#else  // !_X86_ && !_AMD64_
        return FALSE;
#endif // !_X86_ && !_AMD64_
    }
};

#if defined(_X86_) || defined(_AMD64_)
C_ASSERT(sizeof(StackwalkCacheEntry) == 2 * sizeof(UINT_PTR));
#endif // _X86_ || _AMD64_

//************************************************************************

class StackwalkCache 
{
    public:
        BOOL Lookup(UINT_PTR IP);
        void Insert(StackwalkCacheEntry *pCacheEntry);
        inline void ClearEntry () { LEAF_CONTRACT; m_CacheEntry.IP = 0; }
        inline BOOL Enabled() { LEAF_CONTRACT;  return s_Enabled; };
        inline BOOL IsEmpty () { LEAF_CONTRACT;  return m_CacheEntry.IP == 0; }

#ifndef DACCESS_COMPILE
        StackwalkCache();
#endif
        static void Init();

        StackwalkCacheEntry m_CacheEntry; // local copy of Global Cache entry for current IP
        
        static void Invalidate(AppDomain * pDomain);
        
    private:
        unsigned GetKey(UINT_PTR IP);
        
        static BOOL s_Enabled;
};

//************************************************************************

inline StackwalkCacheUnwindInfo::StackwalkCacheUnwindInfo(StackwalkCacheEntry * pCacheEntry) 
{
    LEAF_CONTRACT;
    
    securityObjectOffset = pCacheEntry->securityObjectOffset;
    fUseEbp = pCacheEntry->fUseEbp;
    fUseEbpAsFrameReg = pCacheEntry->fUseEbpAsFrameReg;
}

#endif  // __STACKWALKTYPES_H__
