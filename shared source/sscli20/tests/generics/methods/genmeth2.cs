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

// Test recursion in generic methods
using System;

class M
{
  public static T mymeth<T>(int n, T x)
  {
    if (n==0) 
    {
      Console.WriteLine("Finished mymeth<" + typeof(T) + ">");
      return x;
    }
    else return mymeth<T>(n-1, x);
  }

  public static void Main()
  {
    Console.WriteLine(mymeth<string>(10, "Hello, world."));
    Console.WriteLine(mymeth<Type>(20, typeof(string)));
    Console.WriteLine(mymeth<int>(30, 40));
  }
}
    