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
    
    public abstract class BinaryOp : AST{
      protected AST operand1;
      protected AST operand2;
      protected JSToken operatorTok;
      protected Type type1;
      protected Type type2;
      protected MethodInfo operatorMeth;
      
      internal BinaryOp(Context context, AST operand1, AST operand2)
        : this(context, operand1, operand2, (JSToken)0){
      }
      
      internal BinaryOp(Context context, AST operand1, AST operand2, JSToken operatorTok)
        : base(context) {
        this.operand1 = operand1;
        this.operand2 = operand2;
        this.operatorTok = operatorTok;
        this.type1 = null;
        this.type2 = null;
        this.operatorMeth = null;
      }
      
      internal override void CheckIfOKToUseInSuperConstructorCall(){
        this.operand1.CheckIfOKToUseInSuperConstructorCall();
        this.operand2.CheckIfOKToUseInSuperConstructorCall();
      }

      protected MethodInfo GetOperator(IReflect ir1, IReflect ir2){
        if (ir1 is ClassScope) ir1 = ((ClassScope)ir1).GetUnderlyingTypeIfEnum();
        if (ir2 is ClassScope) ir2 = ((ClassScope)ir2).GetUnderlyingTypeIfEnum();
        Type t1 = ir1 is Type ? (Type)ir1 : Typeob.Object;
        Type t2 = ir2 is Type ? (Type)ir2 : Typeob.Object;
        if (this.type1 == t1 && this.type2 == t2)
          return this.operatorMeth; 
        this.type1 = t1;
        this.type2 = t2;
        this.operatorMeth = null;
        if (t1 == Typeob.String || Convert.IsPrimitiveNumericType(ir1) || Typeob.JSObject.IsAssignableFrom(t1))
          t1 = null; 
        if (t2 == Typeob.String || Convert.IsPrimitiveNumericType(ir2) || Typeob.JSObject.IsAssignableFrom(t2))
          t2 = null;
        if (t1 == null && t2 == null)
          return null;
        //One of the operands is an object of a type that might have a user defined operator.
        String name = "op_NoSuchOp";
        switch (this.operatorTok){
          case JSToken.BitwiseAnd: name = "op_BitwiseAnd"; break;
          case JSToken.BitwiseOr: name = "op_BitwiseOr"; break;
          case JSToken.BitwiseXor: name = "op_ExclusiveOr"; break;
          case JSToken.Divide: name = "op_Division"; break;
          case JSToken.Equal: name = "op_Equality"; break;
          case JSToken.GreaterThan: name = "op_GreaterThan"; break;
          case JSToken.GreaterThanEqual: name = "op_GreaterThanOrEqual"; break;
          case JSToken.LeftShift: name = "op_LeftShift"; break;
          case JSToken.LessThan: name = "op_LessThan"; break;
          case JSToken.LessThanEqual: name = "op_LessThanOrEqual"; break;
          case JSToken.Minus: name = "op_Subtraction"; break;
          case JSToken.Modulo: name = "op_Modulus"; break;
          case JSToken.Multiply: name = "op_Multiply"; break;
          case JSToken.NotEqual: name = "op_Inequality"; break;
          case JSToken.Plus: name = "op_Addition"; break;
          case JSToken.RightShift:name = "op_RightShift"; break;
        }
        Type[] types = new Type[]{this.type1, this.type2};
        if (t1 == t2){
          MethodInfo op = t1.GetMethod(name, BindingFlags.Public|BindingFlags.Static, JSBinder.ob, types, null);
          if (op != null && (op.Attributes & MethodAttributes.SpecialName) != 0 && op.GetParameters().Length == 2)
            this.operatorMeth = op;
        }else{
          //Search both operand types, but only if there is a possibility that they might have operators defined on them
          MethodInfo op1 = t1 == null ? null : t1.GetMethod(name, BindingFlags.Public|BindingFlags.Static, JSBinder.ob, types, null);
          MethodInfo op2 = t2 == null ? null : t2.GetMethod(name, BindingFlags.Public|BindingFlags.Static, JSBinder.ob, types, null);
          this.operatorMeth = JSBinder.SelectOperator(op1, op2, this.type1, this.type2); //Choose the better of the two
        }
        if (this.operatorMeth != null)
          this.operatorMeth = new JSMethodInfo(this.operatorMeth);
        return this.operatorMeth;  
      }
      
      internal override AST PartiallyEvaluate(){
        this.operand1 = this.operand1.PartiallyEvaluate();
        this.operand2 = this.operand2.PartiallyEvaluate();
        try{
          if (this.operand1 is ConstantWrapper){
            if (this.operand2 is ConstantWrapper)
              return new ConstantWrapper(this.Evaluate(), this.context);
            else{
              Object val = ((ConstantWrapper)this.operand1).value;
              if (val is String && ((String)val).Length == 1 && this.operand2.InferType(null) == Typeob.Char)
                ((ConstantWrapper)this.operand1).value = (Char)((String)val)[0];
            }
          }else if (this.operand2 is ConstantWrapper){
            Object val = ((ConstantWrapper)this.operand2).value;
            if (val is String && ((String)val).Length == 1 && this.operand1.InferType(null) == Typeob.Char)
              ((ConstantWrapper)this.operand2).value = (Char)((String)val)[0];
          }
        }catch(JScriptException e){
          this.context.HandleError((JSError)(e.ErrorNumber & 0xFFFF));
        }catch{
          this.context.HandleError(JSError.TypeMismatch);
        }
        return this;
      }
      
      internal override void TranslateToILInitializer(ILGenerator il){
        this.operand1.TranslateToILInitializer(il);
        this.operand2.TranslateToILInitializer(il);
      }
    }
}
