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
    using System.Globalization;
    
    public class ArrayObject : JSObject{
      internal const int MaxIndex = 100000;
      internal const int MinDenseSize = 128;
    
      internal uint len;
      internal Object[] denseArray;
      internal uint denseArrayLength;
      
      internal ArrayObject(ScriptObject prototype)
        : base(prototype){
        this.len = 0;
        this.denseArray = null;
        this.denseArrayLength = 0;
        this.noExpando = false;
      }
      
      internal ArrayObject(ScriptObject prototype, Type subType)
        : base(prototype, subType) {
        this.len = 0;
        this.denseArray = null;
        this.denseArrayLength = 0;
        this.noExpando = false;
      }
      
      internal static long Array_index_for(Object index){
        if (index is Int32) return (int)index;
        IConvertible ic = Convert.GetIConvertible(index);
        switch (Convert.GetTypeCode(index, ic)){
          case TypeCode.Char:
          case TypeCode.SByte:
          case TypeCode.Byte:
          case TypeCode.Int16:
          case TypeCode.UInt16:
          case TypeCode.Int32:
          case TypeCode.UInt32:
          case TypeCode.Int64:
          case TypeCode.UInt64:
          case TypeCode.Decimal:
          case TypeCode.Single:
          case TypeCode.Double:
            double d = ic.ToDouble(null);
            long l = (long)d;
            if (l >= 0 && (double)l == d)
              return l;
            break;
        }
        return -1;
      }

      internal static long Array_index_for(String name){
        int len = name.Length;
        if (len <= 0) return -1;
        Char ch = name[0];
        if (ch < '1' || ch > '9')
          if (ch == '0' && len == 1)
            return 0;
          else
            return -1;
        long result = ch - '0';
        for (int i = 1; i < len; i++){
          ch = name[i];
          if (ch < '0' || ch > '9') return -1;
          result = result*10 + (ch - '0');
          if (result > UInt32.MaxValue) return -1;
        }
        return result;
      }
    
      internal virtual void Concat(ArrayObject source){
        uint sourceLength = source.len;
        if (sourceLength == 0)
          return;
        uint oldLength = this.len;
        this.SetLength(oldLength + (ulong)sourceLength);
        uint slen = sourceLength;
        if (!(source is ArrayWrapper) && sourceLength > source.denseArrayLength) 
          slen = source.denseArrayLength;
        uint j = oldLength;
        for (uint i = 0; i < slen; i++)
          this.SetValueAtIndex(j++, source.GetValueAtIndex(i));
        if (slen == sourceLength) return;
        //Iterate over the sparse indices of source
        IDictionaryEnumerator e = source.NameTable.GetEnumerator();
        while (e.MoveNext()){
          long i = ArrayObject.Array_index_for(e.Key.ToString());
          if (i >= 0)
            this.SetValueAtIndex(oldLength+(uint)i, ((JSField)e.Value).GetValue(null));
        }
      }
      
      internal virtual void Concat(Object value){
        Array arr = value as Array;
        if (arr != null && arr.Rank == 1)
          this.Concat(new ArrayWrapper(ArrayPrototype.ob, arr, true));
        else {
          uint oldLength = this.len;
          this.SetLength(1 + (ulong)oldLength);
          this.SetValueAtIndex(oldLength, value);
        }
      }
      
      internal override bool DeleteMember(String name){
        long i = ArrayObject.Array_index_for(name);
        if (i >= 0)
          return this.DeleteValueAtIndex((uint)i);
        else
          return base.DeleteMember(name);
      }
      
      internal virtual bool DeleteValueAtIndex(uint index){
        if (index < this.denseArrayLength)
          if (this.denseArray[(int)index] is Missing)
            return false;
          else{
            this.denseArray[(int)index] = Missing.Value;
            return true;
          }
        else
          return base.DeleteMember(index.ToString(CultureInfo.InvariantCulture));
      }
      
      private void DeleteRange(uint start, uint end){
        uint denseEnd = this.denseArrayLength;
        if (denseEnd > end)
          denseEnd = end;
        for (; start < denseEnd; start++)
          denseArray[(int)start] = Missing.Value;
        if (denseEnd == end) return;
        //Go through the field table entries, deleting those with names that are indices between start and end  
        IDictionaryEnumerator e = this.NameTable.GetEnumerator();
        ArrayList arr = new ArrayList(this.name_table.count);
        while (e.MoveNext()){
          long i = ArrayObject.Array_index_for(e.Key.ToString());
          if (i >= start && i <= end)
            arr.Add(e.Key);
        }
        IEnumerator ae = arr.GetEnumerator();
        while (ae.MoveNext())
          this.DeleteMember((String)ae.Current);
      }
      
      internal override String GetClassName(){
        return "Array";
      }
    
      internal override Object GetDefaultValue(PreferredType preferred_type){
        if (this.GetParent() is LenientArrayPrototype) return base.GetDefaultValue(preferred_type);
        if (preferred_type == PreferredType.String){
          if (!this.noExpando){
            Object field = this.NameTable["toString"];
            if (field != null) return base.GetDefaultValue(preferred_type);
          }
          return ArrayPrototype.toString(this);
        }else if (preferred_type == PreferredType.LocaleString){
          if (!this.noExpando){
            Object field = this.NameTable["toLocaleString"];
            if (field != null) return base.GetDefaultValue(preferred_type);
          }
          return ArrayPrototype.toLocaleString(this);
        }else{
          if (!this.noExpando){
            Object field = this.NameTable["valueOf"];
            if (field == null && preferred_type == PreferredType.Either)
              field = this.NameTable["toString"];
            if (field != null) return base.GetDefaultValue(preferred_type);
          }
          return ArrayPrototype.toString(this);
        }
      }
      
      
      internal override void GetPropertyEnumerator(ArrayList enums, ArrayList objects){
        if (this.field_table == null) this.field_table = new ArrayList();
        enums.Add(new ArrayEnumerator(this, new ListEnumerator(this.field_table)));
        objects.Add(this);
        if (this.parent != null)
          this.parent.GetPropertyEnumerator(enums, objects);
      }
      
      internal override Object GetValueAtIndex(uint index){
        if (index < this.denseArrayLength){
          Object result = this.denseArray[(int)index];
          if (result != Missing.Value)
            return result;
        }
        return base.GetValueAtIndex(index);
      }
    
#if !DEBUG
      [DebuggerStepThroughAttribute]
      [DebuggerHiddenAttribute]
#endif      
      internal override Object GetMemberValue(String name){
        long index = ArrayObject.Array_index_for(name);
        if (index < 0)
          return base.GetMemberValue(name);
        else
          return this.GetValueAtIndex((uint)index);
      }
    
      public virtual Object length{
        get{
          //Convert the length from a uint to either an int or a double. The latter two are special cased in arith operations.
          if (this.len < Int32.MaxValue)
            return (int)this.len;
          else
            return (double)this.len;
        }
        set{
          IConvertible ic = Convert.GetIConvertible(value);
          uint newLength = Convert.ToUint32(value, ic);
          if ((double)newLength != Convert.ToNumber(value, ic))
            throw new JScriptException(JSError.ArrayLengthAssignIncorrect);
          this.SetLength(newLength);
        }
      }
    
      private void Realloc(uint newLength){
        Debug.PreCondition(this.denseArrayLength >= this.len);
        Debug.PreCondition(newLength <= ArrayObject.MaxIndex);
        uint oldDenseLength = this.denseArrayLength;
        uint newDenseLength = oldDenseLength*2;
        if (newDenseLength < newLength) newDenseLength = newLength;
        Object[] newArray = new Object[(int)newDenseLength];
        if (oldDenseLength > 0)
          ArrayObject.Copy(this.denseArray, newArray, (int)oldDenseLength);
        for (int i = (int)oldDenseLength; i < newDenseLength; i++)
          newArray[i] = Missing.Value;
        this.denseArray = newArray;
        this.denseArrayLength = newDenseLength;
      }
      
      private void SetLength(ulong newLength){
        uint oldLength = this.len;
        if (newLength < oldLength)
          this.DeleteRange((uint)newLength, oldLength);
        else if (newLength > UInt32.MaxValue)
          throw new JScriptException(JSError.ArrayLengthAssignIncorrect);
        else if (newLength > this.denseArrayLength && oldLength <= this.denseArrayLength //The array is dense, try to keep it dense
        && newLength <= ArrayObject.MaxIndex //Small enough to keep dense
        && (newLength <= ArrayObject.MinDenseSize || newLength <= oldLength * 2)) //Close enough to existing dense part
          this.Realloc((uint)newLength);
        this.len = (uint)newLength;
      }
        
#if !DEBUG
      [DebuggerStepThroughAttribute]
      [DebuggerHiddenAttribute]
#endif      
     internal override void SetMemberValue(String name, Object value){
        if (name.Equals("length")){
          this.length = value;
          return;
        }
        long index = ArrayObject.Array_index_for(name);
        if (index < 0)
          base.SetMemberValue(name, value);
        else
          this.SetValueAtIndex((uint)index, value);
      }
      
      internal override void SetValueAtIndex(uint index, Object value){
        if (index >= this.len && index < UInt32.MaxValue)
          this.SetLength(index + 1);
        if (index < this.denseArrayLength)
          this.denseArray[(int)index] = value;
        else
          base.SetMemberValue(index.ToString(CultureInfo.InvariantCulture), value);
      }
    
      internal virtual Object Shift(){
        Object res = null;
        uint thisLength = this.len;
        if (thisLength == 0)
          return res;
        uint lastItemInDense = (this.denseArrayLength >= thisLength) ? thisLength : this.denseArrayLength;
        if (lastItemInDense > 0){
          res = this.denseArray[0];
          ArrayObject.Copy(this.denseArray, 1, this.denseArray, 0, (int)(lastItemInDense-1));
        }else
          res = base.GetValueAtIndex(0);
        for (uint i = lastItemInDense; i < thisLength; i++)
          this.SetValueAtIndex(i-1, this.GetValueAtIndex(i));
        this.SetValueAtIndex(thisLength-1, Missing.Value);
        SetLength(thisLength - 1);
        if (res is Missing) return null;
        return res;
      }
      
      internal virtual void Sort(ScriptFunction compareFn){
        QuickSort qs = new QuickSort(this, compareFn);
        uint length = this.len;
        if (length <= this.denseArrayLength)
          qs.SortArray(0, (int)length - 1);
        else
          qs.SortObject(0, length - 1);
      }
      
      internal virtual void Splice(uint start, uint deleteCount, Object[] args, ArrayObject outArray, uint oldLength, uint newLength){
        if (oldLength > this.denseArrayLength){
          SpliceSlowly(start, deleteCount, args, outArray, oldLength, newLength);
          return;
        }
        if (newLength > oldLength){
          this.SetLength(newLength);
          if (newLength > this.denseArrayLength){
            SpliceSlowly(start, deleteCount, args, outArray, oldLength, newLength);
            return;
          }
        }
        if (deleteCount > oldLength)
          deleteCount = oldLength;
        if (deleteCount > 0)
          ArrayObject.Copy(this.denseArray, (int)start, outArray.denseArray, 0, (int)deleteCount);
        if (oldLength > 0)
          ArrayObject.Copy(this.denseArray, (int)(start+deleteCount), this.denseArray, (int)(start)+args.Length, (int)(oldLength-start-deleteCount));
        if (args != null){
          int n = args.Length;
          if (n > 0)
            ArrayObject.Copy(args, 0, this.denseArray, (int)start, n);
          if (n < deleteCount)
            this.SetLength(newLength);
        }else if (deleteCount > 0)
          this.SetLength(newLength);
      }
    
      protected void SpliceSlowly(uint start, uint deleteCount, Object[] args, ArrayObject outArray, uint oldLength, uint newLength){
        for (uint i = 0; i < deleteCount; i++)
          outArray.SetValueAtIndex(i, this.GetValueAtIndex(i+start));
        uint n = oldLength-start-deleteCount;
        if (newLength < oldLength){
          for (uint i = 0; i < n; i++)
            this.SetValueAtIndex(i+start+(uint)args.Length, this.GetValueAtIndex(i+start+deleteCount));
          this.SetLength(newLength);
        }else{
          if (newLength > oldLength)
            this.SetLength(newLength);
          for (uint i = n; i > 0; i--)
            this.SetValueAtIndex(i+start+(uint)args.Length-1, this.GetValueAtIndex(i+start+deleteCount-1));
        }
        int m = args == null ? 0 : args.Length;
        for (uint i = 0; i < m; i++)
          this.SetValueAtIndex(i+start, args[i]);
      }
    
      internal override void SwapValues(uint pi, uint qi){
        if (pi > qi)
          this.SwapValues(qi, pi);
        else if (pi >= this.denseArrayLength)
          base.SwapValues(pi, qi);
        else{
          Object temp = this.denseArray[(int)pi];
          this.denseArray[(int)pi] = this.GetValueAtIndex(qi);
          if (temp == Missing.Value)
            this.DeleteValueAtIndex(qi);
          else
            this.SetValueAtIndex(qi, temp);
        }
      }
      
      internal virtual Object[] ToArray(){
        int thisLength = (int)this.len;
        if (thisLength == 0)
          return new Object[0];
        else if (thisLength == this.denseArrayLength)
          return this.denseArray;
        else if (thisLength < this.denseArrayLength){
          Object[] result = new Object[thisLength];
          ArrayObject.Copy(this.denseArray, 0, result, 0, thisLength);
          return result;
        }else{
          Object[] result = new Object[thisLength];
          ArrayObject.Copy(this.denseArray, 0, result, 0, (int)this.denseArrayLength);
          for (uint i = this.denseArrayLength; i < thisLength; i++)
            result[i] = this.GetValueAtIndex(i);
          return result;
        }
      }
      
      internal virtual Array ToNativeArray(Type elementType){
        uint n = this.len;
        if (n > Int32.MaxValue)
          throw new JScriptException(JSError.OutOfMemory);
        if (elementType == null) elementType = typeof(Object);
        uint m = this.denseArrayLength;
        if (m > n) m = n;
        Array result = Array.CreateInstance(elementType, (int)n);
        for (int i = 0; i < m; i++)
          result.SetValue(Convert.CoerceT(this.denseArray[i], elementType), i);
        for (int i = (int)m; i < n; i++)
          result.SetValue(Convert.CoerceT(this.GetValueAtIndex((uint)i), elementType), i);
        return result;
      }
      
      internal static void Copy(Object[] source, Object[] target, int n){
        ArrayObject.Copy(source, 0, target, 0, n);
      }
      
      internal static void Copy(Object[] source, int i, Object[] target, int j, int n){
        if (i < j)
          for (int m = n-1; m >= 0; m--)
            target[j+m] = source[i+m];
        else
          for (int m = 0; m < n; m++)
            target[j+m] = source[i+m];
      }
    
      internal virtual ArrayObject Unshift(Object[] args){
        Debug.PreCondition(args != null && args.Length > 0);
        uint oldLength = this.len;
        int numArgs = args.Length;
        ulong newLength = oldLength + (ulong)numArgs;
        this.SetLength(newLength);
        if (newLength <= this.denseArrayLength){
          for (int i = (int)(oldLength - 1); i >= 0; i--)
            this.denseArray[i+numArgs] = this.denseArray[i];
          ArrayObject.Copy(args, 0, this.denseArray, 0, args.Length);
        }else{
          for (long i = oldLength - 1; i >= 0; i--)
            this.SetValueAtIndex((uint)(i + numArgs), this.GetValueAtIndex((uint)i));
          for (uint i = 0; i < numArgs; i++)
            this.SetValueAtIndex(i, args[i]);
        }
        return this;
      }


      // For temporary use of the debugger - a bug in the COM+ debug API's
      // causes 64 bit literal values to be not passed properly as an argument
      // to a func-eval. 
      internal Object DebugGetValueAtIndex(int index){
        return this.GetValueAtIndex((uint)index);
      }
      
      internal void DebugSetValueAtIndex(int index, Object value){
        this.SetValueAtIndex((uint)index, value);
      }

    }
    
    internal sealed class QuickSort{
      internal ScriptFunction compareFn;
      internal Object obj;
    
      internal QuickSort(Object obj, ScriptFunction compareFn){
        this.compareFn = compareFn;
        this.obj = obj;
      }
        
      private int Compare(Object x, Object y){
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
      
      internal void SortObject(long left, long right){ //left and right are longs to allow for values < 0. Their positives values are always < UInt32.MaxValue.
        Object x, y;
        if (right > left){
          long piv = left + (long)((right - left)*MathObject.random());
          LateBinding.SwapValues(this.obj, (uint)piv, (uint)right);
          x = LateBinding.GetValueAtIndex(this.obj, (ulong)right);
          long i = left - 1, j = right;
          while(true){
            do{
              y = LateBinding.GetValueAtIndex(this.obj, (ulong)++i);
            }while(i < j && this.Compare(x, y) >= 0);
            do{
              y = LateBinding.GetValueAtIndex(this.obj, (ulong)--j);
            }while(j > i && this.Compare(x, y) <= 0);
            if (i >= j)
              break;
            LateBinding.SwapValues(this.obj, (uint)i, (uint)j);
          }
          LateBinding.SwapValues(this.obj, (uint)i, (uint)right);
          this.SortObject(left, i-1);
          this.SortObject(i+1, right);
        }
      }
        
      internal void SortArray(int left, int right){
        ArrayObject array = (ArrayObject)this.obj;
        Object x, y;
        if (right > left){
          int piv = left + (int)((right - left)*MathObject.random());
          x = array.denseArray[piv];
          array.denseArray[piv] = array.denseArray[right];
          array.denseArray[right] = x;
          int i = left - 1, j = right;
          while(true){
            do{
              y = array.denseArray[++i];
            }while(i < j && this.Compare(x, y) >= 0);
            do{
              y = array.denseArray[--j];
            }while(j > i && this.Compare(x, y) <= 0);
            if (i >= j)
              break;
            QuickSort.Swap(array.denseArray, i, j);
          }
          QuickSort.Swap(array.denseArray, i, right);
          this.SortArray(left, i-1);
          this.SortArray(i+1, right);
        }
      }
        
      private static void Swap(Object[] array, int i, int j){
        Object temp = array[i];
        array[i] = array[j];
        array[j] = temp;
      }

    }
 }
