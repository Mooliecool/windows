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

    using Microsoft.JScript.Vsa;
    using System;
    using System.Reflection;

    public class ObjectPrototype : JSObject{
      internal static readonly ObjectPrototype ob = new ObjectPrototype();
      internal static ObjectConstructor _constructor;

      internal ObjectPrototype()
        : base(null) {
        //this.constructor is given a value by the proper constructor class
        if (Globals.contextEngine == null){
          this.engine = new VsaEngine(true); //Get here if the class is loaded before an engine is created (ASP+ case).
          this.engine.InitVsaEngine("JS7://Microsoft.JScript.Vsa.VsaEngine", new DefaultVsaSite());
        }
        else
          this.engine = Globals.contextEngine;
      }
      
      public static ObjectConstructor constructor{
        get{
          return ObjectPrototype._constructor;
        }
      }

      internal static ObjectPrototype CommonInstance(){
        return ObjectPrototype.ob;
      }

      [JSFunctionAttribute(JSFunctionAttributeEnum.HasThisObject, JSBuiltin.Object_hasOwnProperty)]
      public static bool hasOwnProperty(Object thisob, Object name){
        String nameStr = Convert.ToString(name);
        if (thisob is ArrayObject) {
          long index = ArrayObject.Array_index_for(nameStr);
          if (index >= 0) {
            Object ob = ((ArrayObject)thisob).GetValueAtIndex((uint)index);
            return ob != null && ob != Missing.Value;
          }
        }
        if (thisob is JSObject){
          MemberInfo[] members = ((JSObject)thisob).GetMember(nameStr, BindingFlags.Public|BindingFlags.Instance|BindingFlags.Static|BindingFlags.DeclaredOnly);
          int n = members.Length;
          if (n > 1) return true;
          if (n < 1) return false;
          if (members[0] is JSPrototypeField){
            //Do not count it as an "own" property unless it has been written to
            return !(((JSPrototypeField)members[0]).value is Missing);
          }
          return true;
        }
        return !(LateBinding.GetMemberValue(thisob, nameStr) is Missing);
      }

      [JSFunctionAttribute(JSFunctionAttributeEnum.HasThisObject, JSBuiltin.Object_isPrototypeOf)]
      public static bool isPrototypeOf(Object thisob, Object ob){
        if (thisob is ScriptObject && ob is ScriptObject)
          while (ob != null) {
            if (ob == thisob)
              return true;
            ob = ((ScriptObject)ob).GetParent();
          }
        return false;
      }

      [JSFunctionAttribute(JSFunctionAttributeEnum.HasThisObject, JSBuiltin.Object_propertyIsEnumerable)]
      public static bool propertyIsEnumerable(Object thisob, Object name){
        String nameStr = Convert.ToString(name);
        if (thisob is ArrayObject) {
          long index = ArrayObject.Array_index_for(nameStr);
          if (index >= 0) {
            Object ob = ((ArrayObject)thisob).GetValueAtIndex((uint)index);
            return ob != null && ob != Missing.Value;
          }
        }
        if (thisob is JSObject){
          FieldInfo f = ((JSObject)thisob).GetField(nameStr, BindingFlags.Public|BindingFlags.Instance|BindingFlags.Static|BindingFlags.DeclaredOnly);
          return f != null && f is JSExpandoField;
        }
        return false;
      }

      [JSFunctionAttribute(JSFunctionAttributeEnum.HasThisObject, JSBuiltin.Object_toLocaleString)]
      public static String toLocaleString(Object thisob){
        return Convert.ToString(thisob);
      }

      [JSFunctionAttribute(JSFunctionAttributeEnum.HasThisObject, JSBuiltin.Object_toString)]
      public static String toString(Object thisob){
        if (thisob is JSObject)
          return "[object "+((JSObject)thisob).GetClassName()+"]";
        else
          return "[object "+thisob.GetType().Name+"]";
      }

      [JSFunctionAttribute(JSFunctionAttributeEnum.HasThisObject, JSBuiltin.Object_valueOf)]
      public static Object valueOf(Object thisob){
        return thisob;
      }
    }
}
