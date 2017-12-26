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
  File:      ReaderWriter.cs

  Summary:   Demonstrates use of ReaderWriterLock synchronization

=====================================================================*/

using System;
using System.Threading;


class Resource {
   ReaderWriterLock rwl = new ReaderWriterLock();

   public void Read(Int32 threadNum) {
      rwl.AcquireReaderLock(Timeout.Infinite);
      try {
         Console.WriteLine("Start Resource reading (Thread={0})", threadNum);
         Thread.Sleep(250);
         Console.WriteLine("Stop  Resource reading (Thread={0})", threadNum);
      }
      finally {
         rwl.ReleaseReaderLock();
      }
   }

   public void Write(Int32 threadNum) {
      rwl.AcquireWriterLock(Timeout.Infinite);
      try {
         Console.WriteLine("Start Resource writing (Thread={0})", threadNum);
         Thread.Sleep(750);
         Console.WriteLine("Stop  Resource writing (Thread={0})", threadNum);
      }
      finally {
         rwl.ReleaseWriterLock();
      }
   }
}


class App {
   static Int32 numAsyncOps = 20;
   static AutoResetEvent asyncOpsAreDone = new AutoResetEvent(false);
   static Resource res = new Resource();

   public static void Main() {
      for (Int32 threadNum = 0; threadNum < 20; threadNum++) {
         ThreadPool.QueueUserWorkItem(new WaitCallback(UpdateResource), threadNum);
      }

      asyncOpsAreDone.WaitOne();
      Console.WriteLine("All operations have completed.");
   }


   // The callback method's signature MUST match that of a System.Threading.TimerCallback 
   // delegate (it takes an Object parameter and returns void)
   static void UpdateResource(Object state) {
      Int32 threadNum = (Int32) state;
      if ((threadNum % 2) != 0) res.Read(threadNum);
      else res.Write(threadNum);

      if (Interlocked.Decrement(ref numAsyncOps) == 0)
         asyncOpsAreDone.Set();
   }
}
