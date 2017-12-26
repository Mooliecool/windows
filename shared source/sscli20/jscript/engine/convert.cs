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
    using System.Globalization;
    using System.Reflection;
    using System.Reflection.Emit;
    using System.Security;
    using System.Text;
    using System.Runtime.InteropServices;
    using System.Diagnostics;

    enum PreferredType : int {Either, Number, String, LocaleString};

    public sealed class Convert{

      public static bool IsBadIndex(AST ast){
        Int32 i;
        if (!(ast is ConstantWrapper))
          return false;
        try{
          i = (Int32)CoerceT(((ConstantWrapper)ast).value, typeof(System.Int32));
        }catch{
           return true;
        }
        return i < 0;
      }

      public static Double CheckIfDoubleIsInteger(Double d){
        if (d == System.Math.Round(d)) return d;
        throw new JScriptException(JSError.TypeMismatch);
      }

      public static Single CheckIfSingleIsInteger(Single s){
        if (s == System.Math.Round(s)) return s;
        throw new JScriptException(JSError.TypeMismatch);
      }

      //This routine converts a value to the desired type, if possible.
      //It throws an exception if the conversion has not been provided for, as well as when loss of information will occur.
      //For example, coercing 1.5 to type Int32 will throw an exception.
      public static Object Coerce(Object value, Object type){
        return Convert.Coerce(value, type, false);
      }

      internal static Object Coerce(Object value, Object type, bool explicitOK){
        TypeExpression te = type as TypeExpression;
        if (te != null)
          type = te.ToIReflect();
        TypedArray ta = type as TypedArray;
        if (ta != null){
          IReflect eIr = ta.elementType;
          int rank = ta.rank;
          Type eTy = eIr is Type ? (Type)eIr : eIr is ClassScope ? ((ClassScope)eIr).GetBakedSuperType() : typeof(Object);
          ArrayObject ao = value as ArrayObject;
          if (ao != null)
            return ao.ToNativeArray(eTy);
          Array arr = value as Array;
          if (arr != null && arr.Rank == rank)
            type = Convert.ToType(TypedArray.ToRankString(rank), eTy);
          if (value == null || value is DBNull)
            return null;
        }
        ClassScope csc = type as ClassScope;
        if (csc != null){
          if (csc.HasInstance(value))
            return value;
          else{
            EnumDeclaration ed = csc.owner as EnumDeclaration;
            if (ed != null){
              EnumWrapper ew = value as EnumWrapper;
              if (ew != null)
                if (ew.classScopeOrType == csc) return value; else throw new JScriptException(JSError.TypeMismatch);
              return new DeclaredEnumValue(Coerce(value, ed.baseType), null, csc);
            }
            if (value == null || value is DBNull) return null;
            throw new JScriptException(JSError.TypeMismatch);
          }
        }else if (!(type is Type))
          type = Convert.ToType(Runtime.TypeRefs, (IReflect)type);
        else if (type == typeof(Type) && value is ClassScope)
            return value;
        else if (((Type)type).IsEnum){
          EnumWrapper ew = value as EnumWrapper;
          if (ew != null) {
            if (ew.classScopeOrType == type)
              return value; 
            else 
              throw new JScriptException(JSError.TypeMismatch);
          }
          
          Type t = type as Type;        
          return MetadataEnumValue.GetEnumValue(t, Convert.CoerceT(value, Convert.GetUnderlyingType(t), explicitOK));
        }
        return Convert.CoerceT(value, (Type)type, explicitOK);
      }

      //This routine converts a value to the desired type, if possible.
      //It throws an exception if the conversion has not been provided for, as well as when loss of information will occur.
      //For example, coercing 1.5 to type Int32 will throw an exception.
      internal static Object CoerceT(Object value, Type type){
        return Convert.CoerceT(value, type, false);
      }

      public static Object CoerceT(Object value, Type t, bool explicitOK) {
        if (t == typeof(Object)) return value;
        if (t == typeof(String) && value is String) return value;
        if (t.IsEnum && !(t is EnumBuilder) && !(t is TypeBuilder)){
          IConvertible ic = Convert.GetIConvertible(value);
          TypeCode vc = Convert.GetTypeCode(value, ic);
          if (vc == TypeCode.String)
            return Enum.Parse(t, ic.ToString(CultureInfo.InvariantCulture));
          else{
            if (!explicitOK && vc != TypeCode.Empty){
              Type vty = value.GetType();
              if (vty.IsEnum)
                if (vty != t) throw new JScriptException(JSError.TypeMismatch); else return value;
            }
            return Enum.ToObject(t, Convert.CoerceT(value, Convert.GetUnderlyingType(t), explicitOK));
          }
        }
        TypeCode c2 = Type.GetTypeCode(t);
        if (c2 != TypeCode.Object)
          return Convert.Coerce2(value, c2, explicitOK);
        if (value is ConcatString) value = value.ToString();
        if (value == null || (value == DBNull.Value && t != typeof(Object)) || value is Missing || value is System.Reflection.Missing)
          if (t.IsValueType) {
            // Activator.CreateInstance looks at the immediate caller to check if an internal type can
            // be created. Only allow public types in this assembly to be created.
            if (!t.IsPublic && t.Assembly == typeof(ActiveXObjectConstructor).Assembly)
              throw new JScriptException(JSError.CantCreateObject);
            return Activator.CreateInstance(t);
          }
          else
            return null;
        else if (t.IsAssignableFrom(value.GetType()))
          return value;
        else if (typeof(Delegate).IsAssignableFrom(t)){
          if (value is Closure)
            return ((Closure)value).ConvertToDelegate(t);
          else if (value is FunctionWrapper)
            return ((FunctionWrapper)value).ConvertToDelegate(t);
          else if (value is FunctionObject) //called at compile-time from ConstantWrapper
            return value; //Do nothing, ConstantWrapper will do the right thing.
        }else if (value is ArrayObject && typeof(Array).IsAssignableFrom(t))
          return ((ArrayObject)value).ToNativeArray(t.GetElementType());
        else if (value is Array && t == typeof(ArrayObject) && ((Array)value).Rank == 1){
          if (Globals.contextEngine == null){
            Globals.contextEngine = new VsaEngine(true); //ASP+ case
            Globals.contextEngine.InitVsaEngine("JS7://Microsoft.JScript.Vsa.VsaEngine", new DefaultVsaSite());
          }
          return Globals.contextEngine.GetOriginalArrayConstructor().ConstructWrapper((Array)value);
        }else if (value is ClassScope && t == typeof(Type))
          return ((ClassScope)value).GetTypeBuilderOrEnumBuilder();
        else if (value is TypedArray && t == typeof(Type))
          return ((TypedArray)value).ToType();
        //Look for a suitable op_Implicit or op_Explicit conversion
        Type source_type = value.GetType();
        MethodInfo meth = null;
        if (explicitOK){
          meth = t.GetMethod("op_Explicit", BindingFlags.ExactBinding|BindingFlags.Public|BindingFlags.Static, null, new Type[]{source_type}, null);
          if (meth != null && (meth.Attributes & MethodAttributes.SpecialName) != 0){
            meth = new JSMethodInfo(meth);
            return meth.Invoke(null, BindingFlags.SuppressChangeType, null, new Object[]{value}, null);
          }
          meth = Convert.GetToXXXXMethod(source_type, t, explicitOK);
          if (meth != null && (meth.Attributes & MethodAttributes.SpecialName) != 0){
            meth = new JSMethodInfo(meth);
            if (meth.IsStatic)
              return meth.Invoke(null, BindingFlags.SuppressChangeType, null, new Object[]{value}, null);
            else
              return meth.Invoke(value, BindingFlags.SuppressChangeType, null, new Object[]{}, null);
          }
        }
        meth = t.GetMethod("op_Implicit", BindingFlags.ExactBinding|BindingFlags.Public|BindingFlags.Static, null, new Type[]{source_type}, null);
        if (meth != null && (meth.Attributes & MethodAttributes.SpecialName) != 0){
          meth = new JSMethodInfo(meth);
          return meth.Invoke(null, BindingFlags.SuppressChangeType, null, new Object[]{value}, null);
        }
        meth = Convert.GetToXXXXMethod(source_type, t, false);
        if (meth != null && (meth.Attributes & MethodAttributes.SpecialName) != 0){
          meth = new JSMethodInfo(meth);
          if (meth.IsStatic)
            return meth.Invoke(null, BindingFlags.SuppressChangeType, null, new Object[]{value}, null);
          else
            return meth.Invoke(value, BindingFlags.SuppressChangeType, null, new Object[]{}, null);
        }
        if (t.IsByRef)
          return Convert.CoerceT(value, t.GetElementType());
        Type vt = value.GetType();
        throw new JScriptException(JSError.TypeMismatch);
      }

      //Call this routine only for target TypeTokens other than Object and Empty
      //It special cases the well known types with JScript specific conversion semantics
      public static Object Coerce2(Object value, TypeCode target, bool truncationPermitted){
        if (truncationPermitted) return Coerce2WithTruncationPermitted(value, target);
        return Coerce2WithNoTrunctation(value, target);
      }

      private  static Object Coerce2WithNoTrunctation(Object value, TypeCode target){
        if (value is EnumWrapper)
          value = ((EnumWrapper)value).value;
        if (value is ConstantWrapper)
          value = ((ConstantWrapper)value).value;
        try{ checked{
          IConvertible ic = Convert.GetIConvertible(value);
          switch (Convert.GetTypeCode(value, ic)){
            case TypeCode.Empty:
              switch (target){
                case TypeCode.DBNull: return DBNull.Value;
                case TypeCode.Boolean: return false;
                case TypeCode.Char: return (Char)0;
                case TypeCode.SByte: return (SByte)0;
                case TypeCode.Byte: return (Byte)0;
                case TypeCode.Int16: return (Int16)0;
                case TypeCode.UInt16: return (UInt16)0;
                case TypeCode.Int32: return (Int32)0;
                case TypeCode.UInt32: return (UInt32)0;
                case TypeCode.Int64: return (Int64)0;
                case TypeCode.UInt64: return (UInt64)0;
                case TypeCode.Single: return Single.NaN;
                case TypeCode.Double: return Double.NaN;
                case TypeCode.Decimal: return (Decimal)0;
                case TypeCode.DateTime: return new DateTime((Int64)0);
                case TypeCode.String: return null;
              }
              break;

            case TypeCode.Object:
              if (value is System.Reflection.Missing || (value is Missing && target != TypeCode.Object)) goto case TypeCode.Empty;
              switch (target){
                case TypeCode.Boolean: return Convert.ToBoolean(value, false);
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
                case TypeCode.Decimal: return Coerce2WithNoTrunctation(Convert.ToNumber(value, ic), target);
                case TypeCode.DateTime:
                  if (value is DateObject)
                    return DatePrototype.getVarDate((DateObject)value);
                  else
                    return Coerce2WithNoTrunctation(Convert.ToNumber(value, ic), target);
                case TypeCode.String: return Convert.ToString(value, ic);
              }
              break;

            case TypeCode.DBNull:
              switch (target){
                case TypeCode.DBNull: return DBNull.Value;
                case TypeCode.Boolean: return false;
                case TypeCode.Char: return (Char)0;
                case TypeCode.SByte: return (SByte)0;
                case TypeCode.Byte: return (Byte)0;
                case TypeCode.Int16: return (Int16)0;
                case TypeCode.UInt16: return (UInt16)0;
                case TypeCode.Int32: return (Int32)0;
                case TypeCode.UInt32: return (UInt32)0;
                case TypeCode.Int64: return (Int64)0;
                case TypeCode.UInt64: return (UInt64)0;
                case TypeCode.Single: return (Single)0;
                case TypeCode.Double: return (Double)0;
                case TypeCode.Decimal: return (Decimal)0;
                case TypeCode.DateTime: return new DateTime((Int64)0);
                case TypeCode.String: return null;
              }
              break;

            case TypeCode.Boolean:
              bool b = ic.ToBoolean(null);
              int bi = b ? 1 : 0;
              switch (target){
                case TypeCode.Boolean: return b;
                case TypeCode.Char: return (Char)bi;
                case TypeCode.SByte: return (SByte)bi;
                case TypeCode.Byte: return (Byte)bi;
                case TypeCode.Int16: return (Int16)bi;
                case TypeCode.UInt16: return (UInt16)bi;
                case TypeCode.Int32: return (Int32)bi;
                case TypeCode.UInt32: return (UInt32)bi;
                case TypeCode.Int64: return (Int64)bi;
                case TypeCode.UInt64: return (UInt64)bi;
                case TypeCode.Single: return (Single)bi;
                case TypeCode.Double: return (Double)bi;
                case TypeCode.Decimal: return (Decimal)bi;
                case TypeCode.DateTime: return new DateTime((Int64)bi);
                case TypeCode.String: return b ? "true" : "false";
              }
              break;

            case TypeCode.Char:
              Char ch = ic.ToChar(null);
              UInt16 us = (UInt16)ch;
              switch (target){
                case TypeCode.Boolean: return us != 0;
                case TypeCode.Char: return ch;
                case TypeCode.SByte: return (SByte)us;
                case TypeCode.Byte:  return (Byte)us;
                case TypeCode.Int16: return (Int16)us;
                case TypeCode.UInt16: return us;
                case TypeCode.Int32: return (Int32)us;
                case TypeCode.UInt32: return (UInt32)us;
                case TypeCode.Int64: return (Int64)us;
                case TypeCode.UInt64: return (UInt64)us;
                case TypeCode.Single: return (Single)us;
                case TypeCode.Double: return (Double)us;
                case TypeCode.Decimal: return (Decimal)us;
                case TypeCode.DateTime: return new DateTime((Int64)us);
                case TypeCode.String: return Char.ToString(ch);
              }
              break;

            case TypeCode.SByte:
              SByte sb = ic.ToSByte(null);
              switch (target){
                case TypeCode.Boolean: return sb != 0;
                case TypeCode.Char: return (Char)sb;
                case TypeCode.SByte: return sb;
                case TypeCode.Byte: return (Byte)sb;
                case TypeCode.Int16: return (Int16)sb;
                case TypeCode.UInt16: return (UInt16)sb;
                case TypeCode.Int32: return (Int32)sb;
                case TypeCode.UInt32: return (UInt32)sb;
                case TypeCode.Int64: return (Int64)sb;
                case TypeCode.UInt64: return (UInt64)sb;
                case TypeCode.Single: return (Single)sb;
                case TypeCode.Double: return (Double)sb;
                case TypeCode.Decimal: return (Decimal)sb;
                case TypeCode.DateTime: return new DateTime((Int64)sb);
                case TypeCode.String: return sb.ToString(CultureInfo.InvariantCulture);
              }
              break;

            case TypeCode.Byte:
              Byte ub = ic.ToByte(null);
              switch (target){
                case TypeCode.Boolean: return ub != 0;
                case TypeCode.Char: return (Char)ub;
                case TypeCode.SByte: return (SByte)ub;
                case TypeCode.Byte: return ub;
                case TypeCode.Int16: return (Int16)ub;
                case TypeCode.UInt16: return (UInt16)ub;
                case TypeCode.Int32: return (Int32)ub;
                case TypeCode.UInt32: return (UInt32)ub;
                case TypeCode.Int64: return (Int64)ub;
                case TypeCode.UInt64: return (UInt64)ub;
                case TypeCode.Single: return (Single)ub;
                case TypeCode.Double: return (Double)ub;
                case TypeCode.Decimal: return (Decimal)ub;
                case TypeCode.DateTime: return new DateTime((Int64)ub);
                case TypeCode.String: return ub.ToString(CultureInfo.InvariantCulture);
              }
              break;

            case TypeCode.Int16:
              Int16 s = ic.ToInt16(null);
              switch (target){
                case TypeCode.Boolean: return s != 0;
                case TypeCode.Char: return (Char)s;
                case TypeCode.SByte: return (SByte)s;
                case TypeCode.Byte: return (Byte)s;
                case TypeCode.Int16: return s;
                case TypeCode.UInt16: return (UInt16)s;
                case TypeCode.Int32: return (Int32)s;
                case TypeCode.UInt32: return (UInt32)s;
                case TypeCode.Int64: return (Int64)s;
                case TypeCode.UInt64: return (UInt64)s;
                case TypeCode.Single: return (Single)s;
                case TypeCode.Double: return (Double)s;
                case TypeCode.Decimal: return (Decimal)s;
                case TypeCode.DateTime: return new DateTime((Int64)s);
                case TypeCode.String: return s.ToString(CultureInfo.InvariantCulture);
              }
              break;

            case TypeCode.UInt16:
              us = ic.ToUInt16(null);
              switch (target){
                case TypeCode.Boolean: return us != 0;
                case TypeCode.Char: return (Char)us;
                case TypeCode.SByte: return (SByte)us;
                case TypeCode.Byte: return (Byte)us;
                case TypeCode.Int16: return (Int16)us;
                case TypeCode.UInt16: return us;
                case TypeCode.Int32: return (Int32)us;
                case TypeCode.UInt32: return (UInt32)us;
                case TypeCode.Int64: return (Int64)us;
                case TypeCode.UInt64: return (UInt64)us;
                case TypeCode.Single: return (Single)us;
                case TypeCode.Double: return (Double)us;
                case TypeCode.Decimal: return (Decimal)us;
                case TypeCode.DateTime: return new DateTime((Int64)us);
                case TypeCode.String: return us.ToString(CultureInfo.InvariantCulture);
              }
              break;

            case TypeCode.Int32:
              Int32 i = ic.ToInt32(null);
              switch (target){
                case TypeCode.Boolean: return i != 0;
                case TypeCode.Char: return (Char)i;
                case TypeCode.SByte: return (SByte)i;
                case TypeCode.Byte: return (Byte)i;
                case TypeCode.Int16: return (Int16)i;
                case TypeCode.UInt16: return (UInt16)i;
                case TypeCode.Int32: return i;
                case TypeCode.UInt32: return (UInt32)i;
                case TypeCode.Int64: return (Int64)i;
                case TypeCode.UInt64: return (UInt64)i;
                case TypeCode.Single: return (Single)i;
                case TypeCode.Double: return (Double)i;
                case TypeCode.Decimal: return (Decimal)i;
                case TypeCode.DateTime: return new DateTime((Int64)i);
                case TypeCode.String: return i.ToString(CultureInfo.InvariantCulture);
              }
              break;

            case TypeCode.UInt32:
              UInt32 ui = ic.ToUInt32(null);
              switch (target){
                case TypeCode.Boolean: return ui != 0;
                case TypeCode.Char: return (Char)ui;
                case TypeCode.SByte: return (SByte)ui;
                case TypeCode.Byte: return (Byte)ui;
                case TypeCode.Int16: return (Int16)ui;
                case TypeCode.UInt16: return (UInt16)ui;
                case TypeCode.Int32: return (Int32)ui;
                case TypeCode.UInt32: return ui;
                case TypeCode.Int64: return (Int64)ui;
                case TypeCode.UInt64: return (UInt64)ui;
                case TypeCode.Single: return (Single)ui;
                case TypeCode.Double: return (Double)ui;
                case TypeCode.Decimal: return (Decimal)ui;
                case TypeCode.DateTime: return new DateTime((Int64)ui);
                case TypeCode.String: return ui.ToString(CultureInfo.InvariantCulture);
              }
              break;

            case TypeCode.Int64:
              Int64 l = ic.ToInt64(null);
              switch (target){
                case TypeCode.Boolean: return l != 0;
                case TypeCode.Char: return (Char)l;
                case TypeCode.SByte: return (SByte)l;
                case TypeCode.Byte: return (Byte)l;
                case TypeCode.Int16: return (Int16)l;
                case TypeCode.UInt16: return (UInt16)l;
                case TypeCode.Int32: return (Int32)l;
                case TypeCode.UInt32: return (UInt32)l;
                case TypeCode.Int64: return l;
                case TypeCode.UInt64: return (UInt64)l;
                case TypeCode.Single: return (Single)l;
                case TypeCode.Double: return (Double)l;
                case TypeCode.Decimal: return (Decimal)l;
                case TypeCode.DateTime: return new DateTime(l);
                case TypeCode.String: return l.ToString(CultureInfo.InvariantCulture);
              }
              break;

            case TypeCode.UInt64:
              UInt64 ul = ic.ToUInt64(null);
              switch (target){
                case TypeCode.Boolean: return ul != 0;
                case TypeCode.Char: return (Char)ul;
                case TypeCode.SByte: return (SByte)ul;
                case TypeCode.Byte: return (Byte)ul;
                case TypeCode.Int16: return (Int16)ul;
                case TypeCode.UInt16: return (UInt16)ul;
                case TypeCode.Int32: return (Int32)ul;
                case TypeCode.UInt32: return (UInt32)ul;
                case TypeCode.Int64: return (Int64)ul;
                case TypeCode.UInt64: return ul;
                case TypeCode.Single: return (Single)ul;
                case TypeCode.Double: return (Double)ul;
                case TypeCode.Decimal: return (Decimal)ul;
                case TypeCode.DateTime: return new DateTime((Int64)ul);
                case TypeCode.String: return ul.ToString(CultureInfo.InvariantCulture);
              }
              break;

            case TypeCode.Single:
              Single f = ic.ToSingle(null);
              switch (target){
                case TypeCode.Boolean: if (f != f) return false; else return f != 0;
                case TypeCode.Single: return f;
                case TypeCode.Double: return (Double)f;
                case TypeCode.Decimal: return (Decimal)f;
                case TypeCode.String: return Convert.ToString((double)f);
                default:
                  if (System.Math.Round(f) == f){
                    switch (target){
                      case TypeCode.Char: return (Char)f;
                      case TypeCode.SByte: return (SByte)f;
                      case TypeCode.Byte: return (Byte)f;
                      case TypeCode.Int16: return (Int16)f;
                      case TypeCode.UInt16: return (UInt16)f;
                      case TypeCode.Int32: return (Int32)f;
                      case TypeCode.UInt32: return (UInt32)f;
                      case TypeCode.Int64: return (Int64)f;
                      case TypeCode.UInt64: return (UInt64)f;
                      case TypeCode.DateTime: return new DateTime((Int64)f);
                    }
                  }
                  break;
              }
              break;

            case TypeCode.Double:
              Double d = ic.ToDouble(null);
              switch (target){
                case TypeCode.Boolean: return Convert.ToBoolean(d);
                case TypeCode.Single: return (float)d;
                case TypeCode.Double: return d;
                case TypeCode.Decimal: return (Decimal)d;
                case TypeCode.String: return Convert.ToString(d);
                default:
                  if (System.Math.Round(d) == d){
                    switch (target){
                      case TypeCode.Char: return (Char)d;
                      case TypeCode.SByte: return (SByte)d;
                      case TypeCode.Byte: return (Byte)d;
                      case TypeCode.Int16: return (Int16)d;
                      case TypeCode.UInt16: return (UInt16)d;
                      case TypeCode.Int32: return (Int32)d;
                      case TypeCode.UInt32: return (UInt32)d;
                      case TypeCode.Int64: return (Int64)d;
                      case TypeCode.UInt64: return (UInt64)d;
                      case TypeCode.DateTime: return new DateTime((Int64)d);
                    }
                  }
                  break;
              }
              break;

            case TypeCode.Decimal:
              Decimal dec = ic.ToDecimal(null);
              switch (target){
                case TypeCode.Boolean: return dec != 0;
                case TypeCode.Char: return (Char)Decimal.ToUInt16(dec);
                case TypeCode.SByte: return Decimal.ToSByte(dec);
                case TypeCode.Byte: return Decimal.ToByte(dec);
                case TypeCode.Int16:return Decimal.ToInt16(dec);
                case TypeCode.UInt16: return Decimal.ToUInt16(dec);
                case TypeCode.Int32: return Decimal.ToInt32(dec);
                case TypeCode.UInt32: return Decimal.ToUInt32(dec);
                case TypeCode.Int64: return Decimal.ToInt64(dec);
                case TypeCode.UInt64: return Decimal.ToUInt64(dec);
                case TypeCode.Single: return Decimal.ToSingle(dec);
                case TypeCode.Double: return Decimal.ToDouble(dec);
                case TypeCode.Decimal: return dec;
                case TypeCode.DateTime: return new DateTime(Decimal.ToInt64(dec));
                case TypeCode.String: return dec.ToString(CultureInfo.InvariantCulture);
              }
              break;

            case TypeCode.DateTime:
              DateTime dt = ic.ToDateTime(null);
              switch (target){
                case TypeCode.Boolean:
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
                case TypeCode.Decimal: return Coerce2WithNoTrunctation(dt.Ticks, target);
                case TypeCode.DateTime: return dt;
                case TypeCode.String: return dt.ToString(CultureInfo.InvariantCulture);
              }
              break;

            case TypeCode.String:
              String str = ic.ToString(null);
              switch (target){
                case TypeCode.Boolean: return Convert.ToBoolean(str, false);
                case TypeCode.Char:
                  if (str.Length == 1) return str[0];
                  throw new JScriptException(JSError.TypeMismatch);
                case TypeCode.SByte:
                case TypeCode.Byte:
                case TypeCode.Int16:
                case TypeCode.UInt16:
                case TypeCode.Int32:
                case TypeCode.UInt32:
                case TypeCode.Double: return Coerce2WithNoTrunctation(Convert.ToNumber(str), target);
                case TypeCode.Single: try {return Single.Parse(str, CultureInfo.InvariantCulture);}catch{goto case TypeCode.Double;}
                case TypeCode.Int64: try {return Int64.Parse(str, CultureInfo.InvariantCulture);}catch{goto case TypeCode.Double;}
                case TypeCode.UInt64: try {return UInt64.Parse(str, CultureInfo.InvariantCulture);}catch{goto case TypeCode.Double;}
                case TypeCode.Decimal: try {return Decimal.Parse(str, CultureInfo.InvariantCulture);}catch{goto case TypeCode.Double;}
                case TypeCode.DateTime: 
                  try{
                    return DateTime.Parse(str, CultureInfo.InvariantCulture);
                  }catch{
                    return DatePrototype.getVarDate(DateConstructor.ob.CreateInstance(DatePrototype.ParseDate(str)));
                  }
                case TypeCode.String: return str;
              }
              break;
          }
        }}catch(OverflowException){}
        throw new JScriptException(JSError.TypeMismatch);
      }

      //Call this routine only for target TypeTokens other than Object and Empty
      //It special cases the well known types with JScript specific conversion semantics
      private static Object Coerce2WithTruncationPermitted(Object value, TypeCode target){
        if (value is EnumWrapper)
          value = ((EnumWrapper)value).value;
        if (value is ConstantWrapper)
          value = ((ConstantWrapper)value).value;
        IConvertible ic = Convert.GetIConvertible(value);
        switch (Convert.GetTypeCode(value, ic)){
          case TypeCode.Empty:
            switch (target){
              case TypeCode.DBNull: return DBNull.Value;
              case TypeCode.Boolean: return false;
              case TypeCode.Char: return (Char)0;
              case TypeCode.SByte: return (SByte)0;
              case TypeCode.Byte: return (Byte)0;
              case TypeCode.Int16: return (Int16)0;
              case TypeCode.UInt16: return (UInt16)0;
              case TypeCode.Int32: return (Int32)0;
              case TypeCode.UInt32: return (UInt32)0;
              case TypeCode.Int64: return (Int64)0;
              case TypeCode.UInt64: return (UInt64)0;
              case TypeCode.Single: return Single.NaN;
              case TypeCode.Double: return Double.NaN;
              case TypeCode.Decimal: return (Decimal)0;
              case TypeCode.DateTime: return new DateTime((Int64)0);
              case TypeCode.String: return "undefined";
            }
            break;

          case TypeCode.Object:
            if (value is System.Reflection.Missing || (value is Missing && target != TypeCode.Object)) goto case TypeCode.Empty;
            switch (target){
              case TypeCode.Boolean: return Convert.ToBoolean(value, ic);
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
              case TypeCode.Decimal: return Coerce2WithTruncationPermitted(Convert.ToNumber(value, ic), target);
              case TypeCode.DateTime:
                if (value is DateObject)
                  return DatePrototype.getVarDate((DateObject)value);
                else
                  return Coerce2WithTruncationPermitted(Convert.ToNumber(value, ic), target);
              case TypeCode.String: return Convert.ToString(value, ic);
            }
            break;

          case TypeCode.DBNull:
            switch (target){
              case TypeCode.DBNull: return DBNull.Value;
              case TypeCode.Boolean: return false;
              case TypeCode.Char: return (Char)0;
              case TypeCode.SByte: return (SByte)0;
              case TypeCode.Byte: return (Byte)0;
              case TypeCode.Int16: return (Int16)0;
              case TypeCode.UInt16: return (UInt16)0;
              case TypeCode.Int32: return (Int32)0;
              case TypeCode.UInt32: return (UInt32)0;
              case TypeCode.Int64: return (Int64)0;
              case TypeCode.UInt64: return (UInt64)0;
              case TypeCode.Single: return (Single)0;
              case TypeCode.Double: return (Double)0;
              case TypeCode.Decimal: return (Decimal)0;
              case TypeCode.DateTime: return new DateTime((Int64)0);
              case TypeCode.String: return "null";
            }
            break;

          case TypeCode.Boolean:
            bool b = ic.ToBoolean(null);
            int bi = b ? 1 : 0;
            switch (target){
              case TypeCode.Boolean: return b;
              case TypeCode.Char: return (Char)bi;
              case TypeCode.SByte: return (SByte)bi;
              case TypeCode.Byte: return (Byte)bi;
              case TypeCode.Int16: return (Int16)bi;
              case TypeCode.UInt16: return (UInt16)bi;
              case TypeCode.Int32: return (Int32)bi;
              case TypeCode.UInt32: return (UInt32)bi;
              case TypeCode.Int64: return (Int64)bi;
              case TypeCode.UInt64: return (UInt64)bi;
              case TypeCode.Single: return (Single)bi;
              case TypeCode.Double: return (Double)bi;
              case TypeCode.Decimal: return (Decimal)bi;
              case TypeCode.DateTime: return new DateTime((Int64)bi);
              case TypeCode.String: return b ? "true" : "false";
            }
            break;

          case TypeCode.Char:
            Char ch = ic.ToChar(null);
            UInt16 us = (UInt16)ch;
            switch (target){
              case TypeCode.Boolean: return us != 0;
              case TypeCode.Char: return ch;
              case TypeCode.SByte: return (SByte)us;
              case TypeCode.Byte:  return (Byte)us;
              case TypeCode.Int16: return (Int16)us;
              case TypeCode.UInt16: return us;
              case TypeCode.Int32: return (Int32)us;
              case TypeCode.UInt32: return (UInt32)us;
              case TypeCode.Int64: return (Int64)us;
              case TypeCode.UInt64: return (UInt64)us;
              case TypeCode.Single: return (Single)us;
              case TypeCode.Double: return (Double)us;
              case TypeCode.Decimal: return (Decimal)us;
              case TypeCode.DateTime: return new DateTime((Int64)us);
              case TypeCode.String: return Char.ToString(ch);
            }
            break;

          case TypeCode.SByte:
            SByte sb = ic.ToSByte(null);
            switch (target){
              case TypeCode.Boolean: return sb != 0;
              case TypeCode.Char: return (Char)sb;
              case TypeCode.SByte: return sb;
              case TypeCode.Byte: return (Byte)sb;
              case TypeCode.Int16: return (Int16)sb;
              case TypeCode.UInt16: return (UInt16)sb;
              case TypeCode.Int32: return (Int32)sb;
              case TypeCode.UInt32: return (UInt32)sb;
              case TypeCode.Int64: return (Int64)sb;
              case TypeCode.UInt64: return (UInt64)sb;
              case TypeCode.Single: return (Single)sb;
              case TypeCode.Double: return (Double)sb;
              case TypeCode.Decimal: return (Decimal)sb;
              case TypeCode.DateTime: return new DateTime((Int64)sb);
              case TypeCode.String: return sb.ToString(CultureInfo.InvariantCulture);
            }
            break;

          case TypeCode.Byte:
            Byte ub = ic.ToByte(null);
            switch (target){
              case TypeCode.Boolean: return ub != 0;
              case TypeCode.Char: return (Char)ub;
              case TypeCode.SByte: return (SByte)ub;
              case TypeCode.Byte: return ub;
              case TypeCode.Int16: return (Int16)ub;
              case TypeCode.UInt16: return (UInt16)ub;
              case TypeCode.Int32: return (Int32)ub;
              case TypeCode.UInt32: return (UInt32)ub;
              case TypeCode.Int64: return (Int64)ub;
              case TypeCode.UInt64: return (UInt64)ub;
              case TypeCode.Single: return (Single)ub;
              case TypeCode.Double: return (Double)ub;
              case TypeCode.Decimal: return (Decimal)ub;
              case TypeCode.DateTime: return new DateTime((Int64)ub);
              case TypeCode.String: return ub.ToString(CultureInfo.InvariantCulture);
            }
            break;

          case TypeCode.Int16:
            Int16 s = ic.ToInt16(null);
            switch (target){
              case TypeCode.Boolean: return s != 0;
              case TypeCode.Char: return (Char)s;
              case TypeCode.SByte: return (SByte)s;
              case TypeCode.Byte: return (Byte)s;
              case TypeCode.Int16: return s;
              case TypeCode.UInt16: return (UInt16)s;
              case TypeCode.Int32: return (Int32)s;
              case TypeCode.UInt32: return (UInt32)s;
              case TypeCode.Int64: return (Int64)s;
              case TypeCode.UInt64: return (UInt64)s;
              case TypeCode.Single: return (Single)s;
              case TypeCode.Double: return (Double)s;
              case TypeCode.Decimal: return (Decimal)s;
              case TypeCode.DateTime: return new DateTime((Int64)s);
              case TypeCode.String: return s.ToString(CultureInfo.InvariantCulture);
            }
            break;

          case TypeCode.UInt16:
            us = ic.ToUInt16(null);
            switch (target){
              case TypeCode.Boolean: return us != 0;
              case TypeCode.Char: return (Char)us;
              case TypeCode.SByte: return (SByte)us;
              case TypeCode.Byte: return (Byte)us;
              case TypeCode.Int16: return (Int16)us;
              case TypeCode.UInt16: return us;
              case TypeCode.Int32: return (Int32)us;
              case TypeCode.UInt32: return (UInt32)us;
              case TypeCode.Int64: return (Int64)us;
              case TypeCode.UInt64: return (UInt64)us;
              case TypeCode.Single: return (Single)us;
              case TypeCode.Double: return (Double)us;
              case TypeCode.Decimal: return (Decimal)us;
              case TypeCode.DateTime: return new DateTime((Int64)us);
              case TypeCode.String: return us.ToString(CultureInfo.InvariantCulture);
            }
            break;

          case TypeCode.Int32:
            Int32 i = ic.ToInt32(null);
            switch (target){
              case TypeCode.Boolean: return i != 0;
              case TypeCode.Char: return (Char)i;
              case TypeCode.SByte: return (SByte)i;
              case TypeCode.Byte: return (Byte)i;
              case TypeCode.Int16: return (Int16)i;
              case TypeCode.UInt16: return (UInt16)i;
              case TypeCode.Int32: return i;
              case TypeCode.UInt32: return (UInt32)i;
              case TypeCode.Int64: return (Int64)i;
              case TypeCode.UInt64: return (UInt64)i;
              case TypeCode.Single: return (Single)i;
              case TypeCode.Double: return (Double)i;
              case TypeCode.Decimal: return (Decimal)i;
              case TypeCode.DateTime: return new DateTime((Int64)i);
              case TypeCode.String: return i.ToString(CultureInfo.InvariantCulture);
            }
            break;

          case TypeCode.UInt32:
            UInt32 ui = ic.ToUInt32(null);
            switch (target){
              case TypeCode.Boolean: return ui != 0;
              case TypeCode.Char: return (Char)ui;
              case TypeCode.SByte: return (SByte)ui;
              case TypeCode.Byte: return (Byte)ui;
              case TypeCode.Int16: return (Int16)ui;
              case TypeCode.UInt16: return (UInt16)ui;
              case TypeCode.Int32: return (Int32)ui;
              case TypeCode.UInt32: return ui;
              case TypeCode.Int64: return (Int64)ui;
              case TypeCode.UInt64: return (UInt64)ui;
              case TypeCode.Single: return (Single)ui;
              case TypeCode.Double: return (Double)ui;
              case TypeCode.Decimal: return (Decimal)ui;
              case TypeCode.DateTime: return new DateTime((Int64)ui);
              case TypeCode.String: return ui.ToString(CultureInfo.InvariantCulture);
            }
            break;

          case TypeCode.Int64:
            Int64 l = ic.ToInt64(null);
            switch (target){
              case TypeCode.Boolean: return l != 0;
              case TypeCode.Char: return (Char)l;
              case TypeCode.SByte: return (SByte)l;
              case TypeCode.Byte: return (Byte)l;
              case TypeCode.Int16: return (Int16)l;
              case TypeCode.UInt16: return (UInt16)l;
              case TypeCode.Int32: return (Int32)l;
              case TypeCode.UInt32: return (UInt32)l;
              case TypeCode.Int64: return l;
              case TypeCode.UInt64: return (UInt64)l;
              case TypeCode.Single: return (Single)l;
              case TypeCode.Double: return (Double)l;
              case TypeCode.Decimal: return (Decimal)l;
              case TypeCode.DateTime: return new DateTime(l);
              case TypeCode.String: return l.ToString(CultureInfo.InvariantCulture);
            }
            break;

          case TypeCode.UInt64:
            UInt64 ul = ic.ToUInt64(null);
            switch (target){
              case TypeCode.Boolean: return ul != 0;
              case TypeCode.Char: return (Char)ul;
              case TypeCode.SByte: return (SByte)ul;
              case TypeCode.Byte: return (Byte)ul;
              case TypeCode.Int16: return (Int16)ul;
              case TypeCode.UInt16: return (UInt16)ul;
              case TypeCode.Int32: return (Int32)ul;
              case TypeCode.UInt32: return (UInt32)ul;
              case TypeCode.Int64: return (Int64)ul;
              case TypeCode.UInt64: return ul;
              case TypeCode.Single: return (Single)ul;
              case TypeCode.Double: return (Double)ul;
              case TypeCode.Decimal: return (Decimal)ul;
              case TypeCode.DateTime: return new DateTime((Int64)ul);
              case TypeCode.String: return ul.ToString(CultureInfo.InvariantCulture);
            }
            break;

          case TypeCode.Single:
            Single f = ic.ToSingle(null);
            switch (target){
              case TypeCode.Boolean: if (f != f) return false; else return f != 0;
              case TypeCode.Single: return f;
              case TypeCode.Double: return (Double)f;
              case TypeCode.Decimal: return (Decimal)f;
              case TypeCode.String: return Convert.ToString((double)f);
            }

            l = Runtime.DoubleToInt64(f);
            switch (target){
              case TypeCode.Char: return (Char)l;
              case TypeCode.SByte: return (SByte)l;
              case TypeCode.Byte: return (Byte)l;
              case TypeCode.Int16: return (Int16)l;
              case TypeCode.UInt16: return (UInt16)l;
              case TypeCode.Int32: return (Int32)l;
              case TypeCode.UInt32: return (UInt32)l;
              case TypeCode.Int64: return (Int64)l;
              case TypeCode.UInt64: return (UInt64)l;
              case TypeCode.DateTime: return new DateTime((Int64)l);
            }
            break;

          case TypeCode.Double:
            Double d = ic.ToDouble(null);
            switch (target){
              case TypeCode.Boolean: return Convert.ToBoolean(d);
              case TypeCode.Single: return (float)d;
              case TypeCode.Double: return d;
              case TypeCode.Decimal: return (Decimal)d;
              case TypeCode.String: return Convert.ToString(d);
            }
            l = Runtime.DoubleToInt64(d);
            switch (target) {
              case TypeCode.Char: return (Char)l;
              case TypeCode.SByte: return (SByte)l;
              case TypeCode.Byte: return (Byte)l;
              case TypeCode.Int16: return (Int16)l;
              case TypeCode.UInt16: return (UInt16)l;
              case TypeCode.Int32: return (Int32)l;
              case TypeCode.UInt32: return (UInt32)l;
              case TypeCode.Int64: return (Int64)l;
              case TypeCode.UInt64: return (UInt64)l;
              case TypeCode.DateTime: return new DateTime((Int64)l);
            }
            break;

          case TypeCode.Decimal:
            Decimal dec = ic.ToDecimal(null);
            switch (target){
              case TypeCode.Boolean: return dec != 0;
              case TypeCode.Char:
              case TypeCode.SByte:
              case TypeCode.Byte:
              case TypeCode.Int16:
              case TypeCode.UInt16:
              case TypeCode.Int32:
              case TypeCode.UInt32:
              case TypeCode.Int64:
              case TypeCode.UInt64:
                return Coerce2WithTruncationPermitted(Runtime.UncheckedDecimalToInt64(dec), target);
              case TypeCode.Single: return Decimal.ToSingle(dec);
              case TypeCode.Double: return Decimal.ToDouble(dec);
              case TypeCode.Decimal: return dec;
              case TypeCode.DateTime: return new DateTime(Runtime.UncheckedDecimalToInt64(dec));
              case TypeCode.String: return dec.ToString(CultureInfo.InvariantCulture);
            }
            break;

          case TypeCode.DateTime:
            DateTime dt = ic.ToDateTime(null);
            switch (target){
              case TypeCode.Boolean:
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
              case TypeCode.Decimal: return Coerce2WithTruncationPermitted(dt.Ticks, target);
              case TypeCode.DateTime: return dt;
              case TypeCode.String: return dt.ToString(CultureInfo.InvariantCulture);
            }
            break;

          case TypeCode.String:
            String str = ic.ToString(null);
            switch (target){
              case TypeCode.Boolean: return Convert.ToBoolean(str, false);
              case TypeCode.Char:
                if (str.Length == 1) return str[0];
                throw new JScriptException(JSError.TypeMismatch);
              case TypeCode.SByte:
              case TypeCode.Byte:
              case TypeCode.Int16:
              case TypeCode.UInt16:
              case TypeCode.Int32:
              case TypeCode.UInt32:
              case TypeCode.Double: return Coerce2WithTruncationPermitted(Convert.ToNumber(str), target);
              case TypeCode.Single: try {return Single.Parse(str, CultureInfo.InvariantCulture);}catch{goto case TypeCode.Double;}
              case TypeCode.Int64: 
                try{
                  return Int64.Parse(str, CultureInfo.InvariantCulture);
                }catch{
                  try{
                    return (long)UInt64.Parse(str, CultureInfo.InvariantCulture);
                  }catch{
                    goto case TypeCode.Double;
                  }
                }
              case TypeCode.UInt64: try {return UInt64.Parse(str, CultureInfo.InvariantCulture);}catch{goto case TypeCode.Double;}
              case TypeCode.Decimal: try {return Decimal.Parse(str, CultureInfo.InvariantCulture);}catch{goto case TypeCode.Double;}
              case TypeCode.DateTime: return DateTime.Parse(str, CultureInfo.InvariantCulture);
              case TypeCode.String: return str;
            }
            break;
        }
        throw new JScriptException(JSError.TypeMismatch);
      }

      //This function emits a compiled version of Coerce for a particular (source_type, target_type) pair.
      //It assumes that an operand of source_type is already on the operand stack. It consumes this operand
      //and leaves a result of target_type in its place.
      internal static void Emit(AST ast, ILGenerator il, Type source_type, Type target_type){
        Convert.Emit(ast, il, source_type, target_type, false);
      }

      internal static void Emit(AST ast, ILGenerator il, Type source_type, Type target_type, bool truncationPermitted){
        if (source_type == target_type) return;
        if (target_type == Typeob.Void){
          il.Emit(OpCodes.Pop);
          return;
        }
        if (target_type.IsEnum){
          if (source_type == Typeob.String || source_type == Typeob.Object){
            il.Emit(OpCodes.Ldtoken, target_type);
            il.Emit(OpCodes.Call, CompilerGlobals.getTypeFromHandleMethod);
            ConstantWrapper.TranslateToILInt(il, truncationPermitted ? 1 : 0);
            il.Emit(OpCodes.Call, CompilerGlobals.coerceTMethod);
            Convert.EmitUnbox(il, target_type, Type.GetTypeCode(Convert.GetUnderlyingType(target_type)));
          }else
            Convert.Emit(ast, il, source_type, Convert.GetUnderlyingType(target_type));
          return;
        }
        if (source_type.IsEnum){
          if (target_type.IsPrimitive){
            Convert.Emit(ast, il, Convert.GetUnderlyingType(source_type), target_type);
            return;
          }
          if (target_type == Typeob.Object || target_type == Typeob.Enum){
            il.Emit(OpCodes.Box, source_type);
            return;
          }
          if (target_type == Typeob.String){
            il.Emit(OpCodes.Box, source_type);
            ConstantWrapper.TranslateToILInt(il, 0);
            il.Emit(OpCodes.Call, CompilerGlobals.toStringMethod);
            return;
          }
        }

        while (source_type is TypeBuilder){
          source_type = source_type.BaseType;
          if (source_type == null) source_type = Typeob.Object; //It is an interface
          if (source_type == target_type) return;
        }

        if (source_type.IsArray && target_type.IsArray)
          //This should only be called if the two types are known to be compatible, so:
          return;

        TypeCode source = Type.GetTypeCode(source_type);
        TypeCode target = target_type is TypeBuilder ? TypeCode.Object : Type.GetTypeCode(target_type);
        switch (source){
          case TypeCode.Empty: //can never occur
            return;

          case TypeCode.Object:
            if (source_type == Typeob.Void){
              il.Emit(OpCodes.Ldnull);
              source_type = Typeob.Object;
            }
            switch (target){
              case TypeCode.Object:
                //The conversion from function object to delegate never happens here. ConstantWrapper takes care of it.
                //First check for array target type or TypeBuilder target type. These do not support IsAssignableFrom.
                if (target_type.IsArray || target_type == Typeob.Array){
                  if (source_type == Typeob.ArrayObject || source_type == Typeob.Object){
                    if (target_type.IsArray)
                      il.Emit(OpCodes.Ldtoken, target_type.GetElementType());
                    else
                      il.Emit(OpCodes.Ldtoken, Typeob.Object);
                    il.Emit(OpCodes.Call, CompilerGlobals.toNativeArrayMethod);
                  }
                  il.Emit(OpCodes.Castclass, target_type);
                  return;
                }else if (target_type is TypeBuilder){
                  il.Emit(OpCodes.Castclass, target_type);
                  return;
                }else if (target_type == Typeob.Enum && source_type.BaseType == Typeob.Enum){
                  il.Emit(OpCodes.Box, source_type);
                  return;
                }else if (target_type == Typeob.Object || target_type.IsAssignableFrom(source_type)){
                  if (source_type.IsValueType)
                    il.Emit(OpCodes.Box, source_type);
                  return;
                }

                if (Typeob.JSObject.IsAssignableFrom(target_type)){
                  if (source_type.IsValueType)
                    il.Emit(OpCodes.Box, source_type);
                  ast.EmitILToLoadEngine(il);
                  il.Emit(OpCodes.Call, CompilerGlobals.toObject2Method); //Do this here so that we need not pass engine to Coerce
                  il.Emit(OpCodes.Castclass, target_type);
                  return;
                }

                if (Convert.EmittedCallToConversionMethod(ast, il, source_type, target_type))
                  return;

                //Either no conversion is possible, or not enough information is available at compile time.
                //PartialEvaluator is supposed to give errors when conversions are known to be impossible.
                //Defer to run-time type checking
                if (target_type.IsValueType || target_type.IsArray){
                  //Some runtime conversions might be possible
                  il.Emit(OpCodes.Ldtoken, target_type);
                  il.Emit(OpCodes.Call, CompilerGlobals.getTypeFromHandleMethod);
                  ConstantWrapper.TranslateToILInt(il, truncationPermitted ? 1 : 0);
                  il.Emit(OpCodes.Call, CompilerGlobals.coerceTMethod);
                }
                if (target_type.IsValueType)
                  Convert.EmitUnbox(il, target_type, target);
                else
                  il.Emit(OpCodes.Castclass, target_type);
                return;

              case TypeCode.Boolean:
                if (source_type.IsValueType)
                  il.Emit(OpCodes.Box, source_type);
                ConstantWrapper.TranslateToILInt(il, truncationPermitted ? 1 : 0);
                il.Emit(OpCodes.Call, CompilerGlobals.toBooleanMethod);
                return;

              case TypeCode.Single:
                if (source_type.IsValueType)
                  il.Emit(OpCodes.Box, source_type);
                il.Emit(OpCodes.Call, CompilerGlobals.toNumberMethod);
                il.Emit(OpCodes.Conv_R4);
                return;

              case TypeCode.Double:
                if (source_type.IsValueType)
                  il.Emit(OpCodes.Box, source_type);
                il.Emit(OpCodes.Call, CompilerGlobals.toNumberMethod);
                return;

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
              case TypeCode.DateTime:
                if (source_type.IsValueType)
                  il.Emit(OpCodes.Box, source_type);
                if (truncationPermitted && target == TypeCode.Int32){
                  il.Emit(OpCodes.Call, CompilerGlobals.toInt32Method);
                  return;
                }else{
                  ConstantWrapper.TranslateToILInt(il, (int)target);
                  ConstantWrapper.TranslateToILInt(il, truncationPermitted ? 1 : 0);
                  il.Emit(OpCodes.Call, CompilerGlobals.coerce2Method);
                }
                if (target_type.IsValueType)
                  Convert.EmitUnbox(il, target_type, target);
                return;

              case TypeCode.String:
                if (source_type.IsValueType)
                  il.Emit(OpCodes.Box, source_type);
                if (truncationPermitted) //explict cast to type string
                  il.Emit(OpCodes.Castclass, Typeob.String);
                else{
                  ConstantWrapper.TranslateToILInt(il, 1);
                  il.Emit(OpCodes.Call, CompilerGlobals.toStringMethod);
                }
                return;
            }
            return;

          case TypeCode.DBNull:
            if (source_type.IsValueType) //Not likely, but a hacker might cause this to be true.
              il.Emit(OpCodes.Box, source_type);
            if (target == TypeCode.Object || (target == TypeCode.String && !truncationPermitted)){
              if (target_type == Typeob.Object)
                return;
              if (!target_type.IsValueType){
                il.Emit(OpCodes.Pop);
                il.Emit(OpCodes.Ldnull);
                return;
              }
            }
            if (target_type.IsValueType){
              il.Emit(OpCodes.Ldtoken, target_type);
              il.Emit(OpCodes.Call, CompilerGlobals.getTypeFromHandleMethod);
              ConstantWrapper.TranslateToILInt(il, truncationPermitted ? 1 : 0);
              il.Emit(OpCodes.Call, CompilerGlobals.coerceTMethod);
              Convert.EmitUnbox(il, target_type, target);
            }else{
              ConstantWrapper.TranslateToILInt(il, (int)target);
              ConstantWrapper.TranslateToILInt(il, truncationPermitted ? 1 : 0);
              il.Emit(OpCodes.Call, CompilerGlobals.coerce2Method); //constructs and boxes whatever value is needed
            }
            return;

          case TypeCode.Boolean:
            switch (target){
              case TypeCode.Object:
                if (target_type != Typeob.Object && Convert.EmittedCallToConversionMethod(ast, il, source_type, target_type))
                  return;
                il.Emit(OpCodes.Box, source_type);
                Convert.Emit(ast, il, Typeob.Object, target_type);
                return;
              case TypeCode.Boolean:
              case TypeCode.Char:
              case TypeCode.SByte:
              case TypeCode.Int16:
              case TypeCode.Int32:
              case TypeCode.Byte:
              case TypeCode.UInt16:
              case TypeCode.UInt32:
                 return;
              case TypeCode.Int64:
              case TypeCode.UInt64:
                il.Emit(OpCodes.Conv_U8);
                return;
              case TypeCode.Single:
                il.Emit(OpCodes.Conv_R4);
                return;
              case TypeCode.Double:
                il.Emit(OpCodes.Conv_R8);
                return;
              case TypeCode.Decimal:
                il.Emit(OpCodes.Call, CompilerGlobals.int32ToDecimalMethod);
                return;
              case TypeCode.DateTime:
                il.Emit(OpCodes.Conv_I8);
                il.Emit(OpCodes.Newobj, CompilerGlobals.dateTimeConstructor);
                return;
              case TypeCode.String:
                Label false_label = il.DefineLabel();
                Label end_label = il.DefineLabel();
                il.Emit(OpCodes.Brfalse, false_label);
                il.Emit(OpCodes.Ldstr, "true");
                il.Emit(OpCodes.Br, end_label);
                il.MarkLabel(false_label);
                il.Emit(OpCodes.Ldstr, "false");
                il.MarkLabel(end_label);
                return;
            }
            break;

          case TypeCode.SByte:
            switch (target){
              case TypeCode.Object:
                if (target_type != Typeob.Object && Convert.EmittedCallToConversionMethod(ast, il, source_type, target_type))
                  return;
                il.Emit(OpCodes.Box, source_type);
                Convert.Emit(ast, il, Typeob.Object, target_type);
                return;
              case TypeCode.SByte:
              case TypeCode.Int16:
              case TypeCode.Int32:
                return;
              case TypeCode.Byte:
                if (truncationPermitted)
                  il.Emit(OpCodes.Conv_U1);
                else
                  il.Emit(OpCodes.Conv_Ovf_U1);
                return;
              case TypeCode.Char:
              case TypeCode.UInt16:
                if (truncationPermitted)
                  il.Emit(OpCodes.Conv_U2);
                else
                  il.Emit(OpCodes.Conv_Ovf_U2);
                return;
              case TypeCode.UInt32:
                if (truncationPermitted)
                  il.Emit(OpCodes.Conv_U4);
                else
                  il.Emit(OpCodes.Conv_Ovf_U4);
                return;
              case TypeCode.Int64:
                il.Emit(OpCodes.Conv_I8);
                return;
              case TypeCode.UInt64:
                if (truncationPermitted)
                  il.Emit(OpCodes.Conv_I8);
                else
                  il.Emit(OpCodes.Conv_Ovf_U8);
                return;
              case TypeCode.Single:
              case TypeCode.Double:
                il.Emit(OpCodes.Conv_R8);
                return;
              case TypeCode.Boolean:
                il.Emit(OpCodes.Ldc_I4_0);
                il.Emit(OpCodes.Ceq);
                il.Emit(OpCodes.Ldc_I4_0);
                il.Emit(OpCodes.Ceq);
                return;
              case TypeCode.Decimal:
                il.Emit(OpCodes.Call, CompilerGlobals.int32ToDecimalMethod);
                return;
              case TypeCode.DateTime:
                il.Emit(OpCodes.Conv_I8);
                il.Emit(OpCodes.Newobj, CompilerGlobals.dateTimeConstructor);
                return;
              case TypeCode.String:
                Convert.EmitLdloca(il, Typeob.Int32);
                il.Emit(OpCodes.Call, CompilerGlobals.int32ToStringMethod);
                return;
            }
            break;

          case TypeCode.Byte:
            switch (target){
              case TypeCode.Object:
                if (target_type != Typeob.Object && Convert.EmittedCallToConversionMethod(ast, il, source_type, target_type))
                  return;
                il.Emit(OpCodes.Box, source_type);
                Convert.Emit(ast, il, Typeob.Object, target_type);
                return;
              case TypeCode.SByte:
                if (truncationPermitted)
                  il.Emit(OpCodes.Conv_I1);
                else
                  il.Emit(OpCodes.Conv_Ovf_I1_Un);
                return;
              case TypeCode.Byte:
              case TypeCode.Int16:
              case TypeCode.Char:
              case TypeCode.UInt16:
              case TypeCode.Int32:
              case TypeCode.UInt32:
                return;
              case TypeCode.Int64:
              case TypeCode.UInt64:
                il.Emit(OpCodes.Conv_U8);
                return;
              case TypeCode.Single:
              case TypeCode.Double:
                il.Emit(OpCodes.Conv_R_Un);
                return;
              case TypeCode.Boolean:
                il.Emit(OpCodes.Ldc_I4_0);
                il.Emit(OpCodes.Ceq);
                il.Emit(OpCodes.Ldc_I4_0);
                il.Emit(OpCodes.Ceq);
                return;
              case TypeCode.Decimal:
                il.Emit(OpCodes.Call, CompilerGlobals.uint32ToDecimalMethod);
                return;
              case TypeCode.DateTime:
                il.Emit(OpCodes.Conv_I8);
                il.Emit(OpCodes.Newobj, CompilerGlobals.dateTimeConstructor);
                return;
              case TypeCode.String:
                Convert.EmitLdloca(il, Typeob.UInt32);
                il.Emit(OpCodes.Call, CompilerGlobals.uint32ToStringMethod);
                return;
            }
            break;

          case TypeCode.Int16:
            switch (target){
              case TypeCode.Object:
                if (target_type != Typeob.Object && Convert.EmittedCallToConversionMethod(ast, il, source_type, target_type))
                  return;
                il.Emit(OpCodes.Box, source_type);
                Convert.Emit(ast, il, Typeob.Object, target_type);
                return;
              case TypeCode.SByte:
                if (truncationPermitted)
                  il.Emit(OpCodes.Conv_I1);
                else
                  il.Emit(OpCodes.Conv_Ovf_I1);
                return;
              case TypeCode.Int16:
              case TypeCode.Int32:
                return;
              case TypeCode.Byte:
                if (truncationPermitted)
                  il.Emit(OpCodes.Conv_U1);
                else
                  il.Emit(OpCodes.Conv_Ovf_U1);
                return;
              case TypeCode.Char:
              case TypeCode.UInt16:
                if (truncationPermitted)
                  il.Emit(OpCodes.Conv_U2);
                else
                  il.Emit(OpCodes.Conv_Ovf_U2);
                return;
              case TypeCode.UInt32:
                if (truncationPermitted)
                  il.Emit(OpCodes.Conv_U4);
                else
                  il.Emit(OpCodes.Conv_Ovf_U4);
                return;
              case TypeCode.Int64:
                il.Emit(OpCodes.Conv_I8);
                return;
              case TypeCode.UInt64:
                if (truncationPermitted)
                  il.Emit(OpCodes.Conv_I8);
                else
                  il.Emit(OpCodes.Conv_Ovf_U8);
                return;
              case TypeCode.Single:
              case TypeCode.Double:
                il.Emit(OpCodes.Conv_R8);
                return;
              case TypeCode.Boolean:
                il.Emit(OpCodes.Ldc_I4_0);
                il.Emit(OpCodes.Ceq);
                il.Emit(OpCodes.Ldc_I4_0);
                il.Emit(OpCodes.Ceq);
                return;
              case TypeCode.Decimal:
                il.Emit(OpCodes.Call, CompilerGlobals.int32ToDecimalMethod);
                return;
              case TypeCode.DateTime:
                il.Emit(OpCodes.Conv_I8);
                il.Emit(OpCodes.Newobj, CompilerGlobals.dateTimeConstructor);
                return;
              case TypeCode.String:
                Convert.EmitLdloca(il, Typeob.Int32);
                il.Emit(OpCodes.Call, CompilerGlobals.int32ToStringMethod);
                return;
            }
            break;

          case TypeCode.Char:
          case TypeCode.UInt16:
            switch (target){
              case TypeCode.Object:
                if (target_type != Typeob.Object && Convert.EmittedCallToConversionMethod(ast, il, source_type, target_type))
                  return;
                il.Emit(OpCodes.Box, source_type);
                Convert.Emit(ast, il, Typeob.Object, target_type);
                return;
              case TypeCode.SByte:
                if (truncationPermitted)
                  il.Emit(OpCodes.Conv_I1);
                else
                  il.Emit(OpCodes.Conv_Ovf_I1);
                return;
              case TypeCode.Byte:
                if (truncationPermitted)
                  il.Emit(OpCodes.Conv_U1);
                else
                  il.Emit(OpCodes.Conv_Ovf_U1);
                return;
              case TypeCode.Int16:
                if (truncationPermitted)
                  il.Emit(OpCodes.Conv_I2);
                else
                  il.Emit(OpCodes.Conv_Ovf_I2);
                return;
              case TypeCode.Char:
              case TypeCode.UInt16:
              case TypeCode.Int32:
              case TypeCode.UInt32:
                return;
              case TypeCode.Int64:
                il.Emit(OpCodes.Conv_I8);
                return;
              case TypeCode.UInt64:
                il.Emit(OpCodes.Conv_U8);
                return;
              case TypeCode.Single:
              case TypeCode.Double:
                il.Emit(OpCodes.Conv_R_Un);
                return;
              case TypeCode.Boolean:
                il.Emit(OpCodes.Ldc_I4_0);
                il.Emit(OpCodes.Ceq);
                il.Emit(OpCodes.Ldc_I4_0);
                il.Emit(OpCodes.Ceq);
                return;
              case TypeCode.Decimal:
                il.Emit(OpCodes.Call, CompilerGlobals.uint32ToDecimalMethod);
                return;
              case TypeCode.DateTime:
                il.Emit(OpCodes.Conv_I8);
                il.Emit(OpCodes.Newobj, CompilerGlobals.dateTimeConstructor);
                return;
              case TypeCode.String:
                if (source == TypeCode.Char)
                  il.Emit(OpCodes.Call, CompilerGlobals.convertCharToStringMethod);
                else{
                  Convert.EmitLdloca(il, Typeob.UInt32);
                  il.Emit(OpCodes.Call, CompilerGlobals.uint32ToStringMethod);
                }
                return;
            }
            break;

          case TypeCode.Int32:
            switch (target){
              case TypeCode.Object:
                if (target_type != Typeob.Object && Convert.EmittedCallToConversionMethod(ast, il, source_type, target_type))
                  return;
                il.Emit(OpCodes.Box, source_type);
                Convert.Emit(ast, il, Typeob.Object, target_type);
                return;
              case TypeCode.SByte:
                if (truncationPermitted)
                  il.Emit(OpCodes.Conv_I1);
                else
                  il.Emit(OpCodes.Conv_Ovf_I1);
                return;
              case TypeCode.Int16:
                if (truncationPermitted)
                  il.Emit(OpCodes.Conv_I2);
                else
                  il.Emit(OpCodes.Conv_Ovf_I2);
                return;
              case TypeCode.Int32:
                return;
              case TypeCode.Byte:
                if (truncationPermitted)
                  il.Emit(OpCodes.Conv_U1);
                else
                  il.Emit(OpCodes.Conv_Ovf_U1);
                return;
              case TypeCode.Char:
              case TypeCode.UInt16:
                if (truncationPermitted)
                  il.Emit(OpCodes.Conv_U2);
                else
                  il.Emit(OpCodes.Conv_Ovf_U2);
                return;
              case TypeCode.UInt32:
                if (truncationPermitted)
                  il.Emit(OpCodes.Conv_U4);
                else
                  il.Emit(OpCodes.Conv_Ovf_U4);
                return;
              case TypeCode.Int64:
                il.Emit(OpCodes.Conv_I8);
                return;
              case TypeCode.UInt64:
                if (truncationPermitted)
                  il.Emit(OpCodes.Conv_U8);
                else
                  il.Emit(OpCodes.Conv_Ovf_U8);
                return;
              case TypeCode.Single:
              case TypeCode.Double:
                il.Emit(OpCodes.Conv_R8);
                return;
              case TypeCode.Boolean:
                il.Emit(OpCodes.Ldc_I4_0);
                il.Emit(OpCodes.Ceq);
                il.Emit(OpCodes.Ldc_I4_0);
                il.Emit(OpCodes.Ceq);
                return;
              case TypeCode.Decimal:
                il.Emit(OpCodes.Call, CompilerGlobals.int32ToDecimalMethod);
                return;
              case TypeCode.DateTime:
                il.Emit(OpCodes.Conv_I8);
                il.Emit(OpCodes.Newobj, CompilerGlobals.dateTimeConstructor);
                return;
              case TypeCode.String:
                Convert.EmitLdloca(il, Typeob.Int32);
                il.Emit(OpCodes.Call, CompilerGlobals.int32ToStringMethod);
                return;
            }
            break;

          case TypeCode.UInt32:
            switch (target){
              case TypeCode.Object:
                if (target_type != Typeob.Object && Convert.EmittedCallToConversionMethod(ast, il, source_type, target_type))
                  return;
                il.Emit(OpCodes.Box, source_type);
                Convert.Emit(ast, il, Typeob.Object, target_type);
                return;
              case TypeCode.SByte:
                if (truncationPermitted)
                  il.Emit(OpCodes.Conv_I1);
                else
                  il.Emit(OpCodes.Conv_Ovf_I1);
                return;
              case TypeCode.Byte:
                if (truncationPermitted)
                  il.Emit(OpCodes.Conv_U1);
                else
                  il.Emit(OpCodes.Conv_Ovf_U1);
                return;
              case TypeCode.Int16:
                if (truncationPermitted)
                  il.Emit(OpCodes.Conv_I2);
                else
                  il.Emit(OpCodes.Conv_Ovf_I2);
                return;
              case TypeCode.Char:
              case TypeCode.UInt16:
                if (truncationPermitted)
                  il.Emit(OpCodes.Conv_U2);
                else
                  il.Emit(OpCodes.Conv_Ovf_U2);
                return;
              case TypeCode.Int32:
                if (truncationPermitted)
                  il.Emit(OpCodes.Conv_I4);
                else
                  il.Emit(OpCodes.Conv_Ovf_I4_Un);
                return;
              case TypeCode.UInt32:
                return;
              case TypeCode.Int64:
                il.Emit(OpCodes.Conv_I8);
                return;
              case TypeCode.UInt64:
                il.Emit(OpCodes.Conv_U8);
                return;
              case TypeCode.Single:
              case TypeCode.Double:
                il.Emit(OpCodes.Conv_R_Un);
                return;
              case TypeCode.Boolean:
                il.Emit(OpCodes.Ldc_I4_0);
                il.Emit(OpCodes.Ceq);
                il.Emit(OpCodes.Ldc_I4_0);
                il.Emit(OpCodes.Ceq);
                return;
              case TypeCode.Decimal:
                il.Emit(OpCodes.Call, CompilerGlobals.uint32ToDecimalMethod);
                return;
              case TypeCode.DateTime:
                il.Emit(OpCodes.Conv_I8);
                il.Emit(OpCodes.Newobj, CompilerGlobals.dateTimeConstructor);
                return;
              case TypeCode.String:
                Convert.EmitLdloca(il, Typeob.UInt32);
                il.Emit(OpCodes.Call, CompilerGlobals.uint32ToStringMethod);
                return;
            }
            break;

          case TypeCode.Int64:
            switch (target){
              case TypeCode.Object:
                if (target_type != Typeob.Object && Convert.EmittedCallToConversionMethod(ast, il, source_type, target_type))
                  return;
                il.Emit(OpCodes.Box, source_type);
                Convert.Emit(ast, il, Typeob.Object, target_type);
                return;
              case TypeCode.SByte:
                if (truncationPermitted)
                  il.Emit(OpCodes.Conv_I1);
                else
                  il.Emit(OpCodes.Conv_Ovf_I1);
                return;
              case TypeCode.Int16:
                if (truncationPermitted)
                  il.Emit(OpCodes.Conv_I2);
                else
                  il.Emit(OpCodes.Conv_Ovf_I2);
                return;
              case TypeCode.Int32:
                if (truncationPermitted)
                  il.Emit(OpCodes.Conv_I4);
                else
                  il.Emit(OpCodes.Conv_Ovf_I4);
                return;
              case TypeCode.Byte:
                if (truncationPermitted)
                  il.Emit(OpCodes.Conv_U1);
                else
                  il.Emit(OpCodes.Conv_Ovf_U1);
                return;
              case TypeCode.Char:
              case TypeCode.UInt16:
                if (truncationPermitted)
                  il.Emit(OpCodes.Conv_U2);
                else
                  il.Emit(OpCodes.Conv_Ovf_U2);
                return;
              case TypeCode.UInt32:
                if (truncationPermitted)
                  il.Emit(OpCodes.Conv_U4);
                else
                  il.Emit(OpCodes.Conv_Ovf_U4);
                return;
              case TypeCode.Int64:
                return;
              case TypeCode.UInt64:
                if (truncationPermitted)
                  il.Emit(OpCodes.Conv_U8);
                else
                  il.Emit(OpCodes.Conv_Ovf_U8);
                return;
              case TypeCode.Single:
              case TypeCode.Double:
                il.Emit(OpCodes.Conv_R8);
                return;
              case TypeCode.Boolean:
                il.Emit(OpCodes.Ldc_I4_0);
                il.Emit(OpCodes.Conv_I8);
                il.Emit(OpCodes.Ceq);
                il.Emit(OpCodes.Ldc_I4_0);
                il.Emit(OpCodes.Ceq);
                return;
              case TypeCode.Decimal:
                il.Emit(OpCodes.Call, CompilerGlobals.int64ToDecimalMethod);
                return;
              case TypeCode.DateTime:
                il.Emit(OpCodes.Newobj, CompilerGlobals.dateTimeConstructor);
                return;
              case TypeCode.String:
                Convert.EmitLdloca(il, Typeob.Int64);
                il.Emit(OpCodes.Call, CompilerGlobals.int64ToStringMethod);
                return;
            }
            break;

          case TypeCode.UInt64:
            switch (target){
              case TypeCode.Object:
                if (target_type != Typeob.Object && Convert.EmittedCallToConversionMethod(ast, il, source_type, target_type))
                  return;
                il.Emit(OpCodes.Box, source_type);
                Convert.Emit(ast, il, Typeob.Object, target_type);
                return;
              case TypeCode.SByte:
                if (truncationPermitted)
                  il.Emit(OpCodes.Conv_I1);
                else
                  il.Emit(OpCodes.Conv_Ovf_I1);
                return;
              case TypeCode.Byte:
                if (truncationPermitted)
                  il.Emit(OpCodes.Conv_U1);
                else
                  il.Emit(OpCodes.Conv_Ovf_U1);
                return;
              case TypeCode.Int16:
                if (truncationPermitted)
                  il.Emit(OpCodes.Conv_I2);
                else
                  il.Emit(OpCodes.Conv_Ovf_I2);
                return;
              case TypeCode.Char:
              case TypeCode.UInt16:
                if (truncationPermitted)
                  il.Emit(OpCodes.Conv_U2);
                else
                  il.Emit(OpCodes.Conv_Ovf_U2);
                return;
              case TypeCode.Int32:
                if (truncationPermitted)
                  il.Emit(OpCodes.Conv_I4);
                else
                  il.Emit(OpCodes.Conv_Ovf_I4);
                return;
              case TypeCode.UInt32:
                if (truncationPermitted)
                  il.Emit(OpCodes.Conv_U4);
                else
                  il.Emit(OpCodes.Conv_Ovf_U4);
                return;
              case TypeCode.Int64:
                if (truncationPermitted)
                  il.Emit(OpCodes.Conv_I8);
                else
                  il.Emit(OpCodes.Conv_Ovf_I8_Un);
                return;
              case TypeCode.UInt64:
                return;
              case TypeCode.Single:
              case TypeCode.Double:
                il.Emit(OpCodes.Conv_R_Un);
                return;
              case TypeCode.Boolean:
                il.Emit(OpCodes.Ldc_I4_0);
                il.Emit(OpCodes.Conv_I8);
                il.Emit(OpCodes.Ceq);
                il.Emit(OpCodes.Ldc_I4_0);
                il.Emit(OpCodes.Ceq);
                return;
              case TypeCode.Decimal:
                il.Emit(OpCodes.Call, CompilerGlobals.uint64ToDecimalMethod);
                return;
              case TypeCode.DateTime:
                il.Emit(OpCodes.Newobj, CompilerGlobals.dateTimeConstructor);
                return;
              case TypeCode.String:
                Convert.EmitLdloca(il, Typeob.UInt64);
                il.Emit(OpCodes.Call, CompilerGlobals.uint64ToStringMethod);
                return;
            }
            break;

          case TypeCode.Single:
            switch (target){
              case TypeCode.Object:
                if (target_type != Typeob.Object && Convert.EmittedCallToConversionMethod(ast, il, source_type, target_type))
                  return;
                il.Emit(OpCodes.Box, source_type);
                Convert.Emit(ast, il, Typeob.Object, target_type);
                return;
              case TypeCode.SByte:
                if (truncationPermitted)
                  Convert.EmitSingleToIntegerTruncatedConversion(il, OpCodes.Conv_I1);
                else{
                  il.Emit(OpCodes.Call, CompilerGlobals.checkIfSingleIsIntegerMethod);
                  il.Emit(OpCodes.Conv_Ovf_I1);
                }
                return;
              case TypeCode.Int16:
                if (truncationPermitted)
                  Convert.EmitSingleToIntegerTruncatedConversion(il, OpCodes.Conv_I2);
                else{
                  il.Emit(OpCodes.Call, CompilerGlobals.checkIfSingleIsIntegerMethod);
                  il.Emit(OpCodes.Conv_Ovf_I2);
                }
                return;
              case TypeCode.Int32:
                if (truncationPermitted)
                  Convert.EmitSingleToIntegerTruncatedConversion(il, OpCodes.Conv_I4);
                else{
                  il.Emit(OpCodes.Call, CompilerGlobals.checkIfSingleIsIntegerMethod);
                  il.Emit(OpCodes.Conv_I4);
                }
                return;
              case TypeCode.Byte:
                if (truncationPermitted)
                  Convert.EmitSingleToIntegerTruncatedConversion(il, OpCodes.Conv_U1);
                else{
                  il.Emit(OpCodes.Call, CompilerGlobals.checkIfSingleIsIntegerMethod);
                  il.Emit(OpCodes.Conv_Ovf_U1);
                }
                return;
              case TypeCode.Char:
              case TypeCode.UInt16:
                if (truncationPermitted)
                  Convert.EmitSingleToIntegerTruncatedConversion(il, OpCodes.Conv_U2);
                else{
                  il.Emit(OpCodes.Call, CompilerGlobals.checkIfSingleIsIntegerMethod);
                  il.Emit(OpCodes.Conv_Ovf_U2);
                }
                return;
              case TypeCode.UInt32:
                if (truncationPermitted)
                  Convert.EmitSingleToIntegerTruncatedConversion(il, OpCodes.Conv_Ovf_U4);
                else{
                  il.Emit(OpCodes.Call, CompilerGlobals.checkIfSingleIsIntegerMethod);
                  il.Emit(OpCodes.Conv_Ovf_U4);
                }
                return;
              case TypeCode.Int64:
                if (truncationPermitted)
                  Convert.EmitSingleToIntegerTruncatedConversion(il, OpCodes.Conv_I8);
                else{
                  il.Emit(OpCodes.Call, CompilerGlobals.checkIfSingleIsIntegerMethod);
                  il.Emit(OpCodes.Conv_I8);
                }
                return;
              case TypeCode.UInt64:
                if (truncationPermitted)
                  Convert.EmitSingleToIntegerTruncatedConversion(il, OpCodes.Conv_U8);
                else{
                  il.Emit(OpCodes.Call, CompilerGlobals.checkIfSingleIsIntegerMethod);
                  il.Emit(OpCodes.Conv_Ovf_U8);
                }
                return;
              case TypeCode.Single:
              case TypeCode.Double:
                return;
              case TypeCode.DateTime:
                if (truncationPermitted)
                  Convert.EmitSingleToIntegerTruncatedConversion(il, OpCodes.Conv_I8);
                else{
                  il.Emit(OpCodes.Call, CompilerGlobals.checkIfSingleIsIntegerMethod);
                  il.Emit(OpCodes.Conv_Ovf_I8);
                }
                il.Emit(OpCodes.Newobj, CompilerGlobals.dateTimeConstructor);
                return;
              case TypeCode.Boolean:
              case TypeCode.Decimal:
              case TypeCode.String:
                il.Emit(OpCodes.Conv_R8);
                Convert.Emit(ast, il, Typeob.Double, target_type);
                return;
            }
            break;

          case TypeCode.Double:
            switch (target){
              case TypeCode.Object:
                if (target_type != Typeob.Object && Convert.EmittedCallToConversionMethod(ast, il, source_type, target_type))
                  return;
                il.Emit(OpCodes.Box, source_type);
                Convert.Emit(ast, il, Typeob.Object, target_type);
                return;
              case TypeCode.SByte:
                if (truncationPermitted)
                  Convert.EmitDoubleToIntegerTruncatedConversion(il, OpCodes.Conv_I1);
                else{
                  il.Emit(OpCodes.Call, CompilerGlobals.checkIfDoubleIsIntegerMethod);
                  il.Emit(OpCodes.Conv_Ovf_I1);
                }
                return;
              case TypeCode.Int16:
                if (truncationPermitted)
                  Convert.EmitDoubleToIntegerTruncatedConversion(il, OpCodes.Conv_I2);
                else{
                  il.Emit(OpCodes.Call, CompilerGlobals.checkIfDoubleIsIntegerMethod);
                  il.Emit(OpCodes.Conv_Ovf_I2);
                }
                return;
              case TypeCode.Int32:
                if (truncationPermitted)
                  Convert.EmitDoubleToIntegerTruncatedConversion(il, OpCodes.Conv_I4);
                else{
                  il.Emit(OpCodes.Call, CompilerGlobals.checkIfDoubleIsIntegerMethod);
                  il.Emit(OpCodes.Conv_Ovf_I4);
                }
                return;
              case TypeCode.Byte:
                if (truncationPermitted)
                  Convert.EmitDoubleToIntegerTruncatedConversion(il, OpCodes.Conv_U1);
                else{
                  il.Emit(OpCodes.Call, CompilerGlobals.checkIfDoubleIsIntegerMethod);
                  il.Emit(OpCodes.Conv_Ovf_U1);
                }
                return;
              case TypeCode.Char:
              case TypeCode.UInt16:
                if (truncationPermitted)
                  Convert.EmitDoubleToIntegerTruncatedConversion(il, OpCodes.Conv_U2);
                else{
                  il.Emit(OpCodes.Call, CompilerGlobals.checkIfDoubleIsIntegerMethod);
                  il.Emit(OpCodes.Conv_Ovf_U2);
                }
                return;
              case TypeCode.UInt32:
                if (truncationPermitted)
                  Convert.EmitDoubleToIntegerTruncatedConversion(il, OpCodes.Conv_U4);
                else{
                  il.Emit(OpCodes.Call, CompilerGlobals.checkIfDoubleIsIntegerMethod);
                  il.Emit(OpCodes.Conv_Ovf_U4);
                }
                return;
              case TypeCode.Int64:
                if (truncationPermitted)
                  Convert.EmitDoubleToIntegerTruncatedConversion(il, OpCodes.Conv_I8);
                else{
                  il.Emit(OpCodes.Call, CompilerGlobals.checkIfDoubleIsIntegerMethod);
                  il.Emit(OpCodes.Conv_I8);
                }
                return;
              case TypeCode.UInt64:
                if (truncationPermitted)
                  Convert.EmitDoubleToIntegerTruncatedConversion(il, OpCodes.Conv_U8);
                else{
                  il.Emit(OpCodes.Call, CompilerGlobals.checkIfDoubleIsIntegerMethod);
                  il.Emit(OpCodes.Conv_Ovf_U8);
                }
                return;
              case TypeCode.Single:
              case TypeCode.Double:
                return;
              case TypeCode.Boolean:
                il.Emit(OpCodes.Call, CompilerGlobals.doubleToBooleanMethod);
                return;
              case TypeCode.Decimal:
                il.Emit(OpCodes.Call, CompilerGlobals.doubleToDecimalMethod);
                return;
              case TypeCode.DateTime:
                if (truncationPermitted)
                  Convert.EmitDoubleToIntegerTruncatedConversion(il, OpCodes.Conv_I8);
                else{
                  il.Emit(OpCodes.Call, CompilerGlobals.checkIfSingleIsIntegerMethod);
                  il.Emit(OpCodes.Conv_Ovf_I8);
                }
                il.Emit(OpCodes.Newobj, CompilerGlobals.dateTimeConstructor);
                return;
              case TypeCode.String:
                il.Emit(OpCodes.Call, CompilerGlobals.doubleToStringMethod);
                return;
            }
            break;

          case TypeCode.Decimal:
            switch (target){
              case TypeCode.Object:
                if (target_type != Typeob.Object && Convert.EmittedCallToConversionMethod(ast, il, source_type, target_type))
                  return;
                il.Emit(OpCodes.Box, source_type);
                Convert.Emit(ast, il, Typeob.Object, target_type);
                return;
              case TypeCode.Char:
              case TypeCode.SByte:
              case TypeCode.Byte:
              case TypeCode.Int16:
              case TypeCode.UInt16:
              case TypeCode.Int32:
                if (truncationPermitted){
                  Convert.EmitDecimalToIntegerTruncatedConversion(il, OpCodes.Conv_I4);
                }else
                  il.Emit(OpCodes.Call, CompilerGlobals.decimalToInt32Method);
                Convert.Emit(ast, il, Typeob.Int32, target_type, truncationPermitted);
                return;
              case TypeCode.UInt32:
                if (truncationPermitted){
                  Convert.EmitDecimalToIntegerTruncatedConversion(il, OpCodes.Conv_U4);
                }else
                  il.Emit(OpCodes.Call, CompilerGlobals.decimalToUInt32Method);
                return;
              case TypeCode.Boolean:
                il.Emit(OpCodes.Ldsfld, CompilerGlobals.decimalZeroField);
                il.Emit(OpCodes.Call, CompilerGlobals.decimalCompare);
                il.Emit(OpCodes.Ldc_I4_0);
                il.Emit(OpCodes.Ceq);
                il.Emit(OpCodes.Ldc_I4_0);
                il.Emit(OpCodes.Ceq);
                return;
              case TypeCode.Single:
              case TypeCode.Double:
                il.Emit(OpCodes.Call, CompilerGlobals.decimalToDoubleMethod);
                Convert.Emit(ast, il, Typeob.Double, target_type, truncationPermitted);
                return;
              case TypeCode.Int64:
                if (truncationPermitted){
                  Convert.EmitDecimalToIntegerTruncatedConversion(il, OpCodes.Conv_I8);
                }else
                  il.Emit(OpCodes.Call, CompilerGlobals.decimalToInt64Method);
                return;
              case TypeCode.UInt64:
                if (truncationPermitted){
                  Convert.EmitDecimalToIntegerTruncatedConversion(il, OpCodes.Conv_U8);
                }else
                  il.Emit(OpCodes.Call, CompilerGlobals.decimalToUInt64Method);
                return;
              case TypeCode.Decimal:
                return;
              case TypeCode.DateTime:
                if (truncationPermitted){
                  Convert.EmitDecimalToIntegerTruncatedConversion(il, OpCodes.Conv_I8);
                }else
                  il.Emit(OpCodes.Call, CompilerGlobals.decimalToInt64Method);
                Convert.Emit(ast, il, Typeob.Int64, target_type);
                return;
              case TypeCode.String:
                Convert.EmitLdloca(il, source_type);
                il.Emit(OpCodes.Call, CompilerGlobals.decimalToStringMethod);
                return;
            }
            break;

          case TypeCode.DateTime:
            switch (target){
              case TypeCode.Object:
                if (target_type != Typeob.Object && Convert.EmittedCallToConversionMethod(ast, il, source_type, target_type))
                  return;
                il.Emit(OpCodes.Box, source_type);
                Convert.Emit(ast, il, Typeob.Object, target_type);
                return;
              case TypeCode.Boolean:
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
                Convert.EmitLdloca(il, source_type);
                il.Emit(OpCodes.Call, CompilerGlobals.dateTimeToInt64Method);
                Convert.Emit(ast, il, Typeob.Int64, target_type, truncationPermitted);
                return;
              case TypeCode.DateTime:
                return;
              case TypeCode.String:
                Convert.EmitLdloca(il, source_type);
                il.Emit(OpCodes.Call, CompilerGlobals.dateTimeToStringMethod);
                return;
            }
            break;

          case TypeCode.String:
            switch (target){
              case TypeCode.Object:
                if (target_type != Typeob.Object && !(target_type is TypeBuilder) && Convert.EmittedCallToConversionMethod(ast, il, source_type, target_type))
                  return;
                Convert.Emit(ast, il, Typeob.Object, target_type);
                return;
              case TypeCode.Boolean:
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
              case TypeCode.DateTime:
                //Resort to calling the Coerce routine. The extra call and the extra box/unbox adds neglible cost to such an expensive conversion.
                if (truncationPermitted && target == TypeCode.Int32){
                  il.Emit(OpCodes.Call, CompilerGlobals.toInt32Method);
                  return;
                }else{
                  ConstantWrapper.TranslateToILInt(il, (int)target);
                  ConstantWrapper.TranslateToILInt(il, truncationPermitted ? 1 : 0);
                  il.Emit(OpCodes.Call, CompilerGlobals.coerce2Method);
                }
                if (target_type.IsValueType) //Should always be true, but may as well check
                  Convert.EmitUnbox(il, target_type, target);
                return;
              case TypeCode.String:
                return;
            }
            break;
        }
        Convert.Emit(ast, il, source_type, Typeob.Object); //The thing on the stack cannot be converted to target_type. Arrange for an error message.
        il.Emit(OpCodes.Call, CompilerGlobals.throwTypeMismatch);
        LocalBuilder tok = il.DeclareLocal(target_type); //Add dummy code to reassure the verifier.
        il.Emit(OpCodes.Ldloc, tok);
      }

      // Emit code sequence used for explicit casts from floating point numbers to integer values.
      // A call is needed because the built in opcodes such as Conv.I4 have unspecified behavior
      // if the value overflows the integer.
      internal static void EmitSingleToIntegerTruncatedConversion(ILGenerator il, OpCode opConversion) {
          il.Emit(OpCodes.Conv_R8);
          EmitDoubleToIntegerTruncatedConversion(il, opConversion);
      }
      internal static void EmitDoubleToIntegerTruncatedConversion(ILGenerator il, OpCode opConversion) {
          il.Emit(OpCodes.Call, CompilerGlobals.doubleToInt64);
          if (!opConversion.Equals(OpCodes.Conv_I8))
              il.Emit(opConversion);
      }
      internal static void EmitDecimalToIntegerTruncatedConversion(ILGenerator il, OpCode opConversion) {
          il.Emit(OpCodes.Call, CompilerGlobals.uncheckedDecimalToInt64Method);
          if (!opConversion.Equals(OpCodes.Conv_I8))
              il.Emit(opConversion);
      }

      internal static void EmitUnbox(ILGenerator il, Type target_type, TypeCode target){
        il.Emit(OpCodes.Unbox, target_type);
        switch(target){
          case TypeCode.Boolean:
          case TypeCode.Byte:
            il.Emit(OpCodes.Ldind_U1); return;
          case TypeCode.Char:
          case TypeCode.UInt16:
            il.Emit(OpCodes.Ldind_U2); return;
          case TypeCode.SByte:
            il.Emit(OpCodes.Ldind_I1); return;
          case TypeCode.Int16:
            il.Emit(OpCodes.Ldind_I2); return;
          case TypeCode.Int32:
            il.Emit(OpCodes.Ldind_I4); return;
          case TypeCode.UInt32:
            il.Emit(OpCodes.Ldind_U4); return;
          case TypeCode.Int64:
          case TypeCode.UInt64:
            il.Emit(OpCodes.Ldind_I8); return;
          case TypeCode.Single:
            il.Emit(OpCodes.Ldind_R4); return;
          case TypeCode.Double:
            il.Emit(OpCodes.Ldind_R8); return;
          default:
            il.Emit(OpCodes.Ldobj, target_type); return;
        }
      }

      private static bool EmittedCallToConversionMethod(AST ast, ILGenerator il, Type source_type, Type target_type){
        //Look for an explicit conversion, call that if there is one
        MethodInfo meth = target_type.GetMethod("op_Explicit", BindingFlags.ExactBinding|BindingFlags.Public|BindingFlags.Static, null, new Type[]{source_type}, null);
        if (meth != null){
          il.Emit(OpCodes.Call, meth);
          Convert.Emit(ast, il, meth.ReturnType, target_type);
          return true;
        }
        meth = Convert.GetToXXXXMethod(source_type, target_type, true);
        if (meth != null){
          il.Emit(OpCodes.Call, meth);
          return true;
        }

        //Look for implicit conversion, call that if there is one
        meth = target_type.GetMethod("op_Implicit", BindingFlags.ExactBinding|BindingFlags.Public|BindingFlags.Static, null, new Type[]{source_type}, null);
        if (meth != null){
          il.Emit(OpCodes.Call, meth);
          Convert.Emit(ast, il, meth.ReturnType, target_type);
          return true;
        }
        meth = Convert.GetToXXXXMethod(source_type, target_type, false);
        if (meth != null){
          il.Emit(OpCodes.Call, meth);
          return true;
        }
        return false;
      }

      internal static void EmitLdarg(ILGenerator il, short argNum){
        switch(argNum){
          case 0 : il.Emit(OpCodes.Ldarg_0); return;
          case 1 : il.Emit(OpCodes.Ldarg_1); return;
          case 2 : il.Emit(OpCodes.Ldarg_2); return;
          case 3 : il.Emit(OpCodes.Ldarg_3); return;
        }
        if (argNum < 256)
          il.Emit(OpCodes.Ldarg_S, (byte)argNum);
        else
          il.Emit(OpCodes.Ldarg, argNum);
      }

      internal static void EmitLdloca(ILGenerator il, Type source_type){
        LocalBuilder tok = il.DeclareLocal(source_type);
        il.Emit(OpCodes.Stloc, tok);
        il.Emit(OpCodes.Ldloca, tok);
      }

      private static IReflect GetArrayElementType(IReflect ir){
        if (ir is TypedArray)
          return ((TypedArray)ir).elementType;
        else if (ir is Type && ((Type)ir).IsArray)
          return ((Type)ir).GetElementType();
        else if (ir is ArrayObject || ir == Typeob.ArrayObject)
          return Typeob.Object;
        else
          return null;
      }

      internal static int GetArrayRank(IReflect ir){
        if (ir == Typeob.ArrayObject || ir is ArrayObject)
          return 1;
        else if (ir is TypedArray)
          return ((TypedArray)ir).rank;
        else if (ir is Type && ((Type)ir).IsArray)
          return ((Type)ir).GetArrayRank();
        else
          return -1;
      }

      internal static IConvertible GetIConvertible(Object ob){
        return ob as IConvertible;
      }

      private static MethodInfo GetToXXXXMethod(IReflect ir, Type desiredType, bool explicitOK){
        if ((ir is TypeBuilder) || (ir is EnumBuilder)) return null;
        MemberInfo[] members = ir.GetMember(explicitOK ? "op_Explicit" : "op_Implicit", BindingFlags.Public|BindingFlags.Static);
        if (members != null)
          foreach(MemberInfo mem in members)
            if (mem is MethodInfo)
              if (((MethodInfo)mem).ReturnType == desiredType)
                return (MethodInfo)mem;
        return null;
      }

      internal static TypeCode GetTypeCode(Object ob, IConvertible ic){
        if (ob == null) return TypeCode.Empty;
        if (ic == null) return TypeCode.Object;
        return ic.GetTypeCode();
      }

      internal static TypeCode GetTypeCode(Object ob){
        return Convert.GetTypeCode(ob, Convert.GetIConvertible(ob));
      }

      internal static Type GetUnderlyingType(Type type){
        if (type is TypeBuilder) return type.UnderlyingSystemType;
        return Enum.GetUnderlyingType(type);
      }

      internal static bool IsArray(IReflect ir){
        return ir == Typeob.Array || ir == Typeob.ArrayObject || ir is TypedArray ||
          ir is ArrayObject || (ir is Type && ((Type)ir).IsArray);
      }

      private static bool IsArrayElementTypeKnown(IReflect ir){
        Debug.PreCondition(IsArray(ir));
        return ir == Typeob.ArrayObject || ir is TypedArray ||
          ir is ArrayObject || (ir is Type && ((Type)ir).IsArray);
      }

      internal static bool IsArrayRankKnown(IReflect ir) {
        Debug.PreCondition(IsArray(ir));
        return ir == Typeob.ArrayObject || ir is TypedArray ||
          ir is ArrayObject || (ir is Type && ((Type)ir).IsArray);
      }

      internal static bool IsArrayType(IReflect ir){
        return ir is TypedArray || ir == Typeob.Array || ir == Typeob.ArrayObject || (ir is Type && ((Type)ir).IsArray);
      }

      internal static bool IsJScriptArray(IReflect ir){
        return ir is ArrayObject || ir == Typeob.ArrayObject;
      }

      internal static bool IsPrimitiveSignedNumericType(Type t){
        switch(Type.GetTypeCode(t)){
          case TypeCode.Single:
          case TypeCode.Double:
          case TypeCode.SByte:
          case TypeCode.Int16:
          case TypeCode.Int32:
          case TypeCode.Int64: return true;
        }
        return false;
      }

      internal static bool IsPrimitiveSignedIntegerType(Type t){
        switch(Type.GetTypeCode(t)){
          case TypeCode.SByte:
          case TypeCode.Int16:
          case TypeCode.Int32:
          case TypeCode.Int64: return true;
        }
        return false;
      }

      internal static bool IsPrimitiveUnsignedIntegerType(Type t){
        switch(Type.GetTypeCode(t)){
          case TypeCode.Byte:
          case TypeCode.UInt16:
          case TypeCode.UInt32:
          case TypeCode.UInt64: return true;
        }
        return false;
      }

      internal static bool IsPrimitiveIntegerType(Type t){
        switch(Type.GetTypeCode(t)){
          case TypeCode.SByte:
          case TypeCode.Byte:
          case TypeCode.Int16:
          case TypeCode.UInt16:
          case TypeCode.Int32:
          case TypeCode.UInt32:
          case TypeCode.Int64:
          case TypeCode.UInt64: return true;
        }
        return false;
      }

      internal static bool IsPrimitiveNumericTypeCode(TypeCode tc){
        switch(tc){
          case TypeCode.SByte:
          case TypeCode.Byte:
          case TypeCode.Int16:
          case TypeCode.UInt16:
          case TypeCode.Int32:
          case TypeCode.UInt32:
          case TypeCode.Int64:
          case TypeCode.UInt64:
          case TypeCode.Single:
          case TypeCode.Double: return true;
        }
        return false;
      }

      internal static bool IsPrimitiveNumericType(IReflect ir){
        Type t = ir as Type;
        if (t == null) return false;
        return Convert.IsPrimitiveNumericTypeCode(Type.GetTypeCode(t));
      }

      internal static bool IsPrimitiveNumericTypeFitForDouble(IReflect ir){
        Type t = ir as Type;
        if (t == null) return false;
        switch (Type.GetTypeCode(t)){
          case TypeCode.SByte:
          case TypeCode.Byte:
          case TypeCode.Int16:
          case TypeCode.UInt16:
          case TypeCode.Int32:
          case TypeCode.UInt32:
          case TypeCode.Single:
          case TypeCode.Double: return true;
        }
        return false;
      }

      /* indicates whether all values of source type can be converted to the target type without loss of information */
      private static bool[,] promotable = new bool[,]
   {             /*target type*/
   /*source       Empty  Object DBNull Boolean Char   SByte  Byte   Int16  UInt16 Int32  UInt32 Int64  UInt64 Single Double Decimal DateTime TimeSpan String*/
   /*Empty*/    { true,  true,  true,  true,   true,  true,  true,  true,  true,  true,  true,  true,  true,  true,  true,  true,   true,    true,    true},
   /*Object*/   { false, false, false, false,  false, false, false, false, false, false, false, false, false, false, false, false,  false,   false,   false},
   /*DBNull*/   { true,  true,  true,  true,   true,  true,  true,  true,  true,  true,  true,  true,  true,  true,  true,  true,   true,    true,    true},
   /*Boolean*/  { false, false, false, true,   true,  true,  true,  true,  true,  true,  true,  true,  true,  true,  true,  true,   true,    true,    false},
   /*Char*/     { false, false, false, false,  true,  false, false, false, true,  true,  true,  true,  true,  true,  true,  true,   true,    true,    false},
   /*SByte*/    { false, false, false, false,  false, true,  false, true,  false, true,  false, true,  false, true,  true,  true,   true,    true,    false},
   /*Byte*/     { false, false, false, false,  true,  false, true,  true,  true,  true,  true,  true,  true,  true,  true,  true,   true,    true,    false},
   /*Int16*/    { false, false, false, false,  false, true,  false, true,  false, true,  false, true,  false, true,  true,  true,   true,    true,    false},
   /*UInt16*/   { false, false, false, false,  true,  false, false, false, true,  true,  true,  true,  true,  true,  true,  true,   true,    true,    false},
   /*Int32*/    { false, false, false, false,  false, false, false, false, false, true,  false, true,  false, false, true,  true,   true,    true,    false},
   /*UInt32*/   { false, false, false, false,  false, false, false, false, false, false, true,  true,  true,  false, true,  true,   true,    true,    false},
   /*Int64*/    { false, false, false, false,  false, false, false, false, false, false, false, true,  false, false, false, true,   true,    true,    false},
   /*UInt64*/   { false, false, false, false,  false, false, false, false, false, false, false, false, true,  false, false, true,   true,    true,    false},
   /*Single*/   { false, false, false, false,  false, false, false, false, false, false, false, false, false, true,  true,  true,   false,   false,   false},
   /*Double*/   { false, false, false, false,  false, false, false, false, false, false, false, false, false, false, true,  true,   false,   false,   false},
   /*Decimal*/  { false, false, false, false,  false, false, false, false, false, false, false, false, false, false, false, true,   false,   false,   false},
   /*DateTime*/ { false, false, false, false,  false, false, false, false, false, false, false, false, false, false, false, false,  true,    false,   false},
   /*TimeSpan*/ { false, false, false, false,  false, false, false, false, false, false, false, false, false, false, false, false,  false,   true,    false},
   /*String*/   { false, false, false, false,  false, false, false, false, false, false, false, false, false, false, false, false,  false,   false,   true}
   };
      // By "promotable" we mean that:
      // * value types may be promoted to other value types if there is no possible loss of data
      //   (Int16 is promotable to Double, but Double is not promotable to Int16.)
      // * value types may be promoted to their wrapper reference types
      //   (A boolean is promotable to a Boolean object.)
      // * reference types may be promoted to reference types they inherit from, but not vice-versa.
      //   (A Derived is promotable to a Base, but a Base is not promotable to a Derived.)

      private static bool IsPromotableTo(Type source_type, Type target_type){
        TypeCode source = Type.GetTypeCode(source_type);
        TypeCode target = Type.GetTypeCode(target_type);
        if (Convert.promotable[(int)source, (int)target])
          return true;
        if ((source == TypeCode.Object || source == TypeCode.String) && target == TypeCode.Object){
          if (target_type.IsAssignableFrom(source_type))
             return true;
          if (target_type == Typeob.BooleanObject && source_type == Typeob.Boolean)
            return true;
          if (target_type == Typeob.StringObject && source_type == Typeob.String)
            return true;
          if (target_type == Typeob.NumberObject && Convert.IsPromotableTo(source_type, Typeob.Double))
            return true;
          if (target_type == Typeob.Array || source_type == Typeob.Array || target_type.IsArray || source_type.IsArray)
            return Convert.IsPromotableToArray(source_type, target_type);
        }
        if (source_type == Typeob.BooleanObject && target_type == Typeob.Boolean)
          return true;
        if (source_type == Typeob.StringObject && target_type == Typeob.String)
          return true;
        if (source_type == Typeob.DateObject && target_type == Typeob.DateTime)
          return true;
        if (source_type == Typeob.NumberObject)
          return Convert.IsPrimitiveNumericType(target_type);
        if (source_type.IsEnum) return !target_type.IsEnum && Convert.IsPromotableTo(Convert.GetUnderlyingType(source_type), target_type);
        if (target_type.IsEnum) return !source_type.IsEnum && Convert.IsPromotableTo(source_type, Convert.GetUnderlyingType(target_type));
        //Look for implicit conversions. 
        MethodInfo meth = target_type.GetMethod("op_Implicit", BindingFlags.ExactBinding|BindingFlags.Public|BindingFlags.Static, null, new Type[]{source_type}, null);
        if (meth != null && (meth.Attributes & MethodAttributes.SpecialName) != 0) return true;
        meth = Convert.GetToXXXXMethod(source_type, target_type, false);
        if (meth != null && (meth.Attributes & MethodAttributes.SpecialName) != 0) return true;
        return false;
      }

      internal static bool IsPromotableTo(IReflect source_ir, IReflect target_ir){
        Type target_type;
        if (source_ir is TypedArray || target_ir is TypedArray ||
            source_ir is ArrayObject || target_ir is ArrayObject ||
            source_ir == Typeob.ArrayObject || target_ir == Typeob.ArrayObject)
          return Convert.IsPromotableToArray(source_ir, target_ir);
        if (target_ir is ClassScope){
          if (((ClassScope)target_ir).owner is EnumDeclaration){
            if (Convert.IsPrimitiveNumericType(source_ir))
              return IsPromotableTo(source_ir, ((EnumDeclaration)((ClassScope)target_ir).owner).baseType.ToType());
            else if (source_ir == Typeob.String) return true;
            else if (source_ir == target_ir) return true;
            return false;
          }
          if (source_ir is ClassScope)
            return (((ClassScope)source_ir).IsSameOrDerivedFrom((ClassScope)target_ir));
          return false; //The source is not in the same compilation unit. Thus it can only extend the target type if there is a circular dependency and separate compilation. Can't handle that.
        }else if (target_ir is Type){
          if (target_ir == Typeob.Object) return !(source_ir is Type) || !((Type)source_ir).IsByRef;
          target_type = (Type)target_ir;
        }else if (target_ir is ScriptFunction)
          target_type = Typeob.ScriptFunction;
        else
          //assert that target_ir is JSObject
          target_type = Globals.TypeRefs.ToReferenceContext(target_ir.GetType());
        if (source_ir is ClassScope)
          return ((ClassScope)source_ir).IsPromotableTo(target_type);
        else
          return IsPromotableTo(source_ir is Type ? (Type)source_ir : Globals.TypeRefs.ToReferenceContext(source_ir.GetType()), target_type);
      }

      // IsPromotableToArray determines whether or not we _know_ at compile time
      // that a given source type may be _promoted_ safely to a given target type.
      // Here we are concerned about two things:
      // (1) Is the target a base class of the source?
      // (2) Is the target a wrapper class of the source?
      //
      // We wish to implement the following semantics:
      //
      // * No non-array is promotable to any array.
      // * An array is only promotable to another array or Object.
      // * Any array (regardless of element type or rank) is promotable to Object.
      // * JScript arrays are not promotable to anything except JS Array and Object.
      // * Every array (other than JScript array) is promotable to System.Array.
      // * System.Array is not promotable to anything -- we do not have enough information
      //   to make a safe promotion.  Note that System Arrays are _assignment compatible_ to
      //   JScript arrays but not _promotable_ to JScript arrays.  We wish to be conservative
      //   here and we do not know if the System.Array is of rank one.
      // * Any rank-one array (regardless of element type) is promotable to a JScript array
      //   via the wrapper class.
      // * A source array is promotable to a typed array only if the source and the target
      //   are known to be both equal in rank and element-type compatible.
      // * By "element type compatible" we mean the following:  Value types are only compatible
      //   with equal value types.  (Int16[] is not promotable to Int32[] even though Int16
      //   is promotable to Int32.)  Value types are never compatible with reference types.
      //   (Int32[] is not promotable to Object[].) Reference types are only compatible if
      //   they are promotable.  (Derived[] is promotable to Base[] but Base[] is not
      //   promotable to Derived[].)

      private static bool IsPromotableToArray(IReflect source_ir, IReflect target_ir){
        Debug.PreCondition(IsArray(source_ir) || IsArray(target_ir));
        if (!IsArray(source_ir))
          return false;
        else if (target_ir == Typeob.Object)
          return true;
        else if (!IsArray(target_ir)){
          if (target_ir is Type){
            Type tt = (Type)target_ir;
            if (tt.IsInterface && tt.IsAssignableFrom(Typeob.Array))
              return source_ir is TypedArray || (source_ir is Type && ((Type)source_ir).IsArray);
          }
          return false;
        }else if (IsJScriptArray(source_ir) && !IsJScriptArray(target_ir))
          return false;
        else if (target_ir == Typeob.Array)
          return !IsJScriptArray(source_ir);
        else if (source_ir == Typeob.Array)
          return false;
        else if (GetArrayRank(source_ir) == 1 && IsJScriptArray(target_ir))
          return true;
        else if (GetArrayRank(source_ir) != GetArrayRank(target_ir))
          return false;

        IReflect source_element_ir = GetArrayElementType(source_ir);
        IReflect target_element_ir = GetArrayElementType(target_ir);

        if (null == source_element_ir || null == target_element_ir)
          return false;

        if ((source_element_ir is Type && ((Type)source_element_ir).IsValueType) ||
            (target_element_ir is Type && ((Type)target_element_ir).IsValueType))
          return source_element_ir == target_element_ir;
        else
          return Convert.IsPromotableTo(source_element_ir, target_element_ir);
      }

      private static bool IsWhiteSpace(char c) {
        switch (c) {
          case (char)0x09:
          case (char)0x0A:
          case (char)0x0B:
          case (char)0x0C:
          case (char)0x0D:
          case (char)0x20:
          case (char)0xA0:
            return true;
          default:
            if (c >= 128)
              return Char.IsWhiteSpace(c);
            else
              return false;
        }
      }

      private static bool IsWhiteSpaceTrailer(char[] s, int i, int max){
        for (; i < max; i++)
          if (!IsWhiteSpace(s[i])) return false;
        return true;
      }

      internal static Object LiteralToNumber(String str){
         return Convert.LiteralToNumber(str, null);
      }

      internal static Object LiteralToNumber(String str, Context context){
        //Called from the parser for integer literals
        uint r = 10;
        if (str[0] == '0' && str.Length > 1)
          if (str[1] == 'x' || str[1] == 'X')
            r = 16;
          else
            r = 8;
        Object result = Convert.parseRadix(str.ToCharArray(), r, r == 16 ? 2 : 0, 1, false);
        if (result != null){
          
          if (r == 8 && context != null && result is Int32 && ((int)result) > 7)
            context.HandleError(JSError.OctalLiteralsAreDeprecated);
          return result;
        }
        context.HandleError(JSError.BadOctalLiteral);
        return Convert.parseRadix(str.ToCharArray(), 10, 0, 1, false);
      }

      internal static bool NeedsWrapper(TypeCode code){
        switch (code){
          case TypeCode.Boolean:
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
          case TypeCode.String: return true;
        }
        return false;
      }

      private static double DoubleParse(String str){
        try{
          return Double.Parse(str, NumberStyles.Float, CultureInfo.InvariantCulture);
        }catch(OverflowException){
          int i = 0; int n = str.Length;
          while (i < n && IsWhiteSpace(str[i])) i++;
          if (i < n && str[i] == '-')
            return Double.NegativeInfinity;
          else
            return Double.PositiveInfinity;
        }
      }

      private static Object parseRadix(char[] s, uint rdx, int i, int sign, bool ignoreTrailers){
        int max = s.Length;
        if (i >= max) return null;
        ulong multmax = (ulong)(UInt64.MaxValue / rdx);
        int digit = RadixDigit(s[i], rdx);
        if (digit < 0) return null;
        ulong result = (ulong)digit;
        int saved_i = i;
        for (;;){
          if (++i == max) goto returnAsInteger;
          digit = RadixDigit(s[i], rdx);
          if (digit < 0)
            if (ignoreTrailers || IsWhiteSpaceTrailer(s, i, max))
              goto returnAsInteger;
            else
              return null;
          if (result > multmax) goto returnAsDouble;
          unchecked{
            ulong r1 = result * rdx;
            ulong r2 = r1 + (ulong)digit;
            if (r1 > r2) goto returnAsDouble;
            result = r2;
          }
        }

        returnAsInteger:
          if (sign < 0){
            if (result <= 2147483648) return (int)(-(long)result);
            if (result < 9223372036854775808) return -(long)result;
            if (result == 9223372036854775808) return -9223372036854775808;
            return -(double)result;
          }
          if (result <= 2147483647) return (int)result;
          if (result <= 9223372036854775807) return (long)result;
          return result;

        returnAsDouble:
          //too long for a ulong. Try double. Be consistent with Double.FromString.
          if (rdx == 10)
            try{
              double r = DoubleParse(new String(s, saved_i, max-saved_i));
              if (r == r) return sign*r;
              if (!ignoreTrailers) return null;
            }catch{}

          //Get here for string with trailers, or for radix other than 10
          //continue accummulating, but with loss of precision. Not quite as good as Double.FromString, but simple.
          double double_result = ((double)result)*rdx + digit;
          for (;;){
            if (++i == max) return sign*double_result;
            digit = RadixDigit(s[i], rdx);
            if (digit < 0)
              if (ignoreTrailers || IsWhiteSpaceTrailer(s, i, max))
                return sign*double_result;
              else
                return null;
            double_result = double_result*rdx + digit;
          }
      }

      private static int RadixDigit(char c, uint r){
        int d;
        if (c >= '0' && c <= '9')
          d = ((int)c) - ((int)'0');
        else if (c >= 'A' && c <= 'Z')
          d = 10 + ((int)c) - ((int)'A');
        else if (c >= 'a' && c <= 'z')
          d = 10 + ((int)c) - ((int)'a');
        else
          return -1;
        if (d >= r)
          return -1;
        else
          return d;
      }

#if !DEBUG
      [DebuggerStepThroughAttribute]
      [DebuggerHiddenAttribute]
#endif
      public static void ThrowTypeMismatch(Object val){
        throw new JScriptException(JSError.TypeMismatch, new Context(new DocumentContext("", null), val.ToString()));
      }

      public static bool ToBoolean(double d){
        return d == d && d != 0;
      }

#if !DEBUG
      [DebuggerStepThroughAttribute]
      [DebuggerHiddenAttribute]
#endif
      public static bool ToBoolean(Object value){
        if (value is Boolean) return (bool)value;
        return Convert.ToBoolean(value, Convert.GetIConvertible(value));
      }

#if !DEBUG
      [DebuggerStepThroughAttribute]
      [DebuggerHiddenAttribute]
#endif
      public static bool ToBoolean(Object value, bool explicitConversion){
        if (value is Boolean) return (bool)value;
        if (!explicitConversion && value is BooleanObject) return ((BooleanObject)value).value;
        return Convert.ToBoolean(value, Convert.GetIConvertible(value));
      }

#if !DEBUG
      [DebuggerStepThroughAttribute]
      [DebuggerHiddenAttribute]
#endif
      internal static bool ToBoolean(Object value, IConvertible ic){
        switch (Convert.GetTypeCode(value, ic)){
          case TypeCode.Empty: return false;
          case TypeCode.Object:
            if (value is Missing || value is System.Reflection.Missing) return false;
            Type t = value.GetType();
            MethodInfo meth = t.GetMethod("op_True", BindingFlags.ExactBinding|BindingFlags.Public|BindingFlags.Static, null, new Type[]{t}, null);
            if (meth != null && (meth.Attributes & MethodAttributes.SpecialName) != 0 && meth.ReturnType == typeof(Boolean)){
              meth = new JSMethodInfo(meth);
              return (bool)meth.Invoke(null, BindingFlags.SuppressChangeType, null, new Object[]{value}, null);
            }
            return true;
          case TypeCode.DBNull: return false;
          case TypeCode.Boolean: return ic.ToBoolean(null);
          case TypeCode.Char: return ic.ToChar(null) != (Char)0;
          case TypeCode.SByte:
          case TypeCode.Byte:
          case TypeCode.Int16:
          case TypeCode.UInt16:
          case TypeCode.Int32: return ic.ToInt32(null) != 0;
          case TypeCode.UInt32:
          case TypeCode.Int64: return ic.ToInt64(null) != 0;
          case TypeCode.UInt64: return ic.ToUInt64(null) != 0;
          case TypeCode.Single:
          case TypeCode.Double:
            double d = ic.ToDouble(null);
            if (d != d) return false; else return d != 0;
          case TypeCode.Decimal: return ic.ToDecimal(null) != (Decimal)0;
          case TypeCode.DateTime: return true;
          case TypeCode.String: return ic.ToString(null).Length != 0;
        }
        return false; //should never get here
      }

      internal static char ToChar(Object value){
        return (char)Convert.ToUint32(value);
      }

      private static char ToDigit(int digit) {
        return digit < 10 ? (char)('0' + digit) : (char)('a' + digit - 10);
      }

      public static Object ToForInObject(Object value, VsaEngine engine){
        if (value is ScriptObject) return value;
        IConvertible ic = Convert.GetIConvertible(value);
        switch (Convert.GetTypeCode(value, ic)){
          case TypeCode.Boolean: return engine.Globals.globalObject.originalBoolean.ConstructImplicitWrapper(ic.ToBoolean(null));
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
          case TypeCode.Decimal: return engine.Globals.globalObject.originalNumber.ConstructImplicitWrapper(value);
          case TypeCode.String: return engine.Globals.globalObject.originalString.ConstructImplicitWrapper(ic.ToString(null));
          case TypeCode.DateTime:  return value;
          case TypeCode.Object:
            return value;
        }
        return engine.Globals.globalObject.originalObject.ConstructObject();
      }

      internal static double ToInteger(double number) {
        if (number != number)
          return 0;
        return Math.Sign(number)*Math.Floor(Math.Abs(number));
      }

      internal static double ToInteger(Object value){
        if (value is Double) return Convert.ToInteger((double)value);
        if (value is Int32) return (double)(int)value;
        return Convert.ToInteger(value, Convert.GetIConvertible(value));
      }

      internal static double ToInteger(Object value, IConvertible ic){
        switch (Convert.GetTypeCode(value, ic)){
          case TypeCode.Empty: return 0;
          case TypeCode.DBNull: return 0;
          case TypeCode.Boolean: return ic.ToBoolean(null) ? 1 : 0;
          case TypeCode.Char: return (double)ic.ToChar(null);
          case TypeCode.SByte:
          case TypeCode.Byte:
          case TypeCode.Int16:
          case TypeCode.UInt16:
          case TypeCode.Int32:
          case TypeCode.UInt32:
          case TypeCode.Int64:
          case TypeCode.UInt64: return ic.ToDouble(null);
          case TypeCode.Single:
          case TypeCode.Double:
          case TypeCode.Decimal: return Convert.ToInteger(ic.ToDouble(null));
          case TypeCode.Object:
          case TypeCode.DateTime:
            Object pval = Convert.ToPrimitive(value, PreferredType.Number, ref ic);
            if (pval != value)
              return Convert.ToInteger(Convert.ToNumber(pval, ic));
            else
              return Double.NaN;
          case TypeCode.String: return Convert.ToInteger(Convert.ToNumber(ic.ToString(null)));
        }
        return 0; //should never get here
      }

      public static int ToInt32(Object value){
        if (value is Double) return (int)Runtime.DoubleToInt64((double)value);
        if (value is Int32) return (int)value;
        return Convert.ToInt32(value, Convert.GetIConvertible(value));
      }

      internal static int ToInt32(Object value, IConvertible ic){
        switch (Convert.GetTypeCode(value, ic)){
          case TypeCode.Empty: return 0;
          case TypeCode.DBNull: return 0;
          case TypeCode.Boolean: return ic.ToBoolean(null) ? 1 : 0;
          case TypeCode.Char: return (int)ic.ToChar(null);
          case TypeCode.SByte:
          case TypeCode.Byte:
          case TypeCode.Int16:
          case TypeCode.UInt16:
          case TypeCode.Int32: return ic.ToInt32(null);
          case TypeCode.UInt32:
          case TypeCode.Int64: return (int)ic.ToInt64(null);
          case TypeCode.UInt64: return (int)ic.ToUInt64(null);
          case TypeCode.Single:
          case TypeCode.Double: return (int)Runtime.DoubleToInt64(ic.ToDouble(null));
          case TypeCode.Decimal: return (int)Runtime.UncheckedDecimalToInt64(ic.ToDecimal(null));
          case TypeCode.Object:
          case TypeCode.DateTime:
            Object pval = Convert.ToPrimitive(value, PreferredType.Number, ref ic);
            if (pval != value)
              return Convert.ToInt32(pval, ic);
            else
              return 0;
          case TypeCode.String: return (int)Runtime.DoubleToInt64(Convert.ToNumber(ic.ToString(null)));
        }
        return 0; //should never get here.
      }

      internal static IReflect ToIReflect(Type t, VsaEngine engine){
        GlobalObject glob = engine.Globals.globalObject;
        Object result = t;
        if (t == Typeob.ArrayObject)
          result = glob.originalArray.Construct();
        else if (t == Typeob.BooleanObject)
          result = glob.originalBoolean.Construct();
        else if (t == Typeob.DateObject)
          result = glob.originalDate.Construct(new Object[0]);
        else if (t == Typeob.EnumeratorObject)
          result = glob.originalEnumerator.Construct(new Object[0]);
        else if (t == Typeob.ErrorObject)
          result = glob.originalError.Construct(new Object[0]);
        else if (t == Typeob.EvalErrorObject)
          result = glob.originalEvalError.Construct(new Object[0]);
        else if (t == Typeob.JSObject)
          result = glob.originalObject.Construct(new Object[0]);
        else if (t == Typeob.NumberObject)
          result = glob.originalNumber.Construct();
        else if (t == Typeob.RangeErrorObject)
          result = glob.originalRangeError.Construct(new Object[0]);
        else if (t == Typeob.ReferenceErrorObject)
          result = glob.originalReferenceError.Construct(new Object[0]);
        else if (t == Typeob.RegExpObject)
          result = glob.originalRegExp.Construct(new Object[0]);
        else if (t == Typeob.ScriptFunction)
          result = FunctionPrototype.ob;
        else if (t == Typeob.StringObject)
          result = glob.originalString.Construct();
        else if (t == Typeob.SyntaxErrorObject)
          result = glob.originalSyntaxError.Construct(new Object[0]);
        else if (t == Typeob.TypeErrorObject)
          result = glob.originalTypeError.Construct(new Object[0]);
        else if (t == Typeob.URIErrorObject)
          result = glob.originalURIError.Construct(new Object[0]);
        else if (t == Typeob.VBArrayObject)
          result = glob.originalVBArray.Construct();
        else if (t == Typeob.ArgumentsObject)
          result = glob.originalObject.Construct(new Object[0]);
        return (IReflect)result;
      }

      public static double ToNumber(Object value){
        if (value is Int32) return (double)(int)value;
        if (value is Double) return (double)value;
        return Convert.ToNumber(value, Convert.GetIConvertible(value));
      }

      internal static double ToNumber(Object value, IConvertible ic){
        switch (Convert.GetTypeCode(value, ic)){
          case TypeCode.Empty:  return Double.NaN;
          case TypeCode.DBNull: return 0;
          case TypeCode.Boolean: return ic.ToBoolean(null) ? 1 : 0;
          case TypeCode.Char: return (double)ic.ToChar(null);
          case TypeCode.SByte:
          case TypeCode.Byte:
          case TypeCode.Int16:
          case TypeCode.UInt16:
          case TypeCode.Int32: return (double)ic.ToInt32(null);
          case TypeCode.UInt32:
          case TypeCode.Int64: return (double)ic.ToInt64(null);
          case TypeCode.UInt64: return (double)ic.ToUInt64(null);
          case TypeCode.Single:
          case TypeCode.Double:
          case TypeCode.Decimal: return ic.ToDouble(null);
          case TypeCode.Object:
          case TypeCode.DateTime:
            Object pval = Convert.ToPrimitive(value, PreferredType.Number, ref ic);
            if (pval != value)
              return Convert.ToNumber(pval, ic);
            else
              return Double.NaN;
          case TypeCode.String: return Convert.ToNumber(ic.ToString(null));
        }
        return 0; //should never get here
      }

      public static double ToNumber(String str){
        return Convert.ToNumber(str, true, false, Missing.Value);
      }

      internal static double ToNumber(String str, bool hexOK, bool octalOK, Object radix){
        //Called from ToNumber, GlobalObject.parseFloat, GlobalObject.parseInt and from the parser for floating point literals
        //ToNumber, parseFloat and parseInt each have their own pecularities when it comes to leading whitespace, trailing characters, signs and hex/octal.
        //For ToNumber: hexOK = true, octalOK = false
        //For parseFloat: hexOK = false, octalOK = false
        //For parseInt: hexOK = true, octalOK = true and radix might be supplied,
        //For floating point literals: hexOK = false, octalOK = false

        if (!octalOK){ //only parseInt will set octalOK. Integers can always be converted without COM+ help
          try{
            double d = DoubleParse(str); //This should work (and be fastest) for the common case
            if (d != 0) return d;
            int i = 0, n = str.Length;
            while (i < n && IsWhiteSpace(str[i])) i++;
            if (i < n && str[i] == '-')
              return (double)(-0.0);
            else
              return (double)0;
          }catch{
            //We might get here if there are trailing characters in the string that should be ignored.
            //parseFloat ignores leading whitespace and any trailing characters which do not form part of a StrDecimalLiteral
            //ToNumber ignores leading and trailing whitespace, but not other trailing characters
            //We should never get here if called from the parser for a floating point literal (COM+ should always work for those).
            int n = str.Length;
            int j = n-1;
            int i = 0;
            //skip leading whitespace
            while (i < n && IsWhiteSpace(str[i])) i++;
            if (hexOK){ //The ToNumber case.
              //Double.FromString might have failed because of trailing whitespace, or because this is a hexadecimal literal.
              while (j >= i && IsWhiteSpace(str[j])) j--; //Strip any trailing whitespace
              if (i > j) //String empty or all whitespace
                return (double)0;
              if (j < n-1) //Had some trailing whitespace, try COM+ again
                return Convert.ToNumber(str.Substring(i, j-i+1), hexOK, octalOK, radix);
              //otherwise drop through and try to parse the number as a hex literal
            }else{ //The parseFloat case: all trailers are ignored. Prefix starting at i must match StrDecimalLiteral
              //Might be "Infinity"
              if (n-i >= 8 && String.CompareOrdinal(str, i, "Infinity", 0, 8) == 0)
                return Double.PositiveInfinity;
              else if (n-i >= 9 && String.CompareOrdinal(str, i, "-Infinity", 0, 8) == 0)
                return Double.NegativeInfinity;
              else if (n-i >= 9 && String.CompareOrdinal(str, i, "+Infinity", 0, 8) == 0)
                return Double.PositiveInfinity;
              //Reduce string length until last character is a decimal digit or '.'
              char ch;
              //If the last character is a digit there could be noise further upstream. First skip over trailing digits
              while (j >= i){
                ch = str[j];
                if (!JSScanner.IsDigit(ch)) break;
                j--;
              }
              while (j >= i){
                ch = str[j];
                if (JSScanner.IsDigit(ch)) break;
                j--;
              }
              if (j < n-1) //Had some trailing noise, try COM+ again
                return Convert.ToNumber(str.Substring(i, j-i+1), hexOK, octalOK, radix);
              //Whatever is left in str does not parse to a double, give up and return NaN.
              return Double.NaN;
            }
          }
        }
        //if we get here, we are dealing with an integer, or we might be dealing with (+|-)Infinity
        //Never get here for parseFloat or floating point literals from the parser.
        //For parseInt Only the prefix matters. Trailing characters (those that can not form part of an integer literal) are ignored.
        //For ToNumber, any trailing characters must be whitespace.
        {
        int n = str.Length;
        int i = 0;
        //skip leading whitespace
        while (i < n && IsWhiteSpace(str[i])) i++;
        if (i >= n)
          if (hexOK && octalOK)
            return double.NaN; //parseInt case
          else
            return (double)0; //ToNumber case
        int sign = 1;
        bool explicitSign = false;
        if (str[i] == '-'){
          sign = -1; i++; explicitSign = true;
        }else if (str[i] == '+'){
          i++; explicitSign = true;
        }
        while (i < n && IsWhiteSpace(str[i])) i++;
        bool radixMissing = radix == null || radix is Missing;
        if (i+8 <= n && radixMissing && !octalOK && str.Substring(i, 8).Equals("Infinity"))
          return sign > 0 ? Double.PositiveInfinity : Double.NegativeInfinity;
        int r = 10;
        if (!radixMissing)
          r = Convert.ToInt32(radix);
        if (r == 0){
          radixMissing = true;
          r = 10;
        }else if (r < 2 || r > 36)
          return Double.NaN;
        if (i < n-2 && str[i] == '0')
          if (str[i+1] == 'x' || str[i+1] == 'X'){
            if (!hexOK)
              return 0;
            if (explicitSign && !octalOK) //ToNumber case
              return Double.NaN;
            if (radixMissing){
              r = 16; i += 2;
            }else if (r == 16)
              i += 2;
          }else if (octalOK && radixMissing)
            r = 8;
        if (i < n)
          return Convert.ToNumber(Convert.parseRadix(str.ToCharArray(), (uint)r, i, sign, hexOK && octalOK));
        else
          return Double.NaN;
        }
      }

      internal static String ToLocaleString(Object value){
        return Convert.ToString(value, PreferredType.LocaleString, true);
      }

      public static Object ToNativeArray(Object value, RuntimeTypeHandle handle){
        if (value is ArrayObject){
          Type elementType = Type.GetTypeFromHandle(handle);
          return ((ArrayObject)value).ToNativeArray(elementType);
        }
        return value;
      }

      public static Object ToObject(Object value, VsaEngine engine){
        if (value is ScriptObject) return value;
        String str = value as String;
        if (str != null) return engine.Globals.globalObject.originalString.ConstructImplicitWrapper(str);
        IConvertible ic = Convert.GetIConvertible(value);
        switch (Convert.GetTypeCode(value, ic)){
          case TypeCode.Boolean: return engine.Globals.globalObject.originalBoolean.ConstructImplicitWrapper(ic.ToBoolean(null));
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
          case TypeCode.Decimal: return engine.Globals.globalObject.originalNumber.ConstructImplicitWrapper(value);
          case TypeCode.String: return engine.Globals.globalObject.originalString.ConstructImplicitWrapper(ic.ToString(null));
          case TypeCode.DateTime: return ic.ToDateTime(null);
          case TypeCode.Object:
            if (value is Array)
              return engine.Globals.globalObject.originalArray.ConstructImplicitWrapper((Array)value);
            else
              return value;
        }
        throw new JScriptException(JSError.NeedObject);
      }

      public static Object ToObject2(Object value, VsaEngine engine){
        if (value is ScriptObject) return value;
        IConvertible ic = Convert.GetIConvertible(value);
        switch (Convert.GetTypeCode(value, ic)){
          case TypeCode.Boolean: return engine.Globals.globalObject.originalBoolean.ConstructImplicitWrapper(ic.ToBoolean(null));
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
          case TypeCode.Decimal: return engine.Globals.globalObject.originalNumber.ConstructImplicitWrapper(value);
          case TypeCode.String: return engine.Globals.globalObject.originalString.ConstructImplicitWrapper(ic.ToString(null));
          case TypeCode.DateTime: return ic.ToDateTime(null);
          case TypeCode.Object:
            if (value is Array)
              return engine.Globals.globalObject.originalArray.ConstructImplicitWrapper((Array)value);
            else
              return value;
        }
        return null;
      }

      internal static Object ToObject3(Object value, VsaEngine engine){
        if (value is ScriptObject) return value;
        IConvertible ic = Convert.GetIConvertible(value);
        switch (Convert.GetTypeCode(value, ic)){
          case TypeCode.Boolean: return engine.Globals.globalObject.originalBoolean.ConstructWrapper(ic.ToBoolean(null));
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
          case TypeCode.Decimal: return engine.Globals.globalObject.originalNumber.ConstructWrapper(value);
          case TypeCode.String: return engine.Globals.globalObject.originalString.ConstructWrapper(ic.ToString(null));
          case TypeCode.DateTime: return ic.ToDateTime(null);
          case TypeCode.Object:
            if (value is Array)
              return engine.Globals.globalObject.originalArray.ConstructWrapper((Array)value);
            else
              return value;
        }
        return null;
      }

#if !DEBUG
      [DebuggerStepThroughAttribute]
      [DebuggerHiddenAttribute]
#endif
      internal static Object ToPrimitive(Object value, PreferredType preferredType){
        IConvertible ic = Convert.GetIConvertible(value);
        TypeCode tcode = Convert.GetTypeCode(value, ic);
        return Convert.ToPrimitive(value, preferredType, ic, tcode);
      }

#if !DEBUG
      [DebuggerStepThroughAttribute]
      [DebuggerHiddenAttribute]
#endif
      internal static Object ToPrimitive(Object value, PreferredType preferredType, ref IConvertible ic){
        TypeCode tcode = Convert.GetTypeCode(value, ic);
        switch (tcode){
          case TypeCode.Object:
          case TypeCode.DateTime:
            Object result = Convert.ToPrimitive(value, preferredType, ic, tcode);
            if (result != value){
              value = result;
              ic = Convert.GetIConvertible(value);
            }
            break;
        }
        return value;
      }


#if !DEBUG
      [DebuggerStepThroughAttribute]
      [DebuggerHiddenAttribute]
#endif
      private static Object ToPrimitive(Object value, PreferredType preferredType, IConvertible ic, TypeCode tcode){
        switch (tcode){
          case TypeCode.Object:
            System.Array arr = value as System.Array;
            if (arr != null && arr.Rank == 1) value = new ArrayWrapper(ArrayPrototype.ob, arr, true);
            if (value is ScriptObject){
              Object result = ((ScriptObject)value).GetDefaultValue(preferredType);
              if (Convert.GetTypeCode(result) != TypeCode.Object)
                return result;
              else if (value == result && preferredType == PreferredType.String || preferredType == PreferredType.LocaleString){
                if (value is JSObject){
                  ScriptObject proto = ((JSObject)value).GetParent();
                  if (proto is ClassScope)
                    return ((ClassScope)proto).GetFullName();
                  return "[object Object]";
                }
                return value.ToString();
              }else
                throw new JScriptException(JSError.TypeMismatch);
            }else if (value is Missing || value is System.Reflection.Missing)
              return null;
            else{
              IReflect ir;
              if (value is IReflect && !(value is Type))
                ir = (IReflect)value;
              else
                ir = value.GetType();

              //Look for an op_Explicit conversion to String or Double (this always fails for IDispatch/Ex objects
              MethodInfo meth = null;
              if (preferredType == PreferredType.String || preferredType == PreferredType.LocaleString)
                meth = Convert.GetToXXXXMethod(ir, typeof(String), true);
              else{
                meth = Convert.GetToXXXXMethod(ir, typeof(Double), true);
                if (meth == null)
                  meth = Convert.GetToXXXXMethod(ir, typeof(Int64), true);
                if (meth == null)
                  meth = Convert.GetToXXXXMethod(ir, typeof(UInt64), true);
              }
              if (meth != null){
                meth = new JSMethodInfo(meth);
                return meth.Invoke(null, BindingFlags.SuppressChangeType, null, new Object[]{value}, null);
              }

              //Invoke the default method/property or get the value of the default field. If an exception is thrown
              //because the target doesn't have a non-paramterized default member, mask it and execute the
              //default handling.
              try{
                try{
                  MemberInfo member = LateBinding.SelectMember(JSBinder.GetDefaultMembers(Runtime.TypeRefs, ir));
                  if (member != null){
                    switch(member.MemberType){
                      case MemberTypes.Field: return ((FieldInfo)member).GetValue(value);
                      case MemberTypes.Method: return ((MethodInfo)member).Invoke(value, new Object[0]);
                      case MemberTypes.Property: return JSProperty.GetValue((PropertyInfo)member, value, null);
                      case MemberTypes.Event: return null;
                      case MemberTypes.NestedType: return member;
                    }
                  }

                  return ir.InvokeMember(String.Empty, BindingFlags.ExactBinding|BindingFlags.SuppressChangeType|
                                                       BindingFlags.InvokeMethod|BindingFlags.GetProperty|BindingFlags.GetField,
                                         null, value, new Object[0], null, null, new String[0]);
                }catch(TargetInvocationException e){
                  throw e.InnerException;
                }
              }catch(ArgumentException){
              }catch(IndexOutOfRangeException){
              }catch(MissingMemberException){
              }catch(SecurityException){
              }catch(TargetParameterCountException){
              }

              if (preferredType != PreferredType.Number)
                if (value is Char[])
                  return new System.String((Char[])value);
                else
                  return value.ToString();
              return value;
            }
          case TypeCode.DateTime:
            return DateConstructor.ob.Construct(ic.ToDateTime(null)).GetDefaultValue(preferredType);
        }
        return value;
      }

      internal static String ToString(Object value){
        return Convert.ToString(value, PreferredType.String, true);
      }

#if !DEBUG
      [DebuggerStepThroughAttribute]
      [DebuggerHiddenAttribute]
#endif
      public static String ToString(Object value, bool explicitOK){
        return Convert.ToString(value, PreferredType.String, explicitOK);
      }

#if !DEBUG
      [DebuggerStepThroughAttribute]
      [DebuggerHiddenAttribute]
#endif
      internal static String ToString(Object value, IConvertible ic){
        return Convert.ToString(value, PreferredType.String, ic, true);
      }

#if !DEBUG
      [DebuggerStepThroughAttribute]
      [DebuggerHiddenAttribute]
#endif
      internal static String ToString(Object value, PreferredType pref, bool explicitOK){
        String str = value as String;
        if (str != null) return str;
        StringObject strObj = value as StringObject;
        if (strObj != null && strObj.noExpando) return strObj.value;
        return Convert.ToString(value, pref, Convert.GetIConvertible(value), explicitOK);
      }

#if !DEBUG
      [DebuggerStepThroughAttribute]
      [DebuggerHiddenAttribute]
#endif
      internal static String ToString(Object value, PreferredType pref, IConvertible ic, bool explicitOK){
        Enum e = value as Enum;
        if (e != null) return e.ToString("G");
        EnumWrapper ew = value as EnumWrapper;
        if (ew != null) return ew.ToString();
        TypeCode code = Convert.GetTypeCode(value, ic);
        if (pref == PreferredType.LocaleString){
          switch (code){
            case TypeCode.SByte:
            case TypeCode.Byte:
            case TypeCode.Int16:
            case TypeCode.UInt16:
            case TypeCode.Int32:
            case TypeCode.UInt32:
            case TypeCode.Single:
            case TypeCode.Double: {
              double d = ic.ToDouble(null);
              return d.ToString(d <= -1e+15 || d >= 1e+15 ? "g" : "n", NumberFormatInfo.CurrentInfo);
            }
            case TypeCode.Int64: return ic.ToInt64(null).ToString("n", NumberFormatInfo.CurrentInfo);
            case TypeCode.UInt64: return ic.ToUInt64(null).ToString("n", NumberFormatInfo.CurrentInfo);
            case TypeCode.Decimal: return ic.ToDecimal(null).ToString("n", NumberFormatInfo.CurrentInfo);
          }
        }
        switch (code){
          case TypeCode.Empty: return explicitOK ? "undefined" : null;
          case TypeCode.Object: return Convert.ToString(Convert.ToPrimitive(value, pref, ref ic), ic);
          case TypeCode.DBNull: return explicitOK ? "null" : null;
          case TypeCode.Boolean: return ic.ToBoolean(null) ? "true" : "false";
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
          case TypeCode.String: return ic.ToString(null);
          case TypeCode.DateTime: return Convert.ToString(DateConstructor.ob.Construct(ic.ToDateTime(null)));
          case TypeCode.Single:
          case TypeCode.Double: return Convert.ToString(ic.ToDouble(null));
        }
        return null; //Should never get here
      }

      public static String ToString(bool b){
        return b ? "true" : "false";
      }

      public static String ToString(double d){
        long i = (long)d;
        if ((double)i == d){
          return i.ToString(CultureInfo.InvariantCulture);
        }else if (d != d)
          return "NaN";
        else if (Double.IsPositiveInfinity(d))
          return "Infinity";
        else if (Double.IsNegativeInfinity(d))
          return "-Infinity";
        else{
          double e = d < 0 ? -d : d;
          int k = 15;
          String result = e.ToString("e14", CultureInfo.InvariantCulture);
          if (DoubleParse(result) != e){
            result = e.ToString("e15", CultureInfo.InvariantCulture);
            k = 16;
            if (DoubleParse(result) != e){
              result = e.ToString("e16", CultureInfo.InvariantCulture);
              k = 17;
              if (DoubleParse(result) != e){
                result = e.ToString("e17", CultureInfo.InvariantCulture);
                k = 18;
              }
            }
          }
          int exp = Int32.Parse(result.Substring(k+2, result.Length-(k+2)), CultureInfo.InvariantCulture);
          while (result[k] == '0') k--; //at the end of the loop, k == the number of significant digits
          int n = exp + 1;
          if (k <= n && n <= 21){
            StringBuilder r = new StringBuilder(n+1);
            if (d < 0)
              r.Append('-');
            r.Append(result[0]);
            if (k > 1)
              r.Append(result, 2, k-1);
            if (exp-k >= 0)
              r.Append('0', n-k);
            return r.ToString();
          }
          if (0 < n && n <= 21){
            StringBuilder r = new StringBuilder(k+2);
            if (d < 0)
              r.Append('-');
            r.Append(result[0]);
            if (n > 1)
              r.Append(result, 2, n-1);
            r.Append('.');
            r.Append(result, n+1, k-n);
            return r.ToString();
          }
          if (-6 < n && n <= 0){
            StringBuilder r = new StringBuilder(2-n);
            if (d < 0)
              r.Append("-0.");
            else
              r.Append("0.");
            if (n < 0)
              r.Append('0', -n);
            r.Append(result[0]);
            r.Append(result, 2, k-1);
            return r.ToString();
          }
          {
          StringBuilder r = new StringBuilder(28);
          if (d < 0)
            r.Append('-');
          r.Append(result.Substring(0, k == 1 ? 1 : k+1));
          r.Append('e');
          if (exp >= 0)
            r.Append('+');
          r.Append(exp);
          return r.ToString();
          }
        }
      }

      private static int[] rgcchSig = new int[] {
        53, 34, 27, 24, 22, 20, 19, 18, 17, 17, 16, 16,
        15, 15, 14, 14, 14, 14, 14, 13, 13, 13, 13, 13,
        13, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12 };

      internal static String ToString(Object value, int radix) {
        if (radix == 10 || radix < 2 || radix > 36)
          return Convert.ToString(value);
        double dbl = Convert.ToNumber(value);
        if (dbl == 0.0)
          return "0";
        if (Double.IsNaN(dbl))
          return "NaN";
        if (Double.IsPositiveInfinity(dbl))
          return "Infinity";
        if (Double.IsNegativeInfinity(dbl))
          return "-Infinity";

        StringBuilder sb = new StringBuilder();
        if (dbl < 0.0) {
          sb.Append('-');
          dbl = -dbl;
        }
        int cchMax = rgcchSig[radix - 2];

        if (dbl < 8.67361737988403547206e-19 || dbl >= 2305843009213693952.0) {
          // Exponential notation

          int wExp = (int)Math.Log(dbl, radix) + 1;
          double dblT = Math.Pow(radix, wExp);
          if (Double.IsPositiveInfinity(dblT))
            dblT = Math.Pow(radix, --wExp);
          else if (dblT == 0.0)
            dblT = Math.Pow(radix, ++wExp);
          dbl /= dblT;
          while (dbl < 1.0) {
            dbl *= radix;
            wExp--;
          }

          int wDig = (int)dbl;
          sb.Append(Convert.ToDigit(wDig));
          cchMax--;
          dbl -= wDig;

          if (dbl != 0.0) {
            sb.Append('.');
            while (dbl != 0.0 && cchMax-- > 0) {
              dbl *= radix;
              wDig = (int)dbl;
              if (wDig >= radix)
                wDig = radix - 1;
              sb.Append(Convert.ToDigit(wDig));
              dbl -= wDig;
            }
          }

          sb.Append((wExp >= 0 ? "(e+" : "(e"));
          sb.Append(wExp.ToString(CultureInfo.InvariantCulture));
          sb.Append(')');

        } else {
          // Regular notation

          int wDig, cchSig;
          if (dbl >= 1.0) {
            // Integral portion

            double dblDen, dblT;
            cchSig = 1;
            for (dblDen = 1.0; (dblT = dblDen * radix) <= dbl; dblDen = dblT)
              cchSig++;

            for (int cch = 0; cch < cchSig; cch++) {
              wDig = (int)(dbl / dblDen);
              if (wDig >= radix)
                wDig = radix - 1;
              sb.Append(Convert.ToDigit(wDig));
              dbl -= wDig * dblDen;
              dblDen /= radix;
            }
          } else {
            sb.Append('0');
            cchSig = 0;
          }

          if (dbl != 0.0 && cchSig < cchMax) {
            // Fractional portion

            sb.Append('.');
            while (dbl != 0.0 && cchSig < cchMax) {
              dbl *= radix;
              wDig = (int)dbl;
              if (wDig >= radix)
                wDig = radix - 1;
              sb.Append(Convert.ToDigit(wDig));
              dbl -= wDig;
              if (wDig != 0 || cchSig != 0)
                cchSig++;
            }
          }
        }

        return sb.ToString();
      }

      internal static Type ToType(IReflect ir){
        return Convert.ToType(Globals.TypeRefs, ir);
      }
      
      internal static Type ToType(TypeReferences typeRefs, IReflect ir){
        if (ir is Type)
          return (Type)ir;
        if (ir is ClassScope)
          return ((ClassScope)ir).GetTypeBuilderOrEnumBuilder();
        if (ir is TypedArray){
          return typeRefs.ToReferenceContext(((TypedArray)ir).ToType());
        }
        if (ir is ScriptFunction)
          return typeRefs.ScriptFunction;
        return typeRefs.ToReferenceContext(ir.GetType());
      }

      internal static Type ToType(String descriptor, Type elementType){
        Module mod = elementType.Module;
        if (mod is ModuleBuilder)
          return mod.GetType(elementType.FullName+descriptor);
        else
          return mod.Assembly.GetType(elementType.FullName+descriptor);
      }

      internal static String ToTypeName(IReflect ir){
        if (ir is ClassScope)
          return ((ClassScope)ir).GetName();
        else if (ir is JSObject)
          return ((JSObject)ir).GetClassName();
        else if (ir is GlobalScope)
          return "Global Object";
        else{
          Debug.Assert(ir is Type || ir is TypedArray);
          return ir.ToString();
        }
      }

      internal static uint ToUint32(Object value){
        if (value is UInt32) return (uint)value;
        return Convert.ToUint32(value, Convert.GetIConvertible(value));
      }

      internal static uint ToUint32(Object value, IConvertible ic){
        switch (Convert.GetTypeCode(value, ic)){
          case TypeCode.Empty: return 0;
          case TypeCode.DBNull: return 0;
          case TypeCode.Boolean: return ic.ToBoolean(null) ? (uint)1 : (uint)0;
          case TypeCode.Char: return (uint)ic.ToChar(null);
          case TypeCode.Byte:
          case TypeCode.UInt16:
          case TypeCode.UInt32: return ic.ToUInt32(null);
          case TypeCode.UInt64: return (uint)ic.ToUInt64(null);
          case TypeCode.SByte:
          case TypeCode.Int16:
          case TypeCode.Int32:
          case TypeCode.Int64: return (uint)ic.ToInt64(null);
          case TypeCode.Single:
          case TypeCode.Double: return (uint)Runtime.DoubleToInt64(ic.ToDouble(null));
          case TypeCode.Decimal: return (uint)Runtime.UncheckedDecimalToInt64(ic.ToDecimal(null));
          case TypeCode.Object:
          case TypeCode.DateTime:
            Object pval = Convert.ToPrimitive(value, PreferredType.Number, ref ic);
            if (pval != value)
              return Convert.ToUint32(pval, ic);
            else
              return 0;
          case TypeCode.String: return (uint)Runtime.DoubleToInt64(Convert.ToNumber(ic.ToString(null)));
        }
        return 0; //should never get here
      }

    }
}
