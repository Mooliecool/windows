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
/*=============================================================================
**
** Class: CLSCompliantAttribute
**
**
** Purpose: Container for assemblies.
**
**
=============================================================================*/

namespace System {
    [AttributeUsage (AttributeTargets.All, Inherited=true, AllowMultiple=false),Serializable]  
[System.Runtime.InteropServices.ComVisible(true)]
    public sealed class CLSCompliantAttribute : Attribute 
	{
		private bool m_compliant;

		public CLSCompliantAttribute (bool isCompliant)
		{
			m_compliant = isCompliant;
		}
		public bool IsCompliant 
		{
			get 
			{
				return m_compliant;
			}
		}
	}
}
