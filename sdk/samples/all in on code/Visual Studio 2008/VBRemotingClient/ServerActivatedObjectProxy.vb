'****************************** Module Header ******************************'
' Module Name:  ServerActivatedObjectProxy.vb
' Project:      VBRemotingClient
' Copyright (c) Microsoft Corporation.
' 
' ServerActivatedObjectProxy.vb defines the proxy of the server-activated 
' types. The proxy has no implementation of the types' methods and properties.
' 
' This source is subject to the Microsoft Public License.
' See http://www.microsoft.com/opensource/licenses.mspx#Ms-PL.
' All other rights reserved.
' 
' THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY KIND, 
' EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED 
' WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR PURPOSE.
'***************************************************************************'


Namespace RemotingShared

    ''' <summary>
    ''' The proxy of the server-activated type for .NET Remoting.
    ''' </summary>
    Friend Class ServerActivatedObject
        Inherits MarshalByRefObject

        Public Property FloatProperty() As Single
            Get
                Throw New NotImplementedException()
            End Get
            Set(ByVal value As Single)
                Throw New NotImplementedException()
            End Set
        End Property

        Public Overridable Function GetRemoteObjectType() As String
            Throw New NotImplementedException()
        End Function

        Public Sub GetProcessThreadID(ByRef processId As UInteger, ByRef threadId As UInteger)
            Throw New NotImplementedException()
        End Sub

    End Class

    Friend Class SingleCallObject
        Inherits ServerActivatedObject
    End Class

    Friend Class SingletonObject
        Inherits ServerActivatedObject
    End Class

End Namespace