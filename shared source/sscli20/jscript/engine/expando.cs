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
  // subclasses of this attribute. It doesn't follow standard naming convention for the
  // same reason.
  [AttributeUsage(AttributeTargets.Class|AttributeTargets.Method, Inherited=true)]
  public class Expando : Attribute{
    public Expando(){}
  } 

}
