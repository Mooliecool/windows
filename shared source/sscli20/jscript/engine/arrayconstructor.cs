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
    using System.Diagnostics;

    public sealed class ArrayConstructor : ScriptFunction{
      internal static readonly ArrayConstructor ob = new ArrayConstructor();
    
      private ArrayPrototype originalPrototype; 
      
      internal ArrayConstructor()
        : base(FunctionPrototype.ob, "Array", 1) {
        this.originalPrototype = ArrayPrototype.ob;
        ArrayPrototype._constructor = this;
        this.proto = ArrayPrototype.ob;
      }
    
      internal ArrayConstructor(LenientFunctionPrototype parent, LenientArrayPrototype prototypeProp)
        : base(parent, "Array", 1) {
        this.originalPrototype = prototypeProp;
        prototypeProp.constructor = this;
        this.proto = prototypeProp;
        this.noExpando = false;
      }
      
      internal override Object Call(Object[] args, Object thisob){
        return Construct(args);
      }
      
      internal ArrayObject Construct(){
        return new ArrayObject(this.originalPrototype, typeof(ArrayObject));
      }
      
      internal override Object Construct(Object[] args){
        return this.CreateInstance(args);
      }
      
      /// <summary>
      /// Helper method that constructs a new JScript array and initializes it with values.
      /// </summary>
      /// <internalonly/>
      public ArrayObject ConstructArray(Object[] args){
        ArrayObject arrayObj = new ArrayObject(originalPrototype, typeof(ArrayObject));
        arrayObj.length = args.Length;
        for (int i = 0; i < args.Length; i++)
          arrayObj.SetValueAtIndex((uint)i, args[i]);
        return arrayObj;
      }
      
      internal ArrayObject ConstructWrapper(){
        return new ArrayWrapper(this.originalPrototype, null, false);
      }

      internal ArrayObject ConstructWrapper(Array arr){
        return new ArrayWrapper(this.originalPrototype, arr, false);
      }
      
      internal ArrayObject ConstructImplicitWrapper(Array arr){
        return new ArrayWrapper(this.originalPrototype, arr, true);
      }

      /// <internalonly/>
      [JSFunctionAttribute(JSFunctionAttributeEnum.HasVarArgs)]
      public new ArrayObject CreateInstance(params Object[] args){
        ArrayObject arrayObj = new ArrayObject(this.originalPrototype, typeof(ArrayObject));
        if (args.Length != 0){
          if (args.Length == 1){
            Object arg0 = args[0];
            IConvertible ic = Convert.GetIConvertible(arg0);
            switch (Convert.GetTypeCode(arg0, ic)){
              case TypeCode.Char: 
              case TypeCode.SByte: 
              case TypeCode.Byte:
              case TypeCode.Int16: 
              case TypeCode.UInt16:
              case TypeCode.Int32:
              case TypeCode.UInt32:
              case TypeCode.Int64:
              case TypeCode.UInt64:
              case TypeCode.Single:
              case TypeCode.Double:
              case TypeCode.Decimal:
                double d = Convert.ToNumber(arg0, ic);
                uint len = Convert.ToUint32(arg0, ic);
                if (d != (double)len)
                  throw new JScriptException(JSError.ArrayLengthConstructIncorrect);
                arrayObj.length = len;
                return arrayObj;
            }
          }
          if (args.Length == 1 && args[0] is Array){
            Array array = (Array)args[0];
            if (array.Rank != 1)
              throw new JScriptException(JSError.TypeMismatch);
            arrayObj.length = array.Length;
            for (int i = 0; i < array.Length; i++)
              arrayObj.SetValueAtIndex((uint)i, array.GetValue(i));
          } else {
            arrayObj.length = args.Length;
            for (int i = 0; i < args.Length; i++)
              arrayObj.SetValueAtIndex((uint)i, args[i]);
          }
        }
        return arrayObj;
      }
      
      [JSFunctionAttribute(JSFunctionAttributeEnum.HasVarArgs)]
      public ArrayObject Invoke(params Object[] args){
        if (args.Length == 1 && args[0] is Array)
          return ConstructWrapper((Array)args[0]);
        else 
          return this.CreateInstance(args);
      }
    }
}
