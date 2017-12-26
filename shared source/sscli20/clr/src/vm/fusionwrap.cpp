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
** Class:  FusionWrap
**
** Purpose: helper to call fusion without com interop
**
===========================================================*/
#include "common.h"

#include "fusionwrap.h"

#include "comstring.h"


FCIMPL4(FC_BOOL_RET, FusionWrap::GetNextAssembly, SafeHandle* hEnumUNSAFE, SafeHandle* hAppCtxUNSAFE, SafeHandle* hNameUNSAFE, UINT32 dwFlags)
{
    CONTRACTL
    {
        THROWS;
        DISABLED(GC_TRIGGERS);
        MODE_COOPERATIVE;
        PRECONDITION(CheckPointer(hEnumUNSAFE));
        PRECONDITION(CheckPointer(hAppCtxUNSAFE));
        PRECONDITION(CheckPointer(hNameUNSAFE));
    }
    CONTRACTL_END
        
    HRESULT hr = S_OK;

    struct _gc
    {
        SAFEHANDLE hEnum;
        SAFEHANDLE hAppCtx;
        SAFEHANDLE hName;
    } gc;

    gc.hEnum = (SAFEHANDLE) hEnumUNSAFE;
    gc.hAppCtx = (SAFEHANDLE) hAppCtxUNSAFE;
    gc.hName = (SAFEHANDLE) hNameUNSAFE;

    HELPER_METHOD_FRAME_BEGIN_RET_PROTECT(gc);

    SafeHandleHolder shhEnum(&gc.hEnum);
    IAssemblyEnum* pAssemblyEnum = (IAssemblyEnum*) gc.hEnum->GetHandle();
    IAssemblyName *pAssemblyName = NULL;
    PVOID pAppCtx = NULL;

    {
        GCX_PREEMP();
        hr = pAssemblyEnum->GetNextAssembly(pAppCtx, &pAssemblyName, dwFlags);
    }

    if (FAILED(hr)) {
        COMPlusThrowHR(hr);
    }

    if (S_OK == hr) {
        
        //
        // hAppCtx and hName are logically out parameters. To simplify things
        // somewhat the SafeHandles are allocated by our managed
        // caller and we assert here that they are empty.
        //
        
        ASSERT(NULL == gc.hAppCtx->GetHandle());
        ASSERT(NULL == gc.hName->GetHandle());
        
        gc.hAppCtx->SetHandle(pAppCtx);
        gc.hName->SetHandle(pAssemblyName);
    }

    HELPER_METHOD_FRAME_END();

    FC_RETURN_BOOL(S_OK == hr);
}
FCIMPLEND

FCIMPL2(StringObject*, FusionWrap::GetDisplayName, SafeHandle* hNameUNSAFE, UINT32 dwDisplayFlags)
{
    CONTRACTL
    {
        THROWS;
        DISABLED(GC_TRIGGERS);
        MODE_COOPERATIVE;
        PRECONDITION(CheckPointer(hNameUNSAFE));
    }
    CONTRACTL_END

    STRINGREF   retVal = NULL;
    StackSString    result;

    SAFEHANDLE hName = (SAFEHANDLE) hNameUNSAFE;

    HELPER_METHOD_FRAME_BEGIN_RET_ATTRIB_2(Frame::FRAME_ATTR_RETURNOBJ, retVal, hName);


    SafeHandleHolder shh(&hName);
    IAssemblyName* pAssemblyName = (IAssemblyName*) hName->GetHandle();
    
    {
        GCX_PREEMP();
        FusionBind::GetAssemblyNameDisplayName(pAssemblyName, result, dwDisplayFlags);
    }

    if (!result.IsEmpty()) {
        retVal = COMString::NewString(result);
    }

    HELPER_METHOD_FRAME_END();

    return (StringObject*)OBJECTREFToObject(retVal);
}
FCIMPLEND

FCIMPL1(void, FusionWrap::ReleaseFusionHandle, INT_PTR pp)
{
    WRAPPER_CONTRACT;
    
    HELPER_METHOD_FRAME_BEGIN_0();

    SafeRelease((IUnknown*)pp);

    HELPER_METHOD_FRAME_END();
}
FCIMPLEND

