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
** Class:  TypeToken
**
**
** Purpose: Represents a Class to the ILGenerator class.
**
** 
===========================================================*/
namespace System.Reflection.Emit {
    
	using System;
	using System.Reflection;
	using System.Threading;
    using System.Security.Permissions;

	[Serializable()] 
    [System.Runtime.InteropServices.ComVisible(true)]
    public struct TypeToken {
    
		public static readonly TypeToken Empty = new TypeToken();

        internal int m_class;
    
        
        internal TypeToken(int str) {
            m_class=str;
        }
    
        public int Token {
            get { return m_class; }
        }
        
    	public override int GetHashCode()
    	{
    		return m_class;
    	}
    	
        public override bool Equals(Object obj)
        {
            if (obj is TypeToken)
                return Equals((TypeToken)obj);
            else
                return false;
        }
        
        public bool Equals(TypeToken obj)
        {
            return obj.m_class == m_class;
        }
    
        public static bool operator ==(TypeToken a, TypeToken b)
        {
            return a.Equals(b);
        }
        
        public static bool operator !=(TypeToken a, TypeToken b)
        {
            return !(a == b);
        }
                
    }
}

