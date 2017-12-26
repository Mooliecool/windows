'----------------------------------------------------------------
' Copyright (c) Microsoft Corporation.  All rights reserved.
'----------------------------------------------------------------

Imports System
Imports System.ServiceModel
Imports System.ServiceModel.Channels
Imports System.Xml

Namespace Microsoft.Samples.ByteStreamEncoder

    Friend Class Client
        Private Const TestFileName As String = "smallsuccess.gif"

        Public Shared Sub Main()
            ' Typically this request would be constructed by a web browser or non-WCF application instead of using WCF

            Console.WriteLine("Starting client with ByteStreamHttpBinding")

            Using cf As ChannelFactory(Of IHttpHandler) = New ChannelFactory(Of IHttpHandler)("byteStreamHttpBinding")
                Dim channel As IHttpHandler = cf.CreateChannel()
                Console.WriteLine("Client channel created")

                Dim byteStream As Message = Message.CreateMessage(MessageVersion.None, "*", New ByteStreamBodyWriter(TestFileName))
                Dim httpRequestProperty As New HttpRequestMessageProperty()
                httpRequestProperty.Headers.Add("Content-Type", "application/octet-stream")
                byteStream.Properties.Add(HttpRequestMessageProperty.Name, httpRequestProperty)

                Console.WriteLine("Client calling service")
                Dim reply As Message = channel.ProcessRequest(byteStream)

                'Get bytes from the reply 
                Dim reader As XmlDictionaryReader = reply.GetReaderAtBodyContents()
                reader.MoveToElement()
                Dim name As String = reader.Name
                Console.WriteLine("First element in the byteStream message is : <" & name & ">")

                Dim array() As Byte = reader.ReadElementContentAsBase64()
                Dim replyMessage As String = System.Text.Encoding.UTF8.GetString(array)
                Console.WriteLine("Client received a reply from service of length :" & replyMessage.Length)
            End Using

            Console.WriteLine("Done")
            Console.WriteLine("Press <ENTER> to exit client")
            Console.ReadLine()

        End Sub
    End Class
End Namespace
