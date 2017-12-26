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

/*
This class keeps track of the members of classes for compile-time purposes.
 
It is also used at run-time, by the Evaluator, to store field instance values.
expandoValue and Clone have to do with the latter usage.
*/
namespace Microsoft.JScript {
    using System;
    using System.Collections;
    using System.Globalization;
    using System.Reflection;
    
    internal sealed class JSMemberField : JSVariableField{
      private Object expandoValue;
      internal JSMemberField nextOverload;
    
      internal JSMemberField(ClassScope obj, String name, Object value, FieldAttributes attributeFlags)
        : base(name, obj, attributeFlags) {
        this.value = value;
        this.nextOverload = null;
      }
      
      internal JSMemberField AddOverload(FunctionObject func, FieldAttributes attributeFlags){
        JSMemberField last = this;
        while (last.nextOverload != null) last = last.nextOverload;
        JSMemberField f = last.nextOverload = new JSMemberField((ClassScope)this.obj, this.Name, func, attributeFlags);
        f.type = this.type;
        return f;
      }

      internal void AddOverloadedMembers(MemberInfoList mems, ClassScope scope, BindingFlags attrs){
        JSMemberField field = this;
        while (field != null){
          MethodInfo meth = ((JSMemberField)field).GetAsMethod(scope);
          if (meth.IsStatic){
            if ((attrs & BindingFlags.Static) == 0) goto next;
          }else{
            if ((attrs & BindingFlags.Instance) == 0) goto next;
          }
          if (meth.IsPublic){
            if ((attrs & BindingFlags.Public) == 0) goto next;
          }else{
            if ((attrs & BindingFlags.NonPublic) == 0) goto next;
          }
          mems.Add(meth);
        next:
          field = field.nextOverload;
        }
        if ((attrs & BindingFlags.DeclaredOnly) != 0 && (attrs&BindingFlags.FlattenHierarchy) == 0) return;
        IReflect superClass = scope.GetSuperType();
        MemberInfo[] supMembers = superClass.GetMember(this.Name, attrs&~BindingFlags.DeclaredOnly);
        foreach (MemberInfo supMember in supMembers)
          if (supMember.MemberType == MemberTypes.Method)
            mems.Add(supMember);
      }

      public override FieldAttributes Attributes{
        get{
          if ((this.attributeFlags & FieldAttributes.Literal) != 0){
            if (this.value is FunctionObject && !((FunctionObject)this.value).isStatic)
              return this.attributeFlags;
            else if (this.value is JSProperty){
              JSProperty prop = (JSProperty)value;
              if (prop.getter != null && !(prop.getter.IsStatic)) return this.attributeFlags;
              if (prop.setter != null && !(prop.setter.IsStatic)) return this.attributeFlags;
            }else
              return this.attributeFlags;
            return this.attributeFlags|FieldAttributes.Static;
          }
          return this.attributeFlags;
        }
      }
      
      internal void CheckOverloadsForDuplicates(){
        JSMemberField current = this;
        while (current != null){
          FunctionObject func = current.value as FunctionObject;
          if (func == null) return;
          for (JSMemberField next = current.nextOverload; next != null; next = next.nextOverload){
            FunctionObject f = (FunctionObject)next.value;
            if (f.implementedIface != func.implementedIface) continue;
            if (Class.ParametersMatch(f.parameter_declarations, func.parameter_declarations)){
              func.funcContext.HandleError(JSError.DuplicateMethod);
              f.funcContext.HandleError(JSError.DuplicateMethod);
              break;
            }
          }
          current = current.nextOverload;
        }
      }

      internal override Object GetMetaData(){
        if (this.metaData == null)
          ((ClassScope)this.obj).GetTypeBuilderOrEnumBuilder();
        return this.metaData;
      }
      
      public override Object GetValue(Object obj){
        if (obj is StackFrame)
          return this.GetValue(((StackFrame)obj).closureInstance, (StackFrame)obj);
        if (obj is ScriptObject)
          return this.GetValue(obj, (ScriptObject)obj);
        return this.GetValue(obj, null);
      }
      
      private Object GetValue(Object obj, ScriptObject scope){
        if (this.IsStatic || this.IsLiteral) return this.value;
        if (this.obj != obj){
          JSObject jsob = obj as JSObject;
          if (jsob != null){
            FieldInfo field = jsob.GetField(this.Name, BindingFlags.Instance|BindingFlags.Static|BindingFlags.Public|BindingFlags.NonPublic);
            if (field != null)
              return field.GetValue(obj);
            else if (jsob.outer_class_instance != null)
              return GetValue(jsob.outer_class_instance, null);
          }
          throw new TargetException();
        }
        if (!this.IsPublic && (scope == null || !this.IsAccessibleFrom(scope)))
          if (((JSObject)this.obj).noExpando)
            throw new JScriptException(JSError.NotAccessible, new Context(new DocumentContext("", null), this.Name));
          else
            return this.expandoValue;
        return this.value;
      }
      
      internal bool IsAccessibleFrom(ScriptObject scope){ //Never call this if the member is public
        while (scope != null && !(scope is ClassScope))
          scope = scope.GetParent();
        ClassScope objType = null;
        if (this.obj is ClassScope)
          objType = (ClassScope)this.obj;
        else
          objType = (ClassScope)((ScriptObject)this.obj).GetParent();
        if (this.IsPrivate)
          if (scope == null)
            return false;
          else
            return scope == objType || ((ClassScope)scope).IsNestedIn(objType, this.IsStatic);
        else if (this.IsFamily)
          if (scope == null)
            return false;
          else
            return ((ClassScope)scope).IsSameOrDerivedFrom(objType) || ((ClassScope)scope).IsNestedIn(objType, this.IsStatic);
        else { //if (this.IsAssembly || this.IsFamilyOrAssembly)
          if (this.IsFamilyOrAssembly && scope != null &&
              (((ClassScope)scope).IsSameOrDerivedFrom(objType) || ((ClassScope)scope).IsNestedIn(objType, this.IsStatic)))
            return true;
          else if (scope == null) //Code is not in a class and hence it is in the default package
            return objType.GetPackage() == null; //null indicates default package
          else
            return objType.GetPackage() == ((ClassScope)scope).GetPackage();
        }
      }

      internal ConstructorInfo[] GetAsConstructors(Object proto){
        JSMemberField field = this;
        int n = 0;
        while (field != null){
          field = field.nextOverload;
          n++;
        }
        ConstructorInfo[] result = new ConstructorInfo[n];
        field = this;
        n = 0;
        while (field != null){
          Debug.Assert(field.IsLiteral);
          FunctionObject func = (FunctionObject)field.value;
          func.isConstructor = true;
          func.proto = proto;
          result[n++] = new JSConstructor(func);
          field = field.nextOverload;
        }
        return result;
      }
      
      public override void SetValue(Object obj, Object value, BindingFlags invokeAttr, Binder binder, CultureInfo locale){
        if (obj is StackFrame)
          this.SetValue(((StackFrame)obj).closureInstance, value, invokeAttr, binder, locale, (StackFrame)obj);
        else if (obj is ScriptObject)
          this.SetValue(obj, value, invokeAttr, binder, locale, (ScriptObject)obj);
        else
          this.SetValue(obj, value, invokeAttr, binder, locale, null);
      }
      
      private void SetValue(Object obj, Object value, BindingFlags invokeAttr, Binder binder, CultureInfo locale, ScriptObject scope){
        if (this.IsStatic || this.IsLiteral){
          if ((this.IsLiteral || this.IsInitOnly) && !(this.value is Missing))
            throw new JScriptException(JSError.AssignmentToReadOnly);
          goto setValue;
        }
        if (this.obj != obj){
          if (obj is JSObject){
            FieldInfo field = ((JSObject)obj).GetField(this.Name, BindingFlags.Instance|BindingFlags.Static|BindingFlags.Public|BindingFlags.NonPublic);
            if (field != null){
              field.SetValue(obj, value, invokeAttr, binder, locale);
              return;
            }
          }
          throw new TargetException();
        }
        if (!this.IsPublic && (scope == null || !this.IsAccessibleFrom(scope))){
          if (((JSObject)this.obj).noExpando)
            throw new JScriptException(JSError.NotAccessible, new Context(new DocumentContext("", null), this.Name));
          else
            this.expandoValue = value;
          return;
        }
      setValue:
        if (this.type != null)
          this.value = Convert.Coerce(value, this.type);
        else
          this.value = value;
      }
    }
}
