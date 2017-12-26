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
// ---------------------------------------------------------------------------
// APIThreadStress.h  (API thread stresser)
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
// This class provides a simple base to wrap "thread stress" logic around an API,
// which will (in thread stress mode) cause an API to "fork" onto many threads
// executing the same operation simulatenously.  This can help to expose race
// conditions.
//
// Usage:
//
// First, subtype APIThreadStress and override Invoke to implement the operation.
// You will likely need to add data members for the arguments.
//
// Next, inside the API, write code like this:
//
// void MyRoutine(int a1, void *a2)
// {
//      class stress : APIThreadStress
//      {
//          int a1;
//          void *a2;
//          stress(int a1, void *a2) : a1(a1), a2(a2) 
//               { DoThreadStress(); }
//          void Invoke() { MyRoutine(a1, a2); }
//      } ts (a1, a2);
//
//      // implementation
//
//      // perhaps we have a common sub-point in the routine where we want the threads to 
//      // queue up and race again
//
//      ts.SyncThreadStress();
//
//      // more implementation    
//  }
// ---------------------------------------------------------------------------

#ifndef _APITHREADSTRESS_H_
#define _APITHREADSTRESS_H_

#include "utilcode.h"

class APIThreadStress
{
 public:
    APIThreadStress();
    ~APIThreadStress();

    BOOL DoThreadStress();
    static void SyncThreadStress();

    static void SetThreadStressCount(int count);

 protected:
    virtual void Invoke() {LEAF_CONTRACT;};

 private:
    static DWORD WINAPI StartThread(void *arg);

    static int s_threadStressCount;     

    int       m_threadCount;
    HANDLE    *m_hThreadArray;
    BOOL      m_setupOK;
    LONG      m_runCount;
    HANDLE    m_syncEvent;

};

#endif  // _APITHREADSTRESS_H_
