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

// tests marshalling of longs and small structures

using System;
using System.Reflection;
using System.Runtime.InteropServices;



[StructLayout(LayoutKind.Sequential)]
struct Struct1 {
    public byte a;
};

[StructLayout(LayoutKind.Sequential)]
struct Struct2 {
    public byte a;
    public byte b;
};

[StructLayout(LayoutKind.Sequential)]
struct Struct3 {
    public byte a;
    public byte b;
    public byte c;
};

[StructLayout(LayoutKind.Sequential)]
struct Struct4 {
    public byte a;
    public byte b;
    public byte c;
    public byte d;
};

[StructLayout(LayoutKind.Sequential)]
struct Struct5 {
    public byte a;
    public byte b;
    public byte c;
    public byte d;
    public byte e;
};

class MainApp {

#if !PLATFORM_UNIX
       internal const String DLLPREFIX = "";
       internal const String DLLSUFFIX = ".dll";
#else // !PLATFORM_UNIX
 #if __APPLE__
       internal const String DLLPREFIX = "lib";
       internal const String DLLSUFFIX = ".dylib";
 #else
       internal const String DLLPREFIX = "lib";
       internal const String DLLSUFFIX = ".so";
 #endif
#endif // !PLATFORM_UNIX

    const String NATIVEDLL = DLLPREFIX + "nativedll" + DLLSUFFIX;

    delegate byte ByteTestDelegate(byte l, ref byte _l);
    delegate sbyte SByteTestDelegate(sbyte l, ref sbyte _l);
    delegate short ShortTestDelegate(short l, ref short _l);
    delegate ushort UShortTestDelegate(ushort l, ref ushort _l);
    delegate long LongTestDelegate(long l, ref long _l);

    static byte ByteTestCallback(byte l, ref byte _l)
    {
        if (l != 22 || _l != 23)
        {
            Console.WriteLine("ByteTestCallback: Unexpected values: " + l + " " + _l);
            Environment.Exit(1);
        }
        _l = 234;
        return 123;
    }
    
    static sbyte SByteTestCallback(sbyte l, ref sbyte _l)
    {
        if (l != 20 || _l != 19)
        {
            Console.WriteLine("SByteTestCallback: Unexpected values: " + l + " " + _l);
            Environment.Exit(1);
        }
        _l = 12;
        return -123;
    }
    
    static short ShortTestCallback(short l, ref short _l)
    {
        if (l != -14035 || _l != -15035)
        {
            Console.WriteLine("ShortTestCallback: Unexpected values: " + l + " " + _l);
            Environment.Exit(1);
        }
        _l = 1200;
        return -12300;
    }    

    static ushort UShortTestCallback(ushort l, ref ushort _l)
    {
        if (l != 53501 || _l != 54501)
        {
            Console.WriteLine("UShortTestCallback: Unexpected values: " + l + " " + _l);
            Environment.Exit(1);
        }
        _l = 23400;
        return 12300;
    }
    
    static long LongTestCallback(long l, ref long _l)
    {
        if (l != 123456789246913578 || _l != 234567891358024679)
        {
            Console.WriteLine("LongTestCallback: Unexpected values: " + l + " " + _l);
            Environment.Exit(1);
        }
        _l = 234000000000;
        return 123000000000;
    }

    [DllImport(NATIVEDLL)]
    static extern byte ByteTest(ByteTestDelegate d, byte l, ref byte _l);
    [DllImport(NATIVEDLL)]
    static extern sbyte SByteTest(sbyte l, SByteTestDelegate d, ref sbyte _l);
    [DllImport(NATIVEDLL)]
    static extern short ShortTest(short l, ref short _l, ShortTestDelegate d);
    [DllImport(NATIVEDLL)]
    static extern ushort UShortTest(UShortTestDelegate d, ushort l, ref ushort _l);
    [DllImport(NATIVEDLL)]
    static extern long LongTest(long l, LongTestDelegate d, ref long _l);
    [DllImport(NATIVEDLL)]
    static extern Struct1 StructTest1(Struct1 s, ref Struct1 _s);
    [DllImport(NATIVEDLL)]
    static extern Struct2 StructTest2(Struct2 s, ref Struct2 _s);
    [DllImport(NATIVEDLL)]
    static extern Struct3 StructTest3(Struct3 s, ref Struct3 _s);
    [DllImport(NATIVEDLL)]
    static extern Struct4 StructTest4(Struct4 s, ref Struct4 _s);
    [DllImport(NATIVEDLL)]
    static extern Struct5 StructTest5(Struct5 s, ref Struct5 _s);

    public static int Main() {

        byte b1, b2, b3;
        b1 = 1;
        b2 = 2;
        b3 = ByteTest(new ByteTestDelegate(ByteTestCallback), b1, ref b2);
        if (b2 != 234 || b3 != 144)
        {
            Console.WriteLine("ByteTest: Unexpected values: " + b2 + " " + b3);
            return 1;
        }
        
        sbyte sb1, sb2, sb3;
        sb1 = -1;
        sb2 = -2;
        sb3 = SByteTest(sb1, new SByteTestDelegate(SByteTestCallback), ref sb2);
        if (sb2 != 12 || sb3 != -102)
        {
            Console.WriteLine("SByteTest: Unexpected values: " + sb2 + " " + sb3);
            return 1;
        }

        short sh1, sh2, sh3;
        sh1 = -1000;
        sh2 = -2000;
        sh3 = ShortTest(sh1, ref sh2, new ShortTestDelegate(ShortTestCallback));
        if (sh2 != 1200 || sh3 != -25335)
        {
            Console.WriteLine("ShortTest: Unexpected values: " + sh2 + " " + sh3);
            return 1;
        }

        ushort ush1, ush2, ush3;
        ush1 = 1000;
        ush2 = 2000;
        ush3 = UShortTest(new UShortTestDelegate(UShortTestCallback), ush1, ref ush2);
        if (ush2 != 23400 || ush3 != 64801)
        {
            Console.WriteLine("UShortTest: Unexpected value: " + ush2 + " " + ush3);
            return 1;
        }
        
        long l1,l2,l3;
        l1 = 123456789123456789;
        l2 = 234567891234567890;
        l3 = LongTest(l1, new LongTestDelegate(LongTestCallback), ref l2);
        if (l2 != 234000000000 || l3 != 123123456789)
        {
            Console.WriteLine("LongTest: Unexpected valuess: " + l2 + " " + l3);
            return 1;
        }

        Struct1 s1;
        s1.a = 1;
        s1 = StructTest1(s1, ref s1);
        if (s1.a != 111) {
            Console.WriteLine("Unexpected values: " + 
                s1.a.ToString());
            return 1;
        }

        Struct2 s2;
        s2.a = 1;
        s2.b = 2;
        s2 = StructTest2(s2, ref s2);
        if (s2.a != 111 || s2.b != 122) {
            Console.WriteLine("Unexpected values: " + 
                s2.a.ToString() + " " + s2.b.ToString());
            return 1;
        }

        Struct3 s3;
        s3.a = 1;
        s3.b = 2;
        s3.c = 3;
        s3 = StructTest3(s3, ref s3);
        if (s3.a != 111 || s3.b != 122 || s3.c != 133) {
            Console.WriteLine("Unexpected values: " + 
                s3.a.ToString() + " " + s3.b.ToString() + " " + s3.c.ToString());
            return 1;
        }

        Struct4 s4;
        s4.a = 1;
        s4.b = 2;
        s4.c = 3;
        s4.d = 4;
        s4 = StructTest4(s4, ref s4);
        if (s4.a != 111 || s4.b != 122 || s4.c != 133 || s4.d != 144) {
            Console.WriteLine("Unexpected values: " + 
                s4.a.ToString() + " " + s4.b.ToString() + " " + s4.c.ToString() + " " + s4.d.ToString());
            return 1;
        }

        Struct5 s5;
        s5.a = 1;
        s5.b = 2;
        s5.c = 3;
        s5.d = 4;
        s5.e = 5;
        s5 = StructTest5(s5, ref s5);
        if (s5.a != 111 || s5.b != 122 || s5.c != 133 || s5.d != 144 || s5.e != 155) {
            Console.WriteLine("Unexpected values: " + 
                s5.a.ToString() + " " + s5.b.ToString() + " " + s5.c.ToString() + " " + s5.d.ToString() + " " + s5.e.ToString());
            return 1;
        }

        Console.WriteLine("All tests passed");
        return 0;
    }
}



