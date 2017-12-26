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
Imports System.Workflow.ComponentModel.Compiler
Imports System.Workflow.ComponentModel
Imports System.Workflow.ComponentModel.Design
Imports System.Workflow.Activities

<ToolboxItemAttribute(GetType(ActivityToolboxItem))> _
Public Class SampleReplicatorChildActivity
    Inherits SequenceActivity
    
    Private Shared InstanceDataProperty As DependencyProperty = DependencyProperty.Register("InstanceData", GetType(System.String), GetType(SampleReplicatorChildActivity))

    <DesignerSerializationVisibilityAttribute(DesignerSerializationVisibility.Visible)> _
   <BrowsableAttribute(True)> _
    Public Property InstanceData() As System.String
        Get
            Return CType(MyBase.GetValue(InstanceDataProperty), String)

        End Get
        Set(ByVal value As System.String)
            MyBase.SetValue(InstanceDataProperty, value)

        End Set
    End Property

    Private Sub CodeHandler(ByVal sender As System.Object, ByVal e As System.EventArgs)
        ' Use the Name property which was populated in the child initialization method on the Replicator
        Console.WriteLine("This is " + Me.InstanceData)
    End Sub
End Class
