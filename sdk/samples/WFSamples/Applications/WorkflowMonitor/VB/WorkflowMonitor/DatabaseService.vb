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
Imports System.Globalization
Imports System.Workflow.Runtime.Tracking


' This class provides database query services against the TrackingService
' via several stored procedure that must be installed prior to running
' the monitor.
Public Class DatabaseService
    Private connectionString As String
    Private serverNameValue As String = String.Empty
    Private databaseNameValue As String = String.Empty

    Friend Sub New()
        connectionString = "Persist Security Info=False;Integrated Security=SSPI;database=" + databaseNameValue + ";server=" + serverNameValue
    End Sub

    Friend Property ServerName() As String
        Get
            Return serverNameValue
        End Get
        Set(ByVal Value As String)
            serverNameValue = Value
            connectionString = "Persist Security Info=False;Integrated Security=SSPI;database=" + databaseNameValue + ";server=" + serverNameValue
        End Set
    End Property

    Friend Property DatabaseName() As String
        Get
            Return databaseNameValue
        End Get
        Set(ByVal Value As String)
            databaseNameValue = Value
            connectionString = "Persist Security Info=False;Integrated Security=SSPI;database=" + databaseNameValue + ";server=" + serverNameValue

        End Set
    End Property


    ' Query for all workflows from the SqlTrackingService using the 'GetWorkflowStatus' stored procedure
    Friend Function GetWorkflows( _
            ByVal workflowEvent As String, _
            ByVal from As System.DateTime, _
            ByVal until As DateTime, _
            ByVal trackingDataItemValue As TrackingDataItemValue) _
            As List(Of SqlTrackingWorkflowInstance)
        Try
            Dim queriedWorkflows As List(Of SqlTrackingWorkflowInstance) = New List(Of SqlTrackingWorkflowInstance)()
            Dim sqlTrackingQuery As SqlTrackingQuery = New SqlTrackingQuery(connectionString)
            Dim sqlTrackingQueryOptions As SqlTrackingQueryOptions = New SqlTrackingQueryOptions()
            sqlTrackingQueryOptions.StatusMinDateTime = from.ToUniversalTime()
            sqlTrackingQueryOptions.StatusMaxDateTime = until.ToUniversalTime()
            ' If QualifiedName, FieldName, or DataValue is not supplied, we will not query since they are all required to match
            If (Not ((trackingDataItemValue.QualifiedName = String.Empty) Or (trackingDataItemValue.FieldName = String.Empty) Or ((trackingDataItemValue.DataValue = String.Empty)))) Then
                sqlTrackingQueryOptions.TrackingDataItems.Add(trackingDataItemValue)
            End If

            queriedWorkflows.Clear()
            If (workflowEvent.ToLower(CultureInfo.InvariantCulture) = "created") Then
                sqlTrackingQueryOptions.WorkflowStatus = System.Workflow.Runtime.WorkflowStatus.Created
                queriedWorkflows.AddRange(sqlTrackingQuery.GetWorkflows(sqlTrackingQueryOptions))
            ElseIf (workflowEvent.ToLower(CultureInfo.InvariantCulture) = "completed") Then
                sqlTrackingQueryOptions.WorkflowStatus = System.Workflow.Runtime.WorkflowStatus.Completed
                queriedWorkflows.AddRange(sqlTrackingQuery.GetWorkflows(sqlTrackingQueryOptions))
            ElseIf (workflowEvent.ToLower(CultureInfo.InvariantCulture) = "running") Then
                sqlTrackingQueryOptions.WorkflowStatus = System.Workflow.Runtime.WorkflowStatus.Running
                queriedWorkflows.AddRange(sqlTrackingQuery.GetWorkflows(sqlTrackingQueryOptions))
            ElseIf (workflowEvent.ToLower(CultureInfo.InvariantCulture) = "suspended") Then
                sqlTrackingQueryOptions.WorkflowStatus = System.Workflow.Runtime.WorkflowStatus.Suspended
                queriedWorkflows.AddRange(sqlTrackingQuery.GetWorkflows(sqlTrackingQueryOptions))
            ElseIf (workflowEvent.ToLower(CultureInfo.InvariantCulture) = "terminated") Then
                sqlTrackingQueryOptions.WorkflowStatus = System.Workflow.Runtime.WorkflowStatus.Terminated
                queriedWorkflows.AddRange(sqlTrackingQuery.GetWorkflows(sqlTrackingQueryOptions))
            ElseIf ((workflowEvent = Nothing) Or _
                    (workflowEvent.ToLower(CultureInfo.InvariantCulture) = "all") Or _
                    (workflowEvent = String.Empty)) Then
                sqlTrackingQueryOptions.WorkflowStatus = System.Workflow.Runtime.WorkflowStatus.Created
                queriedWorkflows.AddRange(sqlTrackingQuery.GetWorkflows(sqlTrackingQueryOptions))

                sqlTrackingQueryOptions.WorkflowStatus = System.Workflow.Runtime.WorkflowStatus.Completed
                queriedWorkflows.AddRange(sqlTrackingQuery.GetWorkflows(sqlTrackingQueryOptions))

                sqlTrackingQueryOptions.WorkflowStatus = System.Workflow.Runtime.WorkflowStatus.Running
                queriedWorkflows.AddRange(sqlTrackingQuery.GetWorkflows(sqlTrackingQueryOptions))

                sqlTrackingQueryOptions.WorkflowStatus = System.Workflow.Runtime.WorkflowStatus.Suspended
                queriedWorkflows.AddRange(sqlTrackingQuery.GetWorkflows(sqlTrackingQueryOptions))

                sqlTrackingQueryOptions.WorkflowStatus = System.Workflow.Runtime.WorkflowStatus.Terminated
                queriedWorkflows.AddRange(sqlTrackingQuery.GetWorkflows(sqlTrackingQueryOptions))

            End If

            Return queriedWorkflows

        Catch exception As Exception
            'Dim errorMessage As String = "Exception in GetWorkflows" + vbCrLf + "Database: " + databaseName + vbCrLf + "Server: " + serverName
            Throw (New Exception("Exception in GetWorkflows", exception))
        End Try
    End Function

    Friend Function GetWorkflow(ByVal workflowInstanceId As Guid) As SqlTrackingWorkflowInstance

        Try
            Dim sqlTrackingQuery As SqlTrackingQuery = New SqlTrackingQuery(connectionString)
            Dim sqlTrackingWorkflowInstance As SqlTrackingWorkflowInstance = Nothing
            sqlTrackingQuery.TryGetWorkflow(workflowInstanceId, sqlTrackingWorkflowInstance)
            Return sqlTrackingWorkflowInstance
        Catch ex As Exception
            Throw New Exception("Exception in GetWorkflow", ex)
        End Try
    End Function
End Class
