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
// ---------------------------------------------------------------------------
// COMEx.cpp
// ---------------------------------------------------------------------------

#include "stdafx.h"
#include "string.h"
#include "ex.h"
#include "holder.h"
#include "corerror.h"

// ---------------------------------------------------------------------------
// COMException class.  Implements exception API for standard COM-based error info
// ---------------------------------------------------------------------------

COMException::~COMException()
{
    WRAPPER_CONTRACT;

    if (m_pErrorInfo != NULL)
        m_pErrorInfo->Release();
}

IErrorInfo *COMException::GetErrorInfo()
{
    LEAF_CONTRACT;

    IErrorInfo *pErrorInfo = m_pErrorInfo;
    if (pErrorInfo != NULL)
        pErrorInfo->AddRef();
    return pErrorInfo;
}

void COMException::GetMessage(SString &string)
{
    STATIC_CONTRACT_THROWS;
    STATIC_CONTRACT_GC_NOTRIGGER;
    
    if (m_pErrorInfo != NULL)
    {
        BSTRHolder message(NULL);
        if (SUCCEEDED(m_pErrorInfo->GetDescription(&message)))
            string.Set(message, SysStringLen(message));
    }
}

