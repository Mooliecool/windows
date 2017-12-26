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
    using System.Runtime.InteropServices.Expando;

    [System.Runtime.InteropServices.ComVisible(true)]
    public class GlobalScope : ActivationObject, IExpando{
      private ArrayList componentScopes; //List of script blocks that collectively form a global or module scope. Script blocks have no component scopes.
      internal GlobalObject globalObject; //null for script blocks and module scopes.
      private bool recursive; //Set this while busy with a lookup, in case the object to which the lookup has been delegated returns the favor.
      internal bool evilScript; //Indicates that script contains an eval, or that it may be followed by separately compiled script blocks
      internal Object thisObject; //Late bound uses of the this literal use this value. Only happens in calls to eval from global code.
      internal bool isComponentScope;
      private TypeReflector globalObjectTR;
      private TypeReflector typeReflector;

      public GlobalScope(GlobalScope parent, VsaEngine engine)
        : this(parent, engine, parent != null){
      }

      internal GlobalScope(GlobalScope parent, VsaEngine engine, bool isComponentScope)
        : base(parent){
        this.componentScopes = null;
        this.recursive = false;
        this.isComponentScope = isComponentScope;
        if (parent == null){
          this.globalObject = engine.Globals.globalObject;
          this.globalObjectTR = TypeReflector.GetTypeReflectorFor(Globals.TypeRefs.ToReferenceContext(this.globalObject.GetType()));
          this.fast = !(this.globalObject is LenientGlobalObject);
        }else{
          this.globalObject = null;
          this.globalObjectTR = null;
          this.fast = parent.fast;
          if (isComponentScope)
            ((GlobalScope)this.parent).AddComponentScope(this);
        }
        this.engine = engine;
        this.isKnownAtCompileTime = this.fast;
        this.evilScript = true; //True by default. Set it false when a single script block is being compiled.
        this.thisObject = this;
        this.typeReflector = TypeReflector.GetTypeReflectorFor(Globals.TypeRefs.ToReferenceContext(this.GetType()));
        if (isComponentScope)
          engine.Scopes.Add(this);
      }


      internal void AddComponentScope(GlobalScope component){
        if (this.componentScopes == null)
          this.componentScopes = new ArrayList();
        this.componentScopes.Add(component);
        component.thisObject = this.thisObject; //Component scopes pretend they are one and the same as their parent scope.
      }

      public FieldInfo AddField(String name){
        if (this.fast)
          return null;
        if (this.isComponentScope)
          return ((GlobalScope)this.parent).AddField(name);
        FieldInfo field = (FieldInfo)this.name_table[name];
        if (field == null){
          field = new JSExpandoField(name);
          this.name_table[name] = field;
          this.field_table.Add(field);
        }
        return field;
      }

      MethodInfo IExpando.AddMethod(String name, Delegate method){
        return null;
      }

      internal override JSVariableField AddNewField(String name, Object value, FieldAttributes attributeFlags){
        if (!this.isComponentScope)
          return base.AddNewField(name, value, attributeFlags);
        //Could get here from eval
        return ((GlobalScope)this.parent).AddNewField(name, value, attributeFlags);
      }

      PropertyInfo IExpando.AddProperty(String name){
        return null;
      }

      internal override bool DeleteMember(String name){
        if (this.isComponentScope)
          return this.parent.DeleteMember(name);
        FieldInfo field = (FieldInfo)this.name_table[name];
        if (field != null){
          if (field is JSExpandoField){
            field.SetValue(this, Missing.Value);
            this.name_table.Remove(name);
            this.field_table.Remove(field);
            return true;
          }else
            return false;
        }else
          return false;
      }

      public override Object GetDefaultThisObject(){
        return this;
      }

      internal override Object GetDefaultValue(PreferredType preferred_type){
        if (preferred_type == PreferredType.String || preferred_type == PreferredType.LocaleString)
          return "";
        else
          return Double.NaN;
      }

      public override FieldInfo GetField(String name, int lexLevel){
        return this.GetField(name, BindingFlags.Public|BindingFlags.Instance|BindingFlags.Static|BindingFlags.DeclaredOnly);
      }

      internal JSField[] GetFields(){
        int n = this.field_table.Count;
        JSField[] result = new JSField[n];
        for (int i = 0; i < n; i++) result[i] = (JSField)this.field_table[i];
        return result;
      }

      public override FieldInfo[] GetFields(BindingFlags bindingAttr){
        return base.GetFields(bindingAttr | BindingFlags.DeclaredOnly);
      }

      public override GlobalScope GetGlobalScope(){
        return this;
      }

      public override FieldInfo GetLocalField(String name){
        return this.GetField(name, BindingFlags.Instance|BindingFlags.Static|BindingFlags.Public|BindingFlags.DeclaredOnly);
      }

      public override MemberInfo[] GetMember(String name, BindingFlags bindingAttr){
        return this.GetMember(name, bindingAttr, false);
      }

      private MemberInfo[] GetMember(String name, BindingFlags bindingAttr, bool calledFromParent){
        if (this.recursive)
          return new MemberInfo[0];
        MemberInfo[] result = null;
        if (!this.isComponentScope){
          //Look for an expando
          MemberInfo[] members = base.GetMember(name, bindingAttr|BindingFlags.DeclaredOnly);
          if (members.Length > 0)
            return members;
          if (this.componentScopes != null)
            for (int i = 0, n = this.componentScopes.Count; i < n; i++){
              GlobalScope sc = (GlobalScope)this.componentScopes[i];
              result = sc.GetMember(name, bindingAttr|BindingFlags.DeclaredOnly, true);
              if (result.Length > 0)
                return result;
            }
          if (this.globalObject != null)
            result = this.globalObjectTR.GetMember(name, bindingAttr & ~BindingFlags.NonPublic | BindingFlags.Static);
          if (result != null && result.Length > 0)
            return ScriptObject.WrapMembers(result, this.globalObject);
        }else{
          //Look for global variables represented as static fields on subclass of GlobalScope. I.e. the script block case.
          result = this.typeReflector.GetMember(name, bindingAttr & ~BindingFlags.NonPublic | BindingFlags.Static);
          int n = result.Length;
          if (n > 0){
            int toBeHidden = 0;
            MemberInfo[] newResult = new MemberInfo[n];
            for (int i = 0; i < n; i++){
              MemberInfo mem = newResult[i] = result[i];
              if (mem.DeclaringType.IsAssignableFrom(Typeob.GlobalScope)){
                newResult[i] = null; toBeHidden++;
              }else if (mem is FieldInfo){
                FieldInfo field = (FieldInfo)mem;
                if (field.IsStatic && field.FieldType == Typeob.Type){
                  Type t = (Type)field.GetValue(null);
                  if (t != null)
                    newResult[i] = t;
                }
              }
            }
            if (toBeHidden == 0) return result;
            if (toBeHidden == n) return new MemberInfo[0];
            MemberInfo[] remainingMembers = new MemberInfo[n-toBeHidden];
            int j = 0;
            foreach (MemberInfo mem in newResult)
              if (mem != null)
                remainingMembers[j++] = mem;
            return remainingMembers;
          }
        }
        if (this.parent != null && !calledFromParent && ((bindingAttr & BindingFlags.DeclaredOnly) == 0 || this.isComponentScope)){
          this.recursive = true;
          try{
            result = ((ScriptObject)this.parent).GetMember(name, bindingAttr);
          }finally{
            this.recursive = false;
          }
          if (result != null && result.Length > 0)
            return result;
        }
        return new MemberInfo[0];
      }

      public override MemberInfo[] GetMembers(BindingFlags bindingAttr){
        if (this.recursive)
          return new MemberInfo[0];
        MemberInfoList result = new MemberInfoList();
        if (this.isComponentScope){
          MemberInfo[] mems = Globals.TypeRefs.ToReferenceContext(this.GetType()).GetMembers(bindingAttr | BindingFlags.DeclaredOnly);
          if (mems != null)
            foreach (MemberInfo mem in mems)
              result.Add(mem);
        }else{
          if (this.componentScopes != null){
            for (int i = 0, n = this.componentScopes.Count; i < n; i++){
              GlobalScope sc = (GlobalScope)this.componentScopes[i];
              this.recursive = true;
              MemberInfo[] mems = null;
              try{
                mems = sc.GetMembers(bindingAttr);
              }finally{
                this.recursive = false;
              }
              if (mems != null)
                foreach (MemberInfo mem in mems)
                  result.Add(mem);
            }
          }
          IEnumerator enu = this.field_table.GetEnumerator();
          while (enu.MoveNext()){
            FieldInfo field = (FieldInfo)enu.Current;
            result.Add(field);
          }
        }
        if (this.parent != null && (this.isComponentScope || ((bindingAttr & BindingFlags.DeclaredOnly) == 0)) ){
          this.recursive = true;
          MemberInfo[] mems = null;
          try{
            mems = ((ScriptObject)this.parent).GetMembers(bindingAttr);
          }finally{
            this.recursive = false;
          }
          if (mems != null)
            foreach (MemberInfo mem in mems)
              result.Add(mem);
        }
        return result.ToArray();
      }

      public override MethodInfo[] GetMethods(BindingFlags bindingAttr){
        return base.GetMethods(bindingAttr | BindingFlags.DeclaredOnly);
      }

      public override PropertyInfo[] GetProperties(BindingFlags bindingAttr){
        return base.GetProperties(bindingAttr | BindingFlags.DeclaredOnly);
      }

      internal override void GetPropertyEnumerator(ArrayList enums, ArrayList objects){
        FieldInfo[] fields = this.GetFields(BindingFlags.Instance|BindingFlags.Static|BindingFlags.Public);
        if (fields.Length > 0){
          enums.Add(fields.GetEnumerator());
          objects.Add(this);
        }
        ScriptObject parent = this.GetParent();
        if (parent != null)
          parent.GetPropertyEnumerator(enums, objects);
      }

      internal void SetFast(){
        this.fast = true;
        this.isKnownAtCompileTime = true;
        if (this.globalObject != null){
          this.globalObject = GlobalObject.commonInstance;
          this.globalObjectTR = TypeReflector.GetTypeReflectorFor(Globals.TypeRefs.ToReferenceContext(this.globalObject.GetType()));
        }
      }

      void IExpando.RemoveMember(MemberInfo m){
        this.DeleteMember(m.Name);
      }

      internal override void SetMemberValue(String name, Object value){
        MemberInfo[] members = this.GetMember(name, BindingFlags.Instance|BindingFlags.Static|BindingFlags.Public);
        if (members.Length == 0){
          // We do not want to create expando fields on the global object when assigning to an undefined
          // variable in the debugger.
          if (VsaEngine.executeForJSEE )
            throw new JScriptException(JSError.UndefinedIdentifier, new Context(new DocumentContext("", null), name));
          FieldInfo field = this.AddField(name);
          if (field != null)
            field.SetValue(this, value);
          return;
        }
        MemberInfo m = LateBinding.SelectMember(members);
        if (m == null)
          throw new JScriptException(JSError.AssignmentToReadOnly);
        LateBinding.SetMemberValue(this, name, value, m, members);
      }

    }
}
