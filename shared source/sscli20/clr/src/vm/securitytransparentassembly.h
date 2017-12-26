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
// securityTransparentAssembly.h
//
// Implementation for transparent code feature
//
//--------------------------------------------------------------------------

#ifndef __SECURITYTRANSPARENT_H__
#define __SECURITYTRANSPARENT_H__



class SecurityTransparent
{
	friend class SecurityDeclarative;
	friend class SecurityStackWalk;
	friend class SecurityPolicy;
	friend class AssertStackWalk;
	friend class Security;
	friend class SecurityMethodDesc;
	
private:
	static BOOL IsFieldTransparent(FieldDesc* pCallerField);
	static BOOL IsMethodTransparent(MethodDesc* pCallerMD);
	static BOOL IsAssemblyTransparent(Assembly* pAssem);
	
public:

	static BOOL CheckAssemblyHasSecurityTransparentAttribute(Assembly* pAssembly);
	static CorInfoIsCallAllowedResult RequiresTransparentCodeChecks(MethodDesc* pCaller, MethodDesc* pCallee);
	static CorInfoIsCallAllowedResult RequiresTransparentAssemblyChecks(MethodDesc* pCaller, MethodDesc* pCallee);
	static void EnforceTransparentAssemblyChecks(MethodDesc* pCaller, MethodDesc* pCallee);
	static CorInfoCanSkipVerificationResult JITCanSkipVerification(DomainAssembly * pAssembly, BOOL fQuickCheckOnly);
	static CorInfoCanSkipVerificationResult JITCanSkipVerification(MethodDesc * pMD, BOOL fQuickCheckOnly);
	static VOID PerformTransparencyChecksForLoadByteArray(MethodDesc* pCallersMD, AssemblySecurityDescriptor* pLoadedSecDesc);
	static BOOL CheckNonPublicCriticalAccess(MethodDesc* pCurrentMD,
                        MethodDesc* pOptionalTargetMethod, 
                        FieldDesc* pOptionalTargetField,
                        MethodTable * pOptionalTargetType);

};

#endif // __SECURITYTRANSPARENT_H__

