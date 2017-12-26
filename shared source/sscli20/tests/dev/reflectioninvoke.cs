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

// test reflection invoke with complex signatures that has a potential endianess problems

using System;
using System.Reflection;

public struct Struct1 {
    public byte a;

    public override String ToString() {
        return a.ToString();
    }

    public void Randomize() {
        a += (byte)MainApp.Randomizer();
    }
};

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
};

public class MainApp {

    public Struct1 _s1;
    static public Struct2 _s2;
    public Struct3 _s3;
    public Struct4 _s4;
    public Struct5 _s5;
    public Struct8 _s8;
    public Valuesa _v;
    public float _f;
    public double _d;
    public Object _o;
    public String _s;
    public Enumacek _e;

    public sbyte _p1;
    static public byte _p2;
    static public short _p3;
    public ushort _p4;
    public int _p5;
    public uint _p6;
    public long _p7;
    public ulong _p8;

    static long _r;

    public static long Randomizer() {
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

        if (values != "97 .")
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

        if (values != "208 .")
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

        if (values != "-9799 .")
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

        if (values != "49464 .")
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

        if (values != "1739539345 .")
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

        if (values != "3300757792 .")
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

        if (values != "-6517938239175606295 .")
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

        if (values != "1703311890815005320 .")
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

        if (values != "104 225 80 57 184 -62 -62 .")
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

        if (values != "17 169 209 17 160 105 8 -19598 -19598 .")
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

        if (values != "209 233 145 41 65 240 153 88 113 -815888606" +
            " -815888606 .")
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

        if (values != "146 80 66 32 242 64 201 168 161 144 249 248" +
            " 209 5410867595358339026 5410867595358339026 .")
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

        if (values != "64 210 16 130 224 50 176 226 224 -23767 606" +
            "228808 2874968872843236865 -9016159779942285264 360942425 4248 49 160 160 .")
        {
            Console.WriteLine("Struct5 Foo(...): unexpected values: " + values);
            Fail(15);
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

        if (values != "121 -16511 697284281 -978709601062956991 15" +
            "81927808989185017 -802663167 32057 193 -5.193024E+18 33392 33392 .")
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

        if (values != "4.005809E+18 -6.58392565490175E+18 23065482" +
            "88 2306548288 .")
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

        if (values != "-8.88217392868046E+18 241 3406623781630010640" +
            " 3406623781630010640 .")
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

        if (values != "226 121 120 2212973731230676456 221297373123" +
            "0676456 .")
        {
            Console.WriteLine("double Foo(...): unexpected values: " + values);
            Fail(19);
        }

        r.Randomize();
        tr += Randomizer().ToString();
        ret = (double)Randomizer();
        return ret;
    }

    static public void Foo(MainApp a, Object o, String s,
        float f1, double d1, float f2, double d2, float f3, double d3,
        long x1, long x2, long x3, long x4, long x5, long x6,
        sbyte y1, byte y2, short y3, ushort y4, int y5, uint y6, long y7, ulong y8,
        float f4, double d4, float f5, double d5, float f6, double d6) {

        String values =
            a.ToString() + " " +
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

        if (values != "MainApp O S 1.3393E+18 5.61788680423607" +
            "E+18 3.258727E+18 9.16789895402191E+18 -3.055988E+18 -4.46137648397179E+18 -5851" +
            "159968668995664 6666931803129677785 -1800446251775719400 5844555569909944241 -52" +
            "81267348037457408 2361696488407452169 104 225 -5552 46649 -1474602824 3693303825" +
            " 591560692012003488 16208348825514708073 5.985838E+18 4.30978217368081E+18 -4.90" +
            "6224E+18 1.29464764673023E+18 -7.168869E+18 1.34929258932224E+18 .")
        {
            Console.WriteLine("static void Foo(...): unexpected values: " + values);
            Fail(20);
        }
    }

    public MainApp() {
    }

    public MainApp(MainApp a,
        Struct1 s1, Struct2 s2, Struct3 s3, Struct4 s4, Struct5 s5, Struct8 s8,
        Valuesa v, float f, double d, Object o, String s, Enumacek e) {

        String values =
            a.ToString() + " " +
            s1.ToString() + " " +
            s2.ToString() + " " +
            s3.ToString() + " " +
            s4.ToString() + " " +
            s5.ToString() + " " +
            s8.ToString() + " " +
            v.ToString() + " " +
            f.ToString() + " " +
            d.ToString() + " " +
            o.ToString() + " " +
            s.ToString() + " " +
            e.ToString() + " " +
            ".";

        if (values != "MainApp 9 104 225 80 57 184 17 160 105 8 65 240" +
            " 153 88 113 64 201 168 161 144 249 248 209 224 -23767 606228808 2874968872843236" +
            "865 -9016159779942285264 360942425 4248 49 -5.193024E+18 -6.58392565490175E+18 M" +
            "ainApp 2212973731230676456 Hali .")
        {
            Console.WriteLine("MainApp(...): unexpected values: " + values);
            Fail(22);
        }
    }

    public void RunTests() {

        String values;

        Struct1 s1 = new Struct1();
        Struct2 s2 = new Struct2();
        Struct3 s3 = new Struct3();
        Struct4 s4 = new Struct4();
        Struct5 s5 = new Struct5();
        Struct8 s8 = new Struct8();
        Valuesa v = new Valuesa();
        float f;
        double d;
        Object o;
        String s;
        Enumacek e;

        s1.Randomize();
        s2.Randomize();
        s3.Randomize();
        s4.Randomize();
        s5.Randomize();
        s8.Randomize();
        v.Randomize();

        f = (float)Randomizer();
        d = (float)Randomizer();
        o = this;
        s = Randomizer().ToString();
        e = Enumacek.Hali;

        Object s1_ = (Object)s1;
        Object s2_ = (Object)s2;
        Object s3_ = (Object)s3;
        Object s4_ = (Object)s4;
        Object s5_ = (Object)s5;
        Object s8_ = (Object)s8;
        Object v_ = (Object)v;
        Object f_ = (Object)f;
        Object d_ = (Object)d;
        Object o_ = (Object)o;
        Object s_ = (Object)s;
        Object e_ = (Object)e;
        Object p1_ = (sbyte)Randomizer();
        Object p2_ = (byte)Randomizer();
        Object p3_ = (short)Randomizer();
        Object p4_ = (ushort)Randomizer();
        Object p5_ = (int)Randomizer();
        Object p6_ = (uint)Randomizer();
        Object p7_ = (long)Randomizer();
        Object p8_ = (ulong)Randomizer();

        // set all fields
        GetType().InvokeMember("_s1",
            BindingFlags.Instance | BindingFlags.Public | BindingFlags.SetField, null,
            this, new Object[] { s1_ }, null, null, null);
        GetType().InvokeMember("_s2",
            BindingFlags.Static | BindingFlags.Public | BindingFlags.SetField, null,
            this, new Object[] { s2_ }, null, null, null);
        GetType().InvokeMember("_s3",
            BindingFlags.Instance | BindingFlags.Public | BindingFlags.SetField, null,
            this, new Object[] { s3_ }, null, null, null);
        GetType().InvokeMember("_s4",
            BindingFlags.Instance | BindingFlags.Public | BindingFlags.SetField, null,
            this, new Object[] { s4_ }, null, null, null);
        GetType().InvokeMember("_s5",
            BindingFlags.Instance | BindingFlags.Public | BindingFlags.SetField, null,
            this, new Object[] { s5_ }, null, null, null);
        GetType().InvokeMember("_s8",
            BindingFlags.Instance | BindingFlags.Public | BindingFlags.SetField, null,
            this, new Object[] { s8_ }, null, null, null);
        GetType().InvokeMember("_v",
            BindingFlags.Instance | BindingFlags.Public | BindingFlags.SetField, null,
            this, new Object[] { v_ }, null, null, null);
        GetType().InvokeMember("_f",
            BindingFlags.Instance | BindingFlags.Public | BindingFlags.SetField, null,
            this, new Object[] { f_ }, null, null, null);
        GetType().InvokeMember("_d",
            BindingFlags.Instance | BindingFlags.Public | BindingFlags.SetField, null,
            this, new Object[] { d_ }, null, null, null);
        GetType().InvokeMember("_o",
            BindingFlags.Instance | BindingFlags.Public | BindingFlags.SetField, null,
            this, new Object[] { o_ }, null, null, null);
        GetType().InvokeMember("_s",
            BindingFlags.Instance | BindingFlags.Public | BindingFlags.SetField, null,
            this, new Object[] { s_ }, null, null, null);
        GetType().InvokeMember("_e",
            BindingFlags.Instance | BindingFlags.Public | BindingFlags.SetField, null,
            this, new Object[] { e_ }, null, null, null);
        GetType().InvokeMember("_p1",
            BindingFlags.Instance | BindingFlags.Public | BindingFlags.SetField, null,
            this, new Object[] { p1_ }, null, null, null);
        GetType().InvokeMember("_p2",
            BindingFlags.Static | BindingFlags.Public | BindingFlags.SetField, null,
            this, new Object[] { p2_ }, null, null, null);
        GetType().InvokeMember("_p3",
            BindingFlags.Static | BindingFlags.Public | BindingFlags.SetField, null,
            this, new Object[] { p3_ }, null, null, null);
        GetType().InvokeMember("_p4",
            BindingFlags.Instance | BindingFlags.Public | BindingFlags.SetField, null,
            this, new Object[] { p4_ }, null, null, null);
        GetType().InvokeMember("_p5",
            BindingFlags.Instance | BindingFlags.Public | BindingFlags.SetField, null,
            this, new Object[] { p5_ }, null, null, null);
        GetType().InvokeMember("_p6",
            BindingFlags.Instance | BindingFlags.Public | BindingFlags.SetField, null,
            this, new Object[] { p6_ }, null, null, null);
        GetType().InvokeMember("_p7",
            BindingFlags.Instance | BindingFlags.Public | BindingFlags.SetField, null,
            this, new Object[] { p7_ }, null, null, null);
        GetType().InvokeMember("_p8",
            BindingFlags.Instance | BindingFlags.Public | BindingFlags.SetField, null,
            this, new Object[] { p8_ }, null, null, null);

        // verify the values of all fields
        values =
            _s1.ToString() + " " +
            _s2.ToString() + " " +
            _s3.ToString() + " " +
            _s4.ToString() + " " +
            _s5.ToString() + " " +
            _s8.ToString() + " " +
            _v.ToString() + " " +
            _f.ToString() + " " +
            _d.ToString() + " " +
            _o.ToString() + " " +
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

        if (values != "9 104 225 80 57 184 17 160 105 8 65 240 153 88 113 64" +
            " 201 168 161 144 249 248 209 224 -23767 606228808 2874968872843236865 -901615977" +
            "9942285264 360942425 4248 49 -5.193024E+18 -6.58392565490175E+18 MainApp 2212973" +
            "731230676456 Hali 97 208 -9799 49464 1739539345 3300757792 -6517938239175606295 " +
            "1703311890815005320 .")
        {
            Console.WriteLine("SetFields: unexpected values: " + values);
            Fail(101);
        }

        Console.WriteLine("Field setters succeeded");

        // invoke the methods

        p1_ = GetType().InvokeMember("Foo",
            BindingFlags.Instance | BindingFlags.Public | BindingFlags.InvokeMethod, null,
            this, new Object[] { p1_ }, null, null, null);
        p2_ = GetType().InvokeMember("Foo",
            BindingFlags.Instance | BindingFlags.Public | BindingFlags.InvokeMethod, null,
            this, new Object[] { p2_ }, null, null, null);
        p3_ = GetType().InvokeMember("Foo",
            BindingFlags.Instance | BindingFlags.Public | BindingFlags.InvokeMethod, null,
            this, new Object[] { p3_ }, null, null, null);
        p4_ = GetType().InvokeMember("Foo",
            BindingFlags.Instance | BindingFlags.Public | BindingFlags.InvokeMethod, null,
            this, new Object[] { p4_ }, null, null, null);
        p5_ = GetType().InvokeMember("Foo",
            BindingFlags.Instance | BindingFlags.Public | BindingFlags.InvokeMethod, null,
            this, new Object[] { p5_ }, null, null, null);
        p6_ = GetType().InvokeMember("Foo",
            BindingFlags.Instance | BindingFlags.Public | BindingFlags.InvokeMethod, null,
            this, new Object[] { p6_ }, null, null, null);
        p7_ = GetType().InvokeMember("Foo",
            BindingFlags.Instance | BindingFlags.Public | BindingFlags.InvokeMethod, null,
            this, new Object[] { p7_ }, null, null, null);
        p8_ = GetType().InvokeMember("Foo",
            BindingFlags.Instance | BindingFlags.Public | BindingFlags.InvokeMethod, null,
            this, new Object[] { p8_ }, null, null, null);

        // verify the values of all fields
        values =
            p1_.ToString() + " " +
            p2_.ToString() + " " +
            p3_.ToString() + " " +
            p4_.ToString() + " " +
            p5_.ToString() + " " +
            p6_.ToString() + " " +
            p7_.ToString() + " " +
            p8_.ToString() + " " +
            ".";

        if (values != "-62 160 -19598 33392 -81588860" +
            "6 2306548288 5410867595358339026 3406623781630010640 .")
        {
            Console.WriteLine("Locals after primitive Invoke: unexpected values: " + values);
            Fail(102);
        }

        Object[] args;
        args = new Object[] { s2_, s3_, p1_, p1_ };
        s1_ = GetType().InvokeMember("Foo",
            BindingFlags.Instance | BindingFlags.Public | BindingFlags.InvokeMethod, null,
            this, args, null, null, null);
        s3_ = args[1]; p1_ = args[3]; 

        args = new Object[] { s4_, s3_, p3_, p3_ };
        s2_ = GetType().InvokeMember("Foo",
            BindingFlags.Instance | BindingFlags.Public | BindingFlags.InvokeMethod, null,
            this, args, null, null, null);
        s4_ = args[0]; p3_ = args[3]; 

        args = new Object[] { s4_, s5_, p5_, p5_ };
        s3_ = GetType().InvokeMember("Foo",
            BindingFlags.Instance | BindingFlags.Public | BindingFlags.InvokeMethod, null,
            this, args, null, null, null);
        s5_ = args[1]; p5_ = args[3]; 

        args = new Object[] { s8_, s5_, p7_, p7_ };
        s4_ = GetType().InvokeMember("Foo",
            BindingFlags.Instance | BindingFlags.Public | BindingFlags.InvokeMethod, null,
            this, args, null, null, null);
        s8_ = args[0]; p7_ = args[3]; 

        args = new Object[] { s8_, v_, p2_, p2_ };
        s5_ = GetType().InvokeMember("Foo",
            BindingFlags.Instance | BindingFlags.Public | BindingFlags.InvokeMethod, null,
            this, args, null, null, null);
        v_ = args[1]; p2_ = args[3]; 

        args = new Object[] { f_, v_, p4_, p4_ };
        s8_ = GetType().InvokeMember("Foo",
            BindingFlags.Instance | BindingFlags.Public | BindingFlags.InvokeMethod, null,
            this, args, null, null, null);
        f_ = args[0]; p4_ = args[3]; 

        args = new Object[] { f_, d_, p6_, p6_ };
        v_ = GetType().InvokeMember("Foo",
            BindingFlags.Instance | BindingFlags.Public | BindingFlags.InvokeMethod, null,
            this, args, null, null, null);
        d_ = args[1]; p6_ = args[3]; 

        args = new Object[] { s1_, d_, p8_, p8_ };
        f_ = GetType().InvokeMember("Foo",
            BindingFlags.Instance | BindingFlags.Public | BindingFlags.InvokeMethod, null,
            this, args, null, null, null);
        s1_ = args[0]; p8_ = args[3]; 

        args = new Object[] { s2_, s1_, s_, s_ };
        d_ = GetType().InvokeMember("Foo",
            BindingFlags.Instance | BindingFlags.Public | BindingFlags.InvokeMethod, null,
            this, args, null, null, null);
        s2_ = args[0]; 

        // verify the values of all fields
        values =
            _s1.ToString() + " " +
            _s2.ToString() + " " +
            _s3.ToString() + " " +
            _s4.ToString() + " " +
            _s5.ToString() + " " +
            _s8.ToString() + " " +
            _v.ToString() + " " +
            _f.ToString() + " " +
            _d.ToString() + " " +
            _o.ToString() + " " +
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

        if (values != "9 104 225 80 57 184 17 160 105 8 65 240 " +
            "153 88 113 64 201 168 161 144 249 248 209 224 -23767 606228808 28749688728432368" +
            "65 -9016159779942285264 360942425 4248 49 -5.193024E+18 -6.58392565490175E+18 Ma" +
            "inApp 2212973731230676456 Hali 97 208 -9799 49464 1739539345 3300757792 -6517938" +
            "239175606295 1703311890815005320 .")
        {
            Console.WriteLine("Fields after Invoke: unexpected values: " + values);
            Fail(103);
        }

        // verify the values of all locals
        values =
            s1_.ToString() + " " +
            s2_.ToString() + " " +
            s3_.ToString() + " " +
            s4_.ToString() + " " +
            s5_.ToString() + " " +
            s8_.ToString() + " " +
            v_.ToString() + " " +
            f_.ToString() + " " +
            d_.ToString() + " " +
            o_.ToString() + " " +
            s_.ToString() + " " +
            e_.ToString() + " " +
            p1_.ToString() + " " +
            p2_.ToString() + " " +
            p3_.ToString() + " " +
            p4_.ToString() + " " +
            p5_.ToString() + " " +
            p6_.ToString() + " " +
            p7_.ToString() + " " +
            p8_.ToString() + " " +
            ".";

        if (values != "226 161 153 217 24 177 105 8 65 240 248 " +
            "209 224 41 72 89 152 49 128 137 232 97 208 145 9760 68383465 -100628058569588952" +
            "8 -1283321389773510207 -27688336 8985 216 -7.769602E+18 -3.84749538525879E+18 Ma" +
            "inApp 2212973731230676456 Hali -102 153 -29822 14240 1493680594 2427897976 59408" +
            "3122152577906 15205575963204995536 .")
        {
            Console.WriteLine("Locals after Invoke: unexpected values: " + values);
            Fail(104);
        }

        Console.WriteLine("Method invokes succeeded");

        // get all fields
        s1_ = GetType().InvokeMember("_s1",
            BindingFlags.Instance | BindingFlags.Public | BindingFlags.GetField, null,
            this, null, null, null, null);
        s2_ = GetType().InvokeMember("_s2",
            BindingFlags.Static | BindingFlags.Public | BindingFlags.GetField, null,
            this, null, null, null, null);
        s3_ = GetType().InvokeMember("_s3",
            BindingFlags.Instance | BindingFlags.Public | BindingFlags.GetField, null,
            this, null, null, null, null);
        s4_ = GetType().InvokeMember("_s4",
            BindingFlags.Instance | BindingFlags.Public | BindingFlags.GetField, null,
            this, null, null, null, null);
        s5_ = GetType().InvokeMember("_s5",
            BindingFlags.Instance | BindingFlags.Public | BindingFlags.GetField, null,
            this, null, null, null, null);
        s8_ = GetType().InvokeMember("_s8",
            BindingFlags.Instance | BindingFlags.Public | BindingFlags.GetField, null,
            this, null, null, null, null);
        v_ = GetType().InvokeMember("_v",
            BindingFlags.Instance | BindingFlags.Public | BindingFlags.GetField, null,
            this, null, null, null, null);
        f_ = GetType().InvokeMember("_f",
            BindingFlags.Instance | BindingFlags.Public | BindingFlags.GetField, null,
            this, null, null, null, null);
        d_ = GetType().InvokeMember("_d",
            BindingFlags.Instance | BindingFlags.Public | BindingFlags.GetField, null,
            this, null, null, null, null);
        o_ = GetType().InvokeMember("_o",
            BindingFlags.Instance | BindingFlags.Public | BindingFlags.GetField, null,
            this, null, null, null, null);
        s_ = GetType().InvokeMember("_s",
            BindingFlags.Instance | BindingFlags.Public | BindingFlags.GetField, null,
            this, null, null, null, null);
        e_ = GetType().InvokeMember("_e",
            BindingFlags.Instance | BindingFlags.Public | BindingFlags.GetField, null,
            this, null, null, null, null);
        p1_ = GetType().InvokeMember("_p1",
            BindingFlags.Instance | BindingFlags.Public | BindingFlags.GetField, null,
            this, null, null, null, null);
        p2_ = GetType().InvokeMember("_p2",
            BindingFlags.Static | BindingFlags.Public | BindingFlags.GetField, null,
            this, null, null, null, null);
        p3_ = GetType().InvokeMember("_p3",
            BindingFlags.Static | BindingFlags.Public | BindingFlags.GetField, null,
            this, null, null, null, null);
        p4_ = GetType().InvokeMember("_p4",
            BindingFlags.Instance | BindingFlags.Public | BindingFlags.GetField, null,
            this, null, null, null, null);
        p5_ = GetType().InvokeMember("_p5",
            BindingFlags.Instance | BindingFlags.Public | BindingFlags.GetField, null,
            this, null, null, null, null);
        p6_ = GetType().InvokeMember("_p6",
            BindingFlags.Instance | BindingFlags.Public | BindingFlags.GetField, null,
            this, null, null, null, null);
        p7_ = GetType().InvokeMember("_p7",
            BindingFlags.Instance | BindingFlags.Public | BindingFlags.GetField, null,
            this, null, null, null, null);
        p8_ = GetType().InvokeMember("_p8",
            BindingFlags.Instance | BindingFlags.Public | BindingFlags.GetField, null,
            this, null, null, null, null);

        // verify the values of all locals
        values =
            s1_.ToString() + " " +
            s2_.ToString() + " " +
            s3_.ToString() + " " +
            s4_.ToString() + " " +
            s5_.ToString() + " " +
            s8_.ToString() + " " +
            v_.ToString() + " " +
            f_.ToString() + " " +
            d_.ToString() + " " +
            o_.ToString() + " " +
            s_.ToString() + " " +
            e_.ToString() + " " +
            p1_.ToString() + " " +
            p2_.ToString() + " " +
            p3_.ToString() + " " +
            p4_.ToString() + " " +
            p5_.ToString() + " " +
            p6_.ToString() + " " +
            p7_.ToString() + " " +
            p8_.ToString() + " " +
            ".";

        if (values != "9 104 225 80 57 184 17 160 105 8 65 240 153 88 113" +
            " 64 201 168 161 144 249 248 209 224 -23767 606228808 2874968872843236865 -901615" +
            "9779942285264 360942425 4248 49 -5.193024E+18 -6.58392565490175E+18 MainApp 2212" +
            "973731230676456 Hali 97 208 -9799 49464 1739539345 3300757792 -65179382391756062" +
            "95 1703311890815005320 .")
        {
            Console.WriteLine("GetFields: unexpected values: " + values);
            Fail(105);
        }

        Console.WriteLine("Field getters succeeded");

        // invoke the method with complex signature
        GetType().InvokeMember("Foo",
            BindingFlags.Static | BindingFlags.Public | BindingFlags.InvokeMethod, null,
            null, new Object[] {
                this, "O", "S",
                (float)Randomizer(), (double)Randomizer(), (float)Randomizer(),
                (double)Randomizer(), (float)Randomizer(), (double)Randomizer(),
                (long)Randomizer(), (long)Randomizer(), (long)Randomizer(),
                (long)Randomizer(), (long)Randomizer(), (long)Randomizer(),
                (sbyte)Randomizer(), (byte)Randomizer(), (short)Randomizer(),
                (ushort)Randomizer(), (int)Randomizer(), (uint)Randomizer(),
                (long)Randomizer(), (ulong)Randomizer(),
                (float)Randomizer(), (double)Randomizer(), (float)Randomizer(),
                (double)Randomizer(), (float)Randomizer(), (double)Randomizer()
            }, null, null, null);

        Console.WriteLine("Complex invoke succeeded");

        // invoke the constructor with complex signature
        GetType().InvokeMember("Foo",
            BindingFlags.Instance | BindingFlags.Public | BindingFlags.CreateInstance, null,
            null, new Object[] {
                this,
                s1_, s2_, s3_, s4_, s5_, s8_, v_, f_, d_, o_, s_, e_,
            }, null, null, null);
        Console.WriteLine("Constructor invoke succeeded");
    }

    public static int Main() {
        new MainApp().RunTests();
        Console.WriteLine("All test passed");
        return 0;
    }
}
