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

using System;

using System.Reflection;


// A trivial parameterized class
public class List<T>
{
}

public class P<T>
{
  // A generic static method
 public static A fst<A,B>(A x, B y) { return x;}
 public static A[] singleton<A>(A x) { return new A[] { x }; }
}

class M
{
  public static void DumpParameter(ParameterInfo pi)
  {
    Console.WriteLine("    Parameter " + pi + " [Name:" + pi.Name + ", ParameterType:" + pi.ParameterType + "]");
  }
  public static void DumpParameters(ParameterInfo[] ps)
  {
    bool first = true;
    foreach (ParameterInfo p in ps)
    {
      if (!first) Console.Write(",");
      first = false;
      Console.Write(p.ParameterType + " " + p.Name);
    }
  }
  public static void DumpMethod(MethodInfo m)
  {
      Console.WriteLine("Method " + m);
      Console.Write("  " + m.ReturnType + " " + m.ReflectedType + "::" + m.Name);
      if (m.HasGenericArguments)
      {	
	Console.Write("<");
	bool first = true;
	foreach (Type ty in m.GetGenericArguments()) {
	  if (first) 
	  {
	    Console.Write("<");
	    first = false;
	  }
	  Console.Write(ty);
	}
	Console.Write(">");
      }
      Console.Write("(");
      DumpParameters(m.GetParameters());
      Console.WriteLine(") declared in " + m.DeclaringType);
//      Console.WriteLine("  BaseDefinition.DeclaringType: " + m.GetBaseDefinition().DeclaringType);
//      Console.WriteLine("  BaseDefinition.ReflectedType: " + m.GetBaseDefinition().ReflectedType);
  }

  public static void Main(string[] args)
  {
    Type ty = Type.GetType("P");
    MethodInfo mfst = ty.GetMethod("fst");
//    DumpMethod(mfst);
    MethodInfo mfstsi = mfst.BindGenericParameters(new Type[] { typeof(string), typeof(int) });
    DumpMethod(mfstsi);
    Console.WriteLine(mfstsi.Invoke(null, new object[] { "My string", (object) 57 }));
    MethodInfo mfstls = mfst.BindGenericParameters(new Type[] { typeof(long), typeof(string) });
    DumpMethod(mfstls);
    Console.WriteLine(mfstls.Invoke(null, new object[] { (object) 12345678987654321L, "My string" }));
  }
}