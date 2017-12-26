'****************************** Module Header ******************************\
' Module Name:  ConfigurationEncryption.aspx.vb
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

Imports System.Web.Configuration

Public Class ConfigurationEncryption_aspx
    Inherits System.Web.UI.Page

    Private Const provider As String = "RSAProtectedConfigurationProvider"
    'Use RSA Provider to encrypt configuration sections

    Protected Sub Page_Load(ByVal sender As Object, ByVal e As System.EventArgs) Handles Me.Load

    End Sub

    Protected Sub btnEncrypt_Click(ByVal sender As Object, ByVal e As EventArgs) Handles btnEncrypt.Click
        If String.IsNullOrEmpty(Me.ddlSection.SelectedValue) Then
            Response.Write("please select a configuration section")
            Return
        End If

        Dim sectionString As String = Me.ddlSection.SelectedValue

        Dim config As System.Configuration.Configuration = WebConfigurationManager.OpenWebConfiguration(Request.ApplicationPath)
        Dim section As ConfigurationSection = config.GetSection(sectionString)
        If section IsNot Nothing Then
            section.SectionInformation.ProtectSection(provider)
            config.Save()
            Response.Write("encrypt successed, please check the configuration file.")
        End If
    End Sub

    Protected Sub btnDecrypt_Click(ByVal sender As Object, ByVal e As EventArgs) Handles btnDecrypt.Click
        Dim sectionString As String = Me.ddlSection.SelectedValue

        Dim config As System.Configuration.Configuration = WebConfigurationManager.OpenWebConfiguration(Request.ApplicationPath)
        Dim section As ConfigurationSection = config.GetSection(sectionString)
        If section IsNot Nothing AndAlso section.SectionInformation.IsProtected Then
            section.SectionInformation.UnprotectSection()
            config.Save()
            Response.Write("decrypt success, please check the configuration file.")
        End If
    End Sub
End Class