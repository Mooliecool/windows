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
** Header:  ShimLoad.hpp
**
** Purpose: Delay load hook used to images to bind to 
**          dll's shim shipped with the EE
**
** Date:    April 4, 2000
**
===========================================================*/
#ifndef _SHIMLOAD_H
#define _SHIMLOAD_H


extern WCHAR g_wszDelayLoadVersion[64];

//*****************************************************************************
// Sets/Gets the directory based on the location of the module. This routine
// is called at COR setup time. Set is called during EEStartup and by the 
// MetaData dispenser.
//*****************************************************************************
HRESULT SetInternalSystemDirectory();
HRESULT GetInternalSystemDirectory(__out_ecount_opt(*pdwLength) LPWSTR buffer, __inout DWORD* pdwLength);

#endif

