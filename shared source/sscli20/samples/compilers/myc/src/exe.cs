//------------------------------------------------------------------------------
// <copyright file="exe.cs" company="Microsoft">
//     
//      Copyright (c) 2006 Microsoft Corporation.  All rights reserved.
//     
//      The use and distribution terms for this software are contained in the file
//      named license.txt, which can be found in the root of this distribution.
//      By using this software in any fashion, you are agreeing to be bound by the
//      terms of this license.
//     
//      You must not remove this notice, or any other, from this software.
//     
// </copyright>                                                                
//------------------------------------------------------------------------------

namespace MyC
{
using System;
using System.Reflection.Emit;
using System.Reflection;
using System.Collections;

public class Exe
{
AppDomain appdomain;		// assembly domain
AssemblyName appname;		// assembly name
AssemblyBuilder appbuild;	// assembly builder
ModuleBuilder emodule;		// module builder
TypeBuilder eclass;		// current class
MethodBuilder emethod;		// current method
String filename;
String classname;
Hashtable opcodehash;
ILGenerator il;
System.Diagnostics.SymbolStore.ISymbolDocumentWriter srcdoc;	// debug source file
bool localsdone = false;	// track if we are now emitting instructions

public void initOpcodeHash()
  {
  opcodehash = new Hashtable(32); // default initial size
  opcodehash["neg"] = OpCodes.Neg;
  opcodehash["mul"] = OpCodes.Mul;
  opcodehash["div"] = OpCodes.Div;
  opcodehash["add"] = OpCodes.Add;
  opcodehash["sub"] = OpCodes.Sub;
  opcodehash["not"] = OpCodes.Not;
  opcodehash["and"] = OpCodes.And;
  opcodehash["or"] = OpCodes.Or;
  opcodehash["xor"] = OpCodes.Xor;
  opcodehash["pop"] = OpCodes.Pop;

  opcodehash["br"] = OpCodes.Br;
  opcodehash["beq"] = OpCodes.Beq;
  opcodehash["bge"] = OpCodes.Bge;
  opcodehash["ble"] = OpCodes.Ble;
  opcodehash["blt"] = OpCodes.Blt;
  opcodehash["bgt"] = OpCodes.Bgt;
  opcodehash["brtrue"] = OpCodes.Brtrue;
  opcodehash["brfalse"] = OpCodes.Brfalse;

  opcodehash["cgt"] = OpCodes.Cgt;
  opcodehash["clt"] = OpCodes.Clt;
  opcodehash["ceq"] = OpCodes.Ceq;

  opcodehash["ldc.i4.-1"] = OpCodes.Ldc_I4_M1;
  opcodehash["ldc.i4.0"] = OpCodes.Ldc_I4_0;
  opcodehash["ldc.i4.1"] = OpCodes.Ldc_I4_1;
  opcodehash["ldc.i4.2"] = OpCodes.Ldc_I4_2;
  opcodehash["ldc.i4.3"] = OpCodes.Ldc_I4_3;
  opcodehash["ldc.i4.4"] = OpCodes.Ldc_I4_4;
  opcodehash["ldc.i4.5"] = OpCodes.Ldc_I4_5;
  opcodehash["ldc.i4.6"] = OpCodes.Ldc_I4_6;
  opcodehash["ldc.i4.7"] = OpCodes.Ldc_I4_7;
  opcodehash["ldc.i4.8"] = OpCodes.Ldc_I4_8;
  }

public Exe(String name)
  {
  filename = name;
  initOpcodeHash();
  }

AssemblyName getAssemblyName(string s)
  {
  AssemblyName a = new AssemblyName();
  a.Name = filename+"_assembly";
  return a;
  }

public void BeginModule(string ifile)
  {
  appdomain = System.Threading.Thread.GetDomain();
  appname = getAssemblyName(filename);
  appbuild = appdomain.DefineDynamicAssembly(appname,
					     AssemblyBuilderAccess.Save,
					     Io.genpath);
  emodule = appbuild.DefineDynamicModule(
  		filename+"_module",
  		Io.GetOutputFilename(),
		Io.gendebug);
  Guid g = System.Guid.Empty;
  if (Io.gendebug)
    srcdoc = emodule.DefineDocument(ifile, g, g, g);

  }

public void EndModule()
  {
  try
    {
    String s = Io.GetOutputFilename();
    appbuild.Save(s);
    Console.WriteLine("Saving assembly as "+s);
    }
  catch (Exception e)
    {
    Io.ICE(e.ToString());
    }
  }

public void BeginClass(String name, TypeAttributes access)
  {
  classname = name;
  eclass = emodule.DefineType(name, access);
  }

public void EndClass()
  {
  eclass.CreateType();		// create the class
  }

/*
 * determine the IL static type
 */
Type ilSType(bool sign, int type)
  {
  if (sign)
    {
    switch (type)
      {
      case Tok.T_CHAR:  	return Type.GetType("System.SByte");
      case Tok.T_SHORT: 	return Type.GetType("System.Int16");
      case Tok.T_DEFTYPE:	return Type.GetType("System.Int32");
      case Tok.T_INT:		return Type.GetType("System.Int32");
      case Tok.T_LONG:		return Type.GetType("System.Int32");
      case Tok.T_FLOAT:		return Type.GetType("System.Single");
      case Tok.T_DOUBLE:	return Type.GetType("System.Double");
      case Tok.T_VOID:		return null;
      default:
	Io.ICE("Unhandled type " + type);
	return null;
      }
    }
  else
    {
    switch (type)
      {
      case Tok.T_CHAR:  	return Type.GetType("U1");
      case Tok.T_SHORT: 	return Type.GetType("U2");
      case Tok.T_DEFTYPE:	return Type.GetType("U4");
      case Tok.T_INT:		return Type.GetType("U4");
      case Tok.T_LONG:		return Type.GetType("U4");
      default:
	Io.ICE("Unhandled type " + type);
	return null;
      }
    }
  }

/*
 * common routine to construct a signature string for a given varlist item
 * requires a destination ptr, will return the updated dest ptr
 */
private Type genDataTypeSig(Var e)
  {
  bool sign = true;
  if (e == null)
    return null;

  if (e.getSign() == Tok.T_UNSIGNED)	/* if var is unsigned, put it in sig */
    sign = false;

  Type sig = ilSType(sign, e.getTypeId());	/* get the datatype */
  return (sig);
  }

void genLoad(Var e)
  {
  int id = e.getClassId();
  if (e == null)
    Io.ICE("Load instruction with no variable ptr");
  if (e.getLocalToken() != null)
    {
    //    LocalToken lt = (LocalToken) e.getLocalToken();
    LocalBuilder lt = (LocalBuilder) e.getLocalToken();
    il.Emit(OpCodes.Ldloc, lt);
    }
  else if (e.getFieldBuilder() != null)
    {
    FieldBuilder fb = (FieldBuilder) e.getFieldBuilder();
    if (id == Tok.T_STATIC)
      il.Emit(OpCodes.Ldsfld, fb);
    else
      il.Emit(OpCodes.Ldfld, fb);
    }
  else
    {
    int index = e.getIndex();
    if (id == Tok.T_PARAM)
      {
      if (index <= 256)
	il.Emit(OpCodes.Ldarg_S, index);
      else
	il.Emit(OpCodes.Ldarg, index);
      }
    else if (id == Tok.T_AUTO || id == Tok.T_DEFCLASS)
      {
      if (index <= 256)
	il.Emit(OpCodes.Ldloc_S, e.getIndex());
      else
	il.Emit(OpCodes.Ldloc, e.getIndex());
      }
    else
      Io.ICE("Instruction load of unknown class ("
				     + e.getClassId()+")");
    }
  }

public void Load(IAsm a)
  {
  Var e = a.getVar();
  genLoad(e);
  }

public void Store(IAsm a)
  {
  Var e = a.getVar();
  int id = e.getClassId();
  if (e == null)
    Io.ICE("Store instruction with no variable ptr");
  if (e.getLocalToken() != null)
    {
    //    LocalToken lt = (LocalToken) e.getLocalToken();
    LocalBuilder lt = (LocalBuilder) e.getLocalToken();
    il.Emit(OpCodes.Stloc, lt);
    }
  else if (e.getFieldBuilder() != null)
    {
    FieldBuilder fb = (FieldBuilder) e.getFieldBuilder();
    if (id == Tok.T_STATIC)
      il.Emit(OpCodes.Stsfld, fb);
    else
      il.Emit(OpCodes.Stfld, fb);
    }
  else
    {
    int index = e.getIndex();
    if (id == Tok.T_PARAM)
      {
      if (index <= 256)
	il.Emit(OpCodes.Starg_S, index);
      else
	il.Emit(OpCodes.Starg, index);
      }
    else if (id == Tok.T_AUTO || id == Tok.T_DEFCLASS)
      il.Emit(OpCodes.Stloc, index);
    else
      Io.ICE("Instruction load of unknown class ("
				     + e.getClassId()+")");
    }
  }

public void FuncBegin(IAsm a)
  {
  Var func = a.getVar();
  Type funcsig = genDataTypeSig(a.getVar()); /* gen return type info */

  VarList paramlist = func.getParams(); /* get any params */
  Type[] paramTypes = null;	// in case no params
  if (paramlist.Length() > 0)
    {
    int max = paramlist.Length();
    paramTypes = new Type[max];
    for (int i = 0; i < max; i++)
      {
      Var e = paramlist.FindByIndex(i);
      paramTypes[i] = genDataTypeSig(e);
      }
    }

  emethod = eclass.DefineMethod(func.getName(),
			MethodAttributes.Static|MethodAttributes.Public,
			funcsig, paramTypes);
  func.setMethodBuilder(emethod); // save the method ref

  /*
   * set the argument symbol info
   */
  for (int i = 0; i < paramlist.Length(); i++)
    emethod.DefineParameter(i+1, 0, paramlist.FindByIndex(i).getName());

  il = emethod.GetILGenerator(); // create new il generator

  if (func.getName().Equals("main")) /* special entry point for main */
    appbuild.SetEntryPoint(emethod);
    //    emodule.SetUserEntryPoint(emethod);

  /*
   * must also re-init the label hashtable for each function
   */
  labelhash = new Hashtable();

  localsdone = false;
  }

public void Call(IAsm a)
  {
  Var func = a.getVar();
  Object o = func.getMethodBuilder(); // get previous declared reference
  if (o == null)
    Io.ICE("No previous extern for (" + func.getName() + ")");
  MethodBuilder mb = (MethodBuilder) o;
//  il.Emit(OpCodes.Ldc_I4_0);	// push 0 for the "this" ptr
//  VarList x = func.getParams(); /* get any params */
//  if (x.Length() > 0)
//    {
//    int max = x.Length();
//    for (int i = 0; i < max; i++)
//      {
//      Var e = x.FindByIndex(i);
//      genLoad(e);
//      }
//    }
  il.Emit(OpCodes.Call, mb);	// call the MethodBuilder
  }

public void Insn(IAsm a)
  {
  Object o = opcodehash[a.getInsn()];
  if (o == null)
    Io.ICE("Instruction opcode (" + a.getInsn() + ") not found in hash");
  il.Emit((OpCode) o);
  }

private Hashtable labelhash;	/* labelname hashtable */ 
/*
 * get and/or create IL label
 * put it in hash for reuse
 */
private Object getILLabel(IAsm a)
  {
  String s = a.getLabel();
  Object l = labelhash[s];
  if (l == null)
    {
    l = (Object) il.DefineLabel();
    labelhash[s] = l;
    }
  return l;
  }

public void Label(IAsm a)
  {
  il.MarkLabel((Label) getILLabel(a));
  }

public void Branch(IAsm a)
  {
  Object o = opcodehash[a.getInsn()];
  if (o == null)
    Io.ICE("Instruction branch opcode (" + a.getInsn() + ") not found in hash");
  il.Emit((OpCode) o, (Label) getILLabel(a));
  }

public void Ret(IAsm a)
  {
  il.Emit(OpCodes.Ret);
  }

public void FuncEnd()
  {
  /*
   * fill in the current IL stream for this method
   */
  //  emethod.CreateMethodBody(il);
  il = null;
  localsdone = false;
  }

public void LocalVars(VarList v)
  {
  int max = v.Length();

  for (int i = 0; i < max; i++)	// loop thru the local params
    {
    Var e = v.FindByIndex(i);	// indexed by number
    Type et = genDataTypeSig(e);
    //    LocalToken t = emethod.DeclareLocal(et);
    LocalBuilder t = il.DeclareLocal(et);
    if (Io.gendebug)
      t.SetLocalSymInfo(e.getName());
    e.setLocalToken(t);
    }
  localsdone = true;
  }

public void FieldDef(IAsm a)
  {
  Var e = a.getVar();			/* get the field var ptr */
  FieldAttributes attr = FieldAttributes.Private; /* default attributes is private */

  if (e.getClassId() == Tok.T_STATIC)
    attr |= FieldAttributes.Static;

  Type t = genDataTypeSig(e);		/* gen type info */

  FieldBuilder f = eclass.DefineField(e.getName(), t, attr); // returns token
  e.setFieldBuilder((Object) f);			// store token for later usage
  }

public void LoadConst(IAsm a)
  {
  int value = Convert.ToInt32(a.getInsn());

  if (value > 127 || value < -128) /* if must use long form */
    {
    il.Emit(OpCodes.Ldc_I4, value);
    }
  else if (value > 8 || value < -1)	/* if must use medium form */
    {
    il.Emit(OpCodes.Ldc_I4_S, value);
    }
  else if (value == -1)
    {
    il.Emit(OpCodes.Ldc_I4_M1);
    }
  else				/* else use short form */
    {
    Object o = opcodehash["ldc.i4."+a.getInsn()];
    if (o == null)
      Io.ICE("Could not find opcode for (Ldc_I4_" + a.getInsn() + ")");
    il.Emit((OpCode) o);
    }
  }

public void Comment(IAsm a)
  {
  if (Io.gendebug)
    {
    if (localsdone)
      {
      if (a != null && il != null)
	{
	if (!IsNextNonInsnGen(a))
	  {

	  //	  Console.WriteLine("Line ("+a.getCommentLine().ToString()+")="+a.getComment());
	  int l = a.getCommentLine();
	  il.MarkSequencePoint(srcdoc, l, 0, l, 0);
	  }
	}
      }
    }
  }

/*
 * is the next assembler directive an non-instruction generation
 *
 * this is used to keep marksequencepoint from getting multiple calls
 * with no intervening instructions, which causes it to work incorrectly
 */
private bool IsNextNonInsnGen(IAsm a)
  {
  IAsm cur = a.getNext();
  if (cur == null)
    return true;
  int type = cur.getIType();

  /*
   * skip intervening labels
   */
  while (type == IAsm.I_LABEL)
    {
    cur = cur.getNext();
    type = cur.getIType();
    }

  /*
   * if next is comment then return true
   */
  if (type == IAsm.I_COMMENT)
    return true;
  return false;
  }

}

}
