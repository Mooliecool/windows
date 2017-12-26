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
// securitymeta.cpp
//
//pre-computes security meta information, from declarative and run-time information
//
//--------------------------------------------------------------------------


#include "common.h"

#include "object.h"
#include "excep.h"
#include "vars.hpp"
#include "security.h"
#include "securitydescriptor.h"
#include "securitydescriptorassembly.h"

#include "perfcounters.h"
#include "comstring.h"
#include "nlstable.h"
#include "frames.h"
#include "dllimport.h"
#include "strongname.h"
#include "eeconfig.h"
#include "field.h"
#include "appdomainhelper.h"
#include "threads.h"
#include "eventtrace.h"
#include "timeline.h"
#include "objectclone.h"
#include "stackcompressor.h"
#include "securitydeclarative.h"
#include "comreflectioncommon.h"
#include "customattribute.h"
#include "securitytransparentassembly.h"

#include "securitymeta.h"

   BOOL g_fUseNoStubsChecked = FALSE;
   BOOL g_fUseNoStubs = TRUE;

VOID FieldSecurityDescriptor::VerifyDataComputed()
{	
	if (_fIsComputed == FALSE)
	{
		// init all results
		_fIsCritical = FALSE;
		_fIsTreatAsSafe = FALSE;		
	     	    	
		// check to see if the class has the critical attribute
		EEClass* pClass = _pFD->GetEnclosingClass();
		_ASSERTE(pClass);

        TypeSecurityDescriptor typeSecDesc(pClass);

		// early out for all critical/treat as safe (common case)
		if (typeSecDesc.IsAllCritical() && typeSecDesc.IsTreatAsSafe())
		{
			_fIsCritical = TRUE;
			//_fIsAllCritical = TRUE;
			_fIsTreatAsSafe = TRUE;		
		}
		else
		{
		        // bool fIsClassCritical = typeSecDesc.IsCritical();			
			 // don't require the class to also be marked critical
			if (!typeSecDesc.IsAllTransparent())		
			{				
		    	IMDInternalImport *mdImport = pClass->GetAssembly()->GetManifestImport();

				// get the field critical attribute info
				TokenSecurityDescriptor fieldTokenSecurityDescriptor(_pFD->GetMemberDef(), mdImport);
			
				// if class is ALL critical 
				if (typeSecDesc.IsAllCritical())
				{
					// class all critical => so override method flags
					_fIsCritical = TRUE; // field  is critical
					//_fIsAllCritical  = TRUE; // everything is critical
				}			
				else
				{
					// pick up the field attributes
					_fIsCritical = fieldTokenSecurityDescriptor.IsCritical();
					//_fIsAllCritical = methodCriticalInfo.IsAllCritical(); // NO OP
				}					
			
				// if field is critical
				if (_fIsCritical == TRUE)
				{
					// if class is marked treat as safe or field is marked treat as safe
					if (typeSecDesc.IsTreatAsSafe() ||  fieldTokenSecurityDescriptor.IsTreatAsSafe())
					{
						//then field is treatasSafe
						_fIsTreatAsSafe  = TRUE; 
					}
				}
			}
		}
		
		// mark computed
		_fIsComputed = TRUE;
	}
}


// All callers to his method will pass in a valid memory location for pMethodSecurityDesc which they are responsible for
// free-ing when done using it. Typically this will be a stack location for perf reasons.
//
// Some details about when we cache MethodSecurityDescriptors and how the linkdemand process works:
// - When we perform the LinkTimeCheck, we follow this order of checks
//     : APTCA check
//     : Class-level declarative security using TypeSecurityDescriptor
//     : Method-level declarative security using MethodSecurityDescriptor
//     : Unmanaged-code check (if required)
//
// For APTCA and Unmanaged code checks, we don't have a permissionset entry in the hashtable that we use when performing the demand. Since
// these are well-known demands, we special-case them. What this means is that we may have a MethodSecurityDescriptor that requires a linktime check
// but does not have DeclActionInfo or TokenDeclActionInfo fields inside. 
// 
// For cases where the Type causes the Link/Inheritance demand, the MethodDesc has the flag set, but the MethodSecurityDescriptor will not have any
// DeclActionInfo or TokenDeclActionInfo.
//
// And the relevance all this has to this method is the following: Don't automatically insert a MethodSecurityDescriptor into the hash table if it has
// linktime or inheritance time check. Only do so if either of the DeclActionInfo or TokenDeclActionInfo fields are non-NULL.
void MethodSecurityDescriptor::LookupOrCreateMethodSecurityDescriptor(MethodSecurityDescriptor* ret_methSecDesc)
{
    CONTRACTL {
        THROWS;
        GC_TRIGGERS;
        MODE_ANY;
        PRECONDITION(CheckPointer(ret_methSecDesc));
    } CONTRACTL_END;

    _ASSERTE(CanMethodSecurityDescriptorBeCached(ret_methSecDesc->_pMD));

    

    MethodSecurityDescriptor* pMethodSecurityDesc = (MethodSecurityDescriptor*)TokenSecurityDescriptor::LookupSecurityDescriptor(ret_methSecDesc->_pMD);
    if (pMethodSecurityDesc == NULL)
    {
        ret_methSecDesc->VerifyDataComputedInternal();// compute all the data that is needed.

        // cache method security desc using some simple heuristics
        // we have some token actions computed, let us cache this method security desc
        
        if (ret_methSecDesc->GetRuntimeDeclActionInfo() != NULL ||
            ret_methSecDesc->GetTokenDeclActionInfo() != NULL)
        {

            // Need to insert this methodSecDesc
            LPVOID pMem = GetAppDomain()->GetLowFrequencyHeap()->AllocMem_NoThrow(sizeof(MethodSecurityDescriptor));
            if (pMem == NULL)
                COMPlusThrowOM();
            
             // allocate a method security descriptor, using the appdomain heap memory
            pMethodSecurityDesc = new (pMem) MethodSecurityDescriptor(ret_methSecDesc->_pMD);
            
            *pMethodSecurityDesc = *ret_methSecDesc; // copy over the fields

            MethodSecurityDescriptor* pExistingMethodSecurityDesc = NULL;
            // insert pMethodSecurityDesc into our hash table
            pExistingMethodSecurityDesc = (MethodSecurityDescriptor*)TokenSecurityDescriptor::InsertSecurityDescriptor(ret_methSecDesc->_pMD, (HashDatum) pMethodSecurityDesc);                        
            if (pExistingMethodSecurityDesc != NULL)
            {
                // if we found an existing method security desc, use it
                // no need to delete the one we had created, as we allocated it in the Appdomain heap
                pMethodSecurityDesc = pExistingMethodSecurityDesc;
            }
        }
    }
    else
    {
        *ret_methSecDesc = *pMethodSecurityDesc;
    }
   
    return;
}

VOID MethodSecurityDescriptor::VerifyDataComputedInternal()
{	
    if (_fIsComputed == FALSE)
    {
        // init all members
        _fIsCritical = FALSE;
        _fIsTreatAsSafe = FALSE;
        _declFlagsDuringPreStub = 0;
        _pRuntimeDeclActionInfo = NULL;
        _pTokenDeclActionInfo = NULL;
        _fAssertAllowed = FALSE;

        // compute CRITICAL/TRANSPARENT STUFF
        ComputeCriticalTransparentInfo();

        // compute RUN-TIME DECLARATIVE SECURITY STUFF 
        // (merges both class and method level run-time declarative security info).
        if (HasRuntimeDeclarativeSecurity())
        {
            ComputeRuntimeDeclarativeSecurityInfo();
         }

        // compute method specific DECLARATIVE STUFF
        if (HasRuntimeDeclarativeSecurity() || HasLinkOrInheritanceDeclarativeSecurity())
        {
            ComputeMethodDeclarativeSecurityInfo();
        }

        // mark computed
        _fIsComputed = TRUE;
    }
}

VOID MethodSecurityDescriptor::ComputeCriticalTransparentInfo()
{
    // CHECK CRITICAL/TRANSPARENT STUFF
    // check to see if the assembly has the critical attribute
    EEClass* pClass = _pMD->GetClass();
    _ASSERTE(pClass);

    TypeSecurityDescriptor typeSecDesc(pClass);
    // early out for all critical/treat as safe (common case)
    if (typeSecDesc.IsAllCritical() && typeSecDesc.IsTreatAsSafe())
    {
    	_fIsCritical = TRUE;
    	//_fIsAllCritical = TRUE;
    	_fIsTreatAsSafe = TRUE;		
    }
    else
    {
    	// bool fIsClassCritical = typeSecDesc.IsCritical();			
                    // don't require the class to also be marked critical
    	if (!typeSecDesc.IsAllTransparent())		
    	{				
    		// get the method critical attribute info
    		TokenSecurityDescriptor methodTokenSecurityDescriptor(GetToken(), GetIMDInternalImport());
    	
    		// if class is ALL critical 
    		if (typeSecDesc.IsAllCritical())
    		{
    			// class all critical => so override method flags
    			_fIsCritical = TRUE; // method  is critical
    			//_fIsAllCritical  = TRUE; // everything is critical
    		}			
    		else
    		{
    			// pick up the method attributes
    			_fIsCritical = methodTokenSecurityDescriptor.IsCritical();
    			//_fIsAllCritical = methodCriticalInfo.IsAllCritical(); // NO OP
    		}					
    	
    		// irrespective of whether method is critical, check for TreatAsSafe
    		//if (_fIsCritical == TRUE)
    		{
    			// if class is marked treat as safe or method is marked treat as safe
    			if (typeSecDesc.IsTreatAsSafe() ||  methodTokenSecurityDescriptor.IsTreatAsSafe())
    			{
    				//then method is treatasSafe
    				_fIsTreatAsSafe  = TRUE; 
    			}
    		}
    	}
    }
}

VOID MethodSecurityDescriptor::ComputeRuntimeDeclarativeSecurityInfo()
{
    // CHECK RUN-TIME DECLARATIVE SECURITY STUFF
    // get security flags for run-time declarative security
    _ASSERTE(HasRuntimeDeclarativeSecurity());
    _declFlagsDuringPreStub = _pMD->GetSecurityFlagsDuringPreStub();
    _ASSERTE(_declFlagsDuringPreStub && " Expected some runtime security action");
    _pRuntimeDeclActionInfo = SecurityDeclarative::DetectDeclActions(_pMD, _declFlagsDuringPreStub);
}

VOID MethodSecurityDescriptor::ComputeMethodDeclarativeSecurityInfo()
{
    _ASSERTE(HasRuntimeDeclarativeSecurity()|| HasLinkOrInheritanceDeclarativeSecurity());
    DWORD dwDeclFlags;
    HRESULT hr = SecurityDeclarative::GetDeclarationFlags(GetIMDInternalImport(), GetToken(), &dwDeclFlags, NULL, NULL);

    if (SUCCEEDED(hr))
    {
        GCX_COOP();
        DWORD tokenSetIndexes[dclMaximumValue + 1];
        SecurityDeclarative::DetectDeclActionsOnToken(GetToken(), dwDeclFlags, tokenSetIndexes, GetIMDInternalImport());
    	

        DWORD dwLocalAction;
        bool builtInCASPermsOnly = TRUE;
        for (dwLocalAction = 0; dwLocalAction <= dclMaximumValue; dwLocalAction++)
        {
            if (tokenSetIndexes[dwLocalAction] != INVALID_SET_INDEX)
            {
                TokenDeclActionInfo::LinkNewDeclAction(&_pTokenDeclActionInfo, (CorDeclSecurity)dwLocalAction, tokenSetIndexes[dwLocalAction]);
                builtInCASPermsOnly = builtInCASPermsOnly && SecurityAttributes::ContainsBuiltinCASPermsOnly(tokenSetIndexes[dwLocalAction], dwLocalAction);
            }
        }
        _fIsBuiltInCASPermsOnly = builtInCASPermsOnly;
        SecurityProperties sp(dwDeclFlags);
        _fIsDemandsOnly = sp.FDemandsOnly();
        if (sp.FAssertionsExist())
        {
            // Do a check to see if the assembly has been granted permission to assert and let's cache that value in the MethodSecurityDesriptor
            
            Module* pModule = _pMD->GetModule();
            PREFIX_ASSUME_MSG(pModule != NULL, "Should be a Module pointer here");
            
            if (Security::CanAssert(pModule))
                _fAssertAllowed = TRUE;
        }
    }
}

VOID MethodSecurityDescriptor::InvokeInheritanceChecks(MethodDesc *pMethod)
{
    CONTRACTL {
        THROWS;
        GC_TRIGGERS;
        MODE_ANY;
    } CONTRACTL_END;

    // Check CAS Inheritance
    OBJECTREF refCasDemands = NULL;
    DWORD dwCasDemandsIndex = ~0;



    HRESULT hr = GetDeclaredPermissionsWithCache(dclInheritanceCheck, &refCasDemands, &dwCasDemandsIndex);
    if (refCasDemands != NULL)
    {
        _ASSERTE(dwCasDemandsIndex != (DWORD)(~0));
        
        // See if inheritor's assembly has passed this demand before
        AssemblySecurityDescriptor *pInheritorAssem = pMethod->GetAssembly()->GetSecurityDescriptor();
        DWORD index;
        bool fSkipCheck = false;
        for (index = 0; index < pInheritorAssem->m_dwNumPassedDemands; index++)
        {
            if (pInheritorAssem->m_arrPassedLinktimeDemands[index] == dwCasDemandsIndex)
            {
                fSkipCheck = true;
                break;
            }
        }

        if (!fSkipCheck)
        {
            GCPROTECT_BEGIN(refCasDemands);
            
            // Perform the check (it's really just a LinkDemand)
            SecurityStackWalk::LinkOrInheritanceCheck(pMethod->GetAssembly()->GetSecurityDescriptor(), refCasDemands, pMethod->GetAssembly(), dclInheritanceCheck);
            
            // Demand passed. Add it to the Inheritor's assembly's list of passed demands
            if (pInheritorAssem->m_dwNumPassedDemands <= (MAX_PASSED_DEMANDS - 1))
                pInheritorAssem->m_arrPassedLinktimeDemands[pInheritorAssem->m_dwNumPassedDemands++] = dwCasDemandsIndex;
            
            // Call the callback
            GetAppDomain()->OnLinktimeCheck(pMethod->GetAssembly(), refCasDemands, NULL);
            
            GCPROTECT_END();
        }
    }


    OBJECTREF refNonCasDemands = NULL;
    hr = GetDeclaredPermissionsWithCache( dclNonCasInheritance, &refNonCasDemands, NULL);
    if (refNonCasDemands != NULL)
    {
        _ASSERTE(((PERMISSIONSETREF)refNonCasDemands)->CheckedForNonCas() && "Declarative permissions should have been checked for nonCAS in PermissionSet.CreateSerialized");
        if (((PERMISSIONSETREF)refNonCasDemands)->ContainsNonCas())
        {
            GCPROTECT_BEGIN(refNonCasDemands);

            // Perform the check
            MethodDescCallSite demand(METHOD__PERMISSION_SET__DEMAND_NON_CAS, &refNonCasDemands);
            ARG_SLOT arg = ObjToArgSlot(refNonCasDemands);
            demand.Call(&arg);

            // Call the callback
            GetAppDomain()->OnLinktimeCheck(pMethod->GetAssembly(), NULL, refNonCasDemands);

            GCPROTECT_END();
        }
    }
}
TypeSecurityDescriptor* TypeSecurityDescriptor::GetTypeSecurityDescriptor(EEClass* pClass)
{
    CONTRACTL {
        THROWS;
        GC_TRIGGERS;
        MODE_ANY;
    } CONTRACTL_END;

    TypeSecurityDescriptor* pTypeSecurityDesc =NULL;
    

    pTypeSecurityDesc = (TypeSecurityDescriptor*)TokenSecurityDescriptor::LookupSecurityDescriptor(pClass);
    if (pTypeSecurityDesc == NULL)
    {
            // didn't find a  security descriptor, create one and insert it
            LPVOID pMem = GetAppDomain()->GetLowFrequencyHeap()->AllocMem_NoThrow(sizeof(TypeSecurityDescriptor));
            if (pMem == NULL)
                COMPlusThrowOM();

            // allocate a  security descriptor, using the appdomain help memory
           pTypeSecurityDesc = new (pMem) TypeSecurityDescriptor(pClass);
           pTypeSecurityDesc->VerifyDataComputedInternal(); // compute all the data that is needed.

            TypeSecurityDescriptor* pExistingTypeSecurityDesc = NULL;
            // insert securitydesc into our hash table
            pExistingTypeSecurityDesc = (TypeSecurityDescriptor*)TokenSecurityDescriptor::InsertSecurityDescriptor(pClass, (HashDatum) pTypeSecurityDesc);                        
            if (pExistingTypeSecurityDesc != NULL)
            {
                // if we found an existing  security desc, use it
                // no need to delete the one we had created, as we allocated it in the Appdomain help
                pTypeSecurityDesc = pExistingTypeSecurityDesc;
            }
    }
   
    return pTypeSecurityDesc;
}


HRESULT TokenDeclActionInfo::GetDeclaredPermissionsWithCache(
								IN CorDeclSecurity action,
								OUT OBJECTREF *pDeclaredPermissions,
								OUT DWORD* pdwSetIndex)
{
        HRESULT         hr = S_OK;
        DWORD dwActionFlag = DclToFlag((CorDeclSecurity)action);

	DWORD dwIndex = 0;
	TokenDeclActionInfo* pCurrentAction = this;
	for (;
		 pCurrentAction;
		 pCurrentAction = pCurrentAction->pNext)
		{
			if (pCurrentAction->dwDeclAction == dwActionFlag)
			{
				dwIndex = pCurrentAction->dwSetIndex;
				break;
			}
		}
	if (pDeclaredPermissions && pCurrentAction)
	{
		*pDeclaredPermissions = SecurityAttributes::GetPermissionSet(dwIndex,action);
	}
	if (pdwSetIndex && pCurrentAction)
	{
		*pdwSetIndex = dwIndex;
	}

	return hr;
}

OBJECTREF TokenDeclActionInfo::GetLinktimePermissions(OBJECTREF *prefNonCasDemands)
{
    
    OBJECTREF refCasDemands = NULL;
    GCPROTECT_BEGIN(refCasDemands);

    GetDeclaredPermissionsWithCache(
        dclLinktimeCheck,
        &refCasDemands, NULL);

    TokenDeclActionInfo::GetDeclaredPermissionsWithCache(
        dclNonCasLinkDemand,
        prefNonCasDemands, NULL);

    GCPROTECT_END();
    return refCasDemands;    
}

VOID TokenDeclActionInfo::InvokeLinktimeChecks(Assembly* pCaller, BOOL *pfResult, OBJECTREF *pThrowable)
{
    CONTRACTL {
        THROWS;
        GC_TRIGGERS;
        MODE_COOPERATIVE;
        INJECT_FAULT(COMPlusThrowOM(););
    } CONTRACTL_END;

    _ASSERTE( pCaller );    

    EX_TRY
    {
        struct gc {
            OBJECTREF refNonCasDemands;
            OBJECTREF refCasDemands;
        } gc;
        ZeroMemory(&gc, sizeof(gc));

        GCPROTECT_BEGIN(gc);

        // CAS LinkDemands
        GetDeclaredPermissionsWithCache(
        dclLinktimeCheck,
        &gc.refCasDemands, NULL);
                 
        if (gc.refCasDemands != NULL)
        {
            SecurityStackWalk::LinkOrInheritanceCheck(pCaller->GetSecurityDescriptor(), gc.refCasDemands, pCaller, dclLinktimeCheck);
        }


        // NON CAS LinkDEMANDS (we shouldn't support this).
        GetDeclaredPermissionsWithCache(
        dclNonCasLinkDemand,
        &gc.refNonCasDemands, NULL);

        GetAppDomain()->OnLinktimeCheck(pCaller, gc.refCasDemands, gc.refNonCasDemands);

        GCPROTECT_END();
    }
    EX_CATCH
    {
        *pfResult = FALSE;
        UpdateThrowable(pThrowable);
    }
    EX_END_CATCH(RethrowTerminalExceptions);
}


VOID TypeSecurityDescriptor::ComputeCriticalTransparentInfo()
{
    // check to see if the assembly has the critical attribute
    Assembly* pAssembly = _pClass->GetAssembly();
    _ASSERTE(pAssembly);
    IMDInternalImport *mdImport = pAssembly->GetManifestImport();
    	
    ModuleSecurityDescriptor* pModuleSecDesc = ModuleSecurityDescriptor::GetModuleSecurityDescriptor(pAssembly);

    // early out for all critical/treat as safe (common case)
    if (pModuleSecDesc->IsAllCritical() && pModuleSecDesc->IsTreatAsSafe())
    {
    	_fIsCritical = TRUE;
    	_fIsAllCritical = TRUE;
    	_fIsTreatAsSafe = TRUE;		
    }
    else
    {
    	// slow path
    	// if the assembly is critical
    	if (pModuleSecDesc->IsCritical())		
    	{				
    		// get the class critical attribute info
    		TokenSecurityDescriptor classTokenSecurityDescriptor(_pClass->GetCl(), mdImport);

            // NOTE: in the non-nested class, enclosingTypeSecurityDescriptor is uninitialized (NULL) and will null-ref if used
            TypeSecurityDescriptor enclosingTypeSecurityDescriptor(_pClass->IsNested()?
                    _pClass->GetMethodTable()->LoadEnclosingMethodTable()->GetClass():NULL);

    		// if assem is ALL critical 
    		if (pModuleSecDesc->IsAllCritical())
    		{
    			// assembly all critical => so override class flags
    			_fIsCritical = TRUE; // class is critical
    			_fIsAllCritical  = TRUE; // everything is critical				
    		}
    		else
    		{
    		    // if class is nested, then cascade all critical to inner class
    		    if (_pClass->IsNested())
                {
                    _fIsAllCritical = enclosingTypeSecurityDescriptor.IsAllCritical();
        			_fIsCritical = _fIsAllCritical;
                }

                // if still not critical, then look at inner/target class
                if (!_fIsCritical)
                {
        			// pick up data from the target class attributes
                    _fIsAllCritical = classTokenSecurityDescriptor.IsAllCritical();
        			_fIsCritical = classTokenSecurityDescriptor.IsCritical();
                }
    		}
    	
    		//  irrespective of class is critical, check for TreatAsSafe
    		//if (_fIsCritical == TRUE)
    		{
    			// if assembly is marked treat as safe or nested class is TAS or target class is TAS
    			if (pModuleSecDesc->IsTreatAsSafe() ||
                    (_pClass->IsNested() && enclosingTypeSecurityDescriptor.IsTreatAsSafe()) ||
                    classTokenSecurityDescriptor.IsTreatAsSafe())
    			{
                       //then class is treatasSafe
    		           _fIsTreatAsSafe  = TRUE;                            				
    			}
    		}    
       }
       else
       {
            // module is Transparent
            _fIsAllTransparent = TRUE;
       }
    }
}

VOID TypeSecurityDescriptor::ComputeTypeDeclarativeSecurityInfo()
{
    // if method doesn't have any security return
    if (!IsTdHasSecurity(_pClass->GetAttrClass())) return;
    
    DWORD dwDeclFlags;
    HRESULT hr = SecurityDeclarative::GetDeclarationFlags(GetIMDInternalImport(), GetToken(), &dwDeclFlags, NULL, NULL);

    if (SUCCEEDED(hr))
    {
        GCX_COOP();
        DWORD tokenSetIndexes[dclMaximumValue + 1];
        SecurityDeclarative::DetectDeclActionsOnToken(GetToken(), dwDeclFlags, tokenSetIndexes, GetIMDInternalImport());
    	

        DWORD dwLocalAction;
        for (dwLocalAction = 0; dwLocalAction <= dclMaximumValue; dwLocalAction++)
        {
            if (tokenSetIndexes[dwLocalAction] != INVALID_SET_INDEX)
            {
                TokenDeclActionInfo::LinkNewDeclAction(&_pTokenDeclActionInfo, (CorDeclSecurity)dwLocalAction, tokenSetIndexes[dwLocalAction]);
            }
        }
    }
}



VOID TypeSecurityDescriptor::VerifyDataComputedInternal()
{
    if (_fIsComputed == FALSE)
    {
        // init all results
        _fIsAllCritical = FALSE;
        _fIsCritical = FALSE;
        _fIsTreatAsSafe = FALSE;
        _pTokenDeclActionInfo = NULL;
        	     	 
        // COMPUTE CRITICAL/TRANSPARENT INFO	
        ComputeCriticalTransparentInfo();

        // COMPUTE Type DECLARATIVE SECURITY INFO
        ComputeTypeDeclarativeSecurityInfo();
        
         // mark computed
        _fIsComputed = TRUE;
    }
}

VOID TypeSecurityDescriptor::InvokeInheritanceChecks(EEClass* pClass)
{
    CONTRACTL {
        THROWS;
        GC_TRIGGERS;
        MODE_COOPERATIVE;
    } CONTRACTL_END;
    
    _ASSERTE(HasInheritanceDeclarativeSecurity());

    // If we have a class that requires inheritance checks,
    // then we require a thread to perform the checks.
    // We won't have a thread when some of the system classes
    // are preloaded, so make sure that none of them have
    // inheritance checks.
    _ASSERTE(GetThread() != NULL);

    struct gc {
        OBJECTREF refCasDemands;
        OBJECTREF refNonCasDemands;
    } gc;
    ZeroMemory(&gc, sizeof(gc));

    GCPROTECT_BEGIN(gc);        

    
    if (_pClass->RequiresCasInheritanceCheck())
    {
        GetDeclaredPermissionsWithCache(dclInheritanceCheck, &gc.refCasDemands, NULL);
    }

    if (_pClass->RequiresNonCasInheritanceCheck())
    {
        GetDeclaredPermissionsWithCache(dclNonCasInheritance, &gc.refNonCasDemands, NULL);
    }

    if (gc.refCasDemands != NULL)
    {
        SecurityStackWalk::LinkOrInheritanceCheck(pClass->GetAssembly()->GetSecurityDescriptor(), gc.refCasDemands, pClass->GetAssembly(), dclInheritanceCheck);
    }

    if (gc.refNonCasDemands != NULL)
    {
        _ASSERTE(((PERMISSIONSETREF)gc.refNonCasDemands)->CheckedForNonCas() && "Declarative permissions should have been checked for nonCAS in PermissionSet.CreateSerialized");
        if(((PERMISSIONSETREF)gc.refNonCasDemands)->ContainsNonCas())
        {
            MethodDescCallSite demand(METHOD__PERMISSION_SET__DEMAND_NON_CAS, &gc.refNonCasDemands);

            ARG_SLOT arg = ObjToArgSlot(gc.refNonCasDemands);
            demand.Call(&arg);
        }
    }


    GetAppDomain()->OnLinktimeCheck(pClass->GetAssembly(), gc.refCasDemands, gc.refNonCasDemands);

    GCPROTECT_END();
}

// Module security descriptor contains static security information about the module
// this information could get persisted in the NGen image

VOID ModuleSecurityDescriptor::VerifyDataComputed()
{
    WRAPPER_CONTRACT;
    if (!_fIsModuleSecurityChecked)
    {
        // init all results
    	_fIsAllCritical = FALSE;
    	_fIsCritical = FALSE;
    	_fIsTreatAsSafe = FALSE;
        _fIsAPTCA = FALSE;
        
    	Assembly* pAssembly = m_pModule->GetAssembly();

        // APTCA CHECK
    	// check for system assembly and non-strong named assemblies first
    	SharedSecurityDescriptor* pSecDesc = pAssembly->GetSharedSecurityDescriptor();
        if (Security::IsSystem(pSecDesc) || !pAssembly->IsStrongNamed())
    	{
        	_fIsAPTCA = TRUE;
        }
    	else
    	{
		// check for APTCA attribute
    		_fIsAPTCA = CheckAssemblyHasSecurityAPTCAttribute(pAssembly);
    	}

        // TRANSPARENCY CHECKS        
    	// check for transparent assembly attribute
        _fIsMarkedTransparent =  SecurityTransparent::CheckAssemblyHasSecurityTransparentAttribute(pAssembly);
    			
        // if assembly is transparent, then it is not critical
    	if (!_fIsMarkedTransparent) 
    	{	
        	       	// set up a token attribute parser for assembly token
            	TokenSecurityDescriptor assemTokenSecurityDescriptor(pAssembly->GetManifestToken(), pAssembly->GetManifestImport());			

    		// if assembly is not transparent then it is critical
    		_fIsCritical = TRUE;
    		if (assemTokenSecurityDescriptor.IsCritical()) // if explicitly marked critical	
    		{
    			// look for other attributes
    		     _fIsAllCritical = assemTokenSecurityDescriptor.IsAllCritical();
    		     _fIsTreatAsSafe = assemTokenSecurityDescriptor.IsTreatAsSafe();
    		}
    		else
    		{
    			// assembly has no attribute, so everything is critical and treat as safe
    			_fIsAllCritical = TRUE;
    			_fIsTreatAsSafe = TRUE;				
    		}
    	}
    	_fIsModuleSecurityChecked = TRUE;			
    }
}

ModuleSecurityDescriptor* ModuleSecurityDescriptor::GetModuleSecurityDescriptor(Assembly *pAssembly)
{
    // if transparency not checked, check now
    Module* pModule = pAssembly->GetManifestModule();
    _ASSERTE(pModule);
    ModuleSecurityDescriptor* pModuleSecurityDesc = pModule->m_pModuleSecurityDescriptor;
    _ASSERTE(pModuleSecurityDesc);
    return pModuleSecurityDesc;
}

VOID ModuleSecurityDescriptor::Save(DataImage *image)
{	
    WRAPPER_CONTRACT;    
}

VOID ModuleSecurityDescriptor::Fixup(DataImage *image)
{
    WRAPPER_CONTRACT;    
}



// grok the meta data and compute the necessary attributes
VOID TokenSecurityDescriptor::VerifyDataComputed()
{
   if (_fIsComputed == FALSE)
   {
   	// compute attributes from Token
   	
	// init results
     _fIsCritical = FALSE;
     _fIsAllCritical = FALSE;
     _fIsTreatAsSafe = FALSE;

	const BYTE *        pVal;                 
	ULONG               cbVal; 
	SecurityCriticalFlags	 flags;

	if(_mdImport->GetCustomAttributeByName(_token, g_SecurityCriticalAttribute,
                                                            (const void**)&pVal, 
                                                            &cbVal) == S_OK)
	{
		_fIsCritical  = TRUE;	 // mark critical	
 		 flags = (SecurityCriticalFlags)*(pVal + 2);
	        if (flags == SecurityCriticalFlags_All)
	        {
			_fIsAllCritical  = TRUE; // everything critical attribute is set
	        }		 
	}
	if(_mdImport->GetCustomAttributeByName(_token,
                                                    g_SecurityTreatAsSafeAttribute,
                                                    NULL,
                                                    NULL) == S_OK)
	{	
		_fIsTreatAsSafe = TRUE;
	}

	// set computed to true
	_fIsComputed = TRUE;
   }
}

HashDatum TokenSecurityDescriptor::LookupSecurityDescriptor(void* pKey)
{
    CONTRACTL {
        THROWS;
        GC_TRIGGERS;
        MODE_ANY;
    } CONTRACTL_END;
    
	
    HashDatum      datum;
    AppDomain*      pDomain  = GetAppDomain();
    
    EEPtrHashTable  &rCachedMethodPermissionsHash = pDomain->m_pSecContext->m_pCachedMethodPermissionsHash;
    
    MethodSecurityDescriptor* pHashValue = NULL;

    // Fast attempt, that may fail (and return FALSE):
    if (rCachedMethodPermissionsHash.GetValueSpeculative(pKey, &datum))
    {
        pHashValue = reinterpret_cast<MethodSecurityDescriptor*>(datum);
    }
    else
    {
        // Slow call
        pHashValue = reinterpret_cast<MethodSecurityDescriptor*>(LookupSecurityDescriptor_Slow(pDomain, pKey, rCachedMethodPermissionsHash));
    }
    return pHashValue;
}

HashDatum TokenSecurityDescriptor::LookupSecurityDescriptor_Slow(AppDomain* pDomain,
                                                                 void* pKey,   
                                                                 EEPtrHashTable  &rCachedMethodPermissionsHash )
    {
    HashDatum      datum;
    SimpleRWLock* prGlobalLock = pDomain->m_pSecContext->m_prCachedMethodPermissionsLock;
    // look up the cache in the slow mode
        // in the false failure case, we'll recheck the cache anyway
        SimpleReadLockHolder readLockHolder(prGlobalLock);
    if (rCachedMethodPermissionsHash.GetValue(pKey, &datum))
        {
        return datum;
    }
    return NULL;
}
    

HashDatum TokenSecurityDescriptor::InsertSecurityDescriptor(void* pKey, HashDatum pHashDatum)
{
    CONTRACTL {
        THROWS;
        GC_TRIGGERS;
        MODE_ANY;
    } CONTRACTL_END;

	
    AppDomain*      pDomain  = GetAppDomain();
    SimpleRWLock* prGlobalLock = pDomain->m_pSecContext->m_prCachedMethodPermissionsLock;
    EEPtrHashTable  &rCachedMethodPermissionsHash = pDomain->m_pSecContext->m_pCachedMethodPermissionsHash;

    HashDatum pFoundHashDatum = NULL;
    // insert the computed details in our hash table
    {
        SimpleWriteLockHolder writeLockHolder(prGlobalLock);
        // since the hash table doesn't support duplicates by
        // default, we need to recheck in case another thread
        // added the value during a context switch
        if (!rCachedMethodPermissionsHash.GetValue(pKey, &pFoundHashDatum))        
        {
            // no entry was found
            _ASSERTE(pFoundHashDatum == NULL);
            // Place the new entry into the hash.
            rCachedMethodPermissionsHash.InsertValue(pKey, pHashDatum);
        }
    }
    // return the value found in the lookup, in case there was a duplicate
    return pFoundHashDatum;
}


BOOL CheckAssemblyHasSecurityAPTCAttribute(Assembly* pAssembly)
{
    CONTRACTL
    {
        THROWS;
        GC_TRIGGERS;
        INJECT_FAULT(COMPlusThrowOM(););
    }
    CONTRACTL_END

    BOOL fIsMarkedAPTCA = FALSE;

	IMDInternalImport *mdImport = pAssembly->GetManifestImport();    
    GCX_COOP();
	EX_TRY
    {        
		if(mdImport->GetCustomAttributeByName(pAssembly->GetManifestToken(),
                                                        g_SecurityAPTCA,
                                                        NULL,
                                                        NULL) == S_OK)
	 	{
	 		fIsMarkedAPTCA = TRUE;
 		}
	}
    EX_CATCH
	{
    	fIsMarkedAPTCA = FALSE;
    }
	EX_END_CATCH(RethrowTerminalExceptions)	

   return fIsMarkedAPTCA;
}
