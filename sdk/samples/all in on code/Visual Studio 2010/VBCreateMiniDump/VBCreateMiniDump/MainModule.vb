'*************************** Module Header ******************************'
' Module Name:  MainModule.vb
' Project:      VBCreateMiniDump
' Copyright (c) Microsoft Corporation.
' 
' When this application starts, it will attach a debugger to the given process. 
' 
' This source is subject to the Microsoft Public License.
' See http://www.microsoft.com/opensource/licenses.mspx#Ms-PL.
' All other rights reserved.
' 
' THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY KIND, 
' EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED 
' WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR PURPOSE.
'*************************************************************************'

Imports System.Diagnostics
Imports System.Security.Permissions
Imports System.ComponentModel

Public Module MainModule
    <MTAThread()>
    <PermissionSet(SecurityAction.LinkDemand, Name:="FullTrust"),
 PermissionSet(SecurityAction.InheritanceDemand, Name:="FullTrust")>
    Sub Main(ByVal args() As String)
        Dim _processID As Integer = 0
        Dim _process As ManagedProcess = Nothing
        Dim _targetProcess As Process = Nothing

        ' The argument must be a number that represents the ID of the target process
        If args.Length = 1 Then
            Integer.TryParse(args(0), _processID)
            _targetProcess = Process.GetProcessById(_processID)
        End If

        ' Must point to a valid process.
        Do While _targetProcess Is Nothing
            Console.WriteLine("Please type the ID of the process to be debugged:")
            Dim idStr As String = Console.ReadLine()
            Integer.TryParse(idStr, _processID)
            _targetProcess = Process.GetProcessById(_processID)
        Loop

        Try
            Console.WriteLine("Process Name: " & _targetProcess.ProcessName)
            Console.WriteLine("Process ID:   " & _targetProcess.Id)
            Console.WriteLine("Start to  Watch...")

            ' Attach a debugger to the target process.
            _process = New ManagedProcess(_targetProcess)

            ' This event occurs when the watch dog start to handle the exception.
            ' If it is cancelled, the watch dog will not continue to handle the 
            ' exception.
            AddHandler _process.StartHandleException,
                AddressOf process_StartHandleException

            ' This event occurs when the watch dog has got the information of the 
            ' unhandled exception.
            AddHandler _process.UnhandledExceptionOccurred,
                AddressOf process_UnhandledExceptionOccurred

            ' Start to debug the target process.
            _process.StartWatch()

        Catch ex As Exception
            Console.WriteLine("Failed to create minidump: " & ex.Message)
        Finally
            If _process IsNot Nothing Then
                _process.Dispose()
            End If
        End Try

        Console.WriteLine("Press ENTER to continue...")
        Console.ReadLine()

    End Sub

    ''' <summary>
    ''' Create a minidump if there is a unhandled exception in the target process.
    ''' </summary>
    Private Sub process_UnhandledExceptionOccurred(ByVal sender As Object,
                                                   ByVal e As ManagedProcessUnhandledExceptionOccurredEventArgs)
        Console.WriteLine("Creating Minidump...")

        Try
            Dim dumpFilePath =
                MiniDump.MiniDumpCreator.CreateMiniDump(e.ProcessID, e.ThreadID, e.ExceptionPointers)
            Console.WriteLine("The minidump file is {0}", dumpFilePath)
            Console.WriteLine("Done...")
        Catch ex As Exception
            Console.WriteLine("Failed to create minidump:")
            Console.WriteLine(ex.Message)
        End Try

    End Sub

    ''' <summary>
    ''' Show the message when the watch dog starts to handle the exception.
    ''' </summary>
    Private Sub process_StartHandleException(ByVal sender As Object,
                                             ByVal e As CancelEventArgs)
        Console.WriteLine("Start to handle exception...")
        Console.WriteLine("Getting exception information...")
    End Sub

End Module
