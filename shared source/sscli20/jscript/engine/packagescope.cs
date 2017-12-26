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
    using System.Globalization;
    
    internal class PackageScope : ActivationObject{
      internal String name;
      internal Package owner;
      
      public PackageScope(ScriptObject parent)
        : base(parent) {
        this.fast = true;
        this.name = null;
        this.owner = null;
        this.isKnownAtCompileTime = true;
      }
      
      internal override JSVariableField AddNewField(String name, Object value, FieldAttributes attributeFlags){
        base.AddNewField(this.name+"."+name, value, attributeFlags);
        return base.AddNewField(name, value, attributeFlags);
      }
      
      internal void AddOwnName(){
        String root = this.name;
        int i = root.IndexOf('.');
        if (i > 0)
          root = root.Substring(0, i);
        base.AddNewField(root, Namespace.GetNamespace(root, this.engine), FieldAttributes.Literal|FieldAttributes.Public);
      }
      
      protected override JSVariableField CreateField(String name, FieldAttributes attributeFlags, Object value){
        return new JSGlobalField(this, name, value, attributeFlags);
      }
      
      internal override String GetName(){
        return this.name;
      }
      
      internal void MergeWith(PackageScope p){
        foreach (Object f in p.field_table){
          JSGlobalField field = (JSGlobalField)f;
          ClassScope csc = field.value as ClassScope;
          if (this.name_table[field.Name] != null){
            if (csc != null){
              csc.owner.context.HandleError(JSError.DuplicateName, field.Name, true);
              csc.owner.name += p.GetHashCode().ToString(CultureInfo.InvariantCulture);
            }
            continue;
          }
          this.field_table.Add(field);
          this.name_table[field.Name] = field;
          if (csc != null){
            csc.owner.enclosingScope = this;
            csc.package = this;
          }
        }
      }
    }
}
