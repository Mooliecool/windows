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
    using System.Collections;
    using System.Reflection;
    using System.Reflection.Emit;
    
    public sealed class ASTList : AST{
      internal int count;
      private AST[] list;
      private Object[] array;
    
      internal ASTList(Context context)
        : base(context) {
        this.count = 0;
        this.list = new AST[16];
        this.array = null;
      }
    
      internal ASTList Append(AST elem){
        int n = this.count++;
        if (this.list.Length == n) this.Grow();
        this.list[n] = elem;
        this.context.UpdateWith(elem.context);
        return this;
      }
       
      internal override Object Evaluate(){
        return this.EvaluateAsArray();
      }
      
      internal Object[] EvaluateAsArray(){
        int n = this.count;
        Object[] array = this.array;
        if (array == null)
          this.array = array = new Object[n];
        AST[] list = this.list;
        for (int i = 0; i < n; i++)
          array[i] = list[i].Evaluate();
        return array;
      }
      
      private void Grow(){
        AST[] list = this.list;
        int n = list.Length;
        AST[] newList = this.list = new AST[n+16];
        for (int i = 0; i < n; i++)
          newList[i] = list[i];
      }
      
      internal override AST PartiallyEvaluate(){
        AST[] list = this.list;
        for (int i = 0, n = this.count; i < n; i++)
          list[i] = list[i].PartiallyEvaluate();
        return this;
      }
      
      internal AST this[int i]{
        get{
          return this.list[i];
        }
        set{
          this.list[i] = value;
        }
      }
      
      internal override void TranslateToIL(ILGenerator il, Type rtype){
        Debug.Assert(rtype.IsArray);
        Type etype = rtype.GetElementType();
        int n = this.count;
        ConstantWrapper.TranslateToILInt(il, n);
        il.Emit(OpCodes.Newarr, etype);
        bool doLdelema = etype.IsValueType && !etype.IsPrimitive;
        AST[] list = this.list;
        for (int i = 0; i < n; i++){
          il.Emit(OpCodes.Dup);
          ConstantWrapper.TranslateToILInt(il, i);
          list[i].TranslateToIL(il, etype);
          if (doLdelema)
            il.Emit(OpCodes.Ldelema, etype);
          Binding.TranslateToStelem(il, etype);
        }
      }
      
      internal override void TranslateToILInitializer(ILGenerator il){
        AST[] list = this.list;
        for (int i = 0, n = this.count; i < n; i++)
          list[i].TranslateToILInitializer(il);
      }
    }

}
