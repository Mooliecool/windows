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
    
    using Microsoft.JScript.Vsa;
    using System;
    using System.Reflection;
    using System.Reflection.Emit;
    
    public sealed class ArrayLiteral : AST{
      internal ASTList elements;
      
      public ArrayLiteral(Context context, ASTList elements)
        : base(context) {
        this.elements = elements;
      }
      
      internal bool AssignmentCompatible(IReflect lhir, bool reportError){
        if (lhir == Typeob.Object || lhir == Typeob.Array || lhir is ArrayObject) return true;
        IReflect target_element_ir;
        if (lhir == Typeob.Array)
          target_element_ir = Typeob.Object;
        else if (lhir is TypedArray){
          TypedArray tArr = ((TypedArray)lhir);
          if (tArr.rank != 1){
            this.context.HandleError(JSError.TypeMismatch, reportError);
            return false;
          }
          target_element_ir = tArr.elementType;
        }else if (lhir is Type && ((Type)lhir).IsArray){
          Type t = ((Type)lhir);
          if (t.GetArrayRank() != 1){
            this.context.HandleError(JSError.TypeMismatch, reportError);
            return false;
          }
          target_element_ir = t.GetElementType();
        }else
          return false;
        for (int i = 0, n = this.elements.count; i < n; i++)
          if (!Binding.AssignmentCompatible(target_element_ir, this.elements[i], this.elements[i].InferType(null), reportError))
            return false;
        return true;
      }
      
      internal override void CheckIfOKToUseInSuperConstructorCall(){
        for (int i = 0, n = this.elements.count; i < n; i++)
          this.elements[i].CheckIfOKToUseInSuperConstructorCall();
      }
      
      internal override Object Evaluate(){
        if( VsaEngine.executeForJSEE )
          throw new JScriptException(JSError.NonSupportedInDebugger);
        int n = this.elements.count;
        Object[] elems = new Object[n];
        for (int i = 0; i < n; i++)
          elems[i] = this.elements[i].Evaluate();
        return this.Engine.GetOriginalArrayConstructor().ConstructArray(elems);
      }
      
      internal bool IsOkToUseInCustomAttribute(){
        int n = this.elements.count;
        for (int i = 0; i < n; i++){
          Object elem = this.elements[i];
          if (!(elem is ConstantWrapper)) return false;
          if (CustomAttribute.TypeOfArgument(((ConstantWrapper)elem).Evaluate()) == null) return false;
        }
        return true;
      }
    
      internal override AST PartiallyEvaluate(){
        int n = this.elements.count;
        for (int i = 0; i < n; i++)
          this.elements[i] = this.elements[i].PartiallyEvaluate();
        return this;
      }
      
      internal override IReflect InferType(JSField inference_target){
        return Typeob.ArrayObject;
      }
    
      internal override void TranslateToIL(ILGenerator il, Type rtype){
        if (rtype == Typeob.Array){
          this.TranslateToILArray(il, Typeob.Object);
          return;
        }
        if (rtype.IsArray && rtype.GetArrayRank() == 1){
          this.TranslateToILArray(il, rtype.GetElementType());
          return;
        }
        int n = this.elements.count;
        MethodInfo constructorMethod = null;
        if (this.Engine.Globals.globalObject is LenientGlobalObject){
          this.EmitILToLoadEngine(il);
          il.Emit(OpCodes.Call, CompilerGlobals.getOriginalArrayConstructorMethod);
          constructorMethod = CompilerGlobals.constructArrayMethod;
        }else
          constructorMethod = CompilerGlobals.fastConstructArrayLiteralMethod;
        ConstantWrapper.TranslateToILInt(il, n);
        il.Emit(OpCodes.Newarr, Typeob.Object);
        for (int i = 0; i < n; i++){
          il.Emit(OpCodes.Dup);
          ConstantWrapper.TranslateToILInt(il, i);
          this.elements[i].TranslateToIL(il, Typeob.Object);
          il.Emit(OpCodes.Stelem_Ref);
        }
        il.Emit(OpCodes.Call, constructorMethod);
        Convert.Emit(this, il,  Typeob.ArrayObject, rtype);
      }
       
      private void TranslateToILArray(ILGenerator il, Type etype){
        int n = this.elements.count;
        ConstantWrapper.TranslateToILInt(il, n);
        TypeCode ecode = Type.GetTypeCode(etype);
        il.Emit(OpCodes.Newarr, etype);
        for (int i = 0; i < n; i++){
          il.Emit(OpCodes.Dup);
          ConstantWrapper.TranslateToILInt(il, i);
          if (etype.IsValueType && !etype.IsPrimitive)
            il.Emit(OpCodes.Ldelema, etype);
          this.elements[i].TranslateToIL(il, etype);
          Binding.TranslateToStelem(il, etype);
        }
      }
      
      internal override void TranslateToILInitializer(ILGenerator il){
        for (int i = 0, n = this.elements.count; i < n; i++)
          this.elements[i].TranslateToILInitializer(il);
      }
    }
}
