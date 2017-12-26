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
// Test newobj for multi-dimensional arrays
using System;

class P<T>
{
  // This should be inlined, at least when sharing is turned off
  public P<T>[,] Inlinable()
  {
    return new P<T>[1,1];
  }

  public virtual void TestPoly()
  {
    T[,] x = new T[1,1];
    Console.WriteLine(x.GetType());
    P<T>[,] y = Inlinable();
    Console.WriteLine(y.GetType());
  }
}

class M
{
  public static Type[,] Inlinable()
  {
    return new Type[1,1];
  }

  // Non-generic test
  public static void TestMono()
  {
    string[,] x = new string[1,1];
    Console.WriteLine(x.GetType());
    Type[,] y = Inlinable();
    Console.WriteLine(y.GetType());
  }

  public static void Main()
  {
    TestMono();
    P<string> ps = new P<string>();
    P<Type> pt = new P<Type>();
    ps.TestPoly();
    pt.TestPoly();
  }
}