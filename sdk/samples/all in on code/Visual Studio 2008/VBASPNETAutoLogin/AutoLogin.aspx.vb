'/**************************** Module Header ********************************\* 
' Module Name:  AutoLogin.aspx.vb
' Project:      VBASPNETAutoLogin
' Copyright (c) Microsoft Corporation.
' 
' This page request the Login.aspx firstly, and get the __VIEWSTATE and __EVENTVALIDATION'fields.
' Then we can set the post data string, such as the __VIEWSTATE, __EVENTVALIDATION,
' UserName,Password and loginButton id parameters.
' We use the webrequest to post these data into the login.aspx to login this site.
' 
' This source is subject to the Microsoft Public License.
' See http://www.microsoft.com/opensource/licenses.mspx#Ms-PL.
' All other rights reserved.
' 
' THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY KIND, 
' EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED 
' WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR PURPOSE.
'\***************************************************************************/

Imports System
Imports System.Net

Partial Public Class AutoLogin
    Inherits System.Web.UI.Page

    Protected Sub Page_Load(ByVal sender As Object, ByVal e As System.EventArgs) Handles Me.Load
        Response.Clear()
    End Sub

    Protected Sub Login1_Click(ByVal sender As Object, ByVal e As EventArgs) Handles Login1.Click

        Dim url As String = HttpContext.Current.Request.Url.AbsoluteUri.ToString().Replace("AutoLogin", "Login")
        Dim myCookieContainer As CookieContainer = New CookieContainer

        Dim request As System.Net.HttpWebRequest
        request = CType(System.Net.WebRequest.Create(url), System.Net.HttpWebRequest)

        request.CookieContainer = myCookieContainer
        request.Method = "GET"
        request.KeepAlive = False

        Dim response As System.Net.HttpWebResponse = CType(request.GetResponse(), System.Net.HttpWebResponse)

        Dim responseStream As System.IO.Stream = response.GetResponseStream()
        Dim reader As New System.IO.StreamReader(responseStream, Encoding.UTF8)
        Dim srcString As String = reader.ReadToEnd()

        ' get the page ViewState                
        Dim viewStateFlag As String = "id=""__VIEWSTATE"" value="""
        Dim i As Integer = srcString.IndexOf(viewStateFlag) + viewStateFlag.Length
        Dim j As Integer = srcString.IndexOf("""", i)
        Dim viewState As String = srcString.Substring(i, j - i)

        ' get page EventValidation                
        Dim eventValidationFlag As String = "id=""__EVENTVALIDATION"" value="""
        i = srcString.IndexOf(eventValidationFlag) + eventValidationFlag.Length
        j = srcString.IndexOf("""", i)
        Dim eventValidation As String = srcString.Substring(i, j - i)

        Dim submitButton As String = "LoginButton"

        ' UserName and Password
        Dim UserName As String = btnUserName.Text
        Dim Password As String = btnPassword.Text
        ' Convert the text into the url encoding string
        viewState = System.Web.HttpUtility.UrlEncode(viewState)
        eventValidation = System.Web.HttpUtility.UrlEncode(eventValidation)
        submitButton = System.Web.HttpUtility.UrlEncode(submitButton)

        ' Concat the string data which will be submit
        Dim formatString As String = "UserName={0}&Password={1}&loginButton={2}&__VIEWSTATE={3}&__EVENTVALIDATION={4}"
        Dim postString As String = String.Format(formatString, UserName, Password, submitButton, viewState, eventValidation)

        ' Convert the submit string data into the byte array
        Dim postData As Byte() = Encoding.ASCII.GetBytes(postString)

        ' Set the request parameters
        request = System.Net.WebRequest.Create(url)
        request.Method = "POST"
        request.Referer = url
        request.KeepAlive = False
        request.UserAgent = "Mozilla/4.0 (compatible; MSIE 8.0; Windows NT 6.1; WOW64; Trident/4.0; SLCC2; .NET CLR 2.0.50727; .NET CLR 3.5.30729; .NET CLR 3.0.30729; Media Center PC 6.0; InfoPath.2; CIBA)"
        request.ContentType = "application/x-www-form-urlencoded"
        request.CookieContainer = myCookieContainer
        Dim ck As New System.Net.Cookie("TestCookie1", "Value of test cookie")
        ck.Domain = request.RequestUri.Host
        request.CookieContainer.Add(ck)
        request.CookieContainer.Add(response.Cookies)
        request.ContentLength = postData.Length

        ' Submite the requeset data
        Dim outputStream As System.IO.Stream = request.GetRequestStream()
        request.AllowAutoRedirect = True
        outputStream.Write(postData, 0, postData.Length)
        outputStream.Close()

        ' Get the return data
        response = request.GetResponse()
        responseStream = response.GetResponseStream()
        reader = New System.IO.StreamReader(responseStream, Encoding.UTF8)
        srcString = reader.ReadToEnd()
        System.Web.HttpContext.Current.Response.Write(srcString)
        System.Web.HttpContext.Current.Response.End()

    End Sub
End Class