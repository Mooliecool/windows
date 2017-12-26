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

    public class EnumeratorPrototype : JSObject{
      internal static readonly EnumeratorPrototype ob = new EnumeratorPrototype(ObjectPrototype.ob);
      internal static EnumeratorConstructor _constructor; 
        
      internal EnumeratorPrototype(ObjectPrototype parent)
        : base(parent) {
        //this.constructor is given a value by the constructor class
      }
    
      [JSFunctionAttribute(JSFunctionAttributeEnum.HasThisObject, JSBuiltin.Enumerator_atEnd)]
      public static bool atEnd(Object thisob){
        if (thisob is EnumeratorObject)
          return ((EnumeratorObject)thisob).atEnd();
        else
          throw new JScriptException(JSError.EnumeratorExpected);
      }
      
      public static EnumeratorConstructor constructor{
        get{
          return EnumeratorPrototype._constructor;
        }
      }
      
      [JSFunctionAttribute(JSFunctionAttributeEnum.HasThisObject, JSBuiltin.Enumerator_item)]
      public static Object item(Object thisob){
        if (thisob is EnumeratorObject)
          return ((EnumeratorObject)thisob).item();
        else
          throw new JScriptException(JSError.EnumeratorExpected);
      }
      
      [JSFunctionAttribute(JSFunctionAttributeEnum.HasThisObject, JSBuiltin.Enumerator_moveFirst)]
      public static void moveFirst(Object thisob){
        if (thisob is EnumeratorObject)
          ((EnumeratorObject)thisob).moveFirst();
        else
          throw new JScriptException(JSError.EnumeratorExpected);
      }
      
      [JSFunctionAttribute(JSFunctionAttributeEnum.HasThisObject, JSBuiltin.Enumerator_moveNext)]
      public static void moveNext(Object thisob){
        if (thisob is EnumeratorObject)
          ((EnumeratorObject)thisob).moveNext();
        else
          throw new JScriptException(JSError.EnumeratorExpected);
      }
    }
}
