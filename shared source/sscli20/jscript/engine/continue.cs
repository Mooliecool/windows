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
    
    internal sealed class Continue : AST{
      private Completion completion;
      private bool leavesFinally;
        
      internal Continue(Context context, int count, bool leavesFinally)
        : base(context) {
        this.completion = new Completion();
        this.completion.Continue = count;
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
        Label continue_label = (Label)compilerGlobals.ContinueLabelStack.Peek(this.completion.Continue-1);
        this.context.EmitLineInfo(il);
        if (this.leavesFinally){
          ConstantWrapper.TranslateToILInt(il, compilerGlobals.ContinueLabelStack.Size()-this.completion.Continue);
          il.Emit(OpCodes.Newobj, CompilerGlobals.continueOutOfFinallyConstructor);
          il.Emit(OpCodes.Throw);
        }else if (compilerGlobals.InsideProtectedRegion)
          il.Emit(OpCodes.Leave, continue_label);
        else
          il.Emit(OpCodes.Br, continue_label);
        //Do not leave anything on the stack, even if rtype != Void.class
      }
      
      internal override void TranslateToILInitializer(ILGenerator il){
      }
    }
 }
