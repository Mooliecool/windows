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
/*=============================================================================
**
** Class: ConsoleSpecialKey
**
**
** Purpose: This enumeration represents how a process can be interrupted with
**          a cancel request from the user, like Control-C and Control-Break.
**          We may eventually add in a small number of other cases.
**
**
=============================================================================*/

using System.Runtime.InteropServices;

namespace System {
    [Serializable]

    public enum ConsoleSpecialKey
    {
        // We realize this is incomplete, and may add values in the future.
        ControlC = 0,
        ControlBreak = 1,
    }
}
