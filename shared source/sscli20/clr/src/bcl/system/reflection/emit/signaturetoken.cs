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
** Signature:  SignatureToken
**
**
** Purpose: Represents a Signature to the ILGenerator signature.
**
** 
===========================================================*/
namespace System.Reflection.Emit {
    
	using System;
	using System.Reflection;
    using System.Security.Permissions;

    [System.Runtime.InteropServices.ComVisible(true)]
    public struct SignatureToken {
    
		public static readonly SignatureToken Empty = new SignatureToken();

        internal int m_signature;
        internal ModuleBuilder m_moduleBuilder;
          
        internal SignatureToken(int str, ModuleBuilder mod) {
            m_signature=str;
            m_moduleBuilder = mod;
        }
    
        public int Token {
            get { return m_signature; }
        }
    	
    	public override int GetHashCode()
    	{
    		return m_signature;
    	}
    
        public override bool Equals(Object obj)
        {
            if (obj is SignatureToken)
                return Equals((SignatureToken)obj);
            else
                return false;
        }
        
        public bool Equals(SignatureToken obj)
        {
            return obj.m_signature == m_signature;
        }
    
        public static bool operator ==(SignatureToken a, SignatureToken b)
        {
            return a.Equals(b);
        }
        
        public static bool operator !=(SignatureToken a, SignatureToken b)
        {
            return !(a == b);
        }
    	
    }
}
