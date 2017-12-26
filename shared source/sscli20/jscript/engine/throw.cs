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
    
    public sealed class Throw : AST{
      private AST operand;
      
      internal Throw(Context context, AST operand)
        : base(context) {
        this.operand = operand;
      }
    
      internal override Object Evaluate(){
        if (this.operand == null){
          ScriptObject scope = this.Engine.ScriptObjectStackTop();
          while (scope != null){
            BlockScope bscope = scope as BlockScope;
            if (bscope != null && bscope.catchHanderScope){
              throw (Exception)(bscope.GetFields(BindingFlags.Public|BindingFlags.Static)[0].GetValue(null));
            }
          }
        }
        throw JScriptThrow(this.operand.Evaluate());
      }
      
      internal override bool HasReturn(){
        return true;
      }
      
      public static Exception JScriptThrow(Object value){
        if (value is Exception)
          return (Exception)value;
        else if (value is ErrorObject && ((ErrorObject)value).exception is Exception)
          return ((Exception)((ErrorObject)value).exception);
        else
          return new JScriptException(value, null);
      }
      
      internal override AST PartiallyEvaluate(){
        if (this.operand == null){
          BlockScope bscope = null;
          for (ScriptObject scope = this.Engine.ScriptObjectStackTop(); scope != null; scope = scope.GetParent()){
            if (scope is WithObject) continue;
            bscope = scope as BlockScope;
            if (bscope == null) break;
            if (bscope.catchHanderScope) break;
          }
          if (bscope == null){
            this.context.HandleError(JSError.BadThrow);
            this.operand = new ConstantWrapper(null, this.context);
          }
        }else
          this.operand = this.operand.PartiallyEvaluate();
        return this;
      }
      
      internal override void TranslateToIL(ILGenerator il, Type rtype){
        this.context.EmitLineInfo(il);
        if (this.operand == null){
          il.Emit(OpCodes.Rethrow);
          return;
        }
        IReflect ir = this.operand.InferType(null);
        if (ir is Type && Typeob.Exception.IsAssignableFrom((Type)ir))
          this.operand.TranslateToIL(il, (Type)ir);
        else{
          this.operand.TranslateToIL(il, Typeob.Object);
          il.Emit(OpCodes.Call, CompilerGlobals.jScriptThrowMethod);
        }
        il.Emit(OpCodes.Throw);
      }
      
      internal override void TranslateToILInitializer(ILGenerator il){
        if (this.operand != null)
          this.operand.TranslateToILInitializer(il);
      }
     }
  }
