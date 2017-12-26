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
Imports System.Workflow.Activities

Public Class FileWatcherWorkflow
    Inherits SequentialWorkflowActivity

    Private Sub BeforeFileSystemEvent(ByVal sender As System.Object, ByVal e As System.EventArgs)
        Dim activity As FileSystemEvent = TryCast(sender, FileSystemEvent)

        Console.WriteLine("FileSystemEvent '" + activity.Name + "' is about to execute...")
        Console.WriteLine(" Path = " + activity.Path)
    End Sub

    Private Sub AfterFileSystemEvent(ByVal sender As System.Object, ByVal e As FileWatcherEventArgs)
        Dim activity As FileSystemEvent = TryCast(sender, FileSystemEvent)

        Console.WriteLine("FileSystemEvent '" + activity.Name + "' is handling an event...")
    End Sub
End Class
