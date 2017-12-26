//-----------------------------------------------------------------------
//  This file is part of the Microsoft .NET Framework SDK Code Samples.
// 
//  Copyright (C) Microsoft Corporation.  All rights reserved.
// 
//This source code is intended only as a supplement to Microsoft
//Development Tools and/or on-line documentation.  See these other
//materials for detailed information regarding Microsoft code samples.
// 
//THIS CODE AND INFORMATION ARE PROVIDED AS IS WITHOUT WARRANTY OF ANY
//KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
//IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
//PARTICULAR PURPOSE.
//
/*=====================================================================
  File:      Sample2Caller.cs

  Summary:   Demonstrates how to use reflection emit.

=====================================================================*/

using System;

public class App {
   public static void Main() {
      try{
         UseHelloWorld();
      }catch(TypeLoadException){
         Console.WriteLine("Unable to load HelloWorld type "+
            "from EmittedAssembly.dll!\nExecute EmitAssembly "+
            "with option 2 from the command line to build the assembly.");
      }
   }

   private static void UseHelloWorld(){
      HelloWorld h = new HelloWorld("HelloWorld!");
      Console.WriteLine(h.GetGreeting());
   }
}