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

// Mutual recursion through an instantiated superclass for a non-generic class
using System;

class List<T> { };

class C<T>
{
  public void mymeth()
  {
    Console.WriteLine("C<" + typeof(T) + ">.mymeth in object of type " + GetType());
    Console.WriteLine("List<T> = " + typeof(List<T>));
  }
}

class D : C<E>
{
}

class E : C<D>
{
}

class M
{
  public static void Main()
  {
    D d = new D();
    E e = new E();
    C<D> cd = new C<D>();
    C<E> ce = new C<E>();
    d.mymeth();
    e.mymeth();
    cd.mymeth();
    ce.mymeth();
  }
}
    