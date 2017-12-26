//-------------------------------------------------------------------------------------------------
//
//  Copyright (c) Microsoft Corporation.  All rights reserved.
//
//  Macros to support the basic compiler libs
//
//-------------------------------------------------------------------------------------------------
#pragma once

//-------------------------------------------------------------------------------------------------
//
// Enabling the following conditional is very helpful for logging failures.  Virtually every entry
// point and exit point in the VB compiler is guarded by these macros.  Therefore they can easily
// be used to generate log files which contain the HR result of all calls.  See the macro'd out
// implementation in VbCompilerMacros.cpp for an example.
//
//-------------------------------------------------------------------------------------------------
#if 0
void VbMacroLogStart( _In_z_ const char* pFunctionName, _In_ HRESULT initialHr);
void VbMacroLogEnd( _In_z_ const char* pFunctionName, _In_ HRESULT finalHr);
#define VB_LOG_START() VbMacroLogStart( __FUNCTION__, hr)
#define VB_LOG_END() VbMacroLogEnd( __FUNCTION__, hr)
#else
#define VB_LOG_START() do{}while(0)
#define VB_LOG_END() do{}while(0)
#endif

#define VB_ENTRY()                  \
    HRESULT hr=S_OK;                \
    try{                            \
    VB_LOG_START()

#define VB_ENTRY1(initVal)          \
    HRESULT hr=initVal;             \
    try{                            \
    VB_LOG_START()

#define VB_ENTRY_NOHR()             \
    try{                            \
    VB_LOG_START()

#define VB_EXIT()                   \
    } catch ( Exception& ex) {      \
        hr = ex.GetHResult();       \
    } catch ( std::exception &) {   \
        hr = E_FAIL;                \
    }                               \
    VB_LOG_END();                   \
    return hr

#define VB_EXIT_LABEL()             \
    } catch ( Exception& ex) {      \
        hr = ex.GetHResult();       \
    } catch ( std::exception &) {   \
        hr = E_FAIL;                \
    }                               \
Error:                              \
    VB_LOG_END();                   \
    return hr

#define VB_EXIT_GO()                \
    } catch ( Exception& ex) {      \
        hr = ex.GetHResult();       \
        goto Error;                 \
    } catch ( std::exception &) {   \
        hr = E_FAIL;                \
        goto Error;                 \
    }                               \
    VB_LOG_END();                   \
    do{}while(0)                    

#define VB_EXIT_NORETURN()          \
    } catch ( Exception& ex) {      \
        hr = ex.GetHResult();       \
    } catch ( std::exception &) {   \
        hr = E_FAIL;                \
    }                               \
    VB_LOG_END();                   \
    do{}while(0)                    

#define VB_EXIT_LABEL_NORETURN()    \
    } catch ( Exception& ex) {      \
        hr = ex.GetHResult();       \
    } catch ( std::exception &) {   \
        hr = E_FAIL;                \
    }                               \
Error:                              \
    VB_LOG_END();                   \
    do{}while(0)

#if IDE 
// Any unsafe entry points in our code must ensure our IDE code is NOT
// already on the stack. If you are hitting the throw below you have
// a re-entrancy bug to investigate.
#define VB_VERIFY_NOT_REENTRANT()                                           \
    int *pnVBCodeONStackTlsValue =VBTLSThreadData::GetVBCodeOnStack();      \
    if (*pnVBCodeONStackTlsValue  > 0)                                      \
    {                                                                       \
        int *pnReentracyMode = VBTLSThreadData::GetReentrancyMode();        \
        if (*pnReentracyMode > 0)                                           \
        {                                                                   \
            RRETURN(E_FAIL);                                                \
        }                                                                   \
        else                                                                \
        {                                                                   \
            VSFAIL("VB IDE REENTRANCY IS DETECTED. PLEASE OPEN A BUG.");    \
            VbThrow(E_FAIL);                                                \
       }                                                                    \
    }                                                                       \
    BackupValue<int> _backup(pnVBCodeONStackTlsValue);                      \
    if (GetCompilerPackage())                                               \
    {                                                                       \
        (*pnVBCodeONStackTlsValue) += 1;                                    \
    } 

#define VB_SET_REENTRANCY_RETURN_RETRYLATER()                               \
    int *pnReentracyMode = VBTLSThreadData::GetReentrancyMode();            \
    BackupValue<int> _backup(pnReentracyMode);                              \
    if (GetCompilerPackage())                                               \
    {                                                                       \
        (*pnReentracyMode) += 1;                                            \
    } 

#define VB_VERIFY_NOT_REENTRANT_THROW()                                     \
    int *pnVBCodeONStackTlsValue =VBTLSThreadData::GetVBCodeOnStack();      \
    if (*pnVBCodeONStackTlsValue  > 0)                                      \
    {                                                                       \
        VSFAIL("VB IDE REENTRANCY IS DETECTED. PLEASE OPEN A BUG.");        \
        VbThrow(E_FAIL);                                                    \
    }                                                                       \
    BackupValue<int> _backup(pnVBCodeONStackTlsValue);                      \
    if (GetCompilerPackage())                                               \
    {                                                                       \
        (*pnVBCodeONStackTlsValue) += 1;                                    \
    } 

#define VB_EXTERNAL_ENTRY()                                                 \
    int *pnVBCodeONStackTlsValue =VBTLSThreadData::GetVBCodeOnStack();      \
    BackupValue<int> _backup(pnVBCodeONStackTlsValue);                      \
    if (GetCompilerPackage())                                               \
    {                                                                       \
        (*pnVBCodeONStackTlsValue) += 1;                                    \
    } 

// This macro is used when we are about to leave a section of 
// code which checks for re-entrancy and enter a block 
// which can call back into a different set of re-entrant guarded
// code.  Use this macro with care
#define VB_EXTERNAL_EXIT()                                                  \
    int *pnVBCodeONStackTlsValue =VBTLSThreadData::GetVBCodeOnStack();      \
    BackupValue<int> _backup(pnVBCodeONStackTlsValue);                      \
    ThrowIfFalse(*pnVBCodeONStackTlsValue != 0);                            \
    if (GetCompilerPackage())                                               \
    {                                                                       \
        (*pnVBCodeONStackTlsValue) = 0;                                     \
    } 

// Similar to VB_EXTERNAL_EXIT except we won't do a check to 
// ensure we are in a section currentyl.  There are several parts of 
// our code in which get called in several ways and it's hard
// to line up the entry point correctly.  This late in the cycle
// we're opting for this solution to prevent crashes for what appear
// to be benign scenarios
#define VB_EXTERNAL_MAYBE_EXIT()                        \
    int *pnVBCodeONStackTlsValue =VBTLSThreadData::GetVBCodeOnStack();      \
    BackupValue<int> _backup(pnVBCodeONStackTlsValue);                      \
    if (GetCompilerPackage())                                               \
    {                                                                       \
        (*pnVBCodeONStackTlsValue) = 0;                                     \
    } 


#else
    #define VB_VERIFY_NOT_REENTRANT()
    #define VB_EXTERNAL_ENTRY() 
    #define VB_EXTERNAL_EXIT()                              
#endif


