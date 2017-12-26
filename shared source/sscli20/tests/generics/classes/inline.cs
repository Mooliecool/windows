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
// Test shared-code inlining
using System;

public class Set<T> { }

public class C<T>
{
  public object mymeth1()
  {
    return new Set<T>();
  }
}

public class D<S>
{
  public object mymeth2()
  {  
    return new C<S[]>().mymeth1();
  }
}

public class M
{
  public static void Main()
  {
    // Generate different tests by uncommenting out different lines

    Console.WriteLine(new D<string>().mymeth2().GetType());
    Console.WriteLine(new D<object>().mymeth2().GetType());

    Console.WriteLine(new D<float>().mymeth2().GetType());

    Console.WriteLine(new D<double>().mymeth2().GetType());

  }
}
