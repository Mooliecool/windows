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
    using System.Collections;
    using System.Reflection;
    using System.Reflection.Emit;
    using System.Runtime.InteropServices.Expando;
    
    public sealed class In : BinaryOp{
      
      internal In(Context context, AST operand1, AST operand2)
        : base(context, operand1, operand2) {
      }
      
      internal override Object Evaluate(){
        Object v1 = this.operand1.Evaluate();
        Object v2 = this.operand2.Evaluate();
        try{
          return JScriptIn(v1, v2);
        }catch(JScriptException e){
          if (e.context == null)
            e.context = this.operand2.context;
          throw e;
        }
      }
      
      internal override IReflect InferType(JSField inference_target){
        return Typeob.Boolean;
      }

      //"x in y" should return true whenever "for (var z in y) if (z === x) return true" returns true
      public static bool JScriptIn(Object v1, Object v2){
        bool result = false;
        if (v2 is ScriptObject)
          return !(((ScriptObject)v2).GetMemberValue(Convert.ToString(v1)) is Missing);
        else if (v2 is Array){
          Array arr = (Array)v2;
          double d = Convert.ToNumber(v1);
          int i = (int)d;
          return d == i && arr.GetLowerBound(0) <= i && i <= arr.GetUpperBound(0);
        }else if (v2 is IEnumerable){
          if (v1 == null) return false;
          //Do not enumerate when a direct lookup is available
          if (v2 is IDictionary)
            return ((IDictionary)v2).Contains(v1);
          if (v2 is IExpando){
            MemberInfo[] members = ((IReflect)v2).GetMember(Convert.ToString(v1), BindingFlags.Instance|BindingFlags.DeclaredOnly|BindingFlags.Public);
            return members.Length > 0;
          }
          IEnumerator enu = ((IEnumerable)v2).GetEnumerator();
          while (!result && enu.MoveNext())
            if (v1.Equals(enu.Current)) return true;
        }else if (v2 is IEnumerator){
          if (v1 == null) return false;          
          IEnumerator enu = (IEnumerator)v2;
          while (!result && enu.MoveNext())
            if (v1.Equals(enu.Current)) return true;
        }
        throw new JScriptException(JSError.ObjectExpected);
      }
      
      internal override void TranslateToIL(ILGenerator il, Type rtype){
        this.operand1.TranslateToIL(il, Typeob.Object);
        this.operand2.TranslateToIL(il, Typeob.Object);
        il.Emit(OpCodes.Call, CompilerGlobals.jScriptInMethod);
        Convert.Emit(this, il, Typeob.Boolean, rtype);
      }
    
    }
}
