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
    
    internal sealed class VoidOp : UnaryOp{
    
      internal VoidOp(Context context, AST operand)
        : base(context, operand) {
      }
    
      internal override Object Evaluate(){
        this.operand.Evaluate();
        return null;
      }
      
      internal override IReflect InferType(JSField inference_target){
        return Typeob.Empty;
      }
    
      internal override AST PartiallyEvaluate(){
        return new ConstantWrapper(null, this.context);
      }
      
      internal override void TranslateToIL(ILGenerator il, Type rtype){
        this.operand.TranslateToIL(il, Typeob.Object); //force evaluation
        if (rtype != Typeob.Void){
          il.Emit(OpCodes.Ldsfld, CompilerGlobals.undefinedField);
          Convert.Emit(this, il, Typeob.Object, rtype);
        }else
          il.Emit(OpCodes.Pop);
      }
      
    }
}
