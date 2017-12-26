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
//  SecurityZone.cs
//
//  Enumeration of the zones code can come from
//

namespace System.Security {
	using System;
	[Serializable]
[System.Runtime.InteropServices.ComVisible(true)]
    public enum SecurityZone
    {
        // Note: this information is referenced in $/Com99/src/vm/security.cpp
        // in ApplicationSecurityDescriptor.GetEvidence().
    
        MyComputer = 0,
        Intranet     = 1,
        Trusted      = 2,
        Internet     = 3,
        Untrusted    = 4,
    
        NoZone       = -1,  // No Zone Information
    }
}
