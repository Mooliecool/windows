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
Imports Microsoft.VisualBasic

Public Class SendEmailWorkflow
    Inherits SequentialWorkflowActivity

    Private Sub OnBeforeSend(ByVal sender As System.Object, ByVal e As CustomActivityEventArgs)
        Console.WriteLine(e.ActivityDescription + ": OnBeforeSend Event Handler - do any pre-processing logic here" + vbLf)
    End Sub
End Class
