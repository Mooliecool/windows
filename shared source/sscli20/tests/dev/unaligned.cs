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

// this is the template used to generate unaligned.il

using System;
using System.Runtime.InteropServices;

struct SmallValue {
    public double x;
};

[StructLayout(LayoutKind.Explicit)]
struct UnalignedValue {
    [FieldOffset(1)]
    public sbyte a;
    [FieldOffset(1)]
    public short b;
    [FieldOffset(1)]
    public int c;
    [FieldOffset(1)]
    public long d;
    [FieldOffset(1)]
    public SmallValue x;
};

class MainApp {
    
    unsafe public static int Main() {
        UnalignedValue v = new UnalignedValue();

        v.d = 0x0FFFFFFFFFFFFFF0;
        v.c = 0x77777777;
        v.b = 0x3333;
        v.a = 0x11;

        long sum = v.a + v.b + v.c + v.d;

        if (sum != (BitConverter.IsLittleEndian ? 0x0FFFFFFFEEEE9944 : 0x11337778113388AB)) {
            Console.WriteLine("Unexpected long value:" + sum.ToString());
            Environment.Exit(1);            
        }

        SmallValue t;
        t = v.x;
        t.x += 1.0;
        v.x = t;

        byte* pa = (byte*)&v.a;
        ushort* pb = (ushort*)&v.b;
        uint* pc = (uint*)&v.c;
        ulong* pd = (ulong*)&v.d;
        
        *pd = 0x0EEEEEEEEEEEEEE0;
        *pc = 0x66666666;
        *pb = 0x2222;
        *pa = 0x00;

        ulong usum = (ulong)*pa + (ulong)*pb + (ulong)*pc + (ulong)*pd;

        if (usum != (BitConverter.IsLittleEndian ? 0x0EEEEEEECCCC6600ul : 0x226666EF115568)) {
            Console.WriteLine("Unexpected ulong value:" + usum.ToString());
            Environment.Exit(2);            
        }

        Console.WriteLine("PASSED");
        return 0;
    }
}
