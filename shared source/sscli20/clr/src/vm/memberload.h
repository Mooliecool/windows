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
// ===========================================================================
// File: MEMBERLOAD.H
//
#ifndef _MEMBERLOAD_H
#define _MEMBERLOAD_H


/*
 *  Include Files
 */
#include "eecontract.h"
#include "argslot.h"
#include "vars.hpp"
#include "cor.h"
#include "clrex.h"
#include "hash.h"
#include "crst.h"
#include "declsec.h"
#include "list.h"
#include "typehandle.h"
#include "methodtable.h"
#include "typectxt.h"


// The MemberLoader logic is analogous to the ClassLoader logic, i.e. it turn 
// tokens into internal EE descriptors.
//
// The implementations of these functions currently lies in class.cpp.
class MemberLoader
{


public:
    static void DECLSPEC_NORETURN ThrowMissingMethodException(MethodTable* pMT,
                                            LPCSTR szMember,
                                            Module *pModule,
                                            PCCOR_SIGNATURE pSig,
                                            DWORD cSig,
                                            const SigTypeContext *pTypeContext);

    static void DECLSPEC_NORETURN ThrowMissingFieldException( MethodTable *pMT,
                                            LPCSTR szMember);


    static void* GetDescFromMemberDefOrRefThrowing(
                                        Module *pModule,            // Scope for the memberRef and mdEnclosingRef
                                        mdMemberRef MemberRef,      // MemberRef to resolve
                                        BOOL *pfIsMethod,           // Returns TRUE if **ppDesc is a MethodDesc, FALSE if it is a FieldDesc
                                        const SigTypeContext *pTypeContext,     // Context for type parameters in the parent type spec
                                        BOOL strictMetadataChecks,  // Throw an exception if no generic method args given for a generic method, otherwise return the 'generic' instantiation
                                        DWORD nGenericMethodArgs = 0, // Used only from GetMethodDescFromMemberDefOrRefOrSpecThrowing to report the parameters of the generic method
                                        TypeHandle *genericMethodArgs = NULL,  // Used only from GetMethodDescFromMemberDefOrRefOrSpecThrowing to report the parameters of the generic method
                                        BOOL allowInstParam = FALSE,
                                        ClassLoadLevel level = CLASS_LOADED);

    static  MethodDesc* GetMethodDescFromMemberDefOrRefOrSpecThrowing(Module *pModule,
                                                                      mdToken MemberRefOrDefOrSpec,
                                                                      const SigTypeContext *pTypeContext, // Context for type parameters in any parent TypeSpec and in the instantiation in a MethodSpec 
                                                                      BOOL strictMetadataChecks,  // Normally true - the zapper is one exception.  Throw an exception if no generic method args given for a generic method, otherwise return the 'generic' instantiation
                                                                      BOOL allowInstParam);


    static HRESULT GetMethodDescFromMemberDefOrRefOrSpecNT(Module *pModule,
                                                           mdMemberRef MemberRef,
                                                           MethodDesc** pDesc,
                                                           OBJECTREF* pThrowable,
                                                           const SigTypeContext *pTypeContext, 
                                                           BOOL strictMetadataChecks,  // Normally true - the zapper is one exception.  Throw an exception if no generic method args given for a generic method, otherwise return the 'generic' instantiation
                                                           BOOL allowInstParam);
    
    static  FieldDesc* GetFieldDescFromMemberRefThrowing(Module *pModule,
                                                         mdMemberRef MemberRef,
                                                         const SigTypeContext *pTypeContext,
                                                         BOOL strictMetadataChecks = TRUE);

    static HRESULT GetFieldDescFromMemberRefNT(Module *pModule,
                                               mdMemberRef MemberRef,          // MemberRef or FieldDef token
                                               FieldDesc **ppFieldDesc,
                                               OBJECTREF* pThrowable,
                                               const SigTypeContext *pTypeContext); // Type context for type parameters in parent class of member-ref

    
    static MethodDesc *GetMethodDescFromMethodDefThrowing(Module *pModule,
                                                          mdMethodDef MethodDef,                // MethodDef token
                                                          DWORD numGenericTypeArgs,         // Number of generic type arguments
                                                          TypeHandle *genericTypeArgs,   // Generic arguments for declaring class
                                                          DWORD nGenericMethodArgs,       // Number of generic method arguments
                                                          TypeHandle *genericMethodArgs); // Generic arguments for declaring method

};


#endif // MEMBERLOAD_H
