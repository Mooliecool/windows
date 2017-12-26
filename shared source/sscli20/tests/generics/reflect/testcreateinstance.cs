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

// Test the members of MethodInfo for instantiated types
using System;
using System.Reflection;


// A trivial parameterized class
public class List<T>
{
}

// The top of a hierarchy
public class P<T>
{
  public T x;
  public P(T xv) { Console.WriteLine("In P(T xv) for T=" + typeof(T)); x = xv; }
  public P() { Console.WriteLine("In P() for T=" + typeof(T)); }

  //disallowed now
  //public P(C c) { Console.WriteLine("In P(C c) for T=" + typeof(T)); }

  // Two methods involving the class parameter
  public virtual T g(T y) { return x; }
  public virtual T s(T xv) { x = xv; return xv; }
}


// Polymorphic inheritance, one method only overridden
public class Q<T> : P<T[]>
{
  public Q(T[] xv) : base(xv) { }
  public override T[] g(T[] y) { return null; }
}

// Polymorphic inheritance, all methods inherited
public class R<T> : Q< List<T> >
{
  public R(List<T>[] xv) : base(xv) { }
}

// Control: normal inheritance
public class C
{  
  public int x;
  public virtual int m() { return 5; }
  public C(int xv) { x = xv; }
  public C() { x = 0; }
}

public class D : C
{
  public override int m() { return 6; }
}

public class E : D
{
  public string y;
}

class M
{
  public static void Main()
  {
    Type ty = typeof(C);
    object o = Activator.CreateInstance(ty, new object[] { 53 });
    Console.WriteLine(o.GetType());
 
    o = Activator.CreateInstance(typeof(P<string>));
    Console.WriteLine(o.GetType());

    o = Activator.CreateInstance(typeof(P<string>), new object[] { "Hello, world" });
    Console.WriteLine(o.GetType());

  } }
