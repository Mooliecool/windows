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
    
    public sealed class Plus : BinaryOp{
      private Object metaData = null;
      
      internal Plus(Context context, AST operand1, AST operand2)
        : base(context, operand1, operand2, JSToken.Plus){
      }
      
      public Plus()
        : base(null, null, null, JSToken.Plus){
      }
      
      internal override Object Evaluate(){
        return this.EvaluatePlus(this.operand1.Evaluate(), this.operand2.Evaluate());
      }
      
#if !DEBUG
      [DebuggerStepThroughAttribute]
      [DebuggerHiddenAttribute]
#endif      
      public Object EvaluatePlus(Object v1, Object v2){
        if (v1 is Int32 && v2 is Int32)
          return Plus.DoOp((Int32)v1, (Int32)v2);
        else if (v1 is Double && v2 is Double)
          return Plus.DoOp((Double)v1, (Double)v2);
        else
          return this.EvaluatePlus2(v1, v2);
      }
      
#if !DEBUG
      [DebuggerStepThroughAttribute]
      [DebuggerHiddenAttribute]
#endif      
      private Object EvaluatePlus2(Object v1, Object v2){
        IConvertible ic1 = Convert.GetIConvertible(v1);
        IConvertible ic2 = Convert.GetIConvertible(v2);
        TypeCode t1 = Convert.GetTypeCode(v1, ic1);
        TypeCode t2 = Convert.GetTypeCode(v2, ic2);
        switch (t1){
          case TypeCode.Empty:
            return Plus.DoOp(v1, v2);
            
          case TypeCode.DBNull:
            switch (t2){
              case TypeCode.Empty: 
                return Double.NaN;
              case TypeCode.DBNull:
                return 0;
              case TypeCode.Boolean:
              case TypeCode.Char:
              case TypeCode.SByte:
              case TypeCode.Byte:
              case TypeCode.Int16:
              case TypeCode.UInt16:
              case TypeCode.Int32:
                return ic2.ToInt32(null);
              case TypeCode.UInt32:
                return ic2.ToUInt32(null);
              case TypeCode.Int64:
                return ic2.ToInt64(null);
              case TypeCode.UInt64:
                return ic2.ToUInt64(null);
              case TypeCode.Single:
              case TypeCode.Double:
                return ic2.ToDouble(null);
              case TypeCode.Object: 
              case TypeCode.Decimal:
              case TypeCode.DateTime: 
                break;
              case TypeCode.String:
                return "null" + ic2.ToString(null);
            }
            break;
             
          case TypeCode.Char:
            {int val = ic1.ToInt32(null);
            switch (t2){
              case TypeCode.Empty: 
                return Double.NaN; 
              case TypeCode.DBNull:
                return val; 
              case TypeCode.Boolean:
              case TypeCode.SByte:
              case TypeCode.Byte:
              case TypeCode.Int16:
              case TypeCode.UInt16:
              case TypeCode.Int32:
                return ((IConvertible)Plus.DoOp(val, ic2.ToInt32(null))).ToChar(null);
              case TypeCode.UInt32:
              case TypeCode.Int64:
                return ((IConvertible)Plus.DoOp((long)val, ic2.ToInt64(null))).ToChar(null);
              case TypeCode.UInt64:
                  return ((IConvertible)Plus.DoOp((ulong)val, ic2.ToUInt64(null))).ToChar(null);
              case TypeCode.Single:
              case TypeCode.Double:
                checked {return (char)(int)(Convert.CheckIfDoubleIsInteger((double)Plus.DoOp((double)val, ic2.ToDouble(null))));}
              case TypeCode.Object: 
              case TypeCode.Decimal:
              case TypeCode.DateTime: 
                return Plus.DoOp(v1, v2);
              case TypeCode.Char:
              case TypeCode.String:
                return ic1.ToString(null) + ic2.ToString(null);
            }
            break;}
            
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
                return val;
              case TypeCode.Char:
                return ((IConvertible)Plus.DoOp(val, ic2.ToInt32(null))).ToChar(null);
              case TypeCode.Boolean:
              case TypeCode.SByte:
              case TypeCode.Byte:
              case TypeCode.Int16:
              case TypeCode.UInt16:
              case TypeCode.Int32:
                return Plus.DoOp(val, ic2.ToInt32(null));
              case TypeCode.UInt32:
              case TypeCode.Int64:
                return Plus.DoOp((long)val, ic2.ToInt64(null));
              case TypeCode.UInt64:
                if (val >= 0)
                  return Plus.DoOp((ulong)val, ic2.ToUInt64(null));
                else                  
                  return Plus.DoOp((double)val, ic2.ToDouble(null));
              case TypeCode.Single:
              case TypeCode.Double:
                return Plus.DoOp((double)val, ic2.ToDouble(null));
              case TypeCode.Object: 
              case TypeCode.Decimal:
              case TypeCode.DateTime: 
                break;
              case TypeCode.String:
                return Convert.ToString(v1) + ic2.ToString(null);
            }
            break;}
            
          case TypeCode.UInt32:
            {uint val = ic1.ToUInt32(null);
            switch (t2){
              case TypeCode.Empty: 
                return Double.NaN;
              case TypeCode.DBNull:
                return val;
              case TypeCode.SByte:
              case TypeCode.Byte:
              case TypeCode.Int16:
              case TypeCode.Int32:
                int val2 = ic2.ToInt32(null);
                if (val2 >= 0)
                  return Plus.DoOp(val, (uint)val2);
                else
                  return Plus.DoOp((long)val, (long)val2);
              case TypeCode.Int64:
                return Plus.DoOp((long)val, ic2.ToInt64(null));
              case TypeCode.Char:
                return ((IConvertible)Plus.DoOp(val, ic2.ToUInt32(null))).ToChar(null);
              case TypeCode.Boolean:
              case TypeCode.UInt16:
              case TypeCode.UInt32:
                return Plus.DoOp(val, ic2.ToUInt32(null));
              case TypeCode.UInt64:
                return Plus.DoOp((ulong)val, ic2.ToUInt64(null));
              case TypeCode.Single:
              case TypeCode.Double:
                return Plus.DoOp((double)val, ic2.ToDouble(null));
              case TypeCode.Object: 
              case TypeCode.Decimal:
              case TypeCode.DateTime: 
                break;
              case TypeCode.String:
                return Convert.ToString(v1) + ic2.ToString(null);
            }
            break;}
            
          case TypeCode.Int64:
            {long val = ic1.ToInt64(null);
            switch (t2){
              case TypeCode.Empty: 
                return Double.NaN;
              case TypeCode.DBNull:
                return val;
              case TypeCode.Char:
                return ((IConvertible)Plus.DoOp(val, ic2.ToInt64(null))).ToChar(null);
              case TypeCode.Boolean:
              case TypeCode.SByte:
              case TypeCode.Byte:
              case TypeCode.Int16:
              case TypeCode.UInt16:
              case TypeCode.Int32:
              case TypeCode.UInt32:
              case TypeCode.Int64:
                return Plus.DoOp(val, ic2.ToInt64(null));
              case TypeCode.UInt64:
                if (val >= 0)
                  return Plus.DoOp((ulong)val, ic2.ToUInt64(null));
                else                  
                  return Plus.DoOp((double)val, ic2.ToDouble(null));
              case TypeCode.Single:
              case TypeCode.Double:
                return Plus.DoOp((double)val, ic2.ToDouble(null));
              case TypeCode.Object: 
              case TypeCode.Decimal:
              case TypeCode.DateTime: 
                break;
              case TypeCode.String:
                return Convert.ToString(v1) + ic2.ToString(null);
            }
            break;}
            
          case TypeCode.UInt64:
            {ulong val = ic1.ToUInt64(null);
            switch (t2){
              case TypeCode.Empty: 
                return Double.NaN;
              case TypeCode.DBNull:
                return val;
              case TypeCode.SByte:
              case TypeCode.Byte:
              case TypeCode.Int16:
              case TypeCode.Int32:
              case TypeCode.Int64:
                long val2 = ic2.ToInt64(null);
                if (val2 >= 0)
                  return Plus.DoOp(val, (ulong)val2);
                else                  
                  return Plus.DoOp((double)val, (double)val2);
              case TypeCode.Char:
                return ((IConvertible)Plus.DoOp(val, ic2.ToUInt64(null))).ToChar(null);
              case TypeCode.UInt16:
              case TypeCode.Boolean:
              case TypeCode.UInt32:
              case TypeCode.UInt64:
                return Plus.DoOp(val, ic2.ToUInt64(null));
              case TypeCode.Single:
              case TypeCode.Double:
                return Plus.DoOp((double)val, ic2.ToDouble(null));
              case TypeCode.Object: 
              case TypeCode.Decimal:
              case TypeCode.DateTime: 
                break;
              case TypeCode.String:
                return Convert.ToString(v1) + ic2.ToString(null);
            }
            break;}
            
          case TypeCode.Single:
          case TypeCode.Double:{
            double d = ic1.ToDouble(null);
            switch (t2){
              case TypeCode.Empty:
                return Double.NaN; 
              case TypeCode.DBNull:
                return ic1.ToDouble(null);
              case TypeCode.Char:
                return System.Convert.ToChar(System.Convert.ToInt32((d + (double)ic2.ToInt32(null))));
              case TypeCode.Boolean:
              case TypeCode.SByte:
              case TypeCode.Byte:
              case TypeCode.Int16:
              case TypeCode.UInt16:
              case TypeCode.Int32:
                return d + (double)ic2.ToInt32(null);
              case TypeCode.UInt32:
              case TypeCode.Int64:
              case TypeCode.UInt64:
              case TypeCode.Single:
              case TypeCode.Double:
                return d + ic2.ToDouble(null);
              case TypeCode.Object: 
              case TypeCode.Decimal:
              case TypeCode.DateTime: 
                break;
              case TypeCode.String:
                return new ConcatString(Convert.ToString(d), ic2.ToString(null));
            }
            break;}
            
          case TypeCode.Object: 
          case TypeCode.Decimal:
          case TypeCode.DateTime: 
            break;
            
          case TypeCode.String:
            switch (t2){
              case TypeCode.Object: 
                break; 
              case TypeCode.String:
                if (v1 is ConcatString)
                  return new ConcatString((ConcatString)v1, ic2.ToString(null));
                else
                  return new ConcatString(ic1.ToString(null), ic2.ToString(null));
              default:
                if (v1 is ConcatString)
                  return new ConcatString((ConcatString)v1, Convert.ToString(v2));
                else
                  return new ConcatString(ic1.ToString(null), Convert.ToString(v2));
            }
            break;
        }
        MethodInfo oper = this.GetOperator(v1 == null ? Typeob.Empty : v1.GetType(), v2 == null ? Typeob.Empty : v2.GetType());
        if (oper != null)
          return oper.Invoke(null, (BindingFlags)0, JSBinder.ob, new Object[]{v1, v2}, null);
        else
          return Plus.DoOp(v1, v2);
      }
      
      private new MethodInfo GetOperator(IReflect ir1, IReflect ir2){
        Type t1 = ir1 is Type ? (Type)ir1 : Typeob.Object;
        Type t2 = ir2 is Type ? (Type)ir2 : Typeob.Object;
        if (this.type1 == t1 && this.type2 == t2)
          return this.operatorMeth; 
        if (t1 == Typeob.String || t2 == Typeob.String ||
           ((Convert.IsPrimitiveNumericType(t1) || Typeob.JSObject.IsAssignableFrom(t1)) &&  
            (Convert.IsPrimitiveNumericType(t2) || Typeob.JSObject.IsAssignableFrom(t2)))){
          this.operatorMeth = null;
          this.type1 = t1;
          this.type2 = t2;
          return null;
        }
        return base.GetOperator(t1, t2);
      }
      
      private static Object DoOp(double x, double y){
        return x + y;
      }
    
      private static Object DoOp(int x, int y){
        int r = x + y;
        // If the result is on the same side of x as y is to 0, no overflow occured.
        if ((r < x) == (y < 0))
          return r;
        return ((double)x) + (double)y;
      }
    
      private static Object DoOp(long x, long y){
        // If the result is on the same side of x as y is to 0, no overflow occured.
        long r = x + y;
        if ((r < x) == (y < 0))
          return r;
        return ((double)x) + (double)y;
      }
    
      private static Object DoOp(uint x, uint y){
        uint r = x + y;
        if (r >= x) // Since y >= 0, no overflow occured if r >= x.
          return r;
        return ((double)x) + (double)y;
      }
    
      private static Object DoOp(ulong x, ulong y){
        ulong r = x + y;
        if (r >= x) // Since y >= 0, no overflow occured if r >= x.
          return r;
        return ((double)x) + (double)y;
      }
    
      public static Object DoOp(Object v1, Object v2){
        IConvertible ic1 = Convert.GetIConvertible(v1);
        IConvertible ic2 = Convert.GetIConvertible(v2);
        v1 = Convert.ToPrimitive(v1, PreferredType.Either, ref ic1);
        v2 = Convert.ToPrimitive(v2, PreferredType.Either, ref ic2);
        TypeCode t1 = Convert.GetTypeCode(v1, ic1);
        TypeCode t2 = Convert.GetTypeCode(v2, ic2);
        if (t1 == TypeCode.String)
          if (v1 is ConcatString)
            return new ConcatString((ConcatString)v1, Convert.ToString(v2, ic2));
          else
            return new ConcatString(ic1.ToString(null), Convert.ToString(v2, ic2));
        else if (t2 == TypeCode.String)
          return Convert.ToString(v1, ic1) + ic2.ToString(null);
        else if (t1 == TypeCode.Char && t2 == TypeCode.Char)
          return ic1.ToString(null) + ic2.ToString(null);
        else if ((t1 == TypeCode.Char && (Convert.IsPrimitiveNumericTypeCode(t2) || t2 == TypeCode.Boolean)) ||
                 (t2 == TypeCode.Char && (Convert.IsPrimitiveNumericTypeCode(t1) || t1 == TypeCode.Boolean)))
          return (char)(int)Runtime.DoubleToInt64(Convert.ToNumber(v1, ic1) + Convert.ToNumber(v2, ic2));
        else
          return Convert.ToNumber(v1, ic1) + Convert.ToNumber(v2, ic2);
      }
    
      internal override IReflect InferType(JSField inference_target){
        Debug.Assert(Globals.TypeRefs.InReferenceContext(this.type1));
        Debug.Assert(Globals.TypeRefs.InReferenceContext(this.type2));
        MethodInfo oper;
        if (this.type1 == null || inference_target != null)
          oper = this.GetOperator(this.operand1.InferType(inference_target), this.operand2.InferType(inference_target));
        else
          oper = this.GetOperator(this.type1, this.type2);
        if (oper != null){
          this.metaData = oper;
          return oper.ReturnType;
        }
        if (this.type1 == Typeob.String || this.type2 == Typeob.String)
          return Typeob.String;
        else if (this.type1 == Typeob.Char && this.type2 == Typeob.Char)
          return Typeob.String;
        else if (Convert.IsPrimitiveNumericTypeFitForDouble(this.type1)){
          if (this.type2 == Typeob.Char)
            return Typeob.Char;
          else if (Convert.IsPrimitiveNumericTypeFitForDouble(this.type2))
            return Typeob.Double;
          else
            return Typeob.Object;
        }else if (Convert.IsPrimitiveNumericTypeFitForDouble(this.type2)){
          if (this.type1 == Typeob.Char)
            return Typeob.Char;
          else if (Convert.IsPrimitiveNumericTypeFitForDouble(this.type1))
            return Typeob.Double;
          else
            return Typeob.Object;
        }else if (this.type1 == Typeob.Boolean && this.type2 == Typeob.Char)
          return Typeob.Char;
        else if (this.type1 == Typeob.Char && this.type2 == Typeob.Boolean)
          return Typeob.Char;
        else
          return Typeob.Object;
      }
      
      internal override void TranslateToIL(ILGenerator il, Type rtype){
        Type type = Convert.ToType(this.InferType(null));
        if (this.metaData == null){
          Type rt = Typeob.Object;
          if (rtype == Typeob.Double)
            rt = rtype;
          else if (this.type1 == Typeob.Char && this.type2 == Typeob.Char)
            rt = Typeob.String;
          else if (Convert.IsPrimitiveNumericType(rtype) && Convert.IsPromotableTo(this.type1, rtype) &&  Convert.IsPromotableTo(this.type2, rtype))
            rt = rtype;
          else if (this.type1 != Typeob.String && this.type2 != Typeob.String) //Both will be converted to numbers
            rt = Typeob.Double; //Won't get here unless InferType returned Typeob.Double.
          else
            rt = Typeob.String;
          if (rt == Typeob.SByte || rt == Typeob.Int16)
            rt = Typeob.Int32;
          else if (rt == Typeob.Byte || rt == Typeob.UInt16 || rt == Typeob.Char)
            rt = Typeob.UInt32;
          if (rt == Typeob.String){
            if (this.operand1 is Plus && this.type1 == rt){
              Plus op1 = (Plus)this.operand1;
              if (op1.operand1 is Plus && op1.type1 == rt){
                Plus op11 = (Plus)op1.operand1;
                if (op11.operand1 is Plus && op11.type1 == rt){
                  int len = op1.TranslateToILArrayOfStrings(il, 1);
                  il.Emit(OpCodes.Dup);
                  ConstantWrapper.TranslateToILInt(il, len-1);
                  this.operand2.TranslateToIL(il, rt);
                  il.Emit(OpCodes.Stelem_Ref);
                  il.Emit(OpCodes.Call, CompilerGlobals.stringConcatArrMethod);
                  Convert.Emit(this, il, rt, rtype);
                  return;
                }
                Plus.TranslateToStringWithSpecialCaseForNull(il, op11.operand1);
                Plus.TranslateToStringWithSpecialCaseForNull(il, op11.operand2);
                Plus.TranslateToStringWithSpecialCaseForNull(il, op1.operand2);
                Plus.TranslateToStringWithSpecialCaseForNull(il, this.operand2);
                il.Emit(OpCodes.Call, CompilerGlobals.stringConcat4Method);
                Convert.Emit(this, il, rt, rtype);
                return;
              }
              Plus.TranslateToStringWithSpecialCaseForNull(il, op1.operand1);
              Plus.TranslateToStringWithSpecialCaseForNull(il, op1.operand2);
              Plus.TranslateToStringWithSpecialCaseForNull(il, this.operand2);
              il.Emit(OpCodes.Call, CompilerGlobals.stringConcat3Method);
              Convert.Emit(this, il, rt, rtype);
              return;
            }
            Plus.TranslateToStringWithSpecialCaseForNull(il, this.operand1);
            Plus.TranslateToStringWithSpecialCaseForNull(il, this.operand2);
            il.Emit(OpCodes.Call, CompilerGlobals.stringConcat2Method);
            Convert.Emit(this, il, rt, rtype);
            return;
          }
          this.operand1.TranslateToIL(il, rt);
          this.operand2.TranslateToIL(il, rt);
          if (rt == Typeob.Object){
            il.Emit(OpCodes.Call, CompilerGlobals.plusDoOpMethod);
          }else if (rt == Typeob.Double || rt == Typeob.Single)
            il.Emit(OpCodes.Add);
          else if (rt == Typeob.Int32 || rt == Typeob.Int64)
            il.Emit(OpCodes.Add_Ovf);
          else
            il.Emit(OpCodes.Add_Ovf_Un);

          if (type == Typeob.Char){
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
        //Also get here when dealing with Int64 and UInt64. These cannot always be converted to doubles. The late-bound code checks for this.
        il.Emit(OpCodes.Ldloc, (LocalBuilder)this.metaData);
        this.operand1.TranslateToIL(il, Typeob.Object);
        this.operand2.TranslateToIL(il, Typeob.Object);
        il.Emit(OpCodes.Callvirt, CompilerGlobals.evaluatePlusMethod);
        Convert.Emit(this, il, Typeob.Object, rtype);
      }
      
      private int TranslateToILArrayOfStrings(ILGenerator il, int n){
        int len = n+2;
        if (this.operand1 is Plus && this.type1 == Typeob.String)
          len = ((Plus)this.operand1).TranslateToILArrayOfStrings(il, n+1);
        else{
          ConstantWrapper.TranslateToILInt(il, len);
          il.Emit(OpCodes.Newarr, Typeob.String);
          il.Emit(OpCodes.Dup);
          il.Emit(OpCodes.Ldc_I4_0);          
          Plus.TranslateToStringWithSpecialCaseForNull(il, this.operand1);
          il.Emit(OpCodes.Stelem_Ref);
        }
        il.Emit(OpCodes.Dup);
        ConstantWrapper.TranslateToILInt(il, len-1-n);
        Plus.TranslateToStringWithSpecialCaseForNull(il, this.operand2);
        il.Emit(OpCodes.Stelem_Ref);
        return len;
      }
      
      internal override void TranslateToILInitializer(ILGenerator il){
        IReflect rtype = this.InferType(null);
        this.operand1.TranslateToILInitializer(il);
        this.operand2.TranslateToILInitializer(il);
        if (rtype != Typeob.Object)
          return;
        this.metaData = il.DeclareLocal(Typeob.Plus);
        il.Emit(OpCodes.Newobj, CompilerGlobals.plusConstructor);
        il.Emit(OpCodes.Stloc, (LocalBuilder)this.metaData);
      }
      
      private static void TranslateToStringWithSpecialCaseForNull(ILGenerator il, AST operand){
        ConstantWrapper cw = operand as ConstantWrapper;
        if (cw != null){
          if (cw.value is DBNull)
            il.Emit(OpCodes.Ldstr, "null");
          else if (cw.value == Empty.Value)
            il.Emit(OpCodes.Ldstr, "undefined");
          else
            cw.TranslateToIL(il, Typeob.String);
        }else
          operand.TranslateToIL(il, Typeob.String);
      }
    }
}
