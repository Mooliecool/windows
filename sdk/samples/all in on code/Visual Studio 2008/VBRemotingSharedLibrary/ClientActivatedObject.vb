'****************************** Module Header ******************************'
' Module Name:  ClientActivatedObject.vb
' Project:      VBRemotingSharedLibrary
' Copyright (c) Microsoft Corporation.
' 
' ClientActivatedObject.vb defines a client-activated type for .NET Remoting.
' Client-activated objects are created by the server and their lifetime is 
' managed by the client. In contrast to server-activated objects, client-
' activated objects are created as soon as the client calls "new" or any 
' other object creation methods. Client-activated objects are specific to the 
' client, and objects are not shared among different clients; object instance 
' exists until the lease expires or the client destroys the object. 
' 
' This source is subject to the Microsoft Public License.
' See http://www.microsoft.com/opensource/licenses.mspx#Ms-PL.
' All other rights reserved.
' 
' THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY KIND, 
' EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED 
' WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR PURPOSE.
'***************************************************************************'

Imports System.Runtime.InteropServices


''' <summary>
''' A client-activated type for .NET Remoting.
''' </summary>
Public Class ClientActivatedObject
    Inherits MarshalByRefObject

    ''' <summary>
    ''' A float property.
    ''' </summary>
    Public Property FloatProperty() As Single
        Get
            Return fField
        End Get
        Set(ByVal value As Single)
            fField = value
        End Set
    End Property

    Private fField As Single


    ''' <summary>
    ''' Get the type of the remote object. 
    ''' </summary>
    Public Overridable Function GetRemoteObjectType() As String
        Return "ClientActivatedObject"
    End Function


    ''' <summary>
    ''' Get the current process ID and thread ID.
    ''' </summary>
    ''' <param name="processId">current process ID</param>
    ''' <param name="threadId">current thread ID</param>
    ''' <remarks></remarks>
    Public Sub GetProcessThreadID(<Out()> ByRef processId As UInt32, <Out()> ByRef threadId As UInt32)
        processId = Process.GetCurrentProcess.Id
        threadId = GetCurrentThreadId()
    End Sub

    ''' <summary>
    ''' Get the current thread ID.
    ''' </summary>
    <DllImport("kernel32.dll")> _
    Friend Shared Function GetCurrentThreadId() As UInt32
    End Function

End Class