' Copyright (c) Microsoft Corporation.  All Rights Reserved.

Imports System

Namespace Microsoft.ServiceModel.Samples

    Public Class Constants

        Public Class Addressing

            Public Const NamespaceUri As String = "http://www.w3.org/2005/08/addressing"
            Public Const NamespaceUriAugust2004 As String = "http://schemas.xmlsoap.org/ws/2004/08/addressing"

            Public Class Elements

                Public Const EndpointReference As String = "EndpointReference"

            End Class

        End Class

        Public Class Policy

            Public Const NamespaceUri As String = "http://schemas.xmlsoap.org/ws/2004/09/policy"

            Public Class Elements

                Public Const AppliesTo As String = "AppliesTo"

            End Class

        End Class

        ' Various constants for WS-Trust
        Public Class Trust

            Public Const NamespaceUri As String = "http://schemas.xmlsoap.org/ws/2005/02/trust"

            Public Class Actions

                Public Const Issue As String = "http://schemas.xmlsoap.org/ws/2005/02/trust/RST/Issue"
                Public Const IssueReply As String = "http://schemas.xmlsoap.org/ws/2005/02/trust/RSTR/Issue"

            End Class

            Public Class Attributes

                Public Const Context As String = "Context"
                Public Const Type As String = "Type"

            End Class

            Public Class Elements

                Public Const KeySize As String = "KeySize"
                Public Const KeyType As String = "KeyType"
                Public Const UseKey As String = "UseKey"
                Public Const Entropy As String = "Entropy"
                Public Const BinarySecret As String = "BinarySecret"
                Public Const RequestSecurityToken As String = "RequestSecurityToken"
                Public Const RequestSecurityTokenResponse As String = "RequestSecurityTokenResponse"
                Public Const RequestType As String = "RequestType"
                Public Const TokenType As String = "TokenType"
                Public Const RequestedSecurityToken As String = "RequestedSecurityToken"
                Public Const RequestedAttachedReference As String = "RequestedAttachedReference"
                Public Const RequestedUnattachedReference As String = "RequestedUnattachedReference"
                Public Const RequestedProofToken As String = "RequestedProofToken"
                Public Const ComputedKey As String = "ComputedKey"
                Public Const Claims As String = "Claims"

            End Class

            Public Class RequestTypes

                Public Const Issue As String = "http://schemas.xmlsoap.org/ws/2005/02/trust/Issue"

            End Class

            Public Class KeyTypes

                Public Const [Public] As String = "http://schemas.xmlsoap.org/ws/2005/02/trust/PublicKey"
                Public Const Symmetric As String = "http://schemas.xmlsoap.org/ws/2005/02/trust/SymmetricKey"

            End Class

            Public Class BinarySecretTypes

                Public Const AsymmetricKey As String = "http://schemas.xmlsoap.org/ws/2005/02/trust/AsymmetricKey"
                Public Const SymmetricKey As String = "http://schemas.xmlsoap.org/ws/2005/02/trust/SymmetricKey"
                Public Const Nonce As String = "http://schemas.xmlsoap.org/ws/2005/02/trust/Nonce"

            End Class

            Public Class ComputedKeyAlgorithms

                Public Const PSHA1 As String = "http://schemas.xmlsoap.org/ws/2005/02/trust/CK/PSHA1"

            End Class

        End Class

    End Class

End Namespace
