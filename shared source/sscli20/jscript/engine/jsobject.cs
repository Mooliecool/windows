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
    using System.Collections;
    using System.Diagnostics;
    using System.Globalization;
    using System.Reflection;
    using System.Runtime.InteropServices.Expando;

    public class JSObject : ScriptObject, IEnumerable, IExpando{
      private bool isASubClass;
      private IReflect subClassIR;
      private SimpleHashtable memberCache;
      internal bool noExpando;
      internal SimpleHashtable name_table;
      protected ArrayList field_table;
      internal JSObject outer_class_instance;

      public JSObject()
        : this(null, false){
        this.noExpando = false;
      }

      internal JSObject(ScriptObject parent)
        : this(parent, true) {
      }

      internal JSObject(ScriptObject parent, bool checkSubType)
        : base(parent) {
        this.memberCache = null;
        this.isASubClass = false;
        this.subClassIR = null;
        if (checkSubType){
          Type subType = Globals.TypeRefs.ToReferenceContext(this.GetType());
          Debug.Assert(subType != Typeob.BuiltinFunction);
          if (subType != Typeob.JSObject){
            this.isASubClass = true;
            this.subClassIR = TypeReflector.GetTypeReflectorFor(subType);
          }
        }else
          Debug.Assert(Globals.TypeRefs.ToReferenceContext(this.GetType()) == Typeob.JSObject);
        this.noExpando = this.isASubClass;
        this.name_table = null;
        this.field_table = null;
        this.outer_class_instance = null;
      }

      internal JSObject(ScriptObject parent, Type subType)
        : base(parent) {
        this.memberCache = null;
        this.isASubClass = false;
        this.subClassIR = null;
        Debug.Assert(subType == this.GetType() || this.GetType() == typeof(BuiltinFunction));
        
        subType = Globals.TypeRefs.ToReferenceContext(subType);
        if (subType != Typeob.JSObject){
          this.isASubClass = true;
          this.subClassIR = TypeReflector.GetTypeReflectorFor(subType);
        }
        this.noExpando = this.isASubClass;
        this.name_table = null;
        this.field_table = null;
      }

      public FieldInfo AddField(String name){
        if (this.noExpando)
          return null;
        FieldInfo field = (FieldInfo)this.NameTable[name];
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

      PropertyInfo IExpando.AddProperty(String name){
        return null;
      }

      internal override bool DeleteMember(String name){
        FieldInfo field = (FieldInfo)this.NameTable[name];
        if (field != null){
          if (field is JSExpandoField){
            field.SetValue(this, Missing.Value);
            this.name_table.Remove(name);
            this.field_table.Remove(field);
            return true;
          }else if (field is JSPrototypeField){
            field.SetValue(this, Missing.Value);
            return true;
          }else
            return false;
        }else if (this.parent != null)
          return LateBinding.DeleteMember(this.parent, name);
        else
          return false;
      }

      internal virtual String GetClassName(){
        return "Object";
      }

#if !DEBUG
      [DebuggerStepThroughAttribute]
      [DebuggerHiddenAttribute]
#endif
      internal override Object GetDefaultValue(PreferredType preferred_type){
        if (preferred_type == PreferredType.String){
          ScriptFunction toString = this.GetMemberValue("toString") as ScriptFunction;
          if (toString != null){
            Object result = toString.Call(new Object[0], this);
            if (result == null) return result;
            IConvertible ic = Convert.GetIConvertible(result);
            if (ic != null && ic.GetTypeCode() != TypeCode.Object) return result;
          }
          ScriptFunction valueOf = this.GetMemberValue("valueOf") as ScriptFunction;
          if (valueOf != null){
            Object result = valueOf.Call(new Object[0], this);
            if (result == null) return result;
            IConvertible ic = Convert.GetIConvertible(result);
            if (ic != null && ic.GetTypeCode() != TypeCode.Object) return result;
          }
        }else if (preferred_type == PreferredType.LocaleString){
          ScriptFunction toLocaleString = this.GetMemberValue("toLocaleString") as ScriptFunction;
          if (toLocaleString != null){
            return toLocaleString.Call(new Object[0], this);
          }
        }else{
          if (preferred_type == PreferredType.Either && this is DateObject)
            return this.GetDefaultValue(PreferredType.String);
          ScriptFunction valueOf = this.GetMemberValue("valueOf") as ScriptFunction;
          if (valueOf != null){
            Object result = valueOf.Call(new Object[0], this);
            if (result == null) return result;
            IConvertible ic = Convert.GetIConvertible(result);
            if (ic != null && ic.GetTypeCode() != TypeCode.Object) return result;
          }
          ScriptFunction toString = this.GetMemberValue("toString") as ScriptFunction;
          if (toString != null){
            Object result = toString.Call(new Object[0], this);
            if (result == null) return result;
            IConvertible ic = Convert.GetIConvertible(result);
            if (ic != null && ic.GetTypeCode() != TypeCode.Object) return result;
          }
        }
        return this;
      }

      IEnumerator IEnumerable.GetEnumerator(){
        return ForIn.JScriptGetEnumerator(this);
      }

      private static bool IsHiddenMember(MemberInfo mem) {
        // Members that are declared in super classes of JSObject are hidden except for those
        // in Object. 
        Type mtype = mem.DeclaringType;
        if (mtype == Typeob.JSObject || mtype == Typeob.ScriptObject || 
              (mtype == Typeob.ArrayWrapper && mem.Name != "length"))
          return true;
        return false;
      }

      private MemberInfo[] GetLocalMember(String name, BindingFlags bindingAttr, bool wrapMembers){
        MemberInfo[] result = null;
        FieldInfo field = this.name_table == null ? null : (FieldInfo)this.name_table[name];
        if (field == null && this.isASubClass){
          if (this.memberCache != null){
            result = (MemberInfo[])this.memberCache[name];
            if (result != null) return result;
          }
          bindingAttr &= ~BindingFlags.NonPublic; //Never expose non public fields of old style objects
          result = this.subClassIR.GetMember(name, bindingAttr);
          if (result.Length == 0)
            result = this.subClassIR.GetMember(name, (bindingAttr&~BindingFlags.Instance)|BindingFlags.Static);
          int n = result.Length;
          if (n > 0){
            //Suppress any members that are declared in JSObject or earlier. But keep the ones in Object.
            int hiddenMembers = 0;
            foreach (MemberInfo mem in result){
              if (JSObject.IsHiddenMember(mem))
                hiddenMembers++;
            }
            if (hiddenMembers > 0 && !(n == 1 && this is ObjectPrototype && name == "ToString")){
              MemberInfo[] newResult = new MemberInfo[n-hiddenMembers];
              int j = 0;
              foreach (MemberInfo mem in result){
                if (!JSObject.IsHiddenMember(mem))
                  newResult[j++] = mem;
              }
              result = newResult;
            }
          }
          if ((result == null || result.Length == 0) && (bindingAttr & BindingFlags.Public) != 0 && (bindingAttr & BindingFlags.Instance) != 0){
            BindingFlags flags = (bindingAttr & BindingFlags.IgnoreCase) | BindingFlags.Public | BindingFlags.Instance;
            if (this is StringObject)
              result = TypeReflector.GetTypeReflectorFor(Typeob.String).GetMember(name, flags);
            else if (this is NumberObject)
              result = TypeReflector.GetTypeReflectorFor(((NumberObject)this).baseType).GetMember(name, flags);
            else if (this is BooleanObject)
              result = TypeReflector.GetTypeReflectorFor(Typeob.Boolean).GetMember(name, flags);
            else if (this is StringConstructor)
              result = TypeReflector.GetTypeReflectorFor(Typeob.String).GetMember(name, (flags|BindingFlags.Static)&~BindingFlags.Instance);
            else if (this is BooleanConstructor)
              result = TypeReflector.GetTypeReflectorFor(Typeob.Boolean).GetMember(name, (flags|BindingFlags.Static)&~BindingFlags.Instance);
            else if (this is ArrayWrapper)
              result = TypeReflector.GetTypeReflectorFor(Typeob.Array).GetMember(name, flags);
          }
          if (result != null && result.Length > 0){
            if (wrapMembers)
              result = ScriptObject.WrapMembers(result, this);
            if (this.memberCache == null) this.memberCache = new SimpleHashtable(32);
            this.memberCache[name] = result;
            return result;
          }
        }
        if ((bindingAttr&BindingFlags.IgnoreCase) != 0 && (result == null || result.Length == 0)){
          result = null;
          IDictionaryEnumerator e = this.name_table.GetEnumerator();
          while (e.MoveNext()){
            if (String.Compare(e.Key.ToString(), name, StringComparison.OrdinalIgnoreCase) == 0){
              field = (FieldInfo)e.Value;
              break;
            }
          }
        }
        if (field != null)
          return new MemberInfo[]{field};
        if (result == null) result = new MemberInfo[0];
        return result;
      }

      public override MemberInfo[] GetMember(String name, BindingFlags bindingAttr){
        return this.GetMember(name, bindingAttr, false);
      }

      private MemberInfo[] GetMember(String name, BindingFlags bindingAttr, bool wrapMembers){
        MemberInfo[] members = this.GetLocalMember(name, bindingAttr, wrapMembers);
        if (members.Length > 0) return members;
        if (this.parent != null){
          if (this.parent is JSObject){
            members = ((JSObject)this.parent).GetMember(name, bindingAttr, true);
            wrapMembers = false;
          }else
            members = this.parent.GetMember(name, bindingAttr);
          foreach (MemberInfo mem in members){
            if (mem.MemberType == MemberTypes.Field){
              FieldInfo field = (FieldInfo)mem;
              JSMemberField mfield = mem as JSMemberField;
              if (mfield != null){ //This can only happen when running in the Evaluator
                if (!mfield.IsStatic){
                  JSGlobalField gfield = new JSGlobalField(this, name, mfield.value, FieldAttributes.Public);
                  this.NameTable[name] = gfield;
                  this.field_table.Add(gfield);
                  field = mfield;
                }
              }else{
                field = new JSPrototypeField(this.parent, (FieldInfo)mem);
                if (!this.noExpando){
                  this.NameTable[name] = field;
                  this.field_table.Add(field);
                }
              }
              return new MemberInfo[]{field};
            }
            if (!this.noExpando){
              if (mem.MemberType == MemberTypes.Method){
              FieldInfo field = new JSPrototypeField(this.parent,
                new JSGlobalField(this, name,
                LateBinding.GetMemberValue(this.parent, name, null, members),
                FieldAttributes.Public|FieldAttributes.InitOnly));
                this.NameTable[name] = field;
                this.field_table.Add(field);
                return new MemberInfo[]{field};
              }
            }
          }
          if (wrapMembers)
            return ScriptObject.WrapMembers(members, this.parent);
          else
            return members;
        }
        return new MemberInfo[0];
      }

      public override MemberInfo[] GetMembers(BindingFlags bindingAttr){
        MemberInfoList mems = new MemberInfoList();
        SimpleHashtable uniqueMems = new SimpleHashtable(32);
        
        if (!this.noExpando && this.field_table != null){ //Add any expando properties
          IEnumerator enu = this.field_table.GetEnumerator();
          while (enu.MoveNext()){
            FieldInfo field = (FieldInfo)enu.Current;
            mems.Add(field);
            uniqueMems[field.Name] = field;
          }
        }
        
        //Add the public members of the built-in objects if they don't already exist
        if (this.isASubClass){ 
          MemberInfo[] ilMembers = this.GetType().GetMembers(bindingAttr & ~BindingFlags.NonPublic); //Never expose non public members of old style objects
          for (int i = 0, n = ilMembers.Length; i < n; i++){
            MemberInfo ilMem = ilMembers[i];
            
            //Hide any infrastructure stuff in JSObject
            if (!ilMem.DeclaringType.IsAssignableFrom(Typeob.JSObject) && uniqueMems[ilMem.Name] == null){
              MethodInfo method = ilMem as MethodInfo;
              if (method == null || !method.IsSpecialName){
                mems.Add(ilMem);
                uniqueMems[ilMem.Name] = ilMem;
              }
            }
          }
        }
        
        //Add parent members if they don't already exist
        if (this.parent != null){
          SimpleHashtable cache = this.parent.wrappedMemberCache;
          if (cache == null)
            cache = this.parent.wrappedMemberCache = new SimpleHashtable(8);
          MemberInfo[] parentMems = ScriptObject.WrapMembers(((IReflect)this.parent).GetMembers(bindingAttr & ~BindingFlags.NonPublic), this.parent, cache);
          for(int i = 0, n = parentMems.Length; i < n; i++){
            MemberInfo parentMem = parentMems[i];
            if(uniqueMems[parentMem.Name] == null){
              mems.Add(parentMem);
              //uniqueMems[parentMem.Name] = parentMem; //No need to add to lookup table - no one else will be looking.
            }
          }
        }
        
        return mems.ToArray();
      }

      internal override void GetPropertyEnumerator(ArrayList enums, ArrayList objects){
        if (this.field_table == null) this.field_table = new ArrayList();
        enums.Add(new ListEnumerator(this.field_table));
        objects.Add(this);
        if (this.parent != null)
          this.parent.GetPropertyEnumerator(enums, objects);
      }

      internal override Object GetValueAtIndex(uint index){ //used by array functions
        String name = System.Convert.ToString(index, CultureInfo.InvariantCulture);
        //Do not defer to to routine below, since Array objects override it and could call back to this routine
        FieldInfo field = (FieldInfo)(this.NameTable[name]);
        if (field != null)
          return field.GetValue(this);
        else{
          Object result = null;
          if (this.parent != null)
            result = this.parent.GetMemberValue(name);
          else
            result = Missing.Value;
          if (this is StringObject && result == Missing.Value){
            String str = ((StringObject)this).value;
            if (index < str.Length)
              return str[(int)index];
          }
          return result;
        }
      }

#if !DEBUG
      [DebuggerStepThroughAttribute]
      [DebuggerHiddenAttribute]
#endif
      internal override Object GetMemberValue(String name){
        FieldInfo field = (FieldInfo)this.NameTable[name];
        if (field == null && this.isASubClass){
          field = this.subClassIR.GetField(name, BindingFlags.Instance|BindingFlags.Static|BindingFlags.Public);
          if (field != null){
            if (field.DeclaringType == Typeob.ScriptObject) return Missing.Value;
          }else{
            PropertyInfo prop = this.subClassIR.GetProperty(name, BindingFlags.Instance|BindingFlags.Public);
            if (prop != null && !prop.DeclaringType.IsAssignableFrom(Typeob.JSObject))
              return JSProperty.GetGetMethod(prop, false).Invoke(this, BindingFlags.SuppressChangeType, null, null, null);
            try{
              MethodInfo method = this.subClassIR.GetMethod(name, BindingFlags.Public|BindingFlags.Static);
              if (method != null){
                Type dt = method.DeclaringType;
                if (dt != Typeob.JSObject && dt != Typeob.ScriptObject && dt != Typeob.Object)
                  return new BuiltinFunction(this, method);
              }
            }catch(AmbiguousMatchException){}
          }
        }
        if (field != null)
          return field.GetValue(this);
        if (this.parent != null)
          return this.parent.GetMemberValue(name);
        return Missing.Value;
      }

      internal SimpleHashtable NameTable{
        get{
          SimpleHashtable result = this.name_table;
          if (result == null){
            this.name_table = result = new SimpleHashtable(16);
            this.field_table = new ArrayList();
          }
          return result;
        }
      }

      void IExpando.RemoveMember(MemberInfo m){
        this.DeleteMember(m.Name);
      }

#if !DEBUG
      [DebuggerStepThroughAttribute]
      [DebuggerHiddenAttribute]
#endif
      internal override void SetMemberValue(String name, Object value){
        this.SetMemberValue2(name, value);
      }

      public void SetMemberValue2(String name, Object value){
        FieldInfo field = (FieldInfo)this.NameTable[name];
        if (field == null && this.isASubClass)
          field = this.GetType().GetField(name);
        if (field == null){
          if (this.noExpando)
            return;
          field = new JSExpandoField(name);
          this.name_table[name] = field;
          this.field_table.Add(field);
        }
        if (!field.IsInitOnly && !field.IsLiteral)
          field.SetValue(this, value);
      }

      internal override void SetValueAtIndex(uint index, Object value){
        this.SetMemberValue(System.Convert.ToString(index, CultureInfo.InvariantCulture), value);
      }

      internal virtual void SwapValues(uint left, uint right){
        String left_name = System.Convert.ToString(left, CultureInfo.InvariantCulture);
        String right_name = System.Convert.ToString(right, CultureInfo.InvariantCulture);
        FieldInfo left_field = (FieldInfo)(this.NameTable[left_name]);
        FieldInfo right_field = (FieldInfo)(this.name_table[right_name]);
        if (left_field == null)
          if (right_field == null)
            return;
          else{
            this.name_table[left_name] = right_field;
            this.name_table.Remove(right_name);
          }
        else if (right_field == null){
          this.name_table[right_name] = left_field;
          this.name_table.Remove(left_name);
        }else{
          this.name_table[left_name] = right_field;
          this.name_table[right_name] = left_field;
        }
      }

      public override String ToString(){
        return Convert.ToString(this);
      }

    }
}
