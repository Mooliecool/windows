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

class BaseClass {

    public virtual int F1(string Msg) {
        Console.WriteLine("BaseClass.F1: {0}", Msg);
        return 1;
    }

    public virtual string F2(string Msg) {
        Console.WriteLine("BaseClass.F2: {0}", Msg);
        return "String returned from BaseClass.F2:" + Msg;
    }

    public string NonVirtualFunc(int n) {
        Console.WriteLine("BaseClass.NonVirtualFunc: {0}", n);
        return "String returned from BaseClass.NonVirtualFunc:" + n;
    }
}

class DerivedClass : BaseClass {

    public override int F1(string Msg) {
        Console.WriteLine("DerivedClass.F1: {0}", Msg);
        return 2;
    }

    public override string F2(string Msg) {
        Console.WriteLine("DerivedClass.F2: {0}", Msg);
        return "String returned from DerivedClass.F2:" + Msg;
    }

    public new string NonVirtualFunc(int n) {
        Console.WriteLine("DerivedClass.NonVirtualFunc: {0}", n*2);
        return "String returned from DerivedClass.NonVirtualFunc:" + n*2;
    }
}

class MainApp {

    static bool failed = false;
    
    static void CheckReturnedInt(int actual, int expected) {
        Console.WriteLine("actual={0} expected={1}", actual, expected);
        if (actual != expected) {
            Console.WriteLine("!!!!!!!!!!! Error detected !!!!!!!!!!!!");
            failed = true;
        }
    }

    static void CheckReturnedString(string actual, string expected) {
        Console.WriteLine("actual=\"{0}\" expected=\"{1}\"", actual, expected);
        if (actual != expected) {
            Console.WriteLine("!!!!!!!!!!! Error detected !!!!!!!!!!!!");
            failed = true;
        }
    }

    public static void Main() {

        BaseClass obj1=new BaseClass();
        CheckReturnedInt(obj1.F1("obj1"), 1);
        CheckReturnedString(obj1.F2("obj1"), "String returned from BaseClass.F2:obj1");
        CheckReturnedString(obj1.NonVirtualFunc(11), "String returned from BaseClass.NonVirtualFunc:11");
        Console.WriteLine("");

        DerivedClass obj2=new DerivedClass();
        CheckReturnedInt(obj2.F1("obj2"), 2);
        CheckReturnedString(obj2.F2("obj2"), "String returned from DerivedClass.F2:obj2");
        CheckReturnedString(obj2.NonVirtualFunc(15), "String returned from DerivedClass.NonVirtualFunc:30");
        Console.WriteLine("");

        BaseClass objectRef;
        objectRef = obj1;
        CheckReturnedInt(objectRef.F1("objectRef is now obj1"), 1);
        CheckReturnedString(objectRef.F2("objectRef is now obj1"), "String returned from BaseClass.F2:objectRef is now obj1");
        CheckReturnedString(objectRef.NonVirtualFunc(11), "String returned from BaseClass.NonVirtualFunc:11");
        Console.WriteLine("");

        objectRef = obj2;
        CheckReturnedInt(objectRef.F1("objectRef is now obj2"), 2);
        CheckReturnedString(objectRef.F2("objectRef is now obj2"), "String returned from DerivedClass.F2:objectRef is now obj2");
        CheckReturnedString(objectRef.NonVirtualFunc(15), "String returned from BaseClass.NonVirtualFunc:15");
        Console.WriteLine("");

        if (failed) {
             System.Environment.ExitCode = 1;
        }
        else {
             System.Environment.ExitCode = 0;
        }
        
    }
        
}
