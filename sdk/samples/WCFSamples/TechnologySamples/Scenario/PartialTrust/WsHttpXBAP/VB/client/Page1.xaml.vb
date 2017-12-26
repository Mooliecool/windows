' Copyright (c) Microsoft Corporation. All rights reserved.

Imports System
Imports System.Windows
Imports System.Windows.Controls
Imports System.Windows.Data
Imports System.Windows.Documents
Imports System.Windows.Media
Imports System.Windows.Media.Imaging
Imports System.Windows.Navigation
Imports System.Windows.Shapes
Imports System.Deployment.Application
Imports System.Security.Policy
Imports System.ServiceModel
Imports Microsoft.servicemodel.samples
Namespace PartialTrustWpfCallingWcf

    Partial Public Class Page1
        Inherits Page

        'Default Service URI.
        Shared ReadOnly DEFAULT_SERVICE_URI As String = "http://localhost/servicemodelsamples/service.svc"

        ' Generated with Svcutil.exe to enable partial trust access.
        ' Keep proxy for lifetime of the Window.
        Private proxy As CalcPlusClient

        'Endpoint address of host service.
        Private serviceAddress As EndpointAddress

        Public Sub New()
            InitializeComponent()

            ' Keep the page alive between navigations
            ' to keep the proxy alive between navigations.
            KeepAlive = True
        End Sub

        Protected Overloads Overrides Sub OnInitialized(ByVal e As EventArgs)
            MyBase.OnInitialized(e)

            ' Handle button click.
            AddHandler _equalsButton.Click, AddressOf _equalsButton_Click

            ' In partial trust, only allowed to call back into the originating server.
            Dim serviceUri As String = GetUpdateLocationUrl()
            serviceAddress = New EndpointAddress(serviceUri)
            proxy = New CalcPlusClient(New WSHttpBinding(SecurityMode.None), serviceAddress)

            _numberTextBlock.Text = serviceUri
        End Sub

        ' Update a URL to use the host of the ClickOnce update location to enable
        ' partial-trust clients to call back to their originating server
        ' (the only place they are allowed to call by default). For example, if this application is launched
        ' from "http://itweb/app.application" and the URL baked into the proxy is for localhost,
        ' then "http://localhost/foo.svc" becomes "http://itweb/foo.svc"
        Private Shared Function GetUpdateLocationUrl() As String
            ' If not launched using ClickOnce, return the original URL.
            If Not ApplicationDeployment.IsNetworkDeployed Then
                Return DEFAULT_SERVICE_URI
            End If

            ' Extract the host from the update location.
            Dim updateLocation As Uri = ApplicationDeployment.CurrentDeployment.UpdateLocation
            Dim host As String = Site.CreateFromUrl(updateLocation.AbsoluteUri).Name

            ' Update service URL to use update location host.
            Dim updatedUrl As New UriBuilder(DEFAULT_SERVICE_URI)
            updatedUrl.Host = host
            Return updatedUrl.ToString()
        End Function

        Private Sub _equalsButton_Click(ByVal sender As Object, ByVal e As RoutedEventArgs)
            Dim operand1 As Double = Double.Parse(_operand1.Text, CultureInfo.InvariantCulture)
            Dim operand2 As Double = Double.Parse(_operand2.Text, CultureInfo.InvariantCulture)
            Dim opcode As String = DirectCast(_op.SelectedValue, TextBlock).Text
            Dim result As CalcResult = Nothing
            ' Start the call to the web service
            Select Case opcode
                Case "+"
                    result = proxy.Add(operand1, operand2)
                    Exit Select
                Case "-"

                    result = proxy.Subtract(operand1, operand2)
                    Exit Select
                Case "*"

                    result = proxy.Multiply(operand1, operand2)
                    Exit Select
                Case "/"

                    result = proxy.Divide(operand1, operand2)
                    Exit Select
            End Select

            If result Is Nothing Then
                _numberTextBlock.Text = ""
                _wordsTextBlock.Text = "(Failed to execute operation)"
            Else
                _numberTextBlock.Text = result.Number.ToString()
                _wordsTextBlock.Text = result.Words
            End If
        End Sub

        Private Sub Page1Parent_Closed(ByVal sender As Object, ByVal e As EventArgs)
            ' Clean up proxy when Window closes.
            DirectCast(proxy, IClientChannel).Close()
            proxy.ChannelFactory.Close()
        End Sub
    End Class
End Namespace
