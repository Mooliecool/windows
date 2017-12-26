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
    using System.Diagnostics;
    
    public sealed class NumericBinary : BinaryOp{
      private Object metaData = null;
    
      internal NumericBinary(Context context, AST operand1, AST operand2, JSToken operatorTok)
        : base(context, operand1, operand2, operatorTok) {
      }
      
      public NumericBinary(int operatorTok)
        : base(null, null, null, (JSToken)operatorTok){
      }
      
      internal override Object Evaluate(){
        return this.EvaluateNumericBinary(this.operand1.Evaluate(), this.operand2.Evaluate());
      }
      
#if !DEBUG
      [DebuggerStepThroughAttribute]
      [DebuggerHiddenAttribute]
#endif      
      public Object EvaluateNumericBinary(Object v1, Object v2){
        if (v1 is Int32 && v2 is Int32)
          return NumericBinary.DoOp((Int32)v1, (Int32)v2, this.operatorTok);
        else if (v1 is Double && v2 is Double)
          return NumericBinary.DoOp((Double)v1, (Double)v2, this.operatorTok);
        else
          return this.EvaluateNumericBinary(v1, v2, this.operatorTok);
      }
      
#if !DEBUG
      [DebuggerStepThroughAttribute]
      [DebuggerHiddenAttribute]
#endif      
      private Object EvaluateNumericBinary(Object v1, Object v2, JSToken operatorTok){
        IConvertible ic1 = Convert.GetIConvertible(v1);
        IConvertible ic2 = Convert.GetIConvertible(v2);
        TypeCode t1 = Convert.GetTypeCode(v1, ic1);
        TypeCode t2 = Convert.GetTypeCode(v2, ic2);
        switch (t1){
          case TypeCode.Empty: 
            return Double.NaN;
            
          case TypeCode.DBNull:
            return this.EvaluateNumericBinary(0, v2, operatorTok);
            
          case TypeCode.Char: {
            Object result;
            int val = ic1.ToInt32(null);
            switch (t2){
              case TypeCode.Empty: 
                return Double.NaN;
              case TypeCode.DBNull:
                return NumericBinary.DoOp(val, 0, operatorTok);
              case TypeCode.Boolean:
              case TypeCode.Char:
              case TypeCode.SByte:
              case TypeCode.Byte:
              case TypeCode.Int16:
              case TypeCode.UInt16:
              case TypeCode.Int32:
                result = NumericBinary.DoOp(val, ic2.ToInt32(null), operatorTok);
                break;
              case TypeCode.UInt32:
              case TypeCode.Int64:
                result = NumericBinary.DoOp((long)val, ic2.ToInt64(null), operatorTok);
                break;
              case TypeCode.UInt64:
                result = NumericBinary.DoOp((double)val, ic2.ToDouble(null), operatorTok);
                break;
              case TypeCode.Single:
              case TypeCode.Double:
                result = NumericBinary.DoOp((double)ic1.ToInt32(null), ic2.ToDouble(null), operatorTok);
                break;
              case TypeCode.String: 
                result = NumericBinary.DoOp(val, Convert.ToNumber(v2, ic2), operatorTok);
                break;
              
              case TypeCode.Object: 
              case TypeCode.Decimal:
              case TypeCode.DateTime: 
              default:
                result = null;
                break; 
            }
            if (this.operatorTok == JSToken.Minus && result != null && t2 != TypeCode.Char){
              return Convert.Coerce2(result, TypeCode.Char, false);
            } else if (result != null)
              return result;
            break;
          }
          case TypeCode.Boolean:
          case TypeCode.SByte:
          case TypeCode.Byte:
          case TypeCode.Int16:
          case TypeCode.UInt16:
          case TypeCode.Int32:
            {int val = ic1.ToInt32(null);
            switch (t2){
              case TypeCode.Empty: 
                return Double.NaN;
              case TypeCode.DBNull:
                return NumericBinary.DoOp(val, 0, operatorTok);
              case TypeCode.Boolean:
              case TypeCode.Char:
              case TypeCode.SByte:
              case TypeCode.Byte:
              case TypeCode.Int16:
              case TypeCode.UInt16:
              case TypeCode.Int32:
                return NumericBinary.DoOp(val, ic2.ToInt32(null), operatorTok);
              case TypeCode.UInt32:
              case TypeCode.Int64:
                return NumericBinary.DoOp((long)val, ic2.ToInt64(null), operatorTok);
              case TypeCode.UInt64:
                if (val >= 0)
                  return NumericBinary.DoOp((ulong)val, ic2.ToUInt64(null), operatorTok);
                else
                  return NumericBinary.DoOp((double)val, ic2.ToDouble(null), operatorTok);
              case TypeCode.Single:
              case TypeCode.Double:
                return NumericBinary.DoOp((double)val, ic2.ToDouble(null), operatorTok);
              case TypeCode.Object: 
              case TypeCode.Decimal:
              case TypeCode.DateTime: 
              case TypeCode.String:
                break; 
            }
            break;}
            
          case TypeCode.UInt32:
            {uint val = ic1.ToUInt32(null);
            switch (t2){
              case TypeCode.Empty: 
                return Double.NaN;
              case TypeCode.DBNull:
                return NumericBinary.DoOp(val, 0, operatorTok);
              case TypeCode.SByte:
              case TypeCode.Byte:
              case TypeCode.Int16:
              case TypeCode.Int32:
                int val2 = ic2.ToInt32(null);
                if (val2 >= 0)
                  return NumericBinary.DoOp(val, (uint)val2, operatorTok);
                else
                  return NumericBinary.DoOp((long)val, (long)val2, operatorTok);
              case TypeCode.Int64:
                return NumericBinary.DoOp((long)val, ic2.ToInt64(null), operatorTok);
              case TypeCode.Boolean:
              case TypeCode.Char:
              case TypeCode.UInt16:
              case TypeCode.UInt32:
                return NumericBinary.DoOp(val, ic2.ToUInt32(null), operatorTok);
              case TypeCode.UInt64:
                return NumericBinary.DoOp((ulong)val, ic2.ToUInt64(null), operatorTok);
              case TypeCode.Single:
              case TypeCode.Double:
                return NumericBinary.DoOp((double)val, ic2.ToDouble(null), operatorTok);
              case TypeCode.Object: 
              case TypeCode.Decimal:
              case TypeCode.DateTime: 
              case TypeCode.String:
                break; 
            }
            break;}
            
          case TypeCode.Int64:
            {long val = ic1.ToInt64(null);
            switch (t2){
              case TypeCode.Empty: 
                return Double.NaN;
              case TypeCode.DBNull:
                return NumericBinary.DoOp(val, 0, operatorTok);
              case TypeCode.Boolean:
              case TypeCode.Char:
              case TypeCode.SByte:
              case TypeCode.Byte:
              case TypeCode.Int16:
              case TypeCode.UInt16:
              case TypeCode.Int32:
              case TypeCode.UInt32:
              case TypeCode.Int64:
                return NumericBinary.DoOp(val, ic2.ToInt64(null), operatorTok);
              case TypeCode.UInt64:
                if (val >= 0)
                  return NumericBinary.DoOp((ulong)val, ic2.ToUInt64(null), operatorTok);
                else
                  return NumericBinary.DoOp((double)val, ic2.ToDouble(null), operatorTok);
              case TypeCode.Single:
              case TypeCode.Double:
                return NumericBinary.DoOp((double)val, ic2.ToDouble(null), operatorTok);
              case TypeCode.Object: 
              case TypeCode.Decimal:
              case TypeCode.DateTime: 
              case TypeCode.String:
                break; 
            }
            break;}
            
          
          case TypeCode.UInt64:
            {ulong val = ic1.ToUInt64(null);
            switch (t2){
              case TypeCode.Empty: 
                return Double.NaN;
              case TypeCode.DBNull:
                return NumericBinary.DoOp(val, 0, operatorTok);
              case TypeCode.SByte:
              case TypeCode.Byte:
              case TypeCode.Int16:
              case TypeCode.Int32:
              case TypeCode.Int64:
                long val2 = ic2.ToInt64(null);
                if (val2 >= 0)
                  return NumericBinary.DoOp(val, (ulong)val2, operatorTok);
                else
                  return NumericBinary.DoOp((double)val, (double)val2, operatorTok);
              case TypeCode.Boolean:
              case TypeCode.Char:
              case TypeCode.UInt16:
              case TypeCode.UInt32:
              case TypeCode.UInt64:
                return NumericBinary.DoOp(val, ic2.ToUInt64(null), operatorTok);
              case TypeCode.Single:
              case TypeCode.Double:
                return NumericBinary.DoOp((double)val, ic2.ToDouble(null), operatorTok);
              case TypeCode.Object: 
              case TypeCode.Decimal:
              case TypeCode.DateTime: 
              case TypeCode.String:
                break; 
            }
            break;}
            
          case TypeCode.Single:
          case TypeCode.Double:
            {double d = ic1.ToDouble(null);
            switch (t2){
              case TypeCode.Empty: 
                return Double.NaN;
              case TypeCode.DBNull:
                return NumericBinary.DoOp(d, 0, operatorTok);
              case TypeCode.Boolean:
              case TypeCode.Char:
              case TypeCode.SByte:
              case TypeCode.Byte:
              case TypeCode.Int16:
              case TypeCode.UInt16:
              case TypeCode.Int32:
                return NumericBinary.DoOp(d, (double)ic2.ToInt32(null), operatorTok);
              case TypeCode.UInt32:
              case TypeCode.Int64:
              case TypeCode.UInt64:
              case TypeCode.Single:
              case TypeCode.Double:
                return NumericBinary.DoOp(d, ic2.ToDouble(null), operatorTok);
              case TypeCode.Object: 
              case TypeCode.Decimal:
              case TypeCode.DateTime: 
              case TypeCode.String:
                break; 
            }
            break;}
            
          case TypeCode.Object: 
          case TypeCode.Decimal:
          case TypeCode.DateTime: 
          case TypeCode.String:
            break;
        }
        if (v2 == null) return Double.NaN;
        MethodInfo oper = this.GetOperator(v1.GetType(), v2.GetType());
        if (oper != null)
          return oper.Invoke(null, (BindingFlags)0, JSBinder.ob, new Object[]{v1, v2}, null);
        else{
          return NumericBinary.DoOp(v1, v2, ic1, ic2, operatorTok);
        }
      }

      public static Object DoOp(Object v1, Object v2, JSToken operatorTok){
        return DoOp(v1, v2, Convert.GetIConvertible(v1), Convert.GetIConvertible(v2), operatorTok);
      }
      
      private static Object DoOp(Object v1, Object v2, IConvertible ic1, IConvertible ic2, JSToken operatorTok){
        if (operatorTok == JSToken.Minus){
          IConvertible ic1a = ic1;
          Object p1 = Convert.ToPrimitive(v1, PreferredType.Either, ref ic1a);
          TypeCode t1 = Convert.GetTypeCode(p1, ic1a);
          if (t1 == TypeCode.Char){
            IConvertible ic2a = ic2;
            Object p2 = Convert.ToPrimitive(v2, PreferredType.Either, ref ic2a);
            TypeCode t2 = Convert.GetTypeCode(p2, ic2a);
            if (t2 == TypeCode.String){
              String str2 = ic2a.ToString(null);
              if (str2.Length == 1){
                t2 = TypeCode.Char;
                p2 = str2[0];
                ic2a = Convert.GetIConvertible(p2);
              }
            }
            Object result = NumericBinary.DoOp(Convert.ToNumber(p1, ic1a), Convert.ToNumber(p2, ic2a), operatorTok);
            if (t2 != TypeCode.Char)
              result = Convert.Coerce2(result, TypeCode.Char, false);
            return result;
          }
        }
        return NumericBinary.DoOp(Convert.ToNumber(v1, ic1), Convert.ToNumber(v2, ic2), operatorTok);
      }
      
      private static Object DoOp(int x, int y, JSToken operatorTok){
        switch (operatorTok){
          case JSToken.Divide:  
            return ((double)x) / (double)y;
          case JSToken.Minus:
            int r = x - y;
            if ((r < x) == (y > 0)) // If the result is on the opposite side of x as y is to 0, no overflow occured.
              return r;
            return ((double)x) - (double)y;
          case JSToken.Modulo:
            if (x <= 0 || y <= 0)
              return ((double)x) % (double)y; //Need to result in a signed 0
            return x % y;
          case JSToken.Multiply:
            if (x == 0 || y == 0) return ((double)x) * (double)y; //Need to result in a signed 0
            try{
              checked {return x * y;}
            }catch(OverflowException){
              return ((double)x) * (double)y;
            }
          default:
            throw new JScriptException(JSError.InternalError);
        }
      }
    
      private static Object DoOp(uint x, uint y, JSToken operatorTok){
        switch (operatorTok){
          case JSToken.Divide:  
            return ((double)x) / (double)y;
          case JSToken.Minus:
            uint r = x - y;
            if (r <= x) // Since y is 0 or positive, if the result left of x or at x, no overflow occured.
              return r;
            return ((double)x) - (double)y;
          case JSToken.Modulo:
            if (y == 0) return Double.NaN;
            return x % y;
          case JSToken.Multiply:
            try{
              checked {return x * y;}
            }catch(OverflowException){
              return ((double)x) * (double)y;
            }
          default:
            throw new JScriptException(JSError.InternalError);
        }
      }
    
      private static Object DoOp(long x, long y, JSToken operatorTok){
        switch (operatorTok){
          case JSToken.Divide:  
            return ((double)x) / (double)y;
          case JSToken.Minus:
            long r = x - y;
            if ((r < x) == (y > 0)) // If the result is on the opposite side of x as y is to 0, no overflow occured.
              return r;
            return ((double)x) - (double)y;
          case JSToken.Modulo:
            if (y == 0) return Double.NaN;
            long result = x % y;
            if (result == 0) //Need to result in a signed 0
              if (x < 0)
                if (y < 0) return 0; else return 1.0 / Double.NegativeInfinity;
              else
                if (y < 0) return 1.0 / Double.NegativeInfinity; else return 0;
            return result;
          case JSToken.Multiply:
            if (x == 0 || y == 0) return ((double)x) * (double)y; //Need to result in a signed 0
            try{
              checked {return x * y;}
            }catch(OverflowException){
              return ((double)x) * (double)y;
            }
          default:
            throw new JScriptException(JSError.InternalError);
        }
      }
    
      private static Object DoOp(ulong x, ulong y, JSToken operatorTok){
        switch (operatorTok){
          case JSToken.Divide:  
            return ((double)x) / (double)y;
          case JSToken.Minus:
            ulong r = x - y;
            if (r <= x) // Since y is 0 or positive, if the result left of x or at x, no overflow occured.
              return r;
            return ((double)x) - (double)y;
          case JSToken.Modulo:
            if (y == 0) return Double.NaN;
            return x % y;
          case JSToken.Multiply:
            try{
              checked {return x * y;}
            }catch(OverflowException){
              return ((double)x) * (double)y;
            }
          default:
            throw new JScriptException(JSError.InternalError);
        }
      }
    
      private static Object DoOp(double x, double y, JSToken operatorTok){
        switch (operatorTok){
          case JSToken.Divide:  
            return x / y;
          case JSToken.Minus:
            return x - y;
          case JSToken.Modulo:
            return x % y;
          case JSToken.Multiply:
            return x * y;
          default:
            throw new JScriptException(JSError.InternalError);
        }
      }
    
      internal override IReflect InferType(JSField inference_target){
        Debug.Assert(Globals.TypeRefs.InReferenceContext(this.type1));
        Debug.Assert(Globals.TypeRefs.InReferenceContext(this.type2));
        MethodInfo oper;
        if (this.type1 == null || inference_target != null){
          oper = this.GetOperator(this.operand1.InferType(inference_target), this.operand2.InferType(inference_target));
        }else          
          oper = this.GetOperator(this.type1, this.type2);
        if (oper != null){
          this.metaData = oper;
          return oper.ReturnType;
        } 
        if (this.type1 == Typeob.Char && this.operatorTok == JSToken.Minus){
          TypeCode t2 = Type.GetTypeCode(this.type2);
          if (Convert.IsPrimitiveNumericTypeCode(t2) || t2 == TypeCode.Boolean)
            return Typeob.Char;
          else if (t2 == TypeCode.Char)
            return Typeob.Int32;
        } 
        if ((Convert.IsPrimitiveNumericTypeFitForDouble(this.type1) || Typeob.JSObject.IsAssignableFrom(this.type1)) 
        &&  (Convert.IsPrimitiveNumericTypeFitForDouble(this.type2) || Typeob.JSObject.IsAssignableFrom(this.type2)))
          return Typeob.Double;
        else
          return Typeob.Object;
      }
    
      internal override void TranslateToIL(ILGenerator il, Type rtype){
        if (this.metaData == null){
          Type rt = Typeob.Double;
          if (Convert.IsPrimitiveNumericType(rtype) && Convert.IsPromotableTo(this.type1, rtype) &&  Convert.IsPromotableTo(this.type2, rtype))
            rt = rtype;
          if (this.operatorTok == JSToken.Divide)
            rt = Typeob.Double;
          else if (rt == Typeob.SByte || rt == Typeob.Int16)
            rt = Typeob.Int32;
          else if (rt == Typeob.Byte || rt == Typeob.UInt16 || rt == Typeob.Char)
            rt = Typeob.UInt32;
          this.operand1.TranslateToIL(il, rt);
          this.operand2.TranslateToIL(il, rt);
          if (rt == Typeob.Double || rt == Typeob.Single){
            switch (this.operatorTok){
              case JSToken.Divide:  
                il.Emit(OpCodes.Div); break;
              case JSToken.Minus:
                il.Emit(OpCodes.Sub); break;
              case JSToken.Modulo:
                il.Emit(OpCodes.Rem); break;
              case JSToken.Multiply:
                il.Emit(OpCodes.Mul); break;
              default:
                throw new JScriptException(JSError.InternalError, this.context);
            }
          }else if (rt == Typeob.Int32 || rt == Typeob.Int64){
            switch (this.operatorTok){
              case JSToken.Divide:  
                il.Emit(OpCodes.Div); break;
              case JSToken.Minus:
                il.Emit(OpCodes.Sub_Ovf); break;
              case JSToken.Modulo:
                il.Emit(OpCodes.Rem); break;
              case JSToken.Multiply:
                il.Emit(OpCodes.Mul_Ovf); break;
              default:
                throw new JScriptException(JSError.InternalError, this.context);
            }
          }else{
            switch (this.operatorTok){
              case JSToken.Divide:  
                il.Emit(OpCodes.Div); break;
              case JSToken.Minus:
                il.Emit(OpCodes.Sub_Ovf_Un); break;
              case JSToken.Modulo:
                il.Emit(OpCodes.Rem); break;
              case JSToken.Multiply:
                il.Emit(OpCodes.Mul_Ovf_Un); break;
              default:
                throw new JScriptException(JSError.InternalError, this.context);
            }
          }
          if (Convert.ToType(this.InferType(null)) == Typeob.Char){
            Convert.Emit(this, il, rt, Typeob.Char);
            Convert.Emit(this, il, Typeob.Char, rtype);
          } else
            Convert.Emit(this, il, rt, rtype);
          return;
        }
        if (this.metaData is MethodInfo){
          MethodInfo oper = (MethodInfo)this.metaData;
          ParameterInfo[] pars = oper.GetParameters();
          this.operand1.TranslateToIL(il, pars[0].ParameterType);
          this.operand2.TranslateToIL(il, pars[1].ParameterType);
          il.Emit(OpCodes.Call, oper);
          Convert.Emit(this, il, oper.ReturnType, rtype);
          return;
        }
        //Getting here is just too bad. We do not know until the code runs whether or not to call an overloaded operator method.
        //Compile operands to objects and devolve the decision making to run time thunks 
        il.Emit(OpCodes.Ldloc, (LocalBuilder)this.metaData);
        this.operand1.TranslateToIL(il, Typeob.Object);
        this.operand2.TranslateToIL(il, Typeob.Object);
        il.Emit(OpCodes.Call, CompilerGlobals.evaluateNumericBinaryMethod);
        Convert.Emit(this, il, Typeob.Object, rtype);
      }
    
      internal override void TranslateToILInitializer(ILGenerator il){
        IReflect rtype = this.InferType(null);
        this.operand1.TranslateToILInitializer(il);
        this.operand2.TranslateToILInitializer(il);
        if (rtype != Typeob.Object)
          return;
        this.metaData = il.DeclareLocal(Typeob.NumericBinary);
        ConstantWrapper.TranslateToILInt(il, (int)this.operatorTok);
        il.Emit(OpCodes.Newobj, CompilerGlobals.numericBinaryConstructor);
        il.Emit(OpCodes.Stloc, (LocalBuilder)this.metaData);
      }
    }
}
