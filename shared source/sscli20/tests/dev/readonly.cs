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
// This is .cs file on which the actual test, readonly.il, is based on.
// I used my own custom tool that allows one to place inline IL instructions
// interspersed with the csharp code. 
//

using System;
using System.Security;

public class ILASM
{
   public static void Inline(string cmd) {} 
}

public interface IPoint
{
    void setx(int x_);
    void sety(int y_);
    void setz(int z_);
    int getx();
    int gety();
    int getz();
}

public struct ValType : IPoint
{
    public int x;
    public int y;
    public int z;
    public ValType(int x_, int y_, int z_)
    {
        x = x_;
        y = y_;
        z = z_;
    }
    public void setx(int x_) { x = x_; }
    public void sety(int y_) { y = y_; }
    public void setz(int z_) { z = z_; }
    public int getx() { return x; }
    public int gety() { return y; }
    public int getz() { return z; }
}

public class RefType : IPoint
{
    public int x;
    public int y;
    public int z;
    public RefType(int x_, int y_, int z_)
    {
        x = x_;
        y = y_;
        z = z_;
    }
    public void setx(int x_) { x = x_; }
    public void sety(int y_) { y = y_; }
    public void setz(int z_) { z = z_; }
    public int getx() { return x; }
    public int gety() { return y; }
    public int getz() { return z; }
}

public class FixedStack<T> where T : IPoint
{
    public T [] m_data;
    public int  m_top;

    public FixedStack(int size)
    {
        m_data = new T[size];
        m_top  = 0;
    }
    public void Push(T datum)
    {
        m_data[m_top++] = datum; 
    }

    public void stobjTest(T obj)
    {
        bool caught = false;

        stobjVerifyPass(obj);

        try
        {
            stobjVerifyFail(obj);
        } 
        catch (VerificationException verexcept)
        {
            caught = true;
        } 
        finally
        {
            ReadonlyTests.Test(caught, "stobjTest");
        }
    }

    private void stobjVerifyPass(T obj)
    {
        ILASM.Inline(".maxstack 10");
        ReadonlyTests.Test(m_data[0].getx() == 100,"stobj1 x"); 
        ReadonlyTests.Test(m_data[0].gety() == 101,"stobj1 y"); 
        ReadonlyTests.Test(m_data[0].getz() == 102,"stobj1 z"); 
        ILASM.Inline("ldarg.0");
        ILASM.Inline("ldfld      !0[] class FixedStack<!0>::m_data");
        ILASM.Inline("ldc.i4.0");
        ILASM.Inline("ldelema !0");
        ILASM.Inline("ldarg.1");
        ILASM.Inline("stobj !0");
        ReadonlyTests.Test(m_data[0].getx() == 345,"stobj2 x"); 
        ReadonlyTests.Test(m_data[0].gety() == 346,"stobj2 y"); 
        ReadonlyTests.Test(m_data[0].getz() == 347,"stobj2 z"); 
    }

    private void stobjVerifyFail(T obj)
    {
        ILASM.Inline(".maxstack 10");
        ILASM.Inline("ldarg.0");
        ILASM.Inline("ldfld      !0[] class FixedStack<!0>::m_data ");
        ILASM.Inline("ldc.i4.0");
        ILASM.Inline("readonly.");
        ILASM.Inline("ldelema !0    ");
        ILASM.Inline("ldarg.1");
        ILASM.Inline("stobj !0  ");
    }

    public void initobjTest()
    {
        bool caught = false;
        initobjVerifyPass();
        try
        {
            initobjVerifyFail();
        } 
        catch (VerificationException verexcept)
        {
            verexcept = null;
            caught = true;
        } 
        finally
        {
            ReadonlyTests.Test(caught, "initobjTest");
        }
    }

    private void initobjVerifyPass()
    {
        ILASM.Inline(".maxstack 10");
        //Console.WriteLine("m_data x " + m_data[0].getx().ToString());
        ReadonlyTests.Test(m_data[1].getx() == 101,"initobj1 x"); 
        ReadonlyTests.Test(m_data[1].gety() == 102,"initobj1 y"); 
        ReadonlyTests.Test(m_data[1].getz() == 103,"initobj1 z"); 
        ILASM.Inline("ldarg.0");
        ILASM.Inline("ldfld      !0[] class FixedStack<!0>::m_data");
        ILASM.Inline("ldc.i4.1");
        ILASM.Inline("ldelema !0");
        ILASM.Inline("initobj !0");
        //
        // this only succeeds with value types)
        // with reftype m_data[0] itself becomes null and it errors out
        //
        ReadonlyTests.Test(m_data[1].getx() == 0,"initobj2 x"); 
        ReadonlyTests.Test(m_data[1].gety() == 0,"initobj2 y"); 
        ReadonlyTests.Test(m_data[1].getz() == 0,"initobj2 z"); 
    }

    private void initobjVerifyFail()
    {
        ILASM.Inline(".maxstack 10");
        ILASM.Inline("ldarg.0");
        ILASM.Inline("ldfld      !0[] class FixedStack<!0>::m_data");
        ILASM.Inline("ldc.i4.1");
        ILASM.Inline("readonly.");
        ILASM.Inline("ldelema !0");
        ILASM.Inline("initobj !0");
    }

    public void mkrefanyTest()
    {
        bool caught = false;
        mkrefanyVerifyPass();
        try
        {
            mkrefanyVerifyFail();
        } 
        catch (VerificationException verexcept)
        {
            verexcept = null;
            caught = true;
        } 
        finally
        {
            ReadonlyTests.Test(caught, "mkrefanyTest");
        }
    }

    private void mkrefanyVerifyPass()
    {
        ILASM.Inline(".maxstack 10");
        ILASM.Inline("ldarg.0");
        ILASM.Inline("ldfld      !0[] class FixedStack<!0>::m_data ");
        ILASM.Inline("ldc.i4.2");
        ILASM.Inline("ldelema !0    ");
        ILASM.Inline("mkrefany !0");
        ILASM.Inline("pop");
    }

    private void mkrefanyVerifyFail()
    {
        ILASM.Inline(".maxstack 10");
        ILASM.Inline("ldarg.0");
        ILASM.Inline("ldfld      !0[] class FixedStack<!0>::m_data ");
        ILASM.Inline("ldc.i4.2");
        ILASM.Inline("readonly.");
        ILASM.Inline("ldelema !0    ");
        ILASM.Inline("mkrefany !0");
        ILASM.Inline("pop");
    }

    public void cpobjTest(T obj)
    {
        bool caught = false;
        cpobjVerifyPass(obj);
        try
        {
            cpobjVerifyFail(obj);
        } 
        catch (VerificationException verexcept)
        {
            verexcept = null;
            caught = true;
        } 
        finally
        {
            ReadonlyTests.Test(caught, "cpobjTest");
        }
    }

    private void cpobjVerifyPass(T obj)
    {
        ILASM.Inline(".maxstack 8");
        T temp = obj;
        //
        // readonly source is OK
        //
        ILASM.Inline("ldarg.1");
        ILASM.Inline("stloc.0 ");
        temp.setx(1);
        temp.sety(1);
        temp.setz(1);
        //
        // cpobj from m_data[3] to temp
        //
        ILASM.Inline("ldloca 0 ");
        ILASM.Inline("ldarg.0");
        ILASM.Inline("ldfld      !0[] class FixedStack<!0>::m_data ");
        ILASM.Inline("ldc.i4.3");
        ILASM.Inline("readonly.");
        ILASM.Inline("ldelema !0    ");
        ILASM.Inline("cpobj !0");
        ReadonlyTests.Test(temp.getx() == 103,"cpobj1 x"); 
        ReadonlyTests.Test(temp.gety() == 104,"cpobj1 y"); 
        ReadonlyTests.Test(temp.getz() == 105,"cpobj1 z"); 
        temp.setx(1);
        temp.sety(1);
        temp.setz(1);
        // this fails with reference types, but succeeds with value types
        // with reference semantics m_data[3].x,y and z become 1 
        ReadonlyTests.Test(m_data[3].getx() == 103,"cpobj2 x"); 
        ReadonlyTests.Test(m_data[3].gety() == 104,"cpobj2 y"); 
        ReadonlyTests.Test(m_data[3].getz() == 105,"cpobj2 z"); 
        //
        // no readonly is OK
        //
        // cpobj from m_data[3] to temp
        //
        ILASM.Inline("ldarg.0");
        ILASM.Inline("ldfld      !0[] class FixedStack<!0>::m_data ");
        ILASM.Inline("ldc.i4.3");
        ILASM.Inline("ldelema !0");
        ILASM.Inline("ldloca 0 ");
        ILASM.Inline("cpobj !0");
        //for(int i = 0; i < 10 ; ++i)
        //{
        //    Console.WriteLine("i: " + i.ToString() + " data: " + m_data[i].getx().ToString());
        //}
        ReadonlyTests.Test(temp.getx() == 103,"cpobj3 x"); 
        ReadonlyTests.Test(temp.gety() == 104,"cpobj3 y"); 
        ReadonlyTests.Test(temp.getz() == 105,"cpobj3 z"); 
    }

    private void cpobjVerifyFail(T obj)
    {
        //
        // readonly dest fails
        //
        ILASM.Inline(".maxstack 8");
        T temp;
        ILASM.Inline("ldarg.1");
        ILASM.Inline("stloc.0  ");
        ILASM.Inline("ldarg.0");
        ILASM.Inline("ldfld      !0[] class FixedStack<!0>::m_data ");
        ILASM.Inline("ldc.i4.3");
        ILASM.Inline("readonly.");
        ILASM.Inline("ldelema !0    ");
        ILASM.Inline("ldloca 0");
        ILASM.Inline("cpobj !0");
    }



    public void constrainedTest()
    {
        bool caught = false;
        try
        {
            constrainedVerifyPass();
        } 
        catch (VerificationException verexcept)
        {
            verexcept = null;
            caught = true;
        } 
        finally
        {
            ReadonlyTests.Test(!caught, "constrainedTest");
        }
    }

    private void constrainedVerifyPass()
    {
        ILASM.Inline(".maxstack 8");
        int result = 0;
        ILASM.Inline("ldarg.0");
        ILASM.Inline("ldfld      !0[] class FixedStack<!0>::m_data");
        ILASM.Inline("ldc.i4.4");
        ILASM.Inline("readonly.");
        ILASM.Inline("ldelema !0");
        ILASM.Inline("constrained. !0");
        ILASM.Inline("callvirt instance int32 IPoint::getx()");
        ILASM.Inline("stloc.0");
        ReadonlyTests.Test(result == 104,"constrainedVerifyPass");
        //Console.WriteLine("result: " + result.ToString());
        //for(int i = 0; i < 10; ++i)
        //{
           //Console.WriteLine("i: " + i.ToString() + " data: " + m_data[i].getx().ToString());
        //}
    }
    
    public void basicBlockTest()
    {
        basicBlockPass();

        bool caught = false;
        try
        {
            basicBlockFail1();
        } 
        catch (VerificationException verexcept)
        {
            verexcept = null;
            caught = true;
        } 
        finally
        {
            ReadonlyTests.Test(caught, "basicBlockFail1");
        }
        caught = false;
        try
        {
            basicBlockFail2();
        } 
        catch (VerificationException verexcept)
        {
            verexcept = null;
            caught = true;
        } 
        finally
        {
            ReadonlyTests.Test(caught, "basicBlockFail2");
        }
    }

    private void basicBlockFail1()
    {
        ILASM.Inline(".maxstack 8");
        bool takeme = true;
        if(!takeme)
        {
            ILASM.Inline("ldarg.0");
            ILASM.Inline("ldfld      !0[] class FixedStack<!0>::m_data");
            ILASM.Inline("ldc.i4.5");
            ILASM.Inline("ldelema !0");
        }
        else
        {
            // note no readonly here
            ILASM.Inline("ldarg.0");
            ILASM.Inline("ldfld      !0[] class FixedStack<!0>::m_data");
            ILASM.Inline("ldc.i4.6");
            ILASM.Inline("readonly.");
            ILASM.Inline("ldelema !0");
        }
        ILASM.Inline("initobj !0"); // verification error     
    }

    private void basicBlockFail2()
    {
        ILASM.Inline(".maxstack 8");
        bool takeme = true;
        if(takeme)
        {
            // note no readonly here
            ILASM.Inline("ldarg.0");
            ILASM.Inline("ldfld      !0[] class FixedStack<!0>::m_data");
            ILASM.Inline("ldc.i4.6");
            ILASM.Inline("readonly.");
            ILASM.Inline("ldelema !0");
        }
        else
        {
            ILASM.Inline("ldarg.0");
            ILASM.Inline("ldfld      !0[] class FixedStack<!0>::m_data");
            ILASM.Inline("ldc.i4.5");
            ILASM.Inline("ldelema !0");
        }
        ILASM.Inline("initobj !0"); // verification error     
    }

    private void basicBlockPass()
    {
        ILASM.Inline(".maxstack 8");
        bool takeme = true;
        int result = 0;
        if(takeme) 
        {
            ILASM.Inline("ldarg.0");
            ILASM.Inline("ldfld      !0[] class FixedStack<!0>::m_data");
            ILASM.Inline("ldc.i4.5");
            ILASM.Inline("readonly.");
            ILASM.Inline("ldelema !0");
        } else {
            ILASM.Inline("ldarg.0");
            ILASM.Inline("ldfld      !0[] class FixedStack<!0>::m_data");
            ILASM.Inline("ldc.i4.6");
            ILASM.Inline("readonly.");
            ILASM.Inline("ldelema !0");
        }
        ILASM.Inline("constrained. !0");
        ILASM.Inline("callvirt instance int32 IPoint::getx()");
        ILASM.Inline("stloc.1");
        ReadonlyTests.Test(result == 105,"basicBlockPass");

        if(!takeme)
        {
            ILASM.Inline("ldarg.0");
            ILASM.Inline("ldfld      !0[] class FixedStack<!0>::m_data");
            ILASM.Inline("ldc.i4.5");
            ILASM.Inline("readonly.");
            ILASM.Inline("ldelema !0");
        }
        else
        {
            // note no readonly here
            ILASM.Inline("ldarg.0");
            ILASM.Inline("ldfld      !0[] class FixedStack<!0>::m_data");
            ILASM.Inline("ldc.i4.6");
            ILASM.Inline("ldelema !0");
        }
        ILASM.Inline("constrained. !0");
        ILASM.Inline("callvirt instance int32 IPoint::getx()");
        ILASM.Inline("stloc.1");
        ReadonlyTests.Test(result == 106,"basicBlockPass");

    }
}


public class ReadonlyTests
{
    private static int s_failed = 0;
    public static int Main()
    {
        try
        {
            Console.WriteLine("****ValType Tests****");

            FixedStack<ValType> valTypeStack = new FixedStack<ValType>(100);
            for(int i = 0; i < 100; ++i)
            {
                valTypeStack.Push(new ValType(i+100,i+101,i+102));
            }

            // verification failures
            valTypeStack.stobjTest(new ValType(345,346,347));
            valTypeStack.initobjTest();
            valTypeStack.mkrefanyTest();
            valTypeStack.cpobjTest(new ValType(456,457,458));

            // verification successes
            valTypeStack.constrainedTest();            
            valTypeStack.basicBlockTest();


            FixedStack<RefType> refTypeStack = new FixedStack<RefType>(100);
            for(int i = 0; i < 100; ++i)
            {
                refTypeStack.Push(new RefType(i+100,i+101,i+102));
            }

         
            //
            // omitting the following test cases for references types,
            // because "generalizing" these instructions doesn't really allow
            // you to write generic test cases
            // the reason is that the "generalized" *obj functions have
            // reference semantics for reference types and value semantics
            // for value types. You can't really program generically in that
            // case. If, for example, initobj meant "replace object with object
            // constructed by default constructor" you could program generically
            // 1) initobj - in the case of value types, initobj intializes the
            // the value, whereas in reference types the reference is set to
            // null. I can't see any way to test this in a generic fashion.
            // 2) cpobj - omitted for same reasons, i put some comments inline
            
            Console.WriteLine("****RefType Tests****");
            
            refTypeStack.stobjTest(new RefType(345,346,347));
            //refTypeStack.initobjTest();
            refTypeStack.mkrefanyTest();
            //refTypeStack.cpobjTest(new RefType(456,457,458));

            // verification successes
            refTypeStack.constrainedTest();            

            callTest();            
            callvirtTest();            

            // stind tests (verification failures)
            // i1 i2 i4 i8 r4 r8 i ref

            stind_i1Test();
            stind_i2Test();
            stind_i4Test();
            stind_i8Test();
            stind_r4Test();
            stind_r8Test();
            stind_iTest();
            stind_refTest();


        }
        catch (Exception e)
        {
            Console.WriteLine("Caught Unexpected Exception");
            Console.WriteLine(e.ToString());
            return 1;
        }

        return s_failed;
    }

    public static void callTest()
    {

        bool caught = false;
        try
        {
            callVerifyPass();
        } 
        catch (VerificationException verexcept)
        {
            verexcept = null;
            caught = true;
        } 
        finally
        {
            ReadonlyTests.Test(!caught, "callTest");
        }
    }

    private static void callVerifyPass()
    {
        ILASM.Inline(".maxstack 8");
        ValType [] varr = new ValType[1];
        varr[0] = new ValType(1,2,3);
        int result = 0;
        ILASM.Inline("ldloc.0"); 
        ILASM.Inline("ldc.i4.0");
        ILASM.Inline("readonly.");
        ILASM.Inline("ldelema ValType");
        ILASM.Inline("call instance int32 ValType::getx()");
        ILASM.Inline("stloc.1");
        ReadonlyTests.Test(result == 1, "callVerifyPass");
    }

    public static void callvirtTest()
    {

        bool caught = false;
        try
        {
            callvirtVerifyPass();
        } 
        catch (VerificationException verexcept)
        {
            verexcept = null;
            caught = true;
        } 
        finally
        {
            ReadonlyTests.Test(!caught, "callvirtTest");
        }
    }

    private static void callvirtVerifyPass()
    {
    }

    public static void stind_i1Test()
    {
        bool caught = false;
        stind_i1Pass();
        try
        {
            stind_i1Fail(); 
        } 
        catch (VerificationException verexcept)
        {
            verexcept = null;
            caught = true;
        }
        finally
        {
            ReadonlyTests.Test(caught, "stind_i1Test");
        }
    }

    public static void stind_i1Pass()
    {
        Byte [] arrayByte = new Byte[1];
        Byte tempByte     = 101;
        arrayByte[0]      = 1;
        ILASM.Inline("ldloc.0");
        ILASM.Inline("ldc.i4.0");
        ILASM.Inline("ldelema [mscorlib]System.Byte");
        ILASM.Inline("ldloc.1");
        ILASM.Inline("stind.i1");
        ReadonlyTests.Test(arrayByte[0] == 101, "stind_i1Pass");
    }

    public static void stind_i1Fail()
    {
        Byte [] arrayByte = new Byte[1];
        Byte tempByte     = 101;
        arrayByte[0]      = 1;
        ILASM.Inline("ldloc.0");
        ILASM.Inline("ldc.i4.0");
        ILASM.Inline("readonly.");
        ILASM.Inline("ldelema [mscorlib]System.Byte");
        ILASM.Inline("ldloc.1");
        ILASM.Inline("stind.i1");
    }

    public static void stind_i2Test()
    {
        bool caught = false;
        stind_i2Pass();
        try
        {
            stind_i2Fail();
        } 
        catch (VerificationException verexcept)
        {
            verexcept = null;
            caught = true;
        }
        finally
        {
            ReadonlyTests.Test(caught, "stind_i2Test");
        }
    }

    public static void stind_i2Pass()
    {
        Int16 [] arrayInt16 = new Int16[1];
        Int16 tempInt16     = 101;
        arrayInt16[0]      = 1;
        ILASM.Inline("ldloc.0");
        ILASM.Inline("ldc.i4.0");
        ILASM.Inline("ldelema [mscorlib]System.Int16");
        ILASM.Inline("ldloc.1");
        ILASM.Inline("stind.i2");
        ReadonlyTests.Test(arrayInt16[0] == 101, "stind_i2Pass");
    }

    public static void stind_i2Fail()
    {
        Int16 [] arrayInt16 = new Int16[1];
        Int16 tempInt16     = 101;
        arrayInt16[0]      = 1;
        ILASM.Inline("ldloc.0");
        ILASM.Inline("ldc.i4.0");
        ILASM.Inline("readonly.");
        ILASM.Inline("ldelema [mscorlib]System.Int16");
        ILASM.Inline("ldloc.1");
        ILASM.Inline("stind.i2");
    }

    public static void stind_i4Test()
    {
        bool caught = false;
        stind_i4Pass();
        try
        {
            stind_i4Fail();
        } 
        catch (VerificationException verexcept)
        {
            verexcept = null;
            caught = true;
        }
        finally
        {
            ReadonlyTests.Test(caught, "stind_i4Test");
        }
    }

    public static void stind_i4Pass()
    {
        Int32 [] arrayInt32 = new Int32[1];
        Int32 tempInt32     = 101;
        arrayInt32[0]      = 1;
        ILASM.Inline("ldloc.0");
        ILASM.Inline("ldc.i4.0");
        ILASM.Inline("ldelema [mscorlib]System.Int32");
        ILASM.Inline("ldloc.1");
        ILASM.Inline("stind.i4");
        ReadonlyTests.Test(arrayInt32[0] == 101, "stind_i4Pass");
    }

    public static void stind_i4Fail()
    {
        Int32 [] arrayInt32 = new Int32[1];
        Int32 tempInt32     = 101;
        arrayInt32[0]      = 1;
        ILASM.Inline("ldloc.0");
        ILASM.Inline("ldc.i4.0");
        ILASM.Inline("readonly.");
        ILASM.Inline("ldelema [mscorlib]System.Int32");
        ILASM.Inline("ldloc.1");
        ILASM.Inline("stind.i4");
    }

    public static void stind_i8Test()
    {
        bool caught = false;
        stind_i8Pass();
        try
        {
            stind_i8Fail();
            // stind
        } 
        catch (VerificationException verexcept)
        {
            verexcept = null;
            caught = true;
        }
        finally
        {
            ReadonlyTests.Test(caught, "stind_i8Test");
        }
    }

    public static void stind_i8Pass()
    {
        Int64 [] arrayInt64 = new Int64[1];
        Int64 tempInt64     = 101;
        arrayInt64[0]      = 1;
        ILASM.Inline("ldloc.0");
        ILASM.Inline("ldc.i4.0");
        ILASM.Inline("ldelema [mscorlib]System.Int64");
        ILASM.Inline("ldloc.1");
        ILASM.Inline("stind.i8");
        ReadonlyTests.Test(arrayInt64[0] == 101, "stind_i8Pass");
    }

    public static void stind_i8Fail()
    {
        Int64 [] arrayInt64 = new Int64[1];
        Int64 tempInt64     = 101;
        arrayInt64[0]      = 1;
        ILASM.Inline("ldloc.0");
        ILASM.Inline("ldc.i4.0");
        ILASM.Inline("readonly.");
        ILASM.Inline("ldelema [mscorlib]System.Int64");
        ILASM.Inline("ldloc.1");
        ILASM.Inline("stind.i8");
    }

    public static void stind_r4Test()
    {
        bool caught = false;
        stind_r4Pass();
        try
        {
            stind_r4Fail();
        } 
        catch (VerificationException verexcept)
        {
            verexcept = null;
            caught = true;
        }
        finally
        {
            ReadonlyTests.Test(caught, "stind_r4Test");
        }
    }

    public static void stind_r4Pass()
    {
        Single [] arraySingle = new Single[1];
        Single tempSingle     = 101;
        arraySingle[0]        = 1;
        ILASM.Inline("ldloc.0");
        ILASM.Inline("ldc.i4.0");
        ILASM.Inline("ldelema [mscorlib]System.Single");
        ILASM.Inline("ldloc.1");
        ILASM.Inline("stind.r4");
        ReadonlyTests.Test(arraySingle[0] == tempSingle, "stind_r4Pass");
    }

    public static void stind_r4Fail()
    {
        Single [] arraySingle = new Single[1];
        Single tempSingle     = 101;
        arraySingle[0]        = 1;
        ILASM.Inline("ldloc.0");
        ILASM.Inline("ldc.i4.0");
        ILASM.Inline("readonly.");
        ILASM.Inline("ldelema [mscorlib]System.Single");
        ILASM.Inline("ldloc.1");
        ILASM.Inline("stind.r4");
    }
    public static void stind_r8Test()
    {
        bool caught = false;
        stind_r8Pass();
        try
        {
            stind_r8Fail();
        } 
        catch (VerificationException verexcept)
        {
            verexcept = null;
            caught = true;
        }
        finally
        {
            ReadonlyTests.Test(caught, "stind_r8Test");
        }
    }

    public static void stind_r8Pass()
    {
        Double [] arrayDouble = new Double[1];
        Double tempDouble     = 101;
        arrayDouble[0]        = 1;
        ILASM.Inline("ldloc.0");
        ILASM.Inline("ldc.i4.0");
        ILASM.Inline("ldelema [mscorlib]System.Double");
        ILASM.Inline("ldloc.1");
        ILASM.Inline("stind.r8");
        ReadonlyTests.Test(arrayDouble[0] == tempDouble, "stind_r8Pass");
    }

    public static void stind_r8Fail()
    {
        Double [] arrayDouble = new Double[1];
        Double tempDouble     = 101;
        arrayDouble[0]        = 1;
        ILASM.Inline("ldloc.0");
        ILASM.Inline("ldc.i4.0");
        ILASM.Inline("readonly.");
        ILASM.Inline("ldelema [mscorlib]System.Double");
        ILASM.Inline("ldloc.1");
        ILASM.Inline("stind.r8");
    }

    public static void stind_iTest()
    {
        bool caught = false;
        stind_iPass();
        try
        {
            stind_iFail();
        } 
        catch (VerificationException verexcept)
        {
            verexcept = null;
            caught = true;
        }
        finally
        {
            ReadonlyTests.Test(caught, "stind_iTest");
        }
    }

    public static void stind_iPass()
    {
        IntPtr [] arrayIntPtr = new IntPtr[1];
        IntPtr tempIntPtr     = (IntPtr)101;
        arrayIntPtr[0]        = (IntPtr)1;
        ILASM.Inline("ldloc.0");
        ILASM.Inline("ldc.i4.0");
        ILASM.Inline("ldelema [mscorlib]System.IntPtr");
        ILASM.Inline("ldloc.1");
        ILASM.Inline("stind.i");
        ReadonlyTests.Test(arrayIntPtr[0] == tempIntPtr, "stind_iPass");
    }

    public static void stind_iFail()
    {
        IntPtr [] arrayIntPtr = new IntPtr[1];
        IntPtr tempIntPtr     = (IntPtr)101;
        arrayIntPtr[0]        = (IntPtr)1;
        ILASM.Inline("ldloc.0");
        ILASM.Inline("ldc.i4.0");
        ILASM.Inline("readonly.");
        ILASM.Inline("ldelema [mscorlib]System.IntPtr");
        ILASM.Inline("ldloc.1");
        ILASM.Inline("stind.i");
    }

    public static void stind_refTest()
    {
        bool caught = false;
        stind_refPass();
        try
        {
            stind_refFail();
        } 
        catch (VerificationException verexcept)
        {
            verexcept = null;
            caught = true;
        }
        finally
        {
            ReadonlyTests.Test(caught, "stind_refTest");
        }
    }

    public static void stind_refPass()
    {
        Object [] arrayObject = new Object[1];
        Object tempObject     = 101 as Object;
        arrayObject[0]        = 1 as Object;
        ILASM.Inline("ldloc.0");
        ILASM.Inline("ldc.i4.0");
        ILASM.Inline("ldelema [mscorlib]System.Object");
        ILASM.Inline("ldloc.1");
        ILASM.Inline("stind.ref");
        ReadonlyTests.Test(arrayObject[0] == tempObject, "stind_refPass");
        ReadonlyTests.Test(Object.ReferenceEquals(arrayObject[0],tempObject),"stind_refPass ReferenceEquals");
    }

    public static void stind_refFail()
    {
        Object [] arrayObject = new Object[1];
        Object tempObject     = 101 as Object;
        arrayObject[0]        = 1 as Object;
        ILASM.Inline("ldloc.0");
        ILASM.Inline("ldc.i4.0");
        ILASM.Inline("readonly.");
        ILASM.Inline("ldelema [mscorlib]System.Object");
        ILASM.Inline("ldloc.1");
        ILASM.Inline("stind.ref");
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
