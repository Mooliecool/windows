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
    
	using System;
    //  This value type is used for constructing System.ArgIterator. 
    // 
    //  SECURITY : m_ptr cannot be set to anything other than null by untrusted
    //  code.  
    // 
    //  This corresponds to EE VARARGS cookie.

	 // Cannot be serialized
[System.Runtime.InteropServices.ComVisible(true)]
    public struct RuntimeArgumentHandle
    {
        private IntPtr m_ptr;

        internal IntPtr Value { get { return m_ptr; } }
    }

}
