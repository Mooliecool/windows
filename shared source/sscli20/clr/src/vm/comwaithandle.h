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
/*============================================================
**
** Header: COMWaitHandle.h
**       
**
** Purpose: Native methods on System.WaitHandle
**
** Date:  August, 1999
**
===========================================================*/

#ifndef _COM_WAITABLE_HANDLE_H
#define _COM_WAITABLE_HANDLE_H


class WaitHandleNative
{
public:
    static FCDECL4(INT32, CorWaitOneNative, SafeHandle* safeWaitHandleUNSAFE, INT32 timeout, CLR_BOOL hasThreadAffinity, CLR_BOOL exitContext);
    static FCDECL4(INT32, CorWaitMultipleNative, Object* waitObjectsUNSAFE, INT32 timeout, CLR_BOOL exitContext, CLR_BOOL waitForAll);
};
#endif
