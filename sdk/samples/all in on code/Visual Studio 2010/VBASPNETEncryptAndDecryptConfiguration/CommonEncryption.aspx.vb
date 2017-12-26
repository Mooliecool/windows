'****************************** Module Header ******************************\
' Module Name:  CommonEncryption.aspx.vb
' Project:      VBASPNETEncryptAndDecryptConfiguration
' Copyright (c) Microsoft Corporation.
' 
'  This sample shows how to use RSA encryption algorithm API to encrypt and decrypt 
'configuration section in order to protect the sensitive information from interception
'or hijack in ASP.NET web application.
'
'  The project contains two snippets. The First One demonstrates how to use RSA provider 
'and RSA container to encrypt and decrypt some words or values in web application.
'the purpose of first snippet is to let us know the overview of RSA mechanism.
'Second one shows how to use RSA configuration provider to encrypt and decrypt
'configuration section in web.config.
' 
' This source is subject to the Microsoft Public License.
' See http://www.microsoft.com/opensource/licenses.mspx#Ms-PL.
' All other rights reserved.
' 
'\***************************************************************************/

Imports System.Security.Cryptography

Public Class CommonEncryption_aspx
    Inherits System.Web.UI.Page

    Protected Sub Page_Load(ByVal sender As Object, ByVal e As System.EventArgs) Handles Me.Load
        AddKeyUpEventOnTextControl()
        AddHandler btnEncrypt.PreRender, AddressOf btnEncrypt_PreRender
        AddHandler btnDecrypt.PreRender, AddressOf btnDecrypt_PreRender
    End Sub

    Private Sub RSAEncryption()
        Dim param As New CspParameters()
        param.KeyContainerName = "MyKeyContainer"
        Using rsa As New RSACryptoServiceProvider(param)
            Dim plaintext As String = Me.tbData.Text
            Dim plaindata As Byte() = System.Text.Encoding.[Default].GetBytes(plaintext)
            Dim encryptdata As Byte() = rsa.Encrypt(plaindata, False)
            Dim encryptstring As String = Convert.ToBase64String(encryptdata)
            Me.tbEncryptData.Text = encryptstring
        End Using
    End Sub

    Private Sub RSADecryption()
        Dim param As New CspParameters()
        param.KeyContainerName = "MyKeyContainer"
        Using rsa As New RSACryptoServiceProvider(param)
            Dim encryptdata As Byte() = Convert.FromBase64String(Me.tbEncryptData.Text)
            Dim decryptdata As Byte() = rsa.Decrypt(encryptdata, False)
            Dim plaindata As String = System.Text.Encoding.[Default].GetString(decryptdata)
            Me.tbDecryptData.Text = plaindata
        End Using
    End Sub

    Private Sub AddKeyUpEventOnTextControl()
        Dim script As String = String.Format("function PressFn(sender) {{" & vbCr & vbLf & " document.getElementById('{0}').disabled = sender.value == '' ? true : false;" & vbCr & vbLf & " }}", btnEncrypt.ClientID)
        tbData.Attributes("onkeyup") = "PressFn(this)"
        Page.ClientScript.RegisterStartupScript(Me.[GetType](), "DataKeyUp", script, True)
    End Sub

    Private Sub EnableDecryptButton()
        btnDecrypt.Enabled = If(Me.tbEncryptData.Text <> String.Empty, True, False)
    End Sub

    Private Sub EnableEncryptButton()
        btnEncrypt.Enabled = If(Me.tbData.Text <> String.Empty, True, False)
    End Sub

    Protected Sub Encrypt_Click(ByVal sender As Object, ByVal e As EventArgs) Handles btnEncrypt.Click
        RSAEncryption()
    End Sub

    Protected Sub Decrypt_Click(ByVal sender As Object, ByVal e As EventArgs) Handles btnDecrypt.Click
        RSADecryption()
    End Sub

    Private Sub btnDecrypt_PreRender(ByVal sender As Object, ByVal e As EventArgs)
        EnableDecryptButton()
    End Sub

    Private Sub btnEncrypt_PreRender(ByVal sender As Object, ByVal e As EventArgs)
        EnableEncryptButton()
    End Sub
End Class