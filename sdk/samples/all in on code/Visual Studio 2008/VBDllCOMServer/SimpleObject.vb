'****************************** Module Header ******************************'
' Module Name:  SimpleObject.vb
' Project:      VBDllCOMServer
' Copyright (c) Microsoft Corporation.
' 
' This VB.NET sample focuses on exposing .NET Framework components to COM,  
' which allows us to write a .NET type and consuming that type from unmanaged  
' code with distinct activities for COM developers. The sample uses the 
' Microsoft.VisualBasic.ComClassAttribute attribute to instruct the compiler 
' to add metadata that allows a class to be exposed as a COM object. The  
' attribute simplifies the process of exposing COM components from Visual 
' Basic. Without ComClassAttribute, the developer needs to follow a number of 
' steps to generate a COM object from Visual Basic. For classes marked with 
' ComClassAttribute, the compiler performs many of these steps automatically. 
' 
' VBDllCOMServer exposes the SimpleObject component:
' 
'   Program ID: VBDllCOMServer.SimpleObject
'   CLSID_SimpleObject: 805303FE-B5A6-308D-9E4F-BF500978AEEB
'   IID__SimpleObject: 90E0BCEA-7AFA-362A-A75E-6D07C1C6FC4B
'   DIID___SimpleObject: 72D3EFB2-0D88-4BA7-A26B-8FFDB92FEBED (EventID)
'   LIBID_VBDllCOMServer: A0CB2839-B70C-4035-9B11-2FF27E08B7DF
' 
'   Properties:
'     ' With both get and set accessor methods
'     FloatProperty As Single
' 
'   Methods:
'     ' HelloWorld returns a string "HelloWorld"
'     Function HelloWorld() As String
'     ' GetProcessThreadID outputs the running process ID and thread ID
'     Sub GetProcessThreadID(ByRef processId As UInteger, 
'                            ByRef threadId As UInteger)
' 
'   Events:
'     ' FloatPropertyChanging is fired before new value is set to the 
'     ' FloatProperty property. The Cancel parameter allows the client to 
'     ' cancel the change of FloatProperty.
'     Event FloatPropertyChanging(ByVal NewValue As Single, 
'                                 ByRef Cancel As Boolean)
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
    = "805303FE-B5A6-308D-9E4F-BF500978AEEB"
    Public Const InterfaceId As String _
    = "90E0BCEA-7AFA-362A-A75E-6D07C1C6FC4B"
    Public Const EventsId As String _
    = "72D3EFB2-0D88-4ba7-A26B-8FFDB92FEBED"

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
