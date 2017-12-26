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

    public class ErrorPrototype : JSObject{
      public readonly String name;
    
      internal static readonly ErrorPrototype ob = new ErrorPrototype(ObjectPrototype.ob, "Error");
      internal ErrorConstructor _constructor; 
     
      internal ErrorPrototype(ScriptObject parent, String name)
        : base(parent) {
        this.name = name;
        //this.constructor is given a value by the constructor class
      }
    
      public ErrorConstructor constructor{
        get{
          return this._constructor;
        }
      }
      
      [JSFunctionAttribute(JSFunctionAttributeEnum.HasThisObject, JSBuiltin.Error_toString)]
      public static String toString(Object thisob){
        if (thisob is ErrorObject){
          String message = ((ErrorObject)thisob).Message;
          if (message.Length == 0)
            return LateBinding.GetMemberValue(thisob, "name").ToString();
          else
            return LateBinding.GetMemberValue(thisob, "name").ToString() + ": " + message;
        }
        return thisob.ToString();
      }
    
    }
}
