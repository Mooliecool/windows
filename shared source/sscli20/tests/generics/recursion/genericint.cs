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

// Recursion through an instantiated interface for a generic class
using System;

interface I<T>
{
  void m(T x);
}

class D<T> : I< D<T> >
{
  public void m(D<T> x)
  {
    object obj = this;
    Console.WriteLine("D.m: this is I<D<T>> = " + (obj is I< D<T> >));
    Console.WriteLine("D.m: this is I<D<string>> = " + (obj is I< D<string> >));
  }
}

class M
{
  public static void Main()
  {
    D<int> di = new D<int>();
    D<string> ds = new D<string>();
    D<object> d = new D<object>();
    I< D<int> > idi = di;
    I< D<string> > ids = ds;
    I< D<object> > id = d;
    idi.m(di);
    ids.m(ds);
    id.m(d);
  }
}
    