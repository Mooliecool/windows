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
    using System.Security.Permissions;
    using System.Runtime.Serialization;

    [Serializable]
    public sealed class ContinueOutOfFinally : ApplicationException{
      public int target;
    
      public ContinueOutOfFinally() : this(0) {
      }
      
      public ContinueOutOfFinally(int target){
        this.target = target;
      }
      
      public ContinueOutOfFinally(string m) : base(m) {
      }
      
      public ContinueOutOfFinally(string m, Exception e) : base(m, e) {
      }
      
      private ContinueOutOfFinally(SerializationInfo s, StreamingContext c) : base(s, c) {
        this.target = s.GetInt32("Target");
      }
      
      [SecurityPermissionAttribute(SecurityAction.Demand, SerializationFormatter=true)]
      public override void GetObjectData(SerializationInfo s, StreamingContext c)
      {
        base.GetObjectData(s, c);
        s.AddValue("Target", this.target);      
      }
    }
}
