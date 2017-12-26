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
** Header:  AppDomain.cpp
**
** Purpose: Implements AppDomain (loader domain) architecture
**
** Date:  Dec 1, 1998
**
===========================================================*/
#ifndef _TOKENITERATORS_H
#define _TOKENITERATORS_H

class SigPtrInstIterator
{
    protected:
    SigPointer m_classInstSig;
    SigPointer m_methodInstSig;
    int    m_MICount;
    int    m_current;
    void SetClassInstSig(PCCOR_SIGNATURE pSig)
    {
        m_current=-1;
        m_classInstSig.SetSig(pSig);
    }
    void SetMethodInstSig(PCCOR_SIGNATURE pSig)
    {
        _ASSERT(*pSig==IMAGE_CEE_CS_CALLCONV_GENERICINST);
        pSig++;
        m_methodInstSig.SetSig(pSig);
        ULONG data;
        IfFailThrow(m_methodInstSig.GetData(&data));
        m_MICount = data;
    };    
public:
    SigPtrInstIterator(Module* pModule, mdToken token);
    BOOL Next();
    SigPointer Current();
};



#endif //_TOKENITERATORS_H
