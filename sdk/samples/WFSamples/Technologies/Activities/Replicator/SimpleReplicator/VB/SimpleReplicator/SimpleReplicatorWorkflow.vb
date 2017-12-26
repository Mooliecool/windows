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
Imports System.ComponentModel
Imports System.Workflow.Activities

Public Class SimpleReplicatorWorkflow
    Inherits SequentialWorkflowActivity
    
    Dim childDataField As System.Collections.ArrayList

    Public ReadOnly Property ChildData() As System.Collections.ArrayList
        Get
            Return childDataField
        End Get
    End Property


    Private Sub ChildInitializer(ByVal sender As System.Object, ByVal e As System.Workflow.Activities.ReplicatorChildEventArgs)
        ' Using the InstanceData passed in (which comes from the "childData" ArrayList above)
        ' populate a property on the SampleReplicatorChildActivity
        CType(e.Activity, SampleReplicatorChildActivity).InstanceData = e.InstanceData.ToString()
    End Sub
    Public Sub New()
        childDataField = New System.Collections.ArrayList
        childDataField.Add("Child Instance 1")
        childDataField.Add("Child Instance 2")
        InitializeComponent()
    End Sub
End Class
