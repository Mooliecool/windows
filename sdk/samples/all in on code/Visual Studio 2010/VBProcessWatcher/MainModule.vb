'************************************ Module Header **************************************
'* Module Name:	MainModule.vb
'* Project:		VBProcessWatcher
'* Copyright (c) Microsoft Corporation.
'* 
'* This project illustrates how to watch the process creation/modify/shutdown events
'* by using Windows Management Instrumentation(WMI).
'*
'* This source is subject to the Microsoft Public License.
'* See http://www.microsoft.com/opensource/licenses.mspx#Ms-PL.
'* All other rights reserved.
'* 
'* THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY KIND,
'* EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED
'* WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR PURPOSE.
'*****************************************************************************************
Imports VBProcessWatcher.WMI
Imports VBProcessWatcher.VBProcessWatcher

    Module MainModule

    Dim processName As String = "notepad.exe" ' Default process name.

        Sub Main(ByVal args As String())
            If args.Length > 0 Then
                processName = args(0)
            End If

            Dim procWatcher As New ProcessWatcher(processName)
            AddHandler procWatcher.ProcessCreated, AddressOf procWatcher_ProcessCreated
            AddHandler procWatcher.ProcessDeleted, AddressOf procWatcher_ProcessDeleted
            AddHandler procWatcher.ProcessModified, AddressOf procWatcher_ProcessModified
            
            procWatcher.Start()

            Console.WriteLine(processName & " is under watch...")
            Console.WriteLine("Press Enter to stop watching...")

            Console.ReadLine()

            procWatcher.Stop()
        End Sub

    Private Sub procWatcher_ProcessCreated(ByVal proc As Win32.Process)
        Console.Write(vbCrLf & "Created " & vbCrLf & proc.Name & " " & proc.ProcessId & "  " & "DateTime:" & DateTime.Now)
    End Sub


    Private Sub procWatcher_ProcessDeleted(ByVal proc As Win32.Process)
        Console.Write(vbCrLf & "Deleted " & vbCrLf & proc.Name & " " & proc.ProcessId & "  " & "DateTime:" & DateTime.Now)
    End Sub

    Private Sub procWatcher_ProcessModified(ByVal proc As Win32.Process)
        Console.Write(vbCrLf & "Modified" & vbCrLf & proc.Name & " " & proc.ProcessId & "  " & "DateTime:" & DateTime.Now)
    End Sub
    End Module


