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
    
    public class EnumeratorObject : JSObject{
      private IEnumerable collection;
      protected IEnumerator enumerator;      
      private Object obj;
      
      internal EnumeratorObject(EnumeratorPrototype parent)
        : base(parent) {
        this.enumerator = null;
        this.collection = null;
        this.noExpando = false;
      }
      
      internal EnumeratorObject(EnumeratorPrototype parent, IEnumerable collection)
        : base(parent) {
        this.collection = collection;
        if (collection != null)
          this.enumerator = collection.GetEnumerator();
        LoadObject();
        this.noExpando = false;
      }
      
      internal virtual bool atEnd(){
        if (this.enumerator != null)
          return this.obj == null;
        else
          return true;
      }

      internal virtual Object item(){
        if (this.enumerator != null)
          return this.obj;
        else
          return null;
      }

      protected void LoadObject(){
        if (this.enumerator != null && this.enumerator.MoveNext())
          this.obj = this.enumerator.Current;
        else
          this.obj = null;
      }
      
      internal virtual void moveFirst(){
        if (this.collection != null)
          this.enumerator = this.collection.GetEnumerator();
        LoadObject();
      }

      internal virtual void moveNext(){
        if (this.enumerator != null)
          LoadObject();
      }

    }
    
}
