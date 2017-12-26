//-----------------------------------------------------------------------
//  This file is part of the Microsoft .NET Framework SDK Code Samples.
// 
//  Copyright (C) Microsoft Corporation.  All rights reserved.
// 
//This source code is intended only as a supplement to Microsoft
//Development Tools and/or on-line documentation.  See these other
//materials for detailed information regarding Microsoft code samples.
// 
//THIS CODE AND INFORMATION ARE PROVIDED AS IS WITHOUT WARRANTY OF ANY
//KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
//IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
//PARTICULAR PURPOSE.
/*=====================================================================
  File:      CustAttr.cs

  Summary:   Demonstrates how to create and use custom attributes.

=====================================================================*/

using System;
using System.Reflection;


// This type defines a custom attribute

// The attributes on this attribute describe where the attribute is legal
// and how often the attribute can be applied to a single target
[AttributeUsage(AttributeTargets.All, AllowMultiple = false)]
public class MyAttribute : System.Attribute {   // Attributes must derive from System.Attribute
    public int y = 111;     // This is a public, named attribute
    public string s;        // This is a public, named attribute
    public int x;           // This is a public, named attribute

    // Since this is the only constructor, every usage of 
    // this attribute requires the 2 positional parameters.
    public MyAttribute(string s, int x) {
        this.s = s;
        this.x = x;
    }

    public static void DisplayAttrInfo(int n, Attribute a) {
        if (a is MyAttribute) {
            // Refer to the one of the custom attributes
            MyAttribute myAttribute = (MyAttribute) a;
            Console.WriteLine("{0}-\"{1}\": {2}", n, a, 
                "X: " + myAttribute.x + ", Y: " + myAttribute.y + ", S: " + myAttribute.s);
        } else {
            Console.WriteLine("{0}-\"{1}\"", n, a);
        }
    }    
}


///////////////////////////////////////////////////////////////////////////////


[Obsolete("Ignore this warning -- just testing the Obsolete attribute ")]
class OldType{
}


///////////////////////////////////////////////////////////////////////////////


// This type has our custom attribute applied to it.

// Apply our attribute to our type. the optional named parameter is used.
// NOTE: For convenience, C# allows "Attribute" to be omitted from "MyAttribute"
[My("AttribOnType", 111, y = 222)]
class App {

    // Apply our attribute to this member. the optional named parameter is NOT used.
    [My("AttribOnMethod", 333)]
    public App() {
        Console.WriteLine("In Application constructor");
    }

    public static void Main() {

        // Get the set of custom attributes associated with the type
        Object[] TypeAttrs = typeof(App).GetCustomAttributes(false);
        Console.WriteLine("Number of custom attributes on Application type: " + TypeAttrs.Length);
        for (int n = 0; n < TypeAttrs.Length; n++) {
            MyAttribute.DisplayAttrInfo(n, (Attribute) TypeAttrs[n]);
        }

        // Get the set of methods associated with the type
        MemberInfo[] mi = typeof(App).FindMembers(
            MemberTypes.Constructor | MemberTypes.Method, 
            BindingFlags.Instance | BindingFlags.Static | BindingFlags.Public | BindingFlags.NonPublic | BindingFlags.DeclaredOnly, 
            Type.FilterName, "*");
        Console.WriteLine("Number of methods (includes constructors): " + mi.Length);

        for (int x = 0; x < mi.Length; x++) {
            // Get the set of custom attributes associated with this method
            Object[] MethodAttrs = mi[x].GetCustomAttributes(false);

            Console.WriteLine("Method name: " + mi[x].Name + "\t("+ MethodAttrs.Length + " attributes)");

            for (int n = 0; n < MethodAttrs.Length; n++) {
                Console.Write("   ");
                MyAttribute.DisplayAttrInfo(n, (Attribute) MethodAttrs[n]);
            }
        }

        // Test the ObsoleteAttribute type
        new OldType();
    }
}


///////////////////////////////// End of File /////////////////////////////////
