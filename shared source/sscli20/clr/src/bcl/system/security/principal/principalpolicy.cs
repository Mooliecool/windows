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

//
//  PrincipalPolicy.cs
//
//  Enum describing what type of principal to create by default (assuming no
//  principal has been set on the AppDomain).
//

namespace System.Security.Principal
{
    [Serializable]
[System.Runtime.InteropServices.ComVisible(true)]
    public enum PrincipalPolicy {
        // Note: it's important that the default policy has the value 0.
        UnauthenticatedPrincipal = 0,
        NoPrincipal = 1,
    }
}
