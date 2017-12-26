' Copyright (c) Microsoft Corporation. All rights reserved.

Imports System
Imports System.Data
Imports System.Configuration
Imports System.Collections
Imports System.Web
Imports System.Web.Security
Imports System.Web.UI
Imports System.Web.UI.WebControls
Imports System.Web.UI.WebControls.WebParts
Imports System.Web.UI.HtmlControls
Imports Microsoft.ServiceModel.Samples

Partial Public Class _Default
    Inherits System.Web.UI.Page

    Private client As WeatherServiceClient

    Protected Sub Page_Load(ByVal sender As Object, ByVal e As EventArgs)

        ' capture a reference to the cached client.  This reference will stay with this page for the
        ' lifetime of the page.
        ' we want to use this same instance of the client for calling BeginGetWeatherData and EndGetWeatherData
        ' in order for any exceptions on the client to bubble up
        ' if we didn't do this, there is a chance that the Global.Client instance could be
        ' replaced and we may mask certain error conditions
        Me.client = [Global].Client

        ' This page is marked Async='true' so we need to 
        ' call the service asynchronously to get the weather data
        AddHandler client.GetWeatherDataCompleted, AddressOf GetWeatherDataCompleted
        Dim localities As String() = {"Los Angeles", "Rio de Janeiro", "New York", "London", "Paris", "Rome", _
         "Cairo", "Beijing"}
        client.GetWeatherDataAsync(localities)


    End Sub

    Private Sub GetWeatherDataCompleted(ByVal sender As Object, ByVal e As GetWeatherDataCompletedEventArgs)
        dataGrid1.DataSource = e.Result
        dataGrid1.DataBind()
    End Sub

End Class