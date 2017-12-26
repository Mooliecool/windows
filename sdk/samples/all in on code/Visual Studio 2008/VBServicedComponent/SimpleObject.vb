'****************************** Module Header ******************************'
' Module Name:  SimpleObject.vb
' Project:      VBServicedComponent
' Copyright (c) Microsoft Corporation.
' 
' VBServicedComponent demonstrates a serviced component written in VB.NET. A 
' serviced component is a class that is authored in a CLS-compliant language 
' and that derives directly or indirectly from the System.EnterpriseServices.
' ServicedComponent class. Classes configured in this way can be hosted in a 
' COM+ application and can use COM+ services by way of the EnterpriseServices 
' namespace. 
' 
' VBServicedComponent exposes the following component:
' 
' Program ID: VBServicedComponent.SimpleObject
' CLSID_SimpleObject: 53B70923-7796-4c6e-8E19-03DA58D51AB0
' IID__SimpleObject: 3CBE3348-E59D-4ce6-8B46-AE0119E4B871
' DIID___SimpleObject: C265CEA3-7A1C-479c-BFFC-05EC03F7D24B (EventID)
' LIBID_VBServicedComponent: 6B8E2f67-6E10-43A6-B8EE-7561E8E71A9E
' 
' Properties:
' With both get and set accessor methods
' FloatProperty As Single
' 
' Methods:
' ' HelloWorld returns a string "HelloWorld"
' Function HelloWorld() As String
' ' GetProcessThreadID outputs the running process ID and thread ID
' Sub GetProcessThreadID(ByRef processId As UInteger, 
'                        ByRef threadId As UInteger)
' ' Transactional operation
' void DoTransaction();
' 
' Events:
' ' FloatPropertyChanging is fired before new value is set to the 
' ' FloatProperty property. The Cancel parameter allows the client to cancel 
' ' the change of FloatProperty.
' Event FloatPropertyChanging(ByVal NewValue As Single, 
'                             ByRef Cancel As Boolean)
' 
' This source is subject to the Microsoft Public License.
' See http://www.microsoft.com/opensource/licenses.mspx#Ms-PL.
' All other rights reserved.
' 
' THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY KIND, 
' EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED 
' WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR PURPOSE.
'***************************************************************************'

#Region "Imports directives"

Imports System.EnterpriseServices

#End Region


' The ObjectPooling attribute is used to configure the component's object 
' pooling. It can enable or disables object pooling and set the min. or  
' max. pool size and object creation timeout. 
' The Transaction attribute creates the component with a new transaction, 
' regardless of the state of the current context.
<ObjectPooling(MinPoolSize:=2, MaxPoolSize:=10, CreationTimeout:=20)> _
<Transaction(TransactionOption.Required)> _
<ComClass(SimpleObject.ClassId, SimpleObject.InterfaceId, _
          SimpleObject.EventsId)> _
Public Class SimpleObject
    Inherits ServicedComponent

#Region "COM Registration"

    Public Const ClassId As String _
    = "53B70923-7796-4c6e-8E19-03DA58D51AB0"
    Public Const InterfaceId As String _
    = "3CBE3348-E59D-4ce6-8B46-AE0119E4B871"
    Public Const EventsId As String _
    = "C265CEA3-7A1C-479c-BFFC-05EC03F7D24B"

#End Region

#Region "Properties"

    Private fField As Single = 0

    ''' <summary>
    ''' A public property with both get and set accessor methods.
    ''' </summary>
    ''' <value></value>
    ''' <returns></returns>
    ''' <remarks></remarks>
    Public Property FloatProperty() As Single
        Get
            Return Me.fField
        End Get
        Set(ByVal value As Single)
            Dim cancel As Boolean = False
            ' Raise the event FloatPropertyChanging
            RaiseEvent FloatPropertyChanging(value, cancel)
            If Not cancel Then
                Me.fField = value
            End If
        End Set
    End Property

#End Region

#Region "Methods"

    ''' <summary>
    ''' A public method that returns a string "HelloWorld".
    ''' </summary>
    ''' <returns></returns>
    ''' <remarks></remarks>
    Public Function HelloWorld() As String
        Return "HelloWorld"
    End Function

    ''' <summary>
    ''' A public method with two outputs: the current process Id and the
    ''' current thread Id.
    ''' </summary>
    ''' <param name="processId"></param>
    ''' <param name="threadId"></param>
    ''' <remarks></remarks>
    Public Sub GetProcessThreadID(ByRef processId As UInteger, ByRef threadId As UInteger)
        processId = NativeMethod.GetCurrentProcessId
        threadId = NativeMethod.GetCurrentThreadId
    End Sub

    Public Sub DoTransaction()
        Try
            ' Operate on the resource managers like DBMS
            ' ...

            ContextUtil.SetComplete()   ' Commit
        Catch ex As Exception
            ContextUtil.SetAbort()      ' Rollback
            Throw ex
        End Try
    End Sub

#End Region

#Region "Events"

    ''' <summary>
    ''' A public event that is fired before new value is set to the
    ''' FloatProperty property. The Cancel parameter allows the client 
    ''' to cancel the change of FloatProperty.
    ''' </summary>
    ''' <param name="NewValue"></param>
    ''' <param name="Cancel"></param>
    ''' <remarks></remarks>
    Public Event FloatPropertyChanging(ByVal NewValue As Single, _
                                       ByRef Cancel As Boolean)

#End Region

End Class
