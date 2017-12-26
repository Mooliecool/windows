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
//--------------------------------------------------------------------------
// securityTransparentAssembly.cpp
//
// Implementation for transparent code feature
//
//--------------------------------------------------------------------------

#include "common.h"
#include "field.h"
#include "securitydeclarative.h"
#include "security.h"
#include "securitydescriptor.h"
#include "comreflectioncommon.h"
#include "customattribute.h"
#include "securitytransparentassembly.h"
#include "securitymeta.h"


// Check for Disable Transparency Enforcement
BOOL IsTransparencyDisabled()
{
	return (g_pConfig->TransparencyDisabled());
}


BOOL SecurityTransparent::CheckNonPublicCriticalAccess(MethodDesc* pCurrentMD,
    MethodDesc* pOptionalTargetMethod,
    FieldDesc* pOptionalTargetField,
    MethodTable * pOptionalTargetType)
{
    // Atmost one of these should be non-NULL
    _ASSERTE(1 >= ((pOptionalTargetMethod ? 1 : 0) +
                   (pOptionalTargetField  ? 1 : 0) +
                   (pOptionalTargetType   ? 1 : 0)));

	BOOL fIsCallerTransparent = FALSE;
	if (pCurrentMD != NULL)
	{
		fIsCallerTransparent = IsMethodTransparent(pCurrentMD);
	}
	// if caller is critical, so we are fine, no more checks needed
	if (!fIsCallerTransparent) return TRUE;
	
	// okay caller is transparent, additional checks needed
	BOOL fIsTargetCritical = FALSE; // check if target is critical
	BOOL fIsTargetSafe = FALSE; // check if target is marked safe
	if (pOptionalTargetMethod != NULL)
	{
		MethodSecurityDescriptor methSecurityDescriptor(pOptionalTargetMethod);
		fIsTargetCritical = methSecurityDescriptor.IsCritical();
		fIsTargetSafe = methSecurityDescriptor.IsTreatAsSafe();
	}
	else
	if (pOptionalTargetField != NULL)
	{
		FieldSecurityDescriptor fieldSecurityDescriptor(pOptionalTargetField);
		fIsTargetCritical = fieldSecurityDescriptor.IsCritical();
		fIsTargetSafe = fieldSecurityDescriptor.IsTreatAsSafe();
	}
	else
	if (pOptionalTargetType != NULL)
	{
		TypeSecurityDescriptor typeSecurityDescriptor(pOptionalTargetType->GetClass());
		fIsTargetCritical = typeSecurityDescriptor.IsAllCritical(); // check for only all critical classes
		fIsTargetSafe = typeSecurityDescriptor.IsTreatAsSafe();
	}
	// if target is not critical, we are fine, no more checks needed
	// if the target is critical and is marked as TreatAsSafe, we are fine, no more checks needed.
	if (!fIsTargetCritical || fIsTargetSafe) return TRUE;
	// otherwise we disallow access, no access to non public critical targets (that don't have TreatAsSafe attribute) from transparent callers
	return FALSE;
	
}


CorInfoCanSkipVerificationResult SecurityTransparent::JITCanSkipVerification(MethodDesc * pMD, BOOL fQuickCheckOnly)
{
    BOOL hasSkipVerificationPermisson = false;
    if (fQuickCheckOnly)
        hasSkipVerificationPermisson = Security::CanSkipVerification(pMD->GetAssembly()->GetDomainAssembly(), FALSE); // fCommit == FALSE
    else
        hasSkipVerificationPermisson = Security::CanSkipVerification(pMD->GetAssembly()->GetDomainAssembly(), TRUE);

    CorInfoCanSkipVerificationResult canSkipVerif = hasSkipVerificationPermisson ? CORINFO_VERIFICATION_CAN_SKIP : CORINFO_VERIFICATION_CANNOT_SKIP;

	// also check to see if the method is marked transparent
	if (hasSkipVerificationPermisson)
	{ 	   	
		// also check to see if the method is marked transparent
		if (SecurityTransparent::IsMethodTransparent(pMD))
		{
			canSkipVerif = CORINFO_VERIFICATION_RUNTIME_CHECK;
		}
	}
	return canSkipVerif;
}

CorInfoCanSkipVerificationResult SecurityTransparent::JITCanSkipVerification(DomainAssembly * pAssembly, BOOL fQuickCheckOnly)
{
	BOOL hasSkipVerificationPermisson = false;
	if (fQuickCheckOnly)
       {   
		hasSkipVerificationPermisson = 
                                Security::CanSkipVerification(pAssembly,
                                FALSE);     // fCommit
       }
	else
		hasSkipVerificationPermisson = Security::CanSkipVerification(pAssembly);
	CorInfoCanSkipVerificationResult canSkipVerif = hasSkipVerificationPermisson ? CORINFO_VERIFICATION_CAN_SKIP : CORINFO_VERIFICATION_CANNOT_SKIP;

	if (hasSkipVerificationPermisson)
	{ 	   	
		// also check to see if the assembly is marked transparent
		/*if (SecurityTransparent::IsAssemblyTransparent(pAssembly->GetAssembly()))
		{
			canSkipVerif = CORINFO_VERIFICATION_RUNTIME_CHECK;
		}*/
	}
	return canSkipVerif;			
}

CorInfoIsCallAllowedResult SecurityTransparent::RequiresTransparentAssemblyChecks(MethodDesc* pCallerMD, MethodDesc* pCalleeMD)
{
	return RequiresTransparentCodeChecks(pCallerMD, pCalleeMD);
}

CorInfoIsCallAllowedResult SecurityTransparent::RequiresTransparentCodeChecks(MethodDesc* pCallerMD, MethodDesc* pCalleeMD)
{
	CONTRACTL {
        THROWS;
        GC_TRIGGERS;
	  PRECONDITION(CheckPointer(pCallerMD));
        PRECONDITION(CheckPointer(pCalleeMD));
    } CONTRACTL_END;

	// check if the caller assembly is transparent
	if (IsMethodTransparent(pCallerMD))	
	{
		if( pCalleeMD->RequiresLinktimeCheck() )
		{
			return CORINFO_CALL_RUNTIME_CHECK;
		}
		else
		{
			return CORINFO_CALL_ALLOWED;
		}
	}		
	
	return CORINFO_CALL_ALLOWED;
}


// Perform appropriate Transparency checks if the caller to the Load(byte[] ) without passing in an input Evidence is Transparent
VOID SecurityTransparent::PerformTransparencyChecksForLoadByteArray(MethodDesc* pCallerMD, AssemblySecurityDescriptor* pLoadedSecDesc)
{	
	CONTRACTL
    {
        THROWS;
        GC_TRIGGERS;
        INJECT_FAULT(COMPlusThrowOM(););
    }
    CONTRACTL_END
	GCX_COOP();
	// check to see if the method that does the Load(byte[] ) is transparent
	if (IsMethodTransparent(pCallerMD))
	{		
		Assembly* pLoadedAssembly = pLoadedSecDesc->GetAssembly();
		// check to see if the byte[] being loaded is critical, i.e. not Transparent
		if (!ModuleSecurityDescriptor::IsMarkedTransparent(pLoadedAssembly))
		{			
			// if transparent code loads a byte[] that is critical, need to inject appropriate demands
			if (Security::IsFullyTrusted(pLoadedSecDesc)) // if the loaded code is full-trust
			{
				// do a full-demand for Full-Trust				
				OBJECTREF permSet = NULL;
				GCPROTECT_BEGIN(permSet);
        			Security::GetPermissionInstance(&permSet, SECURITY_FULL_TRUST);
					Security::DemandSet(SSWT_LATEBOUND_LINKDEMAND, permSet);
				GCPROTECT_END();// do a full-demand for Full-Trust				
			}
			else
			{
				// otherwise inject a Demand for permissions being granted?
				struct _localGC {
                			OBJECTREF granted;
                			OBJECTREF denied;
            			} localGC;
            	ZeroMemory(&localGC, sizeof(localGC));

            	GCPROTECT_BEGIN(localGC);
				{
					localGC.granted = Security::GetGrantedPermissionSet(pLoadedSecDesc, &(localGC.denied));
					Security::DemandSet(SSWT_LATEBOUND_LINKDEMAND, localGC.granted);
            	}
				GCPROTECT_END();
			}
		}		
	}	
}


static void ConvertLinkDemandToFullDemand(MethodDesc* pCallerMD, MethodDesc* pCalleeMD) 
{
    CONTRACTL {
        THROWS;
        GC_TRIGGERS;
        MODE_COOPERATIVE;
    }
    CONTRACTL_END;

    BOOL isEveryoneFullyTrusted = FALSE;
    BOOL isSecurityOn = Security::IsSecurityOn();
    if (!isSecurityOn) 
    {  
      return;
    }
    isEveryoneFullyTrusted = Security::AllDomainsOnStackFullyTrusted();

	 

	if (isEveryoneFullyTrusted) 
     	{
		return;
	}

	if (!pCalleeMD->RequiresLinktimeCheck()) 
      {
      		return;
	}
	
   	struct _gc {
      		OBJECTREF refClassNonCasDemands;
			OBJECTREF refClassCasDemands;
	       OBJECTREF refMethodNonCasDemands;
	       OBJECTREF refMethodCasDemands;
	       OBJECTREF refThrowable;
	    } gc;
	
	ZeroMemory(&gc, sizeof(gc));

	GCPROTECT_BEGIN(gc);	
	BOOL fCallerIsAPTCA = pCallerMD->GetClass()->GetAssembly()->AllowUntrustedCaller();

    
        // Fetch link demand sets from all the places in metadata where we might
        // find them (class and method). These might be split into CAS and non-CAS
        // sets as well.
        Security::RetrieveLinktimeDemands(pCalleeMD,
                                          &gc.refClassCasDemands,
                                          &gc.refClassNonCasDemands,
                                          &gc.refMethodCasDemands,
                                          &gc.refMethodNonCasDemands);
            
        // The following logic turns link demands on the target method into full
        // stack walks in order to close security holes in poorly written
        // reflection users.

        _ASSERTE(pCalleeMD);

        if (fCallerIsAPTCA && Security::IsUntrustedCallerCheckNeeded(pCalleeMD, pCallerMD->GetAssembly()) )
        {               
			// if caller is APTCA convert Non-APTCA full-trust LinkDemands to Full-Demands
			OBJECTREF permSet = NULL;
			GCPROTECT_BEGIN(permSet);
        		Security::GetPermissionInstance(&permSet, SECURITY_FULL_TRUST);
			Security::DemandSet(SSWT_LATEBOUND_LINKDEMAND, permSet);
			GCPROTECT_END();
        }

        // CAS Link Demands
        if (gc.refClassCasDemands != NULL)
            Security::DemandSet(SSWT_LATEBOUND_LINKDEMAND, gc.refClassCasDemands);

        if (gc.refMethodCasDemands != NULL)
            Security::DemandSet(SSWT_LATEBOUND_LINKDEMAND, gc.refMethodCasDemands);

        // Non-CAS demands are not applied against a grant
        // set, they're standalone.
        if (gc.refClassNonCasDemands != NULL)
            Security::CheckNonCasDemand(&gc.refClassNonCasDemands);

        if (gc.refMethodNonCasDemands != NULL)
            Security::CheckNonCasDemand(&gc.refMethodNonCasDemands);

        // We perform automatic linktime checks for UnmanagedCode in three cases:
        //   o  P/Invoke calls.
        //   o  Calls through an interface that have a suppress runtime check
        //      attribute on them (these are almost certainly interop calls).
        //   o  Interop calls made through method impls.
        if (pCalleeMD->IsNDirect() ||
            (pCalleeMD->IsInterface() &&
             (pCalleeMD->GetMDImport()->GetCustomAttributeByName(pCalleeMD->GetMethodTable()->GetCl(),
                                                            COR_SUPPRESS_UNMANAGED_CODE_CHECK_ATTRIBUTE_ANSI,
                                                            NULL,
                                                            NULL) == S_OK ||
              pCalleeMD->GetMDImport()->GetCustomAttributeByName(pCalleeMD->GetMemberDef(),
                                                            COR_SUPPRESS_UNMANAGED_CODE_CHECK_ATTRIBUTE_ANSI,
                                                            NULL,
                                                            NULL) == S_OK) ) ||
            (pCalleeMD->IsComPlusCall() && !pCalleeMD->IsInterface())
           )
        {
	 	if (fCallerIsAPTCA)
	 	{
			// if the caller assembly is APTCA, then only inject this demand, for NON-APTCA we will allow supress unmanaged code
			// NOTE: the JIT would have already performed the LinkDemand for this anyways
			Security::SpecialDemand(SSWT_LATEBOUND_LINKDEMAND, SECURITY_UNMANAGED_CODE);        
	 	}
        }

    GCPROTECT_END();

 /*
    if (isSecurityOn && !fRet) {  
        if (checkSkipVer)
            Security::SpecialDemand(SSWT_LATEBOUND_LINKDEMAND, SECURITY_SKIP_VER);
    }
    */
}


VOID SecurityTransparent::EnforceTransparentAssemblyChecks(MethodDesc* pCallerMD, MethodDesc* pCalleeMD)
{
	CONTRACTL {
        THROWS;
        GC_TRIGGERS;
        PRECONDITION(CheckPointer(pCallerMD));
	   PRECONDITION(CheckPointer(pCalleeMD));
	 INJECT_FAULT(COMPlusThrowOM(););
    	} CONTRACTL_END;

	ConvertLinkDemandToFullDemand(pCallerMD, pCalleeMD);
}


BOOL SecurityTransparent::IsMethodTransparent(MethodDesc* pMD)
{
	// if Transparency is disabled, then Ignore all Transparency aspects
	if (IsTransparencyDisabled()) return FALSE;
        MethodSecurityDescriptor methSecurityDescriptor(pMD);
	return !methSecurityDescriptor.IsCritical();
}

BOOL SecurityTransparent::IsFieldTransparent(FieldDesc* pFD)
{
	// if Transparency is disabled, then Ignore all Transparency aspects
	if (IsTransparencyDisabled()) return FALSE;
	
	FieldSecurityDescriptor fieldSecurityDescriptor(pFD);
	return !fieldSecurityDescriptor.IsCritical();	
}

BOOL SecurityTransparent::IsAssemblyTransparent(Assembly* pAssembly)
{
    ModuleSecurityDescriptor* pModuleSecDesc = ModuleSecurityDescriptor::GetModuleSecurityDescriptor(pAssembly);
    return !pModuleSecDesc->IsCritical();
}

BOOL SecurityTransparent::CheckAssemblyHasSecurityTransparentAttribute(Assembly* pAssembly)
{
    CONTRACTL
    {
        THROWS;
        GC_TRIGGERS;
        INJECT_FAULT(COMPlusThrowOM(););
    }
    CONTRACTL_END

    BOOL fIsTransparent = FALSE;
    BOOL fIsCritical = FALSE;

    IMDInternalImport *mdImport = pAssembly->GetManifestImport();
    GCX_COOP();

    if (mdImport->GetCustomAttributeByName(pAssembly->GetManifestToken(),
                                           g_SecurityTransparentAttribute,
                                           NULL,
                                           NULL) == S_OK)
    {
        fIsTransparent = TRUE;
        if (mdImport->GetCustomAttributeByName(pAssembly->GetManifestToken(),
                                               g_SecurityCriticalAttribute,
                                               NULL,
                                               NULL) == S_OK)
            fIsCritical = TRUE;
    }

    // We cannot have both critical and transparent attributes on the assembly level.
    if (fIsTransparent && fIsCritical)
        COMPlusThrow(kInvalidOperationException, L"InvalidOperation_CriticalTransparentAreMutuallyExclusive");

    return fIsTransparent;
}





