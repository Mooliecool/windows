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
/*===========================================================================
**
** File:    SpecialStatics.h
**       
**
** Purpose: Defines the data structures for thread relative, context relative
**          statics.
**          
**
** Date:    Feb 28, 2000
**
=============================================================================*/
#ifndef _H_SPECIALSTATICS_
#define _H_SPECIALSTATICS_

class AppDomain;

// Data structure for storing special static data like thread relative or
// context relative static data.
typedef struct _STATIC_DATA
{
    DWORD           cElem;
    LPVOID          dataPtr[0];

#ifdef DACCESS_COMPILE
    static ULONG32 DacSize(TADDR addr)
    {
        DWORD cElem = *PTR_DWORD(addr);
        return offsetof(struct _STATIC_DATA, dataPtr) +
            cElem * sizeof(TADDR);
    }

    void EnumMemoryRegions(CLRDataEnumMemoryFlags flags);
#endif
    
} STATIC_DATA;
typedef SPTR(STATIC_DATA) PTR_STATIC_DATA;

typedef SimpleList<OBJECTHANDLE> ObjectHandleList;

    
typedef struct _STATIC_DATA_LIST
{
    PTR_STATIC_DATA             m_pUnsharedStaticData;
    PTR_STATIC_DATA             m_pSharedStaticData;

    //
    // List of pinned value type statics (We need to track these, as we can't be leaking them as
    // threads get created/destroyed).
    //
    ObjectHandleList            m_PinnedThreadStatics;
} STATIC_DATA_LIST;
typedef DPTR(STATIC_DATA_LIST) PTR_STATIC_DATA_LIST;

#endif
