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
    using System.Diagnostics;
    using System.Globalization;
    using System.Reflection;
    using System.Security.Permissions;

    public sealed class Closure : ScriptFunction{
      internal FunctionObject func;
      private ScriptObject enclosing_scope;
      private Object declaringObject;
      
      public Object arguments;
      public Object caller;
      
      public Closure(FunctionObject func)
        : this(func, null){
        if (func.enclosing_scope is StackFrame)
          this.enclosing_scope = func.enclosing_scope;
      }
      
      internal Closure(FunctionObject func, Object declaringObject)
        : base(func.GetParent(), func.name, func.GetNumberOfFormalParameters()) {
        this.func = func;
        this.engine = func.engine;
        this.proto = new JSPrototypeObject(((ScriptObject)func.proto).GetParent(), this);
        this.enclosing_scope = this.engine.ScriptObjectStackTop();
        this.arguments = DBNull.Value;
        this.caller = DBNull.Value;
        this.declaringObject = declaringObject;
        this.noExpando = func.noExpando;
        if (func.isExpandoMethod){
          StackFrame sf = new StackFrame(new WithObject(this.enclosing_scope, declaringObject), new JSLocalField[0], new Object[0], null);
          this.enclosing_scope = sf;
          sf.closureInstance = declaringObject;
        }
      }

#if !DEBUG
      [DebuggerStepThroughAttribute]
      [DebuggerHiddenAttribute]
#endif      
      internal override Object Call(Object[] args, Object thisob){
        return this.Call(args, thisob, JSBinder.ob, null);
      }
      
#if !DEBUG
      [DebuggerStepThroughAttribute]
      [DebuggerHiddenAttribute]
#endif      
      internal override Object Call(Object[] args, Object thisob, Binder binder, CultureInfo culture){
        if (this.func.isExpandoMethod)
          ((StackFrame)this.enclosing_scope).thisObject = thisob;
        else if (this.declaringObject != null && !(this.declaringObject is ClassScope))
          thisob = declaringObject;
        if (thisob == null)
          thisob = ((IActivationObject)this.engine.ScriptObjectStackTop()).GetDefaultThisObject();
        if (this.enclosing_scope is ClassScope && this.declaringObject == null){
          if (thisob is StackFrame)
            thisob = ((StackFrame)thisob).closureInstance;
          if (!((FunctionObject)this.func).isStatic && !((ClassScope)(this.enclosing_scope)).HasInstance(thisob))
            throw new JScriptException(JSError.InvalidCall);
        }
        return this.func.Call(args, thisob, this.enclosing_scope, this, binder, culture);
      }
      
      [PermissionSet(SecurityAction.Demand, Name="FullTrust")]
      internal Delegate ConvertToDelegate(Type delegateType){
        return Delegate.CreateDelegate(delegateType, this.declaringObject, this.func.name);
      }
      
      public override String ToString(){
        return this.func.ToString();
      }
    }
}
