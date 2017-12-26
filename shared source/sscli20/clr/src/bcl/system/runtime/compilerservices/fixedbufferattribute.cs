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
** Class:  FixedBufferAttribute
**
** Purpose: Used by a compiler for generating value types
**   in-place within other value types containing a certain
**   number of elements of the given (primitive) type.  Somewhat
**   similar to P/Invoke's ByValTStr attribute.
**   Used by C# with this syntax: "fixed int buffer[10];"
**
===========================================================*/
using System;

namespace System.Runtime.CompilerServices
{
	[AttributeUsage(AttributeTargets.Field, Inherited=false)]
	public sealed class FixedBufferAttribute : Attribute
	{
		private Type elementType;
		private int length;

		public FixedBufferAttribute(Type elementType, int length)
		{
			this.elementType = elementType;
			this.length = length;
		}

		public Type ElementType {
			get {
				return elementType;
			}
		}

		public int Length {
			get {
				return length;
			}
		}
	}
}
