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

// Lots of run-time types

using System;

class Set<T> { }
class Map<S,T> { }

class M
{
  static public void gmeth1<T>()
  {
    Console.WriteLine(typeof(Set<T>));
    Console.WriteLine(typeof(Map<T,T>));
    Console.WriteLine(typeof(T));
    gmeth2< T,Set<T> >();
    gmeth2< Set<T>, T>();
  }

  static public void gmeth2<A,B>()
  {
    Console.WriteLine(typeof(Set<A>));
    Console.WriteLine(typeof(Set<B>));
    Console.WriteLine(typeof(Set< Map<A,B> >));
    Console.WriteLine(typeof(Map<A,B>));
    Console.WriteLine(typeof(Map<Set<A>,B>));
    Console.WriteLine(typeof(Map<A,Set<B> >));
    Console.WriteLine(typeof(Map<Set<A>,Set<B> >));
  }

  static public void Main()
  {
    gmeth1<string>();
    gmeth1<Type>();
  }
}
