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
    
    internal sealed class Logical_and : BinaryOp{
      
      internal Logical_and(Context context, AST operand1, AST operand2)
        : base(context, operand1, operand2) {
      }
    
      internal override Object Evaluate(){
        Object v1 = this.operand1.Evaluate();
        MethodInfo opFalse = null;
        Type t1 = null;
        if (v1 != null && !(v1 is IConvertible)){
          t1 = v1.GetType();
          opFalse = t1.GetMethod("op_False", BindingFlags.ExactBinding|BindingFlags.Public|BindingFlags.Static, null, new Type[]{t1}, null);
          if (opFalse == null || (opFalse.Attributes & MethodAttributes.SpecialName) == 0 || opFalse.ReturnType != Typeob.Boolean)
            opFalse = null;
        }
        if (opFalse == null){
          if (!Convert.ToBoolean(v1))
            return v1;
          return this.operand2.Evaluate();
        }
        opFalse = new JSMethodInfo(opFalse);
        if ((bool)opFalse.Invoke(null, BindingFlags.SuppressChangeType, null, new Object[]{v1}, null))
          return v1;
        Object v2 = this.operand2.Evaluate();
        Type t2 = null;
        if (v2 != null && !(v2 is IConvertible)){
          t2 = v2.GetType();
          if (t1 == t2){
            MethodInfo bitwiseAnd = t1.GetMethod("op_BitwiseAnd", BindingFlags.ExactBinding|BindingFlags.Public|BindingFlags.Static, null, new Type[]{t1, t1}, null);
            if (bitwiseAnd != null && (bitwiseAnd.Attributes & MethodAttributes.SpecialName) != 0){
              bitwiseAnd = new JSMethodInfo(bitwiseAnd);
              return bitwiseAnd.Invoke(null, BindingFlags.SuppressChangeType, null, new Object[]{v1, v2}, null);
            }
          }
        }
        return v2;    
      }
    
      internal override IReflect InferType(JSField inference_target){
        IReflect t1 = this.operand1.InferType(inference_target);
        IReflect t2 = this.operand2.InferType(inference_target);
        if (t1 == t2) return t1;
        return Typeob.Object;
      }
    
      internal override void TranslateToConditionalBranch(ILGenerator il, bool branchIfTrue, Label label, bool shortForm){
        Label exit = il.DefineLabel();
        if (branchIfTrue){
          this.operand1.TranslateToConditionalBranch(il, false, exit, shortForm);
          this.operand2.TranslateToConditionalBranch(il, true, label, shortForm);
          il.MarkLabel(exit);
        }else{
          this.operand1.TranslateToConditionalBranch(il, false, label, shortForm);
          this.operand2.TranslateToConditionalBranch(il, false, label, shortForm);
        }
      }
    
      internal override void TranslateToIL(ILGenerator il, Type rtype){
        Type t1 = Convert.ToType(this.operand1.InferType(null));
        Type t2 = Convert.ToType(this.operand2.InferType(null));
        if (t1 != t2) t1 = Typeob.Object;
        MethodInfo opFalse = t1.GetMethod("op_False", BindingFlags.ExactBinding|BindingFlags.Public|BindingFlags.Static, null, new Type[]{t1}, null);
        if (opFalse == null || (opFalse.Attributes & MethodAttributes.SpecialName) == 0 || opFalse.ReturnType != Typeob.Boolean)
          opFalse = null;
        MethodInfo bitwiseAnd = null;
        if (opFalse != null)
          bitwiseAnd = t1.GetMethod("op_BitwiseAnd", BindingFlags.ExactBinding|BindingFlags.Public|BindingFlags.Static, null, new Type[]{t1, t1}, null);
        if (bitwiseAnd == null || (bitwiseAnd.Attributes & MethodAttributes.SpecialName) == 0)
          opFalse = null;
        Label exit = il.DefineLabel();
        this.operand1.TranslateToIL(il, t1);
        il.Emit(OpCodes.Dup);
        if (opFalse != null){
          if (t1.IsValueType)
            Convert.EmitLdloca(il, t1);
          il.Emit(OpCodes.Call, opFalse);
          il.Emit(OpCodes.Brtrue, exit);
          this.operand2.TranslateToIL(il, t1);
          il.Emit(OpCodes.Call, bitwiseAnd);
          il.MarkLabel(exit);
          Convert.Emit(this, il, bitwiseAnd.ReturnType, rtype);
        }else{
          Convert.Emit(this, il, t1, Typeob.Boolean, true);
          il.Emit(OpCodes.Brfalse, exit);
          il.Emit(OpCodes.Pop);
          this.operand2.TranslateToIL(il, t1);
          il.MarkLabel(exit);
          Convert.Emit(this, il, t1, rtype);
        }
      }
      
    }
}
