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

// test remoting marshaling with complex signatures that has a potential endianess problems

using System;
using System.Reflection;

[Serializable()]
public struct Struct1 {
    public byte a;

    public override String ToString() {
        return a.ToString();
    }

    public void Randomize() {
        a += (byte)MainApp.Randomizer();
    }
};

[Serializable()]
public struct Struct2 {
    public byte a;
    public byte b;

    public override String ToString() {
        return a.ToString() +
            " " + b.ToString();
    }

    public void Randomize() {
        a += (byte)MainApp.Randomizer();
        b += (byte)MainApp.Randomizer();
    }
};

[Serializable()]
public struct Struct3 {
    public byte a;
    public byte b;
    public byte c;

    public override String ToString() {
        return a.ToString() +
            " " + b.ToString() +
            " " + c.ToString();
    }

    public void Randomize() {
        a += (byte)MainApp.Randomizer();
        b += (byte)MainApp.Randomizer();
        c += (byte)MainApp.Randomizer();
    }
};

[Serializable()]
public struct Struct4 {
    public byte a;
    public byte b;
    public byte c;
    public byte d;

    public override String ToString() {
        return a.ToString() +
            " " + b.ToString() +
            " " + c.ToString() +
            " " + d.ToString();
    }

    public void Randomize() {
        a += (byte)MainApp.Randomizer();
        b += (byte)MainApp.Randomizer();
        c += (byte)MainApp.Randomizer();
        d += (byte)MainApp.Randomizer();
    }
};

[Serializable()]
public struct Struct5 {
    public byte a;
    public byte b;
    public byte c;
    public byte d;
    public byte e;

    public override String ToString() {
        return a.ToString() +
            " " + b.ToString() +
            " " + c.ToString() +
            " " + d.ToString() +
            " " + e.ToString();
    }

    public void Randomize() {
        a += (byte)MainApp.Randomizer();
        b += (byte)MainApp.Randomizer();
        c += (byte)MainApp.Randomizer();
        d += (byte)MainApp.Randomizer();
        e += (byte)MainApp.Randomizer();
    }
};

// Struct6 is a different case from Struct1 because the execution
// engine has optimizations to pass a structure with only one int
// of data as a value type.
[Serializable()]
public struct Struct6 
{
    public int a;
    
    public override String ToString() 
    {
        return a.ToString();
    }

    public void Randomize() 
    {
        a += (int)MainApp.Randomizer();
    }
};


[Serializable()]
public struct Struct8 {
    public byte a;
    public byte b;
    public byte c;
    public byte d;
    public byte e;
    public byte f;
    public byte g;
    public byte h;

    public override String ToString() {
        return a.ToString() +
            " " + b.ToString() +
            " " + c.ToString() +
            " " + d.ToString() +
            " " + e.ToString() +
            " " + f.ToString() +
            " " + g.ToString() +
            " " + h.ToString();
    }

    public void Randomize() {
        a += (byte)MainApp.Randomizer();
        b += (byte)MainApp.Randomizer();
        c += (byte)MainApp.Randomizer();
        d += (byte)MainApp.Randomizer();
        e += (byte)MainApp.Randomizer();
        f += (byte)MainApp.Randomizer();
        g += (byte)MainApp.Randomizer();
        h += (byte)MainApp.Randomizer();
    }
};

[Serializable()]
public struct Valuesa {
    public byte a;
    public short b;
    public int c;
    public long d;
    public long e;
    public int f;
    public short g;
    public byte h;

    public override String ToString() {
        return a.ToString() +
            " " + b.ToString() +
            " " + c.ToString() +
            " " + d.ToString() +
            " " + e.ToString() +
            " " + f.ToString() +
            " " + g.ToString() +
            " " + h.ToString();
    }

    public void Randomize() {
        a += (byte)MainApp.Randomizer();
        b += (short)MainApp.Randomizer();
        c += (int)MainApp.Randomizer();
        d += (long)MainApp.Randomizer();
        e += (long)MainApp.Randomizer();
        f += (int)MainApp.Randomizer();
        g += (short)MainApp.Randomizer();
        h += (byte)MainApp.Randomizer();
    }
};

public enum Enumacek {
    Beli = 0x0087,
    Hali = 0x6300,

    Tmp = 25344,
};

public class MainApp : MarshalByRefObject {

    public Struct1 _s1;
    public Struct2 _s2;
    public Struct3 _s3;
    public Struct4 _s4;
    public Struct5 _s5;
    public Struct6 _s6;
    public Struct8 _s8;
    public Valuesa _v;
    public float _f;
    public double _d;
    public Object _o;
    public String _s;
    public Enumacek _e;

    public sbyte _p1;
    public byte _p2;
    public short _p3;
    public ushort _p4;
    public int _p5;
    public uint _p6;
    public long _p7;
    public ulong _p8;

    static long _r;

    static public long Randomizer() {
        _r = 4309543 * _r + 353289;
        return _r;
    }

    public static void Fail(int code) {
        Environment.Exit(code);
    }

    public sbyte Foo(sbyte x) {
        String values =
            x.ToString() + " " +
            ".";

        if (values != "-48 .")
        {
            Console.WriteLine("sbyte Foo(...): unexpected values: " + values);
            Fail(1);
        }

        return (sbyte)(x + _p1);
    }

    public byte Foo(byte x) {
        String values =
            x.ToString() + " " +
            ".";

        if (values != "185 .")
        {
            Console.WriteLine("byte Foo(...): unexpected values: " + values);
            Fail(2);
        }

        return (byte)(x + _p2);
    }

    public short Foo(short x) {
        String values =
            x.ToString() + " " +
            ".";

        if (values != "-16072 .")
        {
            Console.WriteLine("short Foo(...): unexpected values: " + values);
            Fail(3);
        }

        return (short)(x + _p3);
    }

    public ushort Foo(ushort x) {
        String values =
            x.ToString() + " " +
            ".";

        if (values != "17297 .")
        {
            Console.WriteLine("ushort Foo(...): unexpected values: " + values);
            Fail(4);
        }

        return (ushort)(x + _p4);
    }

    public int Foo(int x) {
        String values =
            x.ToString() + " " +
            ".";

        if (values != "-994209504 .")
        {
            Console.WriteLine("int Foo(...): unexpected values: " + values);
            Fail(5);
        }

        return x + _p5;
    }

    public uint Foo(uint x) {
        String values =
            x.ToString() + " " +
            ".";

        if (values != "2046869481 .")
        {
            Console.WriteLine("uint Foo(...): unexpected values: " + values);
            Fail(6);
        }

        return x + _p6;
    }

    public long Foo(long x) {
        String values =
            x.ToString() + " " +
            ".";

        if (values != "1703311890815005320 .")
        {
            Console.WriteLine("long Foo(...): unexpected values: " + values);
            Fail(7);
        }

        return x + _p7;
    }

    public ulong Foo(ulong x) {
        String values =
            x.ToString() + " " +
            ".";

        if (values != "1413371402307118785 .")
        {
            Console.WriteLine("ulong Foo(...): unexpected values: " + values);
            Fail(8);
        }

        return x + _p8;
    }

    public Struct1 Foo(Struct2 v, ref Struct3 r, sbyte t, ref sbyte tr) {

        Struct1 ret = new Struct1();

        String values =
            v.ToString() + " " +
            r.ToString() + " " +
            t.ToString() + " " +
            tr.ToString() + " " +
            ".";

        if (values != "104 225 80 57 184 -96 -96 .")
        {
            Console.WriteLine("Struct1 Foo(...): unexpected values: " + values);
            Fail(11);
        }

        r.Randomize();
        tr += (sbyte)Randomizer();
        ret.Randomize();
        return ret;
    }

    public Struct2 Foo(ref Struct4 r, Struct3 v, short t, ref short tr) {
        Struct2 ret = new Struct2();

        String values =
            v.ToString() + " " +
            r.ToString() + " " +
            t.ToString() + " " +
            tr.ToString() + " " +
            ".";

        if (values != "89 161 153 17 160 105 8 -32144 -32144 .")
        {
            Console.WriteLine("Struct2 Foo(...): unexpected values: " + values);
            Fail(12);
        }

        r.Randomize();
        tr += (short)Randomizer();
        ret.Randomize();
        return ret;
    }

    public Struct3 Foo(Struct4 v, ref Struct5 r, int t, ref int tr) {
        Struct3 ret = new Struct3();

        String values =
            v.ToString() + " " +
            r.ToString() + " " +
            t.ToString() + " " +
            tr.ToString() + " " +
            ".";

        if (values != "201 177 9 113 65 240 153 88 113 -1988419008 " +
            "-1988419008 .")
        {
            Console.WriteLine("Struct3 Foo(...): unexpected values: " + values);
            Fail(13);
        }

        r.Randomize();
        tr += (int)Randomizer();
        ret.Randomize();
        return ret;
    }

    public Struct4 Foo(ref Struct8 r, Struct5 v, long t, ref long tr) {
        Struct4 ret = new Struct4();

        String values =
            v.ToString() + " " +
            r.ToString() + " " +
            t.ToString() + " " +
            tr.ToString() + " " +
            ".";

        if (values != "218 72 10 152 58 201 168 161 144 249 248" +
            " 209 224 3406623781630010640 3406623781630010640 .")
        {
            Console.WriteLine("Struct4 Foo(...): unexpected values: " + values);
            Fail(14);
        }

        r.Randomize();
        tr += (long)Randomizer();
        ret.Randomize();
        return ret;
    }

    public Struct5 Foo(Struct8 v, ref Valuesa r, byte t, ref byte tr) {
        Struct5 ret = new Struct5();

        String values =
            v.ToString() + " " +
            r.ToString() + " " +
            t.ToString() + " " +
            tr.ToString() + " " +
            ".";

        if (values != "193 121 129 185 65 249 1 57 41 20808 639653" +
            "377 -9016159779942285264 -7525944817302140071 1480724632 6961 128 114 114 .")
        {
            Console.WriteLine("Struct5 Foo(...): unexpected values: " + values);
            Fail(15);
        }

        r.Randomize();
        tr += (byte)Randomizer();
        ret.Randomize();
        return ret;
    }

    public Struct6 Foo(Struct5 v, ref Valuesa r, byte t, ref byte tr) 
    {
        Struct6 ret = new Struct6();

        String values =
            v.ToString() + " " +
            r.ToString() + " " +
            t.ToString() + " " +
            tr.ToString() + " " +
            ".";

        if (values != "112 25 216 241 192 138 -21480 225324986 -59" +
            "58788304505077400 2658617120903057130 486515128 -9446 8 51 51 .")
        {
            Console.WriteLine("Struct6 Foo(...): unexpected values: " + values);
            Fail(21);
        }

        r.Randomize();
        tr += (byte)Randomizer();
        ret.Randomize();
        return ret;
    }

    public Struct8 Foo(ref float r, Valuesa v, ushort t, ref ushort tr) {
        Struct8 ret = new Struct8();

        String values =
            v.ToString() + " " +
            r.ToString() + " " +
            t.ToString() + " " +
            tr.ToString() + " " +
            ".";

        if (values != "211 13120 -1219659045 754467178707868024 40" +
            "85042269830528355 -1490187216 -6293 104 -6.583926E+18 34594 34594 .")
        {
            Console.WriteLine("Struct8 Foo(...): unexpected values: " + values);
            Fail(16);
        }

        r += (float)Randomizer();
        tr += (ushort)Randomizer();
        ret.Randomize();
        return ret;
    }

    public Valuesa Foo(float v, ref double r, uint t, ref uint tr) {
        Valuesa ret = new Valuesa();

        String values =
            v.ToString() + " " +
            r.ToString() + " " +
            t.ToString() + " " +
            tr.ToString() + " " +
            ".";

        if (values != "-1.414468E+19 2.21297372164627E+18 40937389" +
            "62 4093738962 .")
        {
            Console.WriteLine("Valuesa Foo(...): unexpected values: " + values);
            Fail(17);
        }

        r += (double)Randomizer();
        tr += (uint)Randomizer();
        ret.Randomize();
        return ret;
    }

    public float Foo(ref Struct1 r, double v, ulong t, ref ulong tr) {
        float ret;

        String values =
            v.ToString() + " " +
            r.ToString() + " " +
            t.ToString() + " " +
            tr.ToString() + " " +
            ".";

        if (values != "-1.96436844951191E+18 57 2826742804614237570 " +
            "2826742804614237570 .")
        {
            Console.WriteLine("float Foo(...): unexpected values: " + values);
            Fail(18);
        }

        r.Randomize();
        tr += (ulong)Randomizer();
        ret = (float)Randomizer();
        return ret;
    }

    public double Foo(ref Struct2 r, Struct1 v, String t, ref String tr) {
        double ret;

        String values =
            v.ToString() + " " +
            r.ToString() + " " +
            t.ToString() + " " +
            tr.ToString() + " " +
            ".";

        if (values != "170 65 240 -5893266573950244255 -58932665739" +
            "50244255 .")
        {
            Console.WriteLine("double Foo(...): unexpected values: " + values);
            Fail(19);
        }

        r.Randomize();
        /* tr += */ Randomizer().ToString();
        ret = (double)Randomizer();
        return ret;
    }

    public void Foo(MainApp a, Object o, String s,
        float f1, double d1, float f2, double d2, float f3, double d3,
        long x1, long x2, long x3, long x4, long x5, long x6,
        sbyte y1, byte y2, short y3, ushort y4, int y5, uint y6, long y7, ulong y8,
        float f4, double d4, float f5, double d5, float f6, double d6) {

        String values =
            a.GetType().ToString() + " " +
            o.ToString() + " " +
            s.ToString() + " " +
            f1.ToString() + " " +
            d1.ToString() + " " +
            f2.ToString() + " " +
            d2.ToString() + " " +
            f3.ToString() + " " +
            d3.ToString() + " " +
            x1.ToString() + " " +
            x2.ToString() + " " +
            x3.ToString() + " " +
            x4.ToString() + " " +
            x5.ToString() + " " +
            x6.ToString() + " " +
            y1.ToString() + " " +
            y2.ToString() + " " +
            y3.ToString() + " " +
            y4.ToString() + " " +
            y5.ToString() + " " +
            y6.ToString() + " " +
            y7.ToString() + " " +
            y8.ToString() + " " +
            f4.ToString() + " " +
            d4.ToString() + " " +
            f5.ToString() + " " +
            d5.ToString() + " " +
            f6.ToString() + " " +
            d6.ToString() + " " +
            ".";

        if (values != "MainApp O S -9.327176E+17 -3.96300887748629E+1" +
            "8 -5.186961E+18 -1.49305767435013E+18 -5.895485E+18 6.9729019923856E+17 90407457" +
            "83328474920 3866776359567051553 6713255483212945424 1426425148927471225 -7819770" +
            "850983417224 -1407218523638657967 96 169 29640 38785 -1985398096 502707417 -7211" +
            "241837460167912 17618812656075189425 8.68788E+16 -5.610971897227E+18 5.33869E+18" +
            " 4.23216032051367E+17 1.208359E+18 -4.17734217115817E+18 .")
        {
            Console.WriteLine("void Foo(...): unexpected values: " + values);
            Fail(20);
        }
    }




    public MainApp() {
    }

    static public void RunTests(MainApp This) {

        String values;

        Struct1 s1 = new Struct1();
        Struct2 s2 = new Struct2();
        Struct3 s3 = new Struct3();
        Struct4 s4 = new Struct4();
        Struct5 s5 = new Struct5();
        Struct6 s6 = new Struct6();
        Struct8 s8 = new Struct8();
        Valuesa v = new Valuesa();
        float f;
        double d;
        Object o;
        String s;
        Enumacek e;

        sbyte p1;
        byte p2;
        short p3;
        ushort p4;
        int p5;
        uint p6;
        long p7;
        ulong p8;

        s1.Randomize();
        s2.Randomize();
        s3.Randomize();
        s4.Randomize();
        s5.Randomize();
        s6.Randomize();
        s8.Randomize();
        v.Randomize();

        f = (float)Randomizer();
        d = (float)Randomizer();
        o = This;
        s = Randomizer().ToString();
        e = Enumacek.Hali;

        p1 = (sbyte)Randomizer();
        p2 = (byte)Randomizer();
        p3 = (short)Randomizer();
        p4 = (ushort)Randomizer();
        p5 = (int)Randomizer();
        p6 = (uint)Randomizer();
        p7 = (long)Randomizer();
        p8 = (ulong)Randomizer();

        // set all fields
        This._s1 = s1;
        This._s2 = s2;
        This._s3 = s3;
        This._s4 = s4;
        This._s5 = s5;
        This._s6 = s6;
        This._s8 = s8;
        This._v = v;
        This._f = f;
        This._d = d;
        This._o = o;
        This._s = s;
        This._e = e;
        This._p1 = p1;
        This._p2 = p2;
        This._p3 = p3;
        This._p4 = p4;
        This._p5 = p5;
        This._p6 = p6;
        This._p7 = p7;
        This._p8 = p8;

        values = This.ToString();

        if (values != 
            "9 104 225 80 57 184 17 160 105 8 65 240 153 88 113 931407168" +
            " 201 168 161 144 249 248 209 224 41 20808 639653377 -9016159779942285264" +
            " -7525944817302140071 1480724632 6961 128 -6.583926E+18 2.21297372164627E+18" +
            " MainApp -5893266573950244255 Hali -48 185 -16072 17297 -994209504 2046869481" +
            " 1703311890815005320 1413371402307118785 .")
        {
            Console.WriteLine("SetFields: unexpected values: " + values);
            Fail(101);
        }

        Console.WriteLine("Field setters succeeded");

        // invoke the methods

        p1 = This.Foo(p1);
        p2 = This.Foo(p2);
        p3 = This.Foo(p3);
        p4 = This.Foo(p4);
        p5 = This.Foo(p5);
        p6 = This.Foo(p6);
        p7 = This.Foo(p7);
        p8 = This.Foo(p8);

        // verify the values of all fields
        values =
            p1.ToString() + " " +
            p2.ToString() + " " +
            p3.ToString() + " " +
            p4.ToString() + " " +
            p5.ToString() + " " +
            p6.ToString() + " " +
            p7.ToString() + " " +
            p8.ToString() + " " +
            ".";

        if (values != "-96 114 -32144 34594 -19884190" +
            "08 4093738962 3406623781630010640 2826742804614237570 .")
        {
            Console.WriteLine("Locals after primitive Invoke: unexpected values: " + values);
            Fail(102);
        }

        s1 = This.Foo(s2, ref s3, p1, ref p1);
        s2 = This.Foo(ref s4, s3, p3, ref p3);
        s3 = This.Foo(s4, ref s5, p5, ref p5);
        s4 = This.Foo(ref s8, s5, p7, ref p7);
        s5 = This.Foo(s8, ref v, p2, ref p2);
        s6 = This.Foo(s5, ref v, p2, ref p2);
        s8 = This.Foo(ref f, v, p4, ref p4);
        v = This.Foo(f, ref d, p6, ref p6);
        f = This.Foo(ref s1, d, p8, ref p8);
        d = This.Foo(ref s2, s1, s, ref s);

        // verify the values of all fields
        values =
            This.ToString();

        if (values != "9 104 225 80 57 184 17 160 105 8 65 240 " +
            "153 88 113 931407168 201 168 161 144 249 248 209 224 41 20808 639653377 -9016159" +
            "779942285264 -7525944817302140071 1480724632 6961 128 -6.583926E+18 2.2129737216" +
            "4627E+18 MainApp -5893266573950244255 Hali -48 185 -16072 17297 -994209504 20468" +
            "69481 1703311890815005320 1413371402307118785 .")
        {
            Console.WriteLine("Fields after Invoke: unexpected values: " + values);
            Fail(103);
        }

        // verify the values of all locals
        values =
            s1.ToString() + " " +
            s2.ToString() + " " +
            s3.ToString() + " " +
            s4.ToString() + " " +
            s5.ToString() + " " +
            s6.ToString() + " " +
            s8.ToString() + " " +
            v.ToString() + " " +
            f.ToString() + " " +
            d.ToString() + " " +
            o.GetType().ToString() + " " +
            s.ToString() + " " +
            e.ToString() + " " +
            p1.ToString() + " " +
            p2.ToString() + " " +
            p3.ToString() + " " +
            p4.ToString() + " " +
            p5.ToString() + " " +
            p6.ToString() + " " +
            p7.ToString() + " " +
            p8.ToString() + " " +
            ".";

        if (values != "170 233 145 161 144 249 49 128 137 232 1" +
            "12 25 216 241 192 263746504 217 24 177 0 9 104 225 80 17 7072 -1278944919 567925" +
            "9233286952200 964207834727950401 -272742416 14745 88 -7.848656E+18 3.16790391993" +
            "929E+18 MainApp -5893266573950244255 Hali -16 220 21112 49618 -153164056 1655108" +
            "490 -374121548284004952 17419808404417595330 .")
        {
            Console.WriteLine("Locals after Invoke: unexpected values: " + values);
            Fail(104);
        }

        Console.WriteLine("Method invokes succeeded");

        // get all fields
        s1 = This._s1;
        s2 = This._s2;
        s3 = This._s3;
        s4 = This._s4;
        s5 = This._s5;
        s6 = This._s6;
        s8 = This._s8;
        v = This._v;
        f = This._f;
        d = This._d;
        o = This._o;
        s = This._s;
        e = This._e;
        p1 = This._p1;
        p2 = This._p2;
        p3 = This._p3;
        p4 = This._p4;
        p5 = This._p5;
        p6 = This._p6;
        p7 = This._p7;
        p8 = This._p8;

        // verify the values of all locals
        values =
            s1.ToString() + " " +
            s2.ToString() + " " +
            s3.ToString() + " " +
            s4.ToString() + " " +
            s5.ToString() + " " +
            s6.ToString() + " " +
            s8.ToString() + " " +
            v.ToString() + " " +
            f.ToString() + " " +
            d.ToString() + " " +
            o.GetType().ToString() + " " +
            s.ToString() + " " +
            e.ToString() + " " +
            p1.ToString() + " " +
            p2.ToString() + " " +
            p3.ToString() + " " +
            p4.ToString() + " " +
            p5.ToString() + " " +
            p6.ToString() + " " +
            p7.ToString() + " " +
            p8.ToString() + " " +
            ".";

        if (values != "9 104 225 80 57 184 17 160 105 8 65 240 153 88 113" +
            " 931407168 201 168 161 144 249 248 209 224 41 20808 639653377 -90161597799422852" +
            "64 -7525944817302140071 1480724632 6961 128 -6.583926E+18 2.21297372164627E+18 M" +
            "ainApp -5893266573950244255 Hali -48 185 -16072 17297 -994209504 2046869481 1703" +
            "311890815005320 1413371402307118785 .")
        {
            Console.WriteLine("GetFields: unexpected values: " + values);
            Fail(105);
        }

        Console.WriteLine("Field getters succeeded");

        This.Foo(This, "O", "S",
            (float)Randomizer(), (double)Randomizer(), 
            (float)Randomizer(), (double)Randomizer(), 
            (float)Randomizer(), (double)Randomizer(),
            (long)Randomizer(), (long)Randomizer(), (long)Randomizer(), (long)Randomizer(),
            (long)Randomizer(), (long)Randomizer(),
            (sbyte)Randomizer(), (byte)Randomizer(), (short)Randomizer(), (ushort)Randomizer(),
            (int)Randomizer(), (uint)Randomizer(), (long)Randomizer(), (ulong)Randomizer(),
            (float)Randomizer(), (double)Randomizer(),  (float)Randomizer(), (double)Randomizer(), 
            (float)Randomizer(), (double)Randomizer());
    }

    public override String ToString() {
        return 
            _s1.ToString() + " " +
            _s2.ToString() + " " +
            _s3.ToString() + " " +
            _s4.ToString() + " " +
            _s5.ToString() + " " +
            _s6.ToString() + " " +
            _s8.ToString() + " " +
            _v.ToString() + " " +
            _f.ToString() + " " +
            _d.ToString() + " " +
            _o.GetType().ToString() + " " +
            _s.ToString() + " " +
            _e.ToString() + " " +
            _p1.ToString() + " " +
            _p2.ToString() + " " +
            _p3.ToString() + " " +
            _p4.ToString() + " " +
            _p5.ToString() + " " +
            _p6.ToString() + " " +
            _p7.ToString() + " " +
            _p8.ToString() + " " +
            ".";
    }

    public static int Main() {
        AppDomain app = AppDomain.CreateDomain("MyDomain");
        MainApp remoteapp = (MainApp)app.CreateInstanceAndUnwrap ("remotingmarshal", "MainApp");
        RunTests(remoteapp);
        Console.WriteLine("All test passed");

        return 0;
    }
}
