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

/* This class is used to wrap up compile-time constants in AST nodes.
   For the most part, this means enums, primitives, strings, types and name spaces.
   However, it can also be used to wrap the builtin objects when in Fast mode.
   Addditionally null (Empty.Value), DBNull.Value and Missing.Value can be wrapped.
   
   No other kind of value should be wrapped by this class.
*/

namespace Microsoft.JScript {
    
    using System;
    using System.Reflection;
    using System.Reflection.Emit;
    using System.Globalization;
    
    internal class ConstantWrapper : AST{ 
      internal Object value;
      internal bool isNumericLiteral;
      
      internal ConstantWrapper(Object value, Context context)
        : base(context) {
        if (value is ConcatString) value = value.ToString();
        this.value = value;
        this.isNumericLiteral = false;
      }
        
      internal override Object Evaluate(){
        return this.value;
      }
      
      internal override IReflect InferType(JSField inference_target){
        if (this.value == null || this.value is DBNull)
          return Typeob.Object;
        else if (this.value is ClassScope || this.value is TypedArray)
          return Typeob.Type;
        else if (this.value is EnumWrapper)
          return ((EnumWrapper)this.value).classScopeOrType;
        else
          return Globals.TypeRefs.ToReferenceContext(this.value.GetType());
      }

      internal bool IsAssignableTo(Type rtype){
        try{
          Convert.CoerceT(this.value, rtype, false);
          return true;
        }catch{
          return false;
        }
      }
    
      internal override AST PartiallyEvaluate(){
        return this;
      }
      
      public override String ToString(){
        return this.value.ToString();
      }

      internal override void TranslateToIL(ILGenerator il, Type rtype){
        if (rtype == Typeob.Void)
          return;
        Object val = this.value;
        if (val is EnumWrapper && rtype != Typeob.Object && rtype != Typeob.String)
          val = ((EnumWrapper)val).value;
        if (this.isNumericLiteral && (rtype == Typeob.Decimal || rtype == Typeob.Int64 || rtype == Typeob.UInt64 || rtype == Typeob.Single))
          val = this.context.GetCode();
        if (!(rtype is TypeBuilder)){
          try{
            val = Convert.CoerceT(val, rtype);
          }catch{
          }
        }
        this.TranslateToIL(il, val, rtype);
      }
      
      private void TranslateToIL(ILGenerator il, Object val, Type rtype){
        IConvertible ic = Convert.GetIConvertible(val);
        switch (Convert.GetTypeCode(val, ic)){
          case TypeCode.Empty: 
            il.Emit(OpCodes.Ldnull);
            if (rtype.IsValueType)
              Convert.Emit(this, il, Typeob.Object, rtype);
            return;
          case TypeCode.Object:
            break;
          case TypeCode.DBNull:
            il.Emit(OpCodes.Ldsfld, Typeob.Null.GetField("Value"));
            Convert.Emit(this, il, Typeob.Null, rtype);
            return;
          case TypeCode.Boolean: 
            ConstantWrapper.TranslateToILInt(il, ic.ToInt32(null));
            Convert.Emit(this, il, Typeob.Boolean, rtype);
            return;
          case TypeCode.Char: 
          case TypeCode.SByte: 
          case TypeCode.Byte: 
          case TypeCode.Int16: 
          case TypeCode.UInt16:
          case TypeCode.Int32:
            ConstantWrapper.TranslateToILInt(il, ic.ToInt32(null));
            if (rtype.IsEnum) return;
            if (val is EnumWrapper)
              Convert.Emit(this, il, ((EnumWrapper)val).type, rtype);
            else
              Convert.Emit(this, il, Globals.TypeRefs.ToReferenceContext(val.GetType()), rtype);
            return;
          case TypeCode.UInt32: 
            ConstantWrapper.TranslateToILInt(il, (int)ic.ToUInt32(null));
            if (rtype.IsEnum) return;
            if (val is EnumWrapper)
              Convert.Emit(this, il, ((EnumWrapper)val).type, rtype);
            else
              Convert.Emit(this, il, Typeob.UInt32, rtype);
            return;
          case TypeCode.Int64:
            long l = ic.ToInt64(null);
            if (Int32.MinValue <= l && l <= Int32.MaxValue){
              ConstantWrapper.TranslateToILInt(il, (int)l);
              il.Emit(OpCodes.Conv_I8);
            }else
              il.Emit(OpCodes.Ldc_I8, l);
            if (rtype.IsEnum) return;
            if (val is EnumWrapper)
              Convert.Emit(this, il, ((EnumWrapper)val).type, rtype);
            else
              Convert.Emit(this, il, Typeob.Int64, rtype);
            return;
          case TypeCode.UInt64: 
            ulong ul = ic.ToUInt64(null);
            if (ul <= Int32.MaxValue){
              ConstantWrapper.TranslateToILInt(il, (int)ul);
              il.Emit(OpCodes.Conv_I8);
            }else
              il.Emit(OpCodes.Ldc_I8, (long)ul);
            if (rtype.IsEnum) return;
            if (val is EnumWrapper)
              Convert.Emit(this, il, ((EnumWrapper)val).type, rtype);
            else
              Convert.Emit(this, il, Typeob.UInt64, rtype);
            return;
          case TypeCode.Single:
            float f = ic.ToSingle(null);
            if (f == f && (f != 0 || !Single.IsNegativeInfinity(1 / f))){
              int i = (int)Runtime.DoubleToInt64(f);
              if (-128 <= i && i <= 127 && f == (float)i){
                ConstantWrapper.TranslateToILInt(il, i);
                il.Emit(OpCodes.Conv_R4);
              }else
                il.Emit(OpCodes.Ldc_R4, f);
            }else
              il.Emit(OpCodes.Ldc_R4, f);
            Convert.Emit(this, il, Typeob.Single, rtype);
            return;  
          case TypeCode.Double:
            double d = ic.ToDouble(null);
            if (d == d && (d != 0 || !Double.IsNegativeInfinity(1 / d))){
              int i = (int)Runtime.DoubleToInt64(d);
              if (-128 <= i && i <= 127 && d == (double)i){
                ConstantWrapper.TranslateToILInt(il, i);
                il.Emit(OpCodes.Conv_R8);
              }else
                il.Emit(OpCodes.Ldc_R8, d);
            }else
              il.Emit(OpCodes.Ldc_R8, d);
            Convert.Emit(this, il, Typeob.Double, rtype);
            return;  
          case TypeCode.Decimal:
            int[] bits = Decimal.GetBits(ic.ToDecimal(null));
            ConstantWrapper.TranslateToILInt(il, bits[0]);
            ConstantWrapper.TranslateToILInt(il, bits[1]);
            ConstantWrapper.TranslateToILInt(il, bits[2]);
            il.Emit(bits[3] < 0 ? OpCodes.Ldc_I4_1 : OpCodes.Ldc_I4_0); //bool isNegative
            ConstantWrapper.TranslateToILInt(il, (bits[3]&0x7FFFFFFF)>>16);
            il.Emit(OpCodes.Newobj, CompilerGlobals.decimalConstructor);
            Convert.Emit(this, il, Typeob.Decimal, rtype);
            return;
          case TypeCode.DateTime:
            l = ic.ToDateTime(null).Ticks;
            il.Emit(OpCodes.Ldc_I8, l);
            Convert.Emit(this, il, Typeob.Int64, rtype);
            return;
          case TypeCode.String:
            String str = ic.ToString(null);
            if (rtype == Typeob.Char && str.Length == 1){
              ConstantWrapper.TranslateToILInt(il, (int)str[0]);
              return;
            }
            il.Emit(OpCodes.Ldstr, str);
            Convert.Emit(this, il, Typeob.String, rtype);
            return;
        }
        if (val is Enum){
          if (rtype == Typeob.String)
            this.TranslateToIL(il, val.ToString(), rtype);
          else if (rtype.IsPrimitive)
            this.TranslateToIL(il, System.Convert.ChangeType(val, Enum.GetUnderlyingType(Globals.TypeRefs.ToReferenceContext(val.GetType())), CultureInfo.InvariantCulture), rtype);
          else{
            Type et = Globals.TypeRefs.ToReferenceContext(val.GetType());
            Type ut = Enum.GetUnderlyingType(et);
            this.TranslateToIL(il, System.Convert.ChangeType(val, ut, CultureInfo.InvariantCulture), ut);
            il.Emit(OpCodes.Box, et);
            Convert.Emit(this, il, Typeob.Object, rtype);
          }
          return;
        }
        if (val is EnumWrapper){
          if (rtype == Typeob.String)
            this.TranslateToIL(il, val.ToString(), rtype);
          else if (rtype.IsPrimitive)
            this.TranslateToIL(il, ((EnumWrapper)val).ToNumericValue(), rtype);
          else{
            Type et = ((EnumWrapper)val).type;
            Type ut = Globals.TypeRefs.ToReferenceContext(((EnumWrapper)val).value.GetType());
            this.TranslateToIL(il, ((EnumWrapper)val).value, ut);
            il.Emit(OpCodes.Box, et);
            Convert.Emit(this, il, Typeob.Object, rtype);
          }
          return;
        }
        if (val is Type){
          il.Emit(OpCodes.Ldtoken, (Type)val);
          il.Emit(OpCodes.Call, CompilerGlobals.getTypeFromHandleMethod);
          Convert.Emit(this, il, Typeob.Type, rtype);
        }else if (val is Namespace){
          il.Emit(OpCodes.Ldstr, ((Namespace)val).Name);
          this.EmitILToLoadEngine(il);
          il.Emit(OpCodes.Call, CompilerGlobals.getNamespaceMethod);
          Convert.Emit(this, il, Typeob.Namespace, rtype);
        }else if (val is ClassScope){
          il.Emit(OpCodes.Ldtoken, ((ClassScope)val).GetTypeBuilderOrEnumBuilder());
          il.Emit(OpCodes.Call, CompilerGlobals.getTypeFromHandleMethod);
          Convert.Emit(this, il, Typeob.Type, rtype);
        }else if (val is TypedArray){
          il.Emit(OpCodes.Ldtoken, Convert.ToType((TypedArray)val));
          il.Emit(OpCodes.Call, CompilerGlobals.getTypeFromHandleMethod);
          Convert.Emit(this, il, Typeob.Type, rtype);
        }else if (val is NumberObject){
          this.TranslateToIL(il, ((NumberObject)val).value, Typeob.Object);
          this.EmitILToLoadEngine(il);
          il.Emit(OpCodes.Call, CompilerGlobals.toObjectMethod);
          Convert.Emit(this, il, Typeob.NumberObject, rtype);
        }else if (val is StringObject){
          il.Emit(OpCodes.Ldstr, ((StringObject)val).value);
          this.EmitILToLoadEngine(il);
          il.Emit(OpCodes.Call, CompilerGlobals.toObjectMethod);
          Convert.Emit(this, il, Typeob.StringObject, rtype);
        }else if (val is BooleanObject){
          il.Emit(((BooleanObject)val).value ? OpCodes.Ldc_I4_1 : OpCodes.Ldc_I4_0);
          il.Emit(OpCodes.Box, Typeob.Boolean);
          this.EmitILToLoadEngine(il);
          il.Emit(OpCodes.Call, CompilerGlobals.toObjectMethod);
          Convert.Emit(this, il, Typeob.BooleanObject, rtype);
        }else if (val is ActiveXObjectConstructor){
          il.Emit(OpCodes.Call, Typeob.GlobalObject.GetProperty("ActiveXObject").GetGetMethod());
          Convert.Emit(this, il, Typeob.ScriptFunction, rtype);
        }else if (val is ArrayConstructor){
          il.Emit(OpCodes.Call, Typeob.GlobalObject.GetProperty("Array").GetGetMethod());
          Convert.Emit(this, il, Typeob.ScriptFunction, rtype);
        }else if (val is BooleanConstructor){
          il.Emit(OpCodes.Call, Typeob.GlobalObject.GetProperty("Boolean").GetGetMethod());
          Convert.Emit(this, il, Typeob.ScriptFunction, rtype);
        }else if (val is DateConstructor){
          il.Emit(OpCodes.Call, Typeob.GlobalObject.GetProperty("Date").GetGetMethod());
          Convert.Emit(this, il, Typeob.ScriptFunction, rtype);
        }else if (val is EnumeratorConstructor){
          il.Emit(OpCodes.Call, Typeob.GlobalObject.GetProperty("Enumerator").GetGetMethod());
          Convert.Emit(this, il, Typeob.ScriptFunction, rtype);
        }else if (val is ErrorConstructor){
          ErrorConstructor error = (ErrorConstructor)val;
          if (error == ErrorConstructor.evalOb)
            il.Emit(OpCodes.Call, Typeob.GlobalObject.GetProperty("EvalError").GetGetMethod());
          else if (error == ErrorConstructor.rangeOb)
            il.Emit(OpCodes.Call, Typeob.GlobalObject.GetProperty("RangeError").GetGetMethod());
          else if (error == ErrorConstructor.referenceOb)
            il.Emit(OpCodes.Call, Typeob.GlobalObject.GetProperty("ReferenceError").GetGetMethod());
          else if (error == ErrorConstructor.syntaxOb)
            il.Emit(OpCodes.Call, Typeob.GlobalObject.GetProperty("SyntaxError").GetGetMethod());
          else if (error == ErrorConstructor.typeOb)
            il.Emit(OpCodes.Call, Typeob.GlobalObject.GetProperty("TypeError").GetGetMethod());
          else if (error == ErrorConstructor.uriOb)
            il.Emit(OpCodes.Call, Typeob.GlobalObject.GetProperty("URIError").GetGetMethod());
          else
            il.Emit(OpCodes.Call, Typeob.GlobalObject.GetProperty("Error").GetGetMethod());
          Convert.Emit(this, il, Typeob.ScriptFunction, rtype);
        }else if (val is FunctionConstructor){
          il.Emit(OpCodes.Call, Typeob.GlobalObject.GetProperty("Function").GetGetMethod());
          Convert.Emit(this, il, Typeob.ScriptFunction, rtype);
        }else if (val is MathObject){
          il.Emit(OpCodes.Call, Typeob.GlobalObject.GetProperty("Math").GetGetMethod());
          Convert.Emit(this, il, Typeob.JSObject, rtype);
        }else if (val is NumberConstructor){
          il.Emit(OpCodes.Call, Typeob.GlobalObject.GetProperty("Number").GetGetMethod());
          Convert.Emit(this, il, Typeob.ScriptFunction, rtype);
        }else if (val is ObjectConstructor){
          il.Emit(OpCodes.Call, Typeob.GlobalObject.GetProperty("Object").GetGetMethod());
          Convert.Emit(this, il, Typeob.ScriptFunction, rtype);
        }else if (val is RegExpConstructor){
          il.Emit(OpCodes.Call, Typeob.GlobalObject.GetProperty("RegExp").GetGetMethod());
          Convert.Emit(this, il, Typeob.ScriptFunction, rtype);
        }else if (val is StringConstructor){
          il.Emit(OpCodes.Call, Typeob.GlobalObject.GetProperty("String").GetGetMethod());
          Convert.Emit(this, il, Typeob.ScriptFunction, rtype);
        }else if (val is VBArrayConstructor){
          il.Emit(OpCodes.Call, Typeob.GlobalObject.GetProperty("VBArray").GetGetMethod());
          Convert.Emit(this, il, Typeob.ScriptFunction, rtype);
        }else if (val is IntPtr){
          il.Emit(OpCodes.Ldc_I8, (long)(IntPtr)val);
          il.Emit(OpCodes.Conv_I);
          Convert.Emit(this, il, Typeob.IntPtr, rtype);
        }else if (val is UIntPtr){
          il.Emit(OpCodes.Ldc_I8, (long)(UIntPtr)val);
          il.Emit(OpCodes.Conv_U);
          Convert.Emit(this, il, Typeob.UIntPtr, rtype);
        }else if (val is Missing){       
          il.Emit(OpCodes.Ldsfld, CompilerGlobals.missingField);
          Convert.Emit(this, il, Typeob.Object, rtype);
        }else if (val is System.Reflection.Missing){
          if (rtype.IsPrimitive)
            this.TranslateToIL(il, Double.NaN, rtype);
          else if (rtype != Typeob.Object && !rtype.IsValueType)
            il.Emit(OpCodes.Ldnull);
          else{
            il.Emit(OpCodes.Ldsfld, CompilerGlobals.systemReflectionMissingField);
            Convert.Emit(this, il, Typeob.Object, rtype);
          }
        }else if (val != this.value) //Value was coerced to some type we have no compile time knowlegde of
          this.TranslateToIL(il, this.value, rtype);
        else
          throw new JScriptException(JSError.InternalError, this.context); //It should not be possible to wrap any other kind of object
      }
        
      internal static void TranslateToILInt(ILGenerator il, int i){
        switch (i){
          case -1:il.Emit(OpCodes.Ldc_I4_M1); break;
          case 0: il.Emit(OpCodes.Ldc_I4_0); break;
          case 1: il.Emit(OpCodes.Ldc_I4_1); break;
          case 2: il.Emit(OpCodes.Ldc_I4_2); break;
          case 3: il.Emit(OpCodes.Ldc_I4_3); break;
          case 4: il.Emit(OpCodes.Ldc_I4_4); break;
          case 5: il.Emit(OpCodes.Ldc_I4_5); break;
          case 6: il.Emit(OpCodes.Ldc_I4_6); break;
          case 7: il.Emit(OpCodes.Ldc_I4_7); break;
          case 8: il.Emit(OpCodes.Ldc_I4_8); break;
          default:
            if (-128 <= i && i <= 127)
              il.Emit(OpCodes.Ldc_I4_S, (SByte)i);
            else
              il.Emit(OpCodes.Ldc_I4, i);
            break;
        }
      }
      
      internal override void TranslateToILInitializer(ILGenerator il){
      }
      
    }
}
