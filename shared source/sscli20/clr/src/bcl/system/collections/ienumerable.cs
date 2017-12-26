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
** Purpose: Interface for classes providing IEnumerators
**
** 
===========================================================*/
namespace System.Collections {
	using System;
	using System.Runtime.InteropServices;
    // Implement this interface if you need to support VB's foreach semantics.
    // Also, COM classes that support an enumerator will also implement this interface.
    [Guid("496B0ABE-CDEE-11d3-88E8-00902754C43A")]
[System.Runtime.InteropServices.ComVisible(true)]
    public interface IEnumerable
    {
	// Interfaces are not serializable
        // Returns an IEnumerator for this enumerable Object.  The enumerator provides
        // a simple way to access all the contents of a collection.
        [DispId(-4)]
        IEnumerator GetEnumerator();
    }
}
