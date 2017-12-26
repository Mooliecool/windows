Imports System
Imports System.Collections.Generic
Imports System.IO
Imports System.Security.Cryptography.X509Certificates
Imports System.Security.Cryptography.Xml
Imports System.ServiceModel
Imports System.ServiceModel.Channels
Imports System.ServiceModel.Description
Imports System.Text
Imports System.Xml

Namespace ManagedCardWriter
    Friend Class ManagedInformationCard
        ' Methods
        Public Sub New(ByVal cardType As CardType)
            Me.m_cardId = String.Empty
            Me.m_cardName = String.Empty
            Me.m_issuerName = String.Empty
            Me.m_issuerId = String.Empty
            Me.m_language = String.Empty
            Me.m_cardVersion = String.Empty
            Me.m_mimeType = String.Empty
            Me.m_supportedClaims = New List(Of ClaimInfo)
            Me.m_epr = Nothing
            Me.m_hint = String.Empty
            Me.m_credentialIdentifier = String.Empty
            Me.m_cardType = cardType.None
            Me.m_mexUri = String.Empty
            Me.m_cardId = "http://www.fabrikam.com/card/1"
            Me.m_cardName = "My new card"
            Me.m_issuerId = "http://localhost:4444/sts"
            Me.m_issuerName = "Microsoft"
            Me.m_issuedOn = DateTime.Now
            Me.m_expiresOn = DateTime.MaxValue
            Me.m_requireAppliesTo = False
            Me.m_language = "en-us"
            Me.m_cardVersion = "1"
            Me.m_cardType = cardType
            Me.m_mexUri = "https://localhost:4445/sts/mex"
        End Sub

        Public Function IsComplete() As Boolean
            Return (((((Not String.IsNullOrEmpty(Me.m_cardVersion) AndAlso Not String.IsNullOrEmpty(Me.m_cardId)) AndAlso (Not String.IsNullOrEmpty(Me.m_issuerName) AndAlso Not String.IsNullOrEmpty(Me.m_issuerId))) AndAlso ((Not String.IsNullOrEmpty(Me.m_mexUri) AndAlso (Not Me.m_supportedClaims Is Nothing)) AndAlso ((Me.m_supportedClaims.Count > 0) AndAlso (Not Me.m_tokenTypes Is Nothing)))) AndAlso (Me.m_tokenTypes.Length > 0)) AndAlso (((Me.m_cardType <> CardType.SelfIssuedAuth) AndAlso (Me.m_cardType <> CardType.SmartCard)) OrElse Not String.IsNullOrEmpty(Me.m_credentialIdentifier)))
        End Function

        Public Sub SerializeAndSign(ByVal filename As String, ByVal cert As X509Certificate2)
            Dim stream1 As New MemoryStream
            Dim writer1 As XmlWriter = XmlWriter.Create(stream1)
            writer1.WriteStartElement("InformationCard", "http://schemas.xmlsoap.org/ws/2005/05/identity")
            writer1.WriteAttributeString("lang", "http://www.w3.org/XML/1998/namespace", Me.m_language)
            writer1.WriteStartElement("InformationCardReference", "http://schemas.xmlsoap.org/ws/2005/05/identity")
            writer1.WriteElementString("CardId", "http://schemas.xmlsoap.org/ws/2005/05/identity", Me.m_cardId)
            writer1.WriteElementString("CardVersion", "http://schemas.xmlsoap.org/ws/2005/05/identity", Me.m_cardVersion)
            writer1.WriteEndElement()
            If Not String.IsNullOrEmpty(Me.m_cardName) Then
                writer1.WriteStartElement("CardName", "http://schemas.xmlsoap.org/ws/2005/05/identity")
                writer1.WriteString(Me.m_cardName)
                writer1.WriteEndElement()
            End If
            If ((Not Me.m_logo Is Nothing) AndAlso (0 <> Me.m_logo.Length)) Then
                writer1.WriteStartElement("CardImage", "http://schemas.xmlsoap.org/ws/2005/05/identity")
                If Not String.IsNullOrEmpty(Me.m_mimeType) Then
                    writer1.WriteAttributeString("MimeType", Me.m_mimeType)
                End If
                Dim text1 As String = Convert.ToBase64String(Me.m_logo)
                writer1.WriteString(text1)
                writer1.WriteEndElement()
            End If
            writer1.WriteStartElement("Issuer", "http://schemas.xmlsoap.org/ws/2005/05/identity")
            writer1.WriteString(Me.m_issuerId)
            writer1.WriteEndElement()
            'writer1.WriteStartElement("IssuerName", "http://schemas.xmlsoap.org/ws/2005/05/identity")
            'writer1.WriteString(Me.m_issuerName)
            'writer1.WriteEndElement()
            writer1.WriteStartElement("TimeIssued", "http://schemas.xmlsoap.org/ws/2005/05/identity")
            writer1.WriteString(XmlConvert.ToString(Me.m_issuedOn, XmlDateTimeSerializationMode.Utc))
            writer1.WriteEndElement()
            writer1.WriteStartElement("TimeExpires", "http://schemas.xmlsoap.org/ws/2005/05/identity")
            writer1.WriteString(XmlConvert.ToString(Me.m_expiresOn, XmlDateTimeSerializationMode.Utc))
            writer1.WriteEndElement()
            writer1.WriteStartElement("TokenServiceList", "http://schemas.xmlsoap.org/ws/2005/05/identity")
            Dim builder1 As New EndpointAddressBuilder
            builder1.Uri = New Uri(Me.m_issuerId)
            builder1.Identity = New X509CertificateEndpointIdentity(cert)
            If (Not Nothing Is Me.m_mexUri) Then
                Dim reference1 As New MetadataReference
                reference1.Address = New EndpointAddress(Me.m_mexUri)
                reference1.AddressVersion = AddressingVersion.WSAddressing10
                Dim section1 As New MetadataSection
                section1.Metadata = reference1
                Dim set1 As New MetadataSet
                set1.MetadataSections.Add(section1)
                Dim stream2 As New MemoryStream
                Dim writer2 As New XmlTextWriter(stream2, Encoding.UTF8)
                set1.WriteTo(writer2)
                writer2.Flush()
                stream2.Seek(CLng(0), SeekOrigin.Begin)
                Dim reader1 As XmlDictionaryReader = XmlDictionaryReader.CreateTextReader(stream2, XmlDictionaryReaderQuotas.Max)
                builder1.SetMetadataReader(reader1)
            End If
            Me.m_epr = builder1.ToEndpointAddress
            writer1.WriteStartElement("TokenService", "http://schemas.xmlsoap.org/ws/2005/05/identity")
            Me.m_epr.WriteTo(AddressingVersion.WSAddressing10, writer1)
            writer1.WriteStartElement("UserCredential", "http://schemas.xmlsoap.org/ws/2005/05/identity")
            If Not String.IsNullOrEmpty(Me.m_hint) Then
                writer1.WriteStartElement("DisplayCredentialHint", "http://schemas.xmlsoap.org/ws/2005/05/identity")
                writer1.WriteString(Me.m_hint)
                writer1.WriteEndElement()
            End If
            Select Case Me.m_cardType
                Case CardType.UserNamePassword
                    writer1.WriteStartElement("UsernamePasswordCredential", "http://schemas.xmlsoap.org/ws/2005/05/identity")
                    If Not String.IsNullOrEmpty(Me.m_credentialIdentifier) Then
                        writer1.WriteStartElement("Username", "http://schemas.xmlsoap.org/ws/2005/05/identity")
                        writer1.WriteString(Me.m_credentialIdentifier)
                        writer1.WriteEndElement()
                    End If
                    writer1.WriteEndElement()
                    Exit Select
                Case CardType.KerberosAuth
                    writer1.WriteStartElement("KerberosV5Credential", "http://schemas.xmlsoap.org/ws/2005/05/identity")
                    writer1.WriteEndElement()
                    Exit Select
                Case CardType.SelfIssuedAuth
                    writer1.WriteStartElement("SelfIssuedCredential", "http://schemas.xmlsoap.org/ws/2005/05/identity")
                    If String.IsNullOrEmpty(Me.m_credentialIdentifier) Then
                        Throw New Exception("No PPID was specified")
                    End If
                    writer1.WriteStartElement("PrivatePersonalIdentifier", "http://schemas.xmlsoap.org/ws/2005/05/identity")
                    writer1.WriteString(Me.m_credentialIdentifier)
                    writer1.WriteEndElement()
                    writer1.WriteEndElement()
                    Exit Select
                Case CardType.SmartCard
                    writer1.WriteStartElement("X509V3Credential", "http://schemas.xmlsoap.org/ws/2005/05/identity")
                    writer1.WriteStartElement("X509Data", "http://www.w3.org/2000/09/xmldsig#")
                    If String.IsNullOrEmpty(Me.m_credentialIdentifier) Then
                        Throw New Exception("No thumbprint was specified")
                    End If
                    writer1.WriteStartElement("KeyIdentifier", "http://docs.oasis-open.org/wss/2004/01/oasis-200401-wss-wssecurity-secext-1.0.xsd")
                    writer1.WriteAttributeString("ValueType", Nothing, "http://docs.oasis-open.org/wss/2004/xx/oasis-2004xx-wss-soap-message-security-1.1#ThumbprintSHA1")
                    writer1.WriteString(Me.m_credentialIdentifier)
                    writer1.WriteEndElement()
                    writer1.WriteEndElement()
                    writer1.WriteEndElement()
                    Exit Select
            End Select
            writer1.WriteEndElement()
            writer1.WriteEndElement()
            writer1.WriteEndElement()
            writer1.WriteStartElement("SupportedTokenTypeList", "http://schemas.xmlsoap.org/ws/2005/05/identity")
            Dim text2 As String
            For Each text2 In Me.m_tokenTypes
                writer1.WriteElementString("TokenType", "http://schemas.xmlsoap.org/ws/2005/02/trust", text2)
            Next
            writer1.WriteEndElement()
            writer1.WriteStartElement("SupportedClaimTypeList", "http://schemas.xmlsoap.org/ws/2005/05/identity")
            Dim info1 As ClaimInfo
            For Each info1 In Me.m_supportedClaims
                writer1.WriteStartElement("SupportedClaimType", "http://schemas.xmlsoap.org/ws/2005/05/identity")
                writer1.WriteAttributeString("Uri", info1.Id)
                If Not String.IsNullOrEmpty(info1.DisplayTag) Then
                    writer1.WriteElementString("DisplayTag", "http://schemas.xmlsoap.org/ws/2005/05/identity", info1.DisplayTag)
                End If
                If Not String.IsNullOrEmpty(info1.Description) Then
                    writer1.WriteElementString("Description", "http://schemas.xmlsoap.org/ws/2005/05/identity", info1.Description)
                End If
                writer1.WriteEndElement()
            Next
            writer1.WriteEndElement()
            If Me.m_requireAppliesTo Then
                writer1.WriteElementString("RequireAppliesTo", "http://schemas.xmlsoap.org/ws/2005/05/identity", Nothing)
            End If
            If Not String.IsNullOrEmpty(Me.m_privacyNoticeAt) Then
                writer1.WriteStartElement("PrivacyNotice", "http://schemas.xmlsoap.org/ws/2005/05/identity")
                writer1.WriteString(Me.m_privacyNoticeAt)
                writer1.WriteEndElement()
            End If
            writer1.WriteEndElement()
            writer1.Close()
            stream1.Position = 0
            Dim document1 As New XmlDocument
            document1.PreserveWhitespace = False
            document1.Load(stream1)
            Dim xml1 As New SignedXml
            xml1.SigningKey = cert.PrivateKey
            xml1.Signature.SignedInfo.CanonicalizationMethod = "http://www.w3.org/2001/10/xml-exc-c14n#"
            Dim reference2 As New Reference
            reference2.Uri = "#_Object_InfoCard"
            reference2.AddTransform(New XmlDsigExcC14NTransform)
            xml1.AddReference(reference2)
            Dim info2 As New KeyInfo
            Dim data1 As New KeyInfoX509Data(cert, X509IncludeOption.WholeChain)
            info2.AddClause(data1)
            xml1.KeyInfo = info2
            Dim obj1 As New DataObject("_Object_InfoCard", Nothing, Nothing, document1.DocumentElement)
            xml1.AddObject(obj1)
            xml1.ComputeSignature()
            Dim element1 As XmlElement = xml1.GetXml
            Dim writer3 As New XmlTextWriter(filename, Encoding.UTF8)
            element1.WriteTo(writer3)
            writer3.Flush()
            writer3.Close()
        End Sub


        ' Properties
        Public Property CardId() As String
            Get
                Return Me.m_cardId
            End Get
            Set(ByVal value As String)
                Me.m_cardId = value
            End Set
        End Property

        Public Property CardLogo() As Byte()
            Get
                Return Me.m_logo
            End Get
            Set(ByVal value As Byte())
                Me.m_logo = value
            End Set
        End Property

        Public Property CardName() As String
            Get
                Return Me.m_cardName
            End Get
            Set(ByVal value As String)
                Me.m_cardName = value
            End Set
        End Property

        Public ReadOnly Property CardType() As CardType
            Get
                Return Me.m_cardType
            End Get
        End Property

        Public Property CardVersion() As String
            Get
                Return Me.m_cardVersion
            End Get
            Set(ByVal value As String)
                Me.m_cardVersion = value
            End Set
        End Property

        Public Property CredentialHint() As String
            Get
                Return Me.m_hint
            End Get
            Set(ByVal value As String)
                Me.m_hint = value
            End Set
        End Property

        Public Property CredentialIdentifier() As String
            Get
                Return Me.m_credentialIdentifier
            End Get
            Set(ByVal value As String)
                Me.m_credentialIdentifier = value
            End Set
        End Property

        Public Property IssuerId() As String
            Get
                Return Me.m_issuerId
            End Get
            Set(ByVal value As String)
                Me.m_issuerId = value
            End Set
        End Property

        Public Property IssuerName() As String
            Get
                Return Me.m_issuerName
            End Get
            Set(ByVal value As String)
                Me.m_issuerName = value
            End Set
        End Property

        Public Property MexUri() As String
            Get
                Return Me.m_mexUri
            End Get
            Set(ByVal value As String)
                Me.m_mexUri = value
            End Set
        End Property

        Public Property MimeType() As String
            Get
                Return Me.m_mimeType
            End Get
            Set(ByVal value As String)
                Me.m_mimeType = value
            End Set
        End Property

        Public Property PrivacyNoticeAt() As String
            Get
                Return Me.m_privacyNoticeAt
            End Get
            Set(ByVal value As String)
                Me.m_privacyNoticeAt = value
            End Set
        End Property

        Public Property RequireAppliesTo() As Boolean
            Get
                Return Me.m_requireAppliesTo
            End Get
            Set(ByVal value As Boolean)
                Me.m_requireAppliesTo = value
            End Set
        End Property

        Public Property SupportedClaims() As List(Of ClaimInfo)
            Get
                Return Me.m_supportedClaims
            End Get
            Set(ByVal value As List(Of ClaimInfo))
                Me.m_supportedClaims = value
            End Set
        End Property

        Public Property TokenTypes() As String()
            Get
                Return Me.m_tokenTypes
            End Get
            Set(ByVal value As String())
                Me.m_tokenTypes = value
            End Set
        End Property


        ' Fields
        Private m_cardId As String
        Private m_cardName As String
        Private m_cardType As CardType
        Private m_cardVersion As String
        Private m_credentialIdentifier As String
        Private m_epr As EndpointAddress
        Private m_expiresOn As DateTime
        Private m_hint As String
        Private m_issuedOn As DateTime
        Private m_issuerId As String
        Private m_issuerName As String
        Private m_language As String
        Private m_logo As Byte()
        Private m_mexUri As String
        Private m_mimeType As String
        Private m_privacyNoticeAt As String
        Private m_requireAppliesTo As Boolean
        Private m_supportedClaims As List(Of ClaimInfo)
        Private m_tokenTypes As String()
    End Class
End Namespace


