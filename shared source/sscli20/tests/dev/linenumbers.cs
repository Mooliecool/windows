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

// verify correctness of the line numbers in exceptions

using System;

class MainApp {

    static int g_delta;
    static bool g_linesavailable;

    static int[] ExtractLineNumbers(string stacktrace) {    

        int index;
        int count;
        int[] arr;

        // count the number of occurences of ":line "
        count = 0;
        index = 0;
        for (;;) {
            index = stacktrace.IndexOf(":line ", index);
            if (index == -1)
                break;
            index += 6;
            count++;
        }

        if (count == 0)
            return null;

        arr = new int[count];
        
        index = 0;
        count = 0;        
        for (;;) {
            index = stacktrace.IndexOf(":line ", index);
            if (index == -1)
                break;
            index += 6;
            
            int lineend = stacktrace.IndexOf("\n", index);
            if (lineend == -1)
                lineend = stacktrace.Length;

            arr[count] = Int32.Parse(stacktrace.Substring(index, lineend - index));
            count++;
        }

        return arr;
    }

    static bool MatchLineNumbers(int[] actual, int[] expected) {   
    
        if (!g_linesavailable) {
            if (actual == null)
                return true;
            else
                return false;
        }

        if (actual == null) {
            return false;
        }

        if (actual.Length != expected.Length)
            return false;

        for (int i = 0; i < actual.Length; i++) {
            if (actual[i] - g_delta != expected[i])
                return false; 
        }

        return true;
    }

    static void VerifyException(Exception e, int[] stack) {   

        Console.WriteLine(e.ToString());    
    
        if (!MatchLineNumbers(ExtractLineNumbers(e.StackTrace), stack)) {
            Console.WriteLine("FAILED: Line numbers do not match");
            Environment.Exit(1);
        }               
    }

    static void foo() {
       try {
           throw new Exception();
       }
       catch (Exception e) {           
           VerifyException(e, new int[] {90});
       }

       try {
           throw new Exception();
       }
       catch (Exception e) {
           VerifyException(e, new int[] {97});
       }
   }

   static void bar() {
       throw new Exception();
   }
   
   public static void Main() {

      int[] line = ExtractLineNumbers(Environment.StackTrace); 
      g_linesavailable = (line != null);

      if (g_linesavailable)
          g_delta = line[line.Length-1] - 110;
      else
          Console.WriteLine("WARNING: Line numbers are not available!");

      try {
          foo();
          bar();
      }
      catch (Exception e) {
           VerifyException(e, new int[] {105,120});
      }

      try {
          throw new Exception();
      }
      catch (Exception e) {
          VerifyException(e, new int[] {127});
      }

      Console.WriteLine("PASSED");
   }
}
