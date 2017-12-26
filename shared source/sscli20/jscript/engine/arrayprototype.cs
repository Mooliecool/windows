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
    
    using Microsoft.JScript.Vsa;
    using System;
    using System.Reflection;
    using System.Text;

    public class ArrayPrototype : ArrayObject{  
      internal static readonly ArrayPrototype ob = new ArrayPrototype(ObjectPrototype.ob);
      internal static ArrayConstructor _constructor; 
      
      internal ArrayPrototype(ObjectPrototype parent)
        : base(parent) {
        this.noExpando = true;
      }
    
      [JSFunctionAttribute(JSFunctionAttributeEnum.HasThisObject|JSFunctionAttributeEnum.HasVarArgs|JSFunctionAttributeEnum.HasEngine, JSBuiltin.Array_concat)]
      public static ArrayObject concat(Object thisob, VsaEngine engine, params Object[] args){
        ArrayObject arrayObj = engine.GetOriginalArrayConstructor().Construct();
        if (thisob is ArrayObject)
          arrayObj.Concat((ArrayObject)thisob);
        else 
          arrayObj.Concat(thisob);
        for (int i = 0; i < args.Length; i++){
          Object arg = args[i];
          if (arg is ArrayObject)
            arrayObj.Concat((ArrayObject)arg);
          else 
            arrayObj.Concat(arg);
        }
        return arrayObj;
      }
      
      public static ArrayConstructor constructor{
        get{
          return ArrayPrototype._constructor;
        }
      }  
      
      [JSFunctionAttribute(JSFunctionAttributeEnum.HasThisObject, JSBuiltin.Array_join)]
      public static String join(Object thisob, Object separator){
    	if (separator is Missing)
          return Join(thisob, ",", false);
        else
    	  return Join(thisob, Convert.ToString(separator), false);
      }
      
      internal static String Join(Object thisob, String separator, bool localize){
        StringBuilder str = new StringBuilder();
        uint thisLength = Convert.ToUint32(LateBinding.GetMemberValue(thisob, "length"));
        if (thisLength > int.MaxValue)
          throw new JScriptException(JSError.OutOfMemory);
        // Small optimization so we're not doing a bunch of reallocs for large arrays.
        if (thisLength > str.Capacity)
          str.Capacity = (int)thisLength;
        for (uint i = 0; i < thisLength; i++){
          Object value = LateBinding.GetValueAtIndex(thisob, i);
          if (value != null && !(value is Missing)){
            if (localize)
              str.Append(Convert.ToLocaleString(value));
            else
              str.Append(Convert.ToString(value));
          }
          if (i < thisLength-1)
            str.Append(separator);
        }
        return str.ToString();
      }
      
      [JSFunctionAttribute(JSFunctionAttributeEnum.HasThisObject, JSBuiltin.Array_pop)]
      public static Object pop(Object thisob){
        uint thisLength = Convert.ToUint32(LateBinding.GetMemberValue(thisob, "length"));
        if (thisLength == 0){
          LateBinding.SetMemberValue(thisob, "length", 0);
          return null;
        }
        Object result = LateBinding.GetValueAtIndex(thisob, thisLength-1);
        LateBinding.DeleteValueAtIndex(thisob, thisLength-1);
        LateBinding.SetMemberValue(thisob, "length", thisLength-1);
        return result;
      }

      [JSFunctionAttribute(JSFunctionAttributeEnum.HasThisObject|JSFunctionAttributeEnum.HasVarArgs, JSBuiltin.Array_push)]
      public static long push(Object thisob, params Object[] args){
        uint length = Convert.ToUint32(LateBinding.GetMemberValue(thisob, "length"));
        for (uint i = 0; i < args.Length; i++)
          LateBinding.SetValueAtIndex(thisob, i+(ulong)length, args[i]);
        long newLength = length+args.Length;
        LateBinding.SetMemberValue(thisob, "length", newLength);
        return newLength;
      }
      
      [JSFunctionAttribute(JSFunctionAttributeEnum.HasThisObject, JSBuiltin.Array_reverse)]
      public static Object reverse(Object thisob){
        uint thisLength = Convert.ToUint32(LateBinding.GetMemberValue(thisob, "length"));
        uint halfSize = thisLength/2;
        for (uint low = 0, high = thisLength - 1; low < halfSize; low++, high--)
          LateBinding.SwapValues(thisob, low, high);
        return thisob;
      }
        
      internal override void SetMemberValue(String name, Object value){
        if (this.noExpando) //This the fast prototype
          throw new JScriptException(JSError.OLENoPropOrMethod);
        base.SetMemberValue(name, value);
      }
      
      internal override void SetValueAtIndex(uint index, Object value){
        if (this.noExpando) //This the fast prototype 
          throw new JScriptException(JSError.OLENoPropOrMethod);
        base.SetValueAtIndex(index, value);
      }
    
      [JSFunctionAttribute(JSFunctionAttributeEnum.HasThisObject, JSBuiltin.Array_shift)]
      public static Object shift(Object thisob){
        Object res = null;
        if (thisob is ArrayObject)
          return ((ArrayObject)thisob).Shift();
        uint length = Convert.ToUint32(LateBinding.GetMemberValue(thisob, "length"));
        if (length == 0){
          LateBinding.SetMemberValue(thisob, "length", 0);
          return res;
        }
        res = LateBinding.GetValueAtIndex(thisob, 0);
        for (uint i = 1; i < length; i++){
          Object val = LateBinding.GetValueAtIndex(thisob, i);
          if (val is Missing)
            LateBinding.DeleteValueAtIndex(thisob, i-1);
          else
            LateBinding.SetValueAtIndex(thisob, i-1, val);
        }
        LateBinding.DeleteValueAtIndex(thisob, length-1);
        LateBinding.SetMemberValue(thisob, "length", length-1);
        return res;
      }

      [JSFunctionAttribute(JSFunctionAttributeEnum.HasThisObject|JSFunctionAttributeEnum.HasEngine, JSBuiltin.Array_slice)]
      public static ArrayObject slice(Object thisob, VsaEngine engine, double start, Object end){
        ArrayObject array = engine.GetOriginalArrayConstructor().Construct();
        uint length = Convert.ToUint32(LateBinding.GetMemberValue(thisob, "length"));
        // compute the start index
        long startIndex = Runtime.DoubleToInt64(Convert.ToInteger(start));
        if (startIndex < 0){
          startIndex = length + startIndex;
          if (startIndex < 0)
            startIndex = 0;
        }else if (startIndex > length)
          startIndex = length;
        // compute the end index
        long endIndex = length;
        if (end != null && !(end is Missing)){
          endIndex = Runtime.DoubleToInt64(Convert.ToInteger(end));
          if (endIndex < 0){
            endIndex = length + endIndex;
            if (endIndex < 0)
              endIndex = 0;
          }else if (endIndex > length)
            endIndex = length;
        }
        // slice
        if (endIndex > startIndex){    
          array.length = endIndex - startIndex;
          for (ulong i = (ulong)startIndex, j = 0; i < (ulong)endIndex; i++, j++){
            Object val = LateBinding.GetValueAtIndex(thisob, i);
            if (!(val is Missing))
              LateBinding.SetValueAtIndex(array, j, val);
          }
        }
        return array;
      }
      
      [JSFunctionAttribute(JSFunctionAttributeEnum.HasThisObject, JSBuiltin.Array_sort)]
      public static Object sort(Object thisob, Object function){
        ScriptFunction func = null;
        if (function is ScriptFunction)
          func = (ScriptFunction)function;
        uint length = Convert.ToUint32(LateBinding.GetMemberValue(thisob, "length"));
        if (thisob is ArrayObject)
          ((ArrayObject)thisob).Sort(func);
        else if (length <= Int32.MaxValue){
          QuickSort qs = new QuickSort(thisob, func);
          qs.SortObject(0, length);
        }
        return thisob;
      }
      
      [JSFunctionAttribute(JSFunctionAttributeEnum.HasThisObject|JSFunctionAttributeEnum.HasVarArgs|JSFunctionAttributeEnum.HasEngine, JSBuiltin.Array_splice)]
      public static ArrayObject splice(Object thisob, VsaEngine engine, double start, double deleteCnt, params Object[] args){
        uint oldLength = Convert.ToUint32(LateBinding.GetMemberValue(thisob, "length"));
        // compute the start index
        long startIndex = Runtime.DoubleToInt64(Convert.ToInteger(start));
        if (startIndex < 0){
          startIndex = oldLength + startIndex;
          if (startIndex < 0)
            startIndex = 0;
        }else if (startIndex > oldLength)
          startIndex = oldLength;
        
        // compute the number of items to delete
        long deleteCount = Runtime.DoubleToInt64(Convert.ToInteger(deleteCnt));
        if (deleteCount < 0)
          deleteCount = 0;
        else if (deleteCount > oldLength - startIndex)
          deleteCount = oldLength - startIndex;
        long newLength = oldLength + args.Length - deleteCount;
        
        // create an array for the result
        ArrayObject result = engine.GetOriginalArrayConstructor().Construct();
        result.length = deleteCount;
        
        // special case array objects (nice speedup if dense)
        if (thisob is ArrayObject){
          ((ArrayObject)thisob).Splice((uint)startIndex, (uint)deleteCount, args, result, (uint)oldLength, (uint)newLength);
          return result;
        }
        
        // copy the deleted items to the result array
        for (ulong i = 0; i < (ulong)deleteCount; i++)
          result.SetValueAtIndex((uint)i, LateBinding.GetValueAtIndex(thisob, i+(ulong)startIndex));
        
        // shift the remaining elements left or right
        long n = oldLength-startIndex-deleteCount;
        if (newLength < oldLength){
          for (long i = 0; i < n; i++)
            LateBinding.SetValueAtIndex(thisob, (ulong)(i+startIndex+args.Length), LateBinding.GetValueAtIndex(thisob, (ulong)(i+startIndex+deleteCount)));
          LateBinding.SetMemberValue(thisob, "length", newLength);
        }else{
          LateBinding.SetMemberValue(thisob, "length", newLength);
          for (long i = n-1; i >= 0; i--)
            LateBinding.SetValueAtIndex(thisob, (ulong)(i+startIndex+args.Length), LateBinding.GetValueAtIndex(thisob, (ulong)(i+startIndex+deleteCount)));
        }
        
        // splice in the arguments
        int m = args == null ? 0 : args.Length;
        for (uint i = 0; i < m; i++)
          LateBinding.SetValueAtIndex(thisob, i+(ulong)startIndex, args[i]);
        
        return result;
      }
    
      [JSFunctionAttribute(JSFunctionAttributeEnum.HasThisObject, JSBuiltin.Array_toLocaleString)]
      public static String toLocaleString(Object thisob){
        if (thisob is ArrayObject){
          StringBuilder sep = new StringBuilder(System.Globalization.CultureInfo.CurrentCulture.TextInfo.ListSeparator);
          if (sep[sep.Length-1] != ' ')
            sep.Append(' ');
          return ArrayPrototype.Join(thisob, sep.ToString(), true);
        }
        throw new JScriptException(JSError.NeedArrayObject);
      }
      
      [JSFunctionAttribute(JSFunctionAttributeEnum.HasThisObject, JSBuiltin.Array_toString)]
      public static String toString(Object thisob){
        if (thisob is ArrayObject)
          return ArrayPrototype.Join(thisob, ",", false);
        throw new JScriptException(JSError.NeedArrayObject);
      }
    
      [JSFunctionAttribute(JSFunctionAttributeEnum.HasThisObject|JSFunctionAttributeEnum.HasVarArgs, JSBuiltin.Array_unshift)]
      public static Object unshift(Object thisob, params Object[] args){
        if (args == null || args.Length == 0) return thisob;
        if (thisob is ArrayObject)
          return ((ArrayObject)thisob).Unshift(args);
        uint oldLength = Convert.ToUint32(LateBinding.GetMemberValue(thisob, "length"));
        long newLength = oldLength + args.Length;
        LateBinding.SetMemberValue(thisob, "length", newLength);
        // shift the array
        for (long i = oldLength - 1; i >= 0; i--){
          Object val = LateBinding.GetValueAtIndex(thisob, (ulong)i);
          if (val is Missing)
            LateBinding.DeleteValueAtIndex(thisob, (ulong)(i + args.Length));
          else
            LateBinding.SetValueAtIndex(thisob, (ulong)(i + args.Length), val);
        }
        // copy the input args
        for (uint i = 0; i < args.Length; i++)
          LateBinding.SetValueAtIndex(thisob, i, args[i]);
        return thisob;
      }
    
    }
}
