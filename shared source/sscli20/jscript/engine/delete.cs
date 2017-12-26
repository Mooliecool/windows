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
    
    internal sealed class Delete : UnaryOp{
    
      internal Delete(Context context, AST operand)
        : base(context, operand) {
      }
    
      internal override void CheckIfOKToUseInSuperConstructorCall(){
        this.context.HandleError(JSError.NotAllowedInSuperConstructorCall);
      }
      
      internal override Object Evaluate(){
        try{
          return this.operand.Delete();
        }catch(JScriptException){
          return true; //This is according to spec, but not backwards compatible with JS5. The latter throws runtime errors.
        }
      }
      
      internal override IReflect InferType(JSField inference_target){
        return Typeob.Boolean;
      }
    
      internal override AST PartiallyEvaluate(){
        this.operand = this.operand.PartiallyEvaluate();
        if (this.operand is Binding)
          ((Binding)this.operand).CheckIfDeletable();
        else if (this.operand is Call)
          ((Call)this.operand).MakeDeletable();
        else
          this.operand.context.HandleError(JSError.NotDeletable);
        return this;
      }
      
      internal override void TranslateToIL(ILGenerator il, Type rtype){
        this.operand.TranslateToILDelete(il, rtype);
      }  
    }
}
