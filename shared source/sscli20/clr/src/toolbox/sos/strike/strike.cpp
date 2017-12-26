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

/****************************************************************************
* STRIKE.C                                                                  *
*   Routines for the NTSD extension - STRIKE                                *
*                                                                           *
* History:                                                                  *
*   09/07/99  larrysu     Created                                           *
*                                                                           *
*                                                                           *
\***************************************************************************/
#include <wchar.h>
#include <windows.h>

#define NOEXTAPI
#define KDEXT_64BIT
#include <wdbgexts.h>
#undef DECLARE_API

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <malloc.h>
#include <stddef.h>

#include "strike.h"

#ifndef STRESS_LOG
#define STRESS_LOG
#endif // STRESS_LOG
#define STRESS_LOG_READONLY
#include "stresslog.h"
#include <dbghelp.h>

#include "util.h"

#include "corhdr.h"
#include "cor.h"
#include "dacprivate.h"

#define  CORHANDLE_MASK 0x1
#define SWITCHED_OUT_FIBER_OSID 0xbaadf00d;

#define DEFINE_EXT_GLOBALS

#include "data.h"
#include "disasm.h"

#include "PredefTlsSlot.h"

BOOL CallStatus;
int DebugVersionDll = -1;
BOOL ControlC = FALSE;

IMetaDataDispenserEx *pDisp = NULL;
WCHAR g_mdName[mdNameLen];
HMODULE g_hInstance=NULL;

#pragma warning(disable:4244)   // conversion from 'unsigned int' to 'unsigned short', possible loss of data
#pragma warning(disable:4189)   // local variable is initialized but not referenced

#undef assert
#define assert(a)


#undef _ASSERTE
#define _ASSERTE(a) do {} while (0)

#undef assert
#pragma warning(default:4244)
#pragma warning(default:4189)


#ifndef CONTEXT_ALL
#define CONTEXT_ALL CONTEXT_FULL
#endif // CONTEXT_ALL

// Size of a fixed array:
#ifndef ARRAYSIZE
#define ARRAYSIZE(a)		(sizeof(a)/sizeof((a)[0]))
#endif // !ARRAYSIZE


#include "safemath.h"

#define MINIDUMP_NOT_SUPPORTED()   \
    if (IsMiniDumpFile())      \
    {                          \
        ExtOut ("This command is not supported in a minidump without full memory\n"); \
        ExtOut ("To try the command anyway, run !MinidumpMode 0\n"); \
        return Status;         \
    }         

BOOL WINAPI DllMain(HANDLE hInstance, DWORD dwReason, LPVOID lpReserved)
{
    if (dwReason == DLL_PROCESS_ATTACH)
    {
        g_hInstance = (HMODULE) hInstance;
    }
    return true;
}

DECLARE_API (MinidumpMode)
{
    INIT_API ();
    DWORD_PTR Value=0;

    CMDValue arg[] = {
        // vptr, type
        {&Value, COHEX}
    };

    size_t nArg;
    if (!GetCMDOption(args,NULL,0,
                      arg,sizeof(arg)/sizeof(CMDValue),&nArg)) {
        return Status;
    }    

    if (nArg == 0)
    {
        // Print status of current mode
       ExtOut ("Current mode: %s - unsafe minidump commands are %s.\n",
               g_InMinidumpSafeMode ? "1" : "0",
               g_InMinidumpSafeMode ? "disabled" : "enabled");
    }
    else
    {
        if (Value != 0 && Value != 1)
        {
            ExtOut ("Mode must be 0 or 1\n");
            return Status;
        }

        g_InMinidumpSafeMode = (BOOL) Value;
        ExtOut ("Unsafe minidump commands are %s.\n",
                g_InMinidumpSafeMode ? "disabled" : "enabled");
    }

    return Status;
}

/**********************************************************************\
* Routine Description:                                                 *
*                                                                      *
*    This function is called to get the MethodDesc for a given eip     *  
*                                                                      *
\**********************************************************************/
DECLARE_API (IP2MD)
{
    INIT_API ();

    MINIDUMP_NOT_SUPPORTED();    
    
    DWORD_PTR IP = GetExpression(args);
    if (IP == 0)
    {
        ExtOut("%s is not IP\n", args);
        return Status;
    }

    CLRDATA_ADDRESS dwStartAddr = (CLRDATA_ADDRESS) GetExpression(args);    
    DacpMethodDescData MethodDescData;
    if ((Status = MethodDescData.RequestFromIP(g_clrData,dwStartAddr)) != S_OK)
    {
        ExtOut("Failed to request MethodData, not in JIT code range\n");
        return Status;
    }

    ExtOut("MethodDesc: %p\n",MethodDescData.MethodDescPtr);
    DumpMDInfo((DWORD_PTR) MethodDescData.MethodDescPtr);

    return Status;
}


/**********************************************************************\
* Routine Description:                                                 *
*                                                                      *
*    This function displays the stack trace.  It looks at each DWORD   *  
*    on stack.  If the DWORD is a return address, the symbol name or
*    managed function name is displayed.                               *
*                                                                      *
\**********************************************************************/
void DumpStackInternal(DumpStackFlag *pDSFlag)
{    
    ReloadSymbolWithLineInfo();
    
    ULONG64 StackOffset;
    g_ExtRegisters->GetStackOffset (&StackOffset);
    if (pDSFlag->top == 0) {
        pDSFlag->top = (DWORD_PTR)StackOffset;
    }
    size_t value;
    while (g_ExtData->ReadVirtual(pDSFlag->top,&value,sizeof(size_t),NULL) != S_OK) {
        if (IsInterrupt())
            return;
        pDSFlag->top = NextOSPageAddress (pDSFlag->top);
    }

    
    if (pDSFlag->end == 0)
    {
        ExtOut("TEB information is not available so a stack size of 0xFFFF is assumed\n");
        pDSFlag->end = pDSFlag->top + 0xFFFF;
    }
    
    if (pDSFlag->end < pDSFlag->top)
    {
        ExtOut ("Wrong option: stack selection wrong\n");
        return;
    }

    DumpStackDummy (*pDSFlag);
}

// (MAX_STACK_FRAMES is also used by x86 to prevent infinite loops in _EFN_StackTrace)
#define MAX_STACK_FRAMES 1000

DECLARE_API (DumpStack)
{
    INIT_API();

    MINIDUMP_NOT_SUPPORTED();

    DumpStackFlag DSFlag;
    DSFlag.fEEonly = FALSE;
    DSFlag.top = 0;
    DSFlag.end = 0;

    CMDOption option[] = {
        // name, vptr, type, hasValue
        {"-EE", &DSFlag.fEEonly, COBOOL, FALSE},
    };
    CMDValue arg[] = {
        // vptr, type
        {&DSFlag.top, COHEX},
        {&DSFlag.end, COHEX}
    };
    size_t nArg;
    if (!GetCMDOption(args,option,sizeof(option)/sizeof(CMDOption),
                      arg,sizeof(arg)/sizeof(CMDValue),&nArg)) {
        return Status;
    }    

    ULONG id = 0;
    g_ExtSystem->GetCurrentThreadSystemId (&id);
    ExtOut ("OS Thread Id: 0x%x ", id);
    g_ExtSystem->GetCurrentThreadId (&id);
    ExtOut ("(%d)\n", id);

    DumpStackInternal (&DSFlag);
    return Status;
}


/**********************************************************************\
* Routine Description:                                                 *
*                                                                      *
*    This function displays the stack trace for threads that EE knows  *  
*    from ThreadStore.                                                 *
*                                                                      *
\**********************************************************************/
DECLARE_API (EEStack)
{
    INIT_API();    

    MINIDUMP_NOT_SUPPORTED();  

    DumpStackFlag DSFlag;
    DSFlag.fEEonly = FALSE;
    DSFlag.top = 0;
    DSFlag.end = 0;

    BOOL bShortList = FALSE;
    
    CMDOption option[] = {
        // name, vptr, type, hasValue
        {"-EE", &DSFlag.fEEonly, COBOOL, FALSE},
        {"-short", &bShortList, COBOOL, FALSE}
    };    

    if (!GetCMDOption(args,option,sizeof(option)/sizeof(CMDOption),NULL,0,NULL)) 
    {
        return Status;
    }

    ULONG Tid;
    g_ExtSystem->GetCurrentThreadId(&Tid);

    // We need to support minidumps for this command.
    BOOL bMiniDump = IsMiniDumpFile();

    DacpThreadStoreData ThreadStore;
    if ((Status = ThreadStore.Request(g_clrData)) != S_OK)
    {
        ExtOut("Failed to request ThreadStore\n");
        return Status;
    }    

    CLRDATA_ADDRESS CurThread = ThreadStore.firstThread;
    while (CurThread)
    {
        if (IsInterrupt())
            break;

        DacpThreadData Thread;        
        if ((Status = Thread.Request(g_clrData, CurThread, !bMiniDump)) != S_OK)
        {
            ExtOut("Failed to request Thread at %p\n", CurThread);
            return Status;
        }

        ULONG id=0;
        if (g_ExtSystem->GetThreadIdBySystemId (Thread.osThreadId, &id) != S_OK)
        {
            CurThread = Thread.nextThread;    
            continue;
        }
        
        ExtOut ("---------------------------------------------\n");
        ExtOut ("Thread %3d\n", id);
        BOOL doIt = FALSE;

        
#define TS_Hijacked 0x00000080

        if (!bShortList) 
        {
            doIt = TRUE;
        }
        else if ((Thread.lockCount > 0) || (Thread.state & TS_Hijacked)) 
        {             
            doIt = TRUE;
        }
        else 
        {
            ULONG64 IP;
            g_ExtRegisters->GetInstructionOffset (&IP);
            JitType jitType;
            DWORD_PTR methodDesc;
            DWORD_PTR gcinfoAddr;
            IP2MethodDesc ((DWORD_PTR)IP, methodDesc, jitType, gcinfoAddr);
            if (methodDesc)
            {
                doIt = TRUE;
            }
        }
        
        if (doIt) 
        {
            g_ExtSystem->SetCurrentThreadId(id);
            DSFlag.top = 0;
            DSFlag.end = 0;
            DumpStackInternal (&DSFlag);
        }

        CurThread = Thread.nextThread;
    }

    g_ExtSystem->SetCurrentThreadId(Tid);
    return Status;
}

/**********************************************************************\
* Routine Description:                                                 *
*                                                                      *
*    This function is called to dump the address and name of all       *
*    Managed Objects on the stack.                                     *  
*                                                                      *
\**********************************************************************/
DECLARE_API(DumpStackObjects)
{
    INIT_API();
    
    MINIDUMP_NOT_SUPPORTED();
    
    size_t StackTop = 0;
    size_t StackBottom = 0;
    StringHolder exprTop,exprBottom;

    BOOL bVerify = FALSE;
    CMDOption option[] = {
        // name, vptr, type, hasValue
        {"-verify", &bVerify, COBOOL, FALSE}
    };    
    CMDValue arg[] = {
        // vptr, type
        {&exprTop.data, COSTRING},
        {&exprBottom.data, COSTRING}
    };
    size_t nArg;

    if (!GetCMDOption(args,option,sizeof(option)/sizeof(CMDOption),
                      arg,sizeof(arg)/sizeof(CMDValue),&nArg)) {
        return Status;
    }
    
    if (nArg==0)
    {
        ULONG64 StackOffset;
        g_ExtRegisters->GetStackOffset (&StackOffset);

        StackTop = (DWORD_PTR)StackOffset;
    }
    else
    {
        StackTop = GetExpression(exprTop.data);
            if (StackTop == 0)
            {
            ExtOut ("wrong option: %s\n", exprTop.data);
                return Status;
            }

        if (nArg==2)
            {
            StackBottom = GetExpression(exprBottom.data);
                if (StackBottom == 0)
                {
                ExtOut ("wrong option: %s\n", exprBottom.data);
                    return Status;
                }
            }
        }
    
    if (StackBottom == 0)
        StackBottom = StackTop + 0xFFFF;
    
    if (StackBottom < StackTop)
    {
        ExtOut ("Wrong option: stack selection wrong\n");
        return Status;
    }


    // Print thread ID.

    ULONG id = 0;
    g_ExtSystem->GetCurrentThreadSystemId (&id);
    ExtOut ("OS Thread Id: 0x%x ", id);
    g_ExtSystem->GetCurrentThreadId (&id);
    ExtOut ("(%d)\n", id);
    
    DumpStackObjectsHelper (StackTop, StackBottom, bVerify);
    return Status;
}

/**********************************************************************\
* Routine Description:                                                 *
*                                                                      *
*    This function is called to dump the contents of a MethodDesc      *
*    for a given address                                               *  
*                                                                      *
\**********************************************************************/
DECLARE_API(DumpMD)
{
    INIT_API();
    MINIDUMP_NOT_SUPPORTED();
    
    DWORD_PTR dwStartAddr;

    dwStartAddr = GetExpression(args);        
    DumpMDInfo (dwStartAddr);
    
    return Status;
}

BOOL GatherDynamicInfo (DWORD_PTR DynamicMethodObj, DacpObjectData *codeArray, 
                        DacpObjectData *tokenArray, DWORD_PTR *ptokenArrayAddr)
{
    BOOL bRet = FALSE;
    int iOffset;
    DacpObjectData objData; // temp object

    if (codeArray == NULL || tokenArray == NULL)
        return bRet;
    
    if (objData.Request(g_clrData,(CLRDATA_ADDRESS)DynamicMethodObj) != S_OK)
        return bRet;
    
    iOffset = GetObjFieldOffset (DynamicMethodObj, objData.MethodTable, L"m_resolver");
    if (iOffset <= 0)
        return bRet;
    
    DWORD_PTR resolverPtr;
    if (FAILED(MOVE (resolverPtr, DynamicMethodObj + iOffset)))
        return bRet;

    if (objData.Request(g_clrData,(CLRDATA_ADDRESS)resolverPtr) != S_OK)
        return bRet;
    
    iOffset = GetObjFieldOffset (resolverPtr, objData.MethodTable, L"m_code");
    if (iOffset <= 0)
        return bRet;

    DWORD_PTR codePtr;
    if (FAILED(MOVE (codePtr, resolverPtr + iOffset)))
        return bRet;

    if (codeArray->Request(g_clrData,(CLRDATA_ADDRESS)codePtr) != S_OK)
        return bRet;
    
    if (codeArray->dwComponentSize != 1)
        return bRet;
        
    // We also need the resolution table
    iOffset = GetObjFieldOffset (resolverPtr, objData.MethodTable, L"m_scope");
    if (iOffset <= 0)
        return bRet;

    DWORD_PTR scopePtr;
    if (FAILED(MOVE (scopePtr, resolverPtr + iOffset)))
        return bRet;

    if (objData.Request(g_clrData,(CLRDATA_ADDRESS)scopePtr) != S_OK)
        return bRet;
    
    iOffset = GetObjFieldOffset (scopePtr, objData.MethodTable, L"m_tokens");
    if (iOffset <= 0)
        return bRet;

    DWORD_PTR tokensPtr;
    if (FAILED(MOVE (tokensPtr, scopePtr + iOffset)))
        return bRet;

    if (objData.Request(g_clrData,(CLRDATA_ADDRESS)tokensPtr) != S_OK)
        return bRet;
    
    iOffset = GetObjFieldOffset (tokensPtr, objData.MethodTable, L"_items");
    if (iOffset <= 0)
        return bRet;

    DWORD_PTR itemsPtr;
    MOVE (itemsPtr, tokensPtr + iOffset);

    *ptokenArrayAddr = itemsPtr;
    
    if (tokenArray->Request(g_clrData,(CLRDATA_ADDRESS)itemsPtr) != S_OK)
        return bRet;

    bRet = TRUE; // whew.
    return bRet;
}
            
DECLARE_API(DumpIL)
{
    INIT_API();
    MINIDUMP_NOT_SUPPORTED();
    DWORD_PTR dwStartAddr;
    DWORD_PTR dwDynamicMethodObj = NULL;
    
    dwStartAddr = GetExpression(args);        
    if (dwStartAddr == 0)
    {
        ExtOut("Must pass a valid expression\n");
        return Status;
    }

    if (!g_snapshot.Build())
    {
        ExtOut("Unable to build snapshot of the garbage collector state\n");
        return Status;
    }

    if (g_snapshot.GetHeap(dwStartAddr) != NULL)
    {
        dwDynamicMethodObj = dwStartAddr;
    }
    
    if (dwDynamicMethodObj == NULL)
    {
        // We have been given a MethodDesc
        DacpMethodDescData MethodDescData;
        if (MethodDescData.Request(g_clrData,(CLRDATA_ADDRESS)dwStartAddr) != S_OK)
        {
            ExtOut("%p is not a MethodDesc\n", (ULONG64)dwStartAddr);
            return Status;
        }

        if (MethodDescData.bIsDynamic && MethodDescData.managedDynamicMethodObject)
        {
            dwDynamicMethodObj = (DWORD_PTR) MethodDescData.managedDynamicMethodObject;
            if (dwDynamicMethodObj == NULL)
            {
                ExtOut("Unable to print IL for DynamicMethodDesc %p\n", (ULONG64) dwDynamicMethodObj);
                return Status;
            }
        }
        else
        {
            // This is not a dynamic method, print the IL for it.
            // Get the module
            DacpModuleData dmd;    
            if (dmd.Request(g_clrData, MethodDescData.ModulePtr) != S_OK)
            {
                ExtOut ("Unable to get module\n");
                return Status;
            }

            IMetaDataImport *pImport = MDImportForModule(&dmd);
            if (pImport == NULL)
            {
                ExtOut ("bad import\n");
                return Status;
            }

            ULONG pRva;
            DWORD dwFlags;
            if (pImport->GetRVA(MethodDescData.MDToken, &pRva, &dwFlags) != S_OK)
            {
                ExtOut ("error in import\n");
                return Status;
            }    

            CLRDATA_ADDRESS ilAddrClr;
            if (DacpModuleData::FindIL(g_clrData, MethodDescData.ModulePtr, pRva, &ilAddrClr) != S_OK)
            {
                ExtOut ("FindIL failed\n");
                return Status;
            }

            DWORD_PTR ilAddr = (DWORD_PTR) ilAddrClr;
            ULONG Size = GetILSize(ilAddr);
            if (Size == 0)
            {
                ExtOut ("error decoding IL\n");
                return Status;
            }

            ExtOut("ilAddr = %p\n", (ULONG64) ilAddr);

        
            // Read the memory into a local buffer
            BYTE *pArray = new BYTE[Size];
            Status = g_ExtData->ReadVirtual(ilAddr, pArray, Size, NULL);
            if (Status != S_OK)
            {
                ExtOut ("Failed to read memory\n");
                return Status;
            }
            
            DecodeIL(pImport, pArray, Size);
            pImport->Release();
            delete [] pArray;
        }
    }
    
    if (dwDynamicMethodObj != NULL)
    {
        // We have a DynamicMethod managed object, let us visit the town and paint.        
        DacpObjectData codeArray;
        DacpObjectData tokenArray;
        DWORD_PTR tokenArrayAddr;
        if (!GatherDynamicInfo (dwDynamicMethodObj, &codeArray, &tokenArray, &tokenArrayAddr))
        {
            ExtOut ("Error gathering dynamic info from object at %p\n", (ULONG64) dwDynamicMethodObj);
            return Status;
        }
        
        // Read the memory into a local buffer
        BYTE *pArray = new BYTE[codeArray.dwNumComponents];
        if (pArray == NULL)
        {
            ExtOut ("Not enough memory to read IL\n");
            return Status;
        }
        
        Status = g_ExtData->ReadVirtual(codeArray.ArrayDataPtr, pArray, codeArray.dwNumComponents, NULL);
        if (Status != S_OK)
        {
            ExtOut ("Failed to read memory\n");
            delete [] pArray;
            return Status;
        }

        // Now we have a local copy of the IL, and a managed array for token resolution.
        // Visit our IL parser with this info.        
        ExtOut ("This is dynamic IL. Exception info is not reported at this time.\n");
        ExtOut ("If a token is unresolved, run \"!do <addr>\" on the addr given\n");
        ExtOut ("in parenthesis. You can also look at the token table yourself, by\n");
        ExtOut ("running \"!DumpArray %p\".\n\n", (ULONG64) tokenArrayAddr);
        DecodeDynamicIL(pArray, codeArray.dwNumComponents, tokenArray);
        
        delete [] pArray;                
    }    
    return Status;
}

void DumpSigWorker (
        DWORD_PTR dwSigAddr,
        DWORD_PTR dwModuleAddr,
        BOOL fMethod)
{
    //
    // Find the length of the signature and copy it into the debugger process.
    //

    // PREFAST doesn't like the ToDestroy syntax

    ULONG cbSig = 0;
    const ULONG cbSigInc = 256;
    PCOR_SIGNATURE pSig = (PCOR_SIGNATURE)malloc(cbSigInc);
    if (pSig == NULL)
    {
        ReportOOM();        
        return;
    }
    
    ToDestroy des((void**)&pSig);
    CQuickBytes sigString;

    for (;;)
    {
        if (IsInterrupt())
            return;

        ULONG cbCopied;
        if (!SafeReadMemory(dwSigAddr + cbSig, pSig + cbSig, cbSigInc, &cbCopied))
            return;
        cbSig += cbCopied;

        sigString.ReSize(0);
        GetSignatureStringResults result;
        if (fMethod)
            result = GetMethodSignatureString(pSig, cbSig, dwModuleAddr, &sigString);
        else
            result = GetSignatureString(pSig, cbSig, dwModuleAddr, &sigString);

        if (GSS_ERROR == result)
            return;

        if (GSS_SUCCESS == result)
            break;

        // If we didn't get the full amount back, and we failed to parse the
        // signature, it's not valid because of insufficient data
        if (cbCopied < 256)
        {
            ExtOut("Invalid signature\n");
            return;
        }

        PCOR_SIGNATURE pSigNew = (PCOR_SIGNATURE)realloc(pSig, cbSig+cbSigInc);

        if (pSigNew == NULL)
        {
            ExtOut("Out of memory\n");
            return;
        }
        
        pSig = pSigNew;
    }

    ExtOut("%S\n", (PCWSTR)sigString.Ptr());
}


/**********************************************************************\
* Routine Description:                                                 *
*                                                                      *
*    This function is called to dump a method signature                *
*                                                                      *
\**********************************************************************/
DECLARE_API(DumpMethodSig)
{
    INIT_API();

    MINIDUMP_NOT_SUPPORTED();
    
    //
    // Fetch arguments
    //

    StringHolder sigExpr;
    StringHolder moduleExpr;
    CMDValue arg[] = {
        {&sigExpr.data, COSTRING},
        {&moduleExpr.data, COSTRING}
    };
    size_t nArg;
    
    if (!GetCMDOption(args,NULL,0,arg,sizeof(arg)/sizeof(CMDValue),&nArg))
        return Status;

    if (nArg != 2)
    {
        ExtOut("DumpMethodSig <sigaddr> <moduleaddr>\n");
        return Status;
    }

    DWORD_PTR dwSigAddr = GetExpression(sigExpr.data);        
    DWORD_PTR dwModuleAddr = GetExpression(moduleExpr.data);

    if (dwSigAddr == 0 || dwModuleAddr == 0)
    {
        ExtOut ("Invalid parameters %s %s\n", sigExpr.data, moduleExpr.data);
        return Status;
    }
    
    DumpSigWorker(dwSigAddr, dwModuleAddr, TRUE);
    return Status;
}



/**********************************************************************\
* Routine Description:                                                 *
*                                                                      *
*    This function is called to dump a non-method signature            *
*                                                                      *
\**********************************************************************/
DECLARE_API(DumpSig)
{
    INIT_API();

    MINIDUMP_NOT_SUPPORTED();
    

    //
    // Fetch arguments
    //

    StringHolder sigExpr;
    StringHolder moduleExpr;
    CMDValue arg[] = {
        {&sigExpr.data, COSTRING},
        {&moduleExpr.data, COSTRING}
    };
    size_t nArg;
    
    if (!GetCMDOption(args,NULL,0,arg,sizeof(arg)/sizeof(CMDValue),&nArg))
        return Status;

    if (nArg != 2)
    {
        ExtOut("DumpSig <sigaddr> <moduleaddr>\n");
        return Status;
    }

    DWORD_PTR dwSigAddr = GetExpression(sigExpr.data);        
    DWORD_PTR dwModuleAddr = GetExpression(moduleExpr.data);

    if (dwSigAddr == 0 || dwModuleAddr == 0)
    {
        ExtOut ("Invalid parameters %s %s\n", sigExpr.data, moduleExpr.data);
        return Status;
    }

    DumpSigWorker(dwSigAddr, dwModuleAddr, FALSE);
    return Status;
}


/**********************************************************************\
* Routine Description:                                                 *
*                                                                      *
*    This function is called to dump the contents of an EEClass from   *  
*    a given address
*                                                                      *
\**********************************************************************/
DECLARE_API (DumpClass)
{
    DWORD_PTR dwStartAddr = 0;
    
    
    INIT_API();

    MINIDUMP_NOT_SUPPORTED();

    CMDValue arg[] = {
        // vptr, type
        {&dwStartAddr, COHEX}
    };
    size_t nArg;
    if (!GetCMDOption(args,NULL,0,
                      arg,sizeof(arg)/sizeof(CMDValue),&nArg)) {
        return Status;
    }
    
    if (nArg == 0) {
        ExtOut ("Missing EEClass address\n");
        return Status;
    }
    DWORD_PTR dwAddr = dwStartAddr;
    DacpEEClassData EECls;
    if ((Status=EECls.Request(g_clrData,(CLRDATA_ADDRESS) dwAddr))!=S_OK)
    {
        ExtOut("Invalid EEClass address\n");
        return Status;
    }            
    
    ExtOut("Class Name: ");
    NameForMT_s((DWORD_PTR) EECls.MethodTable, g_mdName,mdNameLen);
    ExtOut("%S", g_mdName);
    ExtOut ("\n");


    WCHAR fileName[MAX_PATH];
    FileNameForModule((DWORD_PTR) EECls.Module,fileName);
    ExtOut("mdToken: %p (%S)\n",(ULONG64)EECls.cl, fileName);

    DacpMethodTableData mtdata;
    if ((Status=mtdata.Request(g_clrData,(CLRDATA_ADDRESS) EECls.MethodTable))!=S_OK)
    {
        ExtOut("EEClass has an invalid MethodTable address\n");
        return Status;
    }            

    CLRDATA_ADDRESS ParentEEClass = NULL;
    if (mtdata.ParentMethodTable)
    {
        if ((Status=mtdata.Request(g_clrData,(CLRDATA_ADDRESS) mtdata.ParentMethodTable))!=S_OK)
        {
            ExtOut("EEClass has an invalid MethodTable address\n");
            return Status;
        }                     
        ParentEEClass = mtdata.Class;
    }

    ExtOut("Parent Class: %p\n",(ULONG64)ParentEEClass);
    
    ExtOut("Module: %p\r\n",(ULONG64)EECls.Module);
    
    ExtOut("Method Table: %p\n",(ULONG64)EECls.MethodTable);

    ExtOut("Vtable Slots: %x\n",EECls.wNumVtableSlots);

    ExtOut("Total Method Slots: %x\n",EECls.wNumMethodSlots);

    ExtOut("Class Attributes: %x  ",EECls.dwAttrClass);

    if (IsTdInterface(EECls.dwAttrClass))
    {
        ExtOut ("Interface, ");
    }
    if (IsTdAbstract(EECls.dwAttrClass))
    {
        ExtOut ("Abstract, ");
    }
    if (IsTdImport(EECls.dwAttrClass))
    {
        ExtOut ("ComImport, ");
    }
    
    ExtOut ("\n");        

    ExtOut("NumInstanceFields: %x\n", EECls.wNumInstanceFields);
    ExtOut("NumStaticFields: %x\n", EECls.wNumStaticFields);

    if (EECls.wThreadStaticsSize)
    {
        ExtOut("ThreadStaticOffset: %x\n", EECls.wThreadStaticOffset);
        ExtOut("ThreadStaticsSize: %x\n", EECls.wThreadStaticsSize);
    }

    if (EECls.wContextStaticsSize)
    {
        ExtOut("ContextStaticOffset: %x\n", EECls.wContextStaticOffset);
        ExtOut("ContextStaticsSize: %x\n", EECls.wContextStaticsSize);
    }

    
    if (EECls.wNumInstanceFields + EECls.wNumStaticFields > 0)
    {
        // ExtOut ("FieldDesc*: %p\n", (ULONG64)pEECls->m_pFieldDescList);
        DisplayFields(&EECls);
    }

    return Status;
}



/**********************************************************************\
* Routine Description:                                                 *
*                                                                      *
*    This function is called to dump the contents of a MethodTable     *  
*    from a given address                                              *
*                                                                      *
\**********************************************************************/
/*
DECLARE_API (DumpEEHash)
{
    INIT_API();
    
    DWORD_PTR dwTableAddr = NULL;
    size_t nitem = 1;
    
    CMDOption option[] = {
        // name, vptr, type, hasValue
        {"-length", &nitem, COSIZE_T, TRUE}
    };
    CMDValue arg[] = {
        // vptr, type
        {&dwTableAddr, COHEX}
    };
    size_t nArg;
    if (!GetCMDOption(args,option,sizeof(option)/sizeof(CMDOption),
                      arg,sizeof(arg)/sizeof(CMDValue),&nArg)) {
        return Status;
    }

    if (dwTableAddr == 0) {
        goto Exit;
    }
    EEHashTable vTable;
    vTable.Fill(dwTableAddr);
    ExtOut ("NumBuckets: %d\n", vTable.m_dwNumBuckets);
    ExtOut ("NumEntries: %d\n", vTable.m_dwNumEntries);
    DWORD n;
    size_t dwBucketAddr;
    EEHashEntry vEntry;
    size_t offsetKey = GetFieldOffsetEx(idx_EEHashEntry__Key, EEHashEntry, "Key");
    ExtOut ("Bucket   Data     Key\n");
    for (n = 0; n < vTable.m_dwNumBuckets; n ++) {
        if (IsInterrupt())
            break;
        dwBucketAddr = (size_t)vTable.m_pBuckets + n * sizeof(PVOID);
        moveN (dwBucketAddr, dwBucketAddr);
        while (dwBucketAddr) {
            if (IsInterrupt())
                break;
            DWORD_PTR dwAddr = dwBucketAddr;
            vEntry.Fill(dwAddr);
            size_t Key;
            ExtOut ("%p %p ", (ULONG64)dwBucketAddr, (ULONG64)vEntry.Data);
            dwAddr = dwBucketAddr + offsetKey;
            for (size_t i = 0; i < nitem; i ++) {
                moveN (Key, dwAddr+i*sizeof(size_t));
                ExtOut ("%p ", (ULONG64)Key);
            }
            ExtOut ("\n");
            dwBucketAddr = (size_t)vEntry.pNext;
        }
    }

Exit:
    return Status;
}
*/


/**********************************************************************\
* Routine Description:                                                 *
*                                                                      *
*    This function is called to dump the contents of a MethodTable     *  
*    from a given address                                              *
*                                                                      *
\**********************************************************************/
DECLARE_API (DumpMT)
{
    DWORD_PTR dwStartAddr=0;
    DWORD_PTR dwOriginalAddr;
    
    INIT_API();

    MINIDUMP_NOT_SUPPORTED();
    
    
    BOOL bDumpMDTable = FALSE;
    CMDOption option[] = {
        // name, vptr, type, hasValue
        {"-MD", &bDumpMDTable, COBOOL, FALSE}
    };
    CMDValue arg[] = {
        // vptr, type
        {&dwStartAddr, COHEX}
    };
    size_t nArg;
    if (!GetCMDOption(args,option,sizeof(option)/sizeof(CMDOption),
                      arg,sizeof(arg)/sizeof(CMDValue),&nArg)) {
        return Status;
    }

    if (nArg == 0) {
        ExtOut ("Missing MethodTable address\n");
        return Status;
    }

    dwOriginalAddr = dwStartAddr;
    dwStartAddr = dwStartAddr&~3;
    
    if (!IsMethodTable (dwStartAddr))
    {
        ExtOut ("%p is not a MethodTable\n", (ULONG64)dwOriginalAddr);
        return Status;
    }
 
    DacpMethodTableData vMethTable;
    vMethTable.Request(g_clrData,(CLRDATA_ADDRESS) dwStartAddr);    

    if (vMethTable.bIsFree) {
        ExtOut ("Free MethodTable\n");
        return Status;
    }
    
    ExtOut("EEClass: %p\n",(ULONG64)vMethTable.Class);

    DacpEEClassData eeclass;
    if (eeclass.Request(g_clrData,vMethTable.Class)!=S_OK)
    {
        ExtOut("Unable to retrieve EEClass\n");
        return Status;
    }            
    
    ExtOut("Module: %p\r\n",(ULONG64)eeclass.Module);

    WCHAR fileName[MAX_PATH];
    NameForMT_s (dwStartAddr, g_mdName,mdNameLen);
    // NameForToken_s((DWORD_PTR)eeclass.Module, eeclass.cl, g_mdName, mdNameLen);
    ExtOut ("Name: %S\n", g_mdName);
    FileNameForModule ((DWORD_PTR) eeclass.Module, fileName);
    ExtOut("mdToken: %08x ", eeclass.cl);
    ExtOut( " (%ws)\n",
             fileName[0] ? fileName : L"Unknown Module" );    
    
    ExtOut ("BaseSize: 0x%x\n",vMethTable.BaseSize);    
    ExtOut ("ComponentSize: 0x%x\n",vMethTable.ComponentSize);
    
    ExtOut("Number of IFaces in IFaceMap: %x\r\n",
            vMethTable.wNumInterfaces);
    ExtOut("Slots in VTable: %d\r\n",vMethTable.wTotalVtableSlots);

    if (bDumpMDTable)
    {
        ExtOut ("--------------------------------------\n");
        ExtOut ("MethodDesc Table\n");
        ExtOut ("%" POINTERSIZE "s ", "Entry");
#ifdef _X86_         
        ExtOut ("%10s ", "MethodDesc");
#else
        ExtOut ("%" POINTERSIZE "s ", "MethodDesc");
#endif
        ExtOut ("%8s %s\n","JIT", "Name");
        
        // ExtOut ("  Entry  MethodDesc   JIT   Name\n");
        for (DWORD n = 0; n < vMethTable.wTotalVtableSlots; n ++)
        {
            DWORD_PTR entry=0;
            JitType jitType;
            DWORD_PTR methodDesc=0;
            DWORD_PTR gcinfoAddr;

            CLRDATA_ADDRESS slotLocation;
            if (DacpMethodTableData::GetSlot(g_clrData,dwStartAddr,n,&slotLocation) != S_OK)
            {
                ExtOut ("<error getting slot %d>\n", n);
                continue;
            }
            else
            {
                DWORD_PTR tmpEntry = (DWORD_PTR) slotLocation;
                moveN (entry, tmpEntry);
            }

            IP2MethodDesc (entry, methodDesc, jitType, gcinfoAddr);
            ExtOut ("%p ", (ULONG64)entry);
#ifdef _X86_            
            ExtOut ("  %p ", (ULONG64)methodDesc);
#else
            ExtOut ("%p ", (ULONG64)methodDesc);
#endif

            if (jitType == UNKNOWN && methodDesc != NULL)
            {
                // We can get a more accurate jitType from NativeCodeAddr of the methoddesc,
                // because the methodtable entry hasn't always been patched.
                DacpMethodDescData tmpMethodDescData;
                if (tmpMethodDescData.Request(g_clrData,(CLRDATA_ADDRESS)methodDesc) == S_OK)
                {
                    DacpCodeHeaderData codeHeaderData;                        
                    if (codeHeaderData.Request(g_clrData,tmpMethodDescData.NativeCodeAddr) == S_OK)
                    {        
                        jitType = (JitType) codeHeaderData.JITType;
                    }
                }
            }

            char *pszJitType = "NONE";
            if (jitType == EJIT)
                pszJitType = "EJIT";
            else if (jitType == JIT)
                pszJitType = "JIT";
            else if (jitType == PJIT)
                pszJitType = "PreJIT";
            else
            {
                DacpMethodDescData MethodDescData;
                if (MethodDescData.Request(g_clrData,(CLRDATA_ADDRESS)methodDesc) == S_OK)
                {
                    // Is it an fcall?
                    if ((MethodDescData.NativeCodeAddr >=  moduleInfo[MSCORWKS].baseAddr) &&
                        ((MethodDescData.NativeCodeAddr <  (moduleInfo[MSCORWKS].baseAddr + moduleInfo[MSCORWKS].size))))
                    {
                        pszJitType = "FCALL";
                    }
                }
            }
            ExtOut ("%8s ", pszJitType);
                        
            NameForMD_s(methodDesc,g_mdName,mdNameLen);                        
            ExtOut ("%S\n", g_mdName);
        }
    }
    return Status;    
}

extern size_t Align (size_t nbytes);

HRESULT PrintVC (DWORD_PTR p_MT, DWORD_PTR p_Object, BOOL bPrintFields = TRUE)
{       
    HRESULT Status;
    DacpMethodTableData mtabledata;
    if ((Status = mtabledata.Request(g_clrData,(CLRDATA_ADDRESS) p_MT))!=S_OK)
    {
        return Status;
    }
    
    DWORD_PTR size = 0;
    if ((Status=DacpMethodTableData::GetMethodTableName(g_clrData,(CLRDATA_ADDRESS)p_MT,mdNameLen,g_mdName))!=S_OK)
    {
        return Status;
    }
    ExtOut("Name: %S\n", g_mdName);
    ExtOut("MethodTable %p\n",(ULONG64)p_MT);
    ExtOut("EEClass: %p\n", (ULONG64)mtabledata.Class);
    size = mtabledata.BaseSize;
    ExtOut("Size: %d(0x%x) bytes\n", size,size);

    DacpEEClassData EECls;
    if ((Status=EECls.Request(g_clrData,mtabledata.Class))!=S_OK)
    {
        ExtOut("Invalid EEClass address\n");
        return Status;
    }            

    FileNameForModule((DWORD_PTR) EECls.Module,g_mdName);
    ExtOut( " (%S)\n",
        g_mdName[0] ? g_mdName : L"Unknown Module" );

    if (bPrintFields)
    {
    ExtOut("Fields:\n");

    if (EECls.wNumInstanceFields + EECls.wNumStaticFields > 0)
    {
        DisplayFields(&EECls, p_Object, TRUE, TRUE);
    }
    }
    return S_OK;    
}

void PrintRuntimeTypeInfo (DWORD_PTR p_rtObject, const DacpObjectData & rtObjectData)
{
    // Get the method table
    int iOffset = GetObjFieldOffset (p_rtObject, rtObjectData.MethodTable, L"m_handle");
    if (iOffset > 0)
    {            
        DWORD_PTR mtPtr;
        if (MOVE (mtPtr, p_rtObject + iOffset) == S_OK)
        {
            NameForMT_s (mtPtr, g_mdName,mdNameLen);
            ExtOut ("Type MethodTable: %p\n",(ULONG64) mtPtr);
            ExtOut ("Type Name: %S\n",g_mdName);
        }                        
    }        
}

HRESULT PrintObj (DWORD_PTR p_Object, BOOL bPrintFields = TRUE)
{
    if (!IsObject(p_Object, TRUE))
    {
        ExtOut("<Note: this object has an invalid CLASS field>\n");
    }

    DacpObjectData objData;
    HRESULT Status;
    if ((Status=objData.Request(g_clrData,(CLRDATA_ADDRESS)p_Object)) != S_OK)
    {        
        ExtOut("Invalid object\n");
        return Status;
    }

    if (objData.ObjectType==OBJ_FREE)
    {
        ExtOut("Free Object\n");
        DWORD size = objData.Size;
        ExtOut("Size %d(0x%x) bytes\n", size, size);
        return S_OK;
    }
    
    DWORD_PTR size = 0;
    if ((Status=DacpObjectData::GetObjectClassName(g_clrData,(CLRDATA_ADDRESS)p_Object,mdNameLen,g_mdName))!=S_OK)
    {
        return Status;
    }
    ExtOut("Name: %S\n", g_mdName);
    ExtOut("MethodTable: %p\n",(ULONG64)objData.MethodTable);
    ExtOut("EEClass: %p\n", (ULONG64)objData.EEClass);
    size = objData.Size;
    ExtOut("Size: %d(0x%x) bytes\n", size,size);

    if (wcscmp(g_mdName, L"System.RuntimeType") == 0)
    {
        PrintRuntimeTypeInfo (p_Object, objData);                
    }

    if (wcscmp(g_mdName, L"System.RuntimeType+RuntimeTypeCache") == 0)
    {
        // Get the method table
        int iOffset = GetObjFieldOffset (p_Object, objData.MethodTable, L"m_runtimeType");
        if (iOffset > 0)
        {            
            DWORD_PTR rtPtr;
            if (MOVE (rtPtr, p_Object + iOffset) == S_OK)
            {
                DacpObjectData rtObjectData;
                if ((Status=rtObjectData.Request(g_clrData,(CLRDATA_ADDRESS)rtPtr)) != S_OK)
                {        
                    ExtOut("Error when reading RuntimeType field\n");
                    return Status;
                }

                PrintRuntimeTypeInfo (rtPtr, rtObjectData);
            }                        
        }        
    }
    
    DacpEEClassData EECls;
    if ((Status=EECls.Request(g_clrData,objData.EEClass))!=S_OK)
    {
        ExtOut("Invalid EEClass address\n");
        return Status;
    }            

    if (objData.ObjectType==OBJ_ARRAY)
    {
        ExtOut("Array: Rank %d, Number of elements %d, Type %s\n", objData.dwRank, objData.dwNumComponents, 
               ElementTypeName(objData.ElementType));
        NameForMT_s((DWORD_PTR) objData.ElementTypeHandle, g_mdName,mdNameLen);
        ExtOut("Element Type: %S\n", g_mdName);
        
        /*
        if (vArray.m_ElementType == ELEMENT_TYPE_CHAR)
        {
            ExtOut("Content:\n");
            DWORD_PTR num;
            moveN(num, pObj + sizeof(DWORD_PTR));
            PrintString(pObj + (2 * sizeof(DWORD_PTR)), TRUE, num);
            ExtOut("\n");
        }
        */
    }
    else
    {
        FileNameForModule((DWORD_PTR) EECls.Module,g_mdName);
        ExtOut( " (%S)\n",
            g_mdName[0] ? g_mdName : L"Unknown Module" );
    }

    if (objData.ObjectType == OBJ_STRING)
    {
        ExtOut("String: ");
        StringObjectContent(p_Object);
        ExtOut("\n");
    }
    else if (objData.ObjectType == OBJ_OBJECT)
    {
        ExtOut("Object\n");
    }    

    if (bPrintFields)
    {
        ExtOut("Fields:\n");
        if (EECls.wNumInstanceFields + EECls.wNumStaticFields > 0)
        {
            DisplayFields(&EECls, p_Object, TRUE, FALSE);
        }
        else
        {
            ExtOut("None\n");
        }
    }

    ThinLockInfo lockInfo;
    if ( HasThinLock (p_Object, &lockInfo) )
    {
        ExtOut("ThinLock owner %x (%p), Recursive %x\n", lockInfo.ThreadId, 
            (ULONG64) lockInfo.threadPtr, lockInfo.Recursion);
    }
    
    return S_OK;
}

BOOL IndicesInRange (DWORD * indices, DWORD * lowerBounds, DWORD * bounds, DWORD rank)
{
    for (int i = (int)rank - 1; i >= 0; i--)
    {
        if (indices[i] >= bounds[i] + lowerBounds[i])
        {
            if (i == 0)
            {
                return FALSE;
            }
            
            indices[i] = lowerBounds[i];
            indices[i - 1]++;
        }
    }

    return TRUE;
}

void ExtOutIndices (DWORD * indices, DWORD rank)
{
    for (DWORD i = 0; i < rank; i++)
    {
        ExtOut ("[%d]", indices[i]);
    }
}

size_t OffsetFromIndices (DWORD * indices, DWORD * lowerBounds, DWORD * bounds, DWORD rank)
{
    size_t multiplier = 1;
    size_t offset = 0;

    for (int i = (int)rank-1; i >= 0; i--) 
    {
        DWORD curIndex = indices[i] - lowerBounds[i];
        offset += curIndex * multiplier;
        multiplier *= bounds[i];
    }

    return offset;
}
HRESULT PrintArray(DacpObjectData& objData, DumpArrayFlags& flags, BOOL isPermSetPrint);
#ifdef _DEBUG
HRESULT PrintPermissionSet (DWORD_PTR p_PermSet)
{
    HRESULT Status = S_OK;

    DacpObjectData PermSetData;
    if ((Status=PermSetData.Request(g_clrData,(CLRDATA_ADDRESS)p_PermSet)) != S_OK)
    {        
        ExtOut("Invalid object\n");
        return Status;
    }

    
    CLRDATA_ADDRESS permSetMT = PermSetData.MethodTable;
    NameForMT_s ((DWORD_PTR) permSetMT, g_mdName,mdNameLen);                
    if (wcscmp (L"System.Security.PermissionSet", g_mdName) != 0 && wcscmp (L"System.Security.NamedPermissionSet", g_mdName) != 0)
    {
        ExtOut("Invalid PermissionSet object\n");
        return S_FALSE;
    }

    ExtOut ("PermissionSet object: %p\n", (ULONG64)p_PermSet);
    
    // Print basic info

    // Walk the fields, printing some fields in a special way.

    DacpEEClassData EECls;
    if ((Status=EECls.Request(g_clrData,PermSetData.EEClass))!=S_OK)
    {
        ExtOut("Invalid EEClass address\n");
        return Status;
    }            
    

    int iOffset = GetObjFieldOffset (p_PermSet, PermSetData.MethodTable, L"m_Unrestricted");
    
    if (iOffset > 0)        
    {
        BYTE unrestricted;
        MOVE (unrestricted, p_PermSet + iOffset);
        if (unrestricted)
            ExtOut ("Unrestricted: TRUE\n");
        else
            ExtOut ("Unrestricted: FALSE\n");
    }

    iOffset = GetObjFieldOffset (p_PermSet, PermSetData.MethodTable, L"m_permSet");
    if (iOffset > 0)
    {
        DWORD_PTR tbSetPtr;
        MOVE (tbSetPtr, p_PermSet + iOffset);
        if (tbSetPtr != NULL)
        {
            DacpObjectData tbSetData;
            if ((Status=tbSetData.Request(g_clrData,(CLRDATA_ADDRESS)tbSetPtr)) != S_OK)
            {        
                ExtOut("Invalid object\n");
                return Status;
            }

            iOffset = GetObjFieldOffset (tbSetPtr, tbSetData.MethodTable, L"m_Set");
            if (iOffset > 0)
            {
                DWORD_PTR PermsArrayPtr;
                MOVE (PermsArrayPtr, tbSetPtr + iOffset);
                if (PermsArrayPtr != NULL)
                {
                    // Print all the permissions in the array
                    DacpObjectData objData;
                    if ((Status=objData.Request(g_clrData,(CLRDATA_ADDRESS)PermsArrayPtr)) != S_OK)
                    {        
                        ExtOut("Invalid object\n");
                        return Status;
                    }
                    DumpArrayFlags flags;
                    flags.bDetail = TRUE;
                    return PrintArray(objData, flags, TRUE);
                }
            }

            iOffset = GetObjFieldOffset (tbSetPtr, tbSetData.MethodTable, L"m_Obj");
            if (iOffset > 0)
            {
                DWORD_PTR PermObjPtr;
                MOVE (PermObjPtr, tbSetPtr + iOffset);
                if (PermObjPtr != NULL)
                {
                    // Print the permission object
                    return PrintObj(PermObjPtr);
                }
            }
            

        }
    }
    return Status;
}
#endif // _DEBUG
/**********************************************************************\
* Routine Description:                                                 *
*                                                                      *
*    This function is called to dump the contents of an object from a  *  
*    given address
*                                                                      *
\**********************************************************************/
DECLARE_API(DumpArray)    
{
    INIT_API();

    DumpArrayFlags flags;
    
    MINIDUMP_NOT_SUPPORTED();
    
    CMDOption option[] = {
        // name, vptr, type, hasValue
        {"-start", &flags.startIndex, COSIZE_T, TRUE},
        {"-length", &flags.Length, COSIZE_T, TRUE},
        {"-details", &flags.bDetail, COBOOL, FALSE},
        {"-nofields", &flags.bNoFieldsForElement, COBOOL, FALSE}
    };
    
    CMDValue arg[] = {
        // vptr, type
        {&flags.strObject, COSTRING}
    };
    size_t nArg;
    if (!GetCMDOption(args,option,sizeof(option)/sizeof(CMDOption),
                      arg,sizeof(arg)/sizeof(CMDValue),&nArg)) {
        return Status;
    }

    DWORD_PTR p_Object = GetExpression (flags.strObject);
    if (p_Object == 0)
    {
        ExtOut ("Invalid parameter %s\n", flags.strObject);
        return Status;
    }

    if (!IsObject(p_Object, TRUE))
    {
        ExtOut("<Note: this object has an invalid CLASS field>\n");
    }
    
    DacpObjectData objData;
    if ((Status=objData.Request(g_clrData,(CLRDATA_ADDRESS)p_Object)) != S_OK)
    {        
        ExtOut("Invalid object\n");
        return Status;
    }

    if (objData.ObjectType != OBJ_ARRAY)
    {
        ExtOut("Not an array, please use !DumpObj instead\n");
        return S_OK;
    }   
    return PrintArray(objData, flags, FALSE);
}


HRESULT PrintArray(DacpObjectData& objData, DumpArrayFlags& flags, BOOL isPermSetPrint)
{
    HRESULT Status = S_OK;

    if (objData.dwRank != 1 && (flags.Length != (DWORD_PTR)-1 ||flags.startIndex != 0))
    {
        ExtOut ("For multi-dimension array, length and start index are supported\n");
        return S_OK;
    }

    if ((DWORD)flags.startIndex > objData.dwNumComponents)
    {
        ExtOut ("Start index out of range\n");
        return S_OK;
    }

    if (!flags.bDetail && flags.bNoFieldsForElement)
    {
        ExtOut ("-nofields has no effect unless -details is specified\n");
    }
    
    DWORD i;
    if (!isPermSetPrint)
    {
    NameForMT_s((DWORD_PTR) objData.ElementTypeHandle, g_mdName,mdNameLen);

    ExtOut("Name: %S[", g_mdName);    
    for (i = 1; i < objData.dwRank; i++)
    {
        ExtOut(",");
    }
    ExtOut ("]\n");
        
    ExtOut("MethodTable: %p\n",(ULONG64)objData.MethodTable);
    ExtOut("EEClass: %p\n", (ULONG64)objData.EEClass);
    ExtOut("Size: %d(0x%x) bytes\n", objData.Size, objData.Size);

    DacpEEClassData EECls;
    if ((Status=EECls.Request(g_clrData,objData.EEClass))!=S_OK)
    {
        ExtOut("Invalid EEClass address\n");
        return Status;
    }            

    ExtOut("Array: Rank %d, Number of elements %d, Type %s\n", objData.dwRank, objData.dwNumComponents, 
               ElementTypeName(objData.ElementType));
    ExtOut("Element Methodtable: %p\n", (ULONG64)objData.ElementTypeHandle);    
    }

    BOOL isElementValueType = objData.ElementType == ELEMENT_TYPE_VALUETYPE;        

    DWORD dwRankAllocSize;
    if (!ClrSafeInt<DWORD>::multiply(sizeof(DWORD), objData.dwRank, dwRankAllocSize))
    {
        ExtOut("Integer overflow on array rank\n");
        return Status;
    }
    DWORD * lowerBounds = (DWORD *)alloca (dwRankAllocSize);
    if (!SafeReadMemory ((ULONG_PTR)objData.ArrayLowerBoundsPtr, lowerBounds, dwRankAllocSize, NULL))
    {
        ExtOut ("Failed to read lower bounds info from the array\n");        
        return S_OK;
    }

    DWORD * bounds = (DWORD *)alloca (dwRankAllocSize);
    if (!SafeReadMemory ((ULONG_PTR)objData.ArrayBoundsPtr, bounds, dwRankAllocSize, NULL))
    {
        ExtOut ("Failed to read bounds info from the array\n");        
        return S_OK;
    }

    //length is only supported for single-dimension array
    if (objData.dwRank == 1 && flags.Length != (DWORD_PTR)-1)
    {
        bounds[0] = min(bounds[0], (DWORD)(flags.Length + flags.startIndex) - lowerBounds[0]);
    }
    
    DWORD * indices = (DWORD *)alloca(dwRankAllocSize);
    for (i = 0; i < objData.dwRank; i++)
    {
        indices[i] = lowerBounds[i];
    }

    //start index is only supported for single-dimension array
    if (objData.dwRank == 1)
    {
        indices[0] = (DWORD)flags.startIndex;
    }
    
    //Offset should be calculated by OffsetFromIndices. However because of the way 
    //how we grow indices, incrementing offset by one happens to match indices in every iteration    
    for (size_t offset = OffsetFromIndices (indices, lowerBounds, bounds, objData.dwRank);
        IndicesInRange (indices, lowerBounds, bounds, objData.dwRank); 
        indices[objData.dwRank - 1]++, offset++)
    {      
        if (IsInterrupt())
        {
            ExtOut("interrupted by user\n");
            break;
        }

        DWORD_PTR elementAddress = (DWORD_PTR)((BYTE *)objData.ArrayDataPtr + offset * objData.dwComponentSize);
        DWORD_PTR p_Element = NULL;
        if (isElementValueType)
        {
            p_Element = elementAddress;        
        }
        else if (!SafeReadMemory (elementAddress, &p_Element, sizeof (p_Element), NULL))
        {
            ExtOut ("Failed to read element at ");        
            ExtOutIndices (indices, objData.dwRank);
            ExtOut ("\n");
            continue;
        }

        
        if (p_Element)
        {
            ExtOutIndices (indices, objData.dwRank);
            ExtOut (" %p\n", (ULONG64)p_Element);
        }
        else if (!isPermSetPrint)
        {
            ExtOutIndices (indices, objData.dwRank);
            ExtOut (" null\n");
        }        

        if (flags.bDetail)
        {
            IncrementIndent ();
            if (isElementValueType)
            {
                PrintVC ((DWORD_PTR)objData.ElementTypeHandle, elementAddress, !flags.bNoFieldsForElement);
            }
            else if (p_Element != NULL)
            {
                PrintObj (p_Element, !flags.bNoFieldsForElement);
            }            
            DecrementIndent ();
        }     
    }   
    
    return S_OK;
}

/**********************************************************************\
* Routine Description:                                                 *
*                                                                      *
*    This function is called to dump the contents of an object from a  *  
*    given address
*                                                                      *
\**********************************************************************/
DECLARE_API(DumpObj)    
{
    INIT_API();

    MINIDUMP_NOT_SUPPORTED();    

    BOOL bNoFields = FALSE;
    CMDOption option[] = {
        // name, vptr, type, hasValue
        {"-nofields", &bNoFields, COBOOL, FALSE}
    };

    LPSTR str_Object = NULL;    
    CMDValue arg[] = {
        // vptr, type
        {&str_Object, COSTRING}
    };
    
    size_t nArg;
    if (!GetCMDOption(args,option,sizeof(option)/sizeof(CMDOption),
                      arg,sizeof(arg)/sizeof(CMDValue),&nArg)) {
        return Status;
    }
    
    DWORD_PTR p_Object = GetExpression (str_Object);
    if (p_Object == 0)
    {
        ExtOut ("Invalid parameter %s\n", args);
        return Status;
    }

    return PrintObj (p_Object, !bNoFields);
}

CLRDATA_ADDRESS isExceptionObj(CLRDATA_ADDRESS mtObj)
{
    // We want to follow back until we get the mt for System.Exception
    DacpMethodTableData dmtd;
    CLRDATA_ADDRESS walkMT = mtObj;
    while(walkMT != NULL)
    {
        if (dmtd.Request (g_clrData, walkMT) != S_OK)
        {
            break;            
        }
        NameForMT_s ((DWORD_PTR) walkMT, g_mdName,mdNameLen);                
        if (wcscmp (L"System.Exception", g_mdName) == 0)
        {
            return walkMT;
        }
        walkMT = dmtd.ParentMethodTable;
    }
    return NULL;
}

CLRDATA_ADDRESS isSecurityExceptionObj(CLRDATA_ADDRESS mtObj)
{
    // We want to follow back until we get the mt for System.Exception
    DacpMethodTableData dmtd;
    CLRDATA_ADDRESS walkMT = mtObj;
    while(walkMT != NULL)
    {
        if (dmtd.Request (g_clrData, walkMT) != S_OK)
        {
            break;            
        }
        NameForMT_s ((DWORD_PTR) walkMT, g_mdName,mdNameLen);                
        if (wcscmp (L"System.Security.SecurityException", g_mdName) == 0)
        {
            return walkMT;
        }
        walkMT = dmtd.ParentMethodTable;
    }
    return NULL;
}

// Fill the passed in buffer with a text header for generated exception information.
// Returns the number of characters in the szBuffer array on exit.
// If NULL is passed for szBuffer, just returns the number of characters needed.
size_t AddExceptionHeader (__out_ecount (bufferLength) __out_opt char *szBuffer, size_t bufferLength)
{
    const char *szHeader = "    SP       IP       Function\n";
    if (szBuffer)
    {
        sprintf_s (szBuffer, bufferLength, szHeader);    
    }
    return strlen(szHeader);
}

struct StackTraceElement 
{
    UINT_PTR        ip;
    UINT_PTR        sp;
    DWORD_PTR     pFunc;        // MethodDesc
    DWORD_PTR   owner;     // MethodTable
};

size_t FormatGeneratedException (DWORD_PTR dataPtr, 
    UINT bytes, 
    __out_ecount (bufferLength) __out_opt char *szBuffer, 
    size_t bufferLength, 
    BOOL bNestedCase=FALSE)
{
    UINT count = bytes / sizeof(StackTraceElement);
    size_t Length = 0;

    if (szBuffer && bufferLength>0)
    {
        szBuffer[0] = '\0';
    }
    
    // Buffer is calculated for sprintf below ("   %p %p %S\n");
    CHAR szLineBuffer[mdNameLen + 8 + sizeof(size_t)*2];

    if (count==0)
    {
        return 0;
    }
    
    if (bNestedCase)
    {
        // If we are computing the call stack for a nested exception, we
        // don't want to print the last frame, because the outer exception
        // will have that frame.
        count--;
    }
    
    for (UINT i = 0; i < count; i++)
    {
        StackTraceElement ste;        
        MOVE (ste, dataPtr + i*sizeof(StackTraceElement));

        NameForMD_s (ste.pFunc, g_mdName,mdNameLen);

        //
        BYTE *codeAddr = (BYTE *) ste.ip;
        codeAddr += 1;
        sprintf_s (szLineBuffer, _countof(szLineBuffer), "    %p %p %S\n", (BYTE *)ste.sp, 
            codeAddr, g_mdName);        
        // Length += strlen(szLineBuffer);
        
        if (szBuffer)
        {
            strncat_s (szBuffer, bufferLength, szLineBuffer, bufferLength-Length-1); // _TRUNCATE
        }

        // Move this?
        Length += strlen(szLineBuffer);
    }
    return Length;
}

HRESULT FormatException(DWORD_PTR p_Object)
{
    HRESULT Status = S_OK;

    DacpObjectData objData;
    if ((Status=objData.Request(g_clrData,(CLRDATA_ADDRESS)p_Object)) != S_OK)
    {        
        ExtOut("Invalid object\n");
        return Status;
    }

    // Make sure it is an exception object, and get the MT of Exception
    CLRDATA_ADDRESS exceptionMT = isExceptionObj(objData.MethodTable);
    if (exceptionMT == NULL)
    {
        ExtOut("Not a valid exception object\n");
        return Status;
    }

    ExtOut ("Exception object: %p\n", (ULONG64)p_Object);
    
    if (NameForMT_s ((DWORD_PTR) objData.MethodTable, g_mdName,mdNameLen))
    {
        ExtOut ("Exception type: %S\n", g_mdName);
    }
    else
    {
        ExtOut ("Exception type: <Unknown>\n");
    }

    // Print basic info

    // Walk the fields, printing some fields in a special way.
    // HR, InnerException, Message, StackTrace, StackTraceString

    DacpEEClassData EECls;
    if ((Status=EECls.Request(g_clrData,objData.EEClass))!=S_OK)
    {
        ExtOut("Invalid EEClass address\n");
        return Status;
    }            
    

    int iOffset = GetObjFieldOffset (p_Object, objData.MethodTable, L"_message");
    if (iOffset > 0)
    {
        DWORD_PTR objPtr;
        MOVE (objPtr, p_Object + iOffset);                
        ExtOut ("Message: ");
        if (objPtr)
        {
            StringObjectContent(objPtr);
        }
        else
        {
            ExtOut ("<none>");
        }
        ExtOut ("\n");
    }

    iOffset = GetObjFieldOffset (p_Object, objData.MethodTable, L"_innerException");
    if (iOffset > 0)    
    {
        DWORD_PTR objPtr;
        MOVE (objPtr, p_Object + iOffset);                
        ExtOut ("InnerException: ");
        if (objPtr)
        {
            DWORD_PTR mt;
            MOVE (mt, objPtr);
            NameForMT_s ((DWORD_PTR) mt, g_mdName,mdNameLen);                
            ExtOut ("%S, use !PrintException %p to see more\n", g_mdName, (ULONG64) objPtr);
        }
        else
        {
            ExtOut ("<none>\n");
        }
    }            

    iOffset = GetObjFieldOffset (p_Object, objData.MethodTable, L"_stackTrace");
    if (iOffset > 0)        
    {
        DWORD_PTR arrayPtr;
        // we are reading cross process memory. Better check to see if we read it successfully or not
        HRESULT hr = MOVE (arrayPtr, p_Object + iOffset);
        ExtOut ("StackTrace (generated):\n");
        if (arrayPtr && hr == S_OK)
        {
            DWORD arrayLen;
            MOVE (arrayLen, arrayPtr + sizeof(DWORD_PTR));

            if (arrayLen != 0 && hr == S_OK)
            {
                DWORD_PTR dataPtr = arrayPtr + sizeof(DWORD_PTR) + sizeof(DWORD);
                size_t stackTraceSize = 0;
                MOVE (stackTraceSize, dataPtr);

                DWORD cbStackSize = static_cast<DWORD>(stackTraceSize * sizeof(StackTraceElement));
                dataPtr += sizeof(size_t) + sizeof(size_t); // skip the array header, then goes the data
            
                if (stackTraceSize == 0)
                {
                    ExtOut ("Unable to decipher generated stack trace\n");
                }
                else
                {
                    size_t iHeaderLength = AddExceptionHeader (NULL, 0);
                    size_t iLength = FormatGeneratedException (dataPtr, cbStackSize, NULL, 0);
                    char *pszBuffer = new char[iHeaderLength + iLength + 1];
                    if (pszBuffer)
                    {
                        AddExceptionHeader (pszBuffer, iHeaderLength+1);
                        FormatGeneratedException (dataPtr, cbStackSize, pszBuffer + iHeaderLength, iLength+1);
                        ExtOut ("%s", pszBuffer);
                        delete [] pszBuffer;
                    }
                    ExtOut ("\n");
                }
            }
            else
            {
                ExtOut ("<Not Available>\n");
            }
        }                   
        else
        {
            if (arrayPtr == NULL)
            {
                ExtOut ("<none>\n");
            }
            else
            {
                ExtOut ("<Not Available>\n");
            }
        }
    }

    iOffset = GetObjFieldOffset (p_Object, objData.MethodTable, L"_stackTraceString");
    if (iOffset > 0)        
    {
        DWORD_PTR objPtr;
        MOVE (objPtr, p_Object + iOffset);                
        ExtOut ("StackTraceString: ");
        if (objPtr)
        {
            StringObjectContent(objPtr);
            ExtOut ("\n\n"); // extra newline looks better
        }
        else
        {
            ExtOut ("<none>\n");
        }
    }            

    iOffset = GetObjFieldOffset (p_Object, objData.MethodTable, L"_HResult");
    if (iOffset > 0)        
    {
        DWORD hResult;
        MOVE (hResult, p_Object + iOffset);
        ExtOut ("HResult: %lx\n", hResult);
    }

    if (isSecurityExceptionObj(objData.MethodTable) != NULL)
    {
        // We have a SecurityException Object: print out the debugString if present
        iOffset = GetObjFieldOffset (p_Object, objData.MethodTable, L"m_debugString");
        if (iOffset > 0)        
        {
            DWORD_PTR objPtr;
            MOVE (objPtr, p_Object + iOffset);                
            
            if (objPtr)
            {
                ExtOut ("SecurityException Message: ");
                StringObjectContent(objPtr);
                ExtOut ("\n\n"); // extra newline looks better
            }
        }            
        
    }

    return Status;
}

DECLARE_API(PrintException)
{
    INIT_API();
    
    BOOL bShowNested = FALSE;   
    CMDOption option[] = {
        // name, vptr, type, hasValue
        {"-nested", &bShowNested, COBOOL, FALSE}
    };

    StringHolder strObject;
    CMDValue arg[] = {
        // vptr, type
        {&strObject, COSTRING}
    };
    
    size_t nArg;
    if (!GetCMDOption(args,option,sizeof(option)/sizeof(CMDOption),
                      arg,sizeof(arg)/sizeof(CMDValue),&nArg)) {
        return Status;
    }

    DWORD_PTR p_Object = NULL;

    if (nArg == 0)
    {    
        // Look at the last exception object on this thread

        CLRDATA_ADDRESS threadAddr = GetCurrentManagedThread();
        DacpThreadData Thread;
        
        if ((threadAddr == NULL) || (Thread.Request(g_clrData, threadAddr) != S_OK))
        {
            ExtOut("The current thread is unmanaged\n");
            return Status;
        }

        DWORD_PTR dwAddr = NULL;
        if ((!SafeReadMemory((DWORD_PTR)Thread.lastThrownObjectHandle,
                            &dwAddr,
                            sizeof(dwAddr), NULL)) || (dwAddr==NULL))
        {
            ExtOut ("There is no current managed exception on this thread\n");            
        }    
        else
        {        
            p_Object = dwAddr;        
        }
    }
    else
    {
        p_Object = GetExpression (strObject.data);
        if (p_Object == 0)
        {
            ExtOut ("Invalid exception object %s\n", args);
            return Status;
        }
    }

    if (p_Object)
    {
        FormatException (p_Object);
    }

    // Are there nested exceptions?
    CLRDATA_ADDRESS threadAddr = GetCurrentManagedThread();
    DacpThreadData Thread;
    
    if ((threadAddr == NULL) || (Thread.Request(g_clrData, threadAddr) != S_OK))
    {
        ExtOut("The current thread is unmanaged\n");
        return Status;
    }

    if (Thread.firstNestedException)
    {
        if (!bShowNested)
        {
            ExtOut ("There are nested exceptions on this thread. Run with -nested for details\n");
            return Status;
        }
        
        CLRDATA_ADDRESS currentNested = Thread.firstNestedException;
        DacpNestedExceptionInfo nested;
        do
        {
            Status = nested.Request (g_clrData, currentNested);

            if (Status != S_OK)
            {
                ExtOut ("Error retrieving nested exception info %p\n", (ULONG64) currentNested);
                return Status;
            }

            if (IsInterrupt())
            {
                ExtOut ("<aborted>\n");
                return Status;
            }

            ExtOut ("\nNested exception -------------------------------------------------------------\n");
            Status = FormatException ((DWORD_PTR) nested.exceptionObject);
            if (Status != S_OK)
            {
                return Status;
            }
            
            currentNested = nested.nextNestedException;
        }
        while(currentNested != NULL);        
    }
    return Status;
}

/**********************************************************************\
* Routine Description:                                                 *
*                                                                      *
*    This function is called to dump the contents of an object from a  *  
*    given address
*                                                                      *
\**********************************************************************/
DECLARE_API(DumpVC)
{
    INIT_API();
    MINIDUMP_NOT_SUPPORTED();    
    
    DWORD_PTR p_MT = NULL;
    DWORD_PTR p_Object = NULL;

    CMDValue arg[] = {
        // vptr, type
        {&p_MT, COHEX},
        {&p_Object, COHEX}
    };
    size_t nArg;
    
    if (!GetCMDOption(args,NULL,0,arg,sizeof(arg)/sizeof(CMDValue),&nArg)) {
        return Status;
    }

    if (nArg!=2)
    {
        ExtOut ("Usage: DumpVC <Method Table> <Value object start addr>\n");
        ExtOut ("Example: DumpVC 907b50 67903084\n");
        return Status;
    }
    
    if (!IsMethodTable(p_MT))
    {
        ExtOut("Not a managed object\n");
        return S_OK;
    } 

    return PrintVC (p_MT, p_Object);
}
    
#ifdef _DEBUG
/**********************************************************************\
* Routine Description:                                                 *
*                                                                      *
*    This function is called to dump the contents of a PermissionSet   *
*    from a given address.                                             * 
*                                                                      *
\**********************************************************************/
/* 
    COMMAND: dumppermissionset.
    !DumpPermissionSet <PermissionSet object address>

    This command allows you to examine a PermissionSet object. Note that you can 
    also use DumpObj such an object in greater detail. DumpPermissionSet attempts 
    to extract all the relevant information from a PermissionSet that you might be 
    interested in when performing Code Access Security (CAS) related debugging.

    Here is a simple PermissionSet object:

    0:000> !DumpPermissionSet 014615f4 
    PermissionSet object: 014615f4
    Unrestricted: TRUE

    Note that this is an unrestricted PermissionSet object that does not contain 
    any individual permissions. 

    Here is another example of a PermissionSet object, one that is not unrestricted 
    and contains a single permission:

    0:003> !DumpPermissionSet 01469fa8 
    PermissionSet object: 01469fa8
    Unrestricted: FALSE
    Name: System.Security.Permissions.ReflectionPermission
    MethodTable: 5b731308
    EEClass: 5b7e0d78
    Size: 12(0xc) bytes
     (C:\WINDOWS\Microsoft.NET\Framework\v2.0.x86chk\assembly\GAC_32\mscorlib\2.0.
    0.0__b77a5c561934e089\mscorlib.dll)

    Fields:
          MT    Field   Offset                 Type VT     Attr    Value Name
    5b73125c  4001d66        4         System.Int32  0 instance        2 m_flags

    Here is another example of an unrestricted PermissionSet, one that contains 
    several permissions. The numbers in parentheses before each Permission object 
    represents the index of that Permission in the PermissionSet.

    0:003> !DumpPermissionSet 01467bd8
    PermissionSet object: 01467bd8
    Unrestricted: FALSE
    [1] 01467e90
        Name: System.Security.Permissions.FileDialogPermission
        MethodTable: 5b73023c
        EEClass: 5b7dfb18
        Size: 12(0xc) bytes
         (C:\WINDOWS\Microsoft.NET\Framework\v2.0.x86chk\assembly\GAC_32\mscorlib\2.0.0.0__b77a5c561934e089\mscorlib.dll)
        Fields:
              MT    Field   Offset                 Type VT     Attr    Value Name
        5b730190  4001cc2        4         System.Int32  0 instance        1 access
    [4] 014682a8
        Name: System.Security.Permissions.ReflectionPermission
        MethodTable: 5b731308
        EEClass: 5b7e0d78
        Size: 12(0xc) bytes
         (C:\WINDOWS\Microsoft.NET\Framework\v2.0.x86chk\assembly\GAC_32\mscorlib\2.0.0.0__b77a5c561934e089\mscorlib.dll)
        Fields:
              MT    Field   Offset                 Type VT     Attr    Value Name
        5b73125c  4001d66        4         System.Int32  0 instance        0 m_flags
    [17] 0146c060
        Name: System.Diagnostics.EventLogPermission
        MethodTable: 569841c4
        EEClass: 56a03e5c
        Size: 28(0x1c) bytes
         (C:\WINDOWS\Microsoft.NET\Framework\v2.0.x86chk\assembly\GAC_MSIL\System\2.0.0.0__b77a5c561934e089\System.dll)
        Fields:
              MT    Field   Offset                 Type VT     Attr    Value Name
        5b6d65d4  4003078        4      System.Object[]  0 instance 0146c190 tagNames
        5b6c9ed8  4003079        8          System.Type  0 instance 0146c17c permissionAccessType
        5b6cd928  400307a       10       System.Boolean  0 instance        0 isUnrestricted
        5b6c45f8  400307b        c ...ections.Hashtable  0 instance 0146c1a4 rootTable
        5b6c090c  4003077      bfc        System.String  0   static 00000000 computerName
        56984434  40030e7       14 ...onEntryCollection  0 instance 00000000 innerCollection
    [18] 0146ceb4
        Name: System.Net.WebPermission
        MethodTable: 5696dfc4
        EEClass: 569e256c
        Size: 20(0x14) bytes
         (C:\WINDOWS\Microsoft.NET\Framework\v2.0.x86chk\assembly\GAC_MSIL\System\2.0.0.0__b77a5c561934e089\System.dll)
        Fields:
              MT    Field   Offset                 Type VT     Attr    Value Name
        5b6cd928  400238e        c       System.Boolean  0 instance        0 m_Unrestricted
        5b6cd928  400238f        d       System.Boolean  0 instance        0 m_UnrestrictedConnect
        5b6cd928  4002390        e       System.Boolean  0 instance        0 m_UnrestrictedAccept
        5b6c639c  4002391        4 ...ections.ArrayList  0 instance 0146cf3c m_connectList
        5b6c639c  4002392        8 ...ections.ArrayList  0 instance 0146cf54 m_acceptList
        569476f8  4002393      8a4 ...Expressions.Regex  0   static 00000000 s_MatchAllRegex
    [19] 0146a5fc
        Name: System.Net.DnsPermission
        MethodTable: 56966408
        EEClass: 569d3c08
        Size: 12(0xc) bytes
         (C:\WINDOWS\Microsoft.NET\Framework\v2.0.x86chk\assembly\GAC_MSIL\System\2.0.0.0__b77a5c561934e089\System.dll)
        Fields:
              MT    Field   Offset                 Type VT     Attr    Value Name
        5b6cd928  4001d2c        4       System.Boolean  0 instance        1 m_noRestriction
    [20] 0146d8ec
        Name: System.Web.AspNetHostingPermission
        MethodTable: 569831bc
        EEClass: 56a02ccc
        Size: 12(0xc) bytes
         (C:\WINDOWS\Microsoft.NET\Framework\v2.0.x86chk\assembly\GAC_MSIL\System\2.0.0.0__b77a5c561934e089\System.dll)
        Fields:
              MT    Field   Offset                 Type VT     Attr    Value Name
        56983090  4003074        4         System.Int32  0 instance      600 _level
    [21] 0146e394
        Name: System.Net.NetworkInformation.NetworkInformationPermission
        MethodTable: 5697ac70
        EEClass: 569f7104
        Size: 16(0x10) bytes
         (C:\WINDOWS\Microsoft.NET\Framework\v2.0.x86chk\assembly\GAC_MSIL\System\2.0.0.0__b77a5c561934e089\System.dll)
        Fields:
              MT    Field   Offset                 Type VT     Attr    Value Name
        5697ab38  4002c34        4         System.Int32  0 instance        0 access
        5b6cd928  4002c35        8       System.Boolean  0 instance        0 unrestricted


    The abbreviation !dps can be used for brevity.

    \\
*/
DECLARE_API(DumpPermissionSet)
{
    INIT_API();
    MINIDUMP_NOT_SUPPORTED();    
    
    DWORD_PTR p_Object = NULL;

    CMDValue arg[] = {
        {&p_Object, COHEX}
    };
    size_t nArg;
    
    if (!GetCMDOption(args,NULL,0,arg,sizeof(arg)/sizeof(CMDValue),&nArg)) {
        return Status;
    }

    if (nArg!=1)
    {
        ExtOut ("Usage: DumpPermissionSet <PermissionSet object addr>\n");
        ExtOut ("Example: DumpPermissionSet 67903084\n");
        return Status;
    }
    

    return PrintPermissionSet (p_Object);
}
#endif    
void GCPrintGenerationInfo(DacpGcHeapDetails &heap);
void GCPrintSegmentInfo(DacpGcHeapDetails &heap, DWORD_PTR &total_size);


void DisplayInvalidStructuresMessage()
{
    ExtOut ("The garbage collector data structures are not in a valid state for traversal.\n");
    ExtOut ("It is either in the \"plan phase,\" where objects are being moved around, or\n");
    ExtOut ("we are at the initialization or shutdown of the gc heap. Commands related to \n");
    ExtOut ("displaying, finding or traversing objects as well as gc heap segments may not \n");
    ExtOut ("work properly. !dumpheap and !verifyheap may incorrectly complain of heap \n");
    ExtOut ("consistency errors.\n");
}


HeapStat *stat = NULL;

void PrintGCStat (HeapStat *inStat=NULL)
{
    if (inStat == NULL)
    {
        inStat = stat;
    }

    if (inStat)
    {
        PAL_TRY 
        {
            inStat->Sort();
        }
        PAL_EXCEPT (EXCEPTION_EXECUTE_HANDLER)
        {
            ExtOut ("exception during sorting\n");
            inStat->Delete();
            return;
        }        
        PAL_ENDTRY
        PAL_TRY 
        {
            inStat->Print();
        }
        PAL_EXCEPT (EXCEPTION_EXECUTE_HANDLER)
        {
            ExtOut ("exception during printing\n");
            inStat->Delete();
            return;
        }        
        PAL_ENDTRY
        inStat->Delete();
    }
}



DECLARE_API(VerifyHeap)
{    
    INIT_API();    
    MINIDUMP_NOT_SUPPORTED();    
    
    _ASSERTE(false);
    return E_FAIL;
}

/**********************************************************************\
* Routine Description:                                                 *
*                                                                      *
*    This function dumps what is in the syncblock cache.  By default   *  
*    it dumps all active syncblocks.  Using -all to dump all syncblocks
*                                                                      *
\**********************************************************************/
DECLARE_API(SyncBlk)
{
    INIT_API();    
    MINIDUMP_NOT_SUPPORTED();    
    
    
    BOOL bDumpAll = FALSE;
    size_t nbAsked = 0;
    
    CMDOption option[] = {
        // name, vptr, type, hasValue
        {"-all", &bDumpAll, COBOOL, FALSE}
    };
    CMDValue arg[] = {
        // vptr, type
        {&nbAsked, COSIZE_T}
    };
    size_t nArg;
    if (!GetCMDOption(args,option,sizeof(option)/sizeof(CMDOption),
                      arg,sizeof(arg)/sizeof(CMDValue),&nArg)) {
        return Status;
    }

    DacpSyncBlockData syncBlockData;
    if (syncBlockData.Request(g_clrData,1) != S_OK)
    {
        ExtOut("Error requesting SyncBlk data\n");
        return Status;
    }

    DWORD dwCount = syncBlockData.SyncBlockCount;
    
    ExtOut ("Index" WIN64_8SPACES " SyncBlock MonitorHeld Recursion Owning Thread Info" WIN64_8SPACES "  SyncBlock Owner\n");
    ULONG freeCount = 0;
    ULONG CCWCount = 0;
    ULONG RCWCount = 0;
    ULONG CFCount = 0;
    for (DWORD nb = 1; nb <= dwCount; nb++)
    {
        if (IsInterrupt())
            return Status;
        
        if (nbAsked && nb != nbAsked) 
        {
            continue;
        }

        if (syncBlockData.Request(g_clrData,nb) != S_OK)
        {
            ExtOut("SyncBlock %d is invalid%s\n", nb,
                (nb != nbAsked) ? ", continuing..." : "");
            continue;
        }

        BOOL bPrint = (bDumpAll || nb == nbAsked || (syncBlockData.MonitorHeld > 0 && !syncBlockData.bFree));

        if (bPrint)
        {
            ExtOut ("%5d ", nb);
            if (!syncBlockData.bFree || nb != nbAsked)
            {            
                ExtOut ("%p  ", syncBlockData.SyncBlockPointer); 
                ExtOut ("%11d ", syncBlockData.MonitorHeld);
                ExtOut ("%9d ", syncBlockData.Recursion);
                ExtOut ("%p ", syncBlockData.HoldingThread);

                if (syncBlockData.HoldingThread == -1)
                {
                    ExtOut (" orphaned ");
                }
                else if (syncBlockData.HoldingThread != NULL)
                {
                    DacpThreadData Thread;
                    if ((Status = Thread.Request(g_clrData, syncBlockData.HoldingThread)) != S_OK)
                    {
                        ExtOut("Failed to request Thread at %p\n", syncBlockData.HoldingThread);
                        return Status;
                    }

                    ExtOut ("%5x", Thread.osThreadId);
                    ULONG id;
                    if (g_ExtSystem->GetThreadIdBySystemId (Thread.osThreadId, &id) == S_OK)
                    {
                        ExtOut ("%4d ", id);
                    }
                    else
                    {
                        ExtOut (" XXX ");
                    }
                }
                else
                {
                    ExtOut ("    none  ");
                }

                if (syncBlockData.bFree) {
                    ExtOut ("  %8d", 0);
                }
                else {
                    ExtOut ("  %p", (ULONG64)syncBlockData.Object);
                    NameForObject_s((DWORD_PTR)syncBlockData.Object, g_mdName, mdNameLen);
                    ExtOut (" %S", g_mdName);
                }            
            }
        }
                                    
        if (syncBlockData.bFree) 
        {
            freeCount ++;
            if (bPrint) {
                ExtOut (" Free");
            }
        }
        else 
        {
        }

        if (syncBlockData.MonitorHeld > 1)            
        {
            /*
            ExtOut (" ");
            DWORD_PTR pHead = (DWORD_PTR)vSyncBlock.m_Link.m_pNext;
            DWORD_PTR pNext = pHead;
            Thread vThread;
    
            while (1)
            {
                if (IsInterrupt())
                    return Status;
                DWORD_PTR pWaitEventLink = pNext - offsetLinkSB;
                WaitEventLink vWaitEventLink;
                vWaitEventLink.Fill(pWaitEventLink);
                if (!CallStatus) {
                    break;
                }
                DWORD_PTR dwAddr = (DWORD_PTR)vWaitEventLink.m_Thread;
                ExtOut ("%x ", dwAddr);
                vThread.Fill (dwAddr);
                if (!CallStatus) {
                    break;
                }
                if (bPrint)
                    ExtOut ("%x,", vThread.m_OSThreadId);
                pNext = (DWORD_PTR)vWaitEventLink.m_LinkSB.m_pNext;
                if (pNext == 0)
                    break;
            }  
            */
        }
        
        if (bPrint)
            ExtOut ("\n");
    }
    
    ExtOut ("-----------------------------\n");
    ExtOut ("Total           %d\n", dwCount);
    ExtOut ("CCW             %d\n", CCWCount);
    ExtOut ("RCW             %d\n", RCWCount);
    ExtOut ("ComClassFactory %d\n", CFCount);
    ExtOut ("Free            %d\n", freeCount);
   
    return Status;
}



/**********************************************************************\
* Routine Description:                                                 *
*                                                                      *
*    This function is called to dump the contents of a Module          *
*    for a given address                                               *  
*                                                                      *
\**********************************************************************/
DECLARE_API(FinalizeQueue)
{
    INIT_API();
    MINIDUMP_NOT_SUPPORTED();    
    
    BOOL bDetail = FALSE;

    if (_stricmp (args,"-detail") == 0) {
        bDetail = TRUE;
    }

    
    DacpSyncBlockCleanupData dsbcd;
    CLRDATA_ADDRESS sbCurrent = NULL;
    ULONG cleanCount = 0;
    while ((dsbcd.Request(g_clrData,sbCurrent) == S_OK) && dsbcd.SyncBlockPointer)
    {
        if (bDetail)
        {
            if (cleanCount == 0) // print first time only
            {
                ExtOut ("SyncBlocks to be cleaned by the finalizer thread:\n");
                ExtOut ("%" POINTERSIZE "s %" POINTERSIZE "s %" POINTERSIZE "s %" POINTERSIZE "s\n",
                    "SyncBlock", "RCW", "CCW", "ComClassFactory");                
            }
            
            ExtOut ("%" POINTERSIZE "p %" POINTERSIZE "p %" POINTERSIZE "p %" POINTERSIZE "p\n", 
                (ULONG64) dsbcd.SyncBlockPointer,
                (ULONG64) dsbcd.blockRCW,
                (ULONG64) dsbcd.blockCCW,
                (ULONG64) dsbcd.blockClassFactory);
        }

        cleanCount++;
        sbCurrent = dsbcd.nextSyncBlock;
        if (sbCurrent == NULL)
        {
            break;
        }
    }

    ExtOut ("SyncBlocks to be cleaned up: %d\n", cleanCount);


// noRCW:
    ExtOut ("----------------------------------\n");
    // GC Heap
    DWORD dwNHeaps = GetGcHeapCount();

    if (stat == NULL)
    {
        stat = (HeapStat *)malloc(sizeof (HeapStat));
        stat = new(stat) HeapStat;
    }

    ToDestroy des1 ((void**) &stat);
    if (!IsServerBuild())
    {
        DacpGcHeapDetails heapDetails;
        if (heapDetails.Request(g_clrData) != S_OK)
        {
            ExtOut("Error requesting details\n");
            return Status;
        }

        GatherOneHeapFinalization(heapDetails, stat);
    }
    else
    {   
        DWORD dwAllocSize;
        if (!ClrSafeInt<DWORD>::multiply(sizeof(CLRDATA_ADDRESS), dwNHeaps, dwAllocSize))
        {
            ExtOut("Failed to get GCHeaps:  integer overflow\n");
            return Status;
        }

        CLRDATA_ADDRESS *heapAddrs = (CLRDATA_ADDRESS*)alloca(dwAllocSize);
        if (DacpGcHeapData::GetHeaps(g_clrData, dwNHeaps, heapAddrs) != S_OK)
        {
            ExtOut("Failed to get GCHeaps\n");
            return Status;
        }
        
        for (DWORD n = 0; n < dwNHeaps; n ++)
        {
            DacpGcHeapDetails heapDetails;
            if (heapDetails.Request(g_clrData, heapAddrs[n]) != S_OK)
            {
                ExtOut("Error requesting details\n");
                return Status;
            }

            ExtOut ("------------------------------\n");
            ExtOut ("Heap %d\n", n);
            GatherOneHeapFinalization(heapDetails, stat);
        }        
    }
    
    PrintGCStat();

    return Status;
}

enum {
    // These are the values set in m_dwTransientFlags.
    // Note that none of these flags survive a prejit save/restore.

    M_CRST_NOTINITIALIZED       = 0x00000001,   // Used to prevent destruction of garbage m_crst
    M_LOOKUPCRST_NOTINITIALIZED = 0x00000002,

    SUPPORTS_UPDATEABLE_METHODS = 0x00000020,
    CLASSES_FREED               = 0x00000040,
    HAS_PHONY_IL_RVAS           = 0x00000080,
    IS_EDIT_AND_CONTINUE        = 0x00000200,
};

void ModuleMapTraverse(UINT index, CLRDATA_ADDRESS methodTable,LPVOID token)
{
    ULONG32 rid = (ULONG32) (size_t) token;
    NameForMT_s((DWORD_PTR) methodTable, g_mdName,mdNameLen);
    ExtOut ("%p 0x%08x %S\n",(ULONG64)methodTable, TokenFromRid(rid,index), g_mdName);
}


/**********************************************************************\
* Routine Description:                                                 *
*                                                                      *
*    This function is called to dump the contents of a Module          *
*    for a given address                                               *  
*                                                                      *
\**********************************************************************/
DECLARE_API(DumpModule)
{
    INIT_API();
    MINIDUMP_NOT_SUPPORTED();    
    
    
    DWORD_PTR p_ModuleAddr = NULL;
    BOOL bMethodTables = FALSE;
    
    CMDOption option[] = {
        // name, vptr, type, hasValue
        {"-mt", &bMethodTables, COBOOL, FALSE}
    };
    CMDValue arg[] = {
        // vptr, type
        {&p_ModuleAddr, COHEX}
    };
    size_t nArg;
    if (!GetCMDOption(args,option,sizeof(option)/sizeof(CMDOption),
                      arg,sizeof(arg)/sizeof(CMDValue),&nArg)) {
        return Status;
    }

    if (nArg != 1)
    {
        ExtOut ("Usage: DumpModule [-mt] <Module Address>\n");
        return Status;
    }

    DacpModuleData module;
    if ((Status=module.Request(g_clrData,(CLRDATA_ADDRESS)p_ModuleAddr))!=S_OK)
    {
        ExtOut("Fail to fill Module %p\n", (ULONG64) p_ModuleAddr);
        return Status;
    }
    
    WCHAR FileName[MAX_PATH];
    FileNameForModule (&module, FileName);
    ExtOut ("Name: %ws\n", FileName[0] ? FileName : L"Unknown Module");
    ExtOut ("Attributes: ");
    if (module.bIsPEFile)
        ExtOut ("%s", "PEFile ");
    if (module.bIsReflection)
        ExtOut ("%s", "Reflection ");
    if (module.dwTransientFlags & SUPPORTS_UPDATEABLE_METHODS)
        ExtOut ("%s", "SupportsUpdateableMethods");
    ExtOut ("\n");
    
    ExtOut ("Assembly: %p\n", (ULONG64)module.Assembly);

    ExtOut ("LoaderHeap: %p\n", (ULONG64)module.pLookupTableHeap);
    ExtOut ("TypeDefToMethodTableMap: %p\n",
             (ULONG64)module.TypeDefToMethodTableMap);
    ExtOut ("TypeRefToMethodTableMap: %p\n",
             (ULONG64)module.TypeRefToMethodTableMap);
    ExtOut ("MethodDefToDescMap: %p\n",
             (ULONG64)module.MethodDefToDescMap);
    ExtOut ("FieldDefToDescMap: %p\n",
             (ULONG64)module.FieldDefToDescMap);
    ExtOut ("MemberRefToDescMap: %p\n",
             (ULONG64)module.MemberRefToDescMap);
    ExtOut ("FileReferencesMap: %p\n",
             (ULONG64)module.FileReferencesMap);
    ExtOut ("AssemblyReferencesMap: %p\n",
             (ULONG64)module.ManifestModuleReferencesMap);

    if (module.ilBase && module.metadataStart) {
        ExtOut ("MetaData start address: %p (%d bytes)\n", (ULONG64)module.metadataStart, module.metadataSize);
    }

    if (bMethodTables)
    {
        ExtOut ("\nTypes defined in this module\n\n");
        ExtOut ("%" POINTERSIZE "s %10s %s\n",
                 "MT", "TypeDef", "Name");
                
        ExtOut ("------------------------------------------------------------------------------\n");
        DacpModuleData::DoMapTraverse(g_clrData,(CLRDATA_ADDRESS)p_ModuleAddr,TYPEDEFTOMETHODTABLE,
            ModuleMapTraverse,(LPVOID) mdTypeDefNil);        

        ExtOut ("\nTypes referenced in this module\n\n");
        ExtOut ("%" POINTERSIZE "s %10s %s\n",
                 "MT", "TypeRef", "Name");
        
        ExtOut ("------------------------------------------------------------------------------\n");
        DacpModuleData::DoMapTraverse(g_clrData,(CLRDATA_ADDRESS)p_ModuleAddr,TYPEREFTOMETHODTABLE,
            ModuleMapTraverse,(LPVOID) mdTypeRefNil);        
    }
    
    return Status;
}

/**********************************************************************\
* Routine Description:                                                 *
*                                                                      *
*    This function is called to dump the contents of a Domain          *
*    for a given address                                               *  
*                                                                      *
\**********************************************************************/
DECLARE_API(DumpDomain)
{
    INIT_API();
    MINIDUMP_NOT_SUPPORTED();    
    
    DWORD_PTR p_DomainAddr = GetExpression (args);

    DacpAppDomainStoreData adsData;

    if ((Status=adsData.Request(g_clrData))!=S_OK)
    {
        ExtOut("Unable to get AppDomain information\n");
        return Status;
    }
    
    if (p_DomainAddr)
    {
        DacpAppDomainData appDomain1;
        if ((Status=appDomain1.Request(g_clrData,(CLRDATA_ADDRESS)p_DomainAddr))!=S_OK)
        {
            ExtOut("Fail to fill AppDomain\n");
            return Status;
        }

        ExtOut ("--------------------------------------\n");

        if (p_DomainAddr == adsData.sharedDomain)
        {
            ExtOut ("Shared Domain: %p\n", (ULONG64)adsData.sharedDomain);
        }
        else if (p_DomainAddr == adsData.systemDomain)
        {
            ExtOut ("System Domain: %p\n", (ULONG64)adsData.systemDomain);
        }
        else
        {
            ExtOut ("Domain %d: %p\n", appDomain1.dwId, (ULONG64)p_DomainAddr);
        }

        DomainInfo (&appDomain1);
        return Status;
    }
        
    ExtOut ("--------------------------------------\n");
    ExtOut ("System Domain: %p\n", (ULONG64)adsData.systemDomain);
    DacpAppDomainData appDomain;
    if ((Status=appDomain.Request(g_clrData,adsData.systemDomain))!=S_OK)
    {
        ExtOut("Unable to get system domain info\n");
        return Status;
    }
    DomainInfo (&appDomain);
    
    ExtOut ("--------------------------------------\n");
    ExtOut ("Shared Domain: %p\n", (ULONG64)adsData.sharedDomain);
    if ((Status=appDomain.Request(g_clrData,adsData.sharedDomain))!=S_OK)
    {
        ExtOut("Unable to get shared domain info\n");
        return Status;
    }
    DomainInfo(&appDomain);

    CLRDATA_ADDRESS *pArray = new CLRDATA_ADDRESS[adsData.DomainCount];
    if (pArray==NULL)
    {
        ReportOOM();
        return Status;
    }

    if ((Status=DacpAppDomainStoreData::GetDomains(g_clrData,adsData.DomainCount,pArray))!=S_OK)
    {
        ExtOut("Unable to get array of AppDomains\n");
        delete [] pArray;
        return Status;
    }

    for (int n=0;n<adsData.DomainCount;n++)
    {
        if (IsInterrupt())
            break;

        if ((Status=appDomain.Request(g_clrData,pArray[n]))!=S_OK)
        {
            ExtOut("Failed to get appdomain %p, error %lx\n",(ULONG64)pArray[n],Status);
            delete [] pArray;
            return Status;
        }

        ExtOut ("--------------------------------------\n");
        ExtOut ("Domain %d: %p\n", appDomain.dwId, (ULONG64)pArray[n]);
                
        DomainInfo (&appDomain);
    }

    delete [] pArray;
    return Status;
}

/**********************************************************************\
* Routine Description:                                                 *
*                                                                      *
*    This function is called to dump the contents of a Assembly        *
*    for a given address                                               *  
*                                                                      *
\**********************************************************************/
DECLARE_API(DumpAssembly)
{
    INIT_API();
    MINIDUMP_NOT_SUPPORTED();    
    
    DWORD_PTR p_AssemblyAddr = GetExpression (args);
    if (p_AssemblyAddr == 0)
    {
        ExtOut ("Invalid Assembly %s\n", args);
        return Status;
    }
    
    DacpAssemblyData Assembly;
    if ((Status=Assembly.Request(g_clrData,(CLRDATA_ADDRESS)p_AssemblyAddr))!=S_OK)
    {
        ExtOut ("Fail to fill Assembly\n");
        return Status;
    }
    
    ExtOut ("Parent Domain: %p\n", (ULONG64)Assembly.ParentDomain);
    ExtOut ("Name: ");
    if (DacpAssemblyData::GetName(g_clrData,(CLRDATA_ADDRESS)p_AssemblyAddr,mdNameLen,g_mdName)==S_OK)
    {        
        ExtOut("%S",g_mdName);
    }

    ExtOut ("\n");
    AssemblyInfo (&Assembly);
    return Status;
}

void OutputHostingCapabilities(DWORD hostConfig)
{
    BOOL bAnythingPrinted = FALSE;

#define CHK_AND_PRINT(hType,hStr)                           \
    if (hostConfig & (hType)) {                                \
        ExtOut ("%s%s", bAnythingPrinted ? ", " : "", (hStr)); \
        bAnythingPrinted = TRUE;                               \
    }

    CHK_AND_PRINT(CLRMEMORYHOSTED, "Memory");
    CHK_AND_PRINT(CLRTASKHOSTED, "Task");
    CHK_AND_PRINT(CLRSYNCHOSTED, "Sync");
    CHK_AND_PRINT(CLRTHREADPOOLHOSTED, "Threadpool");
    CHK_AND_PRINT(CLRIOCOMPLETIONHOSTED, "IOCompletion");
    CHK_AND_PRINT(CLRASSEMBLYHOSTED, "Assembly");
    CHK_AND_PRINT(CLRGCHOSTED, "GC");
    CHK_AND_PRINT(CLRSECURITYHOSTED, "Security");    
}

/**********************************************************************\
* Routine Description:                                                 *
*                                                                      *
*    This function is called to dump the managed threads               *
*                                                                      *
\**********************************************************************/
HRESULT PrintThreadsFromThreadStore (BOOL bMiniDump, BOOL bPrintLiveThreadsOnly)
{
    HRESULT Status;
    
    DacpThreadStoreData ThreadStore;
    if ((Status = ThreadStore.Request(g_clrData)) != S_OK)
    {
        ExtOut("Failed to request ThreadStore\n");
        return Status;
    }

    ExtOut ("ThreadCount: %d\n", ThreadStore.threadCount);
    ExtOut ("UnstartedThread: %d\n", ThreadStore.unstartedThreadCount);
    ExtOut ("BackgroundThread: %d\n", ThreadStore.backgroundThreadCount);
    ExtOut ("PendingThread: %d\n", ThreadStore.pendingThreadCount);
    ExtOut ("DeadThread: %d\n", ThreadStore.deadThreadCount);

    ExtOut ("Hosted Runtime: %s", (ThreadStore.fHostConfig & CLRHOSTED) ? "yes" : "no" );
    if (ThreadStore.fHostConfig & ~CLRHOSTED)
    {
        // If there are any specific areas hosted, provide details...
        ExtOut (" (");
        OutputHostingCapabilities(ThreadStore.fHostConfig);
        ExtOut (")\n");
    }
    else
    {
        ExtOut ("\n");
    }
    
    DWORD_PTR finalizerThread = (DWORD_PTR) ThreadStore.finalizerThread;
    DWORD_PTR GcThread = (DWORD_PTR) ThreadStore.gcThread;

    WCHAR fiber[11] = L" Fiber   \0";
    if ((ThreadStore.fHostConfig & CLRTASKHOSTED) == 0)
        fiber[0] = '\0';

    ExtOut ("                                      PreEmptive   GC Alloc           Lock\n");
    ExtOut ("       ID OSID ThreadOBJ    State     GC       Context       Domain   Count APT%s Exception\n", fiber);

    CLRDATA_ADDRESS CurThread = ThreadStore.firstThread;
    while (CurThread)
    {
        if (IsInterrupt())
            break;

        DacpThreadData Thread;
        if ((Status = Thread.Request(g_clrData, CurThread, !bMiniDump)) != S_OK)
        {
            ExtOut("Failed to request Thread at %p\n", CurThread);
            return Status;
        }

        BOOL bSwitchedOutFiber = Thread.osThreadId == SWITCHED_OUT_FIBER_OSID;
        if (!IsKernelDebugger())
        {
            ULONG id = 0;          
            
            if (bSwitchedOutFiber)
            {
                ExtOut ("<<<< ");
            }            
            else if (g_ExtSystem->GetThreadIdBySystemId (Thread.osThreadId,
                                                    &id) == S_OK)
            {
                ExtOut ("%4d ", id);
            }
            else if (bPrintLiveThreadsOnly)
            {
                CurThread = Thread.nextThread;
                continue;
            }
            else
            {
                ExtOut ("XXXX ");
            }
        }
        else
            ExtOut ("    ");
        
        ExtOut ("%4x %4x %p  %8x", Thread.corThreadId, (bSwitchedOutFiber?0:Thread.osThreadId), CurThread,
                Thread.state);
        if (Thread.preemptiveGCDisabled == 1)
            ExtOut (" Disabled");
        else
            ExtOut (" Enabled ");

        ExtOut (" %p:%p", Thread.allocContextPtr,
                Thread.allocContextLimit);

        DacpContextData Context;
        if ((Status = Context.Request(g_clrData, Thread.context)) != S_OK)
        {
            ExtOut("Failed to request Context at %p\n", Thread.context);
            return Status;
        }
        if (Thread.domain)
            ExtOut (" %p", Thread.domain);
        else
        {
            ExtOut (" %p", Context.domain);
        }
        ExtOut (" %5d", Thread.lockCount);

        // Apartment state
            ExtOut (" Ukn");

        if (ThreadStore.fHostConfig & CLRTASKHOSTED)
        {
            ExtOut (" %p", (ULONG64) Thread.fiberData);
        }
        
        if (CurThread == finalizerThread)
            ExtOut (" (Finalizer)");
        if (CurThread == GcThread)
            ExtOut (" (GC)");

        const int TS_ThreadPoolThread       = 0x00800000;    // is this a threadpool thread?
        const int TS_TPWorkerThread         = 0x01000000;    // is this a threadpool worker thread? (if not, it is a threadpool completionport thread)
        
        if (Thread.state & TS_ThreadPoolThread) {
            if (Thread.state & TS_TPWorkerThread) {
                ExtOut (" (Threadpool Worker)");
            }
            else
                ExtOut (" (Threadpool Completion Port)");
        }
        
        DWORD_PTR dwAddr;
        if (SafeReadMemory((DWORD_PTR)Thread.lastThrownObjectHandle,
                            &dwAddr,
                            sizeof(dwAddr), NULL) && dwAddr)
        {
            DWORD_PTR MTAddr;
            if (SafeReadMemory(dwAddr, &MTAddr, sizeof(MTAddr), NULL))
            {
                if (NameForMT_s (MTAddr, g_mdName,mdNameLen))
                {
                    ExtOut (" %S (%p)", g_mdName, (ULONG64) dwAddr);
                }
                else
                {
                    ExtOut (" <Invalid Object> (%p)", (ULONG64) dwAddr);
                }

                // Print something if there are nested exceptions on the thread
                if (Thread.firstNestedException)
                {
                    ExtOut (" (nested exceptions)");
                }
            }
        }
        
        ExtOut ("\n");
        CurThread = Thread.nextThread;
    }

    return Status;
}


DECLARE_API(Threads)
{
    INIT_API();

    BOOL bPrintSpecialThreads = FALSE;
    BOOL bPrintLiveThreadsOnly = FALSE;

    CMDOption option[] = {
        // name, vptr, type, hasValue
        {"-special", &bPrintSpecialThreads, COBOOL, FALSE},
        {"-live", &bPrintLiveThreadsOnly, COBOOL, FALSE}
    };
    
    if (!GetCMDOption(args,option,sizeof(option)/sizeof(CMDOption),
                      NULL, 0, NULL)) {
        return Status;
    }
    
    // We need to support minidumps for this command.
    BOOL bMiniDump = IsMiniDumpFile();

    if (bMiniDump && bPrintSpecialThreads)
    {
        ExtOut ("Special thread information is not available in mini dumps.\n");
    }
    
    Status = PrintThreadsFromThreadStore (bMiniDump, bPrintLiveThreadsOnly);
    if (!bMiniDump && bPrintSpecialThreads)
    {
        ExtOut ("\n-special not supported.\n");
    }

    
    return Status;
}


HRESULT VerifyDACInternal()
{
    HRESULT Status = E_FAIL;
    IXCLRDataStackWalk *pStackWalk = NULL;
    
    /////////////////////////////////////////////////////////////////////////////////////////////////////////
    //
    // step 1
    //

    // SOS doesn't have access to symbol lookup.    


    // reset status 
    Status = E_FAIL;

    /////////////////////////////////////////////////////////////////////////////////////////////////////////
    //
    // step 3 - can we list the 3 domains?
    //
    ExtOut("Verify DumpDomain functionality: \n");
    DacpAppDomainStoreData adsData;

    if ((Status=adsData.Request(g_clrData))!=S_OK)
    {
        ExtOut("Unable to get AppDomain information\n");
        goto ErrExit;
    }
            
    DacpAppDomainData appDomain;
    if ((Status=appDomain.Request(g_clrData,adsData.systemDomain))!=S_OK)
    {
        ExtOut("Unable to get system domain info\n");
        goto ErrExit;
    }
    
    if ((Status=appDomain.Request(g_clrData,adsData.sharedDomain))!=S_OK)
    {
        ExtOut("Unable to get shared domain info\n");
        goto ErrExit;
    }    

    if (adsData.DomainCount != 1)
    {
        ExtOut("Unexpected domain count\n");
        goto ErrExit;
    }
    
    CLRDATA_ADDRESS pDomainPtr = NULL;
    if ((Status=DacpAppDomainStoreData::GetDomains(g_clrData,adsData.DomainCount,&pDomainPtr))!=S_OK)
    {
        ExtOut("Unable to get array of AppDomains\n");
        goto ErrExit;
    }

    if ((Status=appDomain.Request(g_clrData,pDomainPtr))!=S_OK)
    {
        ExtOut("Failed to get appdomain, error %lx\n",Status);
        goto ErrExit;
    }                

    if ((Status=DacpAppDomainData::GetName(g_clrData,appDomain.AppDomainPtr,mdNameLen,g_mdName))!=S_OK)
    {
        ExtOut("Error getting AppDomain friendly name\n");
        goto ErrExit;
    }

    if (_wcsicmp(g_mdName,L"unittest.exe") != 0)
    {
        ExtOut("Error: AppDomain name incorrect\n");
        goto ErrExit;
    }
    
    ExtOut("Verify DumpDomain functionality: Success\n");

    /////////////////////////////////////////////////////////////////////////////////////////////////////////
    //
    // step 4 - can we dump threads?
    //
    ExtOut("Verify that we can list threads: \n");

    DacpThreadStoreData ThreadStore;
    if ((Status = ThreadStore.Request(g_clrData)) != S_OK)
    {
        ExtOut("Failed to request ThreadStore\n");
        goto ErrExit;
    }

    if ((ThreadStore.threadCount < 2) || (ThreadStore.threadCount > 4))
    {
        ExtOut("Error: bad threadcount (should be between 2 and 4)\n");
        goto ErrExit;
    }

    BOOL bFoundFinalizer = FALSE;        
    DacpThreadData Thread;
    CLRDATA_ADDRESS CurThread = ThreadStore.firstThread;
    for (LONG tc=0; tc < ThreadStore.threadCount; tc++)
    {
        if ((Status = Thread.Request(g_clrData, CurThread)) != S_OK)
        {
            ExtOut("Error: Failed to request Thread\n");
            goto ErrExit;
        }

        // This thread should be the finalizer
        if (ThreadStore.finalizerThread == CurThread)
        {
            bFoundFinalizer = TRUE;
        }

        CurThread = Thread.nextThread;
    }

    if (CurThread != NULL)
    {
        ExtOut("More threads were found than specified by DacpThreadStoreData.threadCount\n");
        goto ErrExit;
    }
    
    if (!bFoundFinalizer)
    {
        ExtOut("Error: unable to find the finalizer thread\n");
        goto ErrExit;
    }
    
    ExtOut("Verify that we can list threads: Success\n");
    //
    //

    
    ExtOut("Validation was successful\n");
    Status = S_OK;
    if (pStackWalk)
    {
        pStackWalk->Release();
    }
    return Status;
    
ErrExit:
    ExtOut("Failed\n");
    if (pStackWalk)
    {
        pStackWalk->Release();
    }
    return Status;
}

HRESULT VerifyDAC1()
{
    HRESULT hr = E_FAIL;
    PAL_TRY
    {
        hr = VerifyDACInternal();
    }
    PAL_EXCEPT(EXCEPTION_EXECUTE_HANDLER)
    {
        ExtOut("Error: caught exception in VerifyDACInternal\n");
        hr = E_FAIL;
    }
    PAL_ENDTRY
    return hr;
}

DECLARE_API(VerifyDAC)
{
    INIT_API();
    MINIDUMP_NOT_SUPPORTED();    
    
    return VerifyDAC1();
 }
    

struct PendingBreakpoint
{
    WCHAR szModuleName[MAX_PATH];
    WCHAR szFunctionName[mdNameLen];
    PendingBreakpoint *pNext;
    PendingBreakpoint() : pNext(NULL) { }
};

void IssueDebuggerBPCommand ( CLRDATA_ADDRESS addr )
{
    const int MaxBPsCached = 1024;
    static CLRDATA_ADDRESS alreadyPlacedBPs[MaxBPsCached];
    static int curLimit = 0;

    BOOL bUnique = curLimit >= MaxBPsCached;
    if (!bUnique)
    {
        bUnique = TRUE;
        for ( int i = 0; i < curLimit; ++i )
            if (alreadyPlacedBPs[i] == addr)
            {
                bUnique = FALSE;
                break;
            }
    }
    if (bUnique)
    {
        char buffer[64]; // sufficient for "bp <pointersize>"
        static WCHAR wszNameBuffer[1024]; // should be large enough

        // get the MethodDesc name
        DacpMethodDescData MethodDescData;
        if (MethodDescData.RequestFromIP(g_clrData, addr) != S_OK
            || DacpMethodDescData::GetMethodName(g_clrData, MethodDescData.MethodDescPtr, 1024, wszNameBuffer) != S_OK)
        {
            wcscpy_s(wszNameBuffer, _countof(wszNameBuffer),L"UNKNOWN");        
        }

        sprintf(buffer,"bp %p",(size_t) addr);
        ExtOut ("Setting breakpoint: %s [%S]\n", buffer, wszNameBuffer);
        g_ExtControl->Execute(DEBUG_EXECUTE_NOT_LOGGED, buffer ,0);

        if ( curLimit < MaxBPsCached )
            alreadyPlacedBPs[curLimit++] = addr;
    }
}

class Breakpoints
{
    PendingBreakpoint* m_breakpoints;
public:
    Breakpoints()
    {
        m_breakpoints = NULL;
    }
    ~Breakpoints()
    {
        PendingBreakpoint *pCur = m_breakpoints;
        while(pCur)
        {
            PendingBreakpoint *pNext = pCur->pNext;
            delete pCur;
            pCur = pNext;
        }
        m_breakpoints = NULL;
    }

    void Add(__in __in_z LPWSTR szModule, __in __in_z LPWSTR szName)
    {
        if (!IsIn(szModule, szName))
        {
            PendingBreakpoint *pNew = new PendingBreakpoint();
            wcscpy(pNew->szModuleName, szModule);
            wcscpy(pNew->szFunctionName, szName);
            pNew->pNext = m_breakpoints;
            m_breakpoints = pNew;
        }
    }

    void Update(IXCLRDataModule* mod)
    {
        // Get tokens for any modules that match. If there was a change,
        // update notifications.                
        PendingBreakpoint *pCur = m_breakpoints;
        while(pCur)
        {
            PendingBreakpoint *pNext = pCur->pNext;
            if (ResolvePendingBreakpoint(mod, pCur))
            {
                // Delete the current node, and keep going
                Delete (pCur->szModuleName, pCur->szFunctionName);
            }
            pCur = pNext;
        }                
    }
    void Update(IXCLRDataMethodInstance* method)
    {
        // Some method has been generated, make a breakpoint and remove it.
        ULONG32 len = mdNameLen;
        if (method->GetName(0, mdNameLen, &len, g_mdName) == S_OK)
        {            
            IXCLRDataModule *pMod = NULL;
            method->GetTokenAndScope(NULL, &pMod);
            WCHAR szModuleName[mdNameLen];
            len = mdNameLen;
            if (pMod->GetName(mdNameLen, &len, szModuleName) == S_OK)
            {
                ExtOut ("JITTED %S!%S\n", szModuleName, g_mdName);
                pMod->Release();
                CLRDATA_ADDRESS addr = 0;
                if (method->GetRepresentativeEntryAddress(&addr) == S_OK)
                {
                    IssueDebuggerBPCommand(addr);

                    // now remove BP from pending BPs list, if we're not a generic method
                    IXCLRDataMethodDefinition* pMD = NULL;
                    BOOL bGeneric = FALSE;
                    if (method->GetDefinition(&pMD) == S_OK
                        && pMD->HasClassOrMethodInstantiation(&bGeneric) == S_OK
                        && !bGeneric)
                    {
                        Delete ( szModuleName, g_mdName );
                    }
                }
            }
        }
    }
    
private:    
    BOOL IsIn(__in __in_z LPWSTR szModule, __in __in_z LPWSTR szName)
    {
        PendingBreakpoint *pCur = m_breakpoints;
        while(pCur)
        {
            if (_wcsicmp(pCur->szModuleName, szModule) == 0 &&
                wcscmp(pCur->szFunctionName, szName) == 0)
            {
                return TRUE;
            }
            pCur = pCur->pNext;
        }
        return FALSE;
    }

    void Delete(__in __in_z LPWSTR szModule, __in __in_z LPWSTR szName)
    {
        if (!IsIn(szModule, szName))
        {
            return;
        }

        PendingBreakpoint *pCur = m_breakpoints;
        PendingBreakpoint *pPrev = NULL;
        while(pCur)
        {
            if (_wcsicmp(pCur->szModuleName, szModule) == 0 &&
                wcscmp(pCur->szFunctionName, szName) == 0)
            {
                if (pPrev == NULL)
                {
                    m_breakpoints = pCur->pNext;
                }
                else
                {
                    pPrev->pNext = pCur->pNext;
                }
                delete pCur;
                return;
            }
            pPrev = pCur;
            pCur = pCur->pNext;
        }
    }

    // Return TRUE if there is any JITTED code for pMeth
    BOOL ResolveMethodInstances(IXCLRDataMethodDefinition *pMeth)
    {
        BOOL bFoundCode = FALSE;
        BOOL bNeedDefer = FALSE;
        CLRDATA_ENUM h1;
        
        if (pMeth->StartEnumInstances (NULL, &h1) == S_OK)
        {
            IXCLRDataMethodInstance *inst = NULL;
            while (pMeth->EnumInstance (&h1, &inst) == S_OK)
            {
                CLRDATA_ADDRESS addr = 0;
                if (inst->GetRepresentativeEntryAddress(&addr) == S_OK)
                {
                    IssueDebuggerBPCommand(addr);
                    bFoundCode = TRUE;
                }
                inst->Release();
            }
            pMeth->EndEnumInstances (h1);
        }
        // if this is a generic method we need to add a defered bp
        BOOL bGeneric = FALSE;
        pMeth->HasClassOrMethodInstantiation(&bGeneric);

        bNeedDefer = !bFoundCode || bGeneric;
        // This is down here because we only need to call SetCodeNofiication once.
        if (bNeedDefer)
        {
            if (pMeth->SetCodeNotification (CLRDATA_METHNOTIFY_GENERATED) != S_OK)
            {
                bNeedDefer = FALSE;
                ExtOut ("Failed to set code notification\n");
            }
        }
        return bNeedDefer;
    }

    // Returns TRUE if the PendingBreakpoint was resolved to the IXCLRDataModule passed in
    BOOL ResolvePendingBreakpoint(IXCLRDataModule* mod, PendingBreakpoint *pCur)
    {
        // We need to take the mod's idea of name and turn it into our own.
        BOOL bDeleteCurrent = FALSE;
        DWORD_PTR *moduleList = NULL;
        int numModule;
        ToDestroy des0 ((void**)&moduleList);
        char szName[mdNameLen];
        BOOL bEntryMatches = FALSE;
        DacpGetModuleAddress dgma;

        dgma.Request(mod);        
        
        WideCharToMultiByte(CP_ACP, 0, pCur->szModuleName, (int) (wcslen(pCur->szModuleName) + 1),
                        szName, mdNameLen, NULL, NULL);            
        ModuleFromName(moduleList, szName, numModule);

        for(int i=0;i<numModule;i++)
        {
            // If any one entry in moduleList matches, then the current PendingBreakpoint
            // is the right one.
            bEntryMatches = (moduleList[i] == dgma.ModulePtr);
            if (bEntryMatches)
            {
                break;
            }
        }

        // Only go forward if the IXCLRDataModule matches the current PendingBreakpoint
        if (!bEntryMatches)
        {
            return FALSE;
        }
                
        CLRDATA_ENUM h;
        if (mod->StartEnumMethodDefinitionsByName(pCur->szFunctionName, 0, &h) == S_OK)
        {
            IXCLRDataMethodDefinition *pMeth = NULL;
            while (mod->EnumMethodDefinitionByName(&h, &pMeth) == S_OK)
            {
                // We may not need the code notification. Maybe it was ngen'd and we
                // already have the method?
                // We need to delete the current entry if ResolveMethodInstances() set all BPs
                bDeleteCurrent = !ResolveMethodInstances(pMeth);

                pMeth->Release();
            }
            mod->EndEnumMethodDefinitionsByName(h);
        }
        return bDeleteCurrent;
    }
};

Breakpoints g_bpoints;

class CNotification : public IXCLRDataExceptionNotification
{
    int m_count;
public:
    CNotification() { m_count = 0; }

    STDMETHODIMP QueryInterface (REFIID iid, void **ppvObject)
    {
        if (ppvObject == NULL)
            return E_INVALIDARG;

        if (iid == IID_IUnknown)
        {
            *ppvObject = (IUnknown *) this;
        }
        else if (iid == IID_IXCLRDataExceptionNotification)
        {
            *ppvObject = (IXCLRDataExceptionNotification *) this;
        }
        AddRef();
        return S_OK;
    }

    STDMETHODIMP_(ULONG) AddRef(void) { return ++m_count; }
    STDMETHODIMP_(ULONG) Release(void)
    {
        m_count--;
        if (m_count < 0)
        {
            m_count = 0;
        }
        return m_count;
    }

            
    /*
     * New code was generated or discarded for a method.:
     */
    STDMETHODIMP OnCodeGenerated(IXCLRDataMethodInstance* method)
    {
        g_bpoints.Update(method);
        return S_OK;
    }

    STDMETHODIMP OnCodeDiscarded(IXCLRDataMethodInstance* method)
    {
        return E_NOTIMPL;
    }

    /*
     * The process or task reached the desired execution state.
     */
    STDMETHODIMP OnProcessExecution(ULONG32 state) { return E_NOTIMPL; }
    STDMETHODIMP OnTaskExecution(IXCLRDataTask* task,
                            ULONG32 state) { return E_NOTIMPL; }

    /*
     * The given module was loaded or unloaded.
     */
    STDMETHODIMP OnModuleLoaded(IXCLRDataModule* mod)
    {
        g_bpoints.Update(mod);

        return S_OK;
    }

    STDMETHODIMP OnModuleUnloaded(IXCLRDataModule* mod)
    {
        return E_NOTIMPL;
    }

    /*
     * The given type was loaded or unloaded.
     */
    STDMETHODIMP OnTypeLoaded(IXCLRDataTypeInstance* typeInst) { return E_NOTIMPL; }
    STDMETHODIMP OnTypeUnloaded(IXCLRDataTypeInstance* typeInst) { return E_NOTIMPL; }
};

HRESULT HandleCLRNotificationEvent()
{
    HRESULT Status;

    /*
     * Did we get module load notification? If so, check if any in our pending list
     * need to be registered for jit notification.
     *
     * Did we get a jit notification? If so, check if any can be removed and
     * real breakpoints be set.
     */
    DEBUG_LAST_EVENT_INFO_EXCEPTION dle;
    ULONG Type, ProcessId, ThreadId;
    ULONG ExtraInformationUsed;
    CNotification Notification;
    Status = g_ExtControl->GetLastEventInformation( &Type,
                                                    &ProcessId,
                                                    &ThreadId,
                                                    &dle,
                                                    sizeof(DEBUG_LAST_EVENT_INFO_EXCEPTION),
                                                    &ExtraInformationUsed,
                                                    NULL,
                                                    0,
                                                    NULL);

    if (Status != S_OK)
    {
        ExtOut ("Error in GetLastEventInformation\n");
        return Status;
    }

    if (Type != DEBUG_EVENT_EXCEPTION)
    {
        ExtOut ("Expecting an exception event\n");
        return Status;
    }

    // Notification only needs to live for the lifetime of the call below, so it's a non-static
    // local.
    if (g_clrData->TranslateExceptionRecordToNotification(&dle.ExceptionRecord, &Notification) != S_OK)
    {
        ExtOut ("Error processing exception notification\n");
        return Status;
    }

    return Status;
}

DECLARE_API(bpmd)
{
    INIT_API();    
    MINIDUMP_NOT_SUPPORTED();    
    int i;
    char buffer[1024];    
    
    if (IsDumpFile())
    {
        ExtOut ("!bpmd is not supported on a dump file.\n");
        return Status;
    }
    
    // We keep a list of managed breakpoints the user wants to set, and display pending bps
    // bpmd. If you call bpmd <module name> <method> we will set or update an existing bp.
    // bpmd acts as a feeder of breakpoints to bp when the time is right.
    //

    StringHolder DllName,TypeName;

    BOOL bNotification = FALSE;
    DWORD_PTR pMD = NULL;
    CMDOption option[] = {
        // name, vptr, type, hasValue
        {"-notification", &bNotification, COBOOL, FALSE},
        {"-md", &pMD, COHEX, TRUE},
    };
    
    CMDValue arg[] = {
        // vptr, type
        {&DllName.data, COSTRING},
        {&TypeName.data, COSTRING}
    };
    size_t nArg;
    
    if (!GetCMDOption(args,option,sizeof(option)/sizeof(CMDOption),arg,sizeof(arg)/sizeof(CMDValue),&nArg)) 
    {
        return Status;
    }

    if (nArg != 2 && !bNotification && (pMD==NULL))
    {
        ExtOut("Usage: !bpmd -md <MethodDesc pointer>\n");
        ExtOut("Usage: !bpmd <module name> <managed function name>\n");
        ExtOut("See \"!help bpmd\" for more details.\n");
        return Status;
    }

    if (bNotification)
    {
        return HandleCLRNotificationEvent();
    }

    // Add a breakpoint
    // Do we already have this breakpoint?
    // Or, before setting it, is the module perhaps already loaded and code
    // is available? If so, don't add to our pending list, just go ahead and
    // set the real breakpoint.    
    
    WCHAR ModuleName[mdNameLen];
    WCHAR FunctionName[mdNameLen];
    BOOL bNeedNotificationExceptions=FALSE;

    if (pMD == NULL)
    {
        int numModule;
        DWORD_PTR *moduleList = NULL;
        int numMethods = 0;                
        DWORD_PTR *pMDs = NULL;
        ULONG error = 1;
        ToDestroy des0 ((void**)&moduleList);
        ToDestroyCxxArray<DWORD_PTR> tdcz(&pMDs);    

        MultiByteToWideChar(CP_ACP, 0, DllName.data, -1, ModuleName, mdNameLen);
        MultiByteToWideChar(CP_ACP, 0, TypeName.data, -1, FunctionName, mdNameLen);

        ModuleFromName(moduleList, DllName.data, numModule);
        if (numModule > 1)
        {
            ExtOut ("There are multiple modules with the same name...\n");
            return Status;
        }        
        else if (numModule == 1)
        {
            error = GetMethodDescsFromName(moduleList[0], TypeName.data, pMDs, numMethods);    
            if (error)
            {
                ExtOut("Error getting MethodDescs\n");
                return Status;
            }

            ExtOut("Found %d methods...\n", numMethods);
            
            DacpMethodDescData MethodDescData;

            for (i=0; i<numMethods;i++)
            {
                // this ensures the MD will be considered for a defered breakpoint
                if (pMDs[i] == MD_NOT_YET_LOADED)
                {
                    continue;
                }
                ExtOut("MethodDesc = %p\n", (ULONG64) pMDs[i]);
                if ((MethodDescData.Request(g_clrData,(CLRDATA_ADDRESS) pMDs[i]) == S_OK) &&
                    (MethodDescData.bHasNativeCode))
                {
                    IssueDebuggerBPCommand((size_t) MethodDescData.NativeCodeAddr);
                    pMDs[i] = NULL;
                }
            }

            // Did any methods not get jitted?
            for (i=0;i<numMethods;i++)
            {
                // If any one method is non-null, the pending breakpoint can be added.
                // If there are overloaded methods, the pending breakpoint list will take
                // care that each one gets a jit notification.
                if ((pMDs[i] != NULL))
                {
                    // We didn't find code, add a breakpoint.
                    g_bpoints.Add(ModuleName, FunctionName);
                    DacpModuleData dmd;
                    dmd.Request(g_clrData,(CLRDATA_ADDRESS)moduleList[0]);
                    g_bpoints.Update(dmd.ModuleDefinition);
                    bNeedNotificationExceptions = TRUE;
                    break;
                }
            }
        }
        else
        {
            // We didn't find a module, add a pending breakpoint, and
            // wait for the module load notification.
            g_bpoints.Add(ModuleName, FunctionName);
            bNeedNotificationExceptions = TRUE;
        }
    }
    else /* We were given a MethodDesc already */
    {
        DacpMethodDescData MethodDescData;
        ExtOut("MethodDesc = %p\n", (ULONG64) pMD);
        if (MethodDescData.Request(g_clrData,(CLRDATA_ADDRESS) pMD) != S_OK)
        {
            ExtOut ("%p is not a valid MethodDesc\n", (ULONG64) pMD);
            return Status;
        }
        
        if (MethodDescData.bHasNativeCode)
        {
            IssueDebuggerBPCommand((size_t) MethodDescData.NativeCodeAddr);
        }
        else if (MethodDescData.bIsDynamic)
        {
            // Dynamic methods don't have JIT notifications. This is something we must
            // fix in the next release. Until then, you have a cumbersome user experience.
            ExtOut("This DynamicMethodDesc is not yet JITTED. Placing memory breakpoint at %p\n",
                MethodDescData.AddressOfNativeCodeSlot);
            
            sprintf(buffer,
#ifdef _X86_            
                "ba w4" 
#else
                "ba w8"
#endif // _X86_  

                " /1 %p \"bp poi(%p); g\"",
                (size_t) MethodDescData.AddressOfNativeCodeSlot,
                (size_t) MethodDescData.AddressOfNativeCodeSlot);

            Status = g_ExtControl->Execute(DEBUG_EXECUTE_NOT_LOGGED, buffer ,0);
            if (FAILED(Status))
            {
                ExtOut("Unable to set breakpoint with IDebugControl::Execute: %x\n",Status);
                ExtOut("Attempted to run: %s\n", buffer);                
            }            
        }
        else
        {
            // Must issue a pending breakpoint.
            
            if (DacpMethodDescData::GetMethodName(g_clrData, pMD, mdNameLen, FunctionName) != S_OK)
            {
                ExtOut ("Unable to get method name for MethodDesc %p\n", (ULONG64) pMD);
                return Status;
            }

            FileNameForModule ((DWORD_PTR) MethodDescData.ModulePtr, ModuleName);

            // We didn't find code, add a breakpoint.
            g_bpoints.Add(ModuleName, FunctionName);
            DacpModuleData dmd;
            dmd.Request(g_clrData,MethodDescData.ModulePtr);
            g_bpoints.Update(dmd.ModuleDefinition);
            bNeedNotificationExceptions = TRUE;            
        }
    }

    if (bNeedNotificationExceptions)
    {
        ExtOut ("Adding pending breakpoints...\n");
        sprintf(buffer,"sxe -c \"!bpmd -notification;g\" clrn");
        Status = g_ExtControl->Execute(DEBUG_EXECUTE_NOT_LOGGED, buffer ,0);        
    }

    return Status;
}



/**********************************************************************\
* Routine Description:                                                 *
*                                                                      *
*    This function is called to dump the managed threadpool            *
*                                                                      *
\**********************************************************************/
DECLARE_API(ThreadPool)
{
    INIT_API();
    MINIDUMP_NOT_SUPPORTED();    
    
    DacpThreadpoolData threadpool;

    if ((Status = threadpool.Request(g_clrData)) != S_OK)
    {
        ExtOut("Failed to request ThreadpoolMgr information\n");
        return Status;
    }

    ExtOut ("CPU utilization %d%%\n", threadpool.cpuUtilization);            
    ExtOut ("Worker Thread:");
    ExtOut (" Total: %d", threadpool.NumWorkerThreads);
    ExtOut (" Running: %d", threadpool.NumRunningWorkerThreads);
    ExtOut (" Idle: %d", threadpool.NumIdleWorkerThreads);
    ExtOut (" MaxLimit: %d", threadpool.MaxLimitTotalWorkerThreads);        
    ExtOut (" MinLimit: %d", threadpool.MinLimitTotalWorkerThreads);        
    ExtOut ("\n");        

    ExtOut ("Work Request in Queue: %d\n", threadpool.NumQueuedWorkRequests);    

    CLRDATA_ADDRESS workRequestPtr = threadpool.FirstWorkRequest;
    DacpWorkRequestData workRequestData;
    while (workRequestPtr)
    {
        if ((Status = workRequestData.Request(g_clrData,workRequestPtr))!=S_OK)
        {
            ExtOut("Failed to examine a WorkRequest\n");
            return Status;
        }


        if (workRequestData.Function == threadpool.QueueUserWorkItemCallbackFPtr)
            ExtOut ("QueueUserWorkItemCallback DelegateInfo@%p\n", (ULONG64)workRequestData.Context);
        else if (workRequestData.Function == threadpool.AsyncCallbackCompletionFPtr)
            ExtOut ("AsyncCallbackCompletion AsyncCallback@%p\n", (ULONG64)workRequestData.Context);
        else if (workRequestData.Function == threadpool.AsyncTimerCallbackCompletionFPtr)
            ExtOut ("AsyncTimerCallbackCompletion TimerInfo@%p\n", (ULONG64)workRequestData.Context);
        else
            ExtOut ("Unknown Function: %p  Context: %p\n", (ULONG64)workRequestData.Function,
                (ULONG64)workRequestData.Context);

        workRequestPtr = workRequestData.NextWorkRequest;
    }            
        
    ExtOut ("--------------------------------------\n");
    ExtOut ("Number of Timers: %d\n", threadpool.NumTimers);
    ExtOut ("--------------------------------------\n");
    
    ExtOut ("Completion Port Thread:");
    ExtOut ("Total: %d", threadpool.NumCPThreads);    
    ExtOut (" Free: %d", threadpool.NumFreeCPThreads);    
    ExtOut (" MaxFree: %d", threadpool.MaxFreeCPThreads);    
    ExtOut (" CurrentLimit: %d", threadpool.CurrentLimitTotalCPThreads);
    ExtOut (" MaxLimit: %d", threadpool.MaxLimitTotalCPThreads);
    ExtOut (" MinLimit: %d", threadpool.MinLimitTotalCPThreads);        
    ExtOut ("\n");

    return Status;
}

DECLARE_API(FindAppDomain)
{
    INIT_API();
    MINIDUMP_NOT_SUPPORTED();    
    
    
    DWORD_PTR p_Object = GetExpression (args);

    if ((p_Object == 0) || !IsObject (p_Object))
    {
        ExtOut ("%p is not a valid object\n", (ULONG64) p_Object);
        return Status;
    }
    
    DacpAppDomainStoreData adstore;
    if (adstore.Request(g_clrData) != S_OK)
    {
        ExtOut ("Error getting AppDomain information\n");
        return Status;
    }    

    CLRDATA_ADDRESS appDomain = GetAppDomain ((CLRDATA_ADDRESS)p_Object);

    if (appDomain != NULL)
    {
        ExtOut ("AppDomain: %p\n", (ULONG64) appDomain);
        if (appDomain == adstore.sharedDomain)
        {            
            ExtOut ("Name: Shared Domain\n");
            ExtOut ("ID: (shared domain)\n");            
        }
        else if (appDomain == adstore.systemDomain)
        {            
            ExtOut ("Name: System Domain\n");
            ExtOut ("ID: (system domain)\n");
        }
        else
        {
            DacpAppDomainData domain;
            if ((domain.Request(g_clrData, appDomain) != S_OK) ||
                (DacpAppDomainData::GetName(g_clrData,appDomain,mdNameLen,g_mdName)!=S_OK))
            {
                ExtOut ("Error getting AppDomain %p\n", (ULONG64) appDomain);
                return Status;
            }
            
            ExtOut ("Name: %S\n",(g_mdName[0]!=L'\0') ? g_mdName : L"None");            
            ExtOut ("ID: %d\n", domain.dwId);
        }
    }
    else
    {        
        ExtOut ("The type is declared in the shared domain and other\n");
        ExtOut ("methods of finding the AppDomain failed. Try running\n");
        ExtOut ("!gcroot %p, and if you find a root on a stack, check \n", (ULONG64) p_Object);
        ExtOut ("the AppDomain of that stack with !threads. Note that\n");
        ExtOut ("the Thread could have transitioned between multiple\n");
        ExtOut ("AppDomains.\n");     
    }    
    
    return Status;
}

/**********************************************************************\
* Routine Description:                                                 *
*                                                                      *
*    This function is called to get the COM state (e.g. APT,contexe    *
*    activity.                                                         *  
*                                                                      *
\**********************************************************************/


BOOL traverseEh(UINT clauseIndex,UINT totalClauses,DACEHInfo *pEHInfo,LPVOID token)
{
    size_t methodStart = (size_t) token;
    
    if (IsInterrupt())
    {
        return FALSE;
    }

    ExtOut ("EHHandler %d: %s ", clauseIndex, EHTypeName(pEHInfo->clauseType));

    if (pEHInfo->clauseType == EHTyped &&
        pEHInfo->isCatchAllHandler)
        ExtOut ("catch(...) ");

    if (pEHInfo->isDuplicateClause)
        ExtOut ("(duplicate)");

    ExtOut ("\n");
    ExtOut ("Clause: [%p, %p] [%x, %x]\n", 
        (ULONG64) (pEHInfo->tryStartOffset + methodStart),
        (ULONG64) (pEHInfo->tryEndOffset + methodStart),
        (UINT32) pEHInfo->tryStartOffset,
        (UINT32) pEHInfo->tryEndOffset);
    
    ExtOut ("Handler: [%p, %p] [%x, %x]\n", 
        (ULONG64) (pEHInfo->handlerStartOffset+ methodStart),
        (ULONG64) (pEHInfo->handlerEndOffset + methodStart),
        (UINT32) pEHInfo->handlerStartOffset,
        (UINT32) pEHInfo->handlerEndOffset);

    if (pEHInfo->clauseType == EHFilter)
    {
        ExtOut ("Filter: [%p] [%x]\n",
            (ULONG64) (pEHInfo->filterOffset + methodStart),
            (UINT32) pEHInfo->filterOffset);
    }
    
    ExtOut ("\n");        
    return TRUE;
}


DECLARE_API(EHInfo)
{
    INIT_API();
    MINIDUMP_NOT_SUPPORTED();    
    
    DWORD_PTR dwStartAddr = NULL;


    CMDValue arg[] = {
        // vptr, type
        {&dwStartAddr, COHEX},
    };

    size_t nArg;
    if (!GetCMDOption(args,NULL,0,arg,sizeof(arg)/sizeof(CMDValue),&nArg)) {
        return Status;
    }

    if (0 == nArg)
        return Status;

    DWORD_PTR tmpAddr = dwStartAddr;

    if (!IsMethodDesc(dwStartAddr)) {
        JitType jitType;
        DWORD_PTR methodDesc;
        DWORD_PTR gcinfoAddr;
        IP2MethodDesc (dwStartAddr, methodDesc, jitType, gcinfoAddr);
        if (methodDesc) {
            tmpAddr = methodDesc;
        }
        else
            tmpAddr = 0;
    }

    DacpMethodDescData MD;
    if ((tmpAddr == 0) || (MD.Request(g_clrData,(CLRDATA_ADDRESS)tmpAddr) != S_OK))
    {
        ExtOut("%p is not a MethodDesc\n", (ULONG64)tmpAddr);
        return Status;
    }

    if (1 == nArg && !MD.bHasNativeCode)
    {
        ExtOut("No EH info available\n");
        return Status;
    }

    DacpCodeHeaderData codeHeaderData;
    if (codeHeaderData.Request(g_clrData,(CLRDATA_ADDRESS)MD.NativeCodeAddr) != S_OK)
    {
        ExtOut("Unable to get codeHeader information\n");
        return Status;
    }        
    
    ExtOut("MethodDesc: %p\n",(ULONG64) MD.MethodDescPtr);
    DumpMDInfo((DWORD_PTR) MD.MethodDescPtr);         

    ExtOut ("\n");
    Status = DacpEHInfoTraverse::DoTraverse(g_clrData,traverseEh,(LPVOID)MD.NativeCodeAddr,(CLRDATA_ADDRESS)MD.NativeCodeAddr);

    if (Status == E_ABORT)
    {
        ExtOut ("<user aborted>\n");
    }
    else if (Status != S_OK)
    {
        ExtOut ("Failed to perform EHInfo traverse\n");
    }
    
    return Status;
}



void PrintNothing (const char *fmt, ...)
{
    // Do nothing.
}


BOOL gatherEh(UINT clauseIndex,UINT totalClauses,DACEHInfo *pEHInfo,LPVOID token)
{
    SOSEHInfo *pInfo = (SOSEHInfo *) token;

    if (pInfo == NULL)
    {
        return FALSE;
    }
    
    if (pInfo->m_pInfos == NULL)
    {
        // First time, initialize structure
        pInfo->EHCount = totalClauses;
        pInfo->m_pInfos = new DACEHInfo[totalClauses];
        if (pInfo->m_pInfos == NULL)
        {
            ReportOOM();            
            return FALSE;
        }
    }

    pInfo->m_pInfos[clauseIndex] = *pEHInfo;
    return TRUE;
}


/**********************************************************************\
* Routine Description:                                                 *
*                                                                      *
*    This function is called to unassembly a managed function.         *
*    It tries to print symbolic info for function call, contants...    *  
*                                                                      *
\**********************************************************************/
DECLARE_API(u)
{
    INIT_API();
    MINIDUMP_NOT_SUPPORTED();    
    
    
    DWORD_PTR dwStartAddr;
    BOOL fWithGCInfo = FALSE;
    BOOL fWithEHInfo = FALSE;    
    size_t nArg;

    CMDOption option[] = {
        // name, vptr, type, hasValue
        {"-gcinfo", &fWithGCInfo, COBOOL, FALSE},
        {"-ehinfo", &fWithEHInfo, COBOOL, FALSE},
    };
    
    CMDValue arg[] = {
        // vptr, type
        {&dwStartAddr, COHEX},
    };

    if (!GetCMDOption(args,option,sizeof(option)/sizeof(CMDOption),arg,sizeof(arg)/sizeof(CMDValue),&nArg))
        return Status;

    if (nArg < 1)
        return Status;

    DWORD_PTR tmpAddr = dwStartAddr;
    DWORD_PTR methodDesc = tmpAddr;
    if (!IsMethodDesc (tmpAddr))
    {
        tmpAddr = dwStartAddr;
        JitType jt;
        DWORD_PTR gcinfoAddr;
        IP2MethodDesc (tmpAddr, methodDesc, jt,
                       gcinfoAddr);
        if (!methodDesc || jt == UNKNOWN)
        {
            // It is not managed code.
            ExtOut ("Unmanaged code\n");
            UnassemblyUnmanaged(dwStartAddr);
            return Status;
        }
        tmpAddr = methodDesc;
    }

    DacpMethodDescData MethodDescData;
    if ((Status=MethodDescData.Request(g_clrData,(CLRDATA_ADDRESS)tmpAddr)) != S_OK)
    {
        return Status;
    }    

    if (!MethodDescData.bHasNativeCode)
    {
        ExtOut("Not jitted yet\n");
        return Status;
    }

    DacpCodeHeaderData codeHeaderData;
    if (codeHeaderData.Request(g_clrData,(CLRDATA_ADDRESS)MethodDescData.NativeCodeAddr) != S_OK)
    {
        ExtOut("Unable to get codeHeader information\n");
        return Status;
    }                
    
    if (codeHeaderData.MethodStart == 0)
    {
        ExtOut("not a valid MethodDesc\n");
        return Status;
    }
    
    if (codeHeaderData.JITType == TYPE_UNKNOWN)
    {
        ExtOut ("unknown Jit\n");
        return Status;
    }
    else if (codeHeaderData.JITType == TYPE_EJIT)
    {
        ExtOut ("EJIT generated code\n");
    }
    else if (codeHeaderData.JITType == TYPE_JIT)
    {
        ExtOut ("Normal JIT generated code\n");
    }
    else if (codeHeaderData.JITType == TYPE_PJIT)
    {
        ExtOut ("preJIT generated code\n");
    }

    NameForMD_s(tmpAddr,g_mdName,mdNameLen);
    ExtOut ("%S\n", g_mdName);   
    if (codeHeaderData.ColdRegionStart != NULL)
    {
        ExtOut ("Begin %p, size %x. Cold region begin %p, size %x\n",
            (ULONG64)codeHeaderData.MethodStart, codeHeaderData.HotRegionSize,
            (ULONG64)codeHeaderData.ColdRegionStart, codeHeaderData.ColdRegionSize);
    }
    else
    {
        ExtOut ("Begin %p, size %x\n", (ULONG64)codeHeaderData.MethodStart, codeHeaderData.MethodSize);    
    }


    SOSEHInfo *pInfo = NULL;
    if (fWithEHInfo)
    {
        pInfo = new SOSEHInfo;
        if (pInfo == NULL)
        {
            ReportOOM();                
        }
        else if (DacpEHInfoTraverse::DoTraverse(g_clrData,gatherEh,(LPVOID)pInfo,MethodDescData.NativeCodeAddr) != S_OK)
        {
            ExtOut ("Failed to gather EHInfo data\n");
            delete pInfo;
            pInfo = NULL;            
        }
    }
    
    if (codeHeaderData.ColdRegionStart == NULL)
    {
        Unassembly (
                (DWORD_PTR) codeHeaderData.MethodStart,
                ((DWORD_PTR)codeHeaderData.MethodStart) + codeHeaderData.MethodSize,
                dwStartAddr,
                (DWORD_PTR) MethodDescData.GCStressCodeCopy,
                NULL,
                pInfo
                );
    }
    else
    {
        ExtOut ("Hot region:\n");
        Unassembly (
                (DWORD_PTR) codeHeaderData.MethodStart,
                ((DWORD_PTR)codeHeaderData.MethodStart) + codeHeaderData.HotRegionSize,
                dwStartAddr,
                (DWORD_PTR) MethodDescData.GCStressCodeCopy,
                NULL,
                pInfo
                );

        ExtOut ("Cold region:\n");
        
        Unassembly (
                (DWORD_PTR) codeHeaderData.ColdRegionStart,
                ((DWORD_PTR)codeHeaderData.ColdRegionStart) + codeHeaderData.ColdRegionSize,
                dwStartAddr,
                ((DWORD_PTR) MethodDescData.GCStressCodeCopy) + codeHeaderData.HotRegionSize,                
                NULL,
                pInfo
                );

    }

    if (pInfo)
    {
        delete pInfo;
        pInfo = NULL;
    }
    

    return Status;
}

/**********************************************************************\
* Routine Description:                                                 *
*                                                                      *
*    This function is called to dump the build number and type of the  *  
*    mscoree.dll                                                       *
*                                                                      *
\**********************************************************************/
DECLARE_API (DumpLog)
{        
    INIT_API_NODAC();
    MINIDUMP_NOT_SUPPORTED();    
    

    if (GetEEFlavor() == UNKNOWNEE) 
    {
        ExtOut("CLR not loaded\n");
        return Status;
    }

    const char* fileName = "StressLog.txt";

    CLRDATA_ADDRESS StressLogAddress = NULL;
    
    StringHolder sFileName, sLogAddr;
    CMDOption option[] = {
        // name, vptr, type, hasValue
        {"-addr", &sLogAddr.data, COSTRING, TRUE}
    };
    CMDValue arg[] = {
        // vptr, type
        {&sFileName.data, COSTRING}
    };
    size_t nArg;

    if (!GetCMDOption(args, option, sizeof(option)/sizeof(CMDOption),
                      arg, sizeof(arg)/sizeof(CMDValue), &nArg)) {
        return Status;
    }

    if (nArg > 0 && sFileName.data != NULL)
        fileName = sFileName.data;
    // allow users to specify -addr mscordbdi!StressLog::theLog, for example.
    if (sLogAddr.data != NULL)
        StressLogAddress = GetExpression(sLogAddr.data);

    if (StressLogAddress == NULL)
    {
    if (LoadClrDebugDll() != S_OK)
    {
        // Try to find stress log symbols
        DWORD_PTR dwAddr = GetValueFromExpression("MSCORWKS!StressLog::theLog");
        StressLogAddress = dwAddr;        
        g_bDacBroken = TRUE;
    }
    else
    {
        DacpStressLogData sld;
        if (sld.Request(g_clrData) != S_OK)
        {
            ExtOut("Unable to find stress log via DAC\n");
            return E_FAIL;
        }
        StressLogAddress = sld.StressLogAddress;
        g_bDacBroken = FALSE;
    }
    }

    ExtOut("Attempting to dump Stress log to file '%s'\n", fileName);

    
    
    Status = StressLog::Dump(StressLogAddress, fileName, g_ExtData);

    if (Status == S_OK)
        ExtOut("SUCCESS: Stress log dumped\n");
    else if (Status == S_FALSE)
        ExtOut("No Stress log in the image, no file written\n");
    else
        ExtOut("FAILURE: Stress log not dumped\n");

    return Status;
}

//#define TRACE_GC

#ifdef TRACE_GC

DECLARE_API (DumpGCLog)
{
    INIT_API_NODAC();
    MINIDUMP_NOT_SUPPORTED();    
    

    if (GetEEFlavor() == UNKNOWNEE) 
    {
        ExtOut("CLR not loaded\n");
        return Status;
    }

    const char* fileName = "GCLog.txt";

    while (isspace (*args))
        args ++;

    if (*args != 0)
        fileName = args;
    
    // Try to find stress log symbols
    DWORD_PTR dwAddr = GetValueFromExpression("mscorwks!SVR::gc_log_buffer");
    moveN (dwAddr, dwAddr);

    if (dwAddr == 0)
    {
        dwAddr = GetValueFromExpression("mscorwks!WKS::gc_log_buffer");
        moveN (dwAddr, dwAddr);
        if (dwAddr == 0)
        {
            ExtOut ("Can't get either WKS or SVR GC's log file");
            return E_FAIL;
        }
    }

    ExtOut ("Dumping GC log at %08x\n", dwAddr);

    g_bDacBroken = FALSE;
    
    ExtOut("Attempting to dump GC log to file '%s'\n", fileName);
    
    Status = E_FAIL;

    HANDLE hGCLog = CreateFileA(
        fileName,
        GENERIC_WRITE,
        FILE_SHARE_READ,
        NULL,
        CREATE_ALWAYS,
        FILE_ATTRIBUTE_NORMAL,
        NULL);

    if (hGCLog == INVALID_HANDLE_VALUE)
    {
        ExtOut ("failed to create file: %d\n", GetLastError());
        goto exit;
    }

    int iLogSize = 1024*1024;
    BYTE* bGCLog = new BYTE [iLogSize];
    memset (bGCLog, 0, iLogSize);
    if (!SafeReadMemory((ULONG_PTR)dwAddr, bGCLog, iLogSize, NULL))
    {
        ExtOut ("failed to read memory from %08x\n", dwAddr);
    }

    int iRealLogSize = iLogSize - 1;
    while (iRealLogSize >= 0)
    {
        if (bGCLog[iRealLogSize] != '*')
        {
            break;
        }

        iRealLogSize--;
    }

    DWORD dwWritten = 0;
    WriteFile (hGCLog, bGCLog, iRealLogSize + 1, &dwWritten, NULL);
    Status = S_OK;

exit:

    if (hGCLog != INVALID_HANDLE_VALUE)
    {
        CloseHandle (hGCLog);
    }

    if (Status == S_OK)
        ExtOut("SUCCESS: Stress log dumped\n");
    else if (Status == S_FALSE)
        ExtOut("No Stress log in the image, no file written\n");
    else
        ExtOut("FAILURE: Stress log not dumped\n");

    return Status;
}

#endif //TRACE_GC

#ifdef _DEBUG
DECLARE_API (mda)
{
    INIT_API();
    MINIDUMP_NOT_SUPPORTED();    
    
 
    DacpMda sld;
    if (sld.Request(g_clrData, ExtOut, args) != S_OK)
    {
        ExtOut("Communication with runtime via DAC failed.\n");
        return E_FAIL;
    }

    return Status;
}
#endif






/**********************************************************************\
* Routine Description:                                                 *
*                                                                      *
*    This function is called to find the address of EE data for a      *  
*    metadata token.                                                   *
*                                                                      *
\**********************************************************************/
DECLARE_API (Token2EE)
{
    INIT_API();
    MINIDUMP_NOT_SUPPORTED();    
    
 
    StringHolder DllName;
    ULONG64 token = 0;
    
    CMDValue arg[] = {
        // vptr, type
        {&DllName.data, COSTRING},
        {&token, COHEX}
    };
    size_t nArg;
    
    if (!GetCMDOption(args,NULL,0,arg,sizeof(arg)/sizeof(CMDValue),&nArg)) {
        return Status;
    }

    if (nArg!=2)
    {
        ExtOut ("Usage: Token2EE module_name mdToken\n");
        ExtOut ("       you can pass * for module_name to search all modules\n");
        return Status;
    }

    int numModule;
    DWORD_PTR *moduleList = NULL;

    if ( strcmp (DllName.data, "*") == 0)
    {
        ModuleFromName(moduleList, NULL, numModule);
    }
    else
    {
        ModuleFromName(moduleList, DllName.data, numModule);
    }

    ToDestroy des0 ((void**)&moduleList);
    
    for (int i = 0; i < numModule; i ++)
    {
        if (IsInterrupt())
            break;

        if (i > 0)
        {
            ExtOut ("--------------------------------------\n");
        }        

        DWORD_PTR dwAddr = moduleList[i];
        WCHAR FileName[MAX_PATH];
        FileNameForModule (dwAddr, FileName);

        // We'd like a short form for this output
        LPWSTR pszFilename = wcsrchr (FileName, L'\\');
        if (pszFilename == NULL)
        {
            pszFilename = FileName;
        }
        else
        {
            pszFilename++; // skip past the last "\" character
        }
        
        ExtOut ("Module: %p (%S)\n", (ULONG64) dwAddr, pszFilename);
        
        GetInfoFromModule (dwAddr, (ULONG) token);
    }
     return Status;
}


/**********************************************************************\
* Routine Description:                                                 *
*                                                                      *
*    This function is called to find the address of EE data for a      *  
*    metadata token.
*                                                                      *
\**********************************************************************/
DECLARE_API (Name2EE)
{
    INIT_API();
    MINIDUMP_NOT_SUPPORTED();    
    
 
    StringHolder DllName,TypeName;
    
    CMDValue arg[] = {
        // vptr, type
        {&DllName.data, COSTRING},
        {&TypeName.data, COSTRING}
    };
    size_t nArg;
    
    if (!GetCMDOption(args,NULL,0,arg,sizeof(arg)/sizeof(CMDValue),&nArg)) {
        return Status;
    }

    if (nArg == 1)
    {
        // The input may be in the form <modulename>!<type>
        // If so, do some surgery on the input params.

        // There should be only 1 ! character
        LPSTR pszSeperator = strchr (DllName.data, '!');
        if (pszSeperator != NULL)
        {
            if (strchr (pszSeperator + 1, '!') == NULL)
            {
                size_t capacity_TypeName_data = strlen(pszSeperator + 1) + 1;
                TypeName.data = new char[capacity_TypeName_data];
                if (TypeName.data)
                {
                    // get the type name,
                    strcpy_s (TypeName.data, capacity_TypeName_data, pszSeperator + 1);
                    // and truncate DllName
                    *pszSeperator = '\0';

                    // Do some extra validation
                    if (strlen (DllName.data) >= 1 &&
                        strlen (TypeName.data) > 1)
                    {
                        nArg = 2;
                    }
                }
            }
        }
    }
    
    if (nArg != 2)
    {
        ExtOut ("Usage: !Name2EE module_name item_name\n");
        ExtOut ("  or    !Name2EE module_name!item_name\n");        
        ExtOut ("       use * for module_name to search all loaded modules\n");
        ExtOut ("Examples: !Name2EE  mscorlib.dll System.String.ToString\n");
        ExtOut ("          !Name2EE *!System.String\n");
        return Status;
    }
    
    int numModule;
    DWORD_PTR *moduleList = NULL;
    if ( strcmp (DllName.data, "*") == 0)
    {
        ModuleFromName(moduleList, NULL, numModule);
    }
    else
    {
        ModuleFromName(moduleList, DllName.data, numModule);
    }
    ToDestroy des0 ((void**)&moduleList);
    
    for (int i = 0; i < numModule; i ++)
    {
        if (IsInterrupt())
            break;

        if (i > 0)
        {
            ExtOut ("--------------------------------------\n");
        }
        
        DWORD_PTR dwAddr = moduleList[i];
        WCHAR FileName[MAX_PATH];
        FileNameForModule (dwAddr, FileName);

        // We'd like a short form for this output
        LPWSTR pszFilename = wcsrchr (FileName, L'\\');
        if (pszFilename == NULL)
        {
            pszFilename = FileName;
        }
        else
        {
            pszFilename++; // skip past the last "\" character
        }
        
        ExtOut ("Module: %p (%S)\n", (ULONG64) dwAddr, pszFilename);
            
        GetInfoFromName(dwAddr, TypeName.data);
    }
 
    return Status;
}




struct GCHandleStatsForDomains
{
private:    
    BOOL m_singleDomainMode;
public:
    GCHandleStatistics *m_pStatistics;
    CLRDATA_ADDRESS *m_pDomainPointers;    
    int m_numDomains;    
    
    const static int SHARED_DOMAIN_INDEX = 0;
    const static int SYSTEM_DOMAIN_INDEX = 1;
    
    GCHandleStatsForDomains(BOOL singleDomainMode) 
    { 
        m_singleDomainMode = singleDomainMode; 
        m_numDomains = 0; 
        m_pStatistics = NULL; 
        m_pDomainPointers = NULL; 
    }    
    
    BOOL Init()
    {
        if (m_singleDomainMode)
        {
            m_numDomains = 0;
            m_pStatistics = new GCHandleStatistics();
            if (m_pStatistics == NULL)
            {
                return FALSE;
            }
        }
        else
        {
            DacpAppDomainStoreData adsData;

            if (adsData.Request(g_clrData) != S_OK)
            {
                return FALSE;
            }

            m_numDomains = adsData.DomainCount + 2;
            CLRDATA_ADDRESS *pArray = new CLRDATA_ADDRESS[adsData.DomainCount+2];
            if (pArray==NULL)
            {
                return FALSE;
            }

            pArray[SHARED_DOMAIN_INDEX] = adsData.sharedDomain;
            pArray[SYSTEM_DOMAIN_INDEX] = adsData.systemDomain;
            
            if (DacpAppDomainStoreData::GetDomains(g_clrData,adsData.DomainCount,pArray+2) != S_OK)
            {
                delete [] pArray;
                return FALSE;
            }
            m_pDomainPointers = pArray;
            m_pStatistics = new GCHandleStatistics[adsData.DomainCount+2];
            if (m_pStatistics == NULL)
            {
                return FALSE;
            }
        }
        return TRUE;
    }

    GCHandleStatistics *GetStats(CLRDATA_ADDRESS appDomainPtr)
    {
        if (m_singleDomainMode)
        {
            // You can pass NULL appDomainPtr if you are in singleDomainMode
            return m_pStatistics;
        }
        else
        {
            for (int i=0; i < m_numDomains; i++)
            {
                if (m_pDomainPointers[i] == appDomainPtr)
                {
                    return m_pStatistics + i;
                }
            }
        }
        return NULL;
    }

    ~GCHandleStatsForDomains()
    {
        if (m_singleDomainMode)
        {
            if (m_pStatistics)
                delete m_pStatistics;
        }
        else
        {
            if (m_pStatistics)            
                delete [] m_pStatistics;
            if (m_pDomainPointers)            
                delete [] m_pDomainPointers;
        }
    }
};

BOOL ExamineHandle(CLRDATA_ADDRESS HandleAddr,CLRDATA_ADDRESS HandleValue,int HandleType, 
                   CLRDATA_ADDRESS appDomainPtr,LPVOID token)
{
    DWORD_PTR dwAddrMethTable=0;
    GCHandleStatsForDomains *handleStat = (GCHandleStatsForDomains *) token;
    GCHandleStatistics *pStats = handleStat->GetStats(appDomainPtr);
    
    MOVE (dwAddrMethTable, HandleValue );
    dwAddrMethTable = dwAddrMethTable & ~3;    
    
    if (!IsMTForFreeObj(dwAddrMethTable) && !IsMethodTable (dwAddrMethTable))
    {
        ExtOut("Bad MethodTable for Obj at %p\n", (ULONG64)HandleValue);    
        return TRUE; // don't count this
    }                    
    else
    {
        size_t s = ObjectSize ((DWORD_PTR) HandleValue);
        pStats->hs.Add(dwAddrMethTable, (DWORD) s);                    
    }

    
    if (handleStat != NULL)
    {
        switch (HandleType)
        {
            case HNDTYPE_PINNED: pStats->pinnedHandleCount++; break;
            case HNDTYPE_REFCOUNTED: pStats->refCntHandleCount++; break;    
            case HNDTYPE_STRONG: pStats->strongHandleCount++; break;
            case HNDTYPE_WEAK_SHORT: pStats->weakShortHandleCount++; break;
            case HNDTYPE_WEAK_LONG: pStats->weakLongHandleCount++; break;
            case HNDTYPE_ASYNCPINNED: pStats->asyncPinnedHandleCount++; break;
            default:pStats->unknownHandleCount++; break;
        }
    }
    return !IsInterrupt();
}            

void PrintGCHandleStats (GCHandleStatistics *pStats)
{        
    ExtOut( "Strong Handles: %d\n", pStats->strongHandleCount);
    ExtOut( "Pinned Handles: %d\n", pStats->pinnedHandleCount);
    ExtOut( "Async Pinned Handles: %d\n", pStats->asyncPinnedHandleCount);
    ExtOut( "Ref Count Handles: %d\n", pStats->refCntHandleCount);
    ExtOut( "Weak Long Handles: %d\n", pStats->weakLongHandleCount);
    ExtOut( "Weak Short Handles: %d\n", pStats->weakShortHandleCount);
    ExtOut( "Other Handles: %d\n", pStats->unknownHandleCount);
}
        
/**********************************************************************\
* Routine Description:                                                 *
*                                                                      *
*    This function dumps GC Handle statistics        *
*                                                                      *
\**********************************************************************/
DECLARE_API(GCHandles)
{
    INIT_API();
    MINIDUMP_NOT_SUPPORTED();    
    
    
    BOOL fPerDomain = FALSE;    

    CMDOption option[] = {
        // name, vptr, type, hasValue
        {"-perdomain", &fPerDomain, COBOOL, FALSE},
    };
    
    if (!GetCMDOption(args,option,sizeof(option)/sizeof(CMDOption),NULL,0,NULL))
    {
        return Status;
    }    

    GCHandleStatsForDomains handleStat(!fPerDomain);
    
    if (handleStat.Init() == FALSE)
    {
        ExtOut ("Error getting per-appdomain handle information\n");
        return Status;
    }

    if (DacpHandleTableTraverse::DoTraverse(g_clrData,ExamineHandle,&handleStat) != S_OK)
    {
        ExtOut( "Unable to display GC handles\n");
        if (IsMiniDumpFile())
        {
            ExtOut("A minidump without full memory may not have this information\n");
        }
    }

    if (fPerDomain == FALSE)
    {        
        GCHandleStatistics *pStats = handleStat.GetStats(NULL);
        ExtOut( "GC Handle Statistics:\n");        
        PrintGCHandleStats (pStats);
        PrintGCStat(&(pStats->hs));
    }
    else
    {
        for (int i=0; i < handleStat.m_numDomains; i++)
        {            
            GCHandleStatistics *pStats = &(handleStat.m_pStatistics[i]);

            ExtOut( "------------------------------------------------------------------------------\n");           
            ExtOut( "GC Handle Statistics for AppDomain %p ", (ULONG64) handleStat.m_pDomainPointers[i]);
            if (i == GCHandleStatsForDomains::SHARED_DOMAIN_INDEX)
            {
                ExtOut( "(Shared Domain)\n");
            }
            else if (i == GCHandleStatsForDomains::SYSTEM_DOMAIN_INDEX)
            {
                ExtOut( "(System Domain)\n");
            }
            else
            {
                ExtOut( "\n");
            }
            PrintGCHandleStats (pStats);
            PrintGCStat(&(pStats->hs));
        }
    }
    
    return Status;
}

BOOL derivedFrom(CLRDATA_ADDRESS mtObj,__in __in_z LPWSTR baseString)
{
    // We want to follow back until we get the mt for System.Exception
    DacpMethodTableData dmtd;
    CLRDATA_ADDRESS walkMT = mtObj;
    while(walkMT != NULL)
    {
        if (dmtd.Request (g_clrData, walkMT) != S_OK)
        {
            break;            
        }
        NameForMT_s ((DWORD_PTR) walkMT, g_mdName,mdNameLen);                
        if (wcscmp (baseString, g_mdName) == 0)
        {
            return TRUE;
        }
        walkMT = dmtd.ParentMethodTable;
    }
    return FALSE;
}

DECLARE_API(StopOnException)
{
    INIT_API();
    MINIDUMP_NOT_SUPPORTED();    
    

    char buffer[100+mdNameLen];

    BOOL fDerived = FALSE;
    BOOL fCreate1 = FALSE;    
    BOOL fCreate2 = FALSE;    

    CMDOption option[] = {
        // name, vptr, type, hasValue
        {"-derived", &fDerived, COBOOL, FALSE}, // catch derived exceptions
        {"-create", &fCreate1, COBOOL, FALSE}, // create 1st chance handler
        {"-create2", &fCreate2, COBOOL, FALSE}, // create 2nd chance handler
    };

    StringHolder TypeName,PReg;
    
    CMDValue arg[] = {
        // vptr, type
        {&TypeName.data, COSTRING},
        {&PReg.data, COSTRING}
    };
    size_t nArg;
    
    if (!GetCMDOption(args,option,sizeof(option)/sizeof(CMDOption),
        arg,sizeof(arg)/sizeof(CMDValue),&nArg)) {
        return Status;
    }

    if (IsDumpFile())
    {
        ExtOut ("Live debugging session required\n");
        return Status;
    }
    
    if (nArg < 1 || nArg > 2)
    {
        ExtOut ("usage: StopOnException [-derived] [-create | -create2] <type name>\n");
        ExtOut ("                       [<pseudo-register number for result>]\n");            
        ExtOut ("ex:    StopOnException -create System.OutOfMemoryException 1\n");
        return Status;
    }

    size_t preg = 1; // by default
    if (nArg == 2)
    {
        preg = GetExpression(PReg.data);
        if (preg > 19)
        {
            ExtOut ("Pseudo-register number must be between 0 and 19\n");
            return Status;
        }
    }        

    sprintf_s(buffer,_countof (buffer),
        "r$t%d=0",
        preg);
    Status = g_ExtControl->Execute(DEBUG_EXECUTE_NOT_LOGGED, buffer ,0);
    if (FAILED(Status))
    {
        ExtOut ("Error initialized register $t%d to zero\n", preg);
        return Status;
    }    
    
    if (fCreate1 || fCreate2)
    {            
        sprintf_s(buffer,_countof (buffer),
            "sxe %s \"!soe %s %s %s;.if(@$t%d==0) {g} .else {.echo '%s hit'}\" clr",
            fCreate1 ? "-c" : "-c2",
            fDerived ? "-derived" : "",
            TypeName.data,
            PReg.data,
            preg,
            TypeName.data);
            
        Status = g_ExtControl->Execute(DEBUG_EXECUTE_NOT_LOGGED, buffer ,0);        
        if (FAILED(Status))
        {
            ExtOut ("Error setting breakpoint: %s\n", buffer);
            return Status;
        }        

        ExtOut ("Breakpoint set\n");
        return Status;
    }    

    // Find the last thrown exception on this thread.
    // Does it match? If so, set the register.
    CLRDATA_ADDRESS threadAddr = GetCurrentManagedThread();
    DacpThreadData Thread;
    
    if ((threadAddr == NULL) || (Thread.Request(g_clrData, threadAddr) != S_OK))
    {
        ExtOut("The current thread is unmanaged\n");
        return Status;
    }

    DWORD_PTR dwAddr;
    if (!SafeReadMemory((DWORD_PTR)Thread.lastThrownObjectHandle,
                        &dwAddr,
                        sizeof(dwAddr), NULL))
    {
        ExtOut ("There is no current managed exception on this thread\n");
        return Status;
    }
    
    if (dwAddr)
    {
        WCHAR typeNameWide[mdNameLen];
        MultiByteToWideChar(CP_ACP,0,TypeName.data,-1,typeNameWide,mdNameLen);
        
        DWORD_PTR MTAddr;
        if (SafeReadMemory(dwAddr, &MTAddr, sizeof(MTAddr), NULL))
        {            
            NameForMT_s (MTAddr, g_mdName,mdNameLen);
            if ((wcscmp(g_mdName,typeNameWide) == 0) ||
                (fDerived && derivedFrom(MTAddr,typeNameWide)))
            {
                sprintf_s(buffer,_countof (buffer),
                    "r$t%d=1",
                    preg);
                Status = g_ExtControl->Execute(DEBUG_EXECUTE_NOT_LOGGED, buffer ,0);
                if (FAILED(Status))
                {
                    ExtOut ("Failed to execute the following command: %s\n", buffer);
                }
            }
        }
    }

    return Status;
}



void PrintArgsAndLocals(IXCLRDataStackWalk *pStackWalk, BOOL bArgs, BOOL bLocals)
{
    // Okay, what can I get for arguments?
    IXCLRDataFrame *pFramey = NULL;
    IXCLRDataValue *pVal = NULL;
    
    if (FAILED(pStackWalk->GetFrame(&pFramey)))
    {
        goto TheExit;
    }
    
    ULONG32 argy;
    ULONG32 localy;
    if (FAILED(pFramey->GetNumArguments(&argy)))
    {
        goto TheExit;
    }

    if (FAILED(pFramey->GetNumLocalVariables(&localy)))
    {
        goto TheExit;
    }

    if (bArgs)
    {        
        WCHAR argName[mdNameLen];
        for (ULONG32 i=0; i < argy; i++)
        {   
            if (i == 0)
            {      
                ExtOut ("\n    PARAMETERS:\n");
            }
            
            ULONG32 realNameLen;
            HRESULT hr;
            hr = pFramey->GetArgumentByIndex(i,
                                   &pVal,
                                   sizeof(argName) / sizeof(WCHAR),
                                   &realNameLen,
                                   argName);
            if (FAILED(hr))
            {
                goto TheExit;
            }

            ExtOut ("        ");
            
            if (argName[0] != L'\0')
            {
                ExtOut("%S = ", argName);
            }
            ULONG32 dwSize = 0;
            hr = pVal->GetBytes(0,&dwSize,NULL);
            if (HRESULT_CODE(hr) == ERROR_BUFFER_OVERFLOW)
            {
                BYTE *pByte = new BYTE[dwSize];
                if (pByte == NULL)
                {
                    goto TheExit;
                }
                hr = pVal->GetBytes(dwSize,&dwSize,pByte);
                if (FAILED(hr))
                {
                    ExtOut ("<unable to retrieve data>\n");
                }
                else
                {
                    ULONG64 outVar = 0;
                    switch(dwSize)
                    {
                        case 1: outVar = *((BYTE *) pByte); break;
                        case 2: outVar = *((short *) pByte); break;
                        case 4: outVar = *((DWORD *) pByte); break;
                        case 8: outVar = *((ULONG64 *) pByte); break;
                        default: outVar = 0;
                    }
                    ExtOut ("0x%p\n", (ULONG64) outVar);
                }
                delete [] pByte;
            }
            else
            {
                ExtOut ("<no data>\n");
            }
            
            pVal->Release();
            pVal = NULL;
        }
    }    

    if (bLocals)
    {        
        for (ULONG32 i=0; i < localy; i++)
        {   
            if (i == 0)
            {
                if (argy == 0 || !bArgs)
                {
                    ExtOut ("\n");                    
                }
                ExtOut ("    LOCALS:\n");
            }
            
            HRESULT hr;

            ExtOut ("        ");
            
            hr = pFramey->GetLocalVariableByIndex(i,
                                   &pVal,
                                   0,NULL,NULL);
            if (FAILED(hr))
            {
                goto TheExit;
            }

            ULONG32 numLocations;
            if (SUCCEEDED(pVal->GetNumLocations(&numLocations)) &&
                numLocations == 1)
            {
                ULONG32 flags;
                CLRDATA_ADDRESS addr;
                if (SUCCEEDED(pVal->GetLocationByIndex(0, &flags, &addr)))
                {
                    if (flags == CLRDATA_VLOC_REGISTER)
                    {
                        ExtOut("<CLR reg> ");
                    }
                    else
                    {
                        ExtOut("0x%p ", (ULONG64)addr);
                    }
                }

                // Can I get a name for the item?

                ExtOut ("= ");                
            }
            ULONG32 dwSize = 0;
            hr = pVal->GetBytes(0,&dwSize,NULL);
            if (HRESULT_CODE(hr) == ERROR_BUFFER_OVERFLOW)
            {
                BYTE *pByte = new BYTE[dwSize];
                if (pByte == NULL)
                {
                    goto TheExit;
                }
                hr = pVal->GetBytes(dwSize,&dwSize,pByte);
                if (FAILED(hr))
                {
                    ExtOut ("<unable to retrieve data>\n");
                }
                else
                {
                    ULONG64 outVar = 0;
                    switch(dwSize)
                    {
                        case 1: outVar = *((BYTE *) pByte); break;
                        case 2: outVar = *((short *) pByte); break;
                        case 4: outVar = *((DWORD *) pByte); break;
                        case 8: outVar = *((ULONG64 *) pByte); break;
                        default: outVar = 0;
                    }
                    ExtOut ("0x%p\n", (ULONG64) outVar);
                }
                delete [] pByte;
            }
            else
            {
                ExtOut ("<no data>\n");
            }
            
            pVal->Release();
            pVal = NULL;
        }        
    }
    
TheExit:
    if (pVal)
        pVal->Release();
    if (pFramey)
        pFramey->Release();
}

void TrueStackTrace(PCSTR args);

DECLARE_API(ClrStack)
{
    INIT_API();

    BOOL bAll = FALSE;    
    BOOL bParams = FALSE;
    BOOL bLocals = FALSE;

    CMDOption option[] = {
        // name, vptr, type, hasValue
        {"-a", &bAll, COBOOL, FALSE},
        {"-p", &bParams, COBOOL, FALSE},
        {"-l", &bLocals, COBOOL, FALSE},
    };
    
    if (!GetCMDOption(args,option,sizeof(option)/sizeof(CMDOption),NULL,0,NULL))
    {
        return Status;
    }    
    if (bAll || bParams || bLocals)
    {
        // No parameter or local supports for minidump case!
        MINIDUMP_NOT_SUPPORTED();        
    }

    if (bAll)
    {
        bParams = bLocals = TRUE;
    }

    ULONG id = 0;
    g_ExtSystem->GetCurrentThreadSystemId (&id);
    ExtOut ("OS Thread Id: 0x%x ", id);
    g_ExtSystem->GetCurrentThreadId (&id);
    ExtOut ("(%d)\n", id);

    IXCLRDataStackWalk *pStackWalk = NULL;
    IXCLRDataTask* Task;
    ULONG ThreadId;

    if ((Status = g_ExtSystem->GetCurrentThreadSystemId(&ThreadId)) != S_OK ||
        (Status = g_clrData->GetTaskByOSThreadID(ThreadId, &Task)) != S_OK)
    {
        ExtOut ("Unable to walk the managed stack. The current thread is likely not a \n");
        ExtOut ("managed thread. You can run !threads to get a list of managed threads in\n");
        ExtOut ("the process\n");
        return Status;
    }

    Status = Task->CreateStackWalk(CLRDATA_SIMPFRAME_UNRECOGNIZED |
                                   CLRDATA_SIMPFRAME_MANAGED_METHOD |
                                   CLRDATA_SIMPFRAME_RUNTIME_MANAGED_CODE |
                                   CLRDATA_SIMPFRAME_RUNTIME_UNMANAGED_CODE,
                                   &pStackWalk);

    Task->Release();

    if (Status != S_OK)
    {
        ExtOut("Failed to start stack walk: %lx\n",Status);
        return Status;
    }

    
    ExtOut("%-8s  %-8s\n", "ESP", "EIP");
    while (1)
    {
        if (IsInterrupt())
        {
            ExtOut ("<interrupted>\n");
            goto Exit;
        }
        
        DacpFrameData FrameData;

        if (IsInterrupt())
        {
            ExtOut ("<aborted>\n");
            goto Exit;
        }
        
        if ((Status = FrameData.Request(pStackWalk)) != S_OK)
        {
            ExtOut("Get frame data failed with %lx\n",Status);
            goto Exit;
        }

        CONTEXT context;
        if ((Status=pStackWalk->GetContext(CONTEXT_ALL, sizeof(CONTEXT),
                                           NULL, (BYTE *)&context))!=S_OK)
        {
            ExtOut("GetFrameContext failed: %lx\n",Status);
            goto Exit;
        }

        ExtOut("%08x",FrameData.frameAddr ? FrameData.frameAddr : context.Esp);
        ExtOut(" %08x",context.Eip);

        if (FrameData.frameAddr)
        {
            static WCHAR wszNameBuffer[1024]; // should be large enough
            DWORD_PTR vtAddr;
            MOVE(vtAddr, FrameData.frameAddr);
            if (DacpFrameData::GetFrameName (g_clrData, vtAddr, 1024, wszNameBuffer) == S_OK)
            {
                ExtOut(" [%S: %p]", wszNameBuffer, (ULONG64) FrameData.frameAddr);  
            }
            else
            {                
               ExtOut(" [Frame: %p]",(ULONG64) FrameData.frameAddr);
            }
        }

        ExtOut(" ");

        DacpMethodDescData mdescData;
        
        if (!FrameData.frameAddr)
        {
            // we may have a method, try to get the methoddesc
            if (mdescData.RequestFromIP(g_clrData,context.Eip)==S_OK)
            {
                DumpMDInfo((DWORD_PTR) mdescData.MethodDescPtr,TRUE);
                if (bAll || bParams || bLocals)
                {
                    PrintArgsAndLocals (pStackWalk, bParams, bLocals);
                }
            }
        }
        else
        {
            // See if the frame has associated function info.
            if (mdescData.RequestFromFrame(g_clrData,FrameData.frameAddr)==S_OK)
            {
                DumpMDInfo((DWORD_PTR) mdescData.MethodDescPtr,TRUE);
            }
        }

        ExtOut("\n");
        
        if (pStackWalk->Next()!=S_OK)
            break;
    }
    
Exit:

    if (pStackWalk)
    {
        pStackWalk->Release();
        pStackWalk = NULL;
    }

    return Status;
}

BOOL IsMemoryInfoAvailable()
{
    ULONG Class;
    ULONG Qualifier;
    g_ExtControl->GetDebuggeeType(&Class,&Qualifier);
    if (Qualifier == DEBUG_DUMP_SMALL) 
    {
        g_ExtControl->GetDumpFormatFlags(&Qualifier);
        if ((Qualifier & DEBUG_FORMAT_USER_SMALL_FULL_MEMORY) == 0)            
        {
            if ((Qualifier & DEBUG_FORMAT_USER_SMALL_FULL_MEMORY_INFO) == 0)
            {
                return FALSE;
            }            
        }
    }        
    return TRUE;
}

DECLARE_API( VMMap )
{
    INIT_API();

    if (IsMiniDumpFile() || !IsMemoryInfoAvailable())
    {
        ExtOut ("!VMMap requires a full memory dump (.dump /ma) or a live process.\n");
    }
    else
    {
        vmmap();
    }

    return Status;
}   // DECLARE_API( vmmap )

DECLARE_API( SOSFlush )
{
    INIT_API();

    g_clrData->Flush();
    
    return Status;
}   // DECLARE_API( SOSFlush )

DECLARE_API( VMStat )
{
    INIT_API();

    if (IsMiniDumpFile() || !IsMemoryInfoAvailable())
    {
        ExtOut ("!VMStat requires a full memory dump (.dump /ma) or a live process.\n");
    }
    else
    {
        vmstat();
    }

    return Status;
}   // DECLARE_API( vmmap )

/**********************************************************************\
* Routine Description:                                                 *
*                                                                      *
*    This function saves a dll to a file.                              *  
*                                                                      *
\**********************************************************************/
DECLARE_API(SaveModule)
{
    INIT_API();
    MINIDUMP_NOT_SUPPORTED();    
    

    StringHolder Location;
    DWORD_PTR moduleAddr = NULL;

    CMDValue arg[] = {
        // vptr, type
        {&moduleAddr, COHEX},
        {&Location.data, COSTRING}
    };
    size_t nArg;
    if (!GetCMDOption(args,NULL,0,
                      arg,sizeof(arg)/sizeof(CMDValue),&nArg)) {
        return Status;
    }

    if (nArg != 2)
    {
        ExtOut("Usage: SaveModule <address> <file to save>\n");
        return Status;
    }

    if (moduleAddr == 0) {
        ExtOut ("Invalid arg\n");
        return Status;
    }

    char* ptr = Location.data;
    
    DWORD_PTR dllBase = 0;
    ULONG64 base;
    if (g_ExtSymbols->GetModuleByOffset(moduleAddr,0,NULL,&base) == S_OK) {
        dllBase = (DWORD_PTR) base;
    }
    else if (IsModule (moduleAddr)) {        
        DacpModuleData module;
        module.Request(g_clrData,(CLRDATA_ADDRESS)moduleAddr);
        dllBase = (DWORD_PTR)module.ilBase;
        if (dllBase == 0) {
            ExtOut ("Module does not have base address\n");
            return Status;
        }
    }
    else
    {
        ExtOut ("%p is not a Module or base address\n", (ULONG64)moduleAddr);
        return Status;
    }

    IMAGE_DOS_HEADER DosHeader;
    if (g_ExtData->ReadVirtual(dllBase, &DosHeader, sizeof(DosHeader), NULL) != S_OK)
        return FALSE;
    IMAGE_NT_HEADERS Header;
    if (g_ExtData->ReadVirtual(dllBase + DosHeader.e_lfanew, &Header, sizeof(Header), NULL) != S_OK)
        return FALSE;
    DWORD_PTR sectionAddr = dllBase + DosHeader.e_lfanew + offsetof(IMAGE_NT_HEADERS,OptionalHeader)
            + Header.FileHeader.SizeOfOptionalHeader;    
    IMAGE_SECTION_HEADER section;
    struct MemLocation
    {
        DWORD_PTR VAAddr;
        DWORD_PTR VASize;
        DWORD_PTR FileAddr;
        DWORD_PTR FileSize;
    };
    int nSection = Header.FileHeader.NumberOfSections;
    ExtOut("%u sections in file\n",nSection);
    MemLocation *memLoc = (MemLocation*)_alloca(nSection*sizeof(MemLocation));
    int indxSec = -1;
    int slot;
    for (int n = 0; n < nSection; n++) {
        if (g_ExtData->ReadVirtual(sectionAddr,&section,sizeof(section),NULL) == S_OK) {
            for (slot = 0; slot <= indxSec; slot ++) {
                if (section.PointerToRawData < memLoc[slot].FileAddr) {
                    break;
                }
            }
            for (int k = indxSec; k >= slot; k --) {
                memcpy(&memLoc[k+1], &memLoc[k], sizeof(MemLocation));
            }
            memLoc[slot].VAAddr = section.VirtualAddress;
            memLoc[slot].VASize = section.Misc.VirtualSize;
            memLoc[slot].FileAddr = section.PointerToRawData;
            memLoc[slot].FileSize = section.SizeOfRawData;
            ExtOut("section %d - VA=%x, VASize=%x, FileAddr=%x, FileSize=%x\n",
                n,memLoc[slot].VAAddr,memLoc[slot].VASize,memLoc[slot].FileAddr,
                memLoc[slot].FileSize);
            indxSec ++;
        }
        else
        {
            ExtOut("Fail to read PE section info\n");
            return Status;
        }
        sectionAddr += sizeof(section);
    }

    if (ptr[0] == '\0') {
        ExtOut ("File not specified\n");
        return Status;
    }
    PCSTR file = ptr;
    ptr += strlen(ptr)-1;
    if (isspace(*ptr)) {
        *ptr = '\0';
        ptr --;
    }
    HANDLE hFile = CreateFileA(file,GENERIC_WRITE,0,NULL,CREATE_ALWAYS,0,NULL);
    if (hFile == INVALID_HANDLE_VALUE) {
        ExtOut ("Fail to create file %s\n", file);
        return Status;
    }
    ULONG pageSize = OSPageSize();
    char *buffer = (char *)_alloca(pageSize);
    DWORD nRead;
    DWORD nWrite;
    
    // NT PE Headers
    ULONG64 dwAddr = dllBase;
    ULONG64 dwEnd = dllBase + Header.OptionalHeader.SizeOfHeaders;
    while (dwAddr < dwEnd) {
        nRead = pageSize;
        if (dwEnd - dwAddr < nRead) {
            nRead = (ULONG)(dwEnd - dwAddr);
        }
        if (g_ExtData->ReadVirtual(dwAddr,buffer,nRead,&nRead) == S_OK) {
            WriteFile(hFile,buffer,nRead,&nWrite,NULL);
        }
        else
        {
            ExtOut ("Fail to read memory\n");
            goto end;
        }
        dwAddr += nRead;
    }

    for (slot = 0; slot <= indxSec; slot ++) {
        dwAddr = dllBase + memLoc[slot].VAAddr;
        dwEnd = memLoc[slot].FileSize + dwAddr - 1;
        while (dwAddr <= dwEnd) {
            nRead = pageSize;
            if (dwEnd - dwAddr + 1 < pageSize) {
                nRead = (ULONG)(dwEnd - dwAddr + 1);
            }
            if (g_ExtData->ReadVirtual(dwAddr,buffer,nRead,&nRead) == S_OK) {
                WriteFile(hFile,buffer,nRead,&nWrite,NULL);
            }
            else {
                ExtOut ("Fail to read memory\n");
                goto end;
            }
            dwAddr += pageSize;
        }
    }
end:
    CloseHandle (hFile);
    return Status;
}

#ifdef _DEBUG
DECLARE_API(dbgout)
{
    INIT_API();

    BOOL bOff = FALSE;

    CMDOption option[] = {
        // name, vptr, type, hasValue
        {"-off", &bOff, COBOOL, FALSE},
    };

    if (!GetCMDOption(args, option, sizeof(option)/sizeof(CMDOption), NULL, 0, NULL))
    {
        return Status;
    }    
    g_bDbgOutput = !bOff;
    return Status;
}
#endif

// Rotor doesn't have a debugger that can call these extension functions.

void PrintHelp (__in __in_z LPSTR pszCmdName)
{
    static LPSTR pText = NULL;

    if (pText == NULL) {
#define SOS_DOCUMENT_FILENAME "sosdocs.txt"

        char  lpFilename[MAX_PATH+12]; // + 12 to make enough room for strcat function.
        DWORD nReturnedSize;
        nReturnedSize = GetModuleFileName(g_hInstance, lpFilename, MAX_PATH);
        if ( nReturnedSize == 0 || nReturnedSize == MAX_PATH ) {
            ExtOut ("Error getting the name for the current module\n");
            return;
        }

        // Find the last "\" or "/" in the path.
        char * pChar = lpFilename + strlen(lpFilename) - 1;
        while ( pChar != lpFilename-1 && * pChar != '\\' && * pChar != '/' ) { * pChar-- = 0; }
        strcat(lpFilename, SOS_DOCUMENT_FILENAME);
        
        HANDLE hSosDocFile = CreateFileA(lpFilename,
                                 GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, 0, NULL);
        if (hSosDocFile == INVALID_HANDLE_VALUE) {
            ExtOut ("Error finding documentation file\n");
            return;
        }

        HANDLE hMappedSosDocFile = CreateFileMappingA(hSosDocFile,
                                       NULL, PAGE_READONLY, 0, 0, NULL);
        CloseHandle(hSosDocFile);
        if (hMappedSosDocFile == NULL) { 
            ExtOut ("Error mapping documentation file\n");
            return;
        }

        pText = (LPSTR)MapViewOfFile(hMappedSosDocFile,
                           FILE_MAP_READ, 0, 0, 0);
        CloseHandle(hMappedSosDocFile);
        if (pText == NULL)
        {
            ExtOut ("Error loading documentation file\n");
            return;
        }
    }

    // Find our line in the text file
    char searchString[MAX_PATH];
    sprintf_s (searchString,_countof (searchString), "COMMAND: %s.", pszCmdName);
    LPSTR docs = strstr (pText, searchString);
    if (docs == NULL)
    {
        ExtOut ("Documentation for %s not found\n", pszCmdName);
        return;
    }

    // go to the end of this line:
    while (*docs != '\n' && *docs != '\0')
    {
        docs++;
    }

    if (*docs != '\n')
    {
        ExtOut ("Expected newline in documentation resource\n");
        return;
    }

    docs++;

    // \\ on a line with nothing else terminates command documenation
    while (*docs != '\0')
    {
        if (*docs == '\\' && *(docs+1) == '\\' && (*(docs+2) == '\n' || *(docs+2) == '\r'))
        {
            break;
        }
        ExtOut ("%c", *docs);
        docs++;
    }  
}

/**********************************************************************\
* Routine Description:                                                 *
*                                                                      *
*    This function displays the commands available in strike and the   *  
*    arguments passed into each.
*                                                                      *
\**********************************************************************/
extern "C" HRESULT CALLBACK
Help(PDEBUG_CLIENT Client, PCSTR Args)
{   
    // Call extension initialization functions directly, because we don't need the DAC dll to be initialized to get help.
    HRESULT Status;
    __ExtensionCleanUp __extensionCleanUp;
    if ((Status = ExtQuery(Client)) != S_OK) return Status;
    ControlC = FALSE;

    StringHolder commandName;
    CMDValue arg[] = {
        {&commandName.data, COSTRING}
    };
    size_t nArg;
    
    if (!GetCMDOption(Args,NULL,0,arg,sizeof(arg)/sizeof(CMDValue),&nArg))
    {
        return Status;
    }

    ExtOut ("-------------------------------------------------------------------------------\n");

    if (nArg == 1)
    {        
        // Convert commandName to lower-case
        LPSTR curChar = commandName.data;
        while (*curChar != '\0')
        {
            if ( ((unsigned) *curChar <= 0x7F) && isupper(*curChar))
            {
                *curChar = (CHAR) tolower(*curChar);
            }
            curChar++;
        }

        // Strip off leading "!" if the user put that.
        curChar = commandName.data;
        if (*curChar == '!')
            curChar++;
        
        PrintHelp (curChar);
    }
    else
    {
        PrintHelp ("contents");
    }
    
    
    return S_OK;
}

