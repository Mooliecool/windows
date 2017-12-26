'---------------------------------------------------------------------
'  This file is part of the Windows Workflow Foundation SDK Code Samples.
' 
'  Copyright (C) Microsoft Corporation.  All rights reserved.
' 
'  This source code is intended only as a supplement to Microsoft
'  Development Tools and/or on-line documentation.  See these other
'  materials for detailed information regarding Microsoft code samples.
' 
'  THIS CODE AND INFORMATION ARE PROVIDED AS IS WITHOUT WARRANTY OF ANY
'  KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
'  IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
'  PARTICULAR PURPOSE.
'---------------------------------------------------------------------

Imports System
Imports System.ComponentModel
Imports System.Workflow.Activities

Public Class CorrelatedLocalServiceWorkflow
    Inherits SequentialWorkflowActivity

    Private Sub OnTaskCompleted(ByVal sender As System.Object, ByVal e As ExternalDataEventArgs)
        Console.WriteLine("task " + CType(e, TaskEventArgs).Id + " done")
    End Sub
End Class
