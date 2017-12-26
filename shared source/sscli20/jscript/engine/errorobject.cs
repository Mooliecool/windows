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
    using System.Runtime.InteropServices;

    public class ErrorObject : JSObject{
      public Object message;
      public Object number;
      public Object description;
      internal Object exception;
     
      internal ErrorObject(ErrorPrototype parent, Object[] args)
        : base(parent) {
        this.exception = null;
        this.description = "";
        this.number = 0;
        if (args.Length == 1)
          if (args[0] == null || Convert.IsPrimitiveNumericType(args[0].GetType()))
            this.number = Convert.ToNumber(args[0]);
          else
            this.description = Convert.ToString(args[0]);
        else if (args.Length > 1){
          this.number = Convert.ToNumber(args[0]);
          this.description = Convert.ToString(args[1]);
        }
        this.message = this.description;
        this.noExpando = false;
      }
      
      internal ErrorObject(ErrorPrototype parent, Object e)
        : base(parent) {
        this.exception = e;
        this.number = unchecked((int)(0x800A0000 + (int)JSError.UncaughtException));
        if (e is Exception){
          if (e is JScriptException)
            this.number = ((JScriptException)e).Number;
          else if (e is ExternalException)
            this.number = ((ExternalException)e).ErrorCode;
          this.description = ((Exception)e).Message;
          if (((String)this.description).Length == 0)
            this.description = e.GetType().FullName;
        }
        this.message = this.description;
        this.noExpando = false;
      }
      
      internal override String GetClassName(){
        return "Error";
      }
      
      internal String Message{
        get{
          return Convert.ToString(this.message);
         }
      }

      public static explicit operator Exception(ErrorObject err){
        return err.exception as Exception;
      }
      
      public static Exception ToException(ErrorObject err){
        return (Exception)err;
      }
    }
}
