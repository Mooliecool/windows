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

    public class StringObject : JSObject{
      internal String value;
      private bool implicitWrapper;
     
      protected StringObject(ScriptObject prototype, String value)
        : base(prototype) {
        this.value = value;
        this.noExpando = false;
        this.implicitWrapper = false;
      }
      
      internal StringObject(ScriptObject prototype, String value, bool implicitWrapper)
        : base(prototype, typeof(StringObject)) {
        this.value = value;
        this.noExpando = implicitWrapper;
        this.implicitWrapper = implicitWrapper;
      }
      
      internal override String GetClassName(){
        return "String";
      }
      
      public int length{
        get{
          return this.value.Length;
        }
      }
      
      public override bool Equals(Object ob){
        if (ob is StringObject) ob = ((StringObject)ob).value;
        return this.value.Equals(ob);
      }
      
      internal override Object GetDefaultValue(PreferredType preferred_type){
        if (this.GetParent() is LenientStringPrototype) return base.GetDefaultValue(preferred_type);
        if (preferred_type == PreferredType.String){
          if (!this.noExpando){
            Object field = this.NameTable["toString"];
            if (field != null) return base.GetDefaultValue(preferred_type);
          }
          return this.value;
        }else if (preferred_type == PreferredType.LocaleString){
          return base.GetDefaultValue(preferred_type);
        }else{
          if (!this.noExpando){
            Object field = this.NameTable["valueOf"];
            if (field == null && preferred_type == PreferredType.Either)
              field = this.NameTable["toString"];
            if (field != null) return base.GetDefaultValue(preferred_type);
          }
          return this.value;
        }
      }
      
      public override int GetHashCode(){
        return this.value.GetHashCode();
      }
      
      public new Type GetType(){
        return this.implicitWrapper ? Typeob.String : Typeob.StringObject;
      }
      
      internal override Object GetValueAtIndex(uint index){
        if (this.implicitWrapper && index < this.value.Length)
          return this.value[(int)index];
        else
          return base.GetValueAtIndex(index);
      }
    }
}
