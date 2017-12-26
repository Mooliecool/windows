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
//+------------------------------------------------------------------------
//
//  Declare DLL entry points for Cor API to threadpool
//
//-------------------------------------------------------------------------

#define DllExportOrImport extern "C" 

typedef VOID (__stdcall *WAITORTIMERCALLBACK)(PVOID, BOOL); 

DllExportOrImport  BOOL __cdecl CorRegisterWaitForSingleObject(PHANDLE phNewWaitObject,
                                                      HANDLE hWaitObject,
                                                      WAITORTIMERCALLBACK Callback,
                                                      PVOID Context,
                                                      ULONG timeout,
                                                      BOOL  executeOnlyOnce );



DllExportOrImport BOOL __cdecl CorUnregisterWait(HANDLE hWaitObject,HANDLE CompletionEvent);

DllExportOrImport BOOL __cdecl CorQueueUserWorkItem(LPTHREAD_START_ROUTINE Function,
                                          PVOID Context,
                                          BOOL executeOnlyOnce );


DllExportOrImport BOOL __cdecl CorCreateTimer(PHANDLE phNewTimer,
                                     WAITORTIMERCALLBACK Callback,
                                     PVOID Parameter,
                                     DWORD DueTime,
                                     DWORD Period);

DllExportOrImport BOOL __cdecl CorChangeTimer(HANDLE Timer,
                                              ULONG DueTime,
                                              ULONG Period);

DllExportOrImport BOOL __cdecl CorDeleteTimer(HANDLE Timer,
                                              HANDLE CompletionEvent);

DllExportOrImport  VOID __cdecl CorBindIoCompletionCallback(HANDLE fileHandle, LPOVERLAPPED_COMPLETION_ROUTINE callback); 


DllExportOrImport  VOID __cdecl CorDoDelegateInvocation(int cookie); 
