//------------------------------------------------------------------------------
// <copyright file="SwitchAttribute.cs" company="Microsoft">
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

    [AttributeUsage(AttributeTargets.Assembly | AttributeTargets.Class | AttributeTargets.Constructor |
                    AttributeTargets.Event | AttributeTargets.Method | AttributeTargets.Property)]
    public sealed class SwitchAttribute : Attribute {
        private Type type;
        private string name;
        private string description;

    	public SwitchAttribute (string switchName, Type switchType) {
    	    SwitchName = switchName;
    	    SwitchType = switchType;
        }

    	public string SwitchName {
    	    get { return name; }
    	    set {
    	        if (value == null)
                    throw new ArgumentNullException("value");
                if (value.Length == 0)
    	            throw new ArgumentException(SR.GetString(SR.InvalidNullEmptyArgument, "value"), "value");

    	        name = value;
	        }
        }

    	public Type SwitchType {
    	    get { return type; }
    	    set {
    	        if (value == null)
    	            throw new ArgumentNullException("value");
    	        type = value;
	        }
        }

    	public string SwitchDescription {
    	    get { return description; }
    	    set { description = value;}
        }

    	public static SwitchAttribute[] GetAll(Assembly assembly) {
    	    if (assembly == null)
    	        throw new ArgumentNullException("assembly");

    	    ArrayList  switchAttribs = new ArrayList ();

    	    object[] attribs = assembly.GetCustomAttributes(typeof(SwitchAttribute), false);
    	    switchAttribs.AddRange(attribs);

    	    Type[] types = assembly.GetTypes();
    	    for (int i=0; i<types.Length; i++)
    	        GetAllRecursive(types[i], switchAttribs);

            SwitchAttribute[] ret = new SwitchAttribute[switchAttribs.Count];
            switchAttribs.CopyTo(ret, 0);
    	    return ret;
        }

    	static void GetAllRecursive(Type type, ArrayList   switchAttribs) {
            GetAllRecursive((MemberInfo) type, switchAttribs);
    	    MemberInfo[] members = type.GetMembers(BindingFlags.Public | BindingFlags.NonPublic |
    	                                           BindingFlags.DeclaredOnly | BindingFlags.Instance |
    	                                           BindingFlags.Static);
    	    for (int i=0; i<members.Length; i++) {
                // ignore Types here.  They will get covered by the top level assembly.GetTypes
                if (!(members[i] is Type))
                    GetAllRecursive(members[i], switchAttribs);
    	    }

    	}

    	static void GetAllRecursive(MemberInfo member, ArrayList switchAttribs) {
    	    object[] attribs = member.GetCustomAttributes(typeof(SwitchAttribute), false);
    	    switchAttribs.AddRange(attribs);
    	}


    }
}

