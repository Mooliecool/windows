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

    public class VBArrayPrototype : JSObject{
      internal static readonly VBArrayPrototype ob = new VBArrayPrototype(FunctionPrototype.ob, ObjectPrototype.ob);
      internal static VBArrayConstructor _constructor; 
      
      internal VBArrayPrototype(FunctionPrototype funcprot, ObjectPrototype parent)
        : base(parent) {
        //this.constructor is given a value by the constructor class
      }
    
      public static VBArrayConstructor constructor{
        get{
          return VBArrayPrototype._constructor;
        }
      }
      
      [JSFunctionAttribute(JSFunctionAttributeEnum.HasThisObject, JSBuiltin.VBArray_dimensions)]
      public static int dimensions(Object thisob){
        if (thisob is VBArrayObject)
          return ((VBArrayObject)thisob).dimensions();
        else
          throw new JScriptException(JSError.VBArrayExpected);
      }
      
      [JSFunctionAttribute(JSFunctionAttributeEnum.HasThisObject|JSFunctionAttributeEnum.HasVarArgs, JSBuiltin.VBArray_getItem)]
      public static Object getItem(Object thisob, params Object[] args){
        if (thisob is VBArrayObject)
          return ((VBArrayObject)thisob).getItem(args);
        else
          throw new JScriptException(JSError.VBArrayExpected);
      }
      
      [JSFunctionAttribute(JSFunctionAttributeEnum.HasThisObject, JSBuiltin.VBArray_lbound)]
      public static int lbound(Object thisob, Object dimension){
        if (thisob is VBArrayObject)
          return ((VBArrayObject)thisob).lbound(dimension);
        else
          throw new JScriptException(JSError.VBArrayExpected);
      }
      
      [JSFunctionAttribute(JSFunctionAttributeEnum.HasThisObject|JSFunctionAttributeEnum.HasEngine, JSBuiltin.VBArray_toArray)]
      public static ArrayObject toArray(Object thisob, VsaEngine engine){
        if (thisob is VBArrayObject)
          return ((VBArrayObject)thisob).toArray(engine);
        else
          throw new JScriptException(JSError.VBArrayExpected);
      }
      
      [JSFunctionAttribute(JSFunctionAttributeEnum.HasThisObject, JSBuiltin.VBArray_ubound)]
      public static int ubound(Object thisob, Object dimension){
        if (thisob is VBArrayObject)
          return ((VBArrayObject)thisob).ubound(dimension);
        else
          throw new JScriptException(JSError.VBArrayExpected);
      }
    }
}
