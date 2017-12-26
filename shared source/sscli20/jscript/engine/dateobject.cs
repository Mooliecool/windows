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

    public class DateObject : JSObject{
      internal double value;
     
      internal DateObject(ScriptObject parent, double value)
        : base(parent) {
        this.value = value != value || value > Int64.MaxValue || value < Int64.MinValue
          ? Double.NaN
          : System.Math.Round(value);
        this.noExpando = false;
      }
      
      internal override String GetClassName(){
        return "Date";
      }
      
      internal override Object GetDefaultValue(PreferredType preferred_type){
        if (this.GetParent() is LenientDatePrototype) return base.GetDefaultValue(preferred_type);
        if (preferred_type == PreferredType.String || preferred_type == PreferredType.Either){
          if (!this.noExpando){
            Object field = this.NameTable["toString"];
            if (field != null) return base.GetDefaultValue(preferred_type);
          }
          return DatePrototype.toString(this);
        }else if (preferred_type == PreferredType.LocaleString){
          if (!this.noExpando){
            Object field = this.NameTable["toLocaleString"];
            if (field != null) return base.GetDefaultValue(preferred_type);
          }
          return DatePrototype.toLocaleString(this);
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
      
    }
}
