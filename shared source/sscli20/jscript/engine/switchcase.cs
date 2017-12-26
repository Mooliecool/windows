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
    
    internal sealed class SwitchCase : AST{
      private AST case_value;
      private AST statements;
      private Completion completion;
      
      internal SwitchCase(Context context, AST statements)
        : this(context, null, statements){
      }
    
      internal SwitchCase(Context context, AST case_value, AST statements)
        : base(context) {
        this.case_value = case_value;
        this.statements = statements;
        this.completion = new Completion();
      }
    
      internal override Object Evaluate(){
        return this.statements.Evaluate();
      }
      
      internal Completion Evaluate(Object expression){
        Debug.PreCondition(this.case_value != null);
        if (StrictEquality.JScriptStrictEquals(this.case_value.Evaluate(), expression))
          return (Completion)this.statements.Evaluate();
        else
          return null;
      }
      
      internal bool IsDefault(){
        return case_value == null;
      }
      
      internal override AST PartiallyEvaluate(){
        if (this.case_value != null)
          this.case_value = this.case_value.PartiallyEvaluate();
        this.statements = this.statements.PartiallyEvaluate();
        return this;
      }
      
      internal void TranslateToConditionalBranch(ILGenerator il, Type etype, bool branchIfTrue, Label label, bool shortForm){
        Debug.PreCondition(this.case_value != null);
        Type t1 = etype;
        Type t2 = Convert.ToType(this.case_value.InferType(null));
        if (t1 != t2 && t1.IsPrimitive && t2.IsPrimitive){
          if (t1 == Typeob.Single && t2 == Typeob.Double)
            t2 = Typeob.Single;
          else if (Convert.IsPromotableTo(t2, t1))
            t2 = t1;
          else if (Convert.IsPromotableTo(t1, t2))
            t1 = t2;
        }
        bool nonPrimitive = true;
        if (t1 == t2 && t1 != Typeob.Object){
          Convert.Emit(this, il, etype, t1);
          if (!t1.IsPrimitive && t1.IsValueType)
            il.Emit(OpCodes.Box, t1);
          this.case_value.context.EmitLineInfo(il);
          this.case_value.TranslateToIL(il, t1);
          if (t1 == Typeob.String)
            il.Emit(OpCodes.Call, CompilerGlobals.stringEqualsMethod);
          else if (!t1.IsPrimitive){
            if (t1.IsValueType)
              il.Emit(OpCodes.Box, t1);
            il.Emit(OpCodes.Callvirt, CompilerGlobals.equalsMethod);
          }else
            nonPrimitive = false;
        }else{
          Convert.Emit(this, il, etype, Typeob.Object); 
          this.case_value.context.EmitLineInfo(il);
          this.case_value.TranslateToIL(il, Typeob.Object);
          il.Emit(OpCodes.Call, CompilerGlobals.jScriptStrictEqualsMethod);
        }
        if (branchIfTrue){
          if (nonPrimitive)
            il.Emit(shortForm ? OpCodes.Brtrue_S : OpCodes.Brtrue, label);
          else
            il.Emit(shortForm ? OpCodes.Beq_S : OpCodes.Beq, label);
        }else{
          if (nonPrimitive)
            il.Emit(shortForm ? OpCodes.Brfalse_S : OpCodes.Brfalse, label);
          else
            il.Emit(shortForm ? OpCodes.Bne_Un_S : OpCodes.Bne_Un, label);
        }
      }
    
      internal override void TranslateToIL(ILGenerator il, Type rtype){
        //This assumes that rtype == Void.class
        this.statements.TranslateToIL(il, Typeob.Void);
      }
      
      internal override void TranslateToILInitializer(ILGenerator il){
        if (this.case_value != null)
          this.case_value.TranslateToILInitializer(il);
        this.statements.TranslateToILInitializer(il);
      }
    }
}
