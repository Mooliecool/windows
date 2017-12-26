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
    
    public sealed class BitwiseBinary : BinaryOp{
      private Object metaData = null;
    
      internal BitwiseBinary(Context context, AST operand1, AST operand2, JSToken operatorTok)
        : base(context, operand1, operand2, operatorTok) {
      }
      
      public BitwiseBinary(int operatorTok)
        : base(null, null, null, (JSToken)operatorTok){
      }
      
      internal override Object Evaluate(){
        return this.EvaluateBitwiseBinary(this.operand1.Evaluate(), this.operand2.Evaluate());
      }

#if !DEBUG
      [DebuggerStepThroughAttribute]
      [DebuggerHiddenAttribute]
#endif      
      public Object EvaluateBitwiseBinary(Object v1, Object v2){
        if (v1 is Int32 && v2 is Int32)
          return DoOp((Int32)v1, (Int32)v2, this.operatorTok);
        else
          return EvaluateBitwiseBinary(v1, v2, this.operatorTok);
      }
      
#if !DEBUG
      [DebuggerStepThroughAttribute]
      [DebuggerHiddenAttribute]
#endif      
      private Object EvaluateBitwiseBinary(Object v1, Object v2, JSToken operatorTok){
        IConvertible ic1 = Convert.GetIConvertible(v1);
        IConvertible ic2 = Convert.GetIConvertible(v2);
        TypeCode t1 = Convert.GetTypeCode(v1, ic1);
        TypeCode t2 = Convert.GetTypeCode(v2, ic2);
        switch (t1){
          case TypeCode.Empty: 
          case TypeCode.DBNull:
            return EvaluateBitwiseBinary(0, v2, operatorTok);
            
          case TypeCode.Boolean:
          case TypeCode.Char:
          case TypeCode.SByte:
          case TypeCode.Byte:
          case TypeCode.Int16:
          case TypeCode.UInt16:
          case TypeCode.Int32:
            int i = ic1.ToInt32(null);
            switch (t2){
              case TypeCode.Empty: 
              case TypeCode.DBNull:
                return DoOp(i, 0, operatorTok);
              case TypeCode.Boolean:
              case TypeCode.Char:
              case TypeCode.SByte:
              case TypeCode.Byte:
              case TypeCode.Int16:
              case TypeCode.UInt16:
              case TypeCode.Int32:
                return DoOp(i, ic2.ToInt32(null), operatorTok);
              case TypeCode.UInt32:
              case TypeCode.Int64:
              case TypeCode.UInt64:
              case TypeCode.Single:
              case TypeCode.Double:
                return DoOp(i, (int)Runtime.DoubleToInt64(ic2.ToDouble(null)), operatorTok);
              case TypeCode.Object: 
              case TypeCode.Decimal:
              case TypeCode.DateTime: 
              case TypeCode.String:
                break; 
            }
            break;
            
          case TypeCode.UInt32:
          case TypeCode.Int64:
          case TypeCode.UInt64:
          case TypeCode.Single:
          case TypeCode.Double:
            i = (int)Runtime.DoubleToInt64(ic1.ToDouble(null));
            switch (t2){
              case TypeCode.Empty: 
              case TypeCode.DBNull:
                return DoOp(i, 0, operatorTok);
              case TypeCode.Boolean:
              case TypeCode.Char:
              case TypeCode.SByte:
              case TypeCode.Byte:
              case TypeCode.Int16:
              case TypeCode.UInt16:
              case TypeCode.Int32:
                return DoOp(i, ic2.ToInt32(null), operatorTok);
              case TypeCode.UInt32:
              case TypeCode.Int64:
              case TypeCode.UInt64:
              case TypeCode.Single:
              case TypeCode.Double:
                return DoOp(i, (int)Runtime.DoubleToInt64(ic2.ToDouble(null)), operatorTok);
              case TypeCode.Object: 
              case TypeCode.Decimal:
              case TypeCode.DateTime: 
              case TypeCode.String:
                break; 
            }
            break;
            
          case TypeCode.Object: 
          case TypeCode.Decimal:
          case TypeCode.DateTime: 
          case TypeCode.String:
            break;
        }
        if (v2 == null) return DoOp(Convert.ToInt32(v1), 0, this.operatorTok);
        MethodInfo oper = this.GetOperator(v1.GetType(), v2.GetType());
        if (oper != null)
          return oper.Invoke(null, (BindingFlags)0, JSBinder.ob, new Object[]{v1, v2}, null);
        else
          return DoOp(Convert.ToInt32(v1), Convert.ToInt32(v2), this.operatorTok);
      }
      
      internal static Object DoOp(int i, int j, JSToken operatorTok){
        switch (operatorTok){
          case JSToken.BitwiseAnd:  
            return i & j;
          case JSToken.BitwiseOr:
            return i | j;
          case JSToken.BitwiseXor:
            return i ^ j;
          case JSToken.LeftShift:
            return i << j;
          case JSToken.RightShift:
            return i >> j;
          case JSToken.UnsignedRightShift:
            return ((uint)i) >> j;
          default:
            throw new JScriptException(JSError.InternalError);
        }
      }
    
      internal override IReflect InferType(JSField inference_target){
        MethodInfo oper;
        if (this.type1 == null || inference_target != null){
          oper = this.GetOperator(this.operand1.InferType(inference_target), this.operand2.InferType(inference_target));
        }else
          oper = this.GetOperator(this.type1, this.type2);
        if (oper != null){
          this.metaData = oper;
          return oper.ReturnType;
        }
        return BitwiseBinary.ResultType(this.type1, this.type2, this.operatorTok);
      }

      internal static Type Operand2Type(JSToken operatorTok, Type bbrType){
        switch (operatorTok){
          case JSToken.LeftShift:
          case JSToken.RightShift:
          case JSToken.UnsignedRightShift:
            return Typeob.Int32;
        }
        return bbrType;
      }
      
      internal static Type ResultType(Type type1, Type type2, JSToken operatorTok){
        switch (operatorTok){
          case JSToken.LeftShift:
          case JSToken.RightShift:
            if (Convert.IsPrimitiveIntegerType(type1))
              return type1;
            else if (Typeob.JSObject.IsAssignableFrom(type1)) 
              return Typeob.Int32;
            else
              return Typeob.Object;
          case JSToken.UnsignedRightShift:
            switch(Type.GetTypeCode(type1)){
              case TypeCode.Byte:
              case TypeCode.SByte: return Typeob.Byte;
              case TypeCode.UInt16:
              case TypeCode.Int16: return Typeob.UInt16;
              case TypeCode.Int32:
              case TypeCode.UInt32: return Typeob.UInt32;
              case TypeCode.Int64:
              case TypeCode.UInt64: return Typeob.UInt64;
              default:
                if (Typeob.JSObject.IsAssignableFrom(type1)) 
                  return Typeob.Int32;
                else
                  return Typeob.Object;
            }
        }
        TypeCode t1 = Type.GetTypeCode(type1);
        TypeCode t2 = Type.GetTypeCode(type2);
        switch (t1){
          case TypeCode.Empty: 
          case TypeCode.DBNull:
          case TypeCode.Boolean:
            switch (t2){
              case TypeCode.SByte:
                return Typeob.SByte;
              case TypeCode.Byte:
                return Typeob.Byte;
              case TypeCode.Char:
              case TypeCode.UInt16:
                return Typeob.UInt16;
              case TypeCode.Int16:
                return Typeob.Int16;
              case TypeCode.Empty: 
              case TypeCode.DBNull:
              case TypeCode.Boolean:
              case TypeCode.Int32:
              case TypeCode.Single:
              case TypeCode.Double:
                return Typeob.Int32;
              case TypeCode.UInt32:
                return Typeob.UInt32;
              case TypeCode.Int64:
                return Typeob.Int64;
              case TypeCode.UInt64:
                return Typeob.UInt64;
              case TypeCode.Object: 
                if (Typeob.JSObject.IsAssignableFrom(type2)) return Typeob.Int32;
                break;
              case TypeCode.Decimal:
              case TypeCode.DateTime: 
              case TypeCode.String:
                break; 
            }
            break;            

          case TypeCode.SByte:
            switch (t2){
              case TypeCode.Empty: 
              case TypeCode.DBNull:
              case TypeCode.Boolean:
              case TypeCode.SByte:
                return Typeob.SByte;
              case TypeCode.Byte:
                return Typeob.Byte;
              case TypeCode.Char:
              case TypeCode.Int16:
                return Typeob.Int16;
              case TypeCode.UInt16:
                return Typeob.UInt16;
              case TypeCode.Int32:
              case TypeCode.Single:
              case TypeCode.Double:
                return Typeob.Int32;
              case TypeCode.UInt32:
                return Typeob.UInt32;
              case TypeCode.Int64:
                return Typeob.Int64;
              case TypeCode.UInt64:
                return Typeob.UInt64;
              case TypeCode.Object: 
                if (Typeob.JSObject.IsAssignableFrom(type2)) return Typeob.Int32;
                break;
              case TypeCode.Decimal:
              case TypeCode.DateTime: 
              case TypeCode.String:
                break; 
            }
            break;
            
          case TypeCode.Byte:
            switch (t2){
              case TypeCode.Empty: 
              case TypeCode.DBNull:
              case TypeCode.Boolean:
              case TypeCode.Byte:
              case TypeCode.SByte:
                return Typeob.Byte;
              case TypeCode.Char:
              case TypeCode.UInt16:
              case TypeCode.Int16:
                return Typeob.UInt16;
              case TypeCode.Int32:
              case TypeCode.Single:
              case TypeCode.Double:
              case TypeCode.UInt32:
                return Typeob.UInt32;
              case TypeCode.Int64:
              case TypeCode.UInt64:
                return Typeob.UInt64;
              case TypeCode.Object: 
                if (Typeob.JSObject.IsAssignableFrom(type2)) return Typeob.UInt32;
                break;
              case TypeCode.Decimal:
              case TypeCode.DateTime: 
              case TypeCode.String:
                break; 
            }
            break;            

          case TypeCode.Int16:
            switch (t2){
              case TypeCode.Empty: 
              case TypeCode.DBNull:
              case TypeCode.Boolean:
              case TypeCode.SByte:
              case TypeCode.Int16:
                return Typeob.Int16;
              case TypeCode.Byte:
              case TypeCode.Char:
              case TypeCode.UInt16:
                return Typeob.UInt16;
              case TypeCode.Int32:
              case TypeCode.Single:
              case TypeCode.Double:
                return Typeob.Int32;
              case TypeCode.UInt32:
                return Typeob.UInt32;
              case TypeCode.Int64:
                return Typeob.Int64;
              case TypeCode.UInt64:
                return Typeob.UInt64;
              case TypeCode.Object: 
                if (Typeob.JSObject.IsAssignableFrom(type2)) return Typeob.Int32;
                break;
              case TypeCode.Decimal:
              case TypeCode.DateTime: 
              case TypeCode.String:
                break; 
            }
            break;
            
          case TypeCode.Char:
          case TypeCode.UInt16:
            switch (t2){
              case TypeCode.Empty: 
              case TypeCode.DBNull:
              case TypeCode.Boolean:
              case TypeCode.Byte:
              case TypeCode.Char:
              case TypeCode.UInt16:
              case TypeCode.SByte:
              case TypeCode.Int16:
                return Typeob.UInt16;
              case TypeCode.Int32:
              case TypeCode.Single:
              case TypeCode.Double:
              case TypeCode.UInt32:
                return Typeob.UInt32;
              case TypeCode.Int64:
              case TypeCode.UInt64:
                return Typeob.UInt64;
              case TypeCode.Object: 
                if (Typeob.JSObject.IsAssignableFrom(type2)) return Typeob.UInt32;
                break;
              case TypeCode.Decimal:
              case TypeCode.DateTime: 
              case TypeCode.String:
                break; 
            }
            break;            

          case TypeCode.Int32:
          case TypeCode.Single:
          case TypeCode.Double:
            switch (t2){
              case TypeCode.Empty: 
              case TypeCode.DBNull:
              case TypeCode.Boolean:
              case TypeCode.SByte:
              case TypeCode.Int16:
              case TypeCode.Int32:
              case TypeCode.Single:
              case TypeCode.Double:
                return Typeob.Int32;
              case TypeCode.Byte:
              case TypeCode.Char:
              case TypeCode.UInt16:
              case TypeCode.UInt32:
                return Typeob.UInt32;
              case TypeCode.Int64:
                return Typeob.Int64;
              case TypeCode.UInt64:
                return Typeob.UInt64;
              case TypeCode.Object: 
                if (Typeob.JSObject.IsAssignableFrom(type2)) return Typeob.Int32;
                break;
              case TypeCode.Decimal:
              case TypeCode.DateTime: 
              case TypeCode.String:
                break; 
            }
            break;
            
          case TypeCode.UInt32:
            switch (t2){
              case TypeCode.Empty: 
              case TypeCode.DBNull:
              case TypeCode.Boolean:
              case TypeCode.Byte:
              case TypeCode.UInt16:
              case TypeCode.Char:
              case TypeCode.UInt32:
              case TypeCode.SByte:
              case TypeCode.Int16:
              case TypeCode.Int32:
              case TypeCode.Single:
              case TypeCode.Double:
                return Typeob.UInt32;
              case TypeCode.Int64:
              case TypeCode.UInt64:
                return Typeob.UInt64;
              case TypeCode.Object: 
                if (Typeob.JSObject.IsAssignableFrom(type2)) return Typeob.UInt32;
                break;
              case TypeCode.Decimal:
              case TypeCode.DateTime: 
              case TypeCode.String:
                break; 
            }
            break;

          case TypeCode.Int64:
            switch (t2){
              case TypeCode.Empty: 
              case TypeCode.DBNull:
              case TypeCode.Boolean:
              case TypeCode.SByte:
              case TypeCode.Int16:
              case TypeCode.Int32:
              case TypeCode.Int64:
              case TypeCode.Single:
              case TypeCode.Double:
                return Typeob.Int64;
              case TypeCode.Byte:
              case TypeCode.Char:
              case TypeCode.UInt16:
              case TypeCode.UInt32:
              case TypeCode.UInt64:
                return Typeob.UInt64;
              case TypeCode.Object: 
                if (Typeob.JSObject.IsAssignableFrom(type2)) return Typeob.Int64;
                break;
              case TypeCode.Decimal:
              case TypeCode.DateTime: 
              case TypeCode.String:
                break; 
            }
            break;

          case TypeCode.UInt64:
            switch (t2){
              case TypeCode.Empty: 
              case TypeCode.DBNull:
              case TypeCode.Boolean:
              case TypeCode.Byte:
              case TypeCode.Char:
              case TypeCode.UInt16:
              case TypeCode.UInt32:
              case TypeCode.UInt64:
              case TypeCode.SByte:
              case TypeCode.Int16:
              case TypeCode.Int32:
              case TypeCode.Single:
              case TypeCode.Double:
              case TypeCode.Int64:
                return Typeob.UInt64;
              case TypeCode.Object: 
                if (Typeob.JSObject.IsAssignableFrom(type2)) return Typeob.UInt64;
                break;
              case TypeCode.Decimal:
              case TypeCode.DateTime: 
              case TypeCode.String:
                break; 
            }
            break;
            
          case TypeCode.Object: 
            if (Typeob.JSObject.IsAssignableFrom(type1)) return Typeob.Int32;
            break;

          case TypeCode.Decimal:
          case TypeCode.DateTime: 
          case TypeCode.String:
            break;
        }
        return Typeob.Object;
      }
 
      internal static void TranslateToBitCountMask(ILGenerator il, Type type, AST operand2){
        int mask = 0;
        switch (Type.GetTypeCode(type)){
          case TypeCode.SByte:
          case TypeCode.Byte:
            mask = 7; break;
          case TypeCode.UInt16:
          case TypeCode.Int16:
            mask = 15; break;
          case TypeCode.Int32:
          case TypeCode.UInt32:
            mask = 31; break;
          case TypeCode.Int64:
          case TypeCode.UInt64:
            mask = 63; break;
        }
        ConstantWrapper cw = operand2 as ConstantWrapper;
        if (cw != null){
          int m = Convert.ToInt32(cw.value);
          if (m <= mask) return;
        }
        il.Emit(OpCodes.Ldc_I4_S, mask);
        il.Emit(OpCodes.And);
      }

      internal override void TranslateToIL(ILGenerator il, Type rtype){
        if (this.metaData == null){
          Type bbrType = BitwiseBinary.ResultType(this.type1, this.type2, this.operatorTok);
          if (Convert.IsPrimitiveNumericType(this.type1)){
            this.operand1.TranslateToIL(il, this.type1);
            Convert.Emit(this, il, this.type1, bbrType, true);
          }else{
            this.operand1.TranslateToIL(il, Typeob.Double);
            Convert.Emit(this, il, Typeob.Double, bbrType, true);
          }
          Type op2type = BitwiseBinary.Operand2Type(this.operatorTok, bbrType);
          if (Convert.IsPrimitiveNumericType(this.type2)){
            this.operand2.TranslateToIL(il, this.type2);
            Convert.Emit(this, il, this.type2, op2type, true);
          }else{
            this.operand2.TranslateToIL(il, Typeob.Double);
            Convert.Emit(this, il, Typeob.Double, op2type, true);
          }
          switch (this.operatorTok){
            case JSToken.BitwiseAnd:  
              il.Emit(OpCodes.And);
              break;
            case JSToken.BitwiseOr:
              il.Emit(OpCodes.Or);
              break;
            case JSToken.BitwiseXor:
              il.Emit(OpCodes.Xor);
              break;
            case JSToken.LeftShift:
              BitwiseBinary.TranslateToBitCountMask(il, bbrType, this.operand2);
              il.Emit(OpCodes.Shl);
              break;
            case JSToken.RightShift:
              BitwiseBinary.TranslateToBitCountMask(il, bbrType, this.operand2);
              il.Emit(OpCodes.Shr);
              break;
            case JSToken.UnsignedRightShift:
              BitwiseBinary.TranslateToBitCountMask(il, bbrType, this.operand2);
              il.Emit(OpCodes.Shr_Un);
              break;
            default:
              throw new JScriptException(JSError.InternalError, this.context);
          }
          Convert.Emit(this, il, bbrType, rtype);
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
        il.Emit(OpCodes.Call, CompilerGlobals.evaluateBitwiseBinaryMethod);
        Convert.Emit(this, il, Typeob.Object, rtype);
      }

      internal override void TranslateToILInitializer(ILGenerator il){
        IReflect rtype = this.InferType(null);
        this.operand1.TranslateToILInitializer(il);
        this.operand2.TranslateToILInitializer(il);
        if (rtype != Typeob.Object)
          return;
        this.metaData = il.DeclareLocal(Typeob.BitwiseBinary);
        ConstantWrapper.TranslateToILInt(il, (int)this.operatorTok);
        il.Emit(OpCodes.Newobj, CompilerGlobals.bitwiseBinaryConstructor);
        il.Emit(OpCodes.Stloc, (LocalBuilder)this.metaData);
      }
    }
}
