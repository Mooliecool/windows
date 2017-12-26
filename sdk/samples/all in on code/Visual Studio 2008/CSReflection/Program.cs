/****************************** Module Header ******************************\
* Module Name:  Program.cs
* Project:      CSReflection
* Copyright (c) Microsoft Corporation.
* 
* Reflection provides objects (of type Type) that encapsulate assemblies, 
* modules and types. It allows us to
* 
* 1. Access attributes in your program's metadata.
* 2. Examine and instantiate types in an assembly.
* 3. Dynamically load and use types.
* 4. Emit new types at runtime.
* 
* This example demonstrates 2 and 3. CSEmitAssembly shows the use of 4.
* 
* This source is subject to the Microsoft Public License.
* See http://www.microsoft.com/opensource/licenses.mspx#Ms-PL.
* All other rights reserved.
* 
* THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY KIND, 
* EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED 
* WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR PURPOSE.
\***************************************************************************/

#region Using directives
using System;
using System.Collections.Generic;
using System.Diagnostics;
using System.Linq;
using System.Reflection;
using System.Text;
#endregion


class Program
{
    static void Main(string[] args)
    {
        /////////////////////////////////////////////////////////////////////
        // Dynamically load the assembly.
        // 

        Assembly assembly = Assembly.Load("CSClassLibrary");
        Debug.Assert(assembly != null);


        /////////////////////////////////////////////////////////////////////
        // Get a type and instantiate the type in the assembly.
        // 
        
        Type type = assembly.GetType("CSClassLibrary.CSSimpleObject");
        object obj = Activator.CreateInstance(type, new object[] {});


        /////////////////////////////////////////////////////////////////////
        // Examine the type.
        // 

        Console.WriteLine("Listing all the members of {0}", type);
        Console.WriteLine();

        BindingFlags staticAll = BindingFlags.Static | 
            BindingFlags.NonPublic | BindingFlags.Public;
        BindingFlags instanceAll = BindingFlags.Instance |
            BindingFlags.NonPublic | BindingFlags.Public;

        // Lists static fields first.
        FieldInfo[] fi = type.GetFields(staticAll);
        Console.WriteLine("// Static Fields");
        PrintMembers(fi);

        // Static properties.
        PropertyInfo[] pi = type.GetProperties(staticAll);
        Console.WriteLine("// Static Properties");
        PrintMembers(pi);

        // Static events.
        EventInfo[] ei = type.GetEvents(staticAll);
        Console.WriteLine("// Static Events");
        PrintMembers(ei);

        // Static methods.
        MethodInfo[] mi = type.GetMethods(staticAll);
        Console.WriteLine("// Static Methods");
        PrintMembers(mi);

        // Constructors.
        ConstructorInfo[] ci = type.GetConstructors(instanceAll);
        Console.WriteLine("// Constructors");
        PrintMembers(ci);

        // Instance fields.
        fi = type.GetFields(instanceAll);
        Console.WriteLine("// Instance Fields");
        PrintMembers(fi);

        // Instance properites.
        pi = type.GetProperties(instanceAll);
        Console.WriteLine("// Instance Properties");
        PrintMembers(pi);

        // Instance events.
        ei = type.GetEvents(instanceAll);
        Console.WriteLine("// Instance Events");
        PrintMembers(ei);

        // Instance methods.
        mi = type.GetMethods(instanceAll);
        Console.WriteLine("// Instance Methods");
        PrintMembers(mi);


        /////////////////////////////////////////////////////////////////////
        // Use the type (Late Binding).
        // 

        // Call a public static method
        {
            Console.WriteLine("Call the public method: GetStringLength");

            MethodInfo method = type.GetMethod("GetStringLength");

            // Examine the method parameters
            ParameterInfo[] Params = method.GetParameters();
            foreach (ParameterInfo param in Params)
            {
                Console.WriteLine("Param={0}", param.Name);
                Console.WriteLine(" Type={0}", param.ParameterType);
                Console.WriteLine(" Position={0}", param.Position);
            }

            object result = method.Invoke(null, new object[] { "HelloWorld" });
            Console.WriteLine("Result={0}\n", result);
        }

        // Get the value of a private field
        {
            Console.WriteLine("Get the value of the private field: fField");

            Object result = type.InvokeMember("fField",
                BindingFlags.GetField | 
                // These two flags are necessary for internal functions.
                BindingFlags.Instance | BindingFlags.NonPublic,
                null, obj, new object[] {});
            Console.WriteLine("Result={0}\n", result);
        }


        /////////////////////////////////////////////////////////////////////
        // There is no API to unload an assembly.
        // http://blogs.msdn.com/suzcook/archive/2003/07/08/57211.aspx
        // http://blogs.msdn.com/jasonz/archive/2004/05/31/145105.aspx
        // 

    }

    /// <summary>
    /// Print each member info
    /// </summary>
    /// <param name="members"></param>
    static void PrintMembers(MemberInfo[] members)
    {
        foreach (MemberInfo memberInfo in members)
        {
            Console.WriteLine("{0}", memberInfo);
        }
        Console.WriteLine();
    }
}