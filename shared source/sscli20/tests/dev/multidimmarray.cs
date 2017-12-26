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

struct Struct2 {
    public short x;
};

struct Valuesa {
    public int a;
    public long b;
    public Object c;
    public String d;
    public long e;
};

class MyApp {

    static public void Check(TypedReference r, Object v) {
        if (!v.Equals(TypedReference.ToObject(r))) {
            Console.WriteLine("Expected: " + v);
            Console.WriteLine("Actual: " + TypedReference.ToObject(r));
            Environment.Exit(1);
        }
     }

    static public void Main() {

        int[,] ia =  { { 1, 2, 3, 4}, {5, 6, 7, 8}, {9, 10, 11, 12} };
        if (ia[2,3] != 12) {
            Console.WriteLine("Unexpected ia[2,3]: " + ia[2,3]);
            Environment.Exit(2);
        }
        ia[1,2] = 100;
        Check(__makeref(ia[1,2]), 100);

        try {
            ia[3,2] = 0;
            Console.WriteLine("IndexOutOfRangeException expected");
            Environment.Exit(2);
        }
        catch (IndexOutOfRangeException) {
        }

        ia = null;

        try {
            ia[3,2] = 0;
            Console.WriteLine("NullReferenceException expected");
            Environment.Exit(2);
        }
        catch (NullReferenceException) {
        }

        byte[,,] ba =  { { { 1, 2}, {3, 4 } }, { {5,6}, {7, 8} } };
        if (ba[1,0,1] != 6) {
            Console.WriteLine("Unexpected ba[1,0,1]: " + ba[1,0,1]);
            Environment.Exit(2);
        }
        ba[1,0,1] = 101;
        Check(__makeref(ba[1,0,1]), (byte)101);
        
        short[,,] sa =  { { { -1111, -2222}, {-3333, -4444 } }, { {-5555,-6666}, {-7777, -8888} } };
        if (sa[1,1,1] != -8888) {
            Console.WriteLine("Unexpected sa[1,1,1]: " + sa[1,1,1]);
            Environment.Exit(3);
        }
        sa[0,1,0] = -1002;
        Check(__makeref(sa[0,1,0]), (short)-1002);
               
        long[,] la =  { { 111111111111L, 222222222222L}, {333333333333L, 444444444444L } };
        if (la[1,1] != 444444444444L) {
            Console.WriteLine("Unexpected la[1,1]: " + la[1,1]);
            Environment.Exit(4);
        }
        la[1,0] = 444444444444L;
        Check(__makeref(la[1,0]), (long)444444444444L);
        
        float[,] fa =  { { 1.0F, 2.0F, 3.0F}, {4.0F, 5.0F, 6.0F}, {4.0F, 5.0F, 6.0F} };       
        if (fa[1,2] != 6.0F) {
            Console.WriteLine("Unexpected fa[1,2]: " + fa[1,2]);
            Environment.Exit(5);
        }
        fa[2,1] = Single.PositiveInfinity;
        Check(__makeref(fa[2,1]), Single.PositiveInfinity);
        
        double[,] da =  { { 1.0, 2.0, 3.0, 4.0}, {5.0, 6.0, 7.0, 8.0}, {9.0, 10.0, 11.0, 12.0} };       
        if (da[2,2] != 11.0) {
            Console.WriteLine("Unexpected da[2,2]: " + da[2,2]);
            Environment.Exit(6);
        }
        da[2,3] = 1234.5;
        Check(__makeref(da[2,3]), 1234.5);
        
        String[,,,,,,,,,] stra = new String[2,2,2,2,2,2,2,2,2,2];
        stra[0,1,0,1,0,1,0,1,0,1] = "Hello";
        if (stra[0,1,0,1,0,1,0,1,0,1] != "Hello") {
            Console.WriteLine("Unexpected stra[...]: " + stra[0,1,0,1,0,1,0,1,0,1]);
            Environment.Exit(7);       
        }
        Check(__makeref(stra[0,1,0,1,0,1,0,1,0,1]), "Hello");        
          
        try {
            Object[,,,,,,,,,] oba = stra;
            oba[1,0,1,0,1,0,1,0,1,0] = null;
            oba[1,0,1,0,1,0,1,0,1,0] = new Object();
            Console.WriteLine("ArrayTypeMismatchException expected");
            Environment.Exit(8);
        }
        catch (ArrayTypeMismatchException) {
        }
        
        Struct2 s2;
        Struct2[,] s2a = new Struct2[5,6];
        s2.x = -1234;
        s2a[2,3] = s2;
        s2 = s2a[2,3];
        if (s2.x != -1234) {
            Console.WriteLine("Unexpected s2a[2,3]: " + s2a[2,3].x);
            Environment.Exit(9);
        }
        Check(__makeref(s2a[2,3]), s2);
        
        Valuesa v;
        v.a = 11111111;
        v.b = 1111111111111111;
        v.c = new Object();
        v.d = "Hello";
        v.e = 2222222222222222;
        Valuesa[,,,] va = new Valuesa[1,2,3,4];        
        va[0,1,2,3] = v;
        v = va[0,1,2,3];
        if (v.a != 11111111 || v.b != 1111111111111111 || v.d != "Hello" || v.e != 2222222222222222) {
            Console.WriteLine("Unexpected va[...]");
            Environment.Exit(10);
        }
        Check(__makeref(va[0,1,2,3]), v);

        Console.WriteLine("Passed");
    }
}
