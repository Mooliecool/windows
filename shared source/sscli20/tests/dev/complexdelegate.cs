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

// test delegates with complex signatures

using System;

struct Valuesa {
    public byte a;
    public short b;
    public int c;
    public long d;
    public long e;
    public int f;
    public short g;
    public byte h;
};

struct Valuesb 
{
    public String a;
    public long b;
};

delegate float ComplexDelegate(
    float x1, double x2, 
    Valuesa v,
    double x3, double x4, double x5, double x6, double x7, double x8, double x9, double x10, 
    double x11, double x12, double x13, double x14, double x15, double x16, double x17, float x18);

delegate Valuesb ComplexDelegate2(String a, long b);

class My
{
    static float Worker(
        float x1, double x2, 
        Valuesa v,
        double x3, double x4, double x5, double x6, double x7, double x8, double x9, double x10, 
        double x11, double x12, double x13, double x14, double x15, double x16, double x17, float x18) {
        Console.WriteLine(x1.ToString() + " " + x2.ToString() + " " + v.ToString() + " " + x17.ToString() + " " + x18.ToString());
        long vv = v.a+v.b+v.c+v.d;
        v.a=0;v.b=0;v.c=0;v.d=0;       
        return (float)(x1+x2+x3+vv+v.e+v.f+v.g+v.h+(float)x17+(float)x18);
    }

    static Valuesb Worker2(String a, long b) 
    {
        Valuesb v;

        v.a = a;
        v.b = 2*b;

        return v;
    }

    static void Main(string[] args)
    {
        Valuesa v;
        v.a = 12;
        v.b = 23;
        v.c = 34;
        v.d = 45;
        v.e = 56;
        v.f = 67;
        v.g = 78;
        v.h = 89;
 
        ComplexDelegate d = new ComplexDelegate(Worker);
        d += new ComplexDelegate(Worker);
        d += new ComplexDelegate(Worker);
        
        float result = d(
            (float)1.5,2.5,
            v,
            3.5,4.5,5.5,6.5,7.5,8.5,9.5,10.5,
            11.5,12.5,13.5,14.5,15.5,16.5,17.5,(float)18.5);
            
        Console.WriteLine("Result: " + result.ToString());
            
        if ((double)result != (double)447.5) {
            Console.WriteLine("FAILED");
            Environment.Exit(1);
        }
        

        ComplexDelegate2 d2 = new ComplexDelegate2(Worker2);
        String a = "Test ObjectRef";

        Valuesb result2 = d2(a, 10);

        if ( result2.b != 20 || result2.a != "Test ObjectRef") {
            Console.WriteLine("FAILED");
            Environment.Exit(1);
        }

        Console.WriteLine("PASSED");
    }
}
