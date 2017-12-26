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

    public sealed class Typeof : UnaryOp{
      internal Typeof(Context context, AST operand)
        : base(context, operand) {
      }

      internal override Object Evaluate(){
        try{
          return JScriptTypeof(this.operand.Evaluate(), VsaEngine.executeForJSEE);
        }catch(JScriptException e){
          if ((e.Number & 0xFFFF) == (int)JSError.UndefinedIdentifier)
            return "undefined";
          throw e;
        }
      }

      internal override IReflect InferType(JSField inference_target){
        return Typeob.String;
      }

      public static String JScriptTypeof(Object value){
        return JScriptTypeof(value, false);
      }

      internal static String JScriptTypeof(Object value, bool checkForDebuggerObject){
        switch (Convert.GetTypeCode(value)){
          case TypeCode.Empty:
            return "undefined";
          case TypeCode.DBNull:
            return "object";
          case TypeCode.Object:
            if (value is Missing || value is System.Reflection.Missing) return "undefined";
            return value is ScriptFunction ? "function" : "object";
          case TypeCode.Boolean:
            return "boolean";
          case TypeCode.Char:
          case TypeCode.String:
            return "string";
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
            return "number";
          case TypeCode.DateTime:
            return "date";
        }
        return "unknown";
      }

      internal override void TranslateToIL(ILGenerator il, Type rtype){
        if (this.operand is Binding)
          //Make sure that no exception is thrown if the operand is an undefined identifier
          ((Binding)this.operand).TranslateToIL(il, Typeob.Object, true);
        else
          this.operand.TranslateToIL(il, Typeob.Object);
        il.Emit(OpCodes.Call, CompilerGlobals.jScriptTypeofMethod);
        Convert.Emit(this, il, Typeob.String, rtype);
      }
    }
}
