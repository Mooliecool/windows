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
** Class:  ParameterToken
**
**
** Purpose: metadata tokens for a parameter
**
** 
===========================================================*/
namespace System.Reflection.Emit {
    
	using System;
	using System.Reflection;
    using System.Security.Permissions;

    // The ParameterToken class is an opaque representation of the Token returned
    // by the Metadata to represent the parameter. 
	[Serializable()]  
    [System.Runtime.InteropServices.ComVisible(true)]
    public struct ParameterToken {
    
		public static readonly ParameterToken Empty = new ParameterToken();
        internal int m_tkParameter;
    
        
        internal ParameterToken(int tkParam) {
            m_tkParameter = tkParam;
        }
    
        public int Token {
            get { return m_tkParameter; }
        }
        
        public override int GetHashCode()
        {
            return m_tkParameter;
        }
        
        public override bool Equals(Object obj)
        {
            if (obj is ParameterToken)
                return Equals((ParameterToken)obj);
            else
                return false;
        }
    
        public bool Equals(ParameterToken obj)
        {
            return obj.m_tkParameter == m_tkParameter;
        }
                
        public static bool operator ==(ParameterToken a, ParameterToken b)
        {
            return a.Equals(b);
        }
        
        public static bool operator !=(ParameterToken a, ParameterToken b)
        {
            return !(a == b);
        }

    }
}
