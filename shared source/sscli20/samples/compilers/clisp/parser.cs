//------------------------------------------------------------------------------
// <copyright file="parser.cs" company="Microsoft">
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
using System.Reflection;
using System.Collections;
using Absyn;
using LexToken;

class Parser 
{
    Queue tokens;
    public ExpList e;
    bool success;
    Hashtable CurrentVarScope;
    public Hashtable Functions;
    public Hashtable GlobalVars;
    public Hashtable DoVars;
    public int DoVarsCount;
    
    public TypeCheck Tc;
    
    public Parser(Queue t)
    {
	tokens = t;
	success = true;
	Functions = new Hashtable();
	GlobalVars = new Hashtable();
	DoVars = new Hashtable();
    }

    public bool Parse()
    {
	//e = null;
	e = MatchStm();
	//	Match(TokenType.EoFSym);
	MatchToken(TokenType.EOFSYM);
	Tc = new TypeCheck(e, Functions);
	bool tsuccess = Tc.Check();
	e = Tc.e;                                       //Exp is modified in TypeCheck
	return success && tsuccess;                     //Return success if both parsing and typechecking succeed.
    }

    ExpList MatchStm()
    {
	ExpList ex1 = new ExpList();
	ex1.Head = MatchExp();
	if(((Token)tokens.Peek()).Type != TokenType.EOFSYM){
	    ex1.Tail = MatchStm();
	}
	else 
	    ex1.Tail = null;

	return ex1;
    }
    
    Exp MatchExp()
    {
	AlphaToken at;
	Object o1;
	
	switch(((Token)tokens.Peek()).Type){
	case TokenType.NIL:
	    tokens.Dequeue();
	    return new StringExp(String.Empty);          //NIL is represented as an empty string
	case TokenType.NUMBER:
	    return new IntExp(((NumberToken)tokens.Dequeue()).Value);
	case TokenType.STRING:
	    return new StringExp(((StringToken)tokens.Dequeue()).Value);

	case TokenType.ALPHA:
	    at = (AlphaToken)tokens.Dequeue();
	    
	    if (DoVars != null && DoVars.Count != 0){
		o1 = DoVars[at.Name];
		if (o1 != null){
		    return new DoVarExp(at.Name, (int)o1);
		}
	    }
	    
	    if (CurrentVarScope != null && CurrentVarScope.Count != 0){
		o1 = CurrentVarScope[at.Name];
		if (o1 != null){
		    return new VarExp(at.Name, (int)o1);
		}
	    }
	    o1 = GlobalVars[at.Name];
	    
	    if(o1 != null){
		return new GlobalVarExp(at.Name);
	    }
	    Console.WriteLine("Error 6: Undefined Symbol " + at.Name);
	    return null;

	    
	case TokenType.LPAREN:
	    MatchToken(TokenType.LPAREN);
	    Exp e1, e2, e3;
	    Exp [] ea;
	    //ListExp el1, el2, el3;
	    Token o;
	

	    switch(((Token)tokens.Peek()).Type){
	    case TokenType.PLUS:
	    case TokenType.MINUS:
	    case TokenType.MUL:
	    case TokenType.DIVIDE:
		o = (Token)tokens.Dequeue();
		e1 = MatchExp();
		e2 = MatchExp();
		MatchToken(TokenType.RPAREN);
		return new BinopExp(e1, o ,e2);

	    case TokenType.LT:
	    case TokenType.GT:
	    case TokenType.EQ:
	    case TokenType.LE:
	    case TokenType.GE:
		o = (Token)tokens.Dequeue();
		e1 = MatchExp();
		e2 = MatchExp();
		MatchToken(TokenType.RPAREN);
		return new CompareExp(e1, o ,e2);
		
	    case TokenType.CDR:
		tokens.Dequeue();
		e1 = MatchExp();
		MatchToken(TokenType.RPAREN);
		ea = new Exp[1];

		ea[0] = e1;
		return new CallExp("LispRuntime", "Cdr", ea, typeof(CList));
	    
	    case TokenType.CAR:
		tokens.Dequeue();	    
		e1 = MatchExp();
		MatchToken(TokenType.RPAREN);
		return new CarExp(e1);
	
	    case TokenType.CONS:
		tokens.Dequeue();
		e1 = MatchExp();
		e2 = MatchExp();
		MatchToken(TokenType.RPAREN);
		ea = new Exp[2];
		ea[0] = e1;
		ea[1] = e2;
		return new CallExp("LispRuntime", "Cons", ea, typeof(CList));
	    
	    case TokenType.SUBST:
		tokens.Dequeue();
		e1 = MatchExp();
		e2 = MatchExp();
		e3 = MatchExp();
		MatchToken(TokenType.RPAREN);
		ea = new Exp[3];
		ea[0] = e1;
		ea[1] = e2;
		ea[2] = e3;
		return new CallExp("LispRuntime", "Subst", ea, typeof(CList));
		
	    case TokenType.NULL:
		tokens.Dequeue();
		e1 = MatchExp();
		MatchToken(TokenType.RPAREN);
		ea = new Exp[1];
		ea[0] = e1;
		return new CallExp("LispRuntime", "IsNull", ea, typeof(bool));
		
	    case TokenType.ATOM:
		tokens.Dequeue();
		e1 = MatchExp();
		MatchToken(TokenType.RPAREN);
		ea = new Exp[1];
		ea[0] = e1;
		return new CallExp("LispRuntime", "IsAtom", ea, typeof(bool));
		
	    case TokenType.IF:
		tokens.Dequeue();
		e1 = MatchExp();
		e2 = MatchExp();
		e3 = MatchExp();
		MatchToken(TokenType.RPAREN);
		return new IfExp(e1, e2, e3);

	    case TokenType.DO:
		tokens.Dequeue();
		MatchToken(TokenType.LPAREN);
		ExpList el1, el2;
		if (CurrentVarScope == null){
		    Console.WriteLine("Error 13: Do must be present in a Function");
		    success = false;
		    return null;
		}
		
		el1 = MatchDoExpList();
		MatchToken(TokenType.RPAREN);
		//MatchToken(TokenType.LPAREN);
		el2 = MatchDoCondList();
		MatchToken(TokenType.RPAREN);
		return new DoExp(el1, el2);
				
			    
	    case TokenType.SETQ:
		tokens.Dequeue();
		AlphaToken t1 = (AlphaToken)MatchToken(TokenType.ALPHA);
		if(((Token)tokens.Peek()).Type == TokenType.NUMBER){
		    e1 = new IntExp(((NumberToken)tokens.Dequeue()).Value);
		}
		else
		    e1 = MatchExp();
		GlobalVars.Add(t1.Name, typeof(int));
		MatchToken(TokenType.RPAREN);
		return new GlobalVarDef(t1.Name, e1);
	    
	    case TokenType.DEFUN:
		ArrayList param = new ArrayList();
		int count = 0;
		if (CurrentVarScope != null){
		    Console.WriteLine("Error 12: Nested Functions Not Allowed");
		    success = false;
		    return null;
		}
		CurrentVarScope = new Hashtable();
		DoVars = new Hashtable();
		DoVarsCount = 0;
		
		tokens.Dequeue();
		AlphaToken t = (AlphaToken)MatchToken(TokenType.ALPHA);
	    
		MatchToken(TokenType.LPAREN);
	    
		while(((Token)tokens.Peek()).Type == TokenType.ALPHA){
		    param.Add(((AlphaToken)tokens.Dequeue()).Name);
		    CurrentVarScope.Add(param[count], count++);
		}
		MatchToken(TokenType.RPAREN);
		//CurrentVarScope = param;
		Functions.Add(t.Name, count);
	    
		e1 = MatchExp();
		CurrentVarScope = null;
		MatchToken(TokenType.RPAREN);
		return new FunctionDef(t.Name, param, e1);
	    
	    case TokenType.ALPHA:
		at = (AlphaToken)tokens.Dequeue();
		int iparams;
	    
		if( Functions == null || Functions.Count == 0){
		    Console.WriteLine("Error 6: Undefined Symbol " + at.Name);
		    return null;
		}
		else {
		
		    o1 = Functions[at.Name];
		    if (o1 == null){
			Console.WriteLine("Error 6: Undefined Symbol " + at.Name);
			return null;
		    }
		    else {
			iparams = (int)o1;
		    }
		}
		Exp [] eparams = new Exp[iparams];
	    
		for (int i = 0; i < iparams; i++){
		    eparams[i] = MatchExp();
		}

		MatchToken(TokenType.RPAREN);
		return new CallExp(null, at.Name, eparams);
	    
	    default:
		o = null;
		Console.WriteLine("Error 1");
		success = false;
		return null;
	    }
	default:
	    Console.WriteLine("Error 7: Unexpected Token");
	    tokens.Dequeue();
	    success = false;
	    return null;
	    
	}
	
    }

    ExpList MatchDoExpList()
    {
	ExpList el1 = new ExpList();

	//MatchToken(TokenType.LPAREN);

	el1.Head = MatchDoVar();
	if(((Token)tokens.Peek()).Type != TokenType.RPAREN){
	    el1.Tail = MatchDoExpList();
	}
	else{
	    el1.Tail = null;
	}

	//MatchToken(TokenType.RPAREN);
	
	return el1;
    }

    Exp MatchDoVar()
    {
	AlphaToken at;
	Exp e1, e2;
	MatchToken(TokenType.LPAREN);
	
	switch(((Token)tokens.Peek()).Type){
	case TokenType.ALPHA:
	    at = (AlphaToken)tokens.Dequeue();
	    DoVars.Add(at.Name, DoVarsCount);
	    int pos = DoVarsCount;
	    DoVarsCount++;
	    e1 = MatchExp();
	    e2 = MatchExp();
	    MatchToken(TokenType.RPAREN);
	    return new DoVarDef(at.Name, e1, e2, pos);

	default:
	    Console.WriteLine("Error 10: Expected Variable");
	    success = false;
	    return null;
	}
    }
    
	  

    ExpList MatchDoCondList()
    {
	ExpList el1 = new ExpList();
	
	//MatchToken(TokenType.LPAREN);
	
	el1.Head = MatchDoCond();
	if(((Token)tokens.Peek()).Type != TokenType.RPAREN){
	    el1.Tail = MatchDoCondList();
	}
	else {
	    el1.Tail = null;
	}
	
	//MatchToken(TokenType.RPAREN);
	
	return el1;
	
    }
    
    Exp MatchDoCond()
    {
	Exp e1, e2;
	MatchToken(TokenType.LPAREN);
	e1 = MatchExp();
	e2 = MatchExp();
	MatchToken(TokenType.RPAREN);
	return new DoCondExp(e1, e2);
    }
    
	

    /*
    Exp MatchNumericExp()
    {
	switch(((Token)tokens.Peek()).Type){
	case TokenType.NUMBER:
	    return new IntExp(((NumberToken)tokens.Dequeue()).Value);
	case TokenType.LPAREN:
	    return (NumericExp)MatchExp();


        default:
	    Console.WriteLine("Error 3: Unexpected TOken" + tokens.Peek().ToString());
	    success = false;
	    return null;
	}
	
    }
    
    Exp MatchListExp()
    {
	switch(((Token)tokens.Peek()).Type){
	case TokenType.NIL:
	    MatchToken(TokenType.NIL);
	    return new StringExp(String.Empty);
	case TokenType.STRING:
	    return new StringExp(((StringToken)tokens.Dequeue()).Value);
	case TokenType.LPAREN:
	    return MatchExp();
        default:
	    Console.WriteLine("Error 5: Unexpected TOken" + tokens.Peek().ToString());
	    success = false;
	    return null;
	}
    }
    
	    
    
    ListExp MatchListLiteral()
    {
	MatchToken(TokenType.QUOTE);
	switch(((Token)tokens.Peek()).Type){
	case TokenType.ALPHA:
	    List al = new List();
	    al.Add(((AlphaToken)tokens.Dequeue()).Name);
	    return new ListExp(null);
	case TokenType.LPAREN:
	    return MatchMultipleList();
	default:
	    Console.WriteLine("Error 4: Unexpected Token: " + tokens.Peek().ToString() );
	    success = false;
	    return null;
	}
	
    }
    */
    /*    
    StringExp MatchMultipleList()
    {
	MatchToken(TokenType.LPAREN);
	String str = "(";
		while(((Token)tokens.Peek()).Type == TokenType.ALPHA){
	    al.Add(((AlphaToken)tokens.Dequeue()).Name);
	}
	MatchToken(TokenType.RPAREN);
	return new ListExp(null);
    }
    */
    
    Token MatchToken(TokenType expectedtokentype)
    {
	if(tokens.Count == 0){
	    Console.WriteLine("Error 6: UnExpected End of Program");
	    return null;
	}
	
	Token t = (Token)tokens.Dequeue();
	if (expectedtokentype != t.Type){
	    Console.WriteLine("Error 2: Unexpected Token:" + t.Type.ToString()  + ", expected:" + expectedtokentype.ToString());
	    success = false;
	}
	//Console.WriteLine(t);
	return t;
    }
    
    
}





