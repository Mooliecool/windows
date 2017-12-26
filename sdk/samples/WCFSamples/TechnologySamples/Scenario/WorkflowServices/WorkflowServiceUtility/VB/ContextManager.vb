' Copyright (c) Microsoft Corporation. All rights reserved.

Imports System
Imports System.Collections
Imports System.Collections.Generic
Imports System.IO
Imports System.Runtime.Serialization
Imports System.ServiceModel
Imports System.ServiceModel.Channels
Imports System.Workflow.Activities
Imports System.Workflow.ComponentModel

Namespace Microsoft.WorkflowServices.Samples
    ' Static methods for managing context.
    Public NotInheritable Class ContextManager
        Shared ReadOnly contextHeaderName As String = "Context"
        Shared ReadOnly contextHeaderNamespace As String = "http://schemas.microsoft.com/ws/2006/05/context"

        Private Sub New()
        End Sub
        ' Apply Context to a SendActivity
        Public Shared Sub ApplyContext(ByVal activity As SendActivity, ByVal context As IDictionary(Of String, String))
            If activity.ExecutionStatus = ActivityExecutionStatus.Initialized Then
                activity.Context = context
            End If
        End Sub

        ' Apply EndpointAddress to a SendActivity
        Public Shared Sub ApplyEndpointAddress(ByVal activity As SendActivity, ByVal epr As EndpointAddress)
            If activity.ExecutionStatus = ActivityExecutionStatus.Initialized Then
                If epr.Uri IsNot Nothing Then
                    activity.CustomAddress = epr.Uri.ToString()
                End If
                If epr.Headers IsNot Nothing AndAlso epr.Headers.Count > 0 Then
                    Dim contextHeader As AddressHeader = epr.Headers.FindHeader(contextHeaderName, contextHeaderNamespace)
                    Dim context As IDictionary(Of String, String) = contextHeader.GetValue(Of Dictionary(Of String, String))()
                    activity.Context = context
                End If
            End If
        End Sub

        ' Create EndpointAddress from Uri and Context
        Public Shared Function CreateEndpointAddress(ByVal uri As String, ByVal context As IDictionary(Of String, String)) As EndpointAddress
            Dim epr As EndpointAddress = Nothing
            If context IsNot Nothing AndAlso context.Count > 0 Then
                Dim contextDictionary As Dictionary(Of String, String) = New Dictionary(Of String, String)(context)

                Dim contextHeader As AddressHeader = AddressHeader.CreateAddressHeader(contextHeaderName, contextHeaderNamespace, contextDictionary)
                epr = New EndpointAddress(New Uri(uri), contextHeader)
            Else
                epr = New EndpointAddress(uri)
            End If
            Return epr
        End Function

        ' Create EndpointAddress from Uri and ReceiveActivity
        Public Shared Function CreateEndpointAddress(ByVal uri As String, ByVal receiveActivity As ReceiveActivity) As EndpointAddress
            Return CreateEndpointAddress(uri, receiveActivity.Context)
        End Function

        ' Apply Context to an IClientChannel
        Public Shared Function ApplyContextToChannel(ByVal context As IDictionary(Of String, String), ByVal channel As IClientChannel) As Boolean
            If context IsNot Nothing Then
                Dim cm As IContextManager = channel.GetProperty(Of IContextManager)()
                If cm IsNot Nothing Then
                    ' apply context to ContextChannel
                    cm.SetContext(context)
                    Return True
                End If
            End If
            Return False
        End Function

        ' Extract context from an IClientChannel
        Const WscContextKey As String = "WscContext"
        Public Shared Function ExtractContextFromChannel(ByVal channel As IClientChannel) As IDictionary(Of String, String)
            ' extract context from channel
            Dim cm As IContextManager = channel.GetProperty(Of IContextManager)()
            If cm IsNot Nothing Then
                ' attempt to extract context from channel
                Return cm.GetContext()
            End If
            Return Nothing
        End Function

        ' Deserialize context from file
        Public Shared Function DepersistContext(ByVal fileName As String) As IDictionary(Of String, String)
            ' retrieve context from file
            If File.Exists(fileName) Then
                Dim serializer As New DataContractSerializer(GetType(Dictionary(Of String, String)))
                Dim opDone As Boolean = True
                While opDone
                    Try
                        Dim stream As New FileStream(fileName, FileMode.Open)
                        Return DirectCast(serializer.ReadObject(stream), Dictionary(Of String, String))
                    Catch ex As IOException
                    Finally
                        opDone = False
                    End Try
                End While
            End If
            Return Nothing
        End Function

        ' Serialize context into file
        Public Shared Sub PersistContext(ByVal context As IDictionary(Of String, String), ByVal fileName As String)
            ' persist context to file
            If context IsNot Nothing Then
                Dim serializer As New DataContractSerializer(GetType(Dictionary(Of String, String)))
                Dim opDone As Boolean = True
                While opDone
                    Try
                        Dim stream As New FileStream(fileName, FileMode.Create)
                        serializer.WriteObject(stream, context)
                    Catch ex As IOException
                    Finally
                        opDone = False
                    End Try

                End While
            End If
        End Sub

        ' Delete context file
        Public Shared Sub DeleteContext(ByVal fileName As String)
            Dim opDone As Boolean = True
            While opDone
                Try
                    If File.Exists(fileName) Then
                        File.Delete(fileName)
                    End If

                Catch ex As IOException
                Finally
                    opDone = False
                End Try
            End While

        End Sub

    End Class
End Namespace