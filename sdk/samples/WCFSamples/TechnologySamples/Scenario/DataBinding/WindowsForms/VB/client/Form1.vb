' Copyright (c) Microsoft Corporation. All rights reserved.

Imports System
Imports System.Windows.Forms

Namespace Microsoft.ServiceModel.Samples

    Public Class Form1

        ' keep the client around for the lifetime of the form
        Private client As New WeatherServiceClient()

        Private Sub Form1_FormClosed(ByVal sender As Object, ByVal e As System.Windows.Forms.FormClosedEventArgs) Handles Me.FormClosed

            'Closing the client gracefully closes the connection and cleans up resources
            client.Close()

        End Sub

        Private Sub button1_Click_1(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles button1.Click

            Dim localities As String() = {"Los Angeles", "Rio de Janeiro", "New York", "London", "Paris", "Rome", _
                        "Cairo", "Beijing"}
            AddHandler client.GetWeatherDataCompleted, AddressOf GetWeatherDataCompleted
            client.GetWeatherDataAsync(localities)

        End Sub

        Private Sub GetWeatherDataCompleted(ByVal sender As Object, ByVal e As GetWeatherDataCompletedEventArgs)

            Dim bSource As New BindingSource()
            bSource.DataSource = e.Result
            dataGridView1.DataSource = bSource

        End Sub
    End Class

End Namespace