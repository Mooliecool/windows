'-----------------------------------------------------------------------
'  This file is part of the Microsoft .NET Framework SDK Code Samples.
' 
'  Copyright (C) Microsoft Corporation.  All rights reserved.
' 
'This source code is intended only as a supplement to Microsoft
'Development Tools and/or on-line documentation.  See these other
'materials for detailed information regarding Microsoft code samples.
' 
'THIS CODE AND INFORMATION ARE PROVIDED AS IS WITHOUT WARRANTY OF ANY
'KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
'IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
'PARTICULAR PURPOSE.
'/*=====================================================================
'  File:      Monitor.vb
'
'  Summary:   Demonstrates how to use the Monitor class for thread 
'             synchronization.
'
'=====================================================================*/

Option Explicit On 
Option Strict On

Imports System
Imports System.Threading


Class Resource 
   Public Sub Access(threadNum as Integer) 
      SyncLock (me) 
         Console.WriteLine("Start Resource access (Thread={0})", threadNum)
         Thread.Sleep(500)
         Console.WriteLine("Stop  Resource access (Thread={0})", threadNum)
      End SyncLock
   End Sub
End Class


Class App
   Const numThreadCount As Integer = 5
   Shared dim numAsyncOps as Integer = numThreadCount
   Shared dim asyncOpsAreDone as new AutoResetEvent(false)
   Shared dim res as new Resource()

   Public Shared Sub Main() 
      Dim threadnum as Integer
      For threadnum = 0 to numThreadCount - 1 
         ThreadPool.QueueUserWorkItem(new WaitCallback(AddressOf UpdateResource), threadNum)
      Next threadnum	

      asyncOpsAreDone.WaitOne()
      Console.WriteLine("All operations have completed.")
   End Sub


   ' The callback method's signature MUST match that of a System.Threading.TimerCallback 
   ' delegate (it takes an Object parameter and returns void)
   Shared Sub UpdateResource(state as Object) 
      res.Access(CType(state, Integer))
      If Interlocked.Decrement(numAsyncOps) = 0 Then
         asyncOpsAreDone.Set()
      End If 
   End Sub
End Class
