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

namespace Microsoft.JScript {
    
    using System;
    using System.Reflection;
    using System.Reflection.Emit;
    
    internal sealed class BitwiseBinaryAssign : BinaryOp{
      private BitwiseBinary binOp;
      private Object metaData;
      
      internal BitwiseBinaryAssign(Context context, AST operand1, AST operand2, JSToken operatorTok)
        : base(context, operand1, operand2, operatorTok){
        this.binOp = new BitwiseBinary(context, operand1, operand2, operatorTok);
        this.metaData = null;
      }
      
      internal override Object Evaluate(){
        Object v1 = this.operand1.Evaluate();
        Object v2 = this.operand2.Evaluate();
        Object result = this.binOp.EvaluateBitwiseBinary(v1, v2);
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
        MethodInfo oper;
        if (this.type1 == null){
          oper = this.GetOperator(this.operand1.InferType(inference_target), this.operand2.InferType(inference_target));
        }else
          oper = this.GetOperator(this.type1, this.type2);
        if (oper != null){
          this.metaData = oper;
          return oper.ReturnType;
        }
        if ((this.type1.IsPrimitive || Typeob.JSObject.IsAssignableFrom(this.type1)) 
        &&  (this.type2.IsPrimitive || Typeob.JSObject.IsAssignableFrom(this.type2)))
          return Typeob.Int32;
        else
          return Typeob.Object;
      }
    
      internal override AST PartiallyEvaluate(){
        this.operand1 = this.operand1.PartiallyEvaluateAsReference();
        this.operand2 = this.operand2.PartiallyEvaluate();
        this.binOp = new BitwiseBinary(this.context, this.operand1, this.operand2, this.operatorTok);
        this.operand1.SetPartialValue(this.binOp);
        return this;
      }
      
      private void TranslateToILForNoOverloadCase(ILGenerator il, Type rtype){
        Type lhtype = Convert.ToType(this.operand1.InferType(null));
        Type operand2type = Convert.ToType(this.operand2.InferType(null));
        Type bbrType = BitwiseBinary.ResultType(lhtype, operand2type, this.operatorTok);
        this.operand1.TranslateToILPreSetPlusGet(il);
        Convert.Emit(this, il, lhtype, bbrType, true);
        this.operand2.TranslateToIL(il, operand2type);
        Convert.Emit(this, il, operand2type, BitwiseBinary.Operand2Type(this.operatorTok, bbrType), true);
        switch (this.operatorTok){
          case JSToken.BitwiseAnd:  
            il.Emit(OpCodes.And);
            break;
          case JSToken.BitwiseOr:
            il.Emit(OpCodes.Or);
            break;
          case JSToken.BitwiseXor:
            il.Emit(OpCodes.Xor);
            break;
          case JSToken.LeftShift:
            BitwiseBinary.TranslateToBitCountMask(il, bbrType, this.operand2);
            il.Emit(OpCodes.Shl);
            break;
          case JSToken.RightShift:
            BitwiseBinary.TranslateToBitCountMask(il, bbrType, this.operand2);
            il.Emit(OpCodes.Shr);
            break;
          case JSToken.UnsignedRightShift:  
            BitwiseBinary.TranslateToBitCountMask(il, bbrType, this.operand2);
            il.Emit(OpCodes.Shr_Un);
            break;
          default:
            throw new JScriptException(JSError.InternalError, this.context);
        }
        if (rtype != Typeob.Void){
          LocalBuilder result = il.DeclareLocal(bbrType);
          il.Emit(OpCodes.Dup);
          il.Emit(OpCodes.Stloc, result);
          Convert.Emit(this, il, bbrType, lhtype);
          this.operand1.TranslateToILSet(il);
          il.Emit(OpCodes.Ldloc, result);
          Convert.Emit(this, il, bbrType, rtype);
        }else{
          Convert.Emit(this, il, bbrType, lhtype);
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
          il.Emit(OpCodes.Call, CompilerGlobals.evaluateBitwiseBinaryMethod);
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
        this.metaData = il.DeclareLocal(Typeob.BitwiseBinary);
        ConstantWrapper.TranslateToILInt(il, (int)this.operatorTok);
        il.Emit(OpCodes.Newobj, CompilerGlobals.bitwiseBinaryConstructor);
        il.Emit(OpCodes.Stloc, (LocalBuilder)this.metaData);
      }
    }
  
}
