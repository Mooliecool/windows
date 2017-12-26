'*************************** Module Header ******************************'
' Module Name:  StoredSite.vb
' Project:	    VBWebBrowserAutomation
' Copyright (c) Microsoft Corporation.
' 
' This class represents a site whose html elements are stored. A site is stored 
' as an XML file under StoredSites folder, and can be deserialized.
' 
' This source is subject to the Microsoft Public License.
' See http://www.microsoft.com/opensource/licenses.mspx#Ms-PL.
' All other rights reserved.
' 
' THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY KIND, 
' EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED 
' WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR PURPOSE.
'*************************************************************************'

Imports System.IO
Imports System.Security.Permissions

Public Class StoredSite

    Dim _host As String
    ''' <summary>
    ''' The host of the site.
    ''' </summary>
    Public Property Host() As String
        Get
            Return _host
        End Get
        Set(ByVal value As String)
            _host = value
        End Set
    End Property

    Dim _urls As List(Of String)
    ''' <summary>
    ''' The urls that can be completed automatically in the site .
    ''' </summary>
    Public Property Urls() As List(Of String)
        Get
            Return _urls
        End Get
        Set(ByVal value As List(Of String))
            _urls = value
        End Set
    End Property

    Dim _inputElements As List(Of HtmlInputElement)
    ''' <summary>
    ''' The html input elements that can be completed automatically.
    ''' </summary>
    Public Property InputElements() As List(Of HtmlInputElement)
        Get
            Return _inputElements
        End Get
        Set(ByVal value As List(Of HtmlInputElement))
            _inputElements = value
        End Set
    End Property

    Public Sub New()
        InputElements = New List(Of HtmlInputElement)()
    End Sub

    ''' <summary>
    ''' Save the instance as an XML file.
    ''' </summary>
    Public Sub Save()
        Dim folderPath As String = String.Format("{0}\StoredSites\", _
                                                 Environment.CurrentDirectory)

        If Not Directory.Exists(folderPath) Then
            Directory.CreateDirectory(folderPath)
        End If

        Dim filepath As String = String.Format("{0}\{1}.xml", folderPath, Me.Host)

        XMLSerialization(Of StoredSite).SerializeFromObjectToXML(Me, filepath)
    End Sub

    ''' <summary>
    ''' Complete the web page automatically.
    ''' </summary>
    <PermissionSetAttribute(SecurityAction.LinkDemand, Name:="FullTrust")> _
    Public Sub FillWebPage(ByVal document As HtmlDocument, ByVal submit As Boolean)
        ' The submit button in the page.
        Dim inputSubmit As HtmlElement = Nothing

        ' Set the value of html elements that are stored. If the element is
        ' a submit button, then assign to inputSubmit.
        For Each input As HtmlInputElement In Me.InputElements
            Dim element As HtmlElement = document.GetElementById(input.ID)
            If element Is Nothing Then
                Continue For
            End If
            If TypeOf input Is HtmlSubmit Then
                inputSubmit = element
            Else
                input.SetValue(element)
            End If
        Next input

        ' Click the submit button automatically.
        If submit AndAlso inputSubmit IsNot Nothing Then
            inputSubmit.InvokeMember("click")
        End If
    End Sub

    ''' <summary>
    ''' Get the stored site by the host name.
    ''' </summary>
    Public Shared Function GetStoredSite(ByVal host As String) As StoredSite
        Dim storedForm As StoredSite = Nothing

        Dim folderPath As String = String.Format("{0}\StoredSites\", _
                                                 Environment.CurrentDirectory)

        If Not Directory.Exists(folderPath) Then
            Directory.CreateDirectory(folderPath)
        End If

        Dim filepath As String = String.Format("{0}\{1}.xml", folderPath, host)
        If File.Exists(filepath) Then
            storedForm = XMLSerialization(Of StoredSite).DeserializeFromXMLToObject(filepath)
        End If
        Return storedForm
    End Function
End Class
