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

using System;
using System.Reflection;


namespace System.Reflection
{
	[AttributeUsage (AttributeTargets.Assembly, AllowMultiple=false, Inherited=false)]
[System.Runtime.InteropServices.ComVisible(true)]
	public sealed class ObfuscateAssemblyAttribute : Attribute
	{
		private bool m_assemblyIsPrivate;
		private bool m_strip = true;

		public ObfuscateAssemblyAttribute(bool assemblyIsPrivate)
		{
			m_assemblyIsPrivate = assemblyIsPrivate;
		}

		public bool AssemblyIsPrivate
		{
			get
			{
				return m_assemblyIsPrivate;
			}
		}

		public bool StripAfterObfuscation
		{
			get
			{
				return m_strip;
			}
			set
			{
				m_strip = value;
			}
		}
	}
}

