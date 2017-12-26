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

#ifndef __VMHOLDER_H_
#define __VMHOLDER_H_

#include "holder.h"

template <typename TYPE> 
inline void DoTheReleaseHost(TYPE *value)
{
    if (value)
    {
        BEGIN_SO_TOLERANT_CODE_CALLING_HOST(GetThread());
        value->Release();
        END_SO_TOLERANT_CODE_CALLING_HOST;

    }
}
NEW_HOLDER_TEMPLATE1(HostComHolder, typename TYPE, Wrapper,
                     TYPE*, DoNothing<TYPE*>, DoTheReleaseHost<TYPE>, NULL);


#endif
