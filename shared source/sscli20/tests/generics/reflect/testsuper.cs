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

// Test superclass and interface query methods in System.Type
// Test these on various instantiations of generic types in a hierarchy
// that includes inheritance and overriding
using System;
using System.Reflection;


// A trivial parameterized class
public class List<T>
{
}

// Some interfaces
public interface I<T> { void m1(); }
public interface J<T> { void m2(); }
public interface K<T> : I<T[]> { }

// The top of a hierarchy
public class P<T> : I<T>
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

  // A static method
  public static void static_method(int x) { Console.WriteLine(x); }

  // A property whose type involves the type parameter to the class
  public T prop { get { return instance_field; } }

  public void m1() { }  

  // A polymorphic method
// public A fst<A,B>(A x, B y) { return x;}
// public A[] singleton<A>(A x) { return new A[] { x }; }
}


// Polymorphic inheritance, one method only overridden
public class Q<T> : P<T[]>, J< List<T> >
{
  public Q(T[] xv) : base(xv) { }

  public void m2() { }

  public override T[] virtual_method1(T[] y) { return null; }
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
  // A property whose type involves the type parameter to the class
  public int prop { get { return x; } }

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
  public static void DumpAll(Type ty)
  {
    Console.WriteLine("\n===== Type is " + ty);
    Console.WriteLine("  BaseType: " + ty.BaseType);
    Console.WriteLine("  Interfaces: ");
    Type[] ints = ty.GetInterfaces();
    foreach (Type intty in ints)
    {
      Console.WriteLine("    " + intty);
      InterfaceMapping im = ty.GetInterfaceMap(intty);      
      Console.WriteLine("      IM.TargetType: " + im.TargetType);
      foreach (MethodInfo m in im.TargetMethods)
      { Console.WriteLine("        " + m + " in " + m.DeclaringType); }
      Console.WriteLine("      IM.InterfaceType: " + im.InterfaceType);
      foreach (MethodInfo m in im.InterfaceMethods)
      { Console.WriteLine("        " + m + " in " + m.DeclaringType); }
    }
  }

  public static void Main(string[] args)
  {
    Type[] def = { typeof(P<string>), typeof(P<C>), typeof(Q<string>), typeof(Q<C>), typeof(R<string>), typeof(R<C>) };
    if (args.Length >= 1)
    {
      foreach (string a in args)
      {        
        DumpAll(Type.GetType(a));
      }
    }
    else
    {
      foreach (Type t in def)
      {
        DumpAll(t);
      }
    }
  }
}