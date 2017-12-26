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

// Mutual recursion through an instantiated superclass for a generic class
using System;

class List<T> { }
class C<T>
{
  public virtual void m()
  {
    Console.WriteLine("C<" + typeof(T) + ">.m in object of type " + GetType());
    Console.WriteLine("List<T> = " + typeof(List<T>));
  }
}

class D<T> : C< E<T> >
{
}

class E<T> : C< D<T> >
{
}

class M
{
  public static void Main()
  {
    D<int> di = new D<int>();
    D<string> ds = new D<string>();
    D<object> d = new D<object>();
    E<int> ei = new E<int>();
    E<string> es = new E<string>();
    E<object> e = new E<object>();
    di.m();
    ds.m();
    d.m();
    ei.m();
    es.m();
    e.m();
  }
}
    