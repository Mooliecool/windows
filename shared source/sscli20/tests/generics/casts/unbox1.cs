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
// Test unbox !0 on different reference types
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
    C<string> cs = new C<string>();
    string[] sa = new string[1];
    P<C<string> > pc = new P<C<string> >();
    P<I<string> > pi = new P<I<string> >();
    P<object[]> pa = new P<object[]>();
    C<string> c1 = pc.mymeth(cs);
    I<string> i1 = pi.mymeth(cs);
    object[] a1 = pa.mymeth(sa);
  }
}
