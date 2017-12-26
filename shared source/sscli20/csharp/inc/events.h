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
// File: events.h
//
// ===========================================================================

#ifndef __events_h__
#define __events_h__

#include "nodes.h"


enum ChangeType
{
    CT_ADD,
    CT_REMOVE,
    CT_RENAME,
    CT_UNKNOWN,
    CT_BASECHANGE,
    CT_SIGCHANGE,
    CT_TYPEREFCHANGE,
    CT_ARGCHANGE
};

struct EVENTNODE {
public:
    BASENODE *pNode;
    BASENODE *pParentNode;
    EVENTNODE *pNext;
    ChangeType ctType;
};


#endif // __events_h__
