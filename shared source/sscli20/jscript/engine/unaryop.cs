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
    
    public abstract class UnaryOp : AST{
      protected AST operand;
      
      internal UnaryOp(Context context, AST operand)
        : base(context){
        this.operand = operand;
      }
      
      internal override void CheckIfOKToUseInSuperConstructorCall(){
        this.operand.CheckIfOKToUseInSuperConstructorCall();
      }
      
      internal override AST PartiallyEvaluate(){
        this.operand = this.operand.PartiallyEvaluate();
        if (this.operand is ConstantWrapper)
          try{
            return new ConstantWrapper(this.Evaluate(), this.context);
          }catch(JScriptException e){
            this.context.HandleError((JSError)(e.ErrorNumber & 0xFFFF));
          }catch{
            this.context.HandleError(JSError.TypeMismatch);
          }
        return this;
      }
      
      internal override void TranslateToILInitializer(ILGenerator il){
        this.operand.TranslateToILInitializer(il);
      }
    }
}
