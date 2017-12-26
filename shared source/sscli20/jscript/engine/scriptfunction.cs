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
    using System.Globalization;
    using System.Diagnostics;

    public abstract class ScriptFunction : JSObject{
      protected int ilength;
      internal String name;
      
      internal Object proto;
    
      internal ScriptFunction(ScriptObject parent)
        : base(parent) {
        this.ilength = 0;
        this.name = "Function.prototype";
        this.proto = Missing.Value;
      }
      
      protected ScriptFunction(ScriptObject parent, String name)
        : base(parent, typeof(ScriptFunction)) {
        this.ilength = 0;
        this.name = name;
        this.proto = new JSPrototypeObject(parent.GetParent(), this);
      }
      
      internal ScriptFunction(ScriptObject parent, String name, int length)
        : base(parent) {
        this.ilength = length;
        this.name = name;
        this.proto = new JSPrototypeObject(parent.GetParent(), this);
      }
      
#if !DEBUG
      [DebuggerStepThroughAttribute]
      [DebuggerHiddenAttribute]
#endif      
      internal abstract Object Call(Object[] args, Object thisob);
      
      internal virtual Object Call(Object[] args, Object thisob, Binder binder, CultureInfo culture){
        return this.Call(args, thisob);
      }
      
      internal virtual Object Call(Object[] args, Object thisob, ScriptObject enclosing_scope, Closure calleeClosure, Binder binder, CultureInfo culture){
        return this.Call(args, thisob);
      }

#if !DEBUG
      [DebuggerStepThroughAttribute]
      [DebuggerHiddenAttribute]
#endif      
      internal virtual Object Construct(Object[] args){
        JSObject newob = new JSObject(null, false);
        newob.SetParent(this.GetPrototypeForConstructedObject());
        Object result = this.Call(args, newob);
        if (result is ScriptObject || (this is BuiltinFunction && ((BuiltinFunction)this).method.Name.Equals("CreateInstance")))
          return result;
        return newob;
      }
      
#if !DEBUG
      [DebuggerStepThroughAttribute]
      [DebuggerHiddenAttribute]
#endif      
      [JSFunctionAttribute(JSFunctionAttributeEnum.HasVarArgs)]
      public Object CreateInstance(params Object[] args){
        return this.Construct(args);
      }
      
      internal override String GetClassName(){
        return "Function";
      }
      
      internal virtual int GetNumberOfFormalParameters(){
        return this.ilength;
      }

      protected ScriptObject GetPrototypeForConstructedObject(){
        Object ob = this.proto;
        if (ob is JSObject)
          return (JSObject)ob;
        else if (ob is ClassScope)
          return (ClassScope)ob;
        else
          return (ObjectPrototype)(this.GetParent().GetParent()); //The prototype of function is FunctionPrototype, its Prototype is ObjectPrototype
      }
      
      internal virtual bool HasInstance(Object ob){
        if (!(ob is JSObject))
          return false;
        Object proto = this.proto;
        if (!(proto is ScriptObject))
          throw new JScriptException(JSError.InvalidPrototype);
        ScriptObject parent = ((JSObject)ob).GetParent();
        ScriptObject prot = (ScriptObject)proto;
        while (parent != null){
          if (parent == prot)
            return true;
          else if (parent is WithObject){
            Object wob = ((WithObject)parent).contained_object;
            if (wob == prot && wob is ClassScope) return true;
          }
          parent = parent.GetParent();
        }
        return false;  
      }
      
#if !DEBUG
      [DebuggerStepThroughAttribute]
      [DebuggerHiddenAttribute]
#endif      
      [JSFunctionAttribute(JSFunctionAttributeEnum.HasThisObject|JSFunctionAttributeEnum.HasVarArgs)]
      public Object Invoke(Object thisob, params Object[] args){
        return this.Call(args, thisob);
      }
      
#if !DEBUG
      [DebuggerStepThroughAttribute]
      [DebuggerHiddenAttribute]
#endif      
      public override Object InvokeMember(String name, BindingFlags invokeAttr, Binder binder, Object target, Object[] args,
      ParameterModifier[] modifiers, CultureInfo culture, String[] namedParameters){
        if (target != this)
          throw new TargetException();
        
        String thisName = "this";

        if (name == null || name == String.Empty){
          if ((invokeAttr & BindingFlags.CreateInstance) != 0){
            if ((invokeAttr & (BindingFlags.InvokeMethod|BindingFlags.GetField|BindingFlags.GetProperty|
                  BindingFlags.SetField|BindingFlags.SetProperty|BindingFlags.PutDispProperty)) != 0)
              throw new ArgumentException();
            return this.Construct(args);
          }
          
          if ((invokeAttr & BindingFlags.InvokeMethod) != 0){
            //Get the this object
            Object thisob = null;
            if (namedParameters != null){
              int i = Array.IndexOf(namedParameters, thisName);
              if (i == 0){
                thisob = args[0];
                int n = args.Length-1;
                Object[] args2 = new Object[n];
                ArrayObject.Copy(args, 1, args2, 0, n);
                args = args2;
              }
              if (i != 0 || namedParameters.Length != 1)
                throw new ArgumentException();
            }
            
            //Prefer call only if there GetXXX is not set, or there is one or more args.
            if (args.Length > 0 || (invokeAttr & (BindingFlags.GetField|BindingFlags.GetProperty)) == 0)
              return this.Call(args, thisob, binder, culture);
          }
        }
        return base.InvokeMember(name, invokeAttr, binder, target, args, modifiers, culture, namedParameters);
      }
      
      public virtual int length{
        get{return this.ilength;}
        set{}
      }
    
      public override String ToString(){
        return "function "+this.name+"() {\n    [native code]\n}";
      }
      
      public Object prototype{
        get{return this.proto;}
        set{
          if (!this.noExpando)
            this.proto = value;
        }
      }
    }
}
