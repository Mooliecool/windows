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
    
    internal sealed class StaticInitializer : AST{
      private FunctionObject func;
      private Completion completion;
      
      internal StaticInitializer(Context context, Block body, FunctionScope own_scope)
        : base(context) {
        this.func = new FunctionObject(null, new ParameterDeclaration[0], null, body, own_scope, Globals.ScopeStack.Peek(), context, MethodAttributes.Private|MethodAttributes.Static);
        this.func.isMethod = true;
        this.func.hasArgumentsObject = false;
        this.completion = new Completion();
      }
    
      internal override Object Evaluate(){
        this.func.Call(new Object[0], ((IActivationObject)Globals.ScopeStack.Peek()).GetGlobalScope());
        return this.completion;
      }
      
      internal override AST PartiallyEvaluate(){
        this.func.PartiallyEvaluate();
        return this;
      }
      
      internal override void TranslateToIL(ILGenerator il, Type rtype){
        Debug.Assert(rtype == Typeob.Void);
        this.func.TranslateBodyToIL(il, this.compilerGlobals);
      }
      
      internal override void TranslateToILInitializer(ILGenerator il){
        throw new JScriptException(JSError.InternalError, this.context);
      }
    }
}
