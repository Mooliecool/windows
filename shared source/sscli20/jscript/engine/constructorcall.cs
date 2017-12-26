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
    
    internal class ConstructorCall : AST{
      internal bool isOK;
      internal bool isSuperConstructorCall;
      internal ASTList arguments;
        
      internal ConstructorCall(Context context, ASTList arguments, bool isSuperConstructorCall)
        : base(context) {
        this.isOK = false;
        this.isSuperConstructorCall = isSuperConstructorCall;
        if (arguments == null)
          this.arguments = new ASTList(context);
        else
          this.arguments = arguments;
      }
    
      internal override Object Evaluate(){
        return new Completion(); //do nothing. The enclosing function will call the constructor
      }
      
      internal override AST PartiallyEvaluate(){
        if (!this.isOK){
          this.context.HandleError(JSError.NotOKToCallSuper);
          return this;
        }
        for (int i = 0, n = this.arguments.count; i < n; i++){
          this.arguments[i] = this.arguments[i].PartiallyEvaluate();
          this.arguments[i].CheckIfOKToUseInSuperConstructorCall();
        }
        ScriptObject scope = Globals.ScopeStack.Peek();
        if (!(scope is FunctionScope)){
          this.context.HandleError(JSError.NotOKToCallSuper);            
          return this;
        }
        if (!((FunctionScope)scope).owner.isConstructor)
          this.context.HandleError(JSError.NotOKToCallSuper);            
        ((FunctionScope)scope).owner.superConstructorCall = this;
        return this;
      }
      
      internal override AST PartiallyEvaluateAsReference(){
        throw new JScriptException(JSError.InternalError);
      }
      
      internal override void TranslateToIL(ILGenerator il, Type rtype){
        //do nothing. The enclosing function will emit the call to the constructor
      }
    
      internal override void TranslateToILInitializer(ILGenerator il){
        //do nothing. The enclosing function will emit the code to initialize
      }
    
    }
}
