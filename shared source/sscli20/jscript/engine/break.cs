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
    
    internal sealed class Break : AST{
      private Completion completion;
      private bool leavesFinally;
        
      internal Break(Context context, int count, bool leavesFinally)
        : base(context) {
        this.completion = new Completion();
        this.completion.Exit = count;
        this.leavesFinally = leavesFinally;
      }
    
      internal override Object Evaluate(){
        return this.completion;
      }
      
      internal override AST PartiallyEvaluate(){
        if (this.leavesFinally)
          this.context.HandleError(JSError.BadWayToLeaveFinally);
        return this;
      }
      
      internal override void TranslateToIL(ILGenerator il, Type rtype){
        Label break_label = (Label)compilerGlobals.BreakLabelStack.Peek(this.completion.Exit-1);
        this.context.EmitLineInfo(il);
        if (this.leavesFinally){
          ConstantWrapper.TranslateToILInt(il, compilerGlobals.BreakLabelStack.Size()-this.completion.Exit);
          il.Emit(OpCodes.Newobj, CompilerGlobals.breakOutOfFinallyConstructor);
          il.Emit(OpCodes.Throw);
        }else if (compilerGlobals.InsideProtectedRegion)
          il.Emit(OpCodes.Leave, break_label);
        else
          il.Emit(OpCodes.Br, break_label);
        //Do not leave anything on the stack, even if rtype != Void.class
      }
      
      internal override void TranslateToILInitializer(ILGenerator il){
      }
    }
}
