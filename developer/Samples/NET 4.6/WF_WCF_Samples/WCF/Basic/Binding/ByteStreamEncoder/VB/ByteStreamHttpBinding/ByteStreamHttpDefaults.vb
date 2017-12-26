'----------------------------------------------------------------
' Copyright (c) Microsoft Corporation.  All rights reserved.
'----------------------------------------------------------------

Imports Microsoft.VisualBasic
Imports System
Imports System.ServiceModel
Imports System.Xml

Namespace Microsoft.Samples.ByteStreamEncoder

    Friend NotInheritable Class ByteStreamHttpDefaults

        Friend Const DefaultBypassProxyOnLocal As Boolean = True

        Friend Const DefaultHostNameComparisonMode As HostNameComparisonMode = HostNameComparisonMode.StrongWildcard

        Friend Const DefaultMaxBufferPoolSize As Int64 = 512 * 1024

        Friend Const DefaultMaxBufferSize As Int32 = 65536

        Friend Const DefaultMaxReceivedMessageSize As Int64 = 65536

        Friend Const DefaultProxyAddress As String = Nothing

        Friend Const DefaultReaderQuotas As String = Nothing

        Friend Const DefaultTransferMode As TransferMode = TransferMode.Streamed

        Friend Const DefaultUseDefaultWebProxy As Boolean = True

    End Class
End Namespace
