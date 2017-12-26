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
    using System.Runtime.Serialization;

    [Serializable]
    public sealed class ReturnOutOfFinally : ApplicationException{
    
      public ReturnOutOfFinally(){
      }
      
      public ReturnOutOfFinally(string m) : base(m) {
      }
      
      public ReturnOutOfFinally(string m, Exception e) : base(m, e) {
      }
      
      private ReturnOutOfFinally(SerializationInfo s, StreamingContext c) : base(s, c) {
      }
    }
}
