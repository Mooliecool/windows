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

//----------------------------------------------------------------------//
// Test case generator           					//
//----------------------------------------------------------------------//

using System;
using System.IO;

class TestInstr
{

// Current brace level
static int level;

// Filename and file to which we are writing the IL
static string filename;
static StreamWriter outstream;

// Emit a single line with indentation appropriate to the brace level
static void Emit(string s)
{
  for (int i = 0; i < level; i++)
  {
    outstream.Write("  ");
  }
  outstream.WriteLine(s);
}

// Emit an open brace and adjust the level
static void EmitOpenBrace()
{
  Emit("{");
  level++;
}

// Emit an open brace preceded by a header
static void EmitOpenBrace(string header)
{
  Emit(header);
  EmitOpenBrace();
}

// Emit a close brace and adjust the level
static void EmitCloseBrace()
{
  level--;
  Emit("}");
}

// Emit standard stuff at the top
static void EmitHeader()
{
  EmitOpenBrace(".assembly 'test'");
  Emit(".hash algorithm 0x00008004");
  EmitCloseBrace();
  EmitOpenBrace(".class W<T>");
  Emit(".field !0 f");
  EmitOpenBrace(".method public instance void .ctor(!0 x)");
  Emit(".maxstack 2");
  Emit("ldarg.0");
  Emit("call instance void [mscorlib]System.Object::.ctor()");
  Emit("ldarg.0");
  Emit("ldarg x");
  Emit("stfld !0 class W<!0>::f");
  Emit("ret");
  EmitCloseBrace();
  EmitOpenBrace(".method public static void Main()");
  Emit(".entrypoint");
  Emit(".maxstack 10");
}

// Emit standard stuff at the bottom
static void EmitFooter()
{
  Emit("ret");
  EmitCloseBrace();
  EmitCloseBrace();
}

// Entry in the type table: descriptive type, IL type, instructions for some value of the type
public struct TypeMapEntry
{
  internal string ty;
  internal string ilty;
  internal bool isref;
  internal string instrs;
  internal TypeMapEntry(string ty,string ilty,bool isref,string instrs) 
  { this.ty = ty; this.ilty = ilty; this.instrs = instrs; this.isref = isref; }
};

static TypeMapEntry[] typemap = 
{
  new TypeMapEntry( "int8",   "int8",                false, "ldc.i4 -53"),
  new TypeMapEntry( "short",  "int16",               false, "ldc.i4 -2000"),
  new TypeMapEntry( "int",    "int32",               false, "ldc.i4 12345"),
  new TypeMapEntry( "long",   "int64",               false, "ldc.i8 1234567890"),

  new TypeMapEntry( "ubyte",   "unsigned int8",      false, "ldc.i4 200"),
  new TypeMapEntry( "ushort",  "unsigned int16",     false, "ldc.i4 40000"),
  new TypeMapEntry( "uint",    "unsigned int32",     false, "ldc.i4 12345"),
  new TypeMapEntry( "ulong",   "unsigned int64",     false, "ldc.i8 1234567890"),

  new TypeMapEntry( "char",    "char",               false, "ldc.i4 65"),
  new TypeMapEntry( "bool",    "bool",               false, "ldc.i4 1"),

  new TypeMapEntry( "float",  "float32",             false, "ldc.r4 3.14"),
  new TypeMapEntry( "double", "float64",             false, "ldc.r8 1.23456789"),

  new TypeMapEntry( "string", "class [mscorlib]System.String", true,  "ldstr \"My string\""),
  new TypeMapEntry( "object", "class [mscorlib]System.Object", true,  "newobj instance void [mscorlib]System.Object::.ctor()"),
  new TypeMapEntry( "date",   "value class [mscorlib]System.DateTime", false,
    "ldc.i4 1968 ldc.i4 10 ldc.i4 29 newobj instance void [mscorlib]System.DateTime::.ctor(int32,int32,int32)"),
  new TypeMapEntry( "intarray",  "int32[]",          true,  "ldc.i4 10 newarr int32"),
  new TypeMapEntry( "longarray", "int64[]",          true,  "ldc.i4 10 newarr int64"),
};

static TypeMapEntry poly =   
  new TypeMapEntry( "T",      "!0",                  true,  "ldarg.0 ldfld !0 class W<!0>::f");


static string EmitLocal(TypeMapEntry tme, string prefix)
{
  string varname = "'" + prefix + "_" + tme.ty + "'";
  Emit(".locals(" + tme.ilty + " " + varname + ")");
  return varname;
}

static int labelnum = 0;
static string GenLabel()
{
  labelnum++;
  
  return "L" + labelnum;
}

static string EmitLocal(TypeMapEntry tme)
{
  return EmitLocal(tme, "V");
}

static void EmitWriteLine()
{
  Emit("call void [mscorlib]System.Console::WriteLine(class [mscorlib]System.Object)");
}
  
static void EmitPrint(TypeMapEntry tme)
{
  Emit("box " + tme.ilty);
  EmitWriteLine();
}

static void EmitPrintString(string s)
{
  Emit("ldstr \"" + s + "\"");
  EmitWriteLine();
}

// Delegate representing an emitter parameterized on a type
public delegate void Emitter(TypeMapEntry tme);

// Entry in the test table: description of test and emitter delegate
public struct TestMapEntry
{
  internal string test;
  internal string description;
  internal Emitter emitter;
  internal TestMapEntry(string t, string d, Emitter e) { test = t; description = d; emitter = e; }
};

static TestMapEntry[] testmap =
{
  new TestMapEntry( "box",     "box", new Emitter(EmitBoxAny)),
  new TestMapEntry( "unbox",   "unbox.any", new Emitter(EmitUnboxAny)),
  new TestMapEntry( "unboxfail", "unbox.any with wrong type", new Emitter(EmitUnboxAnyFail)),
  new TestMapEntry( "ldobj",   "ldobj", new Emitter(EmitLdObj)),
  new TestMapEntry( "stobj",   "stobj", new Emitter(EmitStObj)),
  new TestMapEntry( "cpobj",   "cpobj", new Emitter(EmitCpObj)),
  new TestMapEntry( "initobj", "initobj", new Emitter(EmitInitObj)),
  new TestMapEntry( "ldelemany", "ldelem.any", new Emitter(EmitLdElemAny)),
  new TestMapEntry( "stelemany", "stelem.any", new Emitter(EmitStElemAny)),
  new TestMapEntry( "newobj", "newobj", new Emitter(EmitNewObj)),
};

class NotFound : Exception { };

static TypeMapEntry LookupType(string ty)
{
  foreach (TypeMapEntry tme in typemap)
  {
    if (ty == tme.ty) return tme;
  }
  throw new NotFound();
}

static TestMapEntry LookupTest(string test)
{
  foreach (TestMapEntry tme in testmap)
  {
    if (test == tme.test) return tme;
  }
  throw new NotFound();
}

static void EmitLdElemAny(TypeMapEntry tme)
{
  Emit("ldc.i4 10 newarr " + tme.ilty);  
  Emit("dup");
  Emit("ldc.i4 0");
  if (tme.isref)
  {  
    Emit(tme.instrs);
    Emit("stelem.ref");
  }
  else
  {
    Emit("ldelema " + tme.ilty);
    Emit(tme.instrs);
    Emit("stobj " + tme.ilty);
  }    

  Emit("ldc.i4 0");
  Emit("ldelem.any " + tme.ilty);
  EmitPrint(tme);
}

static void EmitStElemAny(TypeMapEntry tme)
{
  Emit("ldc.i4 10 newarr " + tme.ilty);  
  Emit("dup");
  Emit("ldc.i4 0");
  Emit(tme.instrs);
  Emit("stelem.any " + tme.ilty);
  
  Emit("ldc.i4 0");
  if (tme.isref)
  {  
    Emit("ldelem.ref");
  }
  else
  {
    Emit("ldelem.any " + tme.ilty);
    Emit("box " + tme.ilty);
  }    

  EmitWriteLine();
}

static void EmitBoxAny(TypeMapEntry tme)
{
  Emit(tme.instrs);
  Emit("box " + tme.ilty);
  EmitWriteLine();
}

static void EmitUnboxAny(TypeMapEntry tme)
{
  Emit(tme.instrs);
  Emit("box " + tme.ilty);
  Emit("unbox.any " + tme.ilty);
  EmitPrint(tme);
}

static void EmitNewObj(TypeMapEntry tme)
{
  Emit(tme.instrs);
  Emit("newobj instance void class W<" + tme.ilty + ">::.ctor(!0)");
  EmitWriteLine();
}

static void EmitUnboxAnyFail(TypeMapEntry tme)
{
  string l1 = GenLabel();
  string l2 = GenLabel();

  EmitOpenBrace(".try");
  Emit("newobj instance void [mscorlib]System.Object::.ctor()");
  Emit("unbox.any " + tme.ilty);
  Emit("pop");
  Emit("leave " + l1);
  EmitCloseBrace();
  EmitOpenBrace("catch [mscorlib]System.InvalidCastException");
  Emit("pop");
  EmitPrintString("downcast failed\\n");
  Emit("leave " + l2);
  EmitCloseBrace();
  Emit(l1 + ":");
  EmitPrintString("downcast succeeded\\n");
  Emit(l2 + ":");
}

static void EmitLdObj(TypeMapEntry tme)
{
  string varname = EmitLocal(tme);
  Emit(tme.instrs);
  Emit("stloc " + varname);
  Emit("ldloca " + varname);
  Emit("ldobj " + tme.ilty);
  EmitPrint(tme);
}
  
static void EmitStObj(TypeMapEntry tme)
{
  string varname = EmitLocal(tme);
  Emit("ldloca " + varname);
  Emit(tme.instrs);
  Emit("stobj " + tme.ilty);
  Emit("ldloc " + varname);
  EmitPrint(tme);
}

static void EmitCpObj(TypeMapEntry tme)
{
  string src = EmitLocal(tme, "S");
  string dest = EmitLocal(tme, "D");
  Emit(tme.instrs);
  Emit("stloc " + src);
  Emit("ldloca " + dest);
  Emit("ldloca " + src);
  Emit("cpobj " + tme.ilty);
  Emit("ldloc " + dest);
  EmitPrint(tme);
}

static void EmitInitObj(TypeMapEntry tme)
{
  string dest = EmitLocal(tme);
  Emit("ldloca " + dest);
  Emit("initobj " + tme.ilty);
  Emit("ldloc " + dest);
  EmitPrint(tme);
}
  
static void Usage()
{
  string alltypes = "*";
  string alltests = "*";
  foreach (TestMapEntry tme in testmap) { alltests += "|" + tme.test; }
  foreach (TypeMapEntry tme in typemap) { alltypes += "|" + tme.ty; }
  Console.WriteLine("Usage: testinstr [/i:" + alltests + "] [/t:" + alltypes + "] [/f:<filename>]");
}

static int Main(string[] args)
{
  string testname;
  string typename;
  TypeMapEntry[] types = typemap;
  TestMapEntry[] tests = testmap;

  filename = "test.il";

  foreach (string a in args)
  {
    int len = a.Length;
    if (len >= 2 && (a[0] == '/' || a[0] == '-')) 
    {
      switch (Char.ToUpper(a[1])) 
      {
	case 'I':
          testname = a.Substring(3);
          if (testname != "*")          
          {
            tests = new TestMapEntry[1];
            try { tests[0] = LookupTest(testname); } 
            catch (NotFound) { Console.WriteLine("No such test: " + testname); return 1; }
          }
          break;
        case 'T':
          typename = a.Substring(3);
          if (typename != "*")
          {
            types = new TypeMapEntry[1];
            try { types[0] = LookupType(typename); }
            catch (NotFound) { Console.WriteLine("No such type: " + typename); return 1; }
          }
          break;
        case 'F':
          filename = a.Substring(3);
          break;
        case 'X':
          System.AppDomain.CurrentDomain.ExecuteAssembly("ilasm");
          break;
        case '?':
          Usage();
          return 0;
      }
    }
  }

  FileStream fs = new FileStream(filename, FileMode.Create, FileAccess.Write, FileShare.None);
  outstream = new StreamWriter(fs);

  Console.Write("Generating IL to " + filename + "...");
  EmitHeader();

  foreach (TestMapEntry test in tests)  
  {
    EmitPrintString("********************************* " + test.description + " *************************\\n");
    foreach (TypeMapEntry type in types)
    {
      EmitPrintString("  on type " + type.ty + ": ");
      test.emitter(type);
    }
  }


  EmitFooter();
  outstream.Close();
  Console.Write("done.\n");
  return 0;
}
  
}
 