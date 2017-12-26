//------------------------------------------------------------------------------
// <copyright file="emit.cs" company="Microsoft">
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
/*
 * the emit routines add "instruction" to the instruction stream (istream)
 * currently the istream is flushed when the emitAsm routine is called
 */
using System;
using System.Reflection;

class Emit
{
IAsm iroot;			// root of current instruction list
IAsm icur;			// current insn ptr
IAsm LastComment;		// ptr to last comment placeholder

VarList localvars;		// local copy of current localvar list

Io io;				// local copy of current IO handle

Exe exe;			// instance of exe class for generation

/*
 * get the function definition entry
 */
public Var GetFunc()
  {
  IAsm a;
  if (iroot == null)
    return null;
  a = iroot;
  while (a != null)
    {
    if (a.getIType() == IAsm.I_FUNC_BEGIN)
      return (a.getVar());
    a = a.getNext();
    }
  return null;			/* didn't find anything */
  }

void NextInsn(int incr)
  {
  int ncount = 0;
  if (iroot == null)
    {
    icur = iroot = new IAsm();
    }
  else
    {
    ncount = icur.getICount() + incr;	/* propogate previous count */
    icur.setNext(new IAsm());
    icur = icur.getNext();
    }
  icur.setICount(ncount);
  }

/*
 * Emit a field def to ilist
 */
public void FieldDef(Var e)
  {
  NextInsn(0);
  icur.setIType(IAsm.I_FIELD);
  icur.setVar(e);
  }

/*
 * Emit function begin to ilist
 */
public void FuncBegin(Var e)
  {
  NextInsn(0);
  icur.setIType(IAsm.I_FUNC_BEGIN);
  icur.setVar(e);
  }

/*
 * Emit the local declarations
 */
public void LocalVars(VarList v)
  {
  NextInsn(0);
  localvars = v;
  icur.setIType(IAsm.I_LOCALDEF);
  }

/*
 * Emit instruction to ilist
 */
public void Insn(String s)
  {
  NextInsn(1);
  icur.setIType(IAsm.I_INSN);
  icur.setInsn(s);
  }

public void Label(String lname)
  {				// this is the branch target
  NextInsn(0);
  icur.setIType(IAsm.I_LABEL);
  icur.setLabel(lname);
  }

public void Branch(String s, String lname)
  {				// this is the branch source
  NextInsn(1);
  icur.setIType(IAsm.I_BRANCH);
  icur.setInsn(s);
  icur.setLabel(lname);
  }

public void Store(Var e)
  {
  NextInsn(1);
  icur.setIType(IAsm.I_INSN_STORE);
  icur.setVar(e);
  }

public void Load(Var e)
  {
  NextInsn(1);
  icur.setIType(IAsm.I_INSN_LOAD);
  icur.setVar(e);
  }

public void LoadConst(String s)
  {
  NextInsn(1);
  icur.setIType(IAsm.I_INSN_LOAD_CONST);
  icur.setInsn(s);
  }

public void Call(Var e)
  {
  NextInsn(1);
  icur.setIType(IAsm.I_CALL);
  icur.setVar(e);			/* this is the callname */
  }

public void Finish()
  {
  iroot = icur = null;
  }

public void CommentHolder()
  {
  NextInsn(0);
  icur.setIType(IAsm.I_COMMENT);
  LastComment = icur;		/* save away this insn loc to store comment */
  icur.setCommentLine(io.commentGetCurrentLine());
  }

public void CommentFill(String comment)
  {
#if DEBUG
  Console.Write("CommentFill S=");
  for (int _debug_i=0; _debug_i<comment.Length;_debug_i++)
    {
    int _debug_d = comment[_debug_i];
    char _debug_c = (char) (_debug_d + 96);
    if (_debug_d < 32)
      Console.Write("^"+_debug_c);
    else
      Console.Write(comment[_debug_i]);
    Console.Write("[");
    Console.Write(_debug_d);
    Console.Write("],");
    }
  Console.WriteLine(";");
#endif
  if (LastComment != null)
    LastComment.setComment(comment);
  }

public void Ret()
  {
  NextInsn(1);
  icur.setIType(IAsm.I_RET);
  }

public void FuncEnd()
  {
  NextInsn(0);
  icur.setIType(IAsm.I_FUNC_END);
  }

/*
 * Emit exe instructions now
 * this flushes the istream
 */
public void IL()
  {
  IAsm a = iroot;
  IAsm p;

  while (a != null)
    {
    switch (a.getIType())
      {
      case IAsm.I_INSN:
	exe.Insn(a);
	break;
      case IAsm.I_LABEL:
	exe.Label(a);
	break;
      case IAsm.I_BRANCH:
	exe.Branch(a);
	break;
      case IAsm.I_INSN_STORE:
	exe.Store(a);
	break;
      case IAsm.I_INSN_LOAD:
	exe.Load(a);
	break;
      case IAsm.I_INSN_LOAD_CONST:
	exe.LoadConst(a);
	break;
      case IAsm.I_FUNC_BEGIN:
	exe.FuncBegin(a); /* Emit function beginning */
	break;
      case IAsm.I_FUNC_END:
	exe.FuncEnd();
	break;
      case IAsm.I_CALL:
	exe.Call(a);
	break;
      case IAsm.I_RET:
	exe.Ret(a);
	break;
      case IAsm.I_FIELD:
	exe.FieldDef(a);
	break;
      case IAsm.I_LOCALDEF:
	exe.LocalVars(localvars);
	break;
      case IAsm.I_COMMENT:
	exe.Comment(a);
	break;
      default:
	io.Abort("Unhandled instruction type " + a.getIType());
	break;
      }
    p = a;
    a = a.getNext();
    }
  }

/*
 * Emit assembly instructions now
 * this flushes the istream
 */
public void LIST()
  {
  IAsm a = iroot;
  IAsm p;
  Asm x = new Asm(io);

  while (a != null)
    {
    switch (a.getIType())
      {
      case IAsm.I_INSN:
	x.Insn(a);
	break;
      case IAsm.I_LABEL:
	x.Label(a);
	break;
      case IAsm.I_BRANCH:
	x.Branch(a);
	break;
      case IAsm.I_INSN_STORE:
	x.Store(a);
	break;
      case IAsm.I_INSN_LOAD:
	x.Load(a);
	break;
      case IAsm.I_INSN_LOAD_CONST:
	x.LoadConst(a);
	break;
      case IAsm.I_FUNC_BEGIN:
	x.FuncBegin(a); /* Emit function beginning */
	break;
      case IAsm.I_FUNC_END:
	x.FuncEnd();
	break;
      case IAsm.I_CALL:
	x.Call(a);
	break;
      case IAsm.I_RET:
	x.Ret(a);
	break;
      case IAsm.I_COMMENT:
	x.Comment(a);
	break;
      case IAsm.I_FIELD:
	x.FieldDef(a);
	break;
      case IAsm.I_LOCALDEF:
	x.LocalVars(localvars);
	break;
      default:
	io.Abort("Unhandled instruction type " + a.getIType());
	break;
      }
    p = a;
    a = a.getNext();
    }
  }

public void BeginModule()
  {
  exe.BeginModule(io.GetInputFilename());
  }

public void BeginClass()
  {
  exe.BeginClass(Io.GetClassname(), TypeAttributes.Public);
  if (Io.genlist)
    io.Out(".class " + Io.GetClassname() + "{\r\n");
  }

public void EndClass()
  {
  exe.EndClass();
  if (Io.genlist)
    io.Out("}\r\n");
  }

public void EndModule()
  {
  exe.EndModule();
  }

public Emit(Io o)
  {
  io = o;
  exe = new Exe(Io.GetClassname());
  }

}
}
