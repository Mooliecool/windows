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

// This tests inheritance of virtual methods.

using System;

public class C1<T>
{
  public virtual void meth<U>(U x)
  {
    Console.WriteLine("C1<" + typeof(T) + ">.meth<" + typeof(U) + "," + x + ">");
  }
}

public class C2 : C1<int>
{
}

public class M
{
  static public void Main()
  {
    C2 c = new C2();
    c.meth<string>("hello");
  }
}



