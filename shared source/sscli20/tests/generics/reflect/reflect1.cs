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

// Given an instantiated type list its fields, properties and methods
using System;
using System.Reflection;

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

public struct Pair<A,B>
{
  public A fst; 
  public B snd;
  public Pair(string s, int i, A a, B b, int j) 
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
  public static void DumpParameter(ParameterInfo pi)
  {
    Console.WriteLine("    Parameter " + pi + " [Name:" + pi.Name + ", ParameterType:" + pi.ParameterType + "]");
  }
  public static void DumpParameters(ParameterInfo[] ps)
  {
    bool first = true;
    foreach (ParameterInfo p in ps)
    {
      if (!first) Console.Write(",");
      first = false;
      Console.Write(p.ParameterType + " " + p.Name);
    }
  }
  public static void DumpMethods(Type ty)
  {
    foreach (MethodInfo m in ty.GetMethods())
    {
      Console.WriteLine("Method " + m);
      Console.Write("  " + m.ReturnType + " " + m.ReflectedType + "::" + m.Name + "(");
      DumpParameters(m.GetParameters());
      Console.WriteLine(") declared in " + m.DeclaringType);
    }
  }

  public static void DumpConstructors(Type ty)
  {
    foreach (ConstructorInfo c in ty.GetConstructors())
    {
      Console.WriteLine("Constructor " + c);
      Console.Write("  " + c.ReflectedType + "::" + c.Name + "(");
      DumpParameters(c.GetParameters());
      Console.WriteLine(") declared in " + c.DeclaringType);
    }
  }

  public static void DumpAll(Type ty)
  {
    Console.WriteLine("\n===== Type is " + ty.ToString());
    DumpFields(ty);
    DumpProperties(ty);
    DumpConstructors(ty);
    DumpMethods(ty);
  }

  public static void DumpFields(Type ty)
  {
    foreach (FieldInfo f in ty.GetFields())
    {
      Console.WriteLine("Field " + f);
      Console.WriteLine("  " + f.FieldType + " " + f.ReflectedType + "::" + f.Name + " declared in " + f.DeclaringType);
    }
  }

  public static void DumpProperties(Type ty)
  {
    foreach (PropertyInfo p in ty.GetProperties())
    {
      Console.WriteLine("Property " + p);
      Console.WriteLine("  " + p.PropertyType + " " + p.ReflectedType + "::" + p.Name + " declared in " + p.DeclaringType);
    }
  }

  public static void Main(string[] args)
  {
    /*
    if (args.Length == 0)
    {
      Console.WriteLine("Usage: reflect1 <type-name>");
      Console.WriteLine("Example: reflect1 \"Q[System.String],reflect1\"");
      Console.WriteLine("Exmaple: reflect1 \"ISet[System.Int32],reflect1\"");
    }
    else
    { 
      foreach (string a in args)
      {         
        Type t = Type.GetType(a);
	if (t == null)
          Console.WriteLine("No such type: " + a);
        else
          DumpAll(t);
      }
    }
    */
      string[] arguments = {"ISet`1[System.Int32]", "ArraySet`1[System.String]"};
      foreach (string a in arguments)
      {         
          Type t = Type.GetType(a);
          if (t == null)
              Console.WriteLine("No such type: " + a);
          else
              DumpAll(t);
      }
  }
}
