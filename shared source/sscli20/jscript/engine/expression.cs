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
    
    internal sealed class Expression : AST{
      internal AST operand;
      private Completion completion;
      
      internal Expression(Context context, AST operand)
        : base(context) {
        this.operand = operand;
        this.completion = new Completion();
      }
    
      internal override Object Evaluate(){
        this.completion.value = this.operand.Evaluate();
        return this.completion;
      }
      
      internal override AST PartiallyEvaluate(){
        this.operand = this.operand.PartiallyEvaluate();
        if (this.operand is ConstantWrapper)
          this.operand.context.HandleError(JSError.UselessExpression);
        else if (this.operand is Binding)
          ((Binding)this.operand).CheckIfUseless();
        return this;
      }
      
      internal override void TranslateToIL(ILGenerator il, Type rtype){
        this.context.EmitLineInfo(il); 
        this.operand.TranslateToIL(il, rtype);
      }
      
      internal override void TranslateToILInitializer(ILGenerator il){
        this.operand.TranslateToILInitializer(il);
      }
      
    }
 }
