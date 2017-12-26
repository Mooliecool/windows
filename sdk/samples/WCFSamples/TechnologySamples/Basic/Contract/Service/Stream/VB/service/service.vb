' Copyright (c) Microsoft Corporation.  All Rights Reserved.

Imports System
Imports System.Configuration
Imports System.IO
Imports System.Runtime.Serialization
Imports System.ServiceModel
Imports System.Xml.Serialization

Namespace Microsoft.ServiceModel.Samples

    <ServiceContract([Namespace]:="http://Microsoft.ServiceModel.Samples")> _
    Public Interface IStreamingSample

        <OperationContract()> _
        Function GetStream(ByVal data As String) As Stream
        <OperationContract()> _
        Function UploadStream(ByVal stream As Stream) As Boolean
        <OperationContract()> _
        Function EchoStream(ByVal stream As Stream) As Stream
        <OperationContract()> _
        Function GetReversedStream() As Stream

    End Interface

    ' Service class which implements the service contract
    Public Class StreamingService
        Implements IStreamingSample

        Public Function GetStream(ByVal data As String) As Stream Implements IStreamingSample.GetStream

            'this file path assumes the image is in
            ' the Service folder and the service is executing
            ' in service/bin 
            Dim filePath As String = Path.Combine(System.Environment.CurrentDirectory, ".\image.jpg")
            'open the file, this could throw an exception 
            '(e.g. if the file is not found)
            'having includeExceptionDetailInFaults="True" in config 
            ' would cause this exception to be returned to the client
            Try

                Dim imageFile As FileStream = File.OpenRead(filePath)
                Return imageFile

            Catch ex As IOException

                Console.WriteLine([String].Format("An exception was thrown while trying to open file {0}", filePath))
                Console.WriteLine("Exception is: ")
                Console.WriteLine(ex.ToString())
                Throw ex

            End Try

        End Function

        Public Function UploadStream(ByVal stream As Stream) As Boolean Implements IStreamingSample.UploadStream

            'this implementation places the uploaded file
            'in the current directory and calls it "uploadedfile"
            'with no file extension
            Dim filePath As String = Path.Combine(System.Environment.CurrentDirectory, "uploadedfile")
            Try

                Console.WriteLine("Saving to file {0}", filePath)
                Dim outstream As FileStream = File.Open(filePath, FileMode.Create, FileAccess.Write)
                'read from the input stream in 4K chunks
                'and save to output stream
                Const bufferLen As Integer = 4096
                Dim buffer(bufferLen) As Byte
                Dim count As Integer = 0
                count = stream.Read(buffer, 0, bufferLen)
                While (count > 0)

                    Console.Write(".")
                    outstream.Write(buffer, 0, count)
                    count = stream.Read(buffer, 0, bufferLen)

                End While
                outstream.Close()
                stream.Close()
                Console.WriteLine()
                Console.WriteLine("File {0} saved", filePath)

                Return True

            Catch ex As IOException

                Console.WriteLine([String].Format("An exception was thrown while opening or writing to file {0}", filePath))
                Console.WriteLine("Exception is: ")
                Console.WriteLine(ex.ToString())
                Throw ex

            End Try

        End Function

        Public Function EchoStream(ByVal stream As Stream) As Stream Implements IStreamingSample.EchoStream

            'this implementation places the uploaded file
            'in the current directory and calls it "echofile"
            'with no file extension
            Dim filePath As String = Path.Combine(System.Environment.CurrentDirectory, "echofile")
            Try

                Dim outstream As FileStream = File.Open(filePath, FileMode.Create, FileAccess.Write)
                'copy the input stream to the output file
                Me.CopyStream(stream, outstream)
                outstream.Close()
                stream.Close()

                'now open the file for reading
                'and return the stream
                Dim echoFile As FileStream = File.OpenRead(filePath)
                Return echoFile

            Catch ex As IOException

                Console.WriteLine([String].Format("An exception was thrown while opening or writing to file {0}", filePath))
                Console.WriteLine("Exception is: ")
                Console.WriteLine(ex.ToString())
                Throw ex

            End Try

        End Function

        Public Function GetReversedStream() As Stream Implements IStreamingSample.GetReversedStream

            'this file path assumes the image is in
            ' the Service folder and the service is executing
            ' in Service/bin
            Dim filePath As String = Path.Combine(System.Environment.CurrentDirectory, ".\image.jpg")

            Dim stream As New ReverseStream(filePath)
            Return stream

        End Function

        Private Sub CopyStream(ByVal instream As Stream, ByVal outstream As Stream)

            'read from the input stream in 4K chunks
            'and save to output stream
            Const bufferLen As Integer = 4096
            Dim buffer(bufferLen) As Byte
            Dim count As Integer = 0
            count = instream.Read(buffer, 0, bufferLen)
            While (count > 0)

                outstream.Write(buffer, 0, count)
                count = instream.Read(buffer, 0, bufferLen)

            End While

        End Sub

    End Class

    Class ReverseStream
        Inherits Stream

        Private inStream As FileStream
        Friend Sub New(ByVal filePath As String)

            'opens the file and places a StreamReader around it
            inStream = File.OpenRead(filePath)

        End Sub

        Public Overloads Overrides ReadOnly Property CanRead() As Boolean

            Get

                Return inStream.CanRead

            End Get

        End Property

        Public Overloads Overrides ReadOnly Property CanSeek() As Boolean

            Get

                Return False

            End Get

        End Property

        Public Overloads Overrides ReadOnly Property CanWrite() As Boolean

            Get

                Return False

            End Get

        End Property

        Public Overloads Overrides Sub Flush()

            Throw New Exception("This stream does not support writing.")

        End Sub

        Public Overloads Overrides ReadOnly Property Length() As Long

            Get

                Throw New Exception("This stream does not support the Length property.")

            End Get

        End Property

        Public Overloads Overrides Property Position() As Long

            Get

                Return inStream.Position

            End Get
            Set(ByVal value As Long)

                Throw New Exception("This stream does not support setting the Position property.")

            End Set

        End Property

        Public Overloads Overrides Function Read(ByVal buffer As Byte(), ByVal offset As Integer, ByVal count As Integer) As Integer

            Dim countRead As Integer = inStream.Read(buffer, offset, count)
            ReverseBuffer(buffer, offset, countRead)
            Return countRead

        End Function

        Public Overloads Overrides Function Seek(ByVal offset As Long, ByVal origin As SeekOrigin) As Long

            Throw New Exception("This stream does not support seeking.")

        End Function

        Public Overloads Overrides Sub SetLength(ByVal value As Long)

            Throw New Exception("This stream does not support setting the Length.")

        End Sub

        Public Overloads Overrides Sub Write(ByVal buffer As Byte(), ByVal offset As Integer, ByVal count As Integer)

            Throw New Exception("This stream does not support writing.")

        End Sub

        Public Overloads Overrides Sub Close()

            Try
            
                inStream.Close()

            Finally
     
                MyBase.Close()
     
            End Try

        End Sub

        Protected Overloads Overrides Sub Dispose(ByVal disposing As Boolean)

            Try
	
                inStream.Dispose()

            Finally
    
                 MyBase.Dispose(disposing)

            End Try

        End Sub

        Private Sub ReverseBuffer(ByVal buffer As Byte(), ByVal offset As Integer, ByVal count As Integer)

            Dim i As Integer, j As Integer

            i = offset
            j = offset + count - 1
            While i < j

                Dim currenti As Byte = buffer(i)
                buffer(i) = buffer(j)
                buffer(j) = currenti
                i += 1
                j -= 1

            End While

        End Sub

    End Class

End Namespace
