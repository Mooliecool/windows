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
#ifndef __exts_h__
#define __exts_h__

#define KDEXT_64BIT


#include <windows.h>
#if defined(_MSC_VER)
#pragma warning(disable:4245)   // signed/unsigned mismatch
#pragma warning(disable:4100)   // unreferenced formal parameter
#pragma warning(disable:4201)   // nonstandard extension used : nameless struct/union
#pragma warning(disable:4127)   // conditional expression is constant
#pragma warning(disable:4430)   // missing type specifier: C++ doesn't support default-int
#endif
#include "strike.h"

typedef GUID *LPGUID;
// #define FORCEINLINE 
#define DBG_COMMAND_EXCEPTION 0x40010009
#define DEBUG_NO_IMPLEMENTATION

#include <wdbgexts.h>
#include <dbgeng.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifdef __cplusplus
extern "C" {
#endif

#ifndef MINIDUMP

#define EXIT_API     ExtRelease


// Safe release and NULL.
#define EXT_RELEASE(Unk) \
    ((Unk) != NULL ? ((Unk)->Release(), (Unk) = NULL) : NULL)

// Global variables initialized by query.
extern PDEBUG_ADVANCED       g_ExtAdvanced;
extern PDEBUG_CLIENT         g_ExtClient;
extern PDEBUG_CONTROL2        g_ExtControl;
extern PDEBUG_DATA_SPACES    g_ExtData;
extern PDEBUG_DATA_SPACES2   g_ExtData2;
extern PDEBUG_REGISTERS      g_ExtRegisters;
extern PDEBUG_SYMBOLS        g_ExtSymbols;
extern PDEBUG_SYMBOLS2       g_ExtSymbols2;
extern PDEBUG_SYSTEM_OBJECTS g_ExtSystem;

HRESULT
ExtQuery(PDEBUG_CLIENT Client);

void
ExtRelease(void);

// Change indent for ExtOut
void IncrementIndent ();
void DecrementIndent ();
void ExtOutIndent ();

// Normal output.
void __cdecl ExtOut(PCSTR Format, ...);
// Error output.
void __cdecl ExtErr(PCSTR Format, ...);
// Warning output.
void __cdecl ExtWarn(PCSTR Format, ...);
// Verbose output.
void __cdecl ExtVerb(PCSTR Format, ...);

// Extension debugging output.
void __cdecl _ExtDbgOut(PCSTR Format, ...);
#ifdef _DEBUG
extern DWORD g_bDbgOutput;      // controls whether ExtDbgOut writes anything out or not
#define ExtDbgOut _ExtDbgOut
#else
#define ExtDbgOut 1 ? (void)0 : _ExtDbgOut
#endif

extern BOOL ControlC;

inline BOOL IsInterrupt() 
{
    if (ControlC) {
        return ControlC;
    }
    if (g_ExtControl->GetInterrupt() == S_OK)
        ControlC = TRUE;
    return ControlC;
}
    
//
// undef the wdbgexts
//
#undef DECLARE_API


#include "rotor_pal.h"

typedef struct _DEBUG_CLIENT_API_CONTEXT {
    PDEBUG_CLIENT Client;
    PCSTR         args;
} DEBUG_CLIENT_API_CONTEXT, * PDEBUG_CLIENT_API_CONTEXT;

#define DECLARE_API(extension)     \
CPPMOD HRESULT CALLBACK ___##extension(PDEBUG_CLIENT_API_CONTEXT PalContext); \
CPPMOD HRESULT CALLBACK __##extension(PDEBUG_CLIENT Client, PCSTR args); \
CPPMOD HRESULT CALLBACK extension(PDEBUG_CLIENT Client, PCSTR args) \
{ \
    DEBUG_CLIENT_API_CONTEXT context;      \
    context.Client = Client; \
    context.args = args;     \
    return PAL_EntryPoint((LPTHREAD_START_ROUTINE)___##extension, &context); \
} \
CPPMOD HRESULT CALLBACK ___##extension(PDEBUG_CLIENT_API_CONTEXT Context) \
{ \
    return __##extension(Context->Client, Context->args); \
} \
CPPMOD HRESULT CALLBACK __##extension(PDEBUG_CLIENT Client, PCSTR args)


class __ExtensionCleanUp
{
public:
    __ExtensionCleanUp(){}
    ~__ExtensionCleanUp(){ExtRelease();}
};

inline void EENotLoadedMessage(HRESULT Status)
{
    ExtOut("Failed to find runtime DLL (mscorwks.dll), 0x%08x\n",Status);
    ExtOut("Extension commands need mscorwks.dll in order to have something to do.\n");
}

inline void DACMessage(HRESULT Status)
{
    ExtOut("Failed to load data access DLL, 0x%08x\n",Status);
    ExtOut("Verify that 1) you have a recent build of the debugger (6.2.14 or newer)\n");
    ExtOut("            2) the file mscordacwks.dll that matches your version of mscorwks.dll is \n");
    ExtOut("                in the version directory\n");
    ExtOut("            3) or, if you are debugging a dump file, verify that the file \n");
    ExtOut("                mscordacwks_<arch>_<arch>_<version>.dll is on your symbol path.\n");
    ExtOut("            4) you are debugging on the same architecture as the dump file.\n");
    ExtOut("                For example, an IA64 dump file must be debugged on an IA64\n");
    ExtOut("                machine.\n");
    ExtOut("\n");
    ExtOut("You can also run the debugger command .cordll to control the debugger's\n");
    ExtOut("load of mscordacwks.dll.  .cordll -ve -u -l will do a verbose reload.\n");
    ExtOut("If that succeeds, the SOS command should work on retry.\n");
    ExtOut("\n");
    ExtOut("If you are debugging a minidump, you need to make sure that your executable\n");
    ExtOut("path is pointing to mscorwks.dll as well.\n");
}


#define INIT_API_NODAC()                                        \
    HRESULT Status;                                             \
    __ExtensionCleanUp __extensionCleanUp;                      \
    if ((Status = ExtQuery(Client)) != S_OK) return Status;     \
    ControlC = FALSE;                                           \
    if ((Status = CheckEEDll()) != S_OK)                        \
    {                                                           \
        EENotLoadedMessage(Status);                             \
        return Status;                                          \
    }                                                           

#define INIT_API()                                              \
    INIT_API_NODAC()                                            \
    if ((Status = LoadClrDebugDll()) != S_OK)                   \
    {                                                           \
        DACMessage(Status);                                     \
        return Status;                                          \
    }                                                           \
    ResetGlobals();           
    
                                                                                
extern BOOL g_bDacBroken;

#define PAGE_ALIGN64(Va) ((ULONG64)((Va) & ~((ULONG64) ((LONG64) (LONG) PageSize - 1))))

extern ULONG PageSize;

//-----------------------------------------------------------------------------------------
//
//  api declaration macros & api access macros
//
//-----------------------------------------------------------------------------------------

extern WINDBG_EXTENSION_APIS ExtensionApis;
extern ULONG TargetMachine;
extern ULONG g_TargetClass;

extern ULONG g_VDbgEng;


#define OS_PAGE_SIZE   4096
 
#define CACHE_SIZE  OS_PAGE_SIZE
 
struct ReadVirtualCache
{
    BYTE m_cache[CACHE_SIZE];
    ULONG64 m_startCache;
    BOOL m_cacheValid;
    ULONG m_cacheSize;
    
    ReadVirtualCache() { Clear(); }
    HRESULT Read(ULONG64 Offset, PVOID Buffer, ULONG BufferSize, PULONG lpcbBytesRead);    
    void Clear() { m_cacheValid = FALSE; m_cacheSize = CACHE_SIZE; }
};

extern ReadVirtualCache *rvCache;

#define MOVE(dst, src) rvCache->Read((ULONG64)src, &(dst), sizeof(dst), NULL)
#define MOVEN(dst, src) rvCache->Read((ULONG64)src, &(dst), sizeof(dst), NULL)
#define MOVEBLOCK(dst, src, size) rvCache->Read(src, &(dst), size, NULL)
#define MOVEBLOCKA(dst, src, size) rvCache->Read(src, dst, size, NULL)
#define MOVEBLOCKF(dst, src, size, retVal) rvCache->Read(src, dst, size, retVal)
 
#define moveN(dst, src)\
{                                                                       \
    HRESULT ret = rvCache->Read((ULONG64)src, &(dst), sizeof(dst), NULL); \
    if (FAILED(ret)) return ret;                                        \
}
 
#define moveBlockN(dst, src, size)\
{                                                                       \
    HRESULT ret = rvCache->Read((ULONG64)src, &(dst), size, NULL);        \
    if (FAILED(ret)) return ret;                                        \
}
 
#define move(dst, src)\
{                                                                       \
    HRESULT ret = rvCache->Read((ULONG64)src, &(dst), sizeof(dst), NULL); \
    if (FAILED(ret)) return;                                            \
}
 
#define moveBlock(dst, src, size)\
{                                                                       \
    HRESULT ret = rvCache->Read((ULONG64)src, &(dst), size, NULL);        \
    if (FAILED(ret)) return;                                            \
}
 
#define moveBlockFailRet(dst, src, size, retVal)\
{                                                                       \
    HRESULT ret = rvCache->Read((ULONG64)src, &(dst), size, NULL);        \
    if (FAILED(ret)) return retVal;                                     \
}

#ifdef __cplusplus
#define CPPMOD extern "C"
#else
#define CPPMOD
#endif

#define GetExpression           (ExtensionApis.lpGetExpressionRoutine)

#ifndef malloc
#define malloc( n ) HeapAlloc( GetProcessHeap(), 0, (n) )
#endif
#ifndef free
#define free( p ) HeapFree( GetProcessHeap(), 0, (p) )
#endif

//-----------------------------------------------------------------------------------------
//
//  prototypes for internal non-exported support functions
//
//-----------------------------------------------------------------------------------------

/////////////////////////////////////////////
//
//  Util.c
//
/////////////////////////////////////////////

typedef VOID
(*PDUMP_SPLAY_NODE_FN)(
    ULONG64 RemoteAddress,
    ULONG   Level
    );

ULONG
DumpSplayTree(
    IN ULONG64 pSplayLinks,
    IN PDUMP_SPLAY_NODE_FN DumpNodeFn
    );

BOOLEAN
DbgRtlIsRightChild(
    ULONG64 pLinks,
    ULONG64 Parent
    );

BOOLEAN
DbgRtlIsLeftChild(
    ULONG64 pLinks,
    ULONG64 Parent
    );

ULONG
GetBitFieldOffset (
   __in __in_z IN LPSTR     Type, 
   __in __in_z IN LPSTR     Field, 
   OUT PULONG   pOffset,
   OUT PULONG   pSize
   );

ULONG64
GetPointerFromAddress (
    ULONG64 Location
    );

VOID
DumpUnicode(
    UNICODE_STRING u
    );

VOID
DumpUnicode64(
    UNICODE_STRING64 u
    );


ULONG64
GetPointerValue (
    __in __in_z PCHAR String
    );

BOOLEAN
IsHexNumber(
   const char *szExpression
   );

BOOLEAN
IsDecNumber(
   const char *szExpression
   );

BOOLEAN
CheckSingleFilter (
        __in __in_z PCHAR Tag,
        __in __in_z PCHAR Filter
    );

#endif

#ifdef __cplusplus
}
#endif

#endif // __exts_h__

