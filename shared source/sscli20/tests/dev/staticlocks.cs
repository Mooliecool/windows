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
using System.Threading;
using System.Runtime.CompilerServices;

class StaticLocks {

    public int counter;

    [MethodImplAttribute(MethodImplOptions.NoInlining|MethodImplOptions.Synchronized)]
    static void Bar(StaticLocks a)
    {
        a.counter++;
    }

    [MethodImplAttribute(MethodImplOptions.NoInlining|MethodImplOptions.Synchronized)]
    static void Foo(StaticLocks a)
    {
        Thread.Sleep(1);
    }

    void Contention()
    {    
       for (int i = 0; i < 100; i++)
       {
           Foo(this);
       }
    }

    void ForceContention()
    {
        Thread t = new Thread(new ThreadStart(Contention));
        t.Start();
        Contention();
    }

   public void RunTest()
   {
       int i;
       int start = Environment.TickCount;
 
       for (i = 0; i < 100000; i++)
       {
           Bar(this);
       }

       int end = Environment.TickCount;

       Console.WriteLine("Rate (locks/mssec): " + ((double)i / (double)(end - start)).ToString());
   }

   public static void Main() {
      
      StaticLocks a = new StaticLocks();

      a.RunTest();
      a.RunTest();
      a.RunTest();

      Console.WriteLine("Forcing contention");
      a.ForceContention();
      Console.WriteLine("Contention forced");

      a.RunTest();
      a.RunTest();
      a.RunTest();
   }
}
