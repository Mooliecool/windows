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
// ===========================================================================
// File: UTIL.CPP
//
//
// ===========================================================================

#include "common.h"
#include "excep.h"
#include "corhost.h"
#include "eventtrace.h"
#include "timeline.h"
#include "posterror.h"
#include "eemessagebox.h"


#include <winbase.h>
#define MESSAGE_LENGTH       1024

#ifndef DACCESS_COMPILE

// Helper function that encapsulates the parsing rules.
//
// Called first with *pdstout == NULL to figure out how many args there are
// and the size of the required destination buffer.
//
// Called again with a nonnull *pdstout to fill in the actual buffer.
//
// Returns the # of arguments.
static UINT ParseCommandLine(LPCWSTR psrc, __out LPWSTR *pdstout)
{
    CONTRACTL
    {
        NOTHROW;
        GC_NOTRIGGER;
        FORBID_FAULT;
    }
    CONTRACTL_END

    UINT    argcount = 1;       // discovery of arg0 is unconditional, below
    LPWSTR  pdst     = *pdstout;
    BOOL    fDoWrite = (pdst != NULL);

    BOOL    fInQuotes;
    int     iSlash;

    /* A quoted program name is handled here. The handling is much
       simpler than for other arguments. Basically, whatever lies
       between the leading double-quote and next one, or a terminal null
       character is simply accepted. Fancier handling is not required
       because the program name must be a legal NTFS/HPFS file name.
       Note that the double-quote characters are not copied, nor do they
       contribute to numchars.
         
       This "simplification" is necessary for compatibility reasons even
       though it leads to mishandling of certain cases.  For example,
       "c:\tests\"test.exe will result in an arg0 of c:\tests\ and an
       arg1 of test.exe.  In any rational world this is incorrect, but
       we need to preserve compatibility.
    */

    LPCWSTR pStart = psrc;
    BOOL    skipQuote = FALSE;

    if (*psrc == L'\"')
    {
        // scan from just past the first double-quote through the next
        // double-quote, or up to a null, whichever comes first
        while ((*(++psrc) != L'\"') && (*psrc != L'\0'))
            continue;

        skipQuote = TRUE;
    }
    else
    {
        /* Not a quoted program name */

        while (!ISWWHITE(*psrc) && *psrc != L'\0')
            psrc++;
    }

    // We have now identified arg0 as pStart (or pStart+1 if we have a leading
    // quote) through psrc-1 inclusive
    if (skipQuote)
        pStart++;
    while (pStart < psrc)
    {
        if (fDoWrite)
            *pdst = *pStart;

        pStart++;
        pdst++;
    }

    // And terminate it.
    if (fDoWrite)
        *pdst = L'\0';

    pdst++;

    // if we stopped on a double-quote when arg0 is quoted, skip over it
    if (skipQuote && *psrc == L'\"')
        psrc++;

    while ( *psrc != L'\0')
    {
LEADINGWHITE:

        // The outofarg state.
        while (ISWWHITE(*psrc))
            psrc++;

        if (*psrc == L'\0')
            break;
        else
        if (*psrc == L'#')
        {
            while (*psrc != L'\0' && *psrc != L'\n')
                psrc++;     // skip to end of line

            goto LEADINGWHITE;
        }

        argcount++;
        fInQuotes = FALSE;

        while ((!ISWWHITE(*psrc) || fInQuotes) && *psrc != L'\0')
        {
            switch (*psrc)
            {
            case L'\\':
                iSlash = 0;
                while (*psrc == L'\\')
                {
                    iSlash++;
                    psrc++;
                }

                if (*psrc == L'\"')
                {
                    for ( ; iSlash >= 2; iSlash -= 2)
                    {
                        if (fDoWrite)
                            *pdst = L'\\';

                        pdst++;
                    }

                    if (iSlash & 1)
                    {
                        if (fDoWrite)
                            *pdst = *psrc;

                        psrc++;
                        pdst++;
                    }
                    else
                    {
                        fInQuotes = !fInQuotes;
                        psrc++;
                    }
                }
                else
                    for ( ; iSlash > 0; iSlash--)
                    {
                        if (fDoWrite)
                            *pdst = L'\\';

                        pdst++;
                    }

                break;

            case L'\"':
                fInQuotes = !fInQuotes;
                psrc++;
                break;

            default:
                if (fDoWrite)
                    *pdst = *psrc;

                psrc++;
                pdst++;
            }
        }

        if (fDoWrite)
            *pdst = L'\0';

        pdst++;
    }


    _ASSERTE(*psrc == L'\0');
    *pdstout = pdst;
    return argcount;
}


// Function to parse apart a command line and return the 
// arguments just like argv and argc
// This function is a little funky because of the pointer work
// but it is neat because it allows the recipient of the char**
// to only have to do a single delete []
LPWSTR* CommandLineToArgvW(__in LPWSTR lpCmdLine, DWORD *pNumArgs)
{

    CONTRACTL
    {
        NOTHROW;
        GC_NOTRIGGER;
        INJECT_FAULT(return NULL;); 
    }
    CONTRACTL_END

    DWORD argcount = 0;
    LPWSTR retval = NULL;
    LPWSTR *pslot;
    // First we need to find out how many strings there are in the command line
    _ASSERTE(lpCmdLine);
    _ASSERTE(pNumArgs);

    LPWSTR pdst = NULL;
    argcount = ParseCommandLine(lpCmdLine, &pdst);

    // This check is because on WinCE the Application Name is not passed in as an argument to the app!
    if (argcount == 0)
    {
        *pNumArgs = 0;
        return NULL;
    }

    // Now we need alloc a buffer the size of the command line + the number of strings * DWORD
    retval = new (nothrow) WCHAR[(argcount*sizeof(WCHAR*))/sizeof(WCHAR) + (pdst - (LPWSTR)NULL)];
    if(!retval)
        return NULL;

    pdst = (LPWSTR)( argcount*sizeof(LPWSTR*) + (BYTE*)retval );
    ParseCommandLine(lpCmdLine, &pdst);
    pdst = (LPWSTR)( argcount*sizeof(LPWSTR*) + (BYTE*)retval );
    pslot = (LPWSTR*)retval;
    for (DWORD i = 0; i < argcount; i++)
    {
        *(pslot++) = pdst;
        while (*pdst != L'\0')
        {
            pdst++;
        }
        pdst++;
    }

    

    *pNumArgs = argcount;
    return (LPWSTR*)retval;

}




//************************************************************************
// CQuickHeap
//
// A fast non-multithread-safe heap for short term use.
// Destroying the heap frees all blocks allocated from the heap.
// Blocks cannot be freed individually.
//
// The heap uses COM+ exceptions to report errors.
//
// The heap does not use any internal synchronization so it is not
// multithreadsafe.
//************************************************************************
CQuickHeap::CQuickHeap()
{
    LEAF_CONTRACT;

    m_pFirstQuickBlock    = NULL;
    m_pFirstBigQuickBlock = NULL;
    m_pNextFree           = NULL;
}

CQuickHeap::~CQuickHeap()
{
    CONTRACTL
    {
        NOTHROW;
        GC_NOTRIGGER;
        FORBID_FAULT;
    }
    CONTRACTL_END

    QuickBlock *pQuickBlock = m_pFirstQuickBlock;
    while (pQuickBlock) {
        QuickBlock *ptmp = pQuickBlock;
        pQuickBlock = pQuickBlock->m_next;
        delete [] (BYTE*)ptmp;
    }

    pQuickBlock = m_pFirstBigQuickBlock;
    while (pQuickBlock) {
        QuickBlock *ptmp = pQuickBlock;
        pQuickBlock = pQuickBlock->m_next;
        delete [] (BYTE*)ptmp;
    }
}




LPVOID CQuickHeap::Alloc(UINT sz)
{
    CONTRACTL
    {
        THROWS;
        GC_NOTRIGGER;
        SO_TOLERANT;    // So long as we cleanup the heap when we're done, all the memory goes with it
        INJECT_FAULT(COMPlusThrowOM(););
    } CONTRACTL_END;

    sz = (sz+7) & ~7;

    if ( sz > kBlockSize ) {

        QuickBlock *pQuickBigBlock = (QuickBlock*) new BYTE[sz + sizeof(QuickBlock) - 1];
        pQuickBigBlock->m_next = m_pFirstBigQuickBlock;
        m_pFirstBigQuickBlock = pQuickBigBlock;

        return pQuickBigBlock->m_bytes;


    } else {
        if (m_pNextFree == NULL || sz > (UINT)( &(m_pFirstQuickBlock->m_bytes[kBlockSize]) - m_pNextFree )) {
            QuickBlock *pQuickBlock = (QuickBlock*) new BYTE[kBlockSize + sizeof(QuickBlock) - 1];
            pQuickBlock->m_next = m_pFirstQuickBlock;
            m_pFirstQuickBlock = pQuickBlock;
            m_pNextFree = pQuickBlock->m_bytes;
        }
        LPVOID pv = m_pNextFree;
        m_pNextFree += sz;
        return pv;
    }
}

//----------------------------------------------------------------------------
//
// ReserveAlignedMemory - Reserves aligned address space.
//
// This routine assumes it is passed reasonable align and size values.
// Not much error checking is performed...
//
// NOTE: This routine uses a static which is not synchronized.  This is OK.
//
//----------------------------------------------------------------------------

LPVOID _ReserveAlignedMemoryWorker(LPVOID lpvAddr, LPVOID lpvTop, DWORD dwAlign, DWORD dwSize)
{
    CONTRACTL
    {
        NOTHROW;
        GC_NOTRIGGER;
        INJECT_FAULT(return NULL;);
    }
    CONTRACTL_END

    // precompute some alignment helpers
    size_t dwAlignRound = dwAlign - 1;
    size_t dwAlignMask = ~dwAlignRound;

    // align the address
    lpvAddr = (LPVOID)(((size_t)lpvAddr + dwAlignRound) & dwAlignMask);

    while (lpvAddr < lpvTop)
    {
        MEMORY_BASIC_INFORMATION mbe;

        // query the region's charactersitics
        if (!ClrVirtualQuery((LPCVOID)lpvAddr, &mbe, sizeof(mbe)))
            break;

        // see if this is a suitable region - if so then try to grab it
        // on FEATURE_PAL, ClrVirtualQuery returns mbe.RegionSize == 0 if the size of the region is unknown
        if ((lpvAddr != 0) && (mbe.State == MEM_FREE) && ((mbe.RegionSize >= dwSize) || (mbe.RegionSize == 0)))
        {
            void *lpvRet = ClrVirtualAlloc(lpvAddr, dwSize, MEM_RESERVE, PAGE_NOACCESS);
            if (lpvRet) {
                // ok we got it
                return lpvRet;
            }
        }

        // skip ahead to the next region
        LPVOID lpNext = (BYTE*)mbe.BaseAddress + mbe.RegionSize;

        // align the address
        lpNext = (LPVOID)(((size_t)lpNext + dwAlignRound) & dwAlignMask);

        // keep going forward
        if (lpNext == lpvAddr)
            lpNext = (BYTE*)lpvAddr + dwAlign;

        // check for overflow
        if (lpNext < lpvAddr)
            break;

        lpvAddr = lpNext;
    }

    return 0;
}

LPCVOID ReserveAlignedMemory(DWORD dwAlign, DWORD dwSize)
{
    CONTRACTL
    {
        NOTHROW;
        GC_NOTRIGGER;
        INJECT_FAULT(return NULL;);
    }
    CONTRACTL_END

    // preinit our hint address to just after the start of the NULL region
    static LPVOID s_lpvAddrHint = NULL;

    LPVOID lpvAddrHint = s_lpvAddrHint;    
    if (lpvAddrHint == NULL)
        lpvAddrHint = BOT_MEMORY;

    _ASSERTE(dwAlign >= OS_PAGE_SIZE);

    // scan the address space from our hint point to the top
    LPVOID lpvAddr = _ReserveAlignedMemoryWorker(lpvAddrHint, TOP_MEMORY, dwAlign, dwSize);

    // if that failed then scan from the bottom up to our hint point
    if (!lpvAddr)
        lpvAddr = _ReserveAlignedMemoryWorker(BOT_MEMORY, lpvAddrHint, dwAlign, dwSize);

    // update the hint to one byte after dwAddr (which may be zero)
    s_lpvAddrHint = (LPVOID)(((size_t)lpvAddr) + dwSize);

    // return the base address of the memory we reserved
    return (LPCVOID)lpvAddr;
}




//----------------------------------------------------------------------------
// Output functions that avoid the crt's.
//----------------------------------------------------------------------------

static
void NPrintToHandleA(HANDLE Handle, const char *pszString, size_t BytesToWrite)
{
    CONTRACTL
    {
        NOTHROW;
        GC_NOTRIGGER;
        FORBID_FAULT;
    }
    CONTRACTL_END

    if (Handle == INVALID_HANDLE_VALUE || Handle == NULL)
        return;

    BOOL success;
    DWORD   dwBytesWritten;
    const size_t maxWriteFileSize = 32767;

    while (BytesToWrite > 0) {
        DWORD dwChunkToWrite = (DWORD) min(BytesToWrite, maxWriteFileSize);
        if (dwChunkToWrite < BytesToWrite) {
            // must go by char to find biggest string that will fit, taking DBCS chars into account
            dwChunkToWrite = 0;
            const char *charNext = pszString;
            while (dwChunkToWrite < maxWriteFileSize-2 && charNext) {
                charNext = CharNextExA(0, pszString+dwChunkToWrite, 0);
                dwChunkToWrite = (DWORD)(charNext - pszString);
            }
            if (dwChunkToWrite == 0)
                break;
        }
        
        success = WriteFile(Handle, pszString, dwChunkToWrite, &dwBytesWritten, NULL);
        if (!success)
        {
#ifdef _DEBUG
            // This can happen if stdout is a closed pipe.  This might not help
            // much, but we'll have half a chance of seeing this.
            OutputDebugStringA("CLR: Writing out an unhandled exception to stdout failed!\n");
            OutputDebugStringA(pszString);
#endif //_DEBUG
            break;
        }
        else {
            _ASSERTE(dwBytesWritten == dwChunkToWrite);
        }
        pszString = pszString + dwChunkToWrite;
        BytesToWrite -= dwChunkToWrite;
    }

}

static 
void PrintToHandleA(HANDLE Handle, const char *pszString)
{
    CONTRACTL
    {
        NOTHROW;
        GC_NOTRIGGER;
        FORBID_FAULT;
    }
    CONTRACTL_END

    size_t len = strlen(pszString);
    NPrintToHandleA(Handle, pszString, len);
}

void PrintToStdOutA(const char *pszString) {
    CONTRACTL
    {
        NOTHROW;
        GC_NOTRIGGER;
        FORBID_FAULT;
    }
    CONTRACTL_END

    HANDLE  Handle = GetStdHandle(STD_OUTPUT_HANDLE);
    PrintToHandleA(Handle, pszString);
}


void PrintToStdOutW(const WCHAR *pwzString)
{
    CONTRACTL
    {
        THROWS;
        GC_NOTRIGGER;
        INJECT_FAULT(COMPlusThrowOM(););
    }
    CONTRACTL_END

    MAKE_MULTIBYTE_FROMWIDE_BESTFIT(pStr, pwzString, GetConsoleOutputCP());

    PrintToStdOutA(pStr);
}

void PrintToStdErrA(const char *pszString) {
    CONTRACTL
    {
        NOTHROW;
        GC_NOTRIGGER;
        FORBID_FAULT;
    }
    CONTRACTL_END

    HANDLE  Handle = GetStdHandle(STD_ERROR_HANDLE);
    PrintToHandleA(Handle, pszString);
}


void PrintToStdErrW(const WCHAR *pwzString)
{
    CONTRACTL
    {
        THROWS;
        GC_NOTRIGGER;
        INJECT_FAULT(COMPlusThrowOM(););
    }
    CONTRACTL_END

    MAKE_MULTIBYTE_FROMWIDE_BESTFIT(pStr, pwzString, GetConsoleOutputCP());

    PrintToStdErrA(pStr);
}



void NPrintToStdOutA(const char *pszString, size_t nbytes) {
    CONTRACTL
    {
        NOTHROW;
        GC_NOTRIGGER;
        FORBID_FAULT;
    }
    CONTRACTL_END

    HANDLE  Handle = GetStdHandle(STD_OUTPUT_HANDLE);
    NPrintToHandleA(Handle, pszString, nbytes);
}


void NPrintToStdOutW(const WCHAR *pwzString, size_t nchars)
{
    CONTRACTL
    {
        THROWS;
        GC_NOTRIGGER;
        INJECT_FAULT(COMPlusThrowOM(););
    }
    CONTRACTL_END

    LPSTR pStr;
    MAKE_MULTIBYTE_FROMWIDEN_BESTFIT(pStr, pwzString, (int)nchars, nbytes, GetConsoleOutputCP());

    NPrintToStdOutA(pStr, nbytes);
}

void NPrintToStdErrA(const char *pszString, size_t nbytes) {
    CONTRACTL
    {
        NOTHROW;
        GC_NOTRIGGER;
        FORBID_FAULT;
    }
    CONTRACTL_END

    HANDLE  Handle = GetStdHandle(STD_ERROR_HANDLE);
    NPrintToHandleA(Handle, pszString, nbytes);
}


void NPrintToStdErrW(const WCHAR *pwzString, size_t nchars)
{
    CONTRACTL
    {
        THROWS;
        GC_NOTRIGGER;
        INJECT_FAULT(COMPlusThrowOM(););
    }
    CONTRACTL_END

    LPSTR pStr;

    MAKE_MULTIBYTE_FROMWIDEN_BESTFIT(pStr, pwzString, (int)nchars, nbytes, GetConsoleOutputCP());

    NPrintToStdErrA(pStr, nbytes);
}
//----------------------------------------------------------------------------





//+--------------------------------------------------------------------------
//
//  Function:   VMDebugOutputA( . . . . )
//              VMDebugOutputW( . . . . )
//  
//  Synopsis:   Output a message formatted in printf fashion to the debugger.
//              ANSI and wide character versions are both provided.  Only 
//              present in debug builds (i.e. when _DEBUG is defined).
//
//  Arguments:  [format]     ---   ANSI or Wide character format string
//                                 in printf/OutputDebugString-style format.
// 
//              [ ... ]      ---   Variable length argument list compatible
//                                 with the format string.
//
//  Returns:    Nothing.
//  Notes:      Has internal static sized character buffer of 
//              width specified by the preprocessor constant DEBUGOUT_BUFSIZE.
//
//---------------------------------------------------------------------------
#ifdef _DEBUG

#define DEBUGOUT_BUFSIZE 1024

void __cdecl VMDebugOutputA(__in LPSTR format, ...)
{
    STATIC_CONTRACT_NOTHROW;
    STATIC_CONTRACT_GC_NOTRIGGER;
    STATIC_CONTRACT_FORBID_FAULT;

    va_list     argPtr;
    va_start(argPtr, format);

    char szBuffer[DEBUGOUT_BUFSIZE];

    if(vsprintf_s(szBuffer, DEBUGOUT_BUFSIZE-1, format, argPtr) > 0)
        OutputDebugStringA(szBuffer);
    va_end(argPtr);
}

void __cdecl VMDebugOutputW(__in LPWSTR format, ...)
{
    STATIC_CONTRACT_NOTHROW;
    STATIC_CONTRACT_GC_NOTRIGGER;
    STATIC_CONTRACT_FORBID_FAULT;
    STATIC_CONTRACT_DEBUG_ONLY;

    va_list     argPtr;
    va_start(argPtr, format);
    
    WCHAR wszBuffer[DEBUGOUT_BUFSIZE];

    if(vswprintf_s(wszBuffer, DEBUGOUT_BUFSIZE-2, format, argPtr) > 0)
        WszOutputDebugString(wszBuffer);
    va_end(argPtr);
}

#endif   // #ifdef DACCESS_COMPILE

//*****************************************************************************
// Compare VarLoc's
//*****************************************************************************

bool operator ==(const ICorDebugInfo::VarLoc &varLoc1,
                 const ICorDebugInfo::VarLoc &varLoc2)
{
    STATIC_CONTRACT_NOTHROW;
    STATIC_CONTRACT_GC_NOTRIGGER;
    STATIC_CONTRACT_FORBID_FAULT;

    if (varLoc1.vlType != varLoc2.vlType)
        return false;

    switch(varLoc1.vlType)
    {
    case ICorDebugInfo::VLT_REG:
    case ICorDebugInfo::VLT_REG_BYREF:
        return varLoc1.vlReg.vlrReg == varLoc2.vlReg.vlrReg;

    case ICorDebugInfo::VLT_STK: 
    case ICorDebugInfo::VLT_STK_BYREF: 
        return varLoc1.vlStk.vlsBaseReg == varLoc2.vlStk.vlsBaseReg &&
               varLoc1.vlStk.vlsOffset  == varLoc2.vlStk.vlsOffset;

    case ICorDebugInfo::VLT_REG_REG:
        return varLoc1.vlRegReg.vlrrReg1 == varLoc2.vlRegReg.vlrrReg1 &&
               varLoc1.vlRegReg.vlrrReg2 == varLoc2.vlRegReg.vlrrReg2;

    case ICorDebugInfo::VLT_REG_STK:
        return varLoc1.vlRegStk.vlrsReg == varLoc2.vlRegStk.vlrsReg &&
               varLoc1.vlRegStk.vlrsStk.vlrssBaseReg == varLoc2.vlRegStk.vlrsStk.vlrssBaseReg &&
               varLoc1.vlRegStk.vlrsStk.vlrssOffset == varLoc2.vlRegStk.vlrsStk.vlrssOffset;

    case ICorDebugInfo::VLT_STK_REG:
        return varLoc1.vlStkReg.vlsrStk.vlsrsBaseReg == varLoc2.vlStkReg.vlsrStk.vlsrsBaseReg &&
               varLoc1.vlStkReg.vlsrStk.vlsrsOffset == varLoc2.vlStkReg.vlsrStk.vlsrsBaseReg &&
               varLoc1.vlStkReg.vlsrReg == varLoc2.vlStkReg.vlsrReg;

    case ICorDebugInfo::VLT_STK2:
        return varLoc1.vlStk2.vls2BaseReg == varLoc1.vlStk2.vls2BaseReg &&
               varLoc1.vlStk2.vls2Offset == varLoc1.vlStk2.vls2Offset;

    case ICorDebugInfo::VLT_FPSTK:
        return varLoc1.vlFPstk.vlfReg == varLoc1.vlFPstk.vlfReg;

    default:
        _ASSERTE(!"Bad vlType"); return false;
    }
}

#endif // #ifndef DACCESS_COMPILE

//*****************************************************************************
// The following are used to read and write data given NativeVarInfo
// for primitive types. For ValueClasses, FALSE will be returned.
//*****************************************************************************

SIZE_T GetRegOffsInCONTEXT(ICorDebugInfo::RegNum regNum
                           IA64_ARG(ICorDebugInfo::VarLocType type = ICorDebugInfo::VLT_REG)
                           IA64_ARG(CONTEXT *pCtx = NULL))
{
    STATIC_CONTRACT_NOTHROW;
    STATIC_CONTRACT_GC_NOTRIGGER;
    STATIC_CONTRACT_FORBID_FAULT;

#ifdef _X86_
    switch(regNum)
    {
    case ICorDebugInfo::REGNUM_EAX: return offsetof(CONTEXT,Eax);
    case ICorDebugInfo::REGNUM_ECX: return offsetof(CONTEXT,Ecx);
    case ICorDebugInfo::REGNUM_EDX: return offsetof(CONTEXT,Edx);
    case ICorDebugInfo::REGNUM_EBX: return offsetof(CONTEXT,Ebx);
    case ICorDebugInfo::REGNUM_ESP: 
    case ICorDebugInfo::REGNUM_AMBIENT_SP: 
                                    return offsetof(CONTEXT,Esp);
    case ICorDebugInfo::REGNUM_EBP: return offsetof(CONTEXT,Ebp);
    case ICorDebugInfo::REGNUM_ESI: return offsetof(CONTEXT,Esi);
    case ICorDebugInfo::REGNUM_EDI: return offsetof(CONTEXT,Edi);
    default: _ASSERTE(!"Bad regNum"); return (SIZE_T) -1;
    }
#elif defined(_PPC_)
    switch(regNum)
    {
    case ICorDebugInfo::REGNUM_R1: return offsetof(CONTEXT,Gpr1);
    case ICorDebugInfo::REGNUM_R3: return offsetof(CONTEXT,Gpr3);
    case ICorDebugInfo::REGNUM_R4: return offsetof(CONTEXT,Gpr4);
    case ICorDebugInfo::REGNUM_R5: return offsetof(CONTEXT,Gpr5);
    case ICorDebugInfo::REGNUM_R6: return offsetof(CONTEXT,Gpr6);
    case ICorDebugInfo::REGNUM_R7: return offsetof(CONTEXT,Gpr7);
    case ICorDebugInfo::REGNUM_R8: return offsetof(CONTEXT,Gpr8);
    case ICorDebugInfo::REGNUM_R9: return offsetof(CONTEXT,Gpr9);
    case ICorDebugInfo::REGNUM_R10: return offsetof(CONTEXT,Gpr10);
    case ICorDebugInfo::REGNUM_R30: return offsetof(CONTEXT,Gpr30);
    default: _ASSERTE(!"Bad regNum"); return (SIZE_T) -1;
    }
#else
    PORTABILITY_ASSERT("GetRegOffsInCONTEXT is not implemented on this platform.");
    return (SIZE_T) -1;
#endif  // _X86_
}

SIZE_T DereferenceByRefVar(SIZE_T addr)
{
    STATIC_CONTRACT_WRAPPER;

    SIZE_T result = NULL;

#if defined(DACCESS_COMPILE)
    HRESULT hr = DacReadAll(addr, &result, sizeof(result), false);
    if (FAILED(hr))
    {
        result = NULL;
    }

#else  // !DACCESS_COMPILE
    EX_TRY
    {
        AVInRuntimeImplOkayHolder AVOkay(TRUE);

        result = *(SIZE_T*)addr;
    }
    EX_CATCH
    {
    }
    EX_END_CATCH(SwallowAllExceptions);

#endif // !DACCESS_COMPILE

    return result;
}

// How are errors communicated to the caller?
ULONG NativeVarLocations(const ICorDebugInfo::VarLoc &   varLoc,
                         PCONTEXT                        pCtx,
                         ULONG                           numLocs,
                         NativeVarLocation*              locs)
{
    STATIC_CONTRACT_NOTHROW;
    STATIC_CONTRACT_GC_NOTRIGGER;
    STATIC_CONTRACT_FORBID_FAULT;

    _ASSERTE(numLocs >= MAX_NATIVE_VAR_LOCS);

    bool fByRef = false;
    switch(varLoc.vlType)
    {
        SIZE_T regOffs;
        TADDR  baseReg;

    case ICorDebugInfo::VLT_REG_BYREF:
        fByRef = true;                  // fall through
    case ICorDebugInfo::VLT_REG:
        regOffs = GetRegOffsInCONTEXT(varLoc.vlReg.vlrReg  IA64_ARG(ICorDebugInfo::VLT_REG)  IA64_ARG(pCtx));
        locs->addr = (ULONG64)(ULONG_PTR)pCtx + regOffs;
        if (fByRef)
        {
            locs->addr = (ULONG64)DereferenceByRefVar((SIZE_T)locs->addr);
        }
        locs->size = sizeof(SIZE_T);
        {
            locs->contextReg = true;
        }
        return 1;


    case ICorDebugInfo::VLT_STK_BYREF:
        fByRef = true;                      // fall through
    case ICorDebugInfo::VLT_STK:
        regOffs = GetRegOffsInCONTEXT(varLoc.vlStk.vlsBaseReg  IA64_ARG(ICorDebugInfo::VLT_REG)  IA64_ARG(pCtx));
        baseReg = *(TADDR *)(regOffs + (BYTE*)pCtx);
        locs->addr = baseReg + varLoc.vlStk.vlsOffset;
        if (fByRef)
        {
            locs->addr = (ULONG64)DereferenceByRefVar((SIZE_T)locs->addr);
        }
        locs->size = sizeof(SIZE_T);
        locs->contextReg = false;
        return 1;

    case ICorDebugInfo::VLT_REG_REG:
        regOffs = GetRegOffsInCONTEXT(varLoc.vlRegReg.vlrrReg1);
        locs->addr = (ULONG64)(ULONG_PTR)pCtx + regOffs;
        locs->size = sizeof(SIZE_T);
        locs->contextReg = true;
        locs++;

        regOffs = GetRegOffsInCONTEXT(varLoc.vlRegReg.vlrrReg2);
        locs->addr = (ULONG64)(ULONG_PTR)pCtx + regOffs;
        locs->size = sizeof(SIZE_T);
        locs->contextReg = true;
        return 2;

    case ICorDebugInfo::VLT_REG_STK:
        regOffs = GetRegOffsInCONTEXT(varLoc.vlRegStk.vlrsReg);
        locs->addr = (ULONG64)(ULONG_PTR)pCtx + regOffs;
        locs->size = sizeof(SIZE_T);
        locs->contextReg = true;
        locs++;

        regOffs = GetRegOffsInCONTEXT(varLoc.vlRegStk.vlrsStk.vlrssBaseReg);
        baseReg = *(TADDR *)(regOffs + (BYTE*)pCtx);
        locs->addr = baseReg + varLoc.vlRegStk.vlrsStk.vlrssOffset;
        locs->size = sizeof(SIZE_T);
        locs->contextReg = false;
        return 2;

    case ICorDebugInfo::VLT_STK_REG:
        regOffs = GetRegOffsInCONTEXT(varLoc.vlStkReg.vlsrStk.vlsrsBaseReg);
        baseReg = *(TADDR *)(regOffs + (BYTE*)pCtx);
        locs->addr = baseReg + varLoc.vlStkReg.vlsrStk.vlsrsOffset;
        locs->size = sizeof(SIZE_T);
        locs->contextReg = false;
        locs++;

        regOffs = GetRegOffsInCONTEXT(varLoc.vlStkReg.vlsrReg);
        locs->addr = (ULONG64)(ULONG_PTR)pCtx + regOffs;
        locs->size = sizeof(SIZE_T);
        locs->contextReg = true;
        return 2;

    case ICorDebugInfo::VLT_STK2:
        regOffs = GetRegOffsInCONTEXT(varLoc.vlStk2.vls2BaseReg);
        baseReg = *(TADDR *)(regOffs + (BYTE*)pCtx);
        locs->addr = baseReg + varLoc.vlStk2.vls2Offset;
        locs->size = 2 * sizeof(SIZE_T);
        locs->contextReg = false;
        return 1;

    case ICorDebugInfo::VLT_FPSTK:
         _ASSERTE(!"NYI");
         return 0;

    default:
         _ASSERTE(!"Bad locType");
         return 0;
    }
}


BOOL CompareFiles(HANDLE hFile1,HANDLE hFile2)
{

    STATIC_CONTRACT_THROWS;
    STATIC_CONTRACT_GC_NOTRIGGER;
    BY_HANDLE_FILE_INFORMATION fileinfo1;
    BY_HANDLE_FILE_INFORMATION fileinfo2;    
    if (!GetFileInformationByHandle(hFile1,&fileinfo1) ||
        !GetFileInformationByHandle(hFile2,&fileinfo2))
        ThrowLastError();
    return fileinfo1.nFileIndexLow == fileinfo2.nFileIndexLow &&
               fileinfo1.nFileIndexHigh == fileinfo2.nFileIndexHigh &&
               fileinfo1.dwVolumeSerialNumber==fileinfo2.dwVolumeSerialNumber;
}


#ifndef DACCESS_COMPILE

// Returns the location at which the variable
// begins.  Returns NULL for register vars.  For reg-stack
// split, it'll return the addr of the stack part.
// This also works for VLT_REG (a single register).
SIZE_T *NativeVarStackAddr(const ICorDebugInfo::VarLoc &   varLoc,
                           PCONTEXT                        pCtx)
{
    STATIC_CONTRACT_NOTHROW;
    STATIC_CONTRACT_GC_NOTRIGGER;
    STATIC_CONTRACT_FORBID_FAULT;

    SIZE_T *dwAddr = NULL;

    bool fByRef = false;
    switch(varLoc.vlType)
    {
        SIZE_T          regOffs;
        const BYTE *    baseReg;

    case ICorDebugInfo::VLT_REG_BYREF:
        fByRef = true;                      // fall through
    case ICorDebugInfo::VLT_REG:
        regOffs = GetRegOffsInCONTEXT(varLoc.vlReg.vlrReg  IA64_ARG(ICorDebugInfo::VLT_REG)  IA64_ARG(pCtx));
        dwAddr = (SIZE_T *)(regOffs + (BYTE*)pCtx);
        if (fByRef)
        {
            dwAddr = (SIZE_T*)(*dwAddr);
        }
        LOG((LF_CORDB, LL_INFO100, "NVSA: VLT_REG @ 0x%x (by ref = %d)\n", dwAddr, fByRef));
        break;


    case ICorDebugInfo::VLT_STK_BYREF:
        fByRef = true;                      // fall through
    case ICorDebugInfo::VLT_STK:
        regOffs = GetRegOffsInCONTEXT(varLoc.vlStk.vlsBaseReg  IA64_ARG(ICorDebugInfo::VLT_REG)  IA64_ARG(pCtx));
        baseReg = (const BYTE *)*(SIZE_T *)(regOffs + (BYTE*)pCtx);
        dwAddr  = (SIZE_T *)(baseReg + varLoc.vlStk.vlsOffset);
        if (fByRef)
        {
            dwAddr = (SIZE_T*)(*dwAddr);
        }
        LOG((LF_CORDB, LL_INFO100, "NVSA: VLT_STK @ 0x%x (by ref = %d)\n", dwAddr, fByRef));
        break;

    case ICorDebugInfo::VLT_STK2:

        regOffs = GetRegOffsInCONTEXT(varLoc.vlStk2.vls2BaseReg);
        baseReg = (const BYTE *)*(SIZE_T *)(regOffs + (BYTE*)pCtx);
        dwAddr = (SIZE_T *)(baseReg + varLoc.vlStk2.vls2Offset);
        LOG((LF_CORDB, LL_INFO100, "NVSA: VLT_STK_2 @ 0x%x\n",dwAddr));
        break;

    case ICorDebugInfo::VLT_REG_STK:
        regOffs = GetRegOffsInCONTEXT(varLoc.vlRegStk.vlrsStk.vlrssBaseReg);
        baseReg = (const BYTE *)*(SIZE_T *)(regOffs + (BYTE*)pCtx);
        dwAddr = (SIZE_T *)(baseReg + varLoc.vlRegStk.vlrsStk.vlrssOffset);
        LOG((LF_CORDB, LL_INFO100, "NVSA: REG_STK @ 0x%x\n",dwAddr));
        break;

    case ICorDebugInfo::VLT_STK_REG:
        regOffs = GetRegOffsInCONTEXT(varLoc.vlStkReg.vlsrStk.vlsrsBaseReg);
        baseReg = (const BYTE *)*(SIZE_T *)(regOffs + (BYTE*)pCtx);
        dwAddr = (SIZE_T *)(baseReg + varLoc.vlStkReg.vlsrStk.vlsrsOffset);
        LOG((LF_CORDB, LL_INFO100, "NVSA: STK_REG @ 0x%x\n",dwAddr));
        break;

    case ICorDebugInfo::VLT_REG_REG:
    case ICorDebugInfo::VLT_FPSTK:
         _ASSERTE(!"NYI"); break;

    default:
         _ASSERTE(!"Bad locType"); break;
    }

    return dwAddr;

}




bool    GetNativeVarVal(const ICorDebugInfo::VarLoc &   varLoc,
                        PCONTEXT                        pCtx,
                        SIZE_T                      *   pVal1,
                        SIZE_T                      *   pVal2
                        WIN64_ARG(SIZE_T                cbSize))
{

    STATIC_CONTRACT_NOTHROW;
    STATIC_CONTRACT_GC_NOTRIGGER;
    STATIC_CONTRACT_FORBID_FAULT;

    switch(varLoc.vlType)
    {
        SIZE_T          regOffs;

    case ICorDebugInfo::VLT_REG:
        *pVal1  = *NativeVarStackAddr(varLoc,pCtx);
        break;

    case ICorDebugInfo::VLT_STK:
        *pVal1  = *NativeVarStackAddr(varLoc,pCtx);
        break;

    case ICorDebugInfo::VLT_STK2:
        *pVal1  = *NativeVarStackAddr(varLoc,pCtx);
        *pVal2  = *(NativeVarStackAddr(varLoc,pCtx)+ 1);
        break;

    case ICorDebugInfo::VLT_REG_REG:
        regOffs = GetRegOffsInCONTEXT(varLoc.vlRegReg.vlrrReg1);
        *pVal1 = *(SIZE_T *)(regOffs + (BYTE*)pCtx);
        LOG((LF_CORDB, LL_INFO100, "GNVV: STK_REG_REG 1 @ 0x%x\n",
            (SIZE_T *)(regOffs + (BYTE*)pCtx)));

        regOffs = GetRegOffsInCONTEXT(varLoc.vlRegReg.vlrrReg2);
        *pVal2 = *(SIZE_T *)(regOffs + (BYTE*)pCtx);
        LOG((LF_CORDB, LL_INFO100, "GNVV: STK_REG_REG 2 @ 0x%x\n",
            (SIZE_T *)(regOffs + (BYTE*)pCtx)));
        break;

    case ICorDebugInfo::VLT_REG_STK:
        regOffs = GetRegOffsInCONTEXT(varLoc.vlRegStk.vlrsReg);
        *pVal1 = *(SIZE_T *)(regOffs + (BYTE*)pCtx);
        LOG((LF_CORDB, LL_INFO100, "GNVV: STK_REG_STK reg @ 0x%x\n",
            (SIZE_T *)(regOffs + (BYTE*)pCtx)));
        *pVal2 = *NativeVarStackAddr(varLoc,pCtx);
        break;

    case ICorDebugInfo::VLT_STK_REG:
        *pVal1 = *NativeVarStackAddr(varLoc,pCtx);
        regOffs = GetRegOffsInCONTEXT(varLoc.vlStkReg.vlsrReg);
        *pVal2 = *(SIZE_T *)(regOffs + (BYTE*)pCtx);
        LOG((LF_CORDB, LL_INFO100, "GNVV: STK_STK_REG reg @ 0x%x\n",
            (SIZE_T *)(regOffs + (BYTE*)pCtx)));
        break;

    case ICorDebugInfo::VLT_FPSTK:
         _ASSERTE(!"NYI"); break;


    default:
         _ASSERTE(!"Bad locType"); break;
    }

    return true;
}




bool    SetNativeVarVal(const ICorDebugInfo::VarLoc &   varLoc,
                        PCONTEXT                        pCtx,
                        SIZE_T                          val1,
                        SIZE_T                          val2
                        WIN64_ARG(SIZE_T                cbSize))
{
    STATIC_CONTRACT_NOTHROW;
    STATIC_CONTRACT_GC_NOTRIGGER;
    STATIC_CONTRACT_FORBID_FAULT;

    switch(varLoc.vlType)
    {
        SIZE_T          regOffs;

    case ICorDebugInfo::VLT_REG:
        *NativeVarStackAddr(varLoc,pCtx) = val1;
        break;

    case ICorDebugInfo::VLT_STK:
        *NativeVarStackAddr(varLoc,pCtx) = val1;
        break;

    case ICorDebugInfo::VLT_STK2:
        *NativeVarStackAddr(varLoc,pCtx) = val1;
        *(NativeVarStackAddr(varLoc,pCtx)+ 1) = val2;
        break;

    case ICorDebugInfo::VLT_REG_REG:
        regOffs = GetRegOffsInCONTEXT(varLoc.vlRegReg.vlrrReg1);
        *(SIZE_T *)(regOffs + (BYTE*)pCtx) = val1;
        LOG((LF_CORDB, LL_INFO100, "SNVV: STK_REG_REG 1 @ 0x%x\n",
            (SIZE_T *)(regOffs + (BYTE*)pCtx)));

        regOffs = GetRegOffsInCONTEXT(varLoc.vlRegReg.vlrrReg2);
        *(SIZE_T *)(regOffs + (BYTE*)pCtx) = val2;
        LOG((LF_CORDB, LL_INFO100, "SNVV: STK_REG_REG 2 @ 0x%x\n",
            (SIZE_T *)(regOffs + (BYTE*)pCtx)));
        break;

    case ICorDebugInfo::VLT_REG_STK:
        regOffs = GetRegOffsInCONTEXT(varLoc.vlRegStk.vlrsReg);
        *(SIZE_T *)(regOffs + (BYTE*)pCtx) = val1;
        LOG((LF_CORDB, LL_INFO100, "SNVV: STK_REG_STK reg @ 0x%x\n",
            (SIZE_T *)(regOffs + (BYTE*)pCtx)));
        *NativeVarStackAddr(varLoc,pCtx) = val2;
        break;

    case ICorDebugInfo::VLT_STK_REG:
        *NativeVarStackAddr(varLoc,pCtx) = val1;
        regOffs = GetRegOffsInCONTEXT(varLoc.vlStkReg.vlsrReg);
        *(SIZE_T *)(regOffs + (BYTE*)pCtx) = val2;
        LOG((LF_CORDB, LL_INFO100, "SNVV: STK_STK_REG reg @ 0x%x\n",
            (SIZE_T *)(regOffs + (BYTE*)pCtx)));
        break;

    case ICorDebugInfo::VLT_FPSTK:
         _ASSERTE(!"NYI"); break;


    default:
         _ASSERTE(!"Bad locType"); break;
    }

    return true;
}

HRESULT VMPostError(                    // Returned error.
    HRESULT     hrRpt,                  // Reported error.
    ...)                                // Error arguments.
{
    CONTRACTL
    {
        NOTHROW;
        GC_TRIGGERS;
        MODE_ANY;
    }
    CONTRACTL_END;

    GCX_PREEMP();
   
    va_list     marker;                 // User text.
    va_start(marker, hrRpt);
    hrRpt = PostErrorVA(hrRpt, marker);
    va_end(marker);
    
    return hrRpt;
}

void VMDumpCOMErrors(HRESULT hrErr)
{
    CONTRACTL
    {
        NOTHROW;
        GC_TRIGGERS;
        MODE_PREEMPTIVE;
        PRECONDITION(FAILED(hrErr));
    }
    CONTRACTL_END;

    SafeComHolder<IErrorInfo> pIErr(NULL);      // Error interface.
    BSTRHolder bstrDesc(NULL);                  // Description text.

    // Try to get an error info object and display the message.
    if (SafeGetErrorInfo(&pIErr) == S_OK && pIErr->GetDescription(&bstrDesc) == S_OK)
    {
        EEMessageBoxCatastrophic(IDS_EE_GENERIC, IDS_FATAL_ERROR, (BSTR)bstrDesc);
    }
    else
    {
        // Just give out the failed hr return code.
        EEMessageBoxCatastrophic(IDS_COMPLUS_ERROR, IDS_FATAL_ERROR, hrErr);
    }
}

//-----------------------------------------------------------------------------
// Helper method to load mscorsn.dll. It is used when an app requests a legacy
// mode where mscorsn.dll it to be loaded during startup.
//-----------------------------------------------------------------------------
const WCHAR g_pwzOldStrongNameLibrary[] = L"mscorsn.dll";
#define cchOldStrongNameLibrary ( \
    (sizeof(g_pwzOldStrongNameLibrary)/sizeof(WCHAR)))

HRESULT LoadMscorsn()
{
    CONTRACTL
    {
        NOTHROW;
        GC_TRIGGERS;
        INJECT_FAULT(return FALSE;);
    }
    CONTRACTL_END;

    DWORD size = 0;
    HRESULT hr = GetInternalSystemDirectory(NULL, &size);
    if (hr != HRESULT_FROM_WIN32(ERROR_INSUFFICIENT_BUFFER))
        return hr;

    DWORD dwLength = size + cchOldStrongNameLibrary;
    if (dwLength < size)
        return HRESULT_FROM_WIN32(ERROR_ARITHMETIC_OVERFLOW);
    NewArrayHolder<WCHAR> wszPath(new (nothrow) WCHAR[dwLength]);
    if (!wszPath)
        return E_OUTOFMEMORY;

    hr = GetInternalSystemDirectory(wszPath, &size);
    if (FAILED(hr))
        return hr;

    wcscat_s(wszPath, dwLength, g_pwzOldStrongNameLibrary);
    CLRLoadLibrary(wszPath);
    return S_OK;
}

//-----------------------------------------------------------------------------

ThreadLocaleHolder::~ThreadLocaleHolder()
{
    IHostTaskManager *pManager = CorHost2::GetHostTaskManager();
    if (pManager)
    {
        BEGIN_SO_TOLERANT_CODE_CALLING_HOST(GetThread());
        pManager->SetLocale(m_locale);
        END_SO_TOLERANT_CODE_CALLING_HOST;
    }
    else
    {
        SetThreadLocale(m_locale);
    }
}



LPVOID
CLRMapViewOfFileEx(
    IN HANDLE hFileMappingObject,
    IN DWORD dwDesiredAccess,
    IN DWORD dwFileOffsetHigh,
    IN DWORD dwFileOffsetLow,
    IN SIZE_T dwNumberOfBytesToMap,
    IN LPVOID lpBaseAddress
    )
{
#ifdef _DEBUG
#endif // _DEBUG

    LPVOID pv;
    pv = MapViewOfFile(hFileMappingObject,dwDesiredAccess,dwFileOffsetHigh,dwFileOffsetLow,dwNumberOfBytesToMap);

    IHostMemoryManager *memoryManager = CorHost2::GetHostMemoryManager();
    if (pv == NULL && memoryManager)
    {
        BEGIN_SO_TOLERANT_CODE_CALLING_HOST(GetThread());
        if (SUCCEEDED(memoryManager->NeedsVirtualAddressSpace(lpBaseAddress, dwNumberOfBytesToMap)))
        {
        // after host releases VA, let us try again.
            pv = MapViewOfFile(hFileMappingObject,dwDesiredAccess,dwFileOffsetHigh,dwFileOffsetLow,dwNumberOfBytesToMap);
        }
        END_SO_TOLERANT_CODE_CALLING_HOST;
    }

    if (!pv)
    {
        if(GetLastError()==ERROR_SUCCESS)
            SetLastError(ERROR_OUTOFMEMORY);
        return NULL;
    }

#ifdef _DEBUG
#endif // _DEBUG
    {
        if (memoryManager)
        {
            BEGIN_SO_TOLERANT_CODE_CALLING_HOST(GetThread());
            memoryManager->AcquiredVirtualAddressSpace(pv, dwNumberOfBytesToMap);
            END_SO_TOLERANT_CODE_CALLING_HOST;
        }
    }
    if(!pv  && GetLastError()==ERROR_SUCCESS)
            SetLastError(ERROR_OUTOFMEMORY);

    return pv;
}

LPVOID
CLRMapViewOfFile(
    IN HANDLE hFileMappingObject,
    IN DWORD dwDesiredAccess,
    IN DWORD dwFileOffsetHigh,
    IN DWORD dwFileOffsetLow,
    IN SIZE_T dwNumberOfBytesToMap
    )
{
    return CLRMapViewOfFileEx(hFileMappingObject,dwDesiredAccess,dwFileOffsetHigh,dwFileOffsetLow,dwNumberOfBytesToMap,NULL);
}


BOOL
CLRUnmapViewOfFile(
    IN LPVOID lpBaseAddress
    )
{
#ifdef _DEBUG
#endif // _DEBUG
    {
        BOOL result = UnmapViewOfFile(lpBaseAddress);
        if (result)
        {
            IHostMemoryManager *memoryManager = CorHost2::GetHostMemoryManager();
            if (memoryManager)
            {
                BEGIN_SO_TOLERANT_CODE_CALLING_HOST(GetThread());
                memoryManager->ReleasedVirtualAddressSpace(lpBaseAddress);
                END_SO_TOLERANT_CODE_CALLING_HOST;
            }
        }
        return result;
    }
}



static HMODULE CLRLoadLibraryWorker(LPCWSTR lpLibFileName, DWORD *pLastError)
{
    // Don't use dynamic contract: will override GetLastError value
    STATIC_CONTRACT_NOTHROW;
    STATIC_CONTRACT_GC_TRIGGERS;
    STATIC_CONTRACT_FAULT;

    ThreadAffinityHolder affinity;
    HMODULE hMod;
    UINT last = SetErrorMode(SEM_NOOPENFILEERRORBOX|SEM_FAILCRITICALERRORS);
    {
        INDEBUG(PEDecoder::ForceRelocForDLL(lpLibFileName));
        hMod = WszLoadLibrary(lpLibFileName);
        *pLastError = GetLastError();
    }
    SetErrorMode(last);
    return hMod;
}

HMODULE CLRLoadLibrary(LPCWSTR lpLibFileName)
{
    // Don't use dynamic contract: will override GetLastError value
    STATIC_CONTRACT_NOTHROW;
    STATIC_CONTRACT_GC_TRIGGERS;
    STATIC_CONTRACT_FAULT;

    DWORD dwLastError;
    HMODULE hmod = CLRLoadLibraryWorker(lpLibFileName, &dwLastError);
    SetLastError(dwLastError);
    return hmod;
}




HMODULE CLRLoadSystemLibrary(LPCWSTR lpLibFileName)
{
    // Don't use dynamic contract: will override GetLastError value
    STATIC_CONTRACT_NOTHROW;
    STATIC_CONTRACT_GC_TRIGGERS;
    STATIC_CONTRACT_FAULT;

    DWORD lgth = _MAX_PATH + 1;
    WCHAR wszFile[_MAX_PATH + 1];
    
    if(FAILED(GetInternalSystemDirectory(wszFile, &lgth)))
        return NULL;

    if (wcslen(lpLibFileName) + lgth > _MAX_PATH)
        return NULL;

    wcscat_s(wszFile, COUNTOF(wszFile), lpLibFileName);

    return CLRLoadLibrary(wszFile);
}

BOOL CLRFreeLibrary(HMODULE hModule)
{
    // Don't use dynamic contract: will override GetLastError value
    STATIC_CONTRACT_NOTHROW;
    STATIC_CONTRACT_GC_TRIGGERS;
    STATIC_CONTRACT_FORBID_FAULT;

    ThreadAffinityHolder affinity;

    return FreeLibrary(hModule);
}

VOID CLRFreeLibraryAndExitThread(HMODULE hModule,DWORD dwExitCode)
{
    // Don't use dynamic contract: will override GetLastError value
    STATIC_CONTRACT_NOTHROW;
    STATIC_CONTRACT_GC_TRIGGERS;
    STATIC_CONTRACT_FORBID_FAULT;

    ThreadAffinityHolder affinity;

    FreeLibraryAndExitThread(hModule,dwExitCode);
}

#endif // #ifndef DACCESS_COMPILE

GPTR_IMPL(JITNotification, g_pNotificationTable);
GVAL_IMPL(ULONG32, g_dacNotificationFlags);

BOOL IsValidMethodCodeNotification(USHORT Notification)
{
    // If any bit is on other than that given by a valid combination of flags, no good.
    if (Notification & ~(
        CLRDATA_METHNOTIFY_NONE |
        CLRDATA_METHNOTIFY_GENERATED | 
        CLRDATA_METHNOTIFY_DISCARDED))
    {
        return FALSE;
    }
    
    return TRUE;
}

JITNotifications::JITNotifications(JITNotification *jitTable)
{
    if (jitTable)
    {
        // Bookkeeping info is held in the first slot
        m_jitTable = jitTable + 1;
    }
    else
    {
        m_jitTable = NULL;
    }
}

BOOL JITNotifications::FindItem(TADDR clrModule, mdToken token, UINT *indexOut)
{
    if (m_jitTable == NULL)
    {
        return FALSE;
    }

    if (indexOut == NULL)
    {
        return FALSE;
    }

    UINT Length = GetLength();
    for(UINT i=0; i < Length; i++)
    {
        JITNotification *pCurrent = m_jitTable + i;
        if (!pCurrent->IsFree() &&
            pCurrent->clrModule == clrModule &&
            pCurrent->methodToken == token)
        {
            *indexOut = i;
            return TRUE;
        }
    }

    return FALSE;
}

// if clrModule is NULL, all active notifications are changed to NType
BOOL JITNotifications::SetAllNotifications(TADDR clrModule,USHORT NType,BOOL *changedOut)
{
    if (m_jitTable == NULL)
    {
        return FALSE;
    }

    if (changedOut == NULL)
    {
        return FALSE;
    }

    *changedOut = FALSE;
    
    UINT Length = GetLength();
    for(UINT i=0; i < Length; i++)
    {
        JITNotification *pCurrent = m_jitTable + i;
        if (!pCurrent->IsFree() &&
            ((clrModule == NULL) || (pCurrent->clrModule == clrModule))&&
            pCurrent->state != NType)
        {
            pCurrent->state = NType;
            *changedOut = TRUE;
        }
    }    

    if (*changedOut && NType == CLRDATA_METHNOTIFY_NONE)
    {
        // Need to recompute length if we removed notifications
        for (UINT iCurrent=Length; iCurrent > 0; iCurrent--)
        {
            JITNotification *pCurrent = m_jitTable + (iCurrent - 1);
            if (pCurrent->IsFree())
            {
                DecrementLength();
            }
        }
    }
    return TRUE;
}

BOOL JITNotifications::SetNotification(TADDR clrModule, mdToken token, USHORT NType)
{
    UINT iIndex;

    if (!IsActive())
    {
        return FALSE;
    }

    if (clrModule == NULL)
    {
        return FALSE;
    }
    
    if (NType == CLRDATA_METHNOTIFY_NONE)
    {
        // Remove an item if it exists
        if (FindItem(clrModule, token, &iIndex))        
        {       
            JITNotification *pItem = m_jitTable + iIndex;
            pItem->SetFree();
            _ASSERTE(iIndex < GetLength());
            // Update highest?
            if (iIndex == (GetLength()-1))
            {
                DecrementLength();
            }       
        }
        return TRUE;            
    }
    
    if (FindItem(clrModule, token, &iIndex))
    {
        JITNotification *pItem = m_jitTable + iIndex;
        _ASSERTE(pItem->IsFree() == FALSE);
        pItem->state =  NType;        
        return TRUE;
    }

    // Find first free item
    UINT iFirstFree = GetLength();
    for (UINT i = 0; i < iFirstFree; i++)
    {
        JITNotification *pCurrent = m_jitTable + i;
        if (pCurrent->state == CLRDATA_METHNOTIFY_NONE)
        {
            iFirstFree = i;
            break;
        }
    }
    
    if (iFirstFree == GetLength() &&
        iFirstFree == GetTableSize())            
    {
        // No more room
        return FALSE;
    }
    
    JITNotification *pCurrent = m_jitTable + iFirstFree;            
    pCurrent->SetState(clrModule, token, NType);
    if (iFirstFree == GetLength())            
    {
        IncrementLength();
    }
    
    return TRUE;   
}

UINT JITNotifications::GetLength()
{
    _ASSERTE(IsActive());
    
    if (!IsActive())
    {
        return 0;
    }

    return (UINT) (m_jitTable - 1)->methodToken;    
}

void JITNotifications::IncrementLength()
{
    _ASSERTE(IsActive());

    if (!IsActive())
    {
        return;
    }    

    UINT *pShort = (UINT *) &((m_jitTable - 1)->methodToken);
    (*pShort)++;
}

void JITNotifications::DecrementLength()
{
    _ASSERTE(IsActive());

    if (!IsActive())
    {
        return;
    }    

    UINT *pShort = (UINT *) &((m_jitTable - 1)->methodToken);
    (*pShort)--;
}
    
UINT JITNotifications::GetTableSize()
{
    _ASSERTE(IsActive());
    
    if (!IsActive())
    {
        return 0;
    }

    return ((UINT) (m_jitTable - 1)->clrModule);
}

USHORT JITNotifications::Requested(TADDR clrModule, mdToken token)
{
    UINT iIndex;
    if (FindItem(clrModule, token, &iIndex))
    {
        JITNotification *pItem = m_jitTable + iIndex;
        _ASSERTE(pItem->IsFree() == FALSE);
        return pItem->state;
    }
        
    return CLRDATA_METHNOTIFY_NONE;
}

#ifdef DACCESS_COMPILE

JITNotification *JITNotifications::InitializeNotificationTable(UINT TableSize)
{
    // We use the first entry in the table for recordkeeping info.
    
    JITNotification *retTable = new (nothrow) JITNotification[TableSize+1];
    if (retTable)
    {
        // Set the length
        UINT *pUint = (UINT *) &(retTable->methodToken);
        *pUint = 0;
        // Set the table size
        pUint = (UINT *) &(retTable->clrModule);
        *pUint = TableSize;
    }
    return retTable;
}

BOOL JITNotifications::UpdateOutOfProcTable()
{
    if (PTR_HOST_TO_TADDR(g_pNotificationTable) == NULL)
    {
        TADDR Location;
        
        if (DacAllocVirtual(0, sizeof(JITNotification) * GetTableSize() + 1,
                            MEM_COMMIT, PAGE_READWRITE, false,
                            &Location) != S_OK)
        {
            return FALSE;
        }

        DPTR(PTR_JITNotification) ppTable = &g_pNotificationTable;
        *ppTable = PTR_JITNotification(Location);
        if (DacWriteHostInstance(ppTable,false) != S_OK)
        {
            return FALSE;
        }
    }

    // We store recordkeeping info right before the m_jitTable pointer, that must be written as well.
    if (DacWriteAll(PTR_HOST_TO_TADDR(g_pNotificationTable),m_jitTable - 1,
        sizeof(JITNotification)*(GetLength() + 1),false) != S_OK)
    {
        return FALSE;
    }

    return TRUE;
}
#endif // DACCESS_COMPILE

void DACNotifyExceptionHelper(TADDR *args,UINT argCount)
{
    PAL_TRY
    {  
        if (IsDebuggerPresent() && !CORDebuggerAttached()) 
        {
            RaiseException(CLRDATA_NOTIFY_EXCEPTION, 0, argCount, (ULONG_PTR *) args);
        }
    }
    PAL_EXCEPT(EXCEPTION_EXECUTE_HANDLER)
    {        
    }
    PAL_ENDTRY
}

//
//
//
#ifdef _MSC_VER
#pragma optimize("", off)
#endif
void DACNotify::DoJITNotification(MethodDesc *MethodDescPtr)
{
    TADDR Args[2] = { JIT_NOTIFICATION, (TADDR) MethodDescPtr };
    DACNotifyExceptionHelper(Args,2);
}

void DACNotify::DoJITDiscardNotification(MethodDesc *MethodDescPtr)
{
    TADDR Args[2] = { JIT_DISCARD_NOTIFICATION, (TADDR) MethodDescPtr };
    DACNotifyExceptionHelper(Args,2);
}    
   
void DACNotify::DoModuleLoadNotification(Module *ModulePtr)
{
    if ((g_dacNotificationFlags & CLRDATA_NOTIFY_ON_MODULE_LOAD) != 0)
    {
        TADDR Args[2] = { MODULE_LOAD_NOTIFICATION, (TADDR) ModulePtr};
        DACNotifyExceptionHelper(Args, 2);
    }
}

void DACNotify::DoModuleUnloadNotification(Module *ModulePtr)
{
    if ((g_dacNotificationFlags & CLRDATA_NOTIFY_ON_MODULE_UNLOAD) != 0)
    {
        TADDR Args[2] = { MODULE_UNLOAD_NOTIFICATION, (TADDR) ModulePtr};
        DACNotifyExceptionHelper(Args, 2);
    }
}

void DACNotify::DoExceptionNotification(Thread* ThreadPtr)
{
    if ((g_dacNotificationFlags & CLRDATA_NOTIFY_ON_EXCEPTION) != 0)
    {
        TADDR Args[2] = { EXCEPTION_NOTIFICATION, (TADDR) ThreadPtr};
        DACNotifyExceptionHelper(Args, 2);
    }
}

#ifdef _MSC_VER
#pragma optimize("", on)
#endif

    // called from the DAC </STRIP>
int DACNotify::GetType(TADDR Args[])
{
    return Args[0];
}
    
BOOL DACNotify::ParseJITNotification(TADDR Args[], TADDR& MethodDescPtr)
{
    _ASSERTE(Args[0] == JIT_NOTIFICATION);
    if (Args[0] != JIT_NOTIFICATION)
    {
        return FALSE;
    }

    MethodDescPtr = Args[1];

    return TRUE;
}

BOOL DACNotify::ParseJITDiscardNotification(TADDR Args[], TADDR& MethodDescPtr)
{
    _ASSERTE(Args[0] == JIT_DISCARD_NOTIFICATION);
    if (Args[0] != JIT_DISCARD_NOTIFICATION)
    {
        return FALSE;
    }

    MethodDescPtr = Args[1];

    return TRUE;
}

BOOL DACNotify::ParseModuleLoadNotification(TADDR Args[], TADDR& Module)
{
    _ASSERTE(Args[0] == MODULE_LOAD_NOTIFICATION);
    if (Args[0] != MODULE_LOAD_NOTIFICATION)
    {
        return FALSE;
    }

    Module = Args[1];

    return TRUE;
}

BOOL DACNotify::ParseModuleUnloadNotification(TADDR Args[], TADDR& Module)
{
    _ASSERTE(Args[0] == MODULE_UNLOAD_NOTIFICATION);
    if (Args[0] != MODULE_UNLOAD_NOTIFICATION)
    {
        return FALSE;
    }

    Module = Args[1];

    return TRUE;
}

BOOL DACNotify::ParseExceptionNotification(TADDR Args[], TADDR& ThreadPtr)
{
    _ASSERTE(Args[0] == EXCEPTION_NOTIFICATION);
    if (Args[0] != EXCEPTION_NOTIFICATION)
    {
        return FALSE;
    }

    ThreadPtr = Args[1];

    return TRUE;
}

#ifdef _DEBUG
#endif //_DEBUG



// These wrap the SString:L:CompareCaseInsenstive function in a way that makes it
// easy to fix code that uses _stricmp. _stricmp should be avoided as it uses the current
// C-runtime locale rather than the invariance culture.
//
// Note that unlike the real _stricmp, these functions unavoidably have a throws/gc_triggers/inject_fault
// contract. So if need a case-insensitive comparison in a place where you can't tolerate this contract,
// you've got a problem.
int __cdecl stricmpUTF8(const char* szStr1, const char* szStr2)
{
    CONTRACTL
    {
        THROWS;
        GC_TRIGGERS;
        INJECT_FAULT(COMPlusThrowOM());
    }
    CONTRACTL_END

    return SString(SString::Utf8, szStr1).CompareCaseInsensitive(SString(SString::Utf8, szStr2));

}

#ifdef _DEBUG
DisableDelayLoadCheckForOleaut32::DisableDelayLoadCheckForOleaut32()
{
    GetThread()->SetThreadStateNC(Thread::TSNC_DisableOleaut32Check);
}

DisableDelayLoadCheckForOleaut32::~DisableDelayLoadCheckForOleaut32()
{
    GetThread()->ResetThreadStateNC(Thread::TSNC_DisableOleaut32Check);
}

BOOL DelayLoadOleaut32CheckDisabled()
{
    Thread *pThread = GetThread();
    if (pThread && pThread->HasThreadStateNC(Thread::TSNC_DisableOleaut32Check))
    {
        return TRUE;
    }

    return FALSE;
}
#endif

static BOOL TrustMeIAmSafe(void *pLock) 
{
    LEAF_CONTRACT;
    return TRUE;
}

LockOwner g_lockTrustMeIAmThreadSafe = { NULL, TrustMeIAmSafe };


#if !defined(DACCESS_COMPILE) && defined(_DEBUG)

BOOL DbgIsExecutableVM(LPVOID lpMem, SIZE_T length)
{
    WRAPPER_CONTRACT;
    if (IsRunningUnderZapmon())
        return TRUE;
    else
        return DbgIsExecutable(lpMem, length);
}

#endif


#ifndef DACCESS_COMPILE

CpuStoreBufferControl::PFN_FlushWriteBuffers CpuStoreBufferControl::s_pFlushWriteBuffersFn = NULL;
volatile BYTE CpuStoreBufferControl::s_UseVirtualProtect = 0;

#ifdef _DEBUG
bool CpuStoreBufferControl::s_Initialized = false;
#endif    


void CpuStoreBufferControl::Init()
{
    CONTRACTL
    {
        NOTHROW;
        GC_NOTRIGGER;
    }
    CONTRACTL_END;


#ifdef _DEBUG
    s_Initialized = true;
#endif    
}

void CpuStoreBufferControl::FlushStoreBuffers()
{
    CONTRACTL
    {
        NOTHROW;
        GC_NOTRIGGER;
    }
    CONTRACTL_END;

    _ASSERTE(s_Initialized);

}

#endif // #ifndef DACCESS_COMPILE


