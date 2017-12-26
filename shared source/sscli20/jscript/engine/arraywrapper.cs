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
  using System.Reflection;
  using System.Diagnostics;
    
  public class ArrayWrapper : ArrayObject{

    internal Array value;
    private bool implicitWrapper;
    
    internal ArrayWrapper(ScriptObject prototype, Array value, bool implicitWrapper)
      : base(prototype, typeof(ArrayWrapper)) {
      this.value = value;
      this.implicitWrapper = implicitWrapper;
      if (value != null){
        if (value.Rank != 1)
          throw new JScriptException(JSError.TypeMismatch);
        this.len = (uint) value.Length;
      }else
        this.len = 0;
    }

    internal override void Concat(ArrayObject source){
      throw new JScriptException(JSError.ActionNotSupported);
    }

    internal override void Concat(Object value){
      throw new JScriptException(JSError.ActionNotSupported);
    }

    internal override void GetPropertyEnumerator(ArrayList enums, ArrayList objects){
      enums.Add(new ArrayEnumerator(this, new RangeEnumerator(0, (int)this.len - 1)));
      objects.Add(this);
      if (this.parent != null)
        this.parent.GetPropertyEnumerator(enums, objects);
    }

    public new Type GetType(){
      return this.implicitWrapper ? this.value.GetType() : typeof(ArrayObject);
    }
    
    internal override Object GetValueAtIndex(uint index){
      checked { return this.value.GetValue((int)index); }
    }

    public override Object length{
      get{
        return this.len;
      }
      set{
        throw new JScriptException(JSError.AssignmentToReadOnly);
      }
    }
    
#if !DEBUG
      [DebuggerStepThroughAttribute]
      [DebuggerHiddenAttribute]
#endif      
    internal override void SetMemberValue(String name, Object val){
      if (name.Equals("length"))
        throw new JScriptException(JSError.AssignmentToReadOnly);
      long index = ArrayObject.Array_index_for(name);
      if (index < 0)
        base.SetMemberValue(name, val);
      else
        this.value.SetValue(val, (int)index);
    }

    internal override void SetValueAtIndex(uint index, Object val){
      Type arrayType = this.value.GetType();
      checked { this.value.SetValue(Convert.CoerceT(val, arrayType.GetElementType()), (int)index); }
    }


    internal override Object Shift(){
      throw new JScriptException(JSError.ActionNotSupported);
    }

    internal override void Splice(uint start, uint deleteCount, Object[] args, ArrayObject outArray, uint oldLength, uint newLength){
      if (oldLength != newLength)
        throw new JScriptException(JSError.ActionNotSupported); 
      SpliceSlowly(start, deleteCount, args, outArray, oldLength, newLength);
    }

    internal override void Sort(ScriptFunction compareFn){
      SortComparer sc = new SortComparer(compareFn);
      System.Array.Sort(this.value, sc);
    }
      
    internal override void SwapValues(uint pi, uint qi){
      Object po = this.GetValueAtIndex(pi);
      Object qo = this.GetValueAtIndex(qi);
      this.SetValueAtIndex(pi, qo);
      this.SetValueAtIndex(qi, po);
    }
    
    internal override Array ToNativeArray(Type elementType){
      // Ignore the type -- if there is a mismatch, the caller will throw.
      return value;
    }

    internal override Object[] ToArray(){
      Object[] result;
      checked { result = new Object[(int)this.len]; }
      for (uint i = 0; i < this.len; i++)
        result[i] = this.GetValueAtIndex(i);
      return result;
    }
    
    internal override ArrayObject Unshift(Object[] args){
      throw new JScriptException(JSError.ActionNotSupported);
    }

    internal sealed class SortComparer : IComparer{
      internal ScriptFunction compareFn;
      internal SortComparer(ScriptFunction compareFn){
        this.compareFn = compareFn;
      }
      public int Compare(Object x, Object y){
        if (x == null || x is Missing)
          if (y == null || y is Missing)
            return 0;
          else 
            return 1;
        else if (y == null || y is Missing)
          return -1;
        if (this.compareFn != null){
          double result = Convert.ToNumber(this.compareFn.Call(new Object[]{x, y}, null));
          if (result != result)
            throw new JScriptException(JSError.NumberExpected);
          return (int)Runtime.DoubleToInt64(result);
        }else
          return String.CompareOrdinal(Convert.ToString(x), Convert.ToString(y));
      }
    }
  }
}
