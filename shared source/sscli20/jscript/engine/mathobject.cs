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

    public class MathObject : JSObject{
      public const double E = 2.7182818284590452354;
      public const double LN10 = 2.302585092994046;
      public const double LN2 = 0.6931471805599453;
      public const double LOG2E = 1.4426950408889634;
      public const double LOG10E = 0.4342944819032518;
      public const double PI = 3.14159265358979323846;
      public const double SQRT1_2 = 0.7071067811865476;
      public const double SQRT2 = 1.4142135623730951;
    
      private static readonly System.Random internalRandom = new System.Random();
      internal static MathObject ob = null;
                        
      internal MathObject(ScriptObject parent)
        : base(parent) {
      }
    
      [JSFunctionAttribute(0, JSBuiltin.Math_abs)]
      public static double abs(double d){
        if (d < 0)
          return -d;
        else if (d > 0)
          return d;
        else if (d == d)
          return 0.0;
        else
          return d;
      }
      
      [JSFunctionAttribute(0, JSBuiltin.Math_acos)]
      public static double acos(double x){
        return System.Math.Acos(x);
      }
      
      [JSFunctionAttribute(0, JSBuiltin.Math_asin)]
      public static double asin(double x){
        return System.Math.Asin(x);
      }
      
      [JSFunctionAttribute(0, JSBuiltin.Math_atan)]
      public static double atan(double x){
        return System.Math.Atan(x);
      }
      
      [JSFunctionAttribute(0, JSBuiltin.Math_atan2)]
      public static double atan2(double dy, double dx){
        return System.Math.Atan2(dy, dx);
      }
      
      [JSFunctionAttribute(0, JSBuiltin.Math_ceil)]
      public static double ceil(double x){
        return System.Math.Ceiling(x);
      }
      
      private static double Compare(double x, double y){
        if (x != 0 || y != 0)
          if (x == y) //x and y could be infinities, in which case - will not return 0.
            return 0;
          else
            return x - y;
        double x1 = 1 / x; //will be < 0 if x == -0
        double y1 = 1 / y;
        if (x1 < 0){
          return y1 < 0 ? 0 : -1;
        }else if (y1 < 0)
          return 1;
        else
          return 0;
      }
      
      [JSFunctionAttribute(0, JSBuiltin.Math_cos)]
      public static double cos(double x){
        return System.Math.Cos(x);
      }
      
      [JSFunctionAttribute(0, JSBuiltin.Math_exp)]
      public static double exp(double x){
        return System.Math.Exp(x);
      }
      
      [JSFunctionAttribute(0, JSBuiltin.Math_floor)]
      public static double floor(double x){
        return System.Math.Floor(x);
      }
      
      internal override String GetClassName(){
        return "Math";
      }
      
      [JSFunctionAttribute(0, JSBuiltin.Math_log)]
      public static double log(double x){
        return System.Math.Log(x);
      }
      
      [JSFunctionAttribute(JSFunctionAttributeEnum.HasVarArgs, JSBuiltin.Math_max)]
      public static double max(Object x, Object y, params Object[] args){
        if (x is Missing)
          return Double.NegativeInfinity;
        double dx = Convert.ToNumber(x);
        if (y is Missing)
          return dx;
        double dy = Convert.ToNumber(y);
        double result = MathObject.Compare(dx, dy);
        if (result != result) return result;
        double lhMax = dx;
        if (result < 0) lhMax = dy;
        if (args.Length == 0) return lhMax;
        return MathObject.maxv(lhMax, args, 0);
      }
      
      private static double maxv(double lhMax, Object[] args, int start){
        if (args.Length == start)
          return lhMax;
        double head = Convert.ToNumber(args[start]);
        double result = MathObject.Compare(lhMax, head);
        if (result != result) return result;
        if (result > 0) head = lhMax;
        return MathObject.maxv(head, args, start+1);
      }

      [JSFunctionAttribute(JSFunctionAttributeEnum.HasVarArgs, JSBuiltin.Math_min)]
      public static double min(Object x, Object y, params Object[] args){
        if (x is Missing)
          return Double.PositiveInfinity;
        double dx = Convert.ToNumber(x);
        if (y is Missing)
          return dx;
        double dy = Convert.ToNumber(y);
        double result = MathObject.Compare(dx, dy);
        if (result != result) return result;
        double lhMin = dx;
        if (result > 0) lhMin = dy;
        if (args.Length == 0) return lhMin;
        return MathObject.minv(lhMin, args, 0);
      }
      
      private static double minv(double lhMin, Object[] args, int start){
        if (args.Length == start)
          return lhMin;
        double head = Convert.ToNumber(args[start]);
        double result = MathObject.Compare(lhMin, head);
        if (result != result) return result;
        if (result < 0) head = lhMin;
        return MathObject.minv(head, args, start+1);
      }
      
      [JSFunctionAttribute(0, JSBuiltin.Math_pow)]
      public static double pow(double dx, double dy){
        if (dy == 0)
          return 1.0;
        if ((dx == 1.0 || dx == -1.0) && (dy == Double.PositiveInfinity || dy == Double.NegativeInfinity))
          return Double.NaN;
        if (Double.IsNaN(dy))
          return Double.NaN;
        
        // If dy is an odd integer, return -0. This case is inconsistent between x86 and amd64.
        if (dx == Double.NegativeInfinity && dy < 0.0) {
          if (Math.IEEERemainder((-dy)+1.0, 2.0) == 0.0)
            return -0.0;
        }
        
        try{
          return System.Math.Pow(dx, dy);
        }catch{
          if (dx != dx || dy != dy)
            return Double.NaN;
          if (dx == 0.0)
            if (dy < 0.0){
              if ((double)(long)dy == dy && ((long)(-dy)) % 2 > 0){
                double f = 1.0 / dx;
                if (!(f >= 0.0))
                  return Double.NegativeInfinity;
              }
              return Double.PositiveInfinity;
            }
          return Double.NaN;
        }
      }
      
      [JSFunctionAttribute(0, JSBuiltin.Math_random)]
      public static double random(){
        return MathObject.internalRandom.NextDouble();
      }
      
      [JSFunctionAttribute(0, JSBuiltin.Math_round)]
      public static double round(double d){
        if (d == 0)
          return d;
        else
          return System.Math.Floor(d+0.5);
      }
      
      [JSFunctionAttribute(0, JSBuiltin.Math_sin)]
      public static double sin(double x){
        return System.Math.Sin(x);
      }
      
      [JSFunctionAttribute(0, JSBuiltin.Math_sqrt)]
      public static double sqrt(double x){
        return System.Math.Sqrt(x);
      }
      
      [JSFunctionAttribute(0, JSBuiltin.Math_tan)]
      public static double tan(double x){
        return System.Math.Tan(x);
      }
      
    }
}
