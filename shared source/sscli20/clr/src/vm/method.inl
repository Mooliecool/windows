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



#ifndef _METHOD_INL_
#define _METHOD_INL_


inline BOOL MethodDesc::HasTemporaryEntryPoint()
{
    WRAPPER_CONTRACT;
    return GetMethodDescChunk()->HasTemporaryEntryPoints();
}

inline TADDR MethodDesc::GetTemporaryEntryPoint()
{
    WRAPPER_CONTRACT;
    MethodDescChunk* pChunk = GetMethodDescChunk();
    TADDR ret = pChunk->GetTemporaryEntryPoint(GetMethodDescIndex() / pChunk->GetUnit());
    _ASSERTE_IMPL(GetMethodDescFromStubAddr(ret) == this);
    return ret;
}


inline InstantiatedMethodDesc* MethodDesc::AsInstantiatedMethodDesc()
{
    WRAPPER_CONTRACT;
    _ASSERTE(GetClassification() == mcInstantiated);
    return PTR_InstantiatedMethodDesc(PTR_HOST_TO_TADDR(this));
}

inline BOOL MethodDesc::IsDomainNeutral()
{
    WRAPPER_CONTRACT;
    return !IsLCGMethod() && GetDomain()->IsSharedDomain();
}

inline BOOL MethodDesc::IsZapped()
{
    WRAPPER_CONTRACT;
    
    // <NICE> In many cases we can do something cheaper to find out if it is zapped </NICE>
    return (GetZapModule() != NULL);
}


inline PTR_DynamicResolver DynamicMethodDesc::GetResolver()
{
    LEAF_CONTRACT;

    return m_pResolver;
}

inline PTR_LCGMethodResolver DynamicMethodDesc::GetLCGMethodResolver()
{
    CONTRACTL
    {
        MODE_ANY;
        GC_NOTRIGGER;
        NOTHROW;
        PRECONDITION(IsLCGMethod());
        SO_TOLERANT;
    }
    CONTRACTL_END;

    return PTR_LCGMethodResolver(m_pResolver);
}

inline PTR_ILStubResolver DynamicMethodDesc::GetILStubResolver()
{
    CONTRACTL
    {
        MODE_ANY;
        GC_NOTRIGGER;
        NOTHROW;
        PRECONDITION(IsILStub());
        SO_TOLERANT;
    }
    CONTRACTL_END;

    return PTR_ILStubResolver(m_pResolver);
}

inline PTR_DynamicMethodDesc MethodDesc::GetDynamicMethodDesc()
{
    CONTRACTL
    {
        MODE_ANY;
        GC_NOTRIGGER;
        NOTHROW;
        PRECONDITION(IsDynamicMethod());
        SO_TOLERANT;
    }
    CONTRACTL_END;

    return PTR_DynamicMethodDesc(PTR_HOST_TO_TADDR(this));
}

inline bool MethodDesc::IsDynamicMethod()
{
    WRAPPER_CONTRACT;
    return (mcDynamic == GetClassification());
}

inline bool MethodDesc::IsLCGMethod()
{
    WRAPPER_CONTRACT;
    return ((mcDynamic == GetClassification()) && PTR_DynamicMethodDesc(PTR_HOST_TO_TADDR(this))->IsLCGMethod());
}

inline bool MethodDesc::IsILStub()
{
    WRAPPER_CONTRACT;
    return ((mcDynamic == GetClassification()) && PTR_DynamicMethodDesc(PTR_HOST_TO_TADDR(this))->IsILStub());
}

#endif  // _METHOD_INL_

