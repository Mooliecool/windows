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
** Class:  PropertyToken
**
**
** Propertybuilder is for client to define properties for a class
**
** 
===========================================================*/
namespace System.Reflection.Emit {
    
	using System;
	using System.Reflection;
    using System.Security.Permissions;

	[Serializable()] 
    [System.Runtime.InteropServices.ComVisible(true)]
    public struct PropertyToken {
    
		public static readonly PropertyToken Empty = new PropertyToken();

        internal int m_property;

        internal PropertyToken(int str) {
            m_property=str;
        }
    
        public int Token {
            get { return m_property; }
        }
    	
    	// Satisfy value class requirements
    	public override int GetHashCode()
    	{
    		return m_property;
    	}

    	// Satisfy value class requirements
        public override bool Equals(Object obj)
        {
            if (obj is PropertyToken)
                return Equals((PropertyToken)obj);
            else
                return false;
        }
        
        public bool Equals(PropertyToken obj)
        {
            return obj.m_property == m_property;
        }
    
        public static bool operator ==(PropertyToken a, PropertyToken b)
        {
            return a.Equals(b);
        }
        
        public static bool operator !=(PropertyToken a, PropertyToken b)
        {
            return !(a == b);
        }
    	
    }


}
