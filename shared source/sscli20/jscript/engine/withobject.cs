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
    using System.Diagnostics;
    using System.Security;
    using System.Security.Permissions;

    internal sealed class WithObject : ScriptObject, IActivationObject{
      internal Object contained_object;
      internal bool isKnownAtCompileTime;
      private bool isSuperType;

      internal WithObject(ScriptObject parent, Object contained_object)
        : this(parent, contained_object, false) {
      }

      internal WithObject(ScriptObject parent, Object contained_object, bool isSuperType)
        : base(parent) {
        this.contained_object = contained_object;
        this.isKnownAtCompileTime = contained_object is Type ||
                                   (contained_object is ClassScope && ((ClassScope)contained_object).noExpando) ||
                                   (contained_object is JSObject && ((JSObject)contained_object).noExpando);
        this.isSuperType = isSuperType;
      }

      public Object GetDefaultThisObject(){
        return this.contained_object;
      }

      public FieldInfo GetField(String name, int lexLevel){
        if (lexLevel <= 0)
          return null;
        IReflect ir;
        if (this.contained_object is IReflect)
          ir = (IReflect)(this.contained_object);
        else
          ir = (IReflect)(Globals.TypeRefs.ToReferenceContext(this.contained_object.GetType()));
        FieldInfo field = ir.GetField(name, BindingFlags.Instance|BindingFlags.Static|BindingFlags.FlattenHierarchy|BindingFlags.Public);
        if (field != null)
          return new JSWrappedField(field, this.contained_object);
        PropertyInfo prop = ir.GetProperty(name, BindingFlags.Instance|BindingFlags.Static|BindingFlags.Public);
        if (prop != null)
          return new JSPropertyField(prop, this.contained_object);
        if (this.parent != null && lexLevel > 1){
          field = ((IActivationObject)this.parent).GetField(name, lexLevel-1);
          if (field != null)
            return new JSWrappedField(field, this.parent);
        }
        return null;
      }

      public GlobalScope GetGlobalScope(){
        return ((IActivationObject)this.GetParent()).GetGlobalScope();
      }

      FieldInfo IActivationObject.GetLocalField(String name){
        Debug.Assert(false);
        return null;
      }

      public override MemberInfo[] GetMember(String name, BindingFlags bindingAttr){ //Never called from outside the engine
        return this.GetMember(name, bindingAttr, true);
      }

      internal MemberInfo[] GetMember(String name, BindingFlags bindingAttr, bool forceInstanceLookup){
        IReflect ir;
        Type t = null;
        BindingFlags attr = bindingAttr;
        if (forceInstanceLookup && this.isSuperType && (bindingAttr & BindingFlags.FlattenHierarchy) == 0) attr |= BindingFlags.Instance;
        Object obj = this.contained_object;
      try_again_with_outer_class_instance:
        if (obj is IReflect){
          ir = (IReflect)(obj);
          if (obj is Type && !this.isSuperType)
            attr &= ~BindingFlags.Instance;
        }else
          ir = t = Globals.TypeRefs.ToReferenceContext(obj.GetType());
        MemberInfo[] members = ir.GetMember(name, attr&~BindingFlags.DeclaredOnly);
        if (members.Length > 0)
          return ScriptObject.WrapMembers(members, obj);
        if (obj is Type && !this.isSuperType)
          members = Typeob.Type.GetMember(name, BindingFlags.Instance|BindingFlags.Public);
        if (members.Length > 0)
          return ScriptObject.WrapMembers(members, obj);
        if (t != null && t.IsNestedPublic){ //Might happen during call to eval
          try{
            new ReflectionPermission(ReflectionPermissionFlag.TypeInformation | ReflectionPermissionFlag.MemberAccess).Assert();
            FieldInfo outerClassInstance = t.GetField("outer class instance", BindingFlags.NonPublic|BindingFlags.Instance);
            if (outerClassInstance != null){
              obj = outerClassInstance.GetValue(obj);
              goto try_again_with_outer_class_instance;
            }
          }finally{
            CodeAccessPermission.RevertAssert();
          }
        }
        if (members.Length > 0)
          return ScriptObject.WrapMembers(members, obj);
        return new MemberInfo[0];
      }

      public override MemberInfo[] GetMembers(BindingFlags bindingAttr){ //Only called when part of the parent chain of a ClassScope
        return ((IReflect)this.contained_object).GetMembers(bindingAttr);
      }

#if !DEBUG
      [DebuggerStepThroughAttribute]
      [DebuggerHiddenAttribute]
#endif
      internal override Object GetMemberValue(String name){
        Object result = LateBinding.GetMemberValue2(this.contained_object, name);
        if (!(result is Missing))
          return result;
        if (this.parent != null)
          return this.parent.GetMemberValue(name);
        return Missing.Value;
      }

#if !DEBUG
      [DebuggerStepThroughAttribute]
      [DebuggerHiddenAttribute]
#endif
      public Object GetMemberValue(String name, int lexlevel){
        if (lexlevel <= 0)
          return Missing.Value;
        Object result = LateBinding.GetMemberValue2(this.contained_object, name);
        if (result != Missing.Value)
          return result;
        return ((IActivationObject)this.parent).GetMemberValue(name, lexlevel-1);
      }

#if !DEBUG
      [DebuggerStepThroughAttribute]
      [DebuggerHiddenAttribute]
#endif
      internal override void SetMemberValue(String name, Object value){
        if (LateBinding.GetMemberValue2(this.contained_object, name) is Missing)
          ((ScriptObject)this.parent).SetMemberValue(name, value);
        else
          LateBinding.SetMemberValue(this.contained_object, name, value);
      }
    }

}
