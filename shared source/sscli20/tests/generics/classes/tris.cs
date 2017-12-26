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
// Tris benchmark adapted from Viroli & Natali OOPSLA paper
// Good test for a large number of class instantiations
// Usage: tris <num>
// Typical num=10000
using System;

interface ITris {
  ITris GetRotated();
  ITris GetSwapped();
  ITris GetCopied();
  ITris GetCloned();
}

class Tris<A,B,C,D> : ITris
{
  A a; B b; C c; D d;
  public Tris (A a, B b, C c, D d) { this.a = a; this.b = b; this.c = c; this.d = d; }
  public ITris GetRotated() { return new Tris<D,A,B,C>(d,a,b,c); }
  public ITris GetSwapped() { return new Tris<B,A,C,D>(b,a,c,d); }
  public ITris GetCopied() { return new Tris<A,A,C,D>(a,a,c,d); }
  public ITris GetCloned() { return new Tris<A,B,C,D>(a,b,c,d); }
  public void Fill(object[] o) {
    ITris t = this; double r; Random rand = new Random(12345);
    for (int i = 0; i < o.Length; i++) {
      o[i] = t; r = rand.NextDouble();
      if (r<0.1) t = GetCloned(); 
      else if (r<0.4) t = t.GetRotated();
      else if (r<0.7) t = t.GetSwapped();
      else t = t.GetCopied();
    }
  }
  public int CountSimilars(object[] o) {
    int count=0;
    for (int i = 0; i < o.Length; i++) 
      if (o[i] is Tris<A,B,C,D>) count++;
    return count;
  }
}

public class M 
{
  static public void Main(string[] args)
  {
//  int c1 = System.Environment.TickCount;
    /*
    if (args.Length == 0)
    {   
      Console.WriteLine("Usage: tris <size>");
      return;
    }
    int n = Int32.Parse(args[0]);
    */
    int n = 10000;
    Tris<Type,string,object,Random> t = new Tris<Type,string,object,Random>(typeof(int),"1",new object(),new Random());
    object[] o = new object[n];
    t.Fill(o);
    Console.WriteLine(t.CountSimilars(o));
//  int c2 = System.Environment.TickCount;
//  System.Console.WriteLine("time = " + (c2-c1));
  }
}
    
