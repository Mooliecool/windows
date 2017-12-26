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
** Class:  MethodToken
**
**
** Purpose: Represents a Method to the ILGenerator class.
**
** 
===========================================================*/
namespace System.Reflection.Emit {
    
	using System;
	using System.Reflection;
    using System.Security.Permissions;

	[Serializable()] 
    [System.Runtime.InteropServices.ComVisible(true)]
    public struct MethodToken
    {
		public static readonly MethodToken Empty = new MethodToken();
        internal int m_method;
            
        internal MethodToken(int str) {
            m_method=str;
        }
    
        public int Token {
            get { return m_method; }
        }
        
    	public override int GetHashCode()
    	{
    		return m_method;
    	}

        public override bool Equals(Object obj)
        {
            if (obj is MethodToken)
                return Equals((MethodToken)obj);
            else
                return false;
        }
        
        public bool Equals(MethodToken obj)
        {
            return obj.m_method == m_method;
        }
        
        public static bool operator ==(MethodToken a, MethodToken b)
        {
            return a.Equals(b);
        }
        
        public static bool operator !=(MethodToken a, MethodToken b)
        {
            return !(a == b);
        }
    	
    }
}
