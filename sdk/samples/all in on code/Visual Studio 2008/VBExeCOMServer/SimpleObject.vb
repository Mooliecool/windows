'****************************** Module Header ******************************'
' Module Name:  SimpleObject.vb
' Project:      VBExeCOMServer
' Copyright (c) Microsoft Corporation.
' 
' The definition of the COM class, SimpleObject, and its ClassFactory, 
' SimpleObjectClassFactory.
' 
' (Please generate new GUIDs when you are writing your own COM server)
' Program ID: VBExeCOMServer.SimpleObject
' CLSID_SimpleObject: 3CCB29D4-9466-4f3c-BCB2-F5F0A62C2C3C
' IID__SimpleObject: 5EECE765-6416-467c-8D5E-C227F69E7EB7
' DIID___SimpleObjectEvents: 10C862E3-37E6-4e36-96FE-3106477235F1
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

Imports System.Runtime.InteropServices
Imports System.ComponentModel

#End Region


<ComClass(SimpleObject.ClassId, SimpleObject.InterfaceId, _
          SimpleObject.EventsId), ComVisible(True)> _
Public Class SimpleObject
    Inherits ReferenceCountedObject

#Region "COM Registration"

    Public Const ClassId As String _
    = "3CCB29D4-9466-4f3c-BCB2-F5F0A62C2C3C"
    Public Const InterfaceId As String _
    = "5EECE765-6416-467c-8D5E-C227F69E7EB7"
    Public Const EventsId As String _
    = "10C862E3-37E6-4e36-96FE-3106477235F1"

    ' These routines perform the additional COM registration needed by 
    ' the service.

    <ComRegisterFunction(), EditorBrowsable(EditorBrowsableState.Never)> _
    Public Shared Sub Register(ByVal t As Type)
        Try
            COMHelper.RegasmRegisterLocalServer(t)
        Catch ex As Exception
            Console.WriteLine(ex.Message) ' Log the error
            Throw ex ' Re-throw the exception
        End Try
    End Sub

    <EditorBrowsable(EditorBrowsableState.Never), ComUnregisterFunction()> _
    Public Shared Sub Unregister(ByVal t As Type)
        Try
            COMHelper.RegasmUnregisterLocalServer(t)
        Catch ex As Exception
            Console.WriteLine(ex.Message) ' Log the error
            Throw ex ' Re-throw the exception
        End Try
    End Sub

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


''' <summary>
''' Class factory for the class SimpleObject.
''' </summary>
Friend Class SimpleObjectClassFactory
    Implements IClassFactory

    Public Function CreateInstance(ByVal pUnkOuter As IntPtr, ByRef riid As Guid, _
                                   <Out()> ByRef ppvObject As IntPtr) As Integer _
                                   Implements IClassFactory.CreateInstance
        ppvObject = IntPtr.Zero

        If (pUnkOuter <> IntPtr.Zero) Then
            ' The pUnkOuter parameter was non-NULL and the object does 
            ' not support aggregation.
            Marshal.ThrowExceptionForHR(COMNative.CLASS_E_NOAGGREGATION)
        End If

        If ((riid = New Guid(SimpleObject.ClassId)) OrElse _
            (riid = New Guid(COMNative.IID_IDispatch)) OrElse _
            (riid = New Guid(COMNative.IID_IUnknown))) Then
            ' Create the instance of the .NET object
            ppvObject = Marshal.GetComInterfaceForObject( _
            New SimpleObject, GetType(SimpleObject).GetInterface("_SimpleObject"))
        Else
            ' The object that ppvObject points to does not support the 
            ' interface identified by riid.
            Marshal.ThrowExceptionForHR(COMNative.E_NOINTERFACE)
        End If

        Return 0  ' S_OK
    End Function


    Public Function LockServer(ByVal fLock As Boolean) As Integer _
    Implements IClassFactory.LockServer
        Return 0  ' S_OK
    End Function

End Class


''' <summary>
''' Reference counted object base.
''' </summary>
''' <remarks></remarks>
<ComVisible(False)> _
Public Class ReferenceCountedObject

    Public Sub New()
        ' Increment the lock count of objects in the COM server.
        ExeCOMServer.Instance.Lock()
    End Sub

    Protected Overrides Sub Finalize()
        Try
            ' Decrement the lock count of objects in the COM server.
            ExeCOMServer.Instance.Unlock()
        Finally
            MyBase.Finalize()
        End Try
    End Sub

End Class