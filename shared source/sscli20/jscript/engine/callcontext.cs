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
    
    /*
    This class keeps the information around that is necessary to construct a trace of the call stack.
    
    This class is only used by the Evaluator. When running IL it is too expensive and akward to create these things.
    Instead, debugging information is emitted to the IL and used via the CLR debug API. (In the future, that is :-)
    */
    
    internal class CallContext{
      internal readonly Context sourceContext;  // The source context of the function call or eval expression.
      private readonly LateBinding callee; //Null if eval.
      private readonly Object[] actual_parameters;


      internal CallContext(Context sourceContext, LateBinding callee, Object[] actual_parameters){
        this.sourceContext = sourceContext;
        this.callee = callee;
        this.actual_parameters = actual_parameters;
      }

      internal String FunctionName(){
        if (callee == null)
          return "eval";
        return callee.ToString();
      }

    }

}