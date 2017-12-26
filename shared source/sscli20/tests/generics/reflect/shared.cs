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

// Compile this sample hierarchy as a DLL to be inspected by reflect1

using System;

// A trivial generic class
public class List<T>
{
}

// The top of a hierarchy
public class P<T>
{
  // An instance field whose type involves the type parameter to the class
  public T instance_field;

  // A static field
  static public int static_field;

  // A constructor whose parameter types involve the type parameter to the class
  public P(T xv) { instance_field = xv; }

  // Two virtual methods involving the class parameter
  public virtual T virtual_method1(T y) { return instance_field; }
  public virtual T virtual_method2(T xv) { instance_field = xv; return xv; }

  // A non-virtual method involving the class parameter
  public T instance_method1(T y) { return y; }

  // A static method
  public static void static_method(int x) { Console.WriteLine(x); }

  // A property whose type involves the type parameter to the class
  public T prop { get { return instance_field; } }

  // A generic static method
 public static A fst<A,B>(A x, B y) { return x;}
 public static A[] singleton<A>(A x) { return new A[] { x }; }
}


// Polymorphic inheritance, one method only overridden
public class Q<T> : P<T[]>
{
  public Q(T[] xv) : base(xv) { }

  public override T[] virtual_method1(T[] y) { return null; }
}

// Polymorphic inheritance, all methods inherited
public class R<T> : Q< List<T> >
{
  public R(List<T>[] xv) : base(xv) { }
}

