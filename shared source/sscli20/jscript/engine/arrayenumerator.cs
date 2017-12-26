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

/*
The indices in the dense part of an ArrayObject do not show up as properties in the field table of the object. This enumerator includes them in the enumeration.
It is complicated by the fact that properties can be added to both the dense part and the field table as the enumeration proceeds.

Another, major complication is that prior versions of JS enumerate properties in the order they were added to the object, and many test suites depend on this order.

When an array is stored densely, it does not make sense to retain the order in which indices were added to the array, merely to make enumerations work exactly as
they always have. However, some test at least, work if the numerical indices are listed before other properties, but numerical indices that are added after
other properties, are listed afterwards.
*/

namespace Microsoft.JScript {
    
    using System;
    using System.Collections;
    using System.Reflection;
    using System.Globalization;

    internal class ArrayEnumerator : IEnumerator{
      private int curr;
      private bool doDenseEnum;
      private bool didDenseEnum;
      private ArrayObject arrayOb;
      private IEnumerator denseEnum;
      
      internal ArrayEnumerator(ArrayObject arrayOb, IEnumerator denseEnum){
        this.curr = -1;
        this.doDenseEnum = false;
        this.didDenseEnum = false;
        this.arrayOb = arrayOb;
        this.denseEnum = denseEnum;
      }
      
      public virtual bool MoveNext(){
        if (this.doDenseEnum)
          if (this.denseEnum.MoveNext())
            return true;
          else{
            this.doDenseEnum = false;
            this.didDenseEnum = true;
          }
        int next = this.curr+1;
        if (next >= this.arrayOb.len || next >= this.arrayOb.denseArrayLength){
          this.doDenseEnum = !this.didDenseEnum;
          return this.denseEnum.MoveNext();
        }
        this.curr = next;
        if (this.arrayOb.GetValueAtIndex((uint)next) is Missing)
          return this.MoveNext();
        return true;
      }
      
      public virtual Object Current{ //Always returns a String
        get{
          if (this.doDenseEnum)
            return this.denseEnum.Current;
          if (this.curr >= this.arrayOb.len || this.curr >= this.arrayOb.denseArrayLength)
            return this.denseEnum.Current;
          else
            return this.curr.ToString(CultureInfo.InvariantCulture);
        }
      }
      
      public virtual void Reset(){
        this.curr = -1;
        this.doDenseEnum = false;
        this.didDenseEnum = false;
        this.denseEnum.Reset();
      }
    }
}
