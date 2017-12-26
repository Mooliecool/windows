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
    using System.Globalization;
    using System.Text;

    public class NumberPrototype : NumberObject{
      internal static readonly NumberPrototype ob = new NumberPrototype(ObjectPrototype.ob);
      internal static NumberConstructor _constructor;

      internal NumberPrototype(ObjectPrototype parent)
        : base(parent, 0.0) {
        this.noExpando = true;
        //this.constructor is given a value by the proper constructor class
      }

      public static NumberConstructor constructor{
        get{
          return NumberPrototype._constructor;
        }
      }

      private static double ThisobToDouble(Object thisob){
        thisob = NumberPrototype.valueOf(thisob);
        return ((IConvertible)thisob).ToDouble(null);
      }
      
      [JSFunctionAttribute(JSFunctionAttributeEnum.HasThisObject, JSBuiltin.Number_toExponential)]
      public static String toExponential(Object thisob, Object fractionDigits){
        double value = NumberPrototype.ThisobToDouble(thisob);
        double f;
        if (fractionDigits == null || fractionDigits is Missing)
          f = 16;
        else
          f = Convert.ToInteger(fractionDigits);
        if (f < 0 || f > 20)
          throw new JScriptException(JSError.FractionOutOfRange);
        StringBuilder fmt = new StringBuilder("#.");
        for (int i = 0; i < f; i++) fmt.Append('0');
        fmt.Append("e+0");
        return value.ToString(fmt.ToString(), CultureInfo.InvariantCulture);
      }

      [JSFunctionAttribute(JSFunctionAttributeEnum.HasThisObject, JSBuiltin.Number_toFixed)]
      public static String toFixed(Object thisob, double fractionDigits){
        double value = NumberPrototype.ThisobToDouble(thisob);
        if (Double.IsNaN(fractionDigits))
            fractionDigits = 0;
        else if (fractionDigits < 0 || fractionDigits > 20)
          throw new JScriptException(JSError.FractionOutOfRange);
        return value.ToString("f"+((int)fractionDigits).ToString(CultureInfo.InvariantCulture), CultureInfo.InvariantCulture);
      }

      [JSFunctionAttribute(JSFunctionAttributeEnum.HasThisObject, JSBuiltin.Number_toLocaleString)]
      public static String toLocaleString(Object thisob){
        return Convert.ToString(NumberPrototype.valueOf(thisob), PreferredType.LocaleString, true);
      }

      [JSFunctionAttribute(JSFunctionAttributeEnum.HasThisObject, JSBuiltin.Number_toPrecision)]
      public static String toPrecision(Object thisob, Object precision) {
        double value = NumberPrototype.ThisobToDouble(thisob);
        if (precision == null || precision is Missing)
          return Convert.ToString(value);
        double df = Convert.ToInteger(precision);
        if (df < 1 || df > 21)
          throw new JScriptException(JSError.PrecisionOutOfRange);
        int f = (int)df;
        if (Double.IsNaN(value))
          return "NaN";
        if (Double.IsInfinity(value))
          return value > 0.0 ? "Infinity" : "-Infinity";
        String sign;
        if (value >= 0.0)
          sign = "";
        else {
          sign = "-";
          value = -value;
        }
        String mant = value.ToString("e" + (f-1).ToString(CultureInfo.InvariantCulture), CultureInfo.InvariantCulture);
        int exp = Int32.Parse(mant.Substring(mant.Length - 4), CultureInfo.InvariantCulture);
        mant = mant.Substring(0, 1) + mant.Substring(2, f-1);
        if (exp >= f || exp < -6)
          return sign + mant.Substring(0, 1) + (f > 1 ? "." + mant.Substring(1) : "")
            + (exp >= 0 ? "e+" : "e") + exp.ToString(CultureInfo.InvariantCulture);
        if (exp == f-1)
          return sign + mant;
        if (exp >= 0)
          return sign + mant.Substring(0, exp+1) + "." + mant.Substring(exp+1);
        return sign + "0." + mant.PadLeft(f-exp-1, '0');
      }

      [JSFunctionAttribute(JSFunctionAttributeEnum.HasThisObject, JSBuiltin.Number_toString)]
      public static String toString(Object thisob, Object radix){
        int rdx = 10;
        if (radix is IConvertible){
          double rd = ((IConvertible)radix).ToDouble(CultureInfo.InvariantCulture);
          int ri = (int)rd;
          if (rd == (double)ri)
            rdx = ri;
        }
        if (rdx < 2 || rdx > 36)
          rdx = 10;
        return Convert.ToString(NumberPrototype.valueOf(thisob), rdx);
      }

      [JSFunctionAttribute(JSFunctionAttributeEnum.HasThisObject, JSBuiltin.Number_valueOf)]
      public static Object valueOf(Object thisob){
        if (thisob is NumberObject)
          return ((NumberObject)thisob).value;
        switch(Convert.GetTypeCode(thisob)){
          case TypeCode.SByte:
          case TypeCode.Byte:
          case TypeCode.Int16:
          case TypeCode.UInt16:
          case TypeCode.Int32:
          case TypeCode.UInt32:
          case TypeCode.Int64: 
          case TypeCode.UInt64:
          case TypeCode.Single:
          case TypeCode.Double: return thisob;
        }
        throw new JScriptException(JSError.NumberExpected);
      }

    }
}
