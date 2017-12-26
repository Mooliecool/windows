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
    
    public sealed class Instanceof : BinaryOp{
      
      internal Instanceof(Context context, AST operand1, AST operand2)
        : base(context, operand1, operand2) {
      }
      
      internal override Object Evaluate(){
        Object v1 = this.operand1.Evaluate();
        Object v2 = this.operand2.Evaluate();
        try{
          return JScriptInstanceof(v1, v2);
        }catch(JScriptException e){
          if (e.context == null)
            e.context = this.operand2.context;
          throw e;
        }
      }
      
      internal override IReflect InferType(JSField inference_target){
        return Typeob.Boolean;
      }
    
      public static bool JScriptInstanceof(Object v1, Object v2){
        if (v2 is ClassScope)
          return ((ClassScope)v2).HasInstance(v1);
        if (v2 is ScriptFunction)
          return ((ScriptFunction)v2).HasInstance(v1);
        if (v1 == null)
          return false;
        if (v2 is Type){
          Type t1 = v1.GetType();
          if (v1 is IConvertible)
            try{
              Convert.CoerceT(v1, (Type)v2);
              return true;
            }catch(JScriptException){
              return false;
            }
          else
            return ((Type)v2).IsAssignableFrom(t1);
        }


        throw new JScriptException(JSError.NeedType);
      }
      
      internal override void TranslateToIL(ILGenerator il, Type rtype){
        this.operand1.TranslateToIL(il, Typeob.Object);
        Object val = null;
        if (this.operand2 is ConstantWrapper && (val = this.operand2.Evaluate()) is Type && !((Type)val).IsValueType){
          il.Emit(OpCodes.Isinst, (Type)val);
          il.Emit(OpCodes.Ldnull);
          il.Emit(OpCodes.Cgt_Un);
        }else if (val is ClassScope){
          il.Emit(OpCodes.Isinst, ((ClassScope)val).GetTypeBuilderOrEnumBuilder());
          il.Emit(OpCodes.Ldnull);
          il.Emit(OpCodes.Cgt_Un);
        }else{
          this.operand2.TranslateToIL(il, Typeob.Object);
          il.Emit(OpCodes.Call, CompilerGlobals.jScriptInstanceofMethod);
        }
        Convert.Emit(this, il, Typeob.Boolean, rtype);
      }
    }
}
