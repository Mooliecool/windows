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
    
    public sealed class NumericUnary : UnaryOp{
      private Object metaData = null;
      private JSToken operatorTok;
      private MethodInfo operatorMeth;
      private Type type;
      
      internal NumericUnary(Context context, AST operand, JSToken operatorTok)
        : base(context, operand){
        this.operatorTok = operatorTok;
        this.operatorMeth = null;
        this.type = null;
      }
      
      public NumericUnary(int operatorTok)
        : this(null, null, (JSToken)operatorTok){
      }
      
      internal override Object Evaluate(){
        return this.EvaluateUnary(this.operand.Evaluate());
      }
      
#if !DEBUG
      [DebuggerStepThroughAttribute]
      [DebuggerHiddenAttribute]
#endif      
      public Object EvaluateUnary(Object v){
        IConvertible ic = Convert.GetIConvertible(v);
        switch(Convert.GetTypeCode(v, ic)){
          case TypeCode.Empty:  return this.EvaluateUnary(Double.NaN);
          case TypeCode.DBNull: return this.EvaluateUnary(0);
          case TypeCode.Boolean: return this.EvaluateUnary(ic.ToBoolean(null) ? 1 : 0);
          case TypeCode.Char: return this.EvaluateUnary((int)ic.ToChar(null));
          
          case TypeCode.SByte:
          case TypeCode.Byte: 
          case TypeCode.Int16:
          case TypeCode.UInt16: 
          case TypeCode.Int32:
            int i = ic.ToInt32(null);
            switch (this.operatorTok){
              case JSToken.BitwiseNot:
                return ~i;
              case JSToken.LogicalNot:
                return i == 0;
              case JSToken.Minus:
                if (i == 0) return -(double)i;
                if (i == Int32.MinValue) return (ulong)-(double)i;
                return -i;
              case JSToken.Plus:
                return i;
              default:
                throw new JScriptException(JSError.InternalError, this.context);
            }
            
          case TypeCode.UInt32: 
            uint ui = ic.ToUInt32(null);
            switch (this.operatorTok){
              case JSToken.BitwiseNot:
                return ~ui;
              case JSToken.LogicalNot:
                return ui == 0;
              case JSToken.Minus:
                if (ui != 0 && ui <= Int32.MaxValue)
                  return -(int)ui;
                else
                  return -(double)ui;
              case JSToken.Plus:
                return ui;
              default:
                throw new JScriptException(JSError.InternalError, this.context);
            }
            
          case TypeCode.Int64: 
            long l = ic.ToInt64(null);
            switch (this.operatorTok){
              case JSToken.BitwiseNot:
                return ~l;
              case JSToken.LogicalNot:
                return l == 0;
              case JSToken.Minus:
                if (l == 0 || l == Int64.MinValue) return -(double)l;
                return -l;
              case JSToken.Plus:
                return l;
              default:
                throw new JScriptException(JSError.InternalError, this.context);
            }
            
          case TypeCode.UInt64: 
            ulong ul = ic.ToUInt64(null);
            switch (this.operatorTok){
              case JSToken.BitwiseNot:
                return ~ul;
              case JSToken.LogicalNot:
                return ul == 0;
              case JSToken.Minus:
                if (ul != 0 && ul <= Int64.MaxValue)
                  return -(long)ul;
                else
                  return -(double)ul;
              case JSToken.Plus:
                return ul;
              default:
                throw new JScriptException(JSError.InternalError, this.context);
            }
            
          case TypeCode.Single:
          case TypeCode.Double:
            double d = ic.ToDouble(null);
            switch (this.operatorTok){
              case JSToken.BitwiseNot:
                return ~(int)Runtime.DoubleToInt64(d);
              case JSToken.LogicalNot:
                return !Convert.ToBoolean(d);
              case JSToken.Minus:
                return -d;
              case JSToken.Plus:
                return d;
              default:
                throw new JScriptException(JSError.InternalError, this.context);
            }
          
          case TypeCode.String:
            goto no_overload_case;
        }
        
        MethodInfo oper = this.GetOperator(v.GetType());
        if (oper != null)
          return oper.Invoke(null, (BindingFlags)0, JSBinder.ob, new Object[]{v}, null);
      no_overload_case:
        switch (this.operatorTok){
          case JSToken.BitwiseNot:
            return ~Convert.ToInt32(v, ic);
          case JSToken.LogicalNot:
            return !Convert.ToBoolean(v, ic);
          case JSToken.Minus:
            return -Convert.ToNumber(v, ic);
          case JSToken.Plus:
            return Convert.ToNumber(v, ic);
          default:
            throw new JScriptException(JSError.InternalError, this.context);
        }
      }
      
      private MethodInfo GetOperator(IReflect ir){
        Type t = ir is Type ? (Type)ir : Typeob.Object;
        if (this.type == t)
          return this.operatorMeth;
        this.type = t;
        if (Convert.IsPrimitiveNumericType(t) || Typeob.JSObject.IsAssignableFrom(t)){
          this.operatorMeth = null;
          return null;
        }
        switch (this.operatorTok){
          case JSToken.BitwiseNot:
            this.operatorMeth = t.GetMethod("op_OnesComplement", BindingFlags.Public|BindingFlags.Static, JSBinder.ob, new Type[]{t}, null); break;
          case JSToken.LogicalNot:
            this.operatorMeth = t.GetMethod("op_LogicalNot", BindingFlags.Public|BindingFlags.Static, JSBinder.ob, new Type[]{t}, null); break;
          case JSToken.Minus:
            this.operatorMeth = t.GetMethod("op_UnaryNegation", BindingFlags.Public|BindingFlags.Static, JSBinder.ob, new Type[]{t}, null); break;
          case JSToken.Plus:
            this.operatorMeth = t.GetMethod("op_UnaryPlus", BindingFlags.Public|BindingFlags.Static, JSBinder.ob, new Type[]{t}, null); break;
          default:
            throw new JScriptException(JSError.InternalError, this.context);
        }
        if (this.operatorMeth == null || 
        (operatorMeth.Attributes & MethodAttributes.SpecialName) == 0 ||
        operatorMeth.GetParameters().Length != 1)
          this.operatorMeth = null;
        if (this.operatorMeth != null)
          this.operatorMeth = new JSMethodInfo(this.operatorMeth);
        return this.operatorMeth;  
      }
      
      internal override IReflect InferType(JSField inference_target){
        Debug.Assert(Globals.TypeRefs.InReferenceContext(this.type));
        MethodInfo oper;
        if (this.type == null || inference_target != null){
          oper = this.GetOperator(this.operand.InferType(inference_target));
        }else
          oper = this.GetOperator(this.type);
        if (oper != null){
          this.metaData = oper;
          return oper.ReturnType;
        }
        if (this.operatorTok == JSToken.LogicalNot) return Typeob.Boolean;
        switch (Type.GetTypeCode(this.type)){
          case TypeCode.Empty: return this.operatorTok == JSToken.BitwiseNot ? Typeob.Int32 : Typeob.Double;
          case TypeCode.Object: return Typeob.Object;
          case TypeCode.DBNull: return Typeob.Int32;
          case TypeCode.Boolean: return Typeob.Int32;
          case TypeCode.SByte: return (this.operatorTok == JSToken.BitwiseNot) ? Typeob.SByte : Typeob.Int32;
          case TypeCode.Char: return Typeob.Int32;
          case TypeCode.Byte: return (this.operatorTok == JSToken.BitwiseNot) ? Typeob.Byte : Typeob.Int32;
          case TypeCode.Int16: return (this.operatorTok == JSToken.BitwiseNot) ? Typeob.Int16 : Typeob.Int32;
          case TypeCode.UInt16: return (this.operatorTok == JSToken.BitwiseNot) ? Typeob.UInt16 : Typeob.Int32;
          case TypeCode.Int32: return Typeob.Int32;
          case TypeCode.UInt32: return this.operatorTok == JSToken.Minus ? Typeob.Double : Typeob.UInt32;
          case TypeCode.Int64: return Typeob.Int64;
          case TypeCode.UInt64: return this.operatorTok == JSToken.Minus ? Typeob.Double : Typeob.UInt64;
          case TypeCode.Single:
          case TypeCode.Double:
          case TypeCode.String: return this.operatorTok == JSToken.BitwiseNot ? Typeob.Int32 : Typeob.Double;
        }
        if (Typeob.JSObject.IsAssignableFrom(this.type))
          return Typeob.Double;
        else
          return Typeob.Object;
      }
      
      internal override void TranslateToConditionalBranch(ILGenerator il, bool branchIfTrue, Label label, bool shortForm){
        if (this.operatorTok == JSToken.LogicalNot)
          this.operand.TranslateToConditionalBranch(il, !branchIfTrue, label, shortForm);
        else
          base.TranslateToConditionalBranch(il, branchIfTrue, label, shortForm);
      }
      
      internal override void TranslateToIL(ILGenerator il, Type rtype){
        if (this.metaData == null){
          Type rt = this.operatorTok == JSToken.LogicalNot ? Typeob.Boolean : Typeob.Double;
          if (Convert.IsPrimitiveNumericType(rtype) && Convert.IsPromotableTo(this.type, rtype))
            rt = rtype;
          if (this.operatorTok == JSToken.BitwiseNot && !Convert.IsPrimitiveIntegerType(rt)){
            rt = this.type;
            if (!Convert.IsPrimitiveIntegerType(rt))
              rt = Typeob.Int32;
          }
          this.operand.TranslateToIL(il, this.type);
          Convert.Emit(this, il, this.type, rt, true);
          switch (this.operatorTok){
            case JSToken.BitwiseNot:
              il.Emit(OpCodes.Not);
              break;
            case JSToken.LogicalNot:
              Convert.Emit(this, il, rt, Typeob.Boolean, true);
              rt = Typeob.Boolean;
              il.Emit(OpCodes.Ldc_I4_0);
              il.Emit(OpCodes.Ceq);
              break;
            case JSToken.Minus:
              il.Emit(OpCodes.Neg);
              break;
            case JSToken.Plus:
              break;
            default:
              throw new JScriptException(JSError.InternalError, this.context);
          }
          Convert.Emit(this, il, rt, rtype);
          return;
        }
        if (this.metaData is MethodInfo){
          MethodInfo oper = (MethodInfo)this.metaData;
          ParameterInfo[] pars = oper.GetParameters();
          this.operand.TranslateToIL(il, pars[0].ParameterType);
          il.Emit(OpCodes.Call, oper);
          Convert.Emit(this, il, oper.ReturnType, rtype);
          return;
        }
        //Getting here is just too bad. We do not know until the code runs whether or not to call an overloaded operator method.
        //Compile operands to objects and devolve the decision making to run time thunks 
        il.Emit(OpCodes.Ldloc, (LocalBuilder)this.metaData);
        this.operand.TranslateToIL(il, Typeob.Object);
        il.Emit(OpCodes.Call, CompilerGlobals.evaluateUnaryMethod);
        Convert.Emit(this, il, Typeob.Object, rtype);
      }
    
      internal override void TranslateToILInitializer(ILGenerator il){
        IReflect rtype = this.InferType(null);
        this.operand.TranslateToILInitializer(il);
        if (rtype != Typeob.Object)
          return;
        this.metaData = il.DeclareLocal(Typeob.NumericUnary);
        ConstantWrapper.TranslateToILInt(il, (int)this.operatorTok);
        il.Emit(OpCodes.Newobj, CompilerGlobals.numericUnaryConstructor);
        il.Emit(OpCodes.Stloc, (LocalBuilder)this.metaData);
      }
    
    }
}
