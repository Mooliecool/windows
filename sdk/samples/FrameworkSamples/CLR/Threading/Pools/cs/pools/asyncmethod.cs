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
  File:      AsyncMethod.cs

  Summary:   Demonstrates thread pool usage

=====================================================================*/

using System;
using System.Threading;


class App {
   public static void Main() {
      Console.WriteLine("Main thread: Queuing an asynchronous operation.");
      AutoResetEvent asyncOpIsDone = new AutoResetEvent(false);
      ThreadPool.QueueUserWorkItem(new WaitCallback(MyAsyncOperation), asyncOpIsDone);

      Console.WriteLine("Main thread: Performing other operations.");
      // ...

      Console.WriteLine("Main thread: Waiting for asynchronous operation to complete.");
      asyncOpIsDone.WaitOne();
   }


   // The callback method's signature MUST match that of a System.Threading.WaitCallback 
   // delegate (it takes an Object parameter and returns void)
   static void MyAsyncOperation(Object state) {
      Console.WriteLine("WorkItem thread: Performing asynchronous operation.");
      // ...
      Thread.Sleep(5000);	// Sleep for 5 seconds to simulate doing work

      // Signal that the async operation is now complete.
      ((AutoResetEvent)state).Set();
   }
}
