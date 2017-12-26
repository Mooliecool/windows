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

Imports System.Workflow.Activities

Public Class EventService
    Implements IEventService

    Public Sub RaiseSetStateEvent(ByVal instanceId As System.Guid) Implements IEventService.RaiseSetStateEvent
        RaiseEvent SetState(Nothing, New ExternalDataEventArgs(instanceId))
    End Sub

    Public Event SetState(ByVal sender As Object, ByVal e As System.Workflow.Activities.ExternalDataEventArgs) Implements IEventService.SetState
End Class
