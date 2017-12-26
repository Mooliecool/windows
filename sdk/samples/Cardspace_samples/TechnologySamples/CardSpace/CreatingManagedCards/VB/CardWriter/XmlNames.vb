Imports System
Imports System.Xml

Namespace ManagedCardWriter
    Public Class XmlNames
        ' Methods
        Private Sub New()
        End Sub

        Public Shared Function CreateNamespaceManager(ByVal nameTable As XmlNameTable) As XmlNamespaceManager
            Dim manager1 As New XmlNamespaceManager(nameTable)
            manager1.AddNamespace("saml", "urn:oasis:names:tc:SAML:1.0:assertion")
            manager1.AddNamespace("soap", "http://www.w3.org/2003/05/soap-envelope")
            manager1.AddNamespace("wsa", "http://www.w3.org/2005/08/addressing")
            manager1.AddNamespace("wsid", "http://schemas.xmlsoap.org/ws/2005/05/identity")
            manager1.AddNamespace("wsmex", "http://schemas.xmlsoap.org/ws/2004/09/mex")
            manager1.AddNamespace("wsp", "http://schemas.xmlsoap.org/ws/2004/09/policy")
            manager1.AddNamespace("wsse", "http://docs.oasis-open.org/wss/2004/01/oasis-200401-wss-wssecurity-secext-1.0.xsd")
            manager1.AddNamespace("wssu", "http://docs.oasis-open.org/wss/2004/01/oasis-200401-wss-wssecurity-utility-1.0.xsd")
            manager1.AddNamespace("wxf", "http://schemas.xmlsoap.org/ws/2004/09/transfer")
            manager1.AddNamespace("wst", "http://schemas.xmlsoap.org/ws/2005/02/trust")
            manager1.AddNamespace("dsig", "http://www.w3.org/2000/09/xmldsig#")
            manager1.AddNamespace("enc", "http://www.w3.org/2001/04/xmlenc#")
            manager1.AddNamespace("xsd", "http://www.w3.org/2001/XMLSchema")
            Return manager1
        End Function


        ' Nested Types
        Friend NotInheritable Class Saml10
            ' Methods
            Private Sub New()
            End Sub


            ' Fields
            Public Const DefaultPrefix As String = "saml"
            Public Const [Namespace] As String = "urn:oasis:names:tc:SAML:1.0:assertion"
        End Class

        Friend NotInheritable Class Saml11
            ' Methods
            Private Sub New()
            End Sub


            ' Fields
            Public Const AltNamespace As String = "http://docs.oasis-open.org/wss/oasis-wss-saml-token-profile-1.1#SAMLV1.1"
            Public Const DefaultPrefix As String = "saml"
            Public Const [Namespace] As String = "urn:oasis:names:tc:SAML:1.0:assertion"
        End Class

        Friend NotInheritable Class Soap11
            ' Methods
            Private Sub New()
            End Sub


            ' Fields
            Public Const DefaultPrefix As String = "soap"
            Public Const [Namespace] As String = "http://schemas.xmlsoap.org/soap/envelope/"
        End Class

        Friend NotInheritable Class Soap12
            ' Methods
            Private Sub New()
            End Sub


            ' Fields
            Public Const DefaultPrefix As String = "soap"
            Public Const [Namespace] As String = "http://www.w3.org/2003/05/soap-envelope"
        End Class

        Friend NotInheritable Class WSAddressing
            ' Methods
            Private Sub New()
            End Sub


            ' Fields
            Public Const Address As String = "Address"
            Public Const DefaultPrefix As String = "wsa"
            Public Const EndpointReference As String = "EndpointReference"
            Public Const LocalSchemaLocation As String = "addressing.xsd"
            Public Const Metadata As String = "Metadata"
            Public Const [Namespace] As String = "http://www.w3.org/2005/08/addressing"
            Public Const SchemaLocation As String = "http://www.w3.org/2005/08/addressing/addressing.xsd"
        End Class

        Friend NotInheritable Class WSAddressing04
            ' Methods
            Private Sub New()
            End Sub


            ' Fields
            Public Const DefaultPrefix As String = "wsa04"
            Public Const LocalSchemaLocation As String = "addressing04.xsd"
            Public Const [Namespace] As String = "http://schemas.xmlsoap.org/ws/2004/08/addressing"
            Public Const SchemaLocation As String = "http://schemas.xmlsoap.org/ws/2004/08/addressing/"
        End Class

        Friend NotInheritable Class WSIdentity
            ' Methods
            Private Sub New()
            End Sub


            ' Fields
            Public Const BackgroundColorElement As String = "BackgroundColor"
            Public Const CardIdElement As String = "CardId"
            Public Const CardImageElement As String = "CardImage"
            Public Const CardNameElement As String = "CardName"
            Public Const CardVersionElement As String = "CardVersion"
            Public Const ClaimElement As String = "ClaimType"
            Public Const ClaimValueElement As String = "ClaimValue"
            Public Const ClaimValueListElement As String = "ClaimValueList"
            Public Const DefaultPrefix As String = "wsid"
            Public Const DescriptionElement As String = "Description"
            Public Const DictionaryUri As String = "http://schemas.xmlsoap.org/ws/2005/05/identity/claims"
            Public Const DisplayClaimElement As String = "DisplayClaim"
            Public Const DisplayCredentialHintElement As String = "DisplayCredentialHint"
            Public Const DisplayTagElement As String = "DisplayTag"
            Public Const DisplayTokenElement As String = "DisplayToken"
            Public Const DisplayTokenTextElement As String = "DisplayTokenText"
            Public Const DisplayValueElement As String = "DisplayValue"
            Public Const EncryptedStoreElement As String = "EncryptedStore"
            Public Const EndpointIdentity As String = "Identity"
            Public Const HashSaltElement As String = "HashSalt"
            Public Const InfoCardElement As String = "InformationCard"
            Public Const InfoCardMetaDataElement As String = "InfoCardMetaData"
            Public Const InfoCardPrivateDataElement As String = "InfoCardPrivateData"
            Public Const InfoCardRefElement As String = "InformationCardReference"
            Public Const IsManagedElement As String = "IsManaged"
            Public Const IsSelfIssuedElement As String = "IsSelfIssued"
            Public Const IssuerElement As String = "Issuer"
            Public Const IssuerIdElement As String = "IssuerId"
            Public Const IssuerNameElement As String = "IssuerName"
            Public Const IssuerUriElement As String = "IssuerUri"
            Public Const IterationCountElement As String = "IterationCount"
            Public Const KerberosV5Auth As String = "http://schemas.xmlsoap.org/ws/2005/05/identity/A12nMethod/KerberosV5"
            Public Const KerberosV5CredentialElement As String = "KerberosV5Credential"
            Public Const LocalSchemaLocation As String = "identity.xsd"
            Public Const MasterKeyElement As String = "MasterKey"
            Public Const MaxTokenAgeElement As String = "MaxTokenAge"
            Public Const MethodAttribute As String = "Method"
            Public Const MimeTypeAttribute As String = "MimeType"
            Public Const [Namespace] As String = "http://schemas.xmlsoap.org/ws/2005/05/identity"
            Public Const OpaqueEndPointElement As String = "OpaqueEndpoint"
            Public Const OptionalAttribute As String = "Optional"
            Public Const PersonalCategoryUri As String = "http://icardissuer.xmlsoap.org/categories/identitycard"
            Public Const PinDigestElement As String = "PinDigest"
            Public Const PrivacyNoticeAtElement As String = "PrivacyNotice"
            Public Const PrivatePersonalIdentifierElement As String = "PrivatePersonalIdentifier"
            Public Const ProvisionAtElement As String = "ProvisionAt"
            Public Const RequestBrowserToken As String = "RequestBrowserToken"
            Public Const RequestDisplayTokenElement As String = "RequestDisplayToken"
            Public Const RequestedDisplayTokenElement As String = "RequestedDisplayToken"
            Public Const RequireAppliesToElement As String = "RequireAppliesTo"
            Public Const RequirePinProtectionElement As String = "RequirePinProtection"
            Public Const RoamingInfoCardElement As String = "RoamingInformationCard"
            Public Const RoamingStoreElement As String = "RoamingStore"
            Public Const SelfIssuedAuth As String = "http://schemas.xmlsoap.org/ws/2005/05/identity/A12nMethod/SelfIssuedToken"
            Public Const SelfIssuedCredentialElement As String = "SelfIssuedCredential"
            Public Const SelfIssuerUri As String = "http://schemas.xmlsoap.org/ws/2005/05/identity/issuer/self"
            Public Const StoreSaltElement As String = "StoreSalt"
            Public Const SupportedClaimTypeElement As String = "SupportedClaimType"
            Public Const SupportedClaimTypeListElement As String = "SupportedClaimTypeList"
            Public Const SupportedTokenTypeListElement As String = "SupportedTokenTypeList"
            Public Const ThumbprintUri As String = "http://schemas.xmlsoap.org/ws/2005/05/identity#KeyThumbprint"
            Public Const TimeExpiresElement As String = "TimeExpires"
            Public Const TimeIssuedElement As String = "TimeIssued"
            Public Const TimeLastUpdatedElement As String = "TimeLastUpdated"
            Public Const TokenServiceElement As String = "TokenService"
            Public Const TokenServiceListElement As String = "TokenServiceList"
            Public Const UriAttribute As String = "Uri"
            Public Const UserCredentialElement As String = "UserCredential"
            Public Const UserNameElement As String = "Username"
            Public Const UserNamePasswordAuth As String = "http://schemas.xmlsoap.org/ws/2005/05/identity/A12nMethod/UsernamePassword"
            Public Const UserNamePasswordCredentialElement As String = "UsernamePasswordCredential"
            Public Const UserPrincipalNameElement As String = "UserPrincipalName"
            Public Const ValueElement As String = "Value"
            Public Const X509V3CredentialElement As String = "X509V3Credential"
            Public Const X509V3SmartCardAuth As String = "http://schemas.xmlsoap.org/ws/2005/05/identity/A12nMethod/X509V3SmartCard"
            Public Const X509V3SoftAuth As String = "http://schemas.xmlsoap.org/ws/2005/05/identity/A12nMethod/X509V3Soft"
        End Class

        Friend NotInheritable Class WSMetadataExchange
            ' Methods
            Private Sub New()
            End Sub


            ' Fields
            Public Const DefaultPrefix As String = "wsmex"
            Public Const [Namespace] As String = "http://schemas.xmlsoap.org/ws/2004/09/mex"
        End Class

        Friend NotInheritable Class WSPolicy
            ' Methods
            Private Sub New()
            End Sub


            ' Fields
            Public Const AppliesTo As String = "AppliesTo"
            Public Const DefaultPrefix As String = "wsp"
            Public Const LocalSchemaLocation As String = "ws-policy.xsd"
            Public Const [Namespace] As String = "http://schemas.xmlsoap.org/ws/2004/09/policy"
            Public Const Policy As String = "Policy"
            Public Const SchemaLocation As String = "http://schemas.xmlsoap.org/ws/2004/09/policy/ws-policy.xsd"
        End Class

        Friend NotInheritable Class WSSecurityExt
            ' Methods
            Private Sub New()
            End Sub


            ' Fields
            Public Const DefaultPrefix As String = "wsse"
            Public Const KeyIdentifierElement As String = "KeyIdentifier"
            Public Const LocalSchemaLocation As String = "oasis-200401-wss-wssecurity-secext-1.0.xsd"
            Public Const [Namespace] As String = "http://docs.oasis-open.org/wss/2004/01/oasis-200401-wss-wssecurity-secext-1.0.xsd"
            Public Const SchemaLocation As String = "http://docs.oasis-open.org/wss/2004/01/oasis-200401-wss-wssecurity-secext-1.0.xsd"
            Public Const Sha1ThumbrpintKeyTypeValue As String = "http://docs.oasis-open.org/wss/2004/xx/oasis-2004xx-wss-soap-message-security-1.1#ThumbprintSHA1"
            Public Const ValueTypeAttribute As String = "ValueType"
        End Class

        Friend NotInheritable Class WSSecurityUtility
            ' Methods
            Private Sub New()
            End Sub


            ' Fields
            Public Const DefaultPrefix As String = "wssu"
            Public Const LocalSchemaLocation As String = "oasis-200401-wss-wssecurity-utility-1.0.xsd"
            Public Const [Namespace] As String = "http://docs.oasis-open.org/wss/2004/01/oasis-200401-wss-wssecurity-utility-1.0.xsd"
            Public Const SchemaLocation As String = "http://docs.oasis-open.org/wss/2004/01/oasis-200401-wss-wssecurity-utility-1.0.xsd"
        End Class

        Friend NotInheritable Class WSTransfer
            ' Methods
            Private Sub New()
            End Sub


            ' Fields
            Public Const Create As String = "Create"
            Public Const CreateAction As String = "http://schemas.xmlsoap.org/ws/2004/09/transfer/Create"
            Public Const CreateResponseAction As String = "http://schemas.xmlsoap.org/ws/2004/09/transfer/CreateResponse"
            Public Const DefaultPrefix As String = "wxf"
            Public Const Delete As String = "Delete"
            Public Const DeleteAction As String = "http://schemas.xmlsoap.org/ws/2004/09/transfer/Delete"
            Public Const DeleteResponseAction As String = "http://schemas.xmlsoap.org/ws/2004/09/transfer/DeleteResponse"
            Public Const [Get] As String = "Get"
            Public Const GetAction As String = "http://schemas.xmlsoap.org/ws/2004/09/transfer/Get"
            Public Const GetResponseAction As String = "http://schemas.xmlsoap.org/ws/2004/09/transfer/GetResponse"
            Public Const [Namespace] As String = "http://schemas.xmlsoap.org/ws/2004/09/transfer"
            Public Const Put As String = "Put"
            Public Const PutAction As String = "http://schemas.xmlsoap.org/ws/2004/09/transfer/Put"
            Public Const PutResponseAction As String = "http://schemas.xmlsoap.org/ws/2004/09/transfer/PutResponse"
            Public Const ResourceCreated As String = "ResourceCreated"
            Public Const WSTransferContractName As String = "WS-Transfer"
        End Class

        Friend NotInheritable Class WSTrust
            ' Methods
            Shared Sub New()
                WSTrust.KeyTypeSymmetric = New Uri("http://schemas.xmlsoap.org/ws/2005/02/trust/SymmetricKey")
                WSTrust.KeyTypeAsymmetric = New Uri("http://schemas.xmlsoap.org/ws/2005/02/trust/PublicKey")
            End Sub

            Private Sub New()
            End Sub


            ' Fields
            Public Const ClaimsElement As String = "Claims"
            Public Const CustomToken As String = "customToken"
            Public Const DefaultPrefix As String = "wst"
            Public Const EncryptWith As String = "EncryptWith"
            Public Const KeySize As String = "KeySize"
            Public Const KeyType As String = "KeyType"
            Public Shared ReadOnly KeyTypeAsymmetric As Uri
            Public Shared ReadOnly KeyTypeSymmetric As Uri
            Public Const LocalSchemaLocation As String = "ws-trust.xsd"
            Public Const [Namespace] As String = "http://schemas.xmlsoap.org/ws/2005/02/trust"
            Public Const RequestedSecurityToken As String = "RequestedSecurityToken"
            Public Const RequestSecurityToken As String = "RequestSecurityToken"
            Public Const RequestSecurityTokenAction As String = "http://schemas.xmlsoap.org/ws/2005/02/trust/RST/Issue"
            Public Const RequestSecurityTokenResponseAction As String = "http://schemas.xmlsoap.org/ws/2005/02/trust/RSTR/Issue"
            Public Const SchemaLocation As String = "http://schemas.xmlsoap.org/ws/2005/02/trust/ws-trust.xsd"
            Public Const SigAttribute As String = "Sig"
            Public Const TokenType As String = "TokenType"
            Public Const UseKey As String = "UseKey"
        End Class

        Friend NotInheritable Class Xml
            ' Methods
            Private Sub New()
            End Sub


            ' Fields
            Public Const DateTimeFormat As String = "yyyy-MM-ddTHH:mm:ssZ"
            Public Const DefaultPrefix As String = "xml"
            Public Const Language As String = "lang"
            Public Const [Namespace] As String = "http://www.w3.org/XML/1998/namespace"
        End Class

        Friend NotInheritable Class XmlDSig
            ' Methods
            Private Sub New()
            End Sub


            ' Fields
            Public Const DefaultPrefix As String = "dsig"
            Public Const LocalSchemaLocation As String = "xmldsig-core-schema.xsd"
            Public Const [Namespace] As String = "http://www.w3.org/2000/09/xmldsig#"
            Public Const RSAKeyValueElement As String = "RSAKeyValue"
            Public Const SchemaLocation As String = "http://www.w3.org/TR/xmldsig-core/xmldsig-core-schema.xsd"
            Public Const Signature As String = "Signature"
            Public Const X509CertificateElement As String = "X509Certificate"
            Public Const X509DataElement As String = "X509Data"
            Public Const X509IssuerNameElement As String = "X509IssuerName"
            Public Const X509IssuerSerialElement As String = "X509IssuerSerial"
            Public Const X509SerialNumberElement As String = "X509SerialNumber"
        End Class

        Friend NotInheritable Class XmlEnc
            ' Methods
            Private Sub New()
            End Sub


            ' Fields
            Public Const Algorithm As String = "Algorithm"
            Public Const CipherData As String = "CipherData"
            Public Const CipherValue As String = "CipherValue"
            Public Const DefaultPrefix As String = "enc"
            Public Const Encoding As String = "Encoding"
            Public Const EncryptedData As String = "EncryptedData"
            Public Const EncryptionMethod As String = "EncryptionMethod"
            Public Const EncryptionProperties As String = "EncryptionProperties"
            Public Const Id As String = "Id"
            Public Const LocalSchemaLocation As String = "xenc-schema.xsd"
            Public Const MimeType As String = "MimeType"
            Public Const [Namespace] As String = "http://www.w3.org/2001/04/xmlenc#"
            Public Const SchemaLocation As String = "http://www.w3.org/TR/xmlenc-core/xenc-schema.xsd"
            Public Const Type As String = "Type"
        End Class

        Friend NotInheritable Class XmlSchema
            ' Methods
            Private Sub New()
            End Sub


            ' Fields
            Public Const DefaultPrefix As String = "xsd"
            Public Const LocalSchemaLocation As String = "xml.xsd"
            Public Const [Namespace] As String = "http://www.w3.org/2001/XMLSchema"
            Public Const SchemaLocation As String = "http://www.w3.org/2001/xml.xsd"
        End Class
    End Class
End Namespace


