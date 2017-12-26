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

    public sealed class AssemblyCustomAttributeList : AST{
      private CustomAttributeList list;
      internal bool okToUse;
      
      internal AssemblyCustomAttributeList(CustomAttributeList list)
        : base(list.context) {
        this.list = list;
        this.okToUse = false;
      }
      
      internal override Object Evaluate(){
        return null;
      }

      internal void Process(){        
        this.okToUse = true;
        this.list.SetTarget(this);
        this.list.PartiallyEvaluate();
      }

      internal override AST PartiallyEvaluate(){
        if (!this.okToUse)
          this.context.HandleError(JSError.AssemblyAttributesMustBeGlobal);
        return this;
      }
        
      internal override void TranslateToIL(ILGenerator il, Type rtype){
        foreach (CustomAttributeBuilder ca in this.list.GetCustomAttributeBuilders(false))
          this.compilerGlobals.assemblyBuilder.SetCustomAttribute(ca);
        if (rtype != Typeob.Void) {
          il.Emit(OpCodes.Ldnull);
          if (rtype.IsValueType)
            Convert.Emit(this, il, Typeob.Object, rtype);
        }
      }
        
      internal override void TranslateToILInitializer(ILGenerator il){
      }
                
    }
}
