//------------------------------------------------------------------------------
// <copyright file="exp.cs" company="Microsoft">
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
using System;
using System.Collections;
using LexToken;


namespace Absyn {
    
    enum Operator {
	ADD = 2,
	SUB = 3,
	MUL = 4,
	DIVIDE = 5,
	LT = 6,
	GT = 7,
	EQ = 8,
	LE = 9,
	GE = 10
    }

    class ExpList
    {
	public Exp Head;
	public ExpList Tail;
    
	public override String ToString()
	{
	    return (Head.ToString() + ((Tail != null) ? Tail.ToString() : null));
	}
    
    }

    abstract class Exp 
    {
	public Type ExpType;
	
	public abstract void Visit(IExpVisitor v);
	
    }

        

    abstract class NumericExp : Exp
    {
    }

    abstract class BoolExp : Exp
    {
    }

    abstract class ListExp : Exp
    {
    }
    
    class BinopExp : NumericExp
    {
	public Exp Left, Right;
	public Operator Oper;
    
	public BinopExp(Exp l, Token op, Exp r)
	{
	    Left = l;
	    Right = r;
	    ExpType = typeof(int);
	    
	    switch(op.Type){
	    case TokenType.PLUS:
		Oper = Operator.ADD;
		break;
	    case TokenType.MINUS:
		Oper = Operator.SUB;
		break;
	    case TokenType.MUL:
		Oper = Operator.MUL;
		break;
	    case TokenType.DIVIDE:
		Oper = Operator.DIVIDE;
		break;
	    }
	}

	public override String ToString()
	{
	    return ("BinopExp(\n" + Left.ToString() + Oper.ToString() + "\n" + Right.ToString() + "\n)" );
	}
    
	public override void Visit(IExpVisitor v) 
	{
	    v.BinopExp(this);
	}
    }

    class CompareExp : BoolExp
    {
	public Exp Left, Right;
	public Operator Oper;
	
	public CompareExp(Exp l, Token op, Exp r)
	{
	    Left = l;
	    Right = r;
	    
	    switch(op.Type){
	    case TokenType.LT:
		Oper = Operator.LT;
		break;
	    case TokenType.GT:
		Oper = Operator.GT;
		break;
	    case TokenType.EQ:
		Oper = Operator.EQ;
		break;
	    case TokenType.LE:
		Oper = Operator.LE;
		break;
	    case TokenType.GE:
		Oper = Operator.GE;
		break;
	    }
	}
	
	public override String ToString()
	{
	    return ("CompareExp(\n" + Left.ToString() + Oper.ToString() + "\n" + Right.ToString() + "\n)" );
	}

	public override void Visit(IExpVisitor v) 
	{
	    v.CompareExp(this);
	}
    }
    

    class IntExp : NumericExp
    {
	public long Value;
    
	public IntExp(long v)
	{
	    Value = v;
	    ExpType = typeof(int);
	}

	public override String ToString()
	{
	    return ("IntExp(" + Value + ")\n");
	}

	public override void Visit(IExpVisitor v) 
	{
	    v.IntExp(this);
	}
    }

    class IsNullExp : BoolExp
    {
	public Exp Expr;
	
	public IsNullExp(Exp e)
	{
	    Expr = e;
	}
	
	public override String ToString()
	{
	    return ("IsNullExp(\n" + Expr.ToString() + ")\n");
	}

	public override void Visit(IExpVisitor v) 
	{

	}
    }
    
	    
    /*
    class NilExp : ListExp
    {
	public override String ToString()
	{
	    return "NilExp()\n";
	}
	
    }
    */

    class CdrExp : ListExp
    {
	public Exp Left;
    
	public CdrExp(Exp l)
	{
	    Left = l;
	}
    
	public override String ToString()
	{
	    return ("CdrExp(\n" + Left.ToString() + ")\n");
	}
	
	public override void Visit(IExpVisitor v)
	{
	}
	
    
    }

    class CarExp : ListExp
    {
	public Exp Left;
    
	public CarExp(Exp l)
	{
	    Left = l;
	}
    
	public override String ToString()
	{
	    return ("CarExp(\n" + Left.ToString() + ")\n");
	}

	public override void Visit(IExpVisitor v) 
	{
	    v.CarExp(this);
	}
    
    }


    class ConsExp : ListExp
    {
	public Exp Left, Right;
    
	public ConsExp(Exp l, Exp r)
	{
	    Left = l;
	    Right = r;
	}

	public override String ToString()
	{
	    return ("ConsExp(\n" + Left.ToString() + ",\n " + Right.ToString() + ")\n");
	}
	public override void Visit(IExpVisitor v)
	{
	}
    }

    class SubstExp : ListExp
    {
	public Exp Replace, Find, Expr;
    
	public SubstExp(Exp r, Exp f, Exp e)
	{
	    Replace = r;
	    Find = f;
	    Expr = e;
	}
    
	public override String ToString()
	{
	    return ("SubsExp(\n" + Replace.ToString() + ",\n" + Find.ToString() + ",\n" + Expr.ToString() + ")\n");
	}
	public override void Visit(IExpVisitor v)
	{
	}
    }


    class StringExp : ListExp
    {
	public String Value;
    
	public StringExp(String v)
	{
	    Value = v;
	}

	public override String ToString()
	{
	    return ("StringExp(" + Value + ")\n");
	}
    
	public override void Visit(IExpVisitor v) 
	{
	    v.StringExp(this);
	}
    }

    class VarExp : Exp
    {
	public String Name;
	public int Pos;
    
	public VarExp(String n, int p)
	{
	    Name = n;
	    Pos = p;
	}
    
	public override String ToString()
	{
	    return ("VarExp(" + Name + ", " + Pos + ")");
	}

	public override void Visit(IExpVisitor v) 
	{
	    v.VarExp(this);
	}
    
    }

    class GlobalVarExp : Exp
    {
	public String Name;
    
	public GlobalVarExp(String n)
	{
	    Name = n;
	    ExpType = typeof(int);
	}
    
	public override String ToString()
	{
	    return ("GlobalVarExp(" + Name + ")");
	}

	public override void Visit(IExpVisitor v) 
	{
	    v.GlobalVarExp(this);
	}

    
    }


    class CallExp : Exp
    {
	public String ClassName;
	public String FunctionName;
	public Exp[] Params;
	public bool System;
	
	public CallExp(String c, String f, Exp[] p)
	{
	    ClassName = c;
	    FunctionName = f;
	    Params = p;
	    System = false;
	}
    
	public CallExp(String c, String f, Exp[] p, Type t)
	{
	    ClassName = c;
	    FunctionName = f;
	    Params = p;
	    System = true;
	    ExpType = t;
	}
	
	public override String ToString()
	{
	    return ("CallExp(" + FunctionName + ", " + Params.Length + ")");
	}

	public override void Visit(IExpVisitor v) 
	{
	    v.CallExp(this);
	}
    

    }

    class IfExp : Exp
    {
	public Exp EvalExp, ThenExp, ElseExp;
	
	public IfExp(Exp e, Exp thene, Exp elsee)
	{
	    EvalExp = e;
	    ThenExp = thene;
	    ElseExp = elsee;
	}
	
	public override String ToString()
	{
	    return ("IfExp(\n" + EvalExp.ToString() + " ," + ThenExp.ToString() + " ," + ElseExp.ToString() + ")\n");
	}

	public override void Visit(IExpVisitor v) 
	{
	    v.IfExp(this);
	}
    }
    
    
    class GlobalVarDef : Exp
    {
	public String Name;
	public Exp Value;
    
	public GlobalVarDef(String n, Exp v)
	{
	    Name = n;
	    Value = v;
	    ExpType = Value.ExpType;
	}

	public override String ToString()
	{
	    return ("GlobalVarDef(" + Name + ",\n" + Value.ToString() + ")");
	}

	public override void Visit(IExpVisitor v)
	{
	    v.GlobalVarDef(this);
	}
	
    }


    class FunctionDef : Exp
    {
	public String Name;
	public ArrayList Params;
	public Exp Body;
	public int Count;
    

	public FunctionDef(String n, ArrayList p, Exp b)
	{
	    Name = n;
	    Params = p;
	    Body = b;
	    Count = Params.Count;
	    //ExpType = Body.ExpType;
	}
    
	public override String ToString()
	{
	    return ("FunctionDef( " + Name + " " + Params.Count + "\n" + Body.ToString() + ")");
	}

	public override void Visit(IExpVisitor v)
	{
	    v.FunctionDef(this);
	}
    }


    class ToIntExp : NumericExp
    {
	public Exp Value;
	
	public ToIntExp(Exp v)
	{
	    Value = v;
	}
	
	public override String ToString()
	{
	    return ("ToIntExp(\n" + Value.ToString() + ")");
	}
	public override void Visit(IExpVisitor v)
	{
	    v.ToIntExp(this);
	}
    }

    class ToListExp : ListExp
    {
	public Exp Value;
	
	public ToListExp(Exp v)
	{
	    Value = v;
	}
	
	public override String ToString()
	{
	    return ("ToListExp(\n" + Value.ToString() + ")");
	}
	
	public override void Visit(IExpVisitor v)
	{
	    v.ToListExp(this);
	}
    }
    

    
    class DoExp : Exp
    {
	public ExpList Vars, Conds;
	 
	public DoExp(ExpList v, ExpList c)
	{
	    Vars = v;
	    Conds = c;
	}
	
	public override String ToString()
	{
	    return ("DoExp(\n" + Vars.ToString() + ",\n" + Conds.ToString() + ")\n")  ;
	}

	public override void Visit(IExpVisitor v)
	{
	    v.DoExp(this);
	    
	}
	
    }
    
    class DoVarExp : Exp
    {
	public String Name;
	public int Pos;
	
	public DoVarExp(String n, int p)
	{
	    Name = n;
	    Pos = p;
	}
	
	public override String ToString()
	{
	    return ("DoVarExp(\n" + Name + ", " + Pos.ToString() + ")");
	}

	public override void Visit(IExpVisitor v)
	{
	    v.DoVarExp(this);
	    
	}
	
    }
    
	    
    class DoVarDef : Exp
    {
	public String Name;
	public Exp Init, Iter;
	public int Pos;
	
	public DoVarDef(String n, Exp init, Exp it, int p)
	{
	    Name = n;
	    Init = init;
	    Iter = it;
	    Pos = p;
	}
	

	public override String ToString()
	{
	    return ("DoVarDef(\n" + Name + ",\n" + Init.ToString() + ",\n" + Iter.ToString() + ")\n");
	}

	public override void Visit(IExpVisitor v)
	{
	    v.DoVarDef(this);
	}
    }
    
    class DoCondExp : Exp
    {
	public Exp Cond, Ret;
	public System.Reflection.Emit.Label EndLabel;
	
	public DoCondExp(Exp c, Exp r)
	{
	    Cond = c;
	    Ret = r;
	}

	public override String ToString()
	{
	    return ("DoCondExp(\n" + Cond.ToString() + ",\n" + Ret.ToString() + ")\n");
	}

	public override void Visit(IExpVisitor v)
	{
	    v.DoCondExp(this);
	}
	
    }
    
 
}

