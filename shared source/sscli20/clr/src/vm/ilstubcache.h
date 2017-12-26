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

#ifdef _MSC_VER
#pragma once
#endif // _MSC_VER
#ifndef _ILSTUBCACHE_H
#define _ILSTUBCACHE_H


#include "vars.hpp"
#include "util.hpp"
#include "crst.h"

class ILStubHashBlobBase
{
public:
    INT_PTR m_cbSizeOfBlob;  // this is size of entire object!!
};

class ILStubHashBlob : public ILStubHashBlobBase
{
public:
    BYTE    m_rgbBlobData[1];
};


//
// this class caches MethodDesc's for dynamically generated IL stubs
//
class ILStubCache : private CClosedHashBase
{
private:
    //---------------------------------------------------------
    // Hash entry for CClosedHashBase.
    //---------------------------------------------------------
    struct ILCHASHENTRY
    {
        // Values:
        //   NULL  = free
        //   -1    = deleted
        //   other = used
        MethodDesc*     m_pMethodDesc;
        ILStubHashBlob* m_pBlob;
    };

public:

    //---------------------------------------------------------
    // Constructor
    //---------------------------------------------------------
    ILStubCache(LoaderHeap* heap = NULL);

    //---------------------------------------------------------
    // Destructor
    //---------------------------------------------------------
    ~ILStubCache();

    void Init(LoaderHeap* pHeap);

    MethodDesc* GetStubMethodDesc(
        ILStubHashBlob* pParams,
        MethodTable* pMT,
        DWORD dwStubFlags,      // bitmask of NDirectStubFlags
        Module* pSigModule, 
        PCCOR_SIGNATURE pSig, 
        DWORD cbSig);  

    static MethodDesc* CreateNewMethodDesc(
        LoaderHeap* pCreationHeap,
        MethodTable* pMT,
        DWORD dwStubFlags,      // bitmask of NDirectStubFlags
        Module* pSigModule, 
        PCCOR_SIGNATURE pSig, 
        DWORD cbSig,
        bool fIsSecurityStub = false);  

private:

    MethodTable* GetStubMethodTable(Module* pRepresentativeModule);


    // *** OVERRIDES FOR CClosedHashBase ***/

    //*****************************************************************************
    // Hash is called with a pointer to an element in the table.  You must override
    // this method and provide a hash algorithm for your element type.
    //*****************************************************************************
    virtual unsigned long Hash(             // The key value.
        void const*  pData);                // Raw data to hash.
    
    //*****************************************************************************
    // Compare is used in the typical memcmp way, 0 is eqaulity, -1/1 indicate
    // direction of miscompare.  In this system everything is always equal or not.
    //*****************************************************************************
    virtual unsigned long Compare(          // 0, -1, or 1.
        void const*  pData,                 // Raw key data on lookup.
        BYTE*        pElement);             // The element to compare data against.
    
    //*****************************************************************************
    // Return true if the element is free to be used.
    //*****************************************************************************
    virtual ELEMENTSTATUS Status(           // The status of the entry.
        BYTE*        pElement);             // The element to check.

    //*****************************************************************************
    // Sets the status of the given element.
    //*****************************************************************************
    virtual void SetStatus(
        BYTE*         pElement,             // The element to set status for.
        ELEMENTSTATUS eStatus);             // New status.
    
    //*****************************************************************************
    // Returns the internal key value for an element.
    //*****************************************************************************
    virtual void* GetKey(                   // The data to hash on.
        BYTE*        pElement);             // The element to return data ptr for.

private:
    Crst            m_crst;
    LoaderHeap*     m_heap;
    MethodTable*    m_pStubMT;
};

#endif //_ILSTUBCACHE_H

