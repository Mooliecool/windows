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

//Nothing more than a box for a value. Used to store the values of expando properties.
//Kept lightweight because there are many of them and they are not used at compile time.

namespace Microsoft.JScript {
    
    using System;
    using System.Globalization;
    using System.Reflection;
    
    internal sealed class JSExpandoField : JSField{
      private Object value;
      private String name;
      
      internal JSExpandoField(String name)
        : this(name, null){
      }
        
      internal JSExpandoField(String name, Object value){
        this.value = value;
        this.name = name;
      }
            
      public override FieldAttributes Attributes{
        get{
          return FieldAttributes.Public|FieldAttributes.Static;
        }
      }
       
      public override Object GetValue(Object obj){
        return this.value;
      }
      
      public override String Name{
        get{
          return this.name;
        }
      }
    
      public override void SetValue(Object obj, Object value, BindingFlags invokeAttr, Binder binder, CultureInfo locale){
        this.value = value;
      }
    }
}
