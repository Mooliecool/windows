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
    using System.Diagnostics;

    public sealed class ArgumentsObject : JSObject{
      private Object[] arguments;
      private String[] formal_names;
      private ScriptObject scope;
      
      public Object callee;
      public Object caller;
      public Object length;
      
      internal ArgumentsObject(ScriptObject parent, Object[] arguments, FunctionObject function, Closure callee, ScriptObject scope, ArgumentsObject caller)
        : base(parent) {
        this.arguments = arguments;
        this.formal_names = function.formal_parameters;
        this.scope = scope;
        this.callee = callee;
        this.caller = caller;
        this.length = arguments.Length;
        this.noExpando = false;
      }
      
      internal override Object GetValueAtIndex(uint index){
        if (index < this.arguments.Length)
          return this.arguments[(int)index];
        else
          return base.GetValueAtIndex(index);
      }
    
#if !DEBUG
      [DebuggerStepThroughAttribute]
      [DebuggerHiddenAttribute]
#endif      
      internal override Object GetMemberValue(String name){
        long index = ArrayObject.Array_index_for(name);
        if (index < 0)
          return base.GetMemberValue(name);
        else
          return this.GetValueAtIndex((uint)index);
      }
    
      internal override void SetValueAtIndex(uint index, Object value){
        if (index < this.arguments.Length)
          this.arguments[(int)index] = value;
        else
          base.SetValueAtIndex(index, value);
      }
      
      internal Object[] ToArray(){
        return this.arguments;
      }
    }

}
