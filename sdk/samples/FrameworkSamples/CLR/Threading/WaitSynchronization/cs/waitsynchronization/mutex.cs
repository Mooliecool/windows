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
//-----------------------------------------------------------------------
/*=====================================================================
  File:      Mutex.cs

  Summary:   Demonstrates how to wait for an object to go signalled
*/


using System;
using System.Threading;


class Resource {
   Mutex m = new Mutex();

   public void Access(Int32 threadNum) {
      m.WaitOne();
      try {
         Console.WriteLine("Start Resource access (Thread={0})", threadNum);
         Thread.Sleep(500);
         Console.WriteLine("Stop  Resource access (Thread={0})", threadNum);
      }
      finally {
         m.ReleaseMutex();
      }
   }
}


class App {
   static Int32 numAsyncOps = 5;
   static AutoResetEvent asyncOpsAreDone = new AutoResetEvent(false);
   static Resource res = new Resource();

   public static void Main() {
      for (Int32 threadNum = 0; threadNum < 5; threadNum++) {
         ThreadPool.QueueUserWorkItem(new WaitCallback(UpdateResource), threadNum);
      }

      asyncOpsAreDone.WaitOne();
      Console.WriteLine("All operations have completed.");
   }


   // The callback method's signature MUST match that of a System.Threading.TimerCallback 
   // delegate (it takes an Object parameter and returns void)
   static void UpdateResource(Object state) {
      res.Access((Int32) state);
      if (Interlocked.Decrement(ref numAsyncOps) == 0)
         asyncOpsAreDone.Set();
   }
}
