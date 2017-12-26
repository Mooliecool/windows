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
    using System.Reflection;
    using System.Reflection.Emit;
    using System.Diagnostics;
    
    public class Equality : BinaryOp{
      private Object metaData = null;
    
      internal Equality(Context context, AST operand1, AST operand2, JSToken operatorTok)
        : base(context, operand1, operand2, operatorTok) {
      }
      
      public Equality(int operatorTok)
        : base(null, null, null, (JSToken)operatorTok){
      }
      
      internal override Object Evaluate(){
        bool result = this.EvaluateEquality(this.operand1.Evaluate(), this.operand2.Evaluate(), VsaEngine.executeForJSEE);
        if (this.operatorTok == JSToken.Equal)
          return result;
        else
          return !result;
      }

#if !DEBUG
      [DebuggerStepThroughAttribute]
      [DebuggerHiddenAttribute]
#endif      
      public bool EvaluateEquality(Object v1, Object v2){
        return EvaluateEquality(v1, v2, false);
      }

      
#if !DEBUG
      [DebuggerStepThroughAttribute]
      [DebuggerHiddenAttribute]
#endif      
      private bool EvaluateEquality(Object v1, Object v2, bool checkForDebuggerObjects){
        if (v1 is String && v2 is String) return v1.Equals(v2);
        if (v1 is Int32 && v2 is Int32) return ((int)v1) == (int)v2;
        if (v1 is Double && v2 is Double) return ((double)v1) == (double)v2;
        if ((v2 == null || v2 is DBNull || v2 is Missing) && !checkForDebuggerObjects) return (v1 == null || v1 is DBNull || v1 is Missing);
        IConvertible ic1 = Convert.GetIConvertible(v1);
        IConvertible ic2 = Convert.GetIConvertible(v2);
        TypeCode t1 = Convert.GetTypeCode(v1, ic1);
        TypeCode t2 = Convert.GetTypeCode(v2, ic2);
        switch(t1){
          case TypeCode.Empty:
          case TypeCode.DBNull: 
            break;
          case TypeCode.Object:
            switch(t2){
              case TypeCode.Empty:
              case TypeCode.DBNull: 
                break;
              default:
                MethodInfo oper = this.GetOperator(v1.GetType(), v2.GetType());
                if (oper != null){
                  bool result = (bool)oper.Invoke(null, (BindingFlags)0, JSBinder.ob, new Object[]{v1, v2}, null);
                  if (this.operatorTok == JSToken.NotEqual) return !result;
                  return result;
                }
                break;        
            }
            break;
          default:
            switch(t2){
              case TypeCode.Object:
                MethodInfo oper = this.GetOperator(v1.GetType(), v2.GetType());
                if (oper != null){
                  bool result = (bool)oper.Invoke(null, (BindingFlags)0, JSBinder.ob, new Object[]{v1, v2}, null);
                  if (this.operatorTok == JSToken.NotEqual) return !result;
                  return result;
                }
                break;
            }
            break;
        }
        return Equality.JScriptEquals(v1, v2, ic1, ic2, t1, t2, checkForDebuggerObjects);
      }


      
      public static bool JScriptEquals(Object v1, Object v2){
        if (v1 is String && v2 is String) return v1.Equals(v2);
        if (v1 is Int32 && v2 is Int32) return ((int)v1) == (int)v2;
        if (v1 is Double && v2 is Double) return ((double)v1) == (double)v2;
        if ((v2 == null || v2 is DBNull || v2 is Missing)) return (v1 == null || v1 is DBNull || v1 is Missing);
        IConvertible ic1 = Convert.GetIConvertible(v1);
        IConvertible ic2 = Convert.GetIConvertible(v2);
        TypeCode t1 = Convert.GetTypeCode(v1, ic1);
        TypeCode t2 = Convert.GetTypeCode(v2, ic2);
        return Equality.JScriptEquals(v1, v2, ic1, ic2, t1, t2, false);
      }

      private static bool JScriptEquals(Object v1, Object v2, IConvertible ic1, IConvertible ic2, TypeCode t1, TypeCode t2, bool checkForDebuggerObjects){
        if (StrictEquality.JScriptStrictEquals(v1, v2, ic1, ic2, t1, t2, checkForDebuggerObjects))
          return true;
        if (t2 == TypeCode.Boolean){
          v2 = ic2.ToBoolean(null) ? 1 : 0;
          ic2 = Convert.GetIConvertible(v2);
          return Equality.JScriptEquals(v1, v2, ic1, ic2, t1, TypeCode.Int32, false);
        }
        switch (t1){
          case TypeCode.Empty: return t2 == TypeCode.Empty || t2 == TypeCode.DBNull || (t2 == TypeCode.Object && v2 is Missing);
          case TypeCode.Object:
            switch (t2){
              case TypeCode.Empty:
              case TypeCode.DBNull:
                return v1 is Missing;
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
              case TypeCode.String:
                IConvertible pvic1 = ic1;
                Object pv1 = Convert.ToPrimitive(v1, PreferredType.Either, ref pvic1);
                if (pvic1 != null && pv1 != v1)
                  return Equality.JScriptEquals(pv1, v2, pvic1, ic2, pvic1.GetTypeCode(), t2, false);
                else
                  return false;
            }
            return false;
          case TypeCode.DBNull: return t2 == TypeCode.DBNull || t2 == TypeCode.Empty || (t2 == TypeCode.Object && v2 is Missing);
          case TypeCode.Boolean:
            v1 = ic1.ToBoolean(null) ? 1 : 0;
            ic1 = Convert.GetIConvertible(v1);
            return Equality.JScriptEquals(v1, v2, ic1, ic2, TypeCode.Int32, t2, false);
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
            if (t2 == TypeCode.Object){
              IConvertible pvic2 = ic2;
              Object pv2 = Convert.ToPrimitive(v2, PreferredType.Either, ref pvic2);
              if (pvic2 != null && pv2 != v2)
                return Equality.JScriptEquals(v1, pv2, ic1, pvic2, t1, pvic2.GetTypeCode(), false);
              else
                return false;
            }
            if (t2 == TypeCode.String){
              if (v1 is Enum) return Convert.ToString(v1).Equals(ic2.ToString(null));
              v2 = Convert.ToNumber(v2, ic2);
              ic2 = Convert.GetIConvertible(v2);
              return StrictEquality.JScriptStrictEquals(v1, v2, ic1, ic2, t1, TypeCode.Double, false);
            }
            return false;
          case TypeCode.DateTime:
            if (t2 == TypeCode.Object){
              IConvertible pvic2 = ic2;
              Object pv2 = Convert.ToPrimitive(v2, PreferredType.Either, ref pvic2);
              if (pv2 != null && pv2 != v2)
                return StrictEquality.JScriptStrictEquals(v1, pv2, ic1, pvic2, t1, pvic2.GetTypeCode(), false);
            }
            return false;
          case TypeCode.String:
            switch (t2){
              case TypeCode.Object:{
                IConvertible pvic2 = ic2;
                Object pv2 = Convert.ToPrimitive(v2, PreferredType.Either, ref pvic2);
                if (pvic2 != null && pv2 != v2)
                  return Equality.JScriptEquals(v1, pv2, ic1, pvic2, t1, pvic2.GetTypeCode(), false);
                else
                  return false;
              }
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
                if (v2 is Enum) return Convert.ToString(v2).Equals(ic1.ToString(null));
                v1 = Convert.ToNumber(v1, ic1);
                ic1 = Convert.GetIConvertible(v1);
                return StrictEquality.JScriptStrictEquals(v1, v2, ic1, ic2, TypeCode.Double, t2, false);
            }
            return false;
        }
        return false;
      }
    
      internal override IReflect InferType(JSField inference_target){
        return Typeob.Boolean;
      }
      
      internal override void TranslateToConditionalBranch(ILGenerator il, bool branchIfTrue, Label label, bool shortForm){
        if (this.metaData == null){
          Type t1 = this.type1;
          Type t2 = this.type2;
          Type t3 = Typeob.Object;
          bool emitNullAndUndefined = true;
          if (t1.IsPrimitive && t2.IsPrimitive){
            t3 = Typeob.Double;
            if (t1 == Typeob.Single || t2 == Typeob.Single)
              t3 = Typeob.Single;
            else if (Convert.IsPromotableTo(t1, t2))
              t3 = t2;
            else if (Convert.IsPromotableTo(t2, t1))
              t3 = t1;
          }else if (t1 == Typeob.String && (t2 == Typeob.String || t2 == Typeob.Empty || t2 == Typeob.Null)){
            t3 = Typeob.String;
            if (t2 != Typeob.String){
              emitNullAndUndefined = false;
              branchIfTrue = !branchIfTrue;
            }
          }else if ((t1 == Typeob.Empty || t1 == Typeob.Null) && t2 == Typeob.String){
            t3 = Typeob.String;
            emitNullAndUndefined = false;
            branchIfTrue = !branchIfTrue;
          }
          if (t3 == Typeob.SByte || t3 == Typeob.Int16)
            t3 = Typeob.Int32;
          else if (t3 == Typeob.Byte || t3 == Typeob.UInt16)
            t3 = Typeob.UInt32;
          if (emitNullAndUndefined){  
            this.operand1.TranslateToIL(il, t3);
            this.operand2.TranslateToIL(il, t3);
            if (t3 == Typeob.Object)
              il.Emit(OpCodes.Call, CompilerGlobals.jScriptEqualsMethod);
            else if (t3 == Typeob.String)
              il.Emit(OpCodes.Call, CompilerGlobals.stringEqualsMethod);
          }else if (t1 == Typeob.String)
            this.operand1.TranslateToIL(il, t3);
          else if (t2 == Typeob.String)
            this.operand2.TranslateToIL(il, t3);
          if (branchIfTrue){
            if (this.operatorTok == JSToken.Equal)
              if (t3 == Typeob.String || t3 == Typeob.Object)
                il.Emit(shortForm ? OpCodes.Brtrue_S : OpCodes.Brtrue, label);
              else
                il.Emit(shortForm ? OpCodes.Beq_S : OpCodes.Beq, label);
            else
              if (t3 == Typeob.String || t3 == Typeob.Object)
                il.Emit(shortForm ? OpCodes.Brfalse_S : OpCodes.Brfalse, label);
              else
                il.Emit(shortForm ? OpCodes.Bne_Un_S : OpCodes.Bne_Un, label);
          }else{
            if (this.operatorTok == JSToken.Equal)
              if (t3 == Typeob.String || t3 == Typeob.Object)
                il.Emit(shortForm ? OpCodes.Brfalse_S : OpCodes.Brfalse, label);
              else
                il.Emit(shortForm ? OpCodes.Bne_Un_S : OpCodes.Bne_Un, label);
            else
              if (t3 == Typeob.String || t3 == Typeob.Object)
                il.Emit(shortForm ? OpCodes.Brtrue_S : OpCodes.Brtrue, label);
              else
                il.Emit(shortForm ? OpCodes.Beq_S : OpCodes.Beq, label);
          }
          return;
        }
        if (this.metaData is MethodInfo){
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
        }
        //Getting here is just too bad. We do not know until the code runs whether or not to call an overloaded operator method.
        //Compile operands to objects and devolve the decision making to run time thunks 
        il.Emit(OpCodes.Ldloc, (LocalBuilder)this.metaData);
        this.operand1.TranslateToIL(il, Typeob.Object);
        this.operand2.TranslateToIL(il, Typeob.Object);
        il.Emit(OpCodes.Call, CompilerGlobals.evaluateEqualityMethod);
        if (branchIfTrue){
          if (this.operatorTok == JSToken.Equal)
            il.Emit(shortForm ? OpCodes.Brtrue_S : OpCodes.Brtrue, label);
          else
            il.Emit(shortForm ? OpCodes.Brfalse_S : OpCodes.Brfalse, label);
        }else{
          if (this.operatorTok == JSToken.Equal)
            il.Emit(shortForm ? OpCodes.Brfalse_S : OpCodes.Brfalse, label);
          else
            il.Emit(shortForm ? OpCodes.Brtrue_S : OpCodes.Brtrue, label);
        }
        return;
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
        if (this.operand1 is ConstantWrapper){
          Object val = this.operand1.Evaluate();
          if (val == null) this.type1 = Typeob.Empty;
          else if (val is DBNull) this.type1 = Typeob.Null;
        }
        if (this.operand2 is ConstantWrapper){
          Object val = this.operand2.Evaluate();
          if (val == null) this.type2 = Typeob.Empty;
          else if (val is DBNull) this.type2 = Typeob.Null;
        }
        if (this.type1 == Typeob.Empty || this.type1 == Typeob.Null || this.type2 == Typeob.Empty || this.type2 == Typeob.Null) return;
        if ((this.type1.IsPrimitive || this.type1 == Typeob.String || Typeob.JSObject.IsAssignableFrom(this.type1)) 
        &&  (this.type2.IsPrimitive || this.type2 == Typeob.String || Typeob.JSObject.IsAssignableFrom(this.type2)))
          return;
        this.metaData = il.DeclareLocal(Typeob.Equality);
        ConstantWrapper.TranslateToILInt(il, (int)this.operatorTok);
        il.Emit(OpCodes.Newobj, CompilerGlobals.equalityConstructor);
        il.Emit(OpCodes.Stloc, (LocalBuilder)this.metaData);
      }
    }
}
