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
    
    internal sealed class Comma : BinaryOp{
    
      internal Comma(Context context, AST operand1, AST operand2)
        : base(context, operand1, operand2) {
      }
    
      internal override Object Evaluate(){
        this.operand1.Evaluate();
        return this.operand2.Evaluate();
      }
      
      internal override IReflect InferType(JSField inference_target){
        return this.operand2.InferType(inference_target);
      }
    
      internal override void TranslateToIL(ILGenerator il, Type rtype){
        this.operand1.TranslateToIL(il, Typeob.Void);
        this.operand2.TranslateToIL(il, rtype);
      }
      
    }
}
