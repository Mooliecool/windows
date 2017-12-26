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

    // WARNING: This attribute should be sealed but isn't because we accidently shipped
    // it this way. The compiler assumes this attribute is sealed and will not respect
    // subclasses of this attribute. It doesn't follow standard naming convention for the
    // same reason.
    [AttributeUsage(AttributeTargets.Method|AttributeTargets.Field)]
    public class NotRecommended : Attribute{
      private String message;
      
      public NotRecommended(String message){
        this.message = message;
      }

      public Boolean IsError{ 
        get{
          return false;
        }
      }
      
      public String Message{
        get{
          return JScriptException.Localize(this.message, null);
        }
      }
    }
    
}   
