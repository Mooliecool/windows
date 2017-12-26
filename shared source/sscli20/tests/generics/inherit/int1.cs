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

// Interface inheritance
using System;

public interface I<T>
{
  void meth1(T x);
}

public interface J<A,B> : I<B>
{
  void meth2(A y);
}

public class C<A,B> : J<A,B>, I<B>
{
  public void meth1(B x) {
    Console.WriteLine("C<" + typeof(A) + "," + typeof(B) + ">.meth1");
  }
  public void meth2(A y) {
    Console.WriteLine("C<" + typeof(A) + "," + typeof(B) + ">.meth2");
  }
}

public class M
{
  static public void Main()
  {
    C<string,double> csd = new C<string,double>();
    C<Type,double> ctd = new C<Type,double>();
    csd.meth1(1.2);
    csd.meth2("a");
    ctd.meth1(3.4);
    ctd.meth2(typeof(string));
    J<string,double> jsd = csd;
    jsd.meth1(5.6);
    jsd.meth2("c");
    I<double> id = csd;
    id.meth1(7.8);
  }
}


