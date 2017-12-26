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

// MLCACHE.H -
//
// Base class for caching ML stubs.
//


#ifndef __mlcache_h__
#define __mlcache_h__


#include "vars.hpp"
#include "util.hpp"
#include "crst.h"

class Stub;
class StubLinker;

class MLStubCache : private CClosedHashBase
{
private:
    //---------------------------------------------------------
    // Hash entry for CClosedHashBase.
    //---------------------------------------------------------
    struct MLCHASHENTRY
    {
        // Values:
        //   NULL  = free
        //   -1    = deleted
        //   other = used
        Stub    *m_pMLStub;

        // Offset where the RawMLStub begins (the RawMLStub can be
        // preceded by native stub code.)
        UINT16   m_offsetOfRawMLStub;

        // See MLStubCompilationMode enumeration.
        UINT16   m_compilationMode;
    };


public:
    //---------------------------------------------------------
    // Describes the compiled state of an ML stub.
    //---------------------------------------------------------
    enum MLStubCompilationMode
    {
        STANDALONE   = 0, // Completely compiled to a native stub
        INTERPRETED  = 1  // Not compiled - must interpret the ML code
    };

    //---------------------------------------------------------
    // Constructor
    //---------------------------------------------------------
    MLStubCache(LoaderHeap *heap = 0);

    //---------------------------------------------------------
    // Destructor
    //---------------------------------------------------------
    ~MLStubCache();

    //---------------------------------------------------------
    // Returns the equivalent hashed Stub, creating a new hash
    // entry if necessary. If the latter, will call out to CompileMLStub.
    //
    // On successful exit, *pMode is set to describe
    // the compiled nature of the MLStub.
    //
    // callerContext can be used by the caller to push some context through
    // to the compilation routine.
    //
    // Throws on out of memory or other fatal error.
    //---------------------------------------------------------
    Stub *Canonicalize(const BYTE *pRawMLStub, MLStubCompilationMode *pMode, void *callerContext = 0);

    //---------------------------------------------------------
    // Call this occasionally to get rid of unused stubs.
    //---------------------------------------------------------
    VOID FreeUnusedStubs();

protected:
    //---------------------------------------------------------
    // OVERRIDE.
    // Compile a native (ASM) version of the ML stub.
    //
    // This method should compile into the provided stublinker (but
    // not call the Link method.)
    //
    // It should return the chosen compilation mode.
    //
    // If the method fails for some reason, it should return
    // INTERPRETED so that the EE can fall back on the already
    // created ML code.
    //---------------------------------------------------------
    virtual MLStubCompilationMode CompileMLStub(const BYTE *pRawMLStub,
                                                StubLinker *psl,
                                                void *callerContext) = 0;

    //---------------------------------------------------------
    // OVERRIDE
    // Tells the MLStubCache the length of an ML stub.
    //---------------------------------------------------------
    virtual UINT Length(const BYTE *pRawMLStub) = 0;

    //---------------------------------------------------------
    // OVERRIDE (OPTIONAL)
    // Notifies the various derived classes that a new stub has been created
    //---------------------------------------------------------
    virtual void AddStub(const BYTE* pRawMLStub, Stub* pNewStub);


private:
    static BOOL DeleteLoopFunc(BYTE *pEntry, LPVOID);
    static BOOL ForceDeleteLoopFunc(BYTE *pEntry, LPVOID);


    // *** OVERRIDES FOR CClosedHashBase ***/

    //*****************************************************************************
    // Hash is called with a pointer to an element in the table.  You must override
    // this method and provide a hash algorithm for your element type.
    //*****************************************************************************
    virtual unsigned long Hash(             // The key value.
        void const  *pData);                // Raw data to hash.
    
    //*****************************************************************************
    // Compare is used in the typical memcmp way, 0 is eqaulity, -1/1 indicate
    // direction of miscompare.  In this system everything is always equal or not.
    //*****************************************************************************
    virtual unsigned long Compare(          // 0, -1, or 1.
        void const  *pData,                 // Raw key data on lookup.
        BYTE        *pElement);             // The element to compare data against.
    
    //*****************************************************************************
    // Return true if the element is free to be used.
    //*****************************************************************************
    virtual ELEMENTSTATUS Status(           // The status of the entry.
        BYTE        *pElement);             // The element to check.

    //*****************************************************************************
    // Sets the status of the given element.
    //*****************************************************************************
    virtual void SetStatus(
        BYTE        *pElement,              // The element to set status for.
        ELEMENTSTATUS eStatus);             // New status.
    
    //*****************************************************************************
    // Returns the internal key value for an element.
    //*****************************************************************************
    virtual void *GetKey(                   // The data to hash on.
        BYTE        *pElement);             // The element to return data ptr for.




private:
    Crst        m_crst;
    LoaderHeap* m_heap;
};


#endif // __mlcache_h__

