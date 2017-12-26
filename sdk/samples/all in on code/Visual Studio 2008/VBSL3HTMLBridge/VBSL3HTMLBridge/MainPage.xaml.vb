'****************************** Module Header ******************************'
' Module Name:  MainPage.xaml.vb
' Project:      VBSL3HTMLBridge
' Copyright (c) Microsoft Corporation.
' 
' HTML bridge samples code behind file.
' 
' This source is subject to the Microsoft Public License.
' See http://www.microsoft.com/opensource/licenses.mspx#Ms-PL.
' All other rights reserved.
' 
' THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY KIND, 
' EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED 
' WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR PURPOSE.
'***************************************************************************'

Imports System.Windows.Browser


Partial Public Class MainPage
    Inherits UserControl

    Public Sub New()
        InitializeComponent()

        ' Register Scriptable Object.
        HtmlPage.RegisterScriptableObject("silverlightPage", Me)

        ' Attach Html Element event.
        HtmlPage.Document.GetElementById("Text2").AttachEvent( _
            "onkeyup", New EventHandler(AddressOf HtmlKeyUp))

        AddHandler tb1.TextChanged, AddressOf tb1_TextChanged
        AddHandler tb4.TextChanged, AddressOf tb4_TextChanged
    End Sub

    ' Call javascript when first textbox text changed.
    Private Sub tb1_TextChanged(ByVal sender As Object, ByVal e As TextChangedEventArgs)
        HtmlPage.Window.Invoke("changetext", tb1.Text)
    End Sub

    ' Handle html textbox keyup event.
    Private Sub HtmlKeyUp(ByVal sender As Object, ByVal e As EventArgs)
        Me.tb2.Text = DirectCast(sender, HtmlElement).GetProperty("value").ToString()
    End Sub

    ' Create method for javascript.
    <ScriptableMember()> _
    Public Sub ChangeTB3Text(ByVal text As String)
        tb3.Text = text
    End Sub

    ' Create event allow registering by javascript.
    <ScriptableMember()> _
    Public Event TextChanged As EventHandler(Of TextEventArgs)

    Private Sub tb4_TextChanged(ByVal sender As Object, ByVal e As TextChangedEventArgs)
        Dim myargs = New TextEventArgs()
        myargs.Text = tb4.Text
        RaiseEvent TextChanged(Me, myargs)
    End Sub

End Class


Public Class TextEventArgs
    Inherits EventArgs

    Private _text As String

    <ScriptableMember()> _
    Public Property [Text]() As String
        Get
            Return _text
        End Get
        Set(ByVal value As String)
            _text = value
        End Set
    End Property
End Class
