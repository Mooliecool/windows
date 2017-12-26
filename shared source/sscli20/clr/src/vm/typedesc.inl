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

#ifndef _TYPEDESC_INL_
#define _TYPEDESC_INL_

inline MethodTable*  TypeDesc::GetMethodTable() {

    WRAPPER_CONTRACT;
    if (GetInternalCorElementType() == ELEMENT_TYPE_VAR || GetInternalCorElementType() == ELEMENT_TYPE_MVAR)
        return NULL;

    if (GetInternalCorElementType() == ELEMENT_TYPE_FNPTR)
        return TheFnPtrClass();

    _ASSERTE(HasTypeParam());
    ParamTypeDesc* asParam = PTR_ParamTypeDesc(PTR_HOST_TO_TADDR(this));
    return(asParam->m_TemplateMT);
}

inline TypeHandle TypeDesc::GetTypeParam() {
    WRAPPER_CONTRACT;
    if (GetInternalCorElementType() == ELEMENT_TYPE_VAR || GetInternalCorElementType() == ELEMENT_TYPE_MVAR)
        return TypeHandle();
    _ASSERTE(HasTypeParam());
    ParamTypeDesc* asParam = PTR_ParamTypeDesc(PTR_HOST_TO_TADDR(this));
    return(asParam->m_Arg);
}

inline TypeHandle* TypeDesc::GetClassOrArrayInstantiation() {
    WRAPPER_CONTRACT;
    if (GetInternalCorElementType() != ELEMENT_TYPE_FNPTR)
    {
        ParamTypeDesc* asParam = PTR_ParamTypeDesc(PTR_HOST_TO_TADDR(this));
        return(&asParam->m_Arg);
    }
    else
        return NULL;
}


#endif  // _TYPEDESC_INL_



