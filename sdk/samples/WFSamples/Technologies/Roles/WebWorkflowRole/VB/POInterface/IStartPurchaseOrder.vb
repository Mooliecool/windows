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
Imports System.Security.Principal
Imports System.Workflow.Activities

<Serializable()> _
Public Class InitiatePOEventArgs
    Inherits ExternalDataEventArgs

    Dim itemIdValue As Integer
    Dim itemNameValue As String
    Dim amountValue As Single

    Public Sub New()
        MyBase.New(Guid.NewGuid())
    End Sub

    Public Sub New(ByVal instanceId As Guid, ByVal itemIdValue As Integer, ByVal itemNameValue As String, ByVal amountValue As Single)
        MyBase.New(instanceId)
        Me.itemIdValue = itemIdValue
        Me.itemNameValue = itemNameValue
        Me.amountValue = amountValue
    End Sub

    Public Property ItemId() As Integer
        Get
            Return Me.itemIdValue
        End Get
        Set(ByVal Value As Integer)
            Me.itemIdValue = Value
        End Set
    End Property

    Public Property ItemName() As String
        Get
            Return Me.itemNameValue
        End Get
        Set(ByVal Value As String)
            Me.itemNameValue = Value
        End Set
    End Property

    Public Property Amount() As Single
        Get
            Return Me.amountValue
        End Get
        Set(ByVal Value As Single)
            Me.amountValue = Value
        End Set
    End Property
End Class

<ExternalDataExchangeAttribute()> _
Public Interface IStartPurchaseOrder
    Event InitiatePurchaseOrderEventHandler As EventHandler(Of InitiatePOEventArgs)
End Interface

Public Class StartPurchaseOrder
    Implements IStartPurchaseOrder

    Public Event InitiatePurchaseOrderEventHandler(ByVal sender As Object, ByVal e As InitiatePOEventArgs) Implements IStartPurchaseOrder.InitiatePurchaseOrderEventHandler

    Public Sub InvokePORequest(ByVal instanceId As Guid, ByVal itemIdValue As Integer, ByVal itemCost As Single, ByVal itemNameValue As String, ByVal identity As IIdentity)
        Dim args As InitiatePOEventArgs = New InitiatePOEventArgs(instanceId, itemIdValue, itemNameValue, itemCost)
        Dim securityIdentifier As String = Nothing
        Dim windowsIdentity As WindowsIdentity = Nothing

        If TypeOf identity Is WindowsIdentity Then
            windowsIdentity = identity
        End If

        If windowsIdentity IsNot Nothing AndAlso windowsIdentity.User IsNot Nothing Then
            securityIdentifier = windowsIdentity.User.Translate(GetType(NTAccount)).ToString()
        ElseIf identity IsNot Nothing Then
            securityIdentifier = identity.Name
        End If

        args.Identity = securityIdentifier
        Console.WriteLine("Purchase Order initiated by: {0}", identity.Name)

        RaiseEvent InitiatePurchaseOrderEventHandler(Nothing, args)
    End Sub
End Class