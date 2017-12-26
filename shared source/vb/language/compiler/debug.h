//-------------------------------------------------------------------------------------------------
//
//  Copyright (c) Microsoft Corporation.  All rights reserved.
//
//  Macros and helpers for the debug build.
//
//-------------------------------------------------------------------------------------------------

#pragma once

// Setup to use VSAssert as our Assert machanism
#if DEBUG
#define _ASSERTE(expr) VSASSERT(expr, "")
#define ASSERT(expr, comment)  VSASSERT(expr, comment)
#else
#define _ASSERTE(expr)
#define ASSERT(expr, comment)
#endif

// Setup to be able to add Debug Option Switches

#if ID_TEST

//---------------------------------------------------------------------
// Start the debug information.
//---------------------------------------------------------------------

struct IVsTshell;

HRESULT DebInit(IVsTshell *ptshell);

void DebTerm();

// This routine is needed for test.
void __cdecl DebTShellPrintf(
    const char * szFmt,
    ...);

void __cdecl DebPrintf(
    const char * szFmt,
    ...);

void __cdecl DebPrintf(
    const WCHAR * szFmt,
    ...);

void __cdecl DebVPrintf(
    const WCHAR *,
    va_list);

void DebDoPrint(_In_z_ char *sz);

#else // !ID_TEST

#define DebInit(pSP)
#define DebTerm()

#endif // !ID_TEST


#if DEBUG
// This is used by the IfFailRet and other macros - do not use directly.
HRESULT DebTraceReturn(HRESULT hr, _In_z_ const WCHAR *szFile, int iLine);

void
#if HOSTED
    __cdecl
#endif
        DebCheckForResourceLeaks();

bool DebVPAssert();
#else
#define DebCheckForResourceLeaks()
#endif

//
// TEST SWITCHES
//
// To add a test switch, see debug.cpp
//

#if ID_TEST
    #if DEBUG
        #define VBEXTERN_TESTSWITCH(NAME)           VSEXTERN_SWITCH(NAME)
        #define VBFTESTSWITCH(NAME)                 VSFSWITCH(NAME)
        #define DBG_SWITCH_PRINTF(NAME, MESSAGE, ...)            if (VSFSWITCH(NAME)) { DebPrintf(MESSAGE, __VA_ARGS__);}
    #else  // DEBUG
        #define VBEXTERN_TESTSWITCH(NAME)           extern bool g_Switch_ ## NAME
        #define VBFTESTSWITCH(NAME)                 g_Switch_ ## NAME
        #define DBG_SWITCH_PRINTF(NAME, MESSAGE, ...)
    #endif  // DEBUG
#else  // ID_TEST
    #define VBEXTERN_TESTSWITCH(NAME)
    #define VBFTESTSWITCH(NAME)                 FALSE
    #define DBG_SWITCH_PRINTF(NAME, MESSAGE, ...)
#endif // ID_TEST

VSEXTERN_SWITCH(fDebugEE);
VSEXTERN_SWITCH(fDebugEEFields);
VSEXTERN_SWITCH(fDumpBoundEETrees);
VSEXTERN_SWITCH(fDumpCallGraph);
VSEXTERN_SWITCH(fDumpDeclTrees);
VSEXTERN_SWITCH(fDumpDocCommentString);
VSEXTERN_SWITCH(fDumpLineTable);
VSEXTERN_SWITCH(fDumpSyntheticCode);
VSEXTERN_SWITCH(fTraceCodeElementsInt);
VSEXTERN_SWITCH(fTraceCodeElementsExt);
VSEXTERN_SWITCH(fTraceCodeElementsSpit);
VSEXTERN_SWITCH(fTraceCodeModelEvents);
VSEXTERN_SWITCH(fTraceCodeModelEventsExt);
VSEXTERN_SWITCH(fTraceCodeModelInt);
VSEXTERN_SWITCH(fTraceCodeTypeRef);
VSEXTERN_SWITCH(fTraceCollectionsInt);
VSEXTERN_SWITCH(fTraceFileCodeModelInt);
VSEXTERN_SWITCH(fDumpBoundMethodTrees);
VSEXTERN_SWITCH(fDumpUnboundMethodTrees);
VSEXTERN_SWITCH(fDumpText);
VSEXTERN_SWITCH(fDumpStateChanges);
VSEXTERN_SWITCH(fDebugIntellidocCache);
VSEXTERN_SWITCH(fDumpSymbols);
VSEXTERN_SWITCH(fDumpNonVBSymbols);
VSEXTERN_SWITCH(fDumpBasicRep);
VSEXTERN_SWITCH(fDumpClosures);
VSEXTERN_SWITCH(fDumpInference);
VSEXTERN_SWITCH(fDumpOverload);
VSEXTERN_SWITCH(fDumpRelaxedDel);
VSEXTERN_SWITCH(fTraceReturn);
VSEXTERN_SWITCH(fDebugEmptyEdit);
VSEXTERN_SWITCH(fDebugEditClassify);
VSEXTERN_SWITCH(fDebugEditFilter);
VSEXTERN_SWITCH(fDebugEnC);
VSEXTERN_SWITCH(fEnableIntellisenseBuilders);
VSEXTERN_SWITCH(fDebugNewSpanTracking);
VSEXTERN_SWITCH(fDebugOldSpanTracking);
VSEXTERN_SWITCH(fDumpPrettyListMinDistanceStats);
VSEXTERN_SWITCH(fDumpPrettyListMinDistanceArray);
VSEXTERN_SWITCH(fDebugSourceFileView);
VSEXTERN_SWITCH(fDebugTaskList);
VSEXTERN_SWITCH(fDebugSmartTagHover);
VSEXTERN_SWITCH(fDebugLocks);
VSEXTERN_SWITCH(fTraceHiddenText);
VSEXTERN_SWITCH(fTraceHiddenTextEvents);
VSEXTERN_SWITCH(fDebugXMLDocParser);
VSEXTERN_SWITCH(fDumpReferenceUsage);
VSEXTERN_SWITCH(fDumpImportUsage);
VSEXTERN_SWITCH(fDumpReferencesForMetaData);
VSEXTERN_SWITCH(fDumpXML);
VSEXTERN_SWITCH(fDumpFlow);
VSEXTERN_SWITCH(fKeepECWindowVisibleOnFocusLoss);
VSEXTERN_SWITCH(fDebugFindSymbolReference);
VSEXTERN_SWITCH(fTestCrash);
VSEXTERN_SWITCH(fDebugNoMy);
VSEXTERN_SWITCH(fENCDumpMethodLocations);
VSEXTERN_SWITCH(fENCShowHiddenMarkers);
VSEXTERN_SWITCH(fENCDumpLineMap);
VSEXTERN_SWITCH(fENCDumpStatements);
VSEXTERN_SWITCH(fENCDumpDeltas);
VSEXTERN_SWITCH(fENCDumpMetaTokens);
VSEXTERN_SWITCH(fEEShowHiddenLocals);
VSEXTERN_SWITCH(fExceptionNoRethrowNoWatson);
VSEXTERN_SWITCH(fDebugIDEExtensionMethodCacheTestHook);
VSEXTERN_SWITCH(fENCDebugTransientCaching);
VBEXTERN_TESTSWITCH(fDumpDecompiles);
VBEXTERN_TESTSWITCH(fDumpDecompilesForSuites);
VBEXTERN_TESTSWITCH(fDumpFileChangeNotifications);
VBEXTERN_TESTSWITCH(fIDEDumpXML);
VBEXTERN_TESTSWITCH(fDebugLineMark);
VBEXTERN_TESTSWITCH(fDebugEditCacheInOnReplace);
VBEXTERN_TESTSWITCH(fMyGroupAndDefaultInst);
VBEXTERN_TESTSWITCH(fDumpSnippetMarkerInfo);
VSEXTERN_SWITCH(fDumpIntellisenseDropdown);
VSEXTERN_SWITCH(fDumpIntellisenseEvents);
VSEXTERN_SWITCH(fShowSnippetBounds);
VSEXTERN_SWITCH(fThreading);  // foreground /compilerbackground thread
VSEXTERN_SWITCH(fCompCaches);  // Statistics about Compilation Cache 

