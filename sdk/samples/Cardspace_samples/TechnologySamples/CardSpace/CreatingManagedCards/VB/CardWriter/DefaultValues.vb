Imports System
Imports System.Collections.Generic
Imports System.IdentityModel.Claims
Imports System.Text

Namespace ManagedCardWriter
    Public Class DefaultValues
        ' Methods
        Shared Sub New()
            DefaultValues.m_claims = New Dictionary(Of String, ClaimInfo)
            DefaultValues.m_claims.Add(ClaimTypes.GivenName, New ClaimInfo(ClaimTypes.GivenName, "Given Name", "Given Name"))
            DefaultValues.m_claims.Add(ClaimTypes.Surname, New ClaimInfo(ClaimTypes.Surname, "Last Name", "Last Name"))
            DefaultValues.m_claims.Add(ClaimTypes.Email, New ClaimInfo(ClaimTypes.Email, "Email Address", "Email Address"))
            DefaultValues.m_claims.Add(ClaimTypes.StreetAddress, New ClaimInfo(ClaimTypes.StreetAddress, "Street Address", "Street Address"))
            DefaultValues.m_claims.Add(ClaimTypes.Locality, New ClaimInfo(ClaimTypes.Locality, "Locality", "Locality"))
            DefaultValues.m_claims.Add(ClaimTypes.StateOrProvince, New ClaimInfo(ClaimTypes.StateOrProvince, "State or Province", "State or Province"))
            DefaultValues.m_claims.Add(ClaimTypes.PostalCode, New ClaimInfo(ClaimTypes.PostalCode, "Postal Code", "Postal Code"))
            DefaultValues.m_claims.Add(ClaimTypes.Country, New ClaimInfo(ClaimTypes.Country, "Country", "Country"))
            DefaultValues.m_claims.Add(ClaimTypes.HomePhone, New ClaimInfo(ClaimTypes.HomePhone, "Home Phone", "Home Phone"))
            DefaultValues.m_claims.Add(ClaimTypes.OtherPhone, New ClaimInfo(ClaimTypes.OtherPhone, "Other Phone", "Other Phone"))
            DefaultValues.m_claims.Add(ClaimTypes.MobilePhone, New ClaimInfo(ClaimTypes.MobilePhone, "Mobile Phone", "Mobile Phone"))
            DefaultValues.m_claims.Add(ClaimTypes.Gender, New ClaimInfo(ClaimTypes.Gender, "Gender", "Gender"))
            DefaultValues.m_claims.Add(ClaimTypes.DateOfBirth, New ClaimInfo(ClaimTypes.DateOfBirth, "Date of Birth", "Date of Birth"))
            DefaultValues.m_claims.Add(ClaimTypes.PPID, New ClaimInfo(ClaimTypes.PPID, "Site Specific ID", "Site Specific ID"))
            DefaultValues.m_claims.Add(ClaimTypes.Webpage, New ClaimInfo(ClaimTypes.Webpage, "Webpage", "Webpage"))
        End Sub


        ' Properties
        Public Shared ReadOnly Property Claims() As Dictionary(Of String, ClaimInfo)
            Get
                Return DefaultValues.m_claims
            End Get
        End Property

        Public Shared ReadOnly Property ClaimsList() As String()
            Get
                Return New String() {ClaimTypes.GivenName, ClaimTypes.Surname, ClaimTypes.Email, ClaimTypes.StreetAddress, ClaimTypes.Locality, ClaimTypes.StateOrProvince, ClaimTypes.PostalCode, ClaimTypes.Country, ClaimTypes.HomePhone, ClaimTypes.OtherPhone, ClaimTypes.MobilePhone, ClaimTypes.Gender, ClaimTypes.DateOfBirth, ClaimTypes.PPID, ClaimTypes.Webpage}
            End Get
        End Property

        Public Shared ReadOnly Property TokenTypeList() As String()
            Get
                Return New String() {"urn:oasis:names:tc:SAML:1.0:assertion", "http://docs.oasis-open.org/wss/oasis-wss-saml-token-profile-1.1#SAMLV1.1"}
            End Get
        End Property


        ' Fields
        Public Const CardId As String = "http://www.fabrikam.com/card/1"
        Public Const CardName As String = "My new card"
        Public Const CardVersion As String = "1"
        Public Const Issuer As String = "http://localhost:4444/sts"
        Public Const IssuerName As String = "Microsoft"
        Public Const Language As String = "en-us"
        Private Shared m_claims As Dictionary(Of String, ClaimInfo)
        Public Const MexUri As String = "https://localhost:4445/sts/mex"

    End Class
    ' Nested Types
    Public Enum CardType
        ' Fields
        KerberosAuth = 1
        None = 4
        SelfIssuedAuth = 2
        SmartCard = 3
        UserNamePassword = 0
    End Enum

End Namespace


