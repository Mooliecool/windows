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
/***************************************************************************/
/* Name value pair (both strings) which can be linked into a list of pairs */

#ifndef NVPAIR_H
#define NVPAIR_H

#include "binstr.h"

class NVPair
{
public:

    NVPair(BinStr *name, BinStr *value)
    {
        m_Name = name;
        m_Value = value;
        m_Tail = NULL;
    }

    ~NVPair()
    {
        delete m_Name;
        delete m_Value;
        delete m_Tail;
    }

    NVPair *Concat(NVPair *list)
    {
        m_Tail = list;
        return this;
    }

    BinStr *Name() { return m_Name; }
    BinStr *Value() { return m_Value; }
    NVPair *Next() { return m_Tail; }

private:
    BinStr *m_Name;
    BinStr *m_Value;
    NVPair *m_Tail;
};

#endif
