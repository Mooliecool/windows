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
#include "strike.h"
#include "util.h"
#include "disasm.h"
#include <dbghelp.h>

#include "corhdr.h"
#include "cor.h"
#include "dacprivate.h"
#include "corerror.h"
#include "safemath.h"

#ifndef STRESS_LOG
#define STRESS_LOG
#endif // STRESS_LOG
#define STRESS_LOG_READONLY
#include "StressLog.h"

#define MAX_SYMBOL_LEN 4096
#define SYM_BUFFER_SIZE (sizeof(IMAGEHLP_SYMBOL) + MAX_SYMBOL_LEN)
char symBuffer[SYM_BUFFER_SIZE];
PIMAGEHLP_SYMBOL sym = (PIMAGEHLP_SYMBOL) symBuffer;

char *CorElementTypeName[ELEMENT_TYPE_MAX]=
{
#define TYPEINFO(e,ns,c,s,g,ia,ip,if,im)    c,
#include "cortypeinfo.h"
#undef TYPEINFO
};

char *CorElementTypeNamespace[ELEMENT_TYPE_MAX]=
{
#define TYPEINFO(e,ns,c,s,g,ia,ip,if,im)    ns,
#include "cortypeinfo.h"
#undef TYPEINFO
};

IXCLRDataProcess* g_clrData;

/**********************************************************************\
* Routine Description:                                                 *
*                                                                      *
*    This function is called to get the memory address given a symbol  *  
*    name.  It handles difference in symbol name between ntsd and      *
*    windbg.                                                           *
*                                                                      *
\**********************************************************************/
DWORD_PTR GetValueFromExpression (__in __in_z char *instr)
{
    ULONG64 dwAddr;
    char *str = instr;
    char name[256];

    dwAddr = 0;
    HRESULT hr = g_ExtSymbols->GetOffsetByName (str, &dwAddr);
    if (SUCCEEDED(hr))
        return (DWORD_PTR)dwAddr;
    else if (hr == S_FALSE && dwAddr)
        return (DWORD_PTR)dwAddr;

    strcpy_s (name, _countof(name), str);
    char *ptr;
    if ((ptr = strstr (name, "__")) != NULL)
    {
        ptr[0] = ':';
        ptr[1] = ':';
        ptr += 2;
        while ((ptr = strstr(ptr, "__")) != NULL)
        {
            ptr[0] = ':';
            ptr[1] = ':';
            ptr += 2;
        }
        dwAddr = 0;
        hr = g_ExtSymbols->GetOffsetByName (name, &dwAddr);
        if (SUCCEEDED(hr))
            return (DWORD_PTR)dwAddr;
        else if (hr == S_FALSE && dwAddr)
            return (DWORD_PTR)dwAddr;
    }
    else if ((ptr = strstr (name, "::")) != NULL)
    {
        ptr[0] = '_';
        ptr[1] = '_';
        ptr += 2;
        while ((ptr = strstr(ptr, "::")) != NULL)
        {
            ptr[0] = '_';
            ptr[1] = '_';
            ptr += 2;
        }
        dwAddr = 0;
        hr = g_ExtSymbols->GetOffsetByName (name, &dwAddr);
        if (SUCCEEDED(hr))
            return (DWORD_PTR)dwAddr;
        else if (hr == S_FALSE && dwAddr)
            return (DWORD_PTR)dwAddr;
    }
    return 0;
}

ModuleInfo moduleInfo[MSCOREND] = {{0,FALSE,0},{0,FALSE,0},{0,FALSE,0}};

void ReportOOM()
{
    ExtOut ("SOS Error: Out of memory\n");
}

HRESULT CheckEEDll ()
{
    DEBUG_MODULE_PARAMETERS Params;
            
    // Do we have mscorwks.dll
    if (moduleInfo[MSCORWKS].baseAddr == 0)
    {
        if (moduleInfo[MSCORWKS].baseAddr == 0)
            g_ExtSymbols->GetModuleByModuleName ("mscorwks",0,NULL,
                                                 &moduleInfo[MSCORWKS].baseAddr);
        if (moduleInfo[MSCORWKS].baseAddr != 0 && moduleInfo[MSCORWKS].hasPdb == FALSE)
        {
            g_ExtSymbols->GetModuleParameters (1, &moduleInfo[MSCORWKS].baseAddr, 0, &Params);
            if (Params.SymbolType == SymDeferred)
            {
                g_ExtSymbols->Reload("/f mscorwks.dll");
                g_ExtSymbols->GetModuleParameters (1, &moduleInfo[MSCORWKS].baseAddr, 0, &Params);
            }

            if (Params.SymbolType == SymPdb || Params.SymbolType == SymDia)
            {
                moduleInfo[MSCORWKS].hasPdb = TRUE;
            }

            moduleInfo[MSCORWKS].size = Params.Size;
        }
        if (moduleInfo[MSCORWKS].baseAddr != 0 && moduleInfo[MSCORWKS].hasPdb == FALSE)
            dprintf ("PDB symbol for mscorwks.dll not loaded\n");
    }
    
    return (moduleInfo[MSCORWKS].baseAddr != 0) ? S_OK : E_FAIL;
}

EEFLAVOR GetEEFlavor ()
{
    EEFLAVOR flavor = UNKNOWNEE;    
    
    if (SUCCEEDED(g_ExtSymbols->GetModuleByModuleName("mscorwks",0,NULL,NULL))) {
        flavor = MSCORWKS;
    }
    return flavor;
}

BOOL IsDumpFile ()
{
    static int g_fDumpFile = -1;
    if (g_fDumpFile == -1) {
        ULONG Class;
        ULONG Qualifier;
        g_ExtControl->GetDebuggeeType(&Class,&Qualifier);
        if (Qualifier)
            g_fDumpFile = 1;
        else
            g_fDumpFile = 0;
    }
    return g_fDumpFile != 0;
}

BOOL IsMiniDumpFileNODAC ()
{
    ULONG Class;
    ULONG Qualifier;
    g_ExtControl->GetDebuggeeType(&Class,&Qualifier);
    if (Qualifier == DEBUG_DUMP_SMALL) 
    {
        g_ExtControl->GetDumpFormatFlags(&Qualifier);
        if ((Qualifier & DEBUG_FORMAT_USER_SMALL_FULL_MEMORY) == 0)
        {
            return TRUE;
        }
    }        
    return FALSE;
}

BOOL g_InMinidumpSafeMode = FALSE;

// We use this predicate to mean the smallest, most restrictive kind of
// minidump file. There is no heap dump, only that set of information
// gathered to make !clrstack, !threads, !help, !eeversion and !pe work.
BOOL IsMiniDumpFile ()
{
    // It is okay for this to be static, because although the debugger may debug multiple
    // managed processes at once, I don't believe multiple dumpfiles of different
    // types is a scenario to worry about.
    if (IsMiniDumpFileNODAC())
    {
        // Beyond recognizing the dump type above, all we can rely on for this
        // is a flag set by the user indicating they want a safe mode minidump
        // experience. This is primarily for testing.
        return g_InMinidumpSafeMode;
    }        
    return FALSE;
}

ULONG TargetPlatform()
{
    static ULONG platform = -1;
    if (platform == -1) {
        ULONG major;
        ULONG minor;
        ULONG SPNum;
        g_ExtControl->GetSystemVersion(&platform,&major,&minor,NULL,0,NULL,&SPNum,NULL,0,NULL);
    }
    return platform;
}

ULONG DebuggeeType()
{
    static ULONG Class = DEBUG_CLASS_UNINITIALIZED;
    if (Class == DEBUG_CLASS_UNINITIALIZED) {
        ULONG Qualifier;
        g_ExtControl->GetDebuggeeType(&Class,&Qualifier);
    }
    return Class;
}

// Check if a file exist
BOOL FileExist (const char *filename)
{
    WIN32_FIND_DATA FindFileData;
    HANDLE handle = FindFirstFile (filename, &FindFileData);
    if (handle != INVALID_HANDLE_VALUE) {
        FindClose (handle);
        return TRUE;
    }
    else
        return FALSE;
}


BOOL FileExist (const WCHAR *filename)
{
    if (TargetPlatform() == VER_PLATFORM_WIN32_WINDOWS) {
        char filenameA[MAX_PATH+1];
        WideCharToMultiByte (CP_ACP,0,filename,-1,filenameA,MAX_PATH,0,NULL);
        filenameA[MAX_PATH] = '\0';
        return FileExist (filenameA);
    }
    WIN32_FIND_DATAW FindFileData;
    HANDLE handle = FindFirstFileW (filename, &FindFileData);
    if (handle != INVALID_HANDLE_VALUE) {
        FindClose (handle);
        return TRUE;
    }
    else
        return FALSE;
}

/**********************************************************************\
* Routine Description:                                                 *
*                                                                      *
*    This function is called to find out if a dll is bbt-ized          *  
*                                                                      *
\**********************************************************************/
BOOL IsRetailBuild (size_t base)
{
    IMAGE_DOS_HEADER DosHeader;
    if (g_ExtData->ReadVirtual(base, &DosHeader, sizeof(DosHeader), NULL) != S_OK)
        return FALSE;
    IMAGE_NT_HEADERS32 Header32;
    if (g_ExtData->ReadVirtual(base + DosHeader.e_lfanew, &Header32, sizeof(Header32), NULL) != S_OK)
        return FALSE;
    // If there is no COMHeader, this can not be managed code.
    if (Header32.OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_DEBUG].VirtualAddress == 0)
        return FALSE;

    size_t debugDirAddr = base + Header32.OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_DEBUG].VirtualAddress;
    size_t nSize = Header32.OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_DEBUG].Size;
    IMAGE_DEBUG_DIRECTORY debugDir;
    size_t nbytes = 0;
    while (nbytes < nSize) {
        if (g_ExtData->ReadVirtual(debugDirAddr+nbytes, &debugDir, sizeof(debugDir), NULL) != S_OK)
            return FALSE;
        if (debugDir.Type == 0xA) {
            return TRUE;
        }
        nbytes += sizeof(debugDir);
    }
    return FALSE;
}

/**********************************************************************\
* Routine Description:                                                 *
*                                                                      *
*    This function is called to read memory from the debugee's         *  
*    address space.  If the initial read fails, it attempts to read    *
*    only up to the edge of the page containing "offset".              *
*                                                                      *
\**********************************************************************/
BOOL SafeReadMemory (ULONG_PTR offset, PVOID lpBuffer, ULONG_PTR cb,
                     PULONG lpcbBytesRead)
{
    BOOL bRet = FALSE;

    bRet = SUCCEEDED(g_ExtData->ReadVirtual(offset, lpBuffer, (ULONG) cb,
                                            lpcbBytesRead));
    
    if (!bRet)
    {
        cb   = NextOSPageAddress(offset) - offset;
        bRet = SUCCEEDED(g_ExtData->ReadVirtual(offset, lpBuffer, (ULONG) cb,
                                                lpcbBytesRead));
    }
    return bRet;
}

ULONG OSPageSize ()
{
    static ULONG pageSize = 0;
    if (pageSize == 0)
        g_ExtControl->GetPageSize(&pageSize);

    return pageSize;
}

size_t NextOSPageAddress (size_t addr)
{
    size_t pageSize = OSPageSize();
    return (addr+pageSize)&(~(pageSize-1));
}

/**********************************************************************\
* Routine Description:                                                 *
*                                                                      *
*    This function is called to get the address of MethodDesc          *  
*    given an ip address                                               *
*                                                                      *
\**********************************************************************/
// jitType: 1 for normal JIT generated code, 2 for EJIT, 0 for unknown
void IP2MethodDesc (DWORD_PTR IP, DWORD_PTR &methodDesc, JitType &jitType,
                    DWORD_PTR &gcinfoAddr)
{

    CLRDATA_ADDRESS EIP = (CLRDATA_ADDRESS) IP;
    DacpCodeHeaderData codeHeaderData;
    
    methodDesc = NULL;
    gcinfoAddr = NULL;
    
    if (codeHeaderData.Request(g_clrData,(CLRDATA_ADDRESS)EIP) != S_OK)
    {        
        return;
    }

    methodDesc = (DWORD_PTR) codeHeaderData.MethodDescPtr;
    jitType = (JitType) codeHeaderData.JITType;
    gcinfoAddr = (DWORD_PTR) codeHeaderData.GCInfo;    
}

/**********************************************************************\
* Routine Description:                                                 *
*                                                                      *
*    This function is called to print a string beginning at strAddr.   *  
*    If buffer is non-NULL, print to buffer; Otherwise to screen.
*    If bWCHAR is true, treat the memory contents as WCHAR.            *
*    If length is not -1, it specifies the number of CHAR/WCHAR to be  *
*    read; Otherwise the string length is determined by NULL char.     *
*                                                                      *
\**********************************************************************/
// if buffer is not NULL, always convert to WCHAR
DWORD PrintString (DWORD_PTR strAddr, BOOL bWCHAR, DWORD_PTR length, __out_ecount (capacity_buffer) __out_opt WCHAR *buffer, size_t capacity_buffer, BOOL fPrint)
{
    if (buffer)
        buffer[0] = L'\0';
    DWORD len = 0;
    char name[256];
    DWORD totallen = 0;
    int gap;
    if (bWCHAR)
    {
        gap = 2;
        if (length != -1)
            length *= 2;
    }
    else
    {
        gap = 1;
    }
    while (1)
    {
        if (IsInterrupt())
            return 0;
        ULONG readLen = 256;
        if (IsInterrupt())
            return 0;
        if (!SafeReadMemory ((ULONG_PTR)strAddr + totallen, name, readLen,
                             &readLen))
            return 0;
            
        // move might return
        // move (name, (BYTE*)strAddr + totallen);
        if (length == -1)
        {
            for (len = 0; len <= 256u-gap; len += gap)
                if (name[len] == '\0' && (!bWCHAR || name[len+1] == '\0'))
                    break;
        }
        else
            len = 256;
        if (len == 256)
        {
            len -= gap;
            for (int n = 0; n < gap; n ++)
                name[255-n] = '\0';
        }

        if (fPrint)
        {
            if (bWCHAR)
            {
                if (buffer)
                {
                    wcscat_s (buffer, capacity_buffer, (WCHAR*)name);
                }
                else
                    dprintf ("%S", name);
            }
            else
            {
                if (buffer)
                {
                    WCHAR temp[256];
                    int n;
                    for (n = 0; name[n] != '\0'; n ++)
                        temp[n] = name[n];
                    temp[n] = L'\0';
                    wcscat_s (buffer, capacity_buffer, temp);
                }
                else
                    dprintf ("%s", name);
            }
        }
        
        totallen += len;
        if (length != -1)
        {
            if (totallen >= length)
            {
                break;
            }
        }
        else if (len < 255 || totallen > 1024)
        {
            break;
        }
    }

    return totallen / gap;
} 

/**********************************************************************\
* Routine Description:                                                 *
*                                                                      *
*    This function is called to find the address of Methodtable for    *  
*    a given MethodDesc.                                               *
*                                                                      *
\**********************************************************************/
void GetMethodTable(DWORD_PTR MDAddr, DWORD_PTR &methodTable)
{
    methodTable = 0;
    CLRDATA_ADDRESS StartAddr = (CLRDATA_ADDRESS) MDAddr;
    DacpMethodDescData MethodDescData;
    if (MethodDescData.Request(g_clrData,(CLRDATA_ADDRESS)StartAddr) != S_OK)
    {
        return;
    }
    methodTable = (DWORD_PTR) MethodDescData.MethodTablePtr;
}

BOOL IsValueField (DacpFieldDescData *pFD)
{
    return (pFD->Type == ELEMENT_TYPE_VALUETYPE);
}

void DisplayDataMember (DacpFieldDescData* pFD, DWORD_PTR dwAddr, BOOL fAlign=TRUE)
{
    if (dwAddr > 0)
    {
        // we must have called this function for a "real" (non-zero size) data type
        PREFIX_ASSUME(gElementTypeInfo[pFD->Type] != 0);

        DWORD_PTR dwTmp = dwAddr;
        bool bVTStatic = (pFD->bIsStatic && pFD->Type == ELEMENT_TYPE_VALUETYPE);
        
        if (gElementTypeInfo[pFD->Type] != -1 || bVTStatic)
        {
            union Value
            {
                char ch;
                short Short;
                DWORD_PTR ptr;
                int Int;
                unsigned int UInt;
                __int64 Int64;
                unsigned __int64 UInt64;
                float Float;
                double Double;
            } value;

            ZeroMemory(&value, sizeof(value));
            if (bVTStatic)
            {
                // static VTypes are boxed
                moveBlock (value, dwTmp, gElementTypeInfo[ELEMENT_TYPE_CLASS]);
            }
            else
            {
                moveBlock (value, dwTmp, gElementTypeInfo[pFD->Type]);
            }

            switch (pFD->Type) 
            {
                case ELEMENT_TYPE_I1:
                    if (fAlign)
                        dprintf ("%" POINTERSIZE "d", value.ch);
                    else
                        dprintf ("%d", value.ch);
                    break;
                case ELEMENT_TYPE_I2:
                    if (fAlign)
                        dprintf ("%" POINTERSIZE "d", value.Short);
                    else
                        dprintf ("%d", value.Short);
                    break;
                case ELEMENT_TYPE_I4:
                case ELEMENT_TYPE_I:
                    if (fAlign)
                        dprintf ("%" POINTERSIZE "d", value.Int);
                    else
                        dprintf ("%d", value.Int);
                    break;
                case ELEMENT_TYPE_I8:
                    dprintf ("%I64d", value.Int64);
                    break;
                case ELEMENT_TYPE_U1:
                case ELEMENT_TYPE_BOOLEAN:
                    if (fAlign)
                        dprintf ("%" POINTERSIZE "u", value.ch);
                    else
                        dprintf ("%u", value.ch);
                    break;
                case ELEMENT_TYPE_U2:
                    if (fAlign)
                        dprintf ("%" POINTERSIZE "u", value.Short);
                    else
                        dprintf ("%u", value.Short);
                    break;
                case ELEMENT_TYPE_U4:
                case ELEMENT_TYPE_U:
                    if (fAlign)
                        dprintf ("%" POINTERSIZE "u", value.UInt);
                    else
                        dprintf ("%u", value.UInt);
                    break;
                case ELEMENT_TYPE_U8:
                    dprintf ("%I64u", value.UInt64);
                    break;
                case ELEMENT_TYPE_R4:
                    dprintf ("%f", value.Float);
                    break;
                // case ELEMENT_TYPE_R:
                case ELEMENT_TYPE_R8:
                    dprintf ("%f", value.Double);
                    break;
                case ELEMENT_TYPE_CHAR:
                    if (fAlign)
                        dprintf ("%" POINTERSIZE "x", value.Short);
                    else
                        dprintf ("%x", value.Short);
                    break;
                default:
                    dprintf ("%p", (ULONG64)value.ptr);
                    break;
            }
        }
        else
        {
            dprintf ("%p", (ULONG64)dwTmp);
        }
    }
    else
    {
        dprintf ("%" POINTERSIZE "s", " ");
    }
}

void GetStaticFieldPTR(DWORD_PTR* pOutPtr, DacpDomainLocalModuleData* pDLMD, DacpMethodTableData* pMTD, DacpFieldDescData* pFDD, DacpEEClassData *pEECls, BYTE* pFlags = 0)
{
    DWORD_PTR dwTmp;

    if (pFDD->Type == ELEMENT_TYPE_VALUETYPE
            || pFDD->Type == ELEMENT_TYPE_CLASS)
    {
        dwTmp = (DWORD_PTR) pDLMD->pGCStaticDataStart + pFDD->dwOffset;
    }
    else
    {
        dwTmp = (DWORD_PTR) pDLMD->pNonGCStaticDataStart + pFDD->dwOffset;
    }
        

    *pOutPtr = 0;
    
    if (pMTD->bIsDynamic)
    {
        dprintf ("dynamic statics NYI\n");
        return;
    }
    else
    {
        if (pFlags && pMTD->bIsShared)
        {
            BYTE flags;
            DWORD_PTR pTargetFlags = (DWORD_PTR) pDLMD->pClassData + RidFromToken(pEECls->cl) - 1;            
            move (flags, pTargetFlags);

            *pFlags = flags;
        }
               
        
        *pOutPtr = dwTmp;            
    }
    return;
}

void DisplaySharedStatic (SIZE_T dwModuleDomainID, DacpMethodTableData* pMT, DacpFieldDescData *pFD, DacpEEClassData *pEECls)
{
    DacpAppDomainStoreData adsData;
    if (adsData.Request(g_clrData)!=S_OK)
    {
        ExtOut("Unable to get AppDomain information\n");        
    }

    CLRDATA_ADDRESS *pArray = new CLRDATA_ADDRESS[adsData.DomainCount];
    if (pArray==NULL)
    {
        ReportOOM();        
        return;
    }

    if (DacpAppDomainStoreData::GetDomains(g_clrData,adsData.DomainCount,pArray)!=S_OK)
    {
        ExtOut("Unable to get array of AppDomains\n");
        delete [] pArray;
        return;
    }

    dprintf ("    >> Domain:Value ");
    // Skip the SystemDomain and SharedDomain
    for (int i = 0; i < adsData.DomainCount ; i ++)
    {
        DacpAppDomainData appdomainData;
        if (appdomainData.Request(g_clrData,pArray[i])!=S_OK)
        {
            ExtOut("Unable to get AppDomain %lx\n",pArray[i]);
            delete [] pArray;
            return;
        }


        DacpDomainLocalModuleData vDomainLocalModule;                
        vDomainLocalModule.appDomainAddr = appdomainData.AppDomainPtr;
        vDomainLocalModule.ModuleID = dwModuleDomainID;
        if (vDomainLocalModule.Request(g_clrData)!=S_OK)
        {
            dprintf(" %p:NotInit ", (ULONG64)pArray[i]);
            continue;
        }

        DWORD_PTR dwTmp;
        BYTE Flags = 0;
        GetStaticFieldPTR(&dwTmp, &vDomainLocalModule , pMT, pFD, pEECls, &Flags);

        if ((Flags&1) == 0) {
            // We have not initialized this yet.
            dprintf (" %p:NotInit ", (ULONG64)pArray[i]);
            continue;
        }
        else if (Flags & 2) {
            // We have not initialized this yet.
            dprintf (" %p:FailInit", (ULONG64)pArray[i]);
            continue;
        }

        dprintf (" %p:", appdomainData.AppDomainPtr);
        DisplayDataMember(pFD, dwTmp, FALSE);               
    }    
    dprintf (" <<\n");
}

void DisplayContextStatic (DacpFieldDescData *pFD, size_t offset, BOOL fIsShared)
{
    dprintf ("\nDisplay of context static variables is not implemented yet\n");
    /*
    int numDomain;
    DWORD_PTR *domainList = NULL;
    GetDomainList (domainList, numDomain);
    ToDestroy des0 ((void**)&domainList);
    AppDomain vAppDomain;
    Context vContext;
    
    dprintf ("    >> Domain:Value");
    for (int i = 0; i < numDomain; i ++)
    {
        DWORD_PTR dwAddr = domainList[i];
        if (dwAddr == 0) {
            continue;
        }
        vAppDomain.Fill (dwAddr);
        if (vAppDomain.m_pDefaultContext == 0)
            continue;
        dwAddr = (DWORD_PTR)vAppDomain.m_pDefaultContext;
        vContext.Fill (dwAddr);
        
        if (fIsShared)
            dwAddr = (DWORD_PTR)vContext.m_pSharedStaticData;
        else
            dwAddr = (DWORD_PTR)vContext.m_pUnsharedStaticData;
        if (dwAddr == 0)
            continue;
        dwAddr += offsetof(STATIC_DATA, dataPtr);
        dwAddr += offset;
        if (safemove (dwAddr, dwAddr) == 0)
            continue;
        if (dwAddr == 0)
            // We have not initialized this yet.
            continue;
        
        dwAddr += pFD->dwOffset;
        if (pFD->Type == ELEMENT_TYPE_CLASS
            || pFD->Type == ELEMENT_TYPE_VALUETYPE)
        {
            if (safemove (dwAddr, dwAddr) == 0)
                continue;
        }
        if (dwAddr == 0)
            // We have not initialized this yet.
            continue;
        dprintf (" %p:", (ULONG64)domainList[i]);
        DisplayDataMember (pFD, dwAddr, FALSE);
    }
    dprintf (" <<\n");
    */
}

void DisplayThreadStatic (DacpFieldDescData *pFD, size_t offset, BOOL fIsShared)
{
    DacpThreadStoreData ThreadStore;
    ThreadStore.Request(g_clrData);

    dprintf ("    >> Thread:Value");
    CLRDATA_ADDRESS CurThread = ThreadStore.firstThread;
    while (CurThread)
    {
        DacpThreadData Thread;
        if (Thread.Request(g_clrData, CurThread) != S_OK)
        {
            dprintf ("  error getting thread %p, aborting this field\n", (ULONG64) CurThread);
            return;
        }
        
        DWORD_PTR dwAddr;
        if (Thread.osThreadId != 0)
        {        
            if (fIsShared)
                dwAddr = (DWORD_PTR)Thread.pSharedStaticData;
            else
                dwAddr = (DWORD_PTR)Thread.pUnsharedStaticData;

            if (dwAddr != 0)
            {
#ifdef _X86_
                dwAddr += 4; // get the dataPtr address offsetof(STATIC_DATA, dataPtr);
#else
                dwAddr += 8; // get the dataPtr address offsetof(STATIC_DATA, dataPtr);
#endif
                dwAddr += offset*sizeof(LPVOID);

                if (safemove (dwAddr, dwAddr) != 0)
                {
                    if (dwAddr != 0) // have we initialized the field yet?
                    {                                                                  
                        dwAddr += pFD->dwOffset;
                        if (pFD->Type == ELEMENT_TYPE_CLASS)
                        {
                            if (safemove (dwAddr, dwAddr) != 0)
                            {
                                DWORD Slot = (DWORD) dwAddr;
                                CLRDATA_ADDRESS OutAddress;
                                if(DacpFieldDescData::CalculateAddrForManagedStatic(
                                    g_clrData,CurThread,Slot,
                                    &OutAddress) != S_OK)
                                    dwAddr = 0;
                                else
                                    dwAddr = (DWORD_PTR) OutAddress;
                            }
                            else
                                dwAddr = 0;
                        }
                        else if(pFD->Type == ELEMENT_TYPE_VALUETYPE)
                        {
                            if (safemove (dwAddr, dwAddr) == 0)
                                dwAddr = 0;                                
                        }
                        if (dwAddr != 0)
                        {                          
                            dprintf (" %x:", Thread.osThreadId);
                            DisplayDataMember (pFD, dwAddr, FALSE);
                        }
                    }
                }
            }
        }
        
        CurThread = Thread.nextThread;
    }
    dprintf (" <<\n");
}

char *ElementTypeName (unsigned type)
{
    switch (type) {
    case ELEMENT_TYPE_PTR:
        return "PTR";
        break;
    case ELEMENT_TYPE_BYREF:
        return "BYREF";
        break;
    case ELEMENT_TYPE_VALUETYPE:
        return "VALUETYPE";
        break;
    case ELEMENT_TYPE_CLASS:
        return "CLASS";
        break;
    case ELEMENT_TYPE_VAR:
        return "VAR";
        break;
    case ELEMENT_TYPE_ARRAY:
        return "ARRAY";
        break;
    case ELEMENT_TYPE_VALUEARRAY:
        return "VALUEARRAY";
        break;
    case ELEMENT_TYPE_R:
        return "Native Real";
        break;
    case ELEMENT_TYPE_FNPTR:
        return "FNPTR";
        break;
    case ELEMENT_TYPE_SZARRAY:
        return "SZARRAY";
        break;
    case ELEMENT_TYPE_GENERICARRAY:
        return "GENERICARRAY";
        break;
    default:
        if (CorElementTypeName[type] == NULL) {
            return "";
        }
        return CorElementTypeName[type];
        break;
    }
}

char *ElementTypeNamespace (unsigned type)
{
  if (CorElementTypeNamespace[type] == NULL) {
    return "";
  }
  return CorElementTypeNamespace[type];
}

DWORD GetMethodTableSize(CLRDATA_ADDRESS pMT)
{
    DacpMethodTableData mtabledata;
    if (mtabledata.Request(g_clrData,(CLRDATA_ADDRESS) pMT) == S_OK)
    {
        return mtabledata.sizeofMethodTable;
    }    
    return 0;
}


void ComposeName_s(CorElementType Type, __out_ecount(bufSize) LPSTR buffer, size_t capacity_buffer)
{
    char *p = ElementTypeNamespace(Type);
    if ((p) && (*p != '\0'))
    {
        strcpy_s(buffer,capacity_buffer,p);
        strcat_s(buffer,capacity_buffer,".");
        strcat_s(buffer,capacity_buffer,ElementTypeName(Type));
    }
    else
    {
        strcpy_s(buffer,capacity_buffer,ElementTypeName(Type));
    }
}

// NOTE: pszName is changed
// INPUT            MAXCHARS        RETURN
// HelloThere       5               ...re
// HelloThere       8               ...There
LPWSTR FormatTypeName (__out_ecount (maxChars) LPWSTR pszName, UINT maxChars)
{
    UINT iStart = 0;
    UINT iLen = (int) wcslen(pszName);
    if (iLen > maxChars)
    {
        iStart = iLen - maxChars;
        UINT numDots = (maxChars < 3) ? maxChars : 3;
        for (UINT i=0; i < numDots; i++)
            pszName[iStart+i] = '.';        
    }
    return pszName + iStart;
}

/**********************************************************************\
* Routine Description:                                                 *
*                                                                      *
*    This function is called to dump all fields of a managed object.   *  
*    pEECls specifies the type of object.                              *
*    dwStartAddr specifies the beginning memory address.               *
*    bFirst is used to avoid printing header everytime.                *
*                                                                      *
\**********************************************************************/
void DisplayFields (DacpEEClassData *pEECls, DWORD_PTR dwStartAddr, BOOL bFirst, BOOL bValueClass)
{
    static DWORD numInstanceFields = 0;
    if (bFirst)
    {
        ExtOutIndent ();
//        dprintf ("%" POINTERSIZE "s %8s %8s %20s %10s %" POINTERSIZE "s %s\n", 
//                 "MT", "Field", "Offset", "Type", "Attr", "Value", "Name");
        dprintf ("%" POINTERSIZE "s %8s %8s %20s %2s %8s %" POINTERSIZE "s %s\n", 
                 "MT", "Field", "Offset", "Type", "VT", "Attr", "Value", "Name");
        numInstanceFields = 0;
    }
    
    DacpMethodTableData vMethTable;
    if (vMethTable.Request(g_clrData,pEECls->MethodTable)!=S_OK)
        return;    
    BOOL fIsShared = vMethTable.bIsShared;

    if (vMethTable.ParentMethodTable)
    {
        DacpEEClassData vParentEECls;
        DacpMethodTableData vParentMethTable;
        if (vParentMethTable.Request(g_clrData,vMethTable.ParentMethodTable)!=S_OK)
        {
            ExtOut("Invalid parent MethodTable\n");
            return;
        }            

        if (vParentEECls.Request(g_clrData,vParentMethTable.Class)!=S_OK)
        {
            ExtOut("Invalid parent EEClass\n");
            return;
        }            

        DisplayFields (&vParentEECls, dwStartAddr, FALSE, bValueClass);
    }
    DWORD numStaticFields = 0;

    CLRDATA_ADDRESS dwAddr = pEECls->FirstField;
    DacpFieldDescData vFieldDesc;

    // Get the module name
    DacpModuleData module;
    if (module.Request(g_clrData,pEECls->Module)!=S_OK)
        return;    

    IMetaDataImport *pImport = MDImportForModule(&module);
    
    while (numInstanceFields < pEECls->wNumInstanceFields
           || numStaticFields < pEECls->wNumStaticFields)
    {
        if (IsInterrupt())
            return;

        ExtOutIndent ();
        
        if ((vFieldDesc.Request (g_clrData, (CLRDATA_ADDRESS) dwAddr)!=S_OK) ||
            (vFieldDesc.Type >= ELEMENT_TYPE_MAX))
        {
            dprintf ("Unable to display fields\n");
            return;
        }
        dwAddr = vFieldDesc.NextField;

        DWORD offset = vFieldDesc.dwOffset;
        if(!((vFieldDesc.bIsThreadLocal || vFieldDesc.bIsContextLocal || fIsShared) && vFieldDesc.bIsStatic))
        {
            if (!bValueClass)
            {
                offset += sizeof(BaseObject);
            }
        }

  
        dprintf ("%p %8x %8x ",
                vFieldDesc.MTOfType,
                 TokenFromRid(vFieldDesc.mb, mdtFieldDef),
                 offset);

        char ElementName[mdNameLen];
        if ((vFieldDesc.Type == ELEMENT_TYPE_VALUETYPE || 
            vFieldDesc.Type == ELEMENT_TYPE_CLASS) && vFieldDesc.MTOfType)
        {
            NameForMT_s ((DWORD_PTR)vFieldDesc.MTOfType, g_mdName, mdNameLen);            
            dprintf ("%20.20S ", FormatTypeName(g_mdName, 20));            
        }
        else 
        {       
            if (vFieldDesc.Type == ELEMENT_TYPE_CLASS && vFieldDesc.TokenOfType != mdTypeDefNil)
            {
			    // Get the name from Metadata!!!
                NameForToken_s (TokenFromRid(vFieldDesc.TokenOfType, mdtTypeDef), pImport, g_mdName, mdNameLen, false);
                dprintf ("%20.20S ", FormatTypeName(g_mdName, 20));
            }
			else
			{
			    // If ET type from signature is different from fielddesc, then the signature one is more descriptive. 
			    // For example, E_T_STRING in field desc will be E_T_CLASS. In minidump's case, we won't have
			    // the method table for it. 
			    //
                ComposeName_s(vFieldDesc.Type != vFieldDesc.sigType ? vFieldDesc.sigType : vFieldDesc.Type, ElementName, sizeof(ElementName)/sizeof(ElementName[0]));
                dprintf ("%20.20s ", ElementName); 
            }
        }
        
        dprintf ("%2s ", (vFieldDesc.Type == ELEMENT_TYPE_VALUETYPE) ? "1" : "0");

        if (vFieldDesc.bIsStatic && (vFieldDesc.bIsThreadLocal || vFieldDesc.bIsContextLocal))
        {
            numStaticFields ++;
            if (fIsShared)
                dprintf ("%8s %" POINTERSIZE "s", "shared", vFieldDesc.bIsThreadLocal ? "TLstatic" : "CLstatic");
            else
                dprintf ("%8s ", vFieldDesc.bIsThreadLocal ? "TLstatic" : "CLstatic");

            NameForToken_s (TokenFromRid(vFieldDesc.mb, mdtFieldDef), pImport, g_mdName, mdNameLen, false);
            dprintf (" %S\n", g_mdName);

            if (IsMiniDumpFile())
            {
                dprintf (" <no information>\n");
            }
            else
            {
                if (vFieldDesc.bIsThreadLocal)
                {
                    DisplayThreadStatic(&vFieldDesc,
                                        pEECls->wThreadStaticOffset,
                                        fIsShared);
                }
                else if (vFieldDesc.bIsContextLocal)
                {
                    DisplayContextStatic(&vFieldDesc,
                                         pEECls->wContextStaticOffset,
                                         fIsShared);
                }
            }
    
        }
        else if (vFieldDesc.bIsStatic)
        {
            numStaticFields ++;

            if (fIsShared)
            {                
                dprintf ("%8s %" POINTERSIZE "s", "shared", "static");

                NameForToken_s (TokenFromRid(vFieldDesc.mb, mdtFieldDef), pImport, g_mdName, mdNameLen, false);
                dprintf (" %S\n", g_mdName);

                if (IsMiniDumpFile())
                {
                    dprintf (" <no information>\n");
                }
                else
                {
                    DacpModuleData vModule;
                    if (vModule.Request(g_clrData,pEECls->Module) == S_OK)
                    {
                        DisplaySharedStatic(vModule.dwDomainNeutralIndex, &vMethTable, &vFieldDesc, pEECls);                
                    }
                }
            }
            else
            {
                dprintf ("%8s ", "static");
                
                DacpDomainLocalModuleData vDomainLocalModule;
                
                if (vDomainLocalModule.RequestFromModulePtr(g_clrData,pEECls->Module)!=S_OK)
                {
                    dprintf (" <no information>\n");
                }
                else
                {
                    DWORD_PTR dwTmp;
                    GetStaticFieldPTR(&dwTmp, &vDomainLocalModule , &vMethTable, &vFieldDesc, pEECls);
                    DisplayDataMember(&vFieldDesc, dwTmp);

                    NameForToken_s (TokenFromRid(vFieldDesc.mb, mdtFieldDef), pImport, g_mdName, mdNameLen, false);
                    dprintf (" %S\n", g_mdName);
                }
            }
        }
        else
        {
            numInstanceFields ++;

            dprintf ("%8s ", "instance");
            if (dwStartAddr > 0)
            {
                DWORD_PTR dwTmp = dwStartAddr + vFieldDesc.dwOffset
                    + (bValueClass ? 0 : sizeof(BaseObject));
                DisplayDataMember (&vFieldDesc, dwTmp);
            }
            else
            {
                dprintf (" %8s", " ");
            }
            
            NameForToken_s (TokenFromRid(vFieldDesc.mb, mdtFieldDef), pImport, g_mdName, mdNameLen, false);
            dprintf (" %S\n", g_mdName);
        }
        
    }
    
    return;
}

BOOL VerifyMemberFields (DacpEEClassData *pEECls, DWORD_PTR dwStartAddr, BOOL bFirst=TRUE)
{
#define EXITPOINT(EXPR) do { if(!(EXPR)) { return FALSE; } } while (0)

    static DWORD numInstanceFields = 0;
    if (bFirst)
    {
        numInstanceFields = 0;
    }
    
    DacpMethodTableData vMethTable;
    EXITPOINT (vMethTable.Request(g_clrData,pEECls->MethodTable) == S_OK);

    if (vMethTable.ParentMethodTable)
    {
        DacpEEClassData vParentEECls;
        DacpMethodTableData vParentMethTable;
        EXITPOINT (vParentMethTable.Request(g_clrData,vMethTable.ParentMethodTable) == S_OK);
        EXITPOINT (vParentEECls.Request(g_clrData,vParentMethTable.Class) == S_OK);
        EXITPOINT (VerifyMemberFields (&vParentEECls, dwStartAddr, FALSE));
    }

    CLRDATA_ADDRESS dwAddr = pEECls->FirstField;
    DacpFieldDescData vFieldDesc;
    
    while (numInstanceFields < pEECls->wNumInstanceFields)
    {
        EXITPOINT (IsInterrupt() == FALSE);
        EXITPOINT (vFieldDesc.Request (g_clrData, (CLRDATA_ADDRESS) dwAddr) == S_OK);
        EXITPOINT (vFieldDesc.Type < ELEMENT_TYPE_MAX);

        dwAddr = vFieldDesc.NextField;
            
        if (!vFieldDesc.bIsStatic)
        {
            numInstanceFields ++;            
            DWORD_PTR dwTmp = dwStartAddr + vFieldDesc.dwOffset + sizeof(BaseObject);
            if (vFieldDesc.Type == ELEMENT_TYPE_CLASS)
            {
                // Is it a valid object?  
                EXITPOINT (MOVE(dwTmp, dwTmp) == S_OK);
                if (dwTmp != NULL)
                {
                    DacpObjectData objData;
                    EXITPOINT (objData.Request(g_clrData,(CLRDATA_ADDRESS)dwTmp) == S_OK);
                }
            }
        }        
    }
    
    return TRUE;
    
#undef EXITPOINT    
}

// Return value: -1 = error, 
//                0 = field not found, 
//              > 0 = offset to field from objAddr
int GetObjFieldOffset (CLRDATA_ADDRESS objAddr, CLRDATA_ADDRESS methodTable, __in __in_z LPWSTR wszFieldName,
                        BOOL bFirst/*=TRUE*/)
{

#define EXITPOINT(EXPR) do { if(!(EXPR)) { return -1; } } while (0)
    
    DacpObjectData objData;
    DacpMethodTableData dmtd;
    DacpEEClassData EECls;
    DacpFieldDescData vFieldDesc;
    DacpModuleData module;
    static DWORD numInstanceFields = 0; // Static due to recursion visiting parents

    if (bFirst)
    {
        numInstanceFields = 0;
    }
    
    EXITPOINT (objData.Request(g_clrData,(CLRDATA_ADDRESS)objAddr) == S_OK);    
    EXITPOINT (dmtd.Request (g_clrData, methodTable) == S_OK);

    if (dmtd.ParentMethodTable)
    {
        DWORD retVal = GetObjFieldOffset (objAddr, dmtd.ParentMethodTable, 
                                          wszFieldName, FALSE);
        if (retVal != 0)
        {
            // return in case of error or success.
            // Fall through for field-not-found.
            return retVal;
        }
    }
    
    EXITPOINT (EECls.Request(g_clrData,dmtd.Class) == S_OK);
    EXITPOINT (module.Request(g_clrData,EECls.Module) == S_OK);
        
    CLRDATA_ADDRESS dwAddr = EECls.FirstField;            
    IMetaDataImport *pImport = MDImportForModule(&module);
        
    while (numInstanceFields < EECls.wNumInstanceFields)
    {        
        EXITPOINT (vFieldDesc.Request (g_clrData, (CLRDATA_ADDRESS) dwAddr) == S_OK);

        if (!vFieldDesc.bIsStatic)
        {
            DWORD offset = vFieldDesc.dwOffset + sizeof(BaseObject);          
            NameForToken_s (TokenFromRid(vFieldDesc.mb, mdtFieldDef), pImport, g_mdName, mdNameLen, false);
            if (wcscmp (wszFieldName, g_mdName) == 0)
            {
                return offset;
            }
            numInstanceFields ++;                        
        }

        dwAddr = vFieldDesc.NextField;        
    }

    // Field name not found...
    return 0;

#undef EXITPOINT    
}

// Returns an AppDomain address if AssemblyPtr is loaded into that domain only. Otherwise
// returns NULL
CLRDATA_ADDRESS IsInOneDomainOnly(CLRDATA_ADDRESS AssemblyPtr)
{
    CLRDATA_ADDRESS appDomain = NULL;

    DacpAppDomainStoreData adstore;
    if (adstore.Request(g_clrData) != S_OK)
    {
        ExtOut("Unable to get appdomain store\n");
        return NULL;
    }    

    size_t AllocSize;
    if (!ClrSafeInt<size_t>::multiply(sizeof(CLRDATA_ADDRESS), adstore.DomainCount, AllocSize))
    {
        ReportOOM();        
        return NULL;
    }

    CLRDATA_ADDRESS *pArray = (CLRDATA_ADDRESS *) malloc (AllocSize);
    ToDestroy des((void**)&pArray);            
    if (pArray==NULL)
    {
        ReportOOM();        
        return NULL;
    }
    
    if (DacpAppDomainStoreData::GetDomains(g_clrData,adstore.DomainCount,pArray)!=S_OK)
    {
        ExtOut ("Failed to get appdomain list\n");
        return NULL;
    }

    for (int i = 0; i < adstore.DomainCount; i++)
    {
        if (IsInterrupt())
            return NULL;

        DacpAppDomainData dadd;
        if (dadd.Request (g_clrData, pArray[i]) != S_OK)
        {
            ExtOut ("Unable to get AppDomain %p\n", (ULONG64) pArray[i]);
            return NULL;
        }

        if (dadd.AssemblyCount)
        {
            size_t AssemblyAllocSize;
            if (!ClrSafeInt<size_t>::multiply(sizeof(CLRDATA_ADDRESS), dadd.AssemblyCount, AssemblyAllocSize))
            {
                ReportOOM();                        
                return NULL;
            }

            CLRDATA_ADDRESS *pAsmArray = (CLRDATA_ADDRESS *) malloc (AssemblyAllocSize);
            ToDestroy des1((void**)&pAsmArray);            
            
            if (pAsmArray==NULL)
            {
                ReportOOM();                        
                return NULL;
            }
    
            if (DacpAppDomainData::GetAssemblies(g_clrData,dadd.AppDomainPtr,dadd.AssemblyCount,pAsmArray)!=S_OK)
            {
                ExtOut("Unable to get array of Assemblies\n");
                return NULL;  
            }
      
            for (LONG n = 0; n < dadd.AssemblyCount; n ++)
            {
                if (IsInterrupt())
                    return NULL;

                if (AssemblyPtr == pAsmArray[n])
                {
                    if (appDomain != NULL)
                    {
                        // We have found more than one AppDomain that loaded this
                        // assembly, we must return NULL.
                        return NULL;
                    }
                    appDomain = dadd.AppDomainPtr;
                }                
            }    
        }
    } 

    
    return appDomain;
}

CLRDATA_ADDRESS GetAppDomain(CLRDATA_ADDRESS objPtr)
{
    CLRDATA_ADDRESS appDomain = NULL;
    
    DacpObjectData objData;
    if (objData.Request(g_clrData,objPtr) != S_OK)
    {        
        return NULL;
    }

    // First check  eeclass->module->assembly->domain.
    // Then check the object flags word
    // finally, search threads for a reference to the object, and look at the thread context.

    DacpEEClassData eeclass;
    if (eeclass.Request(g_clrData,objData.EEClass) != S_OK)
    {
        return NULL;
    }

    DacpModuleData module;
    if (module.Request(g_clrData,eeclass.Module) != S_OK)
    {
        return NULL;
    }

    DacpAssemblyData assembly;
    if (assembly.Request(g_clrData,module.Assembly) != S_OK)
    {
        return NULL;
    }

    DacpAppDomainStoreData adstore;
    if (adstore.Request(g_clrData) != S_OK)
    {
        return NULL;
    }    
    
    if (assembly.ParentDomain == adstore.sharedDomain)
    {
        ObjHeaderSOS oh;
        if (MOVE (oh, objPtr - sizeof(ObjHeaderSOS)) != S_OK)
        {
            return NULL;
        }
        
        DWORD value = oh.m_SyncBlockValue;
        DWORD adIndex = (value >> SBLK_APPDOMAIN_SHIFT) & SBLK_MASK_APPDOMAININDEX;
        if ( ((value & BIT_SBLK_IS_HASH_OR_SYNCBLKINDEX) != 0) || adIndex==0)
        {
            // No AppDomainID information. We'll make use of a heuristic.
            // If the assembly is in the shared domain, we can report it as
            // being in domain X if the only other domain that has the assembly
            // loaded is domain X.
            appDomain = IsInOneDomainOnly(assembly.AssemblyPtr);
            if (appDomain == NULL && ((value & BIT_SBLK_IS_HASH_OR_SYNCBLKINDEX) != 0))
            {
                if ((value & BIT_SBLK_IS_HASHCODE) == 0)
                {
                    UINT index = value & MASK_SYNCBLOCKINDEX;
                    // We have a syncblock, the appdomain ID may be in there.
                    DacpSyncBlockData syncBlockData;
                    if (syncBlockData.Request(g_clrData,index) == S_OK)
                    {
                        appDomain = syncBlockData.appDomainPtr;
                    }
                }
            }
        }
        else if ((value & BIT_SBLK_IS_HASH_OR_SYNCBLKINDEX) == 0)
        {            
            size_t AllocSize;
            if (!ClrSafeInt<size_t>::multiply(sizeof(CLRDATA_ADDRESS), adstore.DomainCount, AllocSize))
            {
                return NULL;
            }
            // we know we have a non-zero adIndex. Find the appdomain.
            CLRDATA_ADDRESS *pArray = (CLRDATA_ADDRESS *) malloc (AllocSize);
            ToDestroy des((void**)&pArray);            
            if (pArray==NULL)
            {
                return NULL;
            }
            
            if (DacpAppDomainStoreData::GetDomains(g_clrData,adstore.DomainCount,pArray)!=S_OK)
            {
                return NULL;
            }

            for (int i = 0; i < adstore.DomainCount; i++)
            {
                DacpAppDomainData dadd;
                if (dadd.Request (g_clrData, pArray[i]) != S_OK)
                {
                    return NULL;
                }
                if (dadd.dwId == adIndex)
                {
                    appDomain = pArray[i];
                    break;
                }
            } 
        }
    }
    else
    {
        appDomain = assembly.ParentDomain;
    }

    return appDomain;
}


void FileNameForModule (DWORD_PTR pModuleAddr, __out_ecount (MAX_PATH) WCHAR *fileName)
{
    DacpModuleData ModuleData;
    fileName[0] = L'\0';
    if (ModuleData.Request(g_clrData,(CLRDATA_ADDRESS) pModuleAddr)==S_OK)
    {
        FileNameForModule(&ModuleData,fileName);
    }
}

/**********************************************************************\
* Routine Description:                                                 *
*                                                                      *
*    This function is called to find the file name given a Module.     *  
*                                                                      *
\**********************************************************************/
// fileName should be at least MAX_PATH
void FileNameForModule (DacpModuleData *pModule, __out_ecount (MAX_PATH) WCHAR *fileName)
{
    CLRDATA_ADDRESS dwAddr = pModule->File;
    if (dwAddr == 0) {
        fileName[0] = L'\0';
        return;
    }
    DacpPEFileData pefileData;
    if (pefileData.Request(g_clrData,dwAddr)==S_OK)
    {
        if (DacpPEFileData::GetPEFileName(g_clrData,dwAddr, MAX_PATH,fileName)==S_OK)
        {
            if (fileName[0]!=L'\0')
                return; // done
        }

        // Try the base *
        if (pefileData.Base)
        {
            DllsName((ULONG_PTR) pefileData.Base, fileName);
        }
    }
    // If we got here, either DllsName worked, or we couldn't find a name
}


void AssemblyInfo (DacpAssemblyData *pAssembly)
{
    dprintf ("ClassLoader: %p\n", (ULONG64)pAssembly->ClassLoader);
    if ((ULONG64)pAssembly->AssemblySecDesc != NULL)
        dprintf ("SecurityDescriptor: %p\n", (ULONG64)pAssembly->AssemblySecDesc);
    dprintf ("  Module Name\n");
    
    CLRDATA_ADDRESS *Modules = new CLRDATA_ADDRESS[pAssembly->ModuleCount];
    if (DacpAssemblyData::GetModules(g_clrData,pAssembly->AssemblyPtr,pAssembly->ModuleCount,Modules) != S_OK)
    {
       ReportOOM();        
       return;
    }
    
    
    for (UINT n=0;n<pAssembly->ModuleCount;n++)
    {
        if (IsInterrupt())
        {
            delete [] Modules;
            return;
        }

        CLRDATA_ADDRESS ModuleAddr = Modules[n];
        dprintf ("%p ", (ULONG64)ModuleAddr);
        DacpModuleData moduleData;
        if (moduleData.Request(g_clrData,ModuleAddr)==S_OK)
        {
            WCHAR fileName[MAX_PATH];
            FileNameForModule (&moduleData, fileName);
            if (fileName[0])
            {
                dprintf("%S\n",fileName);
            }
            else
            {
                dprintf("%S\n", (moduleData.bIsReflection) ? L"Dynamic Module" : L"Unknown Module");
            }
        }        
    }

    delete [] Modules;
}

char *GetStageText(DacpAppDomainDataStage stage)
{
    switch(stage)
    {
        case STAGE_CREATING:
            return "CREATING";
        case STAGE_READYFORMANAGEDCODE:
            return "READYFORMANAGEDCODE";
        case STAGE_ACTIVE:
            return "ACTIVE";
        case STAGE_OPEN:
            return "OPEN";
        case STAGE_UNLOAD_REQUESTED:
            return "UNLOAD_REQUESTED";
        case STAGE_EXITING:
            return "EXITING";
        case STAGE_EXITED:
            return "EXITED";
        case STAGE_FINALIZING:
            return "FINALIZING";
        case STAGE_FINALIZED:
            return "FINALIZED";
        case STAGE_HANDLETABLE_NOACCESS:
            return "HANDLETABLE_NOACCESS";
        case STAGE_CLEARED:
            return "CLEARED";
        case STAGE_COLLECTED:
            return "COLLECTED";
        case STAGE_CLOSED:
            return "CLOSED";
    }
    return "UNKNOWN";
}

/**********************************************************************\
* Routine Description:                                                 *
*                                                                      *
*    This function is called to dump the contents of a domain.         *  
*                                                                      *
\**********************************************************************/
void DomainInfo (DacpAppDomainData *pDomain)
{
    dprintf ("LowFrequencyHeap: %p\n", (ULONG64)pDomain->pLowFrequencyHeap);
    dprintf ("HighFrequencyHeap: %p\n", (ULONG64)pDomain->pHighFrequencyHeap);
    dprintf ("StubHeap: %p\n", (ULONG64)pDomain->pStubHeap);
    dprintf ("Stage: %s\n", GetStageText(pDomain->appDomainStage));
    if ((ULONG64)pDomain->AppSecDesc != NULL)
        dprintf ("SecurityDescriptor: %p\n", (ULONG64)pDomain->AppSecDesc);
    dprintf ("Name: ");

    if (DacpAppDomainData::GetName(g_clrData,pDomain->AppDomainPtr,mdNameLen,g_mdName)!=S_OK)
    {
        dprintf("Error getting AppDomain friendly name\n");
        return;
    }

    dprintf("%S\n",(g_mdName[0]!=L'\0') ? g_mdName : L"None");

    if (pDomain->AssemblyCount==0)
    {
        return;
    }
    
    CLRDATA_ADDRESS *pArray = new CLRDATA_ADDRESS[pDomain->AssemblyCount];
    if (pArray==NULL)
    {
        ReportOOM();
        return;
    }

    if (DacpAppDomainData::GetAssemblies(g_clrData,pDomain->AppDomainPtr,pDomain->AssemblyCount,pArray)!=S_OK)
    {
        ExtOut("Unable to get array of Assemblies\n");
        delete [] pArray;
        return;  
    }

    LONG n;
    // Assembly vAssembly;
    for (n = 0; n < pDomain->AssemblyCount; n ++)
    {
        if (IsInterrupt())
            return;
        
        if (n != 0) {
            dprintf ("\n");
        }
        dprintf ("Assembly: %p", (ULONG64)pArray[n]);
        DacpAssemblyData assemblyData;
        if (assemblyData.Request(g_clrData,pArray[n], pDomain->AppDomainPtr)==S_OK)
        {
            if (assemblyData.isDynamic) {
                dprintf (" (Dynamic)");
            }
            dprintf (" [");
            if (DacpAssemblyData::GetName(g_clrData,pArray[n],mdNameLen,g_mdName)==S_OK)
            {
                ExtOut("%S",g_mdName);
            }
            dprintf ("]\n");
            AssemblyInfo(&assemblyData);
        }
    }    

    dprintf("\n");
    delete [] pArray;
}

/**********************************************************************\
* Routine Description:                                                 *
*                                                                      *
*    This function is called to find the name of a MethodDesc using    *  
*    metadata API.                                                     *
*                                                                      *
\**********************************************************************/
BOOL NameForMD_s (DWORD_PTR pMD, __out_ecount (capacity_mdName) WCHAR *mdName, size_t capacity_mdName)
{
    mdName[0] = L'\0';
    CLRDATA_ADDRESS StartAddr = (CLRDATA_ADDRESS) pMD;
    DacpMethodDescData MethodDescData;

    // don't need to check for minidump file as all commands are seals
    // We also do not have EEJitManager to validate anyway.
    //
    if (!IsMiniDumpFile() && MethodDescData.Request(g_clrData,(CLRDATA_ADDRESS)StartAddr) != S_OK)
    {
        ExtOut("%p is not a MethodDesc\n", (ULONG64)StartAddr);
        return FALSE;
    }

    if (DacpMethodDescData::GetMethodName(g_clrData, StartAddr, mdNameLen, mdName) != S_OK)
    {
        wcscpy_s(mdName, capacity_mdName,L"UNKNOWN");
        return FALSE;
    }
    return TRUE;
}

// Buffer must hold mdNameLen
BOOL NameForObject_s (DWORD_PTR ObjAddr, __out_ecount (capacity_mdName) WCHAR *mdName, size_t capacity_mdName)
{
    // _ASSERTE is explicitly defined to expand to a NOP in strike.cpp and
    // other files. It is not defined at this point in the file
    // _ASSERTE (capacity_mdName >= mdNameLen); // in accordance with the comment for this function
    mdName[0] = L'\0';
    if (DacpObjectData::GetObjectClassName(g_clrData,(CLRDATA_ADDRESS) ObjAddr, (ULONG32) capacity_mdName, mdName)!=S_OK)
    {
        return FALSE;
    }
    return TRUE;
}

/**********************************************************************\
* Routine Description:                                                 *
*                                                                      *
*    This function is called to find the name of a MethodTable using   *  
*    metadata API.                                                     *
*                                                                      *
\**********************************************************************/
BOOL NameForMT_s (DWORD_PTR MTAddr, __out_ecount (capacity_mdName) WCHAR *mdName, size_t capacity_mdName)
{
    HRESULT hr = S_OK;
    if (IsMTForFreeObj(MTAddr))
    {
        wcscpy_s(mdName, capacity_mdName, L"Free");
    }
    else 
    {
        hr = DacpMethodTableData::GetMethodTableName(g_clrData,(CLRDATA_ADDRESS) MTAddr, 
            (ULONG32)capacity_mdName, mdName);
    }
    return (SUCCEEDED(hr));
}

/**********************************************************************\
* Routine Description:                                                 *
*                                                                      *
*    Return TRUE if str2 is a substring of str1 and str1 and str2      *  
*    share the same file path.
*                                                                      *
\**********************************************************************/
BOOL IsSameModuleName (const char *str1, const char *str2)
{
    if (strlen (str1) < strlen (str2))
        return FALSE;
    const char *ptr1 = str1 + strlen(str1)-1;
    const char *ptr2 = str2 + strlen(str2)-1;
    while (ptr2 >= str2)
    {
        if (tolower(*ptr1) != tolower(*ptr2))
            return FALSE;
        ptr2 --;
        ptr1 --;
    }
    if (ptr1 >= str1 && *ptr1 != '\\' && *ptr1 != ':')
        return FALSE;
    return TRUE;
}

/**********************************************************************\
* Routine Description:                                                 *
*                                                                      *
*    Return TRUE if moduleAddr is the address of a module.             *  
*                                                                      *
\**********************************************************************/
BOOL IsModule (DWORD_PTR moduleAddr)
{
    DacpModuleData module;
    return (module.Request(g_clrData,(CLRDATA_ADDRESS)moduleAddr)==S_OK);
}

/**********************************************************************\
* Routine Description:                                                 *
*                                                                      *
*    Return TRUE if value is the address of a MethodTable.             *  
*    We verify that MethodTable and EEClass are right.
*                                                                      *
\**********************************************************************/
BOOL IsMethodTable (DWORD_PTR value)
{
    DacpMethodTableData mtabledata;
    if (mtabledata.Request(g_clrData,(CLRDATA_ADDRESS) value)!=S_OK)
    {
        return FALSE;
    }
    
    return TRUE;
}

/**********************************************************************\
* Routine Description:                                                 *
*                                                                      *
*    Return TRUE if value is the address of a MethodDesc.              *  
*    We verify that MethodTable and EEClass are right.
*                                                                      *
\**********************************************************************/
BOOL IsMethodDesc (DWORD_PTR value)
{    
    // Just by retrieving one successfully from the DAC, we know we have a MethodDesc.
    DacpMethodDescData MethodDescData;
    if (MethodDescData.Request(g_clrData,(CLRDATA_ADDRESS)value) != S_OK)
    {
        return FALSE;
    }
    
    return TRUE;
}

// This is new, thanks to async appdomain unload.
BOOL IsZombieMT(CLRDATA_ADDRESS MethodTable)
{
    NameForMT_s ((DWORD_PTR)MethodTable, g_mdName, mdNameLen);
    return (wcscmp(g_mdName, L"<Unloaded Type>") == 0);
}
    
BOOL IsObject (size_t obj, BOOL verifyFields/*=FALSE*/)
{
    DacpObjectData objectData;
    if (objectData.Request(g_clrData,(CLRDATA_ADDRESS)obj) != S_OK)
    {
        return FALSE;
    }

    if (verifyFields && !IsMTForFreeObj ((DWORD_PTR) objectData.MethodTable) &&
        !IsZombieMT(objectData.MethodTable))
    {        
        DacpEEClassData EECls;
        if (EECls.Request(g_clrData,objectData.EEClass) != S_OK)
        {            
            return FALSE;
        }            
        return VerifyMemberFields (&EECls, obj);
    }
    
    return TRUE;
}

static DacpUsefulGlobalsData g_special_usefulGlobals;

BOOL IsObjectArray (DacpObjectData *pData)
{
    if (pData->ObjectType == OBJ_ARRAY)
    {
        return (g_special_usefulGlobals.ArrayMethodTable == pData->MethodTable);
    }
    return FALSE;
}

BOOL IsObjectArray (DWORD_PTR objPointer)
{
    DWORD_PTR mtAddr;
    if (SUCCEEDED(MOVE(mtAddr, objPointer)))
    {
        return (g_special_usefulGlobals.ArrayMethodTable == (CLRDATA_ADDRESS) mtAddr);
    }
    return FALSE;
}

BOOL IsStringObject (size_t obj)
{
    DWORD_PTR mtAddr;
    if (SUCCEEDED(MOVE(mtAddr, obj)))
    {
        return (g_special_usefulGlobals.StringMethodTable == (CLRDATA_ADDRESS) mtAddr);
    }   
    return FALSE;
}

void AddToModuleList(DWORD_PTR * &moduleList, int &numModule, int &maxList,
                     DWORD_PTR dwModuleAddr)
{
    int i;
    for (i = 0; i < numModule; i ++)
    {
        if (moduleList[i] == dwModuleAddr)
            break;
    }
    if (i == numModule)
    {
        moduleList[numModule] = dwModuleAddr;
        numModule ++;
        if (numModule == maxList)
        {
            DWORD_PTR *list = (DWORD_PTR *)
                malloc (2*maxList * sizeof(PVOID));
            if (list == NULL)
            {
                numModule = 0;
                ControlC = 1;
                return;
            }
            memcpy (list, moduleList, maxList * sizeof(PVOID));
            free (moduleList);
            moduleList = list;
            maxList *= 2;
        }
    }
}

BOOL IsFusionLoadedModule (LPCSTR fusionName, LPCSTR mName)
{
    LPCSTR commaPos = strchr (fusionName, ',');
    if (commaPos)
    {
        // verify that fusionName and mName match up to a comma.
        while (*fusionName != ',')
        {
            if (*mName == '\0')
            {
                return FALSE;
            }
            
            if (tolower(*fusionName) != tolower(*mName))
            {
                return FALSE;
            }
            fusionName++;
            mName++;
        }
        return TRUE;        
    }
    return FALSE;
}
    
BOOL DebuggerModuleNamesMatch (CLRDATA_ADDRESS PEFileAddr, __in __in_z LPSTR mName)
{
    // Another way to see if a module is the same is
    // to accept that mName may be the debugger's name for
    // a loaded module. We can get the debugger's name for
    // the module we are looking at right now, and compare
    // it with mName, if they match exactly, we can add
    // the module to the list.
    if (PEFileAddr)
    {
        DacpPEFileData pefileData;
        if (pefileData.Request (g_clrData, PEFileAddr) == S_OK)
        {
            if (pefileData.Base)
            {
                ULONG Index;
                ULONG64 base;
                if (g_ExtSymbols->GetModuleByOffset(pefileData.Base, 0, &Index, 
                    &base) == S_OK)
                {                                    
                    CHAR ModuleName[MAX_PATH+1];

                    if (g_ExtSymbols->GetModuleNames (Index, base,
                        NULL, 0, NULL,
                        ModuleName, MAX_PATH, NULL,
                        NULL, 0, NULL) == S_OK)
                    {
                        if (_stricmp (ModuleName, mName) == 0)
                        {
                            return TRUE;
                        }
                    }
                }                                
            }
        }                        
    }
    return FALSE;
}

/**********************************************************************\
* Routine Description:                                                 *
*                                                                      *
*    Find the list of Module address given the name of the Module.     *  
*                                                                      *
*    if mName is NULL, you get all modules                             *
\**********************************************************************/
void ModuleFromName(DWORD_PTR * &moduleList, __in __in_z __in_opt LPSTR mName, int &numModule)
{
    moduleList = NULL;
    numModule = 0;

    DacpAppDomainStoreData adsData;

    if (adsData.Request(g_clrData)!=S_OK)
    {
        return;
    }

    CLRDATA_ADDRESS *pAssemblyArray = NULL;
    CLRDATA_ADDRESS *Modules = NULL;
    CLRDATA_ADDRESS *pArray = new CLRDATA_ADDRESS[adsData.DomainCount+2];
    if (pArray==NULL)
    {
        return;
    }

    pArray[0] = adsData.systemDomain;
    pArray[1] = adsData.sharedDomain;
    if (DacpAppDomainStoreData::GetDomains(g_clrData,adsData.DomainCount,pArray+2)!=S_OK)
    {
        ExtOut("Unable to get array of AppDomains\n");
        delete [] pArray;    
        return;
    }

    // List all domain
    size_t AllocSize;
    int maxList = adsData.DomainCount+2; // account for system and shared domains
    if (maxList <= 0 ||
        !ClrSafeInt<size_t>::multiply(maxList, sizeof(PVOID), AllocSize) ||
        (moduleList = (DWORD_PTR *) malloc (AllocSize)) == NULL)
    {
        delete [] pArray;
        return;
    }
    
    WCHAR StringData[MAX_PATH];
    char fileName[sizeof(StringData)/2];
    // Search all domains to find a module
    for (int n = 0; n < adsData.DomainCount+2; n++)
    {
        if (IsInterrupt())
            break;        
        
        DacpAppDomainData appDomain;
        if (appDomain.Request(g_clrData,pArray[n])!=S_OK)
        {
            goto CLEANUP;
        }

        if (appDomain.AssemblyCount)
        {            
            pAssemblyArray = new CLRDATA_ADDRESS[appDomain.AssemblyCount];
            if (pAssemblyArray==NULL)
            {
                goto CLEANUP;
            }

            if (DacpAppDomainData::GetAssemblies(g_clrData,appDomain.AppDomainPtr,appDomain.AssemblyCount,pAssemblyArray)!=S_OK)
            {
                ExtOut("Unable to get array of Assemblies\n");
                goto CLEANUP;
            }
                
            for (int nAssem = 0;nAssem < appDomain.AssemblyCount;nAssem ++)
            {
                if (IsInterrupt())
                    break;

                DacpAssemblyData assemblyData;
                if (assemblyData.Request(g_clrData,pAssemblyArray[nAssem])!=S_OK)
                {
                    goto CLEANUP;
                }

                Modules = new CLRDATA_ADDRESS[assemblyData.ModuleCount];
                if (DacpAssemblyData::GetModules(g_clrData,assemblyData.AssemblyPtr,assemblyData.ModuleCount,Modules) != S_OK)
                {
                   ReportOOM();                               
                   goto CLEANUP;
                }
                
                for (UINT nModule=0;nModule<assemblyData.ModuleCount;nModule++)
                {
                    if (IsInterrupt())
                        break;

                    CLRDATA_ADDRESS ModuleAddr = Modules[nModule];
                    DacpModuleData ModuleData;
                    if (ModuleData.Request(g_clrData,ModuleAddr)!=S_OK)
                    {
                        goto CLEANUP;
                    }
                    FileNameForModule ((DWORD_PTR)ModuleAddr, StringData);
                    int m;
                    for (m = 0; StringData[m] != L'\0'; m ++)
                    {
                        fileName[m] = (char)StringData[m];
                    }
                    fileName[m] = '\0';
                    if ((mName == NULL) || 
                        IsSameModuleName (fileName, mName) ||
                        DebuggerModuleNamesMatch(ModuleData.File, mName) ||
                        IsFusionLoadedModule (fileName, mName))
                    {
                        AddToModuleList (moduleList, numModule, maxList,
                                         (DWORD_PTR)ModuleAddr);
                    }    
                }                        

                delete [] Modules;
                Modules = NULL;
            }
            delete [] pAssemblyArray;            
            pAssemblyArray = NULL;
        }
    }

CLEANUP:
    if (pAssemblyArray)
        delete [] pAssemblyArray;
    if (Modules)
        delete [] Modules;
    if (pArray)
        delete [] pArray;    
}

/**********************************************************************\
* Routine Description:                                                 *
*                                                                      *
*    Find the EE data given a name.                                    *  
*                                                                      *
\**********************************************************************/
void GetInfoFromName(DWORD_PTR ModulePtr, const char* name)
{
    IMetaDataImport* pImport = MDImportForModule (ModulePtr);    
    if (pImport == 0)
        return;

    static WCHAR wszName[MAX_CLASSNAME_LENGTH];
    size_t n;
    size_t length = strlen (name);
    for (n = 0; n <= length; n ++)
        wszName[n] = name[n];

    mdTypeDef cl;
    mdToken tkEnclose = mdTokenNil;
    WCHAR *pName;
    WCHAR *pHead = wszName;
    while ( ((pName = wcschr (pHead,L'+')) != NULL) ||
             ((pName = wcschr (pHead,L'/')) != NULL)) {
        pName[0] = L'\0';
        if (FAILED(pImport->FindTypeDefByName(pHead,tkEnclose,&tkEnclose)))
            return;
        pHead = pName+1;
    }

    pName = pHead;

    if (SUCCEEDED (pImport->FindTypeDefByName (pName, tkEnclose, &cl)))
    {
        GetInfoFromModule(ModulePtr, cl);
        return;
    }
    
    // See if it is a method
    WCHAR *pwzMethod;
    if ((pwzMethod = wcsrchr(pName, L'.')) == NULL)
        return;

    if (pwzMethod[-1] == L'.')
        pwzMethod --;
    pwzMethod[0] = L'\0';
    pwzMethod ++;
    
    if (SUCCEEDED (pImport->FindTypeDefByName (pName, tkEnclose, &cl)))
    {
        mdMethodDef token;
        ULONG cTokens;
        HCORENUM henum = NULL;
        BOOL fStatus = FALSE;
        while (SUCCEEDED (pImport->EnumMethodsWithName (&henum, cl, pwzMethod,
                                                     &token, 1, &cTokens))
               && cTokens == 1)
        {
            if (fStatus)
                dprintf ("-----------------------\n");
            GetInfoFromModule (ModulePtr, token);
            fStatus = TRUE;
        }
        if (fStatus)
            return;

        // is Member?
        henum = NULL;
        if (SUCCEEDED (pImport->EnumMembersWithName (&henum, cl, pwzMethod,
                                                     &token, 1, &cTokens))
            && cTokens == 1)
        {
            dprintf ("Member (mdToken token) of\n");
            GetInfoFromModule (ModulePtr, cl);
            return;
        }

        // is Field?
        henum = NULL;
        if (SUCCEEDED (pImport->EnumFieldsWithName (&henum, cl, pwzMethod,
                                                     &token, 1, &cTokens))
            && cTokens == 1)
        {
            dprintf ("Field (mdToken token) of\n");
            GetInfoFromModule (ModulePtr, cl);
            return;
        }
    }
}

/**********************************************************************\
* Routine Description:                                                 *
*                                                                      *
*    Find the EE data given a token.                                   *  
*                                                                      *
\**********************************************************************/
DWORD_PTR GetMethodDescFromModule(DWORD_PTR ModuleAddr, ULONG token)
{
    if (TypeFromToken(token) != mdtMethodDef)
        return NULL;        
    
    DacpModuleTokenData mtdata;
    mtdata.Module = ModuleAddr;
    mtdata.Token = token;
    if (mtdata.Request(g_clrData)!=S_OK)
    {
        return NULL;
    }
    else if (0 == mtdata.ReturnValue)
    {
        // a NULL ReturnValue means the method desc is not loaded yet
        return MD_NOT_YET_LOADED;
    } 
    else if ( !IsMethodDesc((DWORD_PTR) mtdata.ReturnValue))
    {
        return NULL;
    }
    
    return (DWORD_PTR) mtdata.ReturnValue;    
}

/**********************************************************************\
* Routine Description:                                                 *
*                                                                      *
*    Find the EE data given a name.                                    *  
*                                                                      *
\**********************************************************************/
ULONG32 GetMethodDescsFromName(DWORD_PTR ModulePtr, const char* name, DWORD_PTR *&pOut,int& numMethods)
{
    pOut = NULL;
    numMethods = 0;
    
    IMetaDataImport* pImport = MDImportForModule (ModulePtr);    
    if (pImport == 0)
        return ERROR_BAD_MODULE;

    static WCHAR wszName[MAX_CLASSNAME_LENGTH];
    size_t n;
    size_t length = strlen (name);
    for (n = 0; n <= length; n ++)
        wszName[n] = name[n];

    mdTypeDef cl;
    mdToken tkEnclose = mdTokenNil;
    WCHAR *pName;
    WCHAR *pHead = wszName;
    while ((pName = wcschr (pHead,L'/')) != NULL) {
        pName[0] = L'\0';
        if (FAILED(pImport->FindTypeDefByName(pHead,tkEnclose,&tkEnclose)))
            return ERROR_UNSPECIFIED;
        pHead = pName+1;
    }

    pName = pHead;
    
    // See if it is a method
    WCHAR *pwzMethod;
    if ((pwzMethod = wcsrchr(pName, L'.')) == NULL)
    {
        return ERROR_NOT_A_METHOD;
    }

    if (pwzMethod[-1] == L'.')
        pwzMethod --;
    pwzMethod[0] = L'\0';
    pwzMethod ++;
    
    if (SUCCEEDED (pImport->FindTypeDefByName (pName, tkEnclose, &cl)))
    {
        mdMethodDef token;
        ULONG cTokens;
        HCORENUM henum = NULL;
        int methodCount = 0;
        while (SUCCEEDED (pImport->EnumMethodsWithName (&henum, cl, pwzMethod,
                                                     &token, 1, &cTokens))
               && cTokens == 1)
        {
            methodCount++;
        }

        if (methodCount > 0)
        {
            pOut = new DWORD_PTR[methodCount];
            if (pOut==NULL)
            {
                ReportOOM();        
                return ERROR_OUT_OF_MEMORY;
            }
            
            numMethods = methodCount;

            henum = NULL;
            int i = 0;
            while (SUCCEEDED (pImport->EnumMethodsWithName (&henum, cl, pwzMethod,
                                                         &token, 1, &cTokens))
                   && cTokens == 1)
            {
                pOut[i] = GetMethodDescFromModule (ModulePtr, token);                
                if(pOut[i] == NULL)
                {
                    delete [] pOut;
                    numMethods = 0;
                    return ERROR_BAD_METHODDESC;
                }
                i++;
            }
        }
    }
    
    return 0;
}
    
/**********************************************************************\
* Routine Description:                                                 *
*                                                                      *
*    Find the EE data given a token.                                   *  
*                                                                      *
\**********************************************************************/
void GetInfoFromModule (DWORD_PTR ModuleAddr, ULONG token, DWORD_PTR *ret)
{
    switch (TypeFromToken(token))
    {
        case mdtMethodDef:
            break;
        case mdtTypeDef:
            break;
        case mdtTypeRef:
            break;
        case mdtFieldDef:
            break;            
        default:
            dprintf ("not supported\n");
            return;
            break;
    }
    
    DacpModuleTokenData mtdata;
    mtdata.Module = ModuleAddr;
    mtdata.Token = token;
    if ((mtdata.Request(g_clrData)!=S_OK) ||
        !IsValidToken (ModuleAddr, token))
    {
        dprintf("Invalid module token\n");
        return;
    }
    
    if (ret != NULL)
    {
        *ret = (DWORD_PTR) mtdata.ReturnValue;
        return;
    }

    dprintf ("Token: 0x%p\n", (ULONG64) token);
 
    switch (TypeFromToken(token))
    {
        case mdtFieldDef:
        {
            NameForToken_s (ModuleAddr, token, g_mdName, mdNameLen);
            dprintf ("Field name: %S\n", g_mdName);
            break;
        }
        case mdtMethodDef:
        {
            if (mtdata.ReturnValue)
            {
                dprintf ("MethodDesc: %p\n", mtdata.ReturnValue);
                // Easiest to get full parameterized method name from ..::GetMethodName
                if (DacpMethodDescData::GetMethodName(g_clrData,mtdata.ReturnValue, mdNameLen, g_mdName) != S_OK)
                {
                    // Fall back to just method name without parameters..
                    
                    NameForToken_s (ModuleAddr, token, g_mdName, mdNameLen);
                }
            }
            else
            {
                dprintf ("MethodDesc: <not loaded yet>\n");    
                NameForToken_s (ModuleAddr, token, g_mdName, mdNameLen);
            }
            
            dprintf ("Name: %S\n", g_mdName);
            // Nice to have a little more data
            if (mtdata.ReturnValue)
            {
                DacpMethodDescData MethodDescData;
                if (MethodDescData.Request(g_clrData,mtdata.ReturnValue) == S_OK)
                {
                    if (MethodDescData.bHasNativeCode)
                    {
                        ExtOut("JITTED Code Address: %p\n",MethodDescData.NativeCodeAddr);                
                    }
                    else
                    {
                        ExtOut("Not JITTED yet. Use !bpmd -md %p to break on run.\n",(ULONG64)mtdata.ReturnValue);
                    }
                }
                else
                {
                    ExtOut ("<Error getting MethodDesc information>\n");
                }
            }
            else
            {
                dprintf ("Not JITTED yet.\n");    
            }
            break;
        }
        case mdtTypeDef:
        case mdtTypeRef:
        {
            if (mtdata.ReturnValue)
            {
                dprintf ("MethodTable: %p\n", (ULONG64)mtdata.ReturnValue);
                DacpMethodTableData mtabledata;
                DacpEEClassData eedata;
                if ((mtabledata.Request (g_clrData, mtdata.ReturnValue) == S_OK) &&
                    (eedata.Request (g_clrData, mtabledata.Class) == S_OK))
                {                    
                    dprintf ("EEClass: %p\n", (ULONG64)mtabledata.Class);
                }
                else
                {
                    dprintf ("EEClass: <error getting EEClass>\n");
                }                
            }
            else
            {
                dprintf ("MethodTable: <not loaded yet>\n");
                dprintf ("EEClass: <not loaded yet>\n");                
            }
            NameForToken_s (ModuleAddr, token, g_mdName, mdNameLen);
            dprintf ("Name: %S\n", g_mdName);
            break;
        }
        default:
            break;
    }
    return;
}

BOOL IsMTForFreeObj(DWORD_PTR pMT)
{
    return (pMT == g_special_usefulGlobals.FreeMethodTable);
}


char *EHTypeName(EHClauseType et)
{
    if (et == EHFault)
        return "FAULT";
    else if (et == EHFinally)
        return "FINALLY";
    else if (et == EHFilter)
        return "FILTER";
    else if (et == EHTyped)
        return "TYPED";
    else
        return "UNKNOWN";
}

void DumpMDInfo(DWORD_PTR dwStartAddr, BOOL fStackTraceFormat)
{

    DacpMethodDescData MethodDescData;
    if (MethodDescData.Request(g_clrData,(CLRDATA_ADDRESS)dwStartAddr) != S_OK)
    {
        ExtOut("%p is not a MethodDesc\n", (ULONG64)dwStartAddr);
        return;
    }

    // DumpMDInfo (dwStartAddr);

    static WCHAR wszNameBuffer[1024]; // should be large enough
    BOOL bFailed = FALSE;
    if (DacpMethodDescData::GetMethodName(g_clrData, dwStartAddr, 1024, wszNameBuffer) != S_OK)
    {
        wcscpy_s(wszNameBuffer, _countof(wszNameBuffer),L"UNKNOWN");        
        bFailed = TRUE;        
    }

    if (!fStackTraceFormat)
    {
        ExtOut("Method Name: %S\n", wszNameBuffer);
        ExtOut("Class: %p\n", MethodDescData.EEClassPtr);
        ExtOut("MethodTable: %p\n", MethodDescData.MethodTablePtr);
        ExtOut("mdToken: %08x\n",MethodDescData.MDToken);
        ExtOut("Module: %p\n",MethodDescData.ModulePtr);
        ExtOut("IsJitted: %s\n",MethodDescData.bHasNativeCode ? "yes" : "no");
        ExtOut("m_CodeOrIL: %p\n",MethodDescData.NativeCodeAddr);                
    }
    else
    {
        if (!bFailed)
        {
            ExtOut("%S", wszNameBuffer);
        }
        else
        {
            // Only clutter the display with module/token for cases where we
            // can't get the MethodDesc name for some reason.
            ExtOut ("Unknown MethodDesc (Module %p, mdToken %08x)", 
                (ULONG64) MethodDescData.ModulePtr,
                MethodDescData.MDToken);
        }
    }    
}

void GetDomainList (DWORD_PTR *&domainList, int &numDomain)
{
    DacpAppDomainStoreData adsData;

    numDomain = 0;            
    
    if (adsData.Request(g_clrData)!=S_OK)
    {
        return;
    }

    // Do prefast integer checks before the malloc.
    size_t AllocSize;
    LONG DomainAllocCount;
    if (!ClrSafeInt<LONG>::addition(adsData.DomainCount, 2, DomainAllocCount) ||
        !ClrSafeInt<size_t>::multiply(DomainAllocCount, sizeof(PVOID), AllocSize) ||
        (domainList = (DWORD_PTR*) malloc (AllocSize)) == NULL)
    {
        return;
    }

    domainList[numDomain++] = (DWORD_PTR) adsData.systemDomain;
    domainList[numDomain++] = (DWORD_PTR) adsData.sharedDomain;
    
    CLRDATA_ADDRESS *pArray = new CLRDATA_ADDRESS[adsData.DomainCount];
    if (pArray==NULL)
    {
        return;
    }

    if (DacpAppDomainStoreData::GetDomains(g_clrData,adsData.DomainCount,pArray)!=S_OK)
    {
        delete [] pArray;
        return;
    }

    for (int n=0;n<adsData.DomainCount;n++)
    {
        if (IsInterrupt())
            break;
        domainList[numDomain++] = (DWORD_PTR) pArray[n];
    }

    delete [] pArray;
}


void GetThreadList (DWORD_PTR *&threadList, int &numThread)
{
    DacpThreadStoreData ThreadStore;

    numThread = 0;

    if ( ThreadStore.Request(g_clrData) != S_OK)
    {
        return;
    }
     
    threadList = (DWORD_PTR*) malloc (ThreadStore.threadCount * sizeof(PVOID));    
    if (threadList == NULL)
        return;
    
    CLRDATA_ADDRESS CurThread = ThreadStore.firstThread;
    while (CurThread != NULL)
    {
        if (IsInterrupt())
            return;

        DacpThreadData Thread;
        if (Thread.Request(g_clrData, CurThread) != S_OK)
        {
            ExtOut("Failed to request Thread at %p\n", CurThread);
            return;
        }

        threadList[numThread++] = (DWORD_PTR) CurThread;
        CurThread = Thread.nextThread;
    }

}

CLRDATA_ADDRESS GetCurrentManagedThread ()
{
    DacpThreadStoreData ThreadStore;
    ThreadStore.Request(g_clrData);

    ULONG Tid;
    g_ExtSystem->GetCurrentThreadSystemId(&Tid);
    
    CLRDATA_ADDRESS CurThread = ThreadStore.firstThread;
    while (CurThread)
    {
        DacpThreadData Thread;
        if (Thread.Request(g_clrData, CurThread) != S_OK)
        {
            return NULL;
        }        
        
        if (Thread.osThreadId == Tid)
        {        
            return CurThread;
        }
        
        CurThread = Thread.nextThread;
    }
    return NULL;
}

void ReloadSymbolWithLineInfo()
{
    static BOOL bLoadSymbol = FALSE;
    if (!bLoadSymbol)
    {
        ULONG Options;
        g_ExtSymbols->GetSymbolOptions (&Options);
        if (!(Options & SYMOPT_LOAD_LINES))
        {
            g_ExtSymbols->AddSymbolOptions (SYMOPT_LOAD_LINES);
            g_ExtSymbols->Reload ("/f mscoree.dll");
            EEFLAVOR flavor = GetEEFlavor ();
            if (flavor == MSCORWKS)
                g_ExtSymbols->Reload ("/f mscorwks.dll");
        }
        
        // reload mscoree.pdb and mscorjit.pdb to get line info
        bLoadSymbol = TRUE;
    }
}

// Return 1 if the function is our stub
// Return MethodDesc if the function is managed
// Otherwise return 0
size_t FunctionType (size_t EIP)
{
    ULONG64 base = 0;
    ULONG   ulLoaded, ulUnloaded, ulIndex;

    // Get the number of loaded and unloaded modules
    if (FAILED(g_ExtSymbols->GetNumberModules(&ulLoaded, &ulUnloaded)))
        return 0;


    if (SUCCEEDED(g_ExtSymbols->GetModuleByOffset(EIP, 0, &ulIndex, &base)) && base != 0)
    {
        if (ulIndex < ulLoaded)
        {
            IMAGE_DOS_HEADER DosHeader;
            if (g_ExtData->ReadVirtual(base, &DosHeader, sizeof(DosHeader), NULL) != S_OK)
                return 0;
            IMAGE_NT_HEADERS32 Header32;
            if (g_ExtData->ReadVirtual(base + DosHeader.e_lfanew, &Header32, sizeof(Header32), NULL) != S_OK)
                return 0;
            // If there is no COMHeader, this can not be managed code.
            if (Header32.OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_COMHEADER].VirtualAddress == 0)
                return 0;
            
            IMAGE_COR20_HEADER ComPlusHeader;
            if (g_ExtData->ReadVirtual(base + Header32.OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_COMHEADER].VirtualAddress,
                                       &ComPlusHeader, sizeof(ComPlusHeader), NULL) != S_OK)
                return 0;
            
            // If there is no Precompiled image info, it can not be prejit code
            if (ComPlusHeader.ManagedNativeHeader.VirtualAddress == 0) {
                return 0;
            }
        }
    }

    CLRDATA_ADDRESS dwStartAddr = (CLRDATA_ADDRESS) EIP;
    DacpMethodDescData MethodDescData;
    if (MethodDescData.RequestFromIP(g_clrData,dwStartAddr) != S_OK)
    {
        return 1;
    }

    return (size_t) MethodDescData.MethodDescPtr;
}

    
size_t ObjectSize(DWORD_PTR obj,BOOL fIsLargeObject)
{
    DWORD_PTR dwMT;
    MOVE(dwMT, obj);
    return ObjectSize(obj, dwMT, FALSE, fIsLargeObject);
}

size_t ObjectSize(DWORD_PTR obj, DWORD_PTR mt, BOOL fIsValueClass, BOOL fIsLargeObject)
{
    BOOL bContainsPointers;
    size_t size = 0;
    if (!GetSizeEfficient(obj, mt, fIsLargeObject, size, bContainsPointers))
    {
        return 0;
    }
    return size;
}

struct strobjInfo
{
    size_t  methodTable;
    DWORD   m_ArrayLength;
    DWORD   m_StringLength;
};

void StringObjectContent (size_t obj, BOOL fLiteral, const int length)
{
    DacpObjectData objData;
    if (objData.Request(g_clrData,(CLRDATA_ADDRESS) obj)!=S_OK)
    {
        ExtOut("<Invalid Object>\n");
        return;
    }
    
    strobjInfo stInfo;

    if (MOVE(stInfo,obj) != S_OK)
    {
        ExtOut ("Error getting string data\n");
        return;
    }

    if (objData.Size > 0x200000 ||
        stInfo.m_ArrayLength > 0x200000 ||
        stInfo.m_StringLength > 0x200000)
    {
        ExtOut ("<String is invalid or too large to print>\n");
        return;
    }
    
    LPWSTR pwszBuf = new WCHAR[stInfo.m_StringLength+1];
    if (pwszBuf == NULL)
    {
        return;
    }
    
    DWORD_PTR dwAddr = (DWORD_PTR) pwszBuf;
    if (DacpObjectData::GetStringObjectData(g_clrData,(CLRDATA_ADDRESS) obj, 
        stInfo.m_StringLength+1, pwszBuf)!=S_OK)
    {
        ExtOut("Error getting string data\n");
        delete [] pwszBuf;
        return;
    }

    if (!fLiteral) 
    {
        pwszBuf[stInfo.m_StringLength] = L'\0';
        ExtOut ("%S", pwszBuf);
    }
    else
    {
        ULONG32 count = stInfo.m_StringLength;
    WCHAR buffer[256];
    WCHAR out[512];
        while (count) 
        {
        DWORD toRead = 255;
        if (count < toRead) {
            toRead = count;
        }
        ULONG bytesRead;
        wcsncpy_s(buffer,_countof(buffer),(LPWSTR) dwAddr, toRead);
        bytesRead = toRead*sizeof(WCHAR);
        DWORD wcharsRead = bytesRead/2;
        buffer[wcharsRead] = L'\0';
        
            ULONG j,k=0;
            for (j = 0; j < wcharsRead; j ++) 
        {
                if (iswprint (buffer[j])) {
                    out[k] = buffer[j];
                    k ++;
                }
                else
                {
                    out[k++] = L'\\';
                    switch (buffer[j]) {
                    case L'\n':
                        out[k++] = L'n';
                        break;
                    case L'\0':
                        out[k++] = L'0';
                        break;
                    case L'\t':
                        out[k++] = L't';
                        break;
                    case L'\v':
                        out[k++] = L'v';
                        break;
                    case L'\b':
                        out[k++] = L'b';
                        break;
                    case L'\r':
                        out[k++] = L'r';
                        break;
                    case L'\f':
                        out[k++] = L'f';
                        break;
                    case L'\a':
                        out[k++] = L'a';
                        break;
                    case L'\\':
                        break;
                    case L'\?':
                        out[k++] = L'?';
                        break;
                    default:
                        out[k++] = L'?';
                        break;
                    }
                }
            }

            out[k] = L'\0';
            ExtOut ("%S", out);

        count -= wcharsRead;
        dwAddr += bytesRead;
        }
    }

    delete [] pwszBuf;
}


BOOL GetValueForCMD (const char *ptr, const char *end, ARGTYPE type, size_t *value)
{   
    if (type == COSTRING) {
        // Allocate memory for the length of the string. Whitespace terminates
        // User must free the string data. 
        char *pszValue = NULL;
        size_t dwSize = (end - ptr);    
        pszValue= new char[dwSize+1];
        if (pszValue == NULL)
        {
            return FALSE;
        }
        strncpy_s(pszValue,dwSize+1,ptr,dwSize); // _TRUNCATE
        *value = (size_t) pszValue;               
    } else {
        char *last;
        if (type == COHEX) {
            *value = strtoul(ptr,&last,16);
        }
        else {     
            *value = strtoul(ptr,&last,10);
        }


        if (last != end) {
            return FALSE;
        }
    }

    return TRUE;
}

void SetValueForCMD (void *vptr, ARGTYPE type, size_t value)
{
    switch (type) {
    case COBOOL:
        *(BOOL*)vptr = (BOOL) value;
        break;
    case COSIZE_T:
    case COSTRING:
    case COHEX:
        *(SIZE_T*)vptr = value;
        break;
    }
}

BOOL GetCMDOption(const char *string, CMDOption *option, size_t nOption,
                  CMDValue *arg, size_t maxArg, size_t *nArg)
{
    const char *end;
    const char *ptr = string;
    BOOL endofOption = FALSE;

    for (size_t n = 0; n < nOption; n ++)
    {
        if (IsInterrupt())
            return FALSE;
        
        option[n].hasSeen = FALSE;
    }

    if (nArg) {
        *nArg = 0;
    }

    while (ptr[0] != '\0')
    {
        if (IsInterrupt())
            return FALSE;
        
        // skip any space
        if (isspace (ptr[0])) {
            while (isspace (ptr[0]))
            {
                if (IsInterrupt())
                    return FALSE;
        
                ptr ++;
            }
            
            continue;
        }

        end = ptr;

        // Arguments can be quoted with ". We'll remove the quotes and
        // allow spaces to exist in the string.
        BOOL bQuotedArg = FALSE;
        if (ptr[0] == '\'' && ptr[1] != '-')
        {            
            bQuotedArg = TRUE;

            // skip quote
            ptr++;
            end++;
            
            while (end[0] != '\'' && end[0] != '\0')
            {
                if (IsInterrupt())
                    return FALSE;
            
                end ++;
            }
            if (end[0] != '\'')
            {
                // Error, th ere was a start quote but no end quote
                ExtOut ("Missing quote in %s\n", ptr);
                return FALSE;
            }
        }
        else // whitespace terminates
        {
            while (!isspace(end[0]) && end[0] != '\0')
            {
                if (IsInterrupt())
                    return FALSE;
            
                end ++;
            }
        }

        if (ptr[0] != '-') {
            if (maxArg == 0) {
                ExtOut ("Incorrect argument: %s\n", ptr);
                return FALSE;
            }
            endofOption = TRUE;
            if (*nArg >= maxArg) {
                ExtOut ("Incorrect argument: %s\n", ptr);
                return FALSE;
            }
            
            size_t value;
            if (!GetValueForCMD (ptr,end,arg[*nArg].type,&value)) {

                char oldChar = *end;
                *(char *)end = '\0';
                value = (size_t)GetExpression (ptr);
                *(char *)end = oldChar;
                
                /*

                    It is silly to do this, what if 0 is a valid expression for
                    the command?
                    
                if (value == 0) {
                    ExtOut ("Invalid argument: %s\n", ptr);
                    return FALSE;
                }
                */
            }

            SetValueForCMD (arg[*nArg].vptr, arg[*nArg].type, value);

            (*nArg) ++;
        }
        else if (endofOption) {
            ExtOut ("Wrong option: %s\n", ptr);
            return FALSE;
        }
        else {
            char buffer[80];
            if (end-ptr > 79) {
                ExtOut ("Invalid option %s\n", ptr);
                return FALSE;
            }
            strncpy_s (buffer,_countof(buffer), ptr, end-ptr);

            size_t n;
            for (n = 0; n < nOption; n ++)
            {
                if (IsInterrupt())
                    return FALSE;
        
                if (_stricmp (buffer, option[n].name) == 0) {
                    if (option[n].hasSeen) {
                        ExtOut ("Invalid option: option specified multiple times: %s\n", buffer);
                        return FALSE;
                    }
                    option[n].hasSeen = TRUE;
                    if (option[n].hasValue) {
                        // skip any space
                        ptr = end;
                        if (isspace (ptr[0])) {
                            while (isspace (ptr[0]))
                            {
                                if (IsInterrupt())
                                    return FALSE;
        
                                ptr ++;
                            }
                        }
                        if (ptr[0] == '\0') {
                            ExtOut ("Missing value for option %s\n", buffer);
                            return FALSE;
                        }
                        end = ptr;
                        while (!isspace(end[0]) && end[0] != '\0')
                        {
                            if (IsInterrupt())
                                return FALSE;
        
                            end ++;
                        }

                        size_t value;
                        if (!GetValueForCMD (ptr,end,option[n].type,&value)) {

                            char oldChar = *end;
                            *(char *)end = '\0';
                            value = (size_t)GetExpression (ptr);
                            *(char *)end = oldChar;
                        }

                        SetValueForCMD (option[n].vptr,option[n].type,value);
                    }
                    else {
                        SetValueForCMD (option[n].vptr,option[n].type,TRUE);
                    }
                    break;
                }
            }
            if (n == nOption) {
                ExtOut ("Unknown option: %s\n", buffer);
                return FALSE;
            }
        }

        ptr = end;
        if (bQuotedArg)
        {
            ptr++;
        }
    }
    return TRUE;
}

ReadVirtualCache g_special_rvCacheSpace;
ReadVirtualCache *rvCache = &g_special_rvCacheSpace;

void ResetGlobals(void)
{
    // There are some globals used in SOS that exist for efficiency in one command,
    // but should be reset because the next execution of an SOS command could be on
    // another managed process. Reset them to a default state here, as this command
    // is called on every SOS entry point.
    g_special_usefulGlobals.Request(g_clrData);
    g_special_mtCache.Clear();
    g_special_rvCacheSpace.Clear();
}

HRESULT
LoadClrDebugDll(void)
{
    WDBGEXTS_CLR_DATA_INTERFACE Query;

    Query.Iid = &__uuidof(IXCLRDataProcess);
    if (!Ioctl(IG_GET_CLR_DATA_INTERFACE, &Query, sizeof(Query)))
    {
        return E_FAIL;
    }

    g_clrData = (IXCLRDataProcess*)Query.Iface;
    
    return S_OK;
}

void
UnloadClrDebugDll(void)
{
    // Debugger manages the DLL, so nothing to do right now.
}

typedef enum
{
    GC_HEAP_INVALID = 0,
    GC_HEAP_WKS     = 1,
    GC_HEAP_SVR     = 2
} GC_HEAP_TYPE;

/**********************************************************************\
* Routine Description:                                                 *
*                                                                      *
*    This function is called to find out if runtime is server build    *  
*                                                                      *
\**********************************************************************/

DacpGcHeapData *g_pHeapData = NULL;
DacpGcHeapData g_HeapData;

inline BOOL InitializeHeapData()
{
    if (g_pHeapData == NULL)
    {        
        if (g_HeapData.Request(g_clrData) != S_OK)
        {
            return FALSE;
        }
        g_pHeapData = &g_HeapData;
    }
    return TRUE;
}

BOOL IsServerBuild() 
{
    return InitializeHeapData() ? g_pHeapData->bServerMode : FALSE;	
}

UINT GetMaxGeneration()
{
    return InitializeHeapData() ? g_pHeapData->g_max_generation : 0;	
}

UINT GetGcHeapCount()
{
    return InitializeHeapData() ? g_pHeapData->HeapCount : 0;	
}

BOOL GetGcStructuresValid()
{
    // We don't want to use the cached HeapData, because this can change
    // each time the program runs for a while.
    DacpGcHeapData heapData;
    if (heapData.Request(g_clrData) != S_OK)
    {
        return FALSE;
    }

    return heapData.bGcStructuresValid;
}

void GetAllocContextPtrs(AllocInfo *pallocInfo)
{
    pallocInfo->num = 0;
    pallocInfo->array = NULL;    
    
    DacpThreadStoreData ThreadStore;
    if ( ThreadStore.Request(g_clrData) != S_OK)
    {
        return;
    }

    int numThread = ThreadStore.threadCount;
    if (numThread)
    {
        pallocInfo->array =
            (needed_alloc_context*)malloc(numThread * sizeof(needed_alloc_context));
        if (pallocInfo->array == NULL)
        {
            return;
        }
    }

    CLRDATA_ADDRESS CurThread = ThreadStore.firstThread;
    while (CurThread != NULL)
    {
        if (IsInterrupt())
            return;

        DacpThreadData Thread;
        if (Thread.Request(g_clrData, CurThread) != S_OK)
        {
            return;
        }

        if (Thread.allocContextPtr != 0)
        {
            int j;      
            for (j = 0; j < pallocInfo->num; j ++)
            {
                if (pallocInfo->array[j].alloc_ptr == (BYTE *) Thread.allocContextPtr)
                    break;
            }
            if (j == pallocInfo->num)
            {
                pallocInfo->num ++;
                pallocInfo->array[j].alloc_ptr = (BYTE *) Thread.allocContextPtr;
                pallocInfo->array[j].alloc_limit = (BYTE *) Thread.allocContextLimit;
            }
        }
        
        CurThread = Thread.nextThread;
    }
}

HRESULT ReadVirtualCache::Read(ULONG64 Offset, PVOID Buffer, ULONG BufferSize, PULONG lpcbBytesRead)
{
    // Offset can be any random ULONG64, as it can come from VerifyObjectMember(), and this
    // can pass random pointer values in case of GC heap corruption
    HRESULT ret;
    ULONG cbBytesRead = 0;

    if (BufferSize == 0)
        return S_OK;

    if (BufferSize > CACHE_SIZE)
    {
        // Don't even try with the cache
        return g_ExtData->ReadVirtual(Offset, Buffer, BufferSize, lpcbBytesRead);
    }
 
    if ((m_cacheValid)
        && (Offset >= m_startCache) 
        && (Offset <= m_startCache + m_cacheSize - BufferSize))

    {
        // It is within the cache
        memcpy(Buffer,(LPVOID) ((ULONG64)m_cache + (Offset - m_startCache)), BufferSize);

        if (lpcbBytesRead != NULL)
        {
           *lpcbBytesRead = BufferSize;
        }
 
        return S_OK;
    }
 
    m_cacheValid = FALSE;
    m_startCache = Offset;

    // avoid an int overflow
    if (m_startCache + CACHE_SIZE < m_startCache)
        m_startCache = (ULONG64)(-CACHE_SIZE);

    ret = g_ExtData->ReadVirtual(m_startCache, m_cache, CACHE_SIZE, &cbBytesRead);
    if (ret != S_OK)
    {
        return ret;
    }
    
    m_cacheSize = cbBytesRead;     
    m_cacheValid = TRUE;
    memcpy(Buffer, (LPVOID) ((ULONG64)m_cache + (Offset - m_startCache)), BufferSize);

    if (lpcbBytesRead != NULL)
    {
        *lpcbBytesRead = cbBytesRead;
    }

    return S_OK;
}

StressLogMem::~StressLogMem ()
{
    MemRange * range = list;
    
    while (range)
    {
        MemRange * temp = range->next;
        delete range;
        range = temp;
    }
}

bool StressLogMem::Init (ULONG64 stressLogAddr, IDebugDataSpaces* memCallBack)
{
    size_t ThreadStressLogAddr = NULL;
    HRESULT hr = memCallBack->ReadVirtual(stressLogAddr + offsetof (StressLog, logs), 
            &ThreadStressLogAddr, sizeof (ThreadStressLogAddr), 0);
    if (hr != S_OK)
    {
        return false;
    }    
   
    while(ThreadStressLogAddr != NULL) 
    {
        size_t ChunkListHeadAddr = NULL;
        HRESULT hr = memCallBack->ReadVirtual(ThreadStressLogAddr + ThreadStressLog::OffsetOfListHead (), 
            &ChunkListHeadAddr, sizeof (ChunkListHeadAddr), 0);
        if (hr != S_OK || ChunkListHeadAddr == NULL)
        {
            return false;
        }

        size_t StressLogChunkAddr = ChunkListHeadAddr;
        
        do
        {
            AddRange (StressLogChunkAddr, sizeof (StressLogChunk));
            hr = memCallBack->ReadVirtual(StressLogChunkAddr + offsetof (StressLogChunk, next), 
                &StressLogChunkAddr, sizeof (StressLogChunkAddr), 0);
            if (hr != S_OK)
            {
                return false;
            }
            if (StressLogChunkAddr == NULL)
            {
                return true;
            }            
        } while (StressLogChunkAddr != ChunkListHeadAddr);

        hr = memCallBack->ReadVirtual(ThreadStressLogAddr + ThreadStressLog::OffsetOfNext (), 
            &ThreadStressLogAddr, sizeof (ThreadStressLogAddr), 0);
        if (hr != S_OK)
        {
            return false;
        }        
    }

    return true;
}

bool StressLogMem::IsInStressLog (ULONG64 addr)
{
    MemRange * range = list;
    while (range)
    {
        if (range->InRange (addr))
            return true;
        range = range->next;
    }

    return false;
}


