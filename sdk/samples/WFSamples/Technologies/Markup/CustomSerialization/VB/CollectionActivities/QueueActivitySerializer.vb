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
Imports System.Runtime.InteropServices
Imports System.Workflow.ComponentModel.Serialization

<ComVisible(False)> _
Public Class QueueActivitySerializer
    Inherits WorkflowMarkupSerializer
    Private serializer As New QueueSerializer()

    Protected Overrides Sub OnBeforeDeserialize(ByVal serializationManager As WorkflowMarkupSerializationManager, ByVal obj As Object)
        MyBase.OnBeforeDeserialize(serializationManager, obj)
        serializationManager.AddSerializationProvider(serializer)
    End Sub

    Protected Overrides Sub OnAfterDeserialize(ByVal serializationManager As WorkflowMarkupSerializationManager, ByVal obj As Object)
        MyBase.OnAfterDeserialize(serializationManager, obj)
        serializationManager.RemoveSerializationProvider(serializer)
    End Sub

    Protected Overrides Sub OnBeforeSerialize(ByVal serializationManager As WorkflowMarkupSerializationManager, ByVal obj As Object)
        MyBase.OnBeforeSerialize(serializationManager, obj)
        serializationManager.AddSerializationProvider(serializer)
    End Sub

    Protected Overrides Sub OnAfterSerialize(ByVal serializationManager As WorkflowMarkupSerializationManager, ByVal obj As Object)
        MyBase.OnAfterSerialize(serializationManager, obj)
        serializationManager.RemoveSerializationProvider(serializer)
    End Sub
End Class
