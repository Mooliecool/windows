//------------------------------------------------------------------------------
// <copyright file="SwitchLevelAttribute .cs" company="Microsoft">
//     
//      Copyright (c) 2006 Microsoft Corporation.  All rights reserved.
//     
//      The use and distribution terms for this software are contained in the file
//      named license.txt, which can be found in the root of this distribution.
//      By using this software in any fashion, you are agreeing to be bound by the
//      terms of this license.
//     
//      You must not remove this notice, or any other, from this software.
//     
// </copyright>                                                                
//------------------------------------------------------------------------------

using System;
using System.Reflection;
using System.Collections;

namespace System.Diagnostics {

    [AttributeUsage(AttributeTargets.Class)]
    public sealed class SwitchLevelAttribute : Attribute {
        private Type type;
        
    	public SwitchLevelAttribute (Type switchLevelType) {
    	    SwitchLevelType = switchLevelType;
        }

    	public Type SwitchLevelType { 
    	    get { return type;}
    	    set { 
    	        if (value == null)
    	            throw new ArgumentNullException("value");
    	        
    	        type = value;
	        }
        }
    }
}
