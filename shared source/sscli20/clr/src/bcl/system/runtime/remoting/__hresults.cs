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
//=============================================================================
//
// Class: __HResults
//
// Purpose: Define HResult constants. Every exception has one of these.
//
// Date: 98/08/31 11:57:11 AM
//
//===========================================================================*/
namespace System.Runtime.Remoting {
	using System;
    internal sealed class __HResults
    {
		public const int COR_E_REMOTING = unchecked((int)0x8013150B);
		public const int COR_E_SERVER = unchecked((int)0x8013150E);		
    }
}
