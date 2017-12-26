'****************************** Module Header ******************************'
' Module Name:              SampleContext.vb
' Project:                  VBSL3Navigation
' Copyright (c) Microsoft Corporation.
' 
' SampleContext class file, SampleContext is used for storing sample and author
' collections and relation reference.
' 
' This source is subject to the Microsoft Public License.
' See http://www.microsoft.com/opensource/licenses.mspx#Ms-PL.
' All other rights reserved.
' 
' THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY KIND, 
' EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED 
' WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR PURPOSE.
'***************************************************************************'

Imports System.Xml.Linq

' Sample entity definition
Public Class Sample
    Public Sub New(ByVal name As String, ByVal lang As String, ByVal au As AuthorInfo, ByVal link As String, ByVal des As String, ByVal imglink As String)
        Me.Name = name
        Me.Language = lang
        Me.Author = au
        Me.Link = link
        Me.Description = des
        Me.ScrImageUri = imglink
    End Sub

    Private _Name As String
    Public Property Name() As String
        Get
            Return _Name
        End Get
        Set(ByVal value As String)
            _Name = value
        End Set
    End Property
    Private _Language As String
    Public Property Language() As String
        Get
            Return _Language
        End Get
        Set(ByVal value As String)
            _Language = value
        End Set
    End Property
    Private _Author As AuthorInfo
    Public Property Author() As AuthorInfo
        Get
            Return _Author
        End Get
        Set(ByVal value As AuthorInfo)
            _Author = value
        End Set
    End Property
    Private _Description As String
    Public Property Description() As String
        Get
            Return _Description
        End Get
        Set(ByVal value As String)
            _Description = value
        End Set
    End Property
    Private _Link As String
    Public Property Link() As String
        Get
            Return _Link
        End Get
        Set(ByVal value As String)
            _Link = value
        End Set
    End Property
    Private _ScrImageUri As String
    Public Property ScrImageUri() As String
        Get
            Return _ScrImageUri
        End Get
        Set(ByVal value As String)
            _ScrImageUri = value
        End Set
    End Property
End Class

' Author entity definition
Public Class AuthorInfo
    Public Sub New(ByVal name As String, ByVal des As String, ByVal gender As String, ByVal phouri As String)
        Me.Name = name
        Me.Description = des
        Me.Gender = gender
        Me.PhotoUri = phouri
    End Sub
    Private _Name As String
    Public Property Name() As String
        Get
            Return _Name
        End Get
        Set(ByVal value As String)
            _Name = value
        End Set
    End Property
    Private _Description As String
    Public Property Description() As String
        Get
            Return _Description
        End Get
        Set(ByVal value As String)
            _Description = value
        End Set
    End Property
    Private _Gender As String
    Public Property Gender() As String
        Get
            Return _Gender
        End Get
        Set(ByVal value As String)
            _Gender = value
        End Set
    End Property
    Private _PhotoUri As String
    Public Property PhotoUri() As String
        Get
            Return _PhotoUri
        End Get
        Set(ByVal value As String)
            _PhotoUri = value
        End Set
    End Property
End Class

' Sample context stores sample and author collections
' and relation reference.
Public Class SampleContext
    Private _Samples As List(Of Sample)
    Public Property Samples() As List(Of Sample)
        Get
            Return _Samples
        End Get
        Protected Set(ByVal value As List(Of Sample))
            _Samples = value
        End Set
    End Property
    Private _AuthorInfos As List(Of AuthorInfo)
    Public Property AuthorInfos() As List(Of AuthorInfo)
        Get
            Return _AuthorInfos
        End Get
        Protected Set(ByVal value As List(Of AuthorInfo))
            _AuthorInfos = value
        End Set
    End Property
    Private _xdoc As XDocument

    Public Sub New(ByVal xmlname As String)
        ' Download xml
        Dim wc As New WebClient()
        Dim xmluri = New Uri(Application.Current.Host.Source, "/" + xmlname)
        AddHandler wc.OpenReadCompleted, AddressOf wc_OpenReadCompleted
        wc.OpenReadAsync(xmluri, xmluri)
    End Sub

    Private Sub wc_OpenReadCompleted(ByVal sender As Object, ByVal e As OpenReadCompletedEventArgs)
        Dim xmluri As Uri = TryCast(e.UserState, Uri)
        If e.[Error] Is Nothing Then
            Try
                _xdoc = XDocument.Load(e.Result)
                InitializeContext()
            Catch
                Dim window = New ErrorWindow("Error occured while parsing xml file" & vbLf & xmluri.ToString())
                window.Show()
            End Try
        Else
            Dim window = New ErrorWindow("Error occured while downloading xml file" & vbLf & xmluri.ToString())
            window.Show()
        End If
    End Sub

    ' Load the entity collection
    Private Sub InitializeContext()
        If _xdoc IsNot Nothing Then
            AuthorInfos = (From a In _xdoc.Descendants("AuthorInfo") _
                Select New AuthorInfo(a.Attribute("Name").Value, GetStringValue(a.Element("Description")), GetStringValue(a.Attribute("Gender")), GetStringValue(a.Attribute("Photo"))) _
                ).ToList()

            Samples = (From s In _xdoc.Descendants("Sample") _
                Select New Sample(GetStringValue(s.Attribute("Name")), GetStringValue(s.Attribute("Language")), _
                                  AuthorInfos.FirstOrDefault(Function(a1 As AuthorInfo) a1.Name.Equals(GetStringValue(s.Attribute("Author")))), _
                                  GetStringValue(s.Attribute("Link")), GetStringValue(s.Attribute("Description")), GetStringValue(s.Attribute("ScrImage")))).ToList()
        Else
            Samples = New List(Of Sample)()
            AuthorInfos = New List(Of AuthorInfo)()
        End If
    End Sub

    Private Function GetStringValue(ByVal xa As XAttribute) As String
        If xa IsNot Nothing Then
            Return xa.Value
        Else
            Return Nothing
        End If
    End Function
    Private Function GetStringValue(ByVal xe As XElement) As String
        If xe IsNot Nothing Then
            Return xe.Value
        Else
            Return Nothing
        End If
    End Function
End Class