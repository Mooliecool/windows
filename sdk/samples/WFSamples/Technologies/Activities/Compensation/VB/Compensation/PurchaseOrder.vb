'----------------------------------------------------------------------
'   This file is part of the Windows Workflow Foundation SDK Code Samples.
'  
'   Copyright (C) Microsoft Corporation.  All rights reserved.
'  
' This source code is intended only as a supplement to Microsoft
' Development Tools and/or on-line documentation.  See these other
' materials for detailed information regarding Microsoft code samples.
'  
' THIS CODE AND INFORMATION ARE PROVIDED AS IS WITHOUT WARRANTY OF ANY
' KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
' IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
' PARTICULAR PURPOSE.
'----------------------------------------------------------------------

Imports System
Imports System.Runtime.Serialization
Imports System.Workflow.Activities
Namespace Microsoft.Samples.Workflow.Compensation
    Public Class PurchaseOrder
        Inherits SequentialWorkflowActivity

        Private Sub ReceiveOrderHandler(ByVal sender As System.Object, ByVal e As System.EventArgs)
            Console.WriteLine("Received order")
        End Sub

        Private Sub WithdrawFundsHandler(ByVal sender As System.Object, ByVal e As System.EventArgs)
            Console.WriteLine("Withdraw funds")
        End Sub

        Private Sub ProductCheckHandler(ByVal sender As System.Object, ByVal e As System.Workflow.Activities.ConditionalEventArgs)
            e.Result = False
        End Sub

        Private Sub ShipHandler(ByVal sender As System.Object, ByVal e As System.EventArgs)
            Console.WriteLine("Ship order")
        End Sub


        Private Sub DiscontinuedProduct_ExecuteCode(ByVal sender As System.Object, ByVal e As System.EventArgs)
            Console.WriteLine("Product discontinued")
        End Sub

        Private Sub RefundHandler(ByVal sender As System.Object, ByVal e As System.EventArgs)
            Console.WriteLine("Refunding")
        End Sub
        Public discontinuedProductException1 As DiscontinuedProductException = New Microsoft.Samples.Workflow.Compensation.DiscontinuedProductException
    End Class

    <SerializableAttribute()> _
     Public Class DiscontinuedProductException
        Inherits Exception
        Public Sub New()
            MyBase.New()
        End Sub

        Public Sub New(ByVal message As String)
            MyBase.New(message)
        End Sub

        Public Sub New(ByVal message As String, ByVal innerException As Exception)
            MyBase.New(message, innerException)
        End Sub

        Protected Sub New(ByVal info As SerializationInfo, ByVal context As StreamingContext)
            MyBase.New(info, context)
        End Sub

    End Class
End Namespace