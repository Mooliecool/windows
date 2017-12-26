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
#include "exts.h"

#define VER_PRODUCTVERSION_W        (0x0100)

//
// globals
//
EXT_API_VERSION         ApiVersion = { (VER_PRODUCTVERSION_W >> 8), (VER_PRODUCTVERSION_W & 0xff), EXT_API_VERSION_NUMBER64, 0 };
WINDBG_EXTENSION_APIS   ExtensionApis;

ULONG PageSize;

//
// Valid for the lifetime of the debug session.
//

ULONG   TargetMachine;
BOOL    Connected;
ULONG   g_TargetClass;
DWORD   g_Indent = 0;
DWORD   g_bDbgOutput = 0;

PDEBUG_ADVANCED       g_ExtAdvanced;
PDEBUG_CLIENT         g_ExtClient;
PDEBUG_CONTROL2       g_ExtControl;
PDEBUG_DATA_SPACES    g_ExtData;
PDEBUG_DATA_SPACES2   g_ExtData2;
PDEBUG_REGISTERS      g_ExtRegisters;
PDEBUG_SYMBOLS        g_ExtSymbols;
PDEBUG_SYMBOLS2       g_ExtSymbols2;
PDEBUG_SYSTEM_OBJECTS g_ExtSystem;

#define EXT_INDENT_STRING "    "

// Queries for all debugger interfaces.
extern "C" HRESULT
ExtQuery(PDEBUG_CLIENT Client)
{
    HRESULT Status;
    
    if ((Status = Client->QueryInterface(__uuidof(IDebugAdvanced),
                                 (void **)&g_ExtAdvanced)) != S_OK)
    {
        goto Fail;
    }
    if ((Status = Client->QueryInterface(__uuidof(IDebugControl2),
                                 (void **)&g_ExtControl)) != S_OK)
    {
        goto Fail;
    }
    if ((Status = Client->QueryInterface(__uuidof(IDebugDataSpaces),
                                 (void **)&g_ExtData)) != S_OK)
    {
        goto Fail;
    }    
    if ((Status = Client->QueryInterface(__uuidof(IDebugDataSpaces2),
                                 (void **)&g_ExtData2)) != S_OK)
    {
        goto Fail;
    }

    if ((Status = Client->QueryInterface(__uuidof(IDebugRegisters),
                                 (void **)&g_ExtRegisters)) != S_OK)
    {
        goto Fail;
    }
    if ((Status = Client->QueryInterface(__uuidof(IDebugSymbols),
                                 (void **)&g_ExtSymbols)) != S_OK)
    {
        goto Fail;
    }
    if (Client->QueryInterface(__uuidof(IDebugSymbols2), 
                                 (void **)&g_ExtSymbols2) != S_OK)
    {
        g_ExtSymbols2 = NULL;
    }
    if ((Status = Client->QueryInterface(__uuidof(IDebugSystemObjects),
                                         (void **)&g_ExtSystem)) != S_OK)
    {
        goto Fail;
    }

    g_ExtClient = Client;
    
    return S_OK;

 Fail:
    ExtRelease();
    return Status;
}

// Cleans up all debugger interfaces.
void
ExtRelease(void)
{
    g_ExtClient = NULL;
    EXT_RELEASE(g_ExtAdvanced);
    EXT_RELEASE(g_ExtControl);
    EXT_RELEASE(g_ExtData);
    EXT_RELEASE(g_ExtData2);
    EXT_RELEASE(g_ExtRegisters);
    EXT_RELEASE(g_ExtSymbols);
    EXT_RELEASE(g_ExtSymbols2);
    EXT_RELEASE(g_ExtSystem);
}

void IncrementIndent ()
{
    g_Indent++;
}

void DecrementIndent ()
{
    g_Indent--;
}

void ExtOutIndent ()
{
    for (DWORD i = 0; i < g_Indent; i++)
        g_ExtControl->Output (DEBUG_OUTPUT_NORMAL, EXT_INDENT_STRING);
}

// Normal output.
void __cdecl
ExtOut(PCSTR Format, ...)
{
    va_list Args;
    
    va_start(Args, Format);
    ExtOutIndent ();
    g_ExtControl->OutputVaList(DEBUG_OUTPUT_NORMAL, Format, Args);
    va_end(Args);
}

// Error output.
void __cdecl
ExtErr(PCSTR Format, ...)
{
    va_list Args;
    
    va_start(Args, Format);
    g_ExtControl->OutputVaList(DEBUG_OUTPUT_ERROR, Format, Args);
    va_end(Args);
}

// Warning output.
void __cdecl
ExtWarn(PCSTR Format, ...)
{
    va_list Args;
    
    va_start(Args, Format);
    g_ExtControl->OutputVaList(DEBUG_OUTPUT_WARNING, Format, Args);
    va_end(Args);
}

// Verbose output.
void __cdecl
ExtVerb(PCSTR Format, ...)
{
    va_list Args;
    
    va_start(Args, Format);
    g_ExtControl->OutputVaList(DEBUG_OUTPUT_VERBOSE, Format, Args);
    va_end(Args);
}

#ifdef _DEBUG
// Extension debugging output.
void __cdecl
_ExtDbgOut(PCSTR Format, ...)
{
    va_list Args;
    
    va_start(Args, Format);
    if (g_bDbgOutput)
        g_ExtControl->OutputVaList(DEBUG_OUTPUT_NORMAL, Format, Args);
    va_end(Args);
}
#endif

BOOL IsMiniDumpFileNODAC();

extern "C"
HRESULT
CALLBACK
DebugExtensionInitialize(PULONG Version, PULONG Flags)
{
    IDebugClient *DebugClient;
    PDEBUG_CONTROL DebugControl;
    HRESULT Hr;

    *Version = DEBUG_EXTENSION_VERSION(1, 0);
    *Flags = 0;
    

    if ((Hr = DebugCreate(__uuidof(IDebugClient),
                          (void **)&DebugClient)) != S_OK)
    {
        return Hr;
    }
    if ((Hr = DebugClient->QueryInterface(__uuidof(IDebugControl),
                                              (void **)&DebugControl)) != S_OK)
    {
        return Hr;
    }

    ExtensionApis.nSize = sizeof (ExtensionApis);
    if ((Hr = DebugControl->GetWindbgExtensionApis64(&ExtensionApis)) != S_OK) {
        return Hr;
    }

    ExtQuery(DebugClient);
    if (IsMiniDumpFileNODAC())
    {
        ExtOut ("------------------------------------------------------------\n");
        ExtOut ("sos.dll needs a full memory dump for complete functionality.\n");
        ExtOut ("You can create one with .dump /ma <filename>\n");
        ExtOut ("------------------------------------------------------------\n");
    }
    ExtRelease();
    
    DebugControl->Release();
    DebugClient->Release();
    return S_OK;
}


extern "C"
void
CALLBACK
DebugExtensionNotify(ULONG Notify, ULONG64 /*Argument*/)
{
    //
    // The first time we actually connect to a target, get the page size
    //

    if ((Notify == DEBUG_NOTIFY_SESSION_ACCESSIBLE) && (!Connected))
    {
        IDebugClient *DebugClient;
        PDEBUG_DATA_SPACES DebugDataSpaces;
        PDEBUG_CONTROL DebugControl;
        HRESULT Hr;
        ULONG64 Page;

        if ((Hr = DebugCreate(__uuidof(IDebugClient),
                              (void **)&DebugClient)) == S_OK)
        {
            //
            // Get the page size and PAE enable flag
            //

            if ((Hr = DebugClient->QueryInterface(__uuidof(IDebugDataSpaces),
                                       (void **)&DebugDataSpaces)) == S_OK)
            {
                if ((Hr = DebugDataSpaces->ReadDebuggerData(
                    DEBUG_DATA_MmPageSize, &Page,
                    sizeof(Page), NULL)) == S_OK)
                {
                    PageSize = (ULONG)(ULONG_PTR)Page;
                }

                DebugDataSpaces->Release();
            }
            //
            // Get the architecture type.
            //

            if ((Hr = DebugClient->QueryInterface(__uuidof(IDebugControl),
                                                  (void **)&DebugControl)) == S_OK)
            {
                if ((Hr = DebugControl->GetActualProcessorType(
                    &TargetMachine)) == S_OK)
                {
                    Connected = TRUE;
                }
                ULONG Qualifier;
                if ((Hr = DebugControl->GetDebuggeeType(&g_TargetClass, &Qualifier)) == S_OK)
                {
                }

                DebugControl->Release();
            }

            DebugClient->Release();
        }
    }


    if (Notify == DEBUG_NOTIFY_SESSION_INACTIVE)
    {
        Connected = FALSE;
        PageSize = 0;
        TargetMachine = 0;
    }

    return;
}

extern "C"
void
CALLBACK
DebugExtensionUninitialize(void)
{
    return;
}


DllInit(
    HANDLE /*hModule*/,
    DWORD  dwReason,
    DWORD  /*dwReserved*/
    )
{
    switch (dwReason) {
        case DLL_THREAD_ATTACH:
            break;

        case DLL_THREAD_DETACH:
            break;

        case DLL_PROCESS_DETACH:
            break;

        case DLL_PROCESS_ATTACH:
            break;
    }

    return TRUE;
}

