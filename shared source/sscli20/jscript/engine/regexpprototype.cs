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

    public class RegExpPrototype : JSObject {
      internal static readonly RegExpPrototype ob = new RegExpPrototype(ObjectPrototype.ob);
      internal static RegExpConstructor _constructor;

      internal RegExpPrototype(ObjectPrototype parent)
        : base(parent) {
        //this.constructor is given a value by the proper constructor class
      }

      [JSFunctionAttribute(JSFunctionAttributeEnum.HasThisObject, JSBuiltin.RegExp_compile)]
      public static RegExpObject compile(Object thisob, Object source, Object flags) {
        RegExpObject regExpObject = thisob as RegExpObject;
        if (regExpObject == null)
          throw new JScriptException(JSError.RegExpExpected);
        return regExpObject.compile(
          source == null || source is Missing ? "" : Convert.ToString(source),
          flags == null || flags is Missing ? "" : Convert.ToString(flags));
      }

      public static RegExpConstructor constructor{
        get{
          return RegExpPrototype._constructor;
        }
      }
      
      [JSFunctionAttribute(JSFunctionAttributeEnum.HasThisObject, JSBuiltin.RegExp_exec)]
      public static Object exec(Object thisob, Object input) {
        RegExpObject regExpObject = thisob as RegExpObject;
        if (regExpObject == null)
          throw new JScriptException(JSError.RegExpExpected);
        if (input is Missing && !regExpObject.regExpConst.noExpando)
          input = regExpObject.regExpConst.input;
        return regExpObject.exec(Convert.ToString(input));
      }

      [JSFunctionAttribute(JSFunctionAttributeEnum.HasThisObject, JSBuiltin.RegExp_test)]
      public static bool test(Object thisob, Object input) {
        RegExpObject regExpObject = thisob as RegExpObject;
        if (regExpObject == null)
          throw new JScriptException(JSError.RegExpExpected);
        if (input is Missing && !regExpObject.regExpConst.noExpando)
          input = regExpObject.regExpConst.input;
        return regExpObject.test(Convert.ToString(input));
      }

      [JSFunctionAttribute(JSFunctionAttributeEnum.HasThisObject, JSBuiltin.RegExp_toString)]
      public static String toString(Object thisob) {
        RegExpObject regExpObject = thisob as RegExpObject;
        if (regExpObject == null)
          throw new JScriptException(JSError.RegExpExpected);
        return regExpObject.ToString();
      }
    }
}
