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
//============================================================
//
// Class: ISerParser
// Purpose: Interface For DeSerialize Parsers
//
// Date:  Sept 14, 1999
//
//============================================================

namespace System.Runtime.Serialization.Formatters.Soap {

	using System.Runtime.Serialization.Formatters;
	using System.Runtime.Serialization;
	using System;
    internal interface ISerParser
    {
    	void Run();
    }


}
