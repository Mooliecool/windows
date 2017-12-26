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

namespace Microsoft.JScript{

    using Microsoft.JScript.Vsa;
    using System;
    using System.Collections;
    using System.Reflection;
    using System.Diagnostics;

    //********************************************************************************************
    // VsaNamedItemScope
    //
    // This class is a ScriptObject that acts like a peer for a named item introduced by a host
    // with the exposeMembers flag set.
    // An instance of this object is pushed into the prototype chain of the specified scope
    // so it can participate in name resolution.
    // When asked for a name (field, property or method) an instance of this class forwards
    // the host object the request.
    // The engine should never deal with this class as if it was a "concrete" script object, that
    // is, should never attempt to add a name to this class. This class should be used by the
    // engine only for name resolution.
    //********************************************************************************************
    internal sealed class VsaNamedItemScope : ScriptObject, IActivationObject{

      internal Object namedItem;
      //Used by GetMembers to cache members that must be delegated to other objects
      private SimpleHashtable namedItemWrappedMemberCache;
            
      private IReflect reflectObj;
      private bool recursive;

      internal VsaNamedItemScope(Object hostObject, ScriptObject parent, VsaEngine engine)
        : base(parent){
        this.namedItem = hostObject;
        if ((this.reflectObj = hostObject as IReflect) == null)
          this.reflectObj = Globals.TypeRefs.ToReferenceContext(hostObject.GetType());
        this.recursive = false;
        this.engine = engine;
      }

      private static MemberInfo[] GetAndWrapMember(IReflect reflect, Object namedItem, String name, BindingFlags bindingAttr){
        PropertyInfo property = reflect.GetProperty(name, bindingAttr);
        if (property != null){
          MethodInfo getMethod = JSProperty.GetGetMethod(property, false);
          MethodInfo setMethod = JSProperty.GetSetMethod(property, false);
          if ((getMethod != null && !getMethod.IsStatic) || (setMethod != null && !setMethod.IsStatic)){
            MethodInfo method = reflect.GetMethod(name, bindingAttr);
            if (method != null && !method.IsStatic){
              MemberInfo[] propMethods = new MemberInfo[1];
              propMethods[0] = new JSWrappedPropertyAndMethod(property, method, namedItem);
              return propMethods;
            }
          }
        }
        MemberInfo[] members = reflect.GetMember(name, bindingAttr);
        if (members != null && members.Length > 0)
          return ScriptObject.WrapMembers(members, namedItem);
        return null;
      }

      public Object GetDefaultThisObject(){
        return ((IActivationObject)this.GetParent()).GetDefaultThisObject();
      }

      public FieldInfo GetField(String name, int lexLevel){
        throw new JScriptException(JSError.InternalError);
      }

      public GlobalScope GetGlobalScope(){
        return ((IActivationObject)this.GetParent()).GetGlobalScope();
      }

      FieldInfo IActivationObject.GetLocalField(String name){
        Debug.Assert(false);
        return null;
      }

#if !DEBUG
      [DebuggerStepThroughAttribute]
      [DebuggerHiddenAttribute]
#endif
      public Object GetMemberValue(String name, int lexlevel){
        if (lexlevel <= 0)
          return Missing.Value;
        Object result = LateBinding.GetMemberValue(this.namedItem, name);
        if (!(result is Missing))
          return result;
        return ((IActivationObject)parent).GetMemberValue(name, lexlevel-1);
      }

      public override MemberInfo[] GetMember(String name, BindingFlags bindingAttr){
        MemberInfo[] members = null;
        if (!this.recursive && this.reflectObj != null){
          this.recursive = true;
          try{
            ISite2 site;
            if (!this.reflectObj.GetType().IsCOMObject || (site = this.engine.Site as ISite2) == null)
              members = ScriptObject.WrapMembers(this.reflectObj.GetMember(name, bindingAttr), this.namedItem);
            else if ((members = VsaNamedItemScope.GetAndWrapMember(this.reflectObj, this.namedItem, name, bindingAttr)) == null){
              Object[] parentChain = site.GetParentChain(this.reflectObj);
              if (parentChain != null){
                int parentChainLength = parentChain.Length;
                for (int i = 0; i < parentChainLength; i++){
                  IReflect parent = parentChain[i] as IReflect;
                  if (parent != null && (members = VsaNamedItemScope.GetAndWrapMember(parent, parent, name, bindingAttr)) != null)
                    break;
                }
              }
            }
          }finally{
            this.recursive = false;
          }
        }
        return members == null ? new MemberInfo[0] : members;
      }

      public override MemberInfo[] GetMembers(BindingFlags bindingAttr){
        MemberInfo[] members = null;
        if (!this.recursive){
          this.recursive = true;
          try{
          members = this.reflectObj.GetMembers(bindingAttr);
          if (members != null)
            if (members.Length > 0){
              SimpleHashtable cache = this.namedItemWrappedMemberCache;
              if (cache == null)
                cache = this.namedItemWrappedMemberCache = new SimpleHashtable(16);
              members = ScriptObject.WrapMembers(members, this.namedItem, cache);
            }
            else
              members = null;
          }finally{
            this.recursive = false;
          }
        }
        return members;
      }

#if !DEBUG
      [DebuggerStepThroughAttribute]
      [DebuggerHiddenAttribute]
#endif
      internal override Object GetMemberValue(String name){
        Object retValue = Missing.Value;

        if (!this.recursive){
          this.recursive = true;
          try{
            FieldInfo field = this.reflectObj.GetField(name, BindingFlags.Instance|BindingFlags.Static|BindingFlags.FlattenHierarchy|BindingFlags.Public);
            if (field == null){
              PropertyInfo prop = this.reflectObj.GetProperty(name, BindingFlags.Instance|BindingFlags.Static|BindingFlags.FlattenHierarchy|BindingFlags.Public);
              if (prop != null)
                retValue = JSProperty.GetValue(prop, this.namedItem, null);
            }else
              retValue = field.GetValue(this.namedItem);

            if (retValue is Missing && null != parent)
              retValue = parent.GetMemberValue(name);
          }finally{
            this.recursive = false;
          }
        }
        return retValue;
      }

#if !DEBUG
      [DebuggerStepThroughAttribute]
      [DebuggerHiddenAttribute]
#endif
      internal override void SetMemberValue(String name, Object value){
        bool assigned = false;
        if (!this.recursive){
          this.recursive = true;
          try{
            FieldInfo field = this.reflectObj.GetField(name, BindingFlags.Instance|BindingFlags.Static|BindingFlags.FlattenHierarchy|BindingFlags.Public);
            if (field == null){
              PropertyInfo prop = this.reflectObj.GetProperty(name, BindingFlags.Instance|BindingFlags.Static|BindingFlags.FlattenHierarchy|BindingFlags.Public);
              if (prop != null){
                JSProperty.SetValue(prop, this.namedItem, value, null);
                assigned = true;
              }
            }
            else{
              field.SetValue(this.namedItem, value);
              assigned = true;
            }

            if (!assigned && null != parent)
              parent.SetMemberValue(name, value);
          }finally{
            this.recursive = false;
          }
        }
      }
    }
}
