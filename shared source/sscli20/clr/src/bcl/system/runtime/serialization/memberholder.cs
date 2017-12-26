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
** Class: MemberHolder
**
**
** Purpose: This is a lightweight class designed to hold the members 
**          and StreamingContext for a particular class.
**
**
============================================================*/
namespace System.Runtime.Serialization {

	using System.Runtime.Remoting;
	using System;
	using System.Reflection;
	[Serializable()]
    internal class MemberHolder {
        internal MemberInfo[] members = null;
        internal Type memberType;
        internal StreamingContext context;
        
        internal MemberHolder(Type type, StreamingContext ctx) {
            memberType = type;
            context = ctx;
        }
    
        public override int GetHashCode() {
            return memberType.GetHashCode();
        }
    
        public override bool Equals(Object obj) {
            if (!(obj is MemberHolder)) {
                return false;
            }
            
            MemberHolder temp = (MemberHolder)obj;
    
            if (temp.memberType==memberType && temp.context.State == context.State) {
                return true;
            }
            
            return false;
        }
    }
}
