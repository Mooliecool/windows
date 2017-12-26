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

#ifndef _FACTORY_H_
#define _FACTORY_H_

template<typename PRODUCT>
class Factory
{
public:
    virtual PRODUCT* Create() = 0;
};

template<typename PRODUCT, DWORD MAX_FACTORY_PRODUCT = 64>
class InlineFactory : public Factory<PRODUCT>
{
public:
    InlineFactory() : m_next(NULL), m_cProduct(0) { WRAPPER_CONTRACT; }
    ~InlineFactory() { WRAPPER_CONTRACT; if (m_next) delete m_next; } 
    PRODUCT* Create();

private:
    InlineFactory* GetNext()
    {
        CONTRACTL {
            THROWS;
            GC_NOTRIGGER;
        } CONTRACTL_END;

        if (m_next == NULL)
        {
            m_next = new InlineFactory<PRODUCT, MAX_FACTORY_PRODUCT>();
        }
        return m_next;
    }

    InlineFactory* m_next;
    PRODUCT m_product[MAX_FACTORY_PRODUCT];
    INT32 m_cProduct;
};

#include "factory.inl"

#endif

