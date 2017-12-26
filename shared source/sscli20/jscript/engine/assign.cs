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
    
    internal sealed class Assign : AST{
      internal AST lhside;
      internal AST rhside;
      
      internal Assign(Context context, AST lhside, AST rhside)
        : base(context) {
        this.lhside = lhside;
        this.rhside = rhside;
      }
    
      internal override Object Evaluate(){
        try{
          if (this.lhside is Call)
            ((Call)this.lhside).EvaluateIndices();
          Object val = this.rhside.Evaluate();
          lhside.SetValue(val);
          return val;
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
     
      internal override IReflect InferType(JSField inference_target){
        return this.rhside.InferType(inference_target);
      }

      internal override AST PartiallyEvaluate(){
        AST lhref = this.lhside.PartiallyEvaluateAsReference();
        this.lhside = lhref;
        this.rhside = this.rhside.PartiallyEvaluate();
        lhref.SetPartialValue(this.rhside);
        return this;
      }
      
      internal override void TranslateToIL(ILGenerator il, Type rtype){
        Type lhtype = Convert.ToType(this.lhside.InferType(null));
        this.lhside.TranslateToILPreSet(il);
        if (rtype != Typeob.Void){
          Type rhtype = Convert.ToType(this.rhside.InferType(null));
          this.rhside.TranslateToIL(il, rhtype);
          LocalBuilder result = il.DeclareLocal(rhtype);
          il.Emit(OpCodes.Dup);
          il.Emit(OpCodes.Stloc, result);
          Convert.Emit(this, il, rhtype, lhtype);
          this.lhside.TranslateToILSet(il);
          il.Emit(OpCodes.Ldloc, result);
          Convert.Emit(this, il, rhtype, rtype);
        }else{
          this.lhside.TranslateToILSet(il, rhside);
        }
      } 
        
      internal override void TranslateToILInitializer(ILGenerator il){
        this.lhside.TranslateToILInitializer(il);
        this.rhside.TranslateToILInitializer(il);
      }
    
    }
 }
