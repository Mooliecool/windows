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

  //*************************************************************************************
  // ScriptStream
  //
  //  This class holds a couple of globals that are used to output messages.
  //  Being public the two globals can be overriden to catch messages.
  //*************************************************************************************
  public class ScriptStream {
    public static System.IO.TextWriter Out = Console.Out;
    public static System.IO.TextWriter Error = Console.Error;
  
    public static void PrintStackTrace() {
      try {
        throw new Exception();
      }catch(Exception e){
        ScriptStream.PrintStackTrace(e);
      }
    }
  
    public static void PrintStackTrace(Exception e) {
      ScriptStream.Out.WriteLine(e.StackTrace);
      ScriptStream.Out.Flush();
    }
    
    public static void Write(String str){
      ScriptStream.Out.Write(str);
    }
    
    public static void WriteLine(String str){
      ScriptStream.Out.WriteLine(str);
    }
  }


}
