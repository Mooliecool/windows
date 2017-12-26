' Copyright (c) Microsoft Corporation.  All Rights Reserved.

Imports System
Imports System.ServiceModel
Imports System.ServiceModel.Channels
Imports System.IO

Namespace Microsoft.ServiceModel.Samples

    'The service contract is defined in generatedClient.vb, generated from the service by the svcutil tool.

    'Client implementation code.
    Class Client

        Public Shared Sub Main()

            Dim binaryData() As Byte = New Byte(999) {}
            Dim stream As New MemoryStream(binaryData)

            ' Upload a stream of 1000 bytes
            Dim client As New UploadClient()
            Console.WriteLine(client.Upload(stream))
            Console.WriteLine()
            stream.Close()

            ' Compare the wire representations of messages with different payloads
            CompareMessageSize(100)
            CompareMessageSize(1000)
            CompareMessageSize(10000)
            CompareMessageSize(100000)
            CompareMessageSize(1000000)

            Console.WriteLine()
            Console.WriteLine("Press <ENTER> to terminate client.")
            Console.ReadLine()

        End Sub

        Private Shared Sub CompareMessageSize(ByVal dataSize As Integer)

            ' Create and buffer a message with a binary payload
            Dim binaryData() As Byte = New Byte(dataSize - 1) {}
            Dim msg As Message = Message.CreateMessage(MessageVersion.Soap12WSAddressing10, "action", binaryData)
            Dim buffer As MessageBuffer = msg.CreateBufferedCopy(Integer.MaxValue)

            ' Print the size of a text encoded copy
            Dim size As Integer = SizeOfTextMessage(buffer.CreateMessage())
            Console.WriteLine("Text encoding with a {0} byte payload: {1}", binaryData.Length, size)

            ' Print the size of an MTOM encoded copy
            size = SizeOfMtomMessage(buffer.CreateMessage())
            Console.WriteLine("MTOM encoding with a {0} byte payload: {1}", binaryData.Length, size)
            Console.WriteLine()
            msg.Close()

        End Sub

        Private Shared Function SizeOfTextMessage(ByVal msg As Message) As Integer

            ' Create a text encoder
            Dim element As MessageEncodingBindingElement = New TextMessageEncodingBindingElement()
            Dim factory As MessageEncoderFactory = element.CreateMessageEncoderFactory()
            Dim encoder As MessageEncoder = factory.Encoder

            ' Write the message and return its length
            Dim stream As New MemoryStream()
            encoder.WriteMessage(msg, stream)
            Dim size As Integer = CInt(stream.Length)

            msg.Close()
            stream.Close()
            Return size

        End Function

        Private Shared Function SizeOfMtomMessage(ByVal msg As Message) As Integer

            ' Create an MTOM encoder
            Dim element As MessageEncodingBindingElement = New MtomMessageEncodingBindingElement()
            Dim factory As MessageEncoderFactory = element.CreateMessageEncoderFactory()
            Dim encoder As MessageEncoder = factory.Encoder

            ' Write the message and return its length
            Dim stream As New MemoryStream()
            encoder.WriteMessage(msg, stream)

            Dim size As Integer = CInt(stream.Length)
            stream.Close()
            msg.Close()
            Return size

        End Function

    End Class

End Namespace
