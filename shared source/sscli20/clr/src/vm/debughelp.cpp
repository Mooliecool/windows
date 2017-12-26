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
#include "common.h"


/*******************************************************************/
/* The folowing routines used to exist in all builds so they could called from the
 * debugger before we had strike.
 * Now most of them are only inclued in debug builds for diagnostics purposes.
*/
/*******************************************************************/

#include "stdlib.h"

BOOL isMemoryReadable(const TADDR start, unsigned len)
{
    CONTRACTL
    {
        NOTHROW;
        GC_NOTRIGGER;
        SO_TOLERANT;
    }
    CONTRACTL_END;

    //
    // To accomplish this in a no-throw way, we have to touch each and every page
    // and see if it is in memory or not.
    //

    //
    // Touch the first and last bytes.
    //
    char buff;

#ifdef DACCESS_COMPILE
    if (DacReadAll(start, &buff, 1, false) != S_OK)
    {
        return 0;
    }
#else
    if (ReadProcessMemory(GetCurrentProcess(), (PVOID)start, &buff, 1, 0) == 0)
    {
        return 0;
    }
#endif

    TADDR location;

    location = start + (len - 1);

#ifdef DACCESS_COMPILE
    if (DacReadAll(location, &buff, 1, false) != S_OK)
    {
        return 0;
    }
#else
    if (ReadProcessMemory(GetCurrentProcess(), (PVOID)location,
                          &buff, 1, 0) == 0)
    {
        return 0;
    }
#endif

    //
    // Now we have to loop thru each and every page in between and touch them.
    //
    location = start;
    while (len > PAGE_SIZE)
    {
        location += PAGE_SIZE;
        len -= PAGE_SIZE;

#ifdef DACCESS_COMPILE
        if (DacReadAll(location, &buff, 1, false) != S_OK)
        {
            return 0;
        }
#else
        if (ReadProcessMemory(GetCurrentProcess(), (PVOID)location,
                              &buff, 1, 0) == 0)
        {
            return 0;
        }
#endif
    }

    return 1;
}


/*******************************************************************/
/* check to see if 'retAddr' is a valid return address (it points to
   someplace that has a 'call' right before it), If possible it is
   it returns the address that was called in whereCalled */

bool isRetAddr(TADDR retAddr, TADDR* whereCalled)
{
    CONTRACTL
    {
        NOTHROW;
        GC_NOTRIGGER;
        SO_NOT_MAINLINE;
    }
    CONTRACTL_END;

    // don't waste time values clearly out of range
    if (retAddr < (TADDR)BOT_MEMORY || retAddr > (TADDR)TOP_MEMORY)
    {
        return false;
    }

    PTR_BYTE spot = PTR_BYTE(retAddr);
    if (!isMemoryReadable(PTR_TO_TADDR(spot) - 7, 7))
    {
        return(false);
    }

    // Note this is possible to be spoofed, but pretty unlikely
    *whereCalled = 0;
    // call XXXXXXXX
    if (spot[-5] == 0xE8)
    {
        *whereCalled = *(PTR_DWORD(retAddr - 4)) + retAddr;
        return(true);
    }

    // call [XXXXXXXX]
    if (spot[-6] == 0xFF && (spot[-5] == 025))
    {
        if (isMemoryReadable(*(PTR_TADDR(retAddr - 4)), 4))
        {
            *whereCalled = *(PTR_TADDR(*(PTR_TADDR(retAddr - 4))));
            return(true);
        }
    }

    // call [REG+XX]
    if (spot[-3] == 0xFF && (spot[-2] & ~7) == 0120 && (spot[-2] & 7) != 4)
    {
        return(true);
    }

    if (spot[-4] == 0xFF && spot[-3] == 0124)       // call [ESP+XX]
    {
        return(true);
    }

    // call [REG+XXXX]
    if (spot[-6] == 0xFF && (spot[-5] & ~7) == 0220 && (spot[-5] & 7) != 4)
    {
        return(true);
    }

    if (spot[-7] == 0xFF && spot[-6] == 0224)       // call [ESP+XXXX]
    {
        return(true);
    }

    // call [REG]
    if (spot[-2] == 0xFF && (spot[-1] & ~7) == 0020 && (spot[-1] & 7) != 4 && (spot[-1] & 7) != 5)
    {
        return(true);
    }

    // call REG
    if (spot[-2] == 0xFF && (spot[-1] & ~7) == 0320 && (spot[-1] & 7) != 4)
    {
        return(true);
    }

    // There are other cases, but I don't believe they are used.
    return(false);
}

/*
 * The remaining methods are included in debug builds only
 */
#ifdef _DEBUG

#ifndef DACCESS_COMPILE


/*******************************************************************/
MethodDesc* IP2MD(ULONG_PTR IP)
{
    CONTRACTL
    {
        NOTHROW;
        GC_NOTRIGGER;
    }
    CONTRACTL_END;

    return(IP2MethodDesc((SLOT)IP));
}

/*******************************************************************/
MethodDesc* Entry2MethodDescMD(BYTE* entry)
{
    CONTRACTL
    {
        NOTHROW;
        GC_NOTRIGGER;
    }
    CONTRACTL_END;

    return(Entry2MethodDesc((BYTE*) entry, 0));
}

/*******************************************************************/
/* if addr is a valid method table, return a poitner to it */
MethodTable* AsMethodTable(size_t addr)
{
    CONTRACTL
    {
        NOTHROW;
        GC_NOTRIGGER;
        DEBUG_ONLY;
    }
    CONTRACTL_END;

    MethodTable* pMT = (MethodTable*) addr;
    if (!isMemoryReadable((TADDR)pMT, sizeof(MethodTable)))
    {
        return(0);
    }

    EEClass* cls = pMT->GetClass();
    if (!isMemoryReadable((TADDR)cls, sizeof(EEClass)))
    {
        return(0);
    }

    if (cls->GetMethodTable() != pMT)
    {
        return(0);
    }

    return(pMT);
}

/*******************************************************************/
/* if addr is a valid method table, return a pointer to it */
MethodDesc* AsMethodDesc(size_t addr)
{
    CONTRACTL
    {
        NOTHROW;
        GC_NOTRIGGER;
        DEBUG_ONLY;
    }
    CONTRACTL_END;

    if (!IS_ALIGNED(addr, sizeof(void*)))
        return(0);

    MethodDesc* pMD = (MethodDesc*) addr;

    if (!isMemoryReadable((TADDR)pMD, sizeof(MethodDesc)))
    {
        return(0);
    }

    MethodDescChunk *chunk = pMD->GetMethodDescChunkUnchecked();

    if (!MethodDescChunk::IsValidMethodDescChunk (pMD, chunk))
        return (0);

    if (!isMemoryReadable((TADDR)chunk, sizeof(MethodDescChunk)))
    {
        return(0);
    }

    MethodTable* pMT = chunk->GetMethodTable();
    if (AsMethodTable((size_t) pMT) == 0)
    {
        return(0);
    }

    return(pMD);
}


//  This function will return NULL if the buffer is not large enough.
/*******************************************************************/

wchar_t* formatMethodTable(MethodTable* pMT,
                           __out_z __inout_ecount(bufSize) wchar_t* buff,
                           DWORD bufSize)
{
    CONTRACTL
    {
        NOTHROW;
        GC_NOTRIGGER;
    }
    CONTRACTL_END;

    if(bufSize == 0)
    {
        return NULL;
    }

    buff[ bufSize - 1] = L'\0';

    EEClass* cls = pMT->GetClass();
    DefineFullyQualifiedNameForClass();

    LPCUTF8 clsName = GetFullyQualifiedNameForClass(cls);

    if (clsName != 0)
    {
        if(_snwprintf_s(buff, bufSize - 1, _TRUNCATE, L"%S", clsName) < 0)
        {
            return NULL;
        }

        buff[ bufSize - 1] = L'\0';

    }
    return(buff);
}

/*******************************************************************/
//  This function will return NULL if the buffer is not large enough, otherwise it will
//  return the buffer position for next write.
/*******************************************************************/

wchar_t* formatMethodDesc(MethodDesc* pMD,
                          __out_z __inout_ecount(bufSize) wchar_t* buff,
                          DWORD bufSize)
{
    CONTRACTL
    {
        NOTHROW;
        GC_NOTRIGGER;
    }
    CONTRACTL_END;

    if(bufSize == 0)
    {
        return NULL;
    }

    buff = formatMethodTable(pMD->GetMethodTable(), buff, bufSize);
    if(buff == NULL)
    {
        return NULL;
    }

    buff[bufSize - 1] = L'\0';    // this will guarantee the buffer is also NULL-terminated
    if(_snwprintf_s( &buff[lstrlenW(buff)] , bufSize -lstrlenW(buff) - 1, _TRUNCATE, L"::%S", pMD->GetName()) < 0)
    {
        return NULL;
    }

#ifdef _DEBUG
    if (pMD->m_pszDebugMethodSignature)
    {
        if(_snwprintf_s(&buff[lstrlenW(buff)],
                      bufSize -lstrlenW(buff) - 1,
                      _TRUNCATE,
                      L" %S",
                      pMD->m_pszDebugMethodSignature) < 0)
        {
            return NULL;
        }

    }
#endif

    if(_snwprintf_s(&buff[lstrlenW(buff)], bufSize -lstrlenW(buff) - 1, _TRUNCATE, L"(%x)", (size_t)pMD) < 0)
    {
        return NULL;
    }

    return(buff);
}




/*******************************************************************/
/* dump the stack, pretty printing IL methods if possible. This
   routine is very robust.  It will never cause an access violation
   and it always find return addresses if they are on the stack
   (it may find some spurious ones however).  */

int dumpStack(BYTE* topOfStack, unsigned len)
{
    CONTRACTL
    {
        THROWS;
        GC_NOTRIGGER;
    }
    CONTRACTL_END;

    size_t* top = (size_t*) topOfStack;
    size_t* end = (size_t*) &topOfStack[len];

    size_t* ptr = (size_t*) (((size_t) top) & ~3);    // make certain dword aligned.
    TADDR whereCalled;

    WszOutputDebugString(L"***************************************************\n");

    CQuickBytes qb;

    int nLen = MAX_CLASSNAME_LENGTH * 4 + 400;  // this should be enough

    wchar_t *buff = (wchar_t *) qb.AllocThrows(nLen * sizeof(wchar_t));

    while (ptr < end)
    {
        buff[nLen - 1] = L'\0';

        wchar_t* buffPtr = buff;

        // stop if we hit unmapped pages
        if (!isMemoryReadable((TADDR)ptr, sizeof(TADDR)))
        {
            break;
        }

        if (isRetAddr((TADDR)*ptr, &whereCalled))
        {
            if (_snwprintf_s(buffPtr, buff+NumItems(buff)-buffPtr-1, _TRUNCATE,  L"STK[%08X] = %08X ", (size_t)ptr, *ptr)  <0)
            {
                return(0);
            }

            buffPtr += lstrlenW(buffPtr);

            wchar_t* kind = L"RETADDR ";

            // Is this a stub (is the return address a MethodDesc?
            MethodDesc* ftn = AsMethodDesc(*ptr);

            if (ftn != 0)
            {

                kind = L"     MD PARAM";

                // If another true return address is not directly before it, it is just
                // a methodDesc param.
                TADDR prevRetAddr = ptr[1];

                if (isRetAddr(prevRetAddr, &whereCalled) && AsMethodDesc(prevRetAddr) == 0)
                {
                    kind = L"STUBCALL";
                }
                else
                {
                    // Is it the magic sequence used by CallDescr?
                    if (isMemoryReadable(prevRetAddr - sizeof(short),
                                         sizeof(short)) &&
                        ((short*) prevRetAddr)[-1] == 0x5A59)   // Pop ECX POP EDX
                    {
                        kind = L"STUBCALL";
                    }

                }

            }
            else    // Is it some other code the EE knows about?
            {
                ftn = IP2MethodDesc((BYTE*)(*ptr));
            }

            if(_snwprintf_s(buffPtr, buff+ nLen -buffPtr-1, _TRUNCATE, L"%s ", kind) < 0)
            {
                return(0);
            }

            buffPtr += lstrlenW(buffPtr);

            if (ftn != 0)
            {
                // buffer is not large enough
                if( formatMethodDesc(ftn, buffPtr, buff+ nLen -buffPtr-1) == NULL)
                {
                    return(0);
                }

                buffPtr += lstrlenW(buffPtr);
            }
            else
            {
                wcsncpy_s(buffPtr, nLen - (buffPtr - buff), L"<UNKNOWN FTN>", _TRUNCATE);
                buffPtr += lstrlenW(buffPtr);
            }

            if (whereCalled != 0)
            {
                if(_snwprintf_s(buffPtr, buff+ nLen -buffPtr-1, _TRUNCATE, L" Caller called Entry %X", whereCalled) <0)
                {
                    return(0);
                }

                buffPtr += lstrlenW(buffPtr);
            }

            wcsncpy_s(buffPtr, nLen - (buffPtr - buff), L"\n", _TRUNCATE);
            buffPtr += lstrlenW(buffPtr);
            WszOutputDebugString(buff);
        }

        MethodTable* pMT = AsMethodTable(*ptr);
        if (pMT != 0)
        {
            buffPtr = buff;
            if( _snwprintf_s(buffPtr, buff+ nLen -buffPtr-1, _TRUNCATE, L"STK[%08X] = %08X          MT PARAM ", (size_t)ptr, *ptr ) <0)
            {
                return(0);
            }

            buffPtr += lstrlenW(buffPtr);

            if( formatMethodTable(pMT, buffPtr, buff+ nLen -buffPtr-1) == NULL)
            {
                return(0);
            }

            buffPtr += lstrlenW(buffPtr);

            wcsncpy_s(buffPtr, nLen - (buffPtr - buff), L"\n", _TRUNCATE);
            WszOutputDebugString(buff);

        }

        ptr++;

    } // while

    return(0);
}

/*******************************************************************/
/* dump the stack from the current ESP.  Stop when we reach a 64K
   boundary */
int DumpCurrentStack()
{
    CONTRACTL
    {
        THROWS;
        GC_NOTRIGGER;
    }
    CONTRACTL_END;

#ifdef _X86_
    BYTE* top = (BYTE *)GetCurrentSP();

        // go back at most 64K, it will stop if we go off the
        // top to unmapped memory
    return(dumpStack(top, 0xFFFF));
#else
    _ASSERTE(!"@NYI - DumpCurrentStack(DebugHelp.cpp)");
    return 0;
#endif // _X86_
}

/*******************************************************************/
WCHAR* StringVal(STRINGREF objref)
{
    CONTRACTL
    {
        THROWS;
        GC_NOTRIGGER;
    }
    CONTRACTL_END;

    return(objref->GetBuffer());
}

LPCUTF8 NameForMethodTable(UINT_PTR pMT)
{
    CONTRACTL
    {
        NOTHROW;
        GC_NOTRIGGER;
    }
    CONTRACTL_END;

    DefineFullyQualifiedNameForClass();
    LPCUTF8 clsName = GetFullyQualifiedNameForClass(((MethodTable*)pMT)->GetClass());
    // Note we're returning local stack space - this should be OK for using in the debugger though
    return clsName;
}

LPCUTF8 ClassNameForObject(UINT_PTR obj)
{
    CONTRACTL
    {
        NOTHROW;
        GC_NOTRIGGER;
    }
    CONTRACTL_END;

    return(NameForMethodTable((UINT_PTR)(((Object*)obj)->GetMethodTable())));
}

LPCUTF8 ClassNameForOBJECTREF(OBJECTREF obj)
{
    CONTRACTL
    {
        NOTHROW;
        GC_NOTRIGGER;
    }
    CONTRACTL_END;

    return(ClassNameForObject((UINT_PTR)(OBJECTREFToObject(obj))));
}

LPCUTF8 NameForMethodDesc(UINT_PTR pMD)
{
    CONTRACTL
    {
        NOTHROW;
        GC_NOTRIGGER;
    }
    CONTRACTL_END;

    return(((MethodDesc*)pMD)->GetName());
}

LPCUTF8 ClassNameForMethodDesc(UINT_PTR pMD)
{
    CONTRACTL
    {
        NOTHROW;
        GC_NOTRIGGER;
    }
    CONTRACTL_END;

    DefineFullyQualifiedNameForClass ();
    if (((MethodDesc *)pMD)->GetClass())
    {
        return GetFullyQualifiedNameForClass(((MethodDesc*)pMD)->GetClass());
    }
    else
        return "GlobalFunctions";
}

PCCOR_SIGNATURE RawSigForMethodDesc(MethodDesc* pMD)
{
    CONTRACTL
    {
        NOTHROW;
        GC_NOTRIGGER;
    }
    CONTRACTL_END;

    return(pMD->GetSig());
}

Thread * CurrentThreadInfo ()
{
    CONTRACTL
    {
        NOTHROW;
        GC_NOTRIGGER;
    }
    CONTRACTL_END;

    return GetThread ();
}

AppDomain *GetAppDomainForObject(UINT_PTR obj)
{
    CONTRACTL
    {
        NOTHROW;
        GC_NOTRIGGER;
    }
    CONTRACTL_END;

    return ((Object*)obj)->GetAppDomain();
}

ADIndex GetAppDomainIndexForObject(UINT_PTR obj)
{
    CONTRACTL
    {
        NOTHROW;
        GC_NOTRIGGER;
    }
    CONTRACTL_END;

    return ((Object*)obj)->GetHeader()->GetAppDomainIndex();
}

AppDomain *GetAppDomainForObjectHeader(UINT_PTR hdr)
{
    CONTRACTL
    {
        NOTHROW;
        GC_NOTRIGGER;
    }
    CONTRACTL_END;

    ADIndex indx = ((ObjHeader*)hdr)->GetAppDomainIndex();
    if (!indx.m_dwIndex)
    {
        return NULL;
    }

    return SystemDomain::GetAppDomainAtIndex(indx);
}

ADIndex GetAppDomainIndexForObjectHeader(UINT_PTR hdr)
{
    CONTRACTL
    {
        NOTHROW;
        GC_NOTRIGGER;
    }
    CONTRACTL_END;

    return ((ObjHeader*)hdr)->GetAppDomainIndex();
}

SyncBlock *GetSyncBlockForObject(UINT_PTR obj)
{
    CONTRACTL
    {
        NOTHROW;
        GC_NOTRIGGER;
    }
    CONTRACTL_END;

    return ((Object*)obj)->GetHeader()->PassiveGetSyncBlock();
}

#include "../ildasm/formattype.cpp"
/*******************************************************************/
void PrintTableForClass(UINT_PTR pClass)
{
    CONTRACTL
    {
        NOTHROW;
        GC_TRIGGERS;
    }
    CONTRACTL_END;

    DefineFullyQualifiedNameForClass();
    LPCUTF8 name = GetFullyQualifiedNameForClass(((EEClass*)pClass));
    ((EEClass*)pClass)->DebugDumpVtable(name, true);
    ((EEClass*)pClass)->DebugDumpFieldLayout(name, true);
    ((EEClass*)pClass)->DebugDumpGCDesc(name, true);
}

void PrintMethodTable(UINT_PTR pMT)
{
    CONTRACTL
    {
        NOTHROW;
        GC_TRIGGERS;
    }
    CONTRACTL_END;

    PrintTableForClass((UINT_PTR) ((MethodTable*)pMT)->GetClass() );
}

void PrintTableForMethodDesc(UINT_PTR pMD)
{
    CONTRACTL
    {
        NOTHROW;
        GC_TRIGGERS;
    }
    CONTRACTL_END;

    PrintMethodTable((UINT_PTR) ((MethodDesc *)pMD)->GetMethodTable() );
}

void PrintException(OBJECTREF pObjectRef)
{
    CONTRACTL
    {
        THROWS;
        GC_NOTRIGGER;
    }
    CONTRACTL_END;

    if(pObjectRef == NULL)
    {
        return;
    }

    GCPROTECT_BEGIN(pObjectRef);

    MethodDescCallSite internalToString(METHOD__EXCEPTION__INTERNAL_TO_STRING, &pObjectRef);

    ARG_SLOT arg[1] = {
        ObjToArgSlot(pObjectRef)
    };

    STRINGREF str = internalToString.Call_RetSTRINGREF(arg);

    if(str->GetBuffer() != NULL)
    {
        WszOutputDebugString(str->GetBuffer());
    }

    GCPROTECT_END();
}

void PrintException(UINT_PTR pObject)
{
    CONTRACTL
    {
        NOTHROW;
        GC_NOTRIGGER;
    }
    CONTRACTL_END;

    OBJECTREF pObjectRef = NULL;
    GCPROTECT_BEGIN(pObjectRef);
    GCPROTECT_END();
}


/*******************************************************************/
char* FormatSigHelper(MethodDesc* pMD, CQuickBytes *out, BaseDomain *pDomain, AllocMemTracker *pamTracker)
{
    PCCOR_SIGNATURE pSig;
    ULONG cSig;
    char *ret = NULL;

    pMD->GetSig(&pSig, &cSig);

    if (pSig == NULL)
    {
        return "<null>";
    }

    EX_TRY
    {
        const char* sigStr = PrettyPrintSig(pSig, cSig, "*", out, pMD->GetMDImport(), 0);

        size_t len = strlen(sigStr)+1;
        char* mem = (char*) pamTracker->Track(pDomain->GetHighFrequencyHeap()->AllocMem(len));

        if (mem != NULL)
        {
            strcpy_s(mem, len, sigStr);
            ret = mem;
        }
    }
    EX_CATCH
    {
    }
    EX_END_CATCH(RethrowTerminalExceptions);

    return ret;
}

/*******************************************************************/
char* FormatSig(MethodDesc* pMD, BaseDomain *pDomain, AllocMemTracker *pamTracker)
{
    CONTRACTL
    {
        THROWS;
        GC_NOTRIGGER;
    }
    CONTRACTL_END;


    CQuickBytes out;
    char *ret;

    // Remove this once the following stack gets fixed:
    //
    //  RefSecContext::Init  - sets GCViolation.
    //  Binder::LookupClass  - sets GCX_COOP
    //  ClassLoader::LoadTypeHandleThrowing  - sets GCX_PREEMP
    //  ClassLoader::LookupInModule - sets NO_TRIGGER (causes GCViolation to be wiped out).
    //
    CONTRACT_VIOLATION(GCViolation);

    ret = FormatSigHelper(pMD, &out, pDomain, pamTracker);

    return(ret);
}


/*******************************************************************/
/* sends a current stack trace to the debug window */

struct PrintCallbackData {
    BOOL toStdout;
    BOOL withAppDomain;
#ifdef _DEBUG
    BOOL toLOG;
#endif
};

StackWalkAction PrintStackTraceCallback(CrawlFrame* pCF, VOID* pData)
{
    CONTRACTL
    {
        DISABLED(NOTHROW);
        DISABLED(GC_TRIGGERS);
    }
    CONTRACTL_END;

    MethodDesc* pMD = pCF->GetFunction();
    wchar_t buff[2048];
    const int nLen = NumItems(buff) - 1;    // keep one character for "\n"
    buff[0] = 0;
    buff[nLen-1] = L'\0';                    // make sure the buffer is always NULL-terminated

    PrintCallbackData *pCBD = (PrintCallbackData *)pData;

    if (pMD != 0)
    {
        EEClass* cls = pMD->GetClass();

        if (pCBD->withAppDomain)
        {
            if(_snwprintf_s(&buff[lstrlenW(buff)],
                          nLen -lstrlenW(buff) - 1,
                          _TRUNCATE,
                          L"{[%3.3x] %s} ",
                          pCF->GetAppDomain()->GetId().m_dwId,
                          pCF->GetAppDomain()->GetFriendlyName(FALSE)) < 0)
            {
                return SWA_CONTINUE;
            }
        }

        if (cls != 0)
        {
            DefineFullyQualifiedNameForClass();

            LPCUTF8 clsName = GetFullyQualifiedNameForClass(cls);

            if (clsName != 0)
            {
                if(_snwprintf_s(&buff[lstrlenW(buff)], nLen -lstrlenW(buff) - 1, _TRUNCATE, L"%S::", clsName) < 0)
                {
                    return SWA_CONTINUE;
                }
            }
        }

        // This prematurely suppressrelease'd AmTracker will leak any memory allocated by FormatSig.
        // But this routine is diagnostic aid, not customer-reachable so we won't bother to plug.
        AllocMemTracker dummyAmTracker;

        int buffLen = _snwprintf_s(&buff[lstrlenW(buff)],
                      nLen -lstrlenW(buff) - 1,
                      _TRUNCATE,
                      L"%S %S  ",
                      pMD->GetName(),
                      FormatSig(pMD, pCF->GetAppDomain(), &dummyAmTracker));

        dummyAmTracker.SuppressRelease();
        if (buffLen < 0 )
        {
            return SWA_CONTINUE;
        }


        if (pCF->IsFrameless() && pCF->GetJitManager() != 0) {

            METHODTOKEN methTok;

            IJitManager* jitMgr = pCF->GetJitManager();

            PREGDISPLAY regs = pCF->GetRegisterSet();

            DWORD offset;

            jitMgr->JitCodeToMethodInfo((PBYTE)GetControlPC(regs), NULL, &methTok, &offset);

            BYTE* start = jitMgr->JitTokenToStartAddress(methTok);

            if(_snwprintf_s(&buff[lstrlenW(buff)],
                          nLen -lstrlenW(buff) - 1,
                          _TRUNCATE,
                          L"JIT ESP:%X MethStart:%X EIP:%X(rel %X)",
                          (size_t)GetRegdisplaySP(regs),
                          (size_t)start,
                          (size_t)GetControlPC(regs),
                          offset) < 0)
            {
                return SWA_CONTINUE;
            }

        }
        else
        {

            if(_snwprintf_s(&buff[lstrlenW(buff)], nLen -lstrlenW(buff) - 1, _TRUNCATE, L"EE implemented") < 0)
            {
                return SWA_CONTINUE;
            }
        }

    }
    else
    {
        Frame* frame = pCF->GetFrame();

        if(_snwprintf_s(&buff[lstrlenW(buff)],
                      nLen -lstrlenW(buff) - 1,
                      _TRUNCATE,
                      L"EE Frame is" LFMT_ADDR,
                      (size_t)DBG_ADDR(frame)) < 0)
        {
            return SWA_CONTINUE;
        }
    }

    if (pCBD->toStdout)
    {
        wcscat_s(buff, COUNTOF(buff), L"\n");
        PrintToStdOutW(buff);
    }
#ifdef _DEBUG
    else if (pCBD->toLOG)
    {
        MAKE_ANSIPTR_FROMWIDE(sbuff, buff);
        // For LogSpewAlways to work rightr the "\n" (newline)
        // must be in the fmt string not part of the args
        LogSpewAlways("    %s\n", sbuff);
    }
#endif
    else
    {
        wcscat_s(buff, COUNTOF(buff), L"\n");
        WszOutputDebugString(buff);
    }

    return SWA_CONTINUE;
}

void PrintStackTrace()
{
    CONTRACTL
    {
        DISABLED(NOTHROW);
        DISABLED(GC_TRIGGERS);
    }
    CONTRACTL_END;

    WszOutputDebugString(L"***************************************************\n");
    PrintCallbackData cbd = {0, 0};
    GetThread()->StackWalkFrames(PrintStackTraceCallback, &cbd, ALLOW_ASYNC_STACK_WALK, 0);
}

void PrintStackTraceToStdout()
{
    CONTRACTL
    {
        DISABLED(NOTHROW);
        DISABLED(GC_TRIGGERS);
    }
    CONTRACTL_END;

    PrintCallbackData cbd = {1, 0};
    GetThread()->StackWalkFrames(PrintStackTraceCallback, &cbd, ALLOW_ASYNC_STACK_WALK, 0);
}

#ifdef _DEBUG
void PrintStackTraceToLog()
{
    CONTRACTL
    {
        DISABLED(NOTHROW);
        DISABLED(GC_TRIGGERS);
    }
    CONTRACTL_END;

    PrintCallbackData cbd = {0, 0, 1};
    GetThread()->StackWalkFrames(PrintStackTraceCallback, &cbd, ALLOW_ASYNC_STACK_WALK, 0);
}
#endif

void PrintStackTraceWithAD()
{
    CONTRACTL
    {
        DISABLED(NOTHROW);
        DISABLED(GC_TRIGGERS);
    }
    CONTRACTL_END;

    WszOutputDebugString(L"***************************************************\n");
    PrintCallbackData cbd = {0, 1};
    GetThread()->StackWalkFrames(PrintStackTraceCallback, &cbd, ALLOW_ASYNC_STACK_WALK, 0);
}

void PrintStackTraceWithADToStdout()
{
    CONTRACTL
    {
        DISABLED(NOTHROW);
        DISABLED(GC_TRIGGERS);
    }
    CONTRACTL_END;

    PrintCallbackData cbd = {1, 1};
    GetThread()->StackWalkFrames(PrintStackTraceCallback, &cbd, ALLOW_ASYNC_STACK_WALK, 0);
}

#ifdef _DEBUG
void PrintStackTraceWithADToLog()
{
    CONTRACTL
    {
        NOTHROW;
        GC_NOTRIGGER;
    }
    CONTRACTL_END;

    PrintCallbackData cbd = {0, 1, 1};
    GetThread()->StackWalkFrames(PrintStackTraceCallback, &cbd, ALLOW_ASYNC_STACK_WALK, 0);
}

void PrintStackTraceWithADToLog(Thread *pThread)
{
    CONTRACTL
    {
        DISABLED(NOTHROW);
        DISABLED(GC_TRIGGERS);
    }
    CONTRACTL_END;

    PrintCallbackData cbd = {0, 1, 1};
    pThread->StackWalkFrames(PrintStackTraceCallback, &cbd, ALLOW_ASYNC_STACK_WALK, 0);
}
#endif

/*******************************************************************/
// Get the system or current domain from the thread.
BaseDomain* GetSystemDomain()
{
    CONTRACTL
    {
        NOTHROW;
        GC_NOTRIGGER;
    }
    CONTRACTL_END;

    return SystemDomain::System();
}

AppDomain* GetCurrentDomain()
{
    CONTRACTL
    {
        NOTHROW;
        GC_NOTRIGGER;
    }
    CONTRACTL_END;

    return SystemDomain::GetCurrentDomain();
}

void PrintDomainName(size_t ob)
{
    CONTRACTL
    {
        THROWS;
        GC_NOTRIGGER;
    }
    CONTRACTL_END;

    AppDomain* dm = (AppDomain*) ob;
    LPCWSTR st = dm->GetFriendlyName(FALSE);

    if(st != NULL)
    {
        WszOutputDebugString(st);
    }
    else
    {
        WszOutputDebugString(L"<Domain with no Name>");
    }
}



/*******************************************************************/
const char* GetClassName(void* ptr)
{
    CONTRACTL
    {
        NOTHROW;
        GC_NOTRIGGER;
    }
    CONTRACTL_END;

    DefineFullyQualifiedNameForClass();
    LPCUTF8 clsName = GetFullyQualifiedNameForClass(((EEClass*)ptr));
    // Note we're returning local stack space - this should be OK for using in the debugger though
    return (const char *) clsName;
}



#ifdef LOGGING
void LogStackTrace()
{
    WRAPPER_CONTRACT;

    PrintCallbackData cbd = {0, 0, 1};
    GetThread()->StackWalkFrames(PrintStackTraceCallback, &cbd,ALLOW_ASYNC_STACK_WALK, 0);
}
#endif

#endif // #ifndef DACCESS_COMPILE
#endif //_DEBUG

