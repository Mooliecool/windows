//------------------------------------------------------------------------------
// <copyright file="iasm.cs" company="Microsoft">
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
using System.Collections;
using System.Reflection;

public class IAsm
  {
  /*
   * instruction types
   */
  public const int I_INSN	= 101;
  public const int I_LABEL	= 102;
  public const int I_BRANCH 	= 103;
  public const int I_CALL	= 104;
  public const int I_RET 	= 105;
  public const int I_INSN_STORE	= 111;
  public const int I_INSN_LOAD	= 112;
  public const int I_INSN_LOAD_CONST = 113;
  public const int I_COMMENT	= 120;
  public const int I_FUNC_BEGIN	= 150;
  public const int I_FUNC_END	= 151;
  public const int I_FIELD	= 161;
  public const int I_LOCALDEF	= 162;

  private IAsm next;
  private int icount;
  private int itype;		/* type of instruction */
  private String insn;		/* instruction */
  private String label;		/* label name ref */
  private Var ivar;		/* variable pointer */
  private String comment;	/* comment buffer */
  private int linenumber;	/* line number */

  public void setNext(IAsm n) { next = n; }
  public IAsm getNext() { return next; }
  public void setICount(int i) { icount = i; }
  public int getICount() { return icount; }
  public void setIType(int i) { itype = i; }
  public int getIType() { return itype; }
  public void setInsn(String s) { insn = s; }
  public String getInsn() { return insn; }
  public void setLabel(String l) { label = l; }
  public String getLabel() { return label; }
  public void setVar(Var v) { ivar = v; }
  public Var getVar() { return ivar; }
  public void setComment(String c) { if (comment != null) Io.ICE("Comment overwrite");
					comment = c; }
  public String getComment() { return comment; }
  public void setCommentLine(int l) { linenumber = l; }
  public int getCommentLine() { return linenumber; }
  }


}
