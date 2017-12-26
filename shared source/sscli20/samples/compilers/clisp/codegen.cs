//------------------------------------------------------------------------------
// <copyright file="codegen.cs" company="Microsoft">
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
using System.Reflection.Emit;
using System.Collections;
using System.Threading;
using Absyn;


class CodeGen : IExpVisitor
{
    ExpList expr;
    TypeBuilder tb;
    MethodBuilder methodb;
    Hashtable Functions;
    Hashtable GlobalVars;
    Hashtable DoVars;
    
    ILGenerator il;
    
    public CodeGen(ExpList ex, Hashtable f, Hashtable g)
    {
	expr = ex;
	Functions = new Hashtable();
	GlobalVars = new Hashtable();
	DoVars = new Hashtable();
    }
    
    void Init()
    {
	
    }

    public void IntExp(IntExp e)
    {
		switch(e.Value){
		case 0:
		il.Emit(OpCodes.Ldc_I4_0);
		break;
		case 1:
		    il.Emit(OpCodes.Ldc_I4_1);
		    break;
		case 2:
		    il.Emit(OpCodes.Ldc_I4_2);
		    break;
		case 3:
		    il.Emit(OpCodes.Ldc_I4_3);
		    break;
		case 4:
		    il.Emit(OpCodes.Ldc_I4_4);
		    break;
		case 5:
		    il.Emit(OpCodes.Ldc_I4_5);
		    break;
		case 6:
		    il.Emit(OpCodes.Ldc_I4_6);
		    break;
		case 7:
		    il.Emit(OpCodes.Ldc_I4_7);
		    break;
		case 8:
		    il.Emit(OpCodes.Ldc_I4_8);
		    break;
		default:
		    if (e.Value > -128 && e.Value < 127)
			il.Emit(OpCodes.Ldc_I4_S, (byte)e.Value);
		    else if (e.Value > Int32.MinValue && e.Value < Int32.MaxValue)
			il.Emit(OpCodes.Ldc_I4, (int)e.Value);
		    else 
			il.Emit(OpCodes.Ldc_I8, e.Value);
		    break;
		}
    }
    
    public void BinopExp(BinopExp e)
    {
	e.Left.Visit(this);
	e.Right.Visit(this);
	switch(e.Oper){
	case Operator.ADD:
	    il.Emit(OpCodes.Add);
	    break;
	case Operator.SUB:
	    il.Emit(OpCodes.Sub);
	    break;
	case Operator.MUL:
	    il.Emit(OpCodes.Mul);
	    break;
	case Operator.DIVIDE:
	    il.Emit(OpCodes.Div);
	    break;
	}
    }

    public void CompareExp(CompareExp e)
    {
	Label TrueLabel = il.DefineLabel();
	Label EndLabel = il.DefineLabel();
	e.Left.Visit(this);
	e.Right.Visit(this);
	switch(e.Oper){
	case Operator.LT:
	    il.Emit(OpCodes.Blt, TrueLabel);
	    break;
	case Operator.GT:
	    il.Emit(OpCodes.Bgt, TrueLabel);
	    break;
	case Operator.EQ:
	    il.Emit(OpCodes.Beq, TrueLabel);
	    break;
	case Operator.LE:
	    il.Emit(OpCodes.Ble, TrueLabel);
	    break;
	case Operator.GE:
	    il.Emit(OpCodes.Bge, TrueLabel);
	    break;

	}
	il.Emit(OpCodes.Ldc_I4_0);
	il.Emit(OpCodes.Br, EndLabel);
	il.MarkLabel(TrueLabel);
	il.Emit(OpCodes.Ldc_I4_1);
	il.MarkLabel(EndLabel);
	
    }
    

    public void CallExp(CallExp e)
    {
	for(int i = 0; i < e.Params.Length; i++){
	    e.Params[i].Visit(this);
	}
	if(e.System){
	    il.Emit(OpCodes.Call, ((typeof(LispRuntime)).GetMethod(e.FunctionName)));
	}
	else{
	    il.Emit(OpCodes.Call, (MethodInfo)Functions[e.FunctionName]);
	}
	
    }
    
    public void CarExp(CarExp e)
    {
	e.Left.Visit(this);
	il.Emit(OpCodes.Call, ((typeof(LispRuntime)).GetMethod("Car")));
    }
    
    public void IsNullExp(IsNullExp e)
    {
    }

    public void ToIntExp(ToIntExp e)
    {
	e.Value.Visit(this);
	il.Emit(OpCodes.Call, ((typeof(LispRuntime)).GetMethod("ToInt")));
    }

    public void ToListExp(ToListExp e)
    {
	e.Value.Visit(this);
	il.Emit(OpCodes.Call, ((typeof(LispRuntime)).GetMethod("ToList")));
    }
    
    
    public void StringExp(StringExp e)
    {
	il.Emit(OpCodes.Ldstr, (e.Value));
	il.Emit(OpCodes.Call, ((typeof(LispRuntime)).GetMethod("Init")));
    }
    
    public void VarExp(VarExp e)
    {
	switch(e.Pos){
	case 0:
	    il.Emit(OpCodes.Ldarg_0);
	    break;
	case 1:
	    il.Emit(OpCodes.Ldarg_1);
	    break;
	case 2:
	    il.Emit(OpCodes.Ldarg_2);
	    break;
	case 3:
	    il.Emit(OpCodes.Ldarg_3);
	    break;
	default:
	    if(e.Pos > -128 && e.Pos < 127)
		il.Emit(OpCodes.Ldarg_S, e.Pos);
	    else 
		il.Emit(OpCodes.Ldarg, e.Pos);
	    break;
	}
    }
    
    public void GlobalVarExp(GlobalVarExp e)
    {
	    il.Emit(OpCodes.Ldsfld, (FieldInfo)GlobalVars[e.Name]);	
    }
    
    public void IfExp(IfExp e)
    {
	Label FalseLabel, EndLabel;
	FalseLabel = il.DefineLabel();
	EndLabel = il.DefineLabel();
	e.EvalExp.Visit(this);
	il.Emit(OpCodes.Brfalse, FalseLabel);
	e.ThenExp.Visit(this);
	il.Emit(OpCodes.Br, EndLabel);
	il.MarkLabel(FalseLabel);
	e.ElseExp.Visit(this);
	il.MarkLabel(EndLabel);
	
    }

    public void DoExp(DoExp e)
    {
	Label l1 = il.DefineLabel();
	Label l2 = il.DefineLabel();
	Label EndLabel = il.DefineLabel();
	
	for (ExpList el = e.Vars; el != null; el = el.Tail){
	    el.Head.Visit(this);
	}
	il.Emit(OpCodes.Br, l1);
	il.MarkLabel(l2);
	for (ExpList el = e.Vars; el != null; el = el.Tail){
	    DoVarIter((DoVarDef)el.Head);
	}
	il.MarkLabel(l1);
	
	for (ExpList el = e.Conds; el != null; el = el.Tail){
	    ((DoCondExp)el.Head).EndLabel = EndLabel;
	    el.Head.Visit(this);
	}
	il.Emit(OpCodes.Br, l2);
	
	il.MarkLabel(EndLabel);
	
    }

    public void DoVarIter(DoVarDef e)
    {
	e.Iter.Visit(this);
	il.Emit(OpCodes.Stloc, (LocalBuilder)DoVars[e.Pos]);
    }
    

    public void DoVarExp(DoVarExp e)
    {
	    il.Emit(OpCodes.Ldloc, (LocalBuilder)DoVars[e.Pos]);
    }
    
    public void DoCondExp(DoCondExp e)
    {
	Label FalseLabel;
	FalseLabel = il.DefineLabel();

	e.Cond.Visit(this);
	il.Emit(OpCodes.Brfalse, FalseLabel);
	e.Ret.Visit(this);
	il.Emit(OpCodes.Br, e.EndLabel);
	
	il.MarkLabel(FalseLabel);
	
    }
    
    public void DoVarDef(DoVarDef e)
    {
	LocalBuilder lt = il.DeclareLocal(e.Init.ExpType);
	DoVars.Add(e.Pos, lt);
	e.Init.Visit(this);
	il.Emit(OpCodes.Stloc, lt);
    }
    
    
    public void FunctionDef(FunctionDef e)
    {
	Type[] param = new Type[e.Count];
	DoVars = new Hashtable();
	
	for (int i = 0; i < e.Count; i++){
	    param[i] = (Type)e.Params[i];
	}
	
	//DescriptorInfo di = new DescriptorInfo(param);
	//di.SetReturnType(e.ExpType);
	//di.MethodAttributes = MethodAttributes.Static | MethodAttributes.Public;
	methodb = tb.DefineMethod(e.Name, MethodAttributes.Static | MethodAttributes.Public, e.ExpType, param);
	Functions.Add(e.Name, methodb);
	ILGenerator ilmain = il;
	il = methodb.GetILGenerator();

	e.Body.Visit(this);
	
	il.Emit(OpCodes.Ret);
	il = ilmain;
    }
    
    public void GlobalVarDef(GlobalVarDef e)
    {
	FieldInfo fi = tb.DefineField(e.Name, e.ExpType, FieldAttributes.Static);
	GlobalVars.Add(e.Name, fi);

	e.Value.Visit(this);
	il.Emit(OpCodes.Stsfld, fi);
    }
    
    public void Generate(String filename)
    {
	AppDomain ad = Thread.GetDomain(); //AppDomain.CreateDomain("First", null, null);
	AssemblyName an = new AssemblyName();
    an.Name = filename + ".exe"; //AssemblyName.CreateSimpleName(filename + ".exe", "LispExe", "Lisp Executable", "default_alias");
	AssemblyBuilder ab = ad.DefineDynamicAssembly(an, AssemblyBuilderAccess.RunAndSave);
	ModuleBuilder mb = ab.DefineDynamicModule(filename + ".exe", filename + ".exe");
	MethodBuilder methodb;
	//DescriptorInfo di = new DescriptorInfo(0);
	tb = mb.DefineType(filename);
	
	//di.SetReturnType(typeof(void));
	//di.MethodAttributes = MethodAttributes.Static | MethodAttributes.Public;
	methodb = tb.DefineMethod("Main", MethodAttributes.Static | MethodAttributes.Public, typeof(void), null);
	il = methodb.GetILGenerator();

	do{
	    expr.Head.Visit(this);
    	    if (expr.Head is FunctionDef)
    		GenerateDefStub(((FunctionDef)expr.Head).Name);
     	    else if (expr.Head is GlobalVarDef)
    		GenerateDefStub(((GlobalVarDef)expr.Head).Name);
 	    else if (expr.Head.ExpType == typeof(int))
  		GenerateNumericExpStub();
    	    else if (expr.Head.ExpType == typeof(bool))
		GenerateBoolExpStub();
    	    else if (expr.Head.ExpType == typeof(CList)) 
    		GenerateListExpStub();
	    
	    expr = expr.Tail;
	}while(expr != null);
	
	il.Emit(OpCodes.Ret);
	tb.CreateType();
	
	ab.SetEntryPoint((mb.GetType(filename)).GetMethod("Main"));
	ab.Save(filename + ".exe");
    }

    void GenerateNumericExpStub()
    {
  	Type[] type = new Type[1];
  	type[0] = typeof(System.Int32);
  	il.Emit(OpCodes.Call, ((typeof(System.Console)).GetMethod("WriteLine", type)));
    }

    void GenerateListExpStub()
    {
  	il.Emit(OpCodes.Call, ((typeof(LispRuntime)).GetMethod("Print")));
  	Type[] type = new Type[1];
  	type[0] = typeof(System.String);
	il.Emit(OpCodes.Ldstr, "");                //This is just to do a WriteLine after the Print.
  	il.Emit(OpCodes.Call, ((typeof(System.Console)).GetMethod("WriteLine", type)));
	
    }

    void GenerateDefStub(String Name)
    {
  	Type[] type = new Type[1];
  	type[0] = typeof(System.String);
	il.Emit(OpCodes.Ldstr, Name);
  	il.Emit(OpCodes.Call, ((typeof(System.Console)).GetMethod("WriteLine", type)));
	
    }

    void GenerateBoolExpStub()
    {
	il.Emit(OpCodes.Call, ((typeof(LispRuntime)).GetMethod("PrintBool")));
    }
    
}
