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

imports System
imports System.IO
imports System.Collections.ObjectModel

Public NotInheritable Class QueryLayer

    Private Sub New()
        ' Static invocation only
    End Sub

    ' Represents the query API layer that reads tracked Workflow Events from tracking file
    Public Shared Function GetTrackedWorkflowEvents(ByVal trackingFile As String) As Collection(Of String)
        ' Creates a collection of workflow events
        Dim workflowEvents As New Collection(Of String)()
        ' Opens the tracking file
        Dim FileStream As FileStream = File.Open(trackingFile, FileMode.Open, FileAccess.Read)

        If FileStream IsNot Nothing Then
            Dim reader As New StreamReader(FileStream)
            Dim line As String
            ' read from the tracking file 

            While Not reader.EndOfStream
                line = reader.ReadLine()
                If (line.Contains("Workflow:")) Then
                    Dim nSeparatorIndex As Integer = line.IndexOf(":", StringComparison.OrdinalIgnoreCase)
                    If nSeparatorIndex >= 0 Then
                        ' add workflow events found to the collection of workflow events
                        workflowEvents.Add(line.Substring(nSeparatorIndex + 1).Trim())
                    End If
                End If
            End While

            reader.Close()
            FileStream.Close()
        End If

        ' return workflow events collection
        Return workflowEvents
    End Function

End Class
