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
'-----------------------------------------------------------------------
'=====================================================================
'  File:      Mutex.vb
'
'  Summary:   Demonstrates how to wait for an object to go signalled
'
'---------------------------------------------------------------------


Option Explicit On 
Option Strict On


Imports System
Imports System.Threading


Class Resource
    Private m As New Mutex()
        
    Public Sub Access(threadNum As Int32)
        m.WaitOne()
        Try
            Console.WriteLine("Start Resource access (Thread={0})", threadNum)
            Thread.Sleep(500)
            Console.WriteLine("Stop  Resource access (Thread={0})", threadNum)
        Finally
            m.ReleaseMutex()
        End Try
    End Sub 'Access
End Class 'Resource



Class App
    Private Shared numAsyncOps As Int32 = 5
    Private Shared asyncOpsAreDone As New AutoResetEvent(False)
    Private Shared res As New Resource()
    
    
    Public Shared Sub Main()
        Dim threadNum As Int32
        For threadNum = 0 To 4
            ThreadPool.QueueUserWorkItem(New WaitCallback(AddressOf UpdateResource), threadNum)
        Next threadNum
        
        asyncOpsAreDone.WaitOne()
        Console.WriteLine("All operations have completed.")
    End Sub 'Main
    
    
    
    ' The callback method's signature MUST match that of a System.Threading.TimerCallback 
    ' delegate (it takes an Object parameter and returns void)
    Shared Sub UpdateResource(state As Object)
        res.Access(CType(state, Int32))
        If Interlocked.Decrement(numAsyncOps) = 0 Then
            asyncOpsAreDone.Set()
        End If
    End Sub 'UpdateResource
End Class 'App
