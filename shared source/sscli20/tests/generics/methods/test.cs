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
  static int Test<T>(int x, int y)
  {
	if (x > 0)
              if (y > 0)
                     return x + y;
              else
                     return x-y;
         else
             return x*y;

  }

  public static void Main()
  {
    int i = Test<string>(23, 45);
    Console.WriteLine("done" + i.ToString());
  }
}
    