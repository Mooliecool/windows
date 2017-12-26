
Imports System
Imports System.Collections.Generic
Imports System.Collections.ObjectModel
Imports System.Text
Imports System.Xml
Imports System.IO
Imports System.ServiceModel
Imports System.ServiceModel.Security
Imports System.IdentityModel
Imports System.IdentityModel.Tokens
Imports System.Security.Cryptography
Imports System.Security.Cryptography.X509Certificates
Imports System.IdentityModel.Selectors
Imports System.IdentityModel.Policy
Imports System.IdentityModel.Claims
Imports System.Runtime.Remoting
Imports System.Security.Principal
Imports System.Configuration
Imports System.ServiceModel.Security.Tokens
Imports System.Web
 
Public Class Token
    ' Methods
    Shared Sub New()
        Token.TripleDes = New Integer() {"http://www.w3.org/2001/04/xmlenc#tripledes-cbc".GetHashCode, "http://www.w3.org/2001/04/xmlenc#kw-tripledes".GetHashCode}
        Token.Aes = New Integer() {"http://www.w3.org/2001/04/xmlenc#aes128-cbc".GetHashCode, "http://www.w3.org/2001/04/xmlenc#aes192-cbc".GetHashCode, "http://www.w3.org/2001/04/xmlenc#aes256-cbc".GetHashCode, "http://www.w3.org/2001/04/xmlenc#kw-aes128".GetHashCode, "http://www.w3.org/2001/04/xmlenc#kw-aes192".GetHashCode, "http://www.w3.org/2001/04/xmlenc#kw-aes256".GetHashCode}
        Token.m_certificates = New Hashtable
        Token._maxTokenSkew = Nothing
    End Sub


    Public Sub New(ByVal xmlToken As String)
        Me.m_token = Nothing
        Me.m_authorizationContext = Nothing
        Me.m_endpointIdentity = Nothing
        Me.m_issuer = Nothing
        Me.m_identityClaims = Nothing
        Me._claimIndexer = Nothing
        Dim buffer1 As Byte() = Token.decryptToken(xmlToken)
        Dim reader1 As XmlReader = New XmlTextReader(New StreamReader(New MemoryStream(buffer1), Encoding.UTF8))
        Me.m_token = DirectCast(WSSecurityTokenSerializer.DefaultInstance.ReadToken(reader1, Nothing), SamlSecurityToken)
        Dim authenticator1 As New SamlSecurityTokenAuthenticator(New List(Of SecurityTokenAuthenticator)(New SecurityTokenAuthenticator() {New RsaSecurityTokenAuthenticator, New X509SecurityTokenAuthenticator}), Token.MaximumTokenSkew)
        If Not authenticator1.CanValidateToken(Me.m_token) Then
            Throw New Exception("Unable to validate the token.")
        End If
        Dim collection1 As ReadOnlyCollection(Of IAuthorizationPolicy) = authenticator1.ValidateToken(Me.m_token)
        Me.m_authorizationContext = System.IdentityModel.Policy.AuthorizationContext.CreateDefaultAuthorizationContext(collection1)
        Me.FindIdentityClaims()
    End Sub


    Private Function CreateIdentityFromClaimSet(ByVal claims As ClaimSet) As EndpointIdentity
        Dim claim1 As Claim
        For Each claim1 In claims.FindClaims(Nothing, Rights.Identity)
            Return EndpointIdentity.CreateIdentity(claim1)
        Next
        Return Nothing
    End Function


    Private Shared Function decryptToken(ByVal xmlToken As String) As Byte()
        Dim num3 As Integer
        Dim reader1 As XmlReader = New XmlTextReader(New StringReader(xmlToken))
        If Not reader1.ReadToDescendant("EncryptionMethod", "http://www.w3.org/2001/04/xmlenc#") Then
            Throw New ArgumentException("Cannot find token EncryptedMethod.")
        End If
        Dim num1 As Integer = reader1.GetAttribute("Algorithm").GetHashCode
        If Not reader1.ReadToFollowing("EncryptionMethod", "http://www.w3.org/2001/04/xmlenc#") Then
            Throw New ArgumentException("Cannot find key EncryptedMethod.")
        End If
        Dim num2 As Integer = reader1.GetAttribute("Algorithm").GetHashCode
        If Not reader1.ReadToFollowing("DigestMethod", "http://www.w3.org/2000/09/xmldsig#") Then
            Throw New ArgumentException("Cannot find Digest Method.")
        End If
        Dim text1 As String = reader1.GetAttribute("Algorithm")
        If Not reader1.ReadToFollowing("KeyIdentifier", "http://docs.oasis-open.org/wss/2004/01/oasis-200401-wss-wssecurity-secext-1.0.xsd") Then
            Throw New ArgumentException("Cannot find Key Identifier.")
        End If
        reader1.Read()
        Dim buffer1 As Byte() = Convert.FromBase64String(reader1.ReadContentAsString)
        If Not reader1.ReadToFollowing("CipherValue", "http://www.w3.org/2001/04/xmlenc#") Then
            Throw New ArgumentException("Cannot find symmetric key.")
        End If
        reader1.Read()
        Dim buffer3 As Byte() = Convert.FromBase64String(reader1.ReadContentAsString)
        If Not reader1.ReadToFollowing("CipherValue", "http://www.w3.org/2001/04/xmlenc#") Then
            Throw New ArgumentException("Cannot find encrypted security token.")
        End If
        reader1.Read()
        Dim buffer2 As Byte() = Convert.FromBase64String(reader1.ReadContentAsString)
        reader1.Close()
        Dim algorithm1 As SymmetricAlgorithm = Nothing
        Dim certificate1 As X509Certificate2 = Token.FindCertificate(buffer1)
        Dim numArray1 As Integer() = Token.Aes
        Dim num5 As Integer = 0
        Do While (num5 < numArray1.Length)
            num3 = numArray1(num5)
            If (num1 = num3) Then
                algorithm1 = New RijndaelManaged
                Exit Do
            End If
            num5 += 1
        Loop
        If (Nothing Is algorithm1) Then
            numArray1 = Token.TripleDes
            num5 = 0
            Do While (num5 < numArray1.Length)
                num3 = numArray1(num5)
                If (num1 = num3) Then
                    algorithm1 = New TripleDESCryptoServiceProvider
                    Exit Do
                End If
                num5 += 1
            Loop
        End If
        If (Nothing Is algorithm1) Then
            Throw New ArgumentException("Could not determine Symmetric Algorithm")
        End If
        algorithm1.Key = TryCast(certificate1.PrivateKey, RSACryptoServiceProvider).Decrypt(buffer3, True)
        Dim num4 As Integer = (algorithm1.BlockSize / 8)
        Dim buffer4 As Byte() = New Byte(num4 - 1) {}
        Buffer.BlockCopy(buffer2, 0, buffer4, 0, buffer4.Length)
        algorithm1.Padding = PaddingMode.ISO10126
        algorithm1.Mode = CipherMode.CBC
        Dim transform1 As ICryptoTransform = algorithm1.CreateDecryptor(algorithm1.Key, buffer4)
        Dim buffer5 As Byte() = transform1.TransformFinalBlock(buffer2, buffer4.Length, (buffer2.Length - buffer4.Length))
        transform1.Dispose()
        Return buffer5
    End Function


    Private Shared Function FindCertificate(ByVal thumbprint As Object) As X509Certificate2
        Dim certificate1 As X509Certificate2 = TryCast(Token.m_certificates.Item(thumbprint), X509Certificate2)
        If (Nothing Is certificate1) Then
            Dim name1 As StoreName = StoreName.My
            Dim location1 As StoreLocation = StoreLocation.LocalMachine
            Dim text1 As String = ConfigurationManager.AppSettings.Item("CertificateThumbprint")
            Dim text2 As String = ConfigurationManager.AppSettings.Item("StoreName")
            Dim text3 As String = ConfigurationManager.AppSettings.Item("StoreLocation")
            If Not String.IsNullOrEmpty(text2) Then
                name1 = DirectCast(System.Enum.Parse(GetType(StoreName), text2, True), StoreName)
            End If
            If Not String.IsNullOrEmpty(text3) Then
                location1 = DirectCast(System.Enum.Parse(GetType(StoreLocation), text3, True), StoreLocation)
            End If
            Dim store1 As New X509Store(name1, location1)
            store1.Open(OpenFlags.ReadOnly)
            Dim text4 As String = ""
            If (Not Nothing Is TryCast(thumbprint, Byte())) Then
                Dim num1 As Byte
                For Each num1 In TryCast(thumbprint, Byte())
                    text4 = (text4 & num1.ToString("X2"))
                Next
            Else
                text4 = thumbprint.ToString
            End If
            Dim collection1 As X509Certificate2Collection = store1.Certificates.Find(X509FindType.FindByThumbprint, text4, True)
            If (collection1.Count = 0) Then
                If String.IsNullOrEmpty(text1) Then
                    Throw New Exception("Relying Party Certificate thumbprint not specified")
                End If
                collection1 = store1.Certificates.Find(X509FindType.FindByThumbprint, text1, True)
                If (collection1.Count = 0) Then
                    Throw New Exception("Unable to find certificate by thumbprint.")
                End If
            End If
            certificate1 = collection1.Item(0)
            Token.m_certificates.Add(thumbprint, certificate1)
        End If
        Return certificate1
    End Function


    Protected Sub FindIdentityClaims()
        Dim text1 As String = ConfigurationManager.AppSettings.Item("IdentityClaimType")
        If String.IsNullOrEmpty(text1) Then
            text1 = ClaimTypes.PPID
        End If
        Dim set1 As ClaimSet = Nothing
        Me.m_endpointIdentity = Nothing
        Dim set2 As ClaimSet
        For Each set2 In Me.m_authorizationContext.ClaimSets
            Dim claim1 As Claim
            For Each claim1 In set2.FindClaims(text1, Rights.PossessProperty)
                Me.m_issuer = Me.CreateIdentityFromClaimSet(set2.Issuer)
                Dim buffer1 As Byte() = TryCast(Me.m_issuer.IdentityClaim.Resource, Byte())
                If (Nothing Is buffer1) Then
                    Dim provider1 As RSACryptoServiceProvider = TryCast(Me.m_issuer.IdentityClaim.Resource, RSACryptoServiceProvider)
                    If (Not Nothing Is provider1) Then
                        buffer1 = provider1.ExportCspBlob(False)
                    End If
                    If (Nothing Is buffer1) Then
                        Throw New Exception("Unsupported IdentityClaim resource type")
                    End If
                End If
                Dim buffer2 As Byte() = Encoding.UTF8.GetBytes(Token.GetResourceValue(claim1))
                Dim buffer3 As Byte() = New Byte((buffer2.Length + buffer1.Length) - 1) {}
                buffer1.CopyTo(buffer3, 0)
                buffer2.CopyTo(buffer3, buffer1.Length)
                Using sha1 As SHA256 = New SHA256Managed
                    Dim claim2 As New Claim(ClaimTypes.Thumbprint, Convert.ToBase64String(sha1.ComputeHash(buffer3)), Rights.Identity)
                    Me.m_endpointIdentity = EndpointIdentity.CreateIdentity(claim2)
                End Using
                Me.m_identityClaims = set2
                Return
            Next
            If (Nothing Is Me.m_endpointIdentity) Then
                set1 = set2
                Me.m_endpointIdentity = Me.CreateIdentityFromClaimSet(set2)
            End If
        Next
        If (Nothing Is Me.m_endpointIdentity) Then
            Throw New Exception("The XML Token data provided no Identity claim.")
        End If
        Me.m_identityClaims = set1
    End Sub



    Protected Shared Function GetResourceValue(ByVal claim As Claim) As String
        Dim reference1 As IdentityReference = TryCast(claim.Resource, IdentityReference)
        If (Not Nothing Is reference1) Then
            Return reference1.Value
        End If
        Dim algorithm1 As ICspAsymmetricAlgorithm = TryCast(claim.Resource, ICspAsymmetricAlgorithm)
        If (Not Nothing Is algorithm1) Then
            Using sha1 As SHA256 = New SHA256Managed
                Return Convert.ToBase64String(sha1.ComputeHash(algorithm1.ExportCspBlob(False)))
            End Using
        End If
        Dim address1 As System.Net.Mail.MailAddress = TryCast(claim.Resource, System.Net.Mail.MailAddress)
        If (Not Nothing Is address1) Then
            Return address1.ToString
        End If
        Dim buffer1 As Byte() = TryCast(claim.Resource, Byte())
        If (Not Nothing Is buffer1) Then
            Return Convert.ToBase64String(buffer1)
        End If
        Return claim.Resource.ToString
    End Function



    ' Properties
    Public ReadOnly Property AuthorizationContext() As AuthorizationContext
        Get
            Return Me.m_authorizationContext
        End Get
    End Property


    Public ReadOnly Property Claims() As ClaimTextCollection
        Get
            If (Nothing Is Me._claimIndexer) Then
                Me._claimIndexer = New ClaimTextCollection(Me)
            End If
            Return Me._claimIndexer
        End Get
    End Property


    Public ReadOnly Property IdentityClaims() As ClaimSet
        Get
            Return Me.m_identityClaims
        End Get
    End Property

    Protected Shared ReadOnly Property MaximumTokenSkew() As TimeSpan
        Get
            If (Nothing Is Token._maxTokenSkew) Then
                Dim num1 As Integer = 300000
                Dim text1 As String = ConfigurationManager.AppSettings.Item("MaximumClockSkew")
                If Not String.IsNullOrEmpty(text1) Then
                    num1 = Integer.Parse(text1)
                End If
                Token._maxTokenSkew = New TimeSpan(0, 0, num1)
            End If
            Return DirectCast(Token._maxTokenSkew, TimeSpan)
        End Get
    End Property




    Public ReadOnly Property IssuerIdentityClaim() As Claim
        Get
            Return Me.m_issuer.IdentityClaim
        End Get
    End Property


    Public ReadOnly Property UniqueID() As String
        Get
            Return Token.GetResourceValue(Me.m_endpointIdentity.IdentityClaim)
        End Get
    End Property



    ' Fields
    Private _claimIndexer As ClaimTextCollection
    Private Shared _maxTokenSkew As Object
    Private Shared Aes As Integer()
    Private m_authorizationContext As AuthorizationContext
    Private Shared m_certificates As Hashtable
    Private m_endpointIdentity As EndpointIdentity
    Private m_identityClaims As ClaimSet
    Private m_issuer As EndpointIdentity
    Private m_token As SamlSecurityToken
    Private Shared TripleDes As Integer()

    ' Nested Types
    <System.Reflection.DefaultMember("Item")> _
    Public Class ClaimTextCollection
        ' Methods
        Public Sub New(ByVal instance As Token)
            Me._instance = instance
        End Sub



        ' Properties
        Public ReadOnly Property Item(ByVal uri As String) As String
            Get
                Dim claim1 As Claim
                For Each claim1 In Me._instance.m_identityClaims.FindClaims(uri, Rights.PossessProperty)
                    Return Token.GetResourceValue(claim1)
                Next
                Throw New ArgumentException(String.Format("Claim {0} not found", uri))
            End Get
        End Property



        ' Fields
        Private _instance As Token
    End Class

    Private NotInheritable Class WSSecurityStrings
        ' Fields
        Public Const KeyIdentifier As String = "KeyIdentifier"
        Public Const [Namespace] As String = "http://docs.oasis-open.org/wss/2004/01/oasis-200401-wss-wssecurity-secext-1.0.xsd"
    End Class

    Private NotInheritable Class XmlEncryptionStrings
        ' Fields
        Public Const Algorithm As String = "Algorithm"
        Public Const CipherValue As String = "CipherValue"
        Public Const Encoding As String = "Encoding"
        Public Const EncryptionMethod As String = "EncryptionMethod"
        Public Const MimeType As String = "MimeType"
        Public Const [Namespace] As String = "http://www.w3.org/2001/04/xmlenc#"
    End Class

    Private NotInheritable Class XmlSignatureStrings
        ' Fields
        Public Const DigestMethod As String = "DigestMethod"
        Public Const KeyInfo As String = "KeyInfo"
        Public Const [Namespace] As String = "http://www.w3.org/2000/09/xmldsig#"
    End Class
End Class


