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
class Dict<K,D> { }
class C<T> { }
class D<T> : C< List<T> > { }
class E<A,B> : D< Dict<B,A> > { }

class M
{
  public static void Test(Type t1, Type t2, bool expected)
  {
    if ((t1.IsSubclassOf(t2)) != expected)
    {
      Console.WriteLine("Failed: " + t1.ToString() + " IsSubClassOf " + t2.ToString() + ". Expected " + expected);
    }
  }

  public static void Main()
  {
    Test(typeof(D<string>), typeof(C<List<string> >), true);
    Test(typeof(D<string>), typeof(C<List<object> >), false);
    Test(typeof(D<string>), typeof(C<object>), false);
    Test(typeof(D<int>), typeof(C<List<string> >), false);
    Test(typeof(List<int>), typeof(C<object>), false);
    Console.WriteLine("Finished!");
  }
}
    
