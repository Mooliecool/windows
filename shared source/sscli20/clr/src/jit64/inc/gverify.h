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

#ifndef __GVERIFY_H
#define __GVERIFY_H

#include "vtypeinfo.h"

class FlowGraphNode;

// global verification structures
typedef struct {
    FlowGraphNode * block;
    int tosIndex;
    int ssaIndex;
} TOSTEMP;

struct tagGLOBALVERIFYDATA;
typedef struct tagGLOBALVERIFYDATA GLOBALVERIFYDATA, * PGLOBALVERIFYDATA;

#define THISUNREACHED -1
#define THISUNINIT    0
#define THISINITED    1

struct tagGLOBALVERIFYDATA {    
    int                         minStack;
    int                         maxStack;
    int                         netStack;
    int                         nTOSTemps;
    TOSTEMP *                   TOSTemps;
    int                         ssaBase;

    int                         stkDepth;
    vertype *                   tiStack;
    bool                        isOnWorklist, blockIsBad;
    PGLOBALVERIFYDATA           worklistPrev, worklistNext;
    FlowGraphNode *             block;
    int                         thisInitialized:2;
};


//typedef struct tagGVWORKLIST {
//    struct tagGVWORKLIST * next;
//    tiPhiNode * phi;
//    typeInfo * type;
//    int op;
//} GVWORKLIST, * PGVWORKLIST;

#endif
