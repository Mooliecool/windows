' Copyright (c) Microsoft Corporation.  All Rights Reserved.

Imports System
Imports System.IO
Imports System.Runtime.Serialization
Imports System.ServiceModel
Imports System.Xml.Serialization

Namespace Microsoft.ServiceModel.Samples

    'The service contract is defined in generatedClient.vb, generated from the service by the svcutil tool.

    'Client implementation code.
    Class Client

        Public Shared Sub Main()

            Dim filePath As String = Path.Combine(System.Environment.CurrentDirectory, "clientfile")
            Console.WriteLine("Press <ENTER> when service is ready")
            Console.ReadLine()
            ' Create a client with given client endpoint configuration

            Dim client1 As New StreamingSampleClient("BasicHttpBinding_IStreamingSample")

            Console.WriteLine("------ Using HTTP ------ ")

            Console.WriteLine("Calling GetStream()")
            Dim stream1 As Stream = client1.GetStream("some dummy data")
            SaveStreamToFile(stream1, filePath)

            Console.WriteLine("Calling UploadStream()")
            Dim instream1 As FileStream = File.OpenRead(filePath)
            Dim result1 As Boolean = client1.UploadStream(instream1)

            instream1.Close()

            Console.WriteLine("Calling GetReversedStream()")
            stream1 = client1.GetReversedStream()
            SaveStreamToFile(stream1, filePath)
            client1.Close()
            'repeating using TCP
            Dim client2 As New StreamingSampleClient("CustomBinding_IStreamingSample")

            Console.WriteLine("------ Using Custom HTTP ------ ")

            Console.WriteLine("Calling GetStream()")
            Dim stream2 As Stream = client2.GetStream("some dummy data")
            SaveStreamToFile(stream2, filePath)

            Console.WriteLine("Calling UploadStream()")
            Dim instream2 As FileStream = File.OpenRead(filePath)
            Dim result2 As Boolean = client2.UploadStream(instream2)

            instream2.Close()

            Console.WriteLine("Calling GetReversedStream()")
            stream2 = client2.GetReversedStream()
            SaveStreamToFile(stream2, filePath)
            client2.Close()

            Console.WriteLine()
            Console.WriteLine("Press <ENTER> to terminate client.")
            Console.ReadLine()

        End Sub

        Private Shared Sub SaveStreamToFile(ByVal stream As Stream, ByVal filePath As String)

            Console.WriteLine("Saving to file {0}", filePath)
            Dim outstream As FileStream = File.Open(filePath, FileMode.Create, FileAccess.Write)
            CopyStream(stream, outstream)
            outstream.Close()
            stream.Close()
            Console.WriteLine()
            Console.WriteLine("File {0} saved", filePath)

        End Sub

        Private Shared Sub CopyStream(ByVal instream As Stream, ByVal outstream As Stream)

            'read from the input stream in 4K chunks
            'and save to output stream
            Const bufferLen As Integer = 4096
            Dim buffer(bufferLen) As Byte
            Dim count As Integer = 0
            Dim bytecount As Integer = 0
            count = instream.Read(buffer, 0, bufferLen)
            While (count > 0)

                outstream.Write(buffer, 0, count)
                Console.Write(".")
                bytecount += count
                count = instream.Read(buffer, 0, bufferLen)

            End While
            Console.WriteLine()
            Console.WriteLine("Wrote {0} bytes to stream", bytecount)

        End Sub

    End Class

End Namespace
