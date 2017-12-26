' Copyright (c) Microsoft Corporation. All rights reserved.

Imports System.ServiceModel

Namespace Microsoft.ServiceModel.Samples

    <ServiceContractAttribute([Namespace]:="http://Microsoft.ServiceModel.Samples")> _
    Interface IAlbumService

        <OperationContract()> _
        Function GetAlbumList() As Album()
        <OperationContract()> _
        Sub AddAlbum(ByVal title As String)

    End Interface

End Namespace
