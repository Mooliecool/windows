//-------------------------------------------------------------------------------------------------
//
//  Copyright (c) Microsoft Corporation.  All rights reserved.
//
//  Wrapper for CComPtrEx which can handle inheritance scenarios where there are multiple paths
//  to IUnknown
//
//-------------------------------------------------------------------------------------------------

#pragma once

template <class T>
class CComPtrEx : public CComPtrBase<T>
{
public:
    CComPtrEx() throw()
    {
    }
    CComPtrEx(T* lp) throw() :
        CComPtrBase<T>(lp)

    {
    }
    CComPtrEx(_In_ const CComPtrEx<T>& lp) throw() :
        CComPtrBase<T>(lp.p)
    {
    }
    T* operator=(_In_opt_ T* lp) throw()
    {
        if(*this!=lp)
        {
            CComPtrEx<T> sp(lp);
            TemplateUtil::Swap(&p, &sp.p);
        }
        return *this;
    }
    T* operator=(_In_ const CComPtrEx<T>& lp) throw()
    {
        if(*this!=lp)
        {
            CComPtrEx<T> sp(lp);
            TemplateUtil::Swap(&p, &sp.p);
        }
        return *this;
    }
};

