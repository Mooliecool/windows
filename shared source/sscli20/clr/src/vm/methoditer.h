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

#ifndef _METHODDESCITER_H_
#define _METHODDESCITER_H_

#include "instmethhash.h"
#include "method.hpp"
#include "appdomain.hpp"
#include "domainfile.h"
#include "typehash.h"


// Iterate all the currently loaded instantiations of a mdMethodDef 
// in a given AppDomain.  Can be used for both generic + nongeneric methods.
// This may give back duplicate entries; and it may give back some extra
// MethodDescs for which IsJitted() returns false. 
// Regarding EnC: MethodDescs only match the latest version in an EnC case.
// Thus this iterator does not go through all previous EnC versions.

// This iterator is almost a nop for the non-generic case.
// This is currently not an efficient implementation for the generic case 
// as we search every entry in every item in the ParamTypes and/or InstMeth tables.
// It is possible we may have
// to make this more efficient, but it should not be used very often (only
// when debugging prejitted generic code, and then only when updating
// methodInfos after the load of a new module, and also when fetching 
// the native code ranges for generic code).  
class LoadedMethodDescIterator
{
    
    Module *m_module;
    mdMethodDef m_md;
    MethodDesc *m_mainMD;
    AppDomain * m_pAppDomain;
 
    // The following hold the state of the iteration....
    // Yes we iterate everything for the moment - we need
    // to get every single module.  Ideally when finding debugging information
    // we should only iterate freshly added modules.  We would also like to only
    // iterate the relevant entries of the hash tables but that means changing the
    // hash functions.
    AppDomain::AssemblyIterator m_assemIterator;
    DomainModuleIterator m_moduleIterator;
    EETypeHashTable::Iterator m_typeIterator;
    EETypeHashEntry *m_typeIteratorEntry;
    BOOL m_startedNonGenericType;
    InstMethodHashTable::Iterator m_methodIterator;
    InstMethodHashEntry *m_methodIteratorEntry;
    BOOL m_startedNonGenericMethod;
    BOOL m_fFirstTime;
        
public:
    BOOL Next();
    MethodDesc *Current();
    void Start(AppDomain * pAppDomain, Module *pModule, mdMethodDef md);
    void Start(AppDomain * pAppDomain, Module *pModule, mdMethodDef md, MethodDesc *pDesc);
    
    LoadedMethodDescIterator(AppDomain * pAppDomain, Module *pModule,
                             mdMethodDef md)
    {
        WRAPPER_CONTRACT;
        Start(pAppDomain, pModule, md);
    }
    LoadedMethodDescIterator(void);
};


#endif // _METHODDESCITER_H_
