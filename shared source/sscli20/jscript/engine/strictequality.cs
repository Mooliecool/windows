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
    
    public class StrictEquality : BinaryOp{
      
      internal StrictEquality(Context context, AST operand1, AST operand2, JSToken operatorTok)
        : base(context, operand1, operand2, operatorTok) {
      }
    
      internal override Object Evaluate(){
        bool result = JScriptStrictEquals(this.operand1.Evaluate(), this.operand2.Evaluate(), VsaEngine.executeForJSEE);
        if (this.operatorTok == JSToken.StrictEqual)
          return result;
        else
          return !result;
      }
     
      public static bool JScriptStrictEquals(Object v1, Object v2){
        return StrictEquality.JScriptStrictEquals(v1, v2, false);
      }

      internal static bool JScriptStrictEquals(Object v1, Object v2, bool checkForDebuggerObjects){
        IConvertible ic1 = Convert.GetIConvertible(v1);
        IConvertible ic2 = Convert.GetIConvertible(v2);
        TypeCode t1 = Convert.GetTypeCode(v1, ic1);
        TypeCode t2 = Convert.GetTypeCode(v2, ic2);
        return StrictEquality.JScriptStrictEquals(v1, v2, ic1, ic2, t1, t2, checkForDebuggerObjects);
      }

      internal static bool JScriptStrictEquals(Object v1, Object v2, IConvertible ic1, IConvertible ic2, TypeCode t1, TypeCode t2, bool checkForDebuggerObjects){
        switch (t1){
          case TypeCode.Empty: return t2 == TypeCode.Empty;
          case TypeCode.Object:
            if (v1 == v2) return true;
            if (v1 is Missing || v1 is System.Reflection.Missing) v1 = null;
            if (v1 == v2) return true;
            if (v2 is Missing || v2 is System.Reflection.Missing) v2 = null;
            return v1 == v2;
          case TypeCode.DBNull: return t2 == TypeCode.DBNull;
          case TypeCode.Boolean: return t2 == TypeCode.Boolean && ic1.ToBoolean(null) == ic2.ToBoolean(null);
          
          case TypeCode.Char: 
            Char ch = ic1.ToChar(null);
            switch(t2){
              case TypeCode.Char: return ch == ic2.ToChar(null);
              case TypeCode.SByte:
              case TypeCode.Byte:
              case TypeCode.Int16:
              case TypeCode.UInt16:
              case TypeCode.Int32:
              case TypeCode.UInt32:
              case TypeCode.Int64: return ch == ic2.ToInt64(null);
              case TypeCode.UInt64: return ch == ic2.ToUInt64(null);
              case TypeCode.Single: 
              case TypeCode.Double: return ch == ic2.ToDouble(null);
              case TypeCode.Decimal: return ((Decimal)(int)ch) == ic2.ToDecimal(null);
              case TypeCode.String:
                String str = ic2.ToString(null);
                return str.Length == 1 && ch == str[0];
            }
            return false;
          
          case TypeCode.SByte:
            SByte sb1 = ic1.ToSByte(null);
            switch (t2){
              case TypeCode.Char: return sb1 == ic2.ToChar(null);
              case TypeCode.SByte:
              case TypeCode.Byte:
              case TypeCode.Int16:
              case TypeCode.UInt16:
              case TypeCode.Int32:
              case TypeCode.UInt32:
              case TypeCode.Int64: return sb1 == ic2.ToInt64(null);
              case TypeCode.UInt64: return sb1 >= 0 && ((UInt64)sb1) == ic2.ToUInt64(null);
              case TypeCode.Single: return sb1 == ic2.ToSingle(null);
              case TypeCode.Double: return sb1 == ic2.ToDouble(null);
              case TypeCode.Decimal: return ((Decimal)sb1) == ic2.ToDecimal(null);
            }
            return false;
            
          case TypeCode.Byte:
            Byte b1 = ic1.ToByte(null);
            switch (t2){
              case TypeCode.Char: return b1 == ic2.ToChar(null);
              case TypeCode.SByte:
              case TypeCode.Byte:
              case TypeCode.Int16:
              case TypeCode.UInt16:
              case TypeCode.Int32:
              case TypeCode.UInt32:
              case TypeCode.Int64: return b1 == ic2.ToInt64(null);
              case TypeCode.UInt64: return b1 == ic2.ToUInt64(null);
              case TypeCode.Single: return b1 == ic2.ToSingle(null);
              case TypeCode.Double: return b1 == ic2.ToDouble(null);
              case TypeCode.Decimal: return ((Decimal)b1) == ic2.ToDecimal(null);
            }
            return false;
            
          case TypeCode.Int16:
            Int16 s1 = ic1.ToInt16(null);
            switch (t2){
              case TypeCode.Char: return s1 == ic2.ToChar(null);
              case TypeCode.SByte:
              case TypeCode.Byte:
              case TypeCode.Int16:
              case TypeCode.UInt16:
              case TypeCode.Int32:
              case TypeCode.UInt32:
              case TypeCode.Int64: return s1 == ic2.ToInt64(null);
              case TypeCode.UInt64: return s1 >= 0 && ((UInt64)s1) == ic2.ToUInt64(null);
              case TypeCode.Single: return s1 == ic2.ToSingle(null);
              case TypeCode.Double: return s1 == ic2.ToDouble(null);
              case TypeCode.Decimal: return ((Decimal)s1) == ic2.ToDecimal(null);
           }
            return false;
            
          case TypeCode.UInt16:
            UInt16 us1 = ic1.ToUInt16(null);
            switch (t2){
              case TypeCode.Char: return us1 == ic2.ToChar(null);
              case TypeCode.SByte:
              case TypeCode.Byte:
              case TypeCode.Int16:
              case TypeCode.UInt16:
              case TypeCode.Int32:
              case TypeCode.UInt32:
              case TypeCode.Int64: return us1 == ic2.ToInt64(null);
              case TypeCode.UInt64: return us1 == ic2.ToUInt64(null);
              case TypeCode.Single: return us1 == ic2.ToSingle(null);
              case TypeCode.Double: return us1 == ic2.ToDouble(null);
              case TypeCode.Decimal: return ((Decimal)us1) == ic2.ToDecimal(null);
            }
            return false;
            
          case TypeCode.Int32:
            Int32 i1 = ic1.ToInt32(null);
            switch (t2){
              case TypeCode.Char: return i1 == ic2.ToChar(null);
              case TypeCode.SByte:
              case TypeCode.Byte:
              case TypeCode.Int16:
              case TypeCode.UInt16:
              case TypeCode.Int32:
              case TypeCode.UInt32:
              case TypeCode.Int64: return i1 == ic2.ToInt64(null);
              case TypeCode.UInt64: return i1 >= 0 && ((UInt64)i1) == ic2.ToUInt64(null);
              case TypeCode.Single: return i1 == ic2.ToSingle(null);
              case TypeCode.Double: return i1 == ic2.ToDouble(null);
              case TypeCode.Decimal: return ((Decimal)i1) == ic2.ToDecimal(null);
            }
            return false;
            
          case TypeCode.UInt32:
            UInt32 ui1 = ic1.ToUInt32(null);
            switch (t2){
              case TypeCode.Char: return ui1 == ic2.ToChar(null);
              case TypeCode.SByte:
              case TypeCode.Byte:
              case TypeCode.Int16:
              case TypeCode.UInt16:
              case TypeCode.Int32:
              case TypeCode.UInt32:
              case TypeCode.Int64: return ui1 == ic2.ToInt64(null);
              case TypeCode.UInt64: return ui1 == ic2.ToUInt64(null);
              case TypeCode.Single: return ui1 == ic2.ToSingle(null);
              case TypeCode.Double: return ui1 == ic2.ToDouble(null);
              case TypeCode.Decimal: return ((Decimal)ui1) == ic2.ToDecimal(null);
            }
            return false;
            
          case TypeCode.Int64:
            Int64 l1 = ic1.ToInt64(null);
            switch (t2){
              case TypeCode.Char: return l1 == ic2.ToChar(null);
              case TypeCode.SByte:
              case TypeCode.Byte:
              case TypeCode.Int16:
              case TypeCode.UInt16:
              case TypeCode.Int32:
              case TypeCode.UInt32:
              case TypeCode.Int64: return l1 == ic2.ToInt64(null);
              case TypeCode.UInt64: return l1 >= 0 && ((UInt64)l1) == ic2.ToUInt64(null);
              case TypeCode.Single: return l1 == ic2.ToSingle(null);
              case TypeCode.Double: return l1 == ic2.ToDouble(null);
              case TypeCode.Decimal: return ((Decimal)l1) == ic2.ToDecimal(null);
            }
            return false;
            
          case TypeCode.UInt64:
            UInt64 ul1 = ic1.ToUInt64(null);
            switch (t2){
              case TypeCode.Char: return ul1 == ic2.ToChar(null);
              case TypeCode.SByte:
              case TypeCode.Byte:
              case TypeCode.Int16:
              case TypeCode.UInt16:
              case TypeCode.Int32:
              case TypeCode.UInt32:
              case TypeCode.Int64:
                l1 = ic2.ToInt64(null);
                return l1 >= 0 && ul1 == (UInt64)l1;
              case TypeCode.UInt64: return ul1 == ic2.ToUInt64(null);
              case TypeCode.Single: return ul1 == ic2.ToSingle(null);
              case TypeCode.Double: return ul1 == ic2.ToDouble(null);
              case TypeCode.Decimal: return ((Decimal)ul1) == ic2.ToDecimal(null);
            }
            return false;
            
          case TypeCode.Single:
            Single f1 = ic1.ToSingle(null);
            switch (t2){
              case TypeCode.Char: return f1 == ic2.ToChar(null);
              case TypeCode.SByte:
              case TypeCode.Byte:
              case TypeCode.Int16:
              case TypeCode.UInt16:
              case TypeCode.Int32:
              case TypeCode.UInt32:
              case TypeCode.Int64: return f1 == ic2.ToInt64(null);
              case TypeCode.UInt64: return f1 == ic2.ToUInt64(null);
              case TypeCode.Single: return f1 == ic2.ToSingle(null);
              case TypeCode.Double: return f1 == ic2.ToSingle(null);
              case TypeCode.Decimal: return ((Decimal)f1) == ic2.ToDecimal(null);
            }
            return false;
            
          case TypeCode.Double:
            Double d1 = ic1.ToDouble(null);
            switch (t2){
              case TypeCode.Char: return d1 == ic2.ToChar(null);
              case TypeCode.SByte:
              case TypeCode.Byte:
              case TypeCode.Int16:
              case TypeCode.UInt16:
              case TypeCode.Int32:
              case TypeCode.UInt32:
              case TypeCode.Int64: return d1 == ic2.ToInt64(null);
              case TypeCode.UInt64: return d1 == ic2.ToUInt64(null);
              case TypeCode.Single: return ((float)d1) == ic2.ToSingle(null);
              case TypeCode.Double: return d1 == ic2.ToDouble(null);
              case TypeCode.Decimal: return ((Decimal)d1) == ic2.ToDecimal(null);
            }
            return false;
            
          case TypeCode.Decimal:
            Decimal de1 = ic1.ToDecimal(null);
            switch (t2){
              case TypeCode.Char: return de1 == (Decimal)(int)ic2.ToChar(null);
              case TypeCode.SByte:
              case TypeCode.Byte:
              case TypeCode.Int16:
              case TypeCode.UInt16:
              case TypeCode.Int32:
              case TypeCode.UInt32:
              case TypeCode.Int64: return de1 == ic2.ToInt64(null);
              case TypeCode.UInt64: return de1 == ic2.ToUInt64(null);
              case TypeCode.Single: return de1 == (Decimal)ic2.ToSingle(null);
              case TypeCode.Double: return de1 == (Decimal)ic2.ToDouble(null);
              case TypeCode.Decimal: return de1 == ic2.ToDecimal(null);
            }
            return false;
            
          case TypeCode.DateTime: return t2 == TypeCode.DateTime && ic1.ToDateTime(null) == ic2.ToDateTime(null);
          case TypeCode.String:
            if (t2 == TypeCode.Char){
              String str = ic1.ToString(null);
              return str.Length == 1 && str[0] == ic2.ToChar(null);
            }
            return t2 == TypeCode.String && (v1 == v2 || ic1.ToString(null).Equals(ic2.ToString(null)));
        }
        return false; //should never get here
      }
    
      internal override IReflect InferType(JSField inference_target){
        return Typeob.Boolean;
      }
      
      internal override void TranslateToConditionalBranch(ILGenerator il, bool branchIfTrue, Label label, bool shortForm){
        Type t1 = Convert.ToType(this.operand1.InferType(null));
        Type t2 = Convert.ToType(this.operand2.InferType(null));
        if (this.operand1 is ConstantWrapper)
          if (this.operand1.Evaluate() == null) t1 = Typeob.Empty;
        if (this.operand2 is ConstantWrapper)
          if (this.operand2.Evaluate() == null) t2 = Typeob.Empty;
        if (t1 != t2 && t1.IsPrimitive && t2.IsPrimitive){
          if (t1 == Typeob.Single)
            t2 = t1;
          else if (t2 == Typeob.Single)
            t1 = t2;
          else if (Convert.IsPromotableTo(t2, t1))
            t2 = t1;
          else if (Convert.IsPromotableTo(t1, t2))
            t1 = t2;
        }
        bool nonPrimitive = true;
        if (t1 == t2 && t1 != Typeob.Object){
          // Operand types are equal and not Object - need to compare values only. Primitive
          // values get compared with IL instructions; other values including value types
          // get compared with Object.Equals. String is special cased for perf.
          Type t = t1; 
          if (!t1.IsPrimitive)
            t = Typeob.Object;
          this.operand1.TranslateToIL(il, t);
          this.operand2.TranslateToIL(il, t);
          if (t1 == Typeob.String)
            il.Emit(OpCodes.Call, CompilerGlobals.stringEqualsMethod);
          else if (!t1.IsPrimitive)
            il.Emit(OpCodes.Callvirt, CompilerGlobals.equalsMethod);
          else
            nonPrimitive = false;
        }else if (t1 == Typeob.Empty){
          this.operand2.TranslateToIL(il, Typeob.Object);
          branchIfTrue = !branchIfTrue;
        }else if (t2 == Typeob.Empty){
          this.operand1.TranslateToIL(il, Typeob.Object);
          branchIfTrue = !branchIfTrue;
        }else{
          this.operand1.TranslateToIL(il, Typeob.Object);
          this.operand2.TranslateToIL(il, Typeob.Object);
          il.Emit(OpCodes.Call, CompilerGlobals.jScriptStrictEqualsMethod);
        }
        if (branchIfTrue){
          if (this.operatorTok == JSToken.StrictEqual)
            if (nonPrimitive)
              il.Emit(shortForm ? OpCodes.Brtrue_S : OpCodes.Brtrue, label);
            else
              il.Emit(shortForm ? OpCodes.Beq_S : OpCodes.Beq, label);
          else
            if (nonPrimitive)
              il.Emit(shortForm ? OpCodes.Brfalse_S : OpCodes.Brfalse, label);
            else
              il.Emit(shortForm ? OpCodes.Bne_Un_S : OpCodes.Bne_Un, label);
        }else{
          if (this.operatorTok == JSToken.StrictEqual)
            if (nonPrimitive)
              il.Emit(shortForm ? OpCodes.Brfalse_S : OpCodes.Brfalse, label);
            else
              il.Emit(shortForm ? OpCodes.Bne_Un_S : OpCodes.Bne_Un, label);
          else
            if (nonPrimitive)
              il.Emit(shortForm ? OpCodes.Brtrue_S : OpCodes.Brtrue, label);
            else
              il.Emit(shortForm ? OpCodes.Beq_S : OpCodes.Beq, label);
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
    }
}
