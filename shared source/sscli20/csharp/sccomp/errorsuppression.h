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

#ifdef _MSC_VER
#pragma once
#endif

#include "controller.h"

class CErrorSuppression {
private:
    CController * m_pctr;
    bool m_fSet;
    bool m_fSuppressOld;

public:
    CErrorSuppression() {
        m_pctr = NULL;
        m_fSet = false;
        m_fSuppressOld = false;
    }
    CErrorSuppression(CController* pCtr) {
        m_pctr = NULL;
        m_fSet = false;
        m_fSuppressOld = false;
        Suppress(pCtr);
    }
    ~CErrorSuppression() {
        Revert();
    }
    void Revert() {
        if (m_fSet) {
            ASSERT(m_pctr);
            if (!m_fSuppressOld)
                m_pctr->SuppressErrors(false);
            m_fSet = false;
            m_fSuppressOld = false;
            m_pctr = NULL;
        }
        ASSERT(!m_fSet && !m_fSuppressOld && !m_pctr);
    }
    void Suppress(CController * pctr) {
        ASSERT(pctr);
        if (m_fSet) {
            ASSERT(m_pctr == pctr);
            return;
        }
        ASSERT(!m_pctr && !m_fSuppressOld);
        m_pctr = pctr;
        m_fSuppressOld = pctr->FErrorsSuppressed();
        if (!m_fSuppressOld)
            pctr->SuppressErrors(true);
        m_fSet = true;
    }
};
