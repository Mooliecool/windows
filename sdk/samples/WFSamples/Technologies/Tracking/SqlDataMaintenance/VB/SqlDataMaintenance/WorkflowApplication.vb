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

Imports Microsoft.VisualBasic
Imports System
Imports System.Threading
Imports System.Workflow.Runtime
Imports System.Workflow.Runtime.Tracking

Class WorkflowApplication
    Shared WaitHandle As New AutoResetEvent(False)

    Shared Sub Main()
        Try
            WaitHandle = New AutoResetEvent(False)

            DataAccess.CreateAndInsertTrackingProfile()
            Using runtime As New WorkflowRuntime()
                AddHandler runtime.WorkflowCompleted, AddressOf OnWorkflowCompleted
                AddHandler runtime.WorkflowTerminated, AddressOf OnWorkflowTerminated

                Dim trackingService As New SqlTrackingService(DataAccess.connectionString)

                ''
                ''  Set partitioning settings on Sql Tracking Service and database
                ''

                'Turn on PartitionOnCompletion setting-- Default is false
                trackingService.PartitionOnCompletion = True

                'Set partition interval-- Default is 'm' (monthly
                DataAccess.SetPartitionInterval("d"c)

                runtime.AddService(trackingService)
                runtime.StartRuntime()


                Dim instance As WorkflowInstance = runtime.CreateWorkflow(GetType(SimpleWorkflow))
                instance.Start()
                WaitHandle.WaitOne()

                runtime.StopRuntime()
                DataAccess.GetWorkflowTrackingEvents(instance.InstanceId)
                Console.WriteLine(vbCrLf + "Done running the workflow.")

                ''
                ''  Show tracking partition information and tables
                ''

                DataAccess.ShowTrackingPartitionInformation()
                DataAccess.ShowPartitionTableInformation()
            End Using

        Catch ex As Exception
            If ex.InnerException IsNot Nothing Then
                Console.WriteLine(ex.InnerException.Message)
            Else
                Console.WriteLine(ex.Message)
            End If
        End Try
    End Sub

    Shared Sub OnWorkflowCompleted(ByVal sender As Object, ByVal e As WorkflowCompletedEventArgs)
        WaitHandle.Set()
    End Sub

    Shared Sub OnWorkflowTerminated(ByVal sender As Object, ByVal e As WorkflowTerminatedEventArgs)
        Console.WriteLine(e.Exception.Message)
        WaitHandle.Set()
    End Sub

End Class


