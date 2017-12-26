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
** Interface:  IEnumerable
**
**
** Purpose: Interface for providing generic IEnumerators
**
** 
===========================================================*/
namespace System.Collections.Generic {
    using System;
    using System.Collections;
    using System.Runtime.InteropServices;
    using System.Runtime.CompilerServices;


    [TypeDependencyAttribute("System.SZArrayHelper")]
    public interface IEnumerable<T> : IEnumerable
    {
    // Interfaces are not serializable
        // Returns an IEnumerator for this enumerable Object.  The enumerator provides
        // a simple way to access all the contents of a collection.
        /// <include file='doc\IEnumerable.uex' path='docs/doc[@for="IEnumerable.GetEnumerator"]/*' />
        new IEnumerator<T> GetEnumerator();
    }
}
