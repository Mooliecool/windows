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

    public class NumberObject : JSObject{
      internal Type baseType;
      internal Object value;
      private bool implicitWrapper;
     
      protected NumberObject(ScriptObject parent, Object value)
        : base(parent) {
        this.baseType = Globals.TypeRefs.ToReferenceContext(value.GetType());
        this.value = value;
        this.noExpando = false;
        this.implicitWrapper = false;
      }
      
      internal NumberObject(ScriptObject parent, Object value, bool implicitWrapper)
        : base(parent, typeof(NumberObject)) {
        this.baseType = Globals.TypeRefs.ToReferenceContext(value.GetType());
        this.value = value;
        this.noExpando = implicitWrapper;
        this.implicitWrapper = implicitWrapper;
      }

      internal NumberObject(ScriptObject parent, Type baseType)
        : base(parent) {
        this.baseType = baseType;
        this.value = 0.0;
        this.noExpando = false;
      }
      
      internal override Object GetDefaultValue(PreferredType preferred_type){
        if (this.GetParent() is LenientNumberPrototype) return base.GetDefaultValue(preferred_type);
        if (preferred_type == PreferredType.String){
          if (!this.noExpando){
            Object field = this.NameTable["toString"];
            if (field != null) return base.GetDefaultValue(preferred_type);
          }
          return Convert.ToString(this.value);
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
      
      internal override String GetClassName(){
        return "Number";
      }
      
      public new Type GetType(){
        return this.implicitWrapper ? this.baseType : Typeob.NumberObject;
      }
    }
    
}
