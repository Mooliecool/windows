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

// Good test for dictionary-passing implementation of generic methods (sharing=on)
using System;


public enum Colour { Red, Green, Blue };

public class List<T> { }
public class C<S>
{
  // Already has two enregistered args
  public void meth2<T>(int i, int j) {
    Console.WriteLine("C<" + typeof(S) + ">::meth2<" + typeof(T) + ">(" + i + "," + j + ")");
    Console.WriteLine("typeof(List<T>) = " + typeof(List<T>));
  }

  // Already has two enregistered args
  public void meth1<T>(int i) {
    Console.WriteLine("C<" + typeof(S) + ">::meth1<" + typeof(T) + ">(" + i + ")");
    Console.WriteLine("typeof(List<T>) = " + typeof(List<T>));
  }

  // Has one enregisted arg
  public void meth0<T>() {
    Console.WriteLine("C<" + typeof(S) + ">::meth0<" + typeof(T) + ">()");
    Console.WriteLine("typeof(List<T>) = " + typeof(List<T>));
  }
}

  
public class M
{
  // Already has two enregistered args
  public static void meth2<T>(int i, int j) {
    Console.WriteLine("meth2<" + typeof(T) + ">(" + i + "," + j + ")");
    Console.WriteLine("typeof(List<T>) = " + typeof(List<T>));
  }

  // Already has one enregistered args
  public static void meth1<T>(int i) {
    Console.WriteLine("meth1<" + typeof(T) + ">(" + i + ")");
    Console.WriteLine("typeof(List<T>) = " + typeof(List<T>));
  }

  // Has no args
  public static void meth0<T>() {
    Console.WriteLine("meth0<" + typeof(T) + ">()");
    Console.WriteLine("typeof(List<T>) = " + typeof(List<T>));
  }

  public static void Main() {
    meth2<string>(1,2);
    meth2<Type>(3,4);
    meth2<int>(5,6);
    meth2<Colour>(9,10);
    meth1<string>(5);
    meth1<Type>(10);
    meth0<string>();
    meth0<Type>();

    C<string> cs = new C<string>();
    C<Type> ct = new C<Type>();
    cs.meth2<string>(1,2);
    cs.meth2<Type>(3,4);
    cs.meth2<int>(5,6);
    cs.meth2<Colour>(9,10);
    cs.meth1<string>(5);
    cs.meth1<object>(10);
    cs.meth0<string>();
    cs.meth0<object>();

    ct.meth2<string>(1,2);
    ct.meth2<Type>(3,4);
    ct.meth2<int>(5,6);
    ct.meth2<Colour>(9,10);
    ct.meth1<string>(5);
    ct.meth1<Type>(10);
    ct.meth0<string>();
    ct.meth0<Type>();

    Console.WriteLine("Finished!");
  }
}