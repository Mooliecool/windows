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
/*============================================================
**
** Class: COMMemoryFailPoint
**
**                                                    
**
** Purpose: Native methods for System.Runtime.MemoryFailPoint.
** These are to implement memory gates to limit allocations
** when progress will likely result in an OOM.
**
** Date:  September 9, 2004
**
===========================================================*/

#ifndef _COMMEMORYFAILPOINT_H
#define _COMMEMORYFAILPOINT_H

#include "fcall.h"

class COMMemoryFailPoint
{
public:
    static FCDECL2(void, GetMemorySettings, UINT32* pMaxGCSegmentSize, UINT64* pTopOfMemory);
};

#endif // _COMMEMORYFAILPOINT_H
