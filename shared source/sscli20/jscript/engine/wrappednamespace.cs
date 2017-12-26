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
    
    using Microsoft.JScript.Vsa;
    using System;
    using System.Reflection;
    
    internal sealed class WrappedNamespace : ActivationObject{
      internal String name;
     
      internal WrappedNamespace(String name, VsaEngine engine)
        : this(name, engine, true) {
      }
      
      internal WrappedNamespace(String name, VsaEngine engine, bool AddReferences)
        : base(null) {
        this.name = name;
        this.engine = engine;
        this.isKnownAtCompileTime = true;
        if (name.Length > 0 && AddReferences)
          engine.TryToAddImplicitAssemblyReference(name);
      }
      
      public override MemberInfo[] GetMember(String name, BindingFlags bindingAttr){
        FieldInfo cachedfield = (FieldInfo)(this.name_table[name]);
        if (cachedfield != null) return new MemberInfo[]{cachedfield};

        FieldAttributes attrs = FieldAttributes.Literal;
        String fullname = this.name == null || this.name.Length == 0 ? name : this.name+"."+name;
        Object t = null;
        if (this.name != null && this.name.Length > 0)
          t = this.engine.GetClass(fullname); //Packages will not have classes residing in the default name space
        if (t == null){
          t = this.engine.GetType(fullname);
          if (t != null && !((Type)t).IsPublic){
            if ((bindingAttr & BindingFlags.NonPublic) == 0)
              t = null;
            else 
              attrs |= FieldAttributes.Private;
          }
        }else{
          if ((((ClassScope)t).owner.attributes&TypeAttributes.Public) == 0){
            if ((bindingAttr & BindingFlags.NonPublic) == 0)
              t = null;
            else
              attrs |= FieldAttributes.Private;
          }
        }
        if (t == null) {
          if (this.parent != null && (bindingAttr&BindingFlags.DeclaredOnly) == 0)
            return this.parent.GetMember(name, bindingAttr);
          return new MemberInfo[0];
        }

        JSGlobalField field = (JSGlobalField)this.CreateField(name, attrs, t);
        if (engine.doFast)
          field.type = new TypeExpression(new ConstantWrapper(Typeob.Type, null));
        this.name_table[name] = field;
        this.field_table.Add(field);
        return new MemberInfo[]{field};
      }
      
      public override String ToString(){
        return this.name;
      }
      
    }

}
