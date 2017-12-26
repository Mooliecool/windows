' Copyright (c) Microsoft Corporation.  All Rights Reserved.

Imports System
Imports System.ServiceModel
Imports System.IO

Namespace Microsoft.ServiceModel.Samples

    ' Service contract for an upload service
    <ServiceContract([Namespace]:="http://Microsoft.ServiceModel.Samples")> _
    Public Interface IUpload

        <OperationContract()> _
        Function Upload(ByVal data As Stream) As Integer

    End Interface
    ' An upload service which takes a Stream and returns its size

    Public Class UploadService
        Implements IUpload

        Public Function Upload(ByVal data As Stream) As Integer Implements IUpload.Upload

            Dim size As Integer = 0
            Dim bytesRead As Integer = 0
            Dim buffer() As Byte = New Byte(1023) {}

            ' Read all the data from the stream
            Do

                bytesRead = data.Read(buffer, 0, buffer.Length)
                size += bytesRead

            Loop While bytesRead > 0
            data.Close()

            Return size

        End Function

    End Class

End Namespace
