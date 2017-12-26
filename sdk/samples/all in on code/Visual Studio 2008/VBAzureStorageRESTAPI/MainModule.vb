'****************************** Module Header ******************************'
' Module Name:  MainModule.vb
' Project:      VBAzureStorageRESTAPI
' Copyright (c) Microsoft Corporation.
' 
' This module shows how to call List Blob REST API to list the blobs of a
' specific container of Blob storage
' 
' This source is subject to the Microsoft Public License.
' See http://www.microsoft.com/opensource/licenses.mspx#Ms-PL.
' All other rights reserved.
' 
' THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY KIND, 
' EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED 
' WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR PURPOSE.
'***************************************************************************'

Imports System.Collections.Specialized
Imports System.Net
Imports System.Text
Imports System.Web
Imports System.IO

Module MainModule

    Const _bloburi As String = "http://127.0.0.1:10000/devstoreaccount1"
    Const _accountname As String = "devstoreaccount1"
    Const _key As String = "Eby8vdM02xNOcqFlqUwJPLlmEtlCDXJ1OUzFT50uSRZ6IFsuFq2UVErCz4I6tq/K1SZFPTOtr/KBHBeksoGMGw=="
    Const _method As String = "GET"

    Sub Main()
        Dim AccountName As String = _accountname
        Dim AccountSharedKey As String = _key
        Dim Address As String = _bloburi
        Dim MessageSignature As String = ""
        Console.WriteLine("Please input the name of the container press <ENTER>. Its blobs info will be listed:")
        ' Get the container name
        Dim container As String = Console.ReadLine()
        ' Set request URI
        Dim QueryString As String = "?restype=container&comp=list"
        Dim requesturi As Uri = New Uri(Address & "/" & container & QueryString)

        ' Create HttpWebRequest object
        Dim Request As HttpWebRequest = CType(HttpWebRequest.Create(requesturi.AbsoluteUri), HttpWebRequest)
        Request.Method = _method
        Request.ContentLength = 0
        ' Add HTTP headers
        Request.Headers.Add("x-ms-date", DateTime.UtcNow.ToString("R"))
        Request.Headers.Add("x-ms-version", "2009-09-19")

        ' Create Signature
        ' Verb
        MessageSignature &= "GET" & Constants.vbLf
        ' Content-Encoding
        MessageSignature &= Constants.vbLf
        ' Content-Language
        MessageSignature &= Constants.vbLf
        ' Content-Length
        MessageSignature &= Constants.vbLf
        ' Content-MD5
        MessageSignature &= Constants.vbLf
        ' Content-Type
        MessageSignature &= Constants.vbLf
        ' Date
        MessageSignature &= Constants.vbLf
        ' If-Modified-Since
        MessageSignature &= Constants.vbLf
        ' If-Match
        MessageSignature &= Constants.vbLf
        ' If-None-Match 
        MessageSignature &= Constants.vbLf
        ' If-Unmodified-Since
        MessageSignature &= Constants.vbLf
        ' Range
        MessageSignature &= Constants.vbLf
        ' CanonicalizedHeaders
        MessageSignature &= GetCanonicalizedHeaders(Request)
        ' CanonicalizedResource
        MessageSignature &= GetCanonicalizedResourceVersion2(requesturi, AccountName)
        ' Use HMAC-SHA256 to sign the signature
        Dim SignatureBytes As Byte() = System.Text.Encoding.UTF8.GetBytes(MessageSignature)
        Dim SHA256 As System.Security.Cryptography.HMACSHA256 = New System.Security.Cryptography.HMACSHA256(Convert.FromBase64String(AccountSharedKey))
        ' Create Authorization HTTP header value
        Dim AuthorizationHeader As String = "SharedKey " & AccountName & ":" & Convert.ToBase64String(SHA256.ComputeHash(SignatureBytes))
        ' Add Authorization HTTP header
        Request.Headers.Add("Authorization", AuthorizationHeader)

        Try
            ' Send Http request and get response
            Using response As HttpWebResponse = DirectCast(Request.GetResponse, HttpWebResponse)
                If (response.StatusCode = HttpStatusCode.OK) Then
                    ' If success
                    Using stream = response.GetResponseStream
                        Using sr = New StreamReader(stream)
                            ' Output response
                            Console.WriteLine(sr.ReadToEnd)
                        End Using
                    End Using
                End If
            End Using
        Catch ex As WebException
            Console.WriteLine(("An error occured. Status code:" & DirectCast(ex.Response, HttpWebResponse).StatusCode))
            Console.WriteLine("Error information:")
            Using stream = ex.Response.GetResponseStream
                Using sr = New StreamReader(stream)
                    Console.WriteLine(sr.ReadToEnd)
                End Using
            End Using
        End Try

        Console.ReadLine()
    End Sub

    Function GetHeaderValues(ByVal headers As NameValueCollection, ByVal headerName As String) As ArrayList
        Dim list As ArrayList = New ArrayList()
        Dim values As String() = headers.GetValues(headerName)
        If Not values Is Nothing Then
            For Each str As String In values
                list.Add(str.TrimStart(New Char() {}))
            Next str
        End If
        Return list
    End Function

    Function GetCanonicalizedHeaders(ByVal request As HttpWebRequest) As String
        Dim list As ArrayList = New ArrayList()
        Dim sb As StringBuilder = New StringBuilder()
        For Each str As String In request.Headers.Keys
            If str.ToLowerInvariant().StartsWith("x-ms-", StringComparison.Ordinal) Then
                list.Add(str.ToLowerInvariant())
            End If
        Next str
        list.Sort()
        For Each str2 As String In list
            Dim builder As StringBuilder = New StringBuilder(str2)
            Dim str3 As String = ":"
            For Each str4 As String In GetHeaderValues(request.Headers, str2)
                Dim str5 As String = str4.Replace(Constants.vbCrLf, String.Empty)
                builder.Append(str3)
                builder.Append(str5)
                str3 = ","
            Next str4
            sb.Append(builder.ToString())
            sb.Append(Constants.vbLf)
        Next str2
        Return sb.ToString()
    End Function

    Function GetCanonicalizedResourceVersion2(ByVal address As Uri, ByVal accountName As String) As String
        Dim builder As StringBuilder = New StringBuilder("/")
        builder.Append(accountName)
        builder.Append(address.AbsolutePath)
        Dim str As CanonicalizedString = New CanonicalizedString(builder.ToString())
        Dim values As NameValueCollection = HttpUtility.ParseQueryString(address.Query)
        Dim values2 As NameValueCollection = New NameValueCollection()
        For Each str2 As String In values.Keys
            Dim list As ArrayList = New ArrayList(values.GetValues(str2))
            list.Sort()
            Dim builder2 As StringBuilder = New StringBuilder()
            For Each obj2 As Object In list
                If builder2.Length > 0 Then
                    builder2.Append(",")
                End If
                builder2.Append(obj2.ToString())
            Next obj2
            If (str2 Is Nothing) Then
                values2.Add(str2, builder2.ToString())
            Else
                values2.Add(str2.ToLowerInvariant(), builder2.ToString())
            End If
        Next str2
        Dim list2 As ArrayList = New ArrayList(values2.AllKeys)
        list2.Sort()
        For Each str3 As String In list2
            Dim builder3 As StringBuilder = New StringBuilder(String.Empty)
            builder3.Append(str3)
            builder3.Append(":")
            builder3.Append(values2(str3))
            str.AppendCanonicalizedElement(builder3.ToString())
        Next str3
        Return str.Value
    End Function

    Friend Class CanonicalizedString
        ' Fields
        Private canonicalizedString_Renamed As StringBuilder = New StringBuilder()

        ' Methods
        Friend Sub New(ByVal initialElement As String)
            Me.canonicalizedString_Renamed.Append(initialElement)
        End Sub

        Friend Sub AppendCanonicalizedElement(ByVal element As String)
            Me.canonicalizedString_Renamed.Append(Constants.vbLf)
            Me.canonicalizedString_Renamed.Append(element)
        End Sub

        ' Properties
        Friend ReadOnly Property Value() As String
            Get
                Return Me.canonicalizedString_Renamed.ToString()
            End Get
        End Property
    End Class

End Module