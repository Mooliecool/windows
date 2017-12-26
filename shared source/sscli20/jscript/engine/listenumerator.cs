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

    internal class ListEnumerator : IEnumerator{
      private int curr;
      private ArrayList list;
      
      internal ListEnumerator(ArrayList list){
        this.curr = -1;
        this.list = list;
      }
      
      public virtual bool MoveNext(){
        return ++this.curr < this.list.Count;
      }
     
      public virtual Object Current{
        get{
          return this.list[this.curr];
        }
      }
      
      public virtual void Reset(){
        this.curr = -1;
      }
    }
}
