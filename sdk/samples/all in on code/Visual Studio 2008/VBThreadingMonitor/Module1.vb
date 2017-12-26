'****************************** Module Header ******************************'
' Module Name:	Module1.vb
' Project:		VBThreadingMonitor
' Copyright (c) Microsoft Corporation.
' 
' This source is subject to the Microsoft Public License.
' See http://www.microsoft.com/opensource/licenses.mspx#Ms-PL.
' All other rights reserved.
' 
' History:
' * 3/22/2010 1:25 AM Colbert Zhou Created
'***************************************************************************'

Imports System.Threading

Module Module1

    Sub Main()
        Dim result As Integer = 0
        Dim cell As New Cell
        Dim prod As New CellProd(cell, 20)
        Dim cons As New CellCons(cell, 20)
        Dim producer As New Thread(New ThreadStart(AddressOf prod.ThreadRun))
        Dim consumer As New Thread(New ThreadStart(AddressOf cons.ThreadRun))
        Try
            producer.Start()
            consumer.Start()
            producer.Join()
            consumer.Join()
        Catch e As ThreadStateException
            Console.WriteLine(e)
            result = 1
        Catch e As ThreadInterruptedException
            Console.WriteLine(e)
            result = 1
        End Try
        Environment.ExitCode = result
    End Sub

    Public Class Cell
        Public Function ReadFromCell() As Integer
            SyncLock Me
                If Not Me.readerFlag Then
                    Try
                        Monitor.Wait(Me)
                    Catch e As SynchronizationLockException
                        Console.WriteLine(e)
                    Catch e As ThreadInterruptedException
                        Console.WriteLine(e)
                    End Try
                End If
                Console.WriteLine("Consume: {0}", Me.cellContents)
                Me.readerFlag = False
                Monitor.Pulse(Me)
            End SyncLock
            Return Me.cellContents
        End Function

        Public Sub WriteToCell(ByVal n As Integer)
            SyncLock Me
                If Me.readerFlag Then
                    Try
                        Monitor.Wait(Me)
                    Catch e As SynchronizationLockException
                        Console.WriteLine(e)
                    Catch e As ThreadInterruptedException
                        Console.WriteLine(e)
                    End Try
                End If
                Me.cellContents = n
                Console.WriteLine("Produce: {0}", Me.cellContents)
                Me.readerFlag = True
                Monitor.Pulse(Me)
            End SyncLock
        End Sub


        ' Fields
        Private cellContents As Integer
        Private readerFlag As Boolean = False
    End Class

    Public Class CellCons
        ' Methods
        Public Sub New(ByVal box As Cell, ByVal request As Integer)
            Me.cell = box
            Me.quantity = request
        End Sub

        Public Sub ThreadRun()
            Dim looper As Integer = 1
            Do While (looper <= Me.quantity)
                Dim valReturned As Integer = Me.cell.ReadFromCell
                looper += 1
            Loop
        End Sub


        ' Fields
        Private cell As Cell
        Private quantity As Integer = 1
    End Class

    Public Class CellProd
        ' Methods
        Public Sub New(ByVal box As Cell, ByVal request As Integer)
            Me.cell = box
            Me.quantity = request
        End Sub

        Public Sub ThreadRun()
            Dim looper As Integer = 1
            Do While (looper <= Me.quantity)
                Me.cell.WriteToCell(looper)
                looper += 1
            Loop
        End Sub


        ' Fields
        Private cell As Cell
        Private quantity As Integer = 1
    End Class
End Module


