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

class List<T> { }
class M
{
  public static void Main()
  {
    Type ty = typeof(List<string>[]);
    MethodInfo mi = ty.GetMethod("Set", new Type[] { typeof(int), typeof(List<string>) });
    Console.WriteLine(mi);
    Console.WriteLine(mi.DeclaringType);
    Console.WriteLine(mi.GetParameters()[1].ParameterType);

    mi = ty.GetMethod("Get", new Type[] { typeof(int) });
    Console.WriteLine(mi);
    Console.WriteLine(mi.ReturnType);
  }
}