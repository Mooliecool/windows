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

#ifndef _FACTORY_INL_
#define _FACTORY_INL_

#include "factory.h"

template<typename PRODUCT, DWORD MAX_FACTORY_PRODUCT>
PRODUCT* InlineFactory<PRODUCT, MAX_FACTORY_PRODUCT>::Create()
{
    WRAPPER_CONTRACT;

    if (m_cProduct == MAX_FACTORY_PRODUCT) 
        return GetNext()->Create();

    return &m_product[m_cProduct++];
}

#endif

