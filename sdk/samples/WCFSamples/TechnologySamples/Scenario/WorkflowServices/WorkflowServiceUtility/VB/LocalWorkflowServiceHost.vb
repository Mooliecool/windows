' Copyright (c) Microsoft Corporation. All rights reserved.

Imports System
Imports System.Collections
Imports System.Collections.Generic
Imports System.ServiceModel
Imports System.ServiceModel.Channels
Imports System.ServiceModel.Description
Imports System.Workflow.Runtime
Imports System.Workflow.Runtime.Hosting

Namespace Microsoft.WorkflowServices.Samples
	''' <summary>
	''' This is a helper class for allowing a local host application to interact with a
	''' single WorkflowService instance. All interaction is accomplished using messaging
	''' via ServiceContracts. 
	''' 
	''' This ServiceHost creates both a ServiceHost for a singleton Service implementation
	''' provided by the local host AND a WorkflowServiceHost for the Workflow Service
	''' implementation. It automatically creates local listener endpoints for the
	''' local Service and Workflow Service, such that each can communicate to eachother.
	'''
	''' The Workflow can communicate with those local host endpoints using a client endpoint 
    ''' named "HostEndpoint". The local host can use CreateLocalChannel&lt;T&gt; to create channels
	''' to the Workflow Service.
	''' 
	''' This class also manages the context for a single Workflow instance, such that an
	''' application can shut-down and restarted, recovering the previous Workflow instance.
	''' This presumes the Workflow instance is being durably stored.
	''' </summary>
	Public Class LocalWorkflowServiceHost
		Inherits WorkflowServiceHost
		Const localBaseAddress As String = "net.pipe://localhost/"
		Const localServiceName As String = "/Listener.svc"
		Const localWorkflowServiceName As String = "/Workflow.svc"
		Const localHostEndpointName As String = "HostEndpoint"
		Const contextFileExtension As String = ".ctx"
		Shared localBinding As Binding = New CustomBinding(New ContextBindingElement(), New BinaryMessageEncodingBindingElement(), New NamedPipeTransportBindingElement())

		Private wfRuntime As WorkflowRuntime = Nothing
        Private localServiceHostInstance As LocalServiceHost = Nothing
		Private localWorkflowAddress As String
		Private contextFileName As String

		Public Sub New(ByVal workflowType As Type, ByVal localServiceInstance As Object, ParamArray baseAddress As Uri())
			MyBase.New(workflowType, baseAddress)
            localServiceHostInstance = New LocalServiceHost(localServiceInstance, baseAddress)
		End Sub

		Private m_recoveredContext As Boolean = False
		Public ReadOnly Property RecoveredContext() As Boolean
			Get
				Return m_recoveredContext
			End Get
		End Property

		Public Function CreateLocalChannel(Of TChannel)() As TChannel
			Dim channelFactory As New ChannelFactory(Of TChannel)(localBinding, localWorkflowAddress)
			Dim channel As TChannel = channelFactory.CreateChannel()
            Dim context As IDictionary(Of String, String) = ContextManager.DepersistContext(contextFileName)
			If context IsNot Nothing AndAlso context.Count > 0 Then
				ContextManager.ApplyContextToChannel(context, DirectCast(channel, IClientChannel))
				m_recoveredContext = True

                ' register handlers to cleanup context file when Workflow completes or terminates.
                AddHandler Me.Description.Behaviors.Find(Of WorkflowRuntimeBehavior)().WorkflowRuntime.WorkflowCompleted, AddressOf WorkflowRuntime_WorkflowCompleted
                AddHandler Me.Description.Behaviors.Find(Of WorkflowRuntimeBehavior)().WorkflowRuntime.WorkflowTerminated, AddressOf WorkflowRuntime_WorkflowTerminated
			End If
			Return channel
		End Function

        Public Function MaintainContext(ByVal channel As IClientChannel) As IDictionary(Of String, String)
            Dim context As IDictionary(Of String, String) = Nothing
            If Not RecoveredContext Then
                context = ContextManager.ExtractContextFromChannel(DirectCast(channel, IClientChannel))
                ContextManager.PersistContext(context, contextFileName)

                ' register handlers to cleanup context file when Workflow completes or terminates.
                AddHandler wfRuntime.WorkflowCompleted, AddressOf WorkflowRuntime_WorkflowCompleted
                AddHandler wfRuntime.WorkflowTerminated, AddressOf WorkflowRuntime_WorkflowTerminated
            End If
            Return context
        End Function

		' Override to add local endpoints
		Protected Overloads Overrides Function CreateDescription(ByRef implementedContracts As IDictionary(Of String, ContractDescription)) As ServiceDescription
			Dim sd As ServiceDescription = MyBase.CreateDescription(implementedContracts)
			If implementedContracts.Count > 0 Then
				' add local ServiceEndpoints for WorkflowService
				localWorkflowAddress = localBaseAddress + sd.Name + localWorkflowServiceName
				For Each item As KeyValuePair(Of String, ContractDescription) In implementedContracts
					sd.Endpoints.Add(New ServiceEndpoint(item.Value, localBinding, New EndpointAddress(localWorkflowAddress)))
				Next
			End If
			Return sd
		End Function

		' Override to add client endpoints and open LocalServiceHost
		Protected Overloads Overrides Sub OnOpening()
			contextFileName = localServiceHostInstance.Description.ServiceType.Name + contextFileExtension

			' add local client endpoints
			wfRuntime = Me.Description.Behaviors.Find(Of WorkflowRuntimeBehavior)().WorkflowRuntime
            wfRuntime.AddService(New ChannelManagerService(localServiceHostInstance.ClientEndpoints))

            localServiceHostInstance.Open()
			MyBase.OnOpening()
		End Sub

		' Override to close LocalServiceHost
		Protected Overloads Overrides Sub OnClosed()
			MyBase.OnClosed()
            localServiceHostInstance.Close()
		End Sub

        Private Sub WorkflowRuntime_WorkflowCompleted(ByVal sender As Object, ByVal e As WorkflowCompletedEventArgs)
            ContextManager.DeleteContext(contextFileName)
        End Sub
        Private Sub WorkflowRuntime_WorkflowTerminated(ByVal sender As Object, ByVal e As WorkflowTerminatedEventArgs)
            ContextManager.DeleteContext(contextFileName)
        End Sub


#Region "private nested class LocalServiceHost"

        ' Private class to automatically populate local endpoints for local Service
        Private Class LocalServiceHost
            Inherits ServiceHost
            Private m_clientEndpoints As IList(Of ServiceEndpoint)
            Public Sub New(ByVal singletonInstance As Object, ByVal ParamArray baseAddresses As Uri())
                MyBase.New(singletonInstance, baseAddresses)
            End Sub

            Friend ReadOnly Property ClientEndpoints() As IList(Of ServiceEndpoint)
                Get
                    Return m_clientEndpoints
                End Get
            End Property

            ' Override to add local endpoints
            Protected Overloads Overrides Function CreateDescription(ByRef implementedContracts As IDictionary(Of String, ContractDescription)) As ServiceDescription
                Dim sd As ServiceDescription = MyBase.CreateDescription(implementedContracts)
                If implementedContracts.Count > 0 Then
                    ' add local service and client endpoints for each Contract
                    m_clientEndpoints = New List(Of ServiceEndpoint)()
                    Dim localAddress As String = localBaseAddress + sd.ServiceType.Name + localServiceName
                    For Each item As KeyValuePair(Of String, ContractDescription) In implementedContracts
                        sd.Endpoints.Add(New ServiceEndpoint(item.Value, localBinding, New EndpointAddress(localAddress)))
                        Dim clientEndpoint As New ServiceEndpoint(item.Value, localBinding, New EndpointAddress(localAddress))
                        clientEndpoint.Name = localHostEndpointName
                        m_clientEndpoints.Add(clientEndpoint)
                    Next
                End If
                Return sd
            End Function

        End Class

#End Region

	End Class
End Namespace