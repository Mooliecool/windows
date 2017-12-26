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
    using System.Collections;

    internal class RangeEnumerator : IEnumerator{
      private int curr;
      private int start;
      private int stop;
      
      internal RangeEnumerator(int start, int stop){
        this.curr = start-1;
        this.start = start;
        this.stop = stop;
      }
      
      public virtual bool MoveNext(){
        return ++this.curr <= this.stop;
      }
     
      public virtual Object Current{
        get{
          return this.curr;
        }
      }
      
      public virtual void Reset(){
        this.curr = this.start;
      }
    }
}
