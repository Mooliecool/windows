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
Imports System.Workflow.ComponentModel

Public Class CustomActivity
    Inherits Activity

    Protected Overrides Function Execute(ByVal executionContext As System.Workflow.ComponentModel.ActivityExecutionContext) As System.Workflow.ComponentModel.ActivityExecutionStatus
        Console.WriteLine("Executing custom activity.  NameToPrint: " + NameToPrint)
        Return MyBase.Execute(executionContext)
    End Function
    Public Shared NameToPrintProperty As DependencyProperty = DependencyProperty.Register("NameToPrint", GetType(System.String), GetType(CustomActivity))

    <DescriptionAttribute("String to be passed to the custom activity")> _
    <DesignerSerializationVisibilityAttribute(DesignerSerializationVisibility.Visible)> _
    <BrowsableAttribute(True)> _
    Public Property NameToPrint() As String
        Get
            Return CType(MyBase.GetValue(CustomActivity.NameToPrintProperty), String)
        End Get
        Set(ByVal value As String)
            MyBase.SetValue(CustomActivity.NameToPrintProperty, value)
        End Set
    End Property

End Class
