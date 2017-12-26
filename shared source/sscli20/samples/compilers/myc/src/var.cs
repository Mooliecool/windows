//------------------------------------------------------------------------------
// <copyright file="var.cs" company="Microsoft">
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
using System.Collections;
using System.Reflection;

public class Var
  {
  private int type;
  private String name;
  private VarList methodparams;	/* possible param varlist */
  private int vclass;
  private int sign;
  private int index;
  private Object fieldbuilder;
  private Object methodbuilder;
  private Object localtoken;

  public int getTypeId() { return type; }
  public void setTypeId(int t) { type = t; }
  public String getName() { return name; }
  public void setName(String s) { name = s; }
  public VarList getParams() { return methodparams; }
  public void setParams(VarList p) { methodparams = p; }
  public int getClassId() { return vclass; }
  public void setClassId(int v) { vclass = v; }
  public int getSign() { return sign; }
  public void setSign(int i) { sign = i; }
  public int getIndex() { return index; }
  public void setIndex(int i) { index = i; }

  public Object getFieldBuilder() { return fieldbuilder; }
  public void setFieldBuilder(Object f) { fieldbuilder = f; }
  public Object getMethodBuilder() { return methodbuilder; }
  public void setMethodBuilder(Object f) { methodbuilder = f; }
  public Object getLocalToken() { return localtoken; }
  public void setLocalToken(Object f) { localtoken = f; }

  public override string ToString()
    {
    StringBuilder sb = new StringBuilder(name);
    sb.Append("(Id=");
    sb.Append(vclass);
    sb.Append(",Sign=");
    sb.Append(sign);
    sb.Append(")");
    return sb.ToString();
    }
  
  }

}
