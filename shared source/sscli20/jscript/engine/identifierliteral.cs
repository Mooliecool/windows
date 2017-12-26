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
    
    internal sealed class IdentifierLiteral : AST{
      private String identifier;
      
      internal IdentifierLiteral(String identifier, Context context)
        : base(context) {
        this.identifier = identifier;
      }
    
      internal override Object Evaluate(){
        throw new JScriptException(JSError.InternalError, this.context);
      }
      
      internal override AST PartiallyEvaluate(){
        throw new JScriptException(JSError.InternalError, this.context);
      }
        
      public override String ToString(){
        return this.identifier;
      }
      
      internal override void TranslateToIL(ILGenerator il, Type rtype){
        throw new JScriptException(JSError.InternalError, this.context);
      }
      
    }
}
