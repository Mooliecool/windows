'***************************** Module Header ******************************\
'* Module Name:	Module1.vb
'* Project:		VBAzureServiceBusWCFDS
'* Copyright (c) Microsoft Corporation.
'* 
'* This Console Application hosts WCF Data Services and expose it via Service Bus
'* 
'* This source is subject to the Microsoft Public License.
'* See http://www.microsoft.com/opensource/licenses.mspx#Ms-PL.
'* All other rights reserved.
'* 
'* THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY KIND, 
'* EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED 
'* WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR PURPOSE.
'\**************************************************************************

Imports System.ServiceModel.Channels
Imports System.IO
Imports System.Xml
Imports System.ServiceModel.Dispatcher
Imports System.ServiceModel.Web
Imports Microsoft.ServiceBus
Imports System.ServiceModel.Description
Imports System.ServiceModel

Module Module1

    Sub Main()
        Console.Write("Your Service Namespace Domain (ex. https://<DOMAIN>.servicebus.windows.net/): ")
        Dim serviceNamespaceDomain As String = Console.ReadLine()

        ' By setting EndToEndWebHttpSecurityMode.Transport we use HTTPS.
        ' If you want to use HTTP please set EndToEndWebHttpSecurityMode.None.
        ' In this sample we need to authenticate client via Access Control Service so
        ' RelayClientAuthenticationType.RelayAccessToken is set. You can set RelayClientAuthenticationType.None
        ' If you don't want to authenticate client via Access Control Service.
        Dim binding As WebHttpRelayBinding = New WebHttpRelayBinding(EndToEndWebHttpSecurityMode.Transport, RelayClientAuthenticationType.RelayAccessToken)
        ' Replace above code with the following one to test in browser
        ' Dim binding As WebHttpRelayBinding = New WebHttpRelayBinding(EndToEndWebHttpSecurityMode.Transport, RelayClientAuthenticationType.None)

        ' Initialize ServiceHost using custom binding
        Dim address As Uri = ServiceBusEnvironment.CreateServiceUri("https", serviceNamespaceDomain, "DataService")
        Dim host As WebServiceHost = New WebServiceHost(GetType(NorthwindDataService), address)
        host.AddServiceEndpoint("System.Data.Services.IRequestHandler", binding, address)
        Dim eb = New TransportClientEndpointBehavior()
        eb.CredentialType = TransportClientCredentialType.SharedSecret
        eb.Credentials.SharedSecret.IssuerName = "owner"
        eb.Credentials.SharedSecret.IssuerSecret = "[Your Secret]"
        host.Description.Endpoints(0).Behaviors.Add(eb)

        ' The following behavior is used to work around exception caused by PUT/POST 
        ' requests when exposing via Service Bus
        Dim mb As MyBehavior = New MyBehavior()
        host.Description.Endpoints(0).Behaviors.Add(mb)

        ' Start service
        host.Open()
        Console.WriteLine("Test the following URI in browser: ")
        Console.WriteLine(address.ToString() & "Customers")
        Console.WriteLine("Use the following URI if you want to generate client proxy for this service")
        Console.WriteLine(address)
        Console.WriteLine()
        Console.WriteLine("Press [Enter] to exit")
        Console.ReadLine()

        host.Close()
    End Sub
    Friend Class MyInspector
        Implements IDispatchMessageInspector

#Region "IDispatchMessageInspector Members"

        Public Function AfterReceiveRequest(ByRef request As System.ServiceModel.Channels.Message, ByVal channel As IClientChannel, ByVal instanceContext As InstanceContext) As Object Implements IDispatchMessageInspector.AfterReceiveRequest
            ' Workaround for Service Bus scenario for PUT&POST
            Dim buffer As MessageBuffer = request.CreateBufferedCopy(Integer.MaxValue)
            Dim copy As Message = buffer.CreateMessage()
            Dim ms As MemoryStream = New MemoryStream()
            Dim encoding As Text.Encoding = Text.Encoding.UTF8
            Dim writerSettings As XmlWriterSettings = New XmlWriterSettings With {.Encoding = encoding}
            Dim writer As XmlDictionaryWriter = XmlDictionaryWriter.CreateDictionaryWriter(XmlWriter.Create(ms))
            copy.WriteBodyContents(writer)
            writer.Flush()
            Dim messageBodyString As String = encoding.GetString(ms.ToArray())
            messageBodyString = "<?xml version=""1.0"" encoding=""utf-8""?><Binary>" & Convert.ToBase64String(Text.Encoding.UTF8.GetBytes(messageBodyString)) & "</Binary>"
            ms = New MemoryStream(encoding.GetBytes(messageBodyString))
            Dim bodyReader As XmlReader = XmlReader.Create(ms)
            Dim originalMessage As Message = request
            request = Message.CreateMessage(originalMessage.Version, Nothing, bodyReader)
            request.Headers.CopyHeadersFrom(originalMessage)

            If (Not request.Properties.ContainsKey(WebBodyFormatMessageProperty.Name)) Then
                request.Properties.Add(WebBodyFormatMessageProperty.Name, New WebBodyFormatMessageProperty(WebContentFormat.Raw))
            End If

            Return Nothing

        End Function

        Public Sub BeforeSendReply(ByRef reply As System.ServiceModel.Channels.Message, ByVal correlationState As Object) Implements IDispatchMessageInspector.BeforeSendReply

        End Sub

#End Region
    End Class
    Friend Class MyBehavior
        Implements IEndpointBehavior

#Region "IEndpointBehavior Members"

        Public Sub AddBindingParameters(ByVal endpoint As ServiceEndpoint, ByVal bindingParameters As System.ServiceModel.Channels.BindingParameterCollection) Implements IEndpointBehavior.AddBindingParameters

        End Sub

        Public Sub ApplyClientBehavior(ByVal endpoint As ServiceEndpoint, ByVal clientRuntime As ClientRuntime) Implements IEndpointBehavior.ApplyClientBehavior

        End Sub

        Public Sub ApplyDispatchBehavior(ByVal endpoint As ServiceEndpoint, ByVal endpointDispatcher As EndpointDispatcher) Implements IEndpointBehavior.ApplyDispatchBehavior
            endpointDispatcher.DispatchRuntime.MessageInspectors.Add(New MyInspector())
        End Sub

        Public Sub Validate(ByVal endpoint As ServiceEndpoint) Implements IEndpointBehavior.Validate

        End Sub

#End Region
    End Class
End Module
