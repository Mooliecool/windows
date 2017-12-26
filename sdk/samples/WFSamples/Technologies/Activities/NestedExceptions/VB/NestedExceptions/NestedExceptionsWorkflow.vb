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
Imports System.Workflow.Activities
Imports Microsoft.VisualBasic

Public Class NestedExceptionsWorkflow
    Inherits SequentialWorkflowActivity

    ' Throws an exception from user code - could also use the ThrowActivity
    Private Sub throwsException_ExecuteCode(ByVal sender As System.Object, ByVal e As System.EventArgs)
        Dim exception As Exception = New Exception("Workflow custom generated exception.")
        Console.WriteLine("Throwing {0}; Message = {1}" + vbLf, exception.GetType().ToString(), exception.Message)
        Throw (exception)
    End Sub

    ' This where the code to process the exception takes place
    Private Sub processThrownException_ExecuteCode(ByVal sender As System.Object, ByVal e As System.EventArgs)
        Dim thrownException As Exception = faultHandlerActivity.Fault
        Console.WriteLine("Caught {0}; Message = {1}", thrownException.GetType().ToString(), thrownException.Message)
    End Sub

End Class
