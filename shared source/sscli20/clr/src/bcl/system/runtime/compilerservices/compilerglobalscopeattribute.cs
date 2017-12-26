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
** Class:  CompilerGlobalScopeAttribute
**
**
** Purpose: Attribute used to communicate to the VS7 debugger
**          that a class should be treated as if it has
**          global scope.
**
** 
===========================================================*/
    

namespace System.Runtime.CompilerServices
{
	[Serializable, AttributeUsage(AttributeTargets.Class)]
[System.Runtime.InteropServices.ComVisible(true)]
    public class CompilerGlobalScopeAttribute : Attribute
	{
	   public CompilerGlobalScopeAttribute () {}
	}
}

