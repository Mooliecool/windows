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

' This workflow has a single Throw activity.  Since the workflow does not implement
' any exception handlers the exception thrown by the Throw activity terminates the workflow.

Public Class ThrowWorkflow
    Inherits SequentialWorkflowActivity
    Private thrownExceptionValue As Exception = New System.Exception("My Exception Message.")

    Public Property ThrownException() As Exception
        Get
            Return thrownExceptionValue
        End Get
        Set(ByVal value As Exception)
            thrownExceptionValue = value
        End Set
    End Property
End Class
