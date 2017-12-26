' Copyright (c) Microsoft Corporation.  All Rights Reserved.

Imports System
Imports System.IO
Imports System.Runtime.Serialization
Imports System.ServiceModel
Imports System.Xml

Namespace Microsoft.ServiceModel.Samples

    Class Client

        Public Shared Sub Main()

            Dim record1 As New Record(1, 2, "+", 3)
            Console.WriteLine("Original record: {0}", record1.ToString())

            Dim stream1 As New MemoryStream()

            'Serialize the Record object to a memory stream using DataContractSerializer.
            Dim serializer As New DataContractSerializer(GetType(Record))
            serializer.WriteObject(stream1, record1)

            stream1.Position = 0

            'Deserialize the Record object back into a new record object
            Dim record2 As Record = DirectCast(serializer.ReadObject(stream1), Record)

            Console.WriteLine("Deserialized record: {0}", record2.ToString())

            Dim stream2 As New MemoryStream()

            Dim binaryDictionaryWriter As XmlDictionaryWriter = XmlDictionaryWriter.CreateBinaryWriter(stream2)
            serializer.WriteObject(binaryDictionaryWriter, record1)
            binaryDictionaryWriter.Flush()

            'report the length of the streams
            Console.WriteLine("Text Stream is {0} bytes long", stream1.Length)
            Console.WriteLine("Binary Stream is {0} bytes long", stream2.Length)

            Console.WriteLine()
            Console.WriteLine("Press <ENTER> to terminate client.")
            Console.ReadLine()

        End Sub

    End Class

    <DataContract([Namespace]:="http://Microsoft.ServiceModel.Samples")> _
    Friend Class Record

        Private n1 As Double
        Private n2 As Double
        Private m_operation As String
        Private m_result As Double

        Friend Sub New(ByVal n1 As Double, ByVal n2 As Double, ByVal operation As String, ByVal result As Double)

            Me.n1 = n1
            Me.n2 = n2
            Me.m_operation = operation
            Me.m_result = result

        End Sub

        <DataMember()> _
        Friend Property OperandNumberOne() As Double

            Get

                Return n1

            End Get

            Set(ByVal value As Double)

                n1 = value

            End Set

        End Property

        <DataMember()> _
        Friend Property OperandNumberTwo() As Double

            Get

                Return n2

            End Get

            Set(ByVal value As Double)

                n2 = value

            End Set

        End Property

        <DataMember()> _
        Friend Property Operation() As String

            Get

                Return m_operation

            End Get

            Set(ByVal value As String)

                m_operation = value

            End Set

        End Property

        <DataMember()> _
        Friend Property Result() As Double

            Get

                Return m_result

            End Get

            Set(ByVal value As Double)

                m_result = value

            End Set

        End Property

        Public Overloads Overrides Function ToString() As String

            Return String.Format("Record: {0} {1} {2} = {3}", n1, m_operation, n2, m_result)

        End Function

    End Class

End Namespace