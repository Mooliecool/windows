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

namespace Microsoft.JScript{
    
    using System;
    using System.Diagnostics;

    static internal class Debug{

      [Conditional("ASSERTION")]     
      public static void Assert(bool condition){
        if (!condition)
          throw new AssertException("Assertion fired");
      }
      
      [Conditional("ASSERTION")]     
      public static void Assert(bool condition, String message){
        if (!condition)
          throw new AssertException(message);
      }
      
      [Conditional("ASSERTION")]     
      public static void NotImplemented(String message){
        throw new AssertException("Method Not Yet Implemented");
      }
      
      [Conditional("ASSERTION")]     
      public static void PostCondition(bool condition){
        if (!condition)
          throw new PostConditionException("PostCondition missed");
      }
      
      [Conditional("ASSERTION")]     
      public static void PostCondition(bool condition, String message){
        if (!condition)
          throw new PostConditionException(message);
      }
      
      [Conditional("ASSERTION")]     
      public static void PreCondition(bool condition){
        if (!condition)
          throw new PreConditionException("PreCondition missed");
      }

      [Conditional("ASSERTION")]     
      public static void PreCondition(bool condition, String message){
        if (!condition)
          throw new PreConditionException(message);
      }
      
      [Conditional("LOGGING")]     
      public static void Print(String str){
        ScriptStream.Out.WriteLine(str);
      }
      
      [Conditional("LOGGING")]     
      internal static void PrintLine(String message) {
        ScriptStream.Out.WriteLine(message);
      }

      [Conditional("LOGGING")]     
      public static void PrintStack(){
        ScriptStream.PrintStackTrace();
      }
      
      [Conditional("LOGGING")]     
      public static void PrintStack(Exception e){
        ScriptStream.PrintStackTrace(e);
      }
      
    }
  
    internal class AssertException : Exception{
      internal AssertException(String message)
        : base(message) {
      }
    }
  
    internal class PreConditionException : AssertException{
      internal PreConditionException(String message)
        : base(message) {
      }
    }
  
    internal class PostConditionException : AssertException{
      internal PostConditionException(String message)
        : base(message) {
      }
    }
}
