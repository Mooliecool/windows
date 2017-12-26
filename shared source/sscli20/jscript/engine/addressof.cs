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

/* This class records the fact that the operand was preceded by an & operator. */

namespace Microsoft.JScript {
    
    using System;
    using System.Reflection;
    using System.Reflection.Emit;

    internal sealed class AddressOf : UnaryOp{
      
      internal AddressOf(Context context, AST operand)
        : base(context, operand) {
        Debug.Assert(operand is Member || operand is Lookup);
      }
       
      internal override Object Evaluate(){
        return this.operand.Evaluate();
      }

      internal FieldInfo GetField(){
        //Used to detect local variables used as out parameters
        //Such locals are not restored when returning from a call out of a function that must
        //save its stack locals for use by nested functions
        if (!(this.operand is Binding)) return null;
        MemberInfo result = ((Binding)this.operand).member;
        if (result is FieldInfo) return (FieldInfo)result;
        return null;
      }

      internal override IReflect InferType(JSField inference_target){
        return this.operand.InferType(inference_target);
      }
      
      internal override AST PartiallyEvaluate(){
        this.operand = this.operand.PartiallyEvaluate();
        if (!(this.operand is Binding) || !((Binding)this.operand).RefersToMemoryLocation())
          this.context.HandleError(JSError.DoesNotHaveAnAddress);
        return this;
      }
      
      internal override void TranslateToIL(ILGenerator il, Type rtype){
        this.operand.TranslateToIL(il, rtype);
      }
      
      internal override void TranslateToILPreSet(ILGenerator il){
        this.operand.TranslateToILPreSet(il);
      }
    
      internal override Object TranslateToILReference(ILGenerator il, Type rtype){
        return this.operand.TranslateToILReference(il, rtype);
      }
      
      internal override void TranslateToILSet(ILGenerator il, AST rhvalue){
        this.operand.TranslateToILSet(il, rhvalue);
      }
    }
}
      
      
