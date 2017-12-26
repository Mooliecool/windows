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

    public class FunctionPrototype : ScriptFunction{
      internal static readonly FunctionPrototype ob = new FunctionPrototype(ObjectPrototype.CommonInstance());
      internal static FunctionConstructor _constructor; 
      
      internal FunctionPrototype(ScriptObject parent)
        : base(parent){
        //this.constructor is given a value by the constructor class
      }
      
      internal override Object Call(Object[] args, Object thisob){
        return null;
      }
      
      [JSFunctionAttribute(JSFunctionAttributeEnum.HasThisObject, JSBuiltin.Function_apply)]
      public static Object apply(Object thisob, Object thisarg, Object argArray){
        if (!(thisob is ScriptFunction))
          throw new JScriptException(JSError.FunctionExpected);
        if (thisarg is Missing)
          thisarg = ((IActivationObject)((ScriptFunction)thisob).engine.ScriptObjectStackTop()).GetDefaultThisObject();
        if (argArray is Missing)
          return ((ScriptFunction)thisob).Call(new Object[0], thisarg);
        if (argArray is ArgumentsObject)
          return ((ScriptFunction)thisob).Call(((ArgumentsObject)argArray).ToArray(), thisarg);
        if (argArray is ArrayObject)
          return ((ScriptFunction)thisob).Call(((ArrayObject)argArray).ToArray(), thisarg);
        throw new JScriptException(JSError.InvalidCall);
      }
      
      [JSFunctionAttribute(JSFunctionAttributeEnum.HasThisObject|JSFunctionAttributeEnum.HasVarArgs, JSBuiltin.Function_call)]
      public static Object call(Object thisob, Object thisarg, params Object[] args){
        if (!(thisob is ScriptFunction))
          throw new JScriptException(JSError.FunctionExpected); 
        if (thisarg is Missing)
          thisarg = ((IActivationObject)((ScriptFunction)thisob).engine.ScriptObjectStackTop()).GetDefaultThisObject();
        return ((ScriptFunction)thisob).Call(args, thisarg);
      }
      
      public static FunctionConstructor constructor{
        get{
          return FunctionPrototype._constructor;
        }
      }
      
      [JSFunctionAttribute(JSFunctionAttributeEnum.HasThisObject, JSBuiltin.Function_toString)]
      public static String toString(Object thisob){
        if (thisob is ScriptFunction)
          return thisob.ToString();
        else{
          throw new JScriptException(JSError.FunctionExpected); 
        }
      }  
    }
}
