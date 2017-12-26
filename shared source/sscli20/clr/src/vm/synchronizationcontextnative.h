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
** Header: SynchronizationContextNative.h
**
** Purpose: Native methods on System.Threading.SynchronizationContext.
**
** Date:  September 19th, 2003
**
===========================================================*/

#ifndef _SYNCHRONIZATIONCONTEXTNATIVE_H
#define _SYNCHRONIZATIONCONTEXTNATIVE_H

class SynchronizationContextNative
{
public:    
    static FCDECL3(DWORD, WaitHelper, PTRArray *handleArrayUNSAFE, CLR_BOOL waitAll, DWORD millis);
};

#endif // _SYNCHRONIZATIONCONTEXTNATIVE_H

