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
// CacheLoad.h
//
// Class for returning the memory image where the image lives
//
//*****************************************************************************
#ifndef __CACHELOAD__H__
#define __CACHELOAD__H__


#undef  INTERFACE   
#define INTERFACE ICacheLoad
DECLARE_INTERFACE_(ICacheLoad, IUnknown)
{
    STDMETHOD(GetCachedImaged)(
       LPVOID* pImage);

    STDMETHOD(SetCachedImaged)(
       LPVOID pImage);
};

#endif
