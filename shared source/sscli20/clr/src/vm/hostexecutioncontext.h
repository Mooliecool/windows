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

class HostExecutionContextManager
{
public:
    static IHostSecurityContext*  m_pRestrictedHostContext;
    static void InitializeRestrictedContext();
    static void SetHostRestrictedContext();
    	
    static FCDECL0(FC_BOOL_RET, HostPresent);	
    static FCDECL1(HRESULT, ReleaseSecurityContext, LPVOID handle);
    static FCDECL1(HRESULT, CaptureSecurityContext, SafeHandle* hTokenUNSAFE);	
    static FCDECL2(HRESULT, CloneSecurityContext, SafeHandle* hTokenUNSAFE, SafeHandle* hTokenClonedUNSAFE);	
    static FCDECL3(HRESULT, SetSecurityContext, SafeHandle* hTokenUNSAFE, CLR_BOOL fReturnPrevious, SafeHandle* hTokenPreviousUNSAFE);	    
};
