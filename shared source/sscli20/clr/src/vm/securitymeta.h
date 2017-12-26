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
// securitymeta.h
//
// pre-computes various security information, declarative and runtime meta-info
//
//--------------------------------------------------------------------------

#ifndef __SECURITYMETA_H__
#define __SECURITYMETA_H__

class SecurityDeclarative;
class SecurityPolicy;
class SecurityStackWalk;
class AssertStackWalk;

#define INVALID_SET_INDEX ((DWORD)~0)

// The enum that describes the value of the SecurityCriticalFlags in SecurityCritical attribute.
enum SecurityCriticalFlags
{
	SecurityCriticalFlags_None = 0,
	SecurityCriticalFlags_All = 0x1
};

static const DWORD DCL_FLAG_MAP[] =
{
    0,                                  // dclActionNil                 = 0
    DECLSEC_REQUESTS,                   // dclRequest                   = 1
    DECLSEC_DEMANDS,                    // dclDemand                    = 2
    DECLSEC_ASSERTIONS,                 // dclAssert                    = 3
    DECLSEC_DENIALS,                    // dclDeny                      = 4
    DECLSEC_PERMITONLY,                 // dclPermitOnly                = 5
    DECLSEC_LINK_CHECKS,                // dclLinktimeCheck             = 6
    DECLSEC_INHERIT_CHECKS,             // dclInheritanceCheck          = 7
    DECLSEC_REQUESTS,                   // dclRequestMinimum            = 8
    DECLSEC_REQUESTS,                   // dclRequestOptional           = 9
    DECLSEC_REQUESTS,                   // dclRequestRefuse             = 10
    0,                                  // dclPrejitGrant               = 11
    0,                                  // dclPrejitDenied              = 12
    DECLSEC_NONCAS_DEMANDS,             // dclNonCasDemand              = 13
    DECLSEC_NONCAS_LINK_DEMANDS,        // dclNonCasLinkDemand          = 14
    DECLSEC_NONCAS_INHERITANCE,         // dclNonCasInheritance         = 15
};
#define DCL_FLAG_MAP_SIZE (sizeof(DCL_FLAG_MAP)/sizeof(DWORD))
#define  DclToFlag(dcl) (((size_t)dcl < DCL_FLAG_MAP_SIZE) ? DCL_FLAG_MAP[dcl] : 0)


struct TokenDeclActionInfo
{
    DWORD           dwDeclAction;   // This'll tell InvokeDeclarativeSecurity whats the action needed
    DWORD           dwSetIndex;     // The index of the cached PermissionSet on which to demand/assert/deny/blah
    TokenDeclActionInfo* pNext;		// pointer to next action link in chain
	
    static TokenDeclActionInfo *Init(DWORD dwAction, DWORD dwSetIndex);
    static void LinkNewDeclAction(TokenDeclActionInfo** ppActionList, CorDeclSecurity action, DWORD dwSetIndex);


   HRESULT GetDeclaredPermissionsWithCache(
								IN CorDeclSecurity action,
								OUT OBJECTREF *pDeclaredPermissions,
								OUT DWORD* pdwSetIndex);

   OBJECTREF GetLinktimePermissions(OBJECTREF *prefNonCasDemands);
   VOID InvokeLinktimeChecks(Assembly* pCaller, BOOL *pfResult, OBJECTREF *pThrowable);
	
};

struct TokenSecurityDescriptor
{
	mdToken _token;
	IMDInternalImport* _mdImport;
	BOOL _fIsCritical;
	BOOL _fIsAllCritical;
	BOOL _fIsTreatAsSafe;
	BOOL  _fIsComputed;

	TokenSecurityDescriptor(mdToken token, IMDInternalImport *mdImport)
	{
		_ASSERTE(mdImport != NULL);
		memset(this, 0, sizeof(TokenSecurityDescriptor));
		_token = token;
		_mdImport = mdImport;
	};

	VOID InitAttributes(bool fIsCritical, bool fIsAllCritical, bool fIsTreatAsSafe)
	{
		_fIsCritical = fIsCritical;
		_fIsAllCritical = fIsAllCritical;
		_fIsTreatAsSafe = fIsTreatAsSafe;
		// mark computed
		_fIsComputed = TRUE;
	}
	
	
	VOID VerifyDataComputed();

	BOOL IsCritical()
	{
		VerifyDataComputed();
		return _fIsCritical;
	}

	BOOL IsTreatAsSafe()
	{
		VerifyDataComputed();
		return _fIsTreatAsSafe;
	}

	BOOL IsAllCritical()
	{
		VerifyDataComputed();
		return _fIsAllCritical;
	}

	// static helper to find cached security descriptors based on token
	static HashDatum LookupSecurityDescriptor(void* pKey);

    static HashDatum TokenSecurityDescriptor::LookupSecurityDescriptor_Slow(AppDomain* pDomain,
                                                                     void* pKey,   
                                                                     EEPtrHashTable  &rCachedMethodPermissionsHash );

	// static helper to insert a security descriptor for a token, dupes not allowed, returns previous entry in hash table
	static HashDatum InsertSecurityDescriptor(void* pKey, HashDatum pHashDatum);
};

struct MethodSecurityDescriptor 
{
	BOOL _fIsCritical;
	BOOL _fIsTreatAsSafe;
	BOOL _fIsComputed;
    BOOL _fIsBuiltInCASPermsOnly;
    BOOL _fIsDemandsOnly;
    BOOL _fAssertAllowed; // Do we know already that the Assembly this MSD belongs to has permission to Assert?
	MethodDesc* _pMD;

	DWORD	_declFlagsDuringPreStub; // declarative run-time security flags,
	DeclActionInfo* _pRuntimeDeclActionInfo; // run-time declarative actions list
	
	TokenDeclActionInfo* _pTokenDeclActionInfo; // link-time declarative actions list
	
	MethodSecurityDescriptor(MethodDesc* pMD)
     :
     _fIsComputed(FALSE),
     _pMD(pMD)
	{
	}
    BOOL CanAssert()
    {
        // No need to do a VerifyDataComputed here -> this value is set by SecurityDeclarative::EnsureAssertAllowed as an optmization
        LEAF_CONTRACT;
        return _fAssertAllowed;
    }
    void SetCanAssert()
	{
        LEAF_CONTRACT;
        _fAssertAllowed = TRUE;
	}
	BOOL HasRuntimeDeclarativeSecurity()
	{
		return _pMD->IsInterceptedForDeclSecurity();
	}

    BOOL HasLinkOrInheritanceDeclarativeSecurity()
    {
         return HasLinktimeDeclarativeSecurity() || HasInheritanceDeclarativeSecurity();
    }
       
	BOOL HasLinktimeDeclarativeSecurity()
	{
		return _pMD->RequiresLinktimeCheck();
	}

	BOOL HasInheritanceDeclarativeSecurity()
	{
            return _pMD->RequiresInheritanceCheck();
	}

	mdToken GetToken()
	{
		return _pMD->GetMemberDef();
	}

	IMDInternalImport *GetIMDInternalImport()
	{
        return _pMD->GetClass()->GetAssembly()->GetManifestImport();
	}


    BOOL ContainsBuiltInCASDemandsOnly()
    {
        VerifyDataComputed();
        return (_fIsBuiltInCASPermsOnly && _fIsDemandsOnly);
    }

	DeclActionInfo* GetRuntimeDeclActionInfo()
	{
		VerifyDataComputed();
		return _pRuntimeDeclActionInfo;
	}

	DWORD GetDeclFlagsDuringPreStub()
	{
		VerifyDataComputed();
		return _declFlagsDuringPreStub;
	}

	TokenDeclActionInfo* GetTokenDeclActionInfo()
	{
		VerifyDataComputed();
		return _pTokenDeclActionInfo;
	}
	
	BOOL IsCritical()
	{
		VerifyDataComputed();
		return _fIsCritical;
	}

	BOOL IsTreatAsSafe()
	{
		VerifyDataComputed();
		return _fIsTreatAsSafe;
	}

        HRESULT GetDeclaredPermissionsWithCache( IN CorDeclSecurity action,
								OUT OBJECTREF *pDeclaredPermissions,
								OUT DWORD* pdwSetIndex)
	{
		WRAPPER_CONTRACT;
	    return GetTokenDeclActionInfo()->GetDeclaredPermissionsWithCache(action, pDeclaredPermissions, pdwSetIndex);
	}
        static HRESULT GetDeclaredPermissionsWithCache(MethodDesc* pMD,
								IN CorDeclSecurity action,
								OUT OBJECTREF *pDeclaredPermissions,
								OUT DWORD* pdwSetIndex)
	{
		WRAPPER_CONTRACT;
	    MethodSecurityDescriptor methodSecurityDesc(pMD);
        LookupOrCreateMethodSecurityDescriptor(&methodSecurityDesc);
	    return methodSecurityDesc.GetDeclaredPermissionsWithCache(action, pDeclaredPermissions, pdwSetIndex);
	}
	
	static OBJECTREF GetLinktimePermissions(MethodDesc* pMD, OBJECTREF *prefNonCasDemands)
	{
	    WRAPPER_CONTRACT;
     	    if (!pMD->RequiresLinktimeCheck()) return NULL;
        MethodSecurityDescriptor methodSecurityDesc(pMD);
        LookupOrCreateMethodSecurityDescriptor(&methodSecurityDesc);
	    return methodSecurityDesc.GetTokenDeclActionInfo()->GetLinktimePermissions(prefNonCasDemands);
	}

	VOID InvokeLinktimeChecks(Assembly* pCaller, BOOL *pfResult, OBJECTREF *pThrowable)
	{
	    WRAPPER_CONTRACT;
        if (!HasLinktimeDeclarativeSecurity()) 
            return;
	     GetTokenDeclActionInfo()->InvokeLinktimeChecks(pCaller, pfResult, pThrowable);
	}

	static VOID InvokeLinktimeChecks(MethodDesc* pMD, Assembly* pCaller, BOOL *pfResult, OBJECTREF *pThrowable)
	{
		WRAPPER_CONTRACT;
		if (!pMD->RequiresLinktimeCheck()) 
          return;
	    MethodSecurityDescriptor methodSecurityDesc(pMD);
        LookupOrCreateMethodSecurityDescriptor(&methodSecurityDesc);
		methodSecurityDesc.InvokeLinktimeChecks(pCaller, pfResult, pThrowable);
	}

    VOID InvokeInheritanceChecks(MethodDesc *pMethod);

    // This method will look for the cached copy of the MethodSecurityDescriptor corresponding to ret_methSecDesc->_pMD
    // If the cache lookup succeeds, we get back the cached copy in ret_methSecDesc
    // If the cache lookup fails, then the data is computed in ret_methSecDesc. If we find that this is a cache-able MSD,
    // a copy is made in AppDomain heap and inserted into the hash table for future lookups.
    static void LookupOrCreateMethodSecurityDescriptor(MethodSecurityDescriptor* ret_methSecDesc);
    static BOOL IsDeclSecurityCASDemandsOnly(DWORD dwMethDeclFlags, mdToken _mdToken, IMDInternalImport *pInternalImport)
    {
        GCX_COOP();
        DWORD tokenSetIndexes[dclMaximumValue + 1];
        SecurityDeclarative::DetectDeclActionsOnToken(_mdToken, dwMethDeclFlags, tokenSetIndexes, pInternalImport);
        SecurityProperties sp(dwMethDeclFlags);
        if (!sp.FDemandsOnly())
            return FALSE;
        
        DWORD dwLocalAction;
        bool builtInCASPermsOnly = TRUE;
        for (dwLocalAction = 0; dwLocalAction <= dclMaximumValue && builtInCASPermsOnly; dwLocalAction++)
        {
            if (tokenSetIndexes[dwLocalAction] != INVALID_SET_INDEX)
            {
                builtInCASPermsOnly = builtInCASPermsOnly && SecurityAttributes::ContainsBuiltinCASPermsOnly(tokenSetIndexes[dwLocalAction], dwLocalAction);
            }
        }
        return (builtInCASPermsOnly); // we only get here if there are only demands...so it suffices to return this value directly
    }
private:
	VOID ComputeCriticalTransparentInfo();
	VOID ComputeRuntimeDeclarativeSecurityInfo();
	VOID ComputeMethodDeclarativeSecurityInfo();
    inline VOID VerifyDataComputed();
    VOID VerifyDataComputedInternal();
    
    static BOOL CanMethodSecurityDescriptorBeCached(MethodDesc* pMD)
    {
        return (pMD->IsInterceptedForDeclSecurity() || 
                pMD->RequiresLinktimeCheck() ||
                pMD->RequiresInheritanceCheck());
            
    }
};

struct FieldSecurityDescriptor
{
	BOOL _fIsCritical;
	BOOL _fIsTreatAsSafe;
	FieldDesc* _pFD;
	BOOL _fIsComputed;
	FieldSecurityDescriptor(FieldDesc* pFD)
	{
		memset(this, 0, sizeof(FieldSecurityDescriptor));
		_pFD = pFD;
	}

	VOID VerifyDataComputed();
	
	BOOL IsCritical()
	{
		VerifyDataComputed();
		return _fIsCritical;
	}

	BOOL IsTreatAsSafe()
	{
		VerifyDataComputed();
		return _fIsTreatAsSafe;
	}
	
};

struct TypeSecurityDescriptor
{
	BOOL _fIsCritical;
	BOOL _fIsAllCritical;
	BOOL _fIsTreatAsSafe;
	BOOL _fIsComputed;
       BOOL _fIsAllTransparent;
	EEClass* _pClass;
	TokenDeclActionInfo* _pTokenDeclActionInfo;
	
	
	TypeSecurityDescriptor(EEClass* pClass)
	{
		memset(this, 0, sizeof(TypeSecurityDescriptor));
		_pClass = pClass;
	}


   BOOL HasLinkOrInheritanceDeclarativeSecurity()
   {
        return HasLinktimeDeclarativeSecurity() || HasInheritanceDeclarativeSecurity();
   }
       
	BOOL HasLinktimeDeclarativeSecurity()
	{
		return _pClass->RequiresLinktimeCheck();
	}

       BOOL HasInheritanceDeclarativeSecurity()
       {
            return _pClass->RequiresInheritanceCheck();
       }
	
	BOOL IsCritical()
	{
		VerifyDataComputed();
		return _fIsCritical;
	}

	BOOL IsAllCritical()
	{
		VerifyDataComputed();
		return _fIsAllCritical;
	}

       BOOL IsAllTransparent()
       {
            VerifyDataComputed();
            return _fIsAllTransparent;
       }
       
	BOOL IsTreatAsSafe()
	{
		VerifyDataComputed();
		return _fIsTreatAsSafe;
	}

	mdToken GetToken()
	{
		return _pClass->GetCl();
	}

	IMDInternalImport *GetIMDInternalImport()
	{
		return _pClass->GetAssembly()->GetManifestImport();
	}	

	TokenDeclActionInfo* GetTokenDeclActionInfo()
	{
		VerifyDataComputed();
		return _pTokenDeclActionInfo;
	}
	


    
     HRESULT GetDeclaredPermissionsWithCache(IN CorDeclSecurity action,
								OUT OBJECTREF *pDeclaredPermissions,
								OUT DWORD* pdwSetIndex)
     {
            return GetTokenDeclActionInfo()->GetDeclaredPermissionsWithCache(action, pDeclaredPermissions, pdwSetIndex);
     }
     
	static HRESULT GetDeclaredPermissionsWithCache(EEClass* pTargetClass,
								IN CorDeclSecurity action,
								OUT OBJECTREF *pDeclaredPermissions,
								OUT DWORD* pdwSetIndex)
	{
		WRAPPER_CONTRACT;
	    TypeSecurityDescriptor* pTypeSecurityDesc = GetTypeSecurityDescriptor(pTargetClass);
	    _ASSERTE(pTypeSecurityDesc != NULL);
	    return pTypeSecurityDesc->GetDeclaredPermissionsWithCache(action, pDeclaredPermissions, pdwSetIndex);
	}

	static OBJECTREF GetLinktimePermissions(EEClass* pClass, OBJECTREF *prefNonCasDemands)
	{
		WRAPPER_CONTRACT;
		 if(!pClass->RequiresLinktimeCheck()) return NULL;
		 TypeSecurityDescriptor* pTypeSecurityDesc = GetTypeSecurityDescriptor(pClass);
	    	_ASSERTE(pTypeSecurityDesc != NULL);    		 
	    	return pTypeSecurityDesc->GetTokenDeclActionInfo()->GetLinktimePermissions(prefNonCasDemands);
	}
	VOID InvokeLinktimeChecks(Assembly* pCaller, BOOL *pfResult, OBJECTREF *pThrowable)
	{
	    WRAPPER_CONTRACT;
		if (!HasLinktimeDeclarativeSecurity()) return;
	     GetTokenDeclActionInfo()->InvokeLinktimeChecks(pCaller, pfResult, pThrowable);
	}
    VOID InvokeInheritanceChecks(EEClass* pClass);

	static VOID InvokeLinktimeChecks(EEClass* pClass, Assembly* pCaller, BOOL *pfResult, OBJECTREF *pThrowable)
	{
		WRAPPER_CONTRACT;
		if(!pClass->RequiresLinktimeCheck()) return;
		GetTypeSecurityDescriptor(pClass)->InvokeLinktimeChecks(pCaller, pfResult, pThrowable);
	}

private:
	VOID ComputeCriticalTransparentInfo();
	VOID ComputeTypeDeclarativeSecurityInfo();
	static TypeSecurityDescriptor* GetTypeSecurityDescriptor(EEClass* pClass);    
    VOID VerifyDataComputedInternal();
	inline VOID VerifyDataComputed();
    static BOOL CanTypeSecurityDescriptorBeCached(EEClass* pClass)
    {
        return  (pClass->RequiresLinktimeCheck() ||
                pClass->RequiresInheritanceCheck());
            
    }

};

// Module security descriptor, this class contains static security information about the module
// this information will get persisted in the NGen image
BOOL CheckAssemblyHasSecurityAPTCAttribute(Assembly* pAssembly);
class ModuleSecurityDescriptor
{
public:
	BOOL		_fIsModuleSecurityChecked;
	BOOL		_fIsAPTCA;
	BOOL		_fIsMarkedTransparent;
	BOOL 		_fIsCritical;
	BOOL 		_fIsAllCritical;
	BOOL 		_fIsTreatAsSafe;


	static BOOL IsMarkedTransparent(Assembly* pAssembly)
	{
		return GetModuleSecurityDescriptor(pAssembly)->IsMarkedTransparent();
	}
	
	static ModuleSecurityDescriptor* GetModuleSecurityDescriptor(Assembly* pAssembly);
		
    PTR_Module      m_pModule;

    ModuleSecurityDescriptor(PTR_Module pModule)
    {
		WRAPPER_CONTRACT;
		_ASSERTE(pModule);
		memset(this, 0, sizeof(ModuleSecurityDescriptor)); 	
		m_pModule = pModule;
    }

    VOID Save(DataImage *image);
    VOID Fixup(DataImage *image);

    VOID VerifyDataComputed();

    BOOL IsMarkedTransparent()
    {
                WRAPPER_CONTRACT;
            VerifyDataComputed();
              return _fIsMarkedTransparent;
    }

	BOOL IsCritical()
	{
		VerifyDataComputed();
		return _fIsCritical;
	}

	BOOL IsAllCritical()
	{
		VerifyDataComputed();
		return _fIsAllCritical;
	}
	BOOL IsTreatAsSafe()
	{
		VerifyDataComputed();
		return _fIsTreatAsSafe;
	}
      
	BOOL IsAPTCA()
	{
		WRAPPER_CONTRACT;
		VerifyDataComputed();
		return _fIsAPTCA;
	}
};

void MethodSecurityDescriptor::VerifyDataComputed()
{
    WRAPPER_CONTRACT;
    if (_fIsComputed)    
        return;

    
    BOOL canMethSecDescCached = CanMethodSecurityDescriptorBeCached(_pMD);
    if (!canMethSecDescCached)
    {
        VerifyDataComputedInternal();
    }
    else
    {
        LookupOrCreateMethodSecurityDescriptor(this);
        _ASSERTE(_fIsComputed);
    }
    
    return;
}

void TypeSecurityDescriptor::VerifyDataComputed()
{
    WRAPPER_CONTRACT;
    if (_fIsComputed)    
        return;

    
    BOOL canTypeSecDescCached = CanTypeSecurityDescriptorBeCached(_pClass);
    if (!canTypeSecDescCached)
    {
        VerifyDataComputedInternal();
    }
    else
    {
        TypeSecurityDescriptor* pCachedTypeSecurityDesc = GetTypeSecurityDescriptor(_pClass);
        *this = *pCachedTypeSecurityDesc; // copy the struct
        _ASSERTE(_fIsComputed);
    }
    
    return;
}

#endif


