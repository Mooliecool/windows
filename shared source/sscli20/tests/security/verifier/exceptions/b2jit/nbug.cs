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

using System;
 

class TestClass {

   public static void Main(System.String[] arg) {

      try {

       try {
          Console.WriteLine("Inside the try");
          throw new Exception();

       }

       finally {
           Console.WriteLine("Inside the finally");
           throw new Exception();

       }

      }

      catch (Exception)

      {
        Console.WriteLine("Inside the handler");
      }

   }

}

