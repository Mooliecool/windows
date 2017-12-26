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

tail.il was produced from this file, but tail. directives were added by hand.


/*
using System;

public class C1
{
    public virtual void meth1<T> (int i) {
        Console.WriteLine("C1::meth1<" + typeof(T) + ">(" + i + ")");
        meth2<T>(i);
    }

    public virtual void meth2<T> (int i) {
        Console.WriteLine("C2::meth2<" + typeof(T) + ">(" + i + ")");
    }
}

public class C2: C1
{
    override public void meth1<T> (int i) {
        Console.WriteLine("C2::meth1<" + typeof(T) + ">(" + i + ")");
        meth2<T>(i);
    }

    override public void meth2<T> (int i) {
        Console.WriteLine("C2::meth2<" + typeof(T) + ">(" + i + ")");
    }
}

class M
{
     public static void Main() {
         C1 c1 = new C1();
         C2 c2 = new C2();
         c1.meth1<string>(42);
         c2.meth1<string>(49);
         c1.meth1<int>(42);
         c2.meth1<int>(49);
         c1 = c2;
         c1.meth1<string>(53);
         c1.meth1<int>(53);
         Console.WriteLine("Finished!");
     }
}
*/