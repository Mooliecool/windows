'---------------------------------------------------------------------
'  This file is part of the Windows Workflow Foundation SDK Code Samples.
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
'---------------------------------------------------------------------

Imports System
Imports System.Threading
Imports System.Collections.Generic

Public NotInheritable Class ThreadMonitor
    Private Shared threadCount As Integer
    Private Shared threadList As New Dictionary(Of String, ConsoleColor)

    Private Sub New()
        ' Static invocation only
    End Sub

    Public Shared Sub Enlist(ByVal thread As Thread, ByVal instanceName As String)
        If (thread.Name Is Nothing) OrElse (thread.Name.Length = 0) Then
            thread.Name = String.Format("{0} Thread [{1}]", instanceName, threadCount)
            threadCount = threadCount + 1
            If Not threadList.ContainsKey(thread.Name) Then
                threadList.Add(thread.Name, GetConsoleColor())
            End If
        End If
    End Sub

    Public Shared Sub WriteToConsole(ByVal thread As Thread, ByVal instanceName As String, ByVal message As String)
        Enlist(thread, instanceName)
        WriteToConsole(thread, message)
    End Sub

    Public Shared Sub WriteToConsole(ByVal thread As Thread, ByVal message As String)
        If threadList.ContainsKey(thread.Name) Then
            Console.ForegroundColor = threadList(thread.Name)
        End If
        Console.WriteLine("{0} --> {1}", thread.Name, message)
    End Sub

    Private Shared Function GetConsoleColor() As ConsoleColor
        If CType(Console.ForegroundColor < 9, Integer) Then
            Return ConsoleColor.White
        Else
            Return CType(CType(Console.ForegroundColor, Integer) - 1, ConsoleColor)
        End If
    End Function
End Class
