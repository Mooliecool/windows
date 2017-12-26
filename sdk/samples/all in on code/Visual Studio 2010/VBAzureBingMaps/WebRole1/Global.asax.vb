Imports System.Collections.Generic
Imports Microsoft.IdentityModel.Tokens
Imports Microsoft.IdentityModel.Web
Imports Microsoft.IdentityModel.Web.Configuration

Public Class [Global]
    Inherits System.Web.HttpApplication

    Private Sub Application_Start(ByVal sender As Object, ByVal e As EventArgs)
        AddHandler FederatedAuthentication.ServiceConfigurationCreated, AddressOf OnServiceConfigurationCreated
    End Sub

    Private Sub Application_End(ByVal sender As Object, ByVal e As EventArgs)
        '  Code that runs on application shutdown

    End Sub

    Private Sub Application_Error(ByVal sender As Object, ByVal e As EventArgs)
        ' Code that runs when an unhandled error occurs

    End Sub

    Private Sub Session_Start(ByVal sender As Object, ByVal e As EventArgs)
        Session.Add("wl_Session_started", True)
    End Sub

    Private Sub Session_End(ByVal sender As Object, ByVal e As EventArgs)
        ' Code that runs when a session ends. 
        ' Note: The Session_End event is raised only when the sessionstate mode
        ' is set to InProc in the Web.config file. If session mode is set to StateServer 
        ' or SQLServer, the event is not raised.

    End Sub

    ''' <summary>
    ''' By default, WIF uses DPAPI to encrypt token.
    ''' But DPAPI is not supported in Windows Azure.
    ''' So we use a certificate instead.
    ''' </summary>
    Private Sub OnServiceConfigurationCreated(ByVal sender As Object, ByVal e As ServiceConfigurationCreatedEventArgs)
        Dim sessionTransforms As New List(Of CookieTransform)(New CookieTransform() { _
                                                              New DeflateCookieTransform(), _
                                                              New RsaEncryptionCookieTransform(e.ServiceConfiguration.ServiceCertificate), _
                                                              New RsaSignatureCookieTransform(e.ServiceConfiguration.ServiceCertificate)} _
                                                          )
        Dim sessionHandler As New SessionSecurityTokenHandler(sessionTransforms.AsReadOnly())
        e.ServiceConfiguration.SecurityTokenHandlers.AddOrReplace(sessionHandler)
    End Sub

End Class