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

// Test thread safety of type and method instantiation
// Usage: polyrec <nthreads> <ninsts> 
// where nthreads is the number of threads to create
// and niters it the number of type/method instantiations to create each thread
using System;
using System.Threading;

// Spice things up a bit with some mutual recursion between instantiations
class C<T>
{
}

class D<T> : C< E<T> >
{
}

class E<T> : C< D<T> >
{
}

public class P 
{ 
  public static int nthreads;
  public static int ninsts;
  public static object x;

  // By the magic of polymorphic recursion we get n instantiations of D
  // and n instantiations of genmeth
  public static void genmeth<S>(int n)
  {
    if (n==0) return;
    else
    {
      x = new D<S>();
      genmeth< D<S> >(n-1);
    }
  }

  public static void Start()
  {
    genmeth<string>(ninsts);
  }

  public static void Main(String[] args)
  {
    /*
    if (args.Length < 2)
    {
      Console.WriteLine("Usage: polyrec <nthreads> <ninsts>");
      return;
    }

    nthreads = Int32.Parse(args[0]);
    ninsts = Int32.Parse(args[1]);
    */
    nthreads = 5;
    ninsts = 10;

    for (int i = 0; i < nthreads; i++)
    {
      Thread t = new Thread(new ThreadStart(Start));
      t.Name = "Thread " + i;	
      t.Start();
    }
    Console.WriteLine("Finished!");
  }
}
