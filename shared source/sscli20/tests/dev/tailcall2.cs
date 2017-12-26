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
using System.Globalization;
using System.IO;

public class Class1 {

    static int MaxDepth = 100000;

    int value;

    public int Recurse1(int depth) {
        if (depth == 0) return value;
        value += depth;

        if (depth % 128 == 43) return Recurse3(depth-1);

        return Recurse2(depth-1, depth, depth+1);
    }

    public int Recurse2(int depth, Object o1, Object o2) {
        if (depth == 0) return value;

        if (depth % (MaxDepth/10) == 100) {
            string s = Environment.StackTrace;
            if (s.IndexOf("Main") == -1) {
                Console.WriteLine("Unexpected stack trace: " + s);
                Console.WriteLine("Test Failed");
                Environment.Exit(0);
            }
            GC.Collect();
        }

        value += (int)o1 + (int)o2;

        return Recurse3(depth-1);
    }

    public virtual int Recurse3(int depth) {
        if (depth == 0) return value;

        if (depth % 128 == 21) return Recurse1(depth-1);

        return Recurse4(depth-1, depth+1, depth+2);
    }

    public virtual int Recurse4(int depth, Object o1, Object o2) {
        if (depth == 0) return value;

        if (depth % (MaxDepth/10) == 200) {
            GC.Collect();

            string s = Environment.StackTrace;
            if (s.IndexOf("Main") == -1) {
                Console.WriteLine("Unexpected stack trace: " + s);
                Console.WriteLine("Test Failed");
                Environment.Exit(0);
            }
        }

        value += (int)o1 + (int)o2;

        return Recurse1(depth-1);
    }


    public static int Main() {
        Console.WriteLine("Test Start");

        Class1 m = new Class1();
        m.Recurse1(MaxDepth);

        int expected = 1;
        if (m.value != expected) {
            Console.WriteLine("Expected result: " + 1 + "Actual result: " + m.value);
            Console.WriteLine("Test Failed");
            return 100; 
        }

        Console.WriteLine("Test Passed");
        return 0;
    }
}
