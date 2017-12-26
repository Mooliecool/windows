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

    public class BooleanObject : JSObject{
      internal bool value;
      private bool implicitWrapper;
     
      protected BooleanObject(ScriptObject prototype, Type subType)
        : base(prototype, subType){
        this.value = value;
        this.noExpando = false;
        this.implicitWrapper = false;
      }
      
      internal BooleanObject(ScriptObject prototype, bool value, bool implicitWrapper)
        : base(prototype, typeof(BooleanObject)) {
        this.value = value;
        this.noExpando = implicitWrapper;
        this.implicitWrapper = implicitWrapper;
      }
      
      internal override String GetClassName(){
        return "Boolean";
      }
      
      internal override Object GetDefaultValue(PreferredType preferred_type){
        if (this.GetParent() is LenientBooleanPrototype) return base.GetDefaultValue(preferred_type);
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
      
      public new Type GetType(){
        return this.implicitWrapper ? Typeob.Boolean : Typeob.BooleanObject;
      }
    }
}
