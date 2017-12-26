'****************************** Module Header ******************************'
' Module Name:                WeatherService.vb
' Project:                    NetTcpWCFService
' Copyright (c) Microsoft Corporation.
' 
' Weather Service implementation.
' 
' This source is subject to the Microsoft Public License.
' See http://www.microsoft.com/opensource/licenses.mspx#Ms-PL.
' All other rights reserved.
' 
' THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY KIND, 
' EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED 
' WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR PURPOSE.
'***************************************************************************'

Imports System.ServiceModel
Imports System.Threading

<ServiceBehavior(InstanceContextMode := InstanceContextMode.PerSession)> _
Public Class WeatherService
    Implements IWeatherService

    Shared Sub New()
        ' Creating a separate thread to generate fake
        ' weather report periodically.
        ThreadPool.QueueUserWorkItem(New WaitCallback(AddressOf CreateWeatherReport))
    End Sub

    Private Shared Sub CreateWeatherReport()
        Dim weatherArray As String() = {"Sunny", "Windy", "Snow", "Rainy"}
        Dim rand As New Random()

        While True
            Thread.Sleep(1000)
            RaiseEvent WeatherReporting(Nothing, New WeatherEventArgs() With { _
             .WeatherReport = weatherArray(rand.[Next](weatherArray.Length)) _
            })
        End While
    End Sub

    Private Shared Event WeatherReporting As EventHandler(Of WeatherEventArgs)
    Private _callback As IWeatherServiceCallback

    Public Sub Subscribe() Implements IWeatherService.Subscribe
        _callback = OperationContext.Current.GetCallbackChannel(Of IWeatherServiceCallback)()
        AddHandler WeatherReporting, New EventHandler(Of WeatherEventArgs)(AddressOf WeatherService_WeatherReporting)
    End Sub

    Public Sub UnSubscribe() Implements IWeatherService.UnSubscribe
        RemoveHandler WeatherReporting, New EventHandler(Of WeatherEventArgs)(AddressOf WeatherService_WeatherReporting)
    End Sub

    Private Sub WeatherService_WeatherReporting(ByVal sender As Object, ByVal e As WeatherEventArgs)
        ' Remember check the callback channel's status before using it.
        If DirectCast(_callback, ICommunicationObject).State = CommunicationState.Opened Then
            _callback.WeatherReport(e.WeatherReport)
        Else
            UnSubscribe()
        End If
    End Sub

End Class

Class WeatherEventArgs
    Inherits EventArgs
    Public Property WeatherReport() As String
        Get
            Return m_WeatherReport
        End Get
        Set(ByVal value As String)
            m_WeatherReport = Value
        End Set
    End Property
    Private m_WeatherReport As String
End Class
