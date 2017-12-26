'***************************** Module Header *******************************\
' Module Name:  MainModule.vb
' Project:      VBThreadingBasic
' Copyright (c) Microsoft Corporation.
' 
' This example demonstrates how to create threads using VB.NET in three 
' different approaches. And it also shows how to create a thread that require
' a parameter. In the target threads, it also shows how to use SyncLock 
' keyword to ensure a code snippet executed without interruption.
' 
' This source is subject to the Microsoft Public License.
' See http://www.microsoft.com/opensource/licenses.mspx#Ms-PL.
' All other rights reserved.
' 
' THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY KIND, 
' EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED 
' WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR PURPOSE.
'***************************************************************************/



Imports System.Threading

Module MainModule

    Dim i As Integer = 0
    Dim o As Object = New Object


    Sub Main()
        Console.WriteLine(("Main Thread's managed thread id: " & Thread.CurrentThread.ManagedThreadId.ToString))

        Dim ts1 As ThreadStart = New ThreadStart(AddressOf ThreadFunction1)
        Dim t1 = New Thread(ts1)
        t1.Start()

        ThreadPool.QueueUserWorkItem(New WaitCallback(AddressOf ThreadFunction2))

        Dim ts2 = New ThreadStart(AddressOf ThreadFunction3)
        ts2.BeginInvoke(Nothing, Nothing)

        Dim pts As ParameterizedThreadStart = New ParameterizedThreadStart(AddressOf ThreadFunction4)
        Dim t2 = New Thread(pts)
        t2.Start("Message")

        Console.ReadKey()

    End Sub

    Private Sub ThreadFunction1()
        SyncLock o
            i = i + 1
            Console.WriteLine((ChrW(13) & ChrW(10) & "Method1: Current thread's managed thread id: " & Thread.CurrentThread.ManagedThreadId.ToString))
            Console.WriteLine((i).ToString)
        End SyncLock
    End Sub

    Private Sub ThreadFunction2(ByVal stateInfo As Object)
        SyncLock o
            i = i + 1
            Console.WriteLine((ChrW(13) & ChrW(10) & "Method2: Current thread's managed thread id: " & Thread.CurrentThread.ManagedThreadId.ToString))
            Console.WriteLine((i).ToString)
        End SyncLock
    End Sub

    Private Sub ThreadFunction3()
        SyncLock o
            i = i + 1
            Console.WriteLine((ChrW(13) & ChrW(10) & "Method3: Current thread's managed thread id: " & Thread.CurrentThread.ManagedThreadId.ToString))
            Console.WriteLine((i).ToString)
        End SyncLock
    End Sub

    Private Sub ThreadFunction4(ByVal message As Object)
        SyncLock o
            i = i + 1
            Console.WriteLine(String.Concat(New Object() {ChrW(13) & ChrW(10) & "Method4: Current thread's managed thread id: ", Thread.CurrentThread.ManagedThreadId.ToString, ChrW(9), message}))
            Console.WriteLine((i).ToString)
        End SyncLock
    End Sub


End Module
