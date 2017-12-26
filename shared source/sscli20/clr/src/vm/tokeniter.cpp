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

#include "common.h"
#include "tokeniter.hpp"


SigPtrInstIterator::SigPtrInstIterator(Module* pModule, mdToken token)
{
    PCCOR_SIGNATURE pSig;
    ULONG cSig;
    m_MICount=0;
    m_current=0;
    switch(TypeFromToken(token))
    {
    case mdtMemberRef:
        {
            mdToken parentTok;
            parentTok=pModule->GetMDImport()->GetParentOfMemberRef(token);
            if (TypeFromToken(parentTok)==mdtTypeSpec)
            {
                pModule->GetMDImport()->GetTypeSpecFromToken(parentTok,&pSig,&cSig);
                SetClassInstSig(pSig);
            }
        }
        break;
    case mdtTypeSpec:
        {
          pModule->GetMDImport()->GetTypeSpecFromToken(token,&pSig,&cSig);
            SetClassInstSig(pSig);
        }
        break;
    case mdtMethodSpec:
        {
            mdMemberRef memberRef;
            pModule->GetMDImport()->GetMethodSpecProps(token,&memberRef,&pSig,&cSig);
            SetMethodInstSig(pSig);    
            if (TypeFromToken(memberRef)==mdtMemberRef)
            {
                mdToken parentTok=pModule->GetMDImport()->GetParentOfMemberRef(memberRef);
                if (TypeFromToken(parentTok)==mdtTypeSpec)
                {
                     pModule->GetMDImport()->GetTypeSpecFromToken(parentTok,&pSig,&cSig);            
                     SetClassInstSig(pSig);
                }
            }
        }
        
    }
    
}

BOOL SigPtrInstIterator::Next()
{

    if (m_current <m_MICount)
    {
        if (m_current++>0)
            m_methodInstSig.SkipExactlyOne();
        return TRUE;
    };
    return FALSE;
}

SigPointer SigPtrInstIterator::Current()
{
    _ASSERTE(m_current<=m_MICount);
    if (m_current==0)
        return m_classInstSig;
    else
        return m_methodInstSig;
}


