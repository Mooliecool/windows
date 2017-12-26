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
//*****************************************************************************
// Debug.cpp
//
// Helper code for debugging.
//*****************************************************************************
#include "stdafx.h"
#include "utilcode.h"
#include "ex.h"

#ifdef _DEBUG
#define LOGGING
#endif


#include "log.h"


// Global state counter to implement SUPPRESS_ALLOCATION_ASSERTS_IN_THIS_SCOPE.
LONG g_DbgSuppressAllocationAsserts = 0;


#ifdef _DEBUG

int LowResourceMessageBoxHelperAnsi(
                  LPCSTR szText,    // Text message
                  LPCSTR szTitle,   // Title
                  UINT uType);      // Style of MessageBox


// On windows, we need to set the MB_SERVICE_NOTIFICATION bit on message
//  boxes, but that bit isn't defined under windows CE.  This bit of code
//  will provide '0' for the value, and if the value ever is defined, will
//  pick it up automatically.
 # define COMPLUS_MB_SERVICE_NOTIFICATION MB_SERVICE_NOTIFICATION


//*****************************************************************************
// This struct tracks the asserts we want to ignore in the rest of this
// run of the application.
//*****************************************************************************
struct _DBGIGNOREDATA
{
    char        rcFile[_MAX_PATH];
    long        iLine;
    bool        bIgnore;
};

typedef CDynArray<_DBGIGNOREDATA> DBGIGNORE;
static BYTE grIgnoreMemory[sizeof(DBGIGNORE)];
inline DBGIGNORE* GetDBGIGNORE()
{
    STATIC_CONTRACT_NOTHROW;
    STATIC_CONTRACT_GC_NOTRIGGER;
    
    static bool fInit; // = false;
    if (!fInit)
    {
        SCAN_IGNORE_THROW; // Doesn't really throw here.
        new (grIgnoreMemory) CDynArray<_DBGIGNOREDATA>();
        fInit = true;
    }

    return (DBGIGNORE*)grIgnoreMemory;
}

// Continue the app on an assert. Still output the assert, but
// Don't throw up a GUI. This is useful for testing fatal error 
// paths (like FEEE) where the runtime asserts.
BOOL ContinueOnAssert()
{
    STATIC_CONTRACT_NOTHROW;
    STATIC_CONTRACT_GC_NOTRIGGER;
    STATIC_CONTRACT_SO_TOLERANT;
    
    static ConfigDWORD fNoGui;
    return fNoGui.val(L"ContinueOnAssert", 0);
}

BOOL NoGuiOnAssert()
{
    STATIC_CONTRACT_NOTHROW;
    STATIC_CONTRACT_GC_NOTRIGGER;
    STATIC_CONTRACT_SO_TOLERANT;
    
    static ConfigDWORD fNoGui;
    return fNoGui.val(L"NoGuiOnAssert", 0);
}

BOOL DebugBreakOnAssert()
{
    STATIC_CONTRACT_NOTHROW;
    STATIC_CONTRACT_GC_NOTRIGGER;
    STATIC_CONTRACT_FORBID_FAULT;
    
    BOOL fRet = FALSE;
    
#ifndef DACCESS_COMPILE  
    static ConfigDWORD fDebugBreak;
    //
    // we don't want this config key to affect mscordacwks as well!
    //
    EX_TRY
    {
        fRet = fDebugBreak.val(L"DebugBreakOnAssert", 0);
    }
    EX_CATCH
    {
    }
    EX_END_CATCH(SwallowAllExceptions);
#endif // DACCESS_COMPILE

    return fRet;
}

VOID TerminateOnAssert()
{
    STATIC_CONTRACT_NOTHROW;
    STATIC_CONTRACT_GC_NOTRIGGER;
    
    ShutdownLogging();
    TerminateProcess(GetCurrentProcess(), 123456789);
}


VOID LogAssert(
    LPCSTR      szFile,
    int         iLine,
    LPCSTR      szExpr
)
{
    STATIC_CONTRACT_NOTHROW;
    STATIC_CONTRACT_GC_NOTRIGGER;
    STATIC_CONTRACT_SO_TOLERANT;

    

    // Log asserts to the stress log. Note that we can't include the szExpr b/c that 
    // may not be a string literal (particularly for formatt-able asserts).
    STRESS_LOG2(LF_ASSERT, LL_ALWAYS, "ASSERT:%s, line:%d\n", szFile, iLine);
        
    SYSTEMTIME st;
    GetSystemTime(&st);

    WCHAR exename[300];
    WszGetModuleFileName(NULL, exename, sizeof(exename)/sizeof(WCHAR));

    LOG((LF_ASSERT,
         LL_FATALERROR,
         "FAILED ASSERT(PID %d [0x%08x], Thread: %d [0x%x]) (%lu/%lu/%lu: %02lu:%02lu:%02lu %s): File: %s, Line %d : %s\n",
         GetCurrentProcessId(),
         GetCurrentProcessId(),
         GetCurrentThreadId(),
         GetCurrentThreadId(),
         (ULONG)st.wMonth,
         (ULONG)st.wDay,
         (ULONG)st.wYear,
         1 + (( (ULONG)st.wHour + 11 ) % 12),
         (ULONG)st.wMinute,
         (ULONG)st.wSecond,
         (st.wHour < 12) ? "am" : "pm",
         szFile,
         iLine,
         szExpr));
    LOG((LF_ASSERT, LL_FATALERROR, "RUNNING EXE: %ws\n", exename));
}

//*****************************************************************************

BOOL LaunchJITDebugger()
{
    STATIC_CONTRACT_NOTHROW;
    STATIC_CONTRACT_GC_NOTRIGGER;

    BOOL fSuccess = FALSE;
    

    return fSuccess;
}


//*****************************************************************************
// This function is called in order to ultimately return an out of memory
// failed hresult.  But this guy will check what environment you are running
// in and give an assert for running in a debug build environment.  Usually
// out of memory on a dev machine is a bogus alloction, and this allows you
// to catch such errors.  But when run in a stress envrionment where you are
// trying to get out of memory, assert behavior stops the tests.
//*****************************************************************************
HRESULT _OutOfMemory(LPCSTR szFile, int iLine)
{
    STATIC_CONTRACT_NOTHROW;
    STATIC_CONTRACT_GC_NOTRIGGER;
    
    DbgWriteEx(L"WARNING:  Out of memory condition being issued from: %hs, line %d\n",
            szFile, iLine);
    return (E_OUTOFMEMORY);
}

int _DbgBreakCount = 0;
static const char * szLowMemoryAssertMessage = "Assert failure (unable to format)";

//*****************************************************************************
// This function will handle ignore codes and tell the user what is happening.
//*****************************************************************************
int _DbgBreakCheck(
    LPCSTR      szFile,
    int         iLine,
    LPCSTR      szExpr, 
    BOOL        fConstrained)
{
    STATIC_CONTRACT_THROWS;
    STATIC_CONTRACT_GC_NOTRIGGER;
    STATIC_CONTRACT_FORBID_FAULT;

    if (DebugBreakOnAssert())
    {
        DebugBreak();
    }

    DBGIGNORE* pDBGIFNORE = GetDBGIGNORE();
    _DBGIGNOREDATA *psData = NULL;
    long        i;
    // Check for ignore all.
    for (i=0, psData = pDBGIFNORE->Ptr();  i<pDBGIFNORE->Count();  i++, psData++)
    {
        if (psData->iLine == iLine && SString::_stricmp(psData->rcFile, szFile) == 0 &&
            psData->bIgnore == true)
            return (false);
    }

    CONTRACT_VIOLATION(FaultNotFatal | GCViolation);
    
    SString debugOutput;
    SString dialogOutput;
    SString modulePath;
    SString dialogTitle;
    SString dialogIgnoreMessage;
    BOOL formattedMessages = FALSE;
    
    // If we are low on memory we cannot even format a message. If this happens we want to
    // contain the exception here but display as much information as we can about the exception.
    if (!fConstrained) 
    {
        EX_TRY
        {
            ClrGetModuleFileName(0, modulePath);
            debugOutput.Printf(L"Assert failure(PID %d [0x%08x], Thread: %d [0x%x]): %hs\n"
                L"    File: %hs, Line: %d Image:\n",
                GetCurrentProcessId(), GetCurrentProcessId(),
                GetCurrentThreadId(), GetCurrentThreadId(),
                szExpr, szFile, iLine);
            debugOutput.Append(modulePath);
            debugOutput.Append(L"\n");
            
            // Change format for message box.  The extra spaces in the title
            // are there to get around format truncation.
            dialogOutput.Printf(L"%hs\n\n%hs, Line: %d\n\nAbort - Kill program\nRetry - Debug\nIgnore - Keep running\n"
                L"\n\nImage:\n", szExpr, szFile, iLine);
            dialogOutput.Append(modulePath);
            dialogOutput.Append(L"\n");
            dialogTitle.Printf(L"Assert Failure (PID %d, Thread %d/%x)        ",
                GetCurrentProcessId(), GetCurrentThreadId(), GetCurrentThreadId());
            
            dialogIgnoreMessage.Printf(L"Ignore the assert for the rest of this run?\nYes - Assert will never fire again.\nNo - Assert will continue to fire.\n\n%hs\nLine: %d\n",
                szFile, iLine);
            
            formattedMessages = TRUE;
        }
        EX_CATCH
        {            
        }
        EX_END_CATCH(SwallowAllExceptions);
    }
    
    // Emit assert in debug output and console for easy access.
    if (formattedMessages) 
    {
        WszOutputDebugString(debugOutput);
        wprintf(debugOutput);    
    }
    else {
        // Note: we cannot convert to unicode or concatenate in this situation.
        OutputDebugStringA(szLowMemoryAssertMessage);        
        OutputDebugStringA("\n");
        OutputDebugStringA(szFile);        
        OutputDebugStringA("\n");
        OutputDebugStringA(szExpr);        
        OutputDebugStringA("\n");
        printf(szLowMemoryAssertMessage);
        printf("\n");
        printf(szFile);
        printf("\n");
        printf(szExpr);
        printf("\n");
    }

    LogAssert(szFile, iLine, szExpr);
    FlushLogging();         // make certain we get the last part of the log
    fflush(stdout);

    if (ContinueOnAssert())
    {
        return false; // don't stop debugger. No gui.
    }

    if (NoGuiOnAssert())
    {
        TerminateOnAssert();
    }

    if (DebugBreakOnAssert())
    {
        return(true);       // like a retry
    }

#ifdef DACCESS_COMPILE
    // In the dac case asserts are only informational,
    // not terminal, as the target may not be in an
    // assertable state if it's stopped at a random
    // point in the debugger.
    {
        static ConfigDWORD fEnableDACAsserts;
        if (!fEnableDACAsserts.val(L"DbgDACEnableAssert", 0))
        {
            return true;
        }
    }
#endif // #ifndef DACCESS_COMPILE

    // Tell user there was an error.
    _DbgBreakCount++;
    int ret;
    if (formattedMessages) {
        ret = UtilMessageBoxCatastrophicNonLocalized(
            dialogOutput, dialogTitle, MB_ABORTRETRYIGNORE | MB_ICONEXCLAMATION, TRUE);
    }
    else 
    {   
        ret = LowResourceMessageBoxHelperAnsi(
            szExpr, szLowMemoryAssertMessage, MB_ABORTRETRYIGNORE | MB_ICONEXCLAMATION);
    }
    --_DbgBreakCount;

    HMODULE hKrnl32;

    switch(ret)
    {
        // For abort, just quit the app.
        case IDABORT:
          TerminateProcess(GetCurrentProcess(), 1);
//        WszFatalAppExit(0, L"Shutting down");
        break;

        // Tell caller to break at the correct loction.
        case IDRETRY:

        hKrnl32 = WszLoadLibrary(L"kernel32.dll");
        _ASSERTE(hKrnl32 != NULL);

        if(hKrnl32)
        {
            typedef BOOL (WINAPI *t_pDbgPres)();
            t_pDbgPres pFcn = (t_pDbgPres) GetProcAddress(hKrnl32, "IsDebuggerPresent");

            // If this function is available, use it.
            if (pFcn)
            {
                if (pFcn())
                {
                    SetErrorMode(0);
                }
                else
                    LaunchJITDebugger();
            }

            FreeLibrary(hKrnl32);
        }

        return (true);

        // If we want to ignore the assert, find out if this is forever.
        case IDIGNORE:
        if (formattedMessages) 
        {
            if (UtilMessageBoxCatastrophicNonLocalized(
                                   dialogIgnoreMessage, 
                                   L"Ignore Assert Forever?", 
                                   MB_ICONQUESTION | MB_YESNO, 
                                   TRUE) != IDYES)
            {
                break;
            }
        }
        else 
        {
            if (LowResourceMessageBoxHelperAnsi(
                                   "Ignore the assert for the rest of this run?\nYes - Assert will never fire again.\nNo - Assert will continue to fire.\n",
                                   "Ignore Assert Forever?",
                                   MB_ICONQUESTION | MB_YESNO) != IDYES) 
            {
            break;
            }                                  
        }
        if ((psData = pDBGIFNORE->Append()) == 0)
            return (false);
        psData->bIgnore = true;
        psData->iLine = iLine;
        strcpy(psData->rcFile, szFile);
        break;
    }

    return (false);
}

int _DbgBreakCheckNoThrow(
    LPCSTR      szFile,
    int         iLine,
    LPCSTR      szExpr, 
    BOOL        fConstrained)
{
    STATIC_CONTRACT_NOTHROW;
    STATIC_CONTRACT_GC_NOTRIGGER;
    STATIC_CONTRACT_FORBID_FAULT;

    if (DebugBreakOnAssert())
    {
        DebugBreak();
    }

    int failed = false;
    int result = false;
    EX_TRY
    {
        result = _DbgBreakCheck(szFile, iLine, szExpr, fConstrained);
    }
    EX_CATCH
    {
        failed = true;   
    }
    EX_END_CATCH(SwallowAllExceptions);
    
    if (failed == TRUE)
    {
        return true;
    }
    return result;
}
    

    // Get the timestamp from the PE file header.  This is useful
unsigned DbgGetEXETimeStamp()
{
    STATIC_CONTRACT_NOTHROW;
    STATIC_CONTRACT_GC_NOTRIGGER;
    
    static ULONG cache = 0;


    return cache;
}


#if !defined(NO_CRT)

#include <math.h>

#define MAX_BUCKETS_MATH 160

double Binomial (DWORD K, DWORD M, DWORD N)
{
    STATIC_CONTRACT_LEAF;
    
    if (K >= MAX_BUCKETS_MATH)
        return -1 ;

    static double rgKFact [MAX_BUCKETS_MATH] ;
    DWORD i ;

    if (rgKFact[0] == 0)
    {
        rgKFact[0] = 1 ;
        for (i=1; i<MAX_BUCKETS_MATH; i++)
            rgKFact[i] = rgKFact[i-1] * i ;
    }

    double MchooseK = 1 ;

    for (i = 0; i < K; i++)
        MchooseK *= (M - i) ;

    MchooseK /= rgKFact[K] ;

    double OneOverNToTheK = pow (1./N, K) ;

    double QToTheMMinusK = pow (1.-1./N, M-K) ;

    double P = MchooseK * OneOverNToTheK * QToTheMMinusK ;

    return N * P ;
}

#endif // !NO_CRT

// Called from within the IfFail...() macros.  Set a breakpoint here to break on
// errors.
VOID DebBreak()
{
  STATIC_CONTRACT_LEAF;
  static int i = 0;  // add some code here so that we'll be able to set a BP
  i++;
}


VOID DebBreakHr(HRESULT hr)
{
  STATIC_CONTRACT_LEAF;
  static int i = 0;  // add some code here so that we'll be able to set a BP
  _ASSERTE(hr != (HRESULT) 0xcccccccc);
  i++;

  #ifdef _DEBUG
  static DWORD dwBreakHR = 99;

  if (dwBreakHR == 99)
         dwBreakHR = REGUTIL::GetConfigDWORD(L"BreakOnHR", 0);
  if (dwBreakHR == (DWORD)hr)
  {
    _DbgBreak();
  }
  #endif
}

CHAR g_szExprWithStack2[10480];
void *dbgForceToMemory;     // dummy pointer that pessimises enregistration


VOID DbgAssertDialog(const char *szFile, int iLine, const char *szExpr)
{
    STATIC_CONTRACT_NOTHROW;
    STATIC_CONTRACT_GC_NOTRIGGER;
    STATIC_CONTRACT_FORBID_FAULT;

    DEBUG_ONLY_FUNCTION;
    

    if (g_DbgSuppressAllocationAsserts > 16) 
        DebugBreak();

    SUPPRESS_ALLOCATION_ASSERTS_IN_THIS_SCOPE;

    dbgForceToMemory = &szFile;     //make certain these args are available in the debugger
    dbgForceToMemory = &iLine;
    dbgForceToMemory = &szExpr;

    if (DebugBreakOnAssert())
    {
        DebugBreak();
    }

    BOOL fConstrained = FALSE;

    if (1 == _DbgBreakCheckNoThrow(szFile, iLine, szExpr, fConstrained))
        _DbgBreak();
}

//-----------------------------------------------------------------------------
// Returns an a stacktrace for a given context. 
// Very useful inside exception filters.
// Returns true if successful, false on failure (such as OOM).
// This never throws.
//-----------------------------------------------------------------------------
bool GetStackTraceAtContext(SString & s, CONTEXT * pContext)
{    
    SUPPRESS_ALLOCATION_ASSERTS_IN_THIS_SCOPE;

    // NULL means use the current context.
    bool fSuccess = false;


    return fSuccess;
}

#endif // _DEBUG

// This helper will throw up a message box without allocating or using stack if possible, and is
// appropriate for either low memory or low stack situations.
int LowResourceMessageBoxHelperAnsi(
                  LPCSTR szText,    // Text message
                  LPCSTR szTitle,   // Title
                  UINT uType)       // Style of MessageBox
{
    CONTRACTL
    {
        NOTHROW;
        INJECT_FAULT(return IDCANCEL;);
    }
    CONTRACTL_END;

    int result = IDCANCEL;
    
    // In low memory or stack constrained code we cannot format or convert strings, so use the
    // ANSI version except for Rotor
    EX_TRY
    {
        SString text;
        SString title; 
        text.Printf(L"%hs", szText);
        title.Printf(L"%hs", szTitle);
        LPWSTR wstrText = (LPWSTR)text.GetUnicode();
        LPWSTR wstrTitle = (LPWSTR)title.GetUnicode();
        result = UtilMessageBoxCatastrophicNonLocalized(wstrText, wstrTitle, uType, FALSE);
    }
    EX_CATCH
    {
        result = IDCANCEL;
    }
    EX_END_CATCH(SwallowAllExceptions);
    

    return result;            
}


/****************************************************************************
   The following two functions are defined to allow Free builds to call
   DebugBreak or to Assert with a stack trace for unexpected fatal errors.
   Typically these paths are enabled via a registry key in a Free Build
*****************************************************************************/

VOID __FreeBuildDebugBreak()
{
    WRAPPER_CONTRACT; // If we're calling this, we're well past caring about contract consistency!

    if (REGUTIL::GetConfigDWORD(L"BreakOnRetailAssert", 0))
    {
        DebugBreak();
    }
}

void *freForceToMemory;     // dummy pointer that pessimises enregistration

void __FreeBuildAssertFail(const char *szFile, int iLine, const char *szExpr)
{
    WRAPPER_CONTRACT; // If we're calling this, we're well past caring about contract consistency!

    freForceToMemory = &szFile;     //make certain these args are available in the debugger
    freForceToMemory = &iLine;
    freForceToMemory = &szExpr;

    __FreeBuildDebugBreak();

    SString buffer;
    SString modulePath;
    
    // Give assert in output for easy access.
    ClrGetModuleFileName(0, modulePath);
    buffer.Printf(L"CLR: Assert failure(PID %d [0x%08x], Thread: %d [0x%x]): %hs\n"
                L"    File: %hs, Line: %d Image:\n",
                GetCurrentProcessId(), GetCurrentProcessId(),
                GetCurrentThreadId(), GetCurrentThreadId(),
                szExpr, szFile, iLine);
    buffer.Append(modulePath);
    buffer.Append(L"\n");
    WszOutputDebugString(buffer);
    // Write out the error to the console
    wprintf(buffer);


    // Log to the stress log. Note that we can't include the szExpr b/c that 
    // may not be a string literal (particularly for formatt-able asserts).
    STRESS_LOG2(LF_ASSERT, LL_ALWAYS, "ASSERT:%s, line:%d\n", szFile, iLine);

    FlushLogging();         // make certain we get the last part of the log

    fflush(stdout);

    //    TerminateOnAssert();
    ShutdownLogging();
    TerminateProcess(GetCurrentProcess(), 123456789);
}

//===================================================================================
// Used by the ex.h macro: EX_CATCH_HRESULT_AND_NGEN_CLEAN(_hr)
// which is used by ngen and mscorsvc to catch unexpected HRESULT
// from one of the RPC calls.
//===================================================================================
void RetailAssertIfExpectedClean()
{
    static ConfigDWORD g_NGenClean;
    if (g_NGenClean.val(L"NGenClean", 0) == 1) 
    {
        _ASSERTE_ALL_BUILDS(!"Error during NGen:  expected no exceptions to be thrown");
    }
}

