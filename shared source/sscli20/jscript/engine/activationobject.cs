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
    using System.Collections;
    using System.Diagnostics;

    /// <internalonly/>
    [System.Runtime.InteropServices.ComVisible(true)]
    public abstract class ActivationObject : ScriptObject, IActivationObject{
      internal bool isKnownAtCompileTime;
      internal bool fast;
      internal SimpleHashtable name_table;
      protected ArrayList field_table;

      internal ActivationObject(ScriptObject parent)
        : base(parent) {
        this.name_table = new SimpleHashtable(32);
        this.field_table = new ArrayList();
      }

      //This method is used to add declared variables/fields to scopes at compile time.
      //It does not add a field if there already is a field with such a name.

      internal virtual JSVariableField AddFieldOrUseExistingField(String name, Object value, FieldAttributes attributeFlags){
        FieldInfo field = (FieldInfo)(this.name_table[name]);
        if (field is JSVariableField){
          if (!(value is Missing))
            //Do not clobber the existing value unless there is an explicit initializer
            //This matters when declaring a variable in an eval string (and the calling scope already has such a variable).
            ((JSVariableField)field).value = value;
          return (JSVariableField)field;
        }
        if (value is Missing) value = null;
        return this.AddNewField(name, value, attributeFlags);
      }

      //Adds a list of classes to result. Superclasses always follow derived classes.

      internal void AddClassesExcluding(ClassScope excludedClass, String name, ArrayList result){
        ArrayList eligibleClasses = new ArrayList();
        foreach (MemberInfo member in this.GetMembers(BindingFlags.Static|BindingFlags.Public|BindingFlags.NonPublic)){
          if (member is JSVariableField && ((JSVariableField)member).IsLiteral){
            Object val = ((JSVariableField)member).value;
            if (val is ClassScope){
              ClassScope csc = (ClassScope)val;
              if (csc.name != member.Name) continue;
              if (excludedClass == null || !excludedClass.IsSameOrDerivedFrom(csc))
                if (csc.GetMember(name, BindingFlags.Public|BindingFlags.NonPublic|BindingFlags.Instance|BindingFlags.DeclaredOnly).Length > 0)
                  eligibleClasses.Add(csc);
            }
          }
        }
        if (eligibleClasses.Count == 0) return;
        ClassScope[] classes = new ClassScope[eligibleClasses.Count]; eligibleClasses.CopyTo(classes);
        Array.Sort(classes);
        result.AddRange(classes);
      }

      //This method is used when the definition must obliterate an existing definition. For example when adding formal parameters at compile time.
      //It is also used when it is known there is no field with this name, since it does not bother to check.

      internal virtual JSVariableField AddNewField(String name, Object value, FieldAttributes attributeFlags){
        JSVariableField result = this.CreateField(name, attributeFlags, value);
        this.name_table[name] = result;
        this.field_table.Add(result);
        return result;
      }

      //This method requires derived classes to provide the right type of field. (JSLocalField/JSGlobalField/JSMemberField).

      protected virtual JSVariableField CreateField(String name, FieldAttributes attributeFlags, Object value){
        return new JSGlobalField(this, name, value, attributeFlags|FieldAttributes.Static);
      }

      public virtual FieldInfo GetField(String name, int lexLevel){
        throw new JScriptException(JSError.InternalError);
      }

      internal virtual String GetName(){
        return null;
      }

      //IActivationObject methods:

      public virtual Object GetDefaultThisObject(){
        return ((IActivationObject)this.GetParent()).GetDefaultThisObject();
      }

      public virtual GlobalScope GetGlobalScope(){
        return ((IActivationObject)this.GetParent()).GetGlobalScope();
      }

      public virtual FieldInfo GetLocalField(String name){
        return (FieldInfo)this.name_table[name];
      }


      public override MemberInfo[] GetMember(String name, BindingFlags bindingAttr){
        FieldInfo field = (FieldInfo)(this.name_table[name]);
        if (field != null) return new MemberInfo[]{field};
        if (this.parent != null && (bindingAttr&BindingFlags.DeclaredOnly) == 0)
          return ScriptObject.WrapMembers(this.parent.GetMember(name, bindingAttr), this.parent);
        return new MemberInfo[0];
      }

      public override MemberInfo[] GetMembers(BindingFlags bindingAttr){
        int n = this.field_table.Count;
        MemberInfo[] result = new MemberInfo[n];
        for (int i = 0; i < n; i++) result[i] = (MemberInfo)this.field_table[i];
        return result;
      }

#if !DEBUG
      [DebuggerStepThroughAttribute]
      [DebuggerHiddenAttribute]
#endif
      public Object GetMemberValue(String name, int lexlevel){
        if (lexlevel <= 0)
          return Missing.Value;
        FieldInfo field = (FieldInfo)(this.name_table[name]);
        if (field != null)
          return field.GetValue(this);
        else if (this.parent != null)
          return ((IActivationObject)this.parent).GetMemberValue(name, lexlevel-1);
        else
          return Missing.Value;
      }
    }
}
