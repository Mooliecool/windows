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
/*=====================================================================
  File:      JoinThread.cs

  Summary:   Demonstrates how to wait for another thread to exit

=====================================================================*/


using System;
using System.Threading;

class App {
   static void MyThreadMethod() {
      Console.WriteLine("This is the secondary thread running.");
   }

   static void Main() {
      Console.WriteLine("This is the primary thread running.");
      // MyThreadMethod is the secondary thread's entry point.
      Thread t = new Thread(new ThreadStart(MyThreadMethod));

      // Start the thread
      t.Start();

      // Wait for the thread to exit
      t.Join();
      Console.WriteLine("The secondary thread has terminated.");
   }
}
