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
////////////////////////////////////////////////////////////////////////////////
// Date: May, 1999
////////////////////////////////////////////////////////////////////////////////

#include "common.h"
#include "commethodrental.h"
#include "corerror.h"

// SwapMethodBody
// This method will take the rgMethod as the new function body for a given method. 
//
FCIMPL6(void, COMMethodRental::SwapMethodBody, ReflectClassBaseObject* clsUNSAFE, INT32 tkMethod, LPVOID rgMethod, INT32 iSize, INT32 flags, StackCrawlMark* stackMark)
{
    CONTRACTL
    {
        THROWS;
        DISABLED(GC_TRIGGERS);
        MODE_COOPERATIVE;
        SO_TOLERANT;
    }
    CONTRACTL_END;
    
    REFLECTCLASSBASEREF cls = (REFLECTCLASSBASEREF) clsUNSAFE;
    HELPER_METHOD_FRAME_BEGIN_1(cls);
    //-[autocvtpro]-------------------------------------------------------

    BYTE        *pNewCode       = NULL;
    MethodDesc  *pMethodDesc;
    ReflectionModule *module;
    ICeeGen*    pGen;
    ULONG       methodRVA;
    HRESULT     hr;

    if ( cls == NULL)
    {
        COMPlusThrowArgumentNull(L"cls");
    }

    MethodTable *pMethodTable = cls->GetType().GetMethodTable();
    PREFIX_ASSUME(pMethodTable != NULL);
    module = (ReflectionModule *) pMethodTable->GetModule();
    pGen = module->GetCeeGen();

    Assembly* caller = SystemDomain::GetCallersAssembly( stackMark );

    _ASSERTE( caller != NULL && "Unable to get calling assembly" );
    _ASSERTE( module->GetCreatingAssembly() != NULL && "ReflectionModule must have a creating assembly to be used with method rental" );

    if (module->GetCreatingAssembly() != caller)
    {
        COMPlusThrow(kSecurityException);
    }

    // Find the methoddesc given the method token
    pMethodDesc = pMethodTable->GetClass()->FindMethod(tkMethod);
    if (pMethodDesc == NULL)
    {
        COMPlusThrowArgumentException(L"methodtoken", NULL);
    }
    if (pMethodDesc->GetMethodTable() != pMethodTable || pMethodDesc->GetNumGenericClassArgs() != 0 || pMethodDesc->GetNumGenericMethodArgs() != 0)
    {
        COMPlusThrowArgumentException(L"methodtoken", L"Argument_TypeDoesNotContainMethod");
    }
    hr = pGen->AllocateMethodBuffer(iSize, &pNewCode, &methodRVA);    
    if (FAILED(hr))
        COMPlusThrowHR(hr);

    if (pNewCode == NULL)
    {
        COMPlusThrowOM();
    }





    // copy the new function body to the buffer
    memcpy(pNewCode, (void *) rgMethod, iSize);

    // make the descr to point to the new code
    // For in-memory module, it is the blob offset that is stored in the method desc
    pMethodDesc->SetRVA(methodRVA);

    // Reset the methoddesc back to unjited state
    pMethodDesc->Reset();

    TADDR pCode = NULL;
    if (flags)
    {
        // JITImmediate
#if _DEBUG
        COR_ILMETHOD* ilHeader = pMethodDesc->GetILHeader();
        _ASSERTE(((BYTE *)ilHeader) == pNewCode);
#endif
        COR_ILMETHOD_DECODER header((COR_ILMETHOD *)pNewCode, pMethodDesc->GetMDImport(), NULL); 

        // minimum validation on the correctness of method header
        if (header.GetCode() == NULL)
            COMPlusThrowHR(VLDTR_E_MD_BADHEADER);

        pCode = pMethodDesc->MakeJitWorker(&header, 0);

    }

    // add feature::
    // If SQL is generating class with inheritance hierarchy, we may need to
    // check the whole vtable to find duplicate entries.

    //-[autocvtepi]-------------------------------------------------------
    HELPER_METHOD_FRAME_END();
}   // COMMethodRental::SwapMethodBody
FCIMPLEND

