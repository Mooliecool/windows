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

Imports System.ComponentModel.Design
Imports System.IO
Imports System.Workflow.ComponentModel
Imports System.Workflow.ComponentModel.Design
Imports System.Workflow.ComponentModel.Serialization

' This type is used to load the workflow definition
Friend NotInheritable Class Loader
    Inherits WorkflowDesignerLoader

    Private workflowDefinitionValue As Activity

    Public Overrides Function GetFileReader(ByVal filePath As String) As System.IO.TextReader
        Return New StreamReader(filePath)
    End Function

    Public Overrides Function GetFileWriter(ByVal filePath As String) As System.IO.TextWriter
        Return New StreamWriter(filePath)
    End Function

    Public Overrides ReadOnly Property FileName() As String
        Get
            Return String.Empty
        End Get
    End Property

    Public WriteOnly Property WorkflowDefinition() As Activity
        Set(ByVal value As Activity)
            Me.workflowDefinitionValue = value
        End Set
    End Property

    Protected Overrides Sub PerformLoad(ByVal serializationManager As System.ComponentModel.Design.Serialization.IDesignerSerializationManager)
        Dim designerHost As IDesignerHost = CType(GetService(GetType(IDesignerHost)), IDesignerHost)
        Dim activity As Activity = Me.workflowDefinitionValue

        'Add the rootactivity the designer
        If activity IsNot Nothing And designerHost IsNot Nothing Then
            Helpers.AddObjectGraphToDesignerHost(designerHost, activity)
            SetBaseComponentClassName(CType(activity, Activity).Name)
        End If
    End Sub

    Protected Overrides Sub PerformFlush(ByVal serializationManager As System.ComponentModel.Design.Serialization.IDesignerSerializationManager)
    End Sub
End Class
