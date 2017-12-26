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
// Test unbox !0 on different reference types; different order of instantiation to unbox1.cs
using System;

public interface I<T> { }
public class C<T> : I<T> { }

public class P<T> 
{
  public T mymeth(object xv) {
    Console.WriteLine("T = " + typeof(T) + " and xv.GetType() = " + xv.GetType());
    return (T) xv;
  } 
}

class M
{
  public static void Main()
  {
    C<string> c = new C<string>();
    P<C<string> > pc = new P<C<string> >();
    P<I<string> > pi = new P<I<string> >();
    I<string> i1 = pi.mymeth(c);
    C<string> c1 = pc.mymeth(c);
  }
}
