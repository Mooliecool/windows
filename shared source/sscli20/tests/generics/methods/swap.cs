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

// This tests the use of generic by-refs
// In particular, this example illustrates the need for generalized ldobj/stobj instructions

using System;

class M
{
  static void Swap<T>(ref T x, ref T y)
  {
    T temp = x;
    x = y;
    y = temp;
  }

  public static void Main()
  {
    string s1 = "Rock";
    string s2 = "Generics";
    double d1 = 1.23456789;
    double d2 = 9.87654321;
    Swap<string>(ref s1,ref s2);
    Swap<double>(ref d1,ref d2);
    Console.WriteLine(s1 + " " + s2);
    Console.WriteLine(d1.ToString() + " " + d2.ToString());
  }
}
    