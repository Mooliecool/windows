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

namespace Microsoft.JScript{

    using System;
    using System.Reflection;
    using System.Reflection.Emit;
    
    internal sealed class PlusAssign : BinaryOp{
      private Plus binOp;
      private Object metaData = null;
      
      internal PlusAssign(Context context, AST operand1, AST operand2)
        : base(context, operand1, operand2, JSToken.Plus){
        this.binOp = new Plus(context, operand1, operand2);
        this.metaData = null;
      }
    
      internal override Object Evaluate(){
        Object v1 = this.operand1.Evaluate();
        Object v2 = this.operand2.Evaluate();
        Object result = this.binOp.EvaluatePlus(v1, v2);
        try{
          this.operand1.SetValue(result);
          return result;
        }catch(JScriptException e){
          if (e.context == null)
            e.context = this.context;
          throw e;
        }catch(Exception e){
          throw new JScriptException(e, this.context);
        }catch{
          throw new JScriptException(JSError.NonClsException, this.context);
        }
      }
      
      internal override IReflect InferType(JSField inference_target){
        Debug.Assert(Globals.TypeRefs.InReferenceContext(this.type1));
        Debug.Assert(Globals.TypeRefs.InReferenceContext(this.type2));
        MethodInfo oper;
        if (this.type1 == null || inference_target != null){
          oper = this.GetOperator(this.operand1.InferType(inference_target), this.operand2.InferType(inference_target));
        }else
          oper = this.GetOperator(this.type1, this.type2);
        if (oper != null){
          this.metaData = oper;
          return oper.ReturnType;
        }
        if (this.type1 == Typeob.String || this.type2 == Typeob.String)
          return Typeob.String;
        if (Convert.IsPrimitiveNumericType(this.type1))
          if (Convert.IsPromotableTo(this.type2, this.type1) || ((this.operand2 is ConstantWrapper) && ((ConstantWrapper)this.operand2).IsAssignableTo(this.type1)))
            return this.type1;
          else if (Convert.IsPrimitiveNumericType(this.type1) && Convert.IsPrimitiveNumericTypeFitForDouble(this.type2))
            return Typeob.Double;
        return Typeob.Object;
      }
    
      internal override AST PartiallyEvaluate(){
        this.operand1 = this.operand1.PartiallyEvaluateAsReference();
        this.operand2 = this.operand2.PartiallyEvaluate();
        this.binOp = new Plus(this.context, this.operand1, this.operand2);
        this.operand1.SetPartialValue(this.binOp);
        if (this.Engine.doFast){
          Binding b = this.operand1 as Binding;
          if (b != null && b.member is JSVariableField){
            TypeExpression te = ((JSVariableField)b.member).type;
            if (te != null && te.InferType(null) == Typeob.String)
              this.operand1.context.HandleError(JSError.StringConcatIsSlow);
          }
        }
        return this;
      }
      
      private void TranslateToILForNoOverloadCase(ILGenerator il, Type rtype){
        Type lhtype = Convert.ToType(this.operand1.InferType(null));
        Type rhtype = Convert.ToType(this.operand2.InferType(null));
        Type rt = Typeob.Object;
        if (lhtype == Typeob.String || rhtype == Typeob.String)
          rt = Typeob.String;
        else if (rtype == Typeob.Void || rtype == lhtype || Convert.IsPrimitiveNumericType(lhtype) && 
            (Convert.IsPromotableTo(rhtype, lhtype) || ((this.operand2 is ConstantWrapper) && ((ConstantWrapper)this.operand2).IsAssignableTo(lhtype))))
          rt = lhtype;
        if (rt == Typeob.SByte || rt == Typeob.Int16)
          rt = Typeob.Int32;
        else if (rt == Typeob.Byte || rt == Typeob.UInt16)
          rt = Typeob.UInt32;

        // If we have "unsigned += signed" or "signed += unsigned" then generating the
        // correct code gets quite complicated.  Just go late-bound for this edge case.
        if (this.operand2 is ConstantWrapper){
          if (!((ConstantWrapper)this.operand2).IsAssignableTo(rt)){
            // eg: "var u : byte = 123; u += -100;" should go late bound because
            // of signed/unsigned mismatch but "u += 1" should not.
            rt = Typeob.Object;
          }
        }else{
          if ((Convert.IsPrimitiveSignedNumericType(rhtype) && Convert.IsPrimitiveUnsignedIntegerType(lhtype)) ||
              (Convert.IsPrimitiveUnsignedIntegerType(rhtype) && Convert.IsPrimitiveSignedIntegerType(lhtype)))
            rt = Typeob.Object;
        }
            
        this.operand1.TranslateToILPreSetPlusGet(il);
        Convert.Emit(this, il, lhtype, rt);
        this.operand2.TranslateToIL(il, rt);
        if (rt == Typeob.Object || rt == Typeob.String){
          il.Emit(OpCodes.Call, CompilerGlobals.plusDoOpMethod);
          rt = Typeob.Object;
        }else if (rt == Typeob.Double || rt == Typeob.Single)
          il.Emit(OpCodes.Add);
        else if (rt == Typeob.Int32 || rt == Typeob.Int64 || rt == Typeob.Int16 || rt == Typeob.SByte)
          il.Emit(OpCodes.Add_Ovf);
        else
          il.Emit(OpCodes.Add_Ovf_Un);
        if (rtype != Typeob.Void){
          LocalBuilder result = il.DeclareLocal(rt);
          il.Emit(OpCodes.Dup);
          il.Emit(OpCodes.Stloc, result);
          Convert.Emit(this, il, rt, lhtype);
          this.operand1.TranslateToILSet(il);
          il.Emit(OpCodes.Ldloc, result);
          Convert.Emit(this, il, rt, rtype);
        }else{
          Convert.Emit(this, il, rt, lhtype);
          this.operand1.TranslateToILSet(il);
        }
      }
         
      internal override void TranslateToIL(ILGenerator il, Type rtype){
        if (this.metaData == null){
          TranslateToILForNoOverloadCase(il, rtype);
          return;
        }
        if (this.metaData is MethodInfo){
          Object result = null;
          MethodInfo oper = (MethodInfo)this.metaData;
          Type type = Convert.ToType(this.operand1.InferType(null));
          ParameterInfo[] pars = oper.GetParameters();
          this.operand1.TranslateToILPreSetPlusGet(il);
          Convert.Emit(this, il, type, pars[0].ParameterType);
          this.operand2.TranslateToIL(il, pars[1].ParameterType);
          il.Emit(OpCodes.Call, oper);
          if (rtype != Typeob.Void){
            result = il.DeclareLocal(rtype);
            il.Emit(OpCodes.Dup);
            Convert.Emit(this, il, type, rtype);
            il.Emit(OpCodes.Stloc, (LocalBuilder)result);
          }
          Convert.Emit(this, il, oper.ReturnType, type);
          this.operand1.TranslateToILSet(il);
          if (rtype != Typeob.Void)
            il.Emit(OpCodes.Ldloc, (LocalBuilder)result);
        }else{
          //Getting here is just too bad. We do not know until the code runs whether or not to call an overloaded operator method.
          //Compile operands to objects and devolve the decision making to run time thunks 
          Type type = Convert.ToType(this.operand1.InferType(null));
          LocalBuilder result = il.DeclareLocal(Typeob.Object);
          this.operand1.TranslateToILPreSetPlusGet(il);
          Convert.Emit(this, il, type, Typeob.Object);
          il.Emit(OpCodes.Stloc, result);
          il.Emit(OpCodes.Ldloc, (LocalBuilder)this.metaData);
          il.Emit(OpCodes.Ldloc, result);
          this.operand2.TranslateToIL(il, Typeob.Object);
          il.Emit(OpCodes.Call, CompilerGlobals.evaluatePlusMethod);
          if (rtype != Typeob.Void){
            il.Emit(OpCodes.Dup);
            il.Emit(OpCodes.Stloc, result);
          }
          Convert.Emit(this, il, Typeob.Object, type);
          this.operand1.TranslateToILSet(il);
          if (rtype != Typeob.Void){
            il.Emit(OpCodes.Ldloc, result);
            Convert.Emit(this, il, Typeob.Object, rtype);
          }
        }
      }
      
      internal override void TranslateToILInitializer(ILGenerator il){
        IReflect rtype = this.InferType(null);
        this.operand1.TranslateToILInitializer(il);
        this.operand2.TranslateToILInitializer(il);
        if (rtype != Typeob.Object)
          return;
        this.metaData = il.DeclareLocal(Typeob.Plus);
        il.Emit(OpCodes.Newobj, CompilerGlobals.plusConstructor);
        il.Emit(OpCodes.Stloc, (LocalBuilder)this.metaData);
      }
    }
  
}
