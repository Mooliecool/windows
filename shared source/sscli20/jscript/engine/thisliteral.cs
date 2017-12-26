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
    
    internal sealed class ThisLiteral : AST{
      internal bool isSuper;
      private MethodInfo method;
      
      internal ThisLiteral(Context context, bool isSuper)
        : base(context){
        this.isSuper = isSuper;
        this.method = null;
      }
      
      internal override void CheckIfOKToUseInSuperConstructorCall(){
        this.context.HandleError(JSError.NotAllowedInSuperConstructorCall);
      }
      
      internal override Object Evaluate(){
        ScriptObject top = Globals.ScopeStack.Peek();
        while (top is WithObject || top is BlockScope)
          top = top.GetParent();
        if (top is StackFrame)
          return ((StackFrame)top).thisObject;
        else //Code is either local to function/method/constructor/initializer (StackFrame) or global (GlobalScope)
          return ((GlobalScope)top).thisObject;
      }
         
      internal override IReflect InferType(JSField inference_target){
        Debug.Assert(Globals.TypeRefs.InReferenceContext(this.method));
        if (this.method != null){
          ParameterInfo[] pars = this.method.GetParameters();
          if (pars == null || pars.Length == 0)
            return this.method.ReturnType;
          else
            return pars[0].ParameterType;
        }
        ScriptObject top = Globals.ScopeStack.Peek();
        while (top is WithObject)
          top = top.GetParent();
        if (top is GlobalScope)
          return top;
        else if (top is FunctionScope && ((FunctionScope)top).isMethod){
          ClassScope csc = (ClassScope)((FunctionScope)top).owner.enclosing_scope;
          if (this.isSuper)
            return csc.GetSuperType();
          else
            return csc;
        }
        return Typeob.Object;
      }
    
      internal override AST PartiallyEvaluate(){
        ScriptObject top = Globals.ScopeStack.Peek();
        while (top is WithObject)
          top = top.GetParent();
        bool isStatic = false;
        if (top is FunctionScope)
          isStatic = ((FunctionScope)top).isStatic && ((FunctionScope)top).isMethod;
        else if (top is StackFrame)
          isStatic = ((StackFrame)top).thisObject is Type;
        if (isStatic){
          this.context.HandleError(JSError.NotAccessible);
          return (new Lookup("this", this.context)).PartiallyEvaluate();
        }
        return this;
      }
      
      internal override AST PartiallyEvaluateAsReference(){        
        this.context.HandleError(JSError.CantAssignThis);
        return (new Lookup("this", this.context)).PartiallyEvaluateAsReference();
      }
      
      internal void ResolveAssignmentToDefaultIndexedProperty(ASTList args, IReflect[] argIRs, AST rhvalue){
        IReflect ir = this.InferType(null);
        Type t = ir is Type ? (Type)ir : null;
        if (ir is ClassScope) t = ((ClassScope)ir).GetBakedSuperType();
        MemberInfo[] defaultMembers = JSBinder.GetDefaultMembers(t);
        if (defaultMembers != null && defaultMembers.Length > 0){
          try{
            PropertyInfo prop = JSBinder.SelectProperty(defaultMembers, argIRs); //Returns property getters as well
            if (prop != null){
              this.method = JSProperty.GetSetMethod(prop, true);
              if (this.method == null)
                this.context.HandleError(JSError.AssignmentToReadOnly, true);
              if (!Binding.CheckParameters(prop.GetIndexParameters(), argIRs, args, this.context, 0, false, true))
                this.method = null;
              return;
            }
          }catch(AmbiguousMatchException){
            this.context.HandleError(JSError.AmbiguousMatch);
            return;
          }
        }
        String tname = ir is ClassScope ? ((ClassScope)ir).GetName() : ((Type)ir).Name;
        this.context.HandleError(JSError.NotIndexable, tname);
      }
      
      internal override void ResolveCall(ASTList args, IReflect[] argIRs, bool constructor, bool brackets){
        if (constructor || !brackets){
          if (this.isSuper)
            this.context.HandleError(JSError.IllegalUseOfSuper);
          else
            this.context.HandleError(JSError.IllegalUseOfThis);
          return;
        }
        IReflect ir = this.InferType(null);
        Type t = ir is Type ? (Type)ir : null;
        if (ir is ClassScope) t = ((ClassScope)ir).GetBakedSuperType();
        MemberInfo[] defaultMembers = JSBinder.GetDefaultMembers(t);
        if (defaultMembers != null && defaultMembers.Length > 0){
          try{
            this.method = JSBinder.SelectMethod(defaultMembers, argIRs); //Returns property getters as well
            if (this.method != null){
              if (!Binding.CheckParameters(this.method.GetParameters(), argIRs, args, this.context, 0, false, true))
                this.method = null;
              return;
            }
          }catch(AmbiguousMatchException){
            this.context.HandleError(JSError.AmbiguousMatch);
            return;
          }
        }
        String tname = ir is ClassScope ? ((ClassScope)ir).GetName() : ((Type)ir).Name;
        this.context.HandleError(JSError.NotIndexable, tname);
      }
      
      internal override void TranslateToIL(ILGenerator il, Type rtype){
        if (rtype == Typeob.Void) return;
        IReflect ir = this.InferType(null);
        if (ir is GlobalScope){
          this.EmitILToLoadEngine(il);
          if (rtype == Typeob.LenientGlobalObject)
            il.Emit(OpCodes.Call, CompilerGlobals.getLenientGlobalObjectMethod);
          else{
            il.Emit(OpCodes.Call, CompilerGlobals.scriptObjectStackTopMethod);
            il.Emit(OpCodes.Castclass, Typeob.IActivationObject);
            il.Emit(OpCodes.Callvirt, CompilerGlobals.getDefaultThisObjectMethod);
          }
          return;
        }
        il.Emit(OpCodes.Ldarg_0);
        Convert.Emit(this, il, Convert.ToType(this.InferType(null)), rtype);
      }
      
      internal override void TranslateToILCall(ILGenerator il, Type rtype, ASTList argList, bool construct, bool brackets){
        MethodInfo meth = this.method;
        if (meth != null){
          Type t = meth.ReflectedType;
          if (!meth.IsStatic){
            this.method = null;
            this.TranslateToIL(il, t);
            this.method = meth;
          }
          ParameterInfo[] pars = meth.GetParameters();
          Binding.PlaceArgumentsOnStack(il, pars, argList, 0, 0, Binding.ReflectionMissingCW);
          if (meth.IsVirtual && !meth.IsFinal && (!t.IsSealed || !t.IsValueType))
            il.Emit(OpCodes.Callvirt, meth);
          else
            il.Emit(OpCodes.Call, meth);
          Convert.Emit(this, il, meth.ReturnType, rtype);
        }else
          base.TranslateToILCall(il, rtype, argList, construct, brackets);
      }
      
      internal override void TranslateToILPreSet(ILGenerator il, ASTList argList){
        MethodInfo meth = this.method;
        if (meth != null){
          Type t = meth.ReflectedType;
          if (!meth.IsStatic)
            this.TranslateToIL(il, t);
          Binding.PlaceArgumentsOnStack(il, meth.GetParameters(), argList, 0, 1, Binding.ReflectionMissingCW);
        }else
          base.TranslateToILPreSet(il, argList);
      }
      
      internal override void TranslateToILSet(ILGenerator il, AST rhvalue){
        MethodInfo meth = this.method;
        if (meth != null){
          if (rhvalue != null)
            rhvalue.TranslateToIL(il, meth.GetParameters()[0].ParameterType);
          Type t = meth.ReflectedType;
          if (meth.IsVirtual && !meth.IsFinal && (!t.IsSealed || !t.IsValueType))
            il.Emit(OpCodes.Callvirt, meth);
          else
            il.Emit(OpCodes.Call, meth);
          if (meth.ReturnType != Typeob.Void) //Should never be the case if the property is well formed, but there is no gaurantee
            il.Emit(OpCodes.Pop);
        }else
          base.TranslateToILSet(il, rhvalue);
      }     
      
      internal override void TranslateToILInitializer(ILGenerator il){
      }
      
    }
}
