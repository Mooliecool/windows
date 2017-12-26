
'***************************** Module Header ******************************\
'* Module Name:	LoginPage.aspx.vb
'* Project:		AzureBingMaps
'* Copyright (c) Microsoft Corporation.
'* 
'* The callback handler. Configure both ACS and Messenger Connect
'* to redirect to this page after the user signs in.
'* 
'* This source is subject to the Microsoft Public License.
'* See http://www.microsoft.com/opensource/licenses.mspx#Ms-PL.
'* All other rights reserved.
'* 
'* THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY KIND, 
'* EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED 
'* WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR PURPOSE.
'\**************************************************************************

Imports System.Linq
Imports System.Net
Imports System.ServiceModel.Syndication
Imports System.Threading
Imports System.Web
Imports System.Xml
Imports Microsoft.IdentityModel.Claims

Partial Public Class FederationCallbackHandler
    Inherits System.Web.UI.Page
    Protected Sub Page_Load(ByVal sender As Object, ByVal e As EventArgs)
        ' Obtain return page from session, and redirect to the page later.
        Dim returnPage As String = "HtmlClient.aspx"
        If Session("ReturnPage") IsNot Nothing Then
            returnPage = DirectCast(Session("ReturnPage"), String)
        End If

        ' Parse wl_internalState cookie,
        ' and extract information for Windows Live Messenger Connect Profile API.
        ' wl_internalState could be null if the user hasn't tried to login using Live ID.
        If Response.Cookies("wl_internalState") IsNot Nothing Then
            Dim accessToken As String = Me.ExtractWindowsLiveInternalState("wl_accessToken")
            Dim cid As String = Me.ExtractWindowsLiveInternalState("wl_cid")
            Dim uri As String = "http://apis.live.net/V4.1/cid-" & cid & "/Profiles"

            ' wl_internalState could be invalid if LiveID login failed.
            If Not String.IsNullOrEmpty(accessToken) AndAlso Not String.IsNullOrEmpty(cid) Then

                ' Make a request to profile API.
                Dim request As HttpWebRequest = DirectCast(HttpWebRequest.Create(uri), HttpWebRequest)
                request.Headers("Authorization") = accessToken
                Dim response As HttpWebResponse = DirectCast(request.GetResponse(), HttpWebResponse)
                If response.StatusCode = HttpStatusCode.OK Then

                    ' Use WCF Syndication API to parse the response.
                    Dim xmlReader As XmlReader = XmlReader.Create(response.GetResponseStream())
                    Dim feed As SyndicationFeed = SyndicationFeed.Load(xmlReader)
                    Dim entry = feed.Items.FirstOrDefault()
                    If entry IsNot Nothing Then
                        Dim content = TryCast(entry.Content, XmlSyndicationContent)
                        If content IsNot Nothing Then

                            ' WindowsLiveProfile is a class
                            ' corresponding to the profile API's response.
                            Dim profile = content.ReadContent(Of WindowsLiveProfile)()
                            Dim liveID = profile.Emails.Where(Function(m) String.Equals(m.Type, "WindowsLiveID")).FirstOrDefault()

                            ' If profile API succeeds,
                            ' we'll be able to obtain the user's LiveID.
                            ' The LiveID will be the user's identity.
                            ' We store user identity in session.
                            If liveID IsNot Nothing Then
                                Session("User") = liveID.Address
                            End If
                        End If
                    End If
                    xmlReader.Close()
                End If
            End If
        End If

        ' The following code deals with ACS via WIF.
        Dim principal = TryCast(Thread.CurrentPrincipal, IClaimsPrincipal)
        If principal IsNot Nothing AndAlso principal.Identities.Count > 0 Then
            Dim identity = principal.Identities(0)

            ' Query for email claim.
            Dim query = From c In identity.Claims Where c.ClaimType = ClaimTypes.Email Select c
            Dim emailClaim = query.FirstOrDefault()
            If emailClaim IsNot Nothing Then

                ' Store user identity in session.
                Session("User") = emailClaim.Value
            End If
        End If
        ' Redirect user to the return page.
        Response.Redirect(returnPage)
    End Sub

    ''' <summary>
    ''' Extract information for Windows Live Messenger Connect Profile API
    ''' from wl_internalState cookie.
    ''' The cookie contains a bunch of information
    ''' such as cid and access token.
    ''' </summary>
    ''' <param name="key">Which data to extract.</param>
    ''' <returns>The value of the data.</returns>
    Private Function ExtractWindowsLiveInternalState(ByVal key As String) As String
        Dim result As String = Request.Cookies("wl_internalState").Value
        Try
            result = HttpUtility.UrlDecode(result)
            result = result.Substring(result.IndexOf(key))
            result = result.Substring(key.Length + 3, result.IndexOf(","c) - key.Length - 4)

            ' wl_internalState could be invalid if LiveID login failed.
            ' In this case, we return null.
        Catch
            result = Nothing
        End Try
        Return result
    End Function
End Class