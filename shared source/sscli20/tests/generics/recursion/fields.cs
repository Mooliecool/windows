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

// Use of instantiated generic structs inside other structures
// In particular, recursive reference to defining class in a struct field
using System;

struct Pair<A,B>
{
  public A fst;
  public B snd;
  public Pair(A a, B b) { fst = a; snd = b; }
}
  

struct Triple<A,B,C>
{
  public A fst;
  public Pair<B,C> snd;
  public Triple(A a, B b, C c) { fst = a; snd = new Pair<B,C>(b,c); }
}

class P
{
  public Triple<int,P,long> fld;
  public P(int x, P y, long z) { fld = new Triple<int,P,long>(x, y, z); }
}

class M
{
  public static void Main()
  {
    P p = new P(5, null, 12345678987654321);
    p.fld.snd.fst = p;    
    Console.WriteLine(p.fld.fst);
    Console.WriteLine(p.fld.snd.fst);
    Console.WriteLine(p.fld.snd.snd);
  }
}