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

using System;

public class C1
{
    public virtual void meth1 (int i) {
        Console.WriteLine("C1::meth1<" + i + ")");
    }
}

public class C2: C1
{
    override public void meth1 (int i) {
        Console.WriteLine("C2::meth1<" + i + ")");
    }
}

class M
{
     public static void Main() {
         C1 c1 = new C1();
         C2 c2 = new C2();
         c1.meth1(42);
         c2.meth1(49);
         c1 = c2;
         c1.meth1(53);
         Console.WriteLine("Finished!");
     }
}