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

#ifndef __HOLDERINST_H_
#define __HOLDERINST_H_

// This file contains holder instantiations which we can't put in holder.h because
// the instantiations require _ASSERTE to be defined, which is not always the case
// for placed that include holder.h.

FORCEINLINE void SafeArrayRelease(SAFEARRAY* p)
{
    SafeArrayDestroy(p);
}


class SafeArrayHolder : public Wrapper<SAFEARRAY*, SafeArrayDoNothing, SafeArrayRelease, NULL>
{
public:
    SafeArrayHolder(SAFEARRAY* p = NULL)
        : Wrapper<SAFEARRAY*, SafeArrayDoNothing, SafeArrayRelease, NULL>(p)
    {
    }

    FORCEINLINE void operator=(SAFEARRAY* p)
    {
        Wrapper<SAFEARRAY*, SafeArrayDoNothing, SafeArrayRelease, NULL>::operator=(p);
    }
};

#endif  // __HOLDERINST_H_
