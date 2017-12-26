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
    using System.Reflection;
    using System.Reflection.Emit;

    internal abstract class EnumWrapper : IConvertible{
      internal abstract Object value {
        get;
      }
      
      internal abstract Type type {
        get;
      }
      
      internal abstract String name {
        get;
      }
      
      internal virtual IReflect classScopeOrType {
        get {
          return this.type;
        }
      }
      
      TypeCode IConvertible.GetTypeCode(){
        return Convert.GetTypeCode(this.value);
      }
      
      bool IConvertible.ToBoolean(IFormatProvider provider){
        return ((IConvertible)this.value).ToBoolean(provider);
      }
      
      char IConvertible.ToChar(IFormatProvider provider){
        return ((IConvertible)this.value).ToChar(provider);
      }
      
      sbyte IConvertible.ToSByte(IFormatProvider provider){
        return ((IConvertible)this.value).ToSByte(provider);
      }
      
      byte IConvertible.ToByte(IFormatProvider provider){
        return ((IConvertible)this.value).ToByte(provider);
      }
      
      short IConvertible.ToInt16(IFormatProvider provider){
        return ((IConvertible)this.value).ToInt16(provider);
      }
      
      ushort IConvertible.ToUInt16(IFormatProvider provider){
        return ((IConvertible)this.value).ToUInt16(provider);
      }
      
      int IConvertible.ToInt32(IFormatProvider provider){
        return ((IConvertible)this.value).ToInt32(provider);
      }
      
      uint IConvertible.ToUInt32(IFormatProvider provider){
        return ((IConvertible)this.value).ToUInt32(provider);
      }
      
      long IConvertible.ToInt64(IFormatProvider provider){
        return ((IConvertible)this.value).ToInt64(provider);
      }
      
      ulong IConvertible.ToUInt64(IFormatProvider provider){
        return ((IConvertible)this.value).ToUInt64(provider);
      }
      
      float IConvertible.ToSingle(IFormatProvider provider){
        return ((IConvertible)this.value).ToSingle(provider);
      }
      
      double IConvertible.ToDouble(IFormatProvider provider){
        return ((IConvertible)this.value).ToDouble(provider);
      }
      
      Decimal IConvertible.ToDecimal(IFormatProvider provider){
        return ((IConvertible)this.value).ToDecimal(provider);
      }
      
      DateTime IConvertible.ToDateTime(IFormatProvider provider){
        return ((IConvertible)this.value).ToDateTime(provider);
      }
      
      String IConvertible.ToString(IFormatProvider provider){
        return ((IConvertible)this.value).ToString(provider);
      }
      
      Object IConvertible.ToType(Type conversionType, IFormatProvider provider){
        return ((IConvertible)this.value).ToType(conversionType, provider);
      }
      
      internal Object ToNumericValue(){
        return this.value;
      }
      
      public override String ToString(){
        if (this.name != null) return this.name;
        FieldInfo[] fields = this.type.GetFields(BindingFlags.Static|BindingFlags.Public);
        foreach (FieldInfo field in fields)
          if (StrictEquality.JScriptStrictEquals(this.value, field.GetValue(null))) return field.Name;
        return Convert.ToString(this.value);
      }
    }
    
    
    internal sealed class DeclaredEnumValue : EnumWrapper {
      private String _name;
      internal ClassScope _classScope;
      internal Object _value;
    
      internal DeclaredEnumValue(Object value, String name, ClassScope classScope){
        this._name = name;
        this._classScope = classScope;
        this._value = value;
      }
      
      internal override Object value {
        get {
          return this._value;
        }
      }
      
      internal override Type type {
        get {
          return this._classScope.GetTypeBuilderOrEnumBuilder();
        }
      }
      
      internal override String name {
        get {
          return this._name;
        }
      }
      
      internal override IReflect classScopeOrType {
        get {
          return this._classScope;
        }
      }
      
      internal void CoerceToBaseType(Type bt, Context errCtx){
        Object val = 0;
        AST pval = ((AST)this.value).PartiallyEvaluate();
        if (pval is ConstantWrapper)
          val = ((ConstantWrapper)pval).Evaluate();
        else
          pval.context.HandleError(JSError.NotConst);
        try{
          this._value = Convert.CoerceT(val, bt);
        }catch{
          errCtx.HandleError(JSError.TypeMismatch);
          this._value = Convert.CoerceT(0, bt);
        }
      }
      
    }
    
    internal sealed class MetadataEnumValue : EnumWrapper {
      private Type _type;
      private Object _value;
      
      internal static Object GetEnumValue(Type type, Object value) {
        Debug.Assert(type.IsEnum);
        if (!type.Assembly.ReflectionOnly)
          return Enum.ToObject(type, value);
        return new MetadataEnumValue(type, value);
      }
      
      private MetadataEnumValue(Type type, Object value) {
        this._type = type;
        this._value = value;
      }
      
      internal override Object value {
        get {
          return this._value;
        }
      }
      
      internal override Type type {
        get {
          return this._type;
        }
      }
      
      internal override String name {
        get {
          String name = Enum.GetName(this._type, this._value);
          if (name == null)
            name = this._value.ToString();
          return name;
        }
      }
    }
 }

