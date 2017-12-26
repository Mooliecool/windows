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
    using System.Collections;
    
    internal sealed class ObjectLiteral : AST{
      internal AST[] keys;
      internal AST[] values;
      
      internal ObjectLiteral(Context context, ASTList propertyList)
        : base(context) {
        int n = propertyList.count;
        this.keys = new AST[n];
        this.values = new AST[n];
        for (int i = 0; i < n; i++){
          ASTList pair = (ASTList)propertyList[i];
          this.keys[i] = pair[0];
          this.values[i] = pair[1];
        }
      }
      
      internal override void CheckIfOKToUseInSuperConstructorCall(){
        for (int i = 0, n = this.values.Length; i < n; i++)
          this.values[i].CheckIfOKToUseInSuperConstructorCall();
      }
      
      internal override Object Evaluate(){
        JSObject result = this.Engine.GetOriginalObjectConstructor().ConstructObject();
        for (int i = 0, n = this.keys.Length; i < n; i++)
          result.SetMemberValue(this.keys[i].Evaluate().ToString(), this.values[i].Evaluate());
        return result;
      }
      
      internal override AST PartiallyEvaluate(){
        int n = this.keys.Length;
        for (int i = 0; i < n; i++){
          this.keys[i] = this.keys[i].PartiallyEvaluate();
          this.values[i] = this.values[i].PartiallyEvaluate();
        }
        return this;
      }
      
      internal override void TranslateToIL(ILGenerator il, Type rtype){
        int n = this.keys.Length;
        this.EmitILToLoadEngine(il);
        il.Emit(OpCodes.Call, CompilerGlobals.getOriginalObjectConstructorMethod);
        il.Emit(OpCodes.Call, CompilerGlobals.constructObjectMethod);
        for (int i = 0; i < n; i++){
          il.Emit(OpCodes.Dup);
          this.keys[i].TranslateToIL(il, Typeob.String);
          this.values[i].TranslateToIL(il, Typeob.Object);
          il.Emit(OpCodes.Call, CompilerGlobals.setMemberValue2Method);
        }
        Convert.Emit(this, il, Typeob.Object, rtype);
      }
      
      internal override void TranslateToILInitializer(ILGenerator il){
        for (int i = 0, n = this.keys.Length; i < n; i++){
          this.keys[i].TranslateToILInitializer(il);
          this.values[i].TranslateToILInitializer(il);
        }
      }
    }
}
