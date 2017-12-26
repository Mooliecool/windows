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

// Recursion through an instantiated interface for a non-generic class
using System;

interface I<T>
{
  void m(T x);
}

class D : I<D>
{
  public void m(D x)
  {
    object obj = this;
    Console.WriteLine("D.m: this is I<D> = " + (obj is I<D>));
  }
}

class M
{
  public static void Main()
  {
    D d = new D();
    I<D> id = d;
    id.m(d);
  }
}
    