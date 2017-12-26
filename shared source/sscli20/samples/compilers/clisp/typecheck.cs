//------------------------------------------------------------------------------
// <copyright file="typecheck.cs" company="Microsoft">
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
using Absyn;

interface IExpVisitor
{
    void BinopExp(BinopExp e);
    void CompareExp(CompareExp e);
    void IntExp(IntExp e);
    void CallExp(CallExp e);
    void CarExp(CarExp e);
    //void IsNullExp(IsNullExp e);
    void StringExp(StringExp e);
    void VarExp(VarExp e);
    void GlobalVarExp(GlobalVarExp e);
    void IfExp(IfExp e);
    void FunctionDef(FunctionDef e);
    void GlobalVarDef(GlobalVarDef e);
    void DoExp(DoExp e);
    void DoVarExp(DoVarExp e);
    void DoVarDef(DoVarDef e);
    void DoCondExp(DoCondExp e);
    void ToIntExp(ToIntExp e);
    void ToListExp(ToListExp e);
    
}


class TypeCheck : IExpVisitor
{
    Hashtable Functions, GlobalVars, CurrentFuncDef, DoVars;
    bool success;
    public ExpList e;
    String CurrentFuncName;
    
    public TypeCheck(ExpList el, Hashtable f)
    {
	e = el;
	Functions = new Hashtable();

	ArrayList Params = new ArrayList();
	Params.Add(typeof(CList));
	Functions.Add("Cdr", new FunctionDef("Cdr", Params, null));
	Functions.Add("IsNull", new FunctionDef("IsNull", Params, null));
	Functions.Add("IsAtom", new FunctionDef("IsAtom", Params, null));
	
	Params.Add(typeof(CList));
	Functions.Add("Cons", new FunctionDef("Cons", Params, null));
	Params.Add(typeof(CList));
	Functions.Add("Subst", new FunctionDef("Subst", Params, null));

	GlobalVars = new Hashtable();
	success = true;
	CurrentFuncDef = null;
    }

    public bool Check()
    {
	
	for (ExpList el = e; el != null; el = el.Tail){
	    el.Head.Visit(this);
	}
	return success;
    }
    
    
    void CheckInt(Exp e)
    {
	if(e.ExpType != typeof(int)){
	    if (e is VarExp && e.ExpType == typeof(void)){        //Param Variable. Type unknown. Assume first occurence
		e.ExpType = typeof(int);
		CurrentFuncDef.Add(((VarExp)e).Name, typeof(int));
	    }
	    else if (e is DoVarExp && e.ExpType == typeof(void)){   //Do Variable. Type unknown. Assume first occurence
		e.ExpType = typeof(int);
		DoVars.Add(((DoVarExp)e).Pos, typeof(int));
	    }
	    else if (e is CallExp && e.ExpType == typeof(void)){       //Recursive function call. Assume first occurence
		e.ExpType = typeof(int);
	    }
	    else if (!(e is CarExp)){                                  //CarExp will be coerceto int
		Console.WriteLine("Int expected");
		success = false;
	    }
	}

    }


    void CheckList(ref Exp e)
    {
	if (e.ExpType == typeof(int)){
	    e = new ToListExp(e);
	    e.ExpType = typeof(CList);
	}
	
	if (e.ExpType != typeof(CList)){
	    if (e is VarExp && e.ExpType == typeof(void)){
		e.ExpType = typeof(CList);
		CurrentFuncDef.Add(((VarExp)e).Name, typeof(CList));
	    }
	    else if (e is DoVarExp && e.ExpType == typeof(void)){
		e.ExpType = typeof(CList);
		DoVars.Add(((DoVarExp)e).Pos, typeof(CList));
	    }
	    else if (e is CallExp && e.ExpType == typeof(void)){       //Recursive function call. Assume first occurence
		e.ExpType = typeof(int);
	    }
	    else if (!(e is IntExp)){                                  //IntExp will be coerceto List
		Console.WriteLine("List expected");
		success = false;
	    }
	}
    }
    
    void CheckBool(Exp e)
    {
	if (e.ExpType != typeof(bool)){
	    if (e is VarExp && e.ExpType == typeof(void)){
		e.ExpType = typeof(CList);
	    }
	    else{
		Console.WriteLine("Bool expected");
		success = false;
	    }
	}
    }
    
    void CheckAny(ref Exp e, Type t)
    {
	if (t == typeof(int)){
	    CheckInt(e);
	}
	else if (t == typeof(CList)){
	    CheckList(ref e);
	}
	else if (t == typeof(bool)){
	    CheckBool(e);
	}
    }
    
    public void BinopExp(BinopExp e)
    {
	e.Left.Visit(this);
	CheckInt(e.Left);
	e.Right.Visit(this);
	CheckInt(e.Right);
	if (e.Left.ExpType != typeof(int)){
	    if (e.Left is CarExp){
		e.Left = new ToIntExp(e.Left);
	    }
	    else{
		Console.WriteLine("Error 101: Cannot Convert expression to Int");
		success = false;
	    }
	    
	}
	if (e.Right.ExpType != typeof(int)){
	    if (e.Right is CarExp){
		e.Right = new ToIntExp(e.Right);
	    }
	    else{
		Console.WriteLine("Error 102: Connot Convert expression to Int");
		success = false;
	    }
	    
	    
	}
	e.ExpType = typeof(int);
	
    }

    public void CompareExp(CompareExp e)
    {
	e.Left.Visit(this);
	CheckInt(e.Left);
	e.Right.Visit(this);
	CheckInt(e.Right);
	if (e.Left.ExpType != typeof(int)){
	    if (e.Left is CarExp){
		e.Left = new ToIntExp(e.Left);
	    }
	    else{
		Console.WriteLine("Error 101: Cannot Convert expression to Int");
		success = false;
	    }
	    
	}
	if (e.Right.ExpType != typeof(int)){
	    if (e.Right is CarExp){
		e.Right = new ToIntExp(e.Right);
	    }
	    else{
		Console.WriteLine("Error 102: Connot Convert expression to Int");
		success = false;
	    }
	}
	e.ExpType = typeof(bool);
    }
        
    public void IntExp(IntExp e)
    {
	e.ExpType = typeof(int);
    }
    
    public void CallExp(CallExp e)
    {
	if (!e.System){
	    if(e.FunctionName == CurrentFuncName){
		for(int i = 0; i < e.Params.Length; i++){
		    e.Params[i].Visit(this);
		}
		
		e.ExpType = typeof(void);            //Recursive Function. Return type unknown.
	    }
	    else{
		FunctionDef f = (FunctionDef)Functions[e.FunctionName];
		for(int i = 0; i < e.Params.Length; i++){
		    e.Params[i].Visit(this);
		    CheckAny(ref e.Params[i], (Type)f.Params[i]);
		}
		e.ExpType = f.ExpType;
	    }
	}
	else{
	    FunctionDef f = (FunctionDef)Functions[e.FunctionName];
	    for (int i = 0; i < e.Params.Length; i++){
		e.Params[i].Visit(this);
		CheckAny(ref e.Params[i], (Type)f.Params[i]);
	    }
	    //ExpType of System functions is set while parsing itself.
	    //e.ExpType = (Type)Functions[e.FunctionName];
	}
	
    }
    
    public void CarExp(CarExp e)
    {
	e.Left.Visit(this);
	CheckList(ref e.Left);
	e.ExpType = typeof(CList);
	
    }

    

    public void IsNullExp(IsNullExp e)
    {
	e.Expr.Visit(this);
	CheckBool(e.Expr);
	e.ExpType = typeof(bool);
	
    }
    
    public void StringExp(StringExp e)
    {
	e.ExpType = typeof(CList);
	
    }
    
    public void VarExp(VarExp e)
    {
	if (CurrentFuncDef[e.Name] != null){
	    e.ExpType = (Type)CurrentFuncDef[e.Name];
	}
	else{
	    e.ExpType = typeof(void);
	}
	
    }
    
    public void GlobalVarExp(GlobalVarExp e)
    {
	e.ExpType = (Type)GlobalVars[e.Name];
    }
    
    public void IfExp(IfExp e)
    {
	e.EvalExp.Visit(this);
	CheckBool(e.EvalExp);
	e.ThenExp.Visit(this);
	e.ElseExp.Visit(this);
	if (e.ThenExp.ExpType != e.ElseExp.ExpType){
	    Console.WriteLine("Error 103: Then expression and Else Expression do not evaluate to same type");
	    success = false;
	}
	else {
	    e.ExpType = e.ThenExp.ExpType;
	}
	
	
    }
    

    public void FunctionDef(FunctionDef e)
    {
	CurrentFuncDef = new Hashtable();
	DoVars = new Hashtable();
	CurrentFuncName = e.Name;
	
	//Functions.Add(e.Name, e);
	e.Body.Visit(this);
	for(int i = 0; i < e.Params.Count; i++){
	    if(CurrentFuncDef[e.Params[i]] == null){
		e.Params[i] = typeof(void);
	    }
	    else{
		e.Params[i] = CurrentFuncDef[e.Params[i]];      //Now Params have Types
	    }
	}

	CurrentFuncDef = null;
	CurrentFuncName = null;
	DoVars = null;
	
	e.ExpType = e.Body.ExpType;
	Functions.Add(e.Name, e);
    }

    public void GlobalVarDef(GlobalVarDef e)
    {
	e.Value.Visit(this);
	e.ExpType = e.Value.ExpType;
	GlobalVars.Add(e.Name, e.Value.ExpType);
    }
    
    public void DoExp(DoExp e)
    {
	Type RetType;
	
	for (ExpList el = e.Vars; el != null; el = el.Tail){
	    el.Head.Visit(this);
	}
	
	for (ExpList el = e.Conds; el != null; el = el.Tail){
	    el.Head.Visit(this);
	}

	RetType = e.Conds.Head.ExpType;
	
	for (ExpList el = e.Conds; el != null; el = el.Tail){
	    if (RetType != el.Head.ExpType){
		Console.WriteLine("Error 104: Return Type from Do Conditionals should be same");
		success = false;
	    }
	}
	
	e.ExpType = RetType;
    }
    
    public void DoVarExp(DoVarExp e)
    {
	if (DoVars[e.Pos] == null){
	    e.ExpType = typeof(void);        //If Type still not known give it typeof(void)
	}                                    //It will get assumed with use. Checkint, checkbool...
	else{
	    e.ExpType = (Type)DoVars[e.Pos];
	}
	
    }
    
    public void DoVarDef(DoVarDef e)
    {
	e.Init.Visit(this);
	if (e.Init.ExpType != typeof(void)){          //If type of init is known put it in.
	    if(DoVars[e.Pos] == null) 
		DoVars.Add(e.Pos, e.Init.ExpType);          //Else DoVarExp will add typeof(void)
	}
	
	e.Iter.Visit(this);
	if (e.Init.ExpType == typeof(void) && e.Init is VarExp){ //If e.Init is VarExp which currently has no type.
	    e.Init.ExpType = (Type)DoVars[e.Pos];                //Assign it since now we know the type required.
	    CurrentFuncDef.Add(((VarExp)e.Init).Name, e.Init.ExpType);
	}
	
	if (e.Init.ExpType != e.Iter.ExpType ){
	    Console.WriteLine("Do Expression: Init and Iterations have different type");
	    success = false;
	}
	
    }
    
    public void DoCondExp(DoCondExp e)
    {
	e.Cond.Visit(this);
	CheckBool(e.Cond);
	e.Ret.Visit(this);
	e.ExpType = e.Ret.ExpType;
    }

    //These will be generated Internally. Typechecking not required.
    public void ToIntExp(ToIntExp e)
	{
	}

    public void ToListExp(ToListExp e)
    {
    }
    

}


