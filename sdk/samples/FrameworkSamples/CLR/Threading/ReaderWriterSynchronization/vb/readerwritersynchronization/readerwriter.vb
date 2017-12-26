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
'=====================================================================
'  File:      ReaderWriter.vb
'
'  Summary:   Demonstrates use of ReaderWriterLock synchronization
'
'=====================================================================

Option Explicit On 
Option Strict On


Imports System
Imports System.Threading


Class Resource
    Private rwl As New ReaderWriterLock()
        
    Public Sub Read(threadNum As Int32)
        rwl.AcquireReaderLock(Timeout.Infinite)
        Try
            Console.WriteLine("Start Resource reading (Thread={0})", threadNum)
            Thread.Sleep(250)
            Console.WriteLine("Stop  Resource reading (Thread={0})", threadNum)
        Finally
            rwl.ReleaseReaderLock()
        End Try
    End Sub 'Read
    
    
    Public Sub Write(threadNum As Int32)
        rwl.AcquireWriterLock(Timeout.Infinite)
        Try
            Console.WriteLine("Start Resource writing (Thread={0})", threadNum)
            Thread.Sleep(750)
            Console.WriteLine("Stop  Resource writing (Thread={0})", threadNum)
        Finally
            rwl.ReleaseWriterLock()
        End Try
    End Sub 'Write
End Class 'Resource



Class App
    Private Shared numAsyncOps As Int32 = 20
    Private Shared asyncOpsAreDone As New AutoResetEvent(False)
    Private Shared res As New Resource()
    
    
    Public Shared Sub Main()
        Dim threadNum As Int32
        For threadNum = 0 To 19
            ThreadPool.QueueUserWorkItem(New WaitCallback(AddressOf UpdateResource), threadNum)
        Next threadNum
        
        asyncOpsAreDone.WaitOne()
        Console.WriteLine("All operations have completed.")
    End Sub 'Main
    
    
    
    ' The callback method's signature MUST match that of a System.Threading.TimerCallback 
    ' delegate (it takes an Object parameter and returns void)
    Shared Sub UpdateResource(state As Object)
        Dim threadNum As Int32 = CType(state, Int32)
        If threadNum Mod 2 <> 0 Then
            res.Read(threadNum)
        Else
            res.Write(threadNum)
        End If 
        If Interlocked.Decrement(numAsyncOps) = 0 Then
            asyncOpsAreDone.Set()
        End If
    End Sub 'UpdateResource
End Class 'App




































