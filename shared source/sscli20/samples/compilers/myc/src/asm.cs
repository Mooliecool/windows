//------------------------------------------------------------------------------
// <copyright file="asm.cs" company="Microsoft">
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
using System.Text;

class Asm
  {
  private Io io;

  public Asm(Io ihandle)
    {
    io = ihandle;
    }
  
  /*
   * determine the IL static type
   */
  private String ilSType(int type)
    {
    switch (type)
      {
      case Tok.T_CHAR:	return "char";
      case Tok.T_SHORT:	return "int16";
      case Tok.T_DEFTYPE:
      case Tok.T_INT:
      case Tok.T_LONG:	return "int32";
      case Tok.T_FLOAT:	return "float";
      case Tok.T_DOUBLE:	return "double float";
      case Tok.T_VOID:	return "void";
      default:
	Console.WriteLine("?Unhandled type " + type);
	Environment.Exit(1);
	break;
      }
    return null;
    }

  /*
   * common routine to construct a signature string for a given varlist item
   * requires a destination ptr, will return the updated dest ptr
   */
  private String genDataTypeSig(Var e)
    {
    if (e == null)
      return null;

    StringBuilder sb = new StringBuilder(MyC.MAXSTR);

    if (e.getSign() == Tok.T_UNSIGNED)	/* if var is unsigned, put it in sig */
      sb.Append("unsigned ");

    sb.Append(ilSType(e.getTypeId()));	/* get the datatype */
    return (sb.ToString());
    }

  private String genFieldRef(Var e)
    {
    if (e == null)
      return null;

    StringBuilder sb = new StringBuilder(MyC.MAXSTR);

    if (e.getSign() == Tok.T_UNSIGNED)	/* if var is unsigned, put it in sig */
      sb.Append("unsigned ");

    sb.Append(ilSType(e.getTypeId()));	/* get the datatype */
    sb.Append(" ");
    sb.Append(Io.GetClassname());	/* get the current classname */
    sb.Append(".");
    sb.Append(e.getName());	/* copy the variable name */
    return (sb.ToString());
    }

  public void Load(IAsm a)
    {
    StringBuilder sb = new StringBuilder(MyC.MAXSTR);
    Var e = a.getVar();
    if (e == null)
      {
      Console.WriteLine("?Load instruction with no variable ptr");
      Environment.Exit(1);
      }
    switch (e.getClassId())
      {
      case Tok.T_STATIC:
	{
	sb.Append("\tldsfld ");
	sb.Append(genFieldRef(e));
	sb.Append("\t//");
	sb.Append(a.getICount());
	sb.Append(", ");
	sb.Append(e.getName());
	sb.Append("\r\n");
	break;
	}
      case Tok.T_AUTO:
      case Tok.T_DEFCLASS:
	sb.Append("\tldloc ");
	sb.Append(e.getIndex());
	sb.Append("\t//");
	sb.Append(a.getICount());
	sb.Append(", ");
	sb.Append(e.getName());
	sb.Append("\r\n");
	break;
      case Tok.T_PARAM:
	sb.Append("\tldarg ");
	sb.Append(e.getIndex());
	sb.Append("\t//");
	sb.Append(a.getICount());
	sb.Append(", ");
	sb.Append(e.getName());
	sb.Append("\r\n");
	break;
      default:
	Console.Write("?Instruction load of unknown class (");
	Console.Write(e.getClassId());
	Console.WriteLine(")");
	Environment.Exit(1);
	break;
      }
    io.Out(sb.ToString());
    }

  public void Store(IAsm a)
    {
    StringBuilder sb = new StringBuilder(MyC.MAXSTR);
    Var e = a.getVar();
    if (e == null)
      {
      Console.WriteLine("?Store instruction with no variable ptr");
      Environment.Exit(1);
      }
    switch (e.getClassId())
      {
      case Tok.T_STATIC:
	sb.Append("\tstsfld ");
	sb.Append(genFieldRef(e));
	sb.Append("\t//");
	sb.Append(a.getICount());
	sb.Append(", ");
	sb.Append(e.getName());
	sb.Append("\r\n");
	break;
      case Tok.T_AUTO:
      case Tok.T_DEFCLASS:
	sb.Append("\tstloc ");
	sb.Append(e.getIndex());
	sb.Append("\t//");
	sb.Append(a.getICount());
	sb.Append(", ");
	sb.Append(e.getName());
	sb.Append("\r\n");
	break;
      case Tok.T_PARAM:
	sb.Append("\tstarg ");
	sb.Append(e.getIndex());
	sb.Append("\t//");
	sb.Append(a.getICount());
	sb.Append(", ");
	sb.Append(e.getName());
	sb.Append("\r\n");
	break;
      default:
	Console.Write("?Instruction load of unknown class (");
	Console.Write(e.getClassId());
	Console.WriteLine(")");
	Environment.Exit(1);
	break;
      }
    io.Out(sb.ToString());
    }

  public void FuncBegin(IAsm a)
    {
    Var func = a.getVar();
    String funcsig = genDataTypeSig(a.getVar()); /* gen type info */

    VarList x = func.getParams(); /* get any params */
    String paramsig = "";
    if (x.Length() > 0)
      {
      int max = x.Length();
      StringBuilder t = new StringBuilder(MyC.MAXSTR);
      for (int i = 0; i < max; i++)
	{
	Var e = x.FindByIndex(i);
	t.Append(genDataTypeSig(e));
	if (i < max-1)
	  t.Append(",");
	}
      paramsig = t.ToString();
      }
    StringBuilder sb = new StringBuilder(MyC.MAXSTR);
    sb.Append("\t.method ");
    sb.Append(funcsig);
    sb.Append(" ");
    sb.Append(func.getName());
    sb.Append("(");
    sb.Append(paramsig);
    sb.Append("){\r\n");
    io.Out(sb.ToString());

    if (func.getName().Equals("main")) /* special entry point for main */
      io.Out("\t.entrypoint\r\n");
    }
  
  public void Call(IAsm a)
    {
    Var func = a.getVar();
    String funcsig = genDataTypeSig(a.getVar()); /* gen type info */

    VarList x = func.getParams(); /* get any params */
    String paramsig = "";
    if (x.Length() > 0)
      {
      int max = x.Length();
      StringBuilder t = new StringBuilder(MyC.MAXSTR);
      for (int i = 0; i < max; i++)
	{
	Var e = x.FindByIndex(i);
	t.Append(genDataTypeSig(e));
	if (i < max-1)
	  t.Append(",");
	}
      paramsig = t.ToString();
      }

    StringBuilder sb = new StringBuilder(MyC.MAXSTR);
    sb.Append("\tcall ");
    sb.Append(funcsig);
    sb.Append("(");
    sb.Append(paramsig);
    sb.Append(")\t//");
    sb.Append(a.getICount());
    sb.Append("\r\n");
    io.Out(sb.ToString());
    }

  public void Comment(IAsm a)
    {
    String sp = a.getComment();	/* source ptr */
    if (sp == null)
      return;			// empty comment
    sp.Trim();			// remove extra whitespace
    if (sp == null || sp.Length == 0) /* sanity check, is there a comment? */
      return;			/* no, then nothing to do */

#if DEBUG
    Console.Write("Comment SP=");
    for (int _debug_i=0; _debug_i<sp.Length;_debug_i++)
      {
      int _debug_d = sp[_debug_i];
      char _debug_c = (char) (_debug_d + 96);
      if (_debug_d < 32)
	Console.Write("^"+Char.ToString(_debug_c));
      else
	Console.Write(sp[_debug_i]);
      Console.Write("[");
      Console.Write(_debug_d);
      Console.Write("],");
      }
    Console.WriteLine(";");
#endif
    StringBuilder buf = new StringBuilder(MyC.MAXSTR); /* a buffer to work with */
    buf.Append("//");
    buf.Append(a.getCommentLine());
    buf.Append(": ");
    int i = 0;
    int p = 0;
    while ((i = sp.IndexOf('\n', i)) >= 0)
      {
      i++;			// move past the newline
      string ts = sp.Substring(p, i-p);
      String s = ts.Trim();
      if (s.Length > 0)
	buf.Append(s);		// copy the substr
      buf.Append("\r\n");		// add the line seperator
      if (i < sp.Length)
	buf.Append("//");	// insert the comment block
      p = i;
      }

    buf.Append(sp.Substring(p)); // append the remaining chars
    buf.Append("\r\n");

    io.Out(buf.ToString());	/* output the comment */
    }

  public void Insn(IAsm a)
    {
    StringBuilder sb = new StringBuilder(MyC.MAXSTR);
    sb.Append("\t");
    sb.Append(a.getInsn());
    sb.Append("\t//");
    sb.Append(a.getICount());
    sb.Append("\r\n");
    io.Out(sb.ToString());
    }

  public void Label(IAsm a)
    {
    StringBuilder sb = new StringBuilder(MyC.MAXSTR);
    sb.Append(a.getLabel());
    sb.Append(":\r\n");
    io.Out(sb.ToString());
    }

  public void Branch(IAsm a)
    {
    StringBuilder sb = new StringBuilder(MyC.MAXSTR);
    sb.Append("\t");
    sb.Append(a.getInsn());
    sb.Append(" ");
    sb.Append(a.getLabel());
    sb.Append("\t//");
    sb.Append(a.getICount());
    sb.Append("\r\n");
    io.Out(sb.ToString());
    }

  public void Ret(IAsm a)
    {
    StringBuilder sb = new StringBuilder(MyC.MAXSTR);
    sb.Append("\tret\t\t//");
    sb.Append(a.getICount());
    sb.Append("\r\n");
    io.Out(sb.ToString());
    }

  public void FuncEnd()
    {
    io.Out("\t}\r\n");
    }

  public void LocalVars(VarList v)
    {
    StringBuilder sb = new StringBuilder(MyC.MAXSTR);
    sb.Append("\t.locals (");
    int max = v.Length();

    for (int i = 0; i < max; i++)	// loop thru the local params
      {
      Var e = v.FindByIndex(i);	// indexed by number
      String stype = "";
      switch (e.getTypeId())
	{
	case Tok.T_CHAR:	stype = "char"; break;
	case Tok.T_SHORT:	stype = "int16"; break;
	case Tok.T_INT:
	case Tok.T_LONG:	stype = "int32"; break;
	case Tok.T_FLOAT:	stype = "float"; break;
	case Tok.T_DOUBLE:	stype = "double float"; break;
	default:
	  Console.WriteLine("?Could not find type for local\n");
	  Environment.Exit(1);
	  break;
	}
      sb.Append(stype);		// append it now
      if (i < max-1)
	sb.Append(",");		// if not last, seperate with comma
      }

    sb.Append(")\r\n");
    io.Out(sb.ToString());
    }

  public void FieldDef(IAsm a)
    {
    Var e = a.getVar();		/* get the field var ptr */
    String prefix = "";
    switch (e.getClassId())
      {
      case Tok.T_STATIC:
	prefix = "\t.field ";
	break;
      case Tok.T_AUTO:
      case Tok.T_DEFCLASS:
	prefix = "\t.field ";
	break;
      default:
	Console.WriteLine("?Unhandled field def type\n");
	Environment.Exit(1);
	break;
      }

    StringBuilder sb = new StringBuilder(MyC.MAXSTR);
    sb.Append(prefix);		/* copy the prefix */
    sb.Append(genDataTypeSig(e)); /* gen type info, rets updated dp */
    sb.Append(" ");
    sb.Append(e.getName());	/* copy the variable name */
    sb.Append("\r\n");
    io.Out(sb.ToString());
    }

  public void LoadConst(IAsm a)
    {
    StringBuilder sb = new StringBuilder(MyC.MAXSTR);
    int value = Convert.ToInt32(a.getInsn());

    sb.Append("\tldc.i4");
    if (value > 127 || value < -128) /* if must use long form */
      {
      sb.Append(" ");
      }
    else if (value > 8 || value < -1)	/* if must use medium form */
      {
      sb.Append(".s ");
      }
    else				/* else use short form */
      {
      sb.Append(".");
      }
    sb.Append(a.getInsn());
    sb.Append("\t//");
    sb.Append(a.getICount());
    sb.Append("\r\n");

    io.Out(sb.ToString());
    }
  }
}
