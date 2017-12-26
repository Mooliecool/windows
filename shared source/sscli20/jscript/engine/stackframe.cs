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
    
    /* 
    A stack frame is used to avoid allocating the name lookup hash table until it is actually needed.
    
    Rather than allocating the hash table directly and then duplicating code that already resides in JSObject and FunctionScope,
    a FunctionScope is allocated and populated whenever the hashtable is needed for a name lookup.
    */
    
    using Microsoft.JScript.Vsa;
    using System;
    using System.Reflection;
    using System.Collections;
    using System.Diagnostics;
    
    public sealed class StackFrame : ScriptObject, IActivationObject{
      internal ArgumentsObject caller_arguments; 
      private JSLocalField[] fields;
      public Object[] localVars;
      private FunctionScope nestedFunctionScope;
      internal Object thisObject;
      public Object closureInstance;
      
      internal StackFrame(ScriptObject parent, JSLocalField[] fields, Object[] local_vars, Object thisObject)
        : base(parent) {
        this.caller_arguments = null;
        this.fields = fields;
        this.localVars = local_vars;
        this.nestedFunctionScope = null;
        this.thisObject = thisObject;
        if (parent is StackFrame)
          this.closureInstance = ((StackFrame)parent).closureInstance;
        else if (parent is JSObject)
          this.closureInstance = parent;
        else
          this.closureInstance = null;
      }
      
      internal JSVariableField AddNewField(String name, Object value, FieldAttributes attributeFlags){
        this.AllocateFunctionScope();
        return this.nestedFunctionScope.AddNewField(name, value, attributeFlags);                                                                                                            
      }
        
      private void AllocateFunctionScope(){
        if (this.nestedFunctionScope != null)
          return;
        this.nestedFunctionScope = new FunctionScope(this.parent);
        if (this.fields != null)
          for (int i = 0, n = this.fields.Length; i < n; i++)
            this.nestedFunctionScope.AddOuterScopeField(this.fields[i].Name, this.fields[i]);
      }
      
      public Object GetDefaultThisObject(){
        ScriptObject parent = this.GetParent();
        IActivationObject iob = parent as IActivationObject;
        if (iob != null) return iob.GetDefaultThisObject();
        return parent;
      }

      public FieldInfo GetField(String name, int lexLevel){
        return null;
      }
      
      public GlobalScope GetGlobalScope(){
        return ((IActivationObject)this.GetParent()).GetGlobalScope();
      }

      FieldInfo IActivationObject.GetLocalField(String name){
        this.AllocateFunctionScope();
        return this.nestedFunctionScope.GetLocalField(name);                                                                                                            
      }
      
      public override MemberInfo[] GetMember(String name, BindingFlags bindingAttr){
        this.AllocateFunctionScope();
        return this.nestedFunctionScope.GetMember(name, bindingAttr);                                                                                                            
      }

      public override MemberInfo[] GetMembers(BindingFlags bindingAttr){
        this.AllocateFunctionScope();
        return this.nestedFunctionScope.GetMembers(bindingAttr);                                                                                                            
      }
      
      internal override void GetPropertyEnumerator(ArrayList enums, ArrayList objects){
        throw new JScriptException(JSError.InternalError);
      }
      
#if !DEBUG
      [DebuggerStepThroughAttribute]
      [DebuggerHiddenAttribute]
#endif      
      internal override Object GetMemberValue(String name){
        this.AllocateFunctionScope();
        return this.nestedFunctionScope.GetMemberValue(name);                                                                                                            
      }
      
#if !DEBUG
      [DebuggerStepThroughAttribute]
      [DebuggerHiddenAttribute]
#endif      
      public Object GetMemberValue(String name, int lexlevel){
        if (lexlevel <= 0)
          return Missing.Value;
        if (this.nestedFunctionScope != null)
          return this.nestedFunctionScope.GetMemberValue(name, lexlevel);
        else
          return ((IActivationObject)this.parent).GetMemberValue(name, lexlevel-1);
      }
      
      public static void PushStackFrameForStaticMethod(RuntimeTypeHandle thisclass, JSLocalField[] fields, VsaEngine engine){
        PushStackFrameForMethod(Type.GetTypeFromHandle(thisclass), fields, engine);
      }
      
      public static void PushStackFrameForMethod(Object thisob, JSLocalField[] fields, VsaEngine engine){
        Globals globals = engine.Globals;
        IActivationObject top = (IActivationObject)globals.ScopeStack.Peek();
        String currentNamespace = thisob.GetType().Namespace;
        WithObject wob = null;
        if (currentNamespace != null && currentNamespace.Length > 0){
          wob = new WithObject(top.GetGlobalScope(), new WrappedNamespace(currentNamespace, engine));
          wob.isKnownAtCompileTime = true; //For use by an eval inside this method
          wob = new WithObject(wob, thisob);
        }else
          wob = new WithObject(top.GetGlobalScope(), thisob);
        wob.isKnownAtCompileTime = true; 
        StackFrame sf = new StackFrame(wob, fields, new Object[fields.Length], thisob);
        sf.closureInstance = thisob;
        globals.ScopeStack.GuardedPush(sf);
      }
      
#if !DEBUG
      [DebuggerStepThroughAttribute]
      [DebuggerHiddenAttribute]
#endif      
      internal override void SetMemberValue(String name, Object value){
        this.AllocateFunctionScope();
        this.nestedFunctionScope.SetMemberValue(name, value, this);
      }
    }
}
