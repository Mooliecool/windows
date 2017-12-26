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
	[AttributeUsage(AttributeTargets.Assembly | AttributeTargets.Class | AttributeTargets.Struct | AttributeTargets.Method | AttributeTargets.Parameter | AttributeTargets.Field | AttributeTargets.Property | AttributeTargets.Event | AttributeTargets.Interface | AttributeTargets.Enum | AttributeTargets.Delegate,
		AllowMultiple = true, Inherited = false)]
[System.Runtime.InteropServices.ComVisible(true)]
	public sealed class ObfuscationAttribute: Attribute
	{
		private bool m_strip = true;
		private bool m_exclude = true;
		private bool m_applyToMembers = true;
		private string m_feature = "all";

		public ObfuscationAttribute()
		{
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

		public bool Exclude
		{
			get
			{
				return m_exclude;
			}
			set
			{
				m_exclude = value;
			}
		}

		public bool ApplyToMembers
		{
			get
			{
				return m_applyToMembers;
			}
			set
			{
				m_applyToMembers = value;
			}
		}

		public string Feature
		{
			get
			{
				return m_feature;
			}
			set
			{
				m_feature = value;
			}
		}
	}
}

