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
// This is .cs file on which the actual test, tailunit.il, is based on.
// You cannot simply compile this, ildasm the .exe and use that .il file
// directly. You must manually add the tail. prefixes to the calls you want
// to be tail recursive.
//
using System;

public struct Point
{
    public int x;
    public int y;
}

public class TailTests
{
    private int m_failed = 0;
    private static int s_testInt = 0;
    public static int Main()
    {

        TailTests t = new TailTests();
        int foo = 0;
        Point p = new Point();
        double d = 0.0;

        try
        {
            foo = t.ManyArgs(100,0,1,2,3,4,5,6,7,8,9,0);
            t.Test(foo == 100, "ManyArgs");

            foo = 0;
            foo = TailTests.ManyArgsStatic(100,0,1,2,3,4,5,6,7,8,9,0);
            t.Test(foo == 100, "ManyArgsStatic");

            foo = 0;
            foo = t.Sum(0,10000000);
            t.Test(foo == 10000000, "SumBlowStack");

            foo = 0;
            foo = TailTests.SumStatic(0,10000000);
            t.Test(foo == 10000000, "SumStaticBlowStack");

            p.x = 1;
            p.y = 2;
            foo = t.StructFacSum(4,p); 
            t.Test(foo == 40 && p.x == 1 && p.y == 2,"StructShallowReg");

            d = t.DoubleFacRec(5,1);
            t.Test(d == 120.0,"DoubleFacRec");

            foo = t.fUneven(1,2,3);
            t.Test(foo == 16, "Uneven");

            p.x = 1;
            p.y = 2;
            foo = t.StructFacSumDeep(100000,p); 
            t.Test(foo == 200003 && p.x == 1 && p.y == 2,"StructRegDeep");

            p.x = 1;
            p.y = 2;
            foo = 0;
            foo = TailTests.StructFacSumDeepStatic(100000,p); 
            t.Test(foo == 200003 && p.x == 1 && p.y == 2,"StructRegDeepStatic");

            p.x = 1;
            p.y = 2;
            foo = 0;
            foo = t.StructFacSumDeepStack(100000,0,0.0,0.9,p,1); 
            t.Test(foo == 200003 && p.x == 1 && p.y == 2,"StructStackDeep");

            p.x = 1;
            p.y = 2;
            foo = 0;
            foo = TailTests.StructFacSumDeepStackStatic(100000,0,0.0,0.9,p,1); 
            t.Test(foo == 200003 && p.x == 1 && p.y == 2,"StructStackDeepStatic");

            p.x = p.y = 0;
            p = t.StructReturnVal(100000,0);
            t.Test(p.x == 100000 && p.y == 200000,"StructReturnDeep");

            p.x = p.y = 0;
            p = TailTests.StructReturnValStatic(100000,0);
            t.Test(p.x == 100000 && p.y == 200000,"StructReturnDeepStatic");

            s_testInt = 0;
            t.NoArgs();
            t.Test(s_testInt == 400000, "NoArgs");

            s_testInt = 0;
            TailTests.NoArgsStatic();
            t.Test(s_testInt == 400000, "NoArgsStatic");

            p.x = 1;
            p.y = 2;
            s_testInt = 100000;
            t.SameStacksString("foo",p);
            t.Test(s_testInt == 0, "SameStacks");
        } 
        catch (Exception e)
        {
            Console.WriteLine("Caught Unexpected Exception");
            Console.WriteLine(e.ToString());
            return 1;
        }

        return t.m_failed;
    }

    public void SameStacksString(string s, Point p)
    {
        if(--s_testInt == 0)
        {
            return;
        }
        this.SameStacksObject(s,p); 
    }

    public void SameStacksObject(object o, Point p)
    {
        if(--s_testInt == 0)
        {
            return;
        }
        this.SameStacksString((string)o,p);
    }

    public void NoArgs()
    {
        if(s_testInt == 400000) return;
        ++s_testInt;
        this.NoArgs();
    }

    public static void NoArgsStatic()
    {
        if(s_testInt == 400000) return;
        s_testInt++;
        TailTests.NoArgsStatic();
    }

    public void Test(bool test, string msg)
    {
        if(test)
        {
            Console.WriteLine("PASSED " + msg);
        }
        else
        {
            Console.WriteLine("FAILED " + msg);
            m_failed = 1;
        }
    }

    public Point StructReturnVal(int x, int sum)
    {
        if(x == 0)
        {
            Point p = new Point();
            p.x = sum;
            p.y = 2*sum;
            return p;
        }
        return StructReturnVal(x-1,sum+1);
    }

    public static Point StructReturnValStatic(int x, int sum)
    {
        if(x == 0)
        {
            Point p = new Point();
            p.x = sum;
            p.y = 2*sum;
            return p;
        }
        return StructReturnValStatic(x-1,sum+1);
    }

    public int StructFacSumDeepStack(int x, int y, double a, double b, Point p, int z)
    {

        if(x == 0)
        {
            return p.x + p.y;
        }
        p.x++;
        p.y++;
        return this.StructFacSumDeepStack(x-1,y,a,b,p,1);
    }

    public static int StructFacSumDeepStackStatic(int x, int y, double a, double b, Point p, int z)
    {

        if(x == 0)
        {
            return p.x + p.y;
        }
        p.x++;
        p.y++;
        return TailTests.StructFacSumDeepStackStatic(x-1,y,a,b,p,1);
    }

    public int StructFacSumDeep(int x, Point p)
    {

        if(x == 0)
        {
            return p.x + p.y;
        }
        p.x++;
        p.y++;
        return this.StructFacSumDeep(x-1,p);
    }

    public static int StructFacSumDeepStatic(int x, Point p)
    {

        if(x == 0)
        {
            return p.x + p.y;
        }
        p.x++;
        p.y++;
        return TailTests.StructFacSumDeepStatic(x-1,p);
    }

    public int fUneven(int a, int b, int c)
    {
        return gUneven(a+b,b+c);
    }
    public int gUneven(int a, int b)
    {
        return hUneven(a+b);
    }
    public int hUneven(int a)
    {
        return a*2;
    }

    public double DoubleFacRec(int x, double prod)
    {
        if(x == 0) return prod;
        return DoubleFacRec(x-1, prod*((double)x));
    }

    public int StructFacSum(int x, Point p)
    {

        if(x == 0)
        {
            return p.x + p.y;
        }
        int t = p.x * p.y;
        p.x = p.y;
        p.y = t;
        return this.StructFacSum(x-1,p);
    }

    public int Sum(int total, int x)
    {
        if(x == 0) 
        {
            return total;
        }
        return Sum(total+1,x-1);
    }

    public static int SumStatic(int total, int x)
    {
        if(x == 0) 
        {
            return total;
        }
        return SumStatic(total+1,x-1);
    }

    public int ManyArgs(int x, 
                        int c0,
                        int c1,
                        int c2,
                        int c3,
                        int c4,
                        int c5,
                        int c6,
                        int c7,
                        int c8, 
                        int c9,
                        int sum)
    {
        if(x == 0)
        {
            return sum;
        }
        return ManyArgs(x-1,c1,c2,c3,c4,c5,c6,c7,c8,c9,c0,sum+1);
    }

    public static int ManyArgsStatic(int x, 
                                     int c0,
                                     int c1,
                                     int c2,
                                     int c3,
                                     int c4,
                                     int c5,
                                     int c6,
                                     int c7,
                                     int c8, 
                                     int c9,
                                     int sum)
    {
        if(x == 0)
        {
            return sum;
        }
        return ManyArgsStatic(x-1,c1,c2,c3,c4,c5,c6,c7,c8,c9,c0,sum+1);
    }
}

