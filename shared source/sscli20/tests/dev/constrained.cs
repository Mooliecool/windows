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

//
// This is .cs file on which the actual test, constrained.il, is based on.
// You cannot simply compile this, ildasm the .exe and use that .il file
// directly. You must manually changed the ldfld for the this pointer to
// ldflda and also add the constrained prefix
//
using System;

public class ConstrainedTests
{
    private static int s_failed = 0;
    public static int Main()
    {
        try
        {
            Container<ValType> cVal = new Container<ValType>(new ValType(23));
            Container<RefType> cRef = new Container<RefType>(new RefType()); 
            Container<ValTypeNewImpl> cValNew = new Container<ValTypeNewImpl>(new ValTypeNewImpl(3));

            ValTypeNewImpl vtni = new ValTypeNewImpl();

            ConstrainedTests.Test(cRef.ToStr() == "RefType" , "Ref.ToStr"); // this is a normal virtcall
            ConstrainedTests.Test(cVal.ToStr() == "ValType" , "Val.ToStr"); // this should box
            ConstrainedTests.Test(cValNew.ToStr() == "Foo" , "ValNew.ToStr"); // this should box

            ConstrainedTests.Test(cRef.DoCall() == 2, "Ref.DoCall"); // normal virtcall
            ConstrainedTests.Test(cVal.DoCall() == 23, "Val.DoCall"); // constrained, no box
            ConstrainedTests.Test(cValNew.DoCall() == 3, "ValNew.DoCall"); // constrained, no box

            ConstrainedTests.Test(cRef.Eq() == true, "Ref.Eq"); // normal virtcall
            ConstrainedTests.Test(cVal.Eq() == true, "Val.Eq"); // must box (testing w/ args)
            ConstrainedTests.Test(cValNew.Eq() == false, "ValNew.Eq"); // must box (testing w/ args)

            ConstrainedTests.Test(cRef.BigArgs(1) == 0xff1, "Ref.BigArgs"); // normal virtcall
            ConstrainedTests.Test(cVal.BigArgs(2) == 0xff2, "Val.BigArgs"); // no box
            ConstrainedTests.Test(cValNew.BigArgs(3) == 0xff3, "ValNew.BigArgs"); // no box
        } 
        catch (Exception e)
        {
            Console.WriteLine("Caught Unexpected Exception");
            Console.WriteLine(e.ToString());
            return 1;
        }

        return s_failed;
    }
    public static void Test(bool test, string msg)
    {
        if(test)
        {
            Console.WriteLine("PASSED " + msg);
        }
        else
        {
            Console.WriteLine("FAILED " + msg);
            s_failed = 1;
        }
    }
}

public class Container<T> where T : IFoo
{
    IntHolder ih = new IntHolder();
    T a;
    public Container(T _a)
    {
        a = _a;
        ih.x = 0xf;
        ih.y = 0xf0;
        ih.z = 0xf00;
    }
    public int DoCall()
    {
        return a.func(); 
    }
    public string ToStr()
    {
        return a.ToString();
    }  
    public bool Eq()
    {
        return a.Equals(a);
    }
    public int BigArgs(int x)
    {
        return a.big(ih,ih,x);
    }
}

public interface IFoo
{
    int func();
    int big(IntHolder ih1, IntHolder ih2, int x);
}

public struct ValType : IFoo
{
    public int x;
    public ValType(int x_)
    {
        x = x_;
    }
    public int func()
    {
        return x;
    }
    public int normal()
    {
        return x;
    }
    public int big(IntHolder ih1, IntHolder ih2, int x)
    {
        return ih1.y + ih2.z + x;
    }
}

public class RefType : IFoo
{
    public int x = 2;

    public int func()
    {
        return x;
    }
    public int big(IntHolder ih1, IntHolder ih2, int x)
    {
        return ih1.y + ih2.z + x;
    }
}

public struct ValTypeNewImpl : IFoo
{
    public int x;

    public ValTypeNewImpl(int x_)
    {
        x = x_;
    }

    public int func()
    {
        return x;
    }
    public override string ToString()
    {
        return "Foo";
    }
    public override bool Equals(Object obj)
    {
        return false;
    }
    public int big(IntHolder ih1, IntHolder ih2, int x)
    {
        return ih1.y + ih2.z + x;
    }
}

public struct IntHolder
{
    public int x,y,z;
}
