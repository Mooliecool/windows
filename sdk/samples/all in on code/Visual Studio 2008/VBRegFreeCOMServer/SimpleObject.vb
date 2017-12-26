'****************************** Module Header ******************************'
' Module Name:  SimpleObject.vb
' Project:      VBRegFreeCOMServer
' Copyright (c) Microsoft Corporation.
' 
' 
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

Imports System.Runtime.InteropServices

#End Region


<ComClass(SimpleObject.ClassId, SimpleObject.InterfaceId, _
          SimpleObject.EventsId), ComVisible(True)> _
Public Class SimpleObject

#Region "COM Registration"

    Public Const ClassId As String _
    = "6176B796-8ACF-4408-AE7C-E4ADBE952D4C"
    Public Const InterfaceId As String _
    = "33E9B27A-D8F3-40b7-AEAC-909E89994E3A"
    Public Const EventsId As String _
    = "D513BED2-22E2-47bd-8619-2B0BDD8A96D3"

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