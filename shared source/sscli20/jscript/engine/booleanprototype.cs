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

    public class BooleanPrototype : BooleanObject{
      internal static readonly BooleanPrototype ob = new BooleanPrototype(ObjectPrototype.ob, typeof(BooleanPrototype));
      internal static BooleanConstructor _constructor; 
     
      protected BooleanPrototype(ObjectPrototype parent, Type baseType)
        : base(parent, baseType) {
        this.noExpando = true;
      }
      
      public static BooleanConstructor constructor{
        get{
          return BooleanPrototype._constructor;
        }
      }
    
      [JSFunctionAttribute(JSFunctionAttributeEnum.HasThisObject, JSBuiltin.Boolean_toString)]
      public static String toString(Object thisob){
        if (thisob is BooleanObject)
          return Convert.ToString(((BooleanObject)thisob).value);
        else if (Convert.GetTypeCode(thisob) == TypeCode.Boolean)
          return Convert.ToString(thisob);
        throw new JScriptException(JSError.BooleanExpected);
      }
    
      [JSFunctionAttribute(JSFunctionAttributeEnum.HasThisObject, JSBuiltin.Boolean_valueOf)]
      public static Object valueOf(Object thisob){
        if (thisob is BooleanObject)
          return ((BooleanObject)thisob).value;
        else if (Convert.GetTypeCode(thisob) == TypeCode.Boolean)
          return thisob;
        throw new JScriptException(JSError.BooleanExpected);
      }
    
    }
}
