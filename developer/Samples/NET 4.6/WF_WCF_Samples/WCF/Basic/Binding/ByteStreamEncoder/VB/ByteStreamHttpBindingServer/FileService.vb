'----------------------------------------------------------------
' Copyright (c) Microsoft Corporation.  All rights reserved.
'----------------------------------------------------------------

Imports Microsoft.VisualBasic
Imports System
Imports System.ServiceModel
Imports System.ServiceModel.Channels

Namespace Microsoft.Samples.ByteStreamEncoder

    <ServiceBehavior(InstanceContextMode:=InstanceContextMode.Single)> _
    Friend Class FileService
        Implements IHttpHandler

        Public Function ProcessRequest(ByVal request As Message) As Message Implements IHttpHandler.ProcessRequest
            Console.WriteLine("Server received a request from client")

            Dim httpResponseProperty As New HttpResponseMessageProperty()
            httpResponseProperty.Headers.Add("Content-Type", "application/octet-stream")

            request.Properties.Add(HttpResponseMessageProperty.Name, httpResponseProperty)

            Console.WriteLine("Server is sending back the same image to client")

            Return request
        End Function
    End Class
End Namespace
