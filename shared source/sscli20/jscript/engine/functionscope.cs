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
    using System.Collections;
    
    internal sealed class FunctionScope : ActivationObject{
      internal bool isMethod;
      internal bool isStatic;
      internal bool mustSaveStackLocals;
      internal JSLocalField returnVar;
      internal FunctionObject owner;
      internal ArrayList nested_functions;
      private ArrayList fields_for_nested_functions;
      internal SimpleHashtable ProvidesOuterScopeLocals;
      internal bool closuresMightEscape;
    
      internal FunctionScope(ScriptObject parent)
        : this(parent, false){
      }
    
      internal FunctionScope(ScriptObject parent, bool isMethod)
        : base(parent) {
        this.isKnownAtCompileTime = true;
        this.isMethod = isMethod;
        this.mustSaveStackLocals = false;
        if (parent != null && parent is ActivationObject)
          this.fast = ((ActivationObject)parent).fast;
        else
          this.fast = false;
        this.returnVar = null;
        this.owner = null; //Given its real value inside new FunctionObject
        this.isStatic = false; //Given its real value elsewhere
        this.nested_functions = null;
        this.fields_for_nested_functions = null;
        if (parent is FunctionScope)
          this.ProvidesOuterScopeLocals = new SimpleHashtable(16);
        else
          this.ProvidesOuterScopeLocals = null;
        this.closuresMightEscape = false;
      }
      
      internal JSVariableField AddNewField(String name, FieldAttributes attributeFlags, FunctionObject func){
        if (this.nested_functions == null){
          this.nested_functions = new ArrayList();
          this.fields_for_nested_functions = new ArrayList();
        }
        this.nested_functions.Add(func);
        JSVariableField result = this.AddNewField(name, (Object)func, attributeFlags);
        this.fields_for_nested_functions.Add(result);
        return result;
      }
      
      protected override JSVariableField CreateField(String name, FieldAttributes attributeFlags, Object value){
        if ((attributeFlags & FieldAttributes.Static) != 0)
          return new JSGlobalField(this, name, value, attributeFlags);
        else
          return new JSLocalField(name, this, this.field_table.Count, value);
      }
      
      internal void AddOuterScopeField(String name, JSLocalField field){
        this.name_table[name] = field;
        this.field_table.Add(field);
      }
      
      internal void AddReturnValueField(){
        if (this.name_table["return value"] != null) return;
        this.returnVar = new JSLocalField("return value", this, this.field_table.Count, Missing.Value);
        this.name_table["return value"] = this.returnVar;
        this.field_table.Add(this.returnVar);
      }
      
      internal void CloseNestedFunctions(StackFrame sf){
        if (this.nested_functions == null)
          return;
        IEnumerator funcs = this.nested_functions.GetEnumerator();
        IEnumerator fields = this.fields_for_nested_functions.GetEnumerator();
        while (funcs.MoveNext() && fields.MoveNext()){
          FieldInfo field = (FieldInfo)fields.Current;
          FunctionObject func = (FunctionObject)funcs.Current;
          func.enclosing_scope = sf;
          field.SetValue(sf, new Closure(func));
        }
      }
      
      internal BitArray DefinedFlags{
        get{
          int n = this.field_table.Count;
          BitArray result = new BitArray(n);
          for (int i = 0; i < n; i++){
            JSLocalField field = (JSLocalField)this.field_table[i];
            if (field.isDefined)
              result[i] = true;
          }
          return result;
        }
        set{
          int n = value.Count;
          for (int i = 0; i < n; i++){
            JSLocalField field = (JSLocalField)this.field_table[i];
            field.isDefined = value[i];
          }
        }
      }
            
      internal JSLocalField[] GetLocalFields(){
        int n = this.field_table.Count;
        JSLocalField[] result = new JSLocalField[this.field_table.Count];
        for (int i = 0; i < n; i++) result[i] = (JSLocalField)this.field_table[i];
        return result;
      }
      
      public override MemberInfo[] GetMember(String name, BindingFlags bindingAttr){
        FieldInfo field = (FieldInfo)(this.name_table[name]);
        if (field != null) return new MemberInfo[]{field};
        bool nestedInInstanceMethod = false;
        ScriptObject parent = this.parent;
        //Treat locals of outer functions as if they were locals of this function
        while (parent is FunctionScope){
          FunctionScope fscope = (FunctionScope)parent;
          nestedInInstanceMethod = fscope.isMethod && !fscope.isStatic;
          JSLocalField lfield = (JSLocalField)fscope.name_table[name];
          if (lfield == null){
            parent = parent.GetParent();
            continue;
          }
          if (lfield.IsLiteral && !(lfield.value is FunctionObject))
            return new MemberInfo[]{lfield};
          JSLocalField f = new JSLocalField(lfield.Name, this, this.field_table.Count, Missing.Value);
          f.outerField = lfield;
          f.debugOn = lfield.debugOn;
          if (!f.debugOn && this.owner.funcContext.document.debugOn && fscope.owner.funcContext.document.debugOn){
            //Check to see it is off because outer field is a parameter
            f.debugOn = Array.IndexOf(fscope.owner.formal_parameters, lfield.Name) >= 0;
          }
          f.isDefined = lfield.isDefined;
          f.debuggerName = "outer" + "." + f.Name;
          if (lfield.IsLiteral){
            f.attributeFlags |= FieldAttributes.Literal;
            f.value = lfield.value;
          }
          this.AddOuterScopeField(name, f);
          if (this.ProvidesOuterScopeLocals[parent] == null) this.ProvidesOuterScopeLocals[parent] = parent;
          ((FunctionScope)parent).mustSaveStackLocals = true;
          return new MemberInfo[]{f};
        }
        if (parent is ClassScope && nestedInInstanceMethod){
          //return class members as if they were local to the function. It is more convenient to wrap up instance members
          //at this stage than it is to figure out later that a special case is involved.
          MemberInfo[] members = parent.GetMember(name, bindingAttr&~BindingFlags.DeclaredOnly);
          int n = members.Length;
          bool giveBadNews = false;
          for (int i = 0; i < n; i++){
            MemberInfo member = members[i];
            switch(member.MemberType){
              case MemberTypes.Field:
                field = (FieldInfo)member;
                if (field.IsLiteral){
                  JSMemberField mfield = field as JSMemberField;
                  if (mfield != null && mfield.value is ClassScope && !((ClassScope)mfield.value).owner.IsStatic)
                    giveBadNews = true;
                }
                if (!field.IsStatic && !field.IsLiteral){
                  members[i] = new JSClosureField(field);
                  giveBadNews = true;
                }
                break;
              case MemberTypes.Method:
                MethodInfo meth = (MethodInfo)member;
                if (!meth.IsStatic){
                  members[i] = new JSClosureMethod(meth);
                  giveBadNews = true;
                }
                break;
              case MemberTypes.Property:
                PropertyInfo prop = (PropertyInfo)member;
                MethodInfo getMeth = JSProperty.GetGetMethod(prop, (bindingAttr&BindingFlags.NonPublic) != 0);
                MethodInfo setMeth = JSProperty.GetSetMethod(prop, (bindingAttr&BindingFlags.NonPublic) != 0);
                bool nonStatic = false;
                if (getMeth != null && !getMeth.IsStatic){
                  nonStatic = true;
                  getMeth = new JSClosureMethod(getMeth);
                }
                if (setMeth != null && !setMeth.IsStatic){
                  nonStatic = true;
                  setMeth = new JSClosureMethod(setMeth);
                }
                if (nonStatic){
                  members[i] = new JSClosureProperty(prop, getMeth, setMeth);
                  giveBadNews = true;
                }
                break;
            }
          }
          if (giveBadNews) this.GiveOuterFunctionsTheBadNews(); //They have to create explicit stack frames
          if (n > 0) return members;
        }
        if ((bindingAttr&BindingFlags.DeclaredOnly) != 0) return new MemberInfo[0];
        return parent.GetMember(name, bindingAttr);
      }

      internal override String GetName(){
        String parentName = null;
        if (this.parent != null)
          parentName = ((ActivationObject)this.parent).GetName();
        if (parentName != null)
          return parentName + "." + this.owner.name;
        else
          return this.owner.name;
      }

      internal int GetNextSlotNumber(){
        return this.field_table.Count;
      }
      
      internal JSLocalField GetOuterLocalField(String name){
        FieldInfo result = (FieldInfo)(this.name_table[name]);
        if (result != null && result is JSLocalField && ((JSLocalField)result).outerField != null)
          return (JSLocalField)result;
        else
          return null;
      }
      
      private void GiveOuterFunctionsTheBadNews(){
        FunctionScope parent = (FunctionScope)this.parent;
        parent.mustSaveStackLocals = true;
        while (!parent.isMethod){
          parent = (FunctionScope)parent.GetParent();
          parent.mustSaveStackLocals = true;
        }
      }
      
      internal void HandleUnitializedVariables(){
        for (int i = 0, n = this.field_table.Count; i < n; i++){
          JSLocalField field = (JSLocalField)this.field_table[i];
          if (field.isUsedBeforeDefinition)
            field.SetInferredType(Typeob.Object, null);
        }
      }
      
      internal override void SetMemberValue(String name, Object value){
        FieldInfo field = (FieldInfo)(this.name_table[name]);
        if (field != null)
          field.SetValue(this, value);
        else
          this.parent.SetMemberValue(name, value);
        //The above behaviour is inconsistent with the general idea of SetMemberValue, in as much as it does
        //not create a new property on the target object. This is to simulate the JScript behavior of adding
        //implicitly declared variables to the global scope, not the local scope.
        
        //WARNING: This means that the engine itself may not use SetMemberValue when it wishes to create a property
        //in an activation object.
      }

      internal void SetMemberValue(String name, Object value, StackFrame sf){
        FieldInfo field = (FieldInfo)(this.name_table[name]);
        if (field != null)
          field.SetValue(sf, value);
        else
          this.parent.SetMemberValue(name, value);
      }
    }
}
