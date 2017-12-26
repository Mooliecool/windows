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

namespace System {

    // This enum is used to indentify DateTime instances in cases when they are known to be in local time, 
    // UTC time or if this information has not been specified or is not applicable.

    [Serializable]
[System.Runtime.InteropServices.ComVisible(true)]
    public enum DateTimeKind
    {
        Unspecified = 0,
        Utc = 1,
        Local = 2,
    }
}
