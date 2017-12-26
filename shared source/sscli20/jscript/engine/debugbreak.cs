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
  
  public class DebugBreak : AST{
   
    internal DebugBreak(Context context) : base(context){
    }
    
    internal override Object Evaluate(){
      Debugger.Break();
      return new Completion();
    }
  
    internal override AST PartiallyEvaluate(){
      return this;
    }
    
    internal override void TranslateToIL(ILGenerator il, Type rtype){
      this.context.EmitLineInfo(il);
      il.Emit(OpCodes.Call, CompilerGlobals.debugBreak);
    }

    internal override void TranslateToILInitializer(ILGenerator il){
    }
  
  }

}
