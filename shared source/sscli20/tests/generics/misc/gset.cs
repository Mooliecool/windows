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

// Set sample, illustrates all kinds of generic definitions:
//   structs (for pairs)
//   interfaces (to define a generic set interface)
//   classes (to implement the interface)
//   methods (a cross-product method)
//   invariant arrays (to implement the set)
//
// It's also a good test of run-time types (use of ToString & Equals methods, new T[], etc.)
// and code sharing (string & Type, int & Colour)
//
using System;

public struct Pair<A,B>
{
  public A fst; 
  public B snd;
  public Pair(A a, B b) 
  { 
    fst = a; snd = b; 
  }
  public override string ToString()
  {
    return "(" + fst.ToString() + "," + snd.ToString() + ")";
  }
  public override bool Equals(object obj)
  {
    Pair<A,B> p = (Pair<A,B>) obj;
    return fst.Equals(p.fst) && snd.Equals(p.snd);
  }
}

interface ISet<T>
{
  void Add(T x);
  void Remove(T x);
  bool Contains(T x);
}

class ArraySet<T> : ISet<T>
{
  public T[] items;
  public int size;
  public ArraySet() {
    items = new T[100]; size = 0;
  }
  public void Add(T x)
  {
    for (int i = 0; i < size; i++)
      if (x.Equals(items[i])) return;
    if (size >= items.Length) {
      T[] tmp = new T[size*2];
      Array.Copy(items,tmp,size);
      items = tmp;
    }
    items[size++] = x;
  }
  public void Remove(T x)
  {
    for (int i = 0; i < size; i++)
      if (x.Equals(items[i])) {
        size--;
        for (int j = i; j < size; j++)
          items[j] = items[j+1];
      }
  }
  public bool Contains(T x)
  {
    for (int i = 0; i < size; i++)
      if (x.Equals(items[i])) return true;
    return false;
  }

  public ArraySet<Pair<T,U> > Cross<U>(ArraySet<U> that)
  {
    ArraySet<Pair<T,U> > result = new ArraySet<Pair<T,U> >();
    int m = this.size;
    int n = that.size;
    for (int i = 0; i < m; i++)
      for (int j = 0; j < n; j++)
        result.Add(new Pair<T,U>(this.items[i], that.items[j]));
    return result;
  }

  public override string ToString()
  {
    string s = "{";
    for (int i = 0; i < size; i++)
    {
      if (i>0) s += ",";
      s += items[i].ToString();
    }
    s += "}";
    return s;
  }
}
  
public enum Colour { Red, Blue, Green };

class M
{
  public static void Main()
  {
    ArraySet<string> sset = new ArraySet<string>();
    ArraySet<Type> tset = new ArraySet<Type>();
    ArraySet<int> iset = new ArraySet<int>();
    ArraySet<Colour> cset = new ArraySet<Colour>();
  
    sset.Add("A"); sset.Add("B");
    iset.Add(1); iset.Add(2); iset.Add(3);
    cset.Add(Colour.Red); cset.Add(Colour.Blue); cset.Add(Colour.Green);
    tset.Add(typeof(string));
  
    Console.WriteLine("sset = " + sset);
    Console.WriteLine("iset = " + iset);
    Console.WriteLine("cset = " + cset);
    Console.WriteLine("tset = " + tset);

    Console.WriteLine("sset X iset = " + sset.Cross<int>(iset));
    Console.WriteLine("sset X tset = " + sset.Cross<Type>(tset));
    Console.WriteLine("sset X sset = " + sset.Cross<string>(sset));
    Console.WriteLine("sset X cset = " + sset.Cross<Colour>(cset));

    Console.WriteLine("tset X sset = " + tset.Cross<string>(sset));
    Console.WriteLine("tset X tset = " + tset.Cross<Type>(tset));
    Console.WriteLine("tset X iset = " + tset.Cross<int>(iset));
    Console.WriteLine("tset X cset = " + tset.Cross<Colour>(cset));

    Console.WriteLine("iset X sset = " + iset.Cross<string>(sset));
    Console.WriteLine("iset X tset = " + iset.Cross<Type>(tset));
    Console.WriteLine("iset X iset = " + iset.Cross<int>(iset));
    Console.WriteLine("iset X cset = " + iset.Cross<Colour>(cset));

    Console.WriteLine("cset X sset = " + cset.Cross<string>(sset));
    Console.WriteLine("cset X tset = " + cset.Cross<Type>(tset));
    Console.WriteLine("cset X iset = " + cset.Cross<int>(iset));
    Console.WriteLine("cset X cset = " + cset.Cross<Colour>(cset));
  }
}