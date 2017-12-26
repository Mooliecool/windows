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
    
    internal enum PostOrPrefix{PostfixDecrement, PostfixIncrement, PrefixDecrement, PrefixIncrement};
    
    public class PostOrPrefixOperator : UnaryOp{
      private MethodInfo operatorMeth;
      private PostOrPrefix operatorTok;
      private Object metaData;
      private Type type;
      
      internal PostOrPrefixOperator(Context context, AST operand)
        : base(context, operand) {
      }
    
      internal PostOrPrefixOperator(Context context, AST operand, PostOrPrefix operatorTok)
        : base(context, operand) {
        this.operatorMeth = null;
        this.operatorTok = operatorTok;
        this.metaData = null;
        this.type = null;
      }
      
      public PostOrPrefixOperator(int operatorTok)
        : this(null, null, (PostOrPrefix)operatorTok){
      }
      
      private Object DoOp(int i){
        switch (this.operatorTok){
          case PostOrPrefix.PostfixIncrement:
          case PostOrPrefix.PrefixIncrement:
            if (i == int.MaxValue) return (double)int.MaxValue + 1.0;
            return i + 1;
          default:
            if (i == int.MinValue) return (double)int.MinValue - 1.0;
            return i - 1;
        }
      }
      
      private Object DoOp(uint i){
        switch (this.operatorTok){
          case PostOrPrefix.PostfixIncrement:
          case PostOrPrefix.PrefixIncrement:
            if (i == uint.MaxValue) return (double)uint.MaxValue + 1.0;
            return i + 1;
          default:
            if (i == uint.MinValue) return (double)uint.MinValue - 1.0;
            return i - 1;
        }
      }
      
      private Object DoOp(long i){
        switch (this.operatorTok)
        {
          case PostOrPrefix.PostfixIncrement:
          case PostOrPrefix.PrefixIncrement:
            if (i == long.MaxValue) return (double)long.MaxValue + 1.0;
            return i + 1;
          default:
            if (i == long.MinValue) return (double)long.MinValue - 1.0;
            return i - 1;
        }
      }
      
      private Object DoOp(ulong i){
        switch (this.operatorTok)
        {
          case PostOrPrefix.PostfixIncrement:
          case PostOrPrefix.PrefixIncrement:
            if (i == ulong.MaxValue) return (double)ulong.MaxValue + 1.0;
            return i + 1;
          default:
            if (i == ulong.MinValue) return (double)ulong.MinValue - 1.0;
            return i - 1;
        }
      }
      
      private Object DoOp(double d){
        switch (this.operatorTok){
          case PostOrPrefix.PostfixIncrement:
          case PostOrPrefix.PrefixIncrement:
            return d+1;
          default:
            return d-1;
        }
      }
      
      internal override Object Evaluate(){
        try{
          Object oldval = this.operand.Evaluate();
          Object newval = this.EvaluatePostOrPrefix(ref oldval);
          this.operand.SetValue(newval);
          switch (this.operatorTok){
            case PostOrPrefix.PostfixDecrement:
            case PostOrPrefix.PostfixIncrement:
               return oldval;
            case PostOrPrefix.PrefixDecrement:
            case PostOrPrefix.PrefixIncrement:
               return newval;
            default:
              throw new JScriptException(JSError.InternalError, this.context);
          }
        }catch(JScriptException e){
          if (e.context == null)
            e.context = this.context;
          throw e;
        }catch(Exception e){
          throw new JScriptException(e, this.context);
        }catch{
          throw new JScriptException(JSError.NonClsException, this.context);
        }
      }
      
#if !DEBUG
      [DebuggerStepThroughAttribute]
      [DebuggerHiddenAttribute]
#endif      
      public Object EvaluatePostOrPrefix(ref Object v){
        int i; uint ui; long l; ulong ul; double d;
        IConvertible ic = Convert.GetIConvertible(v);
        switch (Convert.GetTypeCode(v, ic)){
          case TypeCode.Empty:  v = Double.NaN; return v;
          case TypeCode.DBNull: v = 0; return this.DoOp(0);
          case TypeCode.Boolean: 
          case TypeCode.SByte:
          case TypeCode.Byte: 
          case TypeCode.Int16:
          case TypeCode.UInt16: 
          case TypeCode.Int32: v = i = ic.ToInt32(null); return this.DoOp(i); 
          case TypeCode.Char: i = ic.ToInt32(null); return ((IConvertible)this.DoOp(i)).ToChar(null);
          case TypeCode.UInt32: v = ui = ic.ToUInt32(null); return this.DoOp(ui); 
          case TypeCode.Int64: v = l = ic.ToInt64(null); return this.DoOp(l); 
          case TypeCode.UInt64: v = ul = ic.ToUInt64(null); return this.DoOp(ul); 
          case TypeCode.Single:
          case TypeCode.Double: v = d = ic.ToDouble(null); return this.DoOp(d); 
        }
        MethodInfo oper = this.GetOperator(v.GetType());
        if (oper != null)
          return oper.Invoke(null, (BindingFlags)0, JSBinder.ob, new Object[]{v}, null);
        else{
          v = d = Convert.ToNumber(v, ic);
          return this.DoOp(d);
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
          case PostOrPrefix.PostfixDecrement:
          case PostOrPrefix.PrefixDecrement:
            this.operatorMeth = t.GetMethod("op_Decrement", BindingFlags.Public|BindingFlags.Static, JSBinder.ob, new Type[]{t}, null); break;
          case PostOrPrefix.PostfixIncrement:
          case PostOrPrefix.PrefixIncrement:
            this.operatorMeth = t.GetMethod("op_Increment", BindingFlags.Public|BindingFlags.Static, JSBinder.ob, new Type[]{t}, null); break;
          default:
            throw new JScriptException(JSError.InternalError, this.context);
        }
        if (this.operatorMeth != null && 
        (!this.operatorMeth.IsStatic || (operatorMeth.Attributes & MethodAttributes.SpecialName) == (MethodAttributes)0 || operatorMeth.GetParameters().Length != 1))
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
        if (Convert.IsPrimitiveNumericType(this.type))
          return this.type;
        else if (this.type == Typeob.Char)
          return this.type;
        else if (Typeob.JSObject.IsAssignableFrom(this.type))
          return Typeob.Double;
        else
          return Typeob.Object;
      }
      
      internal override AST PartiallyEvaluate(){
        this.operand = this.operand.PartiallyEvaluateAsReference();
        this.operand.SetPartialValue(this);
        return this;
      }
      
      private void TranslateToILForNoOverloadCase(ILGenerator il, Type rtype){
        Type type = Convert.ToType(this.operand.InferType(null));
        this.operand.TranslateToILPreSetPlusGet(il);
        if (rtype == Typeob.Void){
          Type rt = Typeob.Double;
          if (Convert.IsPrimitiveNumericType(type))
            if (type == Typeob.SByte || type == Typeob.Int16)
              rt = Typeob.Int32;
            else if (type == Typeob.Byte || type == Typeob.UInt16 || type == Typeob.Char)
              rt = Typeob.UInt32;
            else
              rt = type;
          Convert.Emit(this, il, type, rt);
          il.Emit(OpCodes.Ldc_I4_1);
          Convert.Emit(this, il, Typeob.Int32, rt);
          if (rt == Typeob.Double || rt == Typeob.Single){
            if (this.operatorTok == PostOrPrefix.PostfixDecrement ||  this.operatorTok == PostOrPrefix.PrefixDecrement){
              il.Emit(OpCodes.Sub);
            }else{
              il.Emit(OpCodes.Add);
            }
          }else if (rt == Typeob.Int32 || rt == Typeob.Int64){
            if (this.operatorTok == PostOrPrefix.PostfixDecrement ||  this.operatorTok == PostOrPrefix.PrefixDecrement){
              il.Emit(OpCodes.Sub_Ovf);
            }else{
              il.Emit(OpCodes.Add_Ovf);
            }
          }else{
            if (this.operatorTok == PostOrPrefix.PostfixDecrement ||  this.operatorTok == PostOrPrefix.PrefixDecrement){
              il.Emit(OpCodes.Sub_Ovf_Un);
            }else{
              il.Emit(OpCodes.Add_Ovf_Un);
            }
          }
          Convert.Emit(this, il, rt, type);
          this.operand.TranslateToILSet(il);
        }else{
          //set rt to be the smallest type that is precise enough for the result and the variable
          Type rt = Typeob.Double;
          if (Convert.IsPrimitiveNumericType(rtype) && Convert.IsPromotableTo(type, rtype))
            rt = rtype;
          else if (Convert.IsPrimitiveNumericType(type) && Convert.IsPromotableTo(rtype, type))
            rt = type;
          if (rt == Typeob.SByte || rt == Typeob.Int16)
            rt = Typeob.Int32;
          else if (rt == Typeob.Byte || rt == Typeob.UInt16 || rt == Typeob.Char)
            rt = Typeob.UInt32;
          LocalBuilder result = il.DeclareLocal(rtype);
          Convert.Emit(this, il, type, rt);
          if (this.operatorTok == PostOrPrefix.PostfixDecrement){
            il.Emit(OpCodes.Dup);
            if (type == Typeob.Char){
              Convert.Emit(this, il, rt, Typeob.Char);
              Convert.Emit(this, il, Typeob.Char, rtype);
            }else
              Convert.Emit(this, il, rt, rtype);
            il.Emit(OpCodes.Stloc, result);
            il.Emit(OpCodes.Ldc_I4_1);
            Convert.Emit(this, il, Typeob.Int32, rt);
            if (rt == Typeob.Double || rt == Typeob.Single)
              il.Emit(OpCodes.Sub);
            else if (rt == Typeob.Int32 || rt == Typeob.Int64)
              il.Emit(OpCodes.Sub_Ovf);
            else
              il.Emit(OpCodes.Sub_Ovf_Un);
          }else if (this.operatorTok == PostOrPrefix.PostfixIncrement){
            il.Emit(OpCodes.Dup);
            if (type == Typeob.Char){
              Convert.Emit(this, il, rt, Typeob.Char);
              Convert.Emit(this, il, Typeob.Char, rtype);
            }else
              Convert.Emit(this, il, rt, rtype);
            il.Emit(OpCodes.Stloc, result);
            il.Emit(OpCodes.Ldc_I4_1);
            Convert.Emit(this, il, Typeob.Int32, rt);
            if (rt == Typeob.Double || rt == Typeob.Single)
              il.Emit(OpCodes.Add);
            else if (rt == Typeob.Int32 || rt == Typeob.Int64)
              il.Emit(OpCodes.Add_Ovf);
            else
              il.Emit(OpCodes.Add_Ovf_Un);
          }else if (this.operatorTok == PostOrPrefix.PrefixDecrement){
            il.Emit(OpCodes.Ldc_I4_1);
            Convert.Emit(this, il, Typeob.Int32, rt);
            if (rt == Typeob.Double || rt == Typeob.Single)
              il.Emit(OpCodes.Sub);
            else if (rt == Typeob.Int32 || rt == Typeob.Int64)
              il.Emit(OpCodes.Sub_Ovf);
            else
              il.Emit(OpCodes.Sub_Ovf_Un);
            il.Emit(OpCodes.Dup);
            if (type == Typeob.Char){
              Convert.Emit(this, il, rt, Typeob.Char);
              Convert.Emit(this, il, Typeob.Char, rtype);
            } else
              Convert.Emit(this, il, rt, rtype);
            il.Emit(OpCodes.Stloc, result);
          }else{ //if (this.operatorTok == PostOrPrefix.PrefixIncrement)
            il.Emit(OpCodes.Ldc_I4_1);
            Convert.Emit(this, il, Typeob.Int32, rt);
            if (rt == Typeob.Double || rt == Typeob.Single)
              il.Emit(OpCodes.Add);
            else if (rt == Typeob.Int32 || rt == Typeob.Int64)
              il.Emit(OpCodes.Add_Ovf);
            else
              il.Emit(OpCodes.Add_Ovf_Un);
            il.Emit(OpCodes.Dup);
            if (type == Typeob.Char){
              Convert.Emit(this, il, rt, Typeob.Char);
              Convert.Emit(this, il, Typeob.Char, rtype);
            } else
              Convert.Emit(this, il, rt, rtype);
            il.Emit(OpCodes.Stloc, result);
          }
          Convert.Emit(this, il, rt, type);
          this.operand.TranslateToILSet(il);
          il.Emit(OpCodes.Ldloc, result);
        }
      }
      
      internal override void TranslateToIL(ILGenerator il, Type rtype){
        if (this.metaData == null){
          TranslateToILForNoOverloadCase(il, rtype);
          return;
        }
        if (this.metaData is MethodInfo){
          Object result = null;
          Type type = Convert.ToType(this.operand.InferType(null));
          this.operand.TranslateToILPreSetPlusGet(il);
          if (rtype != Typeob.Void){
            result = il.DeclareLocal(rtype);
            if (this.operatorTok == PostOrPrefix.PostfixDecrement || this.operatorTok == PostOrPrefix.PostfixIncrement){
              il.Emit(OpCodes.Dup);
              Convert.Emit(this, il, type, rtype);
              il.Emit(OpCodes.Stloc, (LocalBuilder)result);
            }
          }
          MethodInfo oper = (MethodInfo)this.metaData;
          ParameterInfo[] pars = oper.GetParameters();
          Convert.Emit(this, il, type, pars[0].ParameterType);
          il.Emit(OpCodes.Call, oper);
          if (rtype != Typeob.Void){
            if (this.operatorTok == PostOrPrefix.PrefixDecrement || this.operatorTok == PostOrPrefix.PrefixIncrement){
              il.Emit(OpCodes.Dup);
              Convert.Emit(this, il, type, rtype);
              il.Emit(OpCodes.Stloc, (LocalBuilder)result);
            }
          }
          Convert.Emit(this, il, oper.ReturnType, type);
          this.operand.TranslateToILSet(il);
          if (rtype != Typeob.Void)
            il.Emit(OpCodes.Ldloc, (LocalBuilder)result);
        }else{
          //Getting here is just too bad. We do not know until the code runs whether or not to call an overloaded operator method.
          //Compile operands to objects and devolve the decision making to run time thunks 
          Type type = Convert.ToType(this.operand.InferType(null));
          LocalBuilder result = il.DeclareLocal(Typeob.Object);
          this.operand.TranslateToILPreSetPlusGet(il);
          Convert.Emit(this, il, type, Typeob.Object);
          il.Emit(OpCodes.Stloc, result);
          il.Emit(OpCodes.Ldloc, (LocalBuilder)this.metaData);
          il.Emit(OpCodes.Ldloca, result);
          il.Emit(OpCodes.Call, CompilerGlobals.evaluatePostOrPrefixOperatorMethod);
          if (rtype != Typeob.Void){
            if (this.operatorTok == PostOrPrefix.PrefixDecrement || this.operatorTok == PostOrPrefix.PrefixIncrement){
              il.Emit(OpCodes.Dup);
              il.Emit(OpCodes.Stloc, result);
            }
          }
          Convert.Emit(this, il, Typeob.Object, type);
          this.operand.TranslateToILSet(il);
          if (rtype != Typeob.Void){
            il.Emit(OpCodes.Ldloc, result);
            Convert.Emit(this, il, Typeob.Object, rtype);
          }
        }
      }
    
      internal override void TranslateToILInitializer(ILGenerator il){
        IReflect rtype = this.InferType(null);
        this.operand.TranslateToILInitializer(il);
        if (rtype != Typeob.Object)
          return;
        this.metaData = il.DeclareLocal(Typeob.PostOrPrefixOperator);
        ConstantWrapper.TranslateToILInt(il, (int)this.operatorTok);
        il.Emit(OpCodes.Newobj, CompilerGlobals.postOrPrefixConstructor);
        il.Emit(OpCodes.Stloc, (LocalBuilder)this.metaData);
      }
    }
}
