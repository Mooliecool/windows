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

namespace Microsoft.JScript{
    
  using System;
  using System.Reflection;

  // WARNING: This attribute should be sealed but isn't because we accidently shipped
  // it this way. The compiler assumes this attribute is sealed and will not respect
  // subclasses of this attribute.
  [AttributeUsage(AttributeTargets.Method|AttributeTargets.Constructor)]
  public class JSFunctionAttribute : Attribute{
    internal JSFunctionAttributeEnum attributeValue;
    internal JSBuiltin builtinFunction;

    public JSFunctionAttribute(JSFunctionAttributeEnum value){
      this.attributeValue = value;
      this.builtinFunction = (JSBuiltin)0;
    }

    public JSFunctionAttribute(JSFunctionAttributeEnum value, JSBuiltin builtinFunction){
      this.attributeValue = value;
      this.builtinFunction = builtinFunction;
    }

    public JSFunctionAttributeEnum GetAttributeValue(){
      return this.attributeValue;
    }
  } 

  [System.Runtime.InteropServices.GuidAttribute("BA5ED019-F669-3C35-93AC-3ABF776B62B3")]
  [System.Runtime.InteropServices.ComVisible(true)]
  [Flags()]
  public enum JSFunctionAttributeEnum{
    None            = 0x00,
    HasArguments    = 0x01,
    HasThisObject   = 0x02,
    IsNested        = 0x04,
    HasStackFrame   = 0x08, //For use by debugger to keep outer scope locals in sync with outer scope stack frame
    HasVarArgs      = 0x10,
    HasEngine       = 0x20,
    IsExpandoMethod = 0x40, //A method that is used as a constructor function
    IsInstanceNestedClassConstructor = 0x80, //A constructor of an instance nested class. Takes a trailing hidden parameter providing the nesting class instance.
    // common combinations, not actually used, but improves debugging experience.
    ClassicFunction = HasArguments | HasThisObject | HasEngine,
    NestedFunction  = HasStackFrame | IsNested | HasEngine,      
    ClassicNestedFunction = ClassicFunction | NestedFunction,
  }
}
