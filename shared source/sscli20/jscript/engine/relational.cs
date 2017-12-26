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
    
    public class Relational : BinaryOp{
      private Object metaData = null;
    
      internal Relational(Context context, AST operand1, AST operand2, JSToken operatorTok)
        : base(context, operand1, operand2, operatorTok) {
      }
    
      public Relational(int operatorTok)
        : base(null, null, null, (JSToken)operatorTok){
      }
      
      internal override Object Evaluate(){
        Object v1 = this.operand1.Evaluate();
        Object v2 = this.operand2.Evaluate();
        double result = this.EvaluateRelational(v1, v2);
        switch (this.operatorTok){
          case JSToken.GreaterThan:  
            return result > 0;
          case JSToken.GreaterThanEqual:
            return result >= 0;
          case JSToken.LessThan:
            return result < 0;
          case JSToken.LessThanEqual:
            return result <= 0;
          default:
            throw new JScriptException(JSError.InternalError, this.context);
        }
      }
      
#if !DEBUG
      [DebuggerStepThroughAttribute]
      [DebuggerHiddenAttribute]
#endif      
      public double EvaluateRelational(Object v1, Object v2){
        if (v1 is Int32){
          if (v2 is Int32)
            return ((double)(int)v1) - (double)(int)v2;
          else if (v2 is Double)
            return ((Int32)v1) - (Double)v2;
        }else if (v1 is Double){
          if (v2 is Double){
            double d1 = (Double)v1;
            double d2 = (Double)v2;
            if (d1 == d2) return 0; //d1 and d2 could be infinities
            return d1 - d2;
          }else if (v2 is Int32)
            return ((Double)v1) - (Int32)v2;
        }
        IConvertible ic1 = Convert.GetIConvertible(v1);
        IConvertible ic2 = Convert.GetIConvertible(v2);
        TypeCode t1 = Convert.GetTypeCode(v1, ic1);
        TypeCode t2 = Convert.GetTypeCode(v2, ic2);
        if (t1 == TypeCode.Object && t2 == TypeCode.Object){
          MethodInfo oper = this.GetOperator(v1.GetType(), v2.GetType());
          if (oper != null){
            bool result = Convert.ToBoolean(oper.Invoke(null, (BindingFlags)0, JSBinder.ob, new Object[]{v1, v2}, null));
            switch (this.operatorTok){
              case JSToken.GreaterThan:  
              case JSToken.GreaterThanEqual:
                return result ? 1 : -1;
              case JSToken.LessThan:
              case JSToken.LessThanEqual:
                return result ? -1 : 1;
              default:
                throw new JScriptException(JSError.InternalError, this.context);
            }
          }
        }
        return JScriptCompare2(v1, v2, ic1, ic2, t1, t2);
      }

      internal override IReflect InferType(JSField inference_target){
        return Typeob.Boolean;
      }
    
      public static double JScriptCompare(Object v1, Object v2){
        if (v1 is Int32){
          if (v2 is Int32)
            return ((Int32)v1) - (Int32)v2;
          else if (v2 is Double)
            return ((Int32)v1) - (Double)v2;
        }else if (v1 is Double){
          if (v2 is Double){
            double d1 = (Double)v1;
            double d2 = (Double)v2;
            if (d1 == d2) return 0; //d1 and d2 could be infinities
            return d1 - d2;
          }else if (v2 is Int32)
            return ((Double)v1) - (Int32)v2;
        }
        IConvertible ic1 = Convert.GetIConvertible(v1);
        IConvertible ic2 = Convert.GetIConvertible(v2);
        TypeCode t1 = Convert.GetTypeCode(v1, ic1);
        TypeCode t2 = Convert.GetTypeCode(v2, ic2);
        return JScriptCompare2(v1, v2, ic1, ic2, t1, t2);
      }
      
      private static double JScriptCompare2(Object v1, Object v2, IConvertible ic1, IConvertible ic2, TypeCode t1, TypeCode t2){
        if (t1 == TypeCode.Object){
          v1 = Convert.ToPrimitive(v1, PreferredType.Number, ref ic1);
          t1 = Convert.GetTypeCode(v1, ic1);
        }
        if (t2 == TypeCode.Object){
          v2 = Convert.ToPrimitive(v2, PreferredType.Number, ref ic2);
          t2 = Convert.GetTypeCode(v2, ic2);
        }
        switch (t1){
          case TypeCode.Char:
            if (t2 == TypeCode.String) return String.CompareOrdinal(Convert.ToString(v1, ic1), ic2.ToString(null));
            goto case TypeCode.UInt16;

          case TypeCode.SByte:
          case TypeCode.Byte:
          case TypeCode.Int16:
          case TypeCode.UInt16:
          case TypeCode.Int32:
          case TypeCode.UInt32:
          case TypeCode.Int64:
            long l = ic1.ToInt64(null);
            switch (t2){
              case TypeCode.SByte:
              case TypeCode.Byte:
              case TypeCode.Int16:
              case TypeCode.UInt16:
              case TypeCode.Int32:
              case TypeCode.UInt32:
              case TypeCode.Int64:
                return l - ic2.ToInt64(null);
              case TypeCode.UInt64:
                if (l < 0) return -1;
                ulong ul2 = ic2.ToUInt64(null);
                if (((ulong)l) < ul2) return -1;
                if (((ulong)l) == ul2) return 0;
                return 1;
              case TypeCode.Single:
              case TypeCode.Double:
                return ((double)l) - ic2.ToDouble(null);
              case TypeCode.Decimal:
                return (double)(new Decimal(l) - ic2.ToDecimal(null));
              default:
                Object bd2 = Convert.ToNumber(v2, ic2);
                return JScriptCompare2(v1, bd2, ic1, Convert.GetIConvertible(bd2), t1, TypeCode.Double);
            }

          case TypeCode.UInt64:
            ulong ul = ic1.ToUInt64(null);
            switch (t2){
              case TypeCode.SByte:
              case TypeCode.Byte:
              case TypeCode.Int16:
              case TypeCode.UInt16:
              case TypeCode.Int32:
              case TypeCode.UInt32:
              case TypeCode.Int64:
                long l2 = ic2.ToInt64(null);
                if (l2 < 0) return 1;
                if (ul == (ulong)l2) return 0;
                return -1;
              case TypeCode.UInt64:
                ulong ul2 = ic2.ToUInt64(null);
                if (ul < ul2) return -1;
                if (ul == ul2) return 0;
                return 1;
              case TypeCode.Single:
              case TypeCode.Double:
                return ((double)ul) - ic2.ToDouble(null);
              case TypeCode.Decimal:
                return (double)(new Decimal(ul) - ic2.ToDecimal(null));
              default:
                Object bd2 = Convert.ToNumber(v2, ic2);
                return JScriptCompare2(v1, bd2, ic1, Convert.GetIConvertible(bd2), t1, TypeCode.Double);
            }

          case TypeCode.Decimal:
            Decimal dec1 = ic1.ToDecimal(null);
            switch (t2){
              case TypeCode.SByte:
              case TypeCode.Byte:
              case TypeCode.Int16:
              case TypeCode.UInt16:
              case TypeCode.Int32:
              case TypeCode.UInt32:
              case TypeCode.Int64:
                return (double)(dec1 - new Decimal(ic2.ToInt64(null)));
              case TypeCode.UInt64:
                return (double)(dec1 - new Decimal(ic2.ToUInt64(null)));
              case TypeCode.Single:
              case TypeCode.Double:
                return (double)(dec1 - new Decimal(ic2.ToDouble(null)));
              case TypeCode.Decimal:
                return (double)(dec1 - ic2.ToDecimal(null));
              default:
                return (double)(dec1 - new Decimal(Convert.ToNumber(v2, ic2)));
            }

          case TypeCode.String:
            switch (t2){
              case TypeCode.String: return String.CompareOrdinal(ic1.ToString(null), ic2.ToString(null));
              case TypeCode.Char : return String.CompareOrdinal(ic1.ToString(null), Convert.ToString(v2, ic2));
            }
            goto default;

          default:
            double d1 = Convert.ToNumber(v1, ic1);
            double d2 = Convert.ToNumber(v2, ic2);
            if (d1 == d2) return 0; //d1 and d2 could be infinities
            return d1 - d2;
        }
      }
          
      internal override void TranslateToConditionalBranch(ILGenerator il, bool branchIfTrue, Label label, bool shortForm){
        Type t1 = this.type1;
        Type t2 = this.type2;
        Type t3 = Typeob.Object;
        if (t1.IsPrimitive && t2.IsPrimitive){
          t3 = Typeob.Double;
          if (Convert.IsPromotableTo(t1, t2))
            t3 = t2;
          else if (Convert.IsPromotableTo(t2, t1))
            t3 = t1;
          else if (t1 == Typeob.Int64 || t1 == Typeob.UInt64 || t2 == Typeob.Int64 || t2 == Typeob.UInt64)
            t3 = Typeob.Object; //Cannot convert these to double without loss of precision in some cases, so use the helper method
        }
        if (t3 == Typeob.SByte || t3 == Typeob.Int16)
          t3 = Typeob.Int32;
        else if (t3 == Typeob.Byte || t3 == Typeob.UInt16)
          t3 = Typeob.UInt32;
        if (this.metaData == null){
          this.operand1.TranslateToIL(il, t3);
          this.operand2.TranslateToIL(il, t3);
          if (t3 == Typeob.Object){
            il.Emit(OpCodes.Call, CompilerGlobals.jScriptCompareMethod);
            il.Emit(OpCodes.Ldc_I4_0);
            il.Emit(OpCodes.Conv_R8);
            t3 = Typeob.Double;
          }
        }else if (this.metaData is MethodInfo){
          MethodInfo oper = (MethodInfo)this.metaData;
          ParameterInfo[] pars = oper.GetParameters();
          this.operand1.TranslateToIL(il, pars[0].ParameterType);
          this.operand2.TranslateToIL(il, pars[1].ParameterType);
          il.Emit(OpCodes.Call, oper);
          if (branchIfTrue)
            il.Emit(shortForm ? OpCodes.Brtrue_S : OpCodes.Brtrue, label);
          else
            il.Emit(shortForm ? OpCodes.Brfalse_S : OpCodes.Brfalse, label);
          return;
        }else{
          //Getting here is just too bad. We do not know until the code runs whether or not to call an overloaded operator method.
          //Compile operands to objects and devolve the decision making to run time thunks 
          il.Emit(OpCodes.Ldloc, (LocalBuilder)this.metaData);
          this.operand1.TranslateToIL(il, Typeob.Object);
          this.operand2.TranslateToIL(il, Typeob.Object);
          il.Emit(OpCodes.Call, CompilerGlobals.evaluateRelationalMethod);
          il.Emit(OpCodes.Ldc_I4_0);
          il.Emit(OpCodes.Conv_R8);
          t3 = Typeob.Double;
        }
        if (branchIfTrue){
          if (t3 == Typeob.UInt32 || t3 == Typeob.UInt64)
            switch (this.operatorTok){
              case JSToken.GreaterThan:  
                il.Emit(shortForm ? OpCodes.Bgt_Un_S : OpCodes.Bgt_Un, label); break;
              case JSToken.GreaterThanEqual:
                il.Emit(shortForm ? OpCodes.Bge_Un_S : OpCodes.Bge_Un, label); break;
              case JSToken.LessThan:
                il.Emit(shortForm ? OpCodes.Blt_Un_S : OpCodes.Blt_Un, label); break;
              case JSToken.LessThanEqual:
                il.Emit(shortForm ? OpCodes.Ble_Un_S : OpCodes.Ble_Un, label); break;
              default:
                throw new JScriptException(JSError.InternalError, this.context);
            }
          else
            switch (this.operatorTok){
              case JSToken.GreaterThan:  
                il.Emit(shortForm ? OpCodes.Bgt_S : OpCodes.Bgt, label); break;
              case JSToken.GreaterThanEqual:
                il.Emit(shortForm ? OpCodes.Bge_S : OpCodes.Bge, label); break;
              case JSToken.LessThan:
                il.Emit(shortForm ? OpCodes.Blt_S : OpCodes.Blt, label); break;
              case JSToken.LessThanEqual:
                il.Emit(shortForm ? OpCodes.Ble_S : OpCodes.Ble, label); break;
              default:
                throw new JScriptException(JSError.InternalError, this.context);
            }
        }else{
          if (t3 == Typeob.Int32 || t3 == Typeob.Int64)
            switch (this.operatorTok){
              case JSToken.GreaterThan:
                il.Emit(shortForm ? OpCodes.Ble_S : OpCodes.Ble, label); break;
              case JSToken.GreaterThanEqual:
                il.Emit(shortForm ? OpCodes.Blt_S : OpCodes.Blt, label); break;
              case JSToken.LessThan:
                il.Emit(shortForm ? OpCodes.Bge_S : OpCodes.Bge, label); break;
              case JSToken.LessThanEqual:
                il.Emit(shortForm ? OpCodes.Bgt_S : OpCodes.Bgt, label); break;
              default:
                throw new JScriptException(JSError.InternalError, this.context);
            }
          else
            switch (this.operatorTok){
              case JSToken.GreaterThan:
                il.Emit(shortForm ? OpCodes.Ble_Un_S : OpCodes.Ble_Un, label); break;
              case JSToken.GreaterThanEqual:
                il.Emit(shortForm ? OpCodes.Blt_Un_S : OpCodes.Blt_Un, label); break;
              case JSToken.LessThan:
                il.Emit(shortForm ? OpCodes.Bge_Un_S : OpCodes.Bge_Un, label); break;
              case JSToken.LessThanEqual:
                il.Emit(shortForm ? OpCodes.Bgt_Un_S : OpCodes.Bgt_Un, label); break;
              default:
                throw new JScriptException(JSError.InternalError, this.context);
            }
        }
      }
      
      internal override void TranslateToIL(ILGenerator il, Type rtype){
        Label true_label = il.DefineLabel();
        Label done_label = il.DefineLabel();
        this.TranslateToConditionalBranch(il, true, true_label, true);
        il.Emit(OpCodes.Ldc_I4_0);
        il.Emit(OpCodes.Br_S, done_label);
        il.MarkLabel(true_label);
        il.Emit(OpCodes.Ldc_I4_1);
        il.MarkLabel(done_label);
        Convert.Emit(this, il, Typeob.Boolean, rtype);
      }
    
      internal override void TranslateToILInitializer(ILGenerator il){
        this.operand1.TranslateToILInitializer(il);
        this.operand2.TranslateToILInitializer(il);
        MethodInfo oper = this.GetOperator(this.operand1.InferType(null), this.operand2.InferType(null));
        if (oper != null){
          this.metaData = oper;
          return;
        }
        if ((this.type1.IsPrimitive || Typeob.JSObject.IsAssignableFrom(this.type1)) 
        &&  (this.type2.IsPrimitive || Typeob.JSObject.IsAssignableFrom(this.type2)))
          return;
        this.metaData = il.DeclareLocal(Typeob.Relational);
        ConstantWrapper.TranslateToILInt(il, (int)this.operatorTok);
        il.Emit(OpCodes.Newobj, CompilerGlobals.relationalConstructor);
        il.Emit(OpCodes.Stloc, (LocalBuilder)this.metaData);
      }
    }
}

