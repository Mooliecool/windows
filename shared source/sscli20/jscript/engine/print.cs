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
    
    internal sealed class Print : AST{
      private ASTList operand;
      private Completion completion;
      
      internal Print(Context context, AST operand)
        : base(context) {
        this.operand = (ASTList)operand;
        this.completion = new Completion();
      }
      
      internal override Object Evaluate(){
        Object[] values = this.operand.EvaluateAsArray();
        for (int i = 0; i < values.Length-1; i++)
          ScriptStream.Out.Write(Convert.ToString(values[i]));
        if (values.Length > 0){
          this.completion.value = Convert.ToString(values[values.Length-1]);
          ScriptStream.Out.WriteLine(this.completion.value);
        }else{
          ScriptStream.Out.WriteLine("");
          this.completion.value = null;
        }
        return this.completion;
      }
      
      internal override AST PartiallyEvaluate(){
        this.operand = (ASTList)(this.operand.PartiallyEvaluate());
        return this;
      }
      
      internal override void TranslateToIL(ILGenerator il, Type rtype){
        if (this.context.document.debugOn)
          il.Emit(OpCodes.Nop);
        ASTList astList = this.operand;
        int n = astList.count;
        for (int i = 0; i < n; i++){
          AST ast = astList[i];
          IReflect ir = ast.InferType(null);
          if (ir == Typeob.String)
            ast.TranslateToIL(il, Typeob.String);
          else{
            ast.TranslateToIL(il, Typeob.Object);
            ConstantWrapper.TranslateToILInt(il, 1);
            il.Emit(OpCodes.Call, CompilerGlobals.toStringMethod);
          }
          if (i == n-1)
            il.Emit(OpCodes.Call, CompilerGlobals.writeLineMethod);
          else
            il.Emit(OpCodes.Call, CompilerGlobals.writeMethod);
        }
        if (n == 0){
          il.Emit(OpCodes.Ldstr, "");
          il.Emit(OpCodes.Call, CompilerGlobals.writeLineMethod);
        }
        if (rtype != Typeob.Void){
          il.Emit(OpCodes.Ldsfld, CompilerGlobals.undefinedField);
          Convert.Emit(this, il, Typeob.Object, rtype);
        }
      }
      
      internal override void TranslateToILInitializer(ILGenerator il){
        ASTList astList = this.operand;
        for (int i = 0; i < astList.count; i++)
          astList[i].TranslateToILInitializer(il);
      }
      
    }
}
